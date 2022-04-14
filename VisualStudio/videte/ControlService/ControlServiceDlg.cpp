// ControlServiceDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "ControlService.h"
#include "ControlServiceDlg.h"
#include ".\controlservicedlg.h"
#include "cprofile.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg-Dialogfeld für Anwendungsbefehl 'Info'

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CControlServiceDlg Dialogfeld



CControlServiceDlg::CControlServiceDlg(CMyService* pService, CWnd* pParent /*=NULL*/)
	: CDialog(CControlServiceDlg::IDD, pParent)
	, m_sCtrlApp(_T("HiveInjection.exe"))
	, m_bAutoClose(FALSE)
{
	m_pService = pService;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_sRootRegistryKey = _T("Software\\DVRT\\");
	m_sRootRegistryKey += AfxGetAppName();
}

void CControlServiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CONTROLED_APP, m_sCtrlApp);
	DDX_Check(pDX, IDC_CHECK_AUTO_CLOSE, m_bAutoClose);
}

BEGIN_MESSAGE_MAP(CControlServiceDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_INSTALL_CONTROL_SERVICE, OnBnClickedInstallControlService)
	ON_BN_CLICKED(IDC_UNINSTALL_CONTROL_SERVICE, OnBnClickedUninstallControlService)
	ON_BN_CLICKED(IDC_START_CONTROL_SERVICE, OnBnClickedStartControlService)
	ON_BN_CLICKED(IDC_STOP_CONTROL_SERVICE, OnBnClickedStopControlService)
	ON_BN_CLICKED(IDC_PAUSE_CONTROL_SERVICE, OnBnClickedPauseControlService)
	ON_BN_CLICKED(IDC_CONTINUE_CONTROL_SERVICE, OnBnClickedContinueControlService)
	ON_BN_CLICKED(IDC_CHECK_AUTO_CLOSE, OnBnClickedCheckAutoClose)
END_MESSAGE_MAP()


// CControlServiceDlg Meldungshandler

///////////////////////////////////////////////////////////////////////////
BOOL CControlServiceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
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
	CProfile prof(m_sRootRegistryKey);
	prof.ReadProfileString(_T("ControlApplication"), _T(""), m_sCtrlApp);

	m_bAutoClose = prof.ReadProfileFlag(_T("AutoClose"), TRUE);

	SetWindowText(AfxGetAppName());
	UpdateData(FALSE);

	UpdateDialog();

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// TODO: Hier zusätzliche Initialisierung einfügen
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

///////////////////////////////////////////////////////////////////////////
void CControlServiceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

///////////////////////////////////////////////////////////////////////////
void CControlServiceDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
///////////////////////////////////////////////////////////////////////////
HCURSOR CControlServiceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

///////////////////////////////////////////////////////////////////////////
void CControlServiceDlg::OnBnClickedInstallControlService()
{
	UpdateData(TRUE);
	CProfile prof(m_sRootRegistryKey);

	prof.WriteProfileString(_T("ControlApplication"), m_sCtrlApp);

	if (m_pService->InstallService())
		TRACE(_T("\n\nService Installed Sucessfully\n"));
	else
		TRACE(_T("\n\nError Installing Service\n"));
	UpdateDialog();
}

///////////////////////////////////////////////////////////////////////////
void CControlServiceDlg::OnBnClickedUninstallControlService()
{
	if (m_pService->DeleteService())
		TRACE(_T("\n\nService UnInstalled Sucessfully\n"));
	else
		TRACE(_T("\n\nError UnInstalling Service\n"));
	UpdateDialog();
}

///////////////////////////////////////////////////////////////////////////
void CControlServiceDlg::OnBnClickedStartControlService()
{
	if (m_pService->StartService())
		TRACE(_T("\n\nService start Sucessfully\n"));
	else
		TRACE(_T("\n\nError starting Service\n"));
	UpdateDialog();
}

///////////////////////////////////////////////////////////////////////////
void CControlServiceDlg::OnBnClickedStopControlService()
{
	if (m_pService->EndService())
		TRACE(_T("\n\nService ended Sucessfully\n"));
	else
		TRACE(_T("\n\nError ending Service\n"));
	UpdateDialog();
}

///////////////////////////////////////////////////////////////////////////
void CControlServiceDlg::OnBnClickedPauseControlService()
{
	if (m_pService->PauseService())
		TRACE(_T("\n\nService ended Sucessfully\n"));
	else
		TRACE(_T("\n\nError ending Service\n"));
	UpdateDialog();
}

///////////////////////////////////////////////////////////////////////////
void CControlServiceDlg::OnBnClickedContinueControlService()
{
	if (m_pService->ContinueService())
		TRACE(_T("\n\nService ended Sucessfully\n"));
	else
		TRACE(_T("\n\nError ending Service\n"));
	UpdateDialog();
}

///////////////////////////////////////////////////////////////////////////
BOOL CControlServiceDlg::UpdateDialog(void)
{
	SERVICE_STATUS Status;
	
	if (m_pService->GetStatus(Status))
	{
		GetDlgItem(IDC_CHECK_AUTO_CLOSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CONTROLED_APP)->EnableWindow(FALSE);
		GetDlgItem(IDC_INSTALL_CONTROL_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_UNINSTALL_CONTROL_SERVICE)->EnableWindow(TRUE);

		if (Status.dwCurrentState == SERVICE_STOPPED)
		{
			GetDlgItem(IDC_START_CONTROL_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_STOP_CONTROL_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_PAUSE_CONTROL_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CONTINUE_CONTROL_SERVICE)->EnableWindow(FALSE);
		}
		else if (Status.dwCurrentState == SERVICE_RUNNING)
		{
			GetDlgItem(IDC_START_CONTROL_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_STOP_CONTROL_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_PAUSE_CONTROL_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CONTINUE_CONTROL_SERVICE)->EnableWindow(FALSE);
		}
		else if (Status.dwCurrentState == SERVICE_PAUSED)
		{
			GetDlgItem(IDC_START_CONTROL_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_STOP_CONTROL_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_PAUSE_CONTROL_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CONTINUE_CONTROL_SERVICE)->EnableWindow(TRUE);
		}
	}
	else
	{
		GetDlgItem(IDC_CHECK_AUTO_CLOSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CONTROLED_APP)->EnableWindow(TRUE);
		GetDlgItem(IDC_INSTALL_CONTROL_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_UNINSTALL_CONTROL_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_CONTROL_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOP_CONTROL_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_PAUSE_CONTROL_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONTINUE_CONTROL_SERVICE)->EnableWindow(FALSE);
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
void CControlServiceDlg::OnBnClickedCheckAutoClose()
{
	UpdateData(TRUE);
	CProfile prof(m_sRootRegistryKey);
	prof.WriteProfileFlag(_T("AutoClose"), m_bAutoClose);
}
