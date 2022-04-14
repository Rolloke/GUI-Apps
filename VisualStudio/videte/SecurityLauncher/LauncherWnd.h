#if !defined(AFX_LAUNCHERWND_H__941D6A74_5DA9_11D2_B5C1_00C095EC9EFA__INCLUDED_)
#define AFX_LAUNCHERWND_H__941D6A74_5DA9_11D2_B5C1_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LauncherWnd.h : header file
//

#include "BitmapTextButton.h"
#include "AppBar.h"


/////////////////////////////////////////////////////////////////////////////
// CLauncherWnd window

class CLauncherWnd : public CAppBar
{
// Construction
public:
	CLauncherWnd();

// Attributes
public:

// Operations
public:
	void ClearAskAtExit();
	void SetTime(const CSystemTime& st);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLauncherWnd)
	protected:
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLauncherWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLauncherWnd)
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnAOT();
	afx_msg void OnUpdateAOT(CCmdUI* pCmdUI);
	afx_msg void OnAIH();
	afx_msg void OnUpdateAIH(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT OnConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnQueryEndSession();
	afx_msg LRESULT OnWmPowerBroadcast(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgApplication(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	// Implementation
protected:
	void OnButton();
	void HideFloatAdornments (BOOL fHide);
protected:  // Overridable functions
   void OnAppBarStateChange (BOOL fProposed, UINT uStateProposed);

private:
#if _MFC_VER < 0x0070
	CCoolMenuManager	m_CoolMenuManager;
#endif
	CFont				m_SmallFont;
	CBitmapTextButton	m_Button;		
	BOOL				m_bAskAtExit;
	BOOL				m_bOnQueryEndSession;
	BOOL				m_bOnWmPowerBroadcast;
	CString				m_sDateTime;

	CString				m_sVersion;
	CSize m_szMinTracking;  // The minimum size of the client area
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAUNCHERWND_H__941D6A74_5DA9_11D2_B5C1_00C095EC9EFA__INCLUDED_)
