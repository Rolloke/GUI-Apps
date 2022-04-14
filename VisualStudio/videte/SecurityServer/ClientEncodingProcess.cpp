// ClientEncodingProcess.cpp: implementation of the CClientEncodingProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "ClientEncodingProcess.h"
#include "CipcOutputServerClient.h"
#include "ProcessScheduler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define _USE_YUV

CClientEncodingProcess::CClientEncodingProcess(CProcessScheduler* pScheduler,
											   CSecID idClient,
											   CSecID camID,
											   Resolution res, 
											   Compression comp,
											   CompressionType ct,
											   DWORD dwClientTimeSlice,
											   int iNumPics/*=1*/)
 : CClientProcess(pScheduler,idClient)
{
	m_type = SPT_ENCODE;

	m_pTranscoder = NULL;
	
	m_csFrameCounter.Lock();
	m_iFrameCounter = 1;
	m_csFrameCounter.Unlock();
	
	m_iPictsSendForCamera = 0;
	m_CompressionType = ct;
	CString sMacroName;


	int iPriority = 3;

	m_bUseH263 = FALSE;
	m_bUseMpeg4 = FALSE;
	m_bIsStreaming = FALSE;
	m_bIsMultiCamera = FALSE;
	m_iClientBitrate = 1024*1024; //assume 1MBit default
	m_ClientResolution = res;
	m_UnitResolution = res;

	theApp.GetOutputClients().Lock(_T(__FUNCTION__));
	CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(idClient);
	if(pClient)
	{
		sMacroName.Format(_T("Image %08lx %s"),pClient->GetID().GetID(),pClient->GetConnection().GetSourceHost());
		m_bUseH263 = pClient->UseH263();
		m_bUseMpeg4 = pClient->CanMpeg4() && pClient->IsLowBandwidth();
		m_bIsStreaming = pClient->IsStreaming();
		m_bIsMultiCamera = pClient->IsMultiCamera();
		m_iClientBitrate = pClient->GetBitrate();
		iPriority = pClient->GetPriority();
		m_bCanGOP = pClient->CanGOP();
	}
	theApp.GetOutputClients().Unlock();

	CProcessMacro *pMacro = new CProcessMacro(PMT_RECORD,
											  sMacroName,
											  SECID_NO_ID);

	// force seconds
	if (dwClientTimeSlice) 
	{
		dwClientTimeSlice = (dwClientTimeSlice / 1000)*1000;
	} 
	else 
	{
		dwClientTimeSlice=1000;
	}

	// bound check
	if (dwClientTimeSlice==0) {
		iNumPics=1;
		dwClientTimeSlice=1000;
	} else if (dwClientTimeSlice > 1000*60) {
		WK_TRACE_ERROR(_T("ClientTimeSlice overflow %d\n"),dwClientTimeSlice);
		iNumPics=1;
		dwClientTimeSlice=1000;
	}

	pMacro->SetOverallTime(CPM_TIME_INFINITY);
	pMacro->SetRecordValues(dwClientTimeSlice,iNumPics);
	pMacro->SetIsIntervall(TRUE);	// OOPS okay as default ?

	if (   m_pProcessScheduler->ParentIsMiCo()
		|| m_pProcessScheduler->ParentIsSaVic())
	{
		if (m_bUseMpeg4||m_bUseH263)
		{
			m_UnitResolution = m_pProcessScheduler->GetDefaultResolution(camID);
		}
	}
	pMacro->SetPictureParameter(comp,m_UnitResolution);

	m_pActivation = new CActivation(pMacro, this);
	m_pActivation->SetOutputID(camID);
	m_pActivation->SetPriority(iPriority); 

	m_bDropped = FALSE;

	if (   (m_bUseH263 || m_bUseMpeg4)
		&& m_bIsMultiCamera
		&& (   pScheduler->ParentIsMiCo() 
		    || pScheduler->ParentIsSaVic()
			|| pScheduler->ParentIsQ())
			)
	{
		m_pTranscoder = new CTranscoderThread(idClient);
		m_pTranscoder->StartThread();
	}

	// m_stLastPictureSent;
	m_dwLastPictureSent = 0;

}
//////////////////////////////////////////////////////////////////////
CClientEncodingProcess::~CClientEncodingProcess()
{
	if (m_pTranscoder)
	{
		m_pTranscoder->StopThread();
		WK_DELETE(m_pTranscoder);
	}
}
//////////////////////////////////////////////////////////////////////
void CClientEncodingProcess::OnModified(BOOL bNewOutputID)
{
	m_csFrameCounter.Lock();
	m_iFrameCounter++;
	m_csFrameCounter.Unlock();

//	TRACE(_T("m_iFrameCounter = %d\n"),m_iFrameCounter);
	if (bNewOutputID)
	{
		m_csFrameCounter.Lock();
		if (   !m_bUseH263 
			&& m_iFrameCounter<1)
		{
			m_iFrameCounter = 1;
		}
		m_csFrameCounter.Unlock();
		m_iPictsSendForCamera = 0;
	}
	if (   m_pProcessScheduler->ParentIsMiCo()
		|| m_pProcessScheduler->ParentIsSaVic())
	{
		if (m_bUseMpeg4||m_bUseH263)
		{
			m_ClientResolution = GetMacro().GetResolution();
			CProcessMacro& m = (CProcessMacro&)GetMacro();
			m.SetResolution(m_UnitResolution);
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CClientEncodingProcess::PictureData(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY,DWORD dwUserID)
{
//	TRACE(_T("image nr %d\n"),pict.GetBlockNr());
	if (   m_state != SPS_TERMINATED
		&& (   pict.GetCamID() == GetCamID()
			|| m_iPictsSendForCamera==0)
		)

	{
		m_csTimeStamps.Lock();
		m_stLastPictureGot = pict.GetTimeStamp();
		m_csTimeStamps.Unlock();
		CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(m_idClient);
		if (   pClient
			&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			// make a copy of the picture
			CIPCPictureRecord* pPict = new CIPCPictureRecord(pict);

			if (pict.GetCompressionType() == COMPRESSION_H263)
			{
				SendPicture(*pPict,dwMDX,dwMDY);
			}
			else if (   pPict->GetCompressionType() == COMPRESSION_JPEG
					 || pPict->GetCompressionType() == COMPRESSION_YUV_422
					 || pPict->GetCompressionType() == COMPRESSION_RGB_24)
			{
				if (m_bIsStreaming)
				{
					SendPicture(*pPict,dwMDX,dwMDY);
				}
				else
				{
					m_csFrameCounter.Lock();
					if (m_iFrameCounter>0)
					{
						m_csFrameCounter.Unlock();
						SendPicture(*pPict,dwMDX,dwMDY);
						m_csFrameCounter.Lock();
						m_iFrameCounter--;
//						TRACE(_T("m_iFrameCounter = %d\n"),m_iFrameCounter);
					}
					m_csFrameCounter.Unlock();
				}
			}
			else if (pPict->GetCompressionType() == COMPRESSION_MPEG4)
			{
				// Start mit I-Bild garantieren.
				if (   m_iPictsSendForCamera !=0
					|| pPict->GetPictureType() == SPT_FULL_PICTURE
					|| pPict->GetPictureType() == SPT_GOP_PICTURE)
				{
					if (m_bIsStreaming)
					{
						SendPicture(*pPict,dwMDX,dwMDY);
					}
					else
					{
						m_csFrameCounter.Lock();
						if (m_iFrameCounter>0)
						{
							m_csFrameCounter.Unlock();
							if (   !m_bDropped
								|| pPict->GetPictureType() == SPT_FULL_PICTURE
								|| pPict->GetPictureType() == SPT_GOP_PICTURE)
							{
								SendPicture(*pPict,dwMDX,dwMDY);
/*
								TRACE(_T("CClientEncodingProcess::SendPicture %d %08lx\n"),
									m_iFrameCounter,pPict->GetCamID());
*/
								m_csFrameCounter.Lock();
								if (   pPict->GetPictureType()==SPT_GOP_PICTURE
									&& !m_bCanGOP)
								{
									m_iFrameCounter -= pPict->GetNumberOfPictures();
								}
								else
								{
									m_iFrameCounter--;
								}
								m_csFrameCounter.Unlock();
//								TRACE(_T("m_iFrameCounter = %d\n"),m_iFrameCounter);
								m_bDropped = FALSE;
							}
						}
						else
						{
							m_csFrameCounter.Unlock();
							if (dwUserID == 1)
							{
								SendPicture(*pPict,dwMDX,dwMDY);
							}
							else
							{
								m_csFrameCounter.Lock();
								if (!m_bDropped)
								{
									/*
									TRACE(_T("dropping frame by counter %d Cam %08lx Client%08lx\n"),
										m_iFrameCounter,pPict->GetCamID().GetID(),m_idClient.GetID());
										*/
									m_bDropped = TRUE;
								}
								m_csFrameCounter.Unlock();
							}
						}
					}
				}
				else
				{
				}
			}
			else
			{
				WK_TRACE(_T("unknown compression type for client image%s\n"),NameOfEnum(pPict->GetCompressionType()));
			}
			WK_DELETE(pPict);
		}
		else
		{
			WK_TRACE(_T("client already disconnected for image %08lx\n"),pict.GetCamID().GetID());
		}
	}
	else
	{
		// already terminated
		WK_TRACE(_T("client image wrong for process %s %08lx\n"),NameOfEnum(m_state),pict.GetCamID().GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CClientEncodingProcess::SendPicture(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY)
{
	CIPCOutputServerClient* pClient = theApp.GetOutputClients().GetClientByID(m_idClient);
	if (   pClient
		&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		if (   (m_bUseH263)
			&& m_bIsMultiCamera
			&& (   pict.GetCompressionType() == COMPRESSION_JPEG
			    || pict.GetCompressionType() == COMPRESSION_YUV_422
				|| pict.GetCompressionType() == COMPRESSION_RGB_24
				)
			)
		{
			CIPCPictureRecord copy(pict.GetCIPC(),
									pict.GetData(),
									pict.GetDataLength(),
									pict.GetCamID(),
									m_ClientResolution,
									pict.GetCompression(),
									pict.GetCompressionType(),
									pict.GetTimeStamp(),
									pict.GetJobTime(),
									NULL,
									pict.GetBitrate(),
									pict.GetPictureType(),
									pict.GetBlockNr());
			m_pTranscoder->AddPictureRecordH263(copy);
		}
		else if (   (m_bUseMpeg4)
			&& m_bIsMultiCamera
			&& (   pict.GetCompressionType() == COMPRESSION_JPEG
			|| pict.GetCompressionType() == COMPRESSION_YUV_422
			|| pict.GetCompressionType() == COMPRESSION_RGB_24
			)
			)
		{
			CIPCPictureRecord copy(pict.GetCIPC(),
				pict.GetData(),
				pict.GetDataLength(),
				pict.GetCamID(),
				m_ClientResolution,
				pict.GetCompression(),
				pict.GetCompressionType(),
				pict.GetTimeStamp(),
				pict.GetJobTime(),
				NULL,
				pict.GetBitrate(),
				pict.GetPictureType(),
				pict.GetBlockNr());
			m_pTranscoder->AddPictureRecordMpeg4(copy);
		}
		else
		{
			pClient->SendPicture(pict,m_CompressionType,GetMacro().GetCompression(),dwMDX,dwMDY);
			m_iPictsSendForCamera++;
		}
		m_csTimeStamps.Lock();
		m_stLastPictureSent = pict.GetTimeStamp();
		m_dwLastPictureSent = WK_GetTickCount();
		m_csTimeStamps.Unlock();
	}
}
/////////////////////////////////////////////////////////////////////////
CString CClientEncodingProcess::Format()
{
	CString sRet;

	m_csTimeStamps.Lock();
	sRet.Format(_T("%s FRAMECOUNT=%d,PICSSEND=%d,LASTSEND=%s,LASTGOT=%s,DROPPED=%d"),
		CClientProcess::Format(),m_iFrameCounter,m_iPictsSendForCamera
		,m_stLastPictureSent.GetTime(),m_stLastPictureGot.GetTime(),m_bDropped);
	m_csTimeStamps.Unlock();

	return sRet;
}
/////////////////////////////////////////////////////////////////////////
BOOL CClientEncodingProcess::IsPictureTimeout()
{
	BOOL bRet = FALSE;
	m_csTimeStamps.Lock();
	if (   m_dwLastPictureSent>0
		&& m_iFrameCounter>0)
	{
		bRet = WK_GetTimeSpan(m_dwLastPictureSent) > 5000;
	}
	m_csTimeStamps.Unlock();
	return bRet;
}
/////////////////////////////////////////////////////////////////////////
void CClientEncodingProcess::ResetCounter()
{
	m_csTimeStamps.Lock();
	m_dwLastPictureSent = WK_GetTickCount();
	m_csTimeStamps.Unlock();
}
