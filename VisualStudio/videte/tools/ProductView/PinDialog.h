#if !defined(AFX_PINDIALOG_H__ADA4D556_DCA0_11D4_9987_004005A19028__INCLUDED_)
#define AFX_PINDIALOG_H__ADA4D556_DCA0_11D4_9987_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PinDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPinDialog dialog

class CPinDialog : public CDialog
{
// Construction
public:
	CPinDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPinDialog)
	enum { IDD = IDD_PIN };
	CString	m_sPin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPinDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPinDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PINDIALOG_H__ADA4D556_DCA0_11D4_9987_004005A19028__INCLUDED_)
