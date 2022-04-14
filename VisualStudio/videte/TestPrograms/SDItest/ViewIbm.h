/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewIbm.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewIbm.h $
// CHECKIN:		$Date: 10.05.99 11:56 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 10.05.99 10:58 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTIBMVIEW_H__FE93CAC4_367A_11D2_AFCB_00C095EC2267__INCLUDED_)
#define AFX_SDITESTIBMVIEW_H__FE93CAC4_367A_11D2_AFCB_00C095EC2267__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestView.h"
#include "DocIbm.h"

/////////////////////////////////////////////////////////////////////////////
// CViewIbm form view
class CViewIbm : public CSDITestView
{
protected:
	CViewIbm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CViewIbm)

// Attributes
public:
	CDocIbm* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewIbm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual	void	InitializeData();
	virtual	void	ShowHide();

// Implementation
protected:
	virtual ~CViewIbm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CViewIbm)
	afx_msg void OnRadioChanged();
	afx_msg void OnChangeEditDataFormat();
	afx_msg void OnCheckStatusRequest();
	afx_msg void OnCheckWithTerminalID();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Form Data
public:
	//{{AFX_DATA(CViewIbm)
	enum { IDD = IDD_IBM_FORM };
	CButton	m_btnWithTerminalID;
	CButton	m_btnStatusRequest;
	CEdit	m_editDataFormat;
	CButton	m_btnPointOfData1;
	CButton	m_btnPointOfData2;
	CButton	m_btnPointOfData3;
	CButton	m_btnPointOfData4;
	CButton	m_btnPointOfData5;
	CButton	m_btnPointOfData6;
	CButton	m_btnPointOfData7;
	CButton	m_btnPointOfData8;
	CButton	m_btnPointOfData9;
	CButton	m_btnPointOfData10;
	CButton	m_btnPointOfData11;
	CButton	m_btnPointOfData12;
	CButton	m_btnPointOfData13;
	CButton	m_btnPointOfData14;
	CButton	m_btnPointOfData15;
	CButton	m_btnPointOfData16;
	int		m_iPointOfData;
	CString	m_sDataFormat;
	BOOL	m_bStatusRequest;
	BOOL	m_bWithTerminalID;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in SDITestIbmView.cpp
inline CDocIbm* CViewIbm::GetDocument()
   { return (CDocIbm*)m_pDocument; }
#endif
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTIBMVIEW_H__FE93CAC4_367A_11D2_AFCB_00C095EC2267__INCLUDED_)
