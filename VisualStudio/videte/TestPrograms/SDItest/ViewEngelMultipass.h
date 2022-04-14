/* GlobalReplace: CSDITestEngelView --> CViewEngelMultipass */
/* GlobalReplace: CSDITestEngelDoc --> CDocEngelMultipass */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewEngelMultipass.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewEngelMultipass.h $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 22.04.99 9:06 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTENGELVIEW_H__FAD19761_AC61_11D1_8143_DB20799D17F6__INCLUDED_)
#define AFX_SDITESTENGELVIEW_H__FAD19761_AC61_11D1_8143_DB20799D17F6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocEngelMultipass.h"

/////////////////////////////////////////////////////////////////////////////
// CViewEngelMultipass form view
class CViewEngelMultipass : public CSDITestView
{
protected:
	CViewEngelMultipass();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewEngelMultipass)

// Attributes
public:
	CDocEngelMultipass* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewEngelMultipass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual	void	InitializeData();
	virtual	void	ShowHide();

// Implementation
protected:
	virtual ~CViewEngelMultipass();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewEngelMultipass)
	afx_msg void OnRadioChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Form Data
public:
	//{{AFX_DATA(CViewEngelMultipass)
	enum { IDD = IDD_ENGEL_FORM };
	CButton	m_btnCommandL;
	CButton	m_btnCommandE;
	int		m_iCommand;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in SDITestEngelView.cpp
inline CDocEngelMultipass* CViewEngelMultipass::GetDocument()
   { return (CDocEngelMultipass*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTENGELVIEW_H__FAD19761_AC61_11D1_8143_DB20799D17F6__INCLUDED_)
