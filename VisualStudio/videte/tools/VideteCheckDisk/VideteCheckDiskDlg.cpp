// VideteCheckDiskDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "VideteCheckDisk.h"
#include "VideteCheckDiskDlg.h"

#ifdef _DEBUG
#include "devicedetect\devicedetect.h"
#include ".\videtecheckdiskdlg.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_USER_SET_TAB			(WM_USER + 1)
#define WM_USER_PROCESS			(WM_USER + 2)

#define OUTPUT_TAB		0
#define SETTINGS_TAB	1

const static CString DISKPART_APP(_T("diskpart.exe"));
enum eDiskPartState
{
    DISKPART_CONSOLE_OPEN = 1,
    DISKPART_LIST_DISKS,
    DISKPART_SELECT_DISK,
    DISKPART_DISK_SELECTED,
    DISKPART_LIST_PARTITIONS,
    DISKPART_CREATE_PARTITION_EXT_FINNISHED,
    DISKPART_PARTITION_EXT_OK,
    DISKPART_CREATE_PARTITION_LOG_FINNISHED,
    DISKPART_PARTITION_LOG_OK,
    DISKPART_DRIVE_LETTER_ASSIGNED,
    DISKPART_LIST_VOLUMES,
    DISKPART_SELECT_VOLUME,
    DISKPART_LAST	= 20
};

enum eFormatState
{
    FORMAT_BEGIN = 21,
    FORMAT_CMD_CONSOLE_OPEN,
    FORMAT_CONFIRM_START,
    FORMAT_FINNISHED,
    FORMAT_VALIDATE
};

#define CHKDSK_CMD_CONSOLE_OPEN					30
#define CHKDSK_RUNNING							41

#define STD_OUT_READ_EVENT						100
#define HIDE_DIALOG_WINDOW						101

enum eProductType
{
    VSWT_IDIP=0,
    VSWT_DTS,
    VSWT_VE5900,
    VSWT_WALRUS
};

#define VALIDATE 2
#define RESTORE  3

const static CString BACKUP(            _T("Backup"));
// Check whether EWF is complete
const static CString HKLM_EWF(          _T("SYSTEM\\CurrentControlSet\\Services\\ewf"));
const static CString EWFS_ERROR_CONTROL(_T("ErrorControl"));
const static CString EWFS_GROUP(		_T("Group"));
const static CString EWFS_START(		_T("Start"));
const static CString EWFS_TYPE(			_T("Type"));

const static CString HKLM_EWF_VOLUME(HKLM_EWF + _T("\\Parameters\\Protected\\Volume0"));
const static CString EWF_PARAM_VOLUME_ID(	_T("VolumeID"));
const static CString EWF_PARAM_TYPE(		_T("Type"));
const static CString EWF_PARAM_ARCNAME(	    _T("ArcName"));

#define HKLM_CLASS_VOLUME	_T("SYSTEM\\CurrentControlSet\\Control\\Class\\{71A27CDD-812A-11D0-BEC7-08002BE2092F}")
#define CLASS_VOLUME_UF		_T("UpperFilters")

// prohibit automatic defragmentation
#define HKLM_OPTIMAL_LAYOUT	_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\OptimalLayout")
#define ENABLE_AUTO_LAYOUT	_T("EnableAutoLayout")

// Pagefile
#define HKLM_MEM_MGMT		_T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Memory Management")
#define MEM_MGMT_PAGE_FILE	_T("PagingFiles")	// REG_MULTI_SZ

// Temp Pfade
#define HKCU_ENV_TEMP		_T("Environment")
#define HKLM_ENV_TEMP		_T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment")
#define ENV_TEMP			_T("TEMP")
#define ENV_TMP				_T("TMP")

// windows logging paths
#define HKLM_WBEM			_T("SOFTWARE\\Microsoft\\WBEM\\CIMOM")
#define WBEM_LOGGON_DIR		_T("Logging Directory")

const static CString BSLASH(_T("\\"));
const static CString HKLM_EVENTLOG(_T("SYSTEM\\CurrentControlSet\\Services\\Eventlog"));
const static CString EVENTLOG_FILE(		_T("File"));

#define HKLM_IIS_LOG		_T("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\W3SVC\\Parameters")
#define LOG_DIRECTORY		_T("LogFileDirectory")

#define HKLM_DNS_PARAMS		_T("SYSTEM\\CurrentControlSet\\Services\\DNS\\Parameters")
#define DNS_LOGFILE			_T("LogFilePath")

#define HKLM_HIVE_INJ_SERVICE	_T("SYSTEM\\CurrentControlSet\\Services\\HiveInjectionService")
#define SERVICE_START_TYPE			_T("Start")
#define START_AUTO				2
#define START_DEACTIVE			4

// TODO! RKE: Internet Information Server Zugriff auf CF prüfen!
#define HKCU_SHELL_FOLDERS	_T("Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders")
// HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\User Shell Folders
// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Internet Settings\Cache\Paths
#define SF_CACHE				_T("Cache")			// Temporary Internet Files
#define SF_CD_BURNING			_T("CD Burning")	// CD Burning
#define SF_HISTORY				_T("History")		// History

// interessant
// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved

#define PROFILE_CHECKDISK					_T("CheckDisk")
#define PROFILE_CHECKAFTERREBOOT			_T("CheckAfterReboot")
#define PROFILE_FIXERRORS					_T("FixErrors")
#define PROFILE_SHOWADDINFO					_T("ShowAddInfo")
#define PROFILE_LOCATEANDRECOVERBADSECTORS	_T("LocateAndRecoverBadSectors")
#define PROFILE_FORCEDISMOUNTING			_T("ForceDismounting")
#define PROFILE_CHECKINDEXENTRIES			_T("CheckIndexEntries")
#define PROFILE_CHECKCYCLICFOLDERSTRUCT		_T("CheckCyclicFolderStruct")

#define PROFILE_PAGEFILESIZE				_T("PageFileSize")
#define	PROFILE_PRODUCT_TYPE		        _T("ProductType")
#define	PROFILE_CREATEAUTOPAGEFILE			_T("CreateAutoPageFile")

enum ePathes
{
    OSPI_PAGEFILE=0,
    OSPI_TEMP,
    OSPI_LOG,
    OSPI_PROTOCOL,
    OSPI_LOG_CONFIG,
    OSPI_LOG_DNS,
    OSPI_OUT,
    OSPI_DVRTWWW,
    OSPI_BACKUP,
    OSPI_LOG_IIS,
    OSPI_BACKUP_DV,
    OSPI_BACKUP_SECURITY,
    OSPI_BACKUP_DVRTWWW,
    OSPI_VE5900_PROGRAM
};

CStringMap::CStringMap()
{
}

const CString& CStringMap::operator[](const int pos)
{
    return get_value(pos);
}

const CString& CStringMap::get_value(const int pos) const
{
    const_iterator it = mMap.find(pos);
	if (it != mMap.end())
        return it->second;
    else
    {
        static CString empty;
        return empty;
    }
}

CString& CStringMap::set_value(int pos)
{
    return mMap[pos];
}

void CStringMap::set_value(int  pos, const CString& value)
{
    mMap[pos] = value;
}

void CStringMap::clear()
{
    mMap.clear();
}

CStringMap::iterator CStringMap::get_pos(int pos)
{
    return mMap.find(pos);
}
CStringMap::iterator CStringMap::begin()
{
    return mMap.begin();
}
CStringMap::iterator CStringMap::end()
{
    return mMap.end();
}

////////////////////////////////////////////////////////////////////////////////////////
// CVideteCheckDiskDlg Dialogfeld
CVideteCheckDiskDlg::CVideteCheckDiskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideteCheckDiskDlg::IDD, pParent)
{
	m_bAutoMode		 = FALSE;
	m_bAutoCheckDisk = FALSE;
	m_bAutoEnableEwf = FALSE;
	m_bReboot		 = FALSE;
	m_hIcon			 = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pProcess		 = NULL;
	m_dwSelfCheck	 = 0;
	m_pInfoDlg		 = NULL;
	m_nDisks		 = 0;
	m_nLinesCounted	 = 0;
	m_nTimer		 = 0;
	m_bCloseDialog	 = FALSE;
	m_nCFDisk		 = 0;
	m_nCFPartition	 = 1;
	m_bCheckRegKeys		 = FALSE;
	m_pDelete		 = NULL;

	m_bCheckAfterReboot			  = theApp.GetProfileInt(PROFILE_CHECKDISK, PROFILE_CHECKAFTERREBOOT, FALSE);
	m_bFixErrors				  = theApp.GetProfileInt(PROFILE_CHECKDISK, PROFILE_FIXERRORS, FALSE);
	m_bLocateAndRecoverBadSectors = theApp.GetProfileInt(PROFILE_CHECKDISK, PROFILE_LOCATEANDRECOVERBADSECTORS, FALSE);
	m_bForceDismounting			  = theApp.GetProfileInt(PROFILE_CHECKDISK, PROFILE_FORCEDISMOUNTING, FALSE);		
	m_bShowAddInfo				  = theApp.GetProfileInt(PROFILE_CHECKDISK, PROFILE_SHOWADDINFO, TRUE);				
	m_bCheckIndexEntries		  = theApp.GetProfileInt(PROFILE_CHECKDISK, PROFILE_CHECKINDEXENTRIES, TRUE);	
	m_bCheckCyclicFolderStruct    = theApp.GetProfileInt(PROFILE_CHECKDISK, PROFILE_CHECKCYCLICFOLDERSTRUCT, TRUE);

	m_bCompactFlash		  = theApp.GetProfileInt(PROFILE_COMMON, COMMON_COMPACT_FLASH, FALSE);
	m_nPageFileSize		  = theApp.GetProfileInt(PROFILE_COMMON, PROFILE_PAGEFILESIZE, 0);
    m_nProductType        = theApp.GetProfileInt(PROFILE_COMMON, PROFILE_PRODUCT_TYPE, VSWT_VE5900);;
	m_bCreateAutoPageFile = theApp.GetProfileInt(PROFILE_COMMON, PROFILE_CREATEAUTOPAGEFILE, FALSE);
	m_sDontCheck		  = theApp.GetProfileString(PROFILE_COMMON, PROFILE_DONTCHECK, _T("c:"));
	CWK_Profile wkpDVRT;
	m_bTraceAllOutputStrings = wkpDVRT.GetInt(_T("Debug"), _T("TraceVCDOutputStrings"), -1);
	if (m_bTraceAllOutputStrings == -1)
	{
		m_bTraceAllOutputStrings = TRUE;
		wkpDVRT.WriteInt(_T("Debug"), _T("TraceVCDOutputStrings"), m_bTraceAllOutputStrings);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
CVideteCheckDiskDlg::~CVideteCheckDiskDlg()
{
	WK_DELETE(m_pInfoDlg);
	WK_DELETE(m_pProcess);
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDT_DEBUG_OUT, m_edtOutput);
    DDX_Control(pDX, IDC_PROGRESS_PERCENT, m_cProgress);
    DDX_Check(pDX, IDC_CK_CHECK_AFTER_REBOOT, m_bCheckAfterReboot);
    DDX_Check(pDX, IDC_CK_FIX_ERRORS, m_bFixErrors);
    DDX_Check(pDX, IDC_CK_LOCATE_AND_RECOVER_BAD_SECTORS, m_bLocateAndRecoverBadSectors);
    DDX_Check(pDX, IDC_CK_FORCE_DISMOUNT, m_bForceDismounting);
    DDX_Check(pDX, IDC_CK_CHECK_INDEX_ENTRIES, m_bCheckIndexEntries);
    DDX_Check(pDX, IDC_CK_CHECK_CYCLIC_FOLDER_STRUCTS, m_bCheckCyclicFolderStruct);
    DDX_Text(pDX, IDC_EDT_CHECK_DRIVES, m_sCheckDrives);
    DDX_Text(pDX, IDC_EDT_DONT_CHECK_DRIVES, m_sDontCheck);
    DDX_Control(pDX, IDC_TAB, m_TabCtrl);
    DDX_Check(pDX, IDC_CK_COMPACT_FLASH, m_bCompactFlash);
    DDX_Text(pDX, IDC_EDT_PAGEFILE_SIZE, m_nPageFileSize);
    DDV_MinMaxInt(pDX, m_nPageFileSize, 0, 1024);
    DDX_Text(pDX, IDC_EDT_CF_DISK, m_nCFDisk);
    DDV_MinMaxInt(pDX, m_nCFDisk, 0, 30);
    DDX_Text(pDX, IDC_EDT_CF_PARTITION, m_nCFPartition);
    DDV_MinMaxInt(pDX, m_nCFPartition, 1, 10);
    DDX_CBIndex(pDX, IDC_COMBO_PRODUCT_TYPE, m_nProductType);
    DDX_Check(pDX, IDC_CK_AUTO_PAGE_FILE, m_bCreateAutoPageFile);
    if (pDX->m_bSaveAndValidate)
    {
        m_sDontCheck.MakeLower();
    }
}
////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CVideteCheckDiskDlg, CDialog)
	//{{AFX_MSG_MAP(CVideteCheckDiskDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CREATE_DISKS, OnBnClickedCreateDisks)
	ON_BN_CLICKED(IDC_CHECK_DISKS, OnBnClickedCheckDisks)
	ON_BN_CLICKED(IDC_CREATE_PATHS, OnBnClickedCreatePaths)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnTcnSelchangeTab)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
	ON_BN_CLICKED(IDC_BTN_SAVE_SETTINGS, OnBnClickedBtnSaveSettings)
	ON_BN_CLICKED(IDC_CK_LOCATE_AND_RECOVER_BAD_SECTORS, OnBnClickedCkLocateAndRecoverBadSectors)
	ON_BN_CLICKED(IDC_CK_FORCE_DISMOUNT, OnBnClickedCkForceDismount)
	ON_BN_CLICKED(IDC_BTN_CHECK_CF_REG, OnBnClickedBtnCheckCfReg)
	ON_BN_CLICKED(IDC_BTN_CLEAR_OUTPUT_WINDOW, OnBnClickedBtnClearOutputWindow)
	ON_BN_CLICKED(IDC_CK_COMPACT_FLASH, OnBnClickedCkCompactFlash)
	ON_BN_CLICKED(IDC_CK_CHECK_AFTER_REBOOT, OnChanged)
	ON_BN_CLICKED(IDC_CK_CHECK_CYCLIC_FOLDER_STRUCTS, OnChanged)
	ON_BN_CLICKED(IDC_CK_CHECK_INDEX_ENTRIES, OnChanged)
	ON_BN_CLICKED(IDC_CK_FIX_ERRORS, OnChanged)
	ON_BN_CLICKED(IDC_CK_AUTO_PAGE_FILE, OnChanged)
    ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT_TYPE, OnChanged)
	ON_EN_CHANGE(IDC_EDT_CF_DISK, OnChanged)
	ON_EN_CHANGE(IDC_EDT_CF_PARTITION, OnChanged)
	ON_EN_CHANGE(IDC_EDT_DONT_CHECK_DRIVES, OnChanged)
	ON_EN_CHANGE(IDC_EDT_PAGEFILE_SIZE, OnChanged)
	ON_MESSAGE(WM_USER, OnUser)
	ON_MESSAGE(WM_USER_PROCESS, OnUserProcess)
	ON_MESSAGE(WM_SELFCHECK, OnSelfCheck)
	ON_EN_CHANGE(IDC_EDT_CHECK_DRIVES, OnEnChangeEdtCheckDrives)
	ON_BN_CLICKED(IDC_BTN_CHECK_PATHES, OnBnClickedBtnCheckPathes)
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BTN_RESTORE_PATHES, OnBnClickedBtnRestorePathes)
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////////////////
// CVideteCheckDiskDlg Meldungshandler
BOOL CVideteCheckDiskDlg::OnInitDialog()
{
	if (m_bAutoEnableEwf == TRUE && m_bCompactFlash != TRUE)
	{
		m_bCompactFlash = TRUE;
	}
	else if (m_bAutoEnableEwf == DISABLE && m_bCompactFlash != FALSE)
	{
		m_bCompactFlash = FALSE;
	}
	else
	{
		m_bAutoEnableEwf = FALSE;
	}
	CDialog::OnInitDialog();
	SetWindowText(WK_APP_NAME_VCD);
    InitPathes();

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString str;
		str.LoadString(IDS_ABOUTBOX);
		if (!str.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, str);
		}
		str.LoadString(IDS_HELP);
		if (!str.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_STRING, IDM_HELP, str);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	m_FixedFont.CreatePointFont(9*10, _T("Courier New"));
	CString str;
	str.LoadString(IDS_OUTPUT_TAB);
	m_TabCtrl.InsertItem(0, str);
	str.LoadString(IDS_SETTINGS_TAB);
	m_TabCtrl.InsertItem(1, str);
	
	OnBnClickedCkCompactFlash();
	if (m_bAutoMode || m_bAutoCheckDisk || m_bAutoEnableEwf)
	{
		if (m_pInfoDlg == NULL)
		{
			m_pInfoDlg = new CInfoDlg;
		}
		CString str;
		m_pInfoDlg->Create();
		GetWindowText(str);
		SetInfoText(str);
		if (m_bAutoEnableEwf)
		{
			OnBnClickedCkCompactFlash();
			PostMessage(WM_COMMAND, IDC_BTN_CHECK_CF_REG);
		}
		else if (m_bAutoMode)
		{
			PostMessage(WM_COMMAND, IDC_CREATE_DISKS);
		}
		else if (m_bAutoCheckDisk)
		{
			PostMessage(WM_COMMAND, IDC_CHECK_DISKS);
		}
		m_nTimer = SetTimer(1000, 500, NULL);
	}
	else
	{
		m_cProgress.SetRange(0, 100);

		CRect rcTab, rcOut;
		m_TabCtrl.GetWindowRect(&rcTab);
		ScreenToClient(rcTab);
		m_edtOutput.GetWindowRect(&rcOut);
		ScreenToClient(rcOut);
		rcOut.top = rcTab.bottom;

		m_edtOutput.MoveWindow(&rcOut);
		m_edtOutput.SetFont(&m_FixedFont);
		SetDebuggerWindowHandle(m_edtOutput.m_hWnd);

		EnableToolTips();
		PostMessage(WM_USER, WM_USER_SET_TAB, SETTINGS_TAB);
		SetFirstFixedDriveToCheckEdt();

#ifdef _DEBUG
		//if (m_pInfoDlg == NULL)
		//{
		//	m_pInfoDlg = new CInfoDlg;
		//}
		//m_pInfoDlg->Create(this);
#endif
	}

	GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(FALSE);

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}


void CVideteCheckDiskDlg::InitPathes()
{
    BOOL bCreateDisks = FALSE;
    m_Pathes.clear();
    m_Pathes.set_value(OSPI_PAGEFILE,       _T("pagefile.sys"));    // Pagefile
    m_Pathes.set_value(OSPI_TEMP,           _T("Temp"));            // Temporary files
    m_Pathes.set_value(OSPI_LOG_CONFIG,     _T("Log\\config"));     // Event Log

    if (m_nProductType == VSWT_VE5900)
    {
        m_Pathes.set_value(OSPI_VE5900_PROGRAM, _T("Run_5900"));    // VE5900 program path
    }

    if (   m_nProductType == VSWT_IDIP 
        || m_nProductType == VSWT_DTS)
    {
        m_Pathes.set_value(OSPI_LOG,            _T("Log"));             // idip / dvrt log
        m_Pathes.set_value(OSPI_PROTOCOL,       _T("protocol"));        // idip / dvrt protokoll log
        m_Pathes.set_value(OSPI_OUT,            _T("Out"));             // idip / dvrt output dir
        m_Pathes.set_value(OSPI_DVRTWWW,        _T("dvrtwww"));         // idip / dvrt html maps
        m_Pathes.set_value(OSPI_BACKUP,         _T("backup"));          // idip / dvrt backup
        m_Pathes.set_value(OSPI_BACKUP_DV,      _T("backup\\DV"));      // dvrt backup
        m_Pathes.set_value(OSPI_BACKUP_SECURITY,_T("backup\\Security"));// idip backup
        m_Pathes.set_value(OSPI_BACKUP_DVRTWWW, _T("backup\\dvrtwww")); // idip / dvrt html maps backup 
        m_Pathes.set_value(OSPI_LOG_DNS,        _T("Log\\Dns"));        // DNS debug log
        m_Pathes.set_value(OSPI_LOG_IIS,        _T("Log\\IIS"));        // internet information service log
        bCreateDisks = TRUE;
    }

	GetDlgItem(IDC_CREATE_DISKS)->EnableWindow(bCreateDisks);
}

////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch ((nID & 0xFFF0))
	{
		case IDM_ABOUTBOX:
			theApp.DisplayInfo();
			break;
		case IDM_HELP:
			theApp.DisplayHelp();
			break;
		default:
			CDialog::OnSysCommand(nID, lParam);
			break;
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnPaint() 
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
////////////////////////////////////////////////////////////////////////////////////////
// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CVideteCheckDiskDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedCancel()
{
	int nReturn = AfxMessageBox(m_pProcess != NULL ? IDS_REQUEST_CANCEL:IDS_REQUEST_CLOSE, MB_YESNO|MB_ICONQUESTION);
	if (nReturn == IDYES)
	{
		if (m_pProcess)
		{
			CString sCanceled;
			sCanceled.LoadString(IDS_CANCELED);
			WK_TRACE(_T("%s\n"),  CString(m_pProcess->GetName()) + _T(" ") + m_pProcess->GetCmdLine() + _T(" ") + sCanceled);
			delete m_pProcess;
			m_pProcess = NULL;
		}
		else
		{
			OnCancel();
			DestroyWindow();
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedOk()
{
	if (m_bAutoMode || m_bAutoCheckDisk || m_bAutoEnableEwf)
	{	// automatisch speichern
		theApp.m_nReturnCode = m_bReboot ? 2 : IDOK;
		if (GetDlgItem(IDC_BTN_SAVE_SETTINGS)->IsWindowEnabled())
		{
			OnBnClickedBtnSaveSettings();
		}
	}
	else
	{
		if (m_bCompactFlash)
		{
			CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));
			CString sVolumeID = wkpSystem.GetString(HKLM_EWF_VOLUME, EWF_PARAM_VOLUME_ID, NULL);
			if (sVolumeID.IsEmpty())
			{
				OnBnClickedBtnCheckCfReg();
			}
		}
		if (GetDlgItem(IDC_BTN_SAVE_SETTINGS)->IsWindowEnabled())
		{
			if (IDYES==AfxMessageBox(IDS_ASK_FOR_SAVE_SETTINGS, MB_ICONQUESTION|MB_YESNO))
			{
				OnBnClickedBtnSaveSettings();
			}
		}
		if (m_bReboot)
		{
			int nRet = AfxMessageBox(IDS_ASK_FOR_REBOOT, MB_ICONQUESTION|MB_YESNO);
			if (nRet == IDYES)
			{
				DoExitWindows(EWX_REBOOT);
			}
		}
	}
	if (m_bCompactFlash)
	{
		CEwf ewf(m_sDontCheck.GetAt(0));
		ewf.Commit();
	}
	OnOK();
	DestroyWindow();
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedBtnCheckCfReg()
{
	if (UpdateData())
	{
		SwitchToTab(OUTPUT_TAB);

		WK_TRACE(_T("Checking CF Registry Settings:\n"));
		CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));
		CWK_Profile wkpUser(CWK_Profile::WKP_ABSOLUT, HKEY_CURRENT_USER,_T(""),_T(""));
		CWK_Profile wkpDVRT;
		int nAutoLayout = wkpSystem.GetInt(HKLM_OPTIMAL_LAYOUT, ENABLE_AUTO_LAYOUT, -1);
		WK_TRACE(_T("%s\\%s='%d'\n"), HKLM_OPTIMAL_LAYOUT, ENABLE_AUTO_LAYOUT, nAutoLayout);

		// This registry key disables the Windows XP Embedded background disk 
		// defragmentation service.
        CString sAutoLayoutBackup = ENABLE_AUTO_LAYOUT + BACKUP;
		if (wkpSystem.GetInt(HKLM_OPTIMAL_LAYOUT, sAutoLayoutBackup, -2) == -2)
		{
			wkpSystem.WriteInt(HKLM_OPTIMAL_LAYOUT, sAutoLayoutBackup, nAutoLayout);
		}

		int nErrorControl = wkpSystem.GetInt(HKLM_EWF, EWFS_ERROR_CONTROL, -1);
		WK_TRACE(_T("%s\\%s='%d'\n"), LPCTSTR(HKLM_EWF), LPCTSTR(EWFS_ERROR_CONTROL), nErrorControl);

		CString sGroup = wkpSystem.GetString(HKLM_EWF, EWFS_GROUP, NULL);
		WK_TRACE(_T("%s\\%s='%s'\n"), LPCTSTR(HKLM_EWF), LPCTSTR(EWFS_GROUP), sGroup);

		int nStart = wkpSystem.GetInt(HKLM_EWF, EWFS_START, -1);
		WK_TRACE(_T("%s\\%s='%d'\n"), LPCTSTR(HKLM_EWF), LPCTSTR(EWFS_START), nStart);

		int nType = wkpSystem.GetInt(HKLM_EWF, EWFS_TYPE, -1);
		WK_TRACE(_T("%s\\%s='%d'\n"), LPCTSTR(HKLM_EWF), LPCTSTR(EWFS_TYPE), nType);

		CStringArray saFilters, saBackup;
		CString sFilters, sUpperFiltersBackup;
		wkpSystem.GetMultiString(HKLM_CLASS_VOLUME, CLASS_VOLUME_UF, saFilters);
        ConcatenateStrings(saFilters, sFilters, _T(','));
		WK_TRACE(_T("%s\\%s='%s'\n"), HKLM_CLASS_VOLUME, CLASS_VOLUME_UF, sFilters);

		sUpperFiltersBackup = CLASS_VOLUME_UF + BACKUP;
		BOOL bUpperFiltersBackup = wkpSystem.GetMultiString(HKLM_CLASS_VOLUME, sUpperFiltersBackup, saBackup);
		if (bUpperFiltersBackup == FALSE)
		{
			bUpperFiltersBackup = wkpSystem.WriteMultiString(HKLM_CLASS_VOLUME, sUpperFiltersBackup, saFilters);
			saBackup.RemoveAll();
			saBackup.Append(saFilters);
		}

		CString sVolumeID = wkpSystem.GetString(HKLM_EWF_VOLUME, EWF_PARAM_VOLUME_ID, NULL);
		WK_TRACE(_T("%s\\%s='%s'\n"), LPCTSTR(HKLM_EWF_VOLUME), LPCTSTR(EWF_PARAM_VOLUME_ID), sVolumeID);

		int nEwfVolumeType = wkpSystem.GetInt(HKLM_EWF_VOLUME, EWF_PARAM_TYPE, -1);
		WK_TRACE(_T("%s\\%s='%d'\n"), LPCTSTR(HKLM_EWF_VOLUME), LPCTSTR(EWF_PARAM_TYPE), nEwfVolumeType);

		CString sArcName = wkpSystem.GetString(HKLM_EWF_VOLUME, EWF_PARAM_ARCNAME, NULL);
		WK_TRACE(_T("%s\\%s='%s'\n"), LPCTSTR(HKLM_EWF_VOLUME), LPCTSTR(EWF_PARAM_ARCNAME), sArcName);

		int nRet = IDCANCEL;
		if (m_bAutoEnableEwf == TRUE)
		{
			CString str;
			str.Format(_T("Enable EWF on CF drive %c:\\"), m_sDontCheck.GetAt(0));
			SetInfoText(str);
			nRet = IDYES;
		}
		else if (m_bAutoEnableEwf == DISABLE)
		{
			SetInfoText(_T("Disable EWF"));
			nRet = IDNO;
		}
		else
		{
			nRet = AfxMessageBox(IDS_ASK_INSTALL_EWF, MB_ICONQUESTION|MB_YESNOCANCEL);
		}
		if (nRet == IDYES)
		{
			CString sNewVolumeID, sNewArcName;

			sNewVolumeID = _T("{1EA414D1-6760-4625-8CBE-4F9F85A48E15}");
			sNewArcName.Format(_T("multi(0)disk(%d)rdisk(0)partition(%d)"), m_nCFDisk, m_nCFPartition);
			
			if (   sNewVolumeID != sVolumeID 
				|| sNewArcName  != sArcName
				|| wkpSystem.GetType(HKLM_EWF_VOLUME, EWF_PARAM_TYPE) != REG_DWORD)
			{
				WK_TRACE(_T("Applying CF Registry Settings:\n"));
				wkpSystem.WriteString(HKLM_EWF_VOLUME, EWF_PARAM_VOLUME_ID, sNewVolumeID);
				wkpSystem.WriteString(HKLM_EWF_VOLUME, EWF_PARAM_ARCNAME, sNewArcName);
				sVolumeID = sNewVolumeID;
				sArcName = sNewArcName;

				nAutoLayout = 0;
				wkpSystem.WriteInt(HKLM_OPTIMAL_LAYOUT, ENABLE_AUTO_LAYOUT, nAutoLayout);

				nEwfVolumeType = 1;
				wkpSystem.WriteInt(HKLM_EWF_VOLUME, EWF_PARAM_TYPE, nEwfVolumeType);

				saFilters.RemoveAll();
				saFilters.Add(_T("Ewf"));
				wkpSystem.WriteMultiString(HKLM_CLASS_VOLUME, CLASS_VOLUME_UF, saFilters);
				sFilters = saFilters.GetAt(0);

				CString sHelp(theApp.m_pszHelpFilePath);
				int nFind = sHelp.ReverseFind(_T('\\'));
				if (nFind != -1)
				{
					sHelp = sHelp.Left(nFind+1) + EWF_CONTROL_APPLICATION_EXE;
				}
				wkpSystem.WriteString(HKLM_RUN, EWF_CONTROL_APPLICATION, sHelp);
				WK_TRACE(_T("Setting Run variable: %s(%s)\n"), EWF_CONTROL_APPLICATION, sHelp);
				m_bReboot = TRUE;
			}
			else
			{
				WK_TRACE(_T("CF Registry Settings already applied\n"));
				nRet = IDCANCEL;
			}
		}
		else if (nRet == IDNO)
		{
			if (   wkpSystem.GetType(HKLM_EWF_VOLUME, EWF_PARAM_VOLUME_ID) == REG_SZ
				|| wkpSystem.GetType(HKLM_EWF_VOLUME, EWF_PARAM_ARCNAME  ) == REG_SZ
				|| wkpSystem.GetType(HKLM_EWF_VOLUME, EWF_PARAM_TYPE     ) == REG_DWORD)
			{
				wkpSystem.DeleteEntry(HKLM_EWF_VOLUME, EWF_PARAM_VOLUME_ID);
				wkpSystem.DeleteEntry(HKLM_EWF_VOLUME, EWF_PARAM_TYPE);
				wkpSystem.DeleteEntry(HKLM_EWF_VOLUME, EWF_PARAM_ARCNAME);
				sVolumeID.Empty();
				sArcName.Empty();

				WK_TRACE(_T("Deleting and restoring CF Registry Settings:\n"));
				nAutoLayout = wkpSystem.GetInt(HKLM_OPTIMAL_LAYOUT, sAutoLayoutBackup, -2);
				if (nAutoLayout == -1)
				{
					wkpSystem.DeleteEntry(HKLM_OPTIMAL_LAYOUT, ENABLE_AUTO_LAYOUT);
				}
				else if (nAutoLayout != -2)
				{
					wkpSystem.WriteInt(HKLM_OPTIMAL_LAYOUT, ENABLE_AUTO_LAYOUT, nAutoLayout);
				}
				if (bUpperFiltersBackup)
				{
					wkpSystem.WriteMultiString(HKLM_CLASS_VOLUME, CLASS_VOLUME_UF, saBackup);
					sFilters = saBackup.GetAt(0);
				}
				wkpSystem.DeleteEntry(HKLM_RUN, EWF_CONTROL_APPLICATION);
				m_bReboot = TRUE;
			}
			else
			{
				WK_TRACE(_T("CF Registry Settings already deleted\n"));
				nRet = IDCANCEL;
			}
		}
		if (nRet != IDCANCEL)
		{
			WK_TRACE(_T("%s\\%s='%d'\n"), HKLM_OPTIMAL_LAYOUT, ENABLE_AUTO_LAYOUT, nAutoLayout);
			WK_TRACE(_T("%s\\%s='%s'\n"), LPCTSTR(HKLM_EWF_VOLUME), LPCTSTR(EWF_PARAM_VOLUME_ID), sVolumeID);
			WK_TRACE(_T("%s\\%s='%d'\n"), LPCTSTR(HKLM_EWF_VOLUME), LPCTSTR(EWF_PARAM_TYPE), nEwfVolumeType);
			WK_TRACE(_T("%s\\%s='%s'\n"), LPCTSTR(HKLM_EWF_VOLUME), LPCTSTR(EWF_PARAM_ARCNAME), sArcName);
			WK_TRACE(_T("%s\\%s='%s'\n"), HKLM_CLASS_VOLUME, CLASS_VOLUME_UF, sFilters);
		}
	}
	if (m_bAutoMode)
	{
		PostMessage(WM_COMMAND, IDC_CREATE_DISKS);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedBtnClearOutputWindow()
{
	m_edtOutput.SetWindowText(_T(""));
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedBtnSaveSettings()
{
	if (UpdateData())
	{
		theApp.WriteProfileInt(PROFILE_CHECKDISK, PROFILE_CHECKAFTERREBOOT, m_bCheckAfterReboot);
		theApp.WriteProfileInt(PROFILE_CHECKDISK, PROFILE_FIXERRORS, m_bFixErrors);
		theApp.WriteProfileInt(PROFILE_CHECKDISK, PROFILE_SHOWADDINFO, m_bShowAddInfo);
		theApp.WriteProfileInt(PROFILE_CHECKDISK, PROFILE_LOCATEANDRECOVERBADSECTORS, m_bLocateAndRecoverBadSectors);
		theApp.WriteProfileInt(PROFILE_CHECKDISK, PROFILE_FORCEDISMOUNTING, m_bForceDismounting);
		theApp.WriteProfileInt(PROFILE_CHECKDISK, PROFILE_CHECKINDEXENTRIES, m_bCheckIndexEntries);
		theApp.WriteProfileInt(PROFILE_CHECKDISK, PROFILE_CHECKCYCLICFOLDERSTRUCT, m_bCheckCyclicFolderStruct);

		theApp.WriteProfileString(PROFILE_COMMON, PROFILE_DONTCHECK, m_sDontCheck);
		theApp.WriteProfileInt(PROFILE_COMMON, COMMON_COMPACT_FLASH, m_bCompactFlash);
		theApp.WriteProfileInt(PROFILE_COMMON, PROFILE_PAGEFILESIZE, m_nPageFileSize);
		theApp.WriteProfileInt(PROFILE_COMMON, PROFILE_PRODUCT_TYPE, m_nProductType);
		theApp.WriteProfileInt(PROFILE_COMMON, PROFILE_CREATEAUTOPAGEFILE, m_bCreateAutoPageFile);
		GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(FALSE);

		CWK_Profile wkpDVRT;
		if (m_bCompactFlash)
		{
			wkpDVRT.WriteString(SECTION_COMMON, COMMON_COMPACT_FLASH, m_sDontCheck.Left(2));
		}
		else
		{
			wkpDVRT.DeleteEntry(SECTION_COMMON, COMMON_COMPACT_FLASH);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedCreateDisks()
{
	CString sSystem;
	GetSystemDirectory(sSystem.GetBufferSetLength(MAX_PATH), MAX_PATH);
	sSystem.ReleaseBuffer();
	sSystem = sSystem + _T("\\diskpart.exe");
	if (DoesFileExist(sSystem))
	{
		WK_TRACE(_T("Creating Disks:\n"));
		SwitchToTab(OUTPUT_TAB);
		CString  sFmt;
		CStringArray saUnmountedVolumes;
		DWORD dwFlags, dwMask=1, dwLogical = GetLogicalDrives();
		int i;				

		m_saFreeDrives.RemoveAll();
		m_saAssigned.RemoveAll();
		m_sFormating.Empty();
		// list the available disks
		for (i=3,dwMask=8; i<26; i++,dwMask<<=1)
		{
			sFmt.Format(_T("%c:"), i+_T('a'));
			if (!(dwMask & dwLogical))
			{
				m_saFreeDrives.Add(sFmt);
			}
			else
			{
#ifdef _DEBUG
				_TCHAR szVolumName[MAX_PATH] = _T("");
				_TCHAR szRegKey[MAX_PATH] = _T("");
				_TCHAR szDevName[MAX_PATH] = _T("");
				
				GetVolumeNamesAndKeys(sFmt + _T("\\"), szVolumName, szRegKey, szDevName);
#endif
				UINT nType = GetDriveType(sFmt);
				if (   nType == DRIVE_REMOVABLE
					&& i < 11)	// d: + 7 = k:
				{
					sFmt += _T("\\");
					_TCHAR szVolume[MAX_PATH];
					if (GetVolumeNameForVolumeMountPoint(sFmt, szVolume, MAX_PATH))
					{
						saUnmountedVolumes.Add(szVolume);
						DeleteVolumeMountPoint(sFmt);
						m_saFreeDrives.Add(sFmt);
					}
					else
					{
						DWORD dwError = GetLastError();
						WK_TRACE(_T("Error: \n"), dwError);
					}
				}
			}
		}

		for (i=0; i<saUnmountedVolumes.GetSize(); i++)
		{
			int nFree = m_saFreeDrives.GetSize();
			if (nFree)
			{
				CString sFree = m_saFreeDrives.GetAt(nFree-1);
				sFree += _T("\\");
				SetVolumeMountPoint(sFree, saUnmountedVolumes.GetAt(i));
				m_saFreeDrives.RemoveAt(nFree-1);
			}
		}
		
		WK_DELETE(m_pProcess);
		// open diskpart console
		dwFlags = WKCPF_INPUT|WKCPF_OUTPUT|WKCPF_OUTPUT_CHECK|WKCPF_TERMINATE;
		m_pProcess = new CWKControlledProcess(DISKPART_APP, NULL, NULL, dwFlags);
		m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, DISKPART_CONSOLE_OPEN, 0);
		m_pProcess->StartThread();
	}
	else
	{
		CString str;
		str.Format(IDS_PROCESS_ERROR, _T("Diskpart"));
		SetInfoText(str);
		WK_TRACE_ERROR(_T("File not found %s\n"), sSystem);
		PartitionAndFormatFinished();
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedCheckDisks()
{
	if (UpdateData())
	{
		WK_TRACE(_T("Checking Disks:\n"));
		SwitchToTab(OUTPUT_TAB);
		CString  sFmt;
		DWORD dwFlags, dwMask=1, dwLogical = GetLogicalDrives();
		int i;				// list the available disks
		m_saAssigned.RemoveAll();
		m_sFinnished.Empty();
		if (m_sCheckDrives.IsEmpty())
		{
 			for (i=2,dwMask=4; i<26; i++,dwMask<<=1)
			{
				if (dwMask & dwLogical)
				{
					sFmt.Format(_T("%c:"), i+'a');
					int nType = GetDriveType(sFmt);
					if (nType == DRIVE_FIXED && m_sDontCheck.Find(sFmt) == -1)
					{
						m_saAssigned.Add(sFmt);
					}
				}
			}
		}
		else
		{
			SplitString(m_sCheckDrives, m_saAssigned, _T(','));
		}
		WK_DELETE(m_pProcess);
		if (m_saAssigned.GetCount())
		{
			dwFlags = WKCPF_INPUT|WKCPF_OUTPUT|WKCPF_OUTPUT_CHECK|WKCPF_TERMINATE;
			m_pProcess = new CWKControlledProcess(_T("cmd.exe"), NULL, NULL, dwFlags);
			m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, CHKDSK_CMD_CONSOLE_OPEN, 0);
			m_pProcess->StartThread();
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedCreatePaths()
{
	CWK_Profile wkpDVRT;
	CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE,_T(""),_T(""));
	int i;				

	m_dwSelfCheck = 2;
	m_saAssigned.RemoveAll();

	if (UpdateData())
	{
        InitPathes();
		SwitchToTab(OUTPUT_TAB);
		SetInfoText(IDS_CHECKING_PATHES);
		CString  sFmt;	
		DWORD dwMask=1, dwLogical = GetLogicalDrives();
		if (m_sCheckDrives.IsEmpty())	// list the available disks
		{
 			for (i=2,dwMask=4; i<26; i++,dwMask<<=1)
			{
				if (dwMask & dwLogical)
				{
					sFmt.Format(_T("%c:\\"), i+'a');
					int nType = GetDriveType(sFmt);
					int nFind = m_sDontCheck.Find(sFmt.Left(2));
					if (nType == DRIVE_FIXED && nFind == -1)
					{					// only fixed drives which are allowed
						m_saAssigned.Add(sFmt);
					}
				}
			}
		} 
		else	// take the user defined disks
		{
			SplitString(m_sCheckDrives, m_saAssigned, _T(','));
			for (i=0; i<m_saAssigned.GetCount(); i++)
			{
				sFmt = m_saAssigned.GetAt(i);
				sFmt += _T("\\");
				m_saAssigned.SetAt(i, sFmt);
			}
		}
	}
	else
	{
		return ;
	}


	CString sRoot, sPath;
	BOOL bCreate = FALSE;
	BOOL bAllOK = FALSE;
	BOOL bEnableHive = FALSE;

	if (m_bCheckRegKeys)
	{
		WK_TRACE(_T("Checking Pathes:\n"));
	}
	else
	{
		WK_TRACE(_T("Creating Pathes:\n"));
		// check wether previously defined pathes are on a fixed drive
	}

	// check the pathes
	for (i=0; i<m_saAssigned.GetCount(); i++)
	{
		sRoot = m_saAssigned.GetAt(i);
        sPath = sRoot + m_Pathes[OSPI_PAGEFILE];
		if (DoesFileExist(sPath)==TRUE)
		{
			WK_TRACE(_T("Pagefile %s is existing\n"), sPath);
			bAllOK = TRUE;
            CStringMap::iterator it = m_Pathes.get_pos(OSPI_TEMP);
            while (it != m_Pathes.end())
			{
                sPath = sRoot + it->second;
				if (DoesFileExist(sPath) != DFE_IS_DIRECTORY)
				{
					WK_TRACE(_T("%s is not existing\n"), sPath);
					bAllOK = FALSE;
					if (!m_bCheckRegKeys)
					{
						bCreate = TRUE;
                        switch (it->first)
						{
							case OSPI_TEMP:
							case OSPI_LOG_CONFIG:
							case OSPI_LOG_DNS:
								m_bReboot = TRUE;
								break;
							case OSPI_BACKUP_SECURITY:
							case OSPI_BACKUP_DVRTWWW:
							if (m_nProductType == VSWT_IDIP)
							{
								m_bReboot = TRUE;
								break;
							}
							case OSPI_BACKUP_DV:
							if (m_nProductType == VSWT_DTS)
							{
								m_bReboot = TRUE;
								break;
							}
						}
					}
				}
				else
				{
					WK_TRACE(_T("%s is existing\n"), sPath);
				}
				++it;
			}
			if (bAllOK)
			{
				bEnableHive = TRUE;
				WK_TRACE(_T("All Pathes OK on %s\n"), sRoot);
				break;
			}
			if (bCreate)
			{
				bEnableHive = TRUE;
				WK_TRACE(_T("Repairing Pathes on %s\n"), sRoot);
				break;
			}
		}
		else
		{
			WK_TRACE(_T("No Pagefile on: %s\n"), sRoot);
		}
	}

	if (   !bAllOK
		&& !bCreate)
	{	// nothing yet created
		for (i=0; i<m_saAssigned.GetCount(); i++)
		{
			sRoot = m_saAssigned.GetAt(i);
			sPath = sRoot + m_Pathes[OSPI_PAGEFILE];
			BOOL bExist = DoesFileExist(sPath);
			if (m_bCheckRegKeys)
			{
				if (bExist)
				{
					bCreate = TRUE;
					bEnableHive = TRUE;
					break;
				}
			}
			else
			{	// take the first fixed drive allowed
				if (!bExist)
				{
					WK_TRACE(_T("Creating pathes on %s\n"), sRoot);
					bCreate = TRUE;
					m_bReboot = TRUE;
					bEnableHive = TRUE;
					break;
				}
			}
		}
	}

	if (!bCreate && !m_bCheckRegKeys && !bAllOK)
	{	// no creation nessesary
		// check existing path if its drive has changed
		sPath = wkpDVRT.GetString(SECTION_LOG, LOG_PROTOCOLPATH, NULL);
		if (!sPath.IsEmpty() && sPath.GetAt(0) != m_sDontCheck.GetAt(0))
		{
			int nType = GetDriveType(sPath.Left(3));
			if (nType != DRIVE_FIXED)	// if the path is not a fixed drive anymore
			{
				bCreate = TRUE;			// warp the references
				CString str;
				str.LoadString(IDS_INVALID_PATH_REFERENCE);
				SetInfoText(str + sPath.Left(3));
				sRoot = m_sDontCheck.Left(2) + _T("\\");
				m_bReboot = TRUE;
				m_pDelete = &wkpDVRT;
				bEnableHive = FALSE;
			}
		}
	}

	if (bAllOK && m_bCheckRegKeys != RESTORE)
	{
		m_bCheckRegKeys = VALIDATE;
		bCreate = TRUE;
	}

	if (bCreate)
	{
		CWK_Profile wkpUser(CWK_Profile::WKP_ABSOLUT, HKEY_CURRENT_USER,_T(""),_T(""));

		CString str;
		str.LoadString(IDS_CREATING_PATHES);
		SetInfoText(str + sRoot);

        if (!m_Pathes[OSPI_PAGEFILE].IsEmpty())
        {
	        sPath.Format(_T("%s %d %d"), m_Pathes[OSPI_PAGEFILE], m_nPageFileSize, m_nPageFileSize);
	        sPath = sRoot + sPath;

	        if (m_bCreateAutoPageFile && sRoot.GetAt(0) != m_sDontCheck.GetAt(0))
	        {
		        str.Format(_T("%c:\\%s 0 0"), m_sDontCheck.GetAt(0), m_Pathes[OSPI_PAGEFILE]);
		        str += _T(";");
	        }
	        else
	        {
		        str.Empty();
	        }
	        str += sPath;

	        SetOutSourcePath(wkpSystem, HKLM_MEM_MGMT, MEM_MGMT_PAGE_FILE, str, REG_MULTI_SZ);
        }

        if (!m_Pathes[OSPI_TEMP].IsEmpty())
        {
		    sPath = sRoot + m_Pathes[OSPI_TEMP];
		    CreateDirectory(sPath);
		    SetOutSourcePath(wkpSystem, HKLM_ENV_TEMP, ENV_TEMP, sPath, REG_EXPAND_SZ);
		    SetOutSourcePath(wkpSystem, HKLM_ENV_TEMP, ENV_TMP, sPath, REG_EXPAND_SZ);
		    SetOutSourcePath(wkpUser, HKCU_ENV_TEMP, ENV_TEMP, sPath, REG_EXPAND_SZ);
		    SetOutSourcePath(wkpUser, HKCU_ENV_TEMP, ENV_TMP, sPath, REG_EXPAND_SZ);
        }

        if (!m_Pathes[OSPI_LOG].IsEmpty())
        {
		    m_sLogFilesSource = wkpDVRT.GetString(SECTION_LOG, LOG_LOGPATH, NULL);
		    if (m_sLogFilesSource.IsEmpty())
		    {
			    m_sLogFilesSource.Format(_T("%c:\\%s"), m_sDontCheck.GetAt(0), m_Pathes[OSPI_LOG]);
		    }
		    sPath = sRoot + m_Pathes[OSPI_LOG];
		    m_sLogFilesDestination = sPath;
		    CreateDirectory(sPath);
		    SetOutSourcePath(wkpDVRT, SECTION_LOG, LOG_LOGPATH, sPath);
        }

        if (!m_Pathes[OSPI_PROTOCOL].IsEmpty())
        {
		    sPath = sRoot + m_Pathes[OSPI_PROTOCOL];
		    CreateDirectory(sPath);
		    SetOutSourcePath(wkpDVRT, SECTION_LOG, LOG_PROTOCOLPATH, sPath);
        }

        if (!m_Pathes[OSPI_LOG_CONFIG].IsEmpty())
        {
		    sPath = sRoot + m_Pathes[OSPI_LOG_CONFIG];
		    CreateDirectory(sPath);
            CStringArray fSections;
            int n = wkpSystem.EnumRegKeys(HKLM_EVENTLOG, fSections);
            for (i=0; i<n; i++)
            {
                SetOutSourcePath(wkpSystem, HKLM_EVENTLOG+BSLASH+fSections[i], EVENTLOG_FILE, sPath + BSLASH+fSections[i] +_T(".evt"), REG_EXPAND_SZ);
            }
        }

        if (!m_Pathes[OSPI_LOG_DNS].IsEmpty())
        {
		    sPath = sRoot +  m_Pathes[OSPI_LOG_DNS];
		    CreateDirectory(sPath);
		    SetOutSourcePath(wkpSystem, HKLM_DNS_PARAMS, DNS_LOGFILE, sPath + _T("\\dns.Log"));
        }

        if (!m_Pathes[OSPI_LOG_IIS].IsEmpty())
        {
		    sPath = sRoot +  m_Pathes[OSPI_LOG_IIS];
		    CreateDirectory(sPath);
		    SetOutSourcePath(wkpSystem, HKLM_IIS_LOG, LOG_DIRECTORY, sPath);
        }

        if (!m_Pathes[OSPI_OUT].IsEmpty())
        {
		    sPath = sRoot + m_Pathes[OSPI_OUT];
		    CreateDirectory(sPath);
		    SetOutSourcePath(wkpDVRT, _T("IdipClient\\Settings"), COMMON_OPENDIR, sPath);
		    SetOutSourcePath(wkpDVRT, SECTION_COMMON, COMMON_OPENDIR, sPath);
        }

        if (!m_Pathes[OSPI_DVRTWWW].IsEmpty())
        {
		    sPath = sRoot + m_Pathes[OSPI_DVRTWWW];
		    CreateDirectory(sPath);
		    SetOutSourcePath(wkpDVRT, SECTION_COMMON, COMMON_WWW_ROOT_DIR, sPath);
        }

        if (!m_Pathes[OSPI_BACKUP].IsEmpty())
        {
		    sPath = sRoot + m_Pathes[OSPI_BACKUP];
		    CreateDirectory(sPath);
		    SetOutSourcePath(wkpDVRT, SECTION_COMMON, COMMON_BACKUP_DIR, sPath);

		    SetOutSourcePath(wkpDVRT, SECTION_COMMON, COMMON_DATA_BASE_DIR, sPath);

		    SetOutSourcePath(wkpDVRT, SECTION_COMMON, COMMON_DATABASESERVERCONFIG, sPath + _T("\\DataBaseServer.cfg"));
		    SetOutSourcePath(wkpDVRT, SECTION_COMMON, COMMON_RUNTIME_ERRORS, sPath + _T("\\rterror.dat"));
		    SetOutSourcePath(wkpDVRT, SECTION_HIVE_INJECTION _T("\\") SECTION_HIVE, HIVE_PATH_FILE, sPath + _T("\\Hive\\Dvrt.dat"));

		    if (m_nProductType == VSWT_IDIP)
		    {
			    SetOutSourcePath(wkpDVRT, SECTION_DBS, DBS_MAP_PATH, sPath);
		    }
        }

        if (!m_Pathes[OSPI_BACKUP_DV].IsEmpty())
        {
		    sPath = sRoot + m_Pathes[OSPI_BACKUP_DV];
		    CreateDirectory(sPath);
		    SetOutSourcePath(wkpDVRT, SECTION_HIVE_INJECTION _T("\\") SECTION_DIRECTORIES, _T("C:\\DV"), sPath);
		    sPath = sRoot + m_Pathes[OSPI_BACKUP_DVRTWWW];
		    CreateDirectory(sPath);
		    SetOutSourcePath(wkpDVRT, SECTION_HIVE_INJECTION _T("\\") SECTION_DIRECTORIES, _T("C:\\dvrtwww"), sPath);
		    sPath = sRoot + m_Pathes[OSPI_BACKUP_SECURITY];
		    CreateDirectory(sPath);
		    SetOutSourcePath(wkpDVRT, SECTION_HIVE_INJECTION _T("\\") SECTION_DIRECTORIES, _T("C:\\Security"), sPath);
        }
	}

    if (!m_Pathes[OSPI_BACKUP_DV].IsEmpty())
    {
        // HiveInjection Service
	    int nStartTypeOld = wkpSystem.GetInt(HKLM_HIVE_INJ_SERVICE, SERVICE_START_TYPE, 0);
	    int nStartType = bEnableHive ? START_AUTO : START_DEACTIVE;
	    WK_TRACE(_T("HiveInjection Service: %d -> %d\n"), nStartTypeOld, nStartType);
	    if (!m_bCheckRegKeys && nStartTypeOld != nStartType)
	    {
		    wkpSystem.WriteInt(HKLM_HIVE_INJ_SERVICE, SERVICE_START_TYPE, nStartType);
		    m_bReboot = TRUE;
	    }
    }

	SetInfoText(IDS_CRATE_PATHTES_FINISHED);
	if (m_bAutoMode)
	{
		if (m_bReboot)
		{
			SetInfoText(IDS_REBOOTING);
		}
		m_bCloseDialog = TRUE;
	}
	m_saAssigned.RemoveAll();
	m_pDelete = NULL;
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::CreateDirectory(LPCTSTR sDir)
{
	if (!m_bCheckRegKeys)
	{
		WK_CreateDirectory(sDir);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedCkLocateAndRecoverBadSectors()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_LOCATE_AND_RECOVER_BAD_SECTORS, m_bLocateAndRecoverBadSectors);
	if (m_bLocateAndRecoverBadSectors)
	{
		dx.m_bSaveAndValidate = FALSE;
		m_bFixErrors = TRUE;
		DDX_Check(&dx, IDC_CK_FIX_ERRORS, m_bFixErrors);
	}
	OnChanged();
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedCkForceDismount()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_FORCE_DISMOUNT, m_bForceDismounting);
	if (m_bForceDismounting)
	{
		dx.m_bSaveAndValidate = FALSE;
		m_bFixErrors = TRUE;
		DDX_Check(&dx, IDC_CK_FIX_ERRORS, m_bFixErrors);
	}
	OnChanged();
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedCkCompactFlash()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_COMPACT_FLASH, m_bCompactFlash);
	GetDlgItem(IDC_BTN_CHECK_CF_REG)->EnableWindow(m_bCompactFlash);
	GetDlgItem(IDC_CREATE_PATHS)->EnableWindow(m_bCompactFlash);
	OnChanged();
}
LRESULT CVideteCheckDiskDlg::OnUserProcess(WPARAM wParam, LPARAM )
{
	UINT nIDEvent = wParam;
	if (m_pProcess)
	{
		if (m_pProcess->GetErrorLine())
		{
			CString sName = m_pProcess->GetName();
			if (sName == DISKPART_APP)
			{
				CString str;
				str.Format(IDS_PROCESS_ERROR, sName);
				SetInfoText(str);
				WK_TRACE_ERROR(_T("%d opening %s\n"), m_pProcess->GetErrorLine(), sName);
				PartitionAndFormatFinished();
			}
		}
		BOOL bDiskpartCmdFinnished = FALSE;
		CWK_String *pstr = NULL;
		if (IsBetween(nIDEvent, DISKPART_CONSOLE_OPEN, DISKPART_LAST))
		{
			m_dwSelfCheck = 1;
			pstr = m_pProcess->FindString(_T("DISKPART>"), FALSE, TRUE);
			if (pstr)
			{
				bDiskpartCmdFinnished = TRUE;
				TraceOutputString(*pstr);
				delete pstr;
			}
		}
		switch(nIDEvent)
		{
////////////////////////////////////////////////////////////////////////////////////////
			case STD_OUT_READ_EVENT:
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					delete pstr;
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case CHKDSK_CMD_CONSOLE_OPEN:
			{
				m_dwSelfCheck = 3;
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					if (m_sFinnished.IsEmpty() && pstr->Find(_T(">")) != -1)
					{
						m_sFinnished = *pstr;
					}
					TraceOutputString(*pstr);
					delete pstr;
				}
				CString sFmt;
				sFmt.Format(_T("chkdsk %s"), m_saAssigned.GetAt(0));
				if (m_bFixErrors)					sFmt += _T(" /F");
				if (m_bLocateAndRecoverBadSectors)	sFmt += _T(" /R"); 
				if (m_bForceDismounting)			sFmt += _T(" /X");
				if (m_bShowAddInfo)					sFmt += _T(" /V");
				if (!m_bCheckIndexEntries)			sFmt += _T(" /I");
				if (!m_bCheckCyclicFolderStruct)	sFmt += _T(" /C");

				m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, CHKDSK_RUNNING, 0);
				m_pProcess->WriteStdIn(sFmt, TRUE);
				sFmt.Format(IDS_CHECKING_DISK, m_saAssigned.GetAt(0));
				SetInfoText(sFmt);

			} break;
////////////////////////////////////////////////////////////////////////////////////////
			case CHKDSK_RUNNING:
			{
				BOOL bFinnished = FALSE;
				pstr = m_pProcess->FindString(m_sFinnished, FALSE, TRUE);
				if (pstr)
				{
					bFinnished = TRUE;
					TraceOutputString(*pstr);
					delete pstr;
				}
				if (m_sPercentDone.IsEmpty())	// find the language
				{
					CString sFind, sSection;
					int i;
					for (i=1; ; i++)
					{
						sSection.Format(_T("%d"), i);
						sFind = theApp.GetProfileString(sSection, _T("chkdsk"));
						if (sFind.IsEmpty())
						{
							break;
						}
						pstr = m_pProcess->FindString(sFind, FALSE, TRUE);
						if (pstr)
						{
							TraceOutputString(*pstr);
							delete pstr;
							m_sPercentDone = theApp.GetProfileString(sSection, _T("percentdone"));
							m_sYesNo       = theApp.GetProfileString(sSection, _T("yesno"));
							m_sYes         = theApp.GetProfileString(sSection, _T("yes"));
							m_sNo          = theApp.GetProfileString(sSection, _T("no"));
							break;
						}
					}
				}

				if (!m_sYesNo.IsEmpty() &&  m_pProcess->FindString(m_sYesNo, FALSE, FALSE))
				{
					m_pProcess->WriteStdIn(m_bCheckAfterReboot ? m_sYes : m_sNo, TRUE);
					if (m_bCheckAfterReboot) m_bReboot = TRUE;
				}
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					if (!m_sPercentDone.IsEmpty())
					{
						int nPercent = 0;
						int nResult = _stscanf(*pstr, m_sPercentDone, &nPercent);
						if (nResult == 1)
						{
							m_cProgress.SetPos(nPercent);
							TraceOutputString(*pstr);
							delete pstr;
							pstr = NULL;
						}
					}
					if (pstr)
					{
						TraceOutputString(*pstr);
						delete pstr;
					}
				}
				if (bFinnished)
				{
					if (m_saAssigned.GetCount())
					{
						m_saAssigned.RemoveAt(0);
					}
					if (m_saAssigned.GetCount())
					{
						PostMessage(WM_USER_PROCESS, CHKDSK_CMD_CONSOLE_OPEN);
					}
					else
					{
						CheckDiskFinished();
					}
				}
			} break;
////////////////////////////////////////////////////////////////////////////////////////
			case DISKPART_CONSOLE_OPEN:	// diskpart console opened
			{
				CString sFind, sSection;
				int i;
				for (i=1; ; i++)
				{
					sSection.Format(_T("%d"), i);
					sFind = theApp.GetProfileString(sSection, _T("diskpart"));
					if (sFind.IsEmpty())
					{
						break;
					}
					if (m_pProcess->FindString(sFind, FALSE, FALSE))
					{
						m_sPartition = theApp.GetProfileString(sSection, _T("partition"));
						m_sExtended = theApp.GetProfileString(sSection, _T("extended"));
						WK_TRACE(_T("Found diskpart strings: %s, %s\n"), m_sPartition, m_sExtended);
						break;
					}
				}
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					delete pstr;
				}
				if (bDiskpartCmdFinnished)
				{
					m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, DISKPART_LIST_DISKS, 0);
					m_pProcess->WriteStdIn(_T("list disk"), TRUE);
					SetInfoText(IDS_LISTING_DISKS);
					m_nDisks = 0;
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case DISKPART_LIST_DISKS:	// receive disk list
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					m_nDisks++;
					if (m_nDisks >= 3)
					{
						int i, nFindZeroBytes = pstr->Find(_T("0 B"));
						if (nFindZeroBytes == -1)
						{
							LPCTSTR psz = *pstr;
							for (i=0; i<pstr->GetLength(); i++)
							{
								if (isdigit(psz[i]))
								{
									break;
								}
							}
							CString str = pstr->Mid(i, 3);
							str.TrimRight();
							str.TrimLeft();
							str.MakeLower();
							m_saDisks.Add(str);
							*pstr += _T("#");
						}
					}
					TraceOutputString(*pstr);
					delete pstr;
				}
				if (bDiskpartCmdFinnished)
				{
					CString str;
					if (m_saDisks.GetCount())
					{
						str.LoadString(IDS_CHECKING_DISKS); // _T("Checking disks: ");
						int i;
						for (i=0; i<m_saDisks.GetCount(); i++)
						{
							str += m_saDisks.GetAt(i);
							if (i<m_saDisks.GetCount()-1) 
							{
								str += _T(", ");
							}
						}
					}
					else
					{
						str.LoadString(IDS_NO_EMPTY_DISK); // _T("No empty disks");
					}
					SetInfoText(str);						// 1 line for: diskpart>
					WK_TRACE(_T("%s\n"), str);
					m_nDisks = (int)m_nDisks-2;				// 2 lines are used for headline and dashes of the table
					PostMessage(WM_USER_PROCESS, DISKPART_SELECT_DISK);
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case DISKPART_SELECT_DISK:	// array of disks empty
			if (m_saDisks.GetCount())	// no
			{							// select disk 
				CString sFmt;
				sFmt.Format(_T("select disk %s"), m_saDisks.GetAt(0));	// select disk
				m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, DISKPART_DISK_SELECTED, 0);
				m_pProcess->WriteStdIn(sFmt, TRUE);
				WK_TRACE(_T("%s\n"), sFmt);
				m_sCurrentDisk.Format(_T("disk%d"), 1 + _ttoi(m_saDisks.GetAt(0)));
				m_saDisks.RemoveAt(0);
				m_saPartitions.RemoveAll();
			}
			else						// yes
			{							// list volume
				CString sFmt = _T("list volume");
				m_saPartitions.RemoveAll();
				m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, DISKPART_LIST_VOLUMES, 0);
				m_pProcess->WriteStdIn(_T("list volume"), TRUE);
				WK_TRACE(_T("%s\n"), sFmt);
			}break;
			case DISKPART_LIST_VOLUMES:	// receive volume list
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					m_saPartitions.Add(*pstr);
					TraceOutputString(*pstr);
					delete pstr;
				}
				if (bDiskpartCmdFinnished)
				{
					int i, nCount = m_saPartitions.GetCount();
					if (nCount >= 2)		// parse header lengths of volume list
					{
						_TCHAR szFind[] = _T("- ");
						BOOL bSpace = FALSE;
						const int nPositions = 8;
						int nPos, naPos[nPositions], naLen[nPositions];
						int nType = 4, nDrive = 1, nFS = 3;
						CString sType, sDrive, sFS, sFmt = m_saPartitions.GetAt(1);
						LPCTSTR szFmt = sFmt;

						for (i=0, nPos=0; i<sFmt.GetLength(); i++)
						{
							if (szFmt[i] == szFind[bSpace])
							{
								if (bSpace)
								{
									naLen[nPos] = (i-1) - naPos[nPos];
									nPos++;
									if (nPos >= nPositions)
									{
										break;
									}
									bSpace = FALSE;
								}
								else
								{
									naPos[nPos] = i;
                                    bSpace = TRUE;
								}
							}
						}


						for (i=2; i<nCount; i++)	// parse volume list
						{
							sType	= m_saPartitions.GetAt(i).Mid(naPos[nType], naLen[nType]);
							sType.TrimLeft();
							sType.TrimRight();
							if (sType.CompareNoCase(m_sPartition) == 0)
							{
								sDrive  = m_saPartitions.GetAt(i).Mid(naPos[nDrive], naLen[nDrive]);
								sDrive.TrimLeft();
								sDrive.TrimRight();
								sFS		= m_saPartitions.GetAt(i).Mid(naPos[nFS], naLen[nFS]);
								sFS.TrimLeft();		// file system (NTFS, FAT)
								sFS.TrimRight();
								if (sDrive.IsEmpty())	// assign a letter
								{						// first select volume
									m_sCurrentDisk.Format(_T("volume%d"), i-2);
									m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, DISKPART_SELECT_VOLUME, 0);
									sFmt.Format(_T("select volume %d"), i-2);
									m_pProcess->WriteStdIn(sFmt, TRUE);
									WK_TRACE(_T("%s\n"), sFmt);
									break;
								}
								else if (sFS.IsEmpty())	// no file system
								{						// add to format batch list
									m_sCurrentDisk.Format(_T("volume%d"), i-2);
									AddToFormatList(sDrive, m_sCurrentDisk);
								}
							}
						}
						if (i==nCount)
						{
							StartFormat();
						}
					}
					else
					{
						StartFormat();
					}
				}
			}break;
			case DISKPART_SELECT_VOLUME:	// volume selected
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					delete pstr;
				}
				if (bDiskpartCmdFinnished)
				{
					if (m_saFreeDrives.GetSize() == 0)
					{
						WK_TRACE_ERROR(_T("No free drive available.\n"));
						StartFormat();
						return 0;
					}
					CString sFmt;			// assign a free letter
					sFmt.Format(_T("assign letter=%s"), m_saFreeDrives.GetAt(0));
					m_nLinesCounted = 0;
					m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, DISKPART_DRIVE_LETTER_ASSIGNED, 0);
					m_pProcess->WriteStdIn(sFmt, TRUE);
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case DISKPART_DISK_SELECTED:	// disk selected
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					delete pstr;
				}
				if (bDiskpartCmdFinnished)
				{
					m_nLinesCounted = 0;
					m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, DISKPART_LIST_PARTITIONS, 0);
					m_pProcess->WriteStdIn(_T("list partition"), TRUE);
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case DISKPART_LIST_PARTITIONS:	// receive partition list
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					pstr->MakeLower();
					m_saPartitions.Add(*pstr);
					delete pstr;
				}
				if (bDiskpartCmdFinnished)
				{
					CString sFmt;

					WPARAM evtMsg;
					int nCount = m_saPartitions.GetCount();
					if (nCount == 1)			// no partitions available
					{
						sFmt = _T("create partition extended");
						evtMsg = DISKPART_CREATE_PARTITION_EXT_FINNISHED;
					}
					else						// parse list
					{
						m_sCurrentDisk += _T("_2");
						int i, nFind;
						for (i=0; i<nCount; i++)
						{
							nFind = m_saPartitions.GetAt(i).Find(m_sPartition);
							if (   nFind != -1
								&& m_saPartitions.GetAt(i).Find(m_sExtended)  != -1)
							{
								CString sPartNo = m_saPartitions.GetAt(i).Mid(nFind + m_sPartition.GetLength() + 1, 3);
								sFmt.Format(_T("select partitinon %s\n"), sPartNo);
								evtMsg = DISKPART_CREATE_PARTITION_EXT_FINNISHED;
								break;
							}
						}
						if (i == nCount)
						{
							sFmt = _T("create partition extended");
							evtMsg = DISKPART_CREATE_PARTITION_EXT_FINNISHED;
						}
					}
					m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, evtMsg, 0);
					m_pProcess->WriteStdIn(sFmt, TRUE);
					SetInfoText(IDS_CREATE_PART_EXT);
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case DISKPART_CREATE_PARTITION_EXT_FINNISHED:	// create partition extended finnished
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					delete pstr;
				}
				if (bDiskpartCmdFinnished)
				{
					m_nLinesCounted = 0;
					m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, DISKPART_PARTITION_EXT_OK, 0);
					m_pProcess->WriteStdIn(_T("list partition"), TRUE);
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case DISKPART_PARTITION_EXT_OK:	// extended partition ok
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					delete pstr;
					m_nLinesCounted++;
				}
				if (bDiskpartCmdFinnished)
				{
					if (m_nLinesCounted > 2)	// extended partition created
					{							// create partition logical
						CString sFmt = _T("create partition logical");
						m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, DISKPART_CREATE_PARTITION_LOG_FINNISHED, 0);
						m_pProcess->WriteStdIn(sFmt, TRUE);
						SetInfoText(IDS_CREATE_PART_LOG);
					}
					else
					{
						PostMessage(WM_USER_PROCESS, DISKPART_SELECT_DISK);
					}
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case DISKPART_CREATE_PARTITION_LOG_FINNISHED:	// create partition logical finnished
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					delete pstr;
				}
				if (bDiskpartCmdFinnished)
				{
					m_nLinesCounted = 0;
					m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, DISKPART_PARTITION_LOG_OK, 0);
					m_pProcess->WriteStdIn(_T("list partition"), TRUE);
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case DISKPART_PARTITION_LOG_OK:	// logical partition ok
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					delete pstr;
					m_nLinesCounted++;
				}
				CString sFmt;
				if (bDiskpartCmdFinnished)
				{
					if (m_nLinesCounted > 3)			// logical partition created
					{
						if (m_saFreeDrives.GetSize() == 0)
						{
							WK_TRACE_ERROR(_T("No free drive available.\n"));
							StartFormat();
							return 0;
						}
						sFmt.Format(_T("assign letter=%s"), m_saFreeDrives.GetAt(0));
						m_nLinesCounted = 0;
						m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, DISKPART_DRIVE_LETTER_ASSIGNED, 0);
						m_pProcess->WriteStdIn(sFmt, TRUE);
					}
					else
					{
						PostMessage(WM_USER_PROCESS, DISKPART_SELECT_DISK);
					}
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case DISKPART_DRIVE_LETTER_ASSIGNED: // drive letter assigned?
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					delete pstr;
					m_nLinesCounted++;
				}
				if (bDiskpartCmdFinnished)
				{
					if (m_saFreeDrives.GetSize() == 0)
					{
						WK_TRACE_ERROR(_T("UNEXPECTED: list should contain an entry.\n"));
						StartFormat();
						return 0;
					}
					// two lines mean no success; hint for failure cause is added.
					if (m_nLinesCounted == 1)	// single line means success
					{							// add assigned drives to be formatet
						AddToFormatList(m_saFreeDrives[0], m_sCurrentDisk);
					}
					m_saFreeDrives.RemoveAt(0);// remove the letter always

					PostMessage(WM_USER_PROCESS, DISKPART_SELECT_DISK);
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case FORMAT_BEGIN: // begin to format the assigned letters
			{
				int nDisks = (int)m_saAssigned.GetSize();
				if (nDisks)
				{
					WK_DELETE(m_pProcess);
					DWORD dwFlags = WKCPF_INPUT|WKCPF_OUTPUT|WKCPF_OUTPUT_CHECK|WKCPF_TERMINATE;
					m_pProcess = new CWKControlledProcess(_T("cmd.exe"), NULL, NULL, dwFlags);
					m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, FORMAT_CMD_CONSOLE_OPEN, 0);
					m_pProcess->StartThread();
					WK_TRACE(_T("Starting command console\n"));
				}
				else
				{
					PartitionAndFormatFinished();
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case FORMAT_CMD_CONSOLE_OPEN: // command console started
			{
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					delete pstr;
				}
				int nDisks = (int)m_saAssigned.GetSize();
				if (nDisks)
				{
					CString sFmt;
					CString sAssigned = m_saAssigned[0];
					m_saAssigned.RemoveAt(0);
					sFmt.Format(_T("format %s /FS:NTFS /V:%s /Q"), sAssigned.Left(2), sAssigned.Mid(2));
					m_sFormating = sAssigned;
					m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, FORMAT_CONFIRM_START, 0);
					m_pProcess->WriteStdIn(sFmt, TRUE);
					sFmt.Format(IDS_FORMATING_AS, sAssigned.Left(2), sAssigned.Mid(2)); // _T("formating %s (%s) as NTFS")
					SetInfoText(sFmt);
				}
				else
				{
					PartitionAndFormatFinished();
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case FORMAT_CONFIRM_START: // confirm start format
			{
				CString sFmt;
				sFmt = _T("");
				BOOL bReady = FALSE;
				int iCount;
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					TraceOutputString(*pstr);
					iCount = pstr->Find(_T(")?"));
					if (iCount != -1)
					{
						sFmt = pstr->Mid(iCount-3, 1);
					}
					else if (pstr->Find(_T(":")) != -1 && pstr->Find(_T(">")) != -1)
					{
						bReady = TRUE;
					}
					delete pstr;
					if (!sFmt.IsEmpty() || bReady)
					{
						break;
					}
				}
				if (!sFmt.IsEmpty())
				{
					m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, FORMAT_FINNISHED, 0);
					m_pProcess->WriteStdIn(sFmt, TRUE);
				}
				else if (bReady)
				{
					PostMessage(WM_USER_PROCESS, FORMAT_CMD_CONSOLE_OPEN);
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case FORMAT_FINNISHED: // format finnished
			{
				BOOL bReady = FALSE;
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					if (pstr->Find(_T(":")) != -1 && pstr->Find(_T(">")) != -1)
					{
						bReady =TRUE;
					}
					TraceOutputString(*pstr);
					delete pstr;
				}
				if (bReady)
				{
					Sleep(200);
					CString sFmt;
					sFmt.Format(_T("dir %s"), m_sFormating.Left(2));
					m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_USER_PROCESS, FORMAT_VALIDATE, 0);
					m_pProcess->WriteStdIn(sFmt, TRUE);
					Sleep(200);
				}
			}break;
////////////////////////////////////////////////////////////////////////////////////////
			case FORMAT_VALIDATE:
			{
				CString sDrive;
				BOOL bOk = FALSE, bReady = FALSE;
				if (!m_sFormating.IsEmpty())
				{
					sDrive = m_sFormating.Left(2) + _T("\\");
					sDrive.MakeLower();
				}
				while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
				{
					pstr->MakeLower();
					if (pstr->Find(_T(":")) != -1 && pstr->Find(_T(">")) != -1)
					{
						bReady = TRUE;
					}
					else if (!sDrive.IsEmpty() && pstr->Find(sDrive) != -1)
					{
						bOk = TRUE;
					}
					TraceOutputString(*pstr);
					delete pstr;
				}
				if (!sDrive.IsEmpty())
				{
					if (bOk)
					{
						CWK_Profile wkp;
						WK_TRACE(_T("New drive formated: %s: [%s]\n"), sDrive, m_sFormating.Mid(2));
						if (m_nProductType == VSWT_IDIP)
						{
							// make new formated drive to database drive in idip
							sDrive.MakeUpper();
							wkp.WriteInt(_T("Drives"), sDrive.Left(1), DVR_DB_DRIVE);
							WK_TRACE(_T("Insert new database drive: %s\n"), m_sFormating);
						}
						m_sFormating.Empty();
					}
					else
					{
						if (bReady)
						{
							WK_TRACE_ERROR(_T("Could not format: %s: [%s]\n"), sDrive, m_sFormating.Mid(2));
							m_bReboot = TRUE;
						}
					}
				}
				if (bReady)
				{
					m_sFormating.Empty();
					PostMessage(WM_USER_PROCESS, FORMAT_CMD_CONSOLE_OPEN);
				}
			}break;
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_nTimer)
	{
		if (m_bCloseDialog)
		{
			if (m_bCompactFlash)
			{
				CString sPath;
				sPath.Format(_T("%c:\\%s"), m_sDontCheck[0], m_Pathes[OSPI_PAGEFILE]);
				if (DoesFileExist(sPath))
				{
					if (DeleteFile(sPath))
					{
						WK_TRACE(_T("Deleting pagefile on CF: %s\n"), sPath);
						m_bReboot = TRUE;
					}
					else
					{
						WK_TRACE_ERROR(_T("Could not delete pagefile on CF: %s, error: %d\n"), sPath, GetLastError());
					}
				}
				if (!SetFirstFixedDriveToCheckEdt())
				{
					WK_TRACE_ERROR(_T("no fixed drive, no pagefile\n"), sPath);
					CWK_Profile wkpSystem(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE, _T(""), _T(""));
					CStringArray ar;
					ar.Add(_T(""));
					wkpSystem.WriteMultiString(HKLM_MEM_MGMT, MEM_MGMT_PAGE_FILE, ar);
					m_bReboot = TRUE;
				}
			}
			if (m_pInfoDlg && m_pInfoDlg->PendingMessages())
			{
				return;
			}
			PostMessage(WM_COMMAND, IDOK);
		}
	}

	CDialog::OnTimer(nIDEvent);
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnDestroy()
{
	WK_DELETE(m_pProcess);
	if (m_nTimer)
	{
		KillTimer(m_nTimer);
	}
	CDialog::OnDestroy();
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (IsWindowVisible())
	{
		int nShowOut = SW_SHOW;
		int nShowOthers = SW_HIDE;
		CRect rcOthers, rcOut;
		m_edtOutput.GetWindowRect(&rcOut);
		if (m_TabCtrl.GetCurSel() == 1)
		{
			nShowOut = SW_HIDE;
			nShowOthers = SW_SHOW;
		}
		m_edtOutput.ShowWindow(nShowOut);
		CWnd *pWnd = GetWindow(GW_CHILD);
		while (pWnd)
		{
			if (pWnd != &m_edtOutput)
			{
				pWnd->GetWindowRect(&rcOthers);
				if (rcOut.PtInRect(rcOthers.TopLeft()))
				{
					pWnd->ShowWindow(nShowOthers);
				}
			}
			pWnd = pWnd->GetWindow(GW_HWNDNEXT);
		}
	}
	*pResult = 0;
}
////////////////////////////////////////////////////////////////////////////////////////
BOOL CVideteCheckDiskDlg::OnToolTipNotify(UINT /*id*/, NMHDR *pNMHDR, LRESULT *pResult)
{
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
	CString strTipText;
	UINT nID = (UINT)pNMHDR->idFrom;
	if (pNMHDR->code == TTN_NEEDTEXT && (pTTT->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool
		HWND hwnd = NULL;
		memcpy (&hwnd, &nID, sizeof(UINT));
		nID = ::GetDlgCtrlID(hwnd);
	}
	int nTW = ::SendMessage(pNMHDR->hwndFrom, TTM_GETMAXTIPWIDTH, 0, 0);  
	if (nTW == -1)
	{
		CRect rc;
		GetClientRect(&rc);
		::SendMessage(pNMHDR->hwndFrom, TTM_SETMAXTIPWIDTH, 0, MulDiv(rc.right, 4, 5));
		::SendMessage(pNMHDR->hwndFrom, TTM_SETDELAYTIME, TTDT_AUTOPOP, 20000);
	}

	if (nID != 0)
	{
		if (strTipText.LoadString(nID))
		{
			strTipText.Replace(_T("\n"), _T("\r\n"));
			pTTT->lpszText = NULL;
			m_strTipText = strTipText;
			pTTT->lpszText = (LPTSTR)LPCTSTR(m_strTipText);
		}
		else
		{
			pTTT->lpszText = NULL;
			m_strTipText.Empty();
			return FALSE;
		}
	}
	*pResult = 0;

	return TRUE;    // message was handled
}
////////////////////////////////////////////////////////////////////////////////////////
BOOL CVideteCheckDiskDlg::SetOutSourcePath(CWK_Profile&wkp, LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue/*=NULL*/, int nType/*=0*/) 
{
	CString str, sDrive;
	CStringArray ar;
	if (nType == REG_MULTI_SZ)
	{
		wkp.GetMultiString(szSection, szKey, ar);
        ConcatenateStrings(ar, str, _T(';'));
	}
	else
	{
		str = wkp.GetString(szSection, szKey, NULL);
	}

	WK_TRACE(_T("Read Registry: %s\\%s='%s'\n"), szSection, szKey, LPCTSTR(str));
	if (m_bCheckRegKeys == VALIDATE)
	{
		if (_tcsnicmp(str, szValue, 1) != 0)
		{
			WK_TRACE(_T("Registry settings are corrected from %s to %s\n"), LPCTSTR(str), szValue);
			//m_bCheckRegKeys = FALSE;
		}
	}

    CString sBackupKey = szKey + BACKUP;
    DWORD dwSize = 0;
    DWORD dwResult = wkp.GetType(szSection, sBackupKey, &dwSize);
    if (dwResult == REG_NONE)
    {
        if (nType == REG_MULTI_SZ)
	    {
		    wkp.WriteMultiString(szSection, sBackupKey, ar);
	    }
	    else if (nType == REG_EXPAND_SZ)
	    {
		    wkp.WriteString(szSection, sBackupKey, str, REG_EXPAND_SZ);
	    }
	    else
	    {
		    wkp.WriteString( szSection, sBackupKey, str);
	    }
    }
    else if (m_bCheckRegKeys == RESTORE)
    {
        if (nType == REG_MULTI_SZ)
	    {
            ar.RemoveAll();
    		wkp.GetMultiString(szSection, sBackupKey, ar);
		    wkp.WriteMultiString(szSection, szKey, ar);
            ConcatenateStrings(ar, str, _T(';'));
	    }
	    else if (nType == REG_EXPAND_SZ)
	    {
		    str = wkp.GetString(szSection, sBackupKey, NULL);
		    wkp.WriteString(szSection, szKey, str, REG_EXPAND_SZ);
	    }
	    else
	    {
		    str = wkp.GetString(szSection, sBackupKey, NULL);
		    wkp.WriteString( szSection, szKey, str);
	    }
        WK_TRACE(_T("Restoring to: %s='%s'\n"), szKey, LPCTSTR(str));
    }

	if (szValue)
	{
		str = szValue;
		if (nType == REG_MULTI_SZ)
		{
			ar.RemoveAll();
			SplitString(str, ar, _T(';'));
		}
	}

	if (!m_bCheckRegKeys)
	{
		if (m_pDelete == &wkp)
		{
			wkp.DeleteEntry(szSection, szKey);
			str.Empty();
		}
		else if (nType == REG_MULTI_SZ)
		{
			wkp.WriteMultiString(szSection, szKey, ar);
		}
		else if (nType == REG_EXPAND_SZ)
		{
			wkp.WriteString(szSection, szKey, str, REG_EXPAND_SZ);
		}
		else
		{
			wkp.WriteString( szSection, szKey, str);
		}
		WK_TRACE(_T("%s Registry: %s\\%s='%s'\n"), (m_pDelete == &wkp) ? _T("Delete") : _T("Write"), szSection, szKey, str);
	}
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::DoExitWindows(UINT uFlags) 
{
	HANDLE hToken;					// handle to process token 
	TOKEN_PRIVILEGES tkp;			// ptr. to token structure 

	BOOL fResult;					// system shutdown flag 

	try
	{
		// Get the current process token handle 
		// so we can get debug privilege. 
		fResult = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) ;
		if (!fResult) throw 1;

		// Get the LUID for debug privilege. 
		fResult = LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
		if (!fResult) throw 2;

		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

		// Get shutdown privilege for this process. 
		fResult = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 
		if (!fResult) throw 3;
	}
	catch (int nPos)
	{
	    DWORD dwError = GetLastError();
		WK_TRACE_ERROR(_T("DoExitWindows(), %d, last error: %d, %x\n"),nPos, dwError, dwError);
	}

	if (fResult)
	{
		fResult = ExitWindowsEx(uFlags,0);
		WK_TRACE(_T("ExitWindowsEx(%x): %d\n"), uFlags, fResult);
	}

	if (hToken != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hToken);
	}
	OnOK();
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::SetInfoText(UINT nIDinfo, int nTimeout/*=250*/)
{
	if (m_pInfoDlg)
	{
		CString str;
		if (str.LoadString(nIDinfo))
		{
			SetInfoText(str);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::SetInfoText(LPCTSTR sInfo, int nTimeout/*=250*/)
{
	if (m_pInfoDlg)
	{
		m_pInfoDlg->SetInfoText(sInfo, nTimeout);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::PartitionAndFormatFinished()
{
	WK_DELETE(m_pProcess);
	SetInfoText(IDS_PART_AND_FORM_FINISHED);//_T("Partition and Format finished!"));
	SetFirstFixedDriveToCheckEdt();
	if (m_bAutoMode)
	{
		if (m_bCompactFlash)
		{
			PostMessage(WM_COMMAND, IDC_CREATE_PATHS);
		}
		else
		{
			m_bCloseDialog = TRUE;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::CheckDiskFinished()
{
	WK_DELETE(m_pProcess);
	SetInfoText(IDS_CHKDSKFINISHED);//_T("CheckDisk finished!"));
	if (m_bAutoCheckDisk)
	{
		m_bCloseDialog = TRUE;
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::CheckHidden()
{
	if (m_bAutoMode || m_bAutoCheckDisk || m_bAutoEnableEwf)
	{
		ShowWindow(SW_HIDE);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::AddToFormatList(CString sDrive, const CString&sVolume)
{
	sDrive.MakeLower();
	int i, n = m_saAssigned.GetCount();
	for (i=0; i<n; i++)
	{
		if (m_saAssigned[i][0] == sDrive[0])
		{
			break;
		}
	}
	if (i==n)
	{
		if (sDrive.GetLength() == 1)
		{
			sDrive += _T(":");
		}
		m_saAssigned.Add(sDrive.Left(2) + sVolume);
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::StartFormat()
{
	if (m_pProcess)
	{
		WK_TRACE(_T("Write exit to %s\n"), m_pProcess->GetName());
		m_pProcess->WriteStdIn(_T("Exit"), TRUE);
		Sleep(250);
	}
	WK_TRACE(_T("Starting Format\n"));
	PostMessage(WM_USER_PROCESS, FORMAT_BEGIN);
}
////////////////////////////////////////////////////////////////////////////////////////
BOOL CVideteCheckDiskDlg::SetFirstFixedDriveToCheckEdt()
{
	BOOL bFixedDrive = FALSE;
	DWORD dwMask=1, dwLogical = GetLogicalDrives();
	int i;				// list the available disks
	CString sFmt, sFirst;
 	for (i=2,dwMask=4; i<26; i++,dwMask<<=1)
	{
		if (dwMask & dwLogical)
		{
			sFmt.Format(_T("%c:"), i+'a');
			int nType = GetDriveType(sFmt);
			if (nType == DRIVE_FIXED && m_sDontCheck.Find(sFmt) == -1)
			{
				if (sFirst.IsEmpty())
				{
					sFirst = sFmt;
				}
				bFixedDrive = TRUE;
				break;
			}
		}
	}
	SetDlgItemText(IDC_EDT_CHECK_DRIVES, sFirst);
	return bFixedDrive;
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::SwitchToTab(int nTab)
{
	m_TabCtrl.SetCurSel(nTab);
	LRESULT nResult = 1;
	OnTcnSelchangeTab(NULL, &nResult);
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnChanged()
{
	GetDlgItem(IDC_BTN_SAVE_SETTINGS)->EnableWindow(TRUE);
}
////////////////////////////////////////////////////////////////////////////////////////
LRESULT CVideteCheckDiskDlg::OnUser(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case WM_USER_SET_TAB:
			SwitchToTab(lParam);
			break;
		case WM_SHOWWINDOW:
			ShowWindow(lParam ? SW_SHOW : SW_HIDE);
			break;
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnEnChangeEdtCheckDrives()
{
	CString str;
	GetDlgItemText(IDC_EDT_CHECK_DRIVES, str);
	GetDlgItem(IDC_EDT_DONT_CHECK_DRIVES)->EnableWindow(str.IsEmpty());
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedBtnCheckPathes()
{
	m_bCheckRegKeys = TRUE;
	OnBnClickedCreatePaths();
	m_bCheckRegKeys = FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::OnBnClickedBtnRestorePathes()
{
    m_bCheckRegKeys = RESTORE;
	OnBnClickedCreatePaths();
	m_bCheckRegKeys = FALSE;
}
////////////////////////////////////////////////////////////////////////////////////////
LRESULT CVideteCheckDiskDlg::OnSelfCheck(WPARAM wParam, LPARAM lParam)
{
	if (wParam)
	{
		::PostMessage((HWND)wParam, WM_SELFCHECK, (WPARAM)m_hWnd, lParam);
	}
	return m_dwSelfCheck;
}
////////////////////////////////////////////////////////////////////////////////////////
void CVideteCheckDiskDlg::TraceOutputString(LPCTSTR sOut)
{
	if (m_bTraceAllOutputStrings)
	{
		WK_TRACE(_T("%s\n"), sOut);
	}
}


