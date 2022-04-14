#if !defined(AFX_INFORMATIONDIALOG_H__15549AC1_B2A1_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_INFORMATIONDIALOG_H__15549AC1_B2A1_11D1_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InformationDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInformationDialog dialog

class CInformationDialog : public CWK_Dialog
{
// Construction
public:
	CInformationDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInformationDialog)
	enum { IDD = IDD_INFORMATION };
	CString	m_sEditor;
	CString	m_sInformation;
	CString	m_sObject;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInformationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInformationDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFORMATIONDIALOG_H__15549AC1_B2A1_11D1_8C1B_00C095EC9EFA__INCLUDED_)
