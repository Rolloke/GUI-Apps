/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaDA
// FILE:		$Workfile: TashaDA.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDA/TashaDA.h $
// CHECKIN:		$Date: 26.03.04 12:13 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 25.03.04 11:08 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
	    
#if !defined(AFX_TASHADA_H__5CFCAAC6_97C1_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_TASHADA_H__5CFCAAC6_97C1_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTashaDAApp
// See TashaDA.cpp for the implementation of this class
//

class CTashaDAApp : public CWinApp
{
public:
	CTashaDAApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTashaDAApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTashaDAApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASHADA_H__5CFCAAC6_97C1_11D1_BC01_00A024D29804__INCLUDED_)
