// Jpegtest.h : main header file for the JPEGTEST application
//

#if !defined(AFX_JPEGTEST_H__B3389335_4A42_11D3_8D9F_004005A11E32__INCLUDED_)
#define AFX_JPEGTEST_H__B3389335_4A42_11D3_8D9F_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CJpegtestApp:
// See Jpegtest.cpp for the implementation of this class
//

class CJpegtestApp : public CWinApp
{
public:
	CJpegtestApp();
	virtual ~CJpegtestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJpegtestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CJpegtestApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CJpegtestApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPEGTEST_H__B3389335_4A42_11D3_8D9F_004005A11E32__INCLUDED_)
