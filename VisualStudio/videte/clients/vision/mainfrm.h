// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__4682526A_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_MAINFRM_H__4682526A_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "VisionCoolBar.h"

class CExternalFrameDlg;

class CVisionDoc;
class CVisionView;
class CObjectView;
class CSmallWindow;

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CVisionDoc* GetDocument();
	inline const BOOL	IsActive() const;
	inline const BOOL	IsFullScreen() const;
	inline int GetNoOfMonitors() const;
	int        GetNoOfExternalFrames();

// Operations
public:
	void ChangeFullScreenModus();
	void SetStore(BOOL bOn);
	void UpdateToolbar();
	bool RemoveSWfromExtFrames(CSmallWindow*);
	void AddExternalFrame(int);
	CExternalFrameDlg* GetExternalFrame(int);
	void SetWindowMonitor(bool);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CCoolMenuManager m_CoolMenuManager;
	CVisionCoolBar	m_wndCoolBar;
	CToolBar		m_wndToolBar;
	CStatusBar		m_wndStatusBar;
	BOOL			m_bActive;
	BOOL			m_bFullScreen;
	HMENU			m_hMenu;
	CRect		    m_Rect;
	int				m_iObjectWidth;
	WINDOWPLACEMENT	m_wp;
	
	CSplitterWnd	m_wndSplitter;
	CVisionView*	m_pVisionView;
	CObjectView*	m_pObjectView;
	
private:
	CPtrList    m_ExternalFrames;
	CMonitorInfo	m_MonitorInfo;

	static BOOL CALLBACK KillScreenSaverFunc(HWND hwnd, LPARAM lParam);

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAppAbout();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnViewFullscreen();
	afx_msg void OnUpdateViewFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
   afx_msg void OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
   afx_msg void OnConnect(UINT nID);
   afx_msg void OnDisconnect(UINT nID);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewMonitor(UINT nID);
	afx_msg void OnUpdateViewMonitor(CCmdUI* pCmdUI);
#if _MFC_VER >= 0x0700
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif // _MSC_VER == 1300
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);
public:
	DECLARE_MESSAGE_MAP()

};

inline const BOOL CMainFrame::IsActive() const
{
	return m_bActive;
}
inline const BOOL CMainFrame::IsFullScreen() const
{
	return m_bFullScreen;
}
inline int CMainFrame::GetNoOfMonitors() const
{
	if (m_MonitorInfo.GetNrOfMonitors()) return m_MonitorInfo.GetNrOfMonitors();
	return 1;
}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__4682526A_6AF6_11D1_93E4_00C095EC9EFA__INCLUDED_)
