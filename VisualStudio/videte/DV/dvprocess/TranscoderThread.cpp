// TranscoderThread.cpp: implementation of the CTranscoderThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvprocess.h"
#include "TranscoderThread.h"
#include "IPCOutputDVClient.h"
#include "Camera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static _TCHAR s_szName[8];
static int s_iName = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTranscoderThread::CTranscoderThread(CIPCOutputDVClient* pClient)
 :CWK_Thread(CString(_T("Transcoder"))+CString(_itot(s_iName++,s_szName,16))),m_PerfMonMPEG4(_T("Jpeg -> Mpeg4"))
{
	m_dwStatTime = 10*60*1000;
	m_pClient = pClient;
	m_pJpeg = NULL;
	m_pH263Encoder = NULL;
	m_pMpeg4Encoder = NULL;
	m_dwBitrate = pClient->GetClient()->GetBitrate();
	m_iNumPictures = 1;

	CreateJPEGDecoder();
}
//////////////////////////////////////////////////////////////////////
CTranscoderThread::~CTranscoderThread()
{
	TRACE(_T("CTranscoderThread::~CTranscoderThread()\n"));
	EmptyDataQueue();
	WK_DELETE(m_pJpeg);
	WK_DELETE(m_pH263Encoder);
	WK_DELETE(m_pMpeg4Encoder);
}
//////////////////////////////////////////////////////////////////////
void CTranscoderThread::EmptyDataQueue()
{
	m_PictureQueueH263.Lock();
	m_PictureQueueH263.DeleteAll();
	HANDLE hSemaphore = m_PictureQueueH263.GetSemaphore();
	while (WAIT_TIMEOUT != WaitForSingleObject(hSemaphore,0))
	{
	}
	m_PictureQueueH263.Unlock();

	m_PictureQueueMpeg4.Lock();
	m_PictureQueueMpeg4.DeleteAll();
	hSemaphore = m_PictureQueueMpeg4.GetSemaphore();
	while (WAIT_TIMEOUT != WaitForSingleObject(hSemaphore,0))
	{
	}
	m_PictureQueueMpeg4.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CTranscoderThread::AddPictureRecordH263(const CIPCPictureRecord& pict)
{
	m_PictureQueueH263.SafeAddTail(new CIPCPictureRecord(pict));
}
//////////////////////////////////////////////////////////////////////
void CTranscoderThread::AddPictureRecordMpeg4(const CIPCPictureRecord& pict)
{
	m_PictureQueueMpeg4.SafeAddTail(new CIPCPictureRecord(pict));
}
//////////////////////////////////////////////////////////////////////
BOOL CTranscoderThread::Run(LPVOID lpContext)
{
	HANDLE hHandles[3];
	DWORD dwNum = m_PictureQueueH263.GetCount()+m_PictureQueueMpeg4.GetCount();

	hHandles[0]	= m_StopEvent;
	hHandles[1] = m_PictureQueueH263.GetSemaphore();
	hHandles[2] = m_PictureQueueMpeg4.GetSemaphore();

	// 50 Bilder in einem Run auswerten,
	// damit das Wait auf Ende 10ms nicht zu sehr bremst
	for (int i=0; (i<50) && (dwNum>0); i++)
	{
		DWORD r = WaitForMultipleObjects(3,hHandles,FALSE,100);
		
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
			WK_TRACE(_T("stop saving %s\n"),m_sName);
			m_StopEvent.SetEvent();
			break;
		}
		else if (r==WAIT_OBJECT_0+1)
		{
			// take FirstOut picture of queue
			CIPCPictureRecord* pPictureRecord = m_PictureQueueH263.SafeGetAndRemoveHead();
			if (pPictureRecord)
			{
				TranscodeH263(*pPictureRecord);
			}
			WK_DELETE(pPictureRecord);
		}
		else if (r==WAIT_OBJECT_0+2)
		{
			// take FirstOut picture of queue
			CIPCPictureRecord* pPictureRecord = m_PictureQueueMpeg4.SafeGetAndRemoveHead();
			if (pPictureRecord)
			{
				TranscodeMpeg4(*pPictureRecord);
			}
			WK_DELETE(pPictureRecord);
		}
		else
		{
			WK_TRACE_ERROR(_T("Wait in %s unknown return value\n"),m_sName);
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CTranscoderThread::TranscodeH263(const CIPCPictureRecord& pict)
{
	if (   m_pClient
		&& m_pClient->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		CSize imageDims;
		LPBITMAPINFOHEADER lpBmiHeader = NULL;
		BYTE* pRGBData = NULL;
		if (pict.GetCompressionType() == COMPRESSION_JPEG)
		{
			if (m_pJpeg == NULL)
			{
				m_pJpeg = new CJpeg();
				m_pJpeg->SetProperties(768,576,IJL_YCBCR);
			}
			
			m_pJpeg->DecodeJpegFromMemory((BYTE*)pict.GetData(),
											pict.GetDataLength(),
											TRUE,
											CJ_NOTHING,TRUE);
			imageDims = m_pJpeg->GetImageDims();
		}
		else if (   pict.GetCompressionType() == COMPRESSION_RGB_24
			     || pict.GetCompressionType() == COMPRESSION_YUV_422)
		{
			BYTE* pData = (BYTE*)pict.GetData();
			lpBmiHeader = (LPBITMAPINFOHEADER)pData;
			pRGBData = pData+lpBmiHeader->biSize;
			imageDims.cx = lpBmiHeader->biWidth;
			imageDims.cy = lpBmiHeader->biHeight;
		}

		if (m_pH263Encoder == NULL)
		{
			m_pH263Encoder = new CH26xEncoder();
			m_pH263Encoder->Init(imageDims.cx,
								 imageDims.cy,
								 0,0,FALSE);
		}

		if (pict.GetCompressionType() == COMPRESSION_JPEG)
		{
			if (m_pJpeg)
			{
				m_pH263Encoder->SetYUV422Data(pict.GetCamID(),
											  m_pJpeg->GetImageDims().cx,
											  m_pJpeg->GetImageDims().cy,
											  m_pJpeg->GetDibBuffer());
			}
		}
		else if (pict.GetCompressionType() == COMPRESSION_RGB_24)
		{
			if (pRGBData)
			{
				m_pH263Encoder->SetRGBData(pict.GetCamID(),imageDims.cx,imageDims.cy,pRGBData);
			}
		}
		else if (pict.GetCompressionType() == COMPRESSION_YUV_422)
		{
			if (pRGBData)
			{
				m_pH263Encoder->SetYUV422Data(pict.GetCamID(),imageDims.cx,imageDims.cy,pRGBData);
			}
		}
		m_pH263Encoder->Run();

		if (m_pClient)
		{
			CCamera* pCamera = (CCamera*)theApp.GetOutputGroups().GetOutputByUnitID(pict.GetCamID());
			CSecID id = pict.GetCamID();
			if (pCamera)
			{
				id = pCamera->GetClientID();
			}
			CIPCPictureRecord* pPict = m_pH263Encoder->GetEncodedPicture(m_pClient,
																		 id,
																		 pict.GetTimeStamp());
			if (pPict)
			{
				if (theApp.GetSettings().TraceH263())
				{
					WK_TRACE(_T("sending H.263 %s size %dB %dx%d %08lx\n"),
						(pPict->GetPictureType()==SPT_FULL_PICTURE) ? _T("I") : _T("P"),
						pPict->GetDataLength(),
						m_pH263Encoder->GetWidth(),m_pH263Encoder->GetHeight(),
						pPict->GetCamID().GetID());
				}
				m_pClient->SendPicture(pCamera,*pPict,0,0,TRUE);
			}
			else
			{
				WK_TRACE_ERROR(_T("NO H.263 Encoder\n"));
			}
			WK_DELETE(pPict);
		}
	}
	else
	{
//		WK_TRACE(_T("transcoding client already disappeared\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CTranscoderThread::TranscodeMpeg4(const CIPCPictureRecord& pict)
{
	if (   m_pClient
		&& m_pClient->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		m_PerfMonMPEG4.Start();
		CSize imageDims;
		LPBITMAPINFOHEADER lpBmiHeader = NULL;
		BYTE* pRGBData = NULL;
		if (pict.GetCompressionType() == COMPRESSION_JPEG)
		{
			m_pJpeg->DecodeJpegFromMemory((BYTE*)pict.GetData(),pict.GetDataLength(),FALSE,CJ_NOTHING,TRUE);
			imageDims = m_pJpeg->GetImageDims();
		}
		else if (   pict.GetCompressionType() == COMPRESSION_RGB_24
			|| pict.GetCompressionType() == COMPRESSION_YUV_422)
		{
			BYTE* pData = (BYTE*)pict.GetData();
			lpBmiHeader = (LPBITMAPINFOHEADER)pData;
			pRGBData = pData+lpBmiHeader->biSize;
			imageDims.cx = lpBmiHeader->biWidth;
			imageDims.cy = lpBmiHeader->biHeight;
		}
		int iWidth = imageDims.cx;
		int iHeight = imageDims.cy;

		if (   pict.GetResolution() == RESOLUTION_2CIF
			&& iWidth>384)
		{
			iWidth = 352;
			iHeight = 288;
		}
		else if (   pict.GetResolution() == RESOLUTION_QCIF
			&& iWidth>192)
		{
			iWidth = 176;
			iHeight = 144;
		}

		if (   m_pMpeg4Encoder
			&& (   m_pMpeg4Encoder->GetWidth()!=iWidth
			    || m_pMpeg4Encoder->GetHeight()!=iHeight
				|| m_idLastCamera != pict.GetCamID())
			)
		{
			// Auflösungswechsel neuer Encoder
			// Kamerawechsel neuer Encoder
			WK_DELETE(m_pMpeg4Encoder);
		}

		if (m_pMpeg4Encoder == NULL)
		{
			m_pMpeg4Encoder = new CMPEG4Encoder();
			if (m_pMpeg4Encoder->Init(iWidth,iHeight,32000,6,50))
			{
			}
			else
			{
				WK_DELETE(m_pMpeg4Encoder);
			}
		}

		if (m_pMpeg4Encoder)
		{
			if (pict.GetCompressionType() == COMPRESSION_JPEG)
			{
				if (m_pJpeg)
				{
					m_pMpeg4Encoder->SetYUV422Data(pict.GetCamID(),
						m_pJpeg->GetImageDims().cx,
						m_pJpeg->GetImageDims().cy,
						m_pJpeg->GetDibBuffer());
				}
			}
			else if (pict.GetCompressionType() == COMPRESSION_RGB_24)
			{
				if (pRGBData)
				{
					m_pMpeg4Encoder->SetRGBData(pict.GetCamID(),imageDims.cx,imageDims.cy,pRGBData);
				}
			}
			else if (pict.GetCompressionType() == COMPRESSION_YUV_422)
			{
				if (pRGBData)
				{
					m_pMpeg4Encoder->SetYUV422Data(pict.GetCamID(),imageDims.cx,imageDims.cy,pRGBData);
				}
			}
			m_pMpeg4Encoder->Encode();
			m_idLastCamera = pict.GetCamID();

			if (   m_pClient
				&& m_pClient->GetIPCState() == CIPC_STATE_CONNECTED)
			{
				CCamera* pCamera = (CCamera*)theApp.GetOutputGroups().GetOutputByUnitID(pict.GetCamID());
				CSecID id = pict.GetCamID();
				if (pCamera)
				{
					id = pCamera->GetClientID();
				}
				CIPCPictureRecord* pPict = m_pMpeg4Encoder->GetEncodedPicture(m_pClient,
																			id,
																			pict.GetTimeStamp());
				if (pPict)
				{
					if (pPict)
					{
						if (theApp.GetSettings().TraceH263())
						{
							WK_TRACE(_T("sending Mpeg4 %s size %dB %dx%d %08lx\n"),
								(pPict->GetPictureType()==SPT_FULL_PICTURE) ? _T("I") : _T("P"),
								pPict->GetDataLength(),
								m_pMpeg4Encoder->GetWidth(),m_pMpeg4Encoder->GetHeight(),
								pPict->GetCamID().GetID());
						}
						m_pClient->SendPicture(pCamera,*pPict,0,0,TRUE);
					}
					else
					{
						WK_TRACE_ERROR(_T("NO Mpeg4 Encoder\n"));
					}
				}
				WK_DELETE(pPict);
			}
		}
		m_PerfMonMPEG4.Stop();
	}
	else
	{
		//		WK_TRACE(_T("transcoding client already disappeared\n"));
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CTranscoderThread::StopThread()
{
	m_pClient = NULL;
	EmptyDataQueue();
	return CWK_Thread::StopThread();
}
//////////////////////////////////////////////////////////////////////
void CTranscoderThread::CreateJPEGDecoder()
{
	if (m_pJpeg == NULL)
	{
		m_pJpeg = new CJpeg();
		m_pJpeg->SetProperties(756,288,IJL_YCBCR);
	}
}
//////////////////////////////////////////////////////////////////////
void CTranscoderThread::OnModified(int iNumPictures, DWORD dwBitrate)
{
	if (   m_dwBitrate != dwBitrate
		|| m_iNumPictures != iNumPictures)
	{
		m_dwBitrate = dwBitrate;
		m_iNumPictures = iNumPictures;
		if (m_pH263Encoder)
		{
			m_pH263Encoder->SetBitrate(dwBitrate,iNumPictures);
		}
	}
}
