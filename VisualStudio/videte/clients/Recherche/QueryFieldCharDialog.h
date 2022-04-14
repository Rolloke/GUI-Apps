#if !defined(AFX_QUERYFIELDCHARDIALOG_H__4F824626_D0A4_11D2_B59E_004005A19028__INCLUDED_)
#define AFX_QUERYFIELDCHARDIALOG_H__4F824626_D0A4_11D2_B59E_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueryFieldCharDialog.h : header file
//
#include "QueryFieldDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CQueryFieldCharDialog dialog

class CQueryFieldCharDialog : public CQueryFieldDialog
{
// Construction
public:
	CQueryFieldCharDialog(const CString& sName,CIPCField* pField,CQueryDialog* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQueryFieldCharDialog)
	enum { IDD = IDD_QUERY_FIELD_CHAR };
	CString	m_sValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryFieldCharDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQueryFieldCharDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYFIELDCHARDIALOG_H__4F824626_D0A4_11D2_B59E_004005A19028__INCLUDED_)
