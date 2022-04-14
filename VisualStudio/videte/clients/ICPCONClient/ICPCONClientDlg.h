// ICPCONClientDlg.h : header file
//

#if !defined(AFX_CIPCCONCLIENTDLG_H__FD2063F2_4D16_4153_AB66_967B95B02F00__INCLUDED_)
#define AFX_CIPCCONCLIENTDLG_H__FD2063F2_4D16_4153_AB66_967B95B02F00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CIPCCONClientDlg dialog
class CICPI7000Module;
class CIPCCONClientDlg : public CDialog
{
// Construction
public:
	CIPCCONClientDlg(CWnd* pParent = NULL);	// standard constructor

protected:
// Dialog Data
	//{{AFX_DATA(CIPCCONClientDlg)
	enum { IDD = IDD_RELAISSWITCHBOX_DIALOG };
	CProgressCtrl	m_cProgressPorts;
	CProgressCtrl	m_cProgressAddress;
	CTreeCtrl	m_cTreeOfModules;
	CEdit	m_CErrorLog;
	UINT	   m_dwBaudrate;
	int		m_nComPort;
	CString	m_strModuleID;
	int		m_nModuleAddress;
	int		m_nSizeX;
	int		m_nSizeY;
	BOOL		m_bState;
	BOOL		m_bBaud1200;
	BOOL		m_bBaud2400;
	BOOL		m_bBaud4800;
	BOOL		m_bBaud9600;
	BOOL		m_bBaud19200;
	BOOL		m_bBaud38400;
	BOOL		m_bBaud57600;
	BOOL		m_bBaud115200;
	int		m_nTimeOut;
	int		m_nEdgeOrLevel;
	int		m_nPollTime;
	int		m_nTimeoutEdge;
	BOOL		m_bPollState;
	int		m_nMinimizeTime;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPCCONClientDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CIPCCONClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnBtnPollPort();
	afx_msg void OnBtnScanAllPorts();
	afx_msg void OnDeleteitemTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnSaveSettings();
	afx_msg void OnEndlabeleditTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnCancel();
	afx_msg void OnCkState();
	virtual void OnOK();
	afx_msg void OnRdEdgeOrLevel();
	afx_msg void OnCkComStates();
	afx_msg void OnCkModuleStates();
	afx_msg void OnBtnDeleteTree();
	afx_msg void OnEditTreeLabel();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnUserMessage(WPARAM, LPARAM);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	BOOL        m_bAllowInputs;

private:
   static unsigned int __stdcall ThreadProc(void *);
	void OnPollPort();
	void OnScanAllPorts();
	void SetThreadControlBtn(BOOL );

#ifdef _DEBUG
	void ErrorLog(LPCTSTR, ...);
#endif

   HANDLE       m_hThread;
   unsigned int m_nThreadID;
	int          m_nWhat;
	volatile BOOL m_bStopThread;

	CICPI7000Module *m_pModule;
	int          m_nActive;
	HACCEL         m_hAccelerator;
	CEdit      *m_pTreeEdit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CIPCCONCLIENTDLG_H__FD2063F2_4D16_4153_AB66_967B95B02F00__INCLUDED_)
