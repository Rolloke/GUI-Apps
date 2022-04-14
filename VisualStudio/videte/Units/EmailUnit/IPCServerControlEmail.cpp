// IPCServerControlEmail.cpp: implementation of the CIPCServerControlEmail class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EmailUnit.h"
#include "IPCServerControlEmail.h"

#include "EmailClient.h"
#include "EmailUnitDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCServerControlEmail::CIPCServerControlEmail(CEmailUnitDlg* pDlg)
	:CIPCServerControl(WK_SMServerControlEMail,TRUE)
{
	m_pDlg = pDlg;
	StartThread();
}

CIPCServerControlEmail::~CIPCServerControlEmail()
{
	StopThread();
}


//////////////////////////////////////////////////////////////////////////////////////
// Alarm wurde ausgelöst


void CIPCServerControlEmail::OnRequestAlarmConnection(const CConnectionRecord &c)
{

	WK_TRACE(_T("[CIPCServerControlEmail::OnRequestAlarmConnection] %s\n"),c.GetDestinationHost());

	m_pDlg->m_ConnectionRecords.SafeAddTail(new CConnectionRecord(c));


	//Nachricht an MainWnd ein Bild per Email zu verschicken
	if (m_pDlg)
	{
		m_pDlg->PostMessage(WM_EMAIL,NULL,NULL);
	
	}
	else
	{
		WK_TRACE_ERROR(_T("No Mainwindow!\n"));
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void CIPCServerControlEmail::OnRequestServerReset(const CConnectionRecord &c)
{



}

