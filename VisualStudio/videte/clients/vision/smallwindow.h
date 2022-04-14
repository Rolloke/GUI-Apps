#if !defined(AFX_SMALLWINDOW_H__A18E9307_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_SMALLWINDOW_H__A18E9307_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SmallWindow.h : header file
//

class CVisionView;

#include "SmallToolBar.h"
#include "Server.h"

#define VW_TEXTOFF	2	
/////////////////////////////////////////////////////////////////////////////
// CSmallWindow window

class CSmallWindow : public CWnd
{
	DECLARE_DYNCREATE(CSmallWindow)		
// Construction
public:
	CSmallWindow(CServer* pServer);
	CSmallWindow() {TRACE("Error Do not use default constructor");};

// Attributes
public:
	virtual CSecID    GetID();
	virtual BOOL      IsRequesting();
	virtual BOOL      IsDisplayWindow();
	virtual BOOL      IsHtmlWindow();
	virtual BOOL      IsTextWindow();
	virtual BOOL      IsReferenceWindow();
	inline  WORD      GetServerID();
	inline  WORD      GetHostID();
	inline  CString   GetServerName();
	inline  BOOL      IsActive();
	inline  CServer*  GetServer();
	BOOL              IsFullScreen();
	int               GetMonitor();
	DWORD             GetMonitorFlag();
// Operations
public:
	void SetActive(BOOL bActive=TRUE);
	void EnableTitle(BOOL);
	BOOL operator < (CSmallWindow & second);
	// Overrides
public:

protected:
	virtual CString GetTitleText(CDC* pDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmallWindow)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSmallWindow();

// Implementation
protected:
	virtual void OnDraw(CDC* pDC);
	void DrawTitle(CDC* pDC);
	void DrawTitleTransparent(CDC* pDC);
	virtual void GetFrameRect(LPRECT lpRect);
	void GetVideoClientRect(LPRECT lpRect, CSize picturesize, BOOL b1to1);
	void GetTitleRect(LPRECT lpRect);

	// Generated message map functions
protected:
	//{{AFX_MSG(CSmallWindow)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnVideoFullscreen();
	afx_msg void OnUpdateVideoFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnSmallClose();
	afx_msg void OnUpdateSmallClose(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowonMonitorExclusive();
	afx_msg void OnUpdateShowonMonitorExclusive(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateShowOnMonitor(CCmdUI* pCmdUI);
	afx_msg void OnShowOnMonitor(UINT nID);
	DECLARE_MESSAGE_MAP()

protected:
	CServer*      m_pServer;
	WORD          m_wServerID;
	WORD          m_wHostID;
	CVisionView*  m_pVisionView;
	BOOL          m_bIsActive;
	CSmallToolBar m_wndToolBar;
	CSize         m_ToolBarSize;
	int           m_iImage;			
	CSystemTime   m_stTimeStamp;

	BOOL			m_bLeftClick;
	POINT			m_pointAtLeftClick;
	BOOL			m_bLeftDblClick;
	UINT			m_uTimerID;
	DWORD			m_dwMonitor;
};
/////////////////////////////////////////////////////////////////////////////
inline WORD	CSmallWindow::GetServerID()
{
	return m_wServerID;
}
/////////////////////////////////////////////////////////////////////////////
inline WORD	CSmallWindow::GetHostID()
{
	return m_wHostID;
}
/////////////////////////////////////////////////////////////////////////////
inline CString CSmallWindow::GetServerName()
{
	return m_pServer ? m_pServer->GetName() : "";
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CSmallWindow::IsActive()
{
	return m_bIsActive;
}
/////////////////////////////////////////////////////////////////////////////
inline CServer*	CSmallWindow::GetServer()
{
	return m_pServer;
}
/////////////////////////////////////////////////////////////////////////////
typedef CSmallWindow* CSmallWindowPtr;
WK_PTR_ARRAY_CS(CSmallWindowArray,CSmallWindowPtr,CSmallWindows);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMALLWINDOW_H__A18E9307_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_)
