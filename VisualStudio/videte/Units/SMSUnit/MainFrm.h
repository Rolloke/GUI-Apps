// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__60D2705E_9E3A_11D2_B54A_004005A19028__INCLUDED_)
#define AFX_MAINFRM_H__60D2705E_9E3A_11D2_B54A_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "pager.h"
#include "ChildView.h"
#include "HiddenCapiWnd.h"

class CCapiSMS;

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:
	BOOL	CanSend();

// Operations
public:
	BOOL	SendSMS(const CString& sReceiverID, 
					const CString& sMessage,
					DWORD dwTimeOut);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void	PollConnection();

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CChildView  m_wndView;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnAppAbout();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCapiSMSMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnPager( UINT, LONG );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL	KillMyTimer();

private:
	CCapiSMS*		m_pCapiSMS;
	CHiddenCapiWnd*	m_pCapiWnd;
	CPager			m_Pager;
	DWORD			m_dwTimeOut;
	DWORD			m_dwStart;
	UINT			m_uTimerID;
	BOOL			m_bProviderResponse;
	BOOL			m_bIsNewCapiSMS;
	BOOL			m_bConfirmed;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__60D2705E_9E3A_11D2_B54A_004005A19028__INCLUDED_)
