// LogCount.h : main header file for the LOGCOUNT application
//

#if !defined(AFX_LOGCOUNT_H__74714517_8DBD_11D2_B04E_004005A1D890__INCLUDED_)
#define AFX_LOGCOUNT_H__74714517_8DBD_11D2_B04E_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLogCountApp:
// See LogCount.cpp for the implementation of this class
//

class CLogCountApp : public CWinApp
{
public:
	CLogCountApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogCountApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLogCountApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGCOUNT_H__74714517_8DBD_11D2_B04E_004005A1D890__INCLUDED_)
