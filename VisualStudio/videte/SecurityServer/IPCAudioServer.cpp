// IPCAudioServer.cpp: implementation of the CIPCAudioServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "IPCAudioServer.h"
#include "ProcessSchedulerAudio.h"

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

CIPCAudioServer::CIPCAudioServer(LPCTSTR pName, WORD wID, LPCTSTR pSMName)
:CIPCMedia(pSMName,TRUE),CIOGroup(pName)
{
	m_wGroupID = wID;
	WK_TRACE(_T("CIPCAudioServer(%s)\n"), pSMName);
	m_iHardware = -1;
	m_pProcessScheduler = NULL;
	m_pBroadcastingClient = NULL;
	m_dwHardwareStateFlags = 0;
}
//////////////////////////////////////////////////////////////////////
CIPCAudioServer::~CIPCAudioServer()
{
	WK_TRACE(_T("~CIPCAudioServer(%s)\n"), GetShmName());
	StopThread();
	WK_DELETE(m_pProcessScheduler);
	DeleteAllMedia();
}
//////////////////////////////////////////////////////////////////////
CSecID CIPCAudioServer::GetDecoderID() const
{
	return m_DecoderID;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCAudioServer::Load(CWK_Profile& wkp, int iSize)
{
	for (WORD i=0;i<iSize;i++)
	{
		CMedia* pMedia = new CMedia(m_wGroupID,i);
		if (pMedia->Load(wkp))
		{
			AddMedia(pMedia);
			if (   m_DecoderID == SECID_NO_ID
				&& (pMedia->GetType() == SEC_MEDIA_TYPE_OUTPUT))
			{
				m_DecoderID = pMedia->GetID();
				WK_TRACE(_T("default audio decoder id is %08lx\n"),m_DecoderID.GetID());
			}
		}
		else
		{
			delete pMedia;
			WK_TRACE_ERROR(_T("invalid media no type or no name\n"));
		}
	}
	// TODO Audio, wann ist der richtige Zeitpunkt dafür ??
	m_pProcessScheduler = new CProcessSchedulerAudio(this);
	return GetSize()>0;
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

	DoRequestVersionInfo(GetGroupID());
	DoRequestSetGroupID(m_wGroupID);
	theApp.UpdateStateInfo();
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
	m_dwHardwareStateFlags = 0;
	// Audio OK Errorcode ist 0
	if (m_iHardware == 0) 
	{
		DoRequestReset(m_wGroupID);
		m_dwHardwareStateFlags = dwFlags;
	}
	// inform clients
	CIPCAudioClientsArray* pClients = theApp.GetAudioClients();
	if (pClients)
	{
		if (secID.GetSubID() == SECID_NO_SUBID)
		{
			for (int ci=0;ci<pClients->GetSize();ci++) 
			{
				CIPCExtraMemory* pCopy = new CIPCExtraMemory(*pEM);
				pClients->GetAtFast(ci)->DoConfirmHardware(secID, errorCode, dwFlags, dwIDRequest, pCopy);
				WK_DELETE(pCopy);
			}
		}
		else
		{
			CIPCExtraMemory* pCopy = new CIPCExtraMemory(*pEM);
			pClients->GetClientBySubID(secID.GetSubID())->DoConfirmHardware(secID, errorCode, dwFlags, dwIDRequest, pCopy);
			WK_DELETE(pCopy);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioServer::OnConfirmValues(CSecID secID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	// inform clients
	if (gm_bTraceConfirmValues)
	{
		WK_TRACE(_T("CIPCAudioServer::OnConfirmValues(%x)=> SecID:%08x, ID:%d, F:%x\n"), dwCmd, secID.GetID(), dwID, dwValue);
	}
	
	CIPCAudioClientsArray* pClients = theApp.GetAudioClients();
	if (pClients)
	{
		for (int ci=0;ci<pClients->GetSize();ci++) 
		{
			pClients->GetAtFast(ci)->DoConfirmValues(secID, dwCmd, dwID, dwValue, pData);
		}
 
//		CIPCAudioServerClient*pClient = pClients->GetClientBySubID(secID.GetSubID());
//		if (pClient) pClient->DoConfirmValues(secID, dwCmd, dwID, dwValue, pData);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::OnConfirmReset(WORD wGroupID)
{
	m_dwState = 0L;
	// alles OK Aufnahmeprocesse starten
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioServer::OnRequestDisconnect()
{
	if (m_bShutdownInProgress) 
	{
		return;	// EXIT!
	}
	m_iHardware = -1;
	CIPCAudioClientsArray* pClients = theApp.GetAudioClients();
	if (pClients)
	{
		for (int ci=0;ci<pClients->GetSize();ci++) 
		{
			pClients->GetAtFast(ci)->DoConfirmHardware(CSecID(m_wGroupID,SECID_NO_SUBID), -1);
		}
	}
	theApp.UpdateStateInfo();
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioServer::OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID secID, DWORD dwUserData)
{
	// NOT YET AUDIO processes
	if (gm_bTraceIndicateMediaData)
	{
		TRACE(_T("CIPCAudioServer::OnIndicateMediaData %08lx,%08lx\n"),secID.GetID(),dwUserData);
		WK_TRACE(_T("Server, No: %d, Size: %d\n"), rec.GetPackageNumber(), rec.GetActualLength());
	}

	m_pProcessScheduler->OnIndicateMediaData(rec,secID,dwUserData);
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioServer::DoRequestMediaDecoding(CIPCAudioServerClient*pBCC, const CIPCMediaSampleRecord& media, CSecID mediaID, DWORD dwFlags)
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
//////////////////////////////////////////////////////////////////////////
void CIPCAudioServer::DoRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags)
{
	m_pBroadcastingClient = NULL;
	CIPCMedia::DoRequestStopMediaDecoding(mediaID, dwFlags);
}
//////////////////////////////////////////////////////////////////////
//********************************************************************
//////////////////////////////////////////////////////////////////////
CAudioList::CAudioList()
{
	m_pDecoder = NULL;
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif
}
//////////////////////////////////////////////////////////////////////
CAudioList::~CAudioList()
{
	SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CAudioList::Load(CWK_Profile& wkp)
{
	int i,c;
	CIPCAudioServer* pAudioServer;
	SafeDeleteAll();

	c = wkp.GetInt(SEC_NAME_MEDIAGROUPS, ENT_NAME_MAXGROUPS, 0);
	
	for (i=0;i<c;i++)
	{
		CString sEntry,s,sName,sShmName;
		WORD wID;
		int iSize;
		s.Format(_T("%d"),i+1);

		sEntry   = wkp.GetString(SEC_NAME_MEDIAGROUPS,s,_T(""));
		sName    = wkp.GetStringFromString(sEntry,INI_COMMENT,_T(""));
		sShmName = wkp.GetStringFromString(sEntry,INI_SMNAME,_T(""));
		wID      = (WORD)wkp.GetHexFromString(sEntry,INI_ID,-1);
		iSize    = wkp.GetNrFromString(sEntry,INI_NR,0);

		if (  !sName.IsEmpty()
			&& !sShmName.IsEmpty()
			&& wID != SECID_NO_GROUPID
			&& iSize>0)
		{
			pAudioServer = new CIPCAudioServer(sName,wID,sShmName);
			if (pAudioServer->Load(wkp,iSize))
			{
				WK_TRACE(_T("audio group loaded %s %d\n"),pAudioServer->GetShmName(),pAudioServer->GetSize());
				Add(pAudioServer);

				if (   m_pDecoder == NULL
					&& pAudioServer->GetDecoderID() != SECID_NO_ID)
				{
					m_pDecoder = pAudioServer;
				}
			}
			else
			{
				delete pAudioServer;
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("uncomplete media group\n"));
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CAudioList::StartThreads()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->StartThread();
		Sleep(10);
	}
}
//////////////////////////////////////////////////////////////////////
CIPCAudioServer* CAudioList::GetGroupByID(CSecID id) const
{
	for (int i=0;i<GetSize();i++) 
	{
		if (id.GetGroupID()==GetAtFast(i)->GetGroupID()) 
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
/* Diese Funktion wird nirgendwo mehr benutzt
CMedia* CAudioList::GetAudioByID(CSecID id)
{
	CMedia* pReturn = NULL;
	for (int i=0;i<GetSize();i++) 
	{
		if (id.GetGroupID() == GetAtFast(i)->GetGroupID()) 
		{
			pReturn = GetAtFast(i)->GetMediaAtWritable(id.GetSubID());
			break;
		}
	}
	return pReturn;
}
*/
//////////////////////////////////////////////////////////////////////
CString CAudioList::GetAudioNameByID(CSecID id)
{
	Lock(_T(__FUNCTION__));
	CString sReturn;
	CMedia* pMedia = NULL;
	for (int i=0 ; i<GetSize() ; i++) 
	{
		if (id.GetGroupID() == GetAtFast(i)->GetGroupID()) 
		{
			pMedia = GetAtFast(i)->GetMediaAtWritable(id.GetSubID());
			if (pMedia)
			{
				sReturn = pMedia->GetName();
			}
			break;
		}
	}
	Unlock();
	return sReturn;
}
