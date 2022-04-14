// ICPCONUnitDlg.h : header file
//

#if !defined(AFX_ICPCONUNITDLG_H__4AC85B38_E7B5_4F60_B263_7467D0C4F99A__INCLUDED_)
#define AFX_ICPCONUNITDLG_H__4AC85B38_E7B5_4F60_B263_7467D0C4F99A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CICPCONUnitDlg dialog
#include "resource.h"
class CICPI7000Module;
class CSwitchPanel;

class CICPCONUnitDlg : public CDialog
{
	friend class CICPI7000Thread;
// Construction
public:
	void SetLogText(LPCTSTR);
	void CreateSwitchPanel();
	CICPCONUnitDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CICPCONUnitDlg)
	enum { IDD = IDD_ICPCONUNIT_DIALOG };
	CEdit	m_cLogWindow;
	CListCtrl	m_ModuleList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CICPCONUnitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CICPCONUnitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSettings();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnAbout();
	afx_msg void OnClickListModules(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnUserMessage(WPARAM, LPARAM);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg long OnWinThreadStopThread(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEnterMenuLoop(WPARAM, LPARAM);
	afx_msg LRESULT OnExitMenuLoop(WPARAM, LPARAM);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool SetPortThread(WORD, void*, bool);
	CICPI7000Module* FindModule(int, int);
	void CreateControlThreads();
	void DestroyControlThreads(bool);
	void Unlock();
	void Lock();

private:
	bool InitList();
	UINT	m_nPollTimer;
	int    m_nPollTime;
	int    m_nTimeout;
	BOOL   m_bThreadsArePolling;
	BOOL   m_bPollState;
	CPtrList       m_ThreadList;
	CCriticalSection m_cs;
	CSwitchPanel *m_pSwitchPanel;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICPCONUNITDLG_H__4AC85B38_E7B5_4F60_B263_7467D0C4F99A__INCLUDED_)
