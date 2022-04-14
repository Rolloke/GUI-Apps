/* GlobalReplace: CSDITestSniView --> CViewSniDos */
/* GlobalReplace: CSDITestSniDoc --> CDocSniDos */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewSniDos.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewSniDos.h $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 22.04.99 9:15 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTSNIVIEW_H__FAD19763_AC61_11D1_8143_DB20799D17F6__INCLUDED_)
#define AFX_SDITESTSNIVIEW_H__FAD19763_AC61_11D1_8143_DB20799D17F6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocSniDos.h"

/////////////////////////////////////////////////////////////////////////////
// CViewSniDos form view
class CViewSniDos : public CSDITestView
{
protected:
	CViewSniDos();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewSniDos)

// Attributes
public:
	CDocSniDos* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewSniDos)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual	void	InitializeData();
	virtual	void	ShowHide();

// Implementation
protected:
	virtual ~CViewSniDos();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewSniDos)
	afx_msg void OnRadioChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Form Data
public:
	//{{AFX_DATA(CViewSniDos)
	enum { IDD = IDD_SNI_FORM };
	CButton	m_btnPointOfData0;
	CButton	m_btnPointOfData1;
	CButton	m_btnPointOfData2;
	CButton	m_btnPointOfData3;
	CButton	m_btnPointOfData4;
	CButton	m_btnPointOfData5;
	CButton	m_btnPointOfData6;
	CButton	m_btnPointOfData7;
	CButton	m_btnPointOfData8;
	CButton	m_btnPointOfData9;
	int		m_iPointOfData;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in SDITestView.cpp
inline CDocSniDos* CViewSniDos::GetDocument()
   { return (CDocSniDos*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTSNIVIEW_H__FAD19763_AC61_11D1_8143_DB20799D17F6__INCLUDED_)
