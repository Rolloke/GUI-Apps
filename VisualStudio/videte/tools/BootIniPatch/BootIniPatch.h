// BootIniPatch.h : main header file for the BOOTINIPATCH application
//

#if !defined(AFX_BOOTINIPATCH_H__E21C6388_C78A_4EB9_9E9C_3169E80D1264__INCLUDED_)
#define AFX_BOOTINIPATCH_H__E21C6388_C78A_4EB9_9E9C_3169E80D1264__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBootIniPatchApp:
// See BootIniPatch.cpp for the implementation of this class
//

class CBootIniPatchApp : public CWinApp
{
public:
	CBootIniPatchApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBootIniPatchApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBootIniPatchApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOOTINIPATCH_H__E21C6388_C78A_4EB9_9E9C_3169E80D1264__INCLUDED_)
