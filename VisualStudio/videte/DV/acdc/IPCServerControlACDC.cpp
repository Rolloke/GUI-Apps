// IPCServerControlACDC.cpp: implementation of the CIPCServerControlACDC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "acdc.h"
#include "exitcode.h"


#include "IPCServerControlACDC.h"
#include "IPCBurnDataCarrier.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCServerControlACDC::CIPCServerControlACDC()
: CIPCServerControl(WK_SMServerControlACDC,TRUE)
{
	m_pClient = NULL;	
	StartThread();
}

CIPCServerControlACDC::~CIPCServerControlACDC()
{
	StopThread();
	WK_DELETE(m_pClient);
}
//////////////////////////////////////////////////////////////////////
void CIPCServerControlACDC::OnRequestDCConnection(const CConnectionRecord &c)
{
	theApp.m_Drives.RemoveAll();
	TRACE(_T("CIPCServerControlACDC::OnRequestDCConnection\n"));
	if (m_pClient)
	{
		DoIndicateError(0, SECID_NO_ID, 
						CIPC_ERROR_UNABLE_TO_CONNECT, 4,
						_T("Client denied only one client allowed"));
	}
	else
	{
		CString sShmName;
		sShmName.Format(_T("CIPCShmAcdc%08lx"),GetTickCount());
		m_pClient = new CIPCBurnDataCarrier(sShmName);
		DoConfirmDCConnection(sShmName,_T(""),0
#ifdef _UNICODE
			, MAKELONG(CODEPAGE_UNICODE, 0)
#endif
			);
		
		CAcdcDlg* pAcdcDlg = theApp.GetAcdcDlg();
		if(WK_IS_WINDOW(pAcdcDlg))
		{
			m_sNewMessage = _T("CIPC connection established\n");
			::PostMessage(pAcdcDlg->m_hWnd, WM_BURN_NEW_MESSAGE, NM_SERVER_CONTROL_THREAD, NULL);
		}

	}
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCServerControlACDC::Run(DWORD dwTimeOut)
{
	if (   m_pClient
		&& m_pClient->GetIsMarkedForDelete())
	{
		WK_DELETE(m_pClient);

		CAcdcDlg* pAcdcDlg = theApp.GetAcdcDlg();
		if(WK_IS_WINDOW(pAcdcDlg))
		{
			m_sNewMessage = _T("CIPC connection canceled\n");
			::PostMessage(pAcdcDlg->m_hWnd, WM_BURN_NEW_MESSAGE, NM_SERVER_CONTROL_THREAD, NULL);
		}
	}
	// TODO Timeout wenn client nie benutzt wurde
	return CIPCServerControl::Run(dwTimeOut);
}

CIPCBurnDataCarrier* CIPCServerControlACDC::GetBurnDataCarrier()
{
	return m_pClient;
}

CString CIPCServerControlACDC::GetNewMessage()
{
	return m_sNewMessage;
}