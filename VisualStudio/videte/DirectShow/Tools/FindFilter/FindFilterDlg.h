// FindFilterDlg.h : header file
//

#if !defined(AFX_FindFilterDlg_H__9C0ECE0F_70BE_4F12_BEA1_C2A1249EE645__INCLUDED_)
#define AFX_FindFilterDlg_H__9C0ECE0F_70BE_4F12_BEA1_C2A1249EE645__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

interface IMediaSource;
interface IMediaControl;
interface IMediaEventEx;
/////////////////////////////////////////////////////////////////////////////
// CFindFilterDlg dialog
struct ErrorStruct;
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	DWORD	m_nErrorCode;
	CString	m_sErrorText;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnBtnErrorCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CFindFilterDlg : public CDialog
{
// Construction
public:
	CFindFilterDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CFindFilterDlg();

// Dialog Data
	//{{AFX_DATA(CFindFilterDlg)
	enum { IDD = IDD_DIRECTXTESTDLG_DIALOG };
	CButton	m_btnSearch;
	CListCtrl	m_cFoundFilters;
	CComboBox	m_cFilters;
	CComboBox	m_cCategories;
	BOOL	m_bAllCategories;
	BOOL	m_bExactMatch;
	int		m_nSearchBy;
	CString	m_sFilterGUID;
	CString	m_sSearchString;
	BOOL	m_bMatchCase;
	CString	m_sCategoryGUID;
	//}}AFX_DATA

	IMoniker *m_pMoniker;
	CString   m_sFilterName;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
#ifdef FIND_FILTER
	void ReportError(ErrorStruct*ps);
#endif
	void SearchInCurrenCategory();
	int FindFilterName(int);
	GUID GetCurrentGUID();
	void AddToFindList();
protected:

	HICON m_hIcon;
	GUID  m_guidSearch;
	int m_nSelectedCategory;
	int m_nSelectedFilter;
	CStringArray m_saSearchAnd;
	CStringArray m_saSearchOr;
	

	// Generated message map functions
	//{{AFX_MSG(CFindFilterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeComboFilters();
	afx_msg void OnSelchangeComboCategories();
	afx_msg void OnCkAllCategories();
	afx_msg void OnCkExactMatch();
	afx_msg void OnRdNameGuid();
	afx_msg void OnBtnSearch();
	afx_msg void OnDblclkListFoundFilters(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCkMatchCase();
	virtual void OnOK();
	afx_msg void OnBtnSave();
	afx_msg void OnBtnDetectGuidName();
	afx_msg LRESULT OnGraphNotify(WPARAM, LPARAM);
	afx_msg LRESULT OnUserMessage(WPARAM, LPARAM);
	afx_msg void OnBnClickedShowInterfaces();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
#ifdef FIND_FILTER
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
#endif
#ifdef GRAPH_CREATOR
	afx_msg void OnBnClickedBtnSetName();
	afx_msg void OnBnClickedBtnGetExtraInfo();
#endif
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FindFilterDlg_H__9C0ECE0F_70BE_4F12_BEA1_C2A1249EE645__INCLUDED_)
