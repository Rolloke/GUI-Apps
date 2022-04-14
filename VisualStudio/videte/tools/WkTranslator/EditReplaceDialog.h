#if !defined(AFX_EDITREPLACEDIALOG_H__8F09E8A1_DD9D_11D5_9A6E_004005A19028__INCLUDED_)
#define AFX_EDITREPLACEDIALOG_H__8F09E8A1_DD9D_11D5_9A6E_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditReplaceDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditReplaceDialog dialog

class CEditReplaceDialog : public CDialog
{
// Construction
public:
	CEditReplaceDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditReplaceDialog)
	enum { IDD = IDD_EDIT_REPLACE };
	CEdit	m_cReplace;
	CEdit	m_cFind;
	CString	m_sFind;
	CString	m_sReplace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditReplaceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditReplaceDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITREPLACEDIALOG_H__8F09E8A1_DD9D_11D5_9A6E_004005A19028__INCLUDED_)
