#if !defined(AFX_DLGQUERYFIELD_H__C7680583_AEA6_11D2_B558_004005A19028__INCLUDED_)
#define AFX_DLGQUERYFIELD_H__C7680583_AEA6_11D2_B558_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgQueryField.h : header file
//
class CDlgQuery;
/////////////////////////////////////////////////////////////////////////////
// CDlgQueryField dialog

class CDlgQueryField : public CSkinDialog
{
// Construction
public:
	CDlgQueryField(UINT nIDTemplate,
					  const CString& sName, 
					  CIPCField* pField, 
					  CDlgQuery* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgQueryField)
	//}}AFX_DATA

	// Overrides
public:
	virtual CString GetName();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgQueryField)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgQueryField)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CDlgQuery* m_pDlgQuery;
	CIPCField*	  m_pField;
	CString		  m_sName;
};
//////////////////////////////////////////////////////////////////////
typedef CDlgQueryField* CDlgQueryFieldPtr;
WK_PTR_ARRAY(CDlgQueryFields,CDlgQueryFieldPtr);
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGQUERYFIELD_H__C7680583_AEA6_11D2_B558_004005A19028__INCLUDED_)
