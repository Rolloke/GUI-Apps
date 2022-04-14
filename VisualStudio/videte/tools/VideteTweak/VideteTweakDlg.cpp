// VideteTweakDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VideteTweak.h"
#include "VideteTweakDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EXPLORER_SAVED	 "SavedExlorer"
#define EXPLORER_SAVEDAU "SavedExlorerAU"
#define EXPLORER_IMAGE   "ImageExplorer"
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

CAboutDlg::CAboutDlg() : CPropertyPage(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
/*
   char  szFileName[_MAX_PATH];
   DWORD dwDummy = 0;
   wsprintf(szFileName, "%s.EXE", AfxGetApp()->m_pszExeName);               // Die Dateiversion des Programms ermitteln
	
	int bytes    = GetFileVersionInfoSize(szFileName, &dwDummy);
   int nVersion = 1;
   int nSubVers = 0;
   if(bytes)
   {
      void * buffer = new char[bytes];
      GetFileVersionInfo(szFileName, 0, bytes, buffer);
      VS_FIXEDFILEINFO * Version;
      unsigned int     length;
      if(VerQueryValue(buffer,"\\",(void **) &Version, &length))
      {                                                        // Bsp.: Version 1.20 => 0x00010014
         nVersion = HIWORD(Version->dwFileVersionMS);
         nSubVers = LOWORD(Version->dwFileVersionMS);
         if ((nSubVers > 0) && (nSubVers < 10)) nSubVers *= 10;
      }
      delete[] buffer;
   }	
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CVideteTweakDlg dialog

static const RegKeyInfo g_pExplorer[] =
	{
//		{"SM", "Startmenü", NULL, 0,  0, 0}, // Startmenü
		{"StartMenuLogoff",
		 "Disable Logoff on the Start Menu",
		 "Removes the \"Logoff\" button from the Start menu and prevents\nusers from adding the Logoff button to the Start menu.",
		 REG_DWORD,  0, 1},
		{"ForceStartMenuLogoff",
		 "Force Logoff to the Start Menu",
		 "Forces the Logoff button to the Start menu and prevents users\nfrom removing the Logoff option from the Start menu.",
		 REG_DWORD,  0, 1},
		{"NoRun",
		 "Remove Run item from Start Menu",
		 "Removes the ability to execute programs from the Run option on\nthe Start menu, Task Manager, or by pressing Winkey + R. Also removes\nthe corresponding checkbox from the Start menu customization CPL.",
		 REG_DWORD,  0, 1},
		{"NoFind",
		 "Remove Search item from Start Menu",
		 "Removes the Search item from the Start menu and disables some\nWindows Explorer search elements.",
		 REG_DWORD,  0, 1},
		{"NoClose",
		 "Disable and remove the Turn Off Computer button",
		 "Removes the \"Turn Off Computer\" button from the Start Menu\nand prevents shutting down Windows using the standard shutdown\nuser interface.",
		 REG_DWORD,  0, 1},
		{"NoStartMenuEjectPC",
		 "Disable and remove the Undock PC command",
		 "Removes the \"Undock PC\" button from the Start Menu and\nprevents undocking of the PC (laptop).",
		 REG_DWORD,  0, 1},
		{"NoStartMenuMyMusic",
		 "Remove My Music item from Start Menu", NULL, REG_DWORD,  0, 1},
		{"NoStartMenuSubFolders",
		 "Prohibit Start Menu Sub Folders",
		 "Hides the folders at the top section of the Start menu.\nItems appear, but folders are hidden",
		 REG_DWORD,  0, 1},
		{"NoStartMenuPinnedList",
		 "Remove pinned programs list from the Start menu",
		 "Removes the pinned programs list from the Start menu.\nAlso removes the Internet and E-mail checkboxes from\nthe Start menu customization CPL",
		 REG_DWORD,  0, 1},
		{"NoStartMenuMFUprogramsList",
		 "Remove frequent programs list from the Start Menu",
		 "Removes the frequently-used programs list from the Start menu",
		 REG_DWORD,  0, 1},
		{"NoStartMenuMorePrograms",
		 "Remove More Programs list from the Start menu",
		 "Removes the More Programs list from the Start menu",
		 REG_DWORD,  0, 1},
		{"NoStartMenuNetworkPlaces",
		 "Remove Network Places from the Start Menu",
		 "Removes the Network Places item from the Start menu and the\ncorresponding checkbox from the Start menu customization CPL.",
		 REG_DWORD,  0, 1},
		{"NoSMMyPictures",
		 "Remove My Pictures item from Start Menu", NULL, REG_DWORD,  0, 1},
		{"NoSMMyDocs",
		 "Remove My Documents folder from the Start Menu", NULL, REG_DWORD,  0, 1},
		{"NoSMHelp",
		 "Remove Help item from Start Menu",
		 "Removes the Help item from the Start menu, but does\nnot disable Help files from running.",
		 REG_DWORD,  0, 1},
		{"NoRecentDocsMenu",
		 "Remove Recent Documents item from Start Menu",
		 NULL, REG_DWORD,  0, 1},
		{"NoRecentDocsNetHood",
		 "Do not add shares of recently used documents to Network Places",
		 "Remote shared folders are not added to Network Places\nwhenever you open a document in the shared folder",
		 REG_DWORD,  0, 1},
		{"NoUserNameInStartMenu",
		 "Remove user name from Start Menu",
		 "Removes the user name from the Start menu.",
		 REG_DWORD,  0, 1},
		{"NoSimpleStartMenu",
		 "Disable the new simple Start Menu",
		 "Disables the new simple Start menu\nand forces the classic Start menu.",
		 REG_DWORD,  0, 1},
		{"NoCommonGroups",
		 "Remove common program groups from Start Menu",
		 "Removes items in the All Users profile from\nthe More Programs list on the Start menu.",
		 REG_DWORD,  0, 1},
		{"NoNetworkConnections",
		 "Remove Network Connections from the Start Menu",
		 "Removes the Network Connections item from the Start menu and the\ncorresponding checkbox from the Start menu customization CPL.",
		 REG_DWORD,  0, 1},
		{"NoChangeStartMenu",
		 "Disable drag-and-drop menus on the Start Menu",
		 "Prevents users from modifying the Start menu by dragging\nand dropping items. Other methods of customizing the Start\nmenu are still enabled.",
		 REG_DWORD,  0, 1},
		{"NoSetTaskbar",
		 "Disable changes to Taskbar and Start Menu settings",
		 "Removes the Taskbar & Start Menu item from the Control Panel,\nand removes the Properties item from the Start menu\n(and button) context menu.",
		 REG_DWORD,  0, 1},
		{"NoSetFolders",
		 "Hide Control Panel, Printers and My Computer",
		 "Hides Control Panel, Printers and My Computer\nin Explorer and on the Start Menu.", 
		 REG_DWORD,  0, 1},
		{"GreyMSIAds",
		 "Gray unavailable Windows Installer programs Start Menu shortcuts",
		 "Displays partially installed programs\nin gray text on the Start menu.",
		 REG_DWORD,  0, 1},

//		{"DT", "Desktop", NULL, 0,  0, 0}, // Desktop
		{"NoActiveDesktopChanges",
		 "Prohibit Active Desktop Changes",
		 "Removes the Web tab in Display in Control Panel.\nRemoves Active Desktop from a right-click on the desktop.\nIf Active Desktop is already enabled, users cannot add,\nremove, or edit Web content or disable, lock, or\nsynchronize Active Desktop components",
		 REG_DWORD,  0, 1},
		{"NoSetActiveDesktop",
		 "Remove the Active Desktop item from the Settings menu", NULL, REG_DWORD,  0, 1},
		{"NoActiveDesktop",
		 "Disable Active Desktop",
		 "Removes Active Desktop from the context menu that appears\nwhen the user right-clicks the desktop. Removes the Web tab\nfrom Display in Control Panel. Disables Enable Web content on\nmy desktop on the General tab of the Folder Options dialog box.",
		 REG_DWORD,  1, 1},
		{"DisablePersonalDirChange",
		 "Prohibit user from changing My Documents path",
		 "Prevents users from changing the path to the My Documents folder.",
		 REG_DWORD,  0, 1},

		{"ClassicShell",
		 "Only Classic Shell", "Disables Active Desktop, Web view,\nand thumbnail views, as well as preventing the configuration\nof single-clicking. Users will NOT be able to restore these\nfeatures.", REG_BINARY, 0, 1},
		{"NoDesktopUpdate",
		 "Disable Desktop Update", "Prevents placing new shortcuts on the desktop", REG_BINARY, 0, 1},

		{"NoStartBanner",		// Start und Beenden
		 "Disable Start Banner",
		 "Hides the arrow and Click here to begin caption that\nappear on the taskbar when you start Windows NT",
		 REG_BINARY, 0, 1},
		{"NoSaveSettings",
		 "Don´t Save Settings",
		 "Prevent changes to the positions of icons and open windows,\nand the size and position of the taskbar from being saved.",
		 REG_BINARY, 0, 1},
		{"ClearRecentDocsOnExit",
		 "Clear history of recent documents on exit",
		 "Deletes all shortcuts shown in the Recent Documents Menu\nwhen the user logs off", REG_DWORD,  0, 1},

		{"NoFolderOptions",		// Explorer
		 "No Folder Options",
		 "Removes the Folder Options menu item\nfrom the Settings menu", REG_DWORD,  0, 1},
		{"NoFileMenu",
		 "Remove File Menu in Explorer",
		 NULL, REG_DWORD,  0, 1},
		{"NoEditMenu",
		 "No Edit Menu (unknown)",
		 NULL, REG_DWORD,  0, 1},
		{"HideClock",
		 "Remove Clock in System Tray",
		 NULL, REG_DWORD,  0, 1},
		{"NoFavoritesMenu",
		 "Remove Favorites item from Start Menu",
		 "Prevents users from adding the Favorites item to the Start menu.\nAlso removes the corresponding checkbox from the Start menu\ncustomization CPL.",
		 REG_DWORD,  1, 1},

		{"NoControlPanel", "Disable Control Panel",
		 "When checked, Control Panel is removed from the Settings Menu\nand from Windows Explorer.\n\nUsers can NOT run Control Panel or Control Panel applets. If a\nuser tries to start a Control Panel applet by other means,\nthey receive a message explaining that this action is prevented\nby Group Policy. This entry does NOT disable programs that are\nrepresented by folders in Control Panel, such as Network and Dial-up\nConnections, Scheduled Tasks, and Fonts.",
		 REG_DWORD,  0, 1},
		{"NoRecentDocsHistory",
		 "No Recent Documents History",
		 NULL, REG_BINARY ,  0, 1},
		{"NoWindowsUpdate",
		 "Disable and remove links to Windows Update",
		 "Removes the Windows Update link from the More Programs list\nin Start menu, from the Tools menu in IE, and blocks user\naccess to windowsupdate.Microsoft.com.",
		 REG_DWORD,  0, 1},
		{"NoTrayContextMenu",
		 "Prohibit Tray Context Menu", 
		 "Menus do not display upon right click of the taskbar,\nstart button, clock, or taskbar application icons. The\nentry is only available for NT 4.0 with SP 2 or greater",
		 REG_DWORD,  0, 1},
		{"NoViewContextMenu",
		 "Prohibit View Context Menu",
		 "menus do not display upon right click of the desktop or\nExplorer's results pane. The entry is only available for\nNT 4.0 with SP 2 or greater",
		 REG_DWORD,  0, 1},
		{"NoNetConnectDisconnect",
		 "Prohibit Map and Disconnect Network Drive", "Removes the \"Map Network Drive\"\nand \"Disconnect Network Drive\"\nmenu and right click options", REG_DWORD,  0, 1},
		{"NoDeletePrinter",
		 "No Delete Printer", NULL, REG_DWORD,  0, 1},
		{"NoAddPrinter",
		 "No Add Printer", NULL, REG_DWORD,  0, 1},
		{"NoInstrumentation",
		 "Disable user tracking",
		 "Prevents the system from remembering the\nprograms run, paths followed, and documents used.",
		 REG_DWORD,  0, 1},
		{"DisableMyPicturesDirChange",
		 "Prohibit user from changing My Pictures", NULL, REG_DWORD,  0, 1},
		{"DisableMyMusicDirChange",
		 "Prohibit user from changing My Music", NULL, REG_DWORD,  0, 1},
		{"DisableFavoritesDirChange",
		 "Prohibit user from changing Favorites", NULL, REG_DWORD,  0, 1},

		{"NoLogoff",
		 "Disable the Logoff button from the CTRL+ALT+DEL Security dialog",
		 NULL, REG_DWORD,  0, 1},
		{"NoWorkgroupContents",
		 "Don't Display Workgroup Contents",
		 "If checked, Network Neighborhood does not display\ncomputers in the local workgroup or domain",
		 REG_DWORD,  0, 1},
		{"NoEntireNetwork",
		 "Don't Display Netword Neighbourhood",
		 "Restricts Network Neighborhood from displaying or accessing\ncomputers outside the local workgroup or domain. The user can\nstill use the Start/Run, Map/Connect Network Drive, and the\nCommand Prompt.",
		 REG_DWORD,  0, 1},
		{"NoSharedDocuments",
		 "Remove Shared Documents Folder", NULL, REG_DWORD,  0, 1},
		{"NoNetHood",
		 "Remove the Network Neighborhood icon",
		 "Removes the Network Neighborhood icon and\nprevents networkaccess from explorer\n(it will still work from a command prompt).",
		 REG_DWORD,  0, 1},
		{"NoInternetIcon",
		 "Remove Internet Icon from desktop", NULL, REG_DWORD,  0, 1},

		{"NoResolveTrack",
		 "Do not use the tracking-based method when resolving shell shortcuts",
		 "Prevents the system from using NTFS features to resolve a shortcut.",
		 REG_DWORD,  0, 1},
		{"NoResolveSearch",
		 "Do not use the search-based method when resolving shell shortcuts",
		 "Prevents the system from conducting a comprehensive\nsearch of the target drive to resolve a shortcut.",
		 REG_DWORD,  0, 1},
		{"LinkResolveIgnoreLinkInfo",
		 "Disable Link Tracking for all Shortcuts",
		 "Prohibits shortcuts attempt to resolve to another computer",
		 REG_DWORD,  0, 1},

		{"MemCheckBoxInRunDlg",
		 "Add \"Run in Separate Memory Space\" check box to Run dialog box",
		 "Allows 16-bit programs to run in a dedicated\nVirtual DOS Machine (VDM) process.",
		 REG_DWORD,  0, 1},
		{"NoDeskTop", "Hide all desktop icons", NULL, REG_DWORD,  0, 1},
		{"NoNetworkConnections",
		 "Disable Network Connections", NULL, REG_DWORD,  0, 1},
		{"DisableTaskMgr", "Disable Task Manager", NULL, REG_DWORD,  0, 1},
		{"NoRunasInstallPrompt", "Do not request alternate credentials",
		 "When enabled at:\nUser Configuration\\Administrative Templates\\Windows Components\\Windows Explorer,\nprevents users from submitting alternate credentials when installing from local media.",
		 REG_DWORD,  0, 1}, 
		{"PromptRunasInstallNetPath", "Request credentials for network installations",
		 "When enabled at:\nUser Configuration\\Administrative Templates\\Windows Components\\Windows Explorer,\nprompts users for alternate credentials when installing from a network share",
		 REG_DWORD,  0, 1}, 
		{"NoFileAssociate", "No File Type Associations",
		 "Disables the buttons on the Windows Explorer / Tools / Folder Options ... / File Types tab", REG_DWORD,  0, 1},
		{"NoManageMyComputerVerb",
		 "Remove Computer Management",
		 "Hides the Manage item in the context menu of My Computer and Windows Explorer.\nThis entry does NOT remove Computer Management from Start / Programs / Administrative Tools.",
		 REG_DWORD,  0, 1}, 
		{"NoLowDiskSpaceChecks", "Disable Warning Low Disk Space", NULL, REG_DWORD,  0, 1},

		{NULL, NULL, NULL, 0,  0, 0} 
	};
/*


*	{"RestrictRun",
	 "", "", REG_DWORD,  0, 1},
		In tip 050 we learned that setting the RestrictRun Value in the HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer key to 1 would allow us to configure allowed programs at the RestrictRun key:

		HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer\RestrictRun

		RestrictRun can only works from the Explorer process. It does not prevent users from running programs, such as Task Manager, that are started by the system process or by other processes such as CMD.EXE.

		For Windows NT to operate properly, users must be permitted to run Systray.exe and setup.exe (both are in %SystemRoot%\System32).

		The value entries in this subkey represent local programs which can appear in any order. The value entries have the following syntax:

		Decimal number (starting with 1) of type REG_SZ with a data string which is the name of executable file.

		Example:

		1  REG_SZ  setup.exe
		2  REG_SZ  systray.exe
		3  REG_SZ  Iexplore.exe
		4  REG_SZ  JSITTARH.EXE


*  EnableBalloonTips

* 	{"NoDriveTypeAutoRun",
	 "", "",  REG_DWORD, 0, 0x00000095},
		tip 0007 and tip 0215, I detailed the registry entries that control Autoplay in Windows NT.

		These entries also work in Windows 2000, but an additional entry may also be used. If Autoplay is on for a CD-ROM changer, it will cycle all slots as you change media.

		A Value Name of NoDriveTypeAutoRun, a REG_DWORD data type, can be configured to disable Autoplay on all drives of the specified types. NoDriveTypeAutoRun can be located at:

		HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer

		HKEY_USERS\.DEFAULT\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer

		HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\Explorer

		The data value to set is determinined by adding the following:

		Value   Meaning 
		A data value of 0x95 is the default, which disables Autoplay on unknown drives, floppy drives, and network drives.
		If Autorun is 0, Autoplay is disabled on all drives. If Autorun is 1, you can hold down the shift key as you change media to prevent Media Change Notification (MCN). Appropriate settings in NoDriveAutoRun also disable Autoplay.

		When Autoplay is enabled it begins as soon as you insert media, so setup programs and music start immediately.

		NOTE: If NoDriveAutoRun or NoDriveTypeAutoRun are set in the HKEY_LOCAL_MACHINE hive, then these setting in HKEY_USERS and HKEY_CURRENT_USER are ignored.


		NOTE: If you set the NoDriveTypeAutoRun value via the Disable Autoplay Group Policy, you can add the CD-ROM drive type or disable Autoplay on all drive types. Any other settings must be done via the registry. 

*	{"NoEditMenu", // Unbekannt
	 "", "", REG_DWORD,  0, 1},

*	{"EditLevel",
	 "", "", REG_DWORD,  0, 1},

*	{"NoHelp",
	 "", "", REG_DWORD,  0, 1},

*	{"EnforceShellExtensionSecurity",
	 "Enforce Shell Extension Security", "Causes Windows NT to only load the shell extensions listed in the Approved subkey (HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved).",
	 REG_DWORD,  0, 1},	// unbekannt

Policy:Maximum number of recent documents
Description:Specifies the number of shortcuts displayed on the Recent 
Documents submenu.
Registry Value:"MaxRecentDocs"  

Policy:Remove My Computer from the Start Menu
Description:Removes the My Computer item from the Start Menu and the 
corresponding checkbox from the Start menu customization CPL.
Registry Value:"{20D04FE0-3AEA-1069-A2D8-08002B30309D}" under 
HKCU\Software\Microsoft\Windows\CurrentVersion\Policies\NonEnum. 

 */


CVideteTweakDlg::CVideteTweakDlg()
	: CPropertyPage(CVideteTweakDlg::IDD)
{
	//{{AFX_DATA_INIT(CVideteTweakDlg)
	m_nDisableDrives = 0;
	m_nUsers = 0;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bChanged = false;
}

void CVideteTweakDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideteTweakDlg)
	DDX_Control(pDX, IDC_LIST_NO_VIEW_ON_DRIVE, m_ListNoViewOnDrive);
	DDX_Control(pDX, IDC_LIST_DRIVES_AUTOPLAY, m_ListAutoRun);
	DDX_Control(pDX, IDC_LIST_DRIVES, m_ListDrives);
	DDX_Control(pDX, IDC_LST_POLICY_EXPLORER, m_List);
	DDX_Radio(pDX, IDC_RADIO_DISABLEDRIVES, m_nDisableDrives);
	DDX_Radio(pDX, IDC_RD_CURRENT, m_nUsers);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVideteTweakDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CVideteTweakDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CLICK, IDC_LST_POLICY_EXPLORER, OnChange)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LST_POLICY_EXPLORER, OnGetdispinfoLstPolicyExplorer)
	ON_NOTIFY(NM_RCLICK, IDC_LST_POLICY_EXPLORER, OnRclickLstPolicyExplorer)
	ON_BN_CLICKED(IDC_RADIO_DISABLEDRIVES, OnRadioDisabledrives)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_RESTORE, OnBtnRestore)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_BN_CLICKED(IDC_RD_CURRENT, OnRdUsers)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DRIVES, OnChange)
	ON_BN_CLICKED(IDC_RADIO_DISABLEDRIVES2, OnRadioDisabledrives)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DRIVES_AUTOPLAY, OnChange)
	ON_BN_CLICKED(IDC_RADIO_DISABLEDRIVES3, OnRadioDisabledrives)
	ON_NOTIFY(NM_CLICK, IDC_LIST_NO_VIEW_ON_DRIVE, OnChange)
	ON_BN_CLICKED(IDC_RD_ALL, OnRdUsers)
	ON_BN_CLICKED(IDC_BTN_SET, OnBtnImageSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideteTweakDlg message handlers

BOOL CVideteTweakDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	CWinApp *pApp = AfxGetApp();
	CString sHeadline;
	CRect rc;

   m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

   m_ListDrives.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

   m_ListAutoRun.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

   m_ListNoViewOnDrive.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

	m_ImageList.Create(IDB_LIST_IMAGES, 14, 0, RGB(255, 255, 255));
	sHeadline.LoadString(IDS_HL_EXP_POL);
	m_List.GetClientRect(&rc);
	LV_COLUMN lvc = {LVCF_TEXT|LVCF_WIDTH, 0, rc.right, (char*)LPCTSTR(sHeadline), sHeadline.GetLength(), 0, 0, 0};
	m_List.InsertColumn(0, &lvc);
	m_List.SetImageList(&m_ImageList, LVSIL_SMALL);
	m_ListDrives.InsertColumn(0, &lvc);
	m_ListAutoRun.InsertColumn(0, &lvc);
	m_ListNoViewOnDrive.InsertColumn(0, &lvc);

	OnRdUsers();

	CWnd *pWnd = GetParent();
	if (pWnd)
	{
		pWnd->SetIcon(m_hIcon, TRUE);			// Set big icon
		pWnd->SetIcon(m_hIcon, FALSE);		// Set small icon
		pWnd->ModifyStyleEx(0, WS_EX_CONTEXTHELP);
	}
	OnRadioDisabledrives();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVideteTweakDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CPropertyPage::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVideteTweakDlg::OnPaint() 
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
		CPropertyPage::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVideteTweakDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CVideteTweakDlg::OnApply() 
{
	if (m_bChanged)
	{
		DWORD dwValue, dw, dwMask, dwTypeAR, i;
		DWORD dwCount = sizeof(DWORD);
		HKEY  hSecKey = NULL;
		LONG  lResult = 1;
		if (m_nUsers == 0)
		{
			lResult = RegCreateKeyEx(HKEY_CURRENT_USER, HK_EXPLORER_POLICIES, 0, REG_NONE,
								REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
		}
		else
		{
			lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, HK_EXPLORER_POLICIES, 0, REG_NONE,
								REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
		}

		if (lResult == ERROR_SUCCESS)
		{
			for (i=0; g_pExplorer[i].szKey != NULL; i++)
			{
				if (m_List.GetCheck(i))
				{
					dwValue = g_pExplorer[i].dwSet;
				}
				else
				{
					dwValue = 0;
				}
				if (dwValue != g_pExplorer[i].dwDefault)
				{
					lResult = RegSetValueEx(hSecKey, g_pExplorer[i].szKey, NULL, g_pExplorer[i].dwType, (BYTE*)&dwValue, dwCount);
				}
				else
				{
					RegDeleteValue(hSecKey, g_pExplorer[i].szKey);
				}
			}
		}

		dwValue = 0;
		for (i=0; i<(DWORD)m_ListDrives.GetItemCount(); i++)
		{
			if (m_ListDrives.GetCheck(i))
			{
				dwValue |= m_ListDrives.GetItemData(i);
			}
		}
		if (dwValue)
		{
			lResult = RegSetValueEx(hSecKey, NODRIVES, NULL, REG_DWORD, (BYTE*)&dwValue, dwCount);
		}
		else
		{
			RegDeleteValue(hSecKey, NODRIVES);
		}

		dwValue  = 0;
		dwTypeAR = 0;
		for (i=0; i<(DWORD)m_ListAutoRun.GetItemCount(); i++)
		{
			if (m_ListAutoRun.GetCheck(i))
			{
				dwMask = m_ListAutoRun.GetItemData(i);
				if (dwMask & 0x80000000)
				{
					dwTypeAR |= (dwMask & 0x7fffffff);
				}
				else
				{
					dwValue |= dwMask;
				}
			}
		}
		if (dwValue)
		{
			lResult = RegSetValueEx(hSecKey, NODRIVEAUTORUN, NULL, REG_DWORD, (BYTE*)&dwValue, dwCount);
		}
		else
		{
			RegDeleteValue(hSecKey, NODRIVEAUTORUN);
		}

		if (dwTypeAR)
		{
			lResult = RegSetValueEx(hSecKey, NODRIVETYPEAUTORUN, NULL, REG_DWORD, (BYTE*)&dwTypeAR, dwCount);
		}
		else
		{
			RegDeleteValue(hSecKey, NODRIVETYPEAUTORUN);
		}

		dwValue = 0;
		for (i=0; i<(DWORD)m_ListNoViewOnDrive.GetItemCount(); i++)
		{
			if (m_ListNoViewOnDrive.GetCheck(i))
			{
				dwValue |= m_ListNoViewOnDrive.GetItemData(i);
			}
		}
		if (dwValue)
		{
			lResult = RegSetValueEx(hSecKey, NOVIEWONDRIVE, NULL, REG_DWORD, (BYTE*)&dwValue, dwCount);
		}
		else
		{
			RegDeleteValue(hSecKey, NOVIEWONDRIVE);
		}

		REG_CLOSE_KEY(hSecKey);
		
		SetModified(FALSE);
		m_bChanged = false;
	}
	return CPropertyPage::OnApply();
}

void CVideteTweakDlg::OnChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CListCtrl*pWnd = (CListCtrl*) CWnd::FromHandle(pNMHDR->hwndFrom);
	CPoint pt;
	UINT   nFlags = 0;
	GetCursorPos(&pt);
	pWnd->ScreenToClient(&pt);
	int nHit = pWnd->HitTest(pt, &nFlags);
	if ((nHit != -1) && (nFlags & LVHT_ONITEMSTATEICON))
	{
		SetModified();	
		m_bChanged = true;
	}
	*pResult = 0;
}

void CVideteTweakDlg::OnGetdispinfoLstPolicyExplorer(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
   if(pDispInfo->item.mask & LVIF_TEXT)
   {
      if (pDispInfo->item.iSubItem == 0)
      {
         strncpy(pDispInfo->item.pszText, g_pExplorer[pDispInfo->item.iItem].szPolicy, pDispInfo->item.cchTextMax);
		}
	}

	
	*pResult = 0;
}

void CVideteTweakDlg::OnRclickLstPolicyExplorer(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CListCtrl*pWnd = (CListCtrl*) CWnd::FromHandle(pNMHDR->hwndFrom);
	CPoint pt;
	UINT   nFlags = 0;
	GetCursorPos(&pt);
	pWnd->ScreenToClient(&pt);
	int nHit = pWnd->HitTest(pt, &nFlags);
	if (nHit != -1) 
	{
		if (nFlags & LVHT_ONITEMICON )
		{
			LVITEM lvi;
			lvi.mask     = LVIF_IMAGE;
			lvi.iItem    = nHit;
			lvi.iSubItem = 0;
			m_List.GetItem(&lvi);
			lvi.iImage++;
			if (lvi.iImage > 2) lvi.iImage = 0;
			m_List.SetItem(&lvi);
			m_List.Update(nHit);
		}
		else
		{
			if ((nHit >= 0) && (nHit < m_List.GetItemCount()))
			{
				char *psz = ((g_pExplorer[nHit].szDescription != NULL) ? g_pExplorer[nHit].szDescription : g_pExplorer[nHit].szPolicy);
				::MessageBox(m_hWnd, psz, g_pExplorer[nHit].szKey, MB_OK|MB_ICONINFORMATION);
			}

		}
	}		
	*pResult = 0;
}

void CVideteTweakDlg::OnRadioDisabledrives() 
{
	CDataExchange dx(this, true);
	DDX_Radio(&dx, IDC_RADIO_DISABLEDRIVES, m_nDisableDrives);
	GetDlgItem(IDC_LIST_DRIVES)->ShowWindow((m_nDisableDrives == 0) ? SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_LIST_DRIVES_AUTOPLAY)->ShowWindow((m_nDisableDrives ==	1) ? SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_LIST_NO_VIEW_ON_DRIVE)->ShowWindow((m_nDisableDrives ==	2) ? SW_SHOW:SW_HIDE);
}

HBRUSH CVideteTweakDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
//	TRACE("OnCtlColor\n");
	return hbr;
}


void CVideteTweakDlg::OnBtnRestore() 
{
   CWinApp *pApp = AfxGetApp();
	LVITEM lvi;
	lvi.mask     = LVIF_IMAGE;
	lvi.iSubItem = 0;
	char *pszSave = m_nUsers ? EXPLORER_SAVEDAU: EXPLORER_SAVED;
	for (lvi.iItem=0; g_pExplorer[lvi.iItem].szKey != NULL; lvi.iItem++)
	{
		m_List.SetCheck(lvi.iItem, pApp->GetProfileInt(pszSave, g_pExplorer[lvi.iItem].szKey, 0));
	}
	m_List.InvalidateRect(NULL);

	DWORD i, dwValue, dwTypeAR, dwMask;
	dwValue = pApp->GetProfileInt(EXPLORER_SAVED, NODRIVES, 0);
	for (i=0; i<(DWORD)m_ListDrives.GetItemCount(); i++)
	{
		if (dwValue & m_ListDrives.GetItemData(i))
		{
			m_ListDrives.SetCheck(i, TRUE);
		}
		else
		{
			m_ListDrives.SetCheck(i, FALSE);
		}
	}

	dwValue  = pApp->GetProfileInt(pszSave, NODRIVEAUTORUN, 0);
	dwTypeAR = pApp->GetProfileInt(pszSave, NODRIVETYPEAUTORUN, 0);
	for (i=0; i<(DWORD)m_ListAutoRun.GetItemCount(); i++)
	{
		dwMask = m_ListAutoRun.GetItemData(i);
		if (dwMask & 0x80000000)
		{
			if (dwTypeAR & dwMask)
			{
				m_ListAutoRun.SetCheck(i, TRUE);
			}
			else
			{
				m_ListAutoRun.SetCheck(i, FALSE);
			}
		}
		else
		{
			if (dwValue & dwMask)
			{
				m_ListAutoRun.SetCheck(i, TRUE);
			}
			else
			{
				m_ListAutoRun.SetCheck(i, FALSE);
			}
		}
	}
	
	

	dwValue = pApp->GetProfileInt(pszSave, NOVIEWONDRIVE, dwValue);
	for (i=0; i<(DWORD)m_ListNoViewOnDrive.GetItemCount(); i++)
	{
		if (dwValue & m_ListNoViewOnDrive.GetItemData(i))
		{
			m_ListNoViewOnDrive.SetCheck(i, TRUE);
		}
		else
		{
			m_ListNoViewOnDrive.SetCheck(i, FALSE);
		}
	}
	SetModified();	
	m_bChanged = true;
}

void CVideteTweakDlg::OnBtnSave() 
{
   CWinApp *pApp = AfxGetApp();
	LVITEM lvi;
	lvi.mask     = LVIF_IMAGE;
	lvi.iSubItem = 0;
	char *pszSave = m_nUsers ? EXPLORER_SAVEDAU: EXPLORER_SAVED;
	for (lvi.iItem=0; g_pExplorer[lvi.iItem].szKey != NULL; lvi.iItem++)
	{
		m_List.GetItem(&lvi);
		pApp->WriteProfileInt(EXPLORER_IMAGE, g_pExplorer[lvi.iItem].szKey, lvi.iImage);
		pApp->WriteProfileInt(pszSave, g_pExplorer[lvi.iItem].szKey, m_List.GetCheck(lvi.iItem));
	}	

	DWORD i, dwValue, dwTypeAR, dwMask;
	dwValue = 0;
	for (i=0; i<(DWORD)m_ListDrives.GetItemCount(); i++)
	{
		if (m_ListDrives.GetCheck(i))
		{
			dwValue |= m_ListDrives.GetItemData(i);
		}
	}
	pApp->WriteProfileInt(pszSave, NODRIVES, dwValue);

	dwValue  = 0;
	dwTypeAR = 0;
	for (i=0; i<(DWORD)m_ListAutoRun.GetItemCount(); i++)
	{
		if (m_ListAutoRun.GetCheck(i))
		{
			dwMask = m_ListAutoRun.GetItemData(i);
			if (dwMask & 0x80000000)
			{
				dwTypeAR |= (dwMask & 0x7fffffff);
			}
			else
			{
				dwValue |= dwMask;
			}
		}
	}
	pApp->WriteProfileInt(pszSave, NODRIVEAUTORUN, dwValue);
	pApp->WriteProfileInt(pszSave, NODRIVETYPEAUTORUN, dwTypeAR);

	dwValue = 0;
	for (i=0; i<(DWORD)m_ListNoViewOnDrive.GetItemCount(); i++)
	{
		if (m_ListNoViewOnDrive.GetCheck(i))
		{
			dwValue |= m_ListNoViewOnDrive.GetItemData(i);
		}
	}
	pApp->WriteProfileInt(pszSave, NOVIEWONDRIVE, dwValue);
}

LRESULT CVideteTweakDlg::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
	CString str;
	char *pszKey = m_nUsers ? "HKEY_CURRENT_USER" : "HKEY_LOCAL_MACHINE";
	switch (pHI->iCtrlId)
	{
		case IDC_RADIO_DISABLEDRIVES: str.Format("%s\\%s:\n%s (REG_DWORD)", pszKey, HK_EXPLORER_POLICIES, NODRIVES); break;
		case IDC_RADIO_DISABLEDRIVES2: str.Format("%s\\%s:\n%s (REG_DWORD)\n%s (REG_DWORD)", pszKey, HK_EXPLORER_POLICIES, NODRIVEAUTORUN, NODRIVETYPEAUTORUN); break;
		case IDC_RADIO_DISABLEDRIVES3: str.Format("%s\\%s:\n%s (REG_DWORD)", pszKey, HK_EXPLORER_POLICIES, NOVIEWONDRIVE); break;
		case IDC_LIST_DRIVES: str.Format("%s\\%s:\nCheckmarks for Drive Parameter Bits", pszKey, HK_EXPLORER_POLICIES); break;
		case IDC_LST_POLICY_EXPLORER: str.Format("%s\\%s:\nCheckmarks for Explorer Policies", pszKey, HK_EXPLORER_POLICIES); break;
		default:break;
	}
	if (!str.IsEmpty())
	{
		AfxMessageBox(str);
	}
	return 1;
}

void CVideteTweakDlg::OnRdUsers() 
{
	CString sDrive;
	int   i, j;
	LONG lResult;
	DWORD dwValue           = 0,
		   dwType,
		   dwAwailableDrives = 0,
			dwAutoRun         = 0,
			dwTypeAutoRun     = 0,
			dwNoViewOnDrive   = 0,
			dwMask,
			dwCount           = sizeof(DWORD);
	char szDrive[] = "A:";
	HKEY  hSecKey  = NULL;
	LVITEM lvi;
	lvi.mask       = LVIF_IMAGE;
	lvi.iSubItem   = 0;
   CWinApp *pApp  = AfxGetApp();
	CDataExchange dx(this, true);
	i = m_nUsers;
	DDX_Radio(&dx, IDC_RD_CURRENT, m_nUsers);
	if ((i != m_nUsers) && m_bChanged)
	{
		if (AfxMessageBox(IDS_REQUEST_SAVE_CHANGES, MB_YESNO|MB_ICONQUESTION) == IDYES)
		{
			OnApply(); 
		}
		else
		{
			m_bChanged = false;
		}
	}
	m_List.DeleteAllItems();
	m_ListAutoRun.DeleteAllItems();
	m_ListDrives.DeleteAllItems();
	m_ListNoViewOnDrive.DeleteAllItems();


	if (m_nUsers == 0)
	{
		lResult = RegOpenKeyEx(HKEY_CURRENT_USER, HK_EXPLORER_POLICIES, 0, KEY_READ, &hSecKey);
	}
	else
	{
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, HK_EXPLORER_POLICIES, 0, KEY_READ, &hSecKey);
	}


	if (lResult != ERROR_SUCCESS)
	{
		REG_CLOSE_KEY(hSecKey);
	}

	for (i=0; g_pExplorer[i].szKey != NULL; i++)
	{
		m_List.InsertItem(i, g_pExplorer[i].szPolicy);
		lvi.iImage = pApp->GetProfileInt(EXPLORER_IMAGE, g_pExplorer[i].szKey, 0);
		lvi.iItem  = i;
		m_List.SetItem(&lvi);
		
		if (g_pExplorer[i].dwType)
		{
			if (hSecKey)
			{
				lResult = RegQueryValueEx(hSecKey, (LPTSTR)g_pExplorer[i].szKey, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
			}
			else
			{
				lResult = 1;
			}
			if (lResult == ERROR_SUCCESS)
			{	
				m_List.SetCheck(i, (dwValue != 0) ? TRUE:FALSE);
			}
			else
			{
				m_List.SetCheck(i, (g_pExplorer[i].dwDefault != 0) ? TRUE:FALSE);
			}
		}
	}

	lResult = RegQueryValueEx(hSecKey, NODRIVES, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
	if (lResult != ERROR_SUCCESS)
	{
		dwValue = 0;
	}

	lResult = RegQueryValueEx(hSecKey, NODRIVEAUTORUN, NULL, &dwType, (LPBYTE)&dwAutoRun, &dwCount);
	if (lResult != ERROR_SUCCESS)
	{
		dwAutoRun = 0;
	}
	lResult = RegQueryValueEx(hSecKey, NODRIVETYPEAUTORUN, NULL, &dwType, (LPBYTE)&dwTypeAutoRun, &dwCount);
	if (lResult != ERROR_SUCCESS)
	{
		dwTypeAutoRun = 0;
	}

	lResult = RegQueryValueEx(hSecKey, NOVIEWONDRIVE, NULL, &dwType, (LPBYTE)&dwNoViewOnDrive, &dwCount);
	if (lResult != ERROR_SUCCESS)
	{
		dwNoViewOnDrive = 0;
	}
	
	dwAwailableDrives = GetLogicalDrives();
	for (i=0, j=0; i<32; i++)
	{
		dwMask     = (1<<i);
		szDrive[0] = (char)('A'+i);
		if (dwMask & dwAwailableDrives)
		{
			sDrive = _T(szDrive);
			switch (GetDriveType(szDrive))
			{
				case DRIVE_UNKNOWN:		sDrive += _T("(Unknown)"); break;
				case DRIVE_NO_ROOT_DIR:	sDrive += _T("(NoRootDir)");break;
				case DRIVE_REMOVABLE:	sDrive += _T("(Removeable)"); break;
				case DRIVE_FIXED:			sDrive += _T("(Fixed)"); break;
				case DRIVE_REMOTE:		sDrive += _T("(Remote)"); break;
				case DRIVE_CDROM:			sDrive += _T("(CDROM)"); break;
				case DRIVE_RAMDISK:		sDrive += _T("(RAMDISK)"); break;
			}
			
			m_ListDrives.InsertItem(j, sDrive);
			m_ListDrives.SetCheck(j, (dwMask & dwValue) ? TRUE:FALSE);
			m_ListDrives.SetItemData(j, dwMask);

			m_ListAutoRun.InsertItem(j, sDrive);
			m_ListAutoRun.SetCheck(j, (dwMask & dwAutoRun) ? TRUE:FALSE);
			m_ListAutoRun.SetItemData(j, dwMask);

			m_ListNoViewOnDrive.InsertItem(j, sDrive);
			m_ListNoViewOnDrive.SetCheck(j, (dwMask & dwNoViewOnDrive) ? TRUE:FALSE);
			m_ListNoViewOnDrive.SetItemData(j, dwMask);
			j++;
		}		
		if (szDrive[0] == 'Z') break;
	}

	for (i=DRIVE_UNKNOWN; i<DRIVE_RAMDISK; i++)
	{
		sDrive = _T("Type:");
		dwMask    = (1<<i)|0x80000000;
		switch (i)
		{
			case DRIVE_UNKNOWN:		sDrive += _T("Unknown"); break;
			case DRIVE_NO_ROOT_DIR:	sDrive += _T("NoRootDir");break;
			case DRIVE_REMOVABLE:	sDrive += _T("Removeable"); break;
			case DRIVE_FIXED:			sDrive += _T("Fixed"); break;
			case DRIVE_REMOTE:		sDrive += _T("Remote"); break;
			case DRIVE_CDROM:			sDrive += _T("CDROM"); break;
			case DRIVE_RAMDISK:		sDrive += _T("RAMDISK"); break;
		}
		m_ListAutoRun.InsertItem(j, sDrive);
		m_ListAutoRun.SetCheck(j, (dwMask & dwTypeAutoRun) ? TRUE:FALSE);
		m_ListAutoRun.SetItemData(j, dwMask);
		j++;
/*
		0x1     Disables Autoplay on drives of unknown type. 
		0x4     Disables Autoplay on removable drives. 
		0x8     Disables Autoplay on fixed drives. 
		0x10    Disables Autoplay on network drives. 
		0x20    Disables Autoplay on CD-ROM drives. 
		0x40    Disables Autoplay on RAM disks. 
		0x80    Disables Autoplay on drives of unknown type. 
		0xFF    Disables Autoplay on all types of drives. 
*/
	}

	REG_CLOSE_KEY(hSecKey);

	char *pszSave = m_nUsers ? EXPLORER_SAVEDAU: EXPLORER_SAVED;
	int nTest = pApp->GetProfileInt(pszSave, NODRIVEAUTORUN, -1);
	if (nTest==-1)
	{
		OnBtnSave();
	}
}

void CVideteTweakDlg::OnBtnImageSettings() 
{
	int nCount = m_List.GetItemCount();
	LVITEM lvi;
	lvi.mask     = LVIF_IMAGE;
	lvi.iSubItem = 0;
	for (lvi.iItem=0; lvi.iItem<nCount; lvi.iItem++)
	{
		m_List.GetItem(&lvi);
		BOOL bCheck = (lvi.iImage == 1);
		BOOL bOld   = m_List.GetCheck(lvi.iItem);
		if (bOld != bCheck)
		{
			m_List.SetCheck(lvi.iItem, bCheck);
			m_bChanged = true;
		}
	}
	if (m_bChanged)
	{
		SetModified();	
	}
}
