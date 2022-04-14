/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: MainFrm.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/MainFrm.h $
// CHECKIN:		$Date: 9.03.98 12:46 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 18.02.98 16:00 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_MAINFRM_H__2D22C51D_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_MAINFRM_H__2D22C51D_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	BOOL Resize(CRect& rcNew);

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data
protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	BOOL		m_bResizing;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__2D22C51D_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
