// pwgenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pwgen.h"
#include "pwgenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char Transform(char c)
{
	switch (c)
	{
	case '0':
		return 'q';
	case '1':
		return 'w';
	case '2':
		return 'e';
	case '3':
		return 'r';
	case '4':
		return 't';
	case '5':
		return 'z';
	case '6':
		return 'u';
	case '7':
		return 'i';
	case '8':
		return 'o';
	case '9':
		return 'p';
	case 'A':
		return 'a';
	case 'B':
		return 's';
	case 'C':
		return 'd';
	case 'D':
		return 'f';
	case 'E':
		return 'g';
	case 'F':
		return 'h';
	}
	return ' ';
}
CString TransPassword(CString s)
{
	CString sP = s;
	sP.MakeUpper();
	for (int i=0;i<sP.GetLength();i++)
	{
		sP.SetAt(i,Transform(sP[i]));
	}
	return sP;
}
/////////////////////////////////////////////////////////////////////////////
// CPwgenDlg dialog

CPwgenDlg::CPwgenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPwgenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPwgenDlg)
	m_sPassword = _T("");
	m_sTime = _T("");
	m_sDWORD = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	CTime ct = CTime::GetCurrentTime();
	CString sP;
	DWORD dw = (DWORD)ct.GetTime();
	m_sTime.Format("%02d.%02d.%04d %02d:%02d:%02d",ct.GetDay(),ct.GetMonth(),ct.GetYear(),
		ct.GetHour(),ct.GetMinute(),ct.GetSecond());
	m_sDWORD.Format("%08lx",dw);
	m_sPassword = TransPassword(m_sDWORD);
}

void CPwgenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPwgenDlg)
	DDX_Text(pDX, IDC_TXT_PASSWORD, m_sPassword);
	DDX_Text(pDX, IDC_TIME, m_sTime);
	DDX_Text(pDX, IDC_DWORD, m_sDWORD);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPwgenDlg, CDialog)
	//{{AFX_MSG_MAP(CPwgenDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPwgenDlg message handlers

BOOL CPwgenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPwgenDlg::OnPaint() 
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
HCURSOR CPwgenDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
