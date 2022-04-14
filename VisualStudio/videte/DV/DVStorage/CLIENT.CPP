// Client.cpp: implementation of the CClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVStorage.h"
#include "Client.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CClient::CClient()
{
	m_dwID = GetTickCount();
	m_pCIPCDatabaseStorage = NULL;
	m_bISDN = FALSE;
	m_bTCPIP = FALSE;
	m_bH263 = FALSE;
	m_bMpeg4 = FALSE;

	// assume 1Mbit even for local
	m_iBitrate = 1024*1024;
}
//////////////////////////////////////////////////////////////////////
CClient::~CClient()
{
	WK_DELETE(m_pCIPCDatabaseStorage);
}
//////////////////////////////////////////////////////////////////////
BOOL CClient::Connect(const CConnectionRecord& c)
{
	CString shmName;
	shmName.Format(_T("DVStorage%08lx"),m_dwID);
	m_ConnectionRecord = c;
	c.GetFieldValue(CRF_SERIAL,m_sSerial);
	c.GetFieldValue(CRF_VERSION,m_sVersion);

	CString sMSN,sIPAdress,sBitrate;

	if (c.GetFieldValue(CRF_MSN,sMSN))
	{
		m_bISDN = TRUE;
		// assume max 2 B Channels
		m_iBitrate = 2*64*1024;
		WK_TRACE(_T("incoming ISDN Client %s\n"),sMSN);
	}
	if (c.GetFieldValue(CRF_IPADDRESS,sIPAdress))
	{
		m_bTCPIP = TRUE;
		WK_TRACE(_T("incoming TCP/IP Client %s\n"),sIPAdress);
	}
	if (c.GetFieldValue(CRF_BITRATE,sBitrate))
	{
		_stscanf(sBitrate,_T("%d"),&m_iBitrate);
		WK_TRACE(_T("incoming bitrate is %d\n"),m_iBitrate);
	}

	UTRACE(_T("CClient::Connect:%s:%s:%s\n"), sMSN, sIPAdress, sBitrate);

	BOOL bIsDTS = (c.GetOptions() & SCO_IS_DV)>0;

	if (bIsDTS)
	{
		if (m_sVersion >= _T("3"))
		{
			m_bMpeg4 = IsLowBandwidth();
		}
		else
		{
			m_bH263 = IsLowBandwidth();
		}
	}
	else
	{
		if (m_sVersion >= _T("5"))
		{
			m_bMpeg4 = IsLowBandwidth();
		}
		else
		{
			m_bH263 = IsLowBandwidth();
		}
	}

	m_pCIPCDatabaseStorage = new CIPCDatabaseStorage(this,shmName);

#ifdef _UNICODE
	CString sCP;
	if (c.GetFieldValue(CRF_CODEPAGE, sCP))
	{
		m_pCIPCDatabaseStorage->SetCodePage(sCP);
	}
	else
	{
		m_pCIPCDatabaseStorage->SetCodePage(0);
	}
#endif

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CClient::IsConnected() const
{
	if (m_pCIPCDatabaseStorage)
	{
		if (m_pCIPCDatabaseStorage->GetIPCState()==CIPC_STATE_CONNECTED)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CClient::IsDisconnected() const
{
	if (m_pCIPCDatabaseStorage)
	{
		if (m_pCIPCDatabaseStorage->GetIsMarkedForDelete())
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
CClients::CClients()
{

}
//////////////////////////////////////////////////////////////////////
CClients::~CClients()
{
	SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
CClient* CClients::AddClient()
{
	CClient* pClient = new CClient();
	SafeAdd(pClient);
	return pClient;
}
//////////////////////////////////////////////////////////////////////
CClient* CClients::GetClient(DWORD dwID)
{
	int i;
	BOOL bFound = FALSE;
	CClient* pClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>250)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (   pClient->IsConnected() 
			&& pClient->GetID()==dwID)
		{
			bFound = TRUE;
			break;
		}
	}
	Unlock();

	return bFound ? pClient : NULL;
}
//////////////////////////////////////////////////////////////////////
int	CClients::GetNrOfLicensed()
{
	int r = 0;

	int i;
	CClient* pClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>250)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (!pClient->GetSerial().IsEmpty())
		{
			r++;
		}
	}
	Unlock();

	return r;
}
//////////////////////////////////////////////////////////////////////
BOOL CClients::IsConnected(const CString& sSerial, const CString& sSourceHost)
{
	int i;
	BOOL bFound = FALSE;
	CClient* pClient = NULL;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>250)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (   pClient->GetSerial()==sSerial
			&& sSourceHost != pClient->GetSourceHost())
		{
			bFound = TRUE;
			break;
		}
	}
	Unlock();

	return bFound;
}
//////////////////////////////////////////////////////////////////////
void CClients::OnIdle()
{
	int i;
	CClient* pClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>250)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	int c = GetSize();
	for (i=c-1;i>=0;i--)
	{
		pClient = GetAtFast(i);
		if (pClient->IsDisconnected())
		{
			// remove from tree and array
			WK_TRACE(_T("Disconnected Client %08lx\n"),pClient->GetID());
			WK_DELETE(pClient);
			RemoveAt(i);
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClients::DoIndicateDeleteTape(const CTape& tape)
{
	CClient* pClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>250)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (int i=0;i<GetSize();i++)
	{
		pClient = GetAt(i);
		if (pClient->IsConnected())
		{
			CIPCDatabaseStorage* pCIPCDatabase = pClient->GetCIPC();
			TRACE(_T("indicate delete sequence %04hx %d\n"),tape.GetArchiveNr(),
									tape.GetNr());
			if (tape.IsAlarmList())
			{
				if (pCIPCDatabase->GetAlarmListRequests())
				{
					pCIPCDatabase->DoIndicateDeleteSequence(tape.GetArchiveNr(),
															tape.GetNr());
				}
			}
			else
			{
				if (pCIPCDatabase->GetArchivesRequests())
				{
					pCIPCDatabase->DoIndicateDeleteSequence(tape.GetArchiveNr(),
															tape.GetNr());
				}
			}
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClients::DoIndicateNewTape(const CTape& tape, WORD wPrevSequenceNr,DWORD dwNrOfRecords)
{
	CClient* pClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>250)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (int i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (pClient->IsConnected())
		{
			CIPCDatabaseStorage* pCIPCDatabase = pClient->GetCIPC();
			if (tape.IsAlarmList())
			{
				if (pCIPCDatabase->GetAlarmListRequests())
				{
					CIPCSequenceRecord data;
					pCIPCDatabase->TapeToSequenceRecord(tape,data);
					pCIPCDatabase->DoIndicateNewSequence(data,wPrevSequenceNr,dwNrOfRecords);
				}
			}
			else
			{
				if (pCIPCDatabase->GetArchivesRequests())
				{
					CIPCSequenceRecord data;
					pCIPCDatabase->TapeToSequenceRecord(tape,data);
					pCIPCDatabase->DoIndicateNewSequence(data,wPrevSequenceNr,dwNrOfRecords);
				}
			}
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClients::DoIndicateNewCollection(CCollection& coll)
{
	CClient* pClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>250)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (int i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (pClient->IsConnected())
		{
			CIPCDatabaseStorage* pCIPCDatabase = pClient->GetCIPC();
			if (coll.IsAlarmList())
			{
				if (pCIPCDatabase->GetAlarmListRequests())
				{
					CIPCArchivRecord rec;
					pCIPCDatabase->CollectionToArchiveRecord(coll,rec);
					pCIPCDatabase->DoIndicateNewArchiv(rec);
				}
			}
			else
			{
				if (pCIPCDatabase->GetArchivesRequests())
				{
					CIPCArchivRecord rec;
					pCIPCDatabase->CollectionToArchiveRecord(coll,rec);
					pCIPCDatabase->DoIndicateNewArchiv(rec);
				}
			}
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClients::DoIndicateDeleteCollection(WORD wCollectionNr)
{
	CClient* pClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>250)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (int i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (pClient->IsConnected())
		{
			CIPCDatabaseStorage* pCIPCDatabase = pClient->GetCIPC();
			pCIPCDatabase->DoIndicateRemoveArchiv(wCollectionNr);
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClients::DoIndicateNewCollectionName(DWORD dwClientID, CCollection& coll)
{
	CClient* pClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>250)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (int i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (   pClient->IsConnected()
			&& (pClient->GetID()!=dwClientID))
		{
			CIPCDatabaseStorage* pCIPCDatabase = pClient->GetCIPC();
			CSecID id(SECID_GROUP_ARCHIVE,coll.GetNr());
			if (coll.IsAlarmList())
			{
				if (pCIPCDatabase->GetAlarmListRequests())
				{
					pCIPCDatabase->DoConfirmGetValue(id,CSD_NAME,coll.GetName(),0);
				}
			}
			else
			{
				if (pCIPCDatabase->GetArchivesRequests())
				{
					pCIPCDatabase->DoConfirmGetValue(id,CSD_NAME,coll.GetName(),0);
				}
			}
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClients::DoIndicateDrives()
{
	CClient* pClient;

	DWORD dwTick1 = GetTickCount();
	Lock();
	DWORD dwTick2 = GetTimeSpan(dwTick1);
	if (dwTick2>250)
	{
		WK_TRACE(_T("%s(%d) : lock in %d ms\n"),_T(__FILE__),__LINE__,dwTick2);
	}

	for (int i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (pClient->IsConnected())
		{
			CIPCDatabaseStorage* pCIPCDatabase = pClient->GetCIPC();
			if (pCIPCDatabase->GetDriveRequests())
			{
				pCIPCDatabase->OnRequestDrives();
			}
		}
	}
	Unlock();
}
