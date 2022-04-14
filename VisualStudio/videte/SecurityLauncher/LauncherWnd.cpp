// LauncherWnd.cpp : implementation file
//

#include "stdafx.h"
#include "securitylauncher.h"
#include "LauncherWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_BUTTON 1000

// static TCHAR BASED_CODE szSection[] = _T("SecurityLauncher");
/////////////////////////////////////////////////////////////////////////////
// CLauncherWnd
CLauncherWnd::CLauncherWnd()
{
	m_bAskAtExit = TRUE;
	m_bOnQueryEndSession = FALSE;
	m_bOnWmPowerBroadcast= FALSE;

	CWK_Profile wkp;
	m_sVersion = wkp.GetString(_T("Version"),_T("Number"),_T(""));
	
	CSystemTime st;
	st.GetLocalTime();
	SetTime(st);

	int x;
	x = GetSystemMetrics(SM_CXSCREEN);

	m_SmallFont.CreateFont(-8,0,0,0,0/*FW_BOLD*/,0,0,0,0,0,0,0,0,_T("MS Sans Serif"));
	m_szMinTracking = CSize(0, 0);

	CreateEx(WS_EX_TOOLWINDOW|WS_EX_DLGMODALFRAME,
			 WK_APP_NAME_LAUNCHER,
			 theApp.GetName(),
		     WS_POPUP|WS_VISIBLE,
			 0,0,x,28,NULL,NULL,NULL);
}
/////////////////////////////////////////////////////////////////////////////
CLauncherWnd::~CLauncherWnd()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLauncherWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	BOOL bRet = CAppBar::PreCreateWindow(cs);
	
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::ClearAskAtExit()
{
	m_bAskAtExit = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::SetTime(const CSystemTime& st)
{
	m_sDateTime.LoadString(IDS_SO+st.wDayOfWeek);
	m_sDateTime += _T(" ") + st.GetTime();
	m_sDateTime += _T(" ") + st.GetDate();

	if (WK_IS_WINDOW(this))
	{
		CRect rect;
		GetClientRect(rect);
		rect.left = rect.right - 400;
		InvalidateRect(rect);
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLauncherWnd, CAppBar)
	//{{AFX_MSG_MAP(CLauncherWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_QUERYENDSESSION()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_BUTTON, OnButton)
	ON_COMMAND(ID_AOT, OnAOT)
	ON_UPDATE_COMMAND_UI(ID_AOT, OnUpdateAOT)
	ON_COMMAND(ID_AIH, OnAIH)
	ON_UPDATE_COMMAND_UI(ID_AIH, OnUpdateAIH)
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SELFCHECK, OnConfirmSelfcheck)
	ON_MESSAGE(WM_POWERBROADCAST, OnWmPowerBroadcast)
	ON_MESSAGE(LAUNCHER_APPLICATION, OnMsgApplication)
	END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// Do not call CWnd::OnPaint() for painting messages

	CRect rectClient, rectButton;
	CBrush brush;
	int iOldBkMode;
	CFont* pOldFont;
	
	GetClientRect(rectClient);
	m_Button.GetWindowRect(rectButton);
	ScreenToClient(rectButton);
	
	brush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	
	dc.FillRect(rectClient, &brush);

	pOldFont = dc.SelectObject(&m_SmallFont);
	iOldBkMode = dc.SetBkMode(TRANSPARENT);

	// x ist X-Coordinate of start text, y dito.
	int x,y;

	// Draw App title and OS version, left aligned
	CString sText = theApp.GetVersion() + _T("   ") + theApp.GetOsVersion();
	CSize sizeText = dc.GetOutputTextExtent(sText);
	x = rectButton.right + rectButton.Height();
	y = (rectClient.Height() - sizeText.cy) / 2;
	dc.TextOut(x, y, sText);

	int iTextFrameDistance = 5;

	// Draw Date and Time, right aligned
	sizeText = dc.GetOutputTextExtent(m_sDateTime);
	x = rectClient.right - sizeText.cx - iTextFrameDistance - 1; // 1 for frame
	y = (rectClient.Height() - sizeText.cy) / 2;
	dc.TextOut(x, y, m_sDateTime);
	// Draw 3-D-Frame
	CRect rectDate(x - iTextFrameDistance,
				   rectClient.top + 1,
				   x + sizeText.cx + iTextFrameDistance,
				   rectClient.bottom - 1);
	dc.Draw3dRect(rectDate, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));

	// Draw AlarmPercent, right aligned from Date/Time
	CString sFormat, sOutput;
	InternalProductCode ipc = theApp.m_pDongle->GetProductCode();
	if (   (ipc != IPC_TOBS_RECEIVER)
		&& (ipc != IPC_DTS_RECEIVER)
		&& (ipc != IPC_TOBS_TO3K)
		)
	{
		sFormat.LoadString(IDS_ALARM_PERCENT);
		sOutput.Format(sFormat, theApp.GetAlarmPercent());
		sizeText = dc.GetOutputTextExtent(sOutput);
		x = rectDate.left - sizeText.cx - iTextFrameDistance;
		y = (rectClient.Height() - sizeText.cy) / 2;
		dc.TextOut(x, y, sOutput);
	}
	// Draw 3-D-Frame
	// Draw always, if no text, there will be no frame visible, _T("rect") needed for next text
	CRect rectAlarm(x - iTextFrameDistance,
					rectClient.top + 1,
					x + sizeText.cx + iTextFrameDistance,
					rectClient.bottom - 1);
	dc.Draw3dRect(rectAlarm, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));

	// Draw B-Channels, right aligned from AlarmPercent
	if (   theApp.m_pDongle->RunISDNUnit()
		|| theApp.m_pDongle->RunPTUnit()
		|| theApp.m_pDongle->RunSMSUnit()
		|| theApp.m_pDongle->RunTOBSUnit())
	{
		sFormat.LoadString(IDS_ISDN_BCHANNEL);
		sOutput.Format(sFormat, theApp.GetISDNBChannel());
		sizeText = dc.GetOutputTextExtent(sOutput);
		x = rectAlarm.left - sizeText.cx - iTextFrameDistance;
		y = (rectClient.Height() - sizeText.cy) / 2;
		dc.TextOut(x, y, sOutput);

		CRect rectBChannels(x - iTextFrameDistance,
							rectClient.top + 1,
							x + sizeText.cx + iTextFrameDistance,
							rectClient.bottom - 1);
		dc.Draw3dRect(rectBChannels, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
	}


	dc.SelectObject(pOldFont);
	dc.SetBkMode(iOldBkMode);

	brush.DeleteObject();
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::HideFloatAdornments(BOOL fHide) 
{
   if (fHide) 
   {
	   	ModifyStyle(0, 0, SWP_DRAWFRAME);
   } 
   else 
   {
   		ModifyStyle(0, 0, SWP_DRAWFRAME);
   }
}
/////////////////////////////////////////////////////////////////////////////
int CLauncherWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAppBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CRect rect,br;

	GetWindowRect(rect);

	br = rect;
	br.left += 1;
	br.top  += 1;
	br.bottom = br.top + 20;
	br.right = br.left + 70;

	CString sString;
	sString.LoadString(IDS_SYSTEM);
	m_Button.Create(sString,WS_CHILD|WS_VISIBLE|BS_OWNERDRAW,br,this,IDC_BUTTON);
	m_Button.SetFont(&m_SmallFont);

	// Set the CAppBar class's behavior flags
	m_fdwFlags =  ABF_ALLOWTOPBOTTOM 
				| ABF_MIMICTASKBARAUTOHIDE 
				| ABF_MIMICTASKBARALWAYSONTOP;
	
	
	// Width has no limits, height sizes in client-area-height increments
	m_szSizeInc.cx = 1;
	m_szSizeInc.cy = rect.Height(); 
	
	// The appbar has a minimum client-area size that is determined by the 
	// client area set in the dialog box template.
	m_szMinTracking.cx = rect.Width();
	m_szMinTracking.cy = rect.Height();

	CWK_Profile wkp;
	// Setup default state data for the AppBar
	APPBARSTATE abs;
	abs.m_cbSize = sizeof(abs);
	abs.m_uState = ABE_TOP;
	if (theApp.AmIShell())
	{
		abs.m_fAutohide = FALSE;
		abs.m_fAlwaysOnTop = TRUE;
	}
	else
	{
		abs.m_fAutohide = wkp.GetInt(theApp.GetSection(),_T("AIH"),FALSE);
		abs.m_fAlwaysOnTop = wkp.GetInt(theApp.GetSection(),_T("AOT"),TRUE);
	}
	
	// Set the default floating location of the appbar
	abs.m_rcFloat = rect;
	abs.m_auDimsDock[ABE_LEFT]   = rect.Width();
	abs.m_auDimsDock[ABE_TOP]    = rect.Height();
	abs.m_auDimsDock[ABE_RIGHT]  = rect.Width();
	abs.m_auDimsDock[ABE_BOTTOM] = rect.Height();
	
// TODO gf CoolMenuManager funzt nicht mit MFC 7.1
#if _MFC_VER < 0x0070
	m_CoolMenuManager.m_bAutoAccel = FALSE;
	m_CoolMenuManager.Install(this);
	m_CoolMenuManager.LoadToolbar(IDR_MENU);
#endif

	if (theApp.AmIShell())
	{
		int x,y;
		x = GetSystemMetrics(SM_CXSCREEN);
		y = GetSystemMetrics(SM_CYSCREEN);
		CRect wr(0,29,x,y);
		SystemParametersInfo(SPI_SETWORKAREA,0,&wr,0);
	}

	SetTimer(2,1000,NULL);

	// Set the initial state of the appbar.
	SetState(abs);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::OnDestroy() 
{
	CWK_Profile wkp;
	// Setup default state data for the AppBar
	APPBARSTATE abs;
	abs.m_cbSize = sizeof(abs);
	GetState(&abs);
	if (!theApp.AmIShell())
	{
		wkp.WriteInt(theApp.GetSection(),_T("AIH"),abs.m_fAutohide);
		wkp.WriteInt(theApp.GetSection(),_T("AOT"),abs.m_fAlwaysOnTop);
	}

	KillTimer(2);

	if (theApp.AmIShell())
	{
		SystemParametersInfo(SPI_SETWORKAREA,0,NULL,0);
	}

	CAppBar::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::OnButton() 
{
	CMenu menu,popup;
	CRect rect;
	CString sString;

	GetWindowRect(rect);

	menu.CreatePopupMenu();
	popup.CreatePopupMenu();

	InternalProductCode ipc = theApp.m_pDongle->GetProductCode();

	if (theApp.IsNormalMode())
	{
		// vision / funktionskontrolle
		if (theApp.m_pDongle->RunExplorer())
		{
			menu.AppendMenu(MF_STRING,ID_VISION,COemGuiApi::GetApplicationName(WAI_EXPLORER));
		}

		// recherche / bildsuchprogramm
		if (theApp.m_pDongle->RunDatabaseClient())
		{
			menu.AppendMenu(MF_STRING,ID_DATABASE,COemGuiApi::GetApplicationName(WAI_DATABASE_CLIENT));
		}

		// Systemverwaltung
		if (theApp.m_pDongle->RunSupervisor())
		{
			menu.AppendMenu(MF_STRING,ID_SUPERVISOR,COemGuiApi::GetApplicationName(WAI_SUPERVISOR));
		}

		// ProductVersion, ProductView, LogView
		// Backup/CDR_Writer
		if (   (ipc != IPC_TOBS_RECEIVER)
			&& (ipc != IPC_DTS_RECEIVER)
			)
		{
			popup.AppendMenu(MF_STRING,ID_VERSION,COemGuiApi::GetApplicationName(WAI_PRODUCT_VERSION));
			popup.AppendMenu(MF_STRING,ID_PRODUCT_VIEW,COemGuiApi::GetApplicationName(WAI_PRODUCT_VIEW));
			popup.AppendMenu(MF_STRING,ID_LOG_VIEW,COemGuiApi::GetApplicationName(WAI_LOG_VIEW));
		}

		// Euro-Umstellung
		if (theApp.m_pDongle->RunSDIUnit() || theApp.m_pDongle->RunGAUnit())
		{
			CWK_Profile wkp;
			BOOL bAlreadyDone = (BOOL)wkp.GetInt(_T("Common"),_T("EuroChangeDone"),0);
			if (FALSE == bAlreadyDone) {
				CSystemTime timeActual;
				timeActual.GetLocalTime();
				// Euro-Umstellung am 01.01.2002
				// 1 Monat davor und 6 Monate danach moeglich
				CSystemTime timeChangeMin(1, 12, 2001, 0, 0, 0);
				CSystemTime timeChangeMax(1, 7, 2002, 0, 0, 0);
				if (timeChangeMin < timeActual
					&& timeActual < timeChangeMax)
				{
					sString.LoadString(IDS_EURO_CHANGE);
					popup.AppendMenu(MF_STRING,ID_EURO_CHANGE,sString);
				}
			}
		}

		if (!theApp.AmIShell())
		{
			// Allways on top
			sString.LoadString(ID_AOT);
			popup.AppendMenu(MF_STRING,ID_AOT,sString);
		}
		// Automatisch im Hintergrund
		sString.LoadString(ID_AIH);
		popup.AppendMenu(MF_STRING,ID_AIH,sString);

		if (   (ipc != IPC_TOBS_RECEIVER)
			&& (ipc != IPC_DTS_RECEIVER)
			)
		{
			// Runtime errors
			sString.LoadString(IDS_RTE);
			popup.AppendMenu(MF_STRING,ID_ERROR,sString);
			// Zeitumstellung
			sString.LoadString(IDS_DATETIME);
			popup.AppendMenu(MF_STRING,ID_DATETIME,sString);

			if (theApp.UseUpdateHandler())
			{
				sString = COemGuiApi::GetApplicationName(WAI_UPDATE_HANDLER);
				popup.AppendMenu(MF_STRING,ID_UPDATE_HANDLER,sString);
			}

			// shell and windows explorer stuff
			if (theApp.AmIShell())
			{
				sString.LoadString(IDS_EXPLORER);
				popup.AppendMenu(MF_STRING,ID_EXPLORER,sString);
				sString.LoadString(IDS_EXPLORER_SHELL);
				popup.AppendMenu(MF_STRING,ID_EXPLORER_SHELL,sString);
			}
			else
			{
				CString sString2;
				sString.LoadString(IDS_LAUNCHER_SHELL);
				sString2.Format(sString,theApp.m_pszAppName);
				popup.AppendMenu(MF_STRING,ID_LAUNCHER_SHELL,sString2);
			}
		}
		// Erweitert-Eintrag mit Untermenue
		sString.LoadString(IDS_EXTENDED);
		menu.AppendMenu(MF_POPUP, (UINT)popup.m_hMenu, sString);
		menu.AppendMenu(MF_SEPARATOR);
	}
	// Beenden/Herunterfahren
	sString.LoadString(IDS_EXIT);
	menu.AppendMenu(MF_STRING, ID_APP_EXIT, sString);

	menu.TrackPopupMenu(TPM_LEFTALIGN, rect.left, rect.bottom, this);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLauncherWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam==IDC_BUTTON)
	{
		OnButton();
		return TRUE;
	}

	if (wParam==ID_UPDATE)
	{
		theApp.OnUpdate(lParam);
		return TRUE;
	}
	
	if (wParam==ID_ERROR_ARRIVED)
	{
		theApp.OnNewError();
		return TRUE;
	}
	if (wParam==ID_QUITE_EXIT)
	{
		theApp.QuiteExit();
		return TRUE;
	}
	
	return CAppBar::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::OnClose() 
{
	if (theApp.GetMode()==LM_BACKUP)
	{
		CApplication* pApp = theApp.m_Apps.GetApplication(WAI_DATABASE_CLIENT);
		if (pApp && pApp->IsRunning())
		{
			// do nothing
			AfxMessageBox(IDP_DBBACKUP_STILL_RUNNING);
		}
	}
	else
	{
		if (m_bAskAtExit)
		{
			if (theApp.Login())
			{
				// Windows soll beendet werden, bzw. in den Suspendmode versetzt werden.
				if (m_bOnQueryEndSession || m_bOnWmPowerBroadcast)
					theApp.SetShutdownAtExitShell(TRUE);
				theApp.TerminateAll();
				CAppBar::OnClose();
			}
		}
		else
		{
			// Windows soll beendet werden, bzw. in den Suspendmode versetzt werden.
			if (m_bOnQueryEndSession || m_bOnWmPowerBroadcast)
				theApp.SetShutdownAtExitShell(TRUE);
			CAppBar::OnClose();
		}
	}
	m_bOnQueryEndSession = FALSE;
	m_bOnWmPowerBroadcast= FALSE;
}

/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 2)
	{
		CSystemTime st;
		st.GetLocalTime();
		SetTime(st);
	}
	else
	{
		CAppBar::OnTimer(nIDEvent); // for automatic hidden appbar
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLauncherWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CLauncherWnd::OnConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	WK_ApplicationId AppId = (WK_ApplicationId)wParam;
	theApp.OnConfirmSelfcheck(AppId);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// Tell our derived class that there is a proposed state change
void CLauncherWnd::OnAppBarStateChange (BOOL fProposed, UINT uStateProposed) {

   // Hide the window adorments when docked.
   HideFloatAdornments((uStateProposed == ABE_FLOAT) ? FALSE : TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
   // Get the minimum size of the window assuming it has no client area.
   // This is the width/height of the window that must always be present
   CRect rcBorder(0, 0, 0, 0);
   AdjustWindowRectEx(&rcBorder, GetStyle(), FALSE, GetExStyle());

   if (GetState() == ABE_FLOAT) {
      lpMMI->ptMinTrackSize.x = m_szMinTracking.cx + rcBorder.Width();
      lpMMI->ptMinTrackSize.y = m_szMinTracking.cy + rcBorder.Height();
   }

   // The appbar can't be more than half the width or height
   // of the screen when docked or when floating
   lpMMI->ptMaxTrackSize.x = GetSystemMetrics(SM_CXSCREEN);
   lpMMI->ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN);
   if (!IsEdgeTopOrBottom(GetState()))
      lpMMI->ptMaxTrackSize.x /= 2;

   if (!IsEdgeLeftOrRight(GetState()))
      lpMMI->ptMaxTrackSize.y /= 2;
	
	CAppBar::OnGetMinMaxInfo(lpMMI);
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::OnAOT()
{
	// Get the current state and change the always-on-top setting only.
	APPBARSTATE abs;
	abs.m_cbSize = sizeof(abs);
	GetState(&abs);
	abs.m_fAlwaysOnTop = !IsBarAlwaysOnTop();
	SetState(abs);
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::OnUpdateAOT(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsBarAlwaysOnTop());
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::OnAIH()
{
	// Get the current state and change the always-on-top setting only.
	APPBARSTATE abs;
	abs.m_cbSize = sizeof(abs);
	GetState(&abs);
	abs.m_fAutohide = !IsBarAutohide();
	SetState(abs);
	if (!IsBarAlwaysOnTop())
	{
		OnAOT();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLauncherWnd::OnUpdateAIH(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsBarAutohide());
}

/////////////////////////////////////////////////////////////////////////////
BOOL CLauncherWnd::OnQueryEndSession() 
{
	Beep(100, 100);
	Beep(200, 100);
	Beep(300, 100);

	if (!CAppBar::OnQueryEndSession())
		return FALSE;

	m_bOnQueryEndSession = TRUE;

	PostMessage(WM_CLOSE, 0, 0);
	
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CLauncherWnd::OnWmPowerBroadcast(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0;

	switch (wParam)
	{
		case PBT_APMBATTERYLOW:
			WK_TRACE(_T("Battery power is low\n"));
			break;
		case PBT_APMOEMEVENT:
			WK_TRACE(_T("OEM-defined event occurred.\n"));
			break;
		case PBT_APMPOWERSTATUSCHANGE:
			WK_TRACE(_T("Power status has changed.\n"));
			break;
		case PBT_APMQUERYSUSPEND:
			WK_TRACE(_T("Request for permission to suspend.\n"));
			Beep(100, 100);
			Beep(200, 100);
			Beep(300, 100);
			m_bOnWmPowerBroadcast = TRUE; 
			PostMessage(WM_CLOSE, 0, 0);		
			lResult = BROADCAST_QUERY_DENY;
			break;
		case PBT_APMQUERYSUSPENDFAILED:
			WK_TRACE(_T("Suspension request denied.\n"));
			break;
		case PBT_APMRESUMEAUTOMATIC:
			WK_TRACE(_T("Operation resuming automatically after event.\n"));
			break;
		case PBT_APMRESUMECRITICAL:
			WK_TRACE(_T("Operation resuming after critical suspension.\n"));
			break;
		case PBT_APMRESUMESUSPEND:
			WK_TRACE(_T("Operation resuming after suspension.\n"));	
			break;
		case PBT_APMSUSPEND:
			WK_TRACE(_T("System is suspending operation.\n"));	
			break;
	}
	return lResult;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CLauncherWnd::OnMsgApplication(WPARAM wParam, LPARAM lParam)
{
	WK_ApplicationId wai = (WK_ApplicationId)wParam;
	WK_TRACE(_T("OnMsgApplication(%d, %s)\n"), wai, NameOfEnum(wai));
	return theApp.OnApplication(wai, lParam);
}
//////////////////////////////////////////////////////////////////////////