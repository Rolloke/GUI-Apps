// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Recherche.h"

#include "MainFrm.h"

#include "RechercheDoc.h"
#include "RechercheView.h"
#include "ObjectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////
// CMyCoolBar
//
IMPLEMENT_DYNAMIC(CMainCoolBar, CCoolBar)

////////////////
// This is the virtual function you have to override to add bands
//
BOOL CMainCoolBar::OnCreateBands()
{
	// Create tool bar
	if (!m_wndToolBar.Create(GetParent(),
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
			CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC|CBRS_HIDE_INPLACE))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE; // failed to create
	}
	
	m_wndToolBar.GetToolBarCtrl().SetButtonSize(CSize(22,22));
	m_wndToolBar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	m_ilHot.Create(IDB_HOT, 20, 0, RGB(255, 0, 255));
	m_wndToolBar.SetHotImageList(&m_ilHot);

	m_ilCold.Create(IDB_COLD, 20, 0, RGB(255, 0, 255));
	m_wndToolBar.SetImageList(&m_ilCold);
	
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT);
	if (theApp.GetMapURL().IsEmpty())
	{
		m_wndToolBar.SetButtons(NULL, 15);
	}
	else
	{
		m_wndToolBar.SetButtons(NULL, 16);
	}

	int i,b;
	i = 0;
	b = 0;
	// set up each toolbar button
	m_wndToolBar.SetButtonInfo(i++, ID_CONNECT, TBSTYLE_BUTTON|TBSTYLE_DROPDOWN, b++, 1);
	if (!theApp.GetMapURL().IsEmpty())
	{
		m_wndToolBar.SetButtonInfo(i++, ID_CONNECT_MAP, TBSTYLE_BUTTON, b++, 1);
	}
	else
	{
		b++;
	}
	m_wndToolBar.SetButtonInfo(i++, ID_DISCONNECT, TBSTYLE_BUTTON|TBSTYLE_DROPDOWN, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 10/*Pixel Width*/, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_SEARCH, TBSTYLE_BUTTON, b++, 1);

	m_wndToolBar.SetButtonInfo(i++, ID_FILE_PRINT, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_FILE_DISK, TBSTYLE_BUTTON|TBSTYLE_DROPDOWN, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_FILE_OPEN_DISK, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 10/*Pixel Width*/, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_1, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_2_2, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_3_3, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_FULLSCREEN, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_ORIGINAL, TBSTYLE_BUTTON, b++, 1);

	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 10/*Pixel Width*/);
	m_wndToolBar.SetButtonInfo(i++, ID_APP_ABOUT, TBSTYLE_BUTTON, b++, 1);

	// Get minimum size of bands
	CSize szHorz = m_wndToolBar.CalcDynamicLayout(-1, 0);	      // get min horz size
	CSize szVert = m_wndToolBar.CalcDynamicLayout(-1, LM_HORZ);	// get min vert size

	CSize sFixed = m_wndToolBar.CalcFixedLayout(FALSE,TRUE);

	// Band 1: Add toolbar band
	CRebarBandInfo rbbi;

	
	rbbi.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_COLORS;
	rbbi.fStyle = 0;
	rbbi.hwndChild = m_wndToolBar;
	rbbi.cx = 0;
	rbbi.cxMinChild = sFixed.cx;//szHorz.cx+4;
	rbbi.cyMinChild = 40;
	rbbi.clrFore = GetSysColor(COLOR_BTNTEXT);
	rbbi.clrBack = GetSysColor(COLOR_BTNFACE);
	if (!InsertBand(0, &rbbi))
	{
		WK_TRACE_ERROR(_T("cannot insert toolbar band"));
		return FALSE;
	}


	// Create the Logo
	CBitmap bm;
	bm.LoadBitmap(COemGuiApi::GetOEMLogoId());
	CSize bitmapSize;
	BITMAP bmInfo;
	VERIFY(bm.GetObject(sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
	CRect logorect(0,0,bmInfo.bmWidth,bmInfo.bmHeight);
	m_wndButton.Create(_T("Dummy"),WS_VISIBLE|WS_DISABLED|WS_CHILD|BS_OWNERDRAW,
						logorect,GetParent(),AFX_IDW_TOOLBAR+2);
	m_wndButton.LoadBitmaps(COemGuiApi::GetOEMLogoId());
	m_wndButton.SizeToContent();

	rbbi.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE;
	rbbi.fStyle = RBBS_FIXEDSIZE;
	rbbi.hwndChild = m_wndButton;
	rbbi.cxMinChild = logorect.Width();
	rbbi.cyMinChild = logorect.Height();
	if (!InsertBand(1, &rbbi))
	{
		WK_TRACE_ERROR(_T("cannot insert logo band"));
		return FALSE;
	}

   m_dwStyle |= CBRS_FLYBY; // Für Tooltips in der Statusbar
	return 0; // OK
}

int CMainCoolBar::HitTest()
{
   m_wndToolBar.m_nStateFlags &= ~statusSet;
   return m_wndToolBar.HitButton(true);
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_VIEW_FULLSCREEN, OnViewFullscreen)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULLSCREEN, OnUpdateViewFullscreen)
	ON_COMMAND(ID_SYNC, OnSync)
	ON_UPDATE_COMMAND_UI(ID_SYNC, OnUpdateSync)
	ON_COMMAND(ID_ARCHIV_BACKUP, OnArchivBackup)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
   ON_COMMAND_RANGE(ID_CONNECT_LOW, ID_CONNECT_HIGH, OnConnect)
   ON_COMMAND_RANGE(ID_DISCONNECT_LOW, ID_DISCONNECT_HIGH, OnDisconnect)
   ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)
   ON_WM_DEVICECHANGE()
   ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_pRechercheView = NULL;
	m_pObjectView = NULL;
	m_bFullScreen = FALSE;
	m_bFullScreenChanging = FALSE;
	m_hMenu = NULL;
	memset(&m_wp,0,sizeof(m_wp));
	m_wp.length = sizeof(m_wp);
	m_iObjectWidth = 0;
	m_pwndSyncCoolBar = NULL;
}

CMainFrame::~CMainFrame()
{
	WK_DELETE(m_pwndSyncCoolBar);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (COemGuiApi::GetComCtl32Version()==COMCTL32_471)
	{
		if (!m_wndMainCoolBar.Create(this,
			WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
				RBS_TOOLTIPS|RBS_BANDBORDERS|RBS_VARHEIGHT)) {
			TRACE0("Failed to create cool bar\n");
			return -1;      // fail to create
		}
	}
	else
	{
		if (!m_wndToolBar.Create(this) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
			TRACE0("Failed to create tool bar\n");
			return -1;      // fail to create
		}
		m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
			CBRS_ALIGN_TOP | CBRS_TOOLTIPS);
		m_wndToolBar.SetHeight(40);
		TBBUTTON tb;
		CString s;
		int i,c;
		c = m_wndToolBar.GetToolBarCtrl().GetButtonCount();
		for (i=0;i<c;i++)
		{
			if (m_wndToolBar.GetToolBarCtrl().GetButton(i,&tb))
			{
				if (tb.idCommand != ID_SEPARATOR)
				{
					s.LoadString(tb.idCommand);
					s = s.Left(s.Find(_T('\n')));
					m_wndToolBar.SetButtonText(i,s);
				}
			}
		}
		m_wndToolBar.GetToolBarCtrl().SetButtonSize(CSize(72,42));
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

// TODO gf CoolMenuManager funzt nicht mit MFC 7.1
#if _MFC_VER < 0x0070
	m_CoolMenuManager.m_bAutoAccel = FALSE;
	m_CoolMenuManager.Install(this);
	m_CoolMenuManager.LoadToolbar(IDR_TOOLBARMENU);
#endif

	RecalcLayout();

	CMenu* pMenu = GetMenu();
	if (pMenu)
	{
		CMenu* pPopupArchive = pMenu->GetSubMenu(4);
#ifdef _DTS
		CMenu* pPopupConnection = pMenu->GetSubMenu(0);
		pPopupConnection->DeleteMenu(ID_CONNECT_MAP,MF_BYCOMMAND);
		pPopupConnection->DeleteMenu(ID_SEARCH_AT_START,MF_BYCOMMAND);
		pPopupConnection->DeleteMenu(ID_NEW_LOGIN,MF_BYCOMMAND);
		pPopupConnection->DeleteMenu(3,MF_BYPOSITION);

		CMenu* pPopupEdit = pMenu->GetSubMenu(3);
		pPopupEdit->DeleteMenu(ID_EDIT_STATION,MF_BYCOMMAND);
		pPopupEdit->DeleteMenu(ID_EDIT_COMMENT,MF_BYCOMMAND);

		pPopupArchive->DeleteMenu(ID_ARCHIV_EMPTY,MF_BYCOMMAND);
		pPopupArchive->DeleteMenu(ID_ARCHIV_DELETE,MF_BYCOMMAND);
		pPopupArchive->DeleteMenu(0,MF_BYPOSITION);


		CMenu* pPopupFile = pMenu->GetSubMenu(1);
		CMenu* pPopupSaveAs = pPopupFile->GetSubMenu(0);
		if (pPopupSaveAs)
		{
			pPopupSaveAs->DeleteMenu(ID_FILE_DISK,MF_BYCOMMAND);
			pPopupSaveAs->DeleteMenu(ID_FILE_JPX,MF_BYCOMMAND);
		}
#endif
		
		CWK_Dongle dongle;
		if (!dongle.RunCDRWriter())
		{
			pPopupArchive->DeleteMenu(ID_ARCHIV_BACKUP,MF_BYCOMMAND);
		}
	}
	return 0;
}

BOOL CMainFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	if (!m_wndSplitter.CreateStatic( this, 1, 2))
	{
        TRACE(_T("Failed to CreateStaticSplitter\n"));
        return FALSE;
	}

    // add the first splitter pane - the default view in column 0
    if (!m_wndSplitter.CreateView(0, 0,
        RUNTIME_CLASS(CObjectView), CSize(230, 0), pContext))
    {
        TRACE(_T("Failed to create first pane\n"));
        return FALSE;
    }

    if (!m_wndSplitter.CreateView(0, 1,
        RUNTIME_CLASS(CRechercheView), CSize(200, 0), pContext))
    {
        TRACE(_T("Failed to create second pane\n"));
        return FALSE;
    }
	m_pObjectView = (CObjectView*)m_wndSplitter.GetPane(0,0);
	m_pRechercheView = (CRechercheView*)m_wndSplitter.GetPane(0,1);

    // activate the input view
    SetActiveView((CView*)m_wndSplitter.GetPane(0, 0));
	if (m_MonitorInfo.GetNrOfMonitors() > 1)
	{
		SetWindowMonitor(true);
	}
	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~FWS_ADDTOTITLE;
	cs.style |= WS_MAXIMIZE;
	cs.lpszName = theApp.m_pszAppName;
	cs.lpszClass = WK_APP_NAME_DB_CLIENT;

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

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

/////////////////////////////////////////////////////////////////////////////
CRechercheDoc* CMainFrame::GetDocument()
{
	if (WK_IS_WINDOW(m_pRechercheView))
	{
		return m_pRechercheView->GetDocument();
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnAppAbout() 
{
	COemGuiApi::AboutDialog(this);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    // this function handles the dropdown menus from the toolbar
    NMTOOLBAR* pNMToolBar = (NMTOOLBAR*)pNotifyStruct;
    CRect rect;

    // translate the current toolbar item rectangle into screen coordinates
    // so that we'll know where to pop up the menu
	m_wndMainCoolBar.m_wndToolBar.SendMessage(TB_GETRECT, pNMToolBar->iItem, (LPARAM)&rect);
    m_wndMainCoolBar.m_wndToolBar.ClientToScreen(rect);

    if (pNMToolBar->iItem == ID_CONNECT)
    {
        CMenu menu;

		int i,c;
        CHostArray& ha = theApp.GetHosts();
		CHost* pHost;
		CString sText;
		CString sLocal;
		c = ha.GetSize();

		menu.CreatePopupMenu();

		sLocal.LoadString(IDS_LOCAL_SERVER);
		pHost = ha.GetLocalHost();
		sText.Format(_T("&%s %s"),sLocal,pHost->GetName());
		menu.AppendMenu(MF_STRING,
						ID_CONNECT_LOCAL,
						sText);
    

		for (i=0;i<c && i<9;i++)
		{
			pHost = ha.GetAt(i);
			if (   (pHost->GetTyp()!=_T("SMS")) 
				&& (pHost->GetTyp()!=_T("MINI B3"))
				&& (pHost->GetTyp()!=_T("TeleObserver"))
				&& (pHost->GetTyp()!=_T("FAX"))
				&& (pHost->GetTyp()!=_T("E-Mail"))
				&& (pHost->GetTyp()!=_T("MINI B6"))
				)
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
        menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left, rect.bottom, this);
	}
    else if (pNMToolBar->iItem == ID_SEARCH)
	{
		// TODO later
		CMenu menu;
		CMenu* pPopup;
		menu.LoadMenu(IDR_CONTEXT);
		pPopup = menu.GetSubMenu(6);
        pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, 
							   rect.left, 
							   rect.bottom, 
							   this);
	}
    else if (pNMToolBar->iItem == ID_DISCONNECT)
    {
		if (WK_IS_WINDOW(m_pObjectView))
		{
			m_pObjectView->PopupDisconnectMenu(CPoint(rect.left, rect.bottom));
		}
	}
    else if (pNMToolBar->iItem == ID_FILE_DISK)
    {
		CMenu menu;
		CMenu* pPopup;
		menu.LoadMenu(IDR_CONTEXT);
		pPopup = menu.GetSubMenu(4);
#ifdef _DTS
		pPopup->DeleteMenu(ID_FILE_DISK,MF_BYCOMMAND);
		pPopup->DeleteMenu(ID_FILE_JPX,MF_BYCOMMAND);
#endif
        pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, 
							   rect.left, 
							   rect.bottom, 
							   this);
	}

    *pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnConnect(UINT nID)
{
	CRechercheDoc* pDoc = GetDocument();
	CSecID idHost(SECID_GROUP_HOST,(WORD)(nID-ID_CONNECT_LOW));
	pDoc->Connect(idHost);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDisconnect(UINT nID)
{
	if (IsFullScreen())
	{
		OnViewFullscreen();
	}
	if (WK_IS_WINDOW(m_pObjectView))
	{
		m_pObjectView->DisconnectID(nID-ID_DISCONNECT_LOW);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ChangeFullScreenModus()
{
	OnViewFullscreen();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewFullscreen() 
{
	m_bFullScreen = !m_bFullScreen;
	if (m_bFullScreen)
	{
		m_bFullScreenChanging = TRUE;
		m_hMenu = ::GetMenu(m_hWnd);
		::SetMenu(m_hWnd,NULL);
		GetWindowPlacement(&m_wp);
		CRect rect;

		m_pObjectView->GetClientRect(rect);
		m_iObjectWidth = rect.Width();
		m_wndSplitter.SetColumnInfo(0,0,0);

		ShowWindow(SW_RESTORE);
		ModifyStyle(WS_OVERLAPPEDWINDOW,0);
		CRect workingRect;
		BOOL bPrimary = TRUE;
		if (m_MonitorInfo.GetNrOfMonitors() > 1)
		{
			HMONITOR hMon = m_MonitorInfo.GetMonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
			int      iMon;
			if (m_MonitorInfo.GetMonitorFromHandle(hMon, iMon) && (iMon != 0))
			{
				bPrimary = !m_MonitorInfo.GetMonitorRect(iMon, workingRect);
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
		ShowControlBar(&m_wndStatusBar,FALSE,FALSE);
		if (WK_IS_WINDOW(&m_wndMainCoolBar))
		{
			ShowControlBar(&m_wndMainCoolBar,FALSE,FALSE);
		}
/*
		if (WK_IS_WINDOW(m_pwndSyncCoolBar))
		{
			ShowControlBar(m_pwndSyncCoolBar,FALSE,FALSE);
		}
*/
		if (WK_IS_WINDOW(&m_wndToolBar))
		{
			ShowControlBar(&m_wndToolBar,FALSE,FALSE);
		}
		m_bFullScreenChanging = FALSE;
		GetClientRect(rect);
		m_pRechercheView->MoveWindow(rect);
	}
	else
	{
		m_bFullScreenChanging = TRUE;
		::SetMenu(m_hWnd,m_hMenu);
		ModifyStyle(0,WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(&m_wp);
		ShowControlBar(&m_wndStatusBar,TRUE,FALSE);
		if (WK_IS_WINDOW(&m_wndMainCoolBar))
		{
			ShowControlBar(&m_wndMainCoolBar,TRUE,FALSE);
		}
/*
		if (WK_IS_WINDOW(m_pwndSyncCoolBar))
		{
			ShowControlBar(m_pwndSyncCoolBar,TRUE,FALSE);
		}
*/
		if (WK_IS_WINDOW(&m_wndToolBar))
		{
			ShowControlBar(&m_wndToolBar,TRUE,FALSE);
		}
		m_wndSplitter.SetColumnInfo(0,m_iObjectWidth,m_iObjectWidth);
		m_bFullScreenChanging = FALSE;
		m_wndSplitter.RecalcLayout();
		UpdateWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewFullscreen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pRechercheView->GetSmallWindows()->GetSize());
	pCmdUI->SetCheck(m_bFullScreen);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (WK_IS_WINDOW(m_pObjectView))
	{
		if (m_pObjectView->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		{
			return TRUE;
		}
	}
	
	if (WK_IS_WINDOW(m_pRechercheView))
	{
		if (m_pRechercheView->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		{
			return TRUE;
		}
	}

	if (WK_IS_WINDOW(m_pwndSyncCoolBar))
	{
		if (m_pwndSyncCoolBar->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		{
			return TRUE;
		}
	}

	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnDeviceChange( UINT nEventType, DWORD dwData )
{
	if (WK_IS_WINDOW(m_pRechercheView))
	{
		CBackupDialog* pBackupDialog = m_pRechercheView->GetBackupDialog();
		if (WK_IS_WINDOW(pBackupDialog))
		{
			pBackupDialog->OnDeviceChange(nEventType,dwData);
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_DATABASE_CLIENT, 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSync() 
{
	if (WK_IS_WINDOW(m_pwndSyncCoolBar))
	{
		ShowControlBar(m_pwndSyncCoolBar,FALSE,FALSE);
		m_pwndSyncCoolBar->DestroyWindow();
		WK_DELETE(m_pwndSyncCoolBar);
		if (WK_IS_WINDOW(m_pObjectView))
		{
			m_pObjectView->ShowSyncCheckBoxes(FALSE);
		}
		if (WK_IS_WINDOW(m_pRechercheView))
		{
			m_pRechercheView->OnSmallCloseAll();
		}
	}
	else
	{
		if (WK_IS_WINDOW(m_pRechercheView))
		{
			m_pRechercheView->OnSmallCloseAll();
		}
		m_pwndSyncCoolBar = new CSyncCoolBar;
		if (!m_pwndSyncCoolBar->Create(this,
			WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
				RBS_TOOLTIPS|RBS_BANDBORDERS|RBS_VARHEIGHT)) 
		{
			TRACE(_T("Failed to create sync cool bar\n"));
		}
		if (WK_IS_WINDOW(m_pObjectView))
		{
			m_pObjectView->ShowSyncCheckBoxes();
		}
	}
	RecalcLayout();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateSync(CCmdUI* pCmdUI)
{
	// Zeitsynchrones Abspielen im Backup-Mode verboten
	BOOL bEnable =     (GetDocument()->GetServers().GetSize() > 0)
					&& (GetObjectView()->IsBackupMode() == FALSE);
	pCmdUI->Enable(bEnable);
	// der Menüpunkt wird gecheckt, der Button nicht
	if (pCmdUI->m_pMenu)
	{
		pCmdUI->SetCheck(IsSyncMode());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnIdle()
{
	if (WK_IS_WINDOW(m_pwndSyncCoolBar))
	{
		m_pwndSyncCoolBar->OnIdle();
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	if (AFX_IDW_TOOLBAR == wParam)
	{
		wParam = m_wndMainCoolBar.HitTest();
		// if no button found, check next toolbar
		if (wParam == (-1))
		{
			if (WK_IS_WINDOW(m_pwndSyncCoolBar))
			{
				wParam = m_pwndSyncCoolBar->HitTest();
			}
		}
	}
	lResult = CFrameWnd::OnSetMessageString(wParam, lParam);
	return lResult;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnArchivBackup() 
{
	// On Backup no Sync Playback
	if (WK_IS_WINDOW(m_pwndSyncCoolBar))
	{
		OnSync();
	}
	if (WK_IS_WINDOW(m_pObjectView))
	{
		m_pObjectView->OnArchivBackup();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CFrameWnd::OnShowWindow(bShow, nStatus);
	
	if (bShow)
	{
		SetWindowMonitor(true);
	}
}
///////////////////////////////////////////////////////////////////
void CMainFrame::SetWindowMonitor(bool bMove)
{
	CRechercheApp *pApp = (CRechercheApp*)AfxGetApp();
	if (!(pApp->m_dwMonitorFlags & USE_MONITOR_1))
	{
		DWORD dwFlags;
		int   nMon;
		CRect rc;
		for (nMon=1, dwFlags = 2; nMon < m_MonitorInfo.GetNrOfMonitors(); nMon++, dwFlags>>=1)
		{
			if (pApp->m_dwMonitorFlags & dwFlags)
			{
				int nCurrent = -1;;
				m_MonitorInfo.GetMonitorFromHandle(m_MonitorInfo.GetMonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), nCurrent);
				if (nCurrent != nMon)
				{
					CRect rc;
					if (bMove)
					{
						CRect rcMon, rcCurr;
						m_MonitorInfo.GetMonitorRect(nMon, rcMon);
						m_MonitorInfo.GetMonitorRect(nCurrent, rcCurr);
						GetWindowRect(&rc);
						rc.OffsetRect(rcMon.TopLeft()-rcCurr.TopLeft());
					}
					else
					{
						m_MonitorInfo.GetMonitorRect(nMon, rc, true);
					}
					SetWindowPos(&CWnd::wndTop, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOOWNERZORDER);
				}
				break;
			}
		}
	}
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (WM_DISPLAYCHANGE == message)
	{
		m_MonitorInfo.Init();
		SetWindowMonitor(false);
	}

	return CFrameWnd::WindowProc(message, wParam, lParam);
}
