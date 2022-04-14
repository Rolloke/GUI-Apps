// ViewKebaPasador.h: interface for the CViewKebaPasador class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWKEBAPASADOR_H__B8F7F033_5AD9_11D3_BA6E_00400531137E__INCLUDED_)
#define AFX_VIEWKEBAPASADOR_H__B8F7F033_5AD9_11D3_BA6E_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocKebaPasador.h"

class CViewKebaPasador : public CSDITestView  
{
public:
	CViewKebaPasador();
	DECLARE_DYNCREATE(CViewKebaPasador)

// Attributes
public:
	CDocKebaPasador* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewKebaPasador)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual	void	InitializeData();
	virtual	void	ShowHide();

// Implementation
protected:
	virtual ~CViewKebaPasador();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewKebaPasador)
	afx_msg void OnChangeEditFilename();
	afx_msg void OnBtnBrowse();
	afx_msg void OnKillfocusEditFilename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Form Data
protected:
	//{{AFX_DATA(CViewKebaPasador)
	enum { IDD = IDD_KEBA_PASADOR };
	CString	m_sFileName;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in SDITestView.cpp
inline CDocKebaPasador* CViewKebaPasador::GetDocument()
   { return (CDocKebaPasador*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_VIEWKEBAPASADOR_H__B8F7F033_5AD9_11D3_BA6E_00400531137E__INCLUDED_)
