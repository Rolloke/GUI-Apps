// AudioTest.h : main header file for the AUDIOTEST application
//

#if !defined(AFX_AUDIOTEST_H__AD809A44_3483_4D00_A368_922C2C8FDA66__INCLUDED_)
#define AFX_AUDIOTEST_H__AD809A44_3483_4D00_A368_922C2C8FDA66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAudioTestApp:
// See AudioTest.cpp for the implementation of this class
//

class CAudioTestApp : public CWinApp
{
public:
	CAudioTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAudioTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOTEST_H__AD809A44_3483_4D00_A368_922C2C8FDA66__INCLUDED_)
