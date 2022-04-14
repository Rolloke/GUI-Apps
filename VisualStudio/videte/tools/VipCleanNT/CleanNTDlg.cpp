// CleanNTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vipcleannt.h"
#include "CleanNTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _IDIP
static CCleanNTDlg* m_pThis;

/////////////////////////////////////////////////////////////////////////////
// CCleanNTDlg dialog
CCleanNTDlg::CCleanNTDlg(CIdipCleanDlg* pIdip, CWnd* pParent /*=NULL*/)
	: CDialog(CCleanNTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCleanNTDlg)
	m_sOEM1 = _T("");
	m_sOEM2 = _T("");
	m_sOEM3 = _T("");
	m_sCompName = _T("");
	m_sOrga = _T("");
	m_sLoaderTimeout = _T("");
	m_sDeskFolder = _T("");
	m_sFileList = _T("");
	m_sSysOverview = _T("");
	m_sInfo_Aenderungen = _T("");
	m_sInfo_KeineAenderungen = _T("");
	m_sInfo_KeineOrdnerDatei = _T("");
	m_sInfo_OrdnerDatei = _T("");
	m_sStatus = _T("");
	m_sResult = _T("");
	m_sOSInUse = _T("");
	m_sNero1 = _T("1502");
	m_sNero2 = _T("4240");
	m_sNero3 = _T("1125");
	m_sNero4 = _T("0482");
	m_sNero5 = _T("1088");
	m_sNero6 = _T("9784");
	m_sEditXP = _T("");
	m_sHDD = _T("");
	m_sHDD2 = _T("");
	m_sSetHDDID = _T("");
	//}}AFX_DATA_INIT

	m_pThis					= this;
	m_pIdipCleanDlg			= pIdip;
	m_hDiagnose				= NULL;
	m_hBerichtSpeichern		= NULL;
	m_hBerichtSpeichernMsg	= NULL;
	m_hErrorMsgBox			= NULL;
	m_hBerichtErstellenDlg	= NULL;

	InitStrings();

	m_bFoundDlg				= FALSE;
	m_bBerichtEnde			= FALSE;
	m_sSysOverview			= _T("S.txt"); 
	m_sFileList				= _T("L.txt");
	m_dwDruckenButton		= 0x0592;
	m_dwDatei				= 0x3AB6;
	m_dwAlleRegs			= 0x3AA7;
	m_dwZusammenfassung		= 0x3AAC;
	m_dwDruckButton			= 0x0001;
	m_dwEditDateiName		= 0x0480;
	m_dwSpeicherButton		= 0x0001;

	m_STATUS.bAenderungen = FALSE;
	m_STATUS.bOrdnerDatei = FALSE;

	m_sInfo_Aenderungen		= _T("Einträge geändert !");
	m_sInfo_KeineAenderungen= _T("Keine Einträge geändert !");
	m_sInfo_OrdnerDatei		= _T("Ordner bzw. Datei(en) erstellt !");
	m_sInfo_KeineOrdnerDatei= _T("Kein Ordner bzw. keine Datei(en) erstellt !");
	m_sSetHDDID				= _T("Keine neue HDD-ID gesetzt !");

	m_bSaveAsSerialNumberKey = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCleanNTDlg)
	DDX_Control(pDX, IDC_SET_HDDID, m_ctrlSetHDDID);
	DDX_Control(pDX, IDC_SYSTEM_OVERVIEW, m_ctrlSysOverview);
	DDX_Control(pDX, IDC_FILE_LIST, m_ctrlFileList);
	DDX_Control(pDX, IDC_STATIC_CREATE, m_ctrlCreate);
	DDX_Control(pDX, IDC_CHECK_HDD, m_ctrlSaveHDDID);
	DDX_Control(pDX, IDC_STATIC_LIST, m_ctrlStaticList);
	DDX_Control(pDX, IDC_STATIC_SYS, m_ctrlStaticSys);
	DDX_Control(pDX, IDC_STATIC_ORDNER, m_ctrlStaticOrdner);
	DDX_Control(pDX, IDC_STATIC_HDD_LINE, m_ctrlHDDLine);
	DDX_Control(pDX, IDC_EDIT_HDD2, m_ctrlHDD2);
	DDX_Control(pDX, IDC_EDIT_HDD, m_ctrlHDD);
	DDX_Control(pDX, IDC_STATIC_HDD, m_ctrlStaticHDD);
	DDX_Control(pDX, IDC_STATIC_SECONDS, m_ctrlSeconds);
	DDX_Control(pDX, IDC_LOADERTIMEOUT, m_ctrlLoaderTimeout);
	DDX_Control(pDX, IDC_STATIC_LOADER, m_ctrlLoader);
	DDX_Control(pDX, IDC_UEBERNEHMEN, m_ctrlUebernehmen);
	DDX_Control(pDX, IDC_STATIC_NERO, m_ctrlStaticNero);
	DDX_Control(pDX, IDC_EDIT_NERO6, m_ctrlNero6);
	DDX_Control(pDX, IDC_EDIT_NERO5, m_ctrlNero5);
	DDX_Control(pDX, IDC_EDIT_NERO4, m_ctrlNero4);
	DDX_Control(pDX, IDC_EDIT_NERO3, m_ctrlNero3);
	DDX_Control(pDX, IDC_EDIT_NERO2, m_ctrlNero2);
	DDX_Control(pDX, IDC_EDIT_NERO1, m_ctrlNero1);
	DDX_Control(pDX, IDC_EDIT_XP, m_ctrlEditXPE);
	DDX_Control(pDX, IDC_STATIC_XPE, m_ctrlStaticXPE);
	DDX_Control(pDX, IDC_STATIC_OEM_MIDDLE, m_ctrlStaticOEMMiddle);
	DDX_Control(pDX, IDC_STATIC_OEM, m_ctrlStaticOEM);
	DDX_Control(pDX, IDC_OS_IN_USE, m_ctrlOSInUse);
	DDX_Control(pDX, IDC_RESULT, m_cResult);
	DDX_Control(pDX, IDC_STATUS, m_cStatus);
	DDX_Control(pDX, IDC_ORDNERDATEI, m_cInfo_OrdnerDatei);
	DDX_Control(pDX, IDC_KEINE_ORDNERDATEI, m_cInfo_KeineOrdnerDatei);
	DDX_Control(pDX, IDC_KEINE_AENDERUNGEN, m_cInfo_KeineAenderungen);
	DDX_Control(pDX, IDC_AENDERUNGEN, m_cInfo_Aenderungen);
	DDX_Control(pDX, IDC_ORGA, m_ControlOrga);
	DDX_Control(pDX, IDC_COMPNAME, m_ControlCompName);
	DDX_Control(pDX, IDC_OEM3, m_ControlOEM3);
	DDX_Control(pDX, IDC_OEM2, m_ControlOEM2);
	DDX_Control(pDX, IDC_OEM1, m_ControlOEM1);
	DDX_Control(pDX, IDC_CHECK_SYSTEM_OVERVIEW, m_CheckSystemOverview);
	DDX_Control(pDX, IDC_CHECK_FILE_LIST, m_CheckFileList);
	DDX_Control(pDX, IDC_CHECK_DESK_FOLDER, m_CheckDeskFolder);
	DDX_Control(pDX, IDC_DESK_FOLDER, m_ControlDeskFolder);
	DDX_Text(pDX, IDC_OEM1, m_sOEM1);
	DDX_Text(pDX, IDC_OEM2, m_sOEM2);
	DDX_Text(pDX, IDC_OEM3, m_sOEM3);
	DDX_Text(pDX, IDC_COMPNAME, m_sCompName);
	DDX_Text(pDX, IDC_ORGA, m_sOrga);
	DDX_Text(pDX, IDC_LOADERTIMEOUT, m_sLoaderTimeout);
	DDX_Text(pDX, IDC_DESK_FOLDER, m_sDeskFolder);
	DDX_Text(pDX, IDC_FILE_LIST, m_sFileList);
	DDX_Text(pDX, IDC_SYSTEM_OVERVIEW, m_sSysOverview);
	DDX_Text(pDX, IDC_AENDERUNGEN, m_sInfo_Aenderungen);
	DDX_Text(pDX, IDC_KEINE_AENDERUNGEN, m_sInfo_KeineAenderungen);
	DDX_Text(pDX, IDC_KEINE_ORDNERDATEI, m_sInfo_KeineOrdnerDatei);
	DDX_Text(pDX, IDC_ORDNERDATEI, m_sInfo_OrdnerDatei);
	DDX_Text(pDX, IDC_STATUS, m_sStatus);
	DDX_Text(pDX, IDC_RESULT, m_sResult);
	DDX_Text(pDX, IDC_OS_IN_USE, m_sOSInUse);
	DDX_Text(pDX, IDC_EDIT_NERO1, m_sNero1);
	DDV_MaxChars(pDX, m_sNero1, 4);
	DDX_Text(pDX, IDC_EDIT_NERO2, m_sNero2);
	DDV_MaxChars(pDX, m_sNero2, 4);
	DDX_Text(pDX, IDC_EDIT_NERO3, m_sNero3);
	DDV_MaxChars(pDX, m_sNero3, 4);
	DDX_Text(pDX, IDC_EDIT_NERO4, m_sNero4);
	DDV_MaxChars(pDX, m_sNero4, 4);
	DDX_Text(pDX, IDC_EDIT_NERO5, m_sNero5);
	DDV_MaxChars(pDX, m_sNero5, 4);
	DDX_Text(pDX, IDC_EDIT_NERO6, m_sNero6);
	DDV_MaxChars(pDX, m_sNero6, 4);
	DDX_Text(pDX, IDC_EDIT_XP, m_sEditXP);
	DDX_Text(pDX, IDC_EDIT_HDD, m_sHDD);
	DDV_MaxChars(pDX, m_sHDD, 4);
	DDX_Text(pDX, IDC_EDIT_HDD2, m_sHDD2);
	DDV_MaxChars(pDX, m_sHDD2, 4);
	DDX_Text(pDX, IDC_SET_HDDID, m_sSetHDDID);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCleanNTDlg, CDialog)
	//{{AFX_MSG_MAP(CCleanNTDlg)
	ON_BN_CLICKED(IDC_UEBERNEHMEN, OnUebernehmen)
	ON_EN_CHANGE(IDC_OEM1, OnChangeOem1)
	ON_EN_CHANGE(IDC_OEM2, OnChangeOem2)
	ON_EN_CHANGE(IDC_OEM3, OnChangeOem3)
	ON_EN_CHANGE(IDC_COMPNAME, OnChangeCompname)
	ON_EN_CHANGE(IDC_ORGA, OnChangeOrga)
	ON_EN_SETFOCUS(IDC_OEM2, OnSetfocusOem2)
	ON_EN_SETFOCUS(IDC_OEM3, OnSetfocusOem3)
	ON_EN_SETFOCUS(IDC_ORGA, OnSetfocusOrga)
	ON_EN_SETFOCUS(IDC_COMPNAME, OnSetfocusCompname)
	ON_BN_CLICKED(IDC_CHECK_SYSTEM_OVERVIEW, OnCheckSystemOverview)
	ON_BN_CLICKED(IDC_CHECK_FILE_LIST, OnCheckFileList)
	ON_EN_KILLFOCUS(IDC_OEM1, OnKillfocusOem1)
	ON_EN_KILLFOCUS(IDC_OEM2, OnKillfocusOem2)
	ON_EN_KILLFOCUS(IDC_OEM3, OnKillfocusOem3)
	ON_EN_CHANGE(IDC_EDIT_NERO1, OnChangeEditNero1)
	ON_EN_CHANGE(IDC_EDIT_NERO2, OnChangeEditNero2)
	ON_EN_CHANGE(IDC_EDIT_NERO3, OnChangeEditNero3)
	ON_EN_CHANGE(IDC_EDIT_NERO4, OnChangeEditNero4)
	ON_EN_CHANGE(IDC_EDIT_NERO5, OnChangeEditNero5)
	ON_EN_CHANGE(IDC_EDIT_NERO6, OnChangeEditNero6)
	ON_EN_KILLFOCUS(IDC_EDIT_NERO1, OnKillfocusEditNero1)
	ON_EN_KILLFOCUS(IDC_EDIT_NERO2, OnKillfocusEditNero2)
	ON_EN_KILLFOCUS(IDC_EDIT_NERO3, OnKillfocusEditNero3)
	ON_EN_KILLFOCUS(IDC_EDIT_NERO4, OnKillfocusEditNero4)
	ON_EN_KILLFOCUS(IDC_EDIT_NERO5, OnKillfocusEditNero5)
	ON_EN_KILLFOCUS(IDC_EDIT_NERO6, OnKillfocusEditNero6)
	ON_EN_CHANGE(IDC_EDIT_XP, OnChangeEditXp)
	ON_EN_CHANGE(IDC_LOADERTIMEOUT, OnChangeLoadertimeout)
	ON_BN_CLICKED(IDC_CHECK_DESK_FOLDER, OnCheckDeskFolder)
	ON_EN_CHANGE(IDC_EDIT_HDD, OnChangeEditHdd)
	ON_EN_CHANGE(IDC_EDIT_HDD2, OnChangeEditHdd2)
	ON_BN_CLICKED(IDC_CHECK_HDD, OnCheckHdd)
	ON_BN_CLICKED(IDC_BTN_CF_INIT, OnBnClickedBtnCfInit)
	ON_BN_CLICKED(IDC_BTN_INIT_DISKS, OnBnClickedBtnInitDisks)
	ON_BN_CLICKED(IDC_BTN_CF_UNINIT, OnBnClickedBtnCfUninit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CCleanNTDlg message handlers
//////////////////////////////////////////////////////////////////////
BOOL CCleanNTDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ControlOEM1.SetLimitText(5);	
	m_ControlOEM2.SetLimitText(7);
	m_ControlOEM3.SetLimitText(5);
	m_ControlCompName.SetLimitText(12);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::Start()
{
	InitCleanNTDlg();
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::InitStrings()
{
	LANGID iLanguageID = GetSystemDefaultLangID();
	CString sWinNTPath;
	int iBuf = 260;

	OS_TYPE OsRet(OS_NOT);
	OSVERSIONINFO	osinfo;

	osinfo.dwOSVersionInfoSize = sizeof(osinfo);
	GetVersionEx(&osinfo);
	
	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		CString s;
		if(osinfo.dwMajorVersion  > 4)
		{
			OsRet = OS_XP;
			if(osinfo.dwMinorVersion == 0)
			{
				OsRet = OS_2K;
			}
		}
		else
		{
			OsRet = OS_NT;
		}
	}

	if(OsRet == OS_XP)
	{
		GetSystemDirectory(sWinNTPath.GetBuffer(iBuf),
						  iBuf);
		sWinNTPath.ReleaseBuffer();
		m_sSystem32Path = sWinNTPath;
		CVipCleanReg reg;

		m_sPathToDesktop		= reg.GetKey(_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),_T("Common Desktop"));
		m_sPathToDesktop		+= _T("\\");
		m_sPathToMSInfo			= reg.GetKey(_T("SOFTWARE\\Microsoft\\Shared Tools"), _T("SharedFilesDir"));
		m_sPathToMSInfo			+= _T("MSInfo\\MSInfo32.exe");
	}
	else 
	{
		GetSystemDirectory(sWinNTPath.GetBuffer(iBuf),
						  iBuf);
		sWinNTPath.ReleaseBuffer();
		m_sSystem32Path = sWinNTPath;
		sWinNTPath = sWinNTPath.Left(sWinNTPath.ReverseFind(_T('\\')));

		switch(iLanguageID)
		{
			case 0x0409: // US English
				m_sPathToDesktop = sWinNTPath + _T("\\profiles\\all users\\desktop\\");
				m_sDiagnoseDlg			= _T("Windows NT Diagnostics");
				m_sBerichtDlg			= _T("Create Report");
				m_sBerichtSpeichern		= _T("Save WinMSD Report");
				m_sBerichtErstellenDlg	= _T("Create Report");
				break;

			case 0x0407: // deutsch
				m_sPathToDesktop		= sWinNTPath + _T("\\profiles\\all users\\desktop\\");	
				m_sDiagnoseDlg			= _T("Windows NT-Diagnose");
				m_sBerichtDlg			= _T("Bericht erstellen");
				m_sBerichtSpeichern		= _T("WinMSD-Bericht speichern");
				m_sBerichtErstellenDlg	= _T("Bericht erstellen");

				break;
			
			default:
				break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::InitCleanNTDlg()
{

	CString sValue; 

	//OEM Nummer holen;
	sValue=m_Reg.GetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_PRODUCTID);
	
	m_sOEM1 = _T("11111");
	m_sOEM2 = _T("1111111");
	m_sOEM3 = _T("11111");
	m_sOEMMid = _T("111");
	m_NTDefaults.sOEM1 = m_sOEM1;
	m_NTDefaults.sOEM2 = m_sOEM2;
	m_NTDefaults.sOEM3 = m_sOEM3;

	if(sValue.GetLength() > 1)
	{
		m_sOEM1 = sValue.Left(5);
		m_sOEM2 = sValue.Mid(8,7);
		m_sOEM3 = sValue.Mid(15,5);
		m_sOEMMid = sValue.Mid(5,3);

		m_NTDefaults.sOEM1 = m_sOEM1;
		m_NTDefaults.sOEM2 = m_sOEM2;
		m_NTDefaults.sOEM3 = m_sOEM3;
	}

	//Computername holen
	sValue = m_Reg.GetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME);

/*	sValue = m_Reg.GetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_SERIALNUMBER);
	if (sValue.IsEmpty())
	{
		sValue = m_Reg.GetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME);
		m_bSaveAsSerialNumberKey = FALSE;
	}
	else
	{
		m_bSaveAsSerialNumberKey = TRUE;
	}
*/
	m_sCompName = sValue;
	m_sSysOverview = sValue+m_sSysOverview;
	m_sFileList = sValue+m_sFileList;
	m_NTDefaults.sCompName = m_sCompName;
	m_NTDefaults.sSysOverview = m_sSysOverview;
	m_NTDefaults.sFileList = m_sFileList;

	//NT Loader Timeout holen
	m_sLoaderTimeout = GetNTLoaderTimeout();
	m_NTDefaults.sLoaderTimeout = m_sLoaderTimeout;

	//setze Name für anzulegenden Ordner auf Desktop
	m_sDeskFolder = sValue;
	m_NTDefaults.sDeskFolder = m_sDeskFolder;
	
	//Organisation holen
	sValue = m_Reg.GetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDORGANIZATION);
	m_sOrga = sValue;
	m_NTDefaults.sOrga = m_sOrga;

	//hole NT Wartesekunden bis Start
	sValue.Format(_T("%i"), m_Reg.GetNTLoaderTimeout());
	m_sLoaderTimeout = sValue;
	m_NTDefaults.sLoaderTimeout = m_sLoaderTimeout;

	GetXPDataFromReg();

	m_bSaveHDD = FALSE;

}
/////////////////////////////////////////////////////////////////////////////
BOOL CCleanNTDlg::StartWinDiagnose(CString sWinDiagnose)
{
	BOOL bProcess;
	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	CString sPrgToStart;

	startUpInfo.cb				= sizeof(STARTUPINFO);
    startUpInfo.lpReserved      = NULL;
    startUpInfo.lpDesktop       = NULL;
    startUpInfo.lpTitle			= NULL;
    startUpInfo.dwX				= 0;
    startUpInfo.dwY				= 0;
    startUpInfo.dwXSize			= 0;
    startUpInfo.dwYSize			= 0;
    startUpInfo.dwXCountChars   = 0;
    startUpInfo.dwYCountChars   = 0;
    startUpInfo.dwFillAttribute = 0;
	startUpInfo.dwFlags			= STARTF_USESHOWWINDOW;
    startUpInfo.wShowWindow     = SW_SHOW; 
    startUpInfo.cbReserved2     = 0;
    startUpInfo.lpReserved2     = NULL;

	CString sExe = m_sSystem32Path + _T("\\winmsd.exe ");


	bProcess = CreateProcess(
				(LPCTSTR)sExe,				// pointer to name of executable module 
				(LPTSTR)sExe.GetBuffer(0),		// pointer to command line string
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to process security attributes 
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to thread security attributes 
				FALSE,						// handle inheritance flag 
				NORMAL_PRIORITY_CLASS,		//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
				NULL,						// pointer to new environment block 
				NULL,						// pointer to current directory name 
				&startUpInfo,				// pointer to STARTUPINFO 
				&prozessInformation 		// pointer to PROCESS_INFORMATION  
							);
	sExe.ReleaseBuffer();	

	if (bProcess && prozessInformation.hProcess != NULL)
	{
		DWORD dwWFII;
		dwWFII = WaitForInputIdle(prozessInformation.hProcess,15*1000);
		if(dwWFII == WAIT_TIMEOUT || dwWFII == -1)
		{
			bProcess = FALSE;
		}
	}
	return bProcess;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCleanNTDlg::StartWinDiagnoseXP(CString sWinDiagnose)
{
	BOOL bProcess;
	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	CString sPrgToStart;

	startUpInfo.cb				= sizeof(STARTUPINFO);
    startUpInfo.lpReserved      = NULL;
    startUpInfo.lpDesktop       = NULL;
    startUpInfo.lpTitle			= NULL;
    startUpInfo.dwX				= 0;
    startUpInfo.dwY				= 0;
    startUpInfo.dwXSize			= 0;
    startUpInfo.dwYSize			= 0;
    startUpInfo.dwXCountChars   = 0;
    startUpInfo.dwYCountChars   = 0;
    startUpInfo.dwFillAttribute = 0;
	startUpInfo.dwFlags			= STARTF_USESHOWWINDOW;
    startUpInfo.wShowWindow     = SW_SHOW; 
    startUpInfo.cbReserved2     = 0;
    startUpInfo.lpReserved2     = NULL;

	CString sExe = sWinDiagnose;
	CString sCommandLine =  m_sMSInfoCommandLineParam;

	bProcess = CreateProcess(
				(LPCTSTR)sExe,				// pointer to name of executable module 
				(LPTSTR)sCommandLine.GetBuffer(0),		// pointer to command line string
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to process security attributes 
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to thread security attributes 
				FALSE,						// handle inheritance flag 
				NORMAL_PRIORITY_CLASS,		//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
				NULL,						// pointer to new environment block 
				NULL,						// pointer to current directory name 
				&startUpInfo,				// pointer to STARTUPINFO 
				&prozessInformation 		// pointer to PROCESS_INFORMATION  
							);
	sExe.ReleaseBuffer();	
	sCommandLine.ReleaseBuffer();

	if (bProcess && prozessInformation.hProcess != NULL)
	{
		DWORD dwWFII;
		dwWFII = WaitForInputIdle(prozessInformation.hProcess,15*1000);
		if(dwWFII == WAIT_TIMEOUT || dwWFII == -1)
		{
			bProcess = FALSE;
		}

		DWORD dwWFSO = WaitForSingleObject(prozessInformation.hThread, 60*1000);
		if(dwWFSO == WAIT_OBJECT_0)
		{
			TRACE(_T("Thread beendet\n"));
		}
	}

	return bProcess;
}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::ClickDruckenButton()
{
	HWND hFirstChild = ::GetWindow(m_hDiagnose, GW_CHILD);
	//click Drucken-Button
	if(hFirstChild && IsWindow(hFirstChild))
	{
		::PostMessage(m_hDiagnose,WM_COMMAND,
					(WPARAM)MAKELONG(m_dwDruckenButton,BN_CLICKED),
					(LPARAM)hFirstChild);
	}	
}

//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnOK()
{

}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnCancel()
{

}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::SaveWinDiagnose()
{
	HWND hFirstChild = ::GetWindow(m_hBerichtErstellenDlg, GW_CHILD);
	//click Druck-Button
//	Sleep(500);
	if(hFirstChild && IsWindow(hFirstChild))
	{
		::PostMessage(m_hBerichtErstellenDlg,WM_COMMAND,
					(WPARAM)MAKELONG(m_dwDruckButton,BN_CLICKED),
					(LPARAM)hFirstChild);
	}	

	DWORD dwTick = GetTickCount();
	do
	{	m_hBerichtSpeichern = ::FindWindow(NULL, m_sBerichtSpeichern);
		Sleep(100);
//		TRACE(_T("m_hBerichtSpeichern: 0x%x\n"), m_hBerichtSpeichern);
	}while(!m_hBerichtSpeichern && GetTickCount()-dwTick < 5*1000);

 
	if(m_hBerichtSpeichern && IsWindow(m_hBerichtSpeichern))
	{
		TRACE(_T("Druck Dlg gefunden\n"));
		HWND hEditName = ::GetDlgItem(m_hBerichtSpeichern, m_dwEditDateiName);
		TRACE(_T("hEditName: 0x%x\n"), hEditName);

		//sleep muss hier rein, da das folgende Send ohne sleep zwar ein TRUE
		//zurück liefert, der gesendete Text aber im Editfield nicht ankommt, warum???
		Sleep(500);
		CString sPath = m_sPathToDesktop+m_NTNewValues.sDeskFolder+_T("\\")+m_NTNewValues.sSysOverview;
		if(::SendMessage(hEditName,
						 WM_SETTEXT, 
						 0, 
						 (LPARAM)(LPCTSTR)(sPath)))
		{
			HWND hSpeichern = ::GetDlgItem(m_hBerichtSpeichern, m_dwSpeicherButton);
			if(hSpeichern && IsWindow(hSpeichern))
			{
				::PostMessage(m_hBerichtSpeichern,WM_COMMAND,
						(WPARAM)MAKELONG(m_dwSpeicherButton,BN_CLICKED),
						(LPARAM)hSpeichern);
			}

			//Suche Bericht speichern Dlg der erscheint, falls zu speichernde
			//Datei schon existiert. Diese soll dann überschrieben werden
			//Ebenfalls Suche Diagnose Msg-Bos die z.B. erscheint, wenn die 
			//Netzwerkstatistic nicht zu Verfügung steht. Diese Msg-Box wird geschlossen
			m_bFoundDlg = FALSE;
			m_bBerichtEnde = FALSE;
			LPARAM lParam = 3;
			DWORD dwTick = GetTickCount();
			do
			{
				lParam = 3;
				EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, lParam);
				lParam = 2;
				EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, lParam);
				Sleep(100);
			}while(!m_bBerichtEnde && GetTickCount()-dwTick < 10000);
			TRACE(_T("Zeit für Bericht: %d\n"), GetTickCount()-dwTick);
		}
	}

	if(GetTickCount()-dwTick > 5*1000)
	{
//TODO Fehlermeldung DruckDlg nicht gefunden
		TRACE(_T("Systemübersicht konnte nicht erstellt werden\n"));
	}

}

//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::EndWinDiagnose()
{
	::PostMessage(m_hDiagnose, WM_CLOSE, 0, 0);
}
//////////////////////////////////////////////////////////////////////
CString CCleanNTDlg::GetNTLoaderTimeout()
{
	CString sRet;
	sRet.Format(_T("%i"), m_Reg.GetNTLoaderTimeout());
	return sRet;

}
//////////////////////////////////////////////////////////////////////
BOOL CALLBACK CCleanNTDlg::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{

	BOOL bRet = TRUE;
	if (hWnd && IsWindow(hWnd))
	{

		CString sText;
		::GetWindowText(hWnd,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
		sText.ReleaseBuffer();

		switch(lParam)
		{
		case 0: //Suche Diagnose Dlg
			if(sText.Left(19) == m_pThis->GetStringDiagnoseDlg().Left(19))
			{
				m_pThis ->SetHandleDiagnoseDlg(hWnd);
				TRACE(_T("DiagnoseDlg gefunden\n"));
				bRet = FALSE;
			}
			break;

		case 1: //Suche Bericht erstellen - Dlg
			if(sText.Left(10) == m_pThis->GetStringBerichtErstellenDlg().Left(10))
			{
				m_pThis->SetHandleBerichtErstellenDlg(hWnd);
				TRACE(_T("BerichtDlg gefunden\n"));
				bRet = FALSE;
			}
			break;

		case 2: //Suche Bericht speichern Dlg der erscheint, falls zu speichernde
				//Datei schon existiert. Diese soll dann überschrieben werden
				//Ebenfalls Suche Diagnose Msg-Box die z.B. erscheint, wenn die 
				//Netzwerkstatistik nicht zu Verfügung steht. 
				//Diese Msg-Box wird dann geschlossen
			if(sText == m_pThis->GetStringBerichtSpeichernDlg())
			{
				if(m_pThis->m_hBerichtSpeichern != hWnd)
				{
					m_pThis->SetHandleBerichtSpeichernMsg(hWnd);
					TRACE(_T("BerichtSpeichernMsg gefunden\n"));
					if(hWnd && IsWindow(hWnd))
					{
						DWORD dwJa = 0x0006;  //ja-Button
						HWND hJa = ::GetDlgItem(hWnd, dwJa);
						if(hJa && IsWindow(hJa))
						{
							::SendMessage(hWnd,WM_COMMAND,
								(WPARAM)MAKELONG(dwJa,BN_CLICKED),
								(LPARAM)hJa);
						}
					}
				}
			}

			if(sText.Left(19) == m_pThis->GetStringDiagnoseDlg().Left(19))
			{
				if(m_pThis->GetHandleDiagnoseDlg() != hWnd)
				{
					m_pThis->SetHandleErrorMsgBox(hWnd);
					::PostMessage(hWnd, WM_CLOSE, 0, 0);
				}
			}
			break;
		case 3:
			//Suche Dlg "WinMSD-Bericht erstellen"
			//Wenn gefunden, warten, bis er wieder verschwunden ist. Dann ist 
			//die Bericht-Erstellung beendet
			TRACE(_T("Name: %s\n"), m_pThis->GetStringBerichtErstellenDlg());
			if(sText.Left(24) == m_pThis->GetStringBerichtErstellenDlg().Left(24))
			{
				TRACE(_T("WinMSD-Bericht erstellen gefunden\n"));
				m_pThis->SetHandleBerichtErstellenDlg(hWnd);
				m_pThis->SetBoolFoundDlg(TRUE);
			}

			//Bericht erstellen Dlg war da und ist wieder weg
			//Berichterstellung ist somit beendet
			if(   m_pThis->GetBoolFoundDlg() 
			   && !IsWindow(m_pThis->GetHandleBerichtErstellenDlg()))
			{
				TRACE(_T("Berichterstellung beendet\n"));
				m_pThis->SetBoolBerichtEnde(TRUE);
				bRet = FALSE;
			}
			break;
		default:
			break;

		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnUebernehmen() 
{

	//mache alle abschließenden Kommentarzeilen unsichtbar
	m_cInfo_Aenderungen.ShowWindow(SW_HIDE);
	m_cInfo_KeineAenderungen.ShowWindow(SW_HIDE);
	m_cInfo_OrdnerDatei.ShowWindow(SW_HIDE);
	m_cInfo_KeineOrdnerDatei.ShowWindow(SW_HIDE);
	m_cResult.ShowWindow(SW_HIDE);

	CString sMsgBox, sWndTitle, sText;
	BOOL bSaveAllEntries = TRUE;
	BOOL bSaveSameNames = TRUE;
	BOOL bSaveNTCheckBoxes = TRUE;
	int iResult = IDYES;
//	int iCount = 0;

	//checke folgende Einträge, ob sie geändert wurden bei NT:
	//- OEM (Teil 1, 2 und 3)
	//- Computername
	//- Organisation
	
	//bei XP braucht OEM nicht gecheckt zu werden, sondern folgende Einträge
	//- XPe Lizenz
	//- Nero Lizenz
	//- Computername
	//- Organisation
	if(!Check_AllEntries(iResult)) 
	{
		if(iResult == IDNO)
		{
			bSaveAllEntries = FALSE;
			TRACE(_T("no AllEntries\n"));
		}
	}

	iResult = IDYES;
	if(m_sOSInUse == _T("NT Einstellungen"))
	{
		//Check, ob Erstellen von Desktopordner, Dateiübersicht und Dateiliste(C:\) gewählt
		if(!Check_NTCheckBoxes(iResult))
		{
			if(iResult == IDNO)
			{
				bSaveNTCheckBoxes = FALSE;
				TRACE(_T("no NTCheckBoxes\n"));
			}
			else
			{
				//Check, ob die Namensgebungen des Desktopordners, Dateiübersicht 
				//und Dateiliste mit Computernamen übereinstimmt
				if(!Check_SameNames(iResult))
				{
					if(iResult == IDNO)
					{
						bSaveSameNames = FALSE;
						TRACE(_T("no Samenames\n"));
					}
				}
			}
		}
		else
		{
			//Check, ob die Namensgebungen des Desktopordners, Dateiübersicht 
			//und Dateiliste mit Computernamen übereinstimmt
			if(!Check_SameNames(iResult))
			{
				if(iResult == IDNO)
				{
					bSaveSameNames = FALSE;
					TRACE(_T("no Samenames\n"));
				}
			}
		}
	}
	else
	{
		//neue Festplatten ID speichern
		if(m_ctrlSaveHDDID.GetCheck())
		{
			m_bSaveHDD = TRUE;
		}

		if(    m_NTNewValues.sCompName != m_NTDefaults.sCompName
			&& !m_ctrlSaveHDDID.GetCheck())
		{
			int iResult = 0;
			CString sWndTitle = _T("Neue HDD - ID");
			CString sText = _T("ACHTUNG:\r\nDie neue HDD - ID wird nicht gespeichert!\r\n");
			CString sMsgBox = _T("\r\nKorrekt? ");

			iResult = MessageBox(sText+sMsgBox, sWndTitle, MB_YESNO | MB_ICONSTOP);
			if(iResult == IDNO)
			{
				bSaveAllEntries = FALSE;
			}
		}
	}


	if(bSaveAllEntries) //save OEM, Computername und Orga
	{
		SaveAllEntries();
		m_STATUS.bAenderungen = TRUE;
		TRACE(_T("Einträge speicher\n"));

		if(m_sOSInUse == _T("NT Einstellungen"))
		{
			if(bSaveNTCheckBoxes)
			{
				TRACE(_T("Checkboxen korrekt markiert\n"));
				if(bSaveSameNames)
				{	
					//speicher den Desktop Ordner/ Systemübersicht/ Filelist von C:
					TRACE(_T("Namen speicher\n"));
					SaveSameNames();
				}
				else
				{
					TRACE(_T("Namen NICHT speicher\n"));
				}
			}
			else
			{
				TRACE(_T("Checkboen NICHT korrekt markiert\n"));
			}
		}

	}
	else
	{
		TRACE(_T("Einträge nicht speicher\n"));

	}

	m_ctrlUebernehmen.EnableWindow(FALSE);
	m_pIdipCleanDlg->m_cOK.EnableWindow();

	ShowStatus();

	m_pIdipCleanDlg->SetAenderungenClicked(TRUE);

}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::ShowStatus()
{
	m_sResult = _T("Ergebnis:");
	UpdateData(FALSE);
	m_cResult.ShowWindow(SW_SHOW);
	if(m_STATUS.bAenderungen)
	{
		m_cInfo_Aenderungen.ShowWindow(SW_SHOW);
	}
	else
	{
		m_cInfo_KeineAenderungen.ShowWindow(SW_SHOW);
	}

	if(m_sOSInUse == _T("NT Einstellungen"))
	{
		if(m_STATUS.bOrdnerDatei)
		{

			m_cInfo_OrdnerDatei.ShowWindow(SW_SHOW);
		}
		else
		{
			m_cInfo_KeineOrdnerDatei.ShowWindow(SW_SHOW);
		}
	}
	else
	{
		m_ctrlSetHDDID.ShowWindow(SW_SHOW);
	}


	UpdateWindow();
}
//////////////////////////////////////////////////////////////////////
//wurde in allen Feldern (Abschnitt: Einträge ändern)der Inhalt geändert?  
BOOL CCleanNTDlg::Check_AllEntries(int &iResult)
{
	BOOL bRet = TRUE;
	GetNewEntries();
	int iCount = 0;
	CString sMsgBox;
	CString sValue;

	if(m_sOSInUse == _T("NT Einstellungen"))
	{
		if(m_NTNewValues.sOEM1 == m_NTDefaults.sOEM1)
		{
			sMsgBox += _T("- Teil 1 der OEM\r\n");
			iCount++;
			bRet = FALSE;
		}
		if(m_NTNewValues.sOEM2 == m_NTDefaults.sOEM2)
		{
			sMsgBox += _T("- Teil 2 der OEM\r\n");
			iCount++;
			bRet = FALSE;
		}
		if(m_NTNewValues.sOEM3 == m_NTDefaults.sOEM3)
		{
			sMsgBox += _T("- Teil 3 der OEM\r\n");
			iCount++;
			bRet = FALSE;
		}

		sValue = _T("NT Einträge");
	}
	else
	{

		if(m_NTNewValues.sXPeLicence == m_NTDefaults.sXPeLicence)
		{
			sMsgBox += _T("- XPe Lizenz\n");
			iCount++;
			bRet = FALSE;
		}

		if(	   m_NTNewValues.sNero1 == m_NTDefaults.sNero1
			&& m_NTNewValues.sNero2 == m_NTDefaults.sNero2
			&& m_NTNewValues.sNero3 == m_NTDefaults.sNero3
			&& m_NTNewValues.sNero4 == m_NTDefaults.sNero4
			&& m_NTNewValues.sNero5 == m_NTDefaults.sNero5
			&& m_NTNewValues.sNero6 == m_NTDefaults.sNero6)

		{
			sMsgBox += _T("- Nero Lizenz\n");
			iCount++;
			bRet = FALSE;
		}
				
		sValue = _T("XP Einträge");
	}

	if(m_NTNewValues.sCompName == m_NTDefaults.sCompName)
	{
		sMsgBox += _T("- Computername\n");
		iCount++;
		bRet = FALSE;
	}
	if(m_NTNewValues.sOrga == m_NTDefaults.sOrga)
	{
		sMsgBox += _T("- Organisation\n");
		iCount++;
		bRet = FALSE;
	}


	if(!bRet)
	{
		CString sWndTitle = _T("Einträge nicht geändert !");
		CString sText;
		if(iCount > 1)
		{
			sText.Format(_T("ACHTUNG:\r\nFolgende %s wurden nicht geändert:\r\n\r\n"), sValue);
		}
		else if(iCount == 1)
		{
			sText.Format(_T("ACHTUNG:\r\nFolgender %s wurden nicht geändert:\r\n\r\n"),sValue);
		}

		sMsgBox += _T("\r\nSollen diese Werte unverändert übernommen werden?");
		iResult = MessageBox(sText+sMsgBox, sWndTitle, MB_YESNO | MB_ICONSTOP);
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
//Prüft, ob Namen der Dateien SystemÜbersicht, Filelist und DesktopFolder
//mit dem Computernamen übereinstimmen
BOOL CCleanNTDlg::Check_SameNames(int &iResult)
{
	BOOL bRet = TRUE;
	CString sMsgBox;
	int iCount = 0;
	CString sShortSysOverview = m_NTNewValues.sSysOverview.Left(m_NTNewValues.sSysOverview.GetLength()-5);
	CString sShortFileList = m_NTNewValues.sFileList.Left(m_NTNewValues.sFileList.GetLength()-5);
	if(m_NTNewValues.sDeskFolder != m_NTNewValues.sCompName)
	{
		sMsgBox += _T("\r\n- Desktop Ordner");
		iCount++;
		bRet = FALSE;
	}
	if(sShortSysOverview != m_NTNewValues.sCompName)
	{
		sMsgBox += _T("\r\n- Systemübersicht");
		iCount+=2;
		bRet = FALSE;
	}
	if(sShortFileList != m_NTNewValues.sCompName)
	{
		sMsgBox += _T("\r\n- Dateiliste (C:\\)");
		iCount+=4;
		bRet = FALSE;
	}

	if(!bRet)
	{
		CString sText;
		CString sWndTitle = _T("Keine Übereinstimmung !");
		switch(iCount)
		{
		case 1:
			sText = _T("ACHTUNG:\r\nFolgender Eintrag stimmt mit dem Computernamen nicht überein:\r\n");
			sMsgBox += _T("\r\n\r\nSoll der Desktop Ordner trotzdem angelegt werden ?");
			break;
		case 2:
		case 4: 
			sText = _T("ACHTUNG:\r\nFolgende Eintrag enthält nicht den Computernamen:\r\n");
			sMsgBox += _T("\r\n\r\nSoll diese Datei trotzdem angelegt werden ?");
			break;
		case 3:
		case 5:
		case 7:
			sText = _T("ACHTUNG:\r\nFolgende Einträge enthalten nicht, bzw. stimmen nicht mit dem Computernamen überein:\r\n");
			sMsgBox += _T("\r\n\r\nSollen der Desktop Ordner und die markierte(n) Datei(en) trotzdem angelegt werden ?");			
			break;
		case 6:
			sText = _T("ACHTUNG:\r\nFolgende Einträge enthalten nicht den Computernamen:\r\n");
			sMsgBox += _T("\r\n\r\nSollen die Dateien trotzdem angelegt werden ?");		
			break;
 		default:
			break;
		}
		iResult = MessageBox(sText+sMsgBox, sWndTitle, MB_YESNO | MB_ICONSTOP);
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
//Prüft, ob die Checkboxes beim Anlagen von Orndern/Dateien alle gechecket sind
BOOL CCleanNTDlg::Check_NTCheckBoxes(int &iResult)
{
	BOOL bRet = TRUE;
	if(    !m_CheckDeskFolder.GetCheck()
		|| !m_CheckSystemOverview.GetCheck()
		|| !m_CheckFileList.GetCheck())
	{
		CString sWndTitle = _T("Erstellen von Ordner/Dateien");	
		CString sMsgBox = _T("\r\nACHTUNG:\r\nNicht alle zu erstellenden Ordner bzw. Dateien sind markiert.\r\nSind diese Einstellungen korrekt?");
		iResult = MessageBox(sMsgBox, sWndTitle, MB_YESNO | MB_ICONSTOP);
		bRet = FALSE;
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::SaveAllEntries()
{

	int len;

	//nur bei XP die XP-Einstellungen vornehmen, XPe Lizenz und Nero Lizenz
	if(m_sOSInUse == _T("XP Einstellungen"))
	{

		int len = 0;

		//Nero Lizenz eintragen
		CString sNewNeroLizenz;

		if(m_sNeroLicense != _T("0"))
		{
			sNewNeroLizenz.Format(_T("%s-%s-%s-%s-%s-%s"),m_sNero1,m_sNero2,m_sNero3,
													  m_sNero4,m_sNero5,m_sNero6);
			len = sNewNeroLizenz.GetLength() * sizeof(TCHAR);
			m_Reg.SetKey(_T("SOFTWARE\\Ahead\\Nero - Burning Rom\\Info"),_T("Serial5"), (BYTE*)LPCTSTR(sNewNeroLizenz), len);

		}

		//XPe Lizenz eintragen
		len = m_sEditXP.GetLength() * sizeof(TCHAR);
		m_Reg.SetKey(_T("SOFTWARE\\dvrt\\Version"), _T("XPeLicense"), (BYTE*)LPCTSTR(m_sEditXP), len);

		//neue HDD ID speichern
		if(m_bSaveHDD)
		{
			SaveHDDID();
			m_sSetHDDID = _T("Neue HDD-ID gesetzt !");
			m_bSaveHDD = FALSE;
		}
		else
		{
			m_sSetHDDID = _T("Keine neue HDD-ID gesetzt !");
		}
	}
	else
	{
		//OEM aktualisieren
		CString sOEM = m_NTNewValues.sOEM1 + m_sOEMMid + m_NTNewValues.sOEM2 + m_NTNewValues.sOEM3;
		int len = sOEM.GetLength() * sizeof(TCHAR);
		m_Reg.SetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_PRODUCTID,(BYTE*)LPCTSTR(sOEM),len);
	}	//Computername aktualisieren


	len = m_NTNewValues.sCompName.GetLength() * sizeof(TCHAR);
	m_Reg.SetKey(REGPATH_HKLM_COMPUTERNAME, REGKEY_COMPUTERNAME, (BYTE*)LPCTSTR(m_NTNewValues.sCompName), len);
	m_Reg.SetKey(_T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters"), _T("Hostname"), (BYTE*)LPCTSTR(m_NTNewValues.sCompName),len);
	m_Reg.SetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDOWNER,(BYTE*)LPCTSTR(m_NTNewValues.sCompName),len);

	if(m_sOSInUse == _T("XP Einstellungen"))
	{
		m_Reg.SetKey(_T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters"),_T("NV Hostname"),(BYTE*)LPCTSTR(m_NTNewValues.sCompName),len);

		//Computername im Nero-Zweig aktualisieren
		m_Reg.SetKey(_T("SOFTWARE\\Ahead\\Nero - Burning Rom\\Info\\"),_T("User"),(BYTE*)LPCTSTR(m_NTNewValues.sCompName),len);
	}

	//Organisation aktualisieren
	len = m_NTNewValues.sOrga.GetLength() * sizeof(TCHAR);
	m_Reg.SetKey(REGPATH_HKLM_NT_CURRENTVERSION, REGKEY_REGISTEREDORGANIZATION,(BYTE*)LPCTSTR(m_NTNewValues.sOrga),len);

	if(m_sOSInUse == _T("NT Einstellungen"))
	{
		//Loader Timeout aktualisieren
		m_Reg.SetNTLoaderTimeout(_ttoi(m_NTNewValues.sLoaderTimeout));
	}
}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::SaveSameNames()
{
	BOOL bFolder = TRUE;
	//nur wenn checkbox _T("Desktop Ordner") markiert ist
	if(m_CheckDeskFolder.GetCheck() == 1)
	{
		//Desktop Ordner anlegen
		CString sDeskFolder = m_sPathToDesktop + m_NTNewValues.sDeskFolder;
		if (m_pIdipCleanDlg->DoesFileExist((LPCTSTR)sDeskFolder)==FALSE) 
		{
			bFolder = CreateDirectory(sDeskFolder, NULL);
		}

		if(bFolder) //nur wenn Folder exists
		{		
			m_sResult = _T("Status:");
			m_cResult.ShowWindow(SW_SHOW);
			m_cStatus.ShowWindow(SW_SHOW);

			//nur wenn checkbox markiert ist
			if(m_CheckFileList.GetCheck() == 1)
			{
				m_pIdipCleanDlg->m_cOK.EnableWindow(FALSE);
				m_sStatus = _T("erstelle Dateiliste, bitte warten...");
				m_STATUS.bOrdnerDatei = TRUE;
				UpdateData(FALSE);
				UpdateWindow();
				//Fileliste von C:\ erstellen und in Desktop Ordner ablegen
				CString sResultFile = m_sPathToDesktop+m_NTNewValues.sDeskFolder+_T("\\")+m_NTNewValues.sFileList;
				BOOL bDelete = TRUE;

				//bDelete ist nur beim ersten Aufruf von "PrintTreeRecursiv" auf TRUE
				//da dadurch ein etwaig schon vorhandenes "sResultFile" gelöscht wird
				PrintTreeRecursiv(_T("C:\\"), sResultFile, bDelete);
				m_pIdipCleanDlg->m_cOK.EnableWindow();
			}

			//nur wenn checkbox "System Übersicht" markiert ist
			if(m_CheckSystemOverview.GetCheck() == 1)
			{	
				m_pIdipCleanDlg->m_cOK.EnableWindow(FALSE);
				m_sStatus = _T("erstelle Systemübersicht, bitte warten...");
				UpdateData(FALSE);
				UpdateWindow();
				
				//Systemübersicht erstellen und in Desktop Ordner ablegen
				if(m_sOSInUse == _T("NT Einstellungen"))
				{
					CString sWinDiagnose = m_sSystem32Path + _T("\\winmsd.exe ");
					if(StartWinDiagnose(sWinDiagnose))
					{
						TRACE(_T("Win Diagnose gestartet\n"));
						LPARAM lParam = 0;
						//lParam = 0: Suche den Diagnose Dialog 
						//deutsch:"Windows NT-Diagnose" 
						//englisch: "Windows NT Diagnostics" 
						EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, lParam);
						ClickDruckenButton();
						Sleep(500); //warten, bis "Bericht erstellen" Dlg auf Desktop
						lParam = 1;
						DWORD dwTick = GetTickCount();
						do
						{
							//suche Dlg "Bericht erstellen"
							EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, lParam);
							Sleep(20);
						}while(!m_hBerichtErstellenDlg && (GetTickCount()-dwTick < 1000));

						SetRadioOnWinDiagnoseDlg(); //initialisieren für Datei-Ausdruck
						SaveWinDiagnose();
						EndWinDiagnose();
						GetAndSaveVolumeID();
						m_pIdipCleanDlg->m_cOK.EnableWindow();
						m_STATUS.bOrdnerDatei = TRUE;
					}
				}
				else //XP
				{	
					
					CString sFullPath = m_sPathToDesktop + m_sDeskFolder + _T("\\") + m_sSysOverview;
					m_sTempOverview = _T("C:\\") + m_sSysOverview;
					m_sMSInfoCommandLineParam = m_sTempOverview + _T(" /report ") + m_sTempOverview;
					CString sWinDiagnose = m_sPathToMSInfo;
					DeleteFile(sFullPath);
					if(StartWinDiagnoseXP(sWinDiagnose))
					{
						m_STATUS.bOrdnerDatei = TRUE;
						MoveTempOverviewToDesktopFolder();
					}
				}
			}
			m_cStatus.ShowWindow(SW_HIDE);
			UpdateWindow();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::GetNewEntries()
{
	UpdateData(TRUE);
	m_NTNewValues.sOEM1				= m_sOEM1;
	m_NTNewValues.sOEM2				= m_sOEM2;
	m_NTNewValues.sOEM3				= m_sOEM3;
	m_NTNewValues.sCompName			= m_sCompName;
	m_NTNewValues.sOrga				= m_sOrga;
	m_NTNewValues.sDeskFolder		= m_sDeskFolder;
	m_NTNewValues.sLoaderTimeout	= m_sLoaderTimeout;
	m_NTNewValues.sSysOverview		= m_sSysOverview;
	m_NTNewValues.sFileList			= m_sFileList;

	m_NTNewValues.sXPeLicence		= m_sEditXP;
	m_NTNewValues.sNero1			= m_sNero1;
	m_NTNewValues.sNero2			= m_sNero2;
	m_NTNewValues.sNero3			= m_sNero3;
	m_NTNewValues.sNero4			= m_sNero4;
	m_NTNewValues.sNero5			= m_sNero5;
	m_NTNewValues.sNero6			= m_sNero6;
}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeOem1() 
{
	UpdateData(TRUE);
	UINT uLen = m_sOEM1.GetLength() * sizeof(TCHAR);
	if(uLen == m_ControlOEM1.GetLimitText())
	{
		m_ControlOEM2.SetFocus();
	}
}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeOem2() 
{
	UpdateData(TRUE);
	UINT uLen = m_sOEM2.GetLength() * sizeof(TCHAR);
	if(uLen == m_ControlOEM2.GetLimitText())
	{
		m_ControlOEM3.SetFocus();
	}
}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeOem3() 
{
	UpdateData(TRUE);
	UINT uLen = m_sOEM3.GetLength() * sizeof(TCHAR);
	if(uLen == m_ControlOEM3.GetLimitText())
	{
		m_ControlCompName.SetFocus();
	}
}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeCompname() 
{

	CString sFileType;
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);

	if(m_sOSInUse == _T("NT Einstellungen"))
	{
		//DesktopFolder nach Computername benennen und checkbox setzen
		m_sDeskFolder = m_sCompName;	
		m_CheckDeskFolder.EnableWindow(FALSE);
		m_CheckDeskFolder.SetCheck(1);
		
		//Datei _T("Systemübersicht") nach Computername benennen und checkbox setzen
		sFileType = m_sSysOverview.Right(5);
		m_sSysOverview = m_sCompName+sFileType;
		m_CheckSystemOverview.SetCheck(1);

		//Datei _T("Filelist C:\") nach Computername benennen und checkbox setzen
		sFileType = m_sFileList.Right(5);
		m_sFileList = m_sCompName+sFileType;
		m_CheckFileList.SetCheck(1);
	}
	else
	{
		CryptNewHDDID();
		m_ctrlSaveHDDID.SetCheck(1);
		
	}

	if((UINT)m_sCompName.GetLength() == m_ControlCompName.GetLimitText())
	{
		m_ControlOrga.SetFocus();
	}

	UpdateData(FALSE);

}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeOrga() 
{	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);
}
//////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnSetfocusOem2() 
{
	m_ControlOEM2.SetSel(0, -1);
}
////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnSetfocusOem3() 
{
	m_ControlOEM3.SetSel(0, -1);
}
////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnSetfocusOrga() 
{
	m_ControlOrga.SetSel(0, -1);
}
////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnSetfocusCompname() 
{
	m_ControlCompName.SetSel(0, -1);	
}
////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnCheckSystemOverview() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);
	
	if(m_CheckSystemOverview.GetCheck() == 0)
	{
		if(m_CheckFileList.GetCheck() == 0)
		{
			m_CheckDeskFolder.EnableWindow();
		}
	}
	else if(m_CheckSystemOverview.GetCheck() == 1)
	{
		m_CheckDeskFolder.EnableWindow(FALSE);
		m_CheckDeskFolder.SetCheck(1);
	}
}
////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnCheckFileList() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);
	if(m_CheckFileList.GetCheck() == 0)
	{
		if(m_CheckSystemOverview.GetCheck() == 0)
		{
			m_CheckDeskFolder.EnableWindow();
		}
	}
	else if(m_CheckFileList.GetCheck() == 1)
	{
		m_CheckDeskFolder.EnableWindow(FALSE);
		m_CheckDeskFolder.SetCheck(1);
	}
}
////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnKillfocusOem1() 
{
	// check new value, if lenght of new value < 5 show old value again
	if(m_sOEM1.GetLength() < 5)		
	{
		m_ControlOEM1.SetFocus();
		m_ControlOEM1.SetSel(0, -1, FALSE);
	}
}
////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnKillfocusOem2() 
{
	// check new value, if lenght of new value < 7 show old value again
	if(m_sOEM2.GetLength() < 7)		
	{
		m_ControlOEM2.SetFocus();
		m_ControlOEM2.SetSel(0, -1, FALSE);
	}
	
}
////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnKillfocusOem3() 
{
	// check new value, if lenght of new value < 5 show old value again
	if(m_sOEM3.GetLength() < 5)		
	{
		m_ControlOEM3.SetFocus();
		m_ControlOEM3.SetSel(0, -1, FALSE);
	}	
}
////////////////////////////////////////////////////////////////////
void CCleanNTDlg::SetRadioOnWinDiagnoseDlg()
{
	HWND hRadio = NULL;
	hRadio = ::GetDlgItem(m_hBerichtErstellenDlg, m_dwDatei);
	if(hRadio && IsWindow(hRadio))
	{
		::PostMessage(m_hBerichtErstellenDlg,WM_COMMAND,
					(WPARAM)MAKELONG(m_dwDatei,BN_CLICKED),
					(LPARAM)hRadio);
		hRadio = NULL;
	}

	hRadio = ::GetDlgItem(m_hBerichtErstellenDlg, m_dwZusammenfassung);
	if(hRadio && IsWindow(hRadio))
	{
		::PostMessage(m_hBerichtErstellenDlg,WM_COMMAND,
					(WPARAM)MAKELONG(m_dwDatei,BN_CLICKED),
					(LPARAM)hRadio);
		hRadio = NULL;
	}

	hRadio = ::GetDlgItem(m_hBerichtErstellenDlg, m_dwAlleRegs);
	if(hRadio && IsWindow(hRadio))
	{
		::PostMessage(m_hBerichtErstellenDlg,WM_COMMAND,
					(WPARAM)MAKELONG(m_dwDatei,BN_CLICKED),
					(LPARAM)hRadio);
		hRadio = NULL;
	}
}

//sStartDir = "C:\test" ==> Verzeichnis C:\test (incl. Unterverz.) werden durchsucht
//sResultFile => enthält Fileübersicht der Suche, nur File angeben zB.:"c:\result.txt"
//bDelete => löscht beim ersten Aufruf ein etwaig schon vorhandenes sResultFile
/////////////////////////////////////////////////////////////////////////////
BOOL CCleanNTDlg::PrintTreeRecursiv(CString sStartDir, 
									CString sResultFile,
									BOOL bDelete)
{
	CFile TreeFile;
	CString sDrives;

	//lege File an
	if(!m_pIdipCleanDlg->DoesFileExist((LPCTSTR) sResultFile))
	{
		BOOL bSuccess = TreeFile.Open(sResultFile, CFile::modeCreate | CFile::modeReadWrite   | CFile::modeNoInherit);
		if(bSuccess)
		{
			TRACE(_T("** File wird angelegt\n"));
			int iMax = 60;
			int i;
			CString s, t;
			CString sDot(_T("*"));
			for(i=0; i<iMax; i++)
			{	
				t+=sDot;
			}
			TreeFile.Write(t, t.GetLength());
			s = _T("\r\n Datei-Übersicht aller auf Laufwerk 'C' vorhandenen Dateien\r\n");
			TreeFile.Write(s, s.GetLength());
			t+=_T("\r\n\r\n");
			TreeFile.Write(t, t.GetLength());
			TreeFile.Close();
		}
	}
	else
	{
		CFileStatus Status;
		TreeFile.GetStatus(Status);
		CTime Time;
		if(bDelete && Status.m_ctime < Time.GetCurrentTime())
		{
			TRACE(_T("schon da: File: %s\n"), sResultFile);
			DeleteFile(sResultFile);
			bDelete = FALSE;
			PrintTreeRecursiv(sStartDir, sResultFile, bDelete);
			return FALSE;
		}
	}

/*
	if (sStartDir.IsEmpty())
	{
		// alle lokalen Festplatten
		DWORD dwDrives = GetLogicalDrives();
		DWORD dwDrive = 4; // _T('C')
		CString sRoot;
		_TCHAR c;

		for (dwDrive=4,c=_T('c');dwDrive!=0;dwDrive<<=1,c++)
		{
			if (dwDrive & dwDrives)
			{
				sRoot.Format(_T("%c:\\"),c);
				if (DRIVE_FIXED == GetDriveType(sRoot))
				{
					if (PrintTreeRecursiv(sRoot, sResultFile, bDelete))
					{
						return TRUE;
					}
				}
			}
		}
		return FALSE;
	}
*/
	HANDLE hF;
	WIN32_FIND_DATA FindFileData;
	CString sSearch;
	CString sPath;
	CString sFile;

	sPath = sStartDir;
	if (sPath.GetLength()==0) {
		sPath += _T(".\\");
	} else if (sPath[sPath.GetLength()-1]!=_T('\\')) {
		sPath += _T('\\');
	}
	sSearch = sPath + _T("*.*");

	hF = FindFirstFile((LPCTSTR)sSearch,&FindFileData);
	if (hF != INVALID_HANDLE_VALUE)
	{
		if ( (_tcscmp(FindFileData.cFileName,_T("."))!=0) &&
			 (_tcscmp(FindFileData.cFileName,_T(".."))!=0))
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				sFile = sPath+FindFileData.cFileName;
				if (PrintTreeRecursiv(sFile, sResultFile, bDelete))
				{
					return TRUE;
				}
			}
			else
			{
				CString sText;
				sText = sPath + FindFileData.cFileName + _T("\r\n");
				TreeFile.Open(sResultFile, CFile::modeReadWrite | CFile::modeNoInherit);
				TreeFile.SeekToEnd();
				TreeFile.Write(sText, sText.GetLength());
				TreeFile.Close();
			}
		} 
		while (FindNextFile(hF,&FindFileData))
		{
			if ( (_tcscmp(FindFileData.cFileName,_T("."))!=0) &&
				 (_tcscmp(FindFileData.cFileName,_T(".."))!=0))
			{
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					sFile = sPath+FindFileData.cFileName;
					bDelete = FALSE;
					if (PrintTreeRecursiv(sFile, sResultFile, bDelete))
					{
						return TRUE;
					}
				}
				else
				{
					CString sText;
					sText = sPath + FindFileData.cFileName + _T("\r\n");
					TreeFile.Open(sResultFile, CFile::modeReadWrite | CFile::modeNoInherit);
					TreeFile.SeekToEnd();
					TreeFile.Write(sText, sText.GetLength());
					TreeFile.Close();

				}
			}
		}
		FindClose(hF);
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
//get Volume ID and save to the System-Overview-File
void CCleanNTDlg::GetAndSaveVolumeID()
{
	_TCHAR c1[255];
	_TCHAR c2[255];
	DWORD dw1 = 255;
	DWORD dw2 = 255;
	DWORD dw3 = 255;

  LPCTSTR lpRootPathName = _T("C:\\");         // root directory
  LPTSTR lpVolumeNameBuffer;        // volume name buffer
  DWORD nVolumeNameSize = 255;            // length of name buffer
  LPDWORD lpVolumeSerialNumber;     // volume serial number
  LPDWORD lpMaximumComponentLength; // maximum file name length
  LPDWORD lpFileSystemFlags;        // file system options
  LPTSTR lpFileSystemNameBuffer;    // file system name buffer
  DWORD nFileSystemNameSize  = 255; 

	lpVolumeNameBuffer = c1;
	lpVolumeSerialNumber = &dw1;
	lpMaximumComponentLength = &dw2;
	lpFileSystemFlags = &dw3;
	lpFileSystemNameBuffer = c2;

	GetVolumeInformation(lpRootPathName,
						 lpVolumeNameBuffer,
						 nVolumeNameSize,
						 lpVolumeSerialNumber,
						 lpMaximumComponentLength,
						 lpFileSystemFlags,
						 lpFileSystemNameBuffer,
						 nFileSystemNameSize);
	WORD w2 = LOWORD(dw1);
	WORD w1 = HIWORD(dw1);
	CString sHDDID;
	sHDDID.Format(_T("%x - %x"),w1, w2);
	CTime currentTime = CTime::GetCurrentTime();
	CString sDateTime = currentTime.Format(_T("%d. %b %Y, %H:%M:%S"));
	CString sIDAndTime;//(_T("\r\nSYSTEM - ÜBERSICHT (vom "));
//	sIDAndTime += sDateTime +_T(")\r\n******************");
	sIDAndTime += _T("\r\n\r\nHDD-ID: ") + sHDDID + _T("\r\n\r\n\r\n");


	CString sPath = m_sPathToDesktop+m_NTNewValues.sDeskFolder+_T("\\")+m_NTNewValues.sSysOverview;
	TRACE(_T("path: %s\n"), sPath);
	CFile systemOverview(sPath, CFile::modeReadWrite);
	systemOverview.SeekToBegin();
	systemOverview.Write(sIDAndTime, sIDAndTime.GetLength());
	systemOverview.Close();

	TRACE(_T("Nr: %x - %x\n"), w1, w2);
	
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeEditNero1() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);

	if((UINT)m_sNero1.GetLength() == m_ctrlNero1.GetLimitText())
	{
		m_ctrlNero2.SetFocus();
		m_ctrlNero2.SetSel(0, -1, FALSE);

	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeEditNero2() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);

	if((UINT)m_sNero2.GetLength() == m_ctrlNero2.GetLimitText())
	{
		m_ctrlNero3.SetFocus();
		m_ctrlNero3.SetSel(0, -1, FALSE);

	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeEditNero3() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);

	if((UINT)m_sNero3.GetLength() == m_ctrlNero3.GetLimitText())
	{
		m_ctrlNero4.SetFocus();
		m_ctrlNero4.SetSel(0, -1, FALSE);

	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeEditNero4() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);

	if((UINT)m_sNero4.GetLength() == m_ctrlNero4.GetLimitText())
	{
		m_ctrlNero5.SetFocus();
		m_ctrlNero5.SetSel(0, -1, FALSE);

	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeEditNero5() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);

	if((UINT)m_sNero5.GetLength() == m_ctrlNero5.GetLimitText())
	{
		m_ctrlNero6.SetFocus();
		m_ctrlNero6.SetSel(0, -1, FALSE);

	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeEditNero6() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);

	if((UINT)m_sNero6.GetLength() == m_ctrlNero6.GetLimitText())
	{
		m_ControlCompName.SetFocus();
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnKillfocusEditNero1() 
{
	if(m_sNero1.GetLength() != 0 && m_sNero1.GetLength() < 4)		
	{
		m_ctrlNero1.SetFocus();
		m_ctrlNero1.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnKillfocusEditNero2() 
{
	if(m_sNero2.GetLength() != 0 && m_sNero2.GetLength() < 4)		
	{
		m_ctrlNero2.SetFocus();
		m_ctrlNero2.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnKillfocusEditNero3() 
{
	if(m_sNero3.GetLength() != 0 && m_sNero3.GetLength() < 4)		
	{
		m_ctrlNero3.SetFocus();
		m_ctrlNero3.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnKillfocusEditNero4() 
{
	if(m_sNero4.GetLength() != 0 && m_sNero4.GetLength() < 4)		
	{
		m_ctrlNero4.SetFocus();
		m_ctrlNero4.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnKillfocusEditNero5() 
{
	if(m_sNero5.GetLength() != 0 && m_sNero5.GetLength() < 4)		
	{
		m_ctrlNero5.SetFocus();
		m_ctrlNero5.SetSel(0, -1, FALSE);
	}	
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnKillfocusEditNero6() 
{
	if(m_sNero6.GetLength() != 0 && m_sNero6.GetLength() < 4)		
	{
		m_ctrlNero6.SetFocus();
		m_ctrlNero6.SetSel(0, -1, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::GetXPDataFromReg()
{
	CString value;
	DWORD dwHeigth, dwwidth, color, dwHZ;
	dwHeigth = dwwidth = color = dwHZ = 0;

	//XPe Lizenz holen
	m_sEditXP = m_Reg.GetKey(_T("SOFTWARE\\dvrt\\Version"),_T("XPeLicense"));

	//Nero Lizenz holen
	//prüfe Nero Version 5 oder 6
	CString sSerial = _T("Serial6");
	
	m_sNeroLicense = m_Reg.GetKey(_T("SOFTWARE\\Ahead\\Nero - Burning Rom\\Info"),_T("Serial5"));
	if(m_sNeroLicense != _T("0"))
	{
		int iStep = 4;
		m_sNero1 = m_sNeroLicense.Left(iStep);
		m_sNero2 = m_sNeroLicense.Mid	(iStep+1,iStep);
		m_sNero3 = m_sNeroLicense.Mid	(iStep*2+2,iStep);
		m_sNero4 = m_sNeroLicense.Mid	(iStep*3+3,iStep);
		m_sNero5 = m_sNeroLicense.Mid	(iStep*4+4,iStep);
		m_sNero6 = m_sNeroLicense.Mid	(iStep*5+5,iStep);
	}

	m_NTDefaults.sXPeLicence = m_sEditXP;
	m_NTDefaults.sNero1 = m_sNero1;
	m_NTDefaults.sNero2 = m_sNero2;
	m_NTDefaults.sNero3 = m_sNero3;
	m_NTDefaults.sNero4 = m_sNero4;
	m_NTDefaults.sNero5 = m_sNero5;
	m_NTDefaults.sNero6 = m_sNero6;
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeEditXp() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);
	
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeLoadertimeout() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);
	
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnCheckDeskFolder() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::MoveTempOverviewToDesktopFolder()
{
	MoveFile(m_sTempOverview, m_sPathToDesktop + m_sDeskFolder + _T("\\") + m_sSysOverview);
	DeleteFile(m_sTempOverview);
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeEditHdd() 
{

	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnChangeEditHdd2() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::CryptNewHDDID()
{

//	DWORD dwSerial = 0;
//	DWORD dwDongle = 0;

	CString s,sSerial;

	sSerial = m_sCompName;

	if (!sSerial.IsEmpty())
	{
		sSerial.MakeLower();
		for (int i=0;i<sSerial.GetLength();i++)
		{
			if (   _T('0') <= sSerial[i]
				&& sSerial[i] <= _T('9'))
			{
				s += sSerial[i];
			}
		}
		
		if (s.IsEmpty())
		{
			for (int i=0;i<sSerial.GetLength() && i<8;i++)
			{
				int iNum = sSerial[i] % 10;
				s += (_TCHAR)(_T('0')+iNum);
			}
		}
		

		CString sNewID;
		int iNew = _ttoi(s);
		iNew = 2 + iNew*iNew - 1;
		
		sNewID.Format(_T("%x"), iNew);
		if(sNewID.GetLength() > 8)
		{
			sNewID = sNewID.Left(8);
		}

		while(sNewID.GetLength() < 8)
		{
			//neue HDDID künstlich verlängern
			iNew = 2+iNew*iNew-1;
			sNewID.Format(_T("%x"), iNew);
		}
		
		if(sNewID.GetLength() >= 8)
		{
			m_sHDD = sNewID.Left(4);
			m_sHDD2 = sNewID.Mid(4,8);
			m_sNewHDDID = m_sHDD + _T("-") + m_sHDD2;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCleanNTDlg::SaveHDDID()
{
	CString sExe = _T("volumeid.exe C: ") + m_sNewHDDID;
	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;

	startUpInfo.cb				= sizeof(STARTUPINFO);
    startUpInfo.lpReserved      = NULL;
    startUpInfo.lpDesktop       = NULL;
    startUpInfo.lpTitle			= NULL;
    startUpInfo.dwX				= 0;
    startUpInfo.dwY				= 0;
    startUpInfo.dwXSize			= 0;
    startUpInfo.dwYSize			= 0;
    startUpInfo.dwXCountChars   = 0;
    startUpInfo.dwYCountChars   = 0;
    startUpInfo.dwFillAttribute = 0;
	startUpInfo.dwFlags			= STARTF_USESHOWWINDOW;
    startUpInfo.wShowWindow     = SW_SHOW; 
    startUpInfo.cbReserved2     = 0;
    startUpInfo.lpReserved2     = NULL;

	BOOL bProcess = CreateProcess(
						(LPCTSTR)sExe,				// pointer to name of executable module 
						(LPTSTR)sExe.GetBuffer(0),		// pointer to command line string
						(LPSECURITY_ATTRIBUTES)NULL,// pointer to process security attributes 
						(LPSECURITY_ATTRIBUTES)NULL,// pointer to thread security attributes 
						FALSE,						// handle inheritance flag 
						NORMAL_PRIORITY_CLASS,		//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
						NULL,						// pointer to new environment block 
						NULL,						// pointer to current directory name 
						&startUpInfo,				// pointer to STARTUPINFO 
						&prozessInformation 		// pointer to PROCESS_INFORMATION  
								);
	sExe.ReleaseBuffer();	

	return bProcess;
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnCheckHdd() 
{
	m_ctrlUebernehmen.EnableWindow();
	UpdateData(TRUE);
	
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnBnClickedBtnCfInit()
{
	CString sCmd = theApp.m_sStartDir + _T("\\VideteCheckDisk.exe /e /a");
	int nReturnCode = CVipCleanNTApp::ExecuteProgram(sCmd, TRUE);
	if (nReturnCode == 2)
	{

	}
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnBnClickedBtnInitDisks()
{
	CString sCmd = theApp.m_sStartDir + _T("\\VideteCheckDisk.exe /a");
	int nReturnCode = CVipCleanNTApp::ExecuteProgram(sCmd, TRUE);
	if (nReturnCode == 2)
	{

	}
}
/////////////////////////////////////////////////////////////////////////////
void CCleanNTDlg::OnBnClickedBtnCfUninit()
{
	CString sCmd = theApp.m_sStartDir + _T("\\VideteCheckDisk.exe /d");
	int nReturnCode = CVipCleanNTApp::ExecuteProgram(sCmd, TRUE);
	if (nReturnCode == 2)
	{

	}
}
/////////////////////////////////////////////////////////////////////////////
#endif // _IDIP
