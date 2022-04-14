// TranscoderThread.cpp: implementation of the CTranscoderThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "TranscoderThread.h"
#include "ClientEncodingProcess.h"
#include "ProcessSchedulerVideo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static char szName[8];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTranscoderThread::CTranscoderThread(CSecID idClient)
 :CWK_Thread(CString(_T("Transcoder"))+CString(itoa(idClient.GetID(),szName,16)))
{
	m_dwStatTime = 8*60*60*1000;
	m_idClient = idClient;
	m_pJpeg = NULL;
	m_pH263Encoder = NULL;
	m_pMpeg4Encoder = NULL;
	CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(idClient);
	m_bStreaming = pClient->IsStreaming();
	m_dwBitrate = pClient->GetBitrate();
	m_iNumPictures = 1;
	CreateJPEGDecoder();
}
//////////////////////////////////////////////////////////////////////
CTranscoderThread::~CTranscoderThread()
{
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

	// 5 Bilder in einem Run auswerten,
	// damit das Wait auf Ende 10ms nicht zu sehr bremst
	for (int i=0; (i<5) && (dwNum>0); i++)
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
	BOOL bSendTestImage = TRUE;

	CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(m_idClient);
	if (   pClient
		&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		int	w,h;
		BYTE* pData = NULL;
		w = 0;
		h = 0;

		if (pict.GetJobTime() != 0)
		{
			LPBITMAPINFOHEADER lpBmiHeader = NULL;

			if (pict.GetCompressionType() == COMPRESSION_JPEG)
			{
				if (m_pJpeg->DecodeJpegFromMemory((BYTE*)pict.GetData(),
					pict.GetDataLength(),
					FALSE,
					CJ_NOTHING,
					TRUE))
				{
					w = m_pJpeg->GetImageDims().cx;
					h = m_pJpeg->GetImageDims().cy;
					pData = (BYTE*)m_pJpeg->GetDibBuffer();
					bSendTestImage = FALSE;
				}
			}
			else if (pict.GetCompressionType() == COMPRESSION_YUV_422)
			{
				bSendTestImage = FALSE;
				pData = (BYTE*)pict.GetData();
				lpBmiHeader = (LPBITMAPINFOHEADER)pData;
				pData = pData+lpBmiHeader->biSize;
				w = lpBmiHeader->biWidth;
				h = lpBmiHeader->biHeight;
			}
		}
		
		if (!bSendTestImage)
		{
			int iH263Width = w;
			int iH263Height = h;

			// fuer Multicamera die Auflösung reduzieren

			if (pClient->IsMultiCamera())
			{
				if (   pict.GetResolution() == RESOLUTION_2CIF
					&& iH263Width>384)
				{
					iH263Width = 352;
					iH263Height = 288;
				}
				else if (   pict.GetResolution() == RESOLUTION_QCIF
						 && iH263Width>192)
				{
					iH263Width = 176;
					iH263Height	= 144;
				}
			}

			if (m_pH263Encoder == NULL)
			{
				m_pH263Encoder = new CH26xEncoder();
				m_pH263Encoder->Init(iH263Width,iH263Height,
									 m_dwBitrate,
									 m_iNumPictures,
									 m_bStreaming);
			}
			else
			{
				if (   iH263Width != m_pH263Encoder->GetWidth()
					|| iH263Height > m_pH263Encoder->GetHeight())
				{
					WK_DELETE(m_pH263Encoder);
					m_pH263Encoder = new CH26xEncoder();
					m_pH263Encoder->Init(iH263Width,iH263Height,
										m_dwBitrate,
										m_iNumPictures,
										m_bStreaming);
				}
			}
			m_pH263Encoder->SetYUV422Data(pict.GetCamID(),
										  w,
										  h,
										  pData);
			m_pH263Encoder->Run();
			
			CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(m_idClient);
			if (   pClient
				&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
			{
				CIPCPictureRecord* pPict = m_pH263Encoder->GetEncodedPicture(pClient,
																			 pict.GetCamID(),
																			 pict.GetTimeStamp());
/*
				CString s;
				s.Format(_T("recoded %08lx to %08lx in"),pict.GetCamID(),pPict->GetCamID().GetID());
*/
				if (pPict)
				{
// #ifndef _DEBUG
					if (CProcessSchedulerVideo::m_iTraceH263Data)
// #endif
					{
						WK_TRACE(_T("%08lx %08lx Nr %d sending H.263 %s size %d at %02d.%02d.%02d,%03d\n"),
							pClient->GetID().GetID(),
							pPict->GetCamID().GetID(),
							pPict->GetBlockNr(),
							(pict.GetPictureType() == SPT_FULL_PICTURE)?_T("I"):(pict.GetPictureType() == SPT_GOP_PICTURE)?_T("G"):_T("P"),
							pPict->GetDataLength(),
							pPict->GetTimeStamp().GetHour(),
							pPict->GetTimeStamp().GetMinute(),
							pPict->GetTimeStamp().GetSecond(),
							pPict->GetTimeStamp().GetMilliseconds());
					}
					// TODO archive id
					CSecID idArchive;
					COutput* pOutput = theApp.GetOutputGroups().GetOutputByID(pPict->GetCamID());
					if (pOutput)
					{
						idArchive = pOutput->GetCurrentArchiveID();
					}
					pClient->DoIndicateH263Data(*pPict,pPict->GetCamID().GetID(),idArchive);
				}
				WK_DELETE(pPict);
			}
		}
		else
		{
			TRACE(_T("cannot decode jpeg image\n"));
			CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(m_idClient);
			if (   pClient
				&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
			{
				// sending a test image
				CIPCPictureRecord* pPict = NULL;
				if (pict.GetResolution() == RESOLUTION_2CIF)
				{
					if (theApp.m_lpLargeTestbild)
					{
						pPict = new CIPCPictureRecord(pClient,
													  (const BYTE *)theApp.m_lpLargeTestbild,
														theApp.m_dwLargeTestbildLen,
														pict.GetCamID(),
														RESOLUTION_2CIF,
														pict.GetCompression(),
														pict.GetCompressionType(),
														pict.GetTimeStamp(),
														pict.GetJobTime());
					}
				}
				else
				{
					if (theApp.m_lpSmallTestbild)
					{
						pPict = new CIPCPictureRecord(pClient,
													(const BYTE *)theApp.m_lpSmallTestbild,
													theApp.m_dwSmallTestbildLen,
													pict.GetCamID(),
													RESOLUTION_QCIF,
													pict.GetCompression(),
													pict.GetCompressionType(),
													pict.GetTimeStamp(),
													pict.GetJobTime());
					}
				}
				if (pPict)
				{
					CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(m_idClient);
					if (   pClient
						&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
					{
						pClient->SendPicture(*pPict,COMPRESSION_H263,pPict->GetCompression(),0,0);
					}
					WK_DELETE(pPict);
				}
			}
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
	CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(m_idClient);
	if (   pClient
		&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		CSize imageDims(0, 0);
		LPBITMAPINFOHEADER lpBmiHeader = NULL;
		BYTE* pRGBData = NULL;
		if (pict.GetCompressionType() == COMPRESSION_JPEG)
		{
			m_pJpeg->DecodeJpegFromMemory((BYTE*)pict.GetData(), pict.GetDataLength(), FALSE, CJ_NOTHING, TRUE);
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
		else
		{
			WK_TRACE(_T("invalid compression type %s in transcoder\n"),NameOfEnum(pict.GetCompressionType()));
		}
		int iWidth = imageDims.cx;
		int iHeight = imageDims.cy;

		if (   iWidth  > 0 
			&& iHeight > 0)
		{
			if (pClient->IsMultiCamera())
			{
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
			}

			if (   m_pMpeg4Encoder
				&& (   m_pMpeg4Encoder->GetWidth()!=iWidth
					|| m_pMpeg4Encoder->GetHeight()!=iHeight)
				)
			{
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

				if (   pClient
					&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
				{
					CIPCPictureRecord* pPict = m_pMpeg4Encoder->GetEncodedPicture(pClient,
						pict.GetCamID(),
						pict.GetTimeStamp());
					if (pPict)
					{
						CSecID idArchive;
						COutput* pOutput = theApp.GetOutputGroups().GetOutputByID(pPict->GetCamID());
						if (pOutput)
						{
							idArchive = pOutput->GetCurrentArchiveID();
						}
						pClient->DoIndicateVideo(*pPict,0,0,0,idArchive);
					}
					WK_DELETE(pPict);
				}
			}
		}
		else
		{
			CString sFmt;
			sFmt.Format(_T("Invalid Image Dimensions (%d, %d)"), iWidth, iHeight);
#ifdef _DEBUG
			CWK_RunTimeError err(WAI_SECURITY_SERVER, REL_WARNING, RTE_INVALID, sFmt);
			err.Send();
#endif
			WK_TRACE_ERROR(_T("%s\n"), sFmt);
		}
	}
	else
	{
		//		WK_TRACE(_T("transcoding client already disappeared\n"));
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CTranscoderThread::StopThread()
{
	m_idClient = SECID_NO_ID;
	EmptyDataQueue();
	return CWK_Thread::StopThread();
}
//////////////////////////////////////////////////////////////////////
void CTranscoderThread::CreateJPEGDecoder()
{
	if (m_pJpeg == NULL)
	{
		m_pJpeg = new CJpeg();
		m_pJpeg->SetProperties(768,288,IJL_YCBCR);
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
