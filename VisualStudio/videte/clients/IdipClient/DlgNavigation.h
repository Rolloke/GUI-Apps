#if !defined(AFX_DLGNAVIGATION_H__8BB9F573_D797_11D2_B5A9_004005A19028__INCLUDED_)
#define AFX_DLGNAVIGATION_H__8BB9F573_D797_11D2_B5A9_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NavigationDialog.h : header file
//

class CWndImageRecherche;
/////////////////////////////////////////////////////////////////////////////
// CDlgNavigation dialog

class CDlgNavigation : public CSkinDialog
{
// Construction
public:
	CDlgNavigation(CWndImageRecherche* pImageWindow);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNavigation)
	enum  { IDD = IDD_NAVIGATION };
	CSkinSlider m_ctrlSlider;
	CString	m_sRecordNr;
	//}}AFX_DATA

	// operations
public:
	void EnableSlider(BOOL bEnable);
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNavigation)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNavigation)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CWndImageRecherche* m_pImageWindow;
public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNAVIGATION_H__8BB9F573_D797_11D2_B5A9_004005A19028__INCLUDED_)
