// CDStart.h : main header file for the CDSTART application
//

#if !defined(AFX_CDSTART_H__F7D70E97_DA30_11D3_BAB3_00400531137E__INCLUDED_)
#define AFX_CDSTART_H__F7D70E97_DA30_11D3_BAB3_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

////////////////////////////////////////////////////////////////////////////
static const TCHAR szAppName[]	= _T("CDStart");

////////////////////////////////////////////////////////////////////////////
#define	WKM_INITDLG_READY	WM_USER +0x0001

/////////////////////////////////////////////////////////////////////////////
// CCDStartApp:
// See CDStart.cpp for the implementation of this class
//
class CCDStartApp : public CWinApp
{
public:
	CCDStartApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCDStartApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	void RegisterWindowClass();

	//{{AFX_MSG(CCDStartApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDSTART_H__F7D70E97_DA30_11D3_BAB3_00400531137E__INCLUDED_)
