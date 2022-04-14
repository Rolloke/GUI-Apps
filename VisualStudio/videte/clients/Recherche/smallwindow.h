#if !defined(AFX_SMALLWINDOW_H__A18E9307_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_)
#define AFX_SMALLWINDOW_H__A18E9307_88C7_11D1_93E4_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SmallWindow.h : header file
//

class CRechercheView;

#include "SmallToolBar.h"
#include "Server.h"


#include "FieldDialog.h"
#include "NavigationDialog.h"
#include "QueryRectTracker.h"

#define FIELD_WIDTH		  108
#define NAVIGATION_HEIGHT 18
#define DW_INFO_FONT_SIZE 120
#define VW_TEXTOFF	2	
/////////////////////////////////////////////////////////////////////////////
// CSmallWindow window

class CSmallWindow : public CWnd
{
// Construction
public:
	CSmallWindow(CServer* pServer);

// Attributes
public:
	inline		 WORD		GetServerID();
	inline		 CString	GetServerName();
	virtual		 CSecID		GetID()=0;
	inline		 BOOL		IsActive();
	inline		 CServer*	GetServer();
				 BOOL		IsFullScreen();
	// Operations
public:
	void  SetActive(BOOL bActive=TRUE);
	void  Set1to1(BOOL b1to1);
	BOOL operator < (CSmallWindow & second);
	
	//Rectangle Search
	void ResetTracker();		//delete m_pQueryRectTracker and create new one

	// Overrides
public:
	virtual CString	GetName()=0;
	virtual	BOOL	IsImageWindow();
	virtual	BOOL	IsDisplayWindow();
	virtual	BOOL	IsDibWindow();
	virtual	BOOL	IsHtmlWindow();
	virtual	BOOL	IsAlarmListWindow();
	virtual void	OnIdle() {};
	virtual BOOL	Create(const RECT& rect, CRechercheView* pParentWnd);
	virtual BOOL	PreDestroyWindow(CRechercheView* pRechercheView);

protected:
	virtual CString GetTitleText(CDC* pDC);
	virtual UINT	GetToolBarID()=0;
	virtual void	OnSetWindowSize()=0;
	virtual	void	PopupMenu(CPoint pt)=0;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmallWindow)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSmallWindow();

// Implementation
protected:
	virtual void OnDraw(CDC* pDC);
			void DrawTitle(CDC* pDC);
	virtual void GetFrameRect(LPRECT lpRect);
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
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSmallContext();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

	// data accessable by CImageWindow, CDisplayWindow and CDibWindow too
protected:
	CRechercheView*	m_pRechercheView;
	CSize			m_ToolBarSize;
	CToolTipCtrl	m_ToolTip;
	BOOL			m_b1to1;

	friend class CRechercheView;
	

	// for rectangle search
	CQueryRectTracker*	m_pQueryRectTracker; 

	// private data
protected:
	CServer*		m_pServer;
	WORD			m_wServerID;
	CSmallToolBar	m_wndToolBar;
	BOOL			m_bIsActive;
};
/////////////////////////////////////////////////////////////////////////////
inline WORD	CSmallWindow::GetServerID()
{
	return m_wServerID;
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
