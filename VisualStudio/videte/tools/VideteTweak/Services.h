#if !defined(AFX_SERVICES_H__B9A78799_E437_4A56_A4AD_945900D8DE49__INCLUDED_)
#define AFX_SERVICES_H__B9A78799_E437_4A56_A4AD_945900D8DE49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Services.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CServices dialog

class CServices : public CPropertyPage
{
	DECLARE_DYNCREATE(CServices)

// Construction
public:
	CServices();
	~CServices();

// Dialog Data
	//{{AFX_DATA(CServices)
	enum { IDD = IDD_SERVICES };
	CListCtrl	m_cServices;
	int		m_nProduct;
	//}}AFX_DATA

	bool m_bChanged;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CServices)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CServices)
	afx_msg void OnBtnRestore();
	afx_msg void OnBtnSave();
	virtual BOOL OnInitDialog();
	afx_msg void OnRclickLstServices(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString GetStartText(DWORD);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVICES_H__B9A78799_E437_4A56_A4AD_945900D8DE49__INCLUDED_)
