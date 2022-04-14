/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCameraUnit
// FILE:		$Workfile: USBCameraUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCameraUnit/USBCameraUnit.cpp $
// CHECKIN:		$Date: 6.12.04 12:09 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 3.12.04 10:31 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBCameraUnit.h"
#include "USBCameraUnitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUSBCameraUnitApp

BEGIN_MESSAGE_MAP(CUSBCameraUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CUSBCameraUnitApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBCameraUnitApp construction

CUSBCameraUnitApp::CUSBCameraUnitApp()
{
	m_AppID			= WAI_INVALID;
	m_nInstance		= -1;
	m_pDlg			= NULL;
	m_dwThreadID	= GetCurrentThreadId();
}

CUSBCameraUnitApp::~CUSBCameraUnitApp()
{
//	WK_DELETE(m_pDlg);
	m_pDlg = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUSBCameraUnitApp object

CUSBCameraUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUSBCameraUnitApp initialization

BOOL CUSBCameraUnitApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
#if _MFC_VER >= 0x0710
#else
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif // _AFXDLL
#endif // _MFC_VER

	if      (!WK_IS_RUNNING(GetAppnameFromId(WAI_USBCAMUNIT_1)))
	{
		m_nInstance = 1;
		m_AppID = WAI_USBCAMUNIT_1;
	}
	else if (!WK_IS_RUNNING(GetAppnameFromId(WAI_USBCAMUNIT_2)))
	{
		m_nInstance = 2;
		m_AppID = WAI_USBCAMUNIT_2;
	}
	else if (!WK_IS_RUNNING(GetAppnameFromId(WAI_USBCAMUNIT_3)))
	{
		m_nInstance = 3;
		m_AppID = WAI_USBCAMUNIT_3;
	}
	else if (!WK_IS_RUNNING(GetAppnameFromId(WAI_USBCAMUNIT_4)))
	{
		m_nInstance = 4;
		m_AppID = WAI_USBCAMUNIT_4;
	}
	else
	{
		return FALSE;
	}

	CString sAppName = GetAppnameFromId(m_AppID);
	WK_ALONE(sAppName);
	CWK_RunTimeError::SetDefaultID(m_AppID);

	// Logfilename erzeugen
	CString sLogFile = sAppName+_T(".log");

	// Debugger öffnen
	InitDebugger(sLogFile, m_AppID, NULL);

	WK_TRACE(_T("Starting USB Camera Unit..."));
	WK_PRINT_VERSION(7, 4, 16, 31, _T("$Date: 6.12.04 12:09 $"), // @AUTO_UPDATE
		0);	// DO NOT TOUCH THIS LINE

	free((void*)m_pszAppName);
	m_pszAppName = _tcsdup(COemGuiApi::GetApplicationName(m_AppID));
	AFX_MODULE_STATE*pState = AfxGetModuleState();
	if (pState)
	{
		if (pState->m_lpszCurrentAppName)
		{
			pState->m_lpszCurrentAppName = m_pszAppName;
		}
	}
	
	m_pDlg = new CUSBCameraUnitDlg();
	if (m_pDlg && m_pDlg->Create(IDD_USBCAMERAUNIT_DIALOG))
	{
		CString strPath(m_pszHelpFilePath);
		strPath.MakeUpper();
		if (strPath.Find(_T("C:\\DV\\")))
		{
			m_pDlg->m_bDTS = TRUE;
		}
		m_pMainWnd = m_pDlg;
		return TRUE;
	}
	return FALSE;
}

int CUSBCameraUnitApp::ExitInstance() 
{
	WK_TRACE(_T("ExitInstance\n"));
	CloseDebugger();
	return CWinApp::ExitInstance();
}
