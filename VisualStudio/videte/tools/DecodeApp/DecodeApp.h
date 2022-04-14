// DecodeApp.h : main header file for the DECODEAPP application
//

#if !defined(AFX_DECODEAPP_H__B651B39B_1A18_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_DECODEAPP_H__B651B39B_1A18_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDecodeAppApp:
// See DecodeApp.cpp for the implementation of this class
//

class CDecodeAppApp : public CWinApp
{
public:
	CDecodeAppApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecodeAppApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDecodeAppApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECODEAPP_H__B651B39B_1A18_11D1_BC01_00A024D29804__INCLUDED_)
