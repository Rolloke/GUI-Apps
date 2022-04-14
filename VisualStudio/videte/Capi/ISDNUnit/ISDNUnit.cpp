/* GlobalReplace: GetRufnummerZahl --> FilterDigits */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ISDNUnit.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/ISDNUnit.cpp $
// CHECKIN:		$Date: 20.01.06 12:11 $
// VERSION:		$Revision: 223 $
// LAST CHANGE:	$Modtime: 20.01.06 10:59 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "resource.h"
#include "wk.h"
#include "wk_names.h"

#include "ISDNUnit.h"
#include "IsdnUnitWnd.h"
#include "CIPCServerControlISDN.h"
#include "CapiThread.h"

#include "IsdnUnitDlg.h"
#include "AboutBox.h"
#include "WK_Dongle.h"
#include "PTBox.h"	// for ReadDebugOptions
#include "OemGui/OemGuiApi.h"

#include "Host.h"
#include "User.h"
#include "Permission.h"

#include "CapiQueueRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
static TCHAR BASED_CODE szInitDummy[] = _T("InitDummy");	// used to force an initial update
static TCHAR BASED_CODE szOwnNumber[] = _T("OwnNumber");
static TCHAR BASED_CODE szNrBChannels[] = _T("NrBChannels");
static TCHAR BASED_CODE szSupportMSN[] = _T("SupportMSN");

#define ENABLEKNOCKKNOCKBOX	_T("EnableKnockKnockBox")
#define PROCESSORUSAGE		_T("ProcessorUsage")
#define CLOSEONECHANNEL		_T("CloseOneChannel")
/////////////////////////////////////////////////////////////////////////////
static void TestDongleCrypt();

/////////////////////////////////////////////////////////////////////////////
LPCTSTR NameOfEnum(ISDNProcessorUsage usage)
{
	switch (usage) 
	{
		case IPU_HIGH: return _T("IPU_HIGH"); break;
		case IPU_MEDIUM: return _T("IPU_MEDIUM"); break;
		case IPU_LOW: return _T("IPU_LOW"); break;
		default:
			return _T("IPU_INVALID");
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CISDNUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CISDNUnitApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// The one and only CISDNUnitApp object
CISDNUnitApp		theApp;
/////////////////////////////////////////////////////////////////////////////
// CISDNUnitApp construction
CISDNUnitApp::CISDNUnitApp()
{
	InitVideteLocaleLanguage();
	m_hDefNotify = NULL;
	m_iNumConfiguredBChannels = 1;
	m_pConnection = NULL;
	m_pControl		= NULL;
	m_bModuleClose	= FALSE;
	m_pCapiThread = NULL;

	m_pSettingsDlg = NULL;
	m_pAboutDlg		= NULL;

	m_processorUsage = IPU_MEDIUM;
	m_bEnableKnockBox=TRUE;
	m_iMinimumPrefix = 3;
	m_bCloseOneChannel = FALSE;

	// initial setting via ifdef, the rest via RunAsPTUnit
	m_bRunAsPTUnit = FALSE;
#ifdef RUN_AS_PTUNIT
	m_bRunAsPTUnit = TRUE;
#endif

	m_bDeviceFound = FALSE;
	m_bAllowSecondApp = FALSE;
	m_bIsSecondInstance = FALSE;

	m_wkAppId = WAI_INVALID;	// set in InitInstance
	// INIT m_sProfileSection
	// INIT m_sServerControlShm

	m_bTraceSentCommands = FALSE;
	m_bTraceReceivedCommands = FALSE;
	m_bTracePipes = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CISDNUnitApp::~CISDNUnitApp()
{
	WK_DELETE(m_pAboutDlg);
	// WK_DELETE(m_pSettingsDlg);	// OOPS seems already to be done
	// OOPS already all gone?
}
/////////////////////////////////////////////////////////////////////////////
static void AlreadyRunningBox(WK_ApplicationId id, const CString &sMsg)
{
	CString sFullMsg;
	sFullMsg.Format(_T("Already running\n[application %s]\n[%s]"), NameOfEnum(id),sMsg);
	::MessageBox(HWND_DESKTOP,sFullMsg,_T("Error"),MB_ICONEXCLAMATION);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNUnitApp::InitInstance()
{
/*
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
*/
	SetRegistryKey(WK_RegistryKey);

	if (InitAppMainWndAndDebugger() == FALSE)
	{
		return FALSE;
	}

	if (InitCapiDll() == FALSE)
	{
		return FALSE;
	}

	// can not fail, because checks above
	WK_ALONE(GetAppnameFromId(m_wkAppId));
#if _MFC_VER >= 0x0710
	SetLanguageParameters(0, 0);
#endif

	// dongle check via applicationId
	CWK_Dongle dongle(m_wkAppId);	// already sends RunTimeError 
	if (dongle.IsExeOkay()==FALSE) 
	{
		return FALSE;
	}

#if 0
	TestDongleCrypt();
#endif
	switch(dongle.GetProductCode())
	{
		case IPC_DTS_RECEIVER:
		case IPC_DTS:
		case IPC_DTS_IP_RECEIVER:
		{
			CWK_Profile wkp;
			wkp.WriteString(_T("Version"), _T("Number"), COemGuiApi::GetSoftwareVersion());
		}break;
	}

	ReadSettings();

	ReadDebugOptions();
	// CIPC ServerControl anlegen und starten
	m_pControl = new CIPCServerControlISDN(m_sServerControlShm, CIPC_MASTER);
	m_pControl->SetConnectRetryTime(50);
	m_pControl->StartThread(TRUE);	// multi threaded

	// NEW 140397 seperate thread, the real owner of the
	// connection is the CapiThread

	CWK_Profile wkp;
	int iUsage = wkp.GetInt(GetProfileSection(), PROCESSORUSAGE, 2);
	SetProcessorUsage((ISDNProcessorUsage)iUsage);

	BOOL bEnableKnock = wkp.GetInt(GetProfileSection(), ENABLEKNOCKKNOCKBOX,1);
	EnableKnockBox(bEnableKnock);
	m_bCloseOneChannel =  wkp.GetInt(GetProfileSection(), CLOSEONECHANNEL, m_bCloseOneChannel);


	// InitCapiThread may be fail, if USB device is not connected
	// do not close application, init will be tried again at device change
	// to avoid
	InitCapiThread();

	if (RegisterDeviceDetect(AfxGetMainWnd()->m_hWnd, NULL) == FALSE)
	{
		WK_TRACE_ERROR(_T("RegisterDeviceDetect FAILED\n"));
	}

#if 0
	WK_TRACE(_T("~~~Sizes\n"));

	WK_TRACE(_T("~~~ CConnectionRecord %d\n"),sizeof(CConnectionRecord));
	WK_TRACE(_T("~~~ CIPCFetchResult %d\n"),sizeof(CIPCFetchResult));
	WK_TRACE(_T("~~~ CWK_RunTimeError %d\n"),sizeof(CWK_RunTimeError));
	WK_TRACE(_T("~~~ CHost %d\n"),sizeof(CHost));
	WK_TRACE(_T("~~~ CUser %d\n"),sizeof(CUser));
	WK_TRACE(_T("~~~ CPermission %d\n"),sizeof(CPermission));

	WK_TRACE(_T("~~~ End Of Sizes\n"));
#endif

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CISDNUnitApp::ExitInstance() 
{
	WK_DELETE(m_pControl);

	StopCapiThread();

	UnloadDll();	// in capi4.h

	CWK_Profile wkp;
	wkp.DeleteEntry(m_sProfileSection, szConnectedRemoteNumber);

	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));

	CloseDebugger();

	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNUnitApp::OnIdle(LONG lCount) 
{
	//	OOPS OLD if ( m_HardwareState != HARDWARE_ERROR ) 

	BOOL bCanClose = TRUE;

	if ( m_pConnection && m_pConnection->IsValid() )
	{
		// Wenn das Modul geschlossen werden soll,
		// nur wenn keine Verbindung mehr offen
		if (m_bModuleClose)
		{
			if (m_pConnection->GetConnectState() != CONNECT_CLOSED)
			{
				bCanClose = FALSE;
			}
		}

		CString sRemoteHost = m_pConnection->GetRemoteHostName();
		if (m_pConnection->IsPTConnection())
		{
			sRemoteHost = _T("remote PT");	// fallback defaul;
			// only the the box name for alarm connection
			// else use the local callers name
			if (m_pConnection->GetPTBox()->GetBoxName().GetLength()) 
			{
				sRemoteHost = m_pConnection->GetPTBox()->GetBoxName();
			}
		}
	}
	else 
	{
		// else not a valid connection
	}

	if (m_bModuleClose) 
	{
		if ( bCanClose
			|| GetCurrentTime() >= m_dwModuleCloseTime+5000 )
		{
			// TEST GetMainWnd()->PostMessage(WM_CLOSE);
			if (m_pMainWnd)
			{
				m_pMainWnd->DestroyWindow();	// OOPS in OnIdle?
				Sleep(0);
			}
		}
	}

	return CWinApp::OnIdle(lCount);
}
void CISDNUnitApp::UpdateAboutBox()
{
	// force update of info dialog, limited by timedMessage
	CAboutBox *pAbout = m_pAboutDlg;
	if (pAbout && pAbout->m_hWnd
			&& IsWindow(pAbout->m_hWnd)
			&& pAbout->IsWindowVisible()
			) 
	{
		CTime ct = CTime::GetCurrentTime();
		CString sText;

		pAbout->SetDlgItemText(IDC_TIME, ct.Format(_T("%H:%M:%S")));

		pAbout->DoUpdateConnectState(
			m_pConnection->GetConnectState(),
			m_pConnection->GetRemoteHostName(),
			m_pConnection->GetRemoteNumber(),
			CISDNConnection::ms_bSupportsMSN
			);

		sText.Format(_T("[%d/%d]"), m_pConnection->GetNumBChannels(), GetNumConfiguredBChannels());
		pAbout->SetDlgItemText(IDC_NUM_CHANNELS, sText);

		if (CCapiQueueRecord::gm_avgCompression.GetActualValues())
		{
			CString sValue;
			sValue.Format(_T("%.1f"), CCapiQueueRecord::gm_avgCompression.GetAverage());
			pAbout->SetDlgItemText(IDC_COMPRESSIONRATE, sValue);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNUnitApp::CloseAllIsdnConnections(BOOL bWaitForResponse) 
{
	BOOL bReturn = TRUE;
	if (m_pConnection)
	{
		bReturn = m_pConnection->CrossThreadCloseConnection(bWaitForResponse);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNUnitApp::CloseModule() 
{
	if (!m_bModuleClose) 
	{
		m_bModuleClose = TRUE;
		m_dwModuleCloseTime = GetCurrentTime();
		CloseAllIsdnConnections(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CISDNUnitApp::ShowAboutDlg() 
{
	if (m_pAboutDlg) 
	{
		m_pAboutDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pAboutDlg = new CAboutBox(TRUE, m_pConnection, NULL);
		m_pAboutDlg->Create(m_pAboutDlg->IDD, NULL);
	}

}
/////////////////////////////////////////////////////////////////////////////
void CISDNUnitApp::ShowSettingsDlg() 
{
	if (m_pSettingsDlg==NULL) 
	{
		m_pSettingsDlg = new CISDNUnitDlg(m_pConnection);
		if (!m_pSettingsDlg->Create(m_pSettingsDlg->GetIDD(), m_pMainWnd))
		{
			// OOPS
			WK_TRACE_ERROR(_T("CISDNConnection::InitConnection() CISDNUnitDlg NOT CREATED\n"));
		}
	}

	if (m_pSettingsDlg)
	{
		m_pSettingsDlg->ShowWindow(SW_SHOWNORMAL);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNUnitApp::InitAppMainWndAndDebugger() 
{
	CString sLogName,sSecondLogName;
	CString sSecondProfileSection;
	CString sSecondServerControl;
	WK_ApplicationId appId2;
	WNDCLASS  wndclass;

	// here is the switch between ISDNUnit and PTUnit
	if (RunAsPTUnit())
	{
		m_wkAppId = WAI_PRESENCE_UNIT;
		appId2 = WAI_PRESENCE_UNIT_2;
		wndclass.hIcon =         LoadIcon(IDR_MAINFRAME_PT);
		m_sProfileSection = _T("PTUnit");	// modified if second ISDNUnit is running
		m_sServerControlShm = WK_SMServerControlPT;
		sLogName=_T("PTUnit.log");
		//
		sSecondLogName = _T("PTUnit2.log");
		sSecondProfileSection = _T("PTUnit2");
		sSecondServerControl = WK_SMServerControlPT2;
		// NOT YET other conditions
		if (CPTBox::m_bDoTraceFrameSize)
		{
			SET_WK_STAT_TICK_COUNT(GetTickCount());
		}
	}
	else
	{
		m_wkAppId= WAI_ISDN_UNIT;
		appId2 = WAI_ISDN_UNIT_2;
		wndclass.hIcon =  LoadIcon(IDR_MAINFRAME);
		m_sProfileSection = _T("ISDNUnit");	// modified if second ISDNUnit is running
		m_sServerControlShm = WK_SMServerControlIsdn;
		sLogName=_T("ISDNUnit.log");
		//
		sSecondLogName = _T("ISDNUnit2.log");
		sSecondProfileSection = _T("ISDNUnit2");
		sSecondServerControl = WK_SMServerControlIsdn2;
	}

	// now we can check fo the second instance
	// at this point we know the id 
	// CAVEAT no WK_TRACE before the InitDebugger();
	if (WK_IS_RUNNING(GetAppnameFromId(m_wkAppId)) )
	{
		// first instance is running
		// now see if the second instance is allowed
		CWK_Dongle SecondDongle(appId2);	// already sends RunTimeError 
		m_bAllowSecondApp = SecondDongle.IsExeOkay();
		// NOT YET read activation as set in SystemVerwaltung
		// SecurityLauncher/Modules
		if (m_bAllowSecondApp)
		{
			// first running, second is allowed
			if (WK_IS_RUNNING(GetAppnameFromId(appId2)) )
			{
				// first and second running
				AlreadyRunningBox(appId2,_T("Already have two instances!"));
				return FALSE;
			}
			else
			{
				// first running, second allowed but not running
				// adjust the variables
				m_bIsSecondInstance = TRUE;
				m_wkAppId = appId2;
				sLogName = sSecondLogName;
				m_sProfileSection = sSecondProfileSection;
				m_sServerControlShm = sSecondServerControl;
				// clear reserved caller
				ClearReservedCallingID();
			}
		}
		else
		{
			// first running, second not allowed
			AlreadyRunningBox(m_wkAppId,_T("Already have one instance!"));
			return FALSE;
		}
	}
	else
	{
		// first instance not running, move on
		// clear reserved caller
		ClearReservedCallingID();
		// First instance must know if second allowed
		CWK_Dongle dongle(m_wkAppId);
		if (RunAsPTUnit())
		{
			m_bAllowSecondApp = dongle.RunPTUnit2();
		}
		else
		{
			m_bAllowSecondApp = dongle.RunISDNUnit2();
		}
	}

	// register application window class
    wndclass.style			= 0;
    wndclass.lpfnWndProc	= DefWindowProc;
    wndclass.cbClsExtra		= 0;
    wndclass.cbWndExtra		= 0;
    wndclass.hInstance		= m_hInstance;
    wndclass.hCursor		= LoadCursor(IDC_ARROW);
    wndclass.hbrBackground	= (HBRUSH) (COLOR_WINDOW + 1) ;
    wndclass.lpszMenuName	= 0L;

	// apply settings depending from current application ID
	CWK_RunTimeError::SetDefaultID(m_wkAppId);
	wndclass.lpszClassName	= GetAppnameFromId(m_wkAppId);
	CString sTitle = COemGuiApi::GetApplicationName(m_wkAppId);
	if (IsSecondInstance())
	{
		sTitle +=_T("_2");
	}

	m_pszAppName = _tcsdup(sTitle);    // human readable title
	AfxRegisterClass(&wndclass);	// HERE do the register, all fields are set now

	CIsdnUnitWnd* pWindow = new CIsdnUnitWnd();
	m_pMainWnd = pWindow;

	InitDebugger( sLogName, m_wkAppId );

	CWK_Profile wkp;
	CString sSection = GetProfileSection() + _T("\\Debug");
	BOOL bTraceMS = (BOOL)wkp.GetInt(sSection, _T("TraceMilliSeconds"), 0);
	wkp.WriteInt(sSection, _T("TraceMilliSeconds"), bTraceMS);
	SET_WK_STAT_TICK_COUNT(bTraceMS);

	WK_PRINT_VERSION(4, 3, 0, 36, _T("17/03/99,13:36\0"), // @AUTO_UPDATE
		0);
	WK_STAT_LOG(_T("Reset"),0,_T("Active"));
	WK_STAT_LOG(_T("Reset"),1,_T("Active"));
	WK_STAT_LOG(_T("Reset"),0,_T("NrBChannels"));
	WK_STAT_LOG(_T("Reset"),0,_T("SendData"));
	WK_STAT_LOG(_T("Reset"),0,_T("ReceivedData"));
	WK_STAT_LOG(_T("Reset"),0,_T("Bitrate"));
	WK_STAT_LOG(_T("Reset"),0,_T("BitrateKB"));

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNUnitApp::InitCapiDll()
{
	BOOL bDllOkay = LoadDll();	// in capi4.h
	if (bDllOkay==FALSE) 
	{
		CWK_RunTimeError runTimeError(
			REL_CANNOT_RUN, RTE_CONFIGURATION,
			_T("failed to load capi2032")	// NOT YET locallized
			);
		runTimeError.Send();
	}
	return bDllOkay;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNUnitApp::InitCapiThread()
{
	// Create CapiThread only once
	if (m_pCapiThread == NULL)
	{
		m_pCapiThread = new CCapiThread();
		m_pCapiThread->CreateThread();	// activate the thread
		m_pCapiThread->m_bAutoDelete=FALSE;
	}

	int iInitCount = 10*20;	// 10 seconds
	while (   (m_bDeviceFound == FALSE)
		   && iInitCount)
	{
		Sleep(100);
		m_csConnection.Lock();
		// is the pointer set meanwhile?
		// CapiThread does call SetConncetion to do that
		// CAVEAT: m_pConnection == 0xFFFFFFFF means error
		if (m_pConnection)
		{
			if (m_pConnection == (CISDNConnection*)0xFFFFFFFF)	// error, skip loop
			{
				// set iInitCount to 1, will be decreased once again at end of loop
				iInitCount = 1;
				// set pointer to NULL for safety checks
				m_pConnection = NULL;
			}
			else	// ok
			{
				m_bDeviceFound = TRUE;
			}
		}
		m_csConnection.Unlock();
		iInitCount--;
	}
	if (m_bDeviceFound	)
	{
		// initial setup!
		CString sNumber = m_sOwnNumber;
		int iChannels = GetNumConfiguredBChannels();
		m_sOwnNumber = szInitDummy;	// force a change
		SetupChanged(sNumber,iChannels);
	}
	else
	{
		WK_TRACE(_T("FAILED to init CapiThread, no device found\n"));
		StopCapiThread();
	}
	m_pMainWnd->PostMessage(ISDNUNITWND_UPDATE_ICON, (WPARAM)m_bDeviceFound, NULL);
	return m_bDeviceFound;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CISDNUnitApp::StopCapiThread()
{
	BOOL bOK = FALSE;
	if (m_pCapiThread)	// can be null if InitInstance failed
	{
		m_pCapiThread->m_bTerminate=TRUE;
		// OOPS gf Why Sleep() when WaitForSingleObject afterwards?
		Sleep(50);

		// wait for thread end/state
		DWORD dwTimeoutSeconds = 10;
		DWORD dwThreadEnd = WaitForSingleObject(m_pCapiThread->m_hThread, dwTimeoutSeconds*1000);
		if (dwThreadEnd == WAIT_OBJECT_0)
		{
			bOK = TRUE;
		}
		else if (dwThreadEnd==WAIT_TIMEOUT)
		{
			WK_TRACE(_T("CapiThread did not terminate within %lu seconds!\n"), dwTimeoutSeconds);
		}
		else
		{
			WK_TRACE_ERROR(_T("CapiThread terminate FAILED, dwThreadEnd = %d ?\n"), dwThreadEnd);
		}
		WK_DELETE(m_pCapiThread);
		m_bDeviceFound = FALSE;
		if (m_pMainWnd)
		{
			m_pMainWnd->PostMessage(ISDNUNITWND_UPDATE_ICON, (WPARAM)m_bDeviceFound, NULL);
		}
	}
	return bOK;
}
/////////////////////////////////////////////////////////////////////////////
void CISDNUnitApp::ReadSettings()
{
	CWK_Profile wkp;
	CString sTmp(LISTEN_ALL);
	CWK_Profile::Encode(sTmp);
#ifdef _UNICODE
	m_sOwnNumber = wkp.GetString(GetProfileSection(), szOwnNumber, sTmp, TRUE);	// OOPS strange default and no check !?
#else
	m_sOwnNumber = wkp.GetString(GetProfileSection(), szOwnNumber, sTmp);	// OOPS strange default and no check !?
#endif
	CWK_Profile::Decode(m_sOwnNumber);
	m_sOwnNumber.TrimRight();

	if (RunAsPTUnit()==FALSE) 
	{
		if (   m_sOwnNumber.IsEmpty()
			&& GetApplicationId()==WAI_ISDN_UNIT)
		{
			m_sOwnNumber = sTmp;
		}
		m_iNumConfiguredBChannels = 1; 
#ifdef _DTS
		CWK_Dongle dongle;
		if (dongle.GetProductCode() == IPC_DTS)
		{
			m_iNumConfiguredBChannels = 2;
		}
#endif
		m_iNumConfiguredBChannels = wkp.GetInt(GetProfileSection(), 
											   szNrBChannels, 
											   m_iNumConfiguredBChannels);
	} 
	else 
	{
		m_iNumConfiguredBChannels = 1;
	}
	// range checks in SetupChangend

	m_sOwnHostName = wkp.GetString(_T("Hosts"),_T("OwnID"),_T("UNKNOWN"));
	CISDNConnection::ms_bSupportsMSN = wkp.GetInt(GetProfileSection(), szSupportMSN, 0);
	m_sPrefix = wkp.GetString(GetProfileSection(),_T("Prefix"),_T(""));
	m_iMinimumPrefix = wkp.GetInt(GetProfileSection(),_T("MinimumPrefix"),m_iMinimumPrefix);

	CString sSection = GetProfileSection() + _T("\\Debug");
	m_bTraceSentCommands = wkp.GetInt(sSection, _T("TraceSentCommands"), m_bTraceSentCommands);
	m_bTraceReceivedCommands = wkp.GetInt(sSection, _T("TraceReceivedCommands"), m_bTraceReceivedCommands);
	m_bTracePipes = wkp.GetInt(sSection, _T("TracePipes"), m_bTracePipes);

	wkp.WriteInt(sSection, _T("TraceSentCommands"), m_bTraceSentCommands);
	wkp.WriteInt(sSection, _T("TraceReceivedCommands"), m_bTraceReceivedCommands);
	wkp.WriteInt(sSection, _T("TracePipes"), m_bTracePipes);

	// NOT check for invalid values!
}
/////////////////////////////////////////////////////////////////////////////
/*@MD
Called from SetupChanged, assumes valid values.
*/
void CISDNUnitApp::SaveSettings()
{
	// NOT YET avoid uncoded number in log
	WK_TRACE(_T("SaveSettings %s[%d]\n"),m_sOwnNumber,m_iNumConfiguredBChannels);
	// assume valid values (checked in SetupChanged)
	CString sTmp(m_sOwnNumber);
	CWK_Profile::Encode(sTmp);
	CWK_Profile wkp;
#ifdef _UNICODE
	wkp.WriteString(GetProfileSection(), szOwnNumber, sTmp, TRUE);
#else
	wkp.WriteString(GetProfileSection(), szOwnNumber, sTmp);
#endif	
	wkp.WriteInt(GetProfileSection(), szNrBChannels, m_iNumConfiguredBChannels );
	wkp.WriteString(GetProfileSection(),_T("Prefix"),m_sPrefix);
	wkp.WriteInt(GetProfileSection(),_T("MinimumPrefix"),m_iMinimumPrefix);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNUnitApp::SetupChanged(const CString& sOwnNr,
								   int iNumChannels)
{
	// special hack to recognize the _T("InitDummy") SetupChanged
	CString sOldNumber = m_sOwnNumber;
	if (iNumChannels<0)
	{
		WK_TRACE_ERROR(_T("Invalid %d,at least one BChannel required\n"), iNumChannels);
		iNumChannels=1;
	}
	else if (iNumChannels>2)
	{
		WK_TRACE_ERROR(_T("Invaild %d,can't handle more than 2 BChannels\n"),iNumChannels);
		iNumChannels=2;
	}

	// pre check any changes 
	if (   sOwnNr != m_sOwnNumber	// new number 
		|| iNumChannels != GetNumConfiguredBChannels())
	{
		WK_TRACE(_T("SetupChanged Nr '%s' Channels %d\n"), 
					(LPCTSTR)sOwnNr, iNumChannels
					);
		// NOT YET check exiting state

		if ( (sOwnNr == LISTEN_ALL) || (sOwnNr == LISTEN_REJECTALL) )
		{
			m_sOwnNumber = sOwnNr;
		}
		else
		{
			m_sOwnNumber = CISDNConnection::FilterDigits( sOwnNr );
		}

		m_iNumConfiguredBChannels = iNumChannels;

		if (sOldNumber!=szInitDummy)
		{
			SaveSettings();
			if (m_pConnection)
			{
				m_pConnection->CrossThreadCloseConnection(TRUE);
				m_pConnection->CrossThreadResetCapi();
			}
		}

		if (m_pConnection)
		{
			m_pConnection->DoListen(sOwnNr);
		}
	}
	else
	{
		// no changes
	}

}
/////////////////////////////////////////////////////////////////////////////
/*@MD special thread function, the CapiThread will call this function
App will wait until this functions is called.
*/
void CISDNUnitApp::SetConnection(CISDNConnection *pConnection)
{
	m_csConnection.Lock();
	if (m_pConnection && pConnection) 
	{
		WK_TRACE_ERROR(_T("Internal error CISDNUnitApp::SetConnection\n"));
		Sleep(200);
	}
	else
	{
		// set the pointer
		m_pConnection = pConnection;
	}
	m_csConnection.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CISDNUnitApp::SetProcessorUsage(ISDNProcessorUsage usage)
{
	switch (usage) 
	{
		case IPU_HIGH:
		case IPU_MEDIUM:
		case IPU_LOW:
			m_processorUsage = usage;
				break;
		default:
			m_processorUsage =IPU_MEDIUM;
	}
	CWK_Profile wkp;
	wkp.WriteInt(GetProfileSection(), PROCESSORUSAGE, m_processorUsage);
	WK_TRACE(_T("%s %s\n"), PROCESSORUSAGE, NameOfEnum(m_processorUsage));
}
/////////////////////////////////////////////////////////////////////////////
void CISDNUnitApp::EnableKnockBox(BOOL bDoIt)
{
	m_bEnableKnockBox = bDoIt;
	CWK_Profile wkp;
	wkp.WriteInt(GetProfileSection(), ENABLEKNOCKKNOCKBOX, m_bEnableKnockBox);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNUnitApp::ReadDebugOptions()
{
	CString sFilename;
	if (RunAsPTUnit()==FALSE) 
	{
		sFilename = _T("ISDNUnit.cfg");
	}
	else
	{
		sFilename = _T("PTUnit.cfg");
	}
	CString sDebugFile = _T("c:\\")+sFilename;
	
	// check for C:\ServerDebug first, then in current directory
	BOOL bFoundFile = m_debugOptions.ReadFromFile(sDebugFile);
	if (bFoundFile==FALSE)
	{
		sDebugFile = sFilename;
		bFoundFile = m_debugOptions.ReadFromFile(sDebugFile);
	}
	if (bFoundFile)
	{
		WK_TRACE(_T("ConfigFile %s found\n"),(LPCTSTR)sDebugFile);
	}
	// CAVEAT the last filename is used to save advanced setting

	CPTBox::m_bDoHexDumpSend= m_debugOptions.GetValue(_T("DoHexDumpSend"),0);
	WK_TRACE(_T("DoHexDumpSend %d\n"),CPTBox::m_bDoHexDumpSend);
	CPTBox::m_bDoHexDumpReceive= m_debugOptions.GetValue(_T("DoHexDumpReceive"),0);
	WK_TRACE(_T("DoHexDumpReceive %d\n"),CPTBox::m_bDoHexDumpReceive);
	CPTBox::m_bDisableFrameGrabbing = m_debugOptions.GetValue(_T("DisableFrameGrabbing"),0);
	WK_TRACE(_T("DisableFrameGrabbing %d\n"),CPTBox::m_bDisableFrameGrabbing);
	CPTBox::m_bDoTraceFrameSize= m_debugOptions.GetValue(_T("DoTraceFrameSize"),0);
	WK_TRACE(_T("DoTraceFrameSize=%d\n"),CPTBox::m_bDoTraceFrameSize);
	CPTBox::m_bDisableEEPWrite = m_debugOptions.GetValue(_T("DisableEEPWrite"),0);
	WK_TRACE(_T("DisableEEPWrite =%d\n"),CPTBox::m_bDisableEEPWrite );
	// CAVEAT 1 as default !
	CPTBox::m_bLowAsMid= m_debugOptions.GetValue(_T("LowAsMid"),1);
	WK_TRACE(_T("LowAsMid=%d\n"),CPTBox::m_bLowAsMid);
}
/////////////////////////////////////////////////////////////////////////////
void CISDNUnitApp::WriteDebugOptions()
{
	m_debugOptions.SetValue(_T("DoHexDumpSend"),CPTBox::m_bDoHexDumpSend,_T("HexDump aller gesendeten Daten"));
	m_debugOptions.SetValue(_T("DoHexDumpReceive"),CPTBox::m_bDoHexDumpReceive,_T("HexDump aller empfangenen Daten"));
	m_debugOptions.SetValue(_T("DisableFrameGrabbing"),CPTBox::m_bDisableFrameGrabbing,_T("Gar keine Bilder, vereinfacht das Testen"));
	m_debugOptions.SetValue(_T("DoTraceFrameSize"),CPTBox::m_bDoTraceFrameSize);
	m_debugOptions.SetValue(_T("DisableEEPWrite"),CPTBox::m_bDisableEEPWrite);
	m_debugOptions.SetValue(_T("LowAsMid"),CPTBox::m_bLowAsMid);

	CString sFilename = m_debugOptions.GetFilename();
	
	// the last/active filename is still in m_debugOptions
	BOOL bOkay = m_debugOptions.WriteToFile(sFilename);
	if (bOkay)
	{
		WK_TRACE(_T("Wrote %s\n"),(LPCTSTR)sFilename);
	}
	else
	{
		WK_TRACE(_T("Failed to write %s\n"),(LPCTSTR)sFilename);
	}
}
/////////////////////////////////////////////////////////////////////////////
#if 0
static void TestDongleCrypt()
{
	BOOL bOkay=FALSE;
	char *data1 = _T("Dies ist ein Test.");
	DWORD dwLenCrypt1=0;
	char *crypt1 = CWK_Dongle::Crypt(data1,strlen(data1)+1,bOkay, dwLenCrypt1);
	DWORD dwLen2=0;
	char *ddata1 = CWK_Dongle::Decrypt(crypt1,dwLenCrypt1,bOkay,dwLen2);

	TRACE(_T("Test1 '%s' == '%s'\n"),data1,ddata1);

	char *pBigOne = new char[12345];
	for (int i=0;i<12345;i++) {
		pBigOne[i] = (BYTE)(i & 255);
	}
	DWORD dwBig=0;
	char *cryptBigOne = CWK_Dongle::Crypt(pBigOne,12345,bOkay,dwBig);
	DWORD dwBigBack = 0;
	char *pBigOneBack = CWK_Dongle::Decrypt(cryptBigOne,dwBig,bOkay,dwBigBack);

	BOOL bEqual = TRUE;
	for (int c=0;c<12345;c++) {
		bEqual &= (pBigOne[c]==pBigOneBack[c]);
	}

	TRACE(_T("BigOne %d --> %d --> %d, equal %d\n"),12345,dwBig,dwBigBack,bEqual);

	WK_DELETE_ARRAY(pBigOne);
	WK_DELETE_ARRAY(cryptBigOne);
	WK_DELETE_ARRAY(pBigOneBack);
	WK_DELETE_ARRAY(crypt1);
	WK_DELETE_ARRAY(ddata1);
}
#endif
/////////////////////////////////////////////////////////////////////////////
void CISDNUnitApp::ClearReservedCallingID()
{
	CWK_Profile wkp;
	wkp.DeleteEntry(GetProfileSection(), szConnectedRemoteNumber);
	wkp.DeleteEntry(GetProfileSection(), szChannelsConnected);
}
/////////////////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0710
int CISDNUnitApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
{
	// TODO! RKE: extract resource for multilang
	// m_hLangResourceDLL may be initialized in CWinApp::InitInstance()
	// Do not call InitInstance() of the base class
//	CLoadResourceDll ResourceDll;
//	m_hLangResourceDLL = ResourceDll.DetachResourceDllHandle();
//	if (m_hLangResourceDLL == NULL)
//	{
//		WK_TRACE(_T("Did not find any ResourceDLL\n"));
//		ASSERT(FALSE);
//		return FALSE;
//	}
//	AfxSetResourceHandle(m_hLangResourceDLL);

	if (uCodePage == 0)
	{
		uCodePage = _ttoi(COemGuiApi::GetCodePage());
	}
	else
	{
		COemGuiApi::ChangeLanguageResource();
		InitVideteLocaleLanguage();
	}
//  Set the CodePage for MBCS conversion, if necessary
	CWK_String::SetCodePage(uCodePage);
	CWK_Profile wkp;
	wkp.SetCodePage(CWK_String::GetCodePage());

//	if (dwCPbits == 0)
//	{
//		dwCPbits = COemGuiApi::GetCodePageBits();
//	}
//	if (dwCPbits)
//	{
//		SetFontFaceNamesFromCodePageBits(dwCPbits);
//	}
//	else
//	{
//		CSkinDialog::DoUseGlobalFonts(FALSE);
//	}
	return 0;
}
#endif
/////////////////////////////////////////////////////////////////////////////
/*
void CISDNUnitApp::CapiGetMessageError()
{
	TRACE(_T("OnCapiGetMessageError\n"));
	// Something got totally wrong, we should close those connection and reinitialise CAPI
	theApp.CapiGetMessageError();
}
*/
