// TestPnPDlg.h : header file
//

#if !defined(AFX_TESTPNPDLG_H__69A28B4A_A131_4AB5_B89E_5209FF78B526__INCLUDED_)
#define AFX_TESTPNPDLG_H__69A28B4A_A131_4AB5_B89E_5209FF78B526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestPnPDlg dialog
class CTestPnPDlg : public CDialog
{
// Construction
public:
	void EnumDevices(LPCTSTR);
	void Report(CString&);
	CTestPnPDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CTestPnPDlg();	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestPnPDlg)
	enum { IDD = IDD_TESTPNP_DIALOG };
	CComboBox	m_ComboGUID;
	CEdit	m_Messages;
	CString	m_strGUID;
	//}}AFX_DATA
	CString	m_strDevName;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestPnPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	CString GetDriveType(UINT);

// Implementation
protected:
	HICON m_hIcon;
	CPtrList   m_DevNotify;
	CPtrList   m_FileHandles;

	// Generated message map functions
	//{{AFX_MSG(CTestPnPDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnSelchangeComboGuids();
	afx_msg void OnBtnEnumDevices();
	afx_msg void OnBtnClearOutput();
	afx_msg void OnBtnSaveToFile();
	afx_msg void OnBtnTest();
	afx_msg void OnBtnPrint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	BOOL OnDeviceChange(UINT, DWORD);
	DECLARE_MESSAGE_MAP()

};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTPNPDLG_H__69A28B4A_A131_4AB5_B89E_5209FF78B526__INCLUDED_)
