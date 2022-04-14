// EndcontrolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Endcontrol.h"
#include "EndcontrolDlg.h"
#include <direct.h>
#include <winbase.h>
#include <user.h>  // für CUserArray


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEndcontrolDlg dialog

CEndcontrolDlg::CEndcontrolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEndcontrolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEndcontrolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEndcontrolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEndcontrolDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEndcontrolDlg, CDialog)
	//{{AFX_MSG_MAP(CEndcontrolDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEndcontrolDlg message handlers

BOOL CEndcontrolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_bVideoSaveOK = true;

	
	ShowMainDlg();

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

/////////////////////////////////////////////////////////////////////////////
void CEndcontrolDlg::OnPaint() 
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
HCURSOR CEndcontrolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



/////////////////////////////////////////////////////////////////////////////

void CEndcontrolDlg::ShowMainDlg()
{

	POSITION positionOK;
	POSITION positionNotOK;
	
	if(!(m_ListStatusOK.IsEmpty()) && !(m_ListStatusNotOK.IsEmpty()) ) 
	{
	
		positionOK = m_ListStatusOK.GetHeadPosition();
		positionNotOK = m_ListStatusNotOK.GetHeadPosition();

		do
		{
			GetDlgItem(m_ListStatusOK.GetNext(positionOK))->ShowWindow(SW_SHOW);
			GetDlgItem(m_ListStatusNotOK.GetNext(positionNotOK))->ShowWindow(SW_HIDE);

		}while(positionOK);

	UpdateData(true);
	UpdateWindow();
	}
	
}


/////////////////////////////////////////////////////////////////////////////
CList<int, int>* CEndcontrolDlg::GetListStatusOK()
{
	return &m_ListStatusOK;
}
/////////////////////////////////////////////////////////////////////////////
CList<int, int>* CEndcontrolDlg::GetListStatusNotOK()
{
	
	return &m_ListStatusNotOK;
}
