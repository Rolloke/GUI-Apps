// IPCServerControlSMS.cpp: implementation of the CIPCServerControlSMS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SMSUnit.h"
#include "IPCServerControlSMS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCServerControlSMS::CIPCServerControlSMS()
	: CIPCServerControl(WK_SMServerControlSMS, TRUE)
{
	SetConnectRetryTime(100);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCServerControlSMS::~CIPCServerControlSMS()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlSMS::OnRequestAlarmConnection(const CConnectionRecord &c)
{
	theApp.m_aConnectionRecords.SafeAddTail(new CConnectionRecord(c));
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlSMS::OnRequestServerReset(const CConnectionRecord &c)
{
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_COMMAND,ID_FILE_RESET);
	}
	DoConfirmServerReset();
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlSMS::DoIndicateError(const CString& sMessage)
{
	CIPCServerControl::DoIndicateError(
				SRV_CONTROL_REQUEST_ALARM_CONNECTION,	// OOPS not for all
				SECID_NO_ID, 
				CIPC_ERROR_UNABLE_TO_CONNECT,
				0,
				sMessage
				);
}

