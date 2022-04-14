/* GlobalReplace: CSDITestKebaView --> CViewKebaPc2000 */
/* GlobalReplace: CSDITestKebaDoc --> CDocKebaPc2000 */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewKebaPc2000.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewKebaPc2000.h $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 22.04.99 9:13 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTKEBAVIEW_H__B68595A5_A44D_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDITESTKEBAVIEW_H__B68595A5_A44D_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocKebaPc2000.h"

/////////////////////////////////////////////////////////////////////////////
// CViewKebaPc2000 form view
class CViewKebaPc2000 : public CSDITestView
{
protected:
	CViewKebaPc2000();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewKebaPc2000)

// Attributes
public:
	CDocKebaPc2000* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewKebaPc2000)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual	void	InitializeData();
	virtual	void	ShowHide();

// Implementation
protected:
	virtual ~CViewKebaPc2000();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewKebaPc2000)
	afx_msg void OnRadioChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Form Data
public:
	//{{AFX_DATA(CViewKebaPc2000)
	enum { IDD = IDD_KEBA_FORM };
	CButton	m_btnCardReader1;
	CButton	m_btnCardReader2;
	int		m_iCardReader;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in SDITestView.cpp
inline CDocKebaPc2000* CViewKebaPc2000::GetDocument()
   { return (CDocKebaPc2000*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTKEBAVIEW_H__B68595A5_A44D_11D1_9F29_0000C036AC0D__INCLUDED_)
