/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: SavicUnitApp.cpp $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/SavicUnitApp.cpp $
// CHECKIN:		$Date: 20.06.03 13:15 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 20.06.03 12:25 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CInstanceCounter.h"
#include "SavicUnitApp.h"
#include "SavicUnitDlg.h"
#include "CSavicException.h"
#include "OemGui\OemGuiApi.h"
#include "..\SavicDA\SavicDirectAccess.h"
#include "CMyDebug.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL CleanUp();

/////////////////////////////////////////////////////////////////////////////
CSaVicApp theApp;


/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSaVicApp, CWinApp)
	//{{AFX_MSG_MAP(CSaVicApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CSaVicApp::CSaVicApp()
{
//	afxMemDF = allocMemDF | checkAlwaysMemDF;

	m_pCSaVicException	= NULL ;
	m_pMainWnd			= NULL;
	m_pInstance			= NULL;
	// Suchpfade ermitteln
	InitPathes();
}

/////////////////////////////////////////////////////////////////////////////
CSaVicApp::~CSaVicApp()
{
	WK_TRACE(_T("CSaVicApp::~CSaVicApp()\n"));
	m_pMainWnd  = NULL;
	WK_DELETE(m_pMutex);
	WK_DELETE(m_pCSaVicException);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicApp::Dump( CDumpContext &dc ) const
{
	 CObject::Dump( dc );
	 dc << _T("CSaVicApp = ");
}	  

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicApp::InitInstance()
{
	WK_ApplicationId	AppID	= WAI_INVALID;

	// Instanzen zählen.
	m_pInstance = new CInstanceCounter();
	if (m_pInstance)
		AppID = m_pInstance->GetAppID();
	if (AppID == WAI_INVALID)
		return FALSE;

	// Exceptionhandler umbiegen
	m_pCSaVicException = new CSaVicException(AppID, this);
							   
	CString sAppName = GetAppnameFromId(AppID);
	WK_ALONE(sAppName);	 
	CWK_RunTimeError::SetDefaultID(AppID);

	// Logfilename erzeugen
	CString sLogFile = sAppName+_T(".log");

	// Debugger öffnen
	InitDebugger(sLogFile,AppID,m_pCSaVicException);

	// Debug der Library initialisieren
	m_pDebug = new CMyDebug;
	
	WK_TRACE(_T("Starting SaVicUnit..."));
	WK_PRINT_VERSION(7, 4, 16, 31, _T("07/04/99,16:31\0"), // @AUTO_UPDATE
		0);	// DO NOT TOUCH THIS LINE

	SET_WK_STAT_TICK_COUNT(GetTickCount());

	m_pszAppName = _tcsdup(COemGuiApi::GetApplicationName(AppID));

	// Ist der Gerätetreiber geladen?
	if (!DAIsValid())
	{
		CString sError;
		sError.LoadString(IDS_RTE_DRIVER_NOT_LOADED);

		CWK_RunTimeError RTerr(REL_ERROR, RTE_OS_EXCEPTION, sError, 0, 0);
		RTerr.Send();

		return FALSE;
	}

	// Stimmen die Versionen überein?
	DWORD dwDllVersion = DAGetSaVicDADllVersion();
	if (dwDllVersion != SAVIC_UNIT_VERSION)
	{
		CString sError, sMsg;
		sMsg.LoadString(IDS_RTE_DLL_WRONG_VERSION);

		sError.Format(sMsg, dwDllVersion, SAVIC_UNIT_VERSION);

		CWK_RunTimeError RTerr(REL_CANNOT_RUN, RTE_CONFIGURATION, sError, 0, 0);
		RTerr.Send();

		return FALSE;
	}
	
	// Dongle vorhanden?
/*	CWK_Dongle dongle(AppID);  
	if (dongle.IsExeOkay()==FALSE)
	{
		return FALSE;
	}
*/
	WK_STAT_LOG(_T("Reset"), 0, _T("Active"));
	WK_STAT_LOG(_T("Reset"), 1, _T("Active"));

	// Hauptfenster anlegen
	m_pSaVicUnitDlg = new CSaVicUnitDlg();

	// Und erzeugen
	if (m_pSaVicUnitDlg && m_pSaVicUnitDlg->Create())
	{
		m_pMainWnd = m_pSaVicUnitDlg;
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
int CSaVicApp::ExitInstance()
{
	WK_TRACE(_T("ExitInstance\n"));

	// Den Rest aufräumen.
	CleanUp();

	WK_DELETE(m_pDebug);
	return m_msgCur.wParam; // returns the value from PostQuitMessage
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicApp::CleanUp()
{
	WK_TRACE(_T("CleanUp\n"));

	// Instanzen-objekt zerstören
	WK_DELETE(m_pInstance);

	// Debugger schließen
	CloseDebugger();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicApp::InitPathes()
{
	_TCHAR  szPath[_MAX_PATH];
	CString sPath;

	GetModuleFileName(m_hInstance,szPath,sizeof(szPath));
	sPath = szPath;
	// set important dir informations only once
	int p;

	m_sWorkingDirectory = szPath;
	p = m_sWorkingDirectory.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		m_sWorkingDirectory = m_sWorkingDirectory.Left(p);
	}
	else
	{
		::GetCurrentDirectory(_MAX_PATH,szPath);
		m_sWorkingDirectory = szPath;
	}

	m_sWorkingDirectory.MakeLower();

	::GetWindowsDirectory(szPath,_MAX_PATH);
	m_sWindowsDirectory = szPath;
	
	::GetSystemDirectory(szPath,_MAX_PATH);
	m_sSystemDirectory = szPath;

	::GetCurrentDirectory(_MAX_PATH,szPath);
	m_sCurrentDirectory = szPath;

}

/////////////////////////////////////////////////////////////////////////////
WK_ApplicationId CSaVicApp::GetApplicationId()
{
	WK_ApplicationId AppId = WAI_INVALID;

	if (m_pInstance)
		AppId = m_pInstance->GetAppID();
		
	return AppId;
}
