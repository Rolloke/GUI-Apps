// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__AB5B2369_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
#define AFX_MAINFRM_H__AB5B2369_3834_11D2_B58E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////////////////
#define WPARAM_REQUEST_BACKUP			0x0001
#define WPARAM_BACKUP_FINISHED			0x0002
#define WPARAM_REQUEST_QUERY			0x0003
#define WPARAM_QUERY_FINISHED			0x0004
#define WPARAM_CONNECTION_RECORD		0x0005
#define WPARAM_INITIALIZE_THREAD		0x0006
#define WPARAM_VERIFY_FINISHED			0x0007
#define WPARAM_SEND_ALARM_FUEL			0x0008
#define WPARAM_DRIVE_CHANGED			0x0009

class COscopeView;

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	COscopeView* GetOscopeView();
private:
	void CreateScopeView(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void OnIdle();

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	COscopeView *m_pScopeView;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnAppAbout();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnDeviceChange( UINT nEventType, DWORD dwData );
	afx_msg LRESULT DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnView(UINT nView);
	afx_msg void OnUpdateView(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	UINT m_uTimerID;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__AB5B2369_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
