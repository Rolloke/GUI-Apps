// TestPnP.h : main header file for the TESTPNP application
//

#if !defined(AFX_TESTPNP_H__C6B40F96_F226_4408_94B8_64E0EC670894__INCLUDED_)
#define AFX_TESTPNP_H__C6B40F96_F226_4408_94B8_64E0EC670894__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestPnPApp:
// See TestPnP.cpp for the implementation of this class
//

class CTestPnPApp : public CWinApp
{
public:
	CTestPnPApp();
		CString m_strProgramPath;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestPnPApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestPnPApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTPNP_H__C6B40F96_F226_4408_94B8_64E0EC670894__INCLUDED_)
