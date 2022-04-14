/* GlobalReplace: CSDITestDtpDoc --> CDocLandauDtp */
/* GlobalReplace: CSDITestDtpView --> CViewLandauDtp */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewLandauDtp.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewLandauDtp.h $
// CHECKIN:		$Date: 28.01.00 12:09 $
// VERSION:		$Revision: 14 $
// LAST CHANGE:	$Modtime: 28.01.00 11:45 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTDTPVIEW_H__2D22C52C_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDITESTDTPVIEW_H__2D22C52C_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocLandauDtp.h"

/////////////////////////////////////////////////////////////////////////////
// CViewLandauDtp form view
class CViewLandauDtp : public CSDITestView
{
protected:
	CViewLandauDtp();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewLandauDtp)

// Attributes
public:
	CDocLandauDtp* GetDocument();

// Operations
public:

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewLandauDtp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual	void	InitializeData();
	virtual	void	ShowHide();

// Implementation
protected:
	virtual ~CViewLandauDtp();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewLandauDtp)
	afx_msg void OnRadioChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Form Data
public:
	//{{AFX_DATA(CViewLandauDtp)
	enum { IDD = IDD_DTP_FORM };
	CButton	m_btnPortNr1;
	CButton	m_btnPortNr2;
	CButton	m_btnPortNr3;
	CButton	m_btnPortNr4;
	CButton	m_btnPortUpdate;
	CButton	m_btnCameraNr1;
	CButton	m_btnCameraNr2;
	CButton	m_btnCameraUpdateNull;
	CButton	m_btnCameraUpdateSpace;
	int		m_iPortNr;
	int		m_iCameraNr;
	//}}AFX_DATA

//	friend class CDocLandauDtp;
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in SDITestView.cpp
inline CDocLandauDtp* CViewLandauDtp::GetDocument()
   { return (CDocLandauDtp*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTDTPVIEW_H__2D22C52C_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
