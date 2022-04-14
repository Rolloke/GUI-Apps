#if !defined(AFX_SYSTEMTIMEOUTS_H__E5CFE381_0C8B_4C24_8FC9_06B8F0579542__INCLUDED_)
#define AFX_SYSTEMTIMEOUTS_H__E5CFE381_0C8B_4C24_8FC9_06B8F0579542__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SystemTimeOuts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSystemTimeOuts dialog

class CSystemTimeOuts : public CPropertyPage
{
	DECLARE_DYNCREATE(CSystemTimeOuts)

// Construction
public:
	CSystemTimeOuts();
	~CSystemTimeOuts();

// Dialog Data
	//{{AFX_DATA(CSystemTimeOuts)
	enum { IDD = IDD_SYSTEM_TIMEOUTS };
	CListCtrl	m_List;
	//}}AFX_DATA

	bool m_bChanged;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSystemTimeOuts)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSystemTimeOuts)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetdispinfoLstTimeout(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditLstTimeout(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnRestore();
	afx_msg void OnBtnSave();
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg void OnRclickLstTimeout(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMTIMEOUTS_H__E5CFE381_0C8B_4C24_8FC9_06B8F0579542__INCLUDED_)
