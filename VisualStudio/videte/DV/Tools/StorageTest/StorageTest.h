// StorageTest.h : main header file for the STORAGETEST application
//

#if !defined(AFX_STORAGETEST_H__4F3D4F74_DEC6_4B07_B1D3_29021134A545__INCLUDED_)
#define AFX_STORAGETEST_H__4F3D4F74_DEC6_4B07_B1D3_29021134A545__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CStorageTestApp:
// See StorageTest.cpp for the implementation of this class
//

class CStorageTestApp : public CWinApp
{
public:
	CStorageTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStorageTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CStorageTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STORAGETEST_H__4F3D4F74_DEC6_4B07_B1D3_29021134A545__INCLUDED_)
