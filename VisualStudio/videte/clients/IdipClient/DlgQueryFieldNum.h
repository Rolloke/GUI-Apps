#if !defined(AFX_DLGQUERYFIELDNUM_H__4F824628_D0A4_11D2_B59E_004005A19028__INCLUDED_)
#define AFX_DLGQUERYFIELDNUM_H__4F824628_D0A4_11D2_B59E_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgQueryFieldNum.h : header file
//
#include "DlgQueryField.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryFieldNum dialog

class CDlgQueryFieldNum : public CDlgQueryField
{
// Construction
public:
	CDlgQueryFieldNum(const CString& sName, 
						  CIPCField* pField, 
					      CDlgQuery* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgQueryFieldNum)
	enum  { IDD = IDD_QUERY_FIELD_NUM };
	CString	m_sValue;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgQueryFieldNum)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgQueryFieldNum)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGQUERYFIELDNUM_H__4F824628_D0A4_11D2_B59E_004005A19028__INCLUDED_)
