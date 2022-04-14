// TestUnit.h : main header file for the TESTUNIT application
//

#if !defined(AFX_TESTUNIT_H__5CB64E05_5ADD_11D3_A9C5_004005A11E32__INCLUDED_)
#define AFX_TESTUNIT_H__5CB64E05_5ADD_11D3_A9C5_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestUnitApp:
// See TestUnit.cpp for the implementation of this class
//

class CTestUnitApp : public CWinApp
{
public:
	CTestUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestUnitApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTUNIT_H__5CB64E05_5ADD_11D3_A9C5_004005A11E32__INCLUDED_)
