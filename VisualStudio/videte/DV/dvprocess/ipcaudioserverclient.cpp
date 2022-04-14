// IPCAudioServerClient.cpp: implementation of the CIPCAudioDVClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "IPCAudioServerClient.h"
#include "MediaSampleRecord.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BOOL CIPCAudioDVClient::gm_bTraceDoClientEncoding = FALSE;
BOOL CIPCAudioDVClient::gm_bTraceOnRequestStartMediaEncoding = FALSE;
BOOL CIPCAudioDVClient::gm_bTraceOnRequestStopMediaEncoding = FALSE;
BOOL CIPCAudioDVClient::gm_bTraceRequestMediaDecoding = FALSE;
BOOL CIPCAudioDVClient::gm_bTraceOnRequestStopMediaDecoding = FALSE;
BOOL CIPCAudioDVClient::gm_bTraceOnRequestValues = FALSE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCAudioDVClient::CIPCAudioDVClient(CAudioClient* pClient, LPCTSTR shmName)
	: CIPCMedia(shmName, TRUE)
{
	m_pClient = pClient;
	m_dwOptions = 0;
	m_dwTime = GetTickCount();
	m_bAudioInActive = FALSE;
	StartThread();
	WK_TRACE(_T("CIPCAudioDVClient(%s)\n"), shmName);
}
//////////////////////////////////////////////////////////////////////
CIPCAudioDVClient::~CIPCAudioDVClient()
{
	WK_TRACE(_T("~CIPCAudioDVClient(%s)\n"), GetShmName());
	CUnhandledException::TraceCallStack(NULL);
	StopThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCAudioDVClient::IsTimedOut()
{
	if (GetIPCState() == CIPC_STATE_WRITE_CREATED)
	{
		if (GetTimeSpan(m_dwTime)>30*1000)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnRequestInfoMedia(WORD wGroupID)
{
	int iNumGroups = theApp.GetAudioGroups().GetSize();
	CIPCAudioServer* pAudioGroup;
	CIPCMediaRecordArray records;
	records.SetAutoDelete(true);
	CIPCMediaRecord::MediaRecordFlags Flags;

	for (int g=0;g<iNumGroups;g++) 
	{
		pAudioGroup = theApp.GetAudioGroups().GetAtFast(g);
		if ((wGroupID==SECID_NO_GROUPID || wGroupID == pAudioGroup->GetID()))
		{ 
			for (int i=0;i<pAudioGroup->GetSize();i++)
			{ 
				Flags.Reset();
				const CMedia* pMedia = pAudioGroup->GetAtFast(i);

				if (   pMedia->GetType() == SEC_MEDIA_TYPE_INPUT
					&& !theApp.GetSettings().GetAudioIn())
				{
					// input / microfon forbidden for the client
					continue;
				}

				CIPCMediaRecord* pMediaRecord = new CIPCMediaRecord;
				if (pAudioGroup->GetHardwareState()==0)
				{
					Flags.IsOkay    = true;
					Flags.IsEnabled = true;
					Flags.IsAudio   = true;
				}
				if (pMedia->GetType() == SEC_MEDIA_TYPE_INPUT)
				{
					Flags.IsInput = true;
				}
				else if (pMedia->GetType() == SEC_MEDIA_TYPE_OUTPUT)
				{
					Flags.IsOutput = true;
				}
				Flags.IsDefault = TRUE;

				pMediaRecord->Set(pMedia->GetName(),pMedia->GetID(), Flags);
				records.Add(pMediaRecord);
			}
		}
	}
	DoConfirmInfoMedia(wGroupID,iNumGroups,records.GetSize(),records);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnRequestStartMediaEncoding(CSecID mediaID, DWORD dwFlags, DWORD dwUserData)
{
	if (gm_bTraceOnRequestStartMediaEncoding)
	{
		WK_TRACE(_T("CIPCAudioDVClient::OnRequestStartMediaEncoding\n"));
	}

	// NOT YET AUDIO processes
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(mediaID);
	if (pCIPCAudioServer && pCIPCAudioServer->GetEncoderID() != SECID_NO_ID)
	{
		if (!(dwFlags & MEDIA_ENC_ONLY_INITIALIZE))
		{
			m_bAudioInActive = TRUE;
		}

		pCIPCAudioServer->DoRequestStartMediaEncoding(mediaID,dwFlags,0);
	}
	else
	{
		WK_TRACE_ERROR(_T("no audio group for id %08lx\n"),mediaID.GetID());
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCAudioDVClient::IsAudioInActive()
{
	return m_bAudioInActive;
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnRequestStopMediaEncoding(CSecID mediaID, DWORD dwFlags)
{
	if (gm_bTraceOnRequestStopMediaEncoding)
	{
		WK_TRACE(_T("CIPCAudioDVClient::OnRequestStopMediaEncoding\n"));
	}

	// AUDIO processes
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(mediaID);
	if (pCIPCAudioServer)
	{
		m_bAudioInActive = FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnRequestMediaDecoding(const CIPCMediaSampleRecord& data, CSecID mediaID, DWORD dwFlags)
{
	if (gm_bTraceRequestMediaDecoding)
	{
		WK_TRACE(_T("CIPCAudioDVClient::OnRequestMediaDecoding, No: %d, Size: %d\n"), data.GetPackageNumber(), data.GetActualLength());
	}

	// AUDIO decoder raussuchen!
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(mediaID);
		
	if (pCIPCAudioServer)
	{
		pCIPCAudioServer->DoRequestMediaDecoding(this, data, mediaID, dwFlags);
	}
	else
	{
		pCIPCAudioServer = theApp.GetAudioGroups().GetDecoder();
		if (pCIPCAudioServer)
		{
			pCIPCAudioServer->DoRequestMediaDecoding(this, data, pCIPCAudioServer->GetDecoderID(), dwFlags);
		}
	}
	if (pCIPCAudioServer==NULL)
	{
		TRACE(_T("no decoder found for media decoding\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnRequestStopMediaDecoding(CSecID mediaID, DWORD dwFlags)
{
	if (gm_bTraceOnRequestStopMediaDecoding)
	{
		WK_TRACE(_T("CIPCAudioDVClient::OnRequestStopMediaDecoding()"));
	}
	
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(mediaID);
	if (pCIPCAudioServer)
	{
		pCIPCAudioServer->DoRequestStopMediaDecoding(mediaID, dwFlags);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnRequestHardware(CSecID secID, DWORD dwIDRequest)
{
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(secID);
	if (pCIPCAudioServer)
	{
		secID.SetSubID(CSecID(m_pClient->GetID()).GetSubID());
		pCIPCAudioServer->DoRequestHardware(secID, dwIDRequest);
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnRequestValues(CSecID secID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	if (gm_bTraceOnRequestValues)
	{
		WK_TRACE(_T("CIPCAudioDVClient::OnRequestValues(%x)=> SecID:%08x, ID:%d, F:%x\n"), dwCmd, secID.GetID(), dwID, dwValue);
	}
	CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(secID);
	if (   pCIPCAudioServer == NULL
		&& secID == SECID_NO_ID
		&& theApp.GetAudioGroups().GetSize())
	{
		pCIPCAudioServer = theApp.GetAudioGroups().GetAtFast(0);
		secID = pCIPCAudioServer->GetEncoderID();
	}
	if (pCIPCAudioServer)
	{
//		secID.SetSubID(CSecID(m_pClient->GetID()).GetSubID());
		pCIPCAudioServer->DoRequestValues(secID, dwCmd, dwID, dwValue, pData);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnRequestDisconnect()
{
	DelayedDelete();
	theApp.GetAudioClients().ClientDisconnected(m_pClient->GetID());
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnRequestGetValue(CSecID id,
										   const CString &sKey,
										   DWORD dwServerData)
{
	theApp.OnRequestGetValue(id,sKey,dwServerData,this);
}
//////////////////////////////////////////////////////////////////////
void CIPCAudioDVClient::OnRequestSetValue(CSecID id,
										   const CString &sKey,
										   const CString &sValue,
										   DWORD dwServerData)
{
	theApp.OnRequestSetValue(id,sKey,sValue,dwServerData,this);
}
