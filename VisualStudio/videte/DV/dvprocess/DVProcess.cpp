// DVProcess.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DVProcess.h"

#include "MainFrm.h"
#include "DVProcessDoc.h"
#include "DVProcessView.h"

#include "CIPCServerControlProcess.h"
#include "IPCDatabaseProcess.h"
#include "CameraGroup.h"
#include "RelayGroup.h"
#include "SystemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static _TCHAR szTimer[] = _T("Timer%02d");
static _TCHAR szTimerOff[] = _T("00000000");
static _TCHAR szSection[] = _T("Process");
static _TCHAR szDays[8][3] = {_T("Mo"),_T("Di"),_T("Mi"),_T("Do"),_T("Fr"),_T("Sa"),_T("So"),_T("Mo")};
/////////////////////////////////////////////////////////////////////////////
CString SecTimeSpanStart2Client(const CString& time)
{
	CString r;

	r = StringOnlyCharsInSet(time,_T("0123456789"));

	return r;
}
/////////////////////////////////////////////////////////////////////////////
CString SecTimeSpanEnd2Client(const CString& time)
{
	CString r;

	r = StringOnlyCharsInSet(time,_T("0123456789"));
	if (r==_T("0000"))
	{
		r = _T("2359");
	}

	return r;
}
/////////////////////////////////////////////////////////////////////////////
CString Client2SecTimeSpanStart(const CString& day, const CString& time)
{
	CString r;
	CString a(day);

	a.MakeUpper();

	r = a + ':' + time.Mid(0,2) + ':' + time.Mid(2,2);

	return r;
}
/////////////////////////////////////////////////////////////////////////////
CString Client2SecTimeSpanEnd(const CString& day, const CString& time)
{
	CString r;
	CString minute(time.Mid(6,2));
	CString hour(time.Mid(4,2));
	CString a(day);

	a.MakeUpper();

	if (   minute==_T("59")
		&& hour==_T("23"))
	{
		minute = _T("00");
		hour = _T("00");
	}

	r = a + ':' + hour + ':' + minute;

	return r;
}
/////////////////////////////////////////////////////////////////////////////
void Client2SecTimeSpan(int iDay, const CString& sTime, CString& sStart, CString& sEnd)
{
	CString sDayStart = szDays[iDay];
	sDayStart.MakeUpper();
	CString sDayEnd = sDayStart;

	sStart	= sTime.Mid(0,2) + ':' + sTime.Mid(2,2);
	sEnd	= sTime.Mid(4,2) + ':' + sTime.Mid(6,2);

	// should be ok for times only...
	if (sEnd < sStart)
	{
		// CAVEAT: String array must have appropriate size!
		sDayEnd = szDays[iDay+1];
		sDayEnd.MakeUpper();
	}
	CString sHourEnd(sTime.Mid(4,2));
	CString sMinuteEnd(sTime.Mid(6,2));

	if (   sHourEnd==_T("23")
		&& sMinuteEnd==_T("59"))
	{
		sEnd	= _T("00:00");
	}

	sStart	= sDayStart + ':' + sStart;
	sEnd	= sDayEnd + ':' + sEnd;
}


/////////////////////////////////////////////////////////////////////////////
// CDVProcessApp
BEGIN_MESSAGE_MAP(CDVProcessApp, CWinApp)
	//{{AFX_MSG_MAP(CDVProcessApp)
	ON_COMMAND(ID_FILE_RESET, OnFileReset)
	ON_COMMAND(ID_DATE_CHECK, OnDateCheck)
	ON_COMMAND(ID_TEST_NEW_DAY, OnTestNewDay)
	ON_COMMAND(ID_TEST_NEW_WEEK, OnTestNewWeek)
	ON_UPDATE_COMMAND_UI(ID_PANE_STORAGE, OnUpdateStoragePane)
	ON_UPDATE_COMMAND_UI(ID_PANE_IC, OnUpdateICPane)
	ON_UPDATE_COMMAND_UI(ID_PANE_OC, OnUpdateOCPane)
	ON_UPDATE_COMMAND_UI(ID_PANE_AC, OnUpdateACPane)
	ON_UPDATE_COMMAND_UI(ID_PANE_IG, OnUpdateIGPane)
	ON_UPDATE_COMMAND_UI(ID_PANE_OG, OnUpdateOGPane)
	ON_UPDATE_COMMAND_UI(ID_PANE_AG, OnUpdateAGPane)
	ON_UPDATE_COMMAND_UI(ID_PANE_LED, OnUpdateLEDPane)
	ON_UPDATE_COMMAND_UI(ID_PANE_AUDIO, OnUpdateAudioPane)
	ON_COMMAND(ID_TEST_NEW_MINUTE, OnTestNewMinute)
	ON_COMMAND(ID_TEST_NEW_HOUR, OnTestNewHour)
	ON_COMMAND(ID_AUDIO_RESET, OnAudioReset)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVProcessApp construction

CDVProcessApp::CDVProcessApp()
{
	m_pCIPCServerControlProcess = NULL;
	m_pCIPCDatabaseProcess = NULL;
	m_bResetting = FALSE;
	m_bMustResetProcess = FALSE;
	m_bMustResetDatabase = FALSE;
	m_bMustInitDatabase = FALSE;
	m_bShutDown = FALSE;

	m_currentTime.GetLocalTime();

	m_sSuperPIN = _T("0000");
	m_sOperatorPIN = _T("1111");
	m_bDetectorIgnoreTimer = FALSE;

	m_eVOutModePort[0][0] = VOUT_ACTIVITY;
	m_eVOutModePort[1][0] = VOUT_ACTIVITY;
	m_eVOutModePort[0][1] = VOUT_CLIENT;
	m_eVOutModePort[1][1] = VOUT_CLIENT;

	m_dwFramesToRecord = 0;
	m_bAlarmOutput = FALSE;
	m_dwLastAlarmOutputTime = 0;
	m_bAlarmClosed = FALSE; //TRUE;
	m_bStoreLED = FALSE;

	m_nNrOfJacobs = 0;
	m_nNrOfAllowedJacobs = 0;
	m_nNrOfSavics = 0;
	m_nNrOfTashas = 0;
	m_nNrOfQs = 0;

	m_bErrorOccurred = FALSE;

	m_pDebugger = NULL;

}
/////////////////////////////////////////////////////////////////////////////
// The one and only CDVProcessApp object

CDVProcessApp theApp;
CWK_Timer  theTimer;
LARGE_INTEGER theStart;

CString GetTickCountEx(BOOL bStart)
{
	if (bStart)
	{
		theStart = theTimer.GetMicroTicks();
	}
	else
	{
		LARGE_INTEGER liDiff, liCurrent = theTimer.GetMicroTicks();
		liDiff.QuadPart = liCurrent.QuadPart - theStart.QuadPart;
		CString s;
		s.Format(_T("%d.%d"), liDiff.LowPart/1000, liDiff.LowPart%1000);
		theStart = liCurrent;
		return s;
	}
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::RegisterWindowClass()
{
    WNDCLASS  wndclass;

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
    wndclass.lpszClassName = WK_APP_NAME_SERVER;

    AfxRegisterClass(&wndclass);
}

void RSATest()
{
	DWORD dwSerial;
	DWORD dwDongle;
	CRSA  rsa(0,0);

	TRACE(_T("RSA %04hx,%04hx\n"),rsa.GetSerialHigh(),rsa.GetSerialLow());

	for (dwSerial=030600000; dwSerial<101299999;dwSerial++)
	{
		for (dwDongle=1;dwDongle<4;dwDongle++)
		{
			CString s;
			if (rsa.EncodeSerialDongle(dwSerial,dwDongle,s))
			{
				DWORD dwS,dwD;
				if (rsa.DecodeSerialDongle(s,dwS,dwD))
				{
					if (dwSerial == dwS
						&& dwDongle == dwD)
					{
						TRACE(_T("S=%d,D=%d --> <%s> --> S=%d,D=%d\n"),dwSerial,dwDongle,s,dwS,dwD);
					}
					else
					{
						TRACE(_T("Encoding Decoding mismatch\n"));
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDVProcessApp initialization

BOOL CDVProcessApp::InitInstance()
{
	GetTickCountEx(TRUE);
	if (WK_ALONE(WK_APP_NAME_SERVER)==FALSE) return FALSE;

	RegisterWindowClass();

	AfxEnableControlContainer();

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

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDVProcessDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CDVProcessView));
	AddDocTemplate(pDocTemplate);

	// do not touch
#ifndef _DEBUG
	if (m_nCmdShow!=0) // do not touch if already hidden
	{	
		m_nCmdShow = SW_HIDE;
	}
#endif

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->UpdateWindow();

	InitDebugger(_T("process.log"),WAI_SECURITY_SERVER);

	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	m_Settings.ReadSettings(wkp);
	

	UINT nNumbersOfTashas = 1;
	UINT nNumbersOfJaCobs = 1;
	UINT nNumbersOfSaVics = 1;
	UINT nNumbersOfQs = 1;

	CBoards boards;
	if (boards.IsValid())
	{
		nNumbersOfJaCobs = boards.GetNumbersOfJaCobs();
		nNumbersOfSaVics = boards.GetNumbersOfSaVics();
		nNumbersOfTashas = boards.GetNumbersOfTashas();
		nNumbersOfQs = boards.GetNumbersOfQs();
		if (nNumbersOfQs)
		{
			UINT nNrOfAlarmInputs = boards.GetAvailableQInputs();
			WK_TRACE(_T("nNrOfAlarmInputs=%d\n"),nNrOfAlarmInputs);
		}
	}
	else
	{	// in case of hardware detection failed
	    nNumbersOfJaCobs = wkp.GetInt(_T("DVStarter"), _T("IntegratedJaCobs"), -1);
		if (nNumbersOfJaCobs == -1)
		{
			nNumbersOfJaCobs = 0;
			wkp.WriteInt(_T("DVStarter"), _T("IntegratedJaCobs"), nNumbersOfJaCobs);
		}
		nNumbersOfSaVics = wkp.GetInt(_T("DVStarter"), _T("IntegratedSaVics"), -1);
		if (nNumbersOfSaVics == -1)
		{
			nNumbersOfSaVics = 0;
			wkp.WriteInt(_T("DVStarter"), _T("IntegratedSaVics"), nNumbersOfSaVics);
		}
		nNumbersOfTashas = wkp.GetInt(_T("DVStarter"), _T("IntegratedTashas"), -1);
		if (nNumbersOfTashas == -1)
		{
			nNumbersOfTashas = 0;
			wkp.WriteInt(_T("DVStarter"), _T("IntegratedTashas"), nNumbersOfTashas);
		}
		nNumbersOfTashas = wkp.GetInt(_T("DVStarter"), _T("IntegratedQs"), -1);
		if (nNumbersOfQs == -1)
		{
			nNumbersOfQs = 0;
			wkp.WriteInt(_T("DVStarter"), _T("IntegratedQs"), nNumbersOfQs);
		}
	}

	m_nNrOfAllowedJacobs = wkp.GetInt(_T("DVStarter\\Debug"), _T("AllowedJaCobs"), 4);
	UINT nAllowedSaVics = wkp.GetInt(_T("DVStarter\\Debug"), _T("AllowedSaVics"), 4);
	UINT nAllowedTashas = wkp.GetInt(_T("DVStarter\\Debug"), _T("AllowedTashas"), 4);
	UINT nAllowedQs = wkp.GetInt(_T("DVStarter\\Debug"), _T("AllowedQs"), 1);

	// Die maximale Anzahl der zu startenden Units evtl. begrenzen
	m_nNrOfJacobs = min(m_nNrOfAllowedJacobs, nNumbersOfJaCobs);
	m_nNrOfSavics = min(nAllowedSaVics,nNumbersOfSaVics);
	m_nNrOfTashas = min(nAllowedTashas,nNumbersOfTashas);
	m_nNrOfQs = min(nAllowedQs,nNumbersOfQs);


	//get number of available QUnit cameras from QUnit (default: 4, per dongle: 8 or 16)
	CWK_Profile prof;;
	int nNrOfQUnitAllowedCameras	= 4;
	nNrOfQUnitAllowedCameras = prof.GetInt(_T("QUnit\\EEProm"), _T("AllowedCameras"), nNrOfQUnitAllowedCameras);

	//save number of available QUnit cameras to let the process know
	if(m_nNrOfQs > 0)
	{
		prof.WriteInt(_T("DV\\Process"),_T("NrOfCameras"),nNrOfQUnitAllowedCameras);
	}

	int nCamerasRegistry = m_Settings.GetNrOfCameras();
	int nCamerasHardware = m_nNrOfJacobs*8+m_nNrOfSavics*4+m_nNrOfTashas*8+m_nNrOfQs*nNrOfQUnitAllowedCameras;


    if (nCamerasHardware<nCamerasRegistry)
	{
		WK_TRACE(_T("less hardware than registry cameras %d<%d\n"),nCamerasRegistry,nCamerasHardware);
	}
	else if (   (nCamerasHardware>nCamerasRegistry)
		     && (m_nNrOfJacobs+m_nNrOfSavics+m_nNrOfTashas+m_nNrOfQs>0))
	{
		WK_TRACE(_T("greater hardware than registry cameras %d>%d\n"),nCamerasRegistry,nCamerasHardware);
		if (nCamerasRegistry == 0)
		{
			m_Settings.SetNrOfCameras(nCamerasHardware);
			m_Settings.WriteSettings(wkp);
		}
	}
	else
	{
		WK_TRACE(_T("less or equal hardware than registry cameras %d==%d\n"),nCamerasRegistry,nCamerasHardware);
	}

	BOOL bDebugger = wkp.GetInt(szSection,_T("EnableDebugger"),FALSE);
	if (bDebugger)
	{
		m_pDebugger = new CWK_Debugger();
		WK_TRACE(_T("WK_DEBUGGER enabled\n"));
	}

	Reset();

	m_pCIPCServerControlProcess = new CIPCServerControlProcess();

	ExportHKLMSoftware();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::WriteSettings(BOOL bShutdown)
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));

	for (int i=0;i<NR_OF_CARDS;i++)
	{
		for (int j=0;j<NR_OF_VOUT;j++)
		{
			CString sKey;
			sKey.Format(_T("VOutPortMode_%d_%d"),i,j);
			wkp.WriteInt(szSection,sKey,m_eVOutModePort[i][j]);
		}
	}

	wkp.WriteInt(szSection,_T("EnableDebugger"),(m_pDebugger!=NULL));
	m_Settings.WriteSettings(wkp);

	m_Inputs.Save(wkp,bShutdown);
	m_Outputs.Save(wkp);
	m_Timers.Save(wkp);


	CString s = m_sSuperPIN;
	wkp.Encode(s);
#ifdef _UNICODE
	wkp.WriteString(szSection,_T("PIN"), s, TRUE);
#else
	wkp.WriteString(szSection,_T("PIN"), s);
#endif	
	s = m_sOperatorPIN;
	wkp.Encode(s);
#ifdef _UNICODE
	wkp.WriteString(szSection,_T("OPIN"), s, TRUE);
#else
	wkp.WriteString(szSection,_T("OPIN"), s);
#endif	

	wkp.WriteInt(szSection,CSD_DETECTOR_IGNORE_TIMER,m_bDetectorIgnoreTimer);

	for (i=0;i<m_sAlarmDialingNumbers.GetSize();i++)
	{
		CString sKey;
		sKey.Format(_T("_%d"),i);
		sKey = CSD_ALARM_DIALING_NUMBER + sKey;
		wkp.WriteString(szSection,sKey,m_sAlarmDialingNumbers.GetAt(i));
	}
	CWK_Profile wkp2;
	wkp2.WriteInt(_T("DV\\DVStarter"), _T("UsePiezo"), !m_bAlarmOutput);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::ReadSettings()
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));

	m_Settings.ReadSettings(wkp);

	for (int i=0;i<NR_OF_CARDS;i++)
	{
		for (int j=0;j<NR_OF_VOUT;j++)
		{
			CString sKey;
			sKey.Format(_T("VOutPortMode_%d_%d"),i,j);
			m_eVOutModePort[i][j] = (VOUT_MODE)wkp.GetInt(szSection,sKey,m_eVOutModePort[i][j]);
		}
	}

#ifdef _UNICODE
	CString s = wkp.GetString(szSection, _T("PIN"), _T(""), TRUE);
#else
	CString s = wkp.GetString(szSection, _T("PIN"), _T(""));
#endif	
	if (s.IsEmpty())
	{
		// first time write the PIN for DVStorage
		s = m_sSuperPIN;
		wkp.Encode(s);
#ifdef _UNICODE
		wkp.WriteString(szSection,_T("PIN"), s, TRUE);
#else
		wkp.WriteString(szSection,_T("PIN"), s);
#endif	
	}
	else
	{
		 wkp.Decode(s);
		 m_sSuperPIN = s;
		 TRACE(_T("%s\n"), m_sSuperPIN);
	}
#ifdef _UNICODE
	s = wkp.GetString(szSection, _T("OPIN"), _T(""), TRUE);
#else
	s = wkp.GetString(szSection, _T("OPIN"), _T(""));
#endif	
	if (s.IsEmpty())
	{
		s = m_sOperatorPIN;
		wkp.Encode(s);
#ifdef _UNICODE
		wkp.WriteString(szSection,_T("OPIN"), s, TRUE);
#else
		wkp.WriteString(szSection,_T("OPIN"), s);
#endif	
	}
	else
	{
		 wkp.Decode(s);
		 m_sOperatorPIN = s;
	}

	m_bDetectorIgnoreTimer = wkp.GetInt(szSection,CSD_DETECTOR_IGNORE_TIMER,m_bDetectorIgnoreTimer);

	m_Settings.ReadSettings(wkp);

	for (i=0;i<MAX_ALARM_DIALING_NUMBERS;i++)
	{
		CString sKey;
		sKey.Format(_T("_%d"),i);
		sKey = CSD_ALARM_DIALING_NUMBER + sKey;
		m_sAlarmDialingNumbers.SetAtGrow(i,wkp.GetString(szSection,sKey,_T("")));
	}
	CWK_Profile wkp2;
	m_bAlarmOutput = wkp2.GetInt(_T("DV\\DVStarter"),_T("UsePiezo"),TRUE) == FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::InitTimer(int iNum)
{
	CSecTimer* pTimer;
	CSystemTime st;
	CString s,e,d;
	CString sName;
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));

	// Change Old Format
	s = wkp.GetString(_T("Timer"),_T("MaxTimer"),_T("0"));
	int c = _ttoi(s);
	for (int j=1;j<=c;j++)
	{
		e.Format(_T("%d"),j);
		s = wkp.GetString(_T("Timer"),e,_T(""));
		if (!s.IsEmpty())
		{
			s.Replace(_T("23:59"),_T("00:00"));
			wkp.WriteString(_T("Timer"),e,s);
		}
	}

	st.GetLocalTime();
	m_Timers.Load(st.GetCTime(),wkp);

	if (m_Timers.GetSize() < iNum)
	{
		WORD is = (WORD)m_Timers.GetSize();
		WK_TRACE(_T("creating timers\n"));
		for (WORD i=is;i<iNum;i++)
		{
			pTimer = new CSecTimer();
			pTimer->SetID(CSecID(SECID_GROUP_TIMER,i));
			sName.Format(szTimer,i);
			pTimer->SetName(sName);
			for (int j=0;j<7;j++)
			{
				d = szDays[j];
				d.MakeUpper();
				s = d + _T(":00:00");
				e = d + _T(":00:00");
				pTimer->AddTimeSpan(CSecTimeSpan(st.GetCTime(),s,e));
			}
			m_Timers.Add(pTimer);
		}
		m_Timers.CalcTotalSpans(m_Timers);
		m_Timers.Save(wkp);
	}
	for (int i=0;i<m_Timers.GetSize();i++)
	{
		WK_TRACE(_T("%s %s\n"),m_Timers.GetAtFast(i)->GetName(),
			m_Timers.GetAtFast(i)->CreateRegistryString());
	}
}
LPCTSTR NameOfEnum(VOUT_MODE vm)
{
	switch (vm)
	{
	case VOUT_OFF: 
		return CSD_VOUT_OFF;
	case VOUT_ACTIVITY:
		return CSD_VOUT_ACTIVITY;
	case VOUT_CLIENT:
		return CSD_VOUT_CLIENT;
	case VOUT_SEQUENCE:
		return CSD_VOUT_SEQUENCE;
	case VOUT_PERMANENT:
		return CSD_VOUT_SEQUENCE;
	}
	return _T("");
}
VOUT_MODE StringToEnum(const CString& s)
{
	if (0 == s.CompareNoCase(CSD_VOUT_OFF))
	{
		return VOUT_OFF;
	}
	else if (0 == s.CompareNoCase(CSD_VOUT_ACTIVITY))
	{
		return VOUT_ACTIVITY;
	}
	else if (0 == s.CompareNoCase(CSD_VOUT_CLIENT))
	{
		return VOUT_CLIENT;
	}
	else if (0 == s.CompareNoCase(CSD_VOUT_SEQUENCE))
	{
		return VOUT_SEQUENCE;
	}
	else if (0 == s.CompareNoCase(CSD_VOUT_SEQUENCE))
	{
		return VOUT_PERMANENT;
	}
	return VOUT_OFF;
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnRequestGetValue(CSecID id, const CString& sKey, DWORD dwParam, CIPC* pCipc)
{
	if (0 == sKey.CompareNoCase(CSD_IS_DV))
	{
		pCipc->DoConfirmGetValue(id,sKey,CSD_ON,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_DETECTOR_IGNORE_TIMER))
	{
		pCipc->DoConfirmGetValue(id,sKey,m_bDetectorIgnoreTimer ? CSD_ON : CSD_OFF ,dwParam);
	}
	else if (   0 == sKey.CompareNoCase(CSD_PIN)
		     || 0 == sKey.CompareNoCase(CSD_PIN_SUPERVISOR))
	{
		pCipc->DoConfirmGetValue(id,sKey,m_sSuperPIN,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_PIN_OPERATOR))
	{
		pCipc->DoConfirmGetValue(id,sKey,m_sOperatorPIN,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_PORT0))
	{
		// Video Out Mode Port 0
		// User   Karte 1 VideoOut 1, Standard Manual
		// Intern Card1   CSD_PORT1
		pCipc->DoConfirmGetValue(id,sKey,NameOfEnum(m_eVOutModePort[0][1]),dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_PORT1))
	{
		// Video Out Mode Port 1
		// User   Karte 1 VideoOut 2, Standard Activity
		// Intern Card1   CSD_PORT0
		pCipc->DoConfirmGetValue(id,sKey,NameOfEnum(m_eVOutModePort[0][0]),dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_PORT2))
	{
		// Video Out Mode Port 2
		// User   Karte 2 VideoOut 1, Standard Manual
		// Intern Card2   CSD_PORT1
		pCipc->DoConfirmGetValue(id,sKey,NameOfEnum(m_eVOutModePort[1][1]),dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_PORT3))
	{
		// Video Out Mode Port 3
		// User   Karte 2 VideoOut 1, Standard Activity
		// Intern Card2   CSD_PORT0
		pCipc->DoConfirmGetValue(id,sKey,NameOfEnum(m_eVOutModePort[1][0]),dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_VOUT_COUNT))
	{
		// Nr Of Video Outs over all Boards
		CString s;
		int nNrOfQVideoOuts = 0;
		if(m_nNrOfQs > 0)
		{
			nNrOfQVideoOuts = 1; //default analog output is 1
			//Nr of outputs depends on the Q Unit extension card, if installed
			CWK_Profile wkp;
			nNrOfQVideoOuts = wkp.GetInt(_T("QUnit\\General"),_T("NrOfVideoOuts"), nNrOfQVideoOuts);
			if(nNrOfQVideoOuts > 2)
			{	
				//Q Unit only can have 2 video outs in DTS
				nNrOfQVideoOuts = 2;
			}
		}
		s.Format(_T("%d"),m_nNrOfJacobs*2+m_nNrOfSavics*2+m_nNrOfTashas*2+nNrOfQVideoOuts);
		pCipc->DoConfirmGetValue(id,sKey,s,dwParam);
	}
	else if (0 == _tcsncmp(sKey,szTimer,5))
	{
		CString sValue = szTimerOff;
		CSecID idTimer(SECID_GROUP_TIMER,id.GetSubID());
		// request get timer value!
		m_Timers.Lock();
		const CSecTimer* pTimer = m_Timers.GetTimerByID(idTimer);
		if (pTimer)
		{
			const CSecTimeSpanArray& spans = pTimer->GetTotalSpans();
			if (spans.GetSize()>0)
			{
				int i = -1;
				if (1 == _stscanf(sKey,szTimer,&i))
				{
					for (int j=0;j<spans.GetSize();j++)
					{
						CSecTimeSpan* pSpan = spans.GetAtFast(j);

						if (0 == pSpan->GetStartString().Left(2).CompareNoCase(szDays[i]))
						{
							sValue =   SecTimeSpanStart2Client(pSpan->GetStartString())
									 + SecTimeSpanEnd2Client(pSpan->GetEndString());
							break;
						}
					}
				}
			}
		}
		m_Timers.Unlock();
        pCipc->DoConfirmGetValue(id,sKey,sValue,dwParam);
	}
	else if (0 == _tcsncmp(sKey,CSD_ALARM_DIALING_NUMBER,_tcslen(CSD_ALARM_DIALING_NUMBER)))
	{
		// get alarmnr i
		int i = 0;
		CString sFormat;
		sFormat = CString(CSD_ALARM_DIALING_NUMBER) + _T("_%d");
		if (1==_stscanf(sKey,sFormat,&i))
		{
			if (i<MAX_ALARM_DIALING_NUMBERS)
			{
				TRACE(_T("Confirm %s, %s\n"),sKey,m_sAlarmDialingNumbers[i]);
				pCipc->DoConfirmGetValue(id,sKey,m_sAlarmDialingNumbers[i],dwParam);
			}
		}
	}
	else if (0 == sKey.CompareNoCase(CSD_ALARM_OUTPUT_ON_OFF_STATE))
	{
		pCipc->DoConfirmGetValue(id,sKey,m_bAlarmOutput ? CSD_ON : CSD_OFF,0);
	}
	
	//show system info e.g. serial nr, nero serial...
	else if (0 == sKey.CompareNoCase(CSD_SYSTEM_INFO))
	{
		CSystemInfo SysInfo;
		CString sInfo = SysInfo.GetSystemInformations();
		pCipc->DoConfirmGetValue(id,sKey,sInfo,0);
	}
	else if (0 == sKey.CompareNoCase(CSD_RESOLUTION))
	{
		Resolution resolution = m_Settings.GetResolution();
		pCipc->DoConfirmGetValue(id,sKey,NameOfEnum(resolution),0);
	}
	else if (0 == sKey.CompareNoCase(CSD_AUDIO_IN))
	{
		pCipc->DoConfirmGetValue(id,sKey,m_Settings.GetAudioIn() ? CSD_ON : CSD_OFF,0);
	}
	else if (0 == sKey.CompareNoCase(CSD_AUDIO_OUT))
	{
		pCipc->DoConfirmGetValue(id,sKey,m_Settings.GetAudioOut() ? CSD_ON : CSD_OFF,0);
	}
	else if (0==sKey.CompareNoCase(CSD_AUDIO_CAM))
	{
		// passende kamera für Audio
		COutput* pCamera = m_Outputs.GetOutputByUnitID(m_Settings.GetAudioCam());
		CString sValue;

		if (pCamera)
		{
			sValue.Format(_T("%d"),pCamera->GetClientID().GetSubID()+1);
		}
		else
		{
			sValue = CSD_OFF;
		}
		pCipc->DoConfirmGetValue(id,sKey,sValue,0);
	}
	else if (0==sKey.CompareNoCase(CSD_AUDIO_REC))
	{
		CString sValue;
		
		switch (m_Settings.GetAudioRecordingBits())
		{
		case 0:
			sValue = CSD_AUDIO_RECORDING_OFF;
			break;
		case AUDIO_RECORDING_LEVEL:
			sValue = CSD_AUDIO_RECORDING_L;
			break;
		case AUDIO_RECORDING_ACTIVITY:
			sValue = CSD_AUDIO_RECORDING_A;
			break;
		case AUDIO_RECORDING_EXTERN:
			sValue = CSD_AUDIO_RECORDING_E;
			break;
		case AUDIO_RECORDING_LEVEL|AUDIO_RECORDING_ACTIVITY:
			sValue = CSD_AUDIO_RECORDING_LA;
			break;
		case AUDIO_RECORDING_LEVEL|AUDIO_RECORDING_EXTERN:
			sValue = CSD_AUDIO_RECORDING_LE;
			break;
		case AUDIO_RECORDING_ACTIVITY|AUDIO_RECORDING_EXTERN:
			sValue = CSD_AUDIO_RECORDING_AE;
			break;
		case AUDIO_RECORDING_LEVEL|AUDIO_RECORDING_ACTIVITY|AUDIO_RECORDING_EXTERN:
			sValue = CSD_AUDIO_RECORDING_LAE;
			break;
		}
		pCipc->DoConfirmGetValue(id,sKey,sValue,0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnRequestSetValue(CSecID id, const CString& sKey, const CString& sValue, DWORD dwParam, CIPC* pCipc)
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));

	if (   0 == sKey.CompareNoCase(CSD_PIN)
		|| 0 == sKey.CompareNoCase(CSD_PIN_SUPERVISOR))
	{
		m_sSuperPIN = sValue;
		CString s = m_sSuperPIN;
		wkp.Encode(s);
#ifdef _UNICODE
		wkp.WriteString(szSection,_T("PIN"), s, TRUE);
#else
		wkp.WriteString(szSection,_T("PIN"), s);
#endif
		pCipc->DoConfirmSetValue(id,sKey,m_sSuperPIN,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_DETECTOR_IGNORE_TIMER))
	{
		m_bDetectorIgnoreTimer = (sValue == CSD_ON);
		wkp.WriteInt(szSection,CSD_DETECTOR_IGNORE_TIMER,m_bDetectorIgnoreTimer);
		m_Inputs.CheckActive();
		pCipc->DoConfirmSetValue(id,sKey,sValue,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_PIN_OPERATOR))
	{
		m_sOperatorPIN = sValue;
		CString s = m_sOperatorPIN;
		wkp.Encode(s);
#ifdef _UNICODE
		wkp.WriteString(szSection,_T("OPIN"), s, TRUE);
#else
		wkp.WriteString(szSection,_T("OPIN"), s);
#endif
		pCipc->DoConfirmSetValue(id,sKey,m_sOperatorPIN,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_PORT0))
	{
		// Video Out Mode Port 0
		// User   Karte 1 VideoOut 1, Standard Manual
		// Intern Card1   CSD_PORT1
		m_eVOutModePort[0][1] = StringToEnum(sValue);
		wkp.WriteInt(szSection,_T("VOutPortMode_0_1"),m_eVOutModePort[0][1]);
		m_InputClients.DoConfirmSetValue(id,sKey,sValue,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_PORT1))
	{
		// Video Out Mode Port 1
		// User   Karte 1 VideoOut 2, Standard Activity
		// Intern Card1   CSD_PORT0
		m_eVOutModePort[0][0] = StringToEnum(sValue);
		wkp.WriteInt(szSection,_T("VOutPortMode_0_0"),m_eVOutModePort[0][0]);
		m_InputClients.DoConfirmSetValue(id,sKey,sValue,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_PORT2))
	{
		// Video Out Mode Port 2
		// User   Karte 2 VideoOut 1, Standard Manual
		// Intern Card2   CSD_PORT1
		m_eVOutModePort[1][1] = StringToEnum(sValue);
		wkp.WriteInt(szSection,_T("VOutPortMode_1_1"),m_eVOutModePort[1][1]);
		m_InputClients.DoConfirmSetValue(id,sKey,sValue,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_PORT3))
	{
		// Video Out Mode Port 3
		// User   Karte 2 VideoOut 1, Standard Activity
		// Intern Card2   CSD_PORT0
		m_eVOutModePort[1][0] = StringToEnum(sValue);
		wkp.WriteInt(szSection,_T("VOutPortMode_1_0"),m_eVOutModePort[1][0]);
		m_InputClients.DoConfirmSetValue(id,sKey,sValue,dwParam);
	}
	else if (0 == _tcsncmp(sKey,szTimer,5))
	{
		// request set timer value!
		CString sNewValue;
		// request get timer value!
		CSecID idTimer(SECID_GROUP_TIMER,id.GetSubID());
		m_Timers.Lock();
		CSecTimer* pTimer = (CSecTimer*)m_Timers.GetTimerByID(idTimer);
		if (pTimer)
		{
			int index = -1;
			if (1 == _stscanf(sKey,szTimer,&index))
			{
				CSecTimeSpanArray timeSpanSave;
				CSecTimeSpan* pSpan;
				CString day;
				const CSecTimeSpanArray& spans = pTimer->GetTotalSpans();
				day = szDays[index];
				for (int i=0;i<spans.GetSize();i++)
				{
					pSpan = spans.GetAtFast(i);
					if (0 != pSpan->GetStartString().Left(2).CompareNoCase(day))
					{
						timeSpanSave.Add(new CSecTimeSpan(*pSpan));
					}
				}
				pTimer->RemoveAllTimeSpans();

				for (i=0;i<timeSpanSave.GetSize();i++)
				{
					pSpan = timeSpanSave.GetAtFast(i);
					if (0 != pSpan->GetStartString().Left(2).CompareNoCase(day))
					{
						pTimer->AddTimeSpan(*timeSpanSave.GetAtFast(i));
					}
				}
				if (sValue != szTimerOff)
				{
					CSystemTime st;
					CString s,e,d;
					d = day;
					d.MakeUpper();
//					s = Client2SecTimeSpanStart(d,sValue);
//					e = Client2SecTimeSpanEnd(d,sValue);
// gf change for timespan over midnight
// Actual the timer is implicit for one day only
// new timer depends on SpanEnd relative to SpanStart
// Client2SecTimeSpanNew(d, sValue, &s, &e)
					Client2SecTimeSpan(index, sValue, s, e);
					st.GetLocalTime();
					CSecTimeSpan newSpan(st.GetCTime(),s,e);
					pTimer->AddTimeSpan(newSpan);
				}

				pTimer->CalcTotalSpans(m_Timers);
				timeSpanSave.DeleteAll();
				m_Timers.Save(wkp);

				if (WK_IS_WINDOW(m_pMainWnd))
				{
					m_pMainWnd->PostMessage(WM_COMMAND,ID_TEST_NEW_MINUTE);
				}
			}
		}
		m_Timers.Unlock();
		m_Inputs.CheckActive();
        pCipc->DoConfirmSetValue(id,sKey,sValue,dwParam);
	}
	else if (0 == _tcsncmp(sKey,CSD_ALARM_DIALING_NUMBER,_tcslen(CSD_ALARM_DIALING_NUMBER)))
	{
		// set alarmnr i
		int i = 0;
		CString sFormat;
		sFormat = CString(CSD_ALARM_DIALING_NUMBER) + _T("_%d");
		if (1==_stscanf(sKey,sFormat,&i))
		{
			if (i<MAX_ALARM_DIALING_NUMBERS)
			{
				WK_TRACE(_T("set %d. AlarmDialingNumber to %s\n"),i+1,sValue);
				m_sAlarmDialingNumbers[i] = sValue;
				wkp.WriteString(szSection,sKey,sValue);
				m_CallThread.DialingNumbersChanged();
			}
		}
		pCipc->DoConfirmSetValue(id,sKey,sValue,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_ALARM_OUTPUT_ON_OFF_STATE))
	{
		if (sValue == CSD_ON)
		{
			m_bAlarmOutput = TRUE;
		}
		else if (sValue == CSD_OFF)
		{
			m_bAlarmOutput = FALSE;
		}

		CWK_Profile wkp2;
		wkp2.WriteInt(_T("DV\\DVStarter"), _T("UsePiezo"), !m_bAlarmOutput);

		if (WK_IS_WINDOW(m_pMainWnd))
		{
			HWND hWndStarter = ((CMainFrame*)m_pMainWnd)->GetStarterWindowHandle();
			if (hWndStarter && IsWindow(hWndStarter))
			{
				PostMessage(hWndStarter,WM_PIEZO_USE_CHANGED,!m_bAlarmOutput,0);
			}
		}
		WK_TRACE(_T("Alarm Output Relay set to %s\n"),m_bAlarmOutput?_T("in use"):_T("not in use"));
		pCipc->DoConfirmSetValue(id,sKey,sValue,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_EXPANSION_CODE))
	{
		HandleExpansionCode(pCipc,sValue);
	}
	else if (0 == sKey.CompareNoCase(CSD_RESOLUTION))
	{
		Resolution resolution = (Resolution)(_ttoi(sValue));
		m_Settings.SetResolution(resolution);
		m_Outputs.OnResolutionChanged(resolution);
		pCipc->DoConfirmSetValue(id,sKey,sValue,dwParam);
	}
	else if (0 == sKey.CompareNoCase(CSD_AUDIO_IN))
	{
		m_Settings.SetAudioIn(0==sValue.CompareNoCase(CSD_ON));
		m_Settings.WriteSettings(wkp);
		if (WK_IS_WINDOW(m_pMainWnd))
		{
			m_pMainWnd->PostMessage(WM_COMMAND,ID_AUDIO_RESET);
		}
		CAudioClients& clients = GetAudioClients();
		for (int ci=0;ci<clients.GetSize();ci++) 
		{
			clients.GetAtFast(ci)->GetCIPC()->DoConfirmSetValue(id,sKey,m_Settings.GetAudioIn() ? CSD_ON : CSD_OFF,0);
		}
	}
	else if (0 == sKey.CompareNoCase(CSD_AUDIO_OUT))
	{
		m_Settings.SetAudioOut(0==sValue.CompareNoCase(CSD_ON));
		m_Settings.WriteSettings(wkp);
		if (WK_IS_WINDOW(m_pMainWnd))
		{
			m_pMainWnd->PostMessage(WM_COMMAND,ID_AUDIO_RESET);
		}
		CAudioClients& clients = GetAudioClients();
		for (int ci=0;ci<clients.GetSize();ci++) 
		{
			clients.GetAtFast(ci)->GetCIPC()->DoConfirmSetValue(id,sKey,m_Settings.GetAudioOut() ? CSD_ON : CSD_OFF,0);
		}
	}
	else if (0==sKey.CompareNoCase(CSD_AUDIO_CAM))
	{
		OnRequestSetAudioCam(id,sKey,sValue,dwParam,pCipc,wkp);
	}
	else if (0==sKey.CompareNoCase(CSD_AUDIO_REC))
	{
		if (WK_IS_WINDOW(m_pMainWnd))
		{
			m_pMainWnd->PostMessage(WM_COMMAND,ID_AUDIO_RESET);
		}
		OnRequestSetAudioRec(id,sKey,sValue,dwParam,pCipc,wkp);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnRequestSetAudioCam(CSecID id, const CString& sKey, const CString& sValue, DWORD dwParam, CIPC* pCipc, CWK_Profile& wkp)
{
	// passende kamera für Audio
	BOOL bConfirm = FALSE;
	BOOL bReset = TRUE;
	DWORD dwCamera = 0;
	CSecID idOld = m_Settings.GetAudioCam();

	WK_TRACE(_T("new audio cam requested %s\n"),sValue);
	if (sValue == CSD_OFF)
	{
		m_Settings.SetAudioCam(SECID_NO_ID);
		bConfirm = TRUE;
	}
	else if (1==_stscanf(sValue,_T("%d"),&dwCamera))
	{
		CCamera* pCamera = (CCamera*)m_Outputs.GetOutputByClientID(CSecID(SECID_GROUP_OUTPUT,(WORD)(dwCamera-1)));
		if (pCamera)
		{
			m_Settings.SetAudioCam(pCamera->GetUnitID());
			bConfirm = TRUE;

			// alte camera bereits aktiv, kommen
			// wir ohne reset aus
			if (idOld != SECID_NO_ID)
			{
				CCamera* pOldCamera = (CCamera*)m_Outputs.GetOutputByUnitID(idOld);

				if (pOldCamera)
				{
					CInput* pInput = pOldCamera->GetAudioDetector();
					CMedia* pMedia = pOldCamera->GetMedia();
					
					if (   pInput
						&& pMedia)
					{
						CIPCAudioServer* pCIPCAudioServer = GetAudioGroup();
						if (pCIPCAudioServer)
						{
							// stop the current recording
							// by camera change
							// new alarm will cause new recording
							// stop only if no client is active
							pCIPCAudioServer->SetRecording(AUDIO_RECORDING_ACTIVITY|
														   AUDIO_RECORDING_EXTERN|
														   AUDIO_RECORDING_LEVEL,
														   FALSE);
							
							pOldCamera->SetAudioInput(NULL);
							pOldCamera->SetMedia(NULL);

							pInput->SetCamera(pCamera);
							pMedia->SetCamera(pCamera);

							pCamera->SetAudioInput(pInput);
							pCamera->SetMedia(pMedia);

							DWORD dwFlags = 0;

							if (pInput->IsActive() && m_Settings.GetAudioRecordingLevel())
							{
								dwFlags |= AUDIO_RECORDING_LEVEL;
							}
							if (pCamera->GetMotionDetector()->IsActive() && m_Settings.GetAudioRecordingActivity())
							{
								dwFlags |= AUDIO_RECORDING_ACTIVITY;
							}
							if (pCamera->GetExternDetector()->IsActive() && m_Settings.GetAudioRecordingExtern())
							{
								dwFlags |= AUDIO_RECORDING_EXTERN;
							}
							pCIPCAudioServer->SetRecording(dwFlags,TRUE);
						}
						
						bReset = FALSE;
					}
				}
			}
		}
	}
	if (bConfirm)
	{
		m_Settings.WriteSettings(wkp);
		CAudioClients& clients = GetAudioClients();
		for (int ci=0;ci<clients.GetSize();ci++) 
		{
			clients.GetAtFast(ci)->GetCIPC()->DoConfirmSetValue(id, sKey, sValue, 0);
		}

		if (   bReset
			&& WK_IS_WINDOW(m_pMainWnd))
		{
			m_pMainWnd->PostMessage(WM_COMMAND, ID_AUDIO_RESET);
		}
		else
		{
			clients.UpdateAllClients();
		}
		WK_TRACE(_T("new audio cam confirmed %s\n"),sValue);
	}
	else
	{
		pCipc->DoConfirmSetValue(id, sKey, CSD_ERROR, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnRequestSetAudioRec(CSecID id, const CString& sKey, const CString& sValue, DWORD dwParam, CIPC* pCipc, CWK_Profile& wkp)
{
	DWORD dwFlags = 0;
	if (sValue == CSD_AUDIO_RECORDING_OFF)
	{
		dwFlags = 0;
	}
	else if (sValue == CSD_AUDIO_RECORDING_L)
	{
		dwFlags = AUDIO_RECORDING_LEVEL;
	}
	else if (sValue == CSD_AUDIO_RECORDING_A)
	{
		dwFlags = AUDIO_RECORDING_ACTIVITY;
	}
	else if (sValue == CSD_AUDIO_RECORDING_E)
	{
		dwFlags = AUDIO_RECORDING_EXTERN;
	}
	else if (sValue == CSD_AUDIO_RECORDING_LA)
	{
		dwFlags = AUDIO_RECORDING_LEVEL|AUDIO_RECORDING_ACTIVITY;
	}
	else if (sValue == CSD_AUDIO_RECORDING_LE)
	{
		dwFlags = AUDIO_RECORDING_LEVEL|AUDIO_RECORDING_EXTERN;
	}
	else if (sValue == CSD_AUDIO_RECORDING_AE)
	{
		dwFlags = AUDIO_RECORDING_ACTIVITY|AUDIO_RECORDING_EXTERN;
	}
	else if (sValue == CSD_AUDIO_RECORDING_LAE)
	{
		dwFlags = AUDIO_RECORDING_LEVEL|AUDIO_RECORDING_EXTERN|AUDIO_RECORDING_ACTIVITY;
	}
	m_Settings.SetAudioRecordingBits(dwFlags);
	m_Settings.WriteSettings(wkp);

	CIPCAudioServer* pCIPCAudioServer = GetAudioGroup();
	if (pCIPCAudioServer)
	{
		// stop the current recording
		// by mode change
		// new alarm will cause new recording
		pCIPCAudioServer->SetRecording(AUDIO_RECORDING_ACTIVITY|
			AUDIO_RECORDING_EXTERN|AUDIO_RECORDING_LEVEL,FALSE);
	}

	CAudioClients& clients = GetAudioClients();
	for (int ci=0;ci<clients.GetSize();ci++) 
	{
		clients.GetAtFast(ci)->GetCIPC()->DoConfirmSetValue(id,sKey,sValue,0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::HandleExpansionCode(CIPC* pCipc,const CString& sValue)
{
	DWORD dwSerial = 0;
	DWORD dwDongle = 0;
	CRSA rsa(0,0);

	if (rsa.DecodeSerialDongle(sValue,dwSerial,dwDongle))
	{
		WK_TRACE(_T("expansion code decode success\n"));
		CString sComputerName = CSystemInfo::GetSystemSerialNumber(), s;
		sComputerName.MakeLower();
		WK_TRACE(_T("Orig ComputerName is %s\n"),sComputerName);
		for (int i=0;i<sComputerName.GetLength();i++)
		{
			if (   '0' <= sComputerName[i]
				&& sComputerName[i] <= '9')
			{
				s += sComputerName[i];
			}
		}
		if (s.IsEmpty())
		{
			for (int i=0;i<sComputerName.GetLength() && i<8;i++)
			{
				int iNum = sComputerName[i] % 10;
				s += (_TCHAR)('0'+iNum);
			}
		}
		s.TrimLeft();
		s.TrimRight();
		WK_TRACE(_T("Clean Device Serial is <%s>\n"),s);
		CString sSerial;
		sSerial.Format(_T("%d"),dwSerial);
		sSerial.TrimLeft();
		sSerial.TrimRight();
		WK_TRACE(_T("Clean Dongle Serial is <%s>\n"),sSerial);

		DWORD dw1,dw2;
		dw1 = dw2 = 0;
		_stscanf(sSerial,_T("%d"),&dw1);
		_stscanf(s,_T("%d"),&dw2);

		if (   dwSerial != 0
			&& !s.IsEmpty()
			&& (   s == sSerial 
			    || (   dw1 == dw2
				    && dw1!=0)

				)
			)
		{ 
			//test auf computername erfolgreich,
			//nun test ob flags korrekt

			//max. die ersten 6 flags sind gesetzt, also max. 0x0000002f
			if(dwDongle <= 0x0000002f) //47
			{
				// dongle code dekodiert und jetzt zum Starter
				WK_TRACE(_T("dongle matches computer name writing dongle values\n"));
				if (WK_IS_WINDOW(m_pMainWnd))
				{
					pCipc->DoConfirmSetValue(SECID_NO_ID,CSD_EXPANSION_CODE,sValue,0);
					CWK_Profile wkp;
					if (m_nNrOfJacobs>0)
					{
						wkp.WriteInt(_T("JacobUnit\\Device1\\EEPROM"),_T("RawBits"),dwDongle);
					}
					else if (m_nNrOfSavics>0)
					{
						wkp.WriteInt(_T("SaVicUnit\\Device1\\EEPROM"),_T("RawBits"),dwDongle);
					}
					else if (m_nNrOfTashas>0)
					{
						wkp.WriteInt(_T("TashaUnit\\Device1\\EEPROM"),_T("RawBits"),dwDongle);
					}
					else if (m_nNrOfQs>0)
					{
						wkp.WriteInt(_T("QUnit\\EEPROM"),_T("RawBits"),dwDongle);
					}

					wkp.WriteInt(_T("DV\\DVStarter"), _T("DoReboot"), 1);
					HWND hWnd = ((CMainFrame*)m_pMainWnd)->GetStarterWindowHandle();
					if (hWnd && IsWindow(hWnd))
					{
						PostMessage(hWnd,DVSTARTER_EXIT,0,0);
					}
					else
					{
						WK_TRACE(_T("starter not running for restart after expansion code\n"));
					}
				}
			}
			else
			{
				WK_TRACE(_T("dongle doesn't match available options\n"));
				pCipc->DoIndicateError(CIPC_BASE_REQUEST_SET_VALUE,SECID_NO_ID,CIPC_ERROR_ACCESS_DENIED,0,sValue);
			}
		}
		else
		{
			WK_TRACE(_T("dongle doesn't match computer name\n"));
			pCipc->DoIndicateError(CIPC_BASE_REQUEST_SET_VALUE,SECID_NO_ID,CIPC_ERROR_ACCESS_DENIED,0,sValue);
		}
	}
	else
	{
		pCipc->DoIndicateError(CIPC_BASE_REQUEST_SET_VALUE,SECID_NO_ID,CIPC_ERROR_CANT_DECODE,0,sValue);
	}


}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::SetReset(BOOL bProcess, BOOL bDatabase, BOOL bInformClients)
{
	if (bProcess)
	{
		// reset of process always inits and resets database too
		if (   WK_IS_WINDOW(m_pMainWnd) 
			&& !IsResetting())
		{
			m_bMustResetProcess = TRUE;
		}
	}
	else 
	{
		if (bDatabase)
		{
			TRACE(_T("SetReset bDatabase=%d\n"),bDatabase);
			if (   WK_IS_WINDOW(m_pMainWnd) 
				&& !IsResetting())
			{
				m_bMustInitDatabase = TRUE;
			}
		}
		if (bInformClients)
		{
			if (   WK_IS_WINDOW(m_pMainWnd) 
				&& !IsResetting())
			{
				m_InputClients.UpdateAllClients();
				m_OutputClients.UpdateAllClients();
			}
		}
	}
	if (WK_IS_WINDOW(m_pMainWnd))
	{
		m_pMainWnd->PostMessage(WM_USER);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::Reset()
{
	m_bResetting = TRUE;

	CWK_StopWatch w;

	WK_TRACE(_T("RESET ...\n"));

	Beep(220, 200);

	((CFrameWnd*)m_pMainWnd)->SetMessageText(_T("RESET..."));
	((CFrameWnd*)m_pMainWnd)->RedrawWindow();

	ShutDown();

	// Get COM port info
	DWORD dwMask = GetCommPortInfo();
	CWK_Profile wkpDVRT;
	wkpDVRT.WriteInt(_T("Common"),_T("CommPortInfo"),dwMask);

	ReadSettings();

	CreateInputOutput(m_Settings.GetNrOfCameras());
	CreateAudio();

	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	// load settings
	m_Inputs.Load(wkp);
	m_Outputs.Load(wkp);
	m_Audios.Load(wkp);

	// start cameras and relais
	DWORD dwTick = GetTickCount();
	m_Outputs.StartCIPCThreads();
	m_Outputs.WaitForConnect();
	m_Outputs.WaitForCameraScan();
	WK_TRACE(_T("outputs connected in %d ms\n"),GetTimeSpan(dwTick));

	if (   m_Settings.GetAudioIn() 
		|| m_Settings.GetAudioCam() != SECID_NO_ID
		|| m_Settings.GetAudioOut())
	{
		dwTick = GetTickCount();
		m_Audios.StartCIPCThreads();
		m_Audios.WaitForConnect();
		m_Audios.WaitForReset();
		WK_TRACE(_T("audios connected in %d ms\n"),GetTimeSpan(dwTick));
	}

	InitTimer(m_Outputs.GetNrOfCameras());
	CSystemTime st;
	st.GetLocalTime();
	RecalcTimer(st);

	// start alarms
	dwTick = GetTickCount();
	m_Inputs.StartCIPCThreads();
	m_Inputs.WaitForConnect();
	WK_TRACE(_T("inputs connected in %d ms\n"),GetTimeSpan(dwTick));

	if (InitStorage(FALSE))
	{
		m_bMustResetDatabase = !ResetStorage();
	}
	else
	{
		CWK_RunTimeError e(WAI_SECURITY_SERVER,REL_ERROR,RTE_CONFIGURATION,
							_T("Cannot initialize database. System cannot store."));
		e.Send();
	}

	if (   GetSettings().GetRequestDIBs()
		&& m_nNrOfSavics+m_nNrOfQs>0)
	{
		m_EncoderThread.StartThread();
		if (m_nNrOfQs>0)
		{
			m_EncoderThread.CreateEncoders(&m_Outputs,GetSettings().GetResolution(),GetSettings().GetMPEG4()?COMPRESSION_MPEG4:COMPRESSION_JPEG);
		}
	}
	m_Outputs.StartRequests();
	m_VOutThread.StartThread();
	m_CallThread.StartThread();

	w.StopWatch(_T("RESET ... SUCCESS"));
	
	m_bResetting = FALSE;
	m_bMustResetProcess = FALSE;

	m_InputClients.UpdateAllClients();
	m_OutputClients.UpdateAllClients();

	((CFrameWnd*)m_pMainWnd)->SetMessageText(_T("reset complete"));
	((CFrameWnd*)m_pMainWnd)->RedrawWindow();

	Beep(440, 200);
	Beep(880, 100);
	((CMainFrame*)m_pMainWnd)->SetTimer(5000);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::ShutDownAudio()
{
	// first set all audio references to NULL
	m_Inputs.ResetAudioReferences();
	m_Outputs.ResetAudioReferences();

	CString sShmName;
	sShmName.Format(SM_AudioUnitInput,1);
	m_Inputs.DeleteGroup(sShmName);

	if (m_Audios.GetSize()>0)
	{
		m_Audios.SafeDeleteAll();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::ResetAudio()
{
	WK_TRACE(_T("RESET AUDIO...\n"));
	ShutDownAudio();

	ReadSettings();
	CreateAudio();
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	m_Audios.Load(wkp);

	if (   m_Settings.GetAudioIn() 
		|| m_Settings.GetAudioOut()
		|| m_Settings.GetAudioCam() != SECID_NO_ID)
	{
		DWORD dwTick = GetTickCount();
		m_Audios.StartCIPCThreads();
		m_Audios.WaitForConnect();
		m_Audios.WaitForReset();

		CString sShmName;
		sShmName.Format(SM_AudioUnitInput,1);
		m_Inputs.StartGroup(sShmName);

		WK_TRACE(_T("audios connected in %d ms\n"),GetTimeSpan(dwTick));
	}


	WK_TRACE(_T("RESET AUDIO COMPLETE...\n"));
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::ShutDown()
{
    m_bShutDown = TRUE;
	if (m_Inputs.GetSize()>0)
	{
		Sleep(1000);
	}

	WK_TRACE(_T("SHUTTING DOWN\n"));
	m_CallThread.StopThread();
	m_VOutThread.StopThread();
	ShutDownAudio();
	if (m_Outputs.GetSize()>0)
	{
		m_Outputs.StopRequests();
		m_Outputs.SafeDeleteAll();
		Sleep(100);
	}
	if (m_EncoderThread.IsRunning())
	{
		m_EncoderThread.StopThread();
	}
	if (m_Inputs.GetSize()>0)
	{
		m_Inputs.CleanUp();
		Sleep(100);
	}
	WK_DELETE(m_pCIPCDatabaseProcess);

	WK_TRACE(_T("SHUT DOWN\n"));

    m_bShutDown = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::CreateInputOutput(int iNum)
{
	WK_TRACE(_T("starting with %d Jacob(s) and %d Savic(s) and %d Tasha(s) and %d Q(s)\n"),
		m_nNrOfJacobs,m_nNrOfSavics,m_nNrOfTashas,m_nNrOfQs);
		
	CInputGroup* pAlarm = NULL;
	CInputGroup* pMD = NULL;
	COutputGroup* pRelais = NULL;
	CCameraGroup* pCamera = NULL;
	int iUnits = 0;
	int iMaxCamPerUnit = 8;

	if (   m_nNrOfAllowedJacobs == 1
		&& iNum == 16)
	{
		iMaxCamPerUnit = 16;
	}
	if (m_nNrOfQs == 1) //one QUnit can have 4, 8 or 16 cameras, depending on the dongle 
	{
		//use the available QUnit cameras allowed by dondle
		if(iNum == 4)
		{
			iMaxCamPerUnit = 4;
		}
		else if(iNum == 16)
		{
			iMaxCamPerUnit = 16;
		}
	}

	
	if (m_nNrOfJacobs>0)
	{
		// 1. Jacob 
		// 16 Alarmeingänge
		if (!GetSettings().IgnoreExternalDetectors())
		{
			pAlarm = new CInputGroup(&m_Inputs, SM_MICO_INPUT, SECID_GROUP_INPUT+MAX_UNIT, 16, iMaxCamPerUnit, FALSE);
			m_Inputs.SafeAdd(pAlarm);
		}
		
		// Motion Detection
		// MD muß denselben ID haben wie die Kamera(!)
		pMD = new CInputGroup(&m_Inputs, SM_MICO_MD_INPUT, SECID_GROUP_INPUT, min(iMaxCamPerUnit,iNum), iMaxCamPerUnit, TRUE);
		m_Inputs.SafeAdd(pMD);

		// Kameras
		pCamera = new CCameraGroup(SM_MICO_OUTPUT_CAMERAS,SECID_GROUP_OUTPUT,min(iMaxCamPerUnit,iNum));
		m_Outputs.SafeAdd(pCamera);
		
		InternalLink(pAlarm,pMD,pCamera,iMaxCamPerUnit);
		
		// Relais
		pRelais = new CRelayGroup(SM_MICO_OUTPUT_RELAYS,SECID_GROUP_OUTPUT+MAX_UNIT,4);
		m_Outputs.SafeAdd(pRelais);
		iUnits++;
	}
	if (m_nNrOfJacobs>1)
	{
		iNum = 16;
		// 2. Jacob 
		// 16 Alarmeingänge
		if (!GetSettings().IgnoreExternalDetectors())
		{
			pAlarm = new CInputGroup(&m_Inputs,SM_MICO_INPUT2,SECID_GROUP_INPUT+MAX_UNIT+iUnits,16,iMaxCamPerUnit,FALSE);
			m_Inputs.SafeAdd(pAlarm);
		}
		
		// Motion Detection
		// MD muß denselben ID haben wie die Kamera(!)
		pMD = new CInputGroup(&m_Inputs, SM_MICO_MD_INPUT2, SECID_GROUP_INPUT+iUnits, min(iMaxCamPerUnit,iNum-iMaxCamPerUnit), iMaxCamPerUnit, TRUE);
		m_Inputs.SafeAdd(pMD);
		// Kameras
		pCamera = new CCameraGroup(SM_MICO_OUTPUT_CAMERAS2,SECID_GROUP_OUTPUT+iUnits,min(iMaxCamPerUnit,iNum-iMaxCamPerUnit));
		m_Outputs.SafeAdd(pCamera);
		// Relais
		pRelais = new CRelayGroup(SM_MICO_OUTPUT_RELAYS2,SECID_GROUP_OUTPUT+MAX_UNIT+iUnits,4);
		m_Outputs.SafeAdd(pRelais);
		
		InternalLink(pAlarm,pMD,pCamera,iMaxCamPerUnit);
		iUnits++;
	}

	// eine oder keine Jacob
	// Savic nutzen
	if (m_nNrOfSavics>0)
	{
		// 1. Savic
		// Alarmeingänge
		if (!GetSettings().IgnoreExternalDetectors())
		{
			pAlarm = new CInputGroup(&m_Inputs, SM_SAVIC_INPUT, SECID_GROUP_INPUT+MAX_UNIT+iUnits, 4, 4, FALSE);
			m_Inputs.SafeAdd(pAlarm);
		}
		// Motion Detection
		pMD = new CInputGroup(&m_Inputs, SM_SAVIC_MD_INPUT, SECID_GROUP_INPUT+iUnits, 4, 4, TRUE);
		m_Inputs.SafeAdd(pMD);
		// Kameras
		pCamera = new CCameraGroup(SM_SAVIC_OUTPUT_CAMERAS,SECID_GROUP_OUTPUT+iUnits,4);
		m_Outputs.SafeAdd(pCamera);
	
		// Relais
		pRelais = new CRelayGroup(SM_SAVIC_OUTPUT_RELAYS,SECID_GROUP_OUTPUT+MAX_UNIT+iUnits,1);
		m_Outputs.SafeAdd(pRelais);
		InternalLink(pAlarm,pMD,pCamera,4);

		iUnits++;
	}
	if (m_nNrOfSavics>1)
	{
		// 2. Savic nutzen
		// Alarmeingänge
		if (!GetSettings().IgnoreExternalDetectors())
		{
			pAlarm = new CInputGroup(&m_Inputs, SM_SAVIC_INPUT2, SECID_GROUP_INPUT+MAX_UNIT+iUnits, 4, 4, FALSE);
			m_Inputs.SafeAdd(pAlarm);
		}
		// Motion Detection
		pMD = new CInputGroup(&m_Inputs, SM_SAVIC_MD_INPUT2, SECID_GROUP_INPUT+iUnits, 4, 4, TRUE);
		m_Inputs.SafeAdd(pMD);
		// Kameras
		pCamera = new CCameraGroup(SM_SAVIC_OUTPUT_CAMERAS2,SECID_GROUP_OUTPUT+iUnits,4);
		m_Outputs.SafeAdd(pCamera);
	
		// Relais
		pRelais = new CRelayGroup(SM_SAVIC_OUTPUT_RELAYS2,SECID_GROUP_OUTPUT+MAX_UNIT+iUnits,1);
		m_Outputs.SafeAdd(pRelais);
		InternalLink(pAlarm,pMD,pCamera,4);

		iUnits++;
	}
	if (m_nNrOfSavics>2)
	{
		// 3. Savic nutzen
		// Alarmeingänge
		if (!GetSettings().IgnoreExternalDetectors())
		{
			pAlarm = new CInputGroup(&m_Inputs,SM_SAVIC_INPUT3,SECID_GROUP_INPUT+MAX_UNIT+iUnits,4,4,FALSE);
			m_Inputs.SafeAdd(pAlarm);
		}
		// Motion Detection
		pMD = new CInputGroup(&m_Inputs,SM_SAVIC_MD_INPUT3,SECID_GROUP_INPUT+iUnits,4,4,TRUE);
		m_Inputs.SafeAdd(pMD);
		// Kameras
		pCamera = new CCameraGroup(SM_SAVIC_OUTPUT_CAMERAS3,SECID_GROUP_OUTPUT+iUnits,4);
		m_Outputs.SafeAdd(pCamera);
	
		// Relais
		pRelais = new CRelayGroup(SM_SAVIC_OUTPUT_RELAYS3,SECID_GROUP_OUTPUT+MAX_UNIT+iUnits,1);
		m_Outputs.SafeAdd(pRelais);
		InternalLink(pAlarm,pMD,pCamera,4);

		iUnits++;
	}
	if (m_nNrOfSavics>3)
	{
		// 4. Savic nutzen
		// Alarmeingänge
		if (!GetSettings().IgnoreExternalDetectors())
		{
			pAlarm = new CInputGroup(&m_Inputs,SM_SAVIC_INPUT4,SECID_GROUP_INPUT+MAX_UNIT+iUnits,4,4,FALSE);
			m_Inputs.SafeAdd(pAlarm);
		}
		// Motion Detection
		pMD = new CInputGroup(&m_Inputs,SM_SAVIC_MD_INPUT4,SECID_GROUP_INPUT+iUnits,4,4,TRUE);
		m_Inputs.SafeAdd(pMD);
		// Kameras
		pCamera = new CCameraGroup(SM_SAVIC_OUTPUT_CAMERAS4,SECID_GROUP_OUTPUT+iUnits,4);
		m_Outputs.SafeAdd(pCamera);
	
		// Relais
		pRelais = new CRelayGroup(SM_SAVIC_OUTPUT_RELAYS4,SECID_GROUP_OUTPUT+MAX_UNIT+iUnits,1);
		m_Outputs.SafeAdd(pRelais);
		InternalLink(pAlarm,pMD,pCamera,4);

		iUnits++;
	}

	if (m_nNrOfTashas>0)
	{
		int iNrOfCam = min(iNum,8);
		// 1. Tasha
		// Alarmeingänge
		if (!GetSettings().IgnoreExternalDetectors())
		{
			pAlarm = new CInputGroup(&m_Inputs, SM_TASHA_INPUT, SECID_GROUP_INPUT+MAX_UNIT+iUnits, 8, iNrOfCam, FALSE);
			m_Inputs.SafeAdd(pAlarm);
		}
		// Motion Detection
		pMD = new CInputGroup(&m_Inputs, SM_TASHA_MD_INPUT, SECID_GROUP_INPUT+iUnits, iNrOfCam, iNrOfCam, TRUE);
		m_Inputs.SafeAdd(pMD);
		// Kameras
		pCamera = new CCameraGroup(SM_TASHA_OUTPUT_CAMERAS,SECID_GROUP_OUTPUT+iUnits,iNrOfCam);
		m_Outputs.SafeAdd(pCamera);

		// Relais
		pRelais = new CRelayGroup(SM_TASHA_OUTPUT_RELAYS,SECID_GROUP_OUTPUT+MAX_UNIT+iUnits,1);
		m_Outputs.SafeAdd(pRelais);
		InternalLink(pAlarm,pMD,pCamera,iNrOfCam);

		iUnits++;
	}
	if (m_nNrOfTashas>1)
	{
		// 2. Tasha
		// Alarmeingänge
		if (!GetSettings().IgnoreExternalDetectors())
		{
			pAlarm = new CInputGroup(&m_Inputs, SM_TASHA_INPUT2, SECID_GROUP_INPUT+MAX_UNIT+iUnits, 8, 8, FALSE);
			m_Inputs.SafeAdd(pAlarm);
		}
		// Motion Detection
		pMD = new CInputGroup(&m_Inputs, SM_TASHA_MD_INPUT2, SECID_GROUP_INPUT+iUnits, 8, 8, TRUE);
		m_Inputs.SafeAdd(pMD);
		// Kameras
		pCamera = new CCameraGroup(SM_TASHA_OUTPUT_CAMERAS2,SECID_GROUP_OUTPUT+iUnits,8);
		m_Outputs.SafeAdd(pCamera);

		// Relais
		pRelais = new CRelayGroup(SM_TASHA_OUTPUT_RELAYS2,SECID_GROUP_OUTPUT+MAX_UNIT+iUnits,1);
		m_Outputs.SafeAdd(pRelais);
		InternalLink(pAlarm,pMD,pCamera,8);

		iUnits++;
	}
	if (m_nNrOfQs>0)
	{
		// 1. Q
		// Alarmeingänge
		if (!GetSettings().IgnoreExternalDetectors())
		{
			pAlarm = new CInputGroup(&m_Inputs, SM_Q_INPUT, SECID_GROUP_INPUT+MAX_UNIT+iUnits, iMaxCamPerUnit, iMaxCamPerUnit, FALSE);
			m_Inputs.SafeAdd(pAlarm);
		}
		// Motion Detection
		pMD = new CInputGroup(&m_Inputs, SM_Q_MD_INPUT, SECID_GROUP_INPUT+iUnits, iMaxCamPerUnit, iMaxCamPerUnit, TRUE);
		m_Inputs.SafeAdd(pMD);
		// Kameras
		pCamera = new CCameraGroup(SM_Q_OUTPUT_CAMERA,SECID_GROUP_OUTPUT+iUnits,iMaxCamPerUnit);
		m_Outputs.SafeAdd(pCamera);

		// Relais
		pRelais = new CRelayGroup(SM_Q_OUTPUT_RELAY,SECID_GROUP_OUTPUT+MAX_UNIT+iUnits,4);
		m_Outputs.SafeAdd(pRelais);
		InternalLink(pAlarm,pMD,pCamera,8);

		iUnits++;
	}

	// CommUnit für PTZ-Kameras
	WORD wGroup = HIWORD(SECID_INPUT_COMMUNIT);
	CInputGroup* pPTZGroup = new CInputGroup(&m_Inputs, SM_COMMUNIT_INPUT, wGroup, 1, 1,FALSE);
	m_Inputs.SafeAdd(pPTZGroup);
	CInput* pPTZ  = pPTZGroup->GetAtFast(0);
	pPTZ->SetMode(CInput::IM_PTZ); 

	m_Outputs.CalcClientIDs();
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::CreateAudio()
{
	CCamera* pCamera = (CCamera*)m_Outputs.GetOutputByUnitID(GetSettings().GetAudioCam());
	CInput* pAudio = NULL;
	BOOL bMicrofonOn =     m_Settings.GetAudioIn()
						|| pCamera != NULL;

	CString sKey;
	sKey.Format(AUDIO_UNIT_DEVICE, 1);
	sKey = _T("DVRT\\") + sKey;
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, sKey, "");
	wkp.WriteInt(AU_CIPC_SETTINGS , AU_GENERATE_ALARM, bMicrofonOn);


	if (bMicrofonOn)
	{
		// die Audiomelder
		CString sShmName;
		sShmName.Format(SM_AudioUnitInput,1);
		WORD wGroup = HIWORD(SECID_INPUT_COMMUNIT)-1;
		CInputGroup* pAudioInGroup = new CInputGroup(&m_Inputs,sShmName, wGroup, 1, 1,FALSE);
		pAudio  = pAudioInGroup->GetAtFast(0);
		pAudio->SetMode(CInput::IM_AUDIO); 
		m_Inputs.SafeAdd(pAudioInGroup);

		if (pCamera)
		{
			pAudio->SetCamera(pCamera);
			pCamera->SetAudioInput(pAudio);
		}
	}
	if (   bMicrofonOn
		|| m_Settings.GetAudioOut())
	{
		CString sShmName;
		sShmName.Format(SM_AudioUnitMedia,1);
		CIPCAudioServer* pCIPCAudioServer;
		pCIPCAudioServer = new CIPCAudioServer(SECID_GROUP_MEDIA,
											   sShmName,
											   bMicrofonOn,
											   m_Settings.GetAudioOut());
		m_Audios.Add(pCIPCAudioServer);

		if (   m_Settings.GetAudioIn()
			|| m_Settings.GetAudioCam() != SECID_NO_ID)
		{
			// assume input is at pos 0 always
			pCIPCAudioServer->GetAtFast(0)->SetCamera(pCamera);
			pAudio->SetMedia(pCIPCAudioServer->GetAtFast(0));
			if (pCamera)
			{
				pCamera->SetMedia(pCIPCAudioServer->GetAtFast(0));
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::InternalLink(CInputGroup* pAlarmGroup, 
								 CInputGroup* pMDGroup, 
								 CCameraGroup* pCameraGroup,
								 int iNrExternal)
{
	CInput* pExtern = NULL;
	CInput* pSuspect = NULL;
	CInput* pMotion = NULL;

	CCamera* pCamera = NULL;

	if (pCameraGroup)
	{
		for (int i=0; i<pCameraGroup->GetSize(); i++)
		{
			pExtern = NULL;
			pSuspect = NULL;
			pMotion = NULL;
			pCamera = NULL;

			if (pAlarmGroup)
			{
				pExtern  = pAlarmGroup->GetAtFast(i);
				if (i+iNrExternal<pAlarmGroup->GetSize())
					pSuspect = pAlarmGroup->GetAtFast(i+iNrExternal);
				else
					pSuspect = NULL;
			}
			if (pMDGroup)
			{
				pMotion  = pMDGroup->GetAtFast(i);
			}

			pCamera = (CCamera*)pCameraGroup->GetAtFast(i);
			pCamera->SetInputs(pExtern,pSuspect,pMotion);

			if (pExtern)
			{
				pExtern->SetCamera(pCamera);
			}
			if (pMotion)
			{
				pMotion->SetCamera(pCamera);
			}
			if (pSuspect)
			{
				pSuspect->SetCamera(pCamera);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVProcessApp::SavePicture(CCamera* pCamera,
								const CIPCPictureRecord& pict,
								CIPCFields& fields,
								WORD wArchiveNr)
{

	if (   pict.GetCompressionType() == COMPRESSION_RGB_24
		|| pict.GetCompressionType() == COMPRESSION_YUV_422)
	{
		if (m_EncoderThread.IsRunning())
		{
			CompressionType ct = m_Settings.GetMPEG4() ? COMPRESSION_MPEG4 : COMPRESSION_JPEG;
			fields.Lock();
			m_EncoderThread.AddPictureRecord(pCamera,pict,fields,wArchiveNr,0,ct);
			fields.Unlock();
		}
		return TRUE;
	}
	else 
	{
		if (   m_pCIPCDatabaseProcess
			&& (m_pCIPCDatabaseProcess->GetIPCState() == CIPC_STATE_CONNECTED))
		{
			fields.Lock();
			if (GetSettings().TraceSave())
			{
				WK_TRACE(_T("save    %d %d %02d:%02d:%02d,%03d %s-Frame\n"),
					wArchiveNr,
					pict.GetBlockNr(),
					pict.GetTimeStamp().GetHour(),
					pict.GetTimeStamp().GetMinute(),
					pict.GetTimeStamp().GetSecond(),
					pict.GetTimeStamp().GetMilliseconds(),
					(pict.GetPictureType()==SPT_DIFF_PICTURE)?_T("P"):_T("I"));
			}
			m_pCIPCDatabaseProcess->DoRequestNewSavePicture(wArchiveNr,0,pict,fields);
			fields.Unlock();
			InterlockedIncrement(&m_dwFramesToRecord);
			
			// //ML 08.07.04 Confirm-Timeout erhöht, da die Tasha sehr viele Frames/Sekunde produziert.
			if (m_dwFramesToRecord>=1000)
//			if (m_dwFramesToRecord>=200)
			{
				// 20 images without break could not be stored
				// so raise the alarm
				CString s;
				s.LoadString(IDS_STORAGE);
				CWK_RunTimeError e(WAI_SECURITY_SERVER,
					REL_ERROR,
					RTE_STORAGE,
					s,
					pict.GetCamID().GetID(),
					m_dwFramesToRecord
					);
				e.Send();
				InterlockedExchange(&m_dwFramesToRecord,0);
			}
			
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVProcessApp::SaveMedia(const CIPCMediaSampleRecord& media,WORD wArchiveNr)
{
	if (   m_pCIPCDatabaseProcess
		&& (m_pCIPCDatabaseProcess->GetIPCState() == CIPC_STATE_CONNECTED))
	{
/*
		TRACE(_T("CDVProcessApp::SaveMedia %04x,%d %d\n"),wArchiveNr,
			media.GetPackageNumber(),media.GetActualLength());
*/
		CIPCFields fields;
		m_pCIPCDatabaseProcess->DoRequestSaveMedia(wArchiveNr,
													SECID_NO_ID,
													FALSE,FALSE,
													media,fields);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::ConfirmRecordedFrame()
{
	InterlockedExchange(&m_dwFramesToRecord,0);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::SaveAlarmData(CIPCFields& fields,
							 CSecID camID,
							 WORD wArchiveNr)
{
	if (m_pCIPCDatabaseProcess
		&& (m_pCIPCDatabaseProcess->GetIPCState() == CIPC_STATE_CONNECTED))
	{
		fields.Lock();
		m_pCIPCDatabaseProcess->DoRequestSaveData(wArchiveNr,fields);
		fields.Unlock();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::ConfirmLivePicture(CCamera* pCamera, const CIPCPictureRecord& pict, DWORD x, DWORD y)
{
	m_OutputClients.Lock();
	for (int i=0;i<m_OutputClients.GetSize();i++)
	{
        COutputClient* pC = m_OutputClients.GetAtFast(i);
		if (   pC->IsConnected()
			&& (   !pC->IsLowBandwidth() 
			    || pict.GetCompressionType() != COMPRESSION_MPEG4))
		{
			CIPCOutputDVClient* pOC = pC->GetCIPC();
			pOC->ConfirmPictureSlow(pCamera,pict,x,y);
		}
	}
	m_OutputClients.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::ConnectToStorage()
{
	// check database to server connection
	if (m_pCIPCDatabaseProcess == NULL)
	{
		((CFrameWnd*)m_pMainWnd)->SetMessageText(_T("connecting to storage"));
		m_pCIPCDatabaseProcess = new CIPCDatabaseProcess(_T("DataBaseToServer"));
		int cc = 10;
		while (    (cc-->0)
				&& (m_pCIPCDatabaseProcess->GetIPCState()!=CIPC_STATE_CONNECTED)
				)
		{
			Sleep(50);
		}
		((CFrameWnd*)m_pMainWnd)->SetMessageText(_T("connected to storage"));
	}
	else 
	{
		if (m_pCIPCDatabaseProcess->GetIsMarkedForDelete())
		{
			WK_DELETE(m_pCIPCDatabaseProcess);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVProcessApp::InitDrive(CIPCDrive* pDrive)
{
	BOOL bOK = TRUE;
	CFileFind finder;
	CString sName;
	BOOL bWorking = finder.FindFile(pDrive->GetRootString() + _T("*.*"));
	
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (!finder.IsDots())
		{
			sName = finder.GetFileName();
			sName.MakeLower();
			// ignore . and ..
			if (finder.IsDirectory())
			{
				// Papierkorb ignorieren !
				if (   (sName!=_T("dbs")) 
					&& (sName!=_T("dvs"))
					&& (sName!=_T("recycled")) 						// Win 95/98
					&& (sName!=_T("recycler"))						// Win NT/2000
					&& (sName!=_T("system volume information"))     // Win 2000
					&& (sName!=_T("_restore"))						// Win 2000/ME
					&& (sName!=_T("backup"))						// idip/DTS
					&& (sName!=_T("log"))						// idip/DTS
					&& (sName!=_T("protocol"))						// idip/DTS
					&& (sName!=_T("temp"))						// idip/DTS
					&& (sName!=_T("dvrtwww"))						// idip/DTS
					&& (sName!=_T("out"))						// idip/DTS
					)	
				{
					// foreign directory found !
					CString sDir(pDrive->GetRootString());
					sDir += sName;
					WK_TRACE(_T("foreign dir found %s\n"),sDir);
					if (!pDrive->IsDatabase())
					{
						bOK = FALSE;
						break;
					}
				}
			}
			else
			{
				// foreign file found !
				// simply ignore this
			}
		}
	}

	if (bOK)
	{
		WK_TRACE(_T("using drive %s for database\n"),pDrive->GetRootString());
		pDrive->SetUsedAs(DVR_DB_DRIVE);
	}
	else
	{
		WK_TRACE(_T("not using drive %s\n"),pDrive->GetRootString());
		if (!pDrive->IsWriteBackup())
		{
			pDrive->SetUsedAs(DVR_NORMAL_DRIVE);
		}
	}
	
	return pDrive->IsDatabase();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVProcessApp::InitDrives(CIPCDrives& drives, CWK_Profile& wkp)
{
	((CFrameWnd*)m_pMainWnd)->SetMessageText(_T("initializing drives"));
	// load current settings
	drives.Init(wkp);

	// now get usage for drives
	CIPCDrive* pDrive;
	for (int i=0;i<drives.GetSize();i++)
	{
		pDrive = drives.GetAtFast(i);
		if ((tolower(pDrive->GetLetter())<='b'))
		{
			continue;
		}

		if (pDrive->GetType() == DRIVE_FIXED)
		{
			// it's a hard disk
			if (m_Settings.m_bLocalHD)
			{
				InitDrive(pDrive);
			}
			else
			{
				pDrive->SetUsedAs(DVR_NORMAL_DRIVE);
			}
		}
		else if (pDrive->GetType() == DRIVE_REMOVABLE)
		{
			// it's a removable
			if (m_Settings.m_bRemovable)
			{
				InitDrive(pDrive);
			}
			else
			{
				pDrive->SetUsedAs(DVR_NORMAL_DRIVE);
			}
		}
		else
		{
			pDrive->SetUsedAs(DVR_NORMAL_DRIVE);
		}

	}

	BOOL bRet = FALSE;
	BOOL bBackup = FALSE;
	// mindestens ein Laufwerk
	for (i=0;i<drives.GetSize();i++)
	{
		pDrive = drives.GetAtFast(i);
		if (pDrive->IsDatabase())
		{
			bRet = TRUE;
		}

		if (!bBackup &&
			(tolower(pDrive->GetLetter())>'b')
			)
		{
			if (pDrive->IsCDROM())
			{
				FS_Producer fs = pDrive->GetFSProducer();
				if (   (fs == FSP_UDF_ADAPTEC_DIRECTCD)
					|| (fs == FSP_UDF_CEQUADRAT_PACKETCD)
					|| (fs == FSP_NERO_BURNING_ROM)
					)
				{
					pDrive->SetUsedAs(DVR_WRITE_BACKUP_DRIVE);
					WK_TRACE(_T("using %s as backup drive\n"),pDrive->GetRootString());
					bBackup = TRUE; 
				}
				else
				{
					pDrive->SetUsedAs(DVR_NORMAL_DRIVE);
				}
			}
			if (!m_Settings.m_bRemovable && (pDrive->GetType() == DRIVE_REMOVABLE))
			{
				pDrive->SetUsedAs(DVR_WRITE_BACKUP_DRIVE);
				WK_TRACE(_T("using %s as backup drive\n"),pDrive->GetRootString());
				bBackup = TRUE;
			}
		}
	}

	drives.Trace();
	drives.Save(wkp);

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVProcessApp::InitStorage(BOOL bDeleteAllowed)
{
	BOOL bAudioReset = FALSE;
	CIPCDrives drives;
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));

	WK_TRACE(_T("INIT DATABASE deleting data %s allowed\n"),bDeleteAllowed ? _T("") : _T("not"));
	WK_TRACE(_T("initializing drives\n"));
	if (!InitDrives(drives,wkp))
	{
		CWK_RunTimeError e(WAI_SECURITY_SERVER,REL_ERROR,RTE_CONFIGURATION,
							_T("No valid storage drive or media. System cannot store."));
		e.Send();
		return FALSE;
	}
	drives.Trace();
	DWORD dwAll = drives.GetAvailableMB();
	WK_TRACE(_T("all drives have %d MB\n"),dwAll);
	WK_TRACE(_T("initialized drives\n"));
	WK_TRACE(_T("%d Jacob(s) %d Savic(s) %d Tasha(s) %d Q(s) %d Camera(s)\n"),
		m_nNrOfJacobs,m_nNrOfSavics,m_nNrOfTashas, m_nNrOfQs, m_Settings.GetNrOfCameras());
	int iCameras = m_Outputs.GetNrOfActiveCameras();
	WK_TRACE(_T("nr of active cam %d of %d\n"),iCameras,m_Settings.GetNrOfCameras());

	((CFrameWnd*)m_pMainWnd)->SetMessageText(_T("initializing sizes"));
	

	if (dwAll == 0)
	{
		CWK_RunTimeError e(WAI_SECURITY_SERVER,REL_ERROR,RTE_CONFIGURATION,
							_T("No space on drive or media. System cannot store."));
		e.Send();
		return FALSE;
	}

	__int64 iFpDAll,iFpWAll;
	iFpWAll = 0;
	iFpDAll = 0;
	DWORD dwAlarmAll = 0;

	DWORD dwAlarmOneJPEG,dwPreAlarmOneJPEG;
	DWORD dwAlarmOneMPEG4,dwPreAlarmOneMPEG4;
	
	WORD wAlarmNr,wPreAlarmNr,wMDNr,wCam,wAlarmListNr;
	int iAlarmCams,iMDCams;
	
	dwPreAlarmOneJPEG = (((IMAGE_SIZE_KB * GetSettings().GetNrAlarmSeconds()) * 110) / 100) / 1024;
	dwAlarmOneJPEG = (((((IMAGE_SIZE_KB * 2 * GetSettings().GetNrAlarmSeconds()) * 110) / 100) / 1024) * UVV_NR_ALARM * 3) / 2;
	
	dwPreAlarmOneMPEG4 = 250;
	dwAlarmOneMPEG4 = dwPreAlarmOneMPEG4*12;
	
	iAlarmCams = 0;
	iMDCams = 0;

	CArchivInfoArray aiOld;
	aiOld.Load(wkp);

	// first fix all cams in UVV Mode
	for (int i=0;i<m_Outputs.GetSize();i++)
	{
		COutputGroup* pGroup = m_Outputs.GetAtFast(i);
		if (pGroup->HasCameras())
		{
			for (int j=0;j<pGroup->GetSize();j++)
			{
				CCamera* pCamera = (CCamera*)pGroup->GetAtFast(j);
				wCam = pCamera->GetClientID().GetSubID();
				wAlarmNr = (WORD)(wCam | (ALARM_ARCHIV<<12));
				wMDNr = (WORD)(wCam | (RING_ARCHIV<<12));
				wPreAlarmNr = (WORD)(wCam | (SICHERUNGS_RING_ARCHIV<<12));
				wAlarmListNr = (WORD)(wCam | (ALARM_LIST_ARCHIV<<12)); 
				if (pCamera->IsActive())
				{
					if (pCamera->GetMode() == COutput::OM_UVV)
					{
						if (   pCamera->GetGroup()->IsTasha()
							|| pCamera->GetGroup()->IsQ())
						{
							dwAlarmAll += dwAlarmOneMPEG4 + dwPreAlarmOneMPEG4;
						}
						else
						{
							dwAlarmAll += dwAlarmOneJPEG + dwPreAlarmOneJPEG;
						}
						iAlarmCams++;
					}
					else if (pCamera->GetMode() == COutput::OM_MD)
					{
						iFpDAll += pCamera->GetFramesPerDay();
						iFpWAll += pCamera->GetFramesPerWeek();
						iMDCams++;
					}
				}
				else if (   !pCamera->IsLastSaveOld()
						 || !bDeleteAllowed)
				{
					// Kamera deaktiviert, gibt es noch
					// Archive für diese Kamera ?
					CArchivInfo* pMDOld = aiOld.GetArchivInfo(CSecID(SECID_GROUP_ARCHIVE,wMDNr));
					if (pMDOld != NULL)
					{
						// es gibt noch ein Archiv MD
						iFpDAll += pCamera->GetFramesPerDay();
						iFpWAll += pCamera->GetFramesPerWeek();
						iMDCams++;
					}
					CArchivInfo* pAlarmOld = aiOld.GetArchivInfo(CSecID(SECID_GROUP_ARCHIVE,wAlarmNr));
					if (pAlarmOld != NULL)
					{
						// es gibt noch ein Archiv Alarm
						if (pCamera->GetGroup()->IsTasha()
							||pCamera->GetGroup()->IsQ())
						{
							dwAlarmAll += dwAlarmOneMPEG4 + dwPreAlarmOneMPEG4;
						}
						else
						{
							dwAlarmAll += dwAlarmOneJPEG + dwPreAlarmOneJPEG;
						}
						iAlarmCams++;
					}
				}
				else
				{
					// not active or old pictures
				}
			}
		}
	}

	WK_TRACE(_T("ALARM %d MB\n"),dwAlarmAll);
	DWORD dwMDAll = dwAll - dwAlarmAll;
    WK_TRACE(_T("RING  %d MB\n"),dwMDAll);

	TRACE(_T("FpD %I64d\n"),iFpDAll);
	TRACE(_T("FpW %I64d\n"),iFpWAll);

	CArchivInfoArray aiNew;
	CArchivInfo* pAlarm;
	CArchivInfo* pPreAlarm;
	CArchivInfo* pMD;
	CArchivInfo* pAlarmList;
	BOOL bHadMD,bHadAlarm;


	for (i=0;i<m_Outputs.GetSize();i++)
	{
		COutputGroup* pGroup = m_Outputs.GetAtFast(i);
		if (pGroup->HasCameras())
		{
			for (int j=0;j<pGroup->GetSize();j++)
			{
				bHadMD = FALSE;
				bHadAlarm = FALSE;

				CCamera* pCamera = (CCamera*)pGroup->GetAtFast(j);
				wCam = pCamera->GetClientID().GetSubID();
				wAlarmNr = (WORD)(wCam | (ALARM_ARCHIV<<12));
				wMDNr = (WORD)(wCam | (RING_ARCHIV<<12));
				wPreAlarmNr = (WORD)(wCam | (SICHERUNGS_RING_ARCHIV<<12));
				wAlarmListNr = (WORD)(wCam | (ALARM_LIST_ARCHIV<<12)); 

				if (!pCamera->IsActive())
				{
					// Kein Videosignal Löschen ?
					if (   pCamera->IsLastSaveOld()
						&& bDeleteAllowed)
					{
						CArchivInfo* pMDOld = aiOld.GetArchivInfo(CSecID(SECID_GROUP_ARCHIVE,wMDNr));
						CArchivInfo* pAlarmOld = aiOld.GetArchivInfo(CSecID(SECID_GROUP_ARCHIVE,wAlarmNr));
						if (pMDOld || pAlarmOld)
						{
							WK_TRACE(_T("camera %s not active and last saved picture to old %s\n"),
								pCamera->GetName(),
								pCamera->GetLastSave().GetDateTime());
							WK_TRACE(_T("deleting archive for %s\n"),pCamera->GetName());
						}
						if (pCamera->GetUnitID() == m_Settings.GetAudioCam())
						{
							// die Audiocamera ist lange weg
							// also auf SECID_NO_ID setzen.
							// und audioreset auslösen
							m_Settings.SetAudioCam(SECID_NO_ID);
							bAudioReset = TRUE;
							WK_TRACE(_T("resetting audio camera %s no longer active audio recording\n"),pCamera->GetName());
						}
					}
					else
					{
						WK_TRACE(_T("camera not active and last saved picture to young %s\n"),
							pCamera->GetName(),
							pCamera->GetLastSave().GetDateTime());
						// Kamera deaktiviert, gibt es noch
						// Archive für diese Kamera ?
						CArchivInfo* pMDOld = aiOld.GetArchivInfo(CSecID(SECID_GROUP_ARCHIVE,wMDNr));
						if (pMDOld != NULL)
						{
							// es gibt noch ein Archiv MD
							WK_TRACE(_T("still old MD archive %s\n"),pMDOld->GetName());
							bHadMD = TRUE;
						}
						CArchivInfo* pAlarmOld = aiOld.GetArchivInfo(CSecID(SECID_GROUP_ARCHIVE,wAlarmNr));
						if (pAlarmOld != NULL)
						{
							// es gibt noch ein Archiv Alarm
							WK_TRACE(_T("still old AL archive %s\n"),pAlarmOld->GetName());
							bHadAlarm = TRUE;
						}
					}
				}

				if (   pCamera->IsActive()
					|| bHadMD
					|| bHadAlarm)
				{
					// camera hat videosignal oder Bilder noch nicht löschen!
					// camera nr;
					if (   pCamera->GetMode() == COutput::OM_UVV
						|| bHadAlarm)
					{
						// create Archives
						pAlarm = aiNew.AddArchivInfo();
						pAlarm->SetTyp(ALARM_ARCHIV);
						if (  pCamera->GetGroup()->IsTasha()
							||pCamera->GetGroup()->IsQ())
						{
							pAlarm->SetSizeMB(dwAlarmOneMPEG4);
						}
						else
						{
							pAlarm->SetSizeMB(dwAlarmOneJPEG);
						}
						pAlarm->SetArchivNr(wAlarmNr);
						pAlarm->SetName(pCamera->GetName());
						pCamera->SetAlarmNr(pAlarm->GetArchivNr());
						WK_TRACE(_T("<%s> ALARM %04x %d MD\n"),pCamera->GetName(),wAlarmNr,pAlarm->GetSizeMB());
						
						pPreAlarm = aiNew.AddArchivInfo();
						pPreAlarm->SetTyp(SICHERUNGS_RING_ARCHIV);
						pPreAlarm->SetSafeRingFor(pAlarm->GetArchivNr());
						if (   pCamera->GetGroup()->IsTasha()
							|| pCamera->GetGroup()->IsQ())
						{
							pPreAlarm->SetSizeMB(dwPreAlarmOneMPEG4);
						}
						else
						{
							pPreAlarm->SetSizeMB(dwPreAlarmOneJPEG);
						}
						pPreAlarm->SetArchivNr(wPreAlarmNr);
						pPreAlarm->SetName(CString(_T("Pre "))+pCamera->GetName());
						pCamera->SetPreAlarmNr(pPreAlarm->GetArchivNr());
						WK_TRACE(_T("<%s> PRE ALARM %04x %d MB\n"),
							pCamera->GetName(),wPreAlarmNr,pPreAlarm->GetSizeMB());
					}
					else if (   pCamera->GetMode() == COutput::OM_MD
							 || bHadMD)
					{
						pMD = aiNew.AddArchivInfo();
						pMD->SetName(pCamera->GetName());
						pMD->SetArchivNr(wMDNr);
						pMD->SetTyp(RING_ARCHIV);

						DWORD dwMDOne = 0;
//						int b = 1;
						DWORD dw3;


						dw3 = dwMDAll / 3;
						// fester Anteil 33%
						dwMDOne = dw3 / iMDCams;

						if (m_Settings.GetAlarmList())
						{
							dwMDOne -= 15;
						}

						pMD->SetSizeMB(dwMDOne);
						pMD->SetDynamicSize(TRUE);
						pCamera->SetMDAlarmNr(pMD->GetArchivNr());

						WK_TRACE(_T("<%s> RING %04x %d MB\n"),
							pCamera->GetName(),wMDNr,dwMDOne);

						if (m_Settings.GetAlarmList())
						{
							pAlarmList = aiNew.AddArchivInfo();
							pAlarmList->SetName(pCamera->GetName());
							pAlarmList->SetArchivNr(wAlarmListNr);
							pAlarmList->SetTyp(ALARM_LIST_ARCHIV);
							pAlarmList->SetSizeMB(15); // TODO
							pAlarmList->SetSafeRingFor(pMD->GetArchivNr());
							pCamera->SetAlarmListNr(pAlarmList->GetArchivNr());
							WK_TRACE(_T("<%s> ALARM LIST %04x %d MB\n"),
								pCamera->GetName(),wAlarmListNr,pAlarmList->GetSizeMB());
						}
					}
					else
					{
						WK_TRACE(_T("unknown mode for cam %s\n"),pCamera->GetName());
					}
				}
				else
				{
					WK_TRACE(_T("<%s> will get no archive\n"),pCamera->GetName());
				}
			}
		}
	}

	// jetzt noch altes und neues Array abgleichen!
	for (i=0;i<aiOld.GetSize();i++)
	{
		CArchivInfo* pOldArchive = aiOld.GetAtFast(i);
		if (pOldArchive)
		{
			CArchivInfo* pNewArchive = aiNew.GetArchivInfo(pOldArchive->GetID());
			if (pNewArchive == NULL)
			{
				// altes Archiv vorhanden, neues nicht?
				
				// 1. Wechsel von UVV Kassen zu MD oder umgekehrt?
				BOOL bChangeMode = FALSE;

				BYTE bOldCameraNr = LOBYTE(pOldArchive->GetID().GetSubID());
				for (int j=0;j<aiNew.GetSize();j++)
				{
					BYTE bNewCameraNr = LOBYTE(aiNew.GetAtFast(j)->GetID().GetSubID());
					if (bNewCameraNr == bOldCameraNr)
					{
						// alles OK nur Wechsel von UVV Kassen zu MD oder umgekehrt 
						bChangeMode = TRUE;
						break;
					}
				}

				// 2. LastSaveOld?
				BOOL bLastSaveOld = FALSE;
				CCamera* pCamera = m_Outputs.GetCameraByArchiveNr(pOldArchive->GetArchivNr());
				if (   pCamera
					&& pCamera->IsLastSaveOld())
				{
					bLastSaveOld = TRUE;
				}
				
				// Karte ausgefallen?
				if (  (   bChangeMode
					   || bLastSaveOld)
					&& bDeleteAllowed
					)
				{
					// OK es darf gelöscht werden!
					WK_TRACE(_T("DELETING archive %08x,%s\n"),pOldArchive->GetID().GetID(),pOldArchive->GetName());
				}
				else
				{
					CArchivInfo* pNewArchiveInfo = new CArchivInfo(*pOldArchive);
					if (pNewArchiveInfo->GetName().IsEmpty())
					{
						CString sName;
						sName.Format(_T("%d"),LOBYTE(pNewArchiveInfo->GetID().GetSubID()));
						pNewArchiveInfo->SetName(sName);
						WK_TRACE(_T("setting empty archive name to %s\n"),sName);
					}
					aiNew.Add(pNewArchiveInfo);
				}
			}
		}
	}

	aiNew.Save(wkp);

	if (   bAudioReset
		&& WK_IS_WINDOW(m_pMainWnd))
	{
			m_pMainWnd->PostMessage(WM_COMMAND,ID_AUDIO_RESET);
	}
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVProcessApp::ResetStorage()
{
	BOOL bRet = TRUE;
	if (WK_IS_RUNNING(WK_APP_NAME_DB_SERVER))
	{
		((CFrameWnd*)m_pMainWnd)->SetMessageText(_T("resetting storage"));
		WK_TRACE(_T("resetting storage\n"));
		CConnectionRecord c;
		c.SetDestinationHost(WK_APP_NAME_DB_SERVER);
		c.SetTimeout(5*1000);
		if (CIPCServerControlClientSide::FetchServerReset(c)) 
		{
			bRet = TRUE;
			WK_TRACE(_T("reset storage\n"));
		}
		else
		{
			bRet = FALSE;
			WK_TRACE(_T("error resetting storage\n"));
		}
		((CFrameWnd*)m_pMainWnd)->SetMessageText(_T("reset storage"));
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::CheckReset()
{
	if (m_bMustResetProcess)
	{
		if (!m_Outputs.IsScanning())
		{
			m_pMainWnd->PostMessage(WM_COMMAND,ID_FILE_RESET);
			m_bMustResetProcess = FALSE;
		}

	}
	else if (m_bMustInitDatabase)
	{
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
		m_Outputs.Save(wkp);
		if (InitStorage(TRUE))
		{
			m_bMustInitDatabase = FALSE;
			m_bMustResetDatabase = !ResetStorage();
		}
	}
	else if (m_bMustResetDatabase)
	{
		m_bMustResetDatabase = !ResetStorage();
		TRACE(_T("m_bMustResetDatabase=%d\n"),m_bMustResetDatabase);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDVProcessApp::OnIdle(LONG lCount) 
{
	if (m_InputConnectionRecords.GetCount())
	{
		OnInputConnectionRecord();
	}
	if (m_OutputConnectionRecords.GetCount())
	{
		OnOutputConnectionRecord();
	}
	if (m_AudioConnectionRecords.GetCount())
	{
		OnAudioConnectionRecord();
	}
	
	ConnectToStorage();

	if (m_CallThread.GetState() == CCallThread::ACS_PREPARED)
	{
		if (m_CallThread.GetInputClient())
		{
			if (m_CallThread.GetInputClient()->GetCIPC()==NULL)
			{
				m_CallThread.GetInputClient()->Connect(*m_CallThread.GetConnectionRecord(),TRUE);
			}
		}
		if (m_CallThread.GetOutputClient())
		{
			if (m_CallThread.GetOutputClient()->GetCIPC()==NULL)
			{
				m_CallThread.GetOutputClient()->Connect(*m_CallThread.GetConnectionRecord(),TRUE,TRUE);
			}
		}
	}
	else if (m_CallThread.GetState() == CCallThread::ACS_FAILED)
	{
		if (   m_CallThread.GetInputClient()
			&& m_CallThread.GetInputClient()->GetCIPC()
			&& m_CallThread.GetInputClient()->GetCIPC()->GetIPCState() == CIPC_STATE_WRITE_CREATED)
		{
			m_CallThread.GetInputClient()->GetCIPC()->DelayedDelete();
			m_InputClients.ClientDisconnected(m_CallThread.GetInputClient()->GetID());
		}
		if (   m_CallThread.GetOutputClient()
			&& m_CallThread.GetOutputClient()->GetCIPC()
			&& m_CallThread.GetOutputClient()->GetCIPC()->GetIPCState() == CIPC_STATE_WRITE_CREATED)
		{
			m_CallThread.GetOutputClient()->GetCIPC()->DelayedDelete();
			m_OutputClients.ClientDisconnected(m_CallThread.GetOutputClient()->GetID());
		}
	}

	m_InputClients.OnIdle();
	m_OutputClients.OnIdle();
	m_AudioClients.OnIdle();

	return CWinApp::OnIdle(lCount);
}
/////////////////////////////////////////////////////////////////////////////
int CDVProcessApp::ExitInstance() 
{
	WK_TRACE(_T("EXIT ...\n"));

	WK_DELETE(m_pCIPCServerControlProcess);
	Sleep(100);
	m_InputClients.SafeDeleteAll();
	Sleep(100);
	m_OutputClients.SafeDeleteAll();
	Sleep(100);

	m_Inputs.ClearActive();
	WriteSettings(TRUE);

	ShutDown();

	WK_DELETE(m_pCIPCDatabaseProcess);


	WK_TRACE(_T("EXIT ... SUCCESS\n"));

	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));

	WK_DELETE(m_pDebugger);
	Sleep(10);
	CloseDebugger();

	m_Timers.DeleteAll();
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnFileReset() 
{
	Reset();
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnAudioReset() 
{
	ResetAudio();
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::AddInputConnectionRecord(const CConnectionRecord& c)
{
	m_InputConnectionRecords.SafeAddTail(new CConnectionRecord(c));
	AfxGetMainWnd()->PostMessage(WM_USER,WPARAM_INPUT_CONNECTION_RECORD);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnInputConnectionRecord()
{
	CConnectionRecord* pCR = m_InputConnectionRecords.SafeGetAndRemoveHead();
	if (pCR == NULL)
	{
		return;
	}
	CString sPIN;
	CString sSerial;
	BOOL bConnect = TRUE;

	if (m_InputClients.GetNrOfLicensed() >= 5)
	{
		WK_TRACE_ERROR(_T("max number of clients reached\n"));
		m_pCIPCServerControlProcess->DoIndicateError(0, 
					SECID_NO_ID, CIPC_ERROR_UNABLE_TO_CONNECT, 4);
		bConnect = FALSE;
	}
	else
	{
		if (pCR->GetFieldValue(CRF_SERIAL,sSerial))
		{
			if (CRSA::IsValidSerial(sSerial))
			{
				if (m_InputClients.IsConnected(sSerial,pCR->GetSourceHost()))
				{
					WK_TRACE_ERROR(_T("Serial Nr %s already connected\n"),sSerial);
					m_pCIPCServerControlProcess->DoIndicateError(0, 
								SECID_NO_ID, CIPC_ERROR_INVALID_SERIAL, 0);
					bConnect = FALSE;
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("invalid Serial Nr %s\n"),sSerial);
				m_pCIPCServerControlProcess->DoIndicateError(0, 
							SECID_NO_ID, CIPC_ERROR_INVALID_SERIAL, 1);
				bConnect = FALSE;
			}
		}
	}

	if (bConnect)
	{
		if (!pCR->GetFieldValue(CSD_PIN,sPIN))
		{
			sPIN = pCR->GetPassword();
		}

		TRACE(_T("%s, %s, %s\n"), sPIN, GetSuperPIN(), GetOperatorPIN());
		if (   sPIN == GetSuperPIN()
			|| sPIN == GetOperatorPIN())
		{
			WK_TRACE(_T("PIN correct\n"));
			CInputClient* pClient = m_InputClients.AddInputClient();

			if (pClient->Connect(*pCR,sPIN == GetSuperPIN()))
			{
				CString shmName = pClient->GetCIPC()->GetShmName();
#ifdef _UNICODE
				m_pCIPCServerControlProcess->DoConfirmInputConnection(pClient->GetID(),shmName,_T(""),SCO_IS_DV, MAKELONG(pClient->GetCIPC()->GetCodePage(), 0));
#else
				m_pCIPCServerControlProcess->DoConfirmInputConnection(pClient->GetID(),shmName,_T(""),SCO_IS_DV);
#endif
			}
			else
			{
				m_pCIPCServerControlProcess->DoIndicateError(0, 
							SECID_NO_ID, CIPC_ERROR_INVALID_PASSWORD, 0);
			}
		}
		else
		{
			WK_TRACE(_T("PIN invalid\n"));
			m_pCIPCServerControlProcess->DoIndicateError(0, 
						SECID_NO_ID, CIPC_ERROR_INVALID_PASSWORD, 0);
		}
	}

	WK_DELETE(pCR);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::AddOutputConnectionRecord(const CConnectionRecord& c)
{
	m_OutputConnectionRecords.SafeAddTail(new CConnectionRecord(c));
	AfxGetMainWnd()->PostMessage(WM_USER,WPARAM_OUTPUT_CONNECTION_RECORD);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnOutputConnectionRecord()
{
	CConnectionRecord* pCR = m_OutputConnectionRecords.SafeGetAndRemoveHead();
	if (pCR == NULL)
	{
		return;
	}
	CString sPIN;
	CString sSerial;
	BOOL bConnect = TRUE;

	if (m_OutputClients.GetNrOfLicensed() >= 5)
	{
		WK_TRACE_ERROR(_T("max number of clients reached\n"));
		m_pCIPCServerControlProcess->DoIndicateError(0, 
					SECID_NO_ID, CIPC_ERROR_UNABLE_TO_CONNECT, 4);
		bConnect = FALSE;
	}
	else
	{
		if (pCR->GetFieldValue(CRF_SERIAL,sSerial))
		{
			if (CRSA::IsValidSerial(sSerial))
			{
				if (m_OutputClients.IsConnected(sSerial,pCR->GetSourceHost()))
				{
					WK_TRACE_ERROR(_T("Serial Nr %s already connected\n"),sSerial);
					m_pCIPCServerControlProcess->DoIndicateError(0, 
								SECID_NO_ID, CIPC_ERROR_INVALID_SERIAL, 0);
					bConnect = FALSE;
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("invalid Serial Nr %s\n"),sSerial);
				m_pCIPCServerControlProcess->DoIndicateError(0, 
							SECID_NO_ID, CIPC_ERROR_INVALID_SERIAL, 1);
				bConnect = FALSE;
			}
		}
	}

	if (bConnect)
	{
		if (!pCR->GetFieldValue(CSD_PIN,sPIN))
		{
			sPIN = pCR->GetPassword();
		}

		if (   sPIN == GetSuperPIN()
			|| sPIN == GetOperatorPIN())
		{
			COutputClient* pClient = m_OutputClients.AddOutputClient();

			if (pClient->Connect(*pCR,sPIN == GetSuperPIN()))
			{
				CString shmName = pClient->GetCIPC()->GetShmName();
#ifdef _UNICODE
				m_pCIPCServerControlProcess->DoConfirmOutputConnection(pClient->GetID(),shmName,_T(""),pClient->GetOptions(), MAKELONG(pClient->GetCIPC()->GetCodePage(), 0));
#else
				m_pCIPCServerControlProcess->DoConfirmOutputConnection(pClient->GetID(),shmName,_T(""),pClient->GetOptions());
#endif
			}
			else
			{
				m_pCIPCServerControlProcess->DoIndicateError(0, 
							SECID_NO_ID, CIPC_ERROR_INVALID_PASSWORD, 0);
			}
		}
		else
		{
			m_pCIPCServerControlProcess->DoIndicateError(0, 
						SECID_NO_ID, CIPC_ERROR_INVALID_PASSWORD, 0);
		}
	}

	WK_DELETE(pCR);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::AddAudioConnectionRecord(const CConnectionRecord& c)
{
	m_AudioConnectionRecords.SafeAddTail(new CConnectionRecord(c));
	AfxGetMainWnd()->PostMessage(WM_USER,WPARAM_AUDIO_CONNECTION_RECORD);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnAudioConnectionRecord()
{
	CConnectionRecord* pCR = m_AudioConnectionRecords.SafeGetAndRemoveHead();
	if (pCR == NULL)
	{
		return;
	}
	CString sPIN;
	CString sSerial;
	BOOL bConnect = TRUE;

	if (m_AudioClients.GetNrOfLicensed() >= 5)
	{
		WK_TRACE_ERROR(_T("max number of clients reached\n"));
		m_pCIPCServerControlProcess->DoIndicateError(0, 
					SECID_NO_ID, CIPC_ERROR_UNABLE_TO_CONNECT, 4);
		bConnect = FALSE;
	}
	else
	{
		if (pCR->GetFieldValue(CRF_SERIAL,sSerial))
		{
			if (CRSA::IsValidSerial(sSerial))
			{
				if (m_AudioClients.IsConnected(sSerial,pCR->GetSourceHost()))
				{
					WK_TRACE_ERROR(_T("Serial Nr %s already connected\n"),sSerial);
					m_pCIPCServerControlProcess->DoIndicateError(0, 
								SECID_NO_ID, CIPC_ERROR_INVALID_SERIAL, 0);
					bConnect = FALSE;
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("invalid Serial Nr %s\n"),sSerial);
				m_pCIPCServerControlProcess->DoIndicateError(0, 
							SECID_NO_ID, CIPC_ERROR_INVALID_SERIAL, 1);
				bConnect = FALSE;
			}
		}
	}

	if (bConnect)
	{
		if (!pCR->GetFieldValue(CSD_PIN,sPIN))
		{
			sPIN = pCR->GetPassword();
		}

		if (   sPIN == GetSuperPIN()
			|| sPIN == GetOperatorPIN())
		{
			CAudioClient* pClient = m_AudioClients.AddAudioClient();

			if (pClient->Connect(*pCR,sPIN == GetSuperPIN()))
			{
				CString shmName = pClient->GetCIPC()->GetShmName();
#ifdef _UNICODE
				m_pCIPCServerControlProcess->DoConfirmAudioConnection(pClient->GetID(),shmName,_T(""),0, MAKELONG(pClient->GetCIPC()->GetCodePage(), 0));
#else
				m_pCIPCServerControlProcess->DoConfirmAudioConnection(pClient->GetID(),shmName,_T(""),0);
#endif
			}
			else
			{
				m_pCIPCServerControlProcess->DoIndicateError(0, 
							SECID_NO_ID, CIPC_ERROR_INVALID_PASSWORD, 0);
			}
		}
		else
		{
			m_pCIPCServerControlProcess->DoIndicateError(0, 
						SECID_NO_ID, CIPC_ERROR_INVALID_PASSWORD, 0);
		}
	}

	WK_DELETE(pCR);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnAudioClientDisconnected(CAudioClient* pAudioClient)
{
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnOutputClientDisconnected(COutputClient* pOutputClient)
{
	m_CallThread.OutputDisconnected(pOutputClient);
	m_Outputs.RemoveClient(pOutputClient->GetID());
	if (pOutputClient->IsLowBandwidth())
	{
		m_EncoderThread.RemoveLowBandwidthClient(pOutputClient->GetID());
	}
	else
	{
		m_EncoderThread.RemoveHighBandwidthClient(pOutputClient->GetID());
	}

}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnDateCheck() 
{
	// check for a new day
	CSystemTime t;
	t.GetLocalTime();

	CheckReset();

	if (t.GetMinute() != m_currentTime.GetMinute())
	{
		// it's a new minute
		OnNewMinute(t);
	}
	if (t.GetHour() != m_currentTime.GetHour())
	{
		// it's a new hour
		OnNewHour();
	}
	if (t.GetDay() != m_currentTime.GetDay())
	{
		// it's a new day
		OnNewDay(t.GetDayOfWeek() == 0);
	}
	m_currentTime = t;
	if (!IsResetting())
	{
		int iOutstandingPicts = m_Inputs.GetOutstandingPicts();
		OutStandingPictsChanged(iOutstandingPicts);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::RecalcTimer(CSystemTime& st)
{
	CTime t = st.GetCTime();
	/*
	TRACE(_T("RecalcTimer %02d.%02d.%04d %02d:%02d:%02d\n"),
		t.GetDay(),t.GetMonth(),t.GetYear(),
		t.GetHour(),t.GetMinute(),t.GetSecond());
		*/
	m_Timers.UpdateTimers(t);
	m_Outputs.Lock();
	for (int i=0;i<m_Outputs.GetSize();i++)
	{
		COutputGroup* pOutputGroup = m_Outputs.GetAtFast(i);
		if (pOutputGroup->HasCameras())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)pOutputGroup;
			for (int j=0;j<pCameraGroup->GetSize();j++)
			{
				COutput* pOutput = pCameraGroup->GetAtFast(j);
				CSecID idTimer(SECID_GROUP_TIMER,pOutput->GetClientID().GetSubID());
				m_Timers.Lock();
				const CSecTimer* pTimer = m_Timers.GetTimerByID(idTimer);
				if (pTimer)
				{
					BOOL bIncluded = pTimer->IsIncluded(t);
					pOutput->SetTimerActive(bIncluded);
				}
				m_Timers.Unlock();
			}
		}
	}
	m_Outputs.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnNewMinute(CSystemTime& st)
{
	BOOL bImageTimeOut = FALSE;
	for (int i=0;i<m_Outputs.GetSize();i++)
	{
		COutputGroup* pOutputGroup = m_Outputs.GetAtFast(i);
		if (pOutputGroup->HasCameras())
		{
			CCameraGroup* pCameraGroup = (CCameraGroup*)pOutputGroup;
			if (pCameraGroup->IsImageTimeout())
			{
				bImageTimeOut = TRUE;
				break;
			}
		}
	}
	if (bImageTimeOut)
	{
		WK_TRACE_ERROR(_T("reset by camera timeout, no images\n"));
		m_pMainWnd->PostMessage(WM_COMMAND,ID_FILE_RESET);
	}
	else
	{
		m_Outputs.RescanCameras();
		RecalcTimer(st);
		((CFrameWnd*)m_pMainWnd)->RedrawWindow();

		CheckReset();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnNewHour()
{
	m_Outputs.SaveReferenceImage();

/*
	CWK_Profile wkp;
	CString sPath;

	GetModuleFileName(m_hInstance,sPath.GetBuffer(_MAX_PATH),_MAX_PATH);
	sPath.ReleaseBuffer();
	sPath = sPath.Left(sPath.ReverseFind('\\'));
	CString sFilepath = sPath + _T("\\dvrt.reg");
	CString sFilepath1 = sPath + _T("\\dvrt1.reg");
	CString sFilepath2 = sPath + _T("\\dvrt2.reg");

	MoveFileEx(sFilepath1,sFilepath2,MOVEFILE_REPLACE_EXISTING);
	MoveFileEx(sFilepath,sFilepath1,MOVEFILE_REPLACE_EXISTING);

	wkp.SaveAs(sFilepath,NULL);
	*/
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnNewDay(BOOL bNewWeek)
{
	WK_TRACE(_T("OnNewDay\n"));
	
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	m_Outputs.Save(wkp);

	BOOL bDeleteAllowed = TRUE;
	for (int i=0;i<m_Outputs.GetSize() && bDeleteAllowed ;i++)
	{
		COutputGroup* pOutputGroup = m_Outputs.GetAtFast(i);
		if (   pOutputGroup
			&& pOutputGroup->HasCameras())
		{
			bDeleteAllowed = bDeleteAllowed && (pOutputGroup->GetIPCState() == CIPC_STATE_CONNECTED);
		}
	}
	if (InitStorage(bDeleteAllowed))
	{
		m_bMustResetDatabase = !ResetStorage();
	}
	else
	{
		CWK_RunTimeError e(WAI_SECURITY_SERVER,REL_ERROR,RTE_CONFIGURATION,
							_T("Cannot initialize database. System cannot store."));
		e.Send();
	}

	m_Outputs.ResetFramesPerDay();
	if (bNewWeek)
	{
		m_Outputs.ResetFramesPerWeek();
	}
	m_Outputs.Save(wkp);

	//send collected RTEs to all connected stations
	m_OutputClients.Lock();
	for (int i=0;i<m_OutputClients.GetSize();i++)
	{
		COutputClient* pC = m_OutputClients.GetAtFast(i);
		if (pC)
		{
			CIPCOutputDVClient* pCIPCOutputDVClient = pC->GetCIPC();
			if (pCIPCOutputDVClient)
			{
				pCIPCOutputDVClient->CheckNagScreen();
			}
		}
	}
	m_OutputClients.Unlock();
	
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnTestNewDay() 
{
	OnNewDay(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnTestNewWeek() 
{
	OnNewDay(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnUpdateStoragePane(CCmdUI* pCmdUI) 
{
	CString s;
    if (   m_pCIPCDatabaseProcess
		&& m_pCIPCDatabaseProcess->GetIPCState()==CIPC_STATE_CONNECTED)
	{
		s.LoadString(IDS_STORAGE_CONNECTED);
	}
	else
	{
		s.LoadString(ID_PANE_STORAGE);
	}
	pCmdUI->SetText(s);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnUpdateICPane(CCmdUI* pCmdUI)
{
	CString s;
	s.Format(_T("IC %d"),m_InputClients.GetSize());
	pCmdUI->SetText(s);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnUpdateOCPane(CCmdUI* pCmdUI)
{
	CString s;
	s.Format(_T("OC %d"),m_OutputClients.GetSize());
	pCmdUI->SetText(s);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnUpdateACPane(CCmdUI* pCmdUI)
{
	CString s;
	s.Format(_T("AC %d"),m_AudioClients.GetSize());
	pCmdUI->SetText(s);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnUpdateIGPane(CCmdUI* pCmdUI)
{
	CString s;
	s.Format(_T("IG %d"),m_Inputs.GetSize());
	pCmdUI->SetText(s);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnUpdateOGPane(CCmdUI* pCmdUI)
{
	CString s;
	s.Format(_T("OG %d"),m_Outputs.GetSize());
	pCmdUI->SetText(s);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnUpdateAGPane(CCmdUI* pCmdUI)
{
	CString s;
	s.Format(_T("AG %d"),m_Audios.GetSize());
	pCmdUI->SetText(s);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnUpdateLEDPane(CCmdUI* pCmdUI)
{
	if (!IsResetting())
	{
		CString s;
		int iOutstandingPicts = m_Inputs.GetOutstandingPicts();
		s.Format(_T("%s %04d"),m_bStoreLED ? _T("ON") : _T("OFF"),iOutstandingPicts);
		pCmdUI->SetText(s);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnUpdateAudioPane(CCmdUI* pCmdUI)
{
	if (!IsResetting())
	{
		CString s;

		if (m_Audios.GetSize()>0)
		{
			CIPCAudioServer* pCIPCAudioServer = m_Audios.GetAtFast(0);

			if (pCIPCAudioServer->GetIPCState() == CIPC_STATE_CONNECTED)
			{
				DWORD dwRF = pCIPCAudioServer->GetRecordingFlags();

				if (dwRF & AUDIO_RECORDING_LEVEL)
				{
					s += 'L';
				}
				if (dwRF & AUDIO_RECORDING_ACTIVITY)
				{
					s += 'A';
				}
				if (dwRF & AUDIO_RECORDING_EXTERN)
				{
					s += 'E';
				}
			}

			int iNumActiveClients = 0;
			for (int i=0; i<m_AudioClients.GetSize(); i++)
			{
				CAudioClient*pAudio = m_AudioClients.GetAtFast(i);
				if (   pAudio->GetCIPC()
					&& pAudio->GetCIPC()->GetIPCState() == CIPC_STATE_CONNECTED
					&& pAudio->GetCIPC()->IsAudioInActive())
				{
					iNumActiveClients++;
				}
			}
			CString b;
			b.Format(_T("%d"),iNumActiveClients);
			s += b;
		}

		pCmdUI->SetText(s);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::SetOutputName(DWORD dwClientID, COutput* pOutput, const CString& sName)
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	pOutput->SetName(sName);
	// save name immediately
	pOutput->GetGroup()->Save(wkp);
	// inform other clients
	m_OutputClients.UpdateAllClients(dwClientID);

	if (pOutput->GetGroup()->HasCameras())
	{
		CameraNameChanged((CCamera*)pOutput);
	}
	else
	{
		RelayNameChanged((CRelay*)pOutput);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::RelayNameChanged(CRelay* pOutput)
{
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::CameraNameChanged(CCamera* pOutput)
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	CString sName = pOutput->GetName();
	// change input name
	if (pOutput->GetExternDetector())
	{
		pOutput->GetExternDetector()->SetName(sName);
		pOutput->GetExternDetector()->GetGroup()->Save(wkp,FALSE);
	}
	if (pOutput->GetMotionDetector())
	{
		pOutput->GetMotionDetector()->SetName(sName);
		pOutput->GetMotionDetector()->GetGroup()->Save(wkp,FALSE);
	}
	m_InputClients.UpdateAllClients();

	// inform DVStorage with the new name
	if (m_pCIPCDatabaseProcess)
	{
		CSecID a(SECID_GROUP_ARCHIVE,0);
		if (pOutput->GetAlarmNr() != (WORD)-1)
		{
			a.SetSubID(pOutput->GetAlarmNr());
			m_pCIPCDatabaseProcess->DoRequestSetValue(a,CSD_NAME,sName,0);
			a.SetSubID(pOutput->GetPreAlarmNr());
			m_pCIPCDatabaseProcess->DoRequestSetValue(a,CSD_NAME,CString(_T("Pre "))+sName,0);
		}
		if (pOutput->GetMDAlarmNr() != (WORD)-1)
		{
			a.SetSubID(pOutput->GetMDAlarmNr());
			m_pCIPCDatabaseProcess->DoRequestSetValue(a,CSD_NAME,sName,0);
		}
		if (pOutput->GetAlarmListNr() != (WORD)-1)
		{
			a.SetSubID(pOutput->GetAlarmListNr());
			m_pCIPCDatabaseProcess->DoRequestSetValue(a,CSD_NAME,sName,0);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnTestNewMinute() 
{
	CSystemTime st;
	st.GetLocalTime();
	RecalcTimer(st);
	((CFrameWnd*)m_pMainWnd)->RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnTestNewHour() 
{
	OnNewHour();
	((CFrameWnd*)m_pMainWnd)->RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::SetVideoOut(int iCard, CSecID camID)
{
	m_VOutThread.AddActivity(camID,iCard);
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::ExportHKLMSoftware()
{
	CString sPath;

	GetModuleFileName(m_hInstance,sPath.GetBuffer(_MAX_PATH),_MAX_PATH);
	sPath.ReleaseBuffer();
	sPath = sPath.Left(sPath.ReverseFind('\\'));
	CString sFilepath = sPath + _T("\\software.reg");

	if (!DoesFileExist(sFilepath))
	{
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T(""),_T(""));
		wkp.SaveAs(sFilepath,NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::ActivateAlarmOutput()
{
	m_dwLastAlarmOutputTime = WK_GetTickCount();
	if (m_bAlarmClosed)
	{
		SwitchAlarmOutput(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::SwitchAlarmOutput(BOOL bClose)
{
	CRelayGroup* pRelayGroup = NULL;
	for (int i=0; m_Outputs.GetSize(); i++)
	{
		COutputGroup* pGroup = m_Outputs.GetAtFast(i);
		if (!pGroup->HasCameras())
		{
			pRelayGroup = (CRelayGroup*)pGroup;
			break;
		}
	}
	if (   pRelayGroup
		&& pRelayGroup->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		int iNr = 3;
		if (   pRelayGroup->IsSavic()
			|| pRelayGroup->IsTasha())
		{
			iNr = 0;
		}
		CSecID relayID(pRelayGroup->GetID(),(WORD)(iNr));
		WK_TRACE(_T("Switch relay %08lx to %d\n"),relayID.GetID(),bClose);
		pRelayGroup->DoRequestSetRelay(relayID,bClose);
		m_bAlarmClosed = bClose;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::CheckAlarmOutput(DWORD dwWKTick)
{
	if (   m_bAlarmOutput
		&& !m_bAlarmClosed)
	{
		if (GetTimeSpan(m_dwLastAlarmOutputTime,dwWKTick)>1000)
		{
			SwitchAlarmOutput(TRUE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnSoftwareAlarm(CSecID id, BOOL bAlarm,WORD wX, WORD wY)
{
	TRACE(_T("Software alarm %08lx ,%d (%d,%d)\n"),id.GetID(),bAlarm,wX,wY);
	CInput* pInput = m_Inputs.GetInputByUnitID(id);
	if (pInput)
	{
		if (wX > 0 && wY >0)
		{
			pInput->SetActive(bAlarm,wX,wY);
		}
		else
		{
			pInput->SetActive(bAlarm);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnOutputChanged(BOOL bPTZChanged)
{
	TRACE(_T("CDVProcessApp::OnOutputChanged()\n"));
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));
	m_Outputs.Save(wkp);
	if (bPTZChanged)
	{
		CInput* pPTZInput = m_Inputs.GetPTZInput();
		if (pPTZInput)
		{
			CInputGroup* pGroup = pPTZInput->GetGroup();
			if (pGroup)
			{
				pGroup->DoRequestReset(SECID_NO_GROUPID);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OutStandingPictsChanged(int iOutstandingPicts)
{
	if (m_nNrOfJacobs+m_nNrOfSavics+m_nNrOfTashas>0)
	{
		CCameraGroup* pGroup1 = m_Outputs.GetCameraGroup(0);
		CCameraGroup* pGroup2 = m_Outputs.GetCameraGroup(1);

		if (   iOutstandingPicts>0
			&& m_dwFramesToRecord<200)
		{
			if (!m_bStoreLED)
			{
				if (pGroup1 && pGroup1->GetIPCState() == CIPC_STATE_CONNECTED)
					pGroup1->DoRequestSetValue(CSecID(pGroup1->GetID(),0),CSD_LED2_ON,_T(""),0);
				if (pGroup2 && pGroup2->GetIPCState() == CIPC_STATE_CONNECTED)
					pGroup2->DoRequestSetValue(CSecID(pGroup2->GetID(),0),CSD_LED2_ON,_T(""),0);
				m_bStoreLED = TRUE;
			}
		}
		else
		{
			if (m_bStoreLED)
			{
				if (pGroup1 && pGroup1->GetIPCState() == CIPC_STATE_CONNECTED)
					pGroup1->DoRequestSetValue(CSecID(pGroup1->GetID(),0),CSD_LED2_OFF,_T(""),0);
				if (pGroup2 && pGroup2->GetIPCState() == CIPC_STATE_CONNECTED)
					pGroup2->DoRequestSetValue(CSecID(pGroup2->GetID(),0),CSD_LED2_OFF,_T(""),0);
				m_bStoreLED = FALSE;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CDVProcessApp::GetLocalHostName() const
{
	CString sHostName;
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,_T("DVRT\\DV"),_T(""));

	sHostName = wkp.GetString(_T("Hosts"),_T("OwnID"),_T(""));

	return sHostName;
}
/////////////////////////////////////////////////////////////////////////////
void CDVProcessApp::OnTimer()
{
	if (IsUpAndRunning())
	{
		// check my threads
		BOOL bExit = FALSE;
		BOOL bReset = FALSE;

		for (int i=0;i<m_Outputs.GetSize();i++) 
		{
			COutputGroup* pOutputServer = m_Outputs.GetAtFast(i);
			if (pOutputServer)
			{
				CString sAppname = pOutputServer->GetAppname(); 
		//		TRACE(_T("checking %s %s\n"),sAppname,pOutputServer->GetShmName());
				if (pOutputServer->GetIPCState() != CIPC_STATE_CONNECTED)
				{
					TRACE(_T("%s not connected\n"),sAppname);
					if (WK_IS_RUNNING(sAppname))
					{
						// running and not connected
						WK_TRACE_ERROR(_T("%s,%s unit not connected but running RESET\n"),sAppname,pOutputServer->GetShmName());
						bReset = TRUE;
					}
					else
					{
						TRACE(_T("%s not connected, not running\n"),sAppname);
					}
				}
				if (pOutputServer->HasCameras())
				{
					CCameraGroup* pCameraGroup = (CCameraGroup*)pOutputServer;
					if (pCameraGroup->IsImageTimeout())
					{
						WK_TRACE_ERROR(_T("image timeout %s\n"),pCameraGroup->GetShmName());
						bReset = TRUE;
					}
					if (!pCameraGroup->IsRequestThreadRunning())
					{
						WK_TRACE_ERROR(_T("request thread does not answer %s\n"),pCameraGroup->GetShmName());
						if (m_pDebugger)
						{
							m_pDebugger->Trace(0);
						}
						bExit = TRUE;
					}
				}
			}
		}
#ifdef _DEBUG
		bExit = FALSE;
#endif
		if (bExit)
		{
			m_pMainWnd->DestroyWindow();
			WK_DELETE(m_pDebugger);
			CloseDebugger();
			Sleep(100);
			ExitProcess(0);
		}
		else if (bReset)
		{
			SetReset(TRUE,FALSE,FALSE);
			CheckReset();
		}
		else
		{
			//				TRACE(_T("checking threads OK\n"));
		}
	}
}

