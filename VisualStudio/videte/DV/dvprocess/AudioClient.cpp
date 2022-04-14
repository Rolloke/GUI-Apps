// AudioClient.cpp: implementation of the CAudioClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "AudioClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAudioClient::CAudioClient()
{
	m_dwID = GetTickCount();
	m_pCIPC = NULL;
	m_bISDN = FALSE;
	m_bTCPIP = FALSE;
	m_bSupervisor = FALSE;
	m_bAlarm = FALSE;

	// assume 1Mbit even for local
	m_iBitrate = 1024*1024;
}
//////////////////////////////////////////////////////////////////////
CAudioClient::~CAudioClient()
{
	WK_DELETE(m_pCIPC);
}
//////////////////////////////////////////////////////////////////////
BOOL CAudioClient::Connect(const CConnectionRecord& c,BOOL bSupervisor,BOOL bAlarm/*=FALSE*/)
{
	m_bSupervisor = bSupervisor;
	m_bAlarm = bAlarm;
	CString shmName;
	shmName.Format(_T("DVAudio%08lx"),m_dwID);
	m_ConnectionRecord = c;
	c.GetFieldValue(CRF_SERIAL,m_sSerial);

	CString sMSN,sIPAdress,sBitrate,sTyp;
	sTyp = _T("incoming");
	if (bAlarm)
	{
		sTyp = _T("alarm");
	}

	if (c.GetFieldValue(CRF_MSN,sMSN))
	{
		m_bISDN = TRUE;
		// assume max 2 B Channels
		m_iBitrate = 2*64*1024;
		WK_TRACE(_T("%s ISDN %s, %s\n"),sTyp,sMSN,c.GetSourceHost());
		m_ConnectionRecord.SetSourceHost(sMSN);
	}
	if (c.GetFieldValue(CRF_IPADDRESS,sIPAdress))
	{
		m_bTCPIP = TRUE;	 
		WK_TRACE(_T("%s TCP/IP %s, %s\n"),sTyp,sIPAdress,c.GetSourceHost());
		m_ConnectionRecord.SetSourceHost(sIPAdress);
	}
	if (c.GetFieldValue(CRF_BITRATE,sBitrate))
	{
		_stscanf(sBitrate,_T("%d"),&m_iBitrate);
		WK_TRACE(_T("bitrate is %d\n"),m_iBitrate);
	}

	m_pCIPC = new CIPCAudioDVClient(this,shmName);

#ifdef _UNICODE
	CString sCP;
	if (c.GetFieldValue(CRF_CODEPAGE, sCP))
	{
		m_pCIPC->SetCodePage(sCP);
	}
	else
	{
		m_pCIPC->SetCodePage(0);
	}
#endif
	
	return m_pCIPC != NULL;
}
//////////////////////////////////////////////////////////////////////
void CAudioClient::SetSupervisor(BOOL bSupervisor)
{
	m_bSupervisor = bSupervisor;
}
//////////////////////////////////////////////////////////////////////
BOOL CAudioClient::IsConnected() const
{
	if (m_pCIPC)
	{
		if (m_pCIPC->GetIPCState()==CIPC_STATE_CONNECTED)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CAudioClient::IsTimedOut() const
{
	if (m_pCIPC)
	{
		if (m_pCIPC->IsTimedOut())
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CAudioClient::IsDisconnected() const
{
	if (m_pCIPC)
	{
		if (m_pCIPC->GetIsMarkedForDelete())
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CAudioClient::Disconnect()
{
	WK_DELETE(m_pCIPC);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
CAudioClients::CAudioClients()
{
	m_bIdle = FALSE;
}
//////////////////////////////////////////////////////////////////////
CAudioClients::~CAudioClients()
{
	SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
CAudioClient* CAudioClients::AddAudioClient()
{
	CAudioClient* pAudioClient = new CAudioClient();
	SafeAdd(pAudioClient);
	return pAudioClient;
}
//////////////////////////////////////////////////////////////////////
CAudioClient* CAudioClients::GetAudioClient(DWORD dwID)
{
	int i;
	BOOL bFound = FALSE;
	CAudioClient* pAudioClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pAudioClient = GetAtFast(i);
		if (pAudioClient && pAudioClient->IsConnected() && pAudioClient->GetID()==dwID)
		{
			bFound = TRUE;
			break;
		}
	}
	Unlock();

	return bFound ? pAudioClient : NULL;
}
//////////////////////////////////////////////////////////////////////
int	CAudioClients::GetNrOfLicensed()
{
	int r = 0;

	int i;
	CAudioClient* pAudioClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pAudioClient = GetAtFast(i);
		if (   pAudioClient 
			&& !pAudioClient->GetSerial().IsEmpty()
			)
		{
			r++;
		}
	}
	Unlock();

	return r;
}
//////////////////////////////////////////////////////////////////////
BOOL CAudioClients::IsConnected(const CString& sSerial,const CString& sSourceHost)
{
	int i;
	BOOL bFound = FALSE;
	CAudioClient* pAudioClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pAudioClient = GetAtFast(i);
		if (   pAudioClient
			&& pAudioClient->IsConnected()
			&& pAudioClient->GetSerial()==sSerial
			&& sSourceHost != pAudioClient->GetSourceHost())
		{
			bFound = TRUE;
			break;
		}
	}
	Unlock();

	return bFound;
}
//////////////////////////////////////////////////////////////////////
void CAudioClients::ClientDisconnected(DWORD dwID)
{
	m_bIdle = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CAudioClients::OnIdle()
{
	if (m_bIdle)
	{
		int i;
		CAudioClient* pAudioClient;

		DWORD dwTick1 = GetTickCount();
		Lock();
		DWORD dwTick2 = GetTimeSpan(dwTick1);
		if (dwTick2>50)
		{
			WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
		}

		for (i=0;i<GetSize();i++)
		{
			pAudioClient = GetAtFast(i);
			if (pAudioClient)
			{
				if (   pAudioClient->IsDisconnected()
					|| pAudioClient->IsTimedOut())
				{
					// remove from tree and array
					theApp.OnAudioClientDisconnected(pAudioClient);
					WK_DELETE(pAudioClient);
					RemoveAt(i);
				}
			}
		}
		Unlock();
	}
	m_bIdle = FALSE;
}
//////////////////////////////////////////////////////////////////////
void CAudioClients::UpdateAllClients(DWORD dwID/* = (DWORD)-1*/)
{
//	WK_TRACE(_T("update all audio clients %08lx\n"),dwID);
	CAudioClient* pAudioClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (int i=0;i<GetSize();i++)
	{
		pAudioClient = GetAtFast(i);
		if (   pAudioClient 
			&& pAudioClient->IsConnected()
			&& (dwID != pAudioClient->GetID())
			)
		{
			pAudioClient->GetCIPC()->OnRequestInfoMedia(SECID_NO_GROUPID);
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CAudioClients::DoConfirmGetValue(CSecID id, const CString& sKey, const CString& sValue)
{
	CAudioClient* pAudioClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (int i=0;i<GetSize();i++)
	{
		pAudioClient = GetAtFast(i);
		if (   pAudioClient 
			&& pAudioClient->IsConnected()
			)
		{
			pAudioClient->GetCIPC()->DoConfirmGetValue(id,sKey,sValue,0);
		}
	}
	Unlock();
}
