// DiskTool.h : main header file for the DISKTOOL application
//

#if !defined(AFX_DISKTOOL_H__A99B1297_0521_4016_A4E2_A3832A739BD9__INCLUDED_)
#define AFX_DISKTOOL_H__A99B1297_0521_4016_A4E2_A3832A739BD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDiskToolApp:
// See DiskTool.cpp for the implementation of this class
//

class CDiskToolApp : public CWinApp
{
public:
	CDiskToolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiskToolApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDiskToolApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISKTOOL_H__A99B1297_0521_4016_A4E2_A3832A739BD9__INCLUDED_)
