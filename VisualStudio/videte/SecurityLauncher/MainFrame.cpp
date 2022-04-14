// MainFrame.cpp : implementation file
//

#include "stdafx.h"
#include "SecurityLauncher.h"
#include "MainFrame.h"

#include "oemgui\oemgui.h"
#include ".\mainframe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_BUTTON 1000


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

CMainFrame::CMainFrame()
{
	m_bAskAtExit = TRUE;
	m_bOnQueryEndSession = FALSE;
	m_bOnWmPowerBroadcast= FALSE;

	CWK_Profile wkp;
	m_sVersion = wkp.GetString(_T("Version"),_T("Number"),_T(""));

	CreateEx(0,//WS_EX_TOOLWINDOW|WS_EX_DLGMODALFRAME,
			 WK_APP_NAME_LAUNCHER,
			 theApp.GetName(),
			 WS_SYSMENU|WS_OVERLAPPED|WS_THICKFRAME/*|WS_VISIBLE*/,
			 100,100,100,100,NULL,NULL,NULL);
}

CMainFrame::~CMainFrame()
{
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ClearAskAtExit()
{
	m_bAskAtExit = FALSE;
}
//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_QUERYENDSESSION()
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_SELFCHECK, OnConfirmSelfcheck)
	ON_MESSAGE(WM_POWERBROADCAST, OnWmPowerBroadcast)
	ON_MESSAGE(LAUNCHER_APPLICATION, OnMsgApplication)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_KEYBOARD_CHANGED, OnKeyboardChanged)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::PostNcDestroy()
{
	delete this;
}
//////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CFrameWnd::PreCreateWindow(cs);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	// Nur erforderlich, wenn Icon in der Systemtaskleiste
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
	KillTimer(1);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent)
{
	theApp.Actualize20s();

	CFrameWnd::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Icon ins Systemtray
	NOTIFYICONDATA tnd;

	CString sTip = theApp.GetVersion();

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon		= theApp.LoadIcon(IDR_MAINFRAME);

	_tcsncpy(tnd.szTip, sTip, sizeof(tnd.szTip) / sizeof(tnd.szTip[0]));
	Shell_NotifyIcon(NIM_ADD, &tnd);

//	SetTimer(1,1000,NULL);

	return 0;
}
//////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam==IDC_BUTTON)
	{
		ContextMenu();
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

	return CFrameWnd::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_LBUTTONDBLCLK:
//		ShowWindow(SW_RESTORE); //SW_SHOWNORMAL);
		break;
	case WM_RBUTTONDOWN:
		{
			ContextMenu();
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ContextMenu() 
{
	CSkinMenu menu,popup;
	CString sString;

	CPoint pt;
	GetCursorPos(&pt);

	menu.CreatePopupMenu();
	popup.CreatePopupMenu();

	InternalProductCode ipc = IPC_MIN_CODE;
	BOOL bReceiver = FALSE;
	if (theApp.m_pDongle)
	{
		ipc = theApp.m_pDongle->GetProductCode();
		bReceiver = theApp.m_pDongle->IsReceiver();
	}

	if (theApp.IsNormalMode())
	{
		// idip Client
		menu.AppendMenu(MF_STRING,ID_VISION,COemGuiApi::GetApplicationName(WAI_IDIP_CLIENT));

		// Systemverwaltung
		menu.AppendMenu(MF_STRING,ID_SUPERVISOR,COemGuiApi::GetApplicationName(WAI_SUPERVISOR));

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

			// TODO! RKE: do not insert in special cases
			// TKR nur in DEBUG lassen, da in release die Anzeige abhängig vom angemeldeten
			//Benutzer ist. Nur der SuperVisor darf Softwareupdate und Freischaltung durchführen
#ifdef _DEBUG
			if (!bReceiver)
			{
				sString.LoadString(ID_SOFTWARE_UPDATE);
				popup.AppendMenu(MF_STRING,ID_SOFTWARE_UPDATE,sString);

				sString.LoadString(ID_EXTENSION_CODE);
				popup.AppendMenu(MF_STRING,ID_EXTENSION_CODE,sString);
			}
#endif
		}
		// Erweitert-Eintrag mit Untermenue
		sString.LoadString(IDS_EXTENDED);
		menu.AppendMenu(MF_POPUP, (UINT)popup.m_hMenu, sString);
		menu.AppendMenu(MF_SEPARATOR);
	}
	// Beenden/Herunterfahren
	sString.LoadString(IDS_EXIT);
	menu.AppendMenu(MF_STRING, ID_APP_EXIT, sString);

	menu.ConvertMenu(TRUE);
	menu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
	menu.ConvertMenu(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	WK_ApplicationId AppId = (WK_ApplicationId)wParam;
	theApp.OnConfirmSelfcheck(AppId);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnQueryEndSession() 
{
	if (theApp.ShallReboot())
	{
		Beep(300, 100);
		Beep(200, 100);
		Beep(100, 100);
	}
	else
	{
		Beep(100, 100);
		Beep(200, 100);
		Beep(300, 100);
	}
	if (!CFrameWnd::OnQueryEndSession())
		return FALSE;

	m_bOnQueryEndSession = TRUE;

	PostMessage(WM_CLOSE, 0, 0);

	return theApp.ShallReboot();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnWmPowerBroadcast(WPARAM wParam, LPARAM lParam)
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
LRESULT CMainFrame::OnMsgApplication(WPARAM wParam,LPARAM lParam)
{
	WK_ApplicationId wai = (WK_ApplicationId)wParam;
	WK_TRACE(_T("OnMsgApplication %d, %s\n"), wai, NameOfEnum(wai));
	return theApp.OnApplication(wai, lParam);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnClose()
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
				CFrameWnd::OnClose();
			}
		}
		else
		{
			// Windows soll beendet werden, bzw. in den Suspendmode versetzt werden.
			if (m_bOnQueryEndSession || m_bOnWmPowerBroadcast)
				theApp.SetShutdownAtExitShell(TRUE);
			CFrameWnd::OnClose();
		}
	}
	m_bOnQueryEndSession = FALSE;
	m_bOnWmPowerBroadcast= FALSE;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnKeyboardChanged(WPARAM wParam, LPARAM lParam)
{
	if (wParam)
	{
		CKBInput*pActive = (CKBInput*)wParam;
		CString s = pActive->GetShortName();
		CString sTip = theApp.GetVersion();
		if (!s.IsEmpty())
		{
			sTip = sTip + _T(" [") + s + _T("]");
		}

		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.uFlags = NIF_TIP;
		nid.hWnd   = m_hWnd;
		nid.hIcon  = NULL;
		nid.uID    = 0;
		_tcsncpy(nid.szTip, LPCTSTR(sTip), sizeof(nid.szTip) / sizeof(nid.szTip[0]));
		Shell_NotifyIcon(NIM_MODIFY, &nid);

		CWnd *pWnd = GetFocus();
		while (pWnd)
		{
			if (pWnd == this)
			{
				break;
			}
			if (pWnd->IsKindOf(RUNTIME_CLASS(CDialog)))
			{
				pWnd->PostMessage(WM_KEYBOARD_CHANGED, wParam, lParam);
				break;
			}
			pWnd = pWnd->GetParent();
		}
	}
	return 0;
}

void CMainFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT pDIS)
{
	if (pDIS->CtlType == ODT_MENU)
	{
		CSkinMenu::OnDrawItem(nIDCtl, pDIS);
		return;
	}

	CFrameWnd::OnDrawItem(nIDCtl, pDIS);
}

void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT pMIS)
{
	if (pMIS->CtlType == ODT_MENU)
	{
		CSkinMenu::OnMeasureItem(nIDCtl, pMIS);
		return;
	}

	CFrameWnd::OnMeasureItem(nIDCtl, pMIS);
}
