/* GlobalReplace: CSDITestNcrNewView --> CViewNcr_1_2 */
/* GlobalReplace: CSDITestNcrNewDoc --> CDocNcr_1_2 */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewNcr_1_2.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewNcr_1_2.h $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 22.04.99 9:14 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTNCRNEWVIEW_H__587945C6_A839_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDITESTNCRNEWVIEW_H__587945C6_A839_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocNcr_1_2.h"

/////////////////////////////////////////////////////////////////////////////
// CViewNcr_1_2 dialog
class CViewNcr_1_2 : public CSDITestView
{
// Construction
	DECLARE_DYNCREATE(CViewNcr_1_2)
public:
	CViewNcr_1_2();   // standard constructor

// Attributes
public:
	CDocNcr_1_2* GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewNcr_1_2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual	void	InitializeData();
	virtual	void	ShowHide();

// Implementation
protected:
	virtual ~CViewNcr_1_2();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewNcr_1_2)
	afx_msg void OnRadioCommandChanged();
	afx_msg void OnRadioVariantChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
	//{{AFX_DATA(CViewNcr_1_2)
	enum { IDD = IDD_NCR_NEW_FORM };
	CButton	m_btnVariant1;
	CButton	m_btnVariant2;
	CButton	m_btnVariant3;
	CButton	m_btnStartCommand;
	CButton	m_btnDiagnose;
	int		m_iCommand;
	int		m_iVariant;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in SDITestNcrView.cpp
inline CDocNcr_1_2* CViewNcr_1_2::GetDocument()
   { return (CDocNcr_1_2*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTNCRNEWVIEW_H__587945C6_A839_11D1_9F29_0000C036AC0D__INCLUDED_)
