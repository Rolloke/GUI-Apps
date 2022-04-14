#if !defined(AFX_DLGQUERYFIELDDATE_H__4F824627_D0A4_11D2_B59E_004005A19028__INCLUDED_)
#define AFX_DLGQUERYFIELDDATE_H__4F824627_D0A4_11D2_B59E_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgQueryFieldDate.h : header file
//
#include "DlgQueryField.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryFieldDate dialog

class CDlgQueryFieldDate : public CDlgQueryField
{
// Construction
public:
	CDlgQueryFieldDate(const CString& sName, 
						  CIPCField* pField, 
					      CDlgQuery* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgQueryFieldDate)
	enum  { IDD = IDD_QUERY_FIELD_DATE };
	CSkinDateTimeCtrl	m_ctrlDate;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgQueryFieldDate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgQueryFieldDate)
	virtual BOOL OnInitDialog();
	afx_msg void OnDatetimechangeDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCloseupDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUserstringDate(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGQUERYFIELDDATE_H__4F824627_D0A4_11D2_B59E_004005A19028__INCLUDED_)
