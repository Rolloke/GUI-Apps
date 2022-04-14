/* GlobalReplace: line %s --> line %d */
// Clients.cpp: implementation of the CClients class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBS.h"
#include "Clients.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CClients::CClients()
{
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.TraceLocks() ? 0 : -1;
#endif
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

	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), 50);

	for (i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (pClient && pClient->IsConnected() && pClient->GetID()==dwID)
		{
			bFound = TRUE;
			break;
		}
	}
	acs.Unlock();

	return bFound ? pClient : NULL;
}
//////////////////////////////////////////////////////////////////////
void CClients::OnIdle()
{
	int i;
	CClient* pClient;
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), 50);

	for (i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (pClient)
		{
			if (pClient->IsDisconnected())
			{
				// remove from tree and array
				WK_TRACE(_T("Disconnected Client %08lx\n"),pClient->GetID());
				WK_DELETE(pClient);
				RemoveAt(i);
			}
		}
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClients::DoIndicateDeleteSequence(const CSequence& seq)
{
	CClient* pClient;
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), 50);

	for (int i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (pClient)
		{
			if (pClient->IsConnected())
			{
				CIPCDatabaseServer* pCIPCDatabase = pClient->GetCIPCDatabaseServer();

				if (seq.IsAlarmList())
				{
					if (pCIPCDatabase->GetAlarmListRequests())
					{
						pCIPCDatabase->DoIndicateDeleteSequence(seq.GetArchivNr(),seq.GetNr());
					}
				}
				else
				{
					if (pCIPCDatabase->GetArchivesRequests())
					{
						pCIPCDatabase->DoIndicateDeleteSequence(seq.GetArchivNr(),seq.GetNr());
					}
				}
			}
		}
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClients::DoIndicateNewSequence(const CSequence& seq,
									 WORD  wPrevSequenceNr,
									 DWORD dwNrOfRecords)
{
	CClient* pClient = NULL;
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), 50);

	for (int i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (pClient && pClient->IsConnected())
		{
			CIPCDatabaseServer* pCIPCDatabase = pClient->GetCIPCDatabaseServer();
			if (   (seq.IsAlarmList() && pCIPCDatabase->GetAlarmListRequests())
				|| (!seq.IsAlarmList()&& pCIPCDatabase->GetArchivesRequests())
				)
			{
				CIPCSequenceRecord data;
				pCIPCDatabase->SequenceToSequenceRecord(seq,data);
				pCIPCDatabase->DoIndicateNewSequence(data,wPrevSequenceNr,dwNrOfRecords);
			}
		}
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClients::DoIndicateNewArchiv(CArchiv& arc)
{
	CClient* pClient;
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), 50);

	for (int i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (pClient && pClient->IsConnected())
		{
			CIPCDatabaseServer* pCIPCDatabase = pClient->GetCIPCDatabaseServer();
			if (   (arc.IsAlarmList() && pCIPCDatabase->GetAlarmListRequests())
				|| (!arc.IsAlarmList()&& pCIPCDatabase->GetArchivesRequests()))			
			{
				CIPCArchivRecord rec;
				pCIPCDatabase->ArchiveToArchiveRecord(arc,rec);
				if (pCIPCDatabase->GetArchivesRequests())
				{
					pCIPCDatabase->DoIndicateNewArchiv(rec);
				}
			}
		}
	}
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClients::DoIndicateDeleteArchiv(WORD wArchivNr)
{
	CClient* pClient;
	CAutoCritSec acs(&m_cs, _T(__FUNCTION__), 50);

	for (int i=0;i<GetSize();i++)
	{
		pClient = GetAtFast(i);
		if (pClient && pClient->IsConnected())
		{
			CIPCDatabaseServer* pCIPCDatabase = pClient->GetCIPCDatabaseServer();

			if (pCIPCDatabase->GetArchivesRequests())
			{
				pCIPCDatabase->DoIndicateRemoveArchiv(wArchivNr);
			}
		}
	}
	acs.Unlock();
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
			CIPCDatabaseServer* pCIPCDatabase = pClient->GetCIPCDatabaseServer();
			if (pCIPCDatabase->GetDriveRequests())
			{
				pCIPCDatabase->OnRequestDrives();
			}
		}
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CClients::DoIndicateNewArchivName(DWORD dwClientID, CArchiv& a)
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
			CIPCDatabaseServer* pCIPCDatabase = pClient->GetCIPCDatabaseServer();
			CSecID id(SECID_GROUP_ARCHIVE,a.GetNr());
			if (a.IsAlarmList())
			{
				if (pCIPCDatabase->GetAlarmListRequests())
				{
					pCIPCDatabase->DoConfirmGetValue(id,CSD_NAME,a.GetName(),0);
				}
			}
			else
			{
				if (pCIPCDatabase->GetArchivesRequests())
				{
					pCIPCDatabase->DoConfirmGetValue(id,CSD_NAME,a.GetName(),0);
				}
			}
		}
	}
	Unlock();
}
