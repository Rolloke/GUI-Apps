// IPBook.h : main header file for the IPBOOK application
//

#if !defined(AFX_IPBOOK_H__B5B72937_CC12_11D4_86C3_004005A26A3B__INCLUDED_)
#define AFX_IPBOOK_H__B5B72937_CC12_11D4_86C3_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIPBookApp:
// See IPBook.cpp for the implementation of this class
//

class CIPBookApp : public CWinApp
{
public:
	CIPBookApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPBookApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIPBookApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPBOOK_H__B5B72937_CC12_11D4_86C3_004005A26A3B__INCLUDED_)
