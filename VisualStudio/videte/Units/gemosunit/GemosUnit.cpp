// GemosUnit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GemosUnit.h"

#include "MainFrm.h"

#include "IPCServerControlGemos.h"
#include "IPCGemosToVision.h"
#include "RS232Gemos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGemosUnitApp

BEGIN_MESSAGE_MAP(CGemosUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CGemosUnitApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_RESET, OnFileReset)
	ON_COMMAND(IDM_GEMOS, OnEnableConfirming)
	ON_UPDATE_COMMAND_UI(IDM_GEMOS, OnUpdateConfirming)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGemosUnitApp construction

CGemosUnitApp::CGemosUnitApp()
 : m_ComParameters(szSection)
{
	m_pCIPCServerControlGemos = NULL;
	m_pCIPCGemosToVision = NULL;
	m_bResetting = FALSE;
	m_iCom = 2;
	m_pRS232 = NULL;
	m_wHost  = (WORD)-1;
	m_bGEMOS = FALSE;
	m_nLastCameraInput = -1;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGemosUnitApp object

CGemosUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGemosUnitApp initialization

BOOL CGemosUnitApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER <0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	if (WK_ALONE(WK_APPNAME_GEMOSUNIT)==FALSE) 
	{
		return FALSE;
	}

	CString sTitle;
	sTitle = COemGuiApi::GetApplicationName(WAI_GEMOS_UNIT);
	m_pszAppName = _tcsdup(sTitle);    // human readable title

	InitDebugger(_T("GemosUnit.log"),WAI_GEMOS_UNIT);

	// Dongle vorhanden?
	CWK_Dongle dongle(WAI_GEMOS_UNIT); 
	if (dongle.IsExeOkay()==FALSE)
	{
		return FALSE;
	}

	WK_STAT_LOG(_T("Reset"),1,_T("ServerActive"));

	WK_TRACE(_T("main thread is %08lx\n"),GetCurrentThreadId());

	WNDCLASS  wndclass ;

	// register window class
	wndclass.style =         0;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor =       LoadCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = WK_APPNAME_GEMOSUNIT;

	AfxRegisterClass(&wndclass);

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);


	m_nCmdShow = SW_HIDE;

	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	Reset();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGemosUnitApp message handlers

// App command to run the dialog
void CGemosUnitApp::OnAppAbout()
{
	COemGuiApi::AboutDialog(AfxGetMainWnd());
}
/////////////////////////////////////////////////////////////////////////////
// CGemosUnitApp message handlers
int CGemosUnitApp::ExitInstance() 
{
	CleanUp();
	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));
	CloseDebugger();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::CleanUp()
{
	WK_TRACE(_T("clean up\n"));

	if (m_pRS232 && m_pRS232->IsOpen())
	{
		m_pRS232->Close();
		WK_DELETE(m_pRS232);
	}
	WK_DELETE(m_pCIPCGemosToVision);
	WK_DELETE(m_pCIPCServerControlGemos);
}
/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::Reset()
{
	if (!m_bResetting)
	{
		m_bResetting = TRUE;
		WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));
		WK_TRACE(_T("RESETTING...\n"));

		CleanUp();
		Init();

		WK_TRACE(_T("COMPLETE\n"));

		WK_STAT_LOG(_T("Reset"),1,_T("ServerActive"));
		m_bResetting = FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::Init()
{
	WK_TRACE(_T("initializing\n"));

	ReadSettings();

	m_pRS232 = new CRS232Gemos();
	m_pRS232->Open(m_iCom,m_ComParameters);
	
	m_pCIPCServerControlGemos = new	CIPCServerControlGemos();
}
/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::ReadSettings()
{
	CWK_Profile wkp;
	m_iCom = wkp.GetInt(szSection,_T("COM"),m_iCom);
	m_ComParameters.LoadFromRegistry(wkp);
}
/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::OnFileReset() 
{
	Reset();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGemosUnitApp::IsConnected()
{
	if (m_pCIPCGemosToVision && 
		(m_pCIPCGemosToVision->GetIPCState() == CIPC_STATE_CONNECTED)
		)
	{
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGemosUnitApp::IsDisconnected()
{
	if (m_pCIPCGemosToVision &&
		m_pCIPCGemosToVision->GetIsMarkedForDelete())
	{
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::Connect(WORD wLine, WORD wInput, WORD wState)
{
	m_wHost	 = wLine % 1000;

	CSecID hostID(SECID_GROUP_HOST,m_wHost);
	CWK_Profile wkp;
	CHostArray ha;
	CHost* pHost;
	ha.Load(wkp);

	pHost = ha.GetHost(hostID);

	if (pHost)
	{
		CIPCServerControlClientSide::StartVision();

		CString sShmName;

		sShmName.Format(_T("%s%08lx"),szSection,GetTickCount());

		m_pCIPCGemosToVision = new CIPCGemosToVision(sShmName);


		CConnectionRecord c;
		CSecID camID(SECID_GROUP_OUTPUT, wInput);
//		CSecID camID(SECID_NO_ID);

		c.SetDestinationHost(LOCAL_LOOP_BACK);
		c.SetSourceHost(pHost->GetName());
		c.SetCamID(camID);
		c.SetInputShm(sShmName);

		if (CIPCServerControlClientSide::FetchAlarmConnection(c))
		{
			int i=10;
			while(!IsConnected() && (i-->0))
			{
				Sleep(50);
			}
			if (IsConnected())
			{
				DoConfirmLine(CAM_LINE, 33, 0x16); // Melder 33 dient der Verbindungssteuerung
				WK_TRACE(_T("Connection success switching to full screen\n"));
				m_pCIPCGemosToVision->DoRequestSetValue(SECID_NO_ID, _T("fullscreen"),_T("on"),TRUE);
			}
			else
			{
				WK_TRACE(_T("Connection success still not connect state\n"));
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("FetchAlarmConnection failed\n"));
			WriteCommand(wLine, wInput, 0x19);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("no host found for line %d\n"),wLine);
		WriteCommand(wLine, wInput, 0x19);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::Disconnect()
{
	// Vor dem Disconnect die letzte aktive kamera auf AUS
	if (m_nLastCameraInput != -1)
		DoConfirmLine(CAM_LINE, (WORD)m_nLastCameraInput, 0x0a); 
	m_nLastCameraInput = -1;

	WK_TRACE(_T("disconnecting\n"));

	DoConfirmLine(CAM_LINE, 33, 0x14); // Melder 33 dient der Verbindungssteuerung
	
	WK_DELETE(m_pCIPCGemosToVision);
}
/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::DoConfirmLine(WORD wLine, WORD wInput, WORD wState)
{
	if (m_wHost != -1)
		WriteCommand(wLine+m_wHost,wInput,wState);
}
/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::WriteCommand(WORD wLine, WORD wInput, WORD wState, LPCSTR szText /*= NULL*/)
{
	CString sDaten;

	sDaten.Format(_T("%04x%02x%02x"),wLine,wInput,wState);
	if (szText)
	{
		sDaten += szText;
	}
	
	if (m_pRS232 && m_pRS232->IsOpen())
	{
		WK_TRACE(_T("send command <%s>\n"),sDaten);
		m_pRS232->SendCommand(sDaten);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGemosUnitApp::OnIdle(LONG lCount) 
{
	if (IsDisconnected())
	{
		Disconnect();
	}
	
	return CWinApp::OnIdle(lCount);
}

/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::OnDispatchCommand(WORD wLine, WORD wInput, WORD wState, const CString &sText)
{
	if (m_bGEMOS || IsConnected())
	{
		// new connection ?
		if ((wLine % 1000) == (m_wHost))
		{
			switch ((WORD)(wLine / 1000))
			{
				case ALARM_LINE/1000:	// Alarm
					OnDispatchAlarmCommand(wLine, wInput, wState);
					break;
				case RELAY_LINE/1000:	// Relais
					OnDispatchRelaisCommand(wLine, wInput, wState);
					break;
				case CAM_LINE/1000:	// Camera
					OnDispatchCameraCommand(wLine, wInput, wState);
					break;
				default:
					WK_TRACE(_T("Unknown Line (%d)\n"), wLine); 
			}
		}
		else
		{
			if (!m_bGEMOS)
			{
				// it's a new connection
				Disconnect();
				// sleep a while for safety
				Sleep(200);
				// now connect to the new line and so on			
				Connect(wLine,wInput,wState);
				if (IsConnected())
				{
					// Die Kamera bei der Alarmconnection hat die falsche Outputgruppe
					// deshalb wird hier nach dem Verbindungsaufbau auf die Kamera geschaltet.
					OnDispatchCommand(wLine, wInput, wState, sText);
				}
			}
		}
	}
	else
	{
		if (wState == 0x0b)
		{
			Connect(wLine,wInput,wState);
			if (IsConnected())
			{
				// Die Kamera bei der Alarmconnection hat die falsche Outputgruppe
				// deshalb wird hier nach dem Verbindungsaufbau auf die Kamera geschaltet.
				OnDispatchCommand(wLine, wInput, wState, sText);
			}
		}
		else
		{
			WK_TRACE(_T("ignoring command Line=%d, Input=%02x, State=%02x\n"),wLine,wInput,wState);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::OnDispatchAlarmCommand(WORD wLine, WORD wInput, WORD wState)
{
	// it's the same connection
//	CString sValue;
//	sValue.Format("%d",wInput);
	if (wState == 0x09)
	{
		WK_TRACE(_T("	->ALARM %d on Line %d ON\n"), wInput, wLine);
	}
	else if (wState == 0x08)
	{
		WK_TRACE(_T("	->ALARM %d on Line %d OFF\n"), wInput, wLine);
	}
	else if (wState == 0x14)
	{
		WK_TRACE(_T("	->ALARM %d on Line %d DEACTIVATED\n"), wInput, wLine);
	}
	else if (wState == 0x16)
	{
		WK_TRACE(_T("	->ALARM %d on Line %d ACTIVATED\n"), wInput, wLine);
	}
	else
	{
		WK_TRACE_ERROR(_T("	->ALARM %d on Line %d invalid state %x\n"),wInput, wLine, wState);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::OnDispatchCameraCommand(WORD wLine, WORD wInput, WORD wState)
{
	// it's the same connection
	CString sValue;
	sValue.Format(_T("%d"),wInput);
	if (wState == 0x0b)
	{
		WK_TRACE(_T("	->CAMERA %d on Line %d ON\n"), wInput, wLine);
		m_nLastCameraInput = wInput;
		if (!m_bGEMOS)
			m_pCIPCGemosToVision->DoRequestSetValue(SECID_NO_ID,_T("cam"),sValue,TRUE);
	}
	else if (wState == 0x0a)
	{
		WK_TRACE(_T("	->CAMERA %d on Line %d OFF\n"), wInput, wLine);
		m_nLastCameraInput = -1;
		if (!m_bGEMOS)
			m_pCIPCGemosToVision->DoRequestSetValue(SECID_NO_ID,_T("cam"),sValue,FALSE);
	}
	else if ((wState == 0x14) && (wInput == 33))	// Input 33 dient der Verbindungssteuerung
	{
		WK_TRACE(_T("	->DISCONNECT Line %d\n"), wLine);
		if (!m_bGEMOS)
			Disconnect();
	}
	else if (wState == 0x14)
	{
		m_nLastCameraInput = -1;
		WK_TRACE(_T("	->CAMERA %d on Line %d DEACTIVATED\n"), wInput, wLine);
	}
	else if (wState == 0x16)
	{
		WK_TRACE(_T("	->CAMERA %d on Line %d ACTIVATED\n"), wInput, wLine);
	}
	else if (wState == 0x19)
	{
		WK_TRACE(_T("	->CAMERA %d on Line %d FAILURE\n"), wInput, wLine);
	}
	else
	{
		WK_TRACE_ERROR(_T("	->CAMERA %d on Line %d invalid state %x\n"),wInput, wLine, wState);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::OnDispatchRelaisCommand(WORD wLine, WORD wInput, WORD wState)
{
	CString sValue;
	sValue.Format(_T("%d"),wInput);
	if (wState == 0x0b)
	{
		WK_TRACE(_T("	->RELAIS %d on Line %d ON\n"), wInput, wLine);
		if (!m_bGEMOS)
			m_pCIPCGemosToVision->DoRequestSetValue(SECID_NO_ID,_T("relay"),sValue,TRUE);
	}
	else if (wState == 0x0a)
	{
		WK_TRACE(_T("	->RELAIS %d on Line %d OFF\n"), wInput, wLine);
		if (!m_bGEMOS)
			m_pCIPCGemosToVision->DoRequestSetValue(SECID_NO_ID,_T("relay"),sValue,FALSE);
	}
	else if (wState == 0x14)
	{
		WK_TRACE(_T("	->RELAIS %d on Line %d DEACTIVATED\n"), wInput, wLine);
	}
	else if (wState == 0x16)
	{
		WK_TRACE(_T("	->RELAIS %d on Line %d ACTIVATED\n"), wInput, wLine);
	}
	else if (wState == 0x19)
	{
		WK_TRACE(_T("	->RELAIS %d on Line %d FAILURE %d\n"), wInput, wLine);
	}
	else
	{
		WK_TRACE_ERROR(_T("	->RELAIS %d on Line %d invalid state %x\n"),wInput, wLine, wState);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::OnEnableConfirming() 
{
	m_bGEMOS = !m_bGEMOS;	
}

/////////////////////////////////////////////////////////////////////////////
void CGemosUnitApp::OnUpdateConfirming(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bGEMOS);	
}
