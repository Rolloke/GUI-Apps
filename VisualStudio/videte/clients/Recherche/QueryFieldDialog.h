#if !defined(AFX_QUERYFIELDDIALOG_H__C7680583_AEA6_11D2_B558_004005A19028__INCLUDED_)
#define AFX_QUERYFIELDDIALOG_H__C7680583_AEA6_11D2_B558_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueryFieldDialog.h : header file
//
class CQueryDialog;
/////////////////////////////////////////////////////////////////////////////
// CQueryFieldDialog dialog

class CQueryFieldDialog : public CDialog
{
// Construction
public:
	CQueryFieldDialog(UINT nIDTemplate,
					  const CString& sName, 
					  CIPCField* pField, 
					  CQueryDialog* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQueryFieldDialog)
	//}}AFX_DATA

	// Overrides
public:
	virtual CString GetName();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryFieldDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQueryFieldDialog)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CQueryDialog* m_pQueryDialog;
	CIPCField*	  m_pField;
	CString		  m_sName;
};
//////////////////////////////////////////////////////////////////////
typedef CQueryFieldDialog* CQueryFieldDialogPtr;
WK_PTR_ARRAY(CQueryFieldDialogs,CQueryFieldDialogPtr);
//////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYFIELDDIALOG_H__C7680583_AEA6_11D2_B558_004005A19028__INCLUDED_)
