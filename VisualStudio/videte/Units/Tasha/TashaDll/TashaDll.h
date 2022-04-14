// TashaDll.h : main header file for the TASHADLL DLL
//

#if !defined(AFX_TASHADLL_H__48553305_D084_4740_AC71_0EDF4F6DD65E__INCLUDED_)
#define AFX_TASHADLL_H__48553305_D084_4740_AC71_0EDF4F6DD65E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

static AFX_EXTENSION_MODULE TashaDll = { NULL, NULL };

/////////////////////////////////////////////////////////////////////////////
// CTashaDllApp
// See TashaDll.cpp for the implementation of this class
//

class CTashaDllApp : public CWinApp
{
public:
	CTashaDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTashaDllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTashaDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASHADLL_H__48553305_D084_4740_AC71_0EDF4F6DD65E__INCLUDED_)
