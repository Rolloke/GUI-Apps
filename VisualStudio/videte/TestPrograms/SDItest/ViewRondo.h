#if !defined(AFX_VIEWRONDO_H__09F35436_5F91_11D3_BA75_00400531137E__INCLUDED_)
#define AFX_VIEWRONDO_H__09F35436_5F91_11D3_BA75_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewRondo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocRondo.h"

/////////////////////////////////////////////////////////////////////////////
// CViewRondo form view
class CViewRondo : public CSDITestView
{
protected:
	CViewRondo();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewRondo)

// Attributes
public:
	CDocRondo* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewRondo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual	void	InitializeData();
	virtual	void	ShowHide();

// Implementation
protected:
	virtual ~CViewRondo();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewRondo)
	afx_msg void OnRadioChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Form Data
protected:
	//{{AFX_DATA(CViewRondo)
	enum { IDD = IDD_RONDO };
	CButton	m_btnStatusRequest;
	CButton	m_btnQuiet;
	CButton	m_btnPortrait;
	CButton	m_btnMoney;
	int		m_iCommand;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in ViewAvmCustomerSafe.cpp
inline CDocRondo* CViewRondo::GetDocument()
   { return (CDocRondo*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWRONDO_H__09F35436_5F91_11D3_BA75_00400531137E__INCLUDED_)
