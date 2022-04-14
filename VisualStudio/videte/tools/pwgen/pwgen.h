// pwgen.h : main header file for the PWGEN application
//

#if !defined(AFX_PWGEN_H__0E92A147_D873_11D2_B5AD_004005A19028__INCLUDED_)
#define AFX_PWGEN_H__0E92A147_D873_11D2_B5AD_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPwgenApp:
// See pwgen.cpp for the implementation of this class
//

class CPwgenApp : public CWinApp
{
public:
	CPwgenApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPwgenApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPwgenApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PWGEN_H__0E92A147_D873_11D2_B5AD_004005A19028__INCLUDED_)
