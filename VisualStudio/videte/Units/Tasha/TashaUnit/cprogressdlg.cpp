// CProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CProgressDlg.h"
#include "resource.h"
#include ".\cprogressdlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_PROGRESS	WM_USER+10

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog


/////////////////////////////////////////////////////////////////////////////
CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//Create(CProgressDlg::IDD, AfxGetMainWnd());
	Create(CProgressDlg::IDD, pParent);
}

/////////////////////////////////////////////////////////////////////////////
CProgressDlg::~CProgressDlg()
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_PROGRESS_BOOTING, m_CtrlProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	ON_WM_CREATE()
	ON_MESSAGE(WM_PROGRESS, OnWmProgress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

/////////////////////////////////////////////////////////////////////////////
long CProgressDlg::OnWmProgress(WPARAM wParam, LPARAM /*lParam*/)
{
	m_CtrlProgress.SetPos(wParam);
	TRACE("CProgressDlg::OnWmProgress %d\n", wParam);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_CtrlProgress.SetRange32(0, 99);
	m_CtrlProgress.SetStep(1);

	CenterWindow();
	ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
int CProgressDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CProgressDlg::SetProgress(int nValue, WORD wSource)
{
	CString sText;
	sText.Format(_T("DSP: %d booting successfully."), wSource);
	GetDlgItem(IDC_STATIC_INFOTEXT)->SetWindowText(sText);
	SendMessage(WM_PROGRESS, nValue);
}
