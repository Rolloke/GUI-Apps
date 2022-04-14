// CompareIt.h : main header file for the COMPAREIT application
//

#if !defined(AFX_COMPAREIT_H__9F09D446_886D_11D2_86F1_004005517E7A__INCLUDED_)
#define AFX_COMPAREIT_H__9F09D446_886D_11D2_86F1_004005517E7A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCompareItApp:
// See CompareIt.cpp for the implementation of this class
//

class CCompareItApp : public CWinApp
{
public:
	CCompareItApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompareItApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCompareItApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPAREIT_H__9F09D446_886D_11D2_86F1_004005517E7A__INCLUDED_)
