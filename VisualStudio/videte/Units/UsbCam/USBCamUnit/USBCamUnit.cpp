/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: USBCamUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCamUnit/USBCamUnit.cpp $
// CHECKIN:		$Date: 14.03.03 14:53 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 13.03.03 11:08 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "USBCamUnit.h"
#include "USBCamUnitDlg.h"

#include "oemgui\oemguiapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUSBCamUnitApp

BEGIN_MESSAGE_MAP(CUSBCamUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CUSBCamUnitApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBCamUnitApp construction

CUSBCamUnitApp::CUSBCamUnitApp()
{
	m_pUSBCamUnitDlg= NULL;
	m_AppID         = WAI_INVALID;
	m_nInstance     = -1;
}

CUSBCamUnitApp::~CUSBCamUnitApp()
{
	WK_DELETE(m_pUSBCamUnitDlg);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUSBCamUnitApp object

CUSBCamUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUSBCamUnitApp initialization

BOOL CUSBCamUnitApp::InitInstance()
{
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	if (!WK_IS_RUNNING(WK_APP_NAME_USBCAMUNIT1))
	{
		m_nInstance = 0;
		m_AppID = MAKEAPPID(WAI_USBCAMUNIT_1);
	}
	else if (!WK_IS_RUNNING(WK_APP_NAME_USBCAMUNIT2))
	{
		m_nInstance = 1;
		m_AppID = MAKEAPPID(WAI_USBCAMUNIT_2);
	}
	else if (!WK_IS_RUNNING(WK_APP_NAME_USBCAMUNIT3))
	{
		m_nInstance = 2;
		m_AppID = MAKEAPPID(WAI_USBCAMUNIT_3);
	}
	else if (!WK_IS_RUNNING(WK_APP_NAME_USBCAMUNIT4))
	{
		m_nInstance = 3;
		m_AppID = MAKEAPPID(WAI_USBCAMUNIT_4);
	}
#ifdef _DEBUG
	CString sAppName = WK_APP_NAME_USBCAMUNIT1;
#else
	CString sAppName = GetAppnameFromId(m_AppID); 
#endif

	WK_ALONE(sAppName);
	CWK_RunTimeError::SetDefaultID(m_AppID);

	// Logfilename erzeugen
	CString sLogFile = sAppName+".log";

	// Debugger öffnen
	InitDebugger(sLogFile, m_AppID, NULL);

	
	WK_TRACE("Starting USB Cam Unit...");
	WK_PRINT_VERSION(7, 4, 16, 31, "$Date: 14.03.03 14:53 $", // @AUTO_UPDATE
		0);	// DO NOT TOUCH THIS LINE

	if (m_pszAppName)
	{
		free((void*)m_pszAppName);
	}
#ifdef _DEBUG
	m_pszAppName = _tcsdup(WK_APP_NAME_USBCAMUNIT1);
#else
	m_pszAppName = _tcsdup(COemGuiApi::GetApplicationName(m_AppID));
#endif

	m_pUSBCamUnitDlg = new CUSBCamUnitDlg();
	if (m_pUSBCamUnitDlg && m_pUSBCamUnitDlg->Create(IDD_USBCAMUNIT_DIALOG))
	{
		m_pMainWnd = m_pUSBCamUnitDlg;
		return TRUE;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CUSBCamUnitApp::ExitInstance() 
{
	WK_TRACE("ExitInstance\n");
	CloseDebugger();
	return CWinApp::ExitInstance();
}
