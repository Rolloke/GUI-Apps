/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ChildFrm.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/ChildFrm.h $
// CHECKIN:		$Date: 16.01.98 9:57 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.12.97 13:21 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_CHILDFRM_H__3716C0EE_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
#define AFX_CHILDFRM_H__3716C0EE_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "SDIMDIChildWnd.h"

class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Attributes
public:

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__3716C0EE_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
