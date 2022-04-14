// LogExtract.h : main header file for the LOGEXTRACT application
//

#if !defined(AFX_LOGEXTRACT_H__AE8AB1F5_0DDB_11D3_BA2D_00400531137E__INCLUDED_)
#define AFX_LOGEXTRACT_H__AE8AB1F5_0DDB_11D3_BA2D_00400531137E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLogExtractApp:
// See LogExtract.cpp for the implementation of this class
//

class CLogExtractApp : public CWinApp
{
public:
	CLogExtractApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogExtractApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLogExtractApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGEXTRACT_H__AE8AB1F5_0DDB_11D3_BA2D_00400531137E__INCLUDED_)
