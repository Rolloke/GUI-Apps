// ClientConnection.cpp : implementation file
//

#include "stdafx.h"
#include "ServerMonitor.h"
#include "ClientConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientConnection

CClientConnection::CClientConnection(CServerMonitorDlg* pDlg)
{
	m_pDlg= pDlg;
}

CClientConnection::~CClientConnection()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientConnection, CSocket)
	//{{AFX_MSG_MAP(CClientConnection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientConnection member functions

void CClientConnection::OnReceive(int nErrorCode) 
{
	TRACE("RECEIVING\n");
	const int iLen = 200000;
	char Data[iLen+1];
	int iRec = Receive(Data,iLen);
	Data[iLen]='\0';
	m_sReceiveData = (CString)Data;
	TRACE("InputData: len %i\n",iRec);
	m_pDlg->m_eReceiveData=m_pDlg->m_eReceiveData+(CString)Data;
	m_pDlg->UpdateData(FALSE);
}

void CClientConnection::OnSend(int nErrorCode) 
{
	m_pDlg->m_SendButton.EnableWindow(TRUE);
}

void CClientConnection::OnClose(int nErrorCode) 
{
	TRACE("DISCONNECTED\n");
	
	CSocket::OnClose(nErrorCode);
}
