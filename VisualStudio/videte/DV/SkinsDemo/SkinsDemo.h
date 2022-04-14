// SkinsDemo.h : main header file for the SKINSDEMO application
//

#if !defined(AFX_SKINSDEMO_H__CA0A4145_8217_11D3_8DDB_004005A11E32__INCLUDED_)
#define AFX_SKINSDEMO_H__CA0A4145_8217_11D3_8DDB_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSkinsDemoApp:
// See SkinsDemo.cpp for the implementation of this class
//

class CSkinsDemoApp : public CWinApp
{
public:
	CSkinsDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinsDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSkinsDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINSDEMO_H__CA0A4145_8217_11D3_8DDB_004005A11E32__INCLUDED_)
