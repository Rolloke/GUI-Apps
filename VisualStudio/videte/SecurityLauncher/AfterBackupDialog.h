#if !defined(AFX_AFTERBACKUPDIALOG_H__B2DEE342_397A_11D1_93E1_00C095ECA33E__INCLUDED_)
#define AFX_AFTERBACKUPDIALOG_H__B2DEE342_397A_11D1_93E1_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AfterBackupDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAfterBackupDialog dialog

class CAfterBackupDialog : public CSkinDialog
{
// Construction
public:
	CAfterBackupDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAfterBackupDialog)
	enum eID { IDD = IDD_AFTER_BACKUP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAfterBackupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAfterBackupDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AFTERBACKUPDIALOG_H__B2DEE342_397A_11D1_93E1_00C095ECA33E__INCLUDED_)
