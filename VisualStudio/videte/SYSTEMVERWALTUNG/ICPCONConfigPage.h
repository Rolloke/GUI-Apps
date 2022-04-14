// ICPCONClientDlg.h : header file
//

#if !defined(AFX_CICPCONConfigPage_H__FD2063F2_4D16_4153_AB66_967B95B02F00__INCLUDED_)
#define AFX_CICPCONConfigPage_H__FD2063F2_4D16_4153_AB66_967B95B02F00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVPage.h"
#include "afxwin.h"

#define MINPOS_LEFTTOP		_T("lefttop")
#define MINPOS_LEFTBOTTOM	_T("leftbottom")
#define MINPOS_RIGHTTOP		_T("righttop")
#define MINPOS_RIGHTBOTTOM	_T("rightbottom")
#define MINPOS_NONE			_T("none")

////////////////////////////////////////////////////////////////////////////
// CICPCONConfigPage dialog
class CICPI7000Module;
class CICPCONConfigPage : public CSVPage
{
	DECLARE_DYNAMIC(CICPCONConfigPage)
// Construction
public:
	CICPCONConfigPage(CSVView* pParent = NULL);	// standard constructor

	virtual void Initialize();
	virtual void SaveChanges();
	virtual BOOL IsDataValid();
	virtual void CancelChanges();

	virtual void OnNew();
	virtual void OnDelete();
	
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual BOOL StretchElements();
	virtual BOOL GetToolTip(UINT nID, CString&sText);

//	virtual int  GetNoOfSubDlgs();
//	virtual CWK_Dialog* GetSubDlg(int);

protected:
// Dialog Data
	//{{AFX_DATA(CICPCONConfigPage)
	enum { IDD = IDD_ICPCON_PAGE };
	CButton	m_cBtnConfigModules;
	CProgressCtrl	m_cProgressPorts;
	CProgressCtrl	m_cProgressAddress;
	CTreeCtrl	m_cTreeOfModules;
	UINT		m_dwBaudrate;
	int			m_nComPort;
	CString		m_strModuleID;
	int			m_nModuleAddress;
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
	int		m_nPollTime;
	BOOL		m_bPollState;
	BOOL	   m_bSwitchPanel;
	BOOL	m_bICPCONEnabled;
	CComboBox m_comboMinimize;
	CString m_sAddresses;
	CString m_sPorts;
	//}}AFX_DATA
	CWordArray	m_wComPorts;
	CWordArray	m_wAddresses;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CICPCONConfigPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CICPCONConfigPage)
	afx_msg void OnDestroy();
	afx_msg void OnBtnPollPort();
	afx_msg void OnBtnScanAllPorts();
	afx_msg void OnDeleteitemTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnCancel();
	afx_msg void OnCkComStates();
	afx_msg void OnCkModuleStates();
	afx_msg void OnEditTreeLabel();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChangeEdtModuleAddress();
	afx_msg void OnBtnConfigModule();
	afx_msg void OnChangeEdtTimeout();
	afx_msg void OnChangeEdtTimePollRelais();
	afx_msg void OnCkEnableSwitchPanel();
	afx_msg void OnCkEnableIcpconModule();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnUserMessage(WPARAM, LPARAM);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChkBaud(UINT);
	afx_msg void OnBtnDeleteTree();
	afx_msg void OnCbnSelchangeIpcComboMinimize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void	SetBaudCheck(int);
	static DWORD ExtractNumbers(const CString &sAddresses, WORD wMinVal, WORD wMaxVal, CWordArray&waNumbers);

private:
	void	DeleteGroup(int);
   static unsigned int __stdcall ThreadProc(void *);
	BOOL	OnPollPort();
	void	OnScanAllPorts();
	void	InitModules();
	void	SetThreadControlBtn(BOOL );
	int		DeleteModule(HTREEITEM);

	HANDLE			m_hThread;
	unsigned int	m_nThreadID;
	int				m_nWhat;
	volatile BOOL	m_bStopThread;

	CICPI7000Module	*m_pModule;
	int				m_nActive;
	DWORD			m_dwConfigChanged;
	CString			m_strBtnConfigModule;

	HACCEL			m_hAccelerator;

	DWORD			m_dwChanged;

	CSize			m_szButton;
	BOOL			m_bExpandModules;
	BOOL			m_bIsLocal;

	CString			m_sBaseDVRT;
	CString			m_sRegistryLocation;
	CString			m_sRegistryLocationModules;
	CString			m_sRegistryLocationPorts;
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CICPCONConfigPage_H__FD2063F2_4D16_4153_AB66_967B95B02F00__INCLUDED_)
