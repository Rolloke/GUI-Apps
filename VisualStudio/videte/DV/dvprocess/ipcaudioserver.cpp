// IPCAudioServer.cpp: implementation of the CIPCAudioServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvprocess.h"
#include "IPCAudioServer.h"
#include "Camera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL CIPCAudioServer::gm_bTraceConfirmValues     = FALSE;
BOOL CIPCAudioServer::gm_bTraceIndicateMediaData = FALSE;

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCAudioServer::CIPCAudioServer(WORD wID, LPCTSTR pSMName, BOOL bIn, BOOL bOut)
:CIPCMedia(pSMName,TRUE)
{
	m_wGroupID = wID;
	TRACE(_T("CIPCAudioServer(%s)\n"), pSMName);
	m_iHardware = -1;
	m_dwState		= 0L;			// Der aktuelle Status der Eing. als Bitmask
	m_dwRecordingFlags = 0;
	m_bResetConfirmed = FALSE;
	m_pBroadcastingClient = NULL;

	if (bIn)
	{
		CMedia* pMediaIn = new CMedia(this,m_wGroupID,0);
		Add(pMediaIn);
		pMediaIn->SetType(SEC_MEDIA_TYPE_INPUT);
		m_EncoderID = pMediaIn->GetID();
	}
	if (bOut)
	{
		CMedia* pMediaOut = new CMedia(this,m_wGroupID,1);
		Add(pMediaOut);
		pMediaOut->SetType(SEC_MEDIA_TYPE_OUTPUT);
		m_DecoderID = pMediaOut->GetID();
	}
}
//////////////////////////////////////////////////////////////////////
CIPCAudioServer::~CIPCAudioServer()
{
	TRACE(_T("~CIPCAudioServer(%s)\n"), GetShmName());
	StopThread();
	DeleteAllMedia();
}
//////////////////////////////////////////////////////////////////////
CMedia* CIPCAudioServer::GetMedia(CSecID id)
{
	CMedia* pMedia = NULL;
	for (int i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetID() == id)
		{
			pMedia = GetAtFast(i);
			break;
		}
	}

	return pMedia;
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::Load(CWK_Profile& wkp)
{
	CSecID id = theApp.GetSettings().GetAudioCam();
	CMedia* pEncoder = GetMedia(m_EncoderID);

	if (   id != SECID_NO_ID
		&& pEncoder)
	{
		COutput* pOutput = theApp.GetOutputGroups().GetOutputByUnitID(id);
		CString sCameraName = pOutput ? pOutput->GetName() : _T("");
		pEncoder->SetName(sCameraName);
	}

	CMedia* pDecoder = GetMedia(m_DecoderID);
	if (pDecoder)
	{
		pDecoder->SetName(theApp.GetLocalHostName());
	}

}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::DeleteAllMedia()
{
	SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioServer::DoRequestMediaDecoding(CIPCAudioDVClient*pBCC, const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwFlags)
{
	if (m_pBroadcastingClient)
	{
		if (m_pBroadcastingClient != pBCC)
		{
			pBCC->DoRequestSetValue(mediaID, CSD_REJECT_ANNOUNCEMENT, _T(""));
			return;
		}
	}
	else
	{
		 m_pBroadcastingClient = pBCC;
	}
	CIPCMedia::DoRequestMediaDecoding(media, mediaID, dwFlags);
}
void CIPCAudioServer::DoRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags)
{
	m_pBroadcastingClient = NULL;
	CIPCMedia::DoRequestStopMediaDecoding(mediaID, dwFlags);
}
//////////////////////////////////////////////////////////////////////
CSecID CIPCAudioServer::GetDecoderID() const
{
	return m_DecoderID;
}
//////////////////////////////////////////////////////////////////////
CSecID CIPCAudioServer::GetEncoderID() const
{
	return m_EncoderID;
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::OnConfirmConnection()
{
	CString sMsg;
	sMsg.Format(_T("AudioUnit|%s"),GetShmName());
	WK_STAT_LOG(_T("Units"),1,sMsg);
	CIPC::OnConfirmConnection();

	DoRequestVersionInfo(m_wGroupID);
	DoRequestSetGroupID(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::OnConfirmSetGroupID(WORD wGroupID)
{
	CSecID id(wGroupID, SECID_NO_SUBID);
	DoRequestHardware(id, 0);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::OnConfirmHardware(CSecID secID, int errorCode, DWORD dwFlags, DWORD dwIDRequest, const CIPCExtraMemory *pEM)
{
	m_iHardware = errorCode;

	// Audio OK Errorcode ist 0
	if (m_iHardware == 0) 
	{
		m_bResetConfirmed = FALSE;
		DoRequestReset(m_wGroupID);
	}
	// inform clients
	CAudioClients& clients = theApp.GetAudioClients();
	clients.Lock();
	for (int ci=0;ci<clients.GetSize();ci++) 
	{
		if (clients.GetAtFast(ci)->GetCIPC())
		{
			CIPCExtraMemory* pCopy = new CIPCExtraMemory(*pEM);
			clients.GetAtFast(ci)->GetCIPC()->DoConfirmHardware(secID, errorCode, dwFlags, dwIDRequest, pCopy);
			WK_DELETE(pCopy);
		}
	}
	clients.Unlock();
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioServer::OnConfirmValues(CSecID secID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	if (!theApp.IsUpAndRunning())
	{
		return;
	}
	// inform clients
	if (gm_bTraceConfirmValues)
	{
		WK_TRACE(_T("CIPCAudioServer::OnConfirmValues(%x)=> SecID:%08x, ID:%d, F:%x\n"), dwCmd, secID.GetID(), dwID, dwValue);
	}
	
	CAudioClients& clients = theApp.GetAudioClients();
	clients.Lock();
	for (int ci=0;ci<clients.GetSize();ci++) 
	{
		if (clients.GetAtFast(ci)->GetCIPC())
		{
			clients.GetAtFast(ci)->GetCIPC()->DoConfirmValues(secID, dwCmd, dwID, dwValue, pData);
		}
	}
	clients.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::OnConfirmReset(WORD wGroupID)
{
	m_dwState = 0L;
	m_bResetConfirmed = TRUE;
	// alles OK Aufnahmeprocesse starten
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::OnRequestDisconnect()
{
	m_iHardware = -1;
	CAudioClients& clients = theApp.GetAudioClients();
	clients.Lock();
	for (int ci=0;ci<clients.GetSize();ci++) 
	{
		if (clients.GetAtFast(ci)->GetCIPC())
		{
			clients.GetAtFast(ci)->GetCIPC()->DoConfirmHardware(CSecID(m_wGroupID,SECID_NO_SUBID), -1);
		}
	}
	clients.Unlock();
}
void CIPCAudioServer::OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID secID, DWORD dwUserData)
{
	if (!theApp.IsUpAndRunning())
	{
		return;
	}
	if (gm_bTraceIndicateMediaData)
	{
		TRACE(_T("CIPCAudioServer::OnIndicateMediaData %08lx,%08lx\n"),secID.GetID(),dwUserData);
		WK_TRACE(_T("Server, No: %d, Size: %d\n"), rec.GetPackageNumber(), rec.GetActualLength());
	}

	CAudioClients& clients = theApp.GetAudioClients();
	clients.Lock();
	for (int i=0;i<clients.GetSize();i++) 
	{
		if (   clients.GetAtFast(i)->GetCIPC()
			&& clients.GetAtFast(i)->GetCIPC()->IsAudioInActive())
		{
			clients.GetAtFast(i)->GetCIPC()->DoIndicateMediaData(rec,secID,0);
		}
	}
	clients.Unlock();

	if (m_dwRecordingFlags>0)
	{
		CMedia* pMedia = GetAtFast(secID.GetSubID());
		if (pMedia->GetCamera())
		{
			// TODO samples in die datenbank speichern
			theApp.SaveMedia(rec,pMedia->GetCamera()->GetMDAlarmNr());
		}
		else
		{
			TRACE(_T("no media or camera to store media sanple\n"));
		}
	}

	CheckForStopAudioEncoding();
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::CheckForStopAudioEncoding()
{
	BOOL bSomeActive = FALSE;
	CAudioClients& clients = theApp.GetAudioClients();
	clients.Lock();
	for (int i=0;i<clients.GetSize();i++) 
	{
		if (   clients.GetAtFast(i)->GetCIPC()
			&& clients.GetAtFast(i)->GetCIPC()->IsAudioInActive())
		{
			bSomeActive = TRUE;
		}
	}
	clients.Unlock();
	if (   !bSomeActive
		&& (m_dwRecordingFlags==0)
		)
	{
		DoRequestStopMediaEncoding(m_EncoderID,0);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::SetRecording(DWORD dwFlags, BOOL bActive)
{
//	TRACE(_T("CIPCAudioServer::SetRecording %08lx,%d\n"),dwFlags,bActive);
	if (m_iHardware == 0) 
	{
		DWORD dwOldFlags = m_dwRecordingFlags;

		if (bActive)
		{
			m_dwRecordingFlags = m_dwRecordingFlags|dwFlags;
		}
		else
		{
			m_dwRecordingFlags &= ~dwFlags;
		}

		if (   dwOldFlags == 0
			&& m_dwRecordingFlags>0)
		{
			// start
			DoRequestStartMediaEncoding(GetAtFast(0)->GetID(),MEDIA_ENC_NEW_SEGMENT|MEDIA_ENC_DELIVER_ALWAYS,0);
		}
		// stop will be done in OnIndicateMediaData
		if (   dwOldFlags >0
			&& m_dwRecordingFlags==0)
		{
			CheckForStopAudioEncoding();
		}
	}
}
//////////////////////////////////////////////////////////////////////
//********************************************************************
//////////////////////////////////////////////////////////////////////
CAudioList::CAudioList()
{
	m_pDecoder = 0;
}
//////////////////////////////////////////////////////////////////////
CAudioList::~CAudioList()
{
	SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CAudioList::StartCIPCThreads()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->StartThread();
	}
}
//////////////////////////////////////////////////////////////////////
void CAudioList::WaitForConnect()
{
	BOOL bConnected = FALSE;
	int c = 10;
	while ( (c-- > 0) && (!bConnected))
	{
		bConnected = TRUE;
		for (int i=0;i<GetSize();i++)
		{
			bConnected &= GetAtFast(i)->GetIPCState()==CIPC_STATE_CONNECTED;
		}
		Sleep(50);
	}
}
//////////////////////////////////////////////////////////////////////
void CAudioList::WaitForReset()
{
	BOOL bResetConfirmed = FALSE;
	int c = 10;
	while ( (c-- > 0) && (!bResetConfirmed))
	{
		bResetConfirmed = TRUE;
		for (int i=0;i<GetSize();i++)
		{
			bResetConfirmed &= GetAtFast(i)->ResetConfirmed();
		}
		Sleep(50);
	}
}
//////////////////////////////////////////////////////////////////////
CIPCAudioServer* CAudioList::GetGroupByID(CSecID id) const
{
	for (int i=0;i<GetSize();i++) 
	{
		if (id.GetGroupID()==GetAtFast(i)->GetID()) 
		{
			return (GetAtFast(i));
		}
	}
	return (NULL);
}
//////////////////////////////////////////////////////////////////////
CIPCAudioServer* CAudioList::GetDecoder() const
{
	return m_pDecoder;
}
//////////////////////////////////////////////////////////////////////
CString CAudioList::GetAudioNameByID(CSecID id)
{
	Lock();
	CString sReturn;
	CIPCAudioServer*pAS;
	WORD wSubID = id.GetSubID();

	for (int i=0 ; i<GetSize() ; i++) 
	{
		pAS = GetAtFast(i);
		if (id.GetGroupID() == pAS->GetID()) 
		{
			if (wSubID < pAS->GetSize())
			{
				sReturn = pAS->GetAtFast(wSubID)->GetName();
			}
			break;
		}
	}
	Unlock();
	return sReturn;
}
//////////////////////////////////////////////////////////////////////
void CAudioList::Load(CWK_Profile& wkp)
{
	Lock();

	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->Load(wkp);
	}

	if (GetSize()==1)
	{
		m_pDecoder = GetAtFast(0);
	}

	Unlock();
}

