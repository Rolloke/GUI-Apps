// Client.cpp: implementation of the CClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBS.h"
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
	m_pCIPCDatabaseClient = NULL;
}
//////////////////////////////////////////////////////////////////////
CClient::~CClient()
{
	WK_DELETE(m_pCIPCDatabaseClient);
}
//////////////////////////////////////////////////////////////////////
CIPCError CClient::Connect(const CConnectionRecord& c)
{
	CPermissionArray pa;
	if (!theApp.IsReadOnly())
	{
		CWK_Profile wkp;
		pa.Load(wkp);
	}
	CPermission* pPermission;
	pPermission = pa.CheckPermission(c);

	if (pPermission==NULL)
	{
		WK_TRACE_ERROR(_T("CONNECT no permission\n"));
		return CIPC_ERROR_INVALID_PASSWORD;
		// could also be, but the other case more presumeable
		// return CIPC_ERROR_INVALID_PERMISSIONS;
	}

	if (   !pPermission->IsSuperVisor()
		&& !pPermission->IsSpecialReceiver()
		)
	{
		if (pPermission->GetNumberOfArchivs() == 0)
		{
			WK_TRACE_ERROR(_T("CONNECT no archive permission\n"));
			return CIPC_ERROR_INVALID_PERMISSIONS;
		}
	}

	if (theApp.m_Archives.GetSize() == 0)
	{
		WK_TRACE_ERROR(_T("CONNECT no archives\n"));
		return CIPC_ERROR_ARCHIVE;
	}

	CString shmName;
	shmName.Format(_T("DataBase%08lx"),m_dwID);
	m_pCIPCDatabaseClient = new CIPCDatabaseServer(this,shmName,pPermission);
	m_ConnectionRecord = c;
	m_sName = m_ConnectionRecord.GetSourceHost();
	if (m_sName.IsEmpty())
	{
		if (m_ConnectionRecord.GetDestinationHost()==LOCAL_LOOP_BACK)
		{
			m_sName.LoadString(IDS_LOCAL);
		}
	}
	m_ConnectionRecord.GetFieldValue(CRF_VERSION,m_sVersion);
	if (!m_sVersion.IsEmpty())
	{
		WK_TRACE(_T("client %s is version %s\n"),m_sName,m_sVersion);
	}
#ifdef _UNICODE
	CString sCodePage;
	if (m_ConnectionRecord.GetFieldValue(CRF_CODEPAGE, sCodePage))
	{
		m_pCIPCDatabaseClient->SetCodePage(sCodePage);
	}
	else
	{
		m_pCIPCDatabaseClient->SetCodePage(0);
	}
#endif

	return CIPC_ERROR_OKAY;
}
//////////////////////////////////////////////////////////////////////
BOOL CClient::IsConnected() const
{
	if (m_pCIPCDatabaseClient)
	{
		if (m_pCIPCDatabaseClient->GetIPCState()==CIPC_STATE_CONNECTED)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CClient::IsDisconnected() const
{
	if (m_pCIPCDatabaseClient)
	{
		if (m_pCIPCDatabaseClient->GetIsMarkedForDelete())
		{
			return TRUE;
		}
	}
	return FALSE;
}
