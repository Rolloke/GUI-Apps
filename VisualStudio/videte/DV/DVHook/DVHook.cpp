// DVHook.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DVHook.h"
#include "DVHookDlg.h"
#include "..\LangDll\langdll.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDVHookApp
static const CString DV_DVHOOK = _T("DV\\DVHook\\");

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDVHookApp, CWinApp)
	//{{AFX_MSG_MAP(CDVHookApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVHookApp construction

/////////////////////////////////////////////////////////////////////////////
CDVHookApp::CDVHookApp()
{
	m_hLangDll  = NULL;
	m_sLanguage = _T(" ");
}

CDVHookApp::~CDVHookApp()
{
	if (m_hLangDll) AfxFreeLibrary(m_hLangDll);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDVHookApp object

CDVHookApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDVHookApp initialization

/////////////////////////////////////////////////////////////////////////////
BOOL CDVHookApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

//	AfxMessageBox(_T("Achtung!\nBevor diese Anwendung gestartet wird sollte"))
	if (!WK_ALONE(WK_APP_NAME_DVHOOK))
	{
		return FALSE;
	}

	InitDebugger(_T("DVHook.log"), WAI_DV_HOOK);

	bool bHidden = PrepareHook();

	_TCHAR szPath[_MAX_PATH];
	GetModuleFileName(m_hInstance,szPath,sizeof(szPath));
	m_sHomePath = szPath;
	int nI = m_sHomePath.ReverseFind('\\');
	if (nI != -1)
	{
		m_sHomePath = m_sHomePath.Left(nI+1);
	}

	
	CDVHookDlg dlg;
	m_pMainWnd = &dlg;
	dlg.m_bHidden = bHidden;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CDVHookApp::ExitInstance() 
{
	CloseDebugger();
	
	return CWinApp::ExitInstance();
}

CString CDVHookApp::InitHookValue(CWK_Profile &prof, LPCTSTR strProg, LPCTSTR strCommand, bool bForce/*=false*/)
{
	CString sValue;
	if (bForce) sValue = strCommand;
	else        sValue = prof.GetString(DV_DVHOOK, strProg, strCommand);
	prof.WriteString(DV_DVHOOK, strProg, sValue);
	WK_TRACE(_T("%s -> %s\n"), strProg, sValue);
	return sValue;
}

/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CDVHookApp
 Function   : PrepareHook
 Description: Initializes the registry keys for the applications that are controlled by DVHook

 Parameters: None 

 Result type:  (void)
 <TITLE PrepareHook>
*********************************************************************************************/
bool CDVHookApp::PrepareHook()
{
	CWK_Profile prof;
	CString sValue;
	bool bHidden = false;
	COsVersionInfo osinfo;

	InitHookValue(prof, _T("C2UDFSVC.exe")				, DV_HIDE);
	InitHookValue(prof, _T("Cdewatch.exe")				, DV_HIDE);
	InitHookValue(prof, _T("DBGGUI.exe")				, DV_IGNORE);
	InitHookValue(prof, _T("DVClient.exe")				, DV_IGNORE);
	InitHookValue(prof, _T("DVProcess.exe")				, DV_IGNORE);
	InitHookValue(prof, _T("DVStarter.exe")				, DV_IGNORE);
	InitHookValue(prof, _T("DVStorage.exe")				, DV_IGNORE);
	InitHookValue(prof, _T("ControlService.exe")		, DV_IGNORE);
	InitHookValue(prof, _T("EwfControl.exe")			, DV_IGNORE);
	InitHookValue(prof, _T("HiveInjection.exe")			, DV_IGNORE);
	InitHookValue(prof, _T("HiveInjectionService.exe")	, DV_IGNORE);

	InitHookValue(prof, _T("DVHook.exe")        , DV_CONTROL);	// Kontrolle mit DVHook
	InitHookValue(prof, _T("DVHook.exe<*>")     , DV_IGNORE);	// alle anderen <*> Dialoge ignorieren
	sValue = InitHookValue(prof, _T("DVHook.exe<DVHook>"), DV_HIDE);// "DVHook" selbst verstecken
	if (sValue == DV_HIDE) bHidden = true;
	
#ifdef _DEBUG																	// Debugging
	sValue = DV_IGNORE;
	prof.WriteString(DV_DVHOOK, _T("MSDEV.exe"), sValue);
	WK_TRACE(_T("MSDEV.exe -> %s\n"), sValue);
	prof.WriteString(DV_DVHOOK, _T("Explorer.exe"), sValue);
	WK_TRACE(_T("Explorer.exe -> %s\n"), sValue);
	prof.WriteString(DV_DVHOOK, _T("OUTLOOK.EXE"), sValue);
	WK_TRACE(_T("OUTLOOK.EXE -> %s\n"), sValue);
	prof.WriteString(DV_DVHOOK, _T("taskmgr.exe"), sValue);
	WK_TRACE(_T("taskmgr.exe -> %s\n"), sValue);
	prof.WriteString(DV_DVHOOK, _T("ModifyStyle.exe"), sValue);
	WK_TRACE(_T("ModifyStyle.exe -> %s\n"), sValue);
#else
	if (osinfo.IsWinXP())
	{	// Explorer wird zuerst als Taskbar gestartet und laesst sich nicht mehr schliessen
		InitHookValue(prof, _T("Explorer.exe"), DV_HIDE);
	}
	else
	{	// Explorer wird hier als Fenster gestartet
		InitHookValue(prof, _T("Explorer.exe"), DV_IGNORE);
	}
#endif

	InitHookValue(prof, _T("Winfile.exe")    , DV_IGNORE);
	InitHookValue(prof, _T("VideteTweak.exe"), DV_IGNORE);
	InitHookValue(prof, _T("LogView.exe")    , DV_IGNORE);
	// Units
	InitHookValue(prof, _T("AudioUnit.exe")    , DV_IGNORE);
	InitHookValue(prof, _T("CommUnit.exe")     , DV_IGNORE);
	InitHookValue(prof, _T("ISDNUnit.exe")     , DV_IGNORE);
	InitHookValue(prof, _T("JaCobUnit.exe")    , DV_IGNORE);
	InitHookValue(prof, _T("SaVicUnit.exe")    , DV_IGNORE);
	InitHookValue(prof, _T("TashaUnit.exe")    , DV_IGNORE);
	InitHookValue(prof, _T("QUnit.exe")		   , DV_IGNORE);
	InitHookValue(prof, _T("AudioUnit.exe")    , DV_IGNORE);
	InitHookValue(prof, _T("USBCameraUnit.exe"), DV_IGNORE);
	InitHookValue(prof, _T("SocketUnit.exe")   , DV_IGNORE);

	InitHookValue(prof, _T("Regedit.exe") , DV_IGNORE);
	InitHookValue(prof, _T("RemoteCD.exe"), DV_HIDE);
	InitHookValue(prof, _T("UDFForm.exe") , DV_HIDE);
	InitHookValue(prof, _T("UDFOrig.exe") , DV_HIDE);
	InitHookValue(prof, _T("WatchCD.exe") , DV_HIDE);	//brennen mit DirectCD
	InitHookValue(prof, _T("ACDC.exe")    , DV_HIDE);	//brennen mit ACDC
	InitHookValue(prof, _T("DirectCD.exe"), DV_HIDE);

	InitHookValue(prof, _T("rdisk.exe")   , DV_HIDE);
	InitHookValue(prof, _T("services.exe"), DV_IGNORE);
	InitHookValue(prof, _T("spoolsv.exe") , DV_IGNORE);
	InitHookValue(prof, _T("svhost.exe")  , DV_IGNORE);
	InitHookValue(prof, _T("lsass.exe")   , DV_IGNORE);
	InitHookValue(prof, _T("winlogon.exe"), DV_IGNORE);
	InitHookValue(prof, _T("csrss.exe")   , DV_IGNORE);
	InitHookValue(prof, _T("smss.exe")    , DV_IGNORE);
	
	// Installation über USB
	sValue = prof.GetString(DV_DVHOOK, _T("rundll32.exe"), DV_CONTROL);	// Kontrollieren 
	prof.WriteString(DV_DVHOOK, _T("rundll32.exe"), sValue);					// der rundll32.exe
	prof.WriteString(DV_DVHOOK, _T("rundll32.exe<*>"), DV_CONTROL);		// der Dialoge von rundll32.exe aufgerufen
	prof.WriteString(DV_DVHOOK, _T("rundll32.exe<Hardware Installation>"), DV_IGNORE);// Dialog Hardware Installation (Driver Signing)
	WK_TRACE(_T("rundll32.exe -> %s\n"), sValue);
	// Alle Fenster, die von rundll32.exe gestartet werden kontrollieren
	prof.WriteString(DV_DVHOOK + _T("rundll32.exe<*>\\"), _T("Message01"), DV_SHOW);// Fenster
	prof.WriteString(DV_DVHOOK + _T("rundll32.exe<*>\\"), _T("lParam01") , DV_TOPMOST);// als Topmost
	prof.WriteInt(   DV_DVHOOK + _T("rundll32.exe<*>\\"), _T("wParam01") , 1);		// anzeigen
	prof.WriteString(DV_DVHOOK + _T("rundll32.exe<*>\\"), _T("Message02"), DV_PASSWORD);// Passwortabfrage
	prof.WriteInt(   DV_DVHOOK + _T("rundll32.exe<*>\\"), _T("lParam02") , 180);// 3 Minuten Zeitfenster
	prof.WriteString(DV_DVHOOK + _T("rundll32.exe<*>\\NotOk"), _T("Message01"), DV_CLOSE);// bei nicht ok schließen
	prof.WriteString(DV_DVHOOK + _T("rundll32.exe<*>\\Ok"), _T("Message01"), DV_SHOW);	// bei ok Show
	prof.WriteString(DV_DVHOOK + _T("rundll32.exe<*>\\Ok"), _T("lParam01"), DV_TOPMOST);// als TopMost
	prof.WriteInt(   DV_DVHOOK + _T("rundll32.exe<*>\\Ok"), _T("wParam01"), 1);			// = 1
	
	return bHidden;
}

void CDVHookApp::GetLanguageDll()
{
	// look for language dlls
	CString sSearch = m_sHomePath + _T("DV");
	sSearch += _T("???");	// Do NOT translate, wildcard for chars
#ifdef _DEBUG
	sSearch += _T("Debug");
#endif
	sSearch += _T(".Dll");
	HANDLE hF;
	WIN32_FIND_DATA FindFileData;
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\"),_T(""));
	CString sLanguage = wkp.GetString(_T("DV\\DVClient"), _T("Language"), _T("English"));
	if (m_sLanguage != sLanguage)
	{
		hF = FindFirstFile(LPCTSTR(sSearch), &FindFileData);
		while (hF != INVALID_HANDLE_VALUE)
		{
			if ((_tcscmp(FindFileData.cFileName,_T("."))!=0) &&
				 (_tcscmp(FindFileData.cFileName,_T(".."))!=0))
			{
				if ((FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0)
				{
					HINSTANCE hInstance = AfxLoadLibrary(FindFileData.cFileName);

					if (hInstance)
					{
						CString sFoundLanguage;
						FARPROC theProc = ::GetProcAddress(hInstance, "GetLanguageName");
						if (theProc)
						{
							sFoundLanguage = ((GET_LANGUAGE_NAME_PTR)theProc)();
							if (sLanguage == sFoundLanguage)
							{
								m_hLangDll = hInstance;
								break;
							}
						}
						AfxFreeLibrary(hInstance);
					}
				}
			} 
			if (!FindNextFile(hF,&FindFileData))
			{
				break;
			}
		}
		if (hF != INVALID_HANDLE_VALUE)
		{
			FindClose(hF);
		}
	}
}

/*
	sValue = prof.GetString(DV_DVHOOK, "rundll32.exe<Files Needed>", DV_IGNORE);		// Dialog "Files Needed" ingnorieren
	prof.WriteString(DV_DVHOOK, "rundll32.exe<Files Needed>", sValue);

	// im Dialog "Locate File" Texteingabefeld in der Combobox für Dateinamen Disablen
	prof.WriteString(DV_DVHOOK + "rundll32.exe<Locate File>\\", "Message01", DV_ENABLE);
	prof.WriteInt(   DV_DVHOOK + "rundll32.exe<Locate File>\\"  , "DlgItem01", 0x000047c);
	prof.WriteInt(   DV_DVHOOK + "rundll32.exe<Locate File>\\"  , "wParam01" , 0x0000000);
*/
/*
	// Den Browse Button zu disablen reicht nicht !
	// das Find Files Fenster muß geschlossen werden und dafür
	// der eigene FindFiles Dialog geöffnet werden !
	sValue = prof.GetString(DV_DVHOOK, "rundll32.exe", DV_CONTROL);
	prof.WriteString(DV_DVHOOK, "rundll32.exe", sValue);
	WK_TRACE("rundll32.exe -> %s\n", sValue);
	prof.WriteString("DV\\DVHook\\rundll32.exe<Files Needed>\\", "Message01", DV_ENABLE);
	prof.WriteInt("DV\\DVHook\\rundll32.exe<Files Needed>"     , "DlgItem01", 0x000006e);
	prof.WriteInt("DV\\DVHook\\rundll32.exe<Files Needed>"     , "wParam01" , 0x0000000);
	prof.WriteString("DV\\DVHook\\rundll32.exe<Files Needed>\\", "Message02", DV_PASSWORD);

	prof.WriteString("DV\\DVHook\\rundll32.exe<Files Needed>\\Ok", "Message01", DV_ENABLE);
	prof.WriteInt("DV\\DVHook\\rundll32.exe<Files Needed>\\Ok"   , "DlgItem01", 0x000006e);
	prof.WriteInt("DV\\DVHook\\rundll32.exe<Files Needed>\\Ok"   , "wParam01" , 0x0000001);
	
	prof.WriteString("DV\\DVHook\\rundll32.exe<Files Needed>\\NotOk", "Message01", DV_ENABLE);
	prof.WriteInt("DV\\DVHook\\rundll32.exe<Files Needed>\\NotOk"   , "DlgItem01", 0x000006e);
	prof.WriteInt("DV\\DVHook\\rundll32.exe<Files Needed>\\NotOk"   , "wParam01" , 0x0000000);

	prof.WriteString(DV_DVHOOK, "rundll32.exe<*>", DV_IGNORE);
*/
