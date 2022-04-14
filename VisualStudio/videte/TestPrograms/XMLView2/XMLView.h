// XMLView.h : main header file for the XMLVIEW application
//

#if !defined(AFX_XMLVIEW_H__9D7E0965_6BFB_4D2D_880D_B0096F19AEE4__INCLUDED_)
#define AFX_XMLVIEW_H__9D7E0965_6BFB_4D2D_880D_B0096F19AEE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CXMLViewApp:
// See XMLView.cpp for the implementation of this class
//

class CXMLViewApp : public CWinApp
{
public:
	CXMLViewApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXMLViewApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CXMLViewApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

extern CXMLViewApp theApp;

#endif // !defined(AFX_XMLVIEW_H__9D7E0965_6BFB_4D2D_880D_B0096F19AEE4__INCLUDED_)
