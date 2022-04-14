/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AutoLogin
// FILE:		$Workfile: AutoLoginDlg.cpp $
// ARCHIVE:		$Archive: /Project/Tools/Internal/AutoLogin/AutoLoginDlg.cpp $
// CHECKIN:		$Date: 18.02.99 13:28 $
// VERSION:		$Revision: 14 $
// LAST CHANGE:	$Modtime: 18.02.99 13:28 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoLogin.h"
#include "AutoLoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CAutoLoginDlg::CAutoLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoLoginDlg)
	m_sName = _T("q");
	m_sPassword = _T("q");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/////////////////////////////////////////////////////////////////////////////
CAutoLoginDlg::~CAutoLoginDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
void CAutoLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoLoginDlg)
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CAutoLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CAutoLoginDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CAutoLoginDlg::OnInitDialog()
{
	char		sVal[255];

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Login-Name und Passwort einlesen, sofern vorhanden
	GetPrivateProfileString("Login", "Name", "q", sVal, 255, "AutoLogin.ini");
	m_sName = sVal;
	GetPrivateProfileString("Login", "Password", "q", sVal, 255, "AutoLogin.ini");
	m_sPassword = sVal;
	UpdateData(FALSE);

	// Dialog immer als oberster
	SetWindowPos(&CWnd::wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
	ShowWindow(SW_SHOWMINIMIZED);

	// Jede Sekunde nach einem Logindialog suchen.
	SetTimer(1, 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

///////////////////////////////////////////////////////////////////////////////////////
void CAutoLoginDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
void CAutoLoginDlg::OnPaint() 
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

///////////////////////////////////////////////////////////////////////////////////////
HCURSOR CAutoLoginDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

///////////////////////////////////////////////////////////////////////////////////////
void CAutoLoginDlg::OnTimer(UINT nIDEvent) 
{
	UpdateData(TRUE);

	// Login-Name und Passwort sichern
	WritePrivateProfileString("Login", "Name", m_sName, "AutoLogin.ini");
	WritePrivateProfileString("Login", "Password", m_sPassword, "AutoLogin.ini");

	// Suche starten
	ClimbWindow(this->GetWindow(GW_HWNDFIRST));
	CDialog::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
void CAutoLoginDlg::ClimbWindow(CWnd* pWnd)
{
	CString	sTitle;
	CWnd*	pChild;  
	char	szClassName[256];
	BOOL	bFindFirst = FALSE;
	
	if (m_sName.IsEmpty())
		return;

	while (pWnd)
	{ 
		// Logindialog suchen
		pWnd->GetWindowText(sTitle);
		
		if ((sTitle.Find(" - Login")!= -1) ||
			(sTitle.Find(" - Legitimation") != -1) ||
			(sTitle.Find("*** Systemfehler ***")!= -1))
		{
			pChild = pWnd->GetWindow(GW_CHILD);
			
			// Suche die Editfelder
			while (pChild)
			{
				GetClassName(pChild->m_hWnd, szClassName, sizeof(szClassName)-1);
				if ((CString)szClassName == "Edit")
				{
					if (!bFindFirst)	
					{
						bFindFirst = TRUE;
						// Erstmal den Inhalt markieren
						pChild->PostMessage(EM_SETSEL, 0, -1);
						// Login-Name ins Editfeld kopieren
						for (int i = 0; i < m_sName.GetLength(); i++)
							pChild->PostMessage(WM_CHAR, m_sName[i]);
					}
					else
					{
						// Erstmal den Inhalt markieren
						pChild->PostMessage(EM_SETSEL, 0, -1);
						// Login-Passwort ins Editfeld kopieren
						for (int i = 0; i < m_sPassword.GetLength(); i++)
							pChild->PostMessage(WM_CHAR, m_sPassword[i]);
					}
				}
				pChild = pChild->GetNextWindow();
			}
			
			// OK-Button suchen
			pChild = pWnd->GetWindow(GW_CHILD);
			while (pChild)
			{
				GetClassName(pChild->m_hWnd, szClassName, sizeof(szClassName)-1);
				if ((CString)szClassName == "Button")
				{
					pWnd->PostMessage(WM_COMMAND, MAKELONG(IDOK, BN_CLICKED), (LPARAM)pChild->m_hWnd);
					pChild = NULL;
				}
				else
				{
					pChild = pChild->GetNextWindow();
				}
			}
		}	

		ClimbWindow(pWnd->GetWindow(GW_CHILD));
		pWnd = pWnd->GetNextWindow();
	}
}	
