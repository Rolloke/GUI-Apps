// EmailUnit.h : main header file for the EMAILUNIT application
//

#if !defined(AFX_EMAILUNIT_H__662471B5_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
#define AFX_EMAILUNIT_H__662471B5_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "EmailClient.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CEmailUnitApp:
// See EmailUnit.cpp for the implementation of this class
//

class CEmailUnitApp : public CWinApp
{
public:

	CEmailUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmailUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
#if _MFC_VER >= 0x0710
	int SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif

	//{{AFX_MSG(CEmailUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !



	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMAILUNIT_H__662471B5_0EB0_11D4_A9E6_004005A26A3B__INCLUDED_)
