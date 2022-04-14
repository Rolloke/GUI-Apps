// ViewSniCom.h: interface for the CViewSniCom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWSNICOM_H__CD112DF2_255D_11D3_BA46_00400531137E__INCLUDED_)
#define AFX_VIEWSNICOM_H__CD112DF2_255D_11D3_BA46_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "SDITestView.h"
#include "DocSniCom.h"

/////////////////////////////////////////////////////////////////////////////
// CViewSniCom form view
class CViewSniCom : public CSDITestView  
{
public:
	CViewSniCom();
	DECLARE_DYNCREATE(CViewSniCom)

// Attributes
public:
	CDocSniCom* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewSniCom)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual	void	InitializeData();
	virtual	void	ShowHide();

// Implementation
protected:
	virtual ~CViewSniCom();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewSniCom)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Form Data
public:
	//{{AFX_DATA(CViewSniCom)
	enum { IDD = IDD_SNI_COM_FORM };
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in SDITestView.cpp
inline CDocSniCom* CViewSniCom::GetDocument()
   { return (CDocSniCom*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWSNICOM_H__CD112DF2_255D_11D3_BA46_00400531137E__INCLUDED_)
