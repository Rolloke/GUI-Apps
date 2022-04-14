/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: MainFrm.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/MainFrm.h $
// CHECKIN:		$Date: 25.01.99 11:20 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 25.01.99 11:04 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#if !defined(AFX_MAINFRM_H__3716C0EC_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
#define AFX_MAINFRM_H__3716C0EC_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "SDIConfigCoolBar.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

		   CControlBar* GetToolBar() const;
	inline CStatusBar* GetStatusBar() const;
	BOOL Resize(CRect& rcNew);

// Attributes
public:
protected:  // control bar embedded members
	CCoolMenuManager	m_CoolMenuManager;
	CStatusBar			m_wndStatusBar;
	CSDIConfigCoolBar	m_wndCoolBar;
	CToolBar			m_wndToolBar;
	BOOL				m_bResizing;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnConnect();
	afx_msg void OnUpdateConnect(CCmdUI* pCmdUI);
	afx_msg void OnDisconnect();
	afx_msg void OnUpdateDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnConfigure();
	afx_msg void OnUpdateConfigure(CCmdUI* pCmdUI);
	afx_msg void OnAppAbout();
	afx_msg void OnExternProgramm();
	afx_msg void OnUpdateExternProgramm(CCmdUI* pCmdUI);
	afx_msg void OnConnectLocal();
	afx_msg void OnUpdateConnectLocal(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
inline CStatusBar* CMainFrame::GetStatusBar() const
{
	return (CStatusBar*)&m_wndStatusBar;
}
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__3716C0EC_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
