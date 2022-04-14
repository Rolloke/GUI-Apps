// LogCountDlg.h : header file
//

#if !defined(AFX_LOGCOUNTDLG_H__74714519_8DBD_11D2_B04E_004005A1D890__INCLUDED_)
#define AFX_LOGCOUNTDLG_H__74714519_8DBD_11D2_B04E_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
class CSearchThread;

/////////////////////////////////////////////////////////////////////////////
// Einige Standardstringsdefinieren
static TCHAR BASED_CODE szTemp[]		= _T("Temp");

/////////////////////////////////////////////////////////////////////////////
enum EnumProgress
{
	START,
	SEARCH,
	READY,
	CANCELED,
};
/////////////////////////////////////////////////////////////////////////////
enum EnumSearchFor
{
	SEARCH_UNKNOWN,
	GERMAN_PARCEL_SCANNED_PACKETS,
};
/////////////////////////////////////////////////////////////////////////////
// CLogCountDlg dialog
class CLogCountDlg : public CWK_Dialog
{
// Construction
public:
	CLogCountDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CLogCountDlg();

// Overrides
public:
	virtual BOOL StretchElements();

// Implementation
public:
	CString	GetNextSearchFile();
	void	SearchFileProcessed();
	void	UpdateSearch(DWORD dwPatternCounter, DWORD dwErrorCounter);
	BOOL	IsDateEarlierThanSpan(const CSystemTime& stDate);
	BOOL	IsDateInSpan(const CSystemTime& stDate);
	BOOL	IsDateLaterThanSpan(const CSystemTime& stDate);
protected:
	DWORD	Lock();
	DWORD	Unlock();
	void	AppendSearchFile(const CString& sFile);
	int		GetSearchFilesCount();
	int		GetSearchedFilesCount();
	DWORD	GetPatternCounter();
	DWORD	GetErrorCounter();
	void	FillSearchBox();
	void	FillLogfileBox();
	void	ClearData();
	void	EnableDlgControls();
	void	SelectLogfileInBox();
	BOOL	SearchCriteriaNotEmpty();
	void	SetDateAndTimeControls();
	void	SetDateAndTime();
	void	WorkingProgress();
	void	SearchInFiles();
	BOOL	SortList(CStringArray &FileNames);
	BOOL	StopSearchThread();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogCountDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CLogCountDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonEnd();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRadioSearch();
	afx_msg void OnSelchangeComboSearchFor();
	afx_msg void OnSelchangeComboLogfile();
	afx_msg void OnChangeEditSearchPattern();
	afx_msg void OnChangeEditErrorPattern();
	afx_msg void OnKillfocusEditDirectory();
	afx_msg void OnBtnDirectorySearch();
	afx_msg void OnCheckAll();
	afx_msg void OnDropdownComboLogfile();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
protected:
	//{{AFX_DATA(CLogCountDlg)
	enum { IDD = IDD_LOGCOUNT_DIALOG };
	CButton	m_btnDirectory;
	CButton	m_btnSearch2;
	CButton	m_btnSearch1;
	CButton	m_btnSearchAll;
	CDateTimeCtrl	m_dtpTimeEnd;
	CDateTimeCtrl	m_dtpTimeStart;
	CDateTimeCtrl	m_dtpDateEnd;
	CDateTimeCtrl	m_dtpDateStart;
	CEdit	m_editDirectory;
	CComboBox	m_comboLogfile;
	CEdit	m_editSearchPattern;
	CEdit	m_editErrorPattern;
	CComboBox	m_comboSearchFor;
	CAnimateCtrl	m_Animation;
	CButton	m_btnCancel;
	CButton	m_btnEnd;
	CButton	m_btnOK;
	DWORD	m_dwPatternCounter;
	CString	m_sFilesCounter;
	CString	m_sProgress;
	DWORD	m_dwErrorCounter;
	CString	m_sErrorPattern;
	CString	m_sSearchPattern;
	int		m_iRadioSearch;
	int		m_iLogfilePattern;
	CString	m_sDirectory;
	BOOL	m_bSearchAll;
	//}}AFX_DATA
	HICON m_hIcon;
	CCriticalSection	m_cs;
	CSearchThread*		m_pSearchThread;
	EnumProgress		m_eProgress;
	BOOL				m_bCanceled;

	CStringArray	m_sFileArraySearch;
	int				m_iCounterSearchedFiles;
	CString			m_sLogfilePattern;
	DWORD			m_dwLockCounter;
	DWORD			m_dwUnlockCounter;

	CSystemTime	m_DateTimeStart;
	CSystemTime	m_DateTimeEnd;

	DWORD	m_dwStartSearchTime;
	BOOL	m_bSomethingChanged;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGCOUNTDLG_H__74714519_8DBD_11D2_B04E_004005A1D890__INCLUDED_)
