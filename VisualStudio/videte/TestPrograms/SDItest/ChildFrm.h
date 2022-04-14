/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ChildFrm.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ChildFrm.h $
// CHECKIN:		$Date: 11.03.98 10:34 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 10.03.98 12:29 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_CHILDFRM_H__2D22C51F_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_CHILDFRM_H__2D22C51F_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__2D22C51F_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
