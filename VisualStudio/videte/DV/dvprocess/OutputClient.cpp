// OutputClient.cpp: implementation of the COutputClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "OutputClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static DWORD dwClientID = 1;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COutputClient::COutputClient()
{
	m_dwID = dwClientID++;
	if (m_dwID==0)
	{
		m_dwID = dwClientID++;
	}
	m_pCIPC = NULL;
	m_bISDN = FALSE;
	m_bTCPIP = FALSE;
	m_bSupervisor = FALSE;
	m_bAlarm = FALSE;

	m_bH263 = FALSE;
	m_bMpeg4 = FALSE;
	m_bIsDTS = FALSE;
	m_dwOptions = 0;

	// assume 1Mbit even for local
	m_iBitrate = 1024*1024;
}
//////////////////////////////////////////////////////////////////////
COutputClient::~COutputClient()
{
	WK_DELETE(m_pCIPC);
}
//////////////////////////////////////////////////////////////////////
BOOL COutputClient::Connect(const CConnectionRecord& c,BOOL bSupervisor,BOOL bAlarm/*=FALSE*/)
{
	m_bSupervisor = bSupervisor;
	
	m_dwOptions = c.GetOptions();
	// we can do the following options
	m_dwOptions &= SCO_WANT_RELAYS|SCO_WANT_CAMERAS_COLOR|SCO_JPEG_AS_H263|SCO_NO_STREAM|SCO_MULTI_CAMERA;
	// we are DTS
	m_dwOptions |= SCO_IS_DV;

	m_bAlarm = bAlarm;
	CString shmName;
	shmName.Format(_T("DVOutput%08lx"),m_dwID);
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
	m_bH263 = FALSE;
	m_bMpeg4 = FALSE;

	c.GetFieldValue(CRF_VERSION,m_sVersion);
	m_bIsDTS = (c.GetOptions() & SCO_IS_DV)>0;
	m_bCanGOP = (c.GetOptions() & SCO_CAN_GOP)>0;

	InitH263Mpeg4();

	m_pCIPC = new CIPCOutputDVClient(this,shmName);
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
/////////////////////////////////////////////////////////////////
void COutputClient::SetFetchResult(CIPCFetchResult& fr)
{
	// alarm case
	DWORD dwRemoteBitrate = 0;
	if (1==_stscanf(fr.GetShmName(),_T("%d"),&dwRemoteBitrate))
	{
		m_iBitrate = dwRemoteBitrate;
	}
	m_ConnectionRecord.SetOptions(fr.GetOptions());
	m_bIsDTS = (m_ConnectionRecord.GetOptions() & SCO_IS_DV)>0;
	m_bCanGOP = (m_ConnectionRecord.GetOptions() & SCO_CAN_GOP)>0;
	if (m_pCIPC)
	{
		m_pCIPC->SetCodePage(fr.GetCodePage());
	}
	
	m_sVersion = fr.GetErrorMessage();
	InitH263Mpeg4();
}
/////////////////////////////////////////////////////////////////
void COutputClient::InitH263Mpeg4()
{
	BOOL bMpeg4 = theApp.GetSettings().GetMPEG4();

	if (m_bIsDTS)
	{
		if (m_sVersion >= _T("3"))
		{
			m_bH263 = IsLowBandwidth() && !bMpeg4;
			m_bMpeg4 = bMpeg4;
		}
		else
		{
			m_bH263 = IsLowBandwidth();
			m_bMpeg4 = FALSE;
		}
	}
	else
	{
		if (m_sVersion >= _T("5"))
		{
			m_bH263 = IsLowBandwidth() && !bMpeg4;
			m_bMpeg4 = bMpeg4;
		}
		else
		{
			m_bH263 = IsLowBandwidth();
			m_bMpeg4 = FALSE;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void COutputClient::SetSupervisor(BOOL bSupervisor)
{
	m_bSupervisor = bSupervisor;
}
//////////////////////////////////////////////////////////////////////
BOOL COutputClient::IsConnected() const
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
BOOL COutputClient::IsTimedOut() const
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
BOOL COutputClient::IsDisconnected() const
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
BOOL COutputClient::Disconnect()
{
	WK_DELETE(m_pCIPC);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
COutputClients::COutputClients()
{
	m_bIdle = FALSE;
}
//////////////////////////////////////////////////////////////////////
COutputClients::~COutputClients()
{
	SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
COutputClient* COutputClients::AddOutputClient()
{
	COutputClient* pOutputClient = new COutputClient();
	SafeAdd(pOutputClient);
	return pOutputClient;
}
//////////////////////////////////////////////////////////////////////
COutputClient* COutputClients::GetOutputClient(DWORD dwID)
{
	int i;
	BOOL bFound = FALSE;
	COutputClient* pOutputClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pOutputClient = GetAtFast(i);
		if (pOutputClient->IsConnected() && pOutputClient->GetID()==dwID)
		{
			bFound = TRUE;
			break;
		}
	}
	Unlock();

	return bFound ? pOutputClient : NULL;
}
//////////////////////////////////////////////////////////////////////
int	COutputClients::GetNrOfLicensed()
{
	int r = 0;

	int i;
	COutputClient* pOutputClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pOutputClient = GetAtFast(i);
		if (!pOutputClient->GetSerial().IsEmpty())
		{
			r++;
		}
	}
	Unlock();

	return r;
}
//////////////////////////////////////////////////////////////////////
BOOL COutputClients::IsConnected(const CString& sSerial,const CString& sSourceHost)
{
	int i;
	BOOL bFound = FALSE;
	COutputClient* pOutputClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pOutputClient = GetAtFast(i);
		if (   pOutputClient->IsConnected()
			&& pOutputClient->GetSerial()==sSerial
			&& sSourceHost != pOutputClient->GetSourceHost())
		{
			bFound = TRUE;
			break;
		}
	}
	Unlock();

	return bFound;
}
//////////////////////////////////////////////////////////////////////
void COutputClients::ClientDisconnected(DWORD dwID)
{
	m_bIdle = TRUE;
}
//////////////////////////////////////////////////////////////////////
void COutputClients::OnIdle()
{
	if (m_bIdle)
	{
		int i;
		COutputClient* pOutputClient;

		DWORD dwTick1 = GetTickCount();
		Lock();
		DWORD dwTick2 = GetTimeSpan(dwTick1);
		if (dwTick2>50)
		{
			WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
		}

		int c = GetSize();

		for (i=0;i<GetSize();i++)
		{
			pOutputClient = GetAtFast(i);
			if (   pOutputClient->IsDisconnected()
				|| pOutputClient->IsTimedOut())
			{
				// remove from tree and array
				theApp.OnOutputClientDisconnected(pOutputClient);
				WK_DELETE(pOutputClient);
				RemoveAt(i);
			}
		}
		Unlock();

		if (c>0 && GetSize() == 0)
		{
			// last client disconnected
			theApp.GetOutputGroups().ClearActiveClientCam();
		}
	}
	m_bIdle = FALSE;
}
//////////////////////////////////////////////////////////////////////
void COutputClients::UpdateAllClients(DWORD dwID/* = (DWORD)-1*/)
{
//	WK_TRACE(_T("update all output clients %08lx\n"),dwID);
	COutputClient* pOutputClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (int i=0;i<GetSize();i++)
	{
		pOutputClient = GetAtFast(i);
		if (   pOutputClient->IsConnected()
			&& (dwID != pOutputClient->GetID())
			)
		{
			pOutputClient->GetCIPC()->ResetClientCounter();
			pOutputClient->GetCIPC()->OnRequestInfoOutputs(SECID_NO_GROUPID);
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void COutputClients::DoConfirmGetValue(CSecID id, const CString& sKey, const CString& sValue)
{
	COutputClient* pOutputClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>50)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (int i=0;i<GetSize();i++)
	{
		pOutputClient = GetAtFast(i);
		if (pOutputClient->IsConnected())
		{
			pOutputClient->GetCIPC()->DoConfirmGetValue(id,sKey,sValue,0);
		}
	}
	Unlock();
}
