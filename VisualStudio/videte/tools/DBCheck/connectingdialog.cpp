// ConnectingDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DBCheck.h"
#include "Server.h"
#include "ConnectingDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectingDialog dialog
/////////////////////////////////////////////////////////////////////////////
CConnectingDialog::CConnectingDialog(CServer* pServer,
									 const CString& sHost,
									 DWORD dwConnectTimeout,
									 CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CConnectingDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectingDialog)
	m_sHost = sHost;
	//}}AFX_DATA_INIT
	m_pServer = pServer;
	m_dwConnectTimeout = dwConnectTimeout;	// in ms
	m_uTimerID = 0;

	Create(IDD, pParent);
}
/////////////////////////////////////////////////////////////////////////////
void CConnectingDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectingDialog)
	DDX_Control(pDX, IDC_PROGRESS_CONNECT, m_ctrlProgress);
	DDX_Text(pDX, IDC_HOST, m_sHost);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CConnectingDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CConnectingDialog)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CConnectingDialog message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CConnectingDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CenterWindow();

	DWORD dwTimerInterval = 100;	// in ms
	DWORD dwTicks = m_dwConnectTimeout / dwTimerInterval + 1;
	m_ctrlProgress.SetStep(m_dwConnectTimeout/dwTicks);
	m_ctrlProgress.SetRange32(0, m_dwConnectTimeout);
	m_uTimerID = SetTimer(1, dwTimerInterval, NULL);
	TRACE("timer %u\n", m_uTimerID);
	if (0 == m_uTimerID)
	{
		WK_TRACE_ERROR("No valid timer for connecting\n");
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CConnectingDialog::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	m_ctrlProgress.StepIt();
	
	CWK_Dialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CConnectingDialog::OnCancel() 
{
	m_pServer->CancelConnecting();
	DestroyWindow();;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CConnectingDialog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (0 != m_uTimerID)
	{
		KillTimer(m_uTimerID);
	}
	return CWK_Dialog::DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CConnectingDialog::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	CWK_Dialog::PostNcDestroy();
	delete this;
}
