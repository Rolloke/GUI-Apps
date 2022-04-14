#if !defined(AFX_QUERYFIELDDATEDIALOG_H__4F824627_D0A4_11D2_B59E_004005A19028__INCLUDED_)
#define AFX_QUERYFIELDDATEDIALOG_H__4F824627_D0A4_11D2_B59E_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueryFieldDateDialog.h : header file
//
#include "QueryFieldDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CQueryFieldDateDialog dialog

class CQueryFieldDateDialog : public CQueryFieldDialog
{
// Construction
public:
	CQueryFieldDateDialog(const CString& sName, 
						  CIPCField* pField, 
					      CQueryDialog* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQueryFieldDateDialog)
	enum { IDD = IDD_QUERY_FIELD_DATE };
	CDateTimeCtrl	m_ctrlDate;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryFieldDateDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQueryFieldDateDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDatetimechangeDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCloseupDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUserstringDate(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYFIELDDATEDIALOG_H__4F824627_D0A4_11D2_B59E_004005A19028__INCLUDED_)
