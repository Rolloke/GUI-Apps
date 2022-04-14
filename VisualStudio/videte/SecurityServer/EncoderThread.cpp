// EncoderThread.cpp: implementation of the CEncoderThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EncoderThread.h"
#include "output.h"
#include "sec3.h"
#include "CipcDatabaseClientServer.h"
#include "RequestCollector.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEncoderItem::CEncoderItem(Compression comp,
						   CSecID idArchive,
						   DWORD dwClientID,
						   CompressionType ctDestination,
						   const CIPCPictureRecord& pictRef,
						   const CIPCFields& fields)
{
	m_idArchive = idArchive;
	m_dwClientID = dwClientID;
	m_pPicture = new CIPCPictureRecord(pictRef);
	m_Fields = fields;
	m_Compression = comp;
	m_ctDestination = ctDestination;
	m_bIsValid = TRUE;
	m_dwX = 0;
	m_dwY = 0;
	COutput* pOutput = theApp.GetOutputGroups().GetOutputByID(m_pPicture->GetCamID());
	if (pOutput)
	{
		pOutput->GetMDCoordinates(m_dwX,m_dwY);
	}
}
//////////////////////////////////////////////////////////////////////
CEncoderItem::~CEncoderItem()
{
	WK_DELETE(m_pPicture);
}
//////////////////////////////////////////////////////////////////////
void CEncoderItem::Invalidate()
{
	m_bIsValid = FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CEncoderThread::m_bUseAVCodec = TRUE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEncoderThread::CEncoderThread()
:CWK_Thread("Encoder"), 
m_PerfMonJPEG("JPEG Encoder"),
m_PerfMonMpeg4Encode("MPEG4 Encoder"),
m_PerfMonMpeg4YUV422_420("YUV422 to YUV420"),
m_Statistics(100)
{
	m_dwStatTime = 12*60*60*1000;
	m_dwJpegLen = 1024*1024;
	m_pJpegData = new BYTE[m_dwJpegLen];
	m_pJpeg = NULL;
	m_iMax = 0;
	// m_EmuzedEncoders
	// m_AVEncoders
}
//////////////////////////////////////////////////////////////////////
CEncoderThread::~CEncoderThread()
{
	WK_DELETE(m_pJpeg);
	WK_DELETE_ARRAY(m_pJpegData);
	m_EmuzedEncoders.SafeDeleteAll();
	m_AVEncoders.SafeDeleteAll();
	EmptyDataQueue();
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::CreateJPEGDecoder()
{
	if (m_pJpeg == NULL)
	{
		m_pJpeg = new CJpeg();
		m_pJpeg->SetProperties(756,288,IJL_YCBCR);
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CEncoderThread::StartThread()
{
	return CWK_Thread::StartThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CEncoderThread::StopThread()
{
	EmptyDataQueue();
	m_PerfMonJPEG.ShowResults();
	m_PerfMonMpeg4Encode.ShowResults();
	m_PerfMonMpeg4YUV422_420.ShowResults();
	WK_TRACE(_T("Encoder FPS %.02f\n"),m_Statistics.GetFPS());
	BOOL bRet =  CWK_Thread::StopThread();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::AddPictureRecord(Compression comp,
									  const CIPCPictureRecord& pict,
									  const CIPCFields& fields, 
									  WORD wArchiveNr,
									  DWORD dwClientID,
									  CompressionType ctDestination)
{
	int iSize = m_Queue.GetCount();
	if (iSize>m_iMax)
	{
		m_iMax = iSize;
		if (iSize%20==0)
		{
			TRACE(_T("Encoder Queue Size %d\n"),iSize);
		}
	}
	if (iSize<200)
	{
		if (pict.GetDataLength() > 0)
		{
			CEncoderItem* pEncoderItem;
			CSecID idArchive;

			if (wArchiveNr!=0)
			{
				idArchive = CSecID(SECID_GROUP_ARCHIVE,wArchiveNr);
			}

			pEncoderItem = new CEncoderItem(comp,idArchive,dwClientID,ctDestination,pict,fields);
			m_Queue.SafeAddTail(pEncoderItem);
		}
		else
		{
			WK_TRACE_ERROR(_T("empty picture record to encode\n"));
		}
	}
	else
	{
		TRACE(_T("Encoder Queue Empty %d\n"),iSize);
		EmptyDataQueue();
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CEncoderThread::Run(LPVOID lpContext)
{
	HANDLE hHandles[2];

	hHandles[0]	= m_StopEvent;
	hHandles[1] = m_Queue.GetSemaphore();

	// 50 Bilder in einem Run auswerten,
	// damit das Wait auf Ende 10ms nicht zu sehr bremst
	for (int i=0; (i<50) && (m_Queue.GetCount()>0); i++)
	{
		DWORD r = WaitForMultipleObjects(2,hHandles,FALSE,100);
		
		if (r==WAIT_TIMEOUT)
		{
			// leave the loop
			break;
		}
		else if (r==WAIT_FAILED)
		{
			WK_TRACE(_T("%s wait failed %s\n"),m_sName,GetLastErrorString());
			Sleep(100);
			// leave the loop
			break;
		}
		else if (r==WAIT_OBJECT_0)
		{
			// reset the shutdown event
			WK_TRACE(_T("stop encoding %s\n"),m_sName);
			m_StopEvent.SetEvent();
			break;
		}
		else if (r==WAIT_OBJECT_0+1)
		{
			// take FirstOut picture of queue
			CEncoderItem* pEncoderItem = m_Queue.SafeGetAndRemoveHead();
			if (   pEncoderItem
				&& pEncoderItem->IsValid())
			{
				if (pEncoderItem->GetCompressionType() == COMPRESSION_JPEG)
				{
					if (m_bUseAVCodec)
					{
						EncodeAVEmuzed(*pEncoderItem);
					}
					else
					{
						EncodeIJL(*pEncoderItem);
					}
				}
				else if (pEncoderItem->GetCompressionType() == COMPRESSION_MPEG4)
				{
					EncodeAVEmuzed(*pEncoderItem);
				}
			}
			WK_DELETE(pEncoderItem);
		}
		else
		{
			WK_TRACE_ERROR(_T("Wait in %s unknown return value\n"),m_sName);
		}
	}
	for (int i=m_EmuzedEncoders.GetSize()-1;i>=0;i--)
	{
		if (m_EmuzedEncoders.GetAtFast(i)->IsDelayedDelete())
		{
			delete m_EmuzedEncoders.GetAtFast(i);
			m_EmuzedEncoders.RemoveAt(i);
		}
	}
	for (int i=m_AVEncoders.GetSize()-1;i>=0;i--)
	{
		if (m_AVEncoders.GetAtFast(i)->IsDelayedDelete())
		{
			delete m_AVEncoders.GetAtFast(i);
			m_AVEncoders.RemoveAt(i);
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::EmptyDataQueue()
{
	m_Queue.Lock();
	m_Queue.DeleteAll();
	HANDLE hSemaphore = m_Queue.GetSemaphore();
	while (WAIT_TIMEOUT != WaitForSingleObject(hSemaphore,0))
	{
	}
	m_Queue.Unlock();
}
//////////////////////////////////////////////////////////////////////
CEmuzedMpeg4Encoder* CEncoderThread::CreateEmuzedEncoder(CEmuzedMpeg4Encoder* pEmuzedEncoder,
														 CSecID idArchive, CSecID idCam, 
														 int iWidth, int iHeight,
														 Compression comp,
														 int iBitrate,
														 int iFramerate)
{
	m_EmuzedEncoders.Lock();
	if (pEmuzedEncoder)
	{
		// found one check resolution
		if (   pEmuzedEncoder->GetWidth() != iWidth
			|| pEmuzedEncoder->GetHeight() != iHeight)
		{
			m_EmuzedEncoders.Delete(pEmuzedEncoder);
			pEmuzedEncoder = NULL;
		}
	}
	if (pEmuzedEncoder == NULL)
	{
		// not found or new resolution create new encoder
		pEmuzedEncoder = new CEmuzedMpeg4Encoder(idCam,idArchive.GetID());

		int iMyBitrate = 0;

		if (   idArchive.IsArchiveID()
			&& idArchive!= SECID_NO_ID)
		{
			switch(comp)
			{
			case COMPRESSION_1:
				iMyBitrate = 2048000; // unconstrained
				break;
			case COMPRESSION_2:
				iBitrate = 1024000; // 1MBit
				break;
			case COMPRESSION_3:
				iMyBitrate = 512000; // 0.5Mbit
				break;
			default:
				iMyBitrate = iBitrate;
				break;
			}
		}
		BOOL bLowbandwidth = (iBitrate>0 && iBitrate<128*1024);
		int iIFrameIntervall = CRequestCollector::GetIFrameDefault(bLowbandwidth, iFramerate);

		if (pEmuzedEncoder->Init(iWidth,iHeight,iMyBitrate,iFramerate,iIFrameIntervall))
		{
			m_EmuzedEncoders.Add(pEmuzedEncoder);
		}
		else
		{
			WK_DELETE(pEmuzedEncoder);
		}
	}
	m_EmuzedEncoders.Unlock();
	return pEmuzedEncoder;
}
//////////////////////////////////////////////////////////////////////
/*
void CEncoderThread::CreateEncoders(COutputList* pOutputs, Resolution res, CompressionType ct)
{
	for (int i=0;i<pOutputs->GetSize();i++)
	{
		COutputGroup* pGroup = pOutputs->GetAtFast(i);
		if (   pGroup
			&& pGroup->HasCameras())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)pGroup;
			for (int j=0;j<pCameraGroup->GetSize();j++)
			{
				CCamera* pCamera = (CCamera*)pCameraGroup->GetAtFast(j);
				if (   pCamera->IsActive()
					&& pCamera->IsMD())
				{
					CSecID idArchive(SECID_GROUP_ARCHIVE,pCamera->GetMDAlarmNr());
					CSecID idCam = pCamera->GetUnitID();
					SIZE s = SizeOfEnum(res);

					if (m_bUseAVCodec)
					{
						CreateAVEncoder(NULL,idArchive,idCam,s.cx,s.cy,ct,
							pCamera->GetCompression(),0,pCamera->GetFPS());
					}
					else if (ct == COMPRESSION_MPEG4)
					{
						CreateEmuzedEncoder(NULL,idArchive,idCam,s.cx,s.cy,
							pCamera->GetCompression(),0,pCamera->GetFPS());
					}
				}
			}
		}
	}
}
*/
//////////////////////////////////////////////////////////////////////
CAVEncoder* CEncoderThread::CreateAVEncoder(CAVEncoder* pAVEncoder,
											CSecID idArchive, CSecID idCam, 
											int iWidth, int iHeight,
											CompressionType ct,
											Compression comp,
											int iBitrate,
											int iFramerate)
{
	m_AVEncoders.Lock();
	if (pAVEncoder)
	{
		// found one check resolution
		if (   pAVEncoder->GetWidth() != iWidth
			|| pAVEncoder->GetHeight() != iHeight)
		{
			m_AVEncoders.Delete(pAVEncoder);
			pAVEncoder = NULL;
		}
	}
	if (pAVEncoder == NULL)
	{
		// not found or new resolution create new encoder
		pAVEncoder = new CAVEncoder(idCam,idArchive.GetID());

		int iMyBitrate = 0;

		if (   idArchive.IsArchiveID()
			&& idArchive!= SECID_NO_ID)
		{
			switch(comp)
			{
			case COMPRESSION_1:
				iMyBitrate = 2048000; // unconstrained
				break;
			case COMPRESSION_2:
				iBitrate = 1024000; // 1MBit
				break;
			case COMPRESSION_3:
				iMyBitrate = 512000; // 0.5Mbit
				break;
			default:
				iMyBitrate = iBitrate;
				break;
			}
		}
		BOOL bLowbandwidth = (iBitrate>0 && iBitrate<128*1024);
		int iIFrameIntervall = CRequestCollector::GetIFrameDefault(bLowbandwidth, iFramerate);

		CodecID idCodec = CODEC_ID_MJPEG;
		if (ct == COMPRESSION_MPEG4)
		{
			idCodec = CODEC_ID_MPEG4;
		}

		if (pAVEncoder->CreateEncoder(idCodec,iWidth,iHeight,iMyBitrate,iFramerate,iIFrameIntervall))
		{
			WK_TRACE(_T("AVCodec created for camera %08lx\n"),idCam.GetID());
			m_AVEncoders.Add(pAVEncoder);
		}
		else
		{
			WK_DELETE(pAVEncoder);
		}
	}
	m_AVEncoders.Unlock();
	return pAVEncoder;
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::EncodeIJL(CEncoderItem& item)
{
	const CIPCPictureRecord* pPict = item.GetPictureRecord();
	BYTE* pData = (BYTE*)pPict->GetData();
	LPBITMAPINFO lpBMI = (LPBITMAPINFO)pData;
	JPEG_CORE_PROPERTIES JpegCoreProp;
	DWORD dwJpegLen = 0;
	COutput* pOutput = theApp.GetOutputGroups().GetOutputByID(pPict->GetCamID());

	if (pOutput)
	{
		m_PerfMonJPEG.Start();
		ZeroMemory(&JpegCoreProp,sizeof(JPEG_CORE_PROPERTIES));

		if (ijlInit(&JpegCoreProp) == IJL_OK)
		{
			JpegCoreProp.DIBWidth		= lpBMI->bmiHeader.biWidth;
			JpegCoreProp.DIBHeight		= lpBMI->bmiHeader.biHeight; //TopDown
			JpegCoreProp.DIBBytes		= (BYTE*)&lpBMI->bmiColors[0];;
			JpegCoreProp.JPGWidth		= lpBMI->bmiHeader.biWidth;
			JpegCoreProp.JPGHeight		= abs(lpBMI->bmiHeader.biHeight);
			
			JpegCoreProp.jquality		= pOutput->GetJPEGQuality(item.GetCompression(),pPict->GetResolution());

			JpegCoreProp.JPGBytes		= m_pJpegData;
			JpegCoreProp.JPGSizeBytes	= m_dwJpegLen;
			JpegCoreProp.JPGSubsampling = IJL_422;

			JpegCoreProp.DIBChannels	= 3;//lpBMI->bmiHeader.biBitCount/8;

			if (item.GetPictureRecord()->GetCompressionType() == COMPRESSION_RGB_24)
			{
				JpegCoreProp.DIBColor		= IJL_BGR;
				JpegCoreProp.DIBSubsampling	= IJL_NONE;
			}
			else if (item.GetPictureRecord()->GetCompressionType() == COMPRESSION_YUV_422)
			{
				JpegCoreProp.DIBColor		= IJL_YCBCR;
				JpegCoreProp.DIBSubsampling	= IJL_422;
			}

			int nRet = ijlWrite(&JpegCoreProp, IJL_JBUFF_WRITEWHOLEIMAGE);
			if (nRet == IJL_OK)
			{
				dwJpegLen = JpegCoreProp.JPGSizeBytes;

				pOutput->AddCompressedJPEGSize(dwJpegLen,item.GetCompression(),pPict->GetResolution());

				/*
				TRACE(_T("YUV442->JPEG %08lx %s %s %d Bytes %d%%\n"),pPict->GetCamID().GetID(),
					NameOfEnum(item.GetCompression()),NameOfEnum(pPict->GetResolution()),dwJpegLen,JpegCoreProp.jquality);
					*/

				CSecID idArchive = item.GetArchiveID();
				if (   idArchive.IsArchiveID()
					&& idArchive!= SECID_NO_ID)
				{
					CIPCPictureRecord encodedPict(NULL,
												 m_pJpegData,
												 dwJpegLen,
												 pPict->GetCamID(),
												 pPict->GetResolution(),
												 item.GetCompression(),
												 COMPRESSION_JPEG,
												 pPict->GetTimeStamp(),
												 pPict->GetJobTime());
					m_Statistics.AddValue(encodedPict.GetDataLength());
					theApp.SavePicture(item.GetCompression(),encodedPict,item.GetFields(),idArchive.GetSubID());
				}
				DWORD dwClientID = item.GetClientID();
				if (dwClientID != 0)
				{
					CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(dwClientID);
					if (   pClient
						&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
					{
						CIPCPictureRecord encodedPict(pClient,
													  m_pJpegData,
													  dwJpegLen,
													  pPict->GetCamID(),
													  pPict->GetResolution(),
													  item.GetCompression(),
													  COMPRESSION_JPEG,
													  pPict->GetTimeStamp(),
													  pPict->GetJobTime());
						pClient->SendPicture(encodedPict,COMPRESSION_JPEG,item.GetCompression(),0,0);
					}
				}
			}
			else
			{
				WK_TRACE(_T("ijlWrite failed %d\n"),nRet);
			}

			if (ijlFree(&JpegCoreProp) != IJL_OK)
			{
				WK_TRACE_ERROR(_T("Cannot free Intel(R) JPEG library"));
			}
		}		
		m_PerfMonJPEG.Stop();
	} // no output
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::EncodeAVEmuzed(CEncoderItem& item)
{
	const CIPCPictureRecord* pPict = item.GetPictureRecord();

	CSecID idArchive = item.GetArchiveID();
	CSecID idCam = pPict->GetCamID();
	DWORD  dwClient = item.GetClientID();
	DWORD  dwID = (dwClient!=0) ? dwClient : idArchive.GetID();

	CEmuzedMpeg4Encoder* pEmuzedEncoder = NULL;
	CAVEncoder* pAVEncoder = NULL;
	
	BYTE* pData = (BYTE*)pPict->GetData();
	LPBITMAPINFO lpBMI = (LPBITMAPINFO)pData;
	const BYTE* pDibBuffer = NULL;
	int iWidth = lpBMI->bmiHeader.biWidth;
	int iHeight = lpBMI->bmiHeader.biHeight;
	int iBitrate = 0;
	int iSourceHeight = 0;
	int iSourceWidth = 0;

	if (pPict->GetCompressionType() == COMPRESSION_JPEG)
	{
		CreateJPEGDecoder();
		m_pJpeg->DecodeJpegFromMemory(pData,pPict->GetDataLength(),FALSE,CJ_NOTHING,TRUE);
		iWidth = m_pJpeg->GetImageDims().cx;
		iHeight = m_pJpeg->GetImageDims().cy;
		pDibBuffer = m_pJpeg->GetDibBuffer();
	}
	else
	{
		pDibBuffer = (PBYTE)&lpBMI->bmiColors[0];
	}

	iSourceWidth = iWidth;
	iSourceHeight = iHeight;

//	TRACE(_T("MPEG Encoder res %d,%d\n"),iSourceWidth,iSourceHeight);

	int iHMod = iHeight%16;
	int iWMod = iWidth%16;

	if (iHMod>0)
	{
		iHeight = iHeight + (16-iHMod);
	}
	if (iWMod)
	{
		iWidth = iWidth + (16-iWMod);
	}

	
	if (dwClient!=0)
	{
		CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(dwClient);
		if (   pClient
			&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			iBitrate = (pClient->GetBitrate()/1024)*1000;
			iBitrate = min(2048*1000,iBitrate);
			if (iBitrate<=128*1024)
			{
				if (   pPict->GetResolution() == RESOLUTION_2CIF
					&& iWidth>384)
				{
					iWidth = 352;
					iHeight = 288;
				}
				else if (   pPict->GetResolution() == RESOLUTION_QCIF
					&& iWidth>192)
				{
					iWidth = 176;
					iHeight = 144;
				}
			}
		}
	}

	// try to find the right encoder
	if (m_bUseAVCodec)
	{
		m_AVEncoders.Lock();
		for (int i=0;i<m_AVEncoders.GetSize();i++)
		{
			pAVEncoder = m_AVEncoders.GetAtFast(i);
			if (pAVEncoder)
			{
				if (    dwID == pAVEncoder->GetID()
					&&  idCam == pAVEncoder->GetCamID())
				{
					break;
				}
			}
			pAVEncoder = NULL;
		}

		if (pAVEncoder)
		{
			// found one check resolution
			if (   pAVEncoder->GetWidth() != iWidth
				|| pAVEncoder->GetHeight() != iHeight)
			{
				m_AVEncoders.Delete(pAVEncoder);
				pAVEncoder = NULL;
			}
		}
		m_AVEncoders.Unlock();
	}
	else
	{
		m_EmuzedEncoders.Lock();
		for (int i=0;i<m_EmuzedEncoders.GetSize();i++)
		{
			pEmuzedEncoder = m_EmuzedEncoders.GetAtFast(i);
			if (pEmuzedEncoder)
			{
				if (    dwID == pEmuzedEncoder->GetID()
					&&  idCam == pEmuzedEncoder->GetCamID())
				{
					break;
				}
			}
			pEmuzedEncoder = NULL;
		}

		if (pEmuzedEncoder)
		{
			// found one check resolution
			if (   pEmuzedEncoder->GetWidth() != iWidth
				|| pEmuzedEncoder->GetHeight() != iHeight)
			{
				m_EmuzedEncoders.Delete(pEmuzedEncoder);
				pEmuzedEncoder = NULL;
			}
		}
		m_EmuzedEncoders.Unlock();
	}


	if (   pEmuzedEncoder == NULL
		&& pAVEncoder == NULL
		&& item.IsValid())
	{
		// not found or new resolution create new encoder
		if (m_bUseAVCodec)
		{
			pAVEncoder = new CAVEncoder(idCam,dwID);
		}
		else
		{
			pEmuzedEncoder = new CEmuzedMpeg4Encoder(idCam,dwID);
		}
		
		int iFps = 6;
		int iIFrame = CRequestCollector::GetIFrameDefault(FALSE,iFps);

		if (   idArchive.IsArchiveID()
			&& idArchive!= SECID_NO_ID)
		{
			if (m_bUseAVCodec)
			{
				switch (item.GetCompression())
				{
				case COMPRESSION_5:
					iBitrate = 2000000;
					break;
				case COMPRESSION_7:
					iBitrate = 1500000;
					break;
				case COMPRESSION_12:
					iBitrate = 1000000;
					break;
				case COMPRESSION_17:
					iBitrate = 500000;
					break;
				case COMPRESSION_22:
					iBitrate = 250000;
					break;
				default:
					iBitrate = 2000000;
					break;
				}
			}
			else
			{
				iBitrate = 0;
			}
		}

		if (pAVEncoder)
		{
			CodecID id = CODEC_ID_MJPEG;
			if (item.GetCompressionType() == COMPRESSION_MPEG4)
			{
				id = CODEC_ID_MPEG4;
			}
			if (pAVEncoder->CreateEncoder(id,iWidth,iHeight,iBitrate,iFps,iIFrame))
			{
				m_AVEncoders.Add(pAVEncoder);
			}
			else
			{
				WK_DELETE(pAVEncoder);
			}
		}
		else if (pEmuzedEncoder)
		{
			if (pEmuzedEncoder->Init(iWidth,iHeight,iBitrate,iFps,iIFrame))
			{
				m_EmuzedEncoders.Add(pEmuzedEncoder);
			}
			else
			{
				WK_DELETE(pEmuzedEncoder);
			}
		}
	}

	if (   (pEmuzedEncoder && !pEmuzedEncoder->IsDelayedDelete())
		|| (pAVEncoder && !pAVEncoder->IsDelayedDelete())
		)
	{
		if (   idArchive.IsArchiveID()
			&& idArchive!= SECID_NO_ID)
		{
			if (   theApp.GetDatabase()
				&& theApp.GetDatabase()->GetIPCState() == CIPC_STATE_CONNECTED)
			{
				m_PerfMonMpeg4YUV422_420.Start();
				if (pEmuzedEncoder)	
				{
					pEmuzedEncoder->SetYUV422Data(idCam,iSourceWidth,iSourceHeight,pDibBuffer);
				}
				else if (pAVEncoder)	
				{
					pAVEncoder->SetYUV422Data(idCam,iSourceWidth,iSourceHeight,pDibBuffer);
				}
				m_PerfMonMpeg4YUV422_420.Stop();
				
				m_PerfMonMpeg4Encode.Start();
				if (pEmuzedEncoder) 
				{
					pEmuzedEncoder->Encode();
				}
				else if (pAVEncoder) 
				{
					pAVEncoder->Encode();
				}
				m_PerfMonMpeg4Encode.Stop();

				CIPCPictureRecord* pPict = NULL;
				
				if (pEmuzedEncoder)
				{
					pPict = pEmuzedEncoder->GetEncodedPicture(theApp.GetDatabase(),idCam,item.GetPictureRecord()->GetTimeStamp());
				}
				else if (pAVEncoder)
				{
					pPict = pAVEncoder->GetEncodedPicture(theApp.GetDatabase(),idCam,item.GetPictureRecord()->GetTimeStamp());
				}
				
				if (pPict)
				{
					m_Statistics.AddValue(pPict->GetDataLength());
					theApp.SavePicture(item.GetCompression(),*pPict,item.GetFields(),idArchive.GetSubID());
					WK_DELETE(pPict);
				}
				else
				{
					WK_TRACE_ERROR(_T("no picture record MPEG4 encoder\n"));
				}
			}
		}
		else if (dwClient != 0)
		{
			CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(dwClient);
			if (   pClient
				&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
			{
				m_PerfMonMpeg4YUV422_420.Start();
				if (pEmuzedEncoder)	
				{
					pEmuzedEncoder->SetYUV422Data(idCam,iSourceWidth,iSourceHeight,pDibBuffer);
				}
				else if (pAVEncoder)	
				{
					pAVEncoder->SetYUV422Data(idCam,iSourceWidth,iSourceHeight,pDibBuffer);
				}
				m_PerfMonMpeg4YUV422_420.Stop();

				m_PerfMonMpeg4Encode.Start();
				if (pEmuzedEncoder) 
				{
					pEmuzedEncoder->Encode();
				}
				else if (pAVEncoder) 
				{
					pAVEncoder->Encode();
				}
				m_PerfMonMpeg4Encode.Stop();

				CIPCPictureRecord* pPict = NULL;

				if (pEmuzedEncoder)
				{
					pPict = pEmuzedEncoder->GetEncodedPicture(pClient,idCam,item.GetPictureRecord()->GetTimeStamp());
				}
				else if (pAVEncoder)
				{
					pPict = pAVEncoder->GetEncodedPicture(pClient,idCam,item.GetPictureRecord()->GetTimeStamp());
				}
				if (pPict)
				{
					pClient->SendPicture(*pPict,COMPRESSION_MPEG4,item.GetCompression(),0,0);
					WK_DELETE(pPict);
				}
			}
		}
	}
	else
	{
//		WK_TRACE_ERROR(_T("MPEG4 encoder invalid\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::RemoveClient(DWORD dwClientID)
{
	for (int i=m_EmuzedEncoders.GetSize()-1;i>=0;i--)
	{
		if (m_EmuzedEncoders.GetAtFast(i)->GetID() == dwClientID)
		{
			//			TRACE(_T("delayed delete mpeg 4 encoder %08lx\n"),dwClientID);
			m_EmuzedEncoders.GetAtFast(i)->DelayedDelete();
		}
	}
	for (int i=m_AVEncoders.GetSize()-1;i>=0;i--)
	{
		if (m_AVEncoders.GetAtFast(i)->GetID() == dwClientID)
		{
			//			TRACE(_T("delayed delete mpeg 4 encoder %08lx\n"),dwClientID);
			m_AVEncoders.GetAtFast(i)->DelayedDelete();
		}
	}

	POSITION pos = m_Queue.GetHeadPosition();
	while (pos != NULL)
	{
		CEncoderItem* pEncoderItem = m_Queue.GetNext( pos );
		if (pEncoderItem)
		{
			if (pEncoderItem->GetClientID() == dwClientID)
			{
				pEncoderItem->Invalidate();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::RemoveClient(DWORD dwClientID, CSecID idCam)
{
	for (int i=m_EmuzedEncoders.GetSize()-1;i>=0;i--)
	{
		if (   m_EmuzedEncoders.GetAtFast(i)->GetID() == dwClientID
			&& m_EmuzedEncoders.GetAtFast(i)->GetCamID() == idCam)
		{
			m_EmuzedEncoders.GetAtFast(i)->DelayedDelete();
			//			TRACE(_T("delayed delete mpeg 4 encoder %08lx\n"),dwClientID);
		}
	}
	for (int i=m_AVEncoders.GetSize()-1;i>=0;i--)
	{
		if (   m_AVEncoders.GetAtFast(i)->GetID() == dwClientID
			&& m_AVEncoders.GetAtFast(i)->GetCamID() == idCam)
		{
			m_AVEncoders.GetAtFast(i)->DelayedDelete();
			//			TRACE(_T("delayed delete mpeg 4 encoder %08lx\n"),dwClientID);
		}
	}
	POSITION pos = m_Queue.GetHeadPosition();
	while (pos != NULL)
	{
		CEncoderItem* pEncoderItem = m_Queue.GetNext( pos );
		if (pEncoderItem)
		{
			if (   pEncoderItem->GetClientID() == dwClientID
				&& pEncoderItem->GetPictureRecord()->GetCamID() == idCam)
			{
				pEncoderItem->Invalidate();
				//	TRACE(_T("Invalidate mpeg 4 encoder item %08lx\n"),dwClientID);
			}
		}
	}
}
