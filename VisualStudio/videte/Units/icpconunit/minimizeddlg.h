#if !defined(AFX_MINIMIZEDDLG_H__1AE95EE8_C8B3_4098_8EE3_28106A8247DA__INCLUDED_)
#define AFX_MINIMIZEDDLG_H__1AE95EE8_C8B3_4098_8EE3_28106A8247DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MinimizedDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMinimizedDlg dialog

class CMinimizedDlg : public CDialog
{
// Construction
public:
	CMinimizedDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMinimizedDlg)
	enum { IDD = IDD_MINIMIZED_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMinimizedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMinimizedDlg)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	BOOL m_bFirstMouseMsg;
	UINT m_nTimer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MINIMIZEDDLG_H__1AE95EE8_C8B3_4098_8EE3_28106A8247DA__INCLUDED_)
