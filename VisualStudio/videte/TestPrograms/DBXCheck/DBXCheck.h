/* GlobalReplace: pReportBox --> g_pReportBox */
// DBXCheck.h : main header file for the DBXCHECK application
//

#if !defined(AFX_DBXCHECK_H__1DF3A427_4238_11D2_B929_00C095ECA33E__INCLUDED_)
#define AFX_DBXCHECK_H__1DF3A427_4238_11D2_B929_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDBXCheckApp:
// See DBXCheck.cpp for the implementation of this class
//

extern CListBox *g_pReportBox;

void __cdecl MY_WK_TRACE(LPCSTR lpszFormat, ...);

class CDBXCheckApp : public CWinApp
{
public:
	CDBXCheckApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBXCheckApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDBXCheckApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	private:
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBXCHECK_H__1DF3A427_4238_11D2_B929_00C095ECA33E__INCLUDED_)
