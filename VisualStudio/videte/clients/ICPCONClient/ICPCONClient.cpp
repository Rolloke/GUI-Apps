// RelaisSwitchBox.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ICPCONClient.h"
#include "ICPCONClientDlg.h"

#include "SwitchPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPCCONClient

BEGIN_MESSAGE_MAP(CIPCCONClient, CWinApp)
	//{{AFX_MSG_MAP(CIPCCONClient)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPCCONClient construction

CIPCCONClient::CIPCCONClient()
{
	m_pSwitchPanel = NULL;
	m_sRegistryLocation      = _T(REGISTRY_LOCATION);
	m_sRegistryLocationPorts = _T(REGISTRY_LOCATION_PORTS);
	m_bIsInMenuLoop = FALSE;
}
CIPCCONClient::~CIPCCONClient()
{
	WK_DELETE(m_pSwitchPanel);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIPCCONClient object

CIPCCONClient theApp;

/////////////////////////////////////////////////////////////////////////////
// CIPCCONClient initialization

BOOL CIPCCONClient::InitInstance()
{
	if (WK_ALONE(WK_APP_NAME_ICPCON_CLIENT) == FALSE)
	{
		return FALSE;
	}
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
	int nResponse;
	
	if (m_pszAppName)
	{
		free((void*)m_pszAppName);
	}
	m_pszAppName = _tcsdup(WK_APP_NAME_ICPCON_CLIENT);
	AFX_MODULE_STATE*pState = AfxGetModuleState();
	if (pState)
	{
		if (pState->m_lpszCurrentAppName)
		{
			pState->m_lpszCurrentAppName = m_pszAppName;
		}
	}

	CString str;
	str.Format("%s.log", m_pszAppName);
	InitDebugger(str, WAI_ICPCON_CLIENT);

#ifndef _DEBUG
	CWK_Dongle dongle;
	if (!dongle.RunICPCONClient())
	{
		WK_TRACE("No Dongle available\n");
		return FALSE;
	}
#endif

	if ((m_lpCmdLine[0] == '/') && (m_lpCmdLine[1] == 'c'))
	{
		CIPCCONClientDlg dlg;
		if (m_lpCmdLine[2] == 'u')
		{
			m_sRegistryLocation      = _T(REG_LOC_UNIT);
			m_sRegistryLocationPorts = _T(REG_LOC_PORTS_UNIT);
			dlg.m_bAllowInputs       = TRUE;
		}

		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();
	}
	else
	{
		m_pSwitchPanel  = new CSwitchPanel;
		if (m_pSwitchPanel && m_pSwitchPanel->Create(IDD_SWITCH_PANEL))
		{
			m_pMainWnd = m_pSwitchPanel;
			return TRUE;
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
LONG CIPCCONClient::ReadRegistryString(HKEY hKey, LPCTSTR strSection, CString &strValue)
{
	DWORD dwType, dwCount = 0;
	LONG lResult = RegQueryValueEx(hKey, strSection, NULL, &dwType, NULL, &dwCount);
	if ((lResult == ERROR_SUCCESS) && (dwType == REG_SZ))
	{
		lResult = RegQueryValueEx(hKey, strSection, NULL, &dwType, (LPBYTE)strValue.GetBufferSetLength(dwCount), &dwCount);
		strValue.ReleaseBuffer();
	}
	return lResult;
}
/////////////////////////////////////////////////////////////////////////////
int CIPCCONClient::ExitInstance() 
{
	CloseDebugger();

	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
