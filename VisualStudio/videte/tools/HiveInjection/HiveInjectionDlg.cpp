// HiveInjectionDlg.cpp : Implementierungsdatei
//
#include "stdafx.h"
#include "HiveInjection.h"
#include "HiveInjectionDlg.h"
#include "CProfile.h"
#include "CObserveDirectory.h"

#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg-Dialogfeld für Anwendungsbefehl 'Info'
#define WM_TRAYCLICKED WM_USER + 1


/////////////////////////////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CHiveInjectionDlg Dialogfeld


/////////////////////////////////////////////////////////////////////////////////////////////////////////
CHiveInjectionDlg::CHiveInjectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHiveInjectionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nCountOBDs = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHiveInjectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHiveInjectionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BACKUP, OnBnClickedBackup)
	ON_BN_CLICKED(IDC_RESTORE, OnBnClickedRestore)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)

	ON_WM_CREATE()
END_MESSAGE_MAP()

#include ".\HiveInjectiondlg.h"

// CHiveInjectionDlg Meldungshandler

/////////////////////////////////////////////////////////////////////////////////////////////////////////
int CHiveInjectionDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

		// Icon ins Systemtray
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;

		tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYCLICKED;
		tnd.hIcon		= m_hIcon;

		CString sTip = _T("HiveInjection");
		lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
		Shell_NotifyIcon(NIM_ADD, &tnd);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHiveInjectionDlg::OnInitDialog()
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

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	CenterWindow();
	ShowWindow(SW_MINIMIZE);
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);

	//
	CString sRootRegistryKey = SOFTWARE_KEY _T("\\") WK_PROFILE_ROOT SECTION_HIVE_INJECTION;
	CProfile prof(sRootRegistryKey + _T("\\") SECTION_HIVE);

	DWORD i, dwMask=1, dwLogical = GetLogicalDrives();
	CString sDrive;
	// start with d:, and find first fixed drive
	for (i=3,dwMask=8; i<27; i++,dwMask<<=1)
	{
		if (dwMask & dwLogical)
		{
			sDrive.Format(_T("%c:\\"), i+'a');
			int nType = GetDriveType(sDrive);
			if (nType == DRIVE_FIXED)
			{
				break;
			}
		}
	}

	// Das CHive-Objekt anlegen...
	CString sHivePath = sDrive + _T("Backup\\Hive\\Dvrt.dat");
	prof.ReadProfileString(HIVE_PATH_FILE, sHivePath, sHivePath);
	prof.WriteProfileString(HIVE_PATH_FILE, sHivePath);

	m_hive.Open(HKEY_LOCAL_MACHINE, SOFTWARE_KEY _T("\\") WK_PROFILE_ROOT, sHivePath, TRUE, 1000);
	m_hive.RestoreKeys();

	// Dies sind die überwachten Verzeichnisse...
	CProfile prof2(sRootRegistryKey+_T("\\") SECTION_DIRECTORIES);
	InitObservedDirectoryValues(prof2, _T("C:\\DV\\")      , sDrive + _T("Backup\\DV\\")      , FALSE);
	InitObservedDirectoryValues(prof2, _T("C:\\Security\\"), sDrive + _T("Backup\\Security\\"), FALSE);
	InitObservedDirectoryValues(prof2, _T("C:\\dvrtwww\\") , sDrive + _T("Backup\\dvrtwww\\") , FALSE);

	// Dies sind die überwchten Fileextensions...
	CProfile prof3(sRootRegistryKey);
	CString sFilterExts = _T("*.msk,*.dbf,*.htm,*.jpg,*.gif,*.png,*.bmp,*.ini");
	prof.ReadProfileString(INCLUDED_FILE_EXTENSIONS, sFilterExts, sFilterExts);
	sFilterExts.Remove('*');
	sFilterExts.Remove(' ');
	prof3.WriteProfileString(INCLUDED_FILE_EXTENSIONS, sFilterExts);
	sFilterExts += _T(","); // vereinfacht das parsen der Extensions.

	// Ein CObserveDirectory-Objekt pro überwachtes Verzeichnis anlegen...
	CString sKeyName, sKeyValue;
	int nI = 0;
	while(prof2.EnumKey(nI, sKeyName, sKeyValue) == ERROR_SUCCESS)
	{
		if (!sKeyName.IsEmpty() && !sKeyValue.IsEmpty())
		{
			if (sKeyName[sKeyName.GetLength()-1]!='\\') 
				sKeyName += '\\';
			if (sKeyValue[sKeyValue.GetLength()-1]!='\\') 
				sKeyValue += '\\';

			if (nI < MAX_OBD)
			{
				TRACE(_T("Mirror %s -> %s\n"), sKeyName, sKeyValue);
				m_pObDir[nI] = new CObserveDirectory(sKeyName, sKeyValue, TRUE);
				m_pObDir[nI]->AddFilterExtensionList(sFilterExts);
				m_pObDir[nI++]->RestoreDirectory();

				m_nCountOBDs = nI;

			}
			else
				TRACE(_T("To many observed directories. (%d>%d)\n"), nI+1, MAX_OBD);
		}
	};

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHiveInjectionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_MINIMIZE);
		ShowWindow(SW_HIDE);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
	TRACE(_T("OnSysCommand nID=%d, lParam=%d\n"), nID, lParam);
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHiveInjectionDlg::OnPaint() 
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////
HCURSOR CHiveInjectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHiveInjectionDlg::OnBnClickedBackup()
{
	GetDlgItem(IDC_BACKUP)->EnableWindow(FALSE);
	m_hive.BackupKeys();
	GetDlgItem(IDC_BACKUP)->EnableWindow(TRUE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHiveInjectionDlg::OnBnClickedRestore()
{
	GetDlgItem(IDC_RESTORE)->EnableWindow(FALSE);
	m_hive.RestoreKeys();
	GetDlgItem(IDC_RESTORE)->EnableWindow(TRUE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHiveInjectionDlg::OnDestroy()
{
	CDialog::OnDestroy();

	m_hive.BackupKeys();

	for (int nI = 0; nI < m_nCountOBDs; nI++)
	{
		delete m_pObDir[nI];
		m_pObDir[nI] = NULL;
	}

	// Nur erforderlich, wenn Icon in der Systemtaskleiste
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;
	
	Shell_NotifyIcon(NIM_DELETE, &tnd);

}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHiveInjectionDlg::OnTrayClicked(WPARAM /*wParam*/, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_LBUTTONDOWN:
		{
			if (IsIconic())
				ShowWindow(SW_RESTORE);
			else
			{
				PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
				ShowWindow(SW_MINIMIZE);
			}

		}
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////
CString CHiveInjectionDlg::InitObservedDirectoryValues(CProfile &prof, LPCTSTR strProg, LPCTSTR strCommand, bool bForce/*=false*/)
{
	CString sValue;
	if (bForce)
		sValue = strCommand;
	else
		prof.ReadProfileString(strProg, strCommand, sValue);

	prof.WriteProfileString(strProg, sValue);

	return sValue;
}
