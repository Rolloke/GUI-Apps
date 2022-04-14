#pragma once


// CMainFrame frame

class CMainFrame : public CFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();           // protected constructor used by dynamic creation
	virtual ~CMainFrame();

public:
	void ClearAskAtExit();
	void ContextMenu();

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL    OnQueryEndSession();
	afx_msg LRESULT OnWmPowerBroadcast(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgApplication(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg LRESULT OnKeyboardChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

private:
	BOOL				m_bAskAtExit;
	BOOL				m_bOnQueryEndSession;
	BOOL				m_bOnWmPowerBroadcast;
	CString				m_sDateTime;
	CString				m_sVersion;
public:
};


