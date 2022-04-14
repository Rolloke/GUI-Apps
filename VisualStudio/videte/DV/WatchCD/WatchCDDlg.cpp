// WatchCDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WatchCD.h"
#include "WatchCDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CWatchCDDlg* m_pWatchCD;

/////////////////////////////////////////////////////////////////////////////
// CWatchCDDlg dialog

CWatchCDDlg::CWatchCDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWatchCDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWatchCDDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pWatchCD = this;

	m_hWatchDog		= NULL;
	m_hStartDlg		= NULL;
	m_hDVClient		= NULL;
	m_hUnexpErrorWnd= NULL;
	m_hOnUnexpErrorActiveDCDWnd = NULL;
	m_hEjectDlg		= NULL;
	m_hWaitForFileSystemDlg	 = NULL;
	m_dwTick = 0;
	m_bDirectCDVersion5 = FALSE;

	m_iDCDVersion = DCD_VERSION_UNKNOWN;

	if(theApp.GetDirectCDVersion() == DCD_VERSION_50)
	{
		//TOPDO TKR muss eine INitialisierung für DirectCD Version 5 vorgenommen werden?
		//erstmal wird ja die englische Version benutzt
		TRACE(_T("Noch keine Initialisierung der Language Strings für DirectCD Version 5\n"));
		m_bDirectCDVersion5 = TRUE;
		InitLanguageStringsVersion5();
	}
	else
	{
		InitLanguageStrings();
	}


	m_DCDStatus		= WCD_START;
	m_pDCDError		= new CDirectCDError();
	m_pDCDStart		= new CDirectCDStart(this, m_pDCDError);
	m_pDCDFormat	= new CDirectCDFormat(this, m_pDCDError);
	m_pDCDBackup	= new CDirectCDBackup(this, m_pDCDError);
	m_pDCDFinalize	= new CDirectCDFinalize(this, m_pDCDStart, m_pDCDError); 

	m_bDVClientBackupComplete	= FALSE;
	m_bDVClientCopyingData		= FALSE;
	m_bDVClientCopyingDataError = FALSE;
	m_dwYesButtonOnErrorDlg		= 0x0006;
	m_bDirectCDStarted			= FALSE;
	m_hProcessWizard			= NULL;

	if(theApp.GetDirectCDVersion() == DCD_VERSION_50)
	{
		//starte DirectCD Version 5.xx
		if(ExecuteDirectCDVers5())
		{
			m_bDirectCDStarted = TRUE;
		}
	}
	else
	{
		//starte DirectCD Version 3.xx
		if(ExecuteDirectCD())  //startet DirectCD Version 3.xx
		{
			m_bDirectCDStarted = TRUE;
		}
	}
	Create(IDD);
}

void CWatchCDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWatchCDDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWatchCDDlg, CDialog)
	//{{AFX_MSG_MAP(CWatchCDDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CHECK_BACKUP_STATUS, OnReceiveStatus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchCDDlg message handlers

BOOL CWatchCDDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWatchCDDlg::OnPaint() 
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

HCURSOR CWatchCDDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


//////////////////////////////////////////////////////////////////////
void CWatchCDDlg::InitLanguageStrings()
{
	CString sLoc(_T("directcd.loc"));
	CString sExe(_T("directcd.exe"));
	CString sVersion = GetDirectCDFullVersionString();
	CString sDrive = GetBackupDrive();

	if(sDrive == _T("No CD-Drive"))
	{
		WK_TRACE(_T("CWatchCDDlg::InitLanguageStrings(): Couldn`t get drive letter from reg\n"));

		//get CD-Drive letter from commando parameter
		CString sCommandLine = theApp.m_lpCmdLine;
		if(!sCommandLine.IsEmpty())
		{
			sDrive = sCommandLine.Left(1);
			sDrive.MakeUpper();
			WK_TRACE(_T("CWatchCDDlg::InitLanguageStrings(): Got drive letter from command line\n"));
		}
		else
		{
			WK_TRACE(_T("CWatchCDDlg::InitLanguageStrings(): Couldn`t get drive letter command line\n"));
		}
	}

	WK_TRACE(_T("Version %s\n"),sVersion);
	WK_TRACE(_T("Drive %s\n"),sDrive);

	if(FileExists(GetPathToDirectCD(), sLoc))
	{
		m_sAdaptec = GetStringFromTable(GetPathToDirectCD(),
													sLoc, 128);

		m_sFullDrive = GetStringFromTable(GetPathToDirectCD(),
													sLoc, 151);
		m_sFullDrive.Replace(_T("%c"), sDrive);
	}
	else
	{
		m_sAdaptec = GetStringFromTable(GetPathToDirectCD(),
													sExe, 128);
		m_sFullDrive = GetStringFromTable(GetPathToDirectCD(),
													sExe, 151);
		m_sFullDrive.Replace(_T("%c"), sDrive);
	}


	m_sAdaptec = m_sAdaptec.Left(m_sAdaptec.Find(_T('\n'), 0));
	m_sAdaptecWatchdog = GetStringFromTable(GetPathToDirectCD(),
												sExe, 128);
	m_sAdaptecWatchdog = m_sAdaptecWatchdog.Left(m_sAdaptecWatchdog.Find(_T('\n'), 0));


/*
	if (m_sAdaptec.IsEmpty())
	{
		//set default
		m_sAdaptec = _T("Adaptec DirectCD Wizard");
	}

	if (m_sFullDrive.IsEmpty())
	{
		m_sFullDrive = _T("Drive");
	}
*/
	if(sVersion != _T("undefined") && sDrive != _T("No CD-Drive"))
	{
		m_sDCDWndTitleWithoutDrive = m_sAdaptec + _T(" ") + sVersion;
		m_sDCDWndTitleWithDrive.Format(_T("%s (%s:) %s"), m_sAdaptec, m_sFullDrive, sVersion);

		WK_TRACE(_T("with Drive: %s\n"), m_sDCDWndTitleWithDrive);
		WK_TRACE(_T("without Drive: %s\n"), m_sDCDWndTitleWithoutDrive);


	}
	else
	{
		WK_TRACE(_T("CWatchCDDlg::InitLanguageStrings(): Couldn`t get drive letter or DirectCD version\n"));
	}

}

//////////////////////////////////////////////////////////////////////
void CWatchCDDlg::InitLanguageStringsVersion5()
{
	CString sVersion = GetDirectCDFullVersionStringVersion5();
	CString sDrive = GetBackupDrive();

	if(sVersion != _T("undefined") && sDrive != _T("No CD-Drive"))
	{
		m_sDCDWndTitleWithDriveVersion5.Format(_T("DirectCD %s (Drive %s:)"), sVersion ,sDrive);
		WK_TRACE(_T("with Drive: %s\n"), m_sDCDWndTitleWithDriveVersion5);
	}
	else
	{
		WK_TRACE(_T("CWatchCDDlg::InitLanguageStrings(): Couldn`t get drive letter or DirectCD version\n"));
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CWatchCDDlg::FileExists(CString sPath, CString sFileName)
{
	return DoesFileExist(sPath+_T("\\")+sFileName);
}
//////////////////////////////////////////////////////////////////////
CString CWatchCDDlg::GetPathToDirectCD()
{
	CString sRet;
	CWK_Profile wkProf;
	sRet = wkProf.GetString(_T("dv\\DVClient"), _T("DirectCDPath"), _T(""));


	if(sRet.IsEmpty())
	{
		sRet = FindDirectCDPath();
		WK_TRACE(_T("konnte DirectCDPath in Registry DVClient nicht finden, benutze gefundenen Pfad %s\n"), sRet);
		wkProf.WriteString(_T("DV\\DVClient"),_T("DirectCDPath"), sRet);
	}

	sRet = sRet.Left(sRet.ReverseFind(_T('\\')));
	return sRet;
}

//////////////////////////////////////////////////////////////////////
CString CWatchCDDlg::GetPathToDirectCDVersion5()
{
	CString sRet;
	CWK_Profile wkProf;
	sRet = wkProf.GetString(_T("dv\\DVClient"), _T("DirectCDVers5Path"), _T(""));
	sRet = sRet.Left(sRet.ReverseFind(_T('\\')));

	return sRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CWatchCDDlg::ExecuteDirectCD()
{

	BOOL bRet = FALSE;
	BOOL bProcess;

	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	CString sCommandLine;
	CWK_Profile wkProf;
	CString sDirectCDPath = wkProf.GetString(_T("DV\\DVClient"), _T("DirectCDPath"), _T(""));

	if(sDirectCDPath.IsEmpty())
	{
		sDirectCDPath = FindDirectCDPath();
		WK_TRACE(_T("konnte DirectCDPath in Registry DVClient nicht finden, benutze gefundenen Pfad %s\n"), sDirectCDPath);
		wkProf.WriteString(_T("DV\\DVClient"),_T("DirectCDPath"), sDirectCDPath);
	}
	

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
	startUpInfo.wShowWindow     = SW_HIDE;
	startUpInfo.cbReserved2     = 0;
	startUpInfo.lpReserved2     = NULL;

	CString sExe = sDirectCDPath;
	
	sCommandLine = sExe;

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
	sCommandLine.ReleaseBuffer();	

	if(bProcess)
	{
		WK_TRACE(_T("ExecuteDirectCD(): DirectCD as Watchdog started.\n"));
		m_hProcessWizard = prozessInformation.hProcess;
		bRet = TRUE;
	}
	else
	{
		WK_TRACE_ERROR(_T("ExecuteDirectCD(): DirectCD as Watchdog NOT started.\n"));
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
CString CWatchCDDlg::FindDirectCDPath()
{
	CString sPathToDirectCD(_T("C:\\Programme\\Adaptec\\DirectCD\\directcd.exe"));
	CString sRet = sPathToDirectCD;
	CString sPath(_T("C:\\"));
	CString sDirectCD(_T("directcd.exe"));
	CString sLocation;

	if(!DoesFileExist(sPathToDirectCD))
	{
		//ist File mehrfach vorhanden, wird nur das erste gefunden, nicht mehrere
		SearchFileRecursiv(sPath, sDirectCD, sLocation);
		sRet = sLocation;
	}

	return sRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CWatchCDDlg::ExecuteDirectCDVers5()
{
	BOOL bRet = FALSE;
	BOOL bProcess;

	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	CString sCommandLine;
	CWK_Profile wkProf;
	CString sDirectCDVers5Path = wkProf.GetString(_T("DV\\DVClient"), _T("DirectCDVers5Path"), _T(""));
	//TODO TKR im DVStarter noch den Run Pfad in DVRT Pfad kopieren

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
	startUpInfo.wShowWindow     = SW_HIDE;
	startUpInfo.cbReserved2     = 0;
	startUpInfo.lpReserved2     = NULL;

	CString sExe = sDirectCDVers5Path;
	
	sCommandLine = sExe;

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
	sCommandLine.ReleaseBuffer();	

	if(bProcess)
	{
		WK_TRACE(_T("ExecuteDirectCDVers5(): DirectCDVers5 as Watchdog started.\n"));
		m_hProcessWizard = prozessInformation.hProcess;
		bRet = TRUE;
	}
	else
	{
		WK_TRACE_ERROR(_T("ExecuteDirectCDVers5(): DirectCDVers5 as Watchdog NOT started.\n"));
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
CString CWatchCDDlg::GetBackupDrive()
{
	CString sDriveFound(_T("No CD-Drive"));
	CWK_Profile wkProf(CWK_Profile::WKP_NTLOGON, NULL, _T("Adaptec\\DirectCD"),_T(""));

	switch(m_iDCDVersion)
	{
	case DCD_VERSION_UNKNOWN:
		WK_TRACE_ERROR(_T("GetBackupDrive(): DirectCD version unknown!\n"));
		break;

	case DCD_VERSION_26:
		{
		int iDefaultValue = 0;
		int iKeyValue = 0;
		CString sDrive;
		char cValue;
		
		//try with every drive from 'D' to 'Z' if there is a key with that drivename
		//if there is a key, that`s the cd-rom drive
		for(int i = 0; i <= 22; i++)
		{
			cValue = (char)(i + 68);
			sDrive = cValue;
			sDrive = sDrive + _T(":\\");
			iKeyValue = wkProf.GetInt(_T("2.6"), sDrive, iDefaultValue);
			if(iKeyValue != 0)
			{
				sDriveFound = cValue;
				break;
			}
		}
		}
		break;

	case DCD_VERSION_30:
		{

		int iDefaultValue = 0;
		int iKeyValue = 0;
		CString sDrive;
		char cValue;
		
		//try with every drive from 'D' to 'Z' if there is a key with that drivename
		//if there is a key, that`s the cd-rom drive
		for(int i = 0; i <= 22; i++)
		{
			cValue = (char)(i + 68);
			sDrive = cValue;
			sDrive = sDrive + _T(":\\");
			iKeyValue = wkProf.GetInt(_T("2.6"), sDrive, iDefaultValue);
			if(iKeyValue != 0)
			{
				sDriveFound = cValue;
				break;
			}
		}
		}
		break;
		
	case DCD_VERSION_50:
		{

		int iDefaultValue = 0;
		int iKeyValue = 0;
		CString sDrive;
		char cValue;
		CWK_Profile wkProf;

		//prüfe alle Laufwerkseinträge auf CD-Laufwerk (Nr: 2)
		for(int i = 0; i <= 22; i++)
		{
			cValue = (char)(i + 68);
			sDrive = cValue;
			iKeyValue = wkProf.GetInt(_T("dv\\Drives"), sDrive, iDefaultValue);
			if(iKeyValue == 2)
			{
				//Drive ist Backup Drive (CD-Rom)
				sDriveFound = cValue;
				break;
			}
		}
		}
		break;

	default:
		break;
	};


	return sDriveFound;
}

//////////////////////////////////////////////////////////////////////
CString CWatchCDDlg::GetDirectCDFullVersionString()
{
	CString sRet(_T("undefined"));
	CWK_Profile wkProf(CWK_Profile::WKP_NTLOGON, NULL, _T("Adaptec\\DirectCD"),_T(""));	
	
	int iDefault = -1;
	CString sStartup = _T("StartupBanner");

	if(wkProf.GetInt(_T("3.0"), sStartup, iDefault) == iDefault)
	{
		//Section "3.0" doesn`t exists
		wkProf.DeleteSubSection(_T("3.0"), _T(""));
		if(wkProf.GetInt(_T("2.6"), sStartup, iDefault) == iDefault)
		{
			//Section "2.6" doesn`t exists
			wkProf.DeleteSubSection(_T("2.6"), _T(""));
		}
		else
		{
			//actual version of DirectCD is version 2.6
			sRet = wkProf.GetString(_T("2.5"), _T("Version"), _T("undefined"));
			m_iDCDVersion = DCD_VERSION_26;
		}
	}
	else
	{
		//actual version of DirectCD is version 3.0
		CWK_Profile wkProfV3(CWK_Profile::WKP_NTLOGON, NULL, _T("Adaptec\\DirectCD"),_T(""));	
		sRet = wkProfV3.GetString(_T("3.0"), _T("Version"), _T("undefined"));
		m_iDCDVersion = DCD_VERSION_30;
	}



	return sRet;
}


//////////////////////////////////////////////////////////////////////
CString CWatchCDDlg::GetDirectCDFullVersionStringVersion5()
{
	CString sRet(_T("undefined"));
	CWK_Profile wkProf(CWK_Profile::WKP_NTLOGON, NULL, _T("Adaptec"),_T(""));	
	sRet = wkProf.GetString(_T("DirectCD"), _T("VersionLabel"), _T("undefined"));
	if(sRet != _T("undefined"))
	{
		m_iDCDVersion = DCD_VERSION_50;
	}
		
	return sRet;
}
//////////////////////////////////////////////////////////////////////
CString CWatchCDDlg::GetDCDWndTitleWithoutDrive()
{
	return m_sDCDWndTitleWithoutDrive;
}

//////////////////////////////////////////////////////////////////////
CString CWatchCDDlg::GetDCDWndTitleWithDrive()
{
	return m_sDCDWndTitleWithDrive;
}

//////////////////////////////////////////////////////////////////////
CString CWatchCDDlg::GetDCDWndTitleWithDriveVersion5()
{
	return m_sDCDWndTitleWithDriveVersion5;
}
/////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnIdle()
{

//TODO Falls andere Dialoge als die erwarteten (errros sind schon abgefangen) 
	//hochpoppen, diese abfangen und den Backup
	//Vorgang abbrechen, da dann etwas schief gelaufen ist.
	//z.B. nach dem Finalisieren kann es sein, daß CD-R nicht freigegeben wird.
	//Dialog mit "wait" und "try later" erscheint. Hier wait drücken

	switch(m_DCDStatus)
	{

	case WCD_START:
//		ChangeWCDStatus(WCD_ERROR);
		OnWCDStart();
		PostMessage(WM_USER);
		break;

	case WCD_WAIT_FOR_CDR:
		OnWCDWaitForCDR();
		PostMessage(WM_USER);
		break;

	case WCD_FORMAT:
		OnWCDFormatCDR();
		PostMessage(WM_USER);
		break;

	case WCD_BACKUP:
		OnWCDBackup();
		PostMessage(WM_USER);
		break;

	case WCD_FINALIZE:
		OnWCDFinalize();
		PostMessage(WM_USER);
		break;

	case WCD_ERROR:
		OnWCDError();
		PostMessage(WM_USER);
		break;

	case WCD_IS_DRIVE_LOCKED:
		OnWCDIsDriveLocked();
		PostMessage(WM_USER);
		break;

	case WCD_WAIT_FOR_EJECT:
		OnWCDWaitForEject();
		PostMessage(WM_USER);
		break;

	case WCD_WAIT_FOR_EJECT_FINISHED:
		OnWCDWaitForEjectFinished();
		PostMessage(WM_USER);
		break;

	case WCD_EJECT_ERROR:
		OnWCDEjectError();
		PostMessage(WM_USER);
		break;

	case WCD_WAIT_FOR_FILE_SYSTEM:
		OnWCDWaitForFileSystem();
		PostMessage(WM_USER);
		break;

	case WCD_FORCE_TO_EJECT:
		OnWCDForceToEject();
		PostMessage(WM_USER);
		break;

	case WCD_QUIT:
		OnOK();
		break;

	default:
		break;
	}

}

////////////////////////////////////////////////////////////////////////////
BOOL CWatchCDDlg::IsDVClient()
{
	BOOL bRet = FALSE;
	m_hDVClient = ::FindWindow(WK_APP_NAME_DVCLIENT,NULL);


	if(IsWindow(m_hDVClient))
	{
		bRet = TRUE;
	}
	return bRet;
}

////////////////////////////////////////////////////////////////////////////
HWND CWatchCDDlg::GetHandleDVClient()
{
	return m_hDVClient;
}

////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDStart() 
{
	//Info DirectCD Version5
	//Ablauf geht auch mit DirectCD Version5 da 
	//Versionsunterschiede in DCDStart behandelt werden

	Sleep(20);
	
//TODO TKR wieder rein, nur zum test ohne DVClient
//	if(!IsDVClient() && m_bDirectCDStarted)
	if(IsDVClient() && m_bDirectCDStarted)
	{
		//Start DirectCD5 and leave dialog with "Drive Information" on desktop
		if(m_pDCDStart->IsStarted())
		{
			ChangeWCDStatus(WCD_WAIT_FOR_CDR);
		}
		else
		{
			if(m_pDCDError->IsError())
			{
				m_dwTick = 0;
				ChangeWCDStatus(WCD_ERROR);
			}
		}
	}
	else
	{
		WK_TRACE(_T("OnWCDStart(): DVClient not on desktop or DirectCD Watchdog is not running. Quit WatchCD.\n"));
		ChangeWCDStatus(WCD_QUIT);
	}

	//checks if an error dialog pops up from directCD
	if(CheckForUnexpectedErrors())
	{
		m_dwTick = 0;
		WK_TRACE(_T("CWatchCDDlg::OnWCDStart(): An unexpected error occoured.\n"));
		m_pDCDError->ChangeErrorStatus(ERROR_UNEXPEXTED_ERROR_FROM_DIRECTCD);
		ChangeWCDStatus(WCD_ERROR);
	}
}

////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDWaitForCDR()
{
	//Info DirectCD Version5
	//Ablauf geht auch mit DirectCD Version5 da 
	//Versionsunterschiede in DCDStart behandelt werden

	Sleep(20);
	//Check Drive Status Field in Drive Information dialog 
	//if an empty CDR is or was insereted, start format
	if(m_pDCDStart->IsEmptyCDR())
	{
		ChangeWCDStatus(WCD_FORMAT);
	}
	else
	{
		if(m_pDCDError->IsError())
		{
			m_dwTick = 0;
			ChangeWCDStatus(WCD_ERROR);
		}
	}

	//checks if an error dialog pops up from directCD
	if(CheckForUnexpectedErrors())
	{
		m_dwTick = 0;
		WK_TRACE(_T("CWatchCDDlg::OnWCDWaitForCDR(): An unexpected error occoured.\n"));
		m_pDCDError->ChangeErrorStatus(ERROR_UNEXPEXTED_ERROR_FROM_DIRECTCD);
		ChangeWCDStatus(WCD_ERROR);
	}
}


////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDFormatCDR()
{
	Sleep(20);
	//Start format CDR and wait until CDR is formatted
	if(m_pDCDFormat->IsFormatted())
	{
		WK_TRACE(_T("CWatchCDDlg::OnWCDFormatCDR(): Format complete, start backup data\n"));
		m_bDVClientCopyingData = TRUE; 
		ChangeWCDStatus(WCD_BACKUP);
	}
	else
	{
		if(m_pDCDError->IsError())
		{
			m_dwTick = 0;
			ChangeWCDStatus(WCD_ERROR);
		}
	}
	
	//checks if an error dialog pops up from directCD
	if(CheckForUnexpectedErrors())
	{
		m_dwTick = 0;
		WK_TRACE(_T("CWatchCDDlg::OnWCDFormatCDR(): An unexpected error occoured.\n"));
		m_pDCDError->ChangeErrorStatus(ERROR_UNEXPEXTED_ERROR_FROM_DIRECTCD);
		ChangeWCDStatus(WCD_ERROR);
	}
}


////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		//ask every 60 sec: Is DVClient still copying data to CDR?
		//If true, m_bDVClientCopyingData is setted to true in OnReceiveStatus()
		if(m_bDVClientCopyingData)
		{
			WK_TRACE(_T("Daten werden noch kopiert.\n"));
			::PostMessage(m_hDVClient, WM_CHECK_BACKUP_STATUS, BS_DCD_COPYING_DATA,
														(LPARAM)m_hWnd);
			m_bDVClientCopyingData = FALSE;
		}
		else
		{
			WK_TRACE(_T("Keine Rückmeldung\n"));
			//abnormal stop of copying data
			m_bDVClientCopyingDataError = TRUE;
		}
		break;

	default:
		break;
	}
	
	CDialog::OnTimer(nIDEvent);
}

////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDBackup()
{
	Sleep(20);
	if(m_pDCDBackup->IsBackupComplete())
	{
		KillTimer(1);
		WK_TRACE(_T("CWatchCDDlg::OnWCDBackup(): Backup Complete\n"));
		ChangeWCDStatus(WCD_FINALIZE);
	}
	else
	{
		if(m_pDCDError->IsError())
		{
			m_dwTick = 0;
			ChangeWCDStatus(WCD_ERROR);
		}
	}

	//checks if an error dialog pops up from directCD	
	if(CheckForUnexpectedErrors())
	{
		m_dwTick = 0;
		WK_TRACE(_T("CWatchCDDlg::OnWCDBackup(): An unexpected error occoured.\n"));
		m_pDCDError->ChangeErrorStatus(ERROR_UNEXPEXTED_ERROR_FROM_DIRECTCD);
		ChangeWCDStatus(WCD_ERROR);
	}
}

////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDFinalize()
{
	Sleep(20);
	if(IsDVClient())
	{
		//Start DirectCD and wait until CDR is finalized
		if(m_pDCDFinalize->IsFinalized())
		{
			//msg to DVClient that finalize process successfully finished
			::PostMessage(m_hDVClient, WM_CHECK_BACKUP_STATUS, BS_DCD_FINALIZE_SUCCESS, 0);
			WK_TRACE(_T("Msg BS_DCD_FINALIZE_SUCCESS to DVClient sended.\n"));
			ChangeWCDStatus(WCD_QUIT);
		}
		else
		{
			if(m_pDCDError->IsError())
			{
				m_dwTick = 0;
				ChangeWCDStatus(WCD_ERROR);
			}
		}
	}
	else
	{
		WK_TRACE(_T("OnWCDFinalize(): DVClient not on desktop. Quit WatchCD.\n"));
		ChangeWCDStatus(WCD_QUIT);
	}


	//checks if an error dialog pops up from directCD
	if(CheckForUnexpectedErrors())
	{
		m_dwTick = 0;
		WK_TRACE(_T("CWatchCDDlg::OnWCDFinalize(): An unexpected error occoured.\n"));
		m_pDCDError->ChangeErrorStatus(ERROR_UNEXPEXTED_ERROR_FROM_DIRECTCD);
		ChangeWCDStatus(WCD_ERROR);
	}

}


////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDIsDriveLocked()
{
	//check if the backup drive is locked by DirectCD
	//if not, quit DirectCD and WatchCD
	//if drive is locked,
	//wait until the "Ejecting disc..." Dlg pops up on desktop 
	//only if "Ejecting disc.." Dlg appears, DirectCD UNLOCKS the backupdrive
	//if I close DirectCD before "Ejecting disc..." Dlg appears,
	//the CD-R cannot be removed from backupdrive !!
	//SO Keep WatchCD alive until "Ejecting disc..." Dlg appears

	if(IsDriveLocked())
	{
		ChangeWCDStatus(WCD_WAIT_FOR_EJECT);
	}
	else
	{
		ChangeWCDStatus(WCD_QUIT);
	}
}

////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDWaitForEject()
{
	//wait until "Ejecting disc..." Dlg appears on desktop
	Sleep(500);
	m_hEjectDlg = ::FindWindow(_T("#32770"), GetDCDWndTitleWithDrive());
	if(IsWindow(m_hEjectDlg))
	{
		TCHAR sText[128];

		::GetDlgItemText(m_hEjectDlg, 0x0403, sText, sizeof(sText));
		CString sEject(sText);
		if(sEject == m_pDCDStart->GetEjectingString())
		{
			WK_TRACE(_T("CWatchCDDlg  OnWCDWaitForEject(): found ejecting string: %s\n"), sEject);
			ChangeWCDStatus(WCD_WAIT_FOR_EJECT_FINISHED);
		}
		else if(sEject.Left(15) == m_pDCDStart->GetUnableToLockString().Left(15))
		{
			WK_TRACE(_T("CWatchCDDlg OnWCDWaitForEject(): Unable to lock gefunden !!\n"));
			ChangeWCDStatus(WCD_EJECT_ERROR);
		}
	}
}

////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDWaitForEjectFinished()
{
	Sleep(1000);
	m_hEjectDlg = ::FindWindow(_T("#32770"), GetDCDWndTitleWithDrive());
	if(!IsWindow(m_hEjectDlg))
	{
		WK_TRACE(_T("CWatchCDDlg OnWCDWaitForEjectFinished(): Ejecting dlg not longer on desktop.\n"));
		ChangeWCDStatus(WCD_QUIT);
	}
}


////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDEjectError()
{
	HWND hFirstChild = ::GetWindow(m_hEjectDlg, GW_CHILD);
	//click wait-button
	::PostMessage(m_hEjectDlg,WM_COMMAND,
					(WPARAM)MAKELONG(0x0006,BN_CLICKED),
					(LPARAM)hFirstChild);
	
	ChangeWCDStatus(WCD_WAIT_FOR_FILE_SYSTEM);

}

////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDWaitForFileSystem()
{
	m_hWaitForFileSystemDlg = ::FindWindow(_T("#32770"), GetDCDWndTitleWithDrive());

	if(IsWindow(m_hWaitForFileSystemDlg) && m_hWaitForFileSystemDlg != m_hEjectDlg)
	{
		//close WaitForFileSystem Dlg
		::PostMessage(m_hWaitForFileSystemDlg, WM_CLOSE, 0, 0);
		ChangeWCDStatus(WCD_FORCE_TO_EJECT);
	}

}

////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDForceToEject()
{
	HWND hForce = ::FindWindow(_T("#32770"), GetDCDWndTitleWithDrive());

	if(IsWindow(hForce) && (hForce != m_hEjectDlg) && (hForce != m_hWaitForFileSystemDlg))
	{
		//click Button "Eject Now"
		HWND hFirstChild = ::GetWindow(hForce, GW_CHILD);
		::PostMessage(hForce,WM_COMMAND,
					(WPARAM)MAKELONG(0x0006,BN_CLICKED),
					(LPARAM)hFirstChild);

		ChangeWCDStatus(WCD_WAIT_FOR_EJECT_FINISHED);
	}
}

////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnWCDError()
{

	Sleep(200);
	if(m_dwTick == 0)
	{
		m_dwTick = GetTickCount();
		WK_TRACE_ERROR(GetErrorString(m_pDCDError->GetErrorStatus()));
		//post message to DVClient, that an error occoured
		if(IsWindow(m_hDVClient))
		{

			if(m_pDCDError->GetErrorStatus() == ERROR_TIMEOUT_WAIT_FOR_EMPTY_CDR)
			{
				::PostMessage(m_hDVClient, WM_CHECK_BACKUP_STATUS, BS_DCD_NOT_FOUND_EMPTY_CDR, 0);
			}
			else if(m_pDCDError->GetErrorStatus() == ERROR_FOUND_NOT_EMPTY_CDR)
			{
				::PostMessage(m_hDVClient, WM_CHECK_BACKUP_STATUS, BS_DCD_FOUND_NOT_EMPTY_CDR, 0);
			}
			else if(m_pDCDError->GetErrorStatus() == ERROR_UNEXPEXTED_ERROR_FROM_DIRECTCD)
			{
				::PostMessage(m_hDVClient, WM_CHECK_BACKUP_STATUS, BS_DCD_UNEXPECTED_ERROR, 0);
			}
			else
			{
				::PostMessage(m_hDVClient, WM_CHECK_BACKUP_STATUS, BS_DCD_ERROR, 0);
			}
		}
		else
		{
			WK_TRACE(_T("OnWCDError: DVClient not found on desktop\n"));
		}
	}

	if(!m_bDirectCDVersion5)
	{
		WK_TRACE(_T("OnWCDError(): Check if drive is locked and quit\n"));
		ChangeWCDStatus(WCD_IS_DRIVE_LOCKED);
	}
	else
	{
		//DirectCD Version 5
		//TODO TKR hier noch mit einbauen, was passieren soll, wenn keine leere CD-R eingelegt
		//wurde, sondern eine beschriebene. Ist diese dann gelockt, falls die Wartezeit 
		//(warten auf Format-DLG) überschritten wird.
		//Eine nicht leerer CD-R muss dann noch ausgeworfen werden.
		WK_TRACE(_T("OnWCDError(): Close DirectCD Version5 and quit\n"));
		ChangeWCDStatus(WCD_QUIT);
	}
}


////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnCancel() 
{
	DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::OnOK() 
{

	m_pDCDStart->CloseStartDialogs();

	WK_DELETE(m_pDCDError);
	WK_DELETE(m_pDCDStart);
	WK_DELETE(m_pDCDFormat);
	WK_DELETE(m_pDCDBackup);
	WK_DELETE(m_pDCDFinalize);
	
	HWND hWatchCD = NULL;
	if(!m_bDirectCDVersion5)
	{
		hWatchCD = ::FindWindow(NULL, _T("Adaptec DirectCD Wizard"));
	}
	else
	{
		//DirectCD Version 5
		hWatchCD = ::FindWindow(NULL, _T("DirectCD"));
	}


	if(hWatchCD && IsWindow(hWatchCD))
	{
		WK_TRACE(_T("CWatchCDDlg OnOK(): Close Msg to DirectCD Watchdog gesendet\n"));
		::PostMessage(hWatchCD, WM_QUIT, 0, 0);
	}
	else
	{
		Sleep(1000); //wait and try again to find DirectCD on desktop
		hWatchCD = ::FindWindow(NULL, _T("Adaptec DirectCD Wizard"));
		if(hWatchCD && IsWindow(hWatchCD))
		{
			WK_TRACE(_T("CWatchCDDlg OnOK(): Close Msg to DirectCD Watchdog gesendet\n"));
			::PostMessage(hWatchCD, WM_QUIT, 0, 0);
		}
		else
		{
			WK_TRACE(_T("CWatchCDDlg OnOK(): Couldn`t close DirectCD Watchdog, because no handle: 0x%x\n"), hWatchCD);
		}
	}
//TODO TKR wieder raus ?
	if(m_bDirectCDVersion5)
	{
		while(hWatchCD && IsWindow(hWatchCD))
		{
			Sleep(1000);
			hWatchCD = ::FindWindow(NULL, _T("DirectCD"));
			WK_TRACE(_T("CWatchCDDlg OnOK(): DirectCD5 noch da\n"));
			::PostMessage(hWatchCD, WM_QUIT, 0, 0);
		}
	}
//raus ende

	DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////


void CWatchCDDlg::PostNcDestroy() 
{
	delete this;

}

/////////////////////////////////////////////////////////////////////////////
long CWatchCDDlg::OnReceiveStatus(WPARAM wParam, LPARAM lParam)
{

	switch (wParam)
	{

	case BS_DCD_DATA_COPIED:
		WK_TRACE(_T("DVClient: kopieren beendet\n"));
		m_bDVClientBackupComplete = TRUE;
		break;

	case BS_DCD_COPYING_DATA:
		m_bDVClientCopyingData = TRUE;
		break;

	case BS_DCD_BACKUP_FAILED:
		WK_TRACE(_T("DVClient: Backup failed\n"));
		m_bDVClientCopyingDataError = TRUE;
		break;

	default:
		break;
	}
	PostMessage(WM_USER);
	return 0;
}


////////////////////////////////////////////////////////////////////////////
void CWatchCDDlg::ChangeWCDStatus(DirectCDStatus newStatus)
{
	m_LastDCDStatus = m_DCDStatus;
	m_DCDStatus = newStatus;

	WK_TRACE(_T("CWatchCDDlg() WCDStatus: from last %s to new %s\n")
			,NameOfEnumDirectCD(m_LastDCDStatus)
			,NameOfEnumDirectCD(m_DCDStatus));

}

///////////////////////////////////////////////////////////////////////
//gets the string on the delivered position (iStringID) from the sFile
CString CWatchCDDlg::GetStringFromTable(CString sPath, CString sFile, int iStringID)
{
	CString s(_T(""));
	CString sText(_T(""));

	HMODULE hFile = LoadLibraryEx(sPath + _T("\\") + sFile,NULL,0);

	if(hFile != NULL)
	{
		LoadString(hFile,iStringID,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
		sText.ReleaseBuffer();
	}

	FreeLibrary(hFile);
	return sText;
}



///////////////////////////////////////////////////////////////////////
//gets the string on the delivered position (iStringID) from the sFile
CString CWatchCDDlg::GetStringFromDlg(CString sPath, CString sFile, CString sDlgID)
{

//TODO funktion wieder raus ?? brauche ich sie doch ??
	CString sDlgTitle(_T(""));
	CString s(_T(""));

	HMODULE hFile = LoadLibraryEx(sPath + _T("\\") + sFile,NULL,0);
	HRSRC FindRes = FindResourceEx(hFile, RT_DIALOG, sDlgID, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
	if(FindRes != NULL)
	{
		HGLOBAL LoadRes = LoadResource(hFile, FindRes);
		if(LoadRes != NULL)
		{
			LPCDLGTEMPLATE pLockRes = (LPCDLGTEMPLATE)LockResource(LoadRes);
	
			CDialog dlg;
			dlg.CreateIndirect(pLockRes);
			dlg.ShowWindow(SW_HIDE);	
			dlg.GetWindowText(sDlgTitle);
			dlg.DestroyWindow();
			FreeResource(LoadRes);
		}
		else
		{
			WK_TRACE_ERROR(_T("CWatchCDDlg GetStringFromDlg(): Error %u ; %s\n"),
							GetLastError(), GetLastErrorString());
		}
	}
	else
	{
			WK_TRACE_ERROR(_T("GetStringFromDlg(): Error %u ; %s\n"),
							GetLastError(), GetLastErrorString());
	}
	return sDlgTitle;
}
////////////////////////////////////////////////////////////////////////////
BOOL CWatchCDDlg::IsDriveLocked()
{

	//Prüfen ob DirectCD die CD-R gelockt hat, geht wohl auch, wenn ich den Status der
	//CD-R innerhalb DirectCD abfrage.
	//Gelockt heisst, dass im Dialog Driveinfo die CD-R als DirectCD disc erkannt
	// wird, die nun bereit zu Beschreiben ist.


	CString sDirectCDDisc;
	CString sLoc = _T("DirectCD.loc");

	HWND hWnd = m_pDCDStart->GetHdlDeviceField();
	if (hWnd && IsWindow(hWnd))
	{
		CString sText;
		::GetWindowText(hWnd,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
		sText.ReleaseBuffer();

		if(FileExists(GetPathToDirectCD(), sLoc))
		{
			sDirectCDDisc = GetStringFromTable(GetPathToDirectCD(),
											_T("directcd.loc"), 404);
		}
		else
		{
			sDirectCDDisc = GetStringFromTable(GetPathToDirectCD(),
											_T("directcd.exe"), 404);
		}

		WK_TRACE(_T("CWatchCDDlg::IsDriveLocked(): Status Field Text: %s\n"), sText);
		if(sText.Left(15) == sDirectCDDisc.Left(15))
		{
			WK_TRACE(_T("CWatchCDDlg::IsDriveLocked(): Drive is locked.\n"));
			return !IsCDEjected();
		}

		if(m_pDCDFinalize->IsDeciveLocked())
		{
			return !IsCDEjected();
		}
	}
	else
	{
		//Dialog DriveInfo ist nicht mehr vorhanden
		return !IsCDEjected();
	}

	return FALSE;
}
////////////////////////////////////////////////////////////////////////////
//Post Nachricht zum DirectCD Watchdog zum Auswurf der CD-R
BOOL CWatchCDDlg::IsCDEjected()
{
	BOOL bRet = FALSE;
	//message parameter from DirectCD WatchDog
	HWND hDCDWatchDog = ::FindWindow(NULL, _T("Adaptec DirectCD Wizard"));
	WPARAM wParam = 0x0102;
	LPARAM lParam = 0x0001;
	if(hDCDWatchDog)
	{
		DWORD dwTick = GetTickCount();
		//send different wParams, because 
		//Mitsumi 4804TE needs  104
		//Mitsumi 48x5TE needs  104 
		//LiteOn LTR-0841 needs 103
		//Philips		 needs  106
		for(wParam = 0x0102; wParam <= 0x0107; wParam++)
		{
			::PostMessage(hDCDWatchDog, 0x8100, wParam, lParam);
			WK_TRACE(_T("CWatchCDDlg::EjectCD(): wParam: 0x%x nach %i ms\n"), wParam, GetTickCount()-dwTick );
			bRet = TRUE;
		}
		while((GetTickCount() - dwTick) <= 15 * 1000)
		{
			Sleep(50);
		}
	}

	return bRet;
}
////////////////////////////////////////////////////////////////////////////
BOOL CWatchCDDlg::IsDirectCDVersion5()
{
	return m_bDirectCDVersion5;
}
////////////////////////////////////////////////////////////////////////////
BOOL CWatchCDDlg::CheckForUnexpectedErrors()
{
	
	BOOL bRet = FALSE;
	//check unexpected errors from DirectCD:
	//-all error messages; MsgWindowsText starts with "Error :"

	LPARAM lParam = 0;
	EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, lParam);

	if(IsWindow(m_hUnexpErrorWnd))
	{
		HWND hFirstChild = ::GetWindow(m_hUnexpErrorWnd, GW_CHILD);
		::PostMessage(m_hUnexpErrorWnd,WM_COMMAND,
					(WPARAM)MAKELONG(m_dwYesButtonOnErrorDlg,BN_CLICKED),
					(LPARAM)hFirstChild);
		WK_TRACE(_T("CWatchCDDlg::CheckForUnexpectedErrors(): Yes button on Error Dialog clicked.\n"));
//falls mal Fenster von DirectCD stehen bleiben (außer "eject disc" - fenster, diese schließen
/*
		LPARAM lParam = 1;
		//get active DirectCD window and close it
		EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, lParam);
		if(IsWindow(m_hOnUnexpErrorActiveDCDWnd))
		{
			::PostMessage(m_hOnUnexpErrorActiveDCDWnd, WM_CLOSE, 0,0); 
		}
*/
		bRet = TRUE;

	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CALLBACK CWatchCDDlg::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	BOOL bRet = TRUE;

	if (hWnd && IsWindow(hWnd))
	{
		CString sText;
		::GetWindowText(hWnd,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
		sText.ReleaseBuffer();
	
		switch(lParam)
		{
		case 0:
			//find DirectCD ErrorMessage Window
			if(0 == sText.Find(_T("Error")))
			{
				m_pWatchCD->m_hUnexpErrorWnd = hWnd;
				WK_TRACE(_T("CWatchCDDlg(): Unexpected DirectCD Error: '%s'\n"), sText);
				bRet = FALSE;
			}
			break;

		case 1:
			//find active DirectCD Window if an unexpected error occoured
			if (0 == sText.Find(_T("Adaptec DirectCD Wizard")))
			{
				m_pWatchCD->m_hOnUnexpErrorActiveDCDWnd = hWnd;
				WK_TRACE(_T("CWatchCDDlg(): Close active DirectCD Window.\n"));
				bRet = FALSE;
			}
			break;
	/*
		case 2:
			//find new DirectCD window wich includes the string "Ejecting disc..."
			if(sWndText == m_pWatchCD->GetDCDWndTitleWithDrive())
			{

			}
	*/
		default:
			break;
		
		}
	}

	return bRet;
}


