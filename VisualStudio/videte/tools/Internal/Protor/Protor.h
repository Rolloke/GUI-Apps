// Protor.h : main header file for the PROTOR application
//

#if !defined(AFX_PROTOR_H__12EA3EA6_7270_11D1_B8CD_0060977A76F1__INCLUDED_)
#define AFX_PROTOR_H__12EA3EA6_7270_11D1_B8CD_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

// Protor string defines
#define PSD_PROTOR_MARK		"#Protor#"
#define PSD_STANDARD_MARK	"#Std#"

/////////////////////////////////////////////////////////////////////////////
// CProtorApp:
// See Protor.cpp for the implementation of this class
//

class CProtorApp : public CWinApp
{
public:
	CProtorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProtorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CProtorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROTOR_H__12EA3EA6_7270_11D1_B8CD_0060977A76F1__INCLUDED_)
