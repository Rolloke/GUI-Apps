// TickCount.h : main header file for the TICKCOUNT application
//

#if !defined(AFX_TICKCOUNT_H__53A48375_A8BE_11D4_9942_004005A19028__INCLUDED_)
#define AFX_TICKCOUNT_H__53A48375_A8BE_11D4_9942_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTickCountApp:
// See TickCount.cpp for the implementation of this class
//

class CTickCountApp : public CWinApp
{
public:
	CTickCountApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTickCountApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTickCountApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TICKCOUNT_H__53A48375_A8BE_11D4_9942_004005A19028__INCLUDED_)
