#if !defined(AFX_VIEWDIALOGBANK_H__09F35438_5F91_11D3_BA75_00400531137E__INCLUDED_)
#define AFX_VIEWDIALOGBANK_H__09F35438_5F91_11D3_BA75_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewDialogbank.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocDialogbank.h"

/////////////////////////////////////////////////////////////////////////////
// CViewDialogbank form view
class CViewDialogbank : public CSDITestView
{
protected:
	CViewDialogbank();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewDialogbank)

// Attributes
public:
	CDocDialogbank* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewDialogbank)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual	void	InitializeData();
	virtual	void	ShowHide();

// Implementation
protected:
	virtual ~CViewDialogbank();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewDialogbank)
	afx_msg void OnRadioChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Form Data
public:
	//{{AFX_DATA(CViewDialogbank)
	enum { IDD = IDD_DIALOGBANK };
	CButton	m_btnECCard;
	CButton	m_btnTANCard;
	int		m_iCommand;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in ViewAvmCustomerSafe.cpp
inline CDocDialogbank* CViewDialogbank::GetDocument()
   { return (CDocDialogbank*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWDIALOGBANK_H__09F35438_5F91_11D3_BA75_00400531137E__INCLUDED_)
