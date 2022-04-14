// CInfoDlg.h: interface for the CInfoDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CINFODLG_H__42A9AA73_7A5F_11D4_8F76_004005A11E32__INCLUDED_)
#define AFX_CINFODLG_H__42A9AA73_7A5F_11D4_8F76_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog		  

class CInfoDlg : public CTransparentDialog
{
// Construction
public:
	void ShowInfoText(const CString& sInfo);
	virtual  ~CInfoDlg();
	BOOL Create();
	CInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInfoDlg)
	enum { IDD = IDD_INFO };
	CFrame		m_ctrlBorderInfo;
	CDisplay	m_ctrlIInfoText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInfoDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // !defined(AFX_CINFODLG_H__42A9AA73_7A5F_11D4_8F76_004005A11E32__INCLUDED_)
