#if !defined(AFX_CLEAN98DLG_H__1DF39287_C527_11D4_8C96_004005A1D890__INCLUDED_)
#define AFX_CLEAN98DLG_H__1DF39287_C527_11D4_8C96_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Clean98Dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClean98Dlg dialog

class CClean98Dlg : public CDialog
{
// Construction
public:
	void SetOEM(CString sOEM);
	CClean98Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CClean98Dlg)
	enum { IDD = IDD_CLEAN98 };
	CString	m_sOEM;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClean98Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClean98Dlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLEAN98DLG_H__1DF39287_C527_11D4_8C96_004005A1D890__INCLUDED_)
