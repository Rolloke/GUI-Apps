// IPCServerControlGemos.cpp: implementation of the CIPCServerControlGemos class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GemosUnit.h"
#include "IPCServerControlGemos.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCServerControlGemos::CIPCServerControlGemos()
	: CIPCServerControl(WK_SMServerControlGemos,TRUE)
{
	SetConnectRetryTime(100);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCServerControlGemos::~CIPCServerControlGemos()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlGemos::OnRequestServerReset(const CConnectionRecord &c)
{
	if (theApp.IsResetting())
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
			pWnd->PostMessage(WM_COMMAND,ID_FILE_RESET);
		}
		DoConfirmServerReset();
	}
}
