#if !defined(AFX_SWITCHPANEL_H__8CFB3CCE_55AC_4A33_8789_032D0EBE7DA9__INCLUDED_)
#define AFX_SWITCHPANEL_H__8CFB3CCE_55AC_4A33_8789_032D0EBE7DA9__INCLUDED_

#include "ICP7000Thread.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SwitchPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSwitchPanel dialog
class  CMinimizedDlg;

class CSwitchPanel : public CTransparentDialog
{
	friend class CICPI7000Thread;
// Construction
public:
	CSwitchPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSwitchPanel();
// Dialog Data
	//{{AFX_DATA(CSwitchPanel)
	enum { IDD = IDD_SWITCH_PANEL };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSwitchPanel)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void CreateAllButtons();
	void DeleteAllButtons();
	bool SetPortThread(WORD, void*, bool);

	// Generated message map functions
	//{{AFX_MSG(CSwitchPanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnConfig();
	afx_msg void OnDestroy();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBtnPollPort();
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	afx_msg void OnBtnCancel();
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnCheckSkinBtn(WPARAM, LPARAM);
	afx_msg void OnCheck(UINT);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEnterMenuLoop(WPARAM, LPARAM);
	afx_msg LRESULT OnExitMenuLoop(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

public:
	void DestroyControlThreads();
	void Unlock();
	void Lock();
private:
	CSkinButton	m_cFirstRelais;

	int    m_nEdgeX;
	int    m_nEdgeY;
	CRect  m_rcDlg;
	
	UINT   m_nPollTimer;
	int    m_nPollTime;
	int    m_nTimeout;
	BOOL   m_bPollState;
	int    m_nTimeoutEdge;
	BOOL   m_bThreadsArePolling;

	UINT   m_nMinimizeTimer;
	int    m_nMinimizeCounter;
	int    m_nMinimizeTime;

	int    m_nButtonShape;
	int    m_nSkinColor;

	HICON          m_hIcon;
	CImageList     m_ilRelaisBtn;
	HACCEL         m_hAccelerator;
	CPtrList       m_ThreadList;
	CMinimizedDlg *m_pMiniDlg;
	CRITICAL_SECTION m_cs;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SWITCHPANEL_H__8CFB3CCE_55AC_4A33_8789_032D0EBE7DA9__INCLUDED_)
