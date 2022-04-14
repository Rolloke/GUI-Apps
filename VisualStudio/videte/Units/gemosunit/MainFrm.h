// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__B1CFA5E9_4B32_11D3_9978_004005A19028__INCLUDED_)
#define AFX_MAINFRM_H__B1CFA5E9_4B32_11D3_9978_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

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
	void OnRequestState();
	void OnDispatchCommand(WORD wLine, WORD wInput, WORD wState, const CString &sText);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSendCommands();
	afx_msg long OnCmdReceived(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTraceACK();
	afx_msg void OnTraceCMD();
	afx_msg void OnTraceDC3();
	afx_msg void OnTraceENQ();
	afx_msg void OnTraceEOT();
	afx_msg void OnTraceNAK();
	afx_msg void OnTraceSYN();
	afx_msg void OnUpdateTraceACK(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTraceCMD(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTraceDC3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTraceENQ(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTraceEOT(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTraceNAK(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTraceSYN(CCmdUI* pCmdUI);
	afx_msg void OnTraceState();
	afx_msg void OnUpdateTraceState(CCmdUI* pCmdUI);
	afx_msg void OnTraceDispatch();
	afx_msg void OnUpdateTraceDispatch(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	BOOL			m_bTraceDispatch;
	BOOL			m_bTraceStateRequest;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__B1CFA5E9_4B32_11D3_9978_004005A19028__INCLUDED_)
