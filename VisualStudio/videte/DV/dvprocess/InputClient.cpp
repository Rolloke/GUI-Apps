// InputClient.cpp: implementation of the CInputClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "InputClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInputClient::CInputClient()
{
	m_dwID = GetTickCount();
	m_pCIPC = NULL;
	m_bSupervisor = FALSE;
}
//////////////////////////////////////////////////////////////////////
CInputClient::~CInputClient()
{
	WK_DELETE(m_pCIPC);
}
//////////////////////////////////////////////////////////////////////
BOOL CInputClient::Connect(const CConnectionRecord& c,BOOL bSupervisor)
{
	m_bSupervisor = bSupervisor;
	CString shmName;
	shmName.Format(_T("DVInput%08lx"),m_dwID);
	m_ConnectionRecord = c;
	c.GetFieldValue(CRF_SERIAL,m_sSerial);
	CString sMSN,sIPAdress,sBitrate;

	m_pCIPC = new CIPCInputDVClient(this,shmName);
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
void CInputClient::SetSupervisor(BOOL bSupervisor)
{
	m_bSupervisor = bSupervisor;
}
//////////////////////////////////////////////////////////////////////
BOOL CInputClient::IsConnected() const
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
BOOL CInputClient::IsDisconnected() const
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
BOOL CInputClient::IsTimedOut() const
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
BOOL CInputClient::Disconnect()
{
	WK_DELETE(m_pCIPC);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
CInputClients::CInputClients()
{
	m_bIdle = FALSE;
}
//////////////////////////////////////////////////////////////////////
CInputClients::~CInputClients()
{
	SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
CInputClient* CInputClients::AddInputClient()
{
	CInputClient* pInputClient = new CInputClient();
	SafeAdd(pInputClient);
	return pInputClient;
}
//////////////////////////////////////////////////////////////////////
CInputClient* CInputClients::GetInputClient(DWORD dwID)
{
	int i;
	BOOL bFound = FALSE;
	CInputClient* pInputClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pInputClient = GetAtFast(i);
		if (pInputClient->IsConnected() && pInputClient->GetID()==dwID)
		{
			bFound = TRUE;
			break;
		}
	}
	Unlock();

	return bFound ? pInputClient : NULL;
}
//////////////////////////////////////////////////////////////////////
int	CInputClients::GetNrOfLicensed()
{
	int r = 0;

	int i;
	CInputClient* pInputClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pInputClient = GetAtFast(i);
		if (!pInputClient->GetSerial().IsEmpty())
		{
			r++;
		}
	}
	Unlock();

	return r;
}
//////////////////////////////////////////////////////////////////////
BOOL CInputClients::IsConnected(const CString& sSerial,const CString& sSourceHost)
{
	int i;
	BOOL bFound = FALSE;
	CInputClient* pInputClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pInputClient = GetAtFast(i);
		if (   pInputClient->IsConnected()
			&& pInputClient->GetSerial()==sSerial
			&& sSourceHost != pInputClient->GetSourceHost())
		{
			bFound = TRUE;
			break;
		}
	}
	Unlock();

	return bFound;
}
//////////////////////////////////////////////////////////////////////
void CInputClients::OnIdle()
{
	if (m_bIdle)
	{
		int i;
		CInputClient* pInputClient;

		DWORD dwTick1 = GetTickCount();
		Lock();
		DWORD dwTick2 = GetTimeSpan(dwTick1);
		if (dwTick2>50)
		{
			WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
		}

		for (i=0;i<GetSize();i++)
		{
			pInputClient = GetAtFast(i);
			if (   pInputClient->IsDisconnected()
				|| pInputClient->IsTimedOut())
			{
				// remove from tree and array
				theApp.GetCallThread().InputDisconnected(pInputClient);
				WK_DELETE(pInputClient);
				RemoveAt(i);
			}
		}
		Unlock();
	}

	m_bIdle = FALSE;
}
//////////////////////////////////////////////////////////////////////
void CInputClients::ClientDisconnected(DWORD dwID)
{
	m_bIdle = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CInputClients::UpdateAllClients(DWORD dwID /*= (DWORD)-1*/)
{
	int i;
	CInputClient* pInputClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pInputClient = GetAtFast(i);
		if (   pInputClient->IsConnected()
			&& (dwID != pInputClient->GetID())
			)
		{
			pInputClient->GetCIPC()->OnRequestInfoInputs(SECID_NO_GROUPID);
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CInputClients::DoIndicateAlarmNr(CSecID id ,BOOL bActive,DWORD dwData1,DWORD dwData2)
{
	Lock();

	for (int i=0;i<GetSize();i++) 
	{
		CInputClient* pClient = GetAtFast(i);
		if (pClient->IsConnected())
		{
			pClient->GetCIPC()->DoIndicateAlarmNr(
				id,bActive,dwData1,dwData2);
		}
	}	// end of client loop

	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CInputClients::DoConfirmSetValue(CSecID id, const CString& sKey, const CString& sValue, DWORD dwParam)
{
	Lock();

	for (int i=0;i<GetSize();i++) 
	{
		CInputClient* pClient = GetAtFast(i);
		if (pClient->IsConnected())
		{
			pClient->GetCIPC()->DoConfirmSetValue(id,sKey,sValue,dwParam);
		}
	}	// end of client loop

	Unlock();
}
