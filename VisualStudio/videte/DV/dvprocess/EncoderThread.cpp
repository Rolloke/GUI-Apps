// EncoderThread.cpp: implementation of the CEncoderThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EncoderThread.h"
#include "Camera.h"
#include "dvprocess.h"
#include "IPCDatabaseProcess.h"
#include "CameraGroup.h"
#include "Camera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEncoderItem::CEncoderItem(CCamera* pCamera,
						   CSecID idArchive,
						   DWORD dwClientID,
						   CompressionType ctDestination,
						   const CIPCPictureRecord& pict,
						   const CIPCFields& fields)
{
	m_idArchive = idArchive;
	m_dwClientID = dwClientID;
	m_pPicture = new CIPCPictureRecord(pict);
	m_Fields = fields;
	m_pCamera = pCamera;
	m_ctDestination = ctDestination;
	m_bIsValid = TRUE;
	m_dwX = m_dwY = 0;
	pCamera->GetMDCoordinates(m_dwX,m_dwY);
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
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEncoderThread::CEncoderThread()
:CWK_Thread(_T("EncoderThread")),m_PerfMon(_T("Encoder"))
{
	m_dwStatTime = 10*60*1000;
	m_dwJpegLen = 1024*1024;
	m_pJpegData = new BYTE[m_dwJpegLen];
	m_pJpeg = NULL;

	m_bUseAVCodec = theApp.GetSettings().UseAVCodec();

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
	m_PerfMon.ShowResults();
	return CWK_Thread::StopThread();
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::AddPictureRecord(CCamera* pCamera,
									  const CIPCPictureRecord& pict,
									  const CIPCFields& fields, 
									  WORD wArchiveNr,
									  DWORD dwClientID,
									  CompressionType ctDestination)
{
	if (m_Queue.GetCount()<100)
	{
		CEncoderItem* pEncoderItem;
		CSecID idArchive;
		
		if (wArchiveNr!=0)
		{
			idArchive = CSecID(SECID_GROUP_ARCHIVE,wArchiveNr);
		}
		if (dwClientID !=0)
		{
			TRACE(_T("AddPictureRecord Mpeg4 %08lx\n"),pCamera->GetClientID());
		}
		pEncoderItem = new CEncoderItem(pCamera,idArchive,dwClientID,ctDestination, pict,fields);
		if (pEncoderItem)
		{
			m_Queue.SafeAddTail(pEncoderItem);
			Sleep(0);
		}
	}
	else
	{
		EmptyDataQueue();
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CEncoderThread::Run(LPVOID lpContext)
{
	HANDLE hHandles[2];

	hHandles[0]	= m_StopEvent;
	hHandles[1] = m_Queue.GetSemaphore();

	// 10 Bilder in einem Run auswerten,
	// damit das Wait auf Ende 10ms nicht zu sehr bremst
	for (int i=0; (i<20) && (m_Queue.GetCount()>0); i++)
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
			else
			{
				TRACE(_T("invalid encoder item\n"));
			}
			WK_DELETE(pEncoderItem);
		}
		else
		{
			WK_TRACE_ERROR(_T("Wait in %s unknown return value\n"),m_sName);
		}
	}
	m_EmuzedEncoders.Lock();
	for (int i=m_EmuzedEncoders.GetSize()-1;i>=0;i--)
	{
		if (m_EmuzedEncoders.GetAtFast(i)->IsDelayedDelete())
		{
			delete m_EmuzedEncoders.GetAtFast(i);
			m_EmuzedEncoders.RemoveAt(i);
		}
	}
	m_EmuzedEncoders.Unlock();
	m_AVEncoders.Lock();
	for (int i=m_AVEncoders.GetSize()-1;i>=0;i--)
	{
		if (m_AVEncoders.GetAtFast(i)->IsDelayedDelete())
		{
			delete m_AVEncoders.GetAtFast(i);
			m_AVEncoders.RemoveAt(i);
		}
	}
	m_AVEncoders.Unlock();
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
void CEncoderThread::EncodeIJL(CEncoderItem& item)
{
	const CIPCPictureRecord* pPict = item.GetPictureRecord();
	BYTE* pData = (BYTE*)pPict->GetData();
	LPBITMAPINFO lpBMI = (LPBITMAPINFO)pData;
	JPEG_CORE_PROPERTIES JpegCoreProp;
	DWORD dwJpegLen = 0;

	ZeroMemory(&JpegCoreProp,sizeof(JPEG_CORE_PROPERTIES));

	/*
	TRACE(_T("encode jpeg %08lx %s,%03d\n"),
		pPict->GetCamID().GetID(),
		pPict->GetTimeStamp().GetTime(),
		pPict->GetTimeStamp().GetMilliseconds());*/

	if (ijlInit(&JpegCoreProp) == IJL_OK)
	{
		JpegCoreProp.DIBWidth		= lpBMI->bmiHeader.biWidth;
		JpegCoreProp.DIBHeight		= lpBMI->bmiHeader.biHeight; //TopDown
		JpegCoreProp.DIBBytes		= (BYTE*)&lpBMI->bmiColors[0];
		JpegCoreProp.JPGWidth		= lpBMI->bmiHeader.biWidth;
		JpegCoreProp.JPGHeight		= abs(lpBMI->bmiHeader.biHeight);
		JpegCoreProp.jquality		= item.GetCamera()->GetJPEGQuality();
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

		m_PerfMon.Start();
		int nRet = ijlWrite(&JpegCoreProp, IJL_JBUFF_WRITEWHOLEIMAGE);
		m_PerfMon.Stop();
		if (nRet == IJL_OK)
		{
			dwJpegLen = JpegCoreProp.JPGSizeBytes;
			item.GetCamera()->AddCompressedJPEGSize(dwJpegLen);

			CSecID idArchive = item.GetArchiveID();
			if (   idArchive.IsArchiveID()
				&& idArchive!= SECID_NO_ID)
			{
				if (   theApp.GetDatabase()
					&& theApp.GetDatabase()->GetIPCState() == CIPC_STATE_CONNECTED)
				{
					CIPCPictureRecord encodedPict(theApp.GetDatabase(),
													m_pJpegData,
													dwJpegLen,
													pPict->GetCamID(),
													pPict->GetResolution(),
													item.GetCamera()->GetCompression(),
													COMPRESSION_JPEG,
													pPict->GetTimeStamp(),
													pPict->GetJobTime());
					theApp.SavePicture(item.GetCamera(),encodedPict,item.GetFields(),idArchive.GetSubID());
					Sleep(0);
				}
			}
			DWORD dwClientID = item.GetClientID();
			if (dwClientID != 0)
			{
				COutputClient* pClient = theApp.GetOutputClients().GetOutputClient(dwClientID);
				if (   pClient
					&& pClient->GetCIPC()
					&& pClient->GetCIPC()->GetIPCState() == CIPC_STATE_CONNECTED)
				{
					CIPCPictureRecord encodedPict(pClient->GetCIPC(),
												m_pJpegData,
												dwJpegLen,
												pPict->GetCamID(),
												pPict->GetResolution(),
												item.GetCamera()->GetCompression(),
												COMPRESSION_JPEG,
												pPict->GetTimeStamp(),
												pPict->GetJobTime());
					pClient->GetCIPC()->SendPicture(item.GetCamera(),encodedPict,0,0,TRUE);
//					TRACE(_T("send encode jpeg %08lx\n"),pPict->GetCamID().GetID());
					Sleep(0);
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
		int iIFrameIntervall = CCameraGroup::GetIFrameDefault(iFramerate);
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
		if (iBitrate>0 && iBitrate<128*1024)
		{
			iIFrameIntervall = 128;
		}

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
		int iIFrameIntervall = CCameraGroup::GetIFrameDefault(iFramerate);

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
		if (iBitrate>0 && iBitrate<128*1024)
		{
			iIFrameIntervall = 128;
		}

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
void CEncoderThread::EncodeAVEmuzed(CEncoderItem& item)
{
	const CIPCPictureRecord* pPict = item.GetPictureRecord();
	CSecID idArchive = item.GetArchiveID();
	CSecID idCam = item.GetCamera()->GetClientID();
	DWORD  dwClient = item.GetClientID();
	DWORD  dwID = (dwClient!=0) ? dwClient : idArchive.GetID();

//	TRACE(_T("EncodeMPEG4 ClientID=%08lx ArchiveID=%08lx CamID=%08lx\n"),dwClient,idArchive.GetID(),idCam.GetID());

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
		COutputClient* pClient = theApp.GetOutputClients().GetOutputClient(dwClient);
		if (pClient)
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
	}
	else
	{
		// try to find the right encoder
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

	if (   pAVEncoder == NULL
		&& pEmuzedEncoder == NULL
		&& item.IsValid())
	{
		CompressionType ct = COMPRESSION_JPEG;
		if (theApp.GetSettings().GetMPEG4())
		{
			ct = COMPRESSION_MPEG4;
		}
		if (   idArchive.IsArchiveID()
			&& idArchive!= SECID_NO_ID)
		{
			// create storage encoder
			if (m_bUseAVCodec)
			{
				pAVEncoder = CreateAVEncoder(pAVEncoder,idArchive,idCam,iWidth,iHeight,ct,
					item.GetCamera()->GetCompression(),0,item.GetCamera()->GetFPS());
			}
			else
			{
				pEmuzedEncoder = CreateEmuzedEncoder(pEmuzedEncoder, idArchive,idCam,iWidth,iHeight,
					item.GetCamera()->GetCompression(),0,item.GetCamera()->GetFPS());
			}
		}
		else if (dwClient != 0)
		{
			// TRACE(_T("CreateMPEG4Encoder ClientID=%08lx ArchiveID=%08lx CamID=%08lx\n"),dwClient,idArchive.GetID(),idCam.GetID());
			COutputClient* pClient = theApp.GetOutputClients().GetOutputClient(dwClient);
			if (pClient)
			{
				Compression c = COMPRESSION_NONE;

				if (pClient->IsLowBandwidth())
				{
					c = COMPRESSION_NONE;
					// iBitrate is OK
				}
				else
				{
					c = item.GetCamera()->GetCompression();
					iBitrate = 0;
				}
				if (m_bUseAVCodec)
				{
					pAVEncoder = CreateAVEncoder(pAVEncoder,dwClient,idCam,iWidth,iHeight,ct,
						c,0,item.GetCamera()->GetFPS());
				}
				else
				{
					pEmuzedEncoder = CreateEmuzedEncoder(pEmuzedEncoder,dwClient,idCam,iWidth,iHeight,
						c,iBitrate,item.GetCamera()->GetFPS());
				}
			}
		}
	}


	if (   (pEmuzedEncoder && pEmuzedEncoder->IsValid())
	    || (pAVEncoder && pAVEncoder->IsValid())
		)
	{
		m_PerfMon.Start();

		if (pEmuzedEncoder)
		{
			pEmuzedEncoder->SetYUV422Data(idCam,iSourceWidth,iSourceHeight,pDibBuffer);
			pEmuzedEncoder->Encode();
		}
		else if (pAVEncoder)
		{
			pAVEncoder->SetYUV422Data(idCam,iSourceWidth,iSourceHeight,pDibBuffer);
			pAVEncoder->Encode();
		}
		m_PerfMon.Stop();
		
		CIPC* pCIPC = NULL;
		CIPCOutputDVClient* pCIPCClient = NULL;
		CIPCDatabaseProcess* pDatabase = NULL;

		if (idArchive.IsArchiveID())
		{
			if (   theApp.GetDatabase()
				&& theApp.GetDatabase()->GetIPCState() == CIPC_STATE_CONNECTED)
			{
				pDatabase = theApp.GetDatabase();
				pCIPC = pDatabase;
			}
		}
		else if (dwClient != 0)
		{
			COutputClient* pClient = theApp.GetOutputClients().GetOutputClient(dwClient);
			if (   pClient
				&& pClient->GetCIPC()
				&& pClient->GetCIPC()->GetIPCState() == CIPC_STATE_CONNECTED)
			{
				pCIPCClient = pClient->GetCIPC();
				pCIPC = pCIPCClient;
			}
		}

		CIPCPictureRecord* pPict = NULL;

		if (pEmuzedEncoder)
		{
			pPict = pEmuzedEncoder->GetEncodedPicture(pCIPC,item.GetCamera()->GetClientID(),item.GetPictureRecord()->GetTimeStamp());
		}
		else if (pAVEncoder)
		{
			pPict = pAVEncoder->GetEncodedPicture(pCIPC,item.GetCamera()->GetClientID(),item.GetPictureRecord()->GetTimeStamp());
		}

		if (pPict)
		{
			if (idArchive.IsArchiveID())
			{
				theApp.SavePicture(item.GetCamera(),*pPict,item.GetFields(),idArchive.GetSubID());
				if (   m_bUseAVCodec
					&& pAVEncoder)
				{
					CDWordArray& dwClientIDs = pAVEncoder->GetClientIDs();
					for (int i=0;i<dwClientIDs.GetSize();i++)
					{
						COutputClient* pClient = theApp.GetOutputClients().GetOutputClient(dwClient);
						if (   pClient
							&& pClient->GetCIPC()
							&& pClient->GetCIPC()->GetIPCState() == CIPC_STATE_CONNECTED)
						{
							pClient->GetCIPC()->SendPicture(item.GetCamera(),*pPict,item.GetX(),item.GetY(),FALSE);
						}
					}
				}
			}
			else if (dwClient !=0)
			{
				pCIPCClient->SendPicture(item.GetCamera(),*pPict,item.GetX(),item.GetY(),TRUE);
			}


			WK_DELETE(pPict);
		}

	}
	else
	{
		TRACE(_T("Encoder INVALID %08lx ClientID=%08lx ArchiveID=%08lx CamID=%08lx\n"),
			pEmuzedEncoder, dwClient,idArchive.GetID(),idCam.GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::RemoveLowBandwidthClient(DWORD dwClientID)
{
	for (int i=m_EmuzedEncoders.GetSize()-1;i>=0;i--)
	{
		if (m_EmuzedEncoders.GetAtFast(i)->GetID() == dwClientID)
		{
			m_EmuzedEncoders.GetAtFast(i)->DelayedDelete();
		}
	}
	for (int i=m_AVEncoders.GetSize()-1;i>=0;i--)
	{
		if (m_AVEncoders.GetAtFast(i)->GetID() == dwClientID)
		{
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
void CEncoderThread::AddLowBandwidthClient(DWORD dwClientID, CSecID idCam)
{
	if (m_bUseAVCodec)
	{
		m_AVEncoders.Lock();
		BOOL bFound = FALSE;
		for (int i=0;i<m_AVEncoders.GetSize() && !bFound;i++)
		{
			if (   m_AVEncoders.GetAtFast(i)->GetID() == dwClientID
				&& m_AVEncoders.GetAtFast(i)->GetCamID() == idCam)
			{
				bFound = TRUE;
			}
		}
		if (!bFound)
		{
			CAVEncoder* pAVEncoder = new CAVEncoder(idCam,dwClientID);
			m_AVEncoders.Add(pAVEncoder);
		}
		m_AVEncoders.Unlock();
	}
	else
	{
		m_EmuzedEncoders.Lock();
		BOOL bFound = FALSE;
		for (int i=0;i<m_EmuzedEncoders.GetSize() && !bFound;i++)
		{
			if (   m_EmuzedEncoders.GetAtFast(i)->GetID() == dwClientID
				&& m_EmuzedEncoders.GetAtFast(i)->GetCamID() == idCam)
			{
				bFound = TRUE;
			}
		}
		if (!bFound)
		{
			CEmuzedMpeg4Encoder* pEmuzedEncoder = new CEmuzedMpeg4Encoder(idCam,dwClientID);
			m_EmuzedEncoders.Add(pEmuzedEncoder);
		}
		m_EmuzedEncoders.Unlock();
	}
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::RemoveLowBandwidthClient(DWORD dwClientID, CSecID idCam)
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
				&& pEncoderItem->GetCamera()->GetClientID() == idCam)
			{
				pEncoderItem->Invalidate();
//				TRACE(_T("Invalidate mpeg 4 encoder item %08lx\n"),dwClientID);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::AddHighBandwidthClient(DWORD dwClientID, CSecID idCam)
{
	if (m_bUseAVCodec)
	{
		for (int i=0;i<m_AVEncoders.GetSize();i++)
		{
			if (m_AVEncoders.GetAtFast(i)->GetCamID() == idCam)
			{
				m_AVEncoders.GetAtFast(i)->AddClientID(dwClientID);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::RemoveHighBandwidthClient(DWORD dwClientID)
{
	if (m_bUseAVCodec)
	{
		for (int i=0;i<m_AVEncoders.GetSize();i++)
		{
			m_AVEncoders.GetAtFast(i)->RemoveClientID(dwClientID);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CEncoderThread::RemoveHighBandwidthClient(DWORD dwClientID, CSecID idCam)
{
	if (m_bUseAVCodec)
	{
		for (int i=0;i<m_AVEncoders.GetSize();i++)
		{
			if (m_AVEncoders.GetAtFast(i)->GetCamID() == idCam)
			{
				m_AVEncoders.GetAtFast(i)->RemoveClientID(dwClientID);
			}
		}
	}
}
