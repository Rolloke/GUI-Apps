// TickCountDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TickCount.h"
#include "TickCountDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTickCountDlg dialog

CTickCountDlg::CTickCountDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTickCountDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTickCountDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTickCountDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTickCountDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTickCountDlg, CDialog)
	//{{AFX_MSG_MAP(CTickCountDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTickCountDlg message handlers

BOOL CTickCountDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	SetTimer(1,1000,NULL);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTickCountDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTickCountDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTickCountDlg::OnTimer(UINT nIDEvent) 
{
	DWORD dwTick = GetTickCount();
	DWORD dwMS = dwTick % 1000;
	dwTick /= 1000; // Sekunden
	DWORD dwS = dwTick % 60;
	dwTick /= 60; // Minuten
	DWORD dwM = dwTick % 60;
	dwTick /= 60; // Stunden
	DWORD dwH = dwTick % 24;
	DWORD dwD = dwTick / 24; // Tage

	CString s;
	s.Format("%d Tage,%02d:%02d:%02d,%04d",dwD,dwH,dwM,dwS,dwMS);
	SetWindowText(s);

	
	CDialog::OnTimer(nIDEvent);
}

void CTickCountDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	KillTimer(1);
	
}
