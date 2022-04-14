// Reseter.h : main header file for the RESETER application
//

#if !defined(AFX_RESETER_H__0D1F8B65_217F_11D2_9FB5_00C095ECA33E__INCLUDED_)
#define AFX_RESETER_H__0D1F8B65_217F_11D2_9FB5_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CReseterApp:
// See Reseter.cpp for the implementation of this class
//

class CReseterApp : public CWinApp
{
public:
	CReseterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReseterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CReseterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESETER_H__0D1F8B65_217F_11D2_9FB5_00C095ECA33E__INCLUDED_)
