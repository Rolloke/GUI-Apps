/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: TashaUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/TashaUnit.cpp $
// CHECKIN:		$Date: 3.02.05 13:07 $
// VERSION:		$Revision: 28 $
// LAST CHANGE:	$Modtime: 3.02.05 10:02 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CInstanceCounter.h"
#include "TashaUnit.h"
#include "TashaUnitDlg.h"
#include "CTashaException.h"
#include "CMyDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
LARGE_INTEGER ReadRDTSC()
{
	static LARGE_INTEGER lRDTSC;
	_asm {             
		_asm push eax  
		_asm push edx  
		_asm _emit 0Fh 
		_asm _emit 31h 
		_asm mov lRDTSC.LowPart, eax 
		_asm mov lRDTSC.HighPart, edx
		_asm pop edx            
		_asm pop eax            
	} 
	return lRDTSC;
}

/////////////////////////////////////////////////////////////////////////////
// CTashaUnitApp

BEGIN_MESSAGE_MAP(CTashaUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CTashaUnitApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTashaUnitApp construction

CTashaUnitApp::CTashaUnitApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTashaUnitApp object

CTashaUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTashaUnitApp initialization

BOOL CTashaUnitApp::InitInstance()
{
	WK_ApplicationId	AppID	= WAI_INVALID;

	XTIB::SetThreadName(_T("MainThread"));

	// Instanzen zählen.
	m_pInstance = new CInstanceCounter();
	if (m_pInstance)
		AppID = m_pInstance->GetAppID();
	if (AppID == WAI_INVALID)
		return FALSE;

	// Exceptionhandler umbiegen
	m_pCTashaException = new CTashaException(AppID, this);
							   
	CString sAppName = GetAppnameFromId(AppID);
	WK_ALONE(sAppName);	 
	CWK_RunTimeError::SetDefaultID(AppID);

	// Logfilename erzeugen
	CString sLogFile = sAppName+_T(".log");

	// Debugger öffnen
	InitDebugger(sLogFile,AppID, m_pCTashaException);

	// Debug der Library initialisieren
	m_pDebug = new CMyDebug;

	SET_WK_STAT_TICK_COUNT(1);

	LARGE_INTEGER PerfFreq	= {0,0};
	LARGE_INTEGER PerfStart1	= {0,0};
	LARGE_INTEGER PerfEnd1	= {0,0};

	QueryPerformanceFrequency(&PerfFreq);
	QueryPerformanceCounter(&PerfStart1);
	QueryPerformanceCounter(&PerfEnd1);
	TRACE(_T("%d\n"), PerfEnd1.QuadPart - PerfStart1.QuadPart);

	LARGE_INTEGER PerfStart2= {0,0};
	LARGE_INTEGER PerfEnd2	= {0,0};
	
	PerfStart2 = ReadRDTSC();
	PerfEnd2   = ReadRDTSC();
	TRACE(_T("%d\n"), PerfEnd2.QuadPart - PerfStart2.QuadPart);

#if _MFC_VER <0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	AfxEnableControlContainer();

//	SetPriorityClass(GetCurrentProcess() , HIGH_PRIORITY_CLASS);
	SetPriorityClass(GetCurrentProcess() , NORMAL_PRIORITY_CLASS);
	
	// Hauptfenster anlegen
	m_pTashaUnitDlg = new CTashaUnitDlg();

	// Und erzeugen
	if (m_pTashaUnitDlg && m_pTashaUnitDlg->Create())
	{ 
		m_pMainWnd = m_pTashaUnitDlg;
		return TRUE;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CTashaUnitApp::ExitInstance() 
{
	WK_DELETE(m_pInstance);

	CloseDebugger();
	
	WK_DELETE(m_pCTashaException);
	WK_DELETE(m_pDebug);

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
int CTashaUnitApp::GetCardRef()
{
	return m_pInstance->GetInstance()+1;
}

/////////////////////////////////////////////////////////////////////////////
WK_ApplicationId CTashaUnitApp::GetApplicationId()
{
	WK_ApplicationId AppId = WAI_INVALID;

	if (m_pInstance)
		AppId = m_pInstance->GetAppID();
		
	return AppId;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CTashaUnitApp::CleanUp()
{
	WK_TRACE(_T("CleanUp\n"));

	// Instanzen-objekt zerstören
	WK_DELETE(m_pInstance);

	// Debugger schließen
	CloseDebugger();
	WK_DELETE(m_pDebug);

	return TRUE;
}
