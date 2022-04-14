// EULADlg.cpp : implementation file
//

#include "stdafx.h"
#include "EULA.h"
#include "EULADlg.h"
#include "EULALicenseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEULADlg dialog

CEULADlg::CEULADlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEULADlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEULADlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEULADlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEULADlg)
	DDX_Control(pDX, IDCANCEL, m_ctrlCancel);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_TEXT_TOP_LICENSE, m_ctrlTextTopLicense);
	DDX_Control(pDX, IDC_TEXT_BOTTOM_LICENSE, m_ctrlTextBottomLicense);
	DDX_Control(pDX, IDC_RICHEDIT_EULA, m_EULA);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEULADlg, CDialog)
	//{{AFX_MSG_MAP(CEULADlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_UPDATE(IDC_RICHEDIT_EULA, OnUpdateRicheditEula)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEULADlg message handlers

BOOL CEULADlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString sFile(theApp.GetEulaRTF());
	if (!sFile.IsEmpty())
	{
		if (m_EULAFile.Open(sFile,CFile::modeRead))
		{
			EDITSTREAM es;
			es.dwCookie = (DWORD)this;
			es.pfnCallback = EditStreamCallback;
			es.dwError = 0;
			m_EULA.StreamIn(SF_RTF,es);
			m_EULAFile.Close();
		}
	}
	
	Init();

	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
DWORD CALLBACK CEULADlg::EditStreamCallback( DWORD dwCookie, // application-defined value
											  LPBYTE pbBuff,      // data buffer
											  LONG cb,            // number of bytes to read or write
											  LONG *pcb           // number of bytes transferred
											)
{
	CEULADlg* pThis = (CEULADlg*)dwCookie;
	DWORD dwRet = 0;

	TRY
	{
		*pcb = pThis->m_EULAFile.Read(pbBuff,cb);
		TRACE(_T("read %d Bytes of %d\n"),*pcb,cb);
	}
	CATCH (CFileException, cfe)
	{
		dwRet = cfe->m_lOsError;
	}
	END_CATCH;

	return dwRet;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

/////////////////////////////////////////////////////////////////////////////
void CEULADlg::OnPaint() 
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
HCURSOR CEULADlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////
void CEULADlg::OnUpdateRicheditEula() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	if((m_EULA.GetFirstVisibleLine() >= (m_EULA.GetLineCount()-LINES_VISIBLE)) || m_bLineFeedRichEdit)
	{
		m_ctrlOK.EnableWindow();
		m_ctrlOK.SetFocus();
		m_bLineFeedRichEdit = TRUE;
	}
	else
	{
		m_ctrlOK.EnableWindow(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CEULADlg::Init() 
{
	m_EULA.SetBackgroundColor(FALSE, RGB(190,190, 190));
	m_EULA.SetEventMask(m_EULA.GetEventMask() | ENM_UPDATE);
	m_bLineFeedRichEdit = FALSE;
	CenterWindow(GetDesktopWindow());
}

/////////////////////////////////////////////////////////////////////////////
void CEULADlg::OnOK() 
{	
	ShowWindow(SW_HIDE);
	CEULALicenseDlg dlgLicense;
	int nResponseDlgLicense = dlgLicense.DoModal();
	if(nResponseDlgLicense)
	{
		TRACE(_T("OK\n"));
	}
	CDialog::OnOK();
}

//////////////////////////////////////////////////////////////////////
void CEULADlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	m_EULA.SetFocus();	


}


