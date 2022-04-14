/* GlobalReplace: CSDITestGermanParcelView --> CViewGermanParcel */
/* GlobalReplace: CSDITestGermanParcelDoc --> CDocGermanParcel */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewGermanParcel.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewGermanParcel.h $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 22.04.99 9:12 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTGERMANPARCELVIEW_H__FA98AAB6_3E3D_11D2_AFDB_00C095EC2267__INCLUDED_)
#define AFX_SDITESTGERMANPARCELVIEW_H__FA98AAB6_3E3D_11D2_AFDB_00C095EC2267__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocGermanParcel.h"

/////////////////////////////////////////////////////////////////////////////
// CViewGermanParcel form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CViewGermanParcel : public CSDITestView
{
protected:
	CViewGermanParcel();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewGermanParcel)

// Attributes
public:
	CDocGermanParcel* GetDocument();

// Operations
public:

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewGermanParcel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:

// Implementation
protected:
	virtual ~CViewGermanParcel();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewGermanParcel)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Form Data
public:
	//{{AFX_DATA(CViewGermanParcel)
	enum { IDD = IDD_GERMAN_PARCEL_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in SDITestView.cpp
inline CDocGermanParcel* CViewGermanParcel::GetDocument()
   { return (CDocGermanParcel*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTGERMANPARCELVIEW_H__FA98AAB6_3E3D_11D2_AFDB_00C095EC2267__INCLUDED_)
