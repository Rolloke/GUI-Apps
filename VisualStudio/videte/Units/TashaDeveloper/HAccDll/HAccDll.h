/////////////////////////////////////////////////////////////////////////////
// PROJECT:		HAccDll
// FILE:		$Workfile: HAccDll.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha(Developer)/HAccDll/HAccDll.h $
// CHECKIN:		$Date: 5.01.04 9:52 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 5.01.04 9:25 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
	    
#if !defined(AFX_HACCDLL_H__5CFCAAC6_97C1_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_HACCDLL_H__5CFCAAC6_97C1_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHAccApp
// See HAccDll.cpp for the implementation of this class
//

class CHAccApp : public CWinApp
{
public:
	CHAccApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHAccApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CHAccApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HACCDLL_H__5CFCAAC6_97C1_11D1_BC01_00A024D29804__INCLUDED_)
