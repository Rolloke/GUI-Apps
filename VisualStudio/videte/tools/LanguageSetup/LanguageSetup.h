// LanguageSetup.h : main header file for the LANGUAGESETUP application
//

#if !defined(AFX_LANGUAGESETUP_H__F70FE1E1_3B46_4D8B_A7CB_49B3C34F5D6E__INCLUDED_)
#define AFX_LANGUAGESETUP_H__F70FE1E1_3B46_4D8B_A7CB_49B3C34F5D6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLanguageSetupApp:
// See LanguageSetup.cpp for the implementation of this class
//

class CLanguageSetupApp : public CWinApp
{
public:
	CLanguageSetupApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLanguageSetupApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLanguageSetupApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LANGUAGESETUP_H__F70FE1E1_3B46_4D8B_A7CB_49B3C34F5D6E__INCLUDED_)
