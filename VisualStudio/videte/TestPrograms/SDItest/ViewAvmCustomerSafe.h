// ViewAvmCustomerSafe.h : header file
//
#if !defined(AFX_VIEWAVMCUSTOMERSAFE_H__4C6F5A37_F7B6_11D2_B9FA_00400531137E__INCLUDED_)
#define AFX_VIEWAVMCUSTOMERSAFE_H__4C6F5A37_F7B6_11D2_B9FA_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocAvmCustomerSafe.h"

/////////////////////////////////////////////////////////////////////////////
// CViewAvmCustomerSafe form view
class CViewAvmCustomerSafe : public CSDITestView
{
protected:
	CViewAvmCustomerSafe();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewAvmCustomerSafe)

// Attributes
public:
	CDocAvmCustomerSafe* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewAvmCustomerSafe)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual	void	InitializeData();
	virtual	void	ShowHide();

// Implementation
protected:
	virtual ~CViewAvmCustomerSafe();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewAvmCustomerSafe)
	afx_msg void OnRadioChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Form Data
protected:
	//{{AFX_DATA(CViewAvmCustomerSafe)
	enum { IDD = IDD_AVM_FORM };
	CButton	m_btnCheckAccess;
	CButton	m_btnAccess;
	CButton	m_btnRelease;
	int		m_iCommand;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in ViewAvmCustomerSafe.cpp
inline CDocAvmCustomerSafe* CViewAvmCustomerSafe::GetDocument()
   { return (CDocAvmCustomerSafe*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWAVMCUSTOMERSAFE_H__4C6F5A37_F7B6_11D2_B9FA_00400531137E__INCLUDED_)
