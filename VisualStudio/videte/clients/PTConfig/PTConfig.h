// PTConfig.h : main header file for the PTCONFIG application
//

#if !defined(AFX_PTCONFIG_H__9297D97D_3517_11D1_B8C8_0060977A76F1__INCLUDED_)
#define AFX_PTCONFIG_H__9297D97D_3517_11D1_B8C8_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPTConfigApp:
// See PTConfig.cpp for the implementation of this class
//

class CPTConfigApp : public CWinApp
{
public:
	CPTConfigApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPTConfigApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual BOOL OnIdle(LONG lCount);
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPTConfigApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PTCONFIG_H__9297D97D_3517_11D1_B8C8_0060977A76F1__INCLUDED_)
