// DVHookDlg.h : header file
//

#if !defined(AFX_DVHOOKDLG_H__43389137_6C29_11D4_8F46_004005A11E32__INCLUDED_)
#define AFX_DVHOOKDLG_H__43389137_6C29_11D4_8F46_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDVHookDlg dialog

class CDVHookDlg : public CDialog
{
// Construction
public:
	CDVHookDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CDVHookDlg();

// Dialog Data
	//{{AFX_DATA(CDVHookDlg)
	enum { IDD = IDD_DVHOOK_DIALOG };
	CEdit	m_Output;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVHookDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation

protected:
	bool CheckClose(CWnd*, CString&, LPCTSTR, LPCTSTR);
	bool CheckHide(CWnd*, CString&, LPCTSTR, LPCTSTR);
	void CheckHidden();
	void GetText(CWnd*, CString&, CString&, CString&);
	void SetText(CWnd*, WPARAM, CString&, CString&);
	void StartPasswordDialog(CWnd *, CString&, LPARAM);
	void StartFindDialog(CWnd *, CString &);
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDVHookDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnBtnFlags();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	afx_msg void OnShowHookDlg();
	//}}AFX_MSG
	afx_msg LRESULT OnUserMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnPasswordDlg(WPARAM, LPARAM);
	afx_msg LRESULT OnFindFileDlg(WPARAM, LPARAM);
	afx_msg LRESULT OnPostMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnCloseTimer(WPARAM, LPARAM);
	afx_msg LRESULT CheckForegroundWindow(WPARAM, LPARAM);
	afx_msg LRESULT OnTrayClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	static void _cdecl OutputDebug(LPCTSTR lpszFormat, ...);
private:
	CString GetFileNameFromWindowHandle(HWND hWnd);
	static UINT WatchThread(LPVOID pData);
	static BOOL CALLBACK FindWindowByName(HWND, LPARAM);
	bool   SetWildCardPath(CString&sPath);

private:
	BOOL					m_bRun;
	static CDVHookDlg*m_pThis;
	CWinThread*			m_pWatchThread;
	CHook					m_Hook;
	HWND					m_hWndForeground;
	CWK_Profile			m_prof;
	DWORD					m_dwFlags;
	UINT					m_nTimer;
	CRect					m_rcEdit;
	CRect					m_rcBtnOK;
	CRect					m_rcBtnFlags;
	CPtrList				m_Allocated;
	CPtrList          m_PWdStartedFor;
	CPtrList          m_FindStartedFor;
	bool              m_bHideAgain;
public:
	bool					m_bHidden;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVHOOKDLG_H__43389137_6C29_11D4_8F46_004005A11E32__INCLUDED_)
