// DongleTester.h : main header file for the DONGLETESTER application
//

#if !defined(AFX_DONGLETESTER_H__5FFEF746_493E_11D1_B8C8_0060977A76F1__INCLUDED_)
#define AFX_DONGLETESTER_H__5FFEF746_493E_11D1_B8C8_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDongleTesterApp:
// See DongleTester.cpp for the implementation of this class
//

class CDongleTesterApp : public CWinApp
{
public:
	CDongleTesterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDongleTesterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDongleTesterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DONGLETESTER_H__5FFEF746_493E_11D1_B8C8_0060977A76F1__INCLUDED_)
