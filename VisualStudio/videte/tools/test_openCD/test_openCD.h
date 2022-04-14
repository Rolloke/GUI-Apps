// test_openCD.h : main header file for the TEST_OPENCD application
//

#if !defined(AFX_TEST_OPENCD_H__17C5BEF5_62EB_11D4_ADD6_004005A1D890__INCLUDED_)
#define AFX_TEST_OPENCD_H__17C5BEF5_62EB_11D4_ADD6_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "afxmt.h"
#include "CIPCDrive.h"

/////////////////////////////////////////////////////////////////////////////
// CTest_openCDApp:
// See test_openCD.cpp for the implementation of this class
//

class CTest_openCDApp : public CWinApp
{
public:
	CTest_openCDApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTest_openCDApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTest_openCDApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEST_OPENCD_H__17C5BEF5_62EB_11D4_ADD6_004005A1D890__INCLUDED_)
