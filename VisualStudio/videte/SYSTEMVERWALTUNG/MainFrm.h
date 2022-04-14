// MainFrm.h : interface of the CMainFrame class
//
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "SVCoolBar.h"

class CSVDoc;
/////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CStatusBar*	GetStatusBar() {return &m_wndStatusBar;}
	CWnd*		GetToolBar();
	CSVDoc*		GetDocument();

	// Operations
public:
	void SetWizardCloseProc(bool);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CCoolMenuManager m_CoolMenuManager;
	CStatusBar  m_wndStatusBar;
	CSVCoolBar	m_wndCoolBar;
	CToolBar    m_wndToolBar;

public:
	void SetMonitor();
private:
	static BOOL CALLBACK WizardCloseProc(CFrameWnd* pFrameWnd);
	UINT		m_nTimer;
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnConnect(UINT nID);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnDeviceChange( UINT nEventType, DWORD dwData );
	afx_msg LRESULT OnKeyboardChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewProfessional();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////
