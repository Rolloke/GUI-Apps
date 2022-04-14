// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__F4B60C0F_8ACE_11D3_99EA_004005A19028__INCLUDED_)
#define AFX_MAINFRM_H__F4B60C0F_8ACE_11D3_99EA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////
#define WPARAM_REQUEST_BACKUP		0x0001
#define WPARAM_BACKUP_FINISHED		0x0002
#define WPARAM_REQUEST_QUERY		0x0003
#define WPARAM_QUERY_FINISHED		0x0004
#define WPARAM_CONNECTION_RECORD	0x0005
#define WPARAM_BACKUP_SCAN_FINISHED	0x0006
#define WPARAM_VERIFY_FINISHED		0x0007
////////////////////////////////////////////////////////////////////////////
class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
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
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnTimeChange();
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
	afx_msg void OnSettings();
	afx_msg void OnUpdateSettings(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__F4B60C0F_8ACE_11D3_99EA_004005A19028__INCLUDED_)
