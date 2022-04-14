// ExtractBuildLogDlg.h : header file
//

#if !defined(AFX_EXTRACTBUILDLOGDLG_H__CE2DA229_7526_11D5_BB45_0050BF49CEBF__INCLUDED_)
#define AFX_EXTRACTBUILDLOGDLG_H__CE2DA229_7526_11D5_BB45_0050BF49CEBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CExtractBuildLogDlg dialog

class CExtractBuildLogDlg : public CDialog
{
// Construction
public:
	CExtractBuildLogDlg(int iFileType, CString sWorkspaceDir, CString sWorkspaceName, CWnd* pParent = NULL);	// standard constructor

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtractBuildLogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CExtractBuildLogDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnChangeEditOutputFile();
	afx_msg void OnChangeEditInputFile();
	afx_msg void OnSelchangeComboLibrary();
	afx_msg void OnEditchangeComboLibrary();
	afx_msg void OnRadioFileType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void	UpdateDialog(BOOL bSaveAndValidate = TRUE);
	void	ExtractLinkErrors(CStdioFile& file);
	void	ExtractDocomaticList(CStdioFile& file);

// Dialog Data
protected:
	//{{AFX_DATA(CExtractBuildLogDlg)
	enum { IDD = IDD_EXTRACTBUILDLOG_DIALOG };
	CComboBox	m_cbLibrary;
	CButton	m_btnOK;
	CStatic	m_txtWorkspaceLog;
	CString	m_sOutputFile;
	CString	m_sInputFile;
	CString	m_sStatus;
	CString	m_sLibrary;
	int		m_iFileType;
	//}}AFX_DATA
	HICON	m_hIcon;
	CString	m_sWorkspaceName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTRACTBUILDLOGDLG_H__CE2DA229_7526_11D5_BB45_0050BF49CEBF__INCLUDED_)
