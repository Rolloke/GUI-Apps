// MiCoDLRTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MiCoDLRTest.h"
#include "MiCoDLRTestDlg.h"
#include "wk.h"
#include "wk_trace.h"
#include "wk_names.h"
#include "WK_Profile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMiCoDLRTestDlg dialog
CMiCoDLRTestDlg::CMiCoDLRTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMiCoDLRTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMiCoDLRTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
/////////////////////////////////////////////////////////////////////////////
void CMiCoDLRTestDlg::SetTestEntries()
{
	CWK_Profile prof;
	prof.WriteInt("MiCoUnitPCI\\Device1\\Debug", "Mode", 1);
//	prof.WriteInt("MiCoUnitPCI\\Device1\\Debug", "SaveInLog", 0);
	prof.WriteInt("MiCoUnitPCI\\Device1\\Debug", "DLRTest", 1);
	prof.WriteString("MiCoUnitPCI\\Device1\\Debug", "JpegPath", "C:\\Jpeg\\");
	prof.WriteInt("MiCoUnitPCI\\Device1\\Debug\\Trace", "AlarmIndication", 1);
}
/////////////////////////////////////////////////////////////////////////////
void CMiCoDLRTestDlg::ClearTestEntries()
{
	CWK_Profile prof;
	prof.WriteInt("MiCoUnitPCI\\Device1\\Debug", "Mode", 0);
//	prof.WriteInt("MiCoUnitPCI\\Device1\\Debug", "SaveInLog", 0);
	prof.WriteInt("MiCoUnitPCI\\Device1\\Debug", "DLRTest", 0);
	prof.WriteString("MiCoUnitPCI\\Device1\\Debug", "JpegPath", "E:\\Jpeg1\\");
	prof.WriteInt("MiCoUnitPCI\\Device1\\Debug\\Trace", "AlarmIndication", 0);
}
/////////////////////////////////////////////////////////////////////////////
void CMiCoDLRTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMiCoDLRTestDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMiCoDLRTestDlg, CDialog)
	//{{AFX_MSG_MAP(CMiCoDLRTestDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CMiCoDLRTestDlg message handlers
BOOL CMiCoDLRTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetTestEntries();

	if (WinExec("MiCoUnitPCI.exe", SW_SHOW) <= 31)
	{
		ClearTestEntries();
		MessageBox("Kann die MiCoUnit nicht starten", "Fehler");
		WK_TRACE("Fehler beim Laden der MiCoUnit\n");
		PostMessage(WM_CLOSE, 0, 0);
		return FALSE;
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CMiCoDLRTestDlg::OnPaint() 
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
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMiCoDLRTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMiCoDLRTestDlg::DestroyWindow() 
{
	ClearTestEntries();

	if (WK_IS_RUNNING(WK_APP_NAME_MICOUNIT))
	{
		// MiCoUnit muß neu gestartet werden.
		CWnd *pWnd = FindWindow(NULL, WK_APP_NAME_MICOUNIT);
		if (pWnd && IsWindow(pWnd->m_hWnd))
			pWnd->PostMessage(WM_CLOSE, 0, 0);
	}
	
	return CDialog::DestroyWindow();
}

