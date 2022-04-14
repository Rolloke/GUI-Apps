// SMSUnit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SMSUnit.h"

#include "MainFrm.h"

#include "IPCServerControlSMS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szSection[]   = _T("SMSUnit");
static TCHAR BASED_CODE szOwnNumber[]	= _T("OwnNumber");

/////////////////////////////////////////////////////////////////////////////
// CSMSUnitApp

BEGIN_MESSAGE_MAP(CSMSUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CSMSUnitApp)
	ON_COMMAND(ID_FILE_RESET, OnFileReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSMSUnitApp construction

CSMSUnitApp::CSMSUnitApp()
{
	InitVideteLocaleLanguage();
	CWK_Profile wkp;
	m_pCIPCServerControlSMS = NULL;
	m_sD1 = _T("01712521001");
	m_sD2 = _T("01722278010");
	m_sEPlus =_T("01771167");
	m_sPrefix = _T("");
	m_sOwnNumber = _T("0");

	
	m_bResetting = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSMSUnitApp object

CSMSUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSMSUnitApp initialization

BOOL CSMSUnitApp::InitInstance()
{
	if (WK_ALONE(WK_APPNAME_SMSUNIT)==FALSE)
	{
		TRACE(_T("second instance\n"));
		return FALSE;
	}

	CString sTitle;
	sTitle = COemGuiApi::GetApplicationName(WAI_SMS_UNIT);
	m_pszAppName = _tcsdup(sTitle);    // human readable title

	InitDebugger(_T("smsunit.log"),WAI_SMS_UNIT);
	CWK_RunTimeError::SetDefaultID(WAI_SMS_UNIT);

	CWK_Profile wkp;
	BOOL bTraceMilli = (BOOL)wkp.GetInt(_T("Log"), _T("LogMilliSeconds"), 0);
	if (bTraceMilli)
	{
		SET_WK_STAT_TICK_COUNT(GetTickCount());
	}

	// Dongle vorhanden?
	CWK_Dongle dongle(WAI_SMS_UNIT); 
	if (dongle.IsExeOkay()==FALSE)
	{
		return FALSE;
	}


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
#endif // _AFXDLL
#else
	SetLanguageParameters(0, 0);
#endif // _MFC_VER < 0x0700


	RegisterWindowClass();
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources

	pFrame->LoadFrame(IDR_MAINFRAME,WS_OVERLAPPEDWINDOW , NULL,	NULL);

	// do not touch
	if (m_nCmdShow!=0) // do not touch if already hidden
	{	
		m_nCmdShow = SW_HIDE;
	}

	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_HIDE);
#ifdef _DEBUG
	pFrame->ShowWindow(SW_SHOW);
#endif
	pFrame->UpdateWindow();

	WK_STAT_LOG(_T("Reset"),1,_T("ServerActive"));

	LoadSettings();

	m_pCIPCServerControlSMS = new CIPCServerControlSMS();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CSMSUnitApp::ExitInstance() 
{
	WK_STAT_LOG(_T("Reset"),0,_T("ServerActive"));

	WK_DELETE(m_pCIPCServerControlSMS);

	CloseDebugger();
	
	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
void CSMSUnitApp::LoadSettings()
{
	CWK_Profile wkp;

	m_sD1 = wkp.GetString(szSection,_T("D1"),m_sD1);
	m_sD2 = wkp.GetString(szSection,_T("D2"),m_sD2);
	m_sEPlus  = wkp.GetString(szSection,_T("EPlus"),m_sEPlus);
	m_sPrefix = wkp.GetString(szSection,_T("Prefix"),m_sPrefix);
	m_sOwnNumber = wkp.GetString(szSection, szOwnNumber, m_sOwnNumber);
}
/////////////////////////////////////////////////////////////////////////////
void CSMSUnitApp::RegisterWindowClass()
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
    wndclass.lpszClassName = WK_APPNAME_SMSUNIT;

    AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSMSUnitApp::OnIdle(LONG lCount) 
{
	BOOL bRet = CWinApp::OnIdle(lCount);

	CMainFrame* pMF = (CMainFrame*)m_pMainWnd;
	if (WK_IS_WINDOW(pMF))
	{
		pMF->PollConnection();

		if (pMF->CanSend())
		{
			if (m_aConnectionRecords.GetCount())
			{
				OnConnectionRecord();
			}
		}
		else
		{
			Sleep(0);
			bRet = TRUE;
		}
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CSMSUnitApp::OnConnectionRecord()
{
	CConnectionRecord* pCR = m_aConnectionRecords.SafeGetAndRemoveHead();

	WK_TRACE(_T("OnConnectionRecord <%s> -> <%s>\n"),
		LPCTSTR(pCR->GetSourceHost()),
		LPCTSTR(pCR->GetDestinationHost()));

	CMainFrame* pMF = (CMainFrame*)m_pMainWnd;
	if (WK_IS_WINDOW(pMF))
	{
		CString sMessage,sHost;
		pCR->GetFieldValue(NM_MESSAGE,sMessage);

		if (!sMessage.IsEmpty())
		{
			sHost = pCR->GetDestinationHost();
			if (0==sHost.Find(_T("sms:")) || 0==sHost.Find(_T("SMS:")))
			{
				sHost = sHost.Mid(4);
				sHost = StringOnlyCharsInSet(sHost,_T("0123456789"));
				DWORD dwTimeOut = pCR->GetTimeout();
				if (pMF->SendSMS(sHost,sMessage,dwTimeOut))
				{
					// OK
					// confirm later from CMainFrame::OnPager
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot send <%s> to <%s>\n"), LPCTSTR(sMessage), LPCTSTR(sHost));
					m_pCIPCServerControlSMS->DoIndicateError(_T(""));
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("connection record with wrong host\n"));
				m_pCIPCServerControlSMS->DoIndicateError(_T(""));
			}
		}
		else
		{
			// error
			WK_TRACE_ERROR(_T("connection record with empty message\n"));
			m_pCIPCServerControlSMS->DoIndicateError(_T(""));
		}
	}
	else
	{
		// error
		WK_TRACE_ERROR(_T("connection record without mainframe\n"));
		m_pCIPCServerControlSMS->DoIndicateError(_T(""));
	}

	WK_DELETE(pCR);
}
///////////////////////////////////////////////////////////////////////
void CSMSUnitApp::OnFileReset() 
{
	if (!m_bResetting)
	{
		m_bResetting = TRUE;
		
		LoadSettings();
		
		m_bResetting = FALSE;
	}
}			  
/////////////////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0710
int CSMSUnitApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
{
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