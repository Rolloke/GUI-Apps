// NetSpy.cpp : implementation file
//

#include "stdafx.h"
#include "ServerMonitor.h"
#include "NetSpy.h"
#include "ClientConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetSpy

CNetSpy::CNetSpy(CServerMonitorDlg* pDlg)
{
	m_pDlg=pDlg;

	
}

CNetSpy::~CNetSpy()
{

}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CNetSpy, CSocket)
	//{{AFX_MSG_MAP(CNetSpy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CNetSpy member functions

void CNetSpy::OnAccept(int nErrorCode) 
{

	TRACE("Accept\n");
	CClientConnection *pClientSocket = new CClientConnection(m_pDlg);

	m_pDlg->m_pClientConnection=pClientSocket;
	
	if(Accept(*pClientSocket))
	{
		m_pDlg->m_sMessage="Accepted";
		m_pDlg->UpdateData(FALSE);
	}
	
	//	CString send="220\r\n.\r\n.";
	//	pClientConnection->Send(send,send.GetLength());
	
	
}
