#if !defined(AFX_NAVIGATIONDIALOG_H__8BB9F573_D797_11D2_B5A9_004005A19028__INCLUDED_)
#define AFX_NAVIGATIONDIALOG_H__8BB9F573_D797_11D2_B5A9_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NavigationDialog.h : header file
//

class CImageWindow;
/////////////////////////////////////////////////////////////////////////////
// CNavigationDialog dialog

class CNavigationDialog : public CDialog
{
// Construction
public:
	CNavigationDialog(CImageWindow* pImageWindow);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNavigationDialog)
	enum { IDD = IDD_NAVIGATION };
	CSliderCtrl	m_ctrlSlider;
	CString	m_sRecordNr;
	//}}AFX_DATA

	// operations
public:
	void EnableSlider(BOOL bEnable);
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavigationDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNavigationDialog)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CImageWindow* m_pImageWindow;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAVIGATIONDIALOG_H__8BB9F573_D797_11D2_B5A9_004005A19028__INCLUDED_)
