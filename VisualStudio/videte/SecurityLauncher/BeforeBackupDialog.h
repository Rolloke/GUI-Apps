#if !defined(AFX_BEFOREBACKUPDIALOG_H__CC2117A2_3976_11D1_93E1_00C095ECA33E__INCLUDED_)
#define AFX_BEFOREBACKUPDIALOG_H__CC2117A2_3976_11D1_93E1_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BeforeBackupDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBeforeBackupDialog dialog

class CBeforeBackupDialog : public CSkinDialog
{
// Construction
public:
	CBeforeBackupDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBeforeBackupDialog)
	enum eID { IDD = IDD_BEFORE_BACKUP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBeforeBackupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBeforeBackupDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEFOREBACKUPDIALOG_H__CC2117A2_3976_11D1_93E1_00C095ECA33E__INCLUDED_)
