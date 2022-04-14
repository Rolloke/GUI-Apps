// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__43C9E493_8B84_11D3_99EB_004005A19028__INCLUDED_)
#define AFX_MAINFRM_H__43C9E493_8B84_11D3_99EB_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WPARAM_ALARM_ON		0x01
#define WPARAM_ALARM_OFF	0x02

#define WPARAM_INPUT_CONNECTION_RECORD	0x03
#define WPARAM_OUTPUT_CONNECTION_RECORD	0x04
#define WPARAM_AUDIO_CONNECTION_RECORD	0x05

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	HWND	GetStarterWindowHandle();

// Operations
public:
	void SetTimer(UINT nElapse);

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
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	HWND		m_hwndStarter;
private:
	UINT m_uTimerID;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__43C9E493_8B84_11D3_99EB_004005A19028__INCLUDED_)
