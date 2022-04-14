// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "IdipClient.h"
#include "Mainfrm.h"

#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewDialogBar.h"
#include "ViewCamera.h"
#include "ViewRelais.h"
#include "ViewAudio.h"
#include "ViewAlarmList.h"
#include "ViewArchive.h"
#include "ViewDlg.h"

#include "WndLive.h"
#include "WndHTML.h"
#include "WndMpeg4.h"

#include "DlgSubstitute.h"
#include "DlgBackup.h"
#include "DlgBarIdipClient.h"

#include "oemgui\DlgLoginNew.h"

#include "CIPCAudioIdipClient.h"
#include "CIPCOutputIdipClientDecoder.h"

#include "wk_template.h"
#include ".\mainfrm.h"

#include <DeviceDetect\DeviceDetect.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
CWK_Average<DWORD> g_dwaIdletimes;
LARGE_INTEGER g_liOld;
#endif
//////////////////////////////////////////////////////////////////////////
// CMainFrame
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATEAPP()
	ON_WM_SETCURSOR()
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_VIEW_FULLSCREEN, OnViewFullscreen)
	ON_COMMAND(ID_FORCE_FULLSCREEN, OnViewFullscreen)
	ON_COMMAND_RANGE(ID_CONNECT_LOW, ID_CONNECT_HIGH, OnConnect)
	ON_COMMAND(ID_DISCONNECT, OnDisconnect)
	ON_COMMAND_RANGE(ID_DISCONNECT_LOW, ID_DISCONNECT_HIGH, OnDisconnect)
	ON_COMMAND_RANGE(ID_VIEW_MONITOR2, ID_VIEW_MONITOR1+MAX_MONITORS, OnViewMonitor)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULLSCREEN, OnUpdateViewFullscreen)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_MONITOR2, ID_VIEW_MONITOR1+MAX_MONITORS, OnUpdateViewMonitor)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
	ON_MESSAGE(WM_USER, OnUser)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_WM_SYSCOMMAND()
	ON_WM_NCPAINT()
	ON_WM_NCACTIVATE()
	ON_WM_NCMOUSEMOVE()
	ON_WM_NCLBUTTONDOWN()
	ON_MESSAGE(WM_NCMOUSELEAVE , OnNcMouseLeave)
	ON_MESSAGE(WM_UPDATE_ALL_VIEWS, OnUpdateAllViews)
	ON_MESSAGE(WM_USER_LOGIN_ALARM, OnUserLoginAlarm)
	ON_MESSAGE(LAUNCHER_APPLICATION, OnLaucherApplication)
	ON_MESSAGE(WM_KEYBOARD_CHANGED, OnKeyboardChanged)
	ON_MESSAGE(WM_RESET_AUTO_LOGOUT, OnResetAutoLogout)
	// Recherche
	ON_COMMAND(ID_SYNC, OnSync)
	ON_UPDATE_COMMAND_UI(ID_SYNC, OnUpdateSync)
	ON_WM_DEVICECHANGE()
	ON_COMMAND(ID_ARCHIV_BACKUP, OnArchivBackup)
	ON_WM_DESTROY()
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_COMMAND(ID_VIEW_OBJECT_VIEW, OnViewObjectView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OBJECT_VIEW, OnUpdateViewObjectView)
	ON_COMMAND(ID_VIEW_OEMDIALOG, OnViewOemdialog)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OEMDIALOG, OnUpdateViewOemdialog)
	ON_COMMAND(ID_VIEW_DIALOGBAR, OnViewDialogbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DIALOGBAR, OnUpdateViewDialogbar)
	ON_UPDATE_COMMAND_UI(IDC_CLOSE_DLG_BAR, OnUpdateCloseDlgBar)
	ON_WM_TIMER()
	ON_COMMAND(ID_VIEW_MENU, OnViewMenu)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MENU, OnUpdateViewMenu)
	ON_WM_SIZING()
	ON_WM_CLOSE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_COMMAND(ID_HOST_CAMERA_MAP, OnHostCameraMap)
	ON_UPDATE_COMMAND_UI(ID_HOST_CAMERA_MAP, OnUpdateHostCameraMap)
	ON_COMMAND(ID_HOST_DATETIME, OnHostDatetime)
	ON_UPDATE_COMMAND_UI(ID_HOST_DATETIME, OnUpdateHostDatetime)
	ON_COMMAND(ID_HOST_SAVE_SEQUENCE, OnHostSaveSequence)
	ON_COMMAND(ID_HOST_LOAD_SEQUENCE, OnHostLoadSequence)
	ON_WM_INITMENU()
	ON_WM_INITMENUPOPUP()
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(WM_ISDN_B_CHANNELS, OnISDNBchannels)
	ON_WM_SETFOCUS()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_EXT, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_CAPS, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_NUM, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCRL, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_REC, OnUpdateDummy)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_KANA, OnUpdateDummy)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_WM_MENUSELECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_DATABASE_TEXT,		//  1 
	ID_DATABASE_PANE,		//  2
	ID_SEQUENCE_TEXT,		//  3
	ID_SEQUENCE_PANE,		//  4
	ID_INDICATOR_EXT,		//  5: created Bubbles
	ID_INDICATOR_CAPS,		//  6: Pictures in Live window thread queue
	ID_INDICATOR_NUM,		//  7: reset state
	ID_INDICATOR_SCRL,		//  8: Process usage
	ID_INDICATOR_OVR,		//  9: Avg Video timeout
	ID_INDICATOR_REC,		// 10: Performance Level
	ID_INDICATOR_KANA,		// 11
};

//////////////////////////////////////////////////////////////////////////
// CMainFrame Erstellung/Zerstörung
CMainFrame::CMainFrame()
{
	m_bSplitterWndCreated = false;
	m_hMenu = NULL;

	m_bActive				= TRUE;
	m_bFullScreen			= false;
	m_bFullScreenChanging	= false;

	m_Rect.SetRectEmpty();
	memset(&m_wp,0,sizeof(m_wp));
	m_wp.length = sizeof(m_wp);
	
	m_pwndSyncCoolBar = NULL;

	m_nStyle = 0;
	m_pSkinMenu = NULL;
	m_pToolBarImages = NULL;
	m_nTimerOneSec = 0;
	m_nTimerCounter = 0;
	
	m_pJoyStick = NULL;
	m_pJoyState	= NULL;
	m_dwJoyPTZspeed = 0;
	m_LastCmd = CCC_INVALID;
	m_nZeroRange = 3;
	m_bControlSpeed = TRUE;
	m_bIsInMenuLoop = FALSE;
	m_nTootipTimer = 0;
	m_hTooltipMenu = NULL;

#ifdef _DEBUG
	g_liOld = theTimer.GetMicroTicks();
	g_dwaIdletimes.SetNoOfAvgValues(10000);

	int nMonitors = m_MonitorInfo.GetNrOfMonitors();
	int nTestMonitors = CIdipClientApp::GetIntValue(_T("TryNoOfMonitors"));
	if (nTestMonitors > nMonitors)
	{
		m_MonitorInfo.TestSplitMonitor(nTestMonitors);
	}
#endif
}
//////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
	WK_DELETE(m_pwndSyncCoolBar);
	WK_DELETE(m_pSkinMenu);
	WK_DELETE(m_pToolBarImages);

	m_ExternalFrames.DeleteAll();
	FreeJoystick();

#ifdef _DEBUG
	int i, nVal = g_dwaIdletimes.GetActualValues();
	CString str;
	WK_TRACE(_T("Average IdleTime:%f µs, stddev:%f, var:%f, values:%d\n"), g_dwaIdletimes.GetAverage(), g_dwaIdletimes.GetStandardDeviation(), g_dwaIdletimes.GetVariance(), nVal);
	CStdioFileU file(_T("IdleTimes.Txt"), CFile::modeCreate|CFile::modeWrite);
	file.SetFileAnsi();
	file.WriteString(_T("IdleTimes\n"));
	file.WriteString(_T("n, µs\n"));
	str.Format(_T("%d\n"), nVal);
	file.WriteString(str);
	for (i=0; i<nVal; i++)
	{
		str.Format(_T("%d, %d\n"), i, g_dwaIdletimes.GetAtFast(i));
		file.WriteString(str);
	}
#endif
}
//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CMainFrame::KillScreenSaverFunc(HWND hwnd, LPARAM lParam)
{
   if(::IsWindowVisible(hwnd))
	{
		WK_TRACE(_T("ForegroundWindow found on Other Desktop\n"));
		::PostMessage(hwnd, WM_CLOSE, 0, 0);
		*((BOOL*)lParam) = FALSE;
	}
   return TRUE;
}
//////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// Toolbar settings
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | /*WS_VISIBLE |*/ CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBARMENU))
	{
		TRACE0("Symbolleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}
	//  Toolbar wird nicht angezeigt
	//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//	EnableDocking(CBRS_ALIGN_ANY);
	//	DockControlBar(&m_wndToolBar);
	ShowControlBar(&m_wndToolBar,TRUE,FALSE); // Workaround to get the size
	ShowControlBar(&m_wndToolBar,FALSE,FALSE);

	DWORD dwSbStyle = WS_CHILD | CBRS_BOTTOM;
	if (theApp.GetIntValue(_T("ShowStatusBar")))
	{
		dwSbStyle |= WS_VISIBLE;
	}

	if (m_wndStatusBar.Create(this, dwSbStyle))
	{
		if (m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
		{
			int i, nSize, nCount = m_wndStatusBar.GetCount();
			UINT nID, nStyle;
			for (i=1; i<nCount; i++)
			{
				m_wndStatusBar.GetPaneInfo(i, nID, nStyle, nSize);
				switch (nID)
				{
					case ID_DATABASE_TEXT: case ID_DATABASE_PANE: case ID_SEQUENCE_TEXT: case ID_SEQUENCE_PANE:
						break;
					default:
						m_wndStatusBar.SetPaneInfo(i, nID, nStyle, 30);
						m_wndStatusBar.SetPaneText(i, _T(""), FALSE);
						break;
				}
			}
		}
		else
		{
			m_wndStatusBar.DestroyWindow();
		}
	}

	// Menu settings
	m_hMenu = ::GetMenu(m_hWnd);
	ShowMenu(TRUE);
	CMenu* pMenu = CMenu::FromHandle(m_hMenu);

	// Customize System Menu
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString str;
// these items are available
//		pSysMenu->RemoveMenu(SC_CLOSE,MF_BYCOMMAND);
//		pSysMenu->RemoveMenu(SC_SIZE,MF_BYCOMMAND);
//		pSysMenu->RemoveMenu(SC_MOVE,MF_BYCOMMAND);
//		pSysMenu->RemoveMenu(SC_MAXIMIZE,MF_BYCOMMAND);
//		pSysMenu->RemoveMenu(SC_MINIMIZE,MF_BYCOMMAND);
//		pSysMenu->RemoveMenu(SC_RESTORE,MF_BYCOMMAND);
//		pSysMenu->RemoveMenu(0,MF_BYPOSITION);
		// remove all, we need only our own
		while (pSysMenu->GetMenuItemCount())
		{
			pSysMenu->RemoveMenu(0,MF_BYPOSITION);
		}
		UINT nID = ID_VIEW_MENU;
		if (pMenu && ((CSkinMenu*)pMenu)->FindMenuString(nID, str))
		{
			pSysMenu->AppendMenu(MF_STRING, ID_VIEW_MENU, str);
		}
	}

	// Customize menu
	if (pMenu)
	{
		// No Audio, no Audio Menu
		if (!theApp.m_pDongle->RunAudioUnit())
		{
			WK_TRACE(_T("deleting audio menu item\n"));
			DeleteSubMenu(pMenu, ID_AU_START_PLAY);		// MAIN_MENU_AUDIO
		}
		// In Read-Only mode no 'vision' menues
		if (theApp.IsReadOnlyModus())
		{
			WK_TRACE(_T("deleting relais menu item\n"));
			DeleteSubMenu(pMenu, ID_RELAIS_OPEN);		// MAIN_MENU_RELAY
			WK_TRACE(_T("deleting camera menu item\n"));
			DeleteSubMenu(pMenu, ID_VIDEO_SAVE_ALL);	// MAIN_MENU_CAMERA
			WK_TRACE(_T("deleting input menu item\n"));
			DeleteSubMenu(pMenu, ID_INPUT_INFORMATION);	// MAIN_MENU_INPUT
		}

		UINT nID, nTemp;
		nID = nTemp = theApp.IsShellMode() ? ID_SHELL : ID_NON_SHELL;
		CString str;
		CMenu* pPopup;
		pPopup = ((CSkinMenu*)pMenu)->FindMenuString(nTemp, str);
		if (pPopup)
		{
			pPopup->RemoveMenu(nID, MF_BYCOMMAND);
		}

		if (!theApp.m_pDongle->RunCDRWriter())
		{
			nID = nTemp = ID_ARCHIV_BACKUP;
			pPopup = ((CSkinMenu*)pMenu)->FindMenuString(nTemp, str);
			if (pPopup)
			{
				pPopup->RemoveMenu(nID, MF_BYCOMMAND);
			}
		}
		if (!theApp.AllowHTMLmaps())
		{
			nID = nTemp = ID_CONNECT_MAP;
			pPopup = ((CSkinMenu*)pMenu)->FindMenuString(nTemp, str);
			if (pPopup)
			{
				pPopup->RemoveMenu(nID, MF_BYCOMMAND);
			}
			nID = nTemp = ID_HOST_CAMERA_MAP;
			pPopup = ((CSkinMenu*)pMenu)->FindMenuString(nTemp, str);
			if (pPopup)
			{
				pPopup->RemoveMenu(nID, MF_BYCOMMAND);
			}
			nID = nTemp = ID_EDIT_CREATE_HTML_HOST_PAGE;
			pPopup = ((CSkinMenu*)pMenu)->FindMenuString(nTemp, str);
			if (pPopup)
			{
				pPopup->RemoveMenu(nID, MF_BYCOMMAND);
				pPopup->RemoveMenu(ID_EDIT_CREATE_HTML_CAMERA_PAGE, MF_BYCOMMAND);
			}
		}
/*
// TODO: RKE: Recherche Menufunktionen checken
#ifdef _DTS
		CMenu* pPopupConnection = pMenu->GetSubMenu(MAIN_MENU_CONNECTION);
		pPopupConnection->DeleteMenu(ID_CONNECT_MAP,MF_BYCOMMAND);
		pPopupConnection->DeleteMenu(ID_SEARCH_AT_START,MF_BYCOMMAND);
		pPopupConnection->DeleteMenu(ID_NEW_LOGIN,MF_BYCOMMAND);
		pPopupConnection->DeleteMenu(3,MF_BYPOSITION);

		CMenu* pPopupEdit = pMenu->GetSubMenu(MAIN_MENU_EDIT);
		pPopupEdit->DeleteMenu(ID_EDIT_STATION,MF_BYCOMMAND);
		pPopupEdit->DeleteMenu(ID_EDIT_COMMENT,MF_BYCOMMAND);

		pPopupArchive->DeleteMenu(ID_ARCHIV_EMPTY,MF_BYCOMMAND);
		pPopupArchive->DeleteMenu(ID_ARCHIV_DELETE,MF_BYCOMMAND);
		pPopupArchive->DeleteMenu(0,MF_BYPOSITION);
#endif
*/		
	}
	
	if (theApp.IsWin2000() || theApp.IsWinXP())
	{
		if (CIdipClientApp::GetIntValue(_T("RegisterHotKey")))		
		{	// register the commands of an extended keyboard
			// these commands are quite usefull
//			RegisterHotKey(VK_VOLUME_DOWN);
//			RegisterHotKey(VK_VOLUME_UP);
//			RegisterHotKey(VK_VOLUME_MUTE);
			// these commands may start the explorer
			RegisterHotKey(VK_BROWSER_BACK);
			RegisterHotKey(VK_BROWSER_FORWARD);
			RegisterHotKey(VK_BROWSER_HOME);
			RegisterHotKey(VK_BROWSER_REFRESH);
			RegisterHotKey(VK_BROWSER_STOP);
			RegisterHotKey(VK_BROWSER_SEARCH);
			RegisterHotKey(VK_BROWSER_FAVORITES);

			// these commands may start the media player
			RegisterHotKey(VK_MEDIA_STOP);
			RegisterHotKey(VK_MEDIA_NEXT_TRACK);
			RegisterHotKey(VK_MEDIA_PREV_TRACK);
			RegisterHotKey(VK_MEDIA_PLAY_PAUSE);

			// these commands may launch other programs
			RegisterHotKey(VK_LAUNCH_MAIL);
			RegisterHotKey(VK_LAUNCH_MEDIA_SELECT);
			RegisterHotKey(VK_LAUNCH_APP1);
			RegisterHotKey(VK_LAUNCH_APP2);
			// this command forces sleep mode of the computer
			RegisterHotKey(VK_SLEEP);
		}
	}

	if (CSkinDialog::UseOEMSkin())
	{
		if (m_hMenu)
		{
			WK_TRACE(_T("skinning menu\n"));
			m_pSkinMenu = new CSkinMenu;
			m_pSkinMenu->Attach(m_hMenu);
			m_pSkinMenu->ConvertMenu(TRUE);
			m_pSkinMenu->SetDrawMenuItemImageFkt(DrawMenuItemImageFkt);
		}
		SendMessage(WM_USER, WM_ACTIVATEAPP);
	}
	m_bSplitterWndCreated = true;

	GetViewDlgBar()->GetDlgInView()->PostMessage(WM_USER, WM_INITDIALOG);
	m_nTimerOneSec = SetTimer(3, 1000, NULL);

	RegisterDeviceDetect(m_hWnd, NULL);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnUser(WPARAM wParam, LPARAM lParam)
{
	WORD wLow = LOWORD(wParam);
	switch (wLow)
	{
		case WM_CLOSE:
		{
			if (lParam == (LPARAM)m_wndSplitterHorz.GetViewDlgBar()->GetDlgInView())
			{
				m_wndSplitterHorz.GetViewDlgBar()->ShowDialog(HIWORD(wParam));
				m_wndSplitterVert.UpdateSizes();
			}
			else if (lParam == (LPARAM)m_wndSplitterObjectViews.GetViewOEMDlg()->GetDlgInView())
			{
				m_wndSplitterObjectViews.ShowOEMDialog(HIWORD(wParam));
				m_wndSplitterVert.UpdateSizes();
			}
			else if (lParam == (LPARAM)&m_wndSplitterObjectViews)
			{
				m_wndSplitterVert.ShowObjectView(HIWORD(wParam));
				m_wndSplitterVert.UpdateSizes();
			}
			else
			{
				CDlgExternalFrame *pExt;
				for (int i=0; i<m_ExternalFrames.GetSize(); i++)
				{
					pExt = m_ExternalFrames.GetAtFast(i);
					if (lParam == (LPARAM)pExt)
					{
						CRect rc;
						m_ExternalFrames.RemoveAt(i);
						theApp.m_dwMonitorFlags &= ~pExt->GetMonitorFlag();
						GetViewIdipClient()->GetClientRect(&rc);
						GetViewIdipClient()->SendMessage(WM_SIZE, 0, MAKELONG(rc.right, rc.bottom));
						break;
					}
				}
			}
		}break;
		case WPARAM_CLOSE_CONNECT_DLG:
			GetViewIdipClient()->GetDocument()->CloseConnectionDlg();
			break;
		case WPARAM_CHECK_PERF_LEVEL:
			theApp.m_dwIdleTickCount = GetTickCount();
			theApp.CalcCPUusage();
			theApp.CheckPerformanceLevel();
			break;
		case GET_MONITOR_RECT:
		{
			int nMonitor = HIWORD(wParam);
			CRect rc;
			if (m_MonitorInfo.GetMonitorRect(nMonitor, rc, true))
			{
				*((CRect*)lParam) = rc;
			}
			return 1;
		}break;
		case WPARAM_PANE_INT:
		{
			_TCHAR szText[16];
			_itot(lParam, szText, 10);
			SetPanetext(HIWORD(wParam), szText);
		}break;
		case WPARAM_PANE_TXT:
		{
			LPCTSTR pszText = (LPCTSTR)lParam;
			SetPanetext(HIWORD(wParam), pszText);
			free((void*)pszText);
		}break;
		case REJECT_ANNOUNCEMENT:
		{
			AfxMessageBox(IDS_REJECT_ANNOUNCEMENT);
		}break;
		case REMOVE_SEQUENCE:
		{
			CServer*pServer = GetDocument()->GetServer(HIWORD(wParam));
			if (pServer)
			{
				pServer->RemoveSequence(LOWORD(lParam), HIWORD(lParam));
			}
		}break;
		case NO_MEDIA_RECEIVE_CONFIRM:
		{
			WORD wServer = HIWORD(wParam);
			CSecID id((DWORD)lParam);
			int nResult = AfxMessageBox(IDS_NO_MEDIA_RECEIVE_CONFIRM, MB_YESNOCANCEL);
			if (nResult == IDYES)
			{
				CIdipClientDoc*pDoc = GetDocument();
				if (pDoc)
				{
					CIPCAudioIdipClient *pAudio = NULL;
					if (wServer == SECID_NO_SUBID)
					{
						pAudio = pDoc->GetLocalAudio();
					}
					else
					{
						CServer *pServer = pDoc->GetServer(wServer);
						if (pServer)
						{
							pAudio = pServer->GetAudio();
						}
					}
					if (pAudio)
					{
						pAudio->DoRequestSetValue(id, _T("ReleaseFilters"),_T(""));
						pAudio->ResetSentSamples();
					}
				}
			}
			else if (nResult == IDCANCEL)
			{
				PostCommand(ID_AU_STOP_ANY);
			}
		}break;
		case WPARAM_UNREQUEST_PICTURES:
		{
			CServer *pServer = GetDocument()->GetServer(HIWORD(wParam));
			if (pServer && pServer->IsConnectedOutput())
			{
				pServer->GetOutput()->UnrequestPictures(CSecID((DWORD)lParam));
			}
		}break;
		case INIT_FIELDS:
			theApp.InitFields();
			break;
		case WM_ACTIVATE:	
			// rke: message is used in SplitterObjectView to show or hide the treeviews
			break;
		case WM_ACTIVATEAPP:
		{
			m_nStyle = GetWindowLong(m_hWnd, GWL_STYLE);
			ModifyStyle(WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU, 0);
			// RKE: WS_THICKFRAME is used for sizing of the frame
			// otherwise it must be implemented by the user
			if (m_nStyle & WS_SYSMENU)
			{
				int nImages = 1;
				if (m_nStyle & WS_MAXIMIZEBOX) nImages++;
				if (m_nStyle & WS_MINIMIZEBOX) nImages++;
			}
			
			m_nStyle = m_nStyle&(WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME|WS_SYSMENU);
	//		PostMessage(WM_NCPAINT);
			FlashWindow(TRUE);
		}break;
		case ALARM_CONNECTION:
		{
			// Check screensaver, if Alarmconnection
			BOOL bActive = FALSE;
			// WINVER >= 0x0500
			// #define SPI_GETSCREENSAVERRUNNING 114 
 			SystemParametersInfo(114, FALSE, &bActive, SPIF_SENDWININICHANGE);
			WK_TRACE(_T("ScreenSaverRunning = %d\n"), bActive);
			
			if (bActive)
			{
				HKEY hKey = NULL;
				BOOL bSecure = TRUE;
				if (theApp.IsWinXP())
				{
					LRESULT lResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), 0, KEY_READ, &hKey);
					if (lResult == ERROR_SUCCESS)
					{
						_TCHAR szReg[32];
						DWORD dwType, dwCount = sizeof(szReg);
						lResult = RegQueryValueEx(hKey, _T("ScreenSaverIsSecure"), NULL, &dwType, (LPBYTE)szReg, &dwCount);
						if (lResult == ERROR_SUCCESS && dwType == REG_SZ && dwCount != 0)
						{
							bSecure = _ttoi(szReg);
						}
					}
					if (hKey)
					{
						RegCloseKey(hKey);
					}
				}
				if (bSecure)
				{
					WK_TRACE(_T("ScreenSaver is Secure\n"));
					// TODO! RKE: Letztes Alarmbild speichern und Daueralarmton starten, bis
					// der User sich einloggt.
					Beep(1000, 1000);
				}
				else
				{
					WK_TRACE(_T("Find WindowsScreenSaverClass\n"));
					HWND hwnd = ::FindWindow(_T("WindowsScreenSaverClass"), NULL);
					if (hwnd)
					{
						::PostMessage(hwnd, WM_CLOSE, 0, 0);
						WK_TRACE(_T("WindowsScreenSaverClass found\n"));
						bActive = FALSE;
					}
					else
					{
						// does not work properly
						//	hwnd = ::GetForegroundWindow();
						//	if (hwnd)
						//	{
						//		::PostMessage(hwnd, WM_CLOSE, 0, 0);
						//		WK_TRACE(_T("ForegroundWindow found\n"));
						//		bActive = FALSE;
						//	}

						WK_TRACE(_T("trying Screen-saver Desktop\n"));
						HDESK hdesk = OpenDesktop(_T("Screen-saver"), 0, FALSE, DESKTOP_READOBJECTS | DESKTOP_WRITEOBJECTS);
						if (hdesk)
						{
							EnumDesktopWindows(hdesk, CMainFrame::KillScreenSaverFunc, (LPARAM)&bActive);
							CloseDesktop(hdesk);
							if (!bActive)
							{
								WK_TRACE(_T("WindowsScreenSaver found\n"));
							}
						} 
					}
				}
			}
		}break;
		case 0: break;
		default:
			TRACE(_T("Unknown cmd: %x\n"), wLow);
			break;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::AddExternalFrame(int nMonitor)
{
	CDlgExternalFrame *pExternalFrame;
	pExternalFrame = new CDlgExternalFrame(this, GetViewIdipClient(), nMonitor);
	pExternalFrame->Create(IDD_EXTERNAL_FRAME, this);
	m_ExternalFrames.Add(pExternalFrame);
}
//////////////////////////////////////////////////////////////////////////
bool CMainFrame::RemoveSWfromExtFrames(CWndSmall *pSW)
{
	for (int i=0; i<m_ExternalFrames.GetSize(); i++)
	{
		if (m_ExternalFrames.GetAtFast(i)->RemoveWndSmall(pSW))
		{
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
int CMainFrame::GetNoOfExternalFrames()
{
	return m_ExternalFrames.GetSize();
}
//////////////////////////////////////////////////////////////////////////
CDlgExternalFrame* CMainFrame::GetExternalFrame(int nFrame)
{
	if (nFrame < m_ExternalFrames.GetSize())
	{
		return (CDlgExternalFrame*) m_ExternalFrames.GetAtFast(nFrame);
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewMonitor(UINT nID)
{
	int         nMonitor      = nID - ID_VIEW_MONITOR2+1;
	DWORD       dwMonitorFlag = 1 << nMonitor;

	if (!(theApp.m_dwMonitorFlags & USE_MONITOR_1))	// if the main window is not on first monitor
		return;

	if (dwMonitorFlag & theApp.m_dwMonitorFlags)
	{
		theApp.m_dwMonitorFlags &= ~dwMonitorFlag;
		for (int i=0; i<GetNoOfExternalFrames(); i++)
		{
			CDlgExternalFrame *pExtFrame = GetExternalFrame(i);
			if (pExtFrame->GetMonitorFlag() == dwMonitorFlag)
			{
				pExtFrame->PostMessage(WM_CLOSE, 0, 0);
			}
		}
	}
	else
	{
		theApp.m_dwMonitorFlags |= dwMonitorFlag;
		AddExternalFrame(nMonitor);
		PostMessage(WM_EXITSIZEMOVE); 
	}

	GetViewIdipClient()->PostMessage(WM_COMMAND, ID_VIEW_MONITOR16, 0);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewMonitor(CCmdUI* pCmdUI)
{
	int nMonitor = pCmdUI->m_nID - ID_VIEW_MONITOR2+1,
		nMonitors = GetNoOfMonitors();
	DWORD dwMonitorFlag = 1 << nMonitor;
	if (!(theApp.m_dwMonitorFlags & USE_MONITOR_1))
	{
		dwMonitorFlag = 0;
		nMonitor = nMonitors;
	}
	pCmdUI->Enable((nMonitor < nMonitors) ? TRUE : FALSE);
	pCmdUI->SetCheck((dwMonitorFlag & theApp.m_dwMonitorFlags) ? 1 : 0);
}
//////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL ret;
	
	ret =  CFrameWnd::PreCreateWindow(cs);
	cs.lpszClass = WK_APP_NAME_IDIP_CLIENT;
	cs.style &= ~FWS_ADDTOTITLE;
//	cs.style |= WS_MAXIMIZE;

	return ret;
}

// CMainFrame Diagnose

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

//////////////////////////////////////////////////////////////////////////
CIdipClientDoc* CMainFrame::GetDocument()
{
	CDocument *pDoc = GetActiveDocument();
	if (pDoc)
	{
		ASSERT_KINDOF(CIdipClientDoc, pDoc);
		return (CIdipClientDoc*)pDoc;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
CViewIdipClient* CMainFrame::GetViewIdipClient()
{
	return m_wndSplitterHorz.GetViewIdipClient();
}
//////////////////////////////////////////////////////////////////////////
CViewDialogBar* CMainFrame::GetViewDlgBar()
{
	return m_wndSplitterHorz.GetViewDlgBar();
}
//////////////////////////////////////////////////////////////////////////
CViewDialogBar* CMainFrame::GetViewOEMDlg()
{
	return m_wndSplitterObjectViews.GetViewOEMDlg();
}
//////////////////////////////////////////////////////////////////////////
CViewCamera* CMainFrame::GetViewCamera()
{
	return m_wndSplitterObjectViews.GetViewCamera();
}
//////////////////////////////////////////////////////////////////////////
CViewRelais* CMainFrame::GetViewRelais()
{
	return m_wndSplitterObjectViews.GetViewRelais();
}
//////////////////////////////////////////////////////////////////////////
CViewAlarmList* CMainFrame::GetViewAlarmList()
{
	return m_wndSplitterObjectViews.GetViewAlarmList();
}
//////////////////////////////////////////////////////////////////////////
CViewAudio* CMainFrame::GetViewAudio()
{
	return m_wndSplitterObjectViews.GetViewAudio();
}
//////////////////////////////////////////////////////////////////////////
CViewArchive* CMainFrame::GetViewArchive()
{
	return m_wndSplitterObjectViews.GetViewArchive();
}
//////////////////////////////////////////////////////////////////////////
CSplitterObjectView*CMainFrame::GetSplitterObjectView()
{
	return &m_wndSplitterObjectViews;
}
//////////////////////////////////////////////////////////////////////////
CDlgBarIdipClient*CMainFrame::GetDlgBarIdipClient()
{
	CDlgInView*pDlg = m_wndSplitterHorz.GetViewDlgBar()->GetDlgInView();
	ASSERT(pDlg != NULL);
	ASSERT_KINDOF(CDlgBarIdipClient, pDlg);
	return (CDlgBarIdipClient*)pDlg;
}
//////////////////////////////////////////////////////////////////////////
// CMainFrame Meldungshandler
void CMainFrame::OnAppAbout()
{
	COemGuiApi::AboutDialog(this);
}
//////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CRect rc;
	GetWindowRect(rc);
	int  nDlgBarRow = 1,
		 nSplitterViewsRow = 0;
	if (theApp.m_bDialogBarTop)
	{
		nDlgBarRow = 0;
		nSplitterViewsRow = 1;
	}
	if (theApp.m_bObjectViewsLeft)
	{
		m_wndSplitterVert.SetObjectView(0);
		m_wndSplitterVert.SetTopBottom(1);
	}
	// split vertically
	m_wndSplitterVert.CreateStatic(this, 1, 2); 

	// split horizontally for DlgBar and ClientView
	m_wndSplitterHorz.CreateStatic(&m_wndSplitterVert, 2, 1,  WS_CHILD|WS_VISIBLE,
		m_wndSplitterVert.IdFromRowCol(0, m_wndSplitterVert.GetTopBottom()));

	// split horizontally for the ObjectViews
	int nObjectViews = 6;
	m_wndSplitterObjectViews.CreateStatic(&m_wndSplitterVert, nObjectViews, 1, WS_CHILD|WS_VISIBLE,
		m_wndSplitterVert.IdFromRowCol(0, m_wndSplitterVert.GetObjectView()));

	// create and insert ViewDialogBar into SplitterHorz
	m_wndSplitterHorz.CreateView(nDlgBarRow, 0, RUNTIME_CLASS(CViewDialogBar),
		CSize(rc.Width(), rc.Height()/4), pContext); 
	// create and insert ViewIdipClient for the SmallWindows into SplitterHorz
	m_wndSplitterHorz.CreateView(nSplitterViewsRow, 0, RUNTIME_CLASS(CViewIdipClient),
		CSize(rc.Width(), MulDiv(rc.Height(),3,4)), pContext); 

	// create and insert the object views into SplitterObjectViews
	int nObjectView = 0;
	int nOVHeight   = rc.Height()/nObjectViews;
	m_wndSplitterObjectViews.CreateView(nObjectView++, 0, RUNTIME_CLASS(CViewAlarmList),
							CSize(rc.Width(), nOVHeight), pContext);

	m_wndSplitterObjectViews.CreateView(nObjectView++, 0, RUNTIME_CLASS(CViewCamera),
							CSize(rc.Width(), nOVHeight), pContext); 

	m_wndSplitterObjectViews.CreateView(nObjectView++, 0, RUNTIME_CLASS(CViewAudio),
							CSize(rc.Width(), nOVHeight), pContext);

	m_wndSplitterObjectViews.CreateView(nObjectView++, 0, RUNTIME_CLASS(CViewRelais),
							CSize(rc.Width(), nOVHeight), pContext); 

	m_wndSplitterObjectViews.CreateView(nObjectView++, 0, RUNTIME_CLASS(CViewArchive),
							CSize(rc.Width(), nOVHeight), pContext); 

	m_wndSplitterObjectViews.CreateView(nObjectView++, 0, RUNTIME_CLASS(CViewDialogBar),
							CSize(rc.Width(), nOVHeight), pContext); 

	m_wndSplitterObjectViews.PostMessage(WM_USER, WM_UPDATE_ALL_VIEWS);
	ASSERT(nObjectViews == nObjectView);

	m_wndSplitterVert.InitColumns();

	CMenu*pMainMenu = GetMenu();
	UINT nID = ID_VIEW_MONITOR2;
	CString str;
	CMenu* pPopupView = ((CSkinMenu*)pMainMenu)->FindMenuString(nID, str);

	int iNrOfMonitors = GetNoOfMonitors();
	if (iNrOfMonitors > 1)
	{
		CIdipClientApp *pApp = (CIdipClientApp*)AfxGetApp();
		int i;
		CString str;
		DWORD dwMonitorFlag;
		for (i=1; i<iNrOfMonitors; i++)
		{
			dwMonitorFlag = (1 << i);
			if ((pApp->m_dwMonitorFlags & dwMonitorFlag) && (pApp->m_dwMonitorFlags & USE_MONITOR_1))
			{
				AddExternalFrame(i);
			}
			if (i>1 && pPopupView)
			{
				int nPos = nID +1;	// nID is set to the found item position in FindMenuString(..)
				str.Format(IDS_MONITOR, i+1);
				pPopupView->InsertMenu(nPos, MF_BYPOSITION, ID_VIEW_MONITOR1+i, str);
				nID++;	// if there are more than three monitors increment the position
			}
		}
		if (!(pApp->m_dwMonitorFlags & USE_MONITOR_1))
		{
			SetWindowMonitor();
		}
	}
	else if (pPopupView)
	{
		pPopupView->DeleteMenu(nID, MF_BYPOSITION);
	}

	return TRUE; // CFrameWnd::OnCreateClient(lpcs, pContext);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
	CFrameWnd::OnSizing(fwSide, pRect);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
}
/////////////////////////////////////////////////////////////////////////////
/*
	afx_msg void OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)
void CMainFrame::OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	// this function handles the dropdown menus from the toolbar
	NMTOOLBAR* pNMToolBar = (NMTOOLBAR*)pNotifyStruct;
	CRect rect(pNMToolBar->rcButton);

	if (pNMToolBar->iItem == ID_CONNECT)
	{
		CSkinMenu menu;

		int i,c;
		CHostArray& ha = theApp.GetHosts();
		CHost* pHost;
		CString sText;
		CString sLocal;
		c = ha.GetSize();

		menu.CreatePopupMenu();

		if (theApp.m_pPermission->IsSpecialReceiver() == FALSE)
		{
			sLocal.LoadString(IDS_LOCAL_SERVER);
			pHost = ha.GetLocalHost();
			sText.Format(_T("&%s %s"),sLocal,pHost->GetName());
			menu.AppendMenu(MF_STRING,
							ID_CONNECT_LOCAL,
							sText);
		}

		for (i=0;i<c && i<9;i++)
		{
			pHost = ha.GetAtFast(i);
			if (   pHost->GetTyp()!=_T("SMS")
				&& pHost->GetTyp()!=_T("E-Mail")
				&& pHost->GetTyp()!=_T("FAX"))
			{
				sText.Format(_T("&%x %s (%s)"),i+1,pHost->GetName(),pHost->GetTyp());
				menu.AppendMenu(MF_STRING,
								pHost->GetID().GetSubID()+ID_CONNECT_LOW,
								sText);
			}
		}
		if (c>9)
		{
			sText.LoadString(IDS_CONNECT);
			menu.AppendMenu(MF_STRING,
							ID_CONNECT,
							sText);
		}

		  // the font popup is stored in a resource
		  menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left, rect.top, this);
	}
	else if (pNMToolBar->iItem == ID_DISCONNECT)
	{
		CIdipClientDoc*pDoc = GetDocument();
		if (pDoc)
		{
			CServers*pServers = (CServers*) &pDoc->GetServers();
			pServers->PopupDisconnectMenu(rect.TopLeft());
		}
	}
	else if (pNMToolBar->iItem == ID_SEQUENZE)
	 {
		CMenu menu;
		CMenu* pPopup;

		menu.LoadMenu(IDR_CONTEXT);
		// Sequenzer Popup menu
		pPopup = menu.GetSubMenu(4);
#ifdef _DTS
		// Sequenzer erst ab 4 s
		pPopup->DeleteMenu(ID_SEQUENCE_1,MF_BYCOMMAND);
		pPopup->DeleteMenu(ID_SEQUENCE_2,MF_BYCOMMAND);
		pPopup->DeleteMenu(ID_SEQUENCE_3,MF_BYCOMMAND);
#endif
//      pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left, rect.bottom, this);
      pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left, rect.bottom, AfxGetMainWnd());
	}
	// Recherche
    else if (pNMToolBar->iItem == ID_FILE_DISK)
    {
		CMenu menu;
		CMenu* pPopup;
		menu.LoadMenu(IDR_CONTEXT);
		pPopup = menu.GetSubMenu(4);
#ifdef _DTS
#endif
        pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, 
							   rect.left, 
							   rect.bottom, 
							   this);
	}
    *pResult = 0;
}
*/
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnConnect(UINT nID)
{
	CIdipClientDoc* pDoc = GetDocument();
	if (pDoc)
	{
		CSecID idHost(SECID_GROUP_HOST,(WORD)(nID-ID_CONNECT_LOW));
		pDoc->Connect(idHost);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDisconnect() 
{
	ChangeFullScreenModus(FALSE|FULLSCREEN_FORCE);

	//immer die Station trennen, die zum aktiven Fenster gehört
	//das Trennen einer im Baum markierten Station nur durch Kontextmenü der Station
	//im Baum ermöglichen

	WORD wServerID = SECID_NO_SUBID;
	CView*pView = GetSelectedServerAndView(wServerID);
	if (pView)
	{
		((CIdipClientDoc*)pView->GetDocument())->Disconnect(wServerID);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDisconnect(UINT nID)
{
	if (m_bSplitterWndCreated)
	{
		CIdipClientDoc*pDoc = GetDocument();
		if (pDoc)
		{
			WORD wServerID = LOWORD(nID-ID_DISCONNECT_LOW);
			if (pDoc->GetServer(wServerID))
			{
				pDoc->Disconnect(wServerID);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0700
void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID) 
{
	CFrameWnd::OnActivateApp(bActive, dwThreadID);
#else
void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CFrameWnd::OnActivateApp(bActive, hTask);
#endif // _MFC_VER >= 0x0700
	
	m_bActive = bActive ? true : false;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnMove(int x, int y) 
{
	CFrameWnd::OnMove(x, y);
	if (theApp.m_pDlgLogin)
	{
		theApp.m_pDlgLogin->SetWindowPosition();
		theApp.m_pDlgLogin->SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
// RKE: Use ChangeFullScreenModus instead of directly called, sent or posted commands
void CMainFrame::ChangeFullScreenModus(BOOL bOn/*=FULLSCREEN_TOGGLE*/)
{
	BOOL bMessage = FALSE;
	BOOL bSend = theApp.IsInMainThread();
	UINT nID = ID_VIEW_FULLSCREEN;
	if (bOn != FULLSCREEN_TOGGLE)
	{
		if (bOn & FULLSCREEN_FORCE)
		{
			nID = ID_FORCE_FULLSCREEN;
		}
		if (bOn & FULLSCREEN_POST)
		{
			bSend = FALSE;
		}
	}
	switch (LOWORD(bOn))
	{
		case FULLSCREEN_TOGGLE:	// Toggle
			bMessage = TRUE;
			break;
		case TRUE:	// switch on
			if (!IsFullScreen())
			{
				bMessage = TRUE;
			}break;
		case FALSE:	// switch off
			if (IsFullScreen())
			{
				bMessage = TRUE;
			}break;
		default: break;
	}

	if (bMessage)
	{
		if (bSend)
		{
			SendMessage(WM_COMMAND, nID, 0);
		}
		else
		{
			PostCommand(nID);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewFullscreen() 
{
	m_bFullScreen = !m_bFullScreen;
	CViewIdipClient* pVV = GetViewIdipClient();
	pVV->LockSmallWindowResizing(2);

	if (m_bFullScreen)
	{
		ShowMenu(FALSE);

		m_bFullScreenChanging = true; // Recherche, TKR
		GetWindowPlacement(&m_wp);
		CRect rect;

		ShowWindow(SW_RESTORE);
		if (CSkinDialog::UseOEMSkin())
		{
			ModifyStyle(WS_CAPTION|WS_THICKFRAME, 0);
		}
		else
		{
			ModifyStyle(WS_OVERLAPPEDWINDOW,0);
		}
		CRect workingRect;
		BOOL bPrimary = TRUE;
		if (GetNoOfMonitors() > 1)
		{
			HMONITOR hMon = m_MonitorInfo.GetMonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
			int      iMon;
			if (m_MonitorInfo.GetMonitorFromHandle(hMon, iMon) && (iMon != 0))
			{
				bPrimary = !m_MonitorInfo.GetMonitorRect(iMon, workingRect, true);
			}
		}
		if (bPrimary)
		{
			SystemParametersInfo(SPI_GETWORKAREA,0,&workingRect,0);
		}

		SetWindowPos(&wndTop,
					workingRect.left,
					workingRect.top,
					workingRect.Width(),
					workingRect.Height(),
					0);

		// RKE: These bars are still available and not shown
		if (m_wndToolBar.m_hWnd)
		{
			ShowControlBar(&m_wndToolBar,FALSE,FALSE);
		}

		m_wndSplitterVert.UpdateSizes();
		m_bFullScreenChanging = false; // Recherche, TKR
	}
	else
	{
		ShowMenu(TRUE);
		m_bFullScreenChanging = true; // Recherche, TKR
		if (CSkinDialog::UseOEMSkin())
		{
			ModifyStyle(0, WS_CAPTION|WS_THICKFRAME);
		}
		else
		{
			ModifyStyle(0, WS_OVERLAPPEDWINDOW);
		}

		ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
		SetWindowPlacement(&m_wp);
		ModifyStyle(WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU, 0);
		
		// RKE: These bars are still available and not shown
//		if (WK_IS_WINDOW(&m_wndToolBar))
//		{
//			ShowControlBar(&m_wndToolBar,TRUE,FALSE);
//		}
//		SendMessage(WM_SYSCOMMAND, m_wp.showCmd == SW_SHOWMAXIMIZED ? SC_MAXIMIZE : SC_RESTORE);

		m_wndSplitterVert.UpdateSizes();
		m_bFullScreenChanging = false; // Recherche, TKR
		UpdateWindow();
	}
	for (int i=0; i<GetNoOfExternalFrames(); i++)
	{
		GetExternalFrame(i)->OnViewFullscreen(m_bFullScreen);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewFullscreen(CCmdUI* pCmdUI) 
{
	BOOL bEnable = m_bFullScreen;
	if (m_bSplitterWndCreated && !bEnable)
	{
		CViewIdipClient*pView = GetViewIdipClient();
		if (pView->GetSmallWindows()->GetSize() > 0)
		{
			bEnable = TRUE;
		}
		else if (pView->DoFullScreenOrView1PlusAtAlarm())
		{
			bEnable = TRUE;
		}
	}
	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(m_bFullScreen);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
#ifndef _DEBUG
	// rke: im Releas modus wird gefiltert
	if (nCode == CN_UPDATE_COMMAND_UI || nCode == CN_COMMAND)
#endif
	{
		if (m_bSplitterWndCreated)
		{
			CDocument *pDocument = GetActiveDocument();
			if (pDocument)
			{
				POSITION pos = pDocument->GetFirstViewPosition();
				while (pos != NULL)
				{
					CView* pView = pDocument->GetNextView(pos);
					if (pView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
					{
#ifdef _DEBUG
						// rke: im debugmodus testen, ob auch andere messages behandelt werden!
						if (nCode != CN_UPDATE_COMMAND_UI && nCode != CN_COMMAND)
						{
							TRACE(_T("#Test# View handled non Cmd Msg: nID=%08lx, Code:%08lx\n"),nID, nCode);
						}
#endif
						return TRUE;
					}
				}
			}

			if (m_wndSplitterObjectViews.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			{
#ifdef _DEBUG
				// rke: im debugmodus testen, ob auch andere messages behandelt werden!
				if (nCode != CN_UPDATE_COMMAND_UI && nCode != CN_COMMAND)
				{
					TRACE(_T("#Test# Splitter handled non Cmd Msg: nID=%08lx, Code:%08lx\n"),nID, nCode);
				}
#endif
				return TRUE;
			}

			if (m_pwndSyncCoolBar)
			{
				if (m_pwndSyncCoolBar->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
				{
#ifdef _DEBUG
					// rke: im debugmodus testen, ob auch andere messages behandelt werden!
					if (nCode != CN_UPDATE_COMMAND_UI && nCode != CN_COMMAND)
					{
						TRACE(_T("#Test# SyncCoolBar handled non Cmd Msg: nID=%08lx, Code:%08lx\n"),nID, nCode);
					}
#endif
					return TRUE;
				}
			}
		}
	}
//	else
//	{
//		TRACE(_T("filter OnCmdMsg nID =%08lx %08lx\n"),nID,nCode);
//	}

	// then pump through frame
	if (CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// last but not least, pump through app
	CWinApp* pApp = AfxGetApp();
	if (pApp != NULL && pApp->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	return FALSE;
//	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
#ifdef _DEBUG
LPCTSTR NameOfEvent(UINT nEventType)
{
	switch (nEventType)
	{
	case DBT_DEVICEARRIVAL: 
		return _T("DBT_DEVICEARRIVAL");
	case DBT_DEVICEQUERYREMOVE: 
		return _T("DBT_DEVICEQUERYREMOVE");
	case DBT_DEVICEQUERYREMOVEFAILED: 
		return _T("DBT_DEVICEQUERYREMOVEFAILED");
	case DBT_DEVICEREMOVEPENDING: 
		return _T("DBT_DEVICEREMOVEPENDING");
	case DBT_DEVICEREMOVECOMPLETE: 
		return _T("DBT_DEVICEREMOVECOMPLETE");
	case DBT_DEVICETYPESPECIFIC: 
		return _T("DBT_DEVICETYPESPECIFIC");
	case DBT_CONFIGCHANGED:
		return _T("DBT_CONFIGCHANGED");
	case DBT_DEVNODES_CHANGED:
		return _T("DBT_DEVNODES_CHANGED");
	default:
		return _T("unknown");
	}
}
#endif // _DEBUG
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnDeviceChange( UINT nEventType, DWORD dwData )
{
	if (m_bSplitterWndCreated)
	{
#ifdef _DEBUG
		TRACE(_T("WM_DEVICE_CHANGE %s\n"), NameOfEvent(nEventType));
#endif
		DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)dwData;
		if (pHdr)
		{
			if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
			{
				DEV_BROADCAST_VOLUME *pDBV  = (DEV_BROADCAST_VOLUME*)pHdr;
				_TCHAR  szDrive[] = _T("A:");
				int   nDrive;
				DWORD dwUnitMask = pDBV->dbcv_unitmask;
				for (nDrive=-1; nDrive<32 && dwUnitMask!=0; nDrive++) dwUnitMask >>= 1;
				szDrive[0] = (_TCHAR)(_T('A') + nDrive);
				if (GetDriveType(szDrive) == DRIVE_REMOVABLE)
				{
					if (nEventType == DBT_DEVICEARRIVAL)
					{
						theApp.m_sOpenDirectory = szDrive;
						theApp.m_nRemoveableDriveLetter = szDrive[0];
					}
					else
					{
						theApp.ReadOpenDirPath();
						theApp.m_nRemoveableDriveLetter = 0;
					}
				}
			}
			if (pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE_EX)
			{
				CDevIntEx dix((DEV_BROADCAST_DEVICEINTERFACE_EX*)pHdr);
#ifdef _DEBUG
				CString sClass    = dix.GetGuidClass().GetString();
				CString sDevClass = dix.GetGuidDevClass().GetString();
				CString sClassHidUSB, sClassUSB, sDevClassPrinter, sDevClassDiskDrive;
				TRACE(_T("Name: %s from: %s, Service: %s\nClass:%s\nDevClass:%s\nRegKey HKLM:%s\n"), dix.GetDeviceName(), dix.GetManufacturer(), dix.GetService(), sClass, sDevClass, dix.GetRegistryKey());
#endif
				if (   dix.GetGuidClass()    == GUID_DEVINTCLASS_HUDINT
					&& dix.GetGuidDevClass() == GUID_DEVCLASS_HID)
				{
					if (   nEventType == DBT_DEVICEARRIVAL
						|| nEventType == DBT_DEVICEREMOVECOMPLETE)
					{
						InitJoystick();
					}
				}
				else if (dix.GetGuidDevClass() == GUID_DEVCLASS_DISK_DRIVE)// DiskDrive: USBSTORAGE
				{
					if (nEventType == DBT_DEVICEARRIVAL)
					{
						CString sKey = dix.GetRegistryKey();
						CWK_Profile prof(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, sKey, _T(""));
						CString sSection = DBDI_DEV_PARAM;
						sSection += _T("\\");
						sSection += DBDI_CLASSPNP;
						prof.WriteInt(sSection, DBDI_USERREMOVALPOLICY, 3);// disable Drive Cache
					}
				}
			}
		}
		CDlgBackup* pDlgBackup = GetViewIdipClient()->GetDlgBackup();
		if (pDlgBackup)
		{
			pDlgBackup->OnDeviceChange(nEventType, dwData);
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, WAI_IDIP_CLIENT, 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSync() 
{
	//Syncplay is active
	if (m_pwndSyncCoolBar)
	{
		ShowControlBar(m_pwndSyncCoolBar,FALSE,FALSE);

		//stop all sync plays
		m_pwndSyncCoolBar->OnSyncStop();
		m_pwndSyncCoolBar->DestroyWindow();
		WK_DELETE(m_pwndSyncCoolBar);
		if (m_bSplitterWndCreated)
		{
			GetViewArchive()->ShowSyncCheckBoxes(FALSE);

			//den während des SyncPlay versteckten Slider wieder anzeigen
			GetViewIdipClient()->PostMessage(WM_USER, WPARAM_UPDATE_SYNCDLGS_IN_WNDPLAY);

			//TKR on syncplay stop leave all open windows open
			//GetViewIdipClient()->OnCloseWndSmallAllLive();
		}
	}
	else
	{
		if (m_bSplitterWndCreated)
		{
			//TKR on syncplay, leave all open windows open
			//GetViewIdipClient()->OnCloseWndSmallAll();
		}

		m_pwndSyncCoolBar = new CSyncCoolBar;
		if (!m_pwndSyncCoolBar->Create(this,
			WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
				RBS_TOOLTIPS|RBS_BANDBORDERS|RBS_VARHEIGHT)) 
		{
			TRACE(_T("Failed to create sync cool bar\n"));
		}
//		ShowControlBar(m_pwndSyncCoolBar,TRUE,FALSE);

		if (m_bSplitterWndCreated)
		{
			GetViewArchive()->ShowSyncCheckBoxes();
		}
	}
	RecalcLayout();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateSync(CCmdUI* pCmdUI)
{
	// Zeitsynchrones Abspielen im Backup-Mode verboten
	BOOL bEnable = (   theApp.IsValidUserLoggedIn()
					&& (GetDocument()->GetServers().GetSize() > 0)
					&& (GetViewArchive()->IsBackupMode() == FALSE)
					);
	if (!bEnable && IsSyncMode())
	{
		OnSync();
	}

	pCmdUI->Enable(bEnable);
	// der Menüpunkt wird gecheckt, der Button nicht
	// TODO gf Warum nicht?
//	if (pCmdUI->m_pMenu)
	{
		pCmdUI->SetCheck(IsSyncMode());
	}
}
///////////////////////////////////////////////////////////////////////////////
CWnd* CMainFrame::GetMessageBar()
{
	if (m_wndStatusBar.m_hWnd)
	{
		return &m_wndStatusBar;
	}
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;

	if (IsBetween(wParam, ID_VIEW_MONITOR1, ID_VIEW_MONITOR16))
	{
		m_sToolTip.Format(ID_VIEW_MONITOR2, wParam - ID_VIEW_MONITOR1 + 1);
	}
	else if (IsBetween(wParam, ID_CONNECT_LOW, ID_CONNECT_HIGH))
	{
		m_sToolTip.LoadString(ID_CONNECT);
	}
	else
	{
		m_sToolTip.LoadString(wParam);
	}
	int nFind = m_sToolTip.Find(_T("\n"));
	if (nFind != -1)
	{
		m_sToolTip = m_sToolTip.Left(nFind);
	}

	lParam = (LPARAM)LPCTSTR(m_sToolTip);

	lResult = CFrameWnd::OnSetMessageString(0, lParam);
	if (theApp.m_bShowMenuToolTips)
	{
		KillToolTipTimer();
		m_nTootipTimer = (UINT)SetTimer((UINT_PTR) 1, 800, NULL);
		m_nIDHelp = wParam;
	}
	return lResult;
}
///////////////////////////////////////////////////////////////////
void CMainFrame::OnArchivBackup() 
{
	// On Backup no Sync Playback
	if (m_pwndSyncCoolBar)
	{
		OnSync();
	}

	if (m_bSplitterWndCreated)
	{
		GetViewArchive()->OnArchivBackup();
	}
}

///////////////////////////////////////////////////////////////////
void CMainFrame::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
/*
	CViewIdipClient* pView = theApp.GetMainFrame()->GetViewIdipClient();
	if (pView)
	{
		pView->LockSmallWindows(_T(__FUNCTION__));
		CWndLive* pDW = pView->GetWndLiveCmdActive();
		if (pDW)
		{
			pDW->SendMessage(WM_ENTERMENULOOP, (WPARAM)bIsTrackPopupMenu, NULL);
		}
		pView->UnlockSmallWindows();
	}
*/
//	TRACE(_T("CMainFrame Enter menu\n"));
	m_bIsInMenuLoop = TRUE;
	if (theApp.m_bShowMenuToolTips)
	{
		m_nFlags |= WF_TOOLTIPS;
//		m_nFlags |= WF_TRACKINGTOOLTIPS;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	CFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);
	m_hTooltipMenu = hSysMenu;
//	TRACE(_T("OnMenuSelect(%x, %x, %x), %x\n"), nItemID, nFlags, hSysMenu, m_nIDLastMessage);
}
/////////////////////////////////////////////////////////////////////////////
INT_PTR CMainFrame::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	if (   m_bIsInMenuLoop
		&& pTI != NULL)
	{
		AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
		CToolTipCtrl* pToolTip = pModuleThreadState->m_pToolTip;
		if (pModuleThreadState->m_lastInfo.uId != m_nIDHelp)
		{
			pToolTip->SendMessage(TTM_DELTOOL, 0, (LPARAM)&pModuleThreadState->m_lastInfo);
			pToolTip->SendMessage(TTM_ACTIVATE, FALSE);
		}
		pToolTip->SendMessage(TTM_SETMAXTIPWIDTH, 0, 200);

		pTI->hwnd     = ((CMainFrame*)this)->GetDlgBarIdipClient()->CDialog::GetDlgItem(ID_NEW_LOGIN)->m_hWnd;
		::GetWindowRect(pTI->hwnd, &pTI->rect);
		pTI->uId      = (UINT)m_nIDHelp;
		pTI->lpszText = (LPTSTR)LPCTSTR(m_sToolTip);
		pTI->hinst    = AfxGetInstanceHandle();
		pTI->uFlags  |= TTF_ALWAYSTIP|TTF_ABSOLUTE|TTF_TRACK;
		return m_nIDHelp;

/*
		if (!m_sToolTip.IsEmpty())
		{
			pTI->uId      = (UINT) m_hWnd;
			pTI->uFlags  |= TTF_IDISHWND;
			pTI->hwnd     = m_hWnd;
			pTI->lpszText = _tcsdup(m_sToolTip);
		}
		return m_nIDHelp;
*/
	}

	return CFrameWnd::OnToolHitTest(point, pTI);
}
///////////////////////////////////////////////////////////////////
void CMainFrame::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{
	if ( theApp.m_bShowMenuToolTips)
	{
		AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
		CToolTipCtrl* pToolTip = pModuleThreadState->m_pToolTip;
		if (pToolTip)
		{
			pToolTip->SendMessage(TTM_DELTOOL, 0, (LPARAM)&pModuleThreadState->m_lastInfo);
			pToolTip->SendMessage(TTM_ACTIVATE, FALSE);
		}
		m_nFlags &= ~(WF_TOOLTIPS|WF_TRACKINGTOOLTIPS);
		KillToolTipTimer();
	}

	m_hTooltipMenu = NULL;
	m_bIsInMenuLoop = FALSE;
//	TRACE(_T("CMainFrame Exit menu\n"));
/*
	CWndLive* pDW = GetViewIdipClient()->GetWndLiveCmdActive();
	if (pDW)
	{
		pDW->SendMessage(WM_EXITMENULOOP, (WPARAM)bIsTrackPopupMenu, NULL);
	}
*/
	if (m_SystemMenu.m_hMenu)
	{
		m_SystemMenu.ConvertMenu(FALSE);
		m_SystemMenu.Detach();
	}
}
///////////////////////////////////////////////////////////////////
void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CFrameWnd::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		SetWindowMonitor();
	}
}
///////////////////////////////////////////////////////////////////
void CMainFrame::SetWindowMonitor()
{
	DWORD dwFlags;
	int   nMon, nMonitors = GetNoOfMonitors();
	for (nMon=0, dwFlags = 1; nMon < nMonitors; nMon++, dwFlags<<=1)
	{
		if (theApp.m_dwMonitorFlags & dwFlags)
		{
			int nCurrent = -1;;
			m_MonitorInfo.GetMonitorFromHandle(m_MonitorInfo.GetMonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), nCurrent);
			if (nCurrent != nMon)
			{
				CRect rc;
				WINDOWPLACEMENT wpl = {0};
				wpl.length = sizeof(WINDOWPLACEMENT);
				GetWindowPlacement(&wpl);
				ShowWindow(SW_RESTORE);

				CRect rcMon, rcCurr;
				m_MonitorInfo.GetMonitorRect(nMon, rcMon);
				m_MonitorInfo.GetMonitorRect(nCurrent, rcCurr);
				rc = wpl.rcNormalPosition;
				if (rc.IsRectEmpty())
				{
					rc = rcCurr;
				}
				else
				{
					rc.OffsetRect(rcMon.TopLeft()-rcCurr.TopLeft());
				}

				SetWindowPos(&CWnd::wndTop, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOOWNERZORDER);
				PostMessage(WM_SYSCOMMAND, wpl.showCmd == SW_SHOWMAXIMIZED ? SC_MAXIMIZE : SC_RESTORE);
			}
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
	int i, nFrames = GetNoOfExternalFrames();
	for (i=0; i<nFrames; i++)
	{
		CDlgExternalFrame *pWnd = GetExternalFrame(i);
		if (pWnd)
		{
			pWnd->WindowProc(WM_IDLEUPDATECMDUI, wParam, lParam);
		}
	}
	if (m_bSplitterWndCreated)
	{
		m_wndSplitterHorz.UpdateDlgControls(this, TRUE);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnIdle()
{
	if (m_bSplitterWndCreated)
	{
		GetViewIdipClient()->OnIdle();
	}
	if (m_pwndSyncCoolBar)
	{
		m_pwndSyncCoolBar->OnIdle();
	}
	if (HasStatusBar())
	{
		_TCHAR szText[16];
		_itot(CIPCExtraMemory::GetCreatedBubbles(), szText, 10);
		SetPanetext(5, szText);
	}

#ifdef _DEBUG
	LARGE_INTEGER liNow = theTimer.GetMicroTicks();
	g_dwaIdletimes.AddValue((DWORD) (liNow.QuadPart - g_liOld.QuadPart));
	g_liOld = liNow;
#endif

	if (   m_pJoyStick && m_pJoyState	// joystick available
		&& SUCCEEDED(m_pJoyStick->UpdateInputState(m_pJoyState, NULL)))
	{
		DWORD dwCaps = m_pJoyStick->GetCapabilities();
		BOOL bPTZHandled = FALSE;
		CameraControlCmd cmd     = CCC_INVALID;
		CameraControlCmd cmdStop = CCC_INVALID;

		CViewIdipClient*pView = GetViewIdipClient();
		pView->LockSmallWindows(_T(__FUNCTION__));
		CWndLive*pActive = pView->GetWndLiveCmdActive();
		if (pActive && pActive->IsSn())	// PTZ camera window (Schwenk/Neige)
		{
			DWORD dwSpeed = 0;
			if (dwCaps & JOY_CAPS_XAXIS)
			{
				if (m_pJoyState->lX > m_nZeroRange)
				{
					dwSpeed = m_pJoyState->lX-m_nZeroRange;
					cmd     = CCC_PAN_RIGHT;
					cmdStop = CCC_PAN_STOP;
				}
				else if (m_pJoyState->lX < -m_nZeroRange)
				{
					dwSpeed = -(m_pJoyState->lX+m_nZeroRange);
					cmd     = CCC_PAN_LEFT;
					cmdStop = CCC_PAN_STOP;
				}
			}

			if (dwCaps & JOY_CAPS_YAXIS)
			{
				DWORD dwSpeedY = dwSpeed;
				if (m_pJoyState->lY > m_nZeroRange)
				{
					dwSpeedY = m_pJoyState->lY-m_nZeroRange;
					switch (cmd)
					{
						case CCC_INVALID:   cmd = CCC_TILT_UP;       cmdStop = CCC_TILT_STOP; dwSpeed = dwSpeedY; break;
						case CCC_PAN_LEFT:  cmd = CCC_MOVE_LEFT_DOWN;  cmdStop = CCC_MOVE_STOP; break;
						case CCC_PAN_RIGHT: cmd = CCC_MOVE_RIGHT_DOWN; cmdStop = CCC_MOVE_STOP; break;
					}
				}
				else if (m_pJoyState->lY < -m_nZeroRange)
				{
					dwSpeedY = -(m_pJoyState->lY+m_nZeroRange);
					switch (cmd)
					{
						case CCC_INVALID:   cmd = CCC_TILT_DOWN;       cmdStop = CCC_TILT_STOP; dwSpeed = dwSpeedY; break;
						case CCC_PAN_LEFT:  cmd = CCC_MOVE_LEFT_UP;  cmdStop = CCC_MOVE_STOP; break;
						case CCC_PAN_RIGHT: cmd = CCC_MOVE_RIGHT_UP; cmdStop = CCC_MOVE_STOP; break;
					}
				}
				if (   dwSpeed != dwSpeedY
					&& pActive->GetPTZFunctionsEx() & PTZF_EX_SPEED_XY 
					)
				{
					dwSpeed = MAKELONG(LOWORD(dwSpeed), LOWORD(dwSpeedY));
				}
			}

			if (dwCaps & JOY_CAPS_RZAXIS && cmd == CCC_INVALID)
			{
				if (m_pJoyState->lRz > m_nZeroRange)
				{
					dwSpeed = m_pJoyState->lRz-m_nZeroRange;
					cmd     = CCC_ZOOM_IN;
					cmdStop = CCC_ZOOM_STOP;
				}
				else if (m_pJoyState->lRz < -m_nZeroRange)
				{
					dwSpeed = -(m_pJoyState->lRz+m_nZeroRange);
					cmd     = CCC_ZOOM_OUT;
					cmdStop = CCC_ZOOM_STOP;
				}
			}

			if (cmd != CCC_INVALID)
			{
				if (   !(pActive->GetPTZFunctionsEx() & PTZF_EX_SPEED_READJUST)
					|| !m_bControlSpeed)
				{
					dwSpeed = pActive->GetPTZSpeed();
				}
				if (   m_dwJoyPTZspeed != dwSpeed
					|| m_LastCmd       != cmd)
				{
					TRACE(_T("Speed:(%d, %d)\n"), LOWORD(dwSpeed), HIWORD(dwSpeed));
					pActive->ResetLastCameraControlCmd();
					pActive->SetLastCameraControlCmd(cmd, cmdStop, dwSpeed);
				}
				m_LastCmd = cmd;
				m_dwJoyPTZspeed = dwSpeed;
				bPTZHandled = TRUE;
			}
			else if (m_dwJoyPTZspeed)
			{
				m_dwJoyPTZspeed = 0;
				m_LastCmd = cmd;
				pActive->SetLastCameraControlCmd(CCC_INVALID, CCC_INVALID, 0);
				bPTZHandled = TRUE;
			}

		}
		if (!bPTZHandled)
		{
			cmd     = CCC_INVALID;
			cmdStop = CCC_INVALID;
			int i, nBtn = min(MAX_JOYSTICK_BUTTONS, m_pJoyStick->GetNoOfButtons());
			for (i=0; i<nBtn; i++)
			{
				if (m_pJoyState->rgbButtons[i] != 0)
				{
					switch (m_nButtonMap[i])
					{
						case  0: cmd = CCC_ZOOM_IN;    cmdStop = CCC_ZOOM_STOP; break;
						case  1: cmd = CCC_ZOOM_OUT;   cmdStop = CCC_ZOOM_STOP; break;
						case  2: cmd = CCC_FOCUS_FAR;  cmdStop = CCC_FOCUS_STOP; break;
						case  3: cmd = CCC_FOCUS_NEAR; cmdStop = CCC_FOCUS_STOP; break;
						case  4: cmd = CCC_IRIS_OPEN;  cmdStop = CCC_IRIS_STOP; break;
						case  5: cmd = CCC_IRIS_CLOSE; cmdStop = CCC_IRIS_STOP; break;
						case  6: cmd = CCC_FOCUS_AUTO; cmdStop = CCC_INVALID; break;
						case  7: cmd = CCC_IRIS_AUTO;  cmdStop = CCC_INVALID; break;
						case  8: cmd = CCC_TURN_180;   cmdStop = CCC_INVALID; break;
						case  9: cmd = (CameraControlCmd)(255 + VK_RETURN);  cmdStop = CCC_INVALID; break;
						case 10: cmd = (CameraControlCmd)(255 + VK_NUMPAD0);  cmdStop = CCC_INVALID; break;
						case 11: cmd = (CameraControlCmd)(255 + VK_NUMPAD1);  cmdStop = CCC_INVALID; break;
						case 12: cmd = (CameraControlCmd)(255 + VK_NUMPAD2);  cmdStop = CCC_INVALID; break;
						case 13: cmd = (CameraControlCmd)(255 + VK_NUMPAD3);  cmdStop = CCC_INVALID; break;
						case 14: cmd = (CameraControlCmd)(255 + VK_NUMPAD4);  cmdStop = CCC_INVALID; break;
						case 15: cmd = (CameraControlCmd)(255 + VK_NUMPAD5);  cmdStop = CCC_INVALID; break;
						case 16: cmd = (CameraControlCmd)(255 + VK_NUMPAD6);  cmdStop = CCC_INVALID; break;
						case 17: cmd = (CameraControlCmd)(255 + VK_NUMPAD7);  cmdStop = CCC_INVALID; break;
						case 18: cmd = (CameraControlCmd)(255 + VK_NUMPAD8);  cmdStop = CCC_INVALID; break;
						case 19: cmd = (CameraControlCmd)(255 + VK_NUMPAD9);  cmdStop = CCC_INVALID; break;
						case 20: cmd = (CameraControlCmd)(255 + VK_DIVIDE);   cmdStop = CCC_INVALID; break;
						case 21: cmd = (CameraControlCmd)(255 + VK_MULTIPLY); cmdStop = CCC_INVALID; break;
						case 22: cmd = (CameraControlCmd)(1001); cmdStop = CCC_INVALID; break;
						case 23: cmd = (CameraControlCmd)(1002); cmdStop = CCC_INVALID; break;
						case 24: cmd = (CameraControlCmd)(1003); cmdStop = CCC_INVALID; break;
						case 25: cmd = (CameraControlCmd)(1004); cmdStop = CCC_INVALID; break;
					}
					break;
				}
			}
			if (cmd != CCC_INVALID)
			{
				if (cmd != m_LastCmd)
				{
					if (cmd == 1001)
					{
						GetViewIdipClient()->OnTab(FALSE);
					}
					else if (cmd == 1002)
					{
						GetViewIdipClient()->OnTab(TRUE);
					}
					if (cmd == 1003)
					{
						GetDlgBarIdipClient()->OnKeyDown(VK_ADD, 1, 0);
					}
					else if (cmd == 1004)
					{
						GetDlgBarIdipClient()->OnKeyDown(VK_SUBTRACT, 1, 0);
					}
					else if (cmd > 255)
					{
						GetDlgBarIdipClient()->OnKeyDown(cmd - 255, 1, 0);
					}
					else if (pActive)
					{
						pActive->SetLastCameraControlCmd(cmd, cmdStop, pActive->GetPTZSpeed());
					}
				}
				m_LastCmd = cmd;
			}
			else if (m_LastCmd != CCC_INVALID)
			{
				if (m_LastCmd > 255)
				{
					GetDlgBarIdipClient()->OnKeyUp(m_LastCmd - 255, 1, 0);
				}
				else  if (pActive)
				{
					pActive->SetLastCameraControlCmd(CCC_INVALID, CCC_INVALID, pActive->GetPTZSpeed());
				}
				m_LastCmd = CCC_INVALID;
			}
		}
		pView->UnlockSmallWindows();
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
	if (GetNoOfExternalFrames())
	{
		int iMonitor;
		if (m_MonitorInfo.GetMonitorFromHandle(m_MonitorInfo.GetMonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), iMonitor))
		{
			if (iMonitor != 0)
			{
//				WINDOWPLACEMENT wpl = {0};
//				bool bMove = true;
//				wpl.length = sizeof(WINDOWPLACEMENT);
//				GetWindowPlacement(&wpl);
//				if (wpl.showCmd == SW_SHOWMAXIMIZED)
//				{
//					bMove = false;
//					ShowWindow(SW_RESTORE);
//				}
				SetWindowMonitor();
			}
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	m_MonitorInfo.Init();
	if (lParam == 0)
	{
		theApp.m_dwMonitorFlags = wParam;
	}
	SetWindowMonitor();
	int i, nFrames = GetNoOfExternalFrames();
	for (i=0; i<nFrames; i++)
	{
		CDlgExternalFrame *pWnd = GetExternalFrame(i);
		if (pWnd)
		{
			pWnd->SetWindowSize();
		}
	}	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	int nSCmd = nID&0xfff0;
    if(nSCmd == SC_SCREENSAVE)
	{
		TRACE(_T("SC_SCREENSAVE\n"));
	}
//	TRACE(_T("sCmd:%x\n"), nSCmd);

	if (nID == ID_VIEW_MENU)
	{
		PostCommand(ID_VIEW_MENU);
	}

	if (CSkinDialog::UseOEMSkin())
	{
		switch (nSCmd)
		{
			case SC_MAXIMIZE: case SC_RESTORE:
				ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
				CFrameWnd::OnSysCommand(nID, lParam);
				ModifyStyle(WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU, 0);
				return;// FALLTHROUGH
			case SC_MINIMIZE:
				ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
				CFrameWnd::OnSysCommand(nID, lParam);
				GetSystemMenu(FALSE)->SetDefaultItem(GetStyle()&WS_MAXIMIZE ? SC_MAXIMIZE : SC_RESTORE, MF_BYCOMMAND);
				return;// FALLTHROUGH
			case SC_MOVE:
				if (GetStyle() & WS_MAXIMIZE) return;
				break;
		}
	} 

	if (nSCmd == SC_CLOSE)
	{
		// Avoid action
		// PostCommand(ID_APP_EXIT);
	}
	else
	{
		CFrameWnd::OnSysCommand(nID, lParam);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnNcPaint()
{
	if (CSkinDialog::UseOEMSkin())
	{
		CWindowDC dc(this);
		CSkinDialog::DrawNCArea(this, &dc, GetActiveWindow()== this, CSkinDialog::GetBaseColor(), CSkinDialog::GetTextColor(), m_nStyle);
	}
	else
	{
		CFrameWnd::OnNcPaint();
		// Do not call CFrameWnd::OnNcPaint() for painting messages
		// RKE: why not
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnNcActivate(BOOL bActive)
{
	if (CSkinDialog::UseOEMSkin())
	{
		CWindowDC dc(this);
		CSkinDialog::DrawNCArea(this, &dc, bActive, CSkinDialog::GetBaseColor(), CSkinDialog::GetTextColor(), m_nStyle);
		return bActive ? FALSE : TRUE;
	}
	else
	{
		BOOL bReturn = CFrameWnd::OnNcActivate(bActive);
		return bReturn;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	CFrameWnd::OnNcMouseMove(nHitTest, point);
	CSkinDialog::NcMouseMove(this, point, m_nStyle, CSkinDialog::GetBaseColor(), CSkinDialog::GetTextColor());
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnNcMouseLeave(WPARAM wParam, LPARAM lParam)
{
	CSkinDialog::NcMouseLeave(this, m_nStyle, CSkinDialog::GetBaseColor(), CSkinDialog::GetTextColor());
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	CSkinDialog::NcLButtonDown(this, nHitTest, point, m_nStyle);
	CFrameWnd::OnNcLButtonDown(nHitTest, point);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if (CSkinDialog::UseOEMSkin())
	{
		if (   nHitTest == HTCAPTION 
			&& !(GetStyle() & WS_MAXIMIZE))
		{
			PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
		}
	}
	CFrameWnd::OnNcLButtonDblClk(nHitTest, point);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy()
{
	if (m_pSkinMenu)
	{
		m_pSkinMenu->ConvertMenu(FALSE);
		m_pSkinMenu->Detach();
	}
	theApp.m_pDlgLogin = NULL;
	KillTimer(m_nTimerOneSec);
	KillToolTipTimer();
	CFrameWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT pDIS)
{
	if (pDIS->CtlType == ODT_MENU && m_pSkinMenu)
	{
		m_pSkinMenu->OnDrawItem(nIDCtl, pDIS);
		return;
	}

	CFrameWnd::OnDrawItem(nIDCtl, pDIS);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT pMIS)
{
	if (pMIS->CtlType == ODT_MENU && m_pSkinMenu)
	{
		m_pSkinMenu->OnMeasureItem(nIDCtl, pMIS);
		pMIS->itemHeight += 2;
		return;
	}

	CFrameWnd::OnMeasureItem(nIDCtl, pMIS);
}
/////////////////////////////////////////////////////////////////////////////
CSize CALLBACK CMainFrame::DrawMenuItemImageFkt(CDC*pDC, CPoint ptLT, UINT nItemID)
{
	if (theApp.m_bShowMenuToolTips == 2)
	{
		CMainFrame*pMF = theApp.GetMainFrame();
		if (pMF)
		{
			int nIndex = pMF->m_wndToolBar.CommandToIndex(nItemID);
			if (nIndex!= -1)
			{
				if (pDC == NULL)
				{
					return CSize(25, 24);
				}
				else
				{
					UINT nStyle, nIDn;
					int nImage;
					pMF->m_wndToolBar.GetButtonInfo(nIndex, nIDn, nStyle, nImage);
					ptLT.x += 2;
					ptLT.y += 2;
					pMF->m_wndToolBar.GetToolBarCtrl().GetImageList()->Draw(pDC, nImage, ptLT, ILD_TRANSPARENT);
					return CSize(25, 24);
				}
			}
		}
	}
	//TRACE(_T("DrawMenuItemImageFkt\n"));
	return CSize(0,0);
}
/////////////////////////////////////////////////////////////////////////////
CImageList* CMainFrame::GetToolBarImageListFromID(UINT nID, int& nImage)
{
	if (m_wndToolBar.m_hWnd)
	{
		int nIndex = m_wndToolBar.CommandToIndex(nID);
		if (nIndex!= -1)
		{
			UINT nStyle, nIDn;
			m_wndToolBar.GetButtonInfo(nIndex, nIDn, nStyle, nImage);
			CImageList *pIL = m_wndToolBar.GetToolBarCtrl().GetImageList();
			if (pIL)
			{
				if (m_pToolBarImages == NULL)
				{
					m_pToolBarImages = new CImageList;
					m_pToolBarImages->Create(pIL);
				}
				return m_pToolBarImages;
			}
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewObjectView()
{
	OnUser(MAKELONG(WM_CLOSE, !m_wndSplitterObjectViews.IsObjectViewVisible()), (LPARAM)&m_wndSplitterObjectViews);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewObjectView(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndSplitterObjectViews.IsObjectViewVisible());
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewOemdialog()
{
	CDlgInView *pDB = m_wndSplitterObjectViews.GetViewOEMDlg()->GetDlgInView();
	OnUser(MAKELONG(WM_CLOSE, m_wndSplitterObjectViews.IsOEMDialogVisible() ? 0 : 1), (LPARAM)pDB);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewOemdialog(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndSplitterObjectViews.IsOEMDialogVisible());
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewDialogbar()
{
	CDlgInView *pDB = m_wndSplitterHorz.GetViewDlgBar()->GetDlgInView();
	OnUser(MAKELONG(WM_CLOSE, pDB->IsWindowVisible() ? 0 : 1), (LPARAM)pDB);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewDialogbar(CCmdUI *pCmdUI)
{
// RKE 16:01 changes
//	BOOL bVisible = m_wndSplitterHorz.GetViewDlgBar()->IsDlgVisible();
//	pCmdUI->Enable();
//	pCmdUI->SetCheck(bVisible);
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndSplitterVert.IsDialogBarVisible());
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateCloseDlgBar(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 0)
	{
		CFrameWnd::OnTimer(nIDEvent);
		return;
	}
	if (   m_bSplitterWndCreated
		&& m_nTimerOneSec == nIDEvent)
	{
		theApp.m_dwIdleTickCount = GetTickCount();
		theApp.CalcCPUusage();
		GetViewIdipClient()->SendMessage(WM_TIMER, nIDEvent, 0);
		GetViewArchive()->SendMessage(WM_TIMER, nIDEvent, 0);
		GetViewOEMDlg()->GetDlgInView()->SendMessage(WM_TIMER, nIDEvent, 0);
		m_nTimerCounter++;
		if (m_nTimerCounter % 5 == 0)	// alle 5 Sekunden prüfen
		{
			if (	theApp.m_bConnectToLocalServer == DO_CONNECT_LOCAL
				&&	theApp.IsValidUserLoggedIn()
				&&	GetDocument()->GetLocalServer() == NULL)
			{
				PostCommand(ID_CONNECT_LOCAL);
			}
			((CServers*)&GetDocument()->GetServers())->CheckDatabaseConnections();
			theApp.CheckPerformanceLevel();
		}
		else
		{
			CWndMpeg4::CheckPerformanceLevel(0);
		}
		if ((m_nTimerCounter & 0x0001ffff) == 0x0001ffff) // alle 36 min, 25 sec
		{
			((CServers*)&GetDocument()->GetServers())->UpdateTimeOffsets();
		}
		if (HasStatusBar())
		{
			SetPanetext(10, CWndMpeg4::GetPerformanceLevelString());
		}

		GetViewDlgBar()->GetDlgInView()->SendMessage(WM_TIMER, nIDEvent, 0);
	}
	else if (m_nTootipTimer == nIDEvent)
	{
		KillToolTipTimer();

		MSG msg = { m_hWnd, WM_MOUSEMOVE, 0, 0, 0, {0,0}};
		GetCursorPos(&msg.pt);
		CPoint pt = msg.pt;
		ScreenToClient(&pt);
		msg.lParam = MAKELONG(pt.x, pt.y);
		FilterToolTipMessage(&msg);

		if ((m_nFlags & WF_TRACKINGTOOLTIPS) == 0)
		{
			AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
			CToolTipCtrl* pToolTip = pModuleThreadState->m_pToolTip;
			if (pToolTip && pToolTip->m_hWnd)
			{
				if (m_hTooltipMenu)
				{
					CRect rc, rcTT, rcM;
					CPoint ptTT;
					pToolTip->GetClientRect(&rcTT);
					CString s;
					UINT nID = m_nIDHelp;
					CSkinMenu sm;
					sm.Attach(m_hTooltipMenu);
					CMenu *pMenu = sm.FindMenuString(nID, s);
					if (!pMenu)
					{
						pMenu = &sm;
					}
					GetMenuItemRect(m_hWnd, pMenu->m_hMenu, nID, &rc);
					GetCursorPos(&pt);
					pt.x += rc.Width();
					int iMonitor;
					if (m_MonitorInfo.GetMonitorFromHandle(CMonitorInfo::GetMonitorFromPoint(pt, 0), iMonitor))
					{
						m_MonitorInfo.GetMonitorRect(iMonitor, rcM);
						pt.x += rcTT.right;
						if (rcM.PtInRect(pt))
						{
							pt.x -= rcTT.right;
						}
						else
						{
							pt.x = pt.x - (rc.Width() * 3 + rcTT.right);
						}
					}
					sm.Detach();
				}
				pToolTip->SendMessage(TTM_TRACKACTIVATE, TRUE, (LPARAM)&pModuleThreadState->m_lastInfo);
				::SetWindowPos(pToolTip->m_hWnd, NULL, pt.x, pt.y, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOOWNERZORDER);
			}
		}

	}

	CFrameWnd::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ResetTimerCounter()
{
	m_nTimerCounter = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewMenu()
{
	theApp.m_bShowMainMenu = !theApp.m_bShowMainMenu;
	ShowMenu(theApp.m_bShowMainMenu);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewMenu(CCmdUI *pCmdUI)
{
	BOOL bEnable = (theApp.m_pDlgLogin == NULL);
	CIdipClientDoc *pDoc = GetDocument();
	if(pDoc)
	{
		if (!theApp.m_bShowAlarmMenu)
		{
			CServer *pServer = pDoc->GetActiveServer();
			if(pServer)
			{
				bEnable &= !pServer->IsAlarm();
			}
		}
		if (IsIconic())
		{
			bEnable = FALSE;
		}
	}

	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(theApp.m_bShowMainMenu);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ActivateFrame(int nCmdShow)
{
	if (CSkinDialog::UseOEMSkin()) ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU);
	CFrameWnd::ActivateFrame(nCmdShow);
	if (CSkinDialog::UseOEMSkin()) ModifyStyle(WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU, 0);
	if (nCmdShow == SW_SHOWMAXIMIZED)
	{
		m_wndSplitterVert.UpdateSizes(); 
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::RecalcLayout(BOOL bNotify)
{
//	TRACE(_T("CMainFrame::RecalcLayout\n"));
	// GF This function is still called after OnClose() (BTW: WHY? ...)
	// when the mainframe window and its childs (splitter!) are already closed
	// resp. on their way to the trash ...
	// Calling base class or updating splitters in that case causes an GP
	// resp. memory leaks because it bypasses the ExitInstance

	// TODO gf MIFT! keine Auswirkung, in Arbeit

	if (   m_bSplitterWndCreated
//		&& WK_IS_WINDOW(this)
		&& !(theApp.m_dwClosing & APP_IS_CLOSING)
		)
	{
		CViewIdipClient* pVV = GetViewIdipClient();
		pVV->LockSmallWindowResizing(2);
		// CMainFrame has to be at least in minimal size in window mode
		CRect rcClient;
		GetClientRect(rcClient);
		int nWidthTopBottom = 0;
		int nWidthObjectView = 0;
		if (IsZoomed() == FALSE)
		{
			CRect rcWnd;
			GetWindowRect(rcWnd);
			// Get the minimum required width of both SplitterWnds
			m_wndSplitterVert.GetWidths(-1, nWidthTopBottom, nWidthObjectView);
			int iBorderAndGap = m_wndSplitterVert.GetGapAndBorderWidth();
			int iDiff = rcClient.Width() - (nWidthTopBottom + nWidthObjectView + iBorderAndGap);
			if (iDiff < 0)
			{
				rcWnd.right -= iDiff;
				SetWindowPos(
							NULL,
							NULL,
							NULL,
							rcWnd.Width(),
							rcWnd.Height(),
							SWP_NOZORDER|SWP_NOMOVE 
							);
			}
		}
		else
		{
//			TRACE(_T("CMainFrame::RecalcLayout ZOOMED\n"));
		}

		// Get the real width of both SplitterWnds
		GetClientRect(rcClient);
		m_wndSplitterVert.GetWidths(rcClient.Width(), nWidthTopBottom, nWidthObjectView);

		if (   m_wndToolBar.m_hWnd
			&& m_wndToolBar.IsVisible()
			)
		{
			CRect rcToolBar;
			m_wndToolBar.GetWindowRect(rcToolBar);
			rcClient.bottom -= rcToolBar.Height();
//			TRACE(_T("CMainFrame::RecalcLayout reduce rcToolBar %d\n"), rcToolBar.Height());
		}

		if (   m_pwndSyncCoolBar
			&& m_pwndSyncCoolBar->IsVisible()
			)
		{
			CRect rcSyncCoolBar;
			m_pwndSyncCoolBar->GetWindowRect(rcSyncCoolBar);
			rcClient.bottom -= rcSyncCoolBar.Height();
//			TRACE(_T("CMainFrame::RecalcLayout reduce rcSyncCoolBar %d\n"), rcSyncCoolBar.Height());
		}
//		m_wndSplitterVert.SendMessage(WM_SIZE, SIZE_UPDATE, MAKELPARAM(rcClient.Width(), rcClient.Height()));
//		m_wndSplitterHorz.SendMessage(WM_SIZE, SIZE_UPDATE, MAKELPARAM(rcClient.Width(), rcClient.Height()));

		// First calculate the layout ...
//		TRACE(_T("CMainFrame::RecalcLayout call base class\n"));
		CFrameWnd::RecalcLayout(bNotify);

		// .. then update child splitters
		m_wndSplitterVert.UpdateSizes();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ShowMenu(BOOL bShow)
{
	BOOL bShowMenu;
	if (theApp.m_bShowAlarmMenu)
	{
		//on alarm connection show main menu or not
		bShowMenu = theApp.m_bShowMainMenu && bShow;
	}
	else
	{
		bShowMenu = theApp.m_bShowMainMenu && theApp.IsValidUserLoggedIn() && bShow;
	}
	::SetMenu(m_hWnd, (bShowMenu ? m_hMenu : NULL));
}
/////////////////////////////////////////////////////////////////////////////
// deletes the submenu that contains nIDFirstItem
// note, it deletes only submenues of the first level
BOOL CMainFrame::DeleteSubMenu(CMenu*pMenu, UINT nIDFirstItem)
{
	CString str;
	CMenu *pSub = ((CSkinMenu*)pMenu)->FindMenuString(nIDFirstItem, str);
	if (pSub)
	{
		int i, nItems = pMenu->GetMenuItemCount();
		for (i=0; i<nItems; i++)
		{
			if (pMenu->GetSubMenu(i) == pSub)
			{
				return pMenu->DeleteMenu(i, MF_BYPOSITION);
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnUpdateAllViews(WPARAM wParam, LPARAM lHint)
{
	CObject*pHint = (CObject*)wParam;
	CDocument *pDoc = GetActiveDocument();
	if (pDoc)
	{
		pDoc->UpdateAllViews(NULL, lHint, pHint);
	}
	WK_DELETE(pHint);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult =0;
	if (message == WM_SETTEXT)
	{
		CString str;
		if (theApp.IsReadOnlyModus())
		{
			lParam = (LPARAM)theApp.m_pszAppName;
		}
		else
		{
			str = COemGuiApi::CalcProductName();
			lParam = (LPARAM)(LPCTSTR)str;
		}
		lResult = CFrameWnd::WindowProc(message, wParam, lParam);
		return lResult;
	}
	return CFrameWnd::WindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	TRACE(_T("CMainFrame::OnClose\n"));
	theApp.m_dwClosing |= APP_IS_CLOSING;

	CFrameWnd::OnClose();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnUserLoginAlarm(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1 && theApp.m_pDlgLogin)
	{
		theApp.LoginOK();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnLaucherApplication(WPARAM wParam, LPARAM lParam)
{
	WK_ApplicationId wai = (WK_ApplicationId) wParam;
	WORD wStartParam = LOWORD((DWORD)lParam);
	WORD wIsRunning  = HIWORD((DWORD)lParam);
	if (   wai == WAI_SUPERVISOR 
		&& wStartParam == LAUNCHER_CLOSE_APPLICATION
		&& !wIsRunning)
	{
		Sleep(50);
		PostCommand(ID_NEW_LOGIN);
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnHostCameraMap() 
{
	WORD wServerID = SECID_NO_SUBID;
	CView*pView = GetSelectedServerAndView(wServerID);;

	if (pView)
	{
		pView->GetDocument()->UpdateAllViews(pView, MAKELONG(VDH_CAMERA_MAP, wServerID),NULL);
	}
}
////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateHostCameraMap(CCmdUI* pCmdUI) 
{
	if (   !theApp.IsReadOnlyModus()
		&& theApp.AllowHTMLmaps()
		&& theApp.m_sVersion>=_T("4.1"))
	{
		CString sFile;
		BOOL bShowMap = FALSE;
		WORD wServerID = SECID_NO_SUBID;
		CView*pView = GetSelectedServerAndView(wServerID);
		if (pView)
		{
			CSecID secID (SECID_LOCAL_HOST); // Local Host
			CServer* pServer = ((CIdipClientDoc*)pView->GetDocument())->GetServer(wServerID);
			if (pServer)
			{
				secID = pServer->GetHostID();
				sFile.Format(_T("%s\\%08lx\\%s"), CNotificationMessage::GetWWWRoot(), secID, MAIN_CAMERAMAP_FILE_NAME);
				if(DoesFileExist(sFile))
				{
					bShowMap = TRUE;
				}
			}
		}
		pCmdUI->Enable(bShowMap);
	}
	else 
	{
		pCmdUI->Enable(FALSE);
	}
}
////////////////////////////////////////////////////////////////////////////
CView* CMainFrame::GetSelectedServerAndView(WORD &wServer)
{
	CViewObjectTree *pVOT = GetSplitterObjectView()->GetViewSelected();
	if (pVOT)
	{
		wServer = pVOT->GetSelectedServer();
		if (wServer != SECID_NO_SUBID)
		{
			return pVOT;
		}
	}
	
	pVOT = GetSplitterObjectView()->GetSelectedServerAndView(wServer);
	if (pVOT)
	{
		return pVOT;
	}

	CWndSmall*pSW =  GetViewIdipClient()->GetWndSmallCmdActive();
	if (pSW)
	{
		if (pSW->GetType() == WST_HTML)
		{
			ASSERT_KINDOF(CWndHTML, pSW);
			if (((CWndHTML*)pSW)->GetHTMLwndType() != HTML_TYPE_CAMERA_MAP)
			{
				return NULL;
			}
		}
		wServer = pSW->GetServerID();
		if (wServer != SECID_NO_SUBID)
		{
			return GetViewIdipClient();
		}
	}
	return NULL;
}
////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnInitMenu(CMenu* pMenu)
{
	CFrameWnd::OnInitMenu(pMenu);
}
////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	if (pPopupMenu == GetSystemMenu(FALSE))
	{
		m_SystemMenu.Attach(pPopupMenu->m_hMenu);
		m_SystemMenu.ConvertMenu(TRUE);
		COemGuiApi::DoUpdatePopupMenu(AfxGetMainWnd(), &m_SystemMenu, FALSE);
	}
}
////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnHostDatetime() 
{
	WORD wServer;
	CView*pView = GetSelectedServerAndView(wServer);
	if (pView)
	{
		CServer* pServer = ((CIdipClientDoc*)pView->GetDocument())->GetServer(wServer);
		if (pServer && pServer->IsConnectedLive())
		{
			COEMDateTimeDialog dlg(pView);
			if (pServer->IsLocal())
			{
				dlg.m_sHost.LoadString(IDS_LOCAL_SERVER);
			}
			else
			{
				dlg.m_sHost = pServer->GetName();
			}

			if (IDOK==dlg.DoModal())
			{
				// new check after modal dialog, keep it!
				pView = GetSelectedServerAndView(wServer);
				if (pView)
				{
					pServer = ((CIdipClientDoc*)pView->GetDocument())->GetServer(wServer);
					if (pServer && pServer->IsConnectedLive())
					{
						CIPCInputIdipClient* pInput = pServer->GetInput();
						pInput->DoRequestSetSystemTime(dlg.GetSystemTime());
					}
				}
			}
		}
	}	
}
////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateHostDatetime(CCmdUI* pCmdUI) 
{
	WORD wServer = SECID_NO_SUBID;
	pCmdUI->Enable(   !theApp.IsReadOnlyModus()
				   && GetSelectedServerAndView(wServer)
				   && theApp.m_pPermission
				   && (theApp.m_pPermission->GetFlags() & WK_ALLOW_DATE_TIME));
}
////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnHostLoadSequence() 
{
	WORD wServer;
	if (GetSelectedServerAndView(wServer))
	{
		GetViewCamera()->OnHostLoadSequence(wServer);
	}
}
////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnHostSaveSequence() 
{
	WORD wServer;
	if (GetSelectedServerAndView(wServer))
	{
		GetViewCamera()->OnHostSaveSequence(wServer);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CMainFrame::GetNoOfMonitors() const
{
	if (   theApp.m_pDongle 
		&& theApp.m_pDongle->AllowMultiMonitor()
		&& m_MonitorInfo.GetNrOfMonitors())
	{
		return m_MonitorInfo.GetNrOfMonitors();
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::GetMonitorRect(int i, CRect &rc, bool bWorkingArea/*=false*/)
{
	return m_MonitorInfo.GetMonitorRect(i, rc, bWorkingArea);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnKeyboardChanged(WPARAM wParam, LPARAM lParam)
{
	if (wParam != NULL && m_bSplitterWndCreated)
	{
		return GetDlgBarIdipClient()->OnKeyboardChanged((CKBInput*)wParam);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::RegisterHotKey(UINT uID)
{
	BOOL bRet;
	bRet = ::RegisterHotKey(m_hWnd, uID, 0, uID);
	if (!bRet)
	{
		WK_TRACE(_T("RegisterHotKey(%d) Error %d\n"), uID, GetLastError());
	}
	else
	{
		bRet = SendMessage(WM_SETHOTKEY, uID, 0);
		if (bRet != 1)
		{
			WK_TRACE(_T("%d = WM_SETHOTKEY(%d) Error %d\n"), bRet, uID, GetLastError());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		// TODO! RKE: Implement Commands
//		case VK_VOLUME_DOWN: break;
//		case VK_VOLUME_UP: break;
//		case VK_VOLUME_MUTE: break;

		// CWndHTML navigation, but which window?
//		case VK_BROWSER_HOME: break;
//		case VK_BROWSER_REFRESH: break;

//		case VK_BROWSER_STOP: break;
//		case VK_BROWSER_SEARCH: break; // Database Search?
//		case VK_BROWSER_FAVORITES: break; // Open Host List?

		case VK_MEDIA_STOP: PostCommand(ID_NAVIGATE_EJECT); break;
		case VK_BROWSER_BACK: PostCommand(ID_NAVIGATE_SKIP_BACK); break;
		case VK_BROWSER_FORWARD: PostCommand(ID_NAVIGATE_SKIP_FORWARD); break;
		case VK_MEDIA_NEXT_TRACK: PostCommand(ID_NAVIGATE_NEXT); break;
		case VK_MEDIA_PREV_TRACK: PostCommand(ID_NAVIGATE_PREV); break;
		case VK_MEDIA_PLAY_PAUSE: PostCommand(ID_NAVIGATE_FORWARD); break;

// TODO! RKE: Launch maintenance programs ?
//		case VK_LAUNCH_MAIL: break;
//		case VK_LAUNCH_MEDIA_SELECT: break;
//		case VK_LAUNCH_APP1: break;
//		case VK_LAUNCH_APP2: break;

		case VK_SLEEP: PostCommand(ID_EXIT_CLIENT); break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTab()
{
	CView *pView = GetActiveView();
	CViewIdipClient*pVIC = GetViewIdipClient();
	BOOL bNext   = GetKeyState(VK_SHIFT)   & 0x80 ? FALSE : TRUE;
	BOOL bSelect = GetKeyState(VK_CONTROL) & 0x80 ? TRUE : FALSE;

	if (pVIC == pView)
	{
		if (bSelect)
		{
			pVIC->OnTab(bNext);
		}
		else
		{
			m_wndSplitterObjectViews.OnTab(bNext ? -1 : -2, bSelect);
		}
	}
	else if (!m_wndSplitterObjectViews.OnTab(bNext, bSelect))
	{
		SetActiveView(pVIC);	
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnISDNBchannels(WPARAM wParam, LPARAM lParam)
{
	WK_ApplicationId wai = (WK_ApplicationId) HIWORD(wParam);
	WORD wCurrent = LOWORD(lParam);
	WORD wOther   = HIWORD(lParam);
	WORD wTotal   = LOWORD(wParam);
	CString sText;
	if (wOther)
	{
		if (wai == WAI_ISDN_UNIT)
		{
			sText.Format(_T("%d|%d"), wCurrent, wOther);
		}
		else
		{
			sText.Format(_T("%d|%d"), wOther, wCurrent);
		}
	}
	else
	{
		sText.Format(_T("%d"), wTotal);
	}
	GetDlgBarIdipClient()->OnISDNBchannels(wTotal, sText);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::PostCommand(UINT nID)
{
	PostMessage(WM_COMMAND, nID, (LPARAM)m_hWnd);
}
#define IDC_EDIT_LOGIN_USER             5084
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	CFrameWnd::OnSetFocus(pOldWnd);

	if (   theApp.m_pDlgLogin 
		&& theApp.m_pDlgLogin->m_hWnd 
		&& ::GetDlgItem(theApp.m_pDlgLogin->m_hWnd, IDC_EDIT_LOGIN_USER) != ::GetFocus())
	{
		theApp.m_pDlgLogin->SetFocus();
//		theApp.m_pDlgLogin->PostMessage(WM_USER, WM_SHOWWINDOW);
//		TRACE(_T("CMainFrame::OnSetFocus(%x) %x\n"), pOldWnd, this);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetPanetext(int nPane, LPCTSTR sText)
{
	if (m_wndStatusBar.m_hWnd && nPane < m_wndStatusBar.GetCount())
	{
		if (theApp.IsInMainThread())
		{
			m_wndStatusBar.SetPaneText(nPane, sText);
		}
		else
		{
			PostMessage(WM_USER, MAKELONG(WPARAM_PANE_TXT, nPane), (LPARAM)_tcsdup(sText));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateDummy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::InitJoystick()
{
	if (m_pJoyStick)
	{
		FreeJoystick();
	}
	CWK_Profile wkp;
	m_nZeroRange = wkp.GetInt(SECTION_COMMON, COMMON_JOYSTICK_ZERO_RANGE, m_nZeroRange);

	m_pJoyStick = new CJoyStickDX;
	int nRange = 10 + m_nZeroRange;
	m_pJoyStick->SetAxisRange(-nRange, nRange);
	if (S_OK == m_pJoyStick->Init(m_hWnd, DISCL_EXCLUSIVE|DISCL_FOREGROUND, 0))
	{
		m_pJoyState = new DIJOYSTATE;
		m_bControlSpeed = wkp.GetInt(SECTION_COMMON, COMMON_JOYSTICK_CONTROLSPEED, m_bControlSpeed);
		ZeroMemory(m_pJoyState, sizeof(DIJOYSTATE));
		UINT nSize = 0;
		BYTE *pData = NULL;
		if (wkp.GetBinary(SECTION_COMMON, COMMON_JOYSTICK_BUTTONS, (LPBYTE*)&pData, &nSize))
		{
			CopyMemory((void*)&m_nButtonMap[0], pData, sizeof(m_nButtonMap));
		}
		else
		{
			int i;
			for (i=0; i<MAX_JOYSTICK_BUTTONS; i++)
			{
				m_nButtonMap[i] = i;
			}
		}
		WK_DELETE(pData);
	}
	else
	{
		FreeJoystick();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::FreeJoystick()
{
	WK_DELETE(m_pJoyState);
	WK_DELETE(m_pJoyStick);
	m_dwJoyPTZspeed = 0;
	m_LastCmd = CCC_INVALID;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileNew()
{
	theApp.LoadSettings();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnResetAutoLogout(WPARAM, LPARAM)
{
	theApp.ResetAutoLogout();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::KillToolTipTimer()
{
	if (m_nTootipTimer)
	{
		KillTimer(m_nTootipTimer);
	}
	m_nTootipTimer = 0;
}
