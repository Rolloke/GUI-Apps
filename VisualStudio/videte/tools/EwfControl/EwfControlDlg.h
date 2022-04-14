// EwfControlDlg.h : Headerdatei
//

#pragma once


// CEwfControlDlg Dialogfeld
class CEwfControlDlg : public CDialog
{
// Konstruktion
public:
	CEwfControlDlg(CWnd* pParent = NULL);	// Standardkonstruktor
	virtual ~CEwfControlDlg();	// Destruktor

// Dialogfelddaten
	//{{AFX_DATA(CEwfControlDlg)
	enum { IDD = IDD_EWFCONTROL_DIALOG };
protected:
	BOOL m_bCommit;
	BOOL m_bDisable;
	BOOL m_bEnable;
	int  m_nEndsession;
	//}}AFX_DATA

	HICON m_hIcon;
	CEwf*m_pEwf;
	BOOL m_bEwfEnabled;
	BOOL m_bQueryEndSession;
	BOOL m_bShellIconSet;
	OVERLAPPED m_ovEventLowSpace;
	CWinThread *m_pLowSpaceThread;
	UINT		m_nShutDownTimer;
	int			m_nShutdownCounter;
	int			m_nEwfRamThreshold;
	CString		m_sShutDown;
	UINT		m_nControlTimer;
	DWORD		m_dwCurrentDataSize;
	DWORD		m_dwCurrentDataSizeLow;
	CMemoryStatus m_MemoryStatus;
public:
	TCHAR	m_cProtectedDrive;
	BOOL	m_bAsync;
	LONGLONG m_llLowSpace;
	BOOL	m_bSilent;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEwfControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL


// Implementierung
protected:
	void InitEwfEnabled();
	void InitEwfDisabled();
	BOOL InitEwfConfig(BOOL bFirstTime);
	BOOL UpdateEwfDataSize();
	void CheckAutoRun();
	void HideDialog();
	void DoExitWindows(UINT uFlags);
	static UINT LowSpaceThreadFunc(LPVOID pParam);
	
#ifdef _DEBUG
	void Test();
#endif

	// Generierte Funktionen für die Meldungstabellen
	//{{AFX_MSG(CEwfControlDlg)
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnQueryEndSession(WPARAM, LPARAM); 
	afx_msg LRESULT OnUser(WPARAM, LPARAM); 
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCkEnableEwf();
	afx_msg void OnBnClickedCkDisableEwf();
	afx_msg void OnBnClickedCkCommitData();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedRd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEndSession(BOOL bEnding);
};
