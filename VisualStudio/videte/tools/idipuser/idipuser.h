// idipuser.h : main header file for the IDIPUSER application
//

#if !defined(AFX_IDIPUSER_H__414BF8AF_47D5_4134_ADA3_87DD8891B930__INCLUDED_)
#define AFX_IDIPUSER_H__414BF8AF_47D5_4134_ADA3_87DD8891B930__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIdipuserApp:
// See idipuser.cpp for the implementation of this class
//

class CIdipuserApp : public CWinApp
{
public:
	CIdipuserApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIdipuserApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIdipuserApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDIPUSER_H__414BF8AF_47D5_4134_ADA3_87DD8891B930__INCLUDED_)
