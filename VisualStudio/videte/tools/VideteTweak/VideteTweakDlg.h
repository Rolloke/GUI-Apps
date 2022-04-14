// VideteTweakDlg.h : header file
//

#if !defined(AFX_VIDETETWEAKDLG_H__8D3852DD_EBA8_465F_BAF6_47C42DD8CA88__INCLUDED_)
#define AFX_VIDETETWEAKDLG_H__8D3852DD_EBA8_465F_BAF6_47C42DD8CA88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVideteTweakDlg dialog
class CAboutDlg : public CPropertyPage
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CVideteTweakDlg : public CPropertyPage
{
// Construction
public:
	CVideteTweakDlg();	// standard constructor

// Dialog Data
	//{{AFX_DATA(CVideteTweakDlg)
	enum { IDD = IDD_VIDETETWEAK_DIALOG };
	CListCtrl	m_ListNoViewOnDrive;
	CListCtrl	m_ListAutoRun;
	CListCtrl	m_ListDrives;
	CListCtrl	m_List;
	int		m_nDisableDrives;
	int		m_nUsers;
	//}}AFX_DATA
	bool m_bChanged;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideteTweakDlg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CImageList m_ImageList;
	// Generated message map functions
	//{{AFX_MSG(CVideteTweakDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoLstPolicyExplorer(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickLstPolicyExplorer(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioDisabledrives();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBtnRestore();
	afx_msg void OnBtnSave();
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg void OnRdUsers();
	afx_msg void OnBtnImageSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDETETWEAKDLG_H__8D3852DD_EBA8_465F_BAF6_47C42DD8CA88__INCLUDED_)
