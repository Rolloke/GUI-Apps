
#include "stdafx.h"
#include "CipcDatabaseClientServer.h"
#include "sec3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
CIPCDatabaseClientServer::CIPCDatabaseClientServer(LPCTSTR shmName)
: CIPCDatabaseClient(shmName)
{
	m_tmIamAlive.SetDelta(1000*60*60);
	StartThread();
	WK_STAT_LOG(_T("Reset"),1,_T("DatabaseActive"));
}
//////////////////////////////////////////////////////////////////////
CIPCDatabaseClientServer::~CIPCDatabaseClientServer()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDatabaseClientServer::Run(DWORD dwTimeOut)
{
	if (GetIPCState() == CIPC_STATE_CONNECTED)
	{
		m_tmIamAlive.StatMessage(_T("Reset"),-2,_T("DatabaseActive"));
	}
	return CIPCDatabaseClient::Run(dwTimeOut);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientServer::OnConfirmConnection()
{
	WK_STAT_LOG(_T("Reset"),0,_T("DatabaseActive"));
	CIPCDatabaseClient::OnConfirmConnection();
	DoRequestAlarmListArchives();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientServer::OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[])
{
	CIPCDatabaseClient::OnConfirmInfo(iNumRecords,data);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientServer::OnConfirmAlarmListArchives(int iNumRecords, const CIPCArchivRecord data[])
{
	for (int i=0;i<iNumRecords;i++)
	{
		OnIndicateNewArchiv(data[i]);
	}

	CIPCArchivRecords& records = GetRecords();

	for (int i=0;i<records.GetSize();i++)
	{
		CIPCArchivRecord* pRecord = records.GetAtFast(i);
		if (   pRecord
			&& pRecord->IsAlarmList())
		{
			m_idAlarmList = CSecID(SECID_GROUP_ARCHIVE, pRecord->GetID());
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientServer::OnRequestDisconnect()
{
	m_tmIamAlive.StatMessage(_T("Reset"),0,_T("DatabaseActive"));
	DelayedDelete();
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientServer::OnIndicateError(DWORD dwCmd, 
												CSecID id, 
												CIPCError error, 
												int iErrorCode,
												const CString &sErrorMessage)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientServer::OnConfirmNewSavePicture(WORD wArchivNr,
														CSecID camID)
{
}
