// VipCleanNT.h : main header file for the VIPCLEANNT application
//

#if !defined(AFX_VIPCLEANNT_H__C08581A7_FFC4_11D3_A9D7_004005A26A3B__INCLUDED_)
#define AFX_VIPCLEANNT_H__C08581A7_FFC4_11D3_A9D7_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVipCleanNTApp:
// See VipCleanNT.cpp for the implementation of this class
//

class CVipCleanNTApp : public CWinApp
{
public:
	CVipCleanNTApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVipCleanNTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVipCleanNTApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
	BOOL IsFactoryReset(); //wenn FALSE, dann kein Factory Reset ausführen sondern
						  //von Hand Einstellungen vornehmen
public:
	BOOL ShellExecute(LPCTSTR sProgram);
	static int ExecuteProgram(LPCTSTR sCmdline, BOOL bWait);


	CString m_sStartDir;
};

extern CVipCleanNTApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIPCLEANNT_H__C08581A7_FFC4_11D3_A9D7_004005A26A3B__INCLUDED_)
