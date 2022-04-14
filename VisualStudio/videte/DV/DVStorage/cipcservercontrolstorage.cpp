// CIPCServerControlStorage.cpp: implementation of the CIPCServerControlStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVStorage.h"
#include "CIPCServerControlStorage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCServerControlStorage::CIPCServerControlStorage()
	: CIPCServerControl(WK_SMDatabaseControl, TRUE)
{
	m_iCounter = 0;
	SetConnectRetryTime(50);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCServerControlStorage::~CIPCServerControlStorage()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlStorage::OnRequestDataBaseConnection(const CConnectionRecord &c)
{
	WK_TRACE(_T("new connection requested by %s\n"),c.GetSourceHost());
	if (theApp.IsResetting())
	{
		DoIndicateError(0, SECID_NO_ID, 
						CIPC_ERROR_UNABLE_TO_CONNECT, 3,
						_T("Client denied due to server reset"));
	}
	else
	{
		theApp.AddConnectionRecord(c);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlStorage::OnRequestServerReset(const CConnectionRecord &c)
{
	if (   theApp.IsResetting()
		|| theApp.IsFastResetting())
	{
		DoIndicateError(0, SECID_NO_ID, 
						CIPC_ERROR_UNABLE_TO_CONNECT, 3,
						_T("Client denied due to server reset"));
	}
	else
	{
		CWnd* pWnd = AfxGetMainWnd();
		if (WK_IS_WINDOW(pWnd))
		{
			// pWnd->PostMessage(WM_COMMAND,ID_FILE_RESET);
			 pWnd->PostMessage(WM_COMMAND,ID_FILE_FAST_RESET);
		}
		DoConfirmServerReset();
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCServerControlStorage::Run(DWORD dwTimeOut)
{
	m_iCounter++;
	if (m_iCounter==100) 
	{
		m_iCounter = 0;
		CWnd* pWnd = AfxGetMainWnd();
		if (WK_IS_WINDOW(pWnd))
		{
			pWnd->PostMessage(WM_COMMAND,ID_DATE_CHECK);
		}
	}

	return CIPCServerControl::Run(dwTimeOut);
}
