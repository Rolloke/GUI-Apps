// MainFrm.h : Schnittstelle der Klasse CMainFrame
//


#pragma once
class CIdipClientDoc;

class CViewIdipClient;
class CViewDlgBar;
class CViewCamera;
class CViewArchive;
class CViewRelais;
class CViewAlarmList;

class CDlgExternalFrame;
class CDlgBarIdipClient;
class CWndSmall;
class CSplitterObjectView;

#include "SplitterTopBottom.h"
#include "SplitterLeftRight.h"
#include "SplitterObjectView.h"
#include "DlgExternalFrame.h"

#include "SyncCoolBar.h"
#include "ICStatusBar.h"


class CDlgSubstitute;


#define APP_IS_CLOSING			1
#define LAUNCHER_IS_CLOSING	2

#define FULLSCREEN_TOGGLE   -1
#define FULLSCREEN_FORCE	0x00010000
#define FULLSCREEN_POST		0x00020000

class CMainFrame : public CFrameWnd
{
	
protected: // Nur aus Serialisierung erstellen
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attribute
public:
	inline const bool	IsSplitterWndCreated() const;
	inline const BOOL	IsActive() const;
	inline const bool	IsFullScreen() const;
	inline const BOOL	IsFullScreenChanging() const; // Recherche TKR
	inline const BOOL	IsSyncMode() const;	// Recherche
	inline const BOOL	HasStatusBar() const;	// Recherche
	inline const UINT	GetOneSecTimerID() const;
	int					GetNoOfExternalFrames();
	int					GetNoOfMonitors() const;
	BOOL				GetMonitorRect(int i, CRect &rc, bool bWorkingArea=false);

	CIdipClientDoc*		GetDocument();
	CView*				GetSelectedServerAndView(WORD &wServer);
	CViewIdipClient*	GetViewIdipClient();
	CViewCamera*		GetViewCamera();
	CViewRelais*		GetViewRelais();
	CViewAlarmList*		GetViewAlarmList();
	CViewAudio*			GetViewAudio();
	CViewArchive*		GetViewArchive();
	CViewDialogBar*		GetViewDlgBar();
	CViewDialogBar*		GetViewOEMDlg();
	CDlgBarIdipClient*	GetDlgBarIdipClient();
	CSplitterObjectView*GetSplitterObjectView();
	inline CSyncCoolBar*GetSyncCoolBar();// Recherche
	CImageList*			GetToolBarImageListFromID(UINT nID, int &nImage);
	CDlgExternalFrame*	GetExternalFrame(int);

	// Überschreibungen
public:
	virtual ~CMainFrame();
	virtual void RecalcLayout(BOOL bNotify = TRUE);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual CWnd* GetMessageBar();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

	// Operationen
public:
	void	ShowMenu(BOOL bShow);
	void	ChangeFullScreenModus(BOOL bOn=FULLSCREEN_TOGGLE);
	bool	RemoveSWfromExtFrames(CWndSmall*);
	void	OnIdle();	// Recherche
	void	OnTab();
	void	RegisterHotKey(UINT uID);
	void	PostCommand(UINT nID);
	static  BOOL	DeleteSubMenu(CMenu*pMenu, UINT nIDFirstItem);
	void	SetPanetext(int nPane, LPCTSTR sText);
	void	InitJoystick();
	void	FreeJoystick();
	void	ResetTimerCounter();

protected:
	void	AddExternalFrame(int);
	void	SetWindowMonitor();
	void	KillToolTipTimer();


	// Generated message map functions
	//{{AFX_MSG(CMainFrame)
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAppAbout();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnViewFullscreen();
	afx_msg void OnUpdateViewFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnConnect(UINT nID);
	afx_msg void OnDisconnect();
	afx_msg void OnDisconnect(UINT nID);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewMonitor(UINT nID);
	afx_msg void OnUpdateViewMonitor(CCmdUI* pCmdUI);
	afx_msg LRESULT OnUserLoginAlarm(WPARAM wParam, LPARAM lParam);
#if _MFC_VER >= 0x0700
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif // _MSC_VER == 1300
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExitSizeMove(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnViewObjectView();
	afx_msg void OnUpdateViewObjectView(CCmdUI *pCmdUI);
	afx_msg void OnViewOemdialog();
	afx_msg void OnUpdateViewOemdialog(CCmdUI *pCmdUI);
	afx_msg void OnViewDialogbar();
	afx_msg void OnUpdateViewDialogbar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCloseDlgBar(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnViewMenu();
	afx_msg void OnUpdateViewMenu(CCmdUI *pCmdUI);
	afx_msg LRESULT OnUpdateAllViews(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLaucherApplication(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnHostCameraMap();
	afx_msg void OnUpdateHostCameraMap(CCmdUI* pCmdUI);
	afx_msg void OnHostDatetime();
	afx_msg void OnUpdateHostDatetime(CCmdUI* pCmdUI);
	afx_msg void OnHostLoadSequence();
	afx_msg void OnHostSaveSequence();
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg LRESULT OnSetMenuToolInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnKeyboardChanged(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnDeviceChange( UINT nEventType, DWORD dwData );
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnISDNBchannels(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnFileNew();
	afx_msg LRESULT OnResetAutoLogout(WPARAM, LPARAM);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
// Recherche
	afx_msg void OnArchivBackup();
	afx_msg void OnSync();
	afx_msg void OnUpdateSync(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDummy(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	static BOOL CALLBACK KillScreenSaverFunc(HWND hwnd, LPARAM lParam);
	static CSize CALLBACK DrawMenuItemImageFkt(CDC*pDC, CPoint ptLT, UINT nItemID);

protected:  // Eingebundene Elemente der Steuerleiste
	CToolBar		m_wndToolBar;
	CICStatusBar	m_wndStatusBar;
	HMENU			m_hMenu;
	CSkinMenu		m_SystemMenu;

	CSplitterLeftRight  m_wndSplitterVert;
	CSplitterTopBottom  m_wndSplitterHorz;
	CSplitterObjectView m_wndSplitterObjectViews;

	CRect		    m_Rect;
	WINDOWPLACEMENT	m_wp;
	UINT            m_nTimerOneSec;	// skins
public:
	UINT            m_nTimerCounter;
private:
	bool			m_bSplitterWndCreated;
	bool			m_bFullScreen;
	bool			m_bFullScreenChanging; // Recherche TKR
	bool			m_bActive;

	CDlgExternalFrameArray	m_ExternalFrames;
	CMonitorInfo	m_MonitorInfo;

	UINT            m_nStyle;	// skins
	CSkinMenu*		m_pSkinMenu;

	CSyncCoolBar*	m_pwndSyncCoolBar;// Recherche
	CImageList*		m_pToolBarImages;

	CJoyStickDX*	m_pJoyStick;
	DIJOYSTATE*		m_pJoyState;
	int				m_nButtonMap[MAX_JOYSTICK_BUTTONS];
	DWORD			m_dwJoyPTZspeed;
	CameraControlCmd m_LastCmd;
	int				m_nZeroRange;
	BOOL			m_bControlSpeed;
	BOOL			m_bIsInMenuLoop;
	CString			m_sToolTip;
	UINT			m_nTootipTimer;
	HMENU			m_hTooltipMenu;

public:
protected:
};

/////////////////////////////////////////////////////////////////////////////
inline const bool CMainFrame::IsSplitterWndCreated() const
{
	return m_bSplitterWndCreated;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CMainFrame::IsActive() const
{
	return m_bActive;
}
/////////////////////////////////////////////////////////////////////////////
inline const bool CMainFrame::IsFullScreen() const
{
	return m_bFullScreen;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CMainFrame::IsSyncMode() const
{
	return m_pwndSyncCoolBar != NULL;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CMainFrame::HasStatusBar() const
{
	return m_wndStatusBar.m_hWnd != NULL;
}
/////////////////////////////////////////////////////////////////////////////
inline CSyncCoolBar* CMainFrame::GetSyncCoolBar()
{
	return m_pwndSyncCoolBar;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CMainFrame::IsFullScreenChanging() const
{
	return m_bFullScreenChanging;
}
/////////////////////////////////////////////////////////////////////////////
inline const UINT CMainFrame::GetOneSecTimerID() const
{
	return m_nTimerOneSec;
}
