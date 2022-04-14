// CIPCServerControlProcess.cpp: implementation of the CIPCServerControlProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVProcess.h"
#include "CIPCServerControlProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCServerControlProcess::CIPCServerControlProcess()
	: CIPCServerControl(WK_SMServerControl, TRUE)
{
	m_iCounter = 0;
	SetConnectRetryTime(100);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCServerControlProcess::~CIPCServerControlProcess()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlProcess::OnRequestInputConnection(const CConnectionRecord &c)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(0, SECID_NO_ID, 
						CIPC_ERROR_UNABLE_TO_CONNECT, 3,
						_T("Client denied due to server reset"));
	}
	else
	{
		theApp.AddInputConnectionRecord(c);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlProcess::OnRequestOutputConnection(const CConnectionRecord &c)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(0, SECID_NO_ID, 
						CIPC_ERROR_UNABLE_TO_CONNECT, 3,
						_T("Client denied due to server reset"));
	}
	else
	{
		theApp.AddOutputConnectionRecord(c);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlProcess::OnRequestAudioConnection(const CConnectionRecord &c)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(0, SECID_NO_ID, 
						CIPC_ERROR_UNABLE_TO_CONNECT, 3,
						_T("Client denied due to server reset"));
	}
	else
	{
		theApp.AddAudioConnectionRecord(c);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlProcess::OnRequestServerReset(const CConnectionRecord &c)
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
//////////////////////////////////////////////////////////////////////
BOOL CIPCServerControlProcess::Run(DWORD dwTimeOut)
{
	m_iCounter++;
	if (m_iCounter == 20) 
	{
		m_iCounter = 0;
		CWnd* pWnd = AfxGetMainWnd();
		if (WK_IS_WINDOW(pWnd))
		{
			pWnd->PostMessage(WM_COMMAND,ID_DATE_CHECK);
		}
	}
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}

	return CIPCServerControl::Run(dwTimeOut);
}
