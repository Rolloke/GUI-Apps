// CIPCServerControlDBS.cpp: implementation of the CIPCServerControlDBS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBS.h"
#include "CIPCServerControlDBS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCServerControlDBS::CIPCServerControlDBS()
	: CIPCServerControl(WK_SMDatabaseControl, TRUE)
{
	SetConnectRetryTime(50);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCServerControlDBS::~CIPCServerControlDBS()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlDBS::OnRequestDataBaseConnection(const CConnectionRecord &c)
{
	TRACE(_T("new connection requested by %s\n"),c.GetSourceHost());
// GF todo und was ist mit lokaler Station beim DTS-Receiver?
// vorerst gecancelt, muss neu gefixt werden
//	if ((c.GetOptions() & SCO_IS_DV) == 0)
	{
		if (theApp.IsResetting())
		{
			DoIndicateError(SRV_CONTROL_REQUEST_DB_CONNECTION, SECID_NO_ID, 
							CIPC_ERROR_UNABLE_TO_CONNECT, 3,
							_T("Client denied due to server reset"));
		}
		else
		{
			theApp.AddConnectionRecord(c);
		}
	}
/*	else
	{
		DoIndicateError(SRV_CONTROL_REQUEST_DB_CONNECTION,SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT,0);
	}
*/
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlDBS::OnRequestServerReset(const CConnectionRecord &c)
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
			if (theApp.IsDTS())
			{
				pWnd->PostMessage(WM_COMMAND,ID_FILE_FAST_RESET);
			}
			else
			{
				pWnd->PostMessage(WM_COMMAND,ID_FILE_RESET);
			}
		}
		DoConfirmServerReset();
	}
}
