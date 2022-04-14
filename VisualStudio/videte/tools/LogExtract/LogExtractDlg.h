// LogExtractDlg.h : header file
//

#if !defined(AFX_LOGEXTRACTDLG_H__AE8AB1F7_0DDB_11D3_BA2D_00400531137E__INCLUDED_)
#define AFX_LOGEXTRACTDLG_H__AE8AB1F7_0DDB_11D3_BA2D_00400531137E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CLogExtractDlg dialog
class CLogExtractDlg : public CWK_Dialog
{
// Construction
public:
	CLogExtractDlg(CWnd* pParent = NULL);	// standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogExtractDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
public:
	virtual BOOL StretchElements();

// Implementation
	// Generated message map functions
	//{{AFX_MSG(CLogExtractDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnSearchFile();
	afx_msg void OnChangeEditOutputFile();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCheckOnlyCom();
	afx_msg void OnChangeEditCom();
	afx_msg void OnCheckLineBreakAfter();
	afx_msg void OnChangeEditLineBreakAfter();
	afx_msg void OnAsciiBin();
	afx_msg void OnChangeEditSearchText();
	afx_msg void OnRadioSearchWhat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
protected:
	void	ShowHide();
	BOOL	PrepareSearchFile();
	BOOL	ReadLine(CString& sLine);
	void	SearchAndCopy(const CString& sLine);
	BOOL	DecompressLGZFile(const CString& sName, const CString& sNew);
	BOOL	DeleteTempFile(const CString& sTempFileName);

// Dialog Data
protected:
	HICON m_hIcon;

	//{{AFX_DATA(CLogExtractDlg)
	enum { IDD = IDD_LOGEXTRACT_DIALOG };
	CButton	m_btnAsciiToBin;
	CEdit	m_editLineBreakAfter;
	CButton	m_btnLineBreakAfter;
	CButton	m_btnOnlyCom;
	CEdit	m_editCom;
	CButton	m_btnOK;
	CButton	m_btnCancel;
	CEdit	m_editOutputFile;
	CButton	m_btnSearchFile;
	CString	m_sOutputFile;
	CString	m_sSearchFile;
	CString	m_sCom;
	BOOL	m_bOnlyCom;
	BOOL	m_bLineBreakAfter;
	CString	m_sLineBreakAfter;
	CString	m_sSearchText;
	int		m_iSearchWhat;
	//}}AFX_DATA

	CFile	m_fileOutput;
	CString	m_sLogFile;
	BOOL	m_bSearching;
	int		m_iCurrentIndex;
	CString	m_sSearchFor;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGEXTRACTDLG_H__AE8AB1F7_0DDB_11D3_BA2D_00400531137E__INCLUDED_)
