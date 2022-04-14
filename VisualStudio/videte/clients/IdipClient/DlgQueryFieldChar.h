#if !defined(AFX_DLGQUERYFIELDCHAR_H__4F824626_D0A4_11D2_B59E_004005A19028__INCLUDED_)
#define AFX_DLGQUERYFIELDCHAR_H__4F824626_D0A4_11D2_B59E_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgQueryFieldChar.h : header file
//
#include "DlgQueryField.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgQueryFieldChar dialog

class CDlgQueryFieldChar : public CDlgQueryField
{
// Construction
public:
	CDlgQueryFieldChar(const CString& sName,CIPCField* pField,CDlgQuery* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgQueryFieldChar)
	enum  { IDD = IDD_QUERY_FIELD_CHAR };
	CString	m_sValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgQueryFieldChar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgQueryFieldChar)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGQUERYFIELDCHAR_H__4F824626_D0A4_11D2_B59E_004005A19028__INCLUDED_)
