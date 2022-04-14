// DlgConnecting.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "Server.h"
#include "DlgConnecting.h"
#include ".\dlgconnecting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConnecting dialog


CDlgConnecting::CDlgConnecting(CServer* pServer,
									 const CString& sHost,
									 DWORD dwConnectTimeout,
									 CWnd* pParent /*=NULL*/)
	: CSkinDialog(CDlgConnecting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConnecting)
	m_sHost = sHost;
	//}}AFX_DATA_INIT
	m_wServerID = pServer->GetID();
	m_dwConnectTimeout = dwConnectTimeout;	// in ms
	m_uTimerID = 0;

	m_nInitToolTips = FALSE;
	MoveOnClientAreaClick(true);
	Create(IDD, pParent);
}


void CDlgConnecting::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConnecting)
	DDX_Control(pDX, IDC_PROGRESS_CONNECT, m_ctrlProgress);
	DDX_Text(pDX, IDC_HOST, m_sHost);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgConnecting, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgConnecting)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDlgConnecting message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgConnecting::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	AutoCreateSkinStatic();
	
	CenterWindow();

	DWORD dwTimerInterval = 100;	// in ms
	DWORD dwTicks = m_dwConnectTimeout / dwTimerInterval + 1;
	m_ctrlProgress.SetStep(m_dwConnectTimeout/dwTicks);
	m_ctrlProgress.SetRange32(0, m_dwConnectTimeout);
	m_uTimerID = SetTimer(1, dwTimerInterval, NULL);
	TRACE(_T("timer %u\n"), m_uTimerID);
	if (0 == m_uTimerID)
	{
		WK_TRACE_ERROR(_T("No valid timer for connecting\n"));
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgConnecting::OnTimer(UINT nIDEvent) 
{
	m_ctrlProgress.StepIt();
	
	CSkinDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgConnecting::Cancel()
{
	OnCancel();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgConnecting::OnCancel() 
{
	CServer *pServer = theApp.GetServer(m_wServerID);
	if (pServer)
	{
		pServer->CancelConnecting();
	}
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgConnecting::PostNcDestroy() 
{
	CSkinDialog::PostNcDestroy();
	delete this;
}

void CDlgConnecting::OnDestroy()
{
	if (0 != m_uTimerID)
	{
		KillTimer(m_uTimerID);
	}
	CSkinDialog::OnDestroy();
}

void CDlgConnecting::OnOK()
{
	CSkinDialog::OnOK();
	DestroyWindow();
}
