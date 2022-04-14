// CreateAvi.h : main header file for the CREATEAVI application
//

#if !defined(AFX_CREATEAVI_H__5CDA0C15_6E8D_11D4_BAE7_00400531137E__INCLUDED_)
#define AFX_CREATEAVI_H__5CDA0C15_6E8D_11D4_BAE7_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCreateAviApp:
// See CreateAvi.cpp for the implementation of this class
//

class CCreateAviApp : public CWinApp
{
public:
	CCreateAviApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateAviApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCreateAviApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATEAVI_H__5CDA0C15_6E8D_11D4_BAE7_00400531137E__INCLUDED_)
