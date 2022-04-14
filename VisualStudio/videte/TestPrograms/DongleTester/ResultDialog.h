#if !defined(AFX_RESULTDIALOG_H__5FFEF750_493E_11D1_B8C8_0060977A76F1__INCLUDED_)
#define AFX_RESULTDIALOG_H__5FFEF750_493E_11D1_B8C8_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ResultDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ResultDialog dialog

class ResultDialog : public CDialog
{
// Construction
public:
	ResultDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ResultDialog)
	enum { IDD = IDD_RESULT };
	CString	m_sElapsedTime;
	CString	m_sNumTests;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ResultDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ResultDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESULTDIALOG_H__5FFEF750_493E_11D1_B8C8_0060977A76F1__INCLUDED_)
