// MiCoDLRTest.h : main header file for the MICODLRTEST application
//

#if !defined(AFX_MICODLRTEST_H__3284E1E5_E73E_11D2_8CFD_004005A11E32__INCLUDED_)
#define AFX_MICODLRTEST_H__3284E1E5_E73E_11D2_8CFD_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMiCoDLRTestApp:
// See MiCoDLRTest.cpp for the implementation of this class
//

class CMiCoDLRTestApp : public CWinApp
{
public:
	CMiCoDLRTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiCoDLRTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMiCoDLRTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MICODLRTEST_H__3284E1E5_E73E_11D2_8CFD_004005A11E32__INCLUDED_)
