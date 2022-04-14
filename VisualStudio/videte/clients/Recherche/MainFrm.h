// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__517E1EFE_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_MAINFRM_H__517E1EFE_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WPARAM_CONNECTED	0x01

/////////////////
// My Cool bar: specialized CCoolBar creates bands.
//
class CMainCoolBar : public CCoolBar 
{
public:
	int HitTest();
protected:
	DECLARE_DYNAMIC(CMainCoolBar)
	virtual BOOL   OnCreateBands();

private:
	CCoolToolBar	m_wndToolBar;			 // toolbar
	CBitmapButton	m_wndButton;

	CImageList		m_ilCold;
	CImageList		m_ilHot;

	friend class CMainFrame;
};

#include "SyncCoolBar.h"

class CRechercheDoc;
class CRechercheView;
class CObjectView;

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
protected:
public:
	CRechercheDoc*	GetDocument();
	inline CSyncCoolBar*	GetSyncCoolBar();
	inline const BOOL	IsFullScreen() const;
	inline const BOOL	IsSyncMode() const;
	inline const BOOL	IsFullScreenChanging() const;

	inline CRechercheView* GetRechercheView();
	inline CObjectView*	   GetObjectView();

// Operations
public:
	void ChangeFullScreenModus();
	void OnIdle();
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

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAppAbout();
	afx_msg void OnViewFullscreen();
	afx_msg void OnUpdateViewFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnSync();
	afx_msg void OnUpdateSync(CCmdUI* pCmdUI);
	afx_msg void OnArchivBackup();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
    afx_msg void OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnConnect(UINT nID);
    afx_msg void OnDisconnect(UINT nID);
	afx_msg BOOL OnDeviceChange( UINT nEventType, DWORD dwData );
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

private:
	CRechercheView* m_pRechercheView;
	CObjectView*	m_pObjectView;

	CCoolMenuManager m_CoolMenuManager;
	CStatusBar		m_wndStatusBar;
	CMainCoolBar	m_wndMainCoolBar;
	CSyncCoolBar*	m_pwndSyncCoolBar;
	CToolBar		m_wndToolBar;
	CSplitterWnd	m_wndSplitter;
	BOOL			m_bFullScreen;
	BOOL			m_bFullScreenChanging;
	HMENU			m_hMenu;
	CRect		    m_Rect;
	int				m_iObjectWidth;
	WINDOWPLACEMENT	m_wp;

	CMonitorInfo	m_MonitorInfo;
};
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CMainFrame::IsFullScreen() const
{
	return m_bFullScreen;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CMainFrame::IsFullScreenChanging() const
{
	return m_bFullScreenChanging;
}
/////////////////////////////////////////////////////////////////////////////
inline const BOOL CMainFrame::IsSyncMode() const
{
	return WK_IS_WINDOW(m_pwndSyncCoolBar);
}
/////////////////////////////////////////////////////////////////////////////
inline CSyncCoolBar* CMainFrame::GetSyncCoolBar()
{
	return m_pwndSyncCoolBar;
}
/////////////////////////////////////////////////////////////////////////////
inline CRechercheView* CMainFrame::GetRechercheView()
{
	return m_pRechercheView;
}
/////////////////////////////////////////////////////////////////////////////
inline CObjectView*	CMainFrame::GetObjectView()
{
	return m_pObjectView;
}
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__517E1EFE_9D66_11D1_8C1B_00C095EC9EFA__INCLUDED_)
