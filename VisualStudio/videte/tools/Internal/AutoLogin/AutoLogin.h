/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AutoLogin
// FILE:		$Workfile: AutoLogin.h $
// ARCHIVE:		$Archive: /Project/Tools/Internal/AutoLogin/AutoLogin.h $
// CHECKIN:		$Date: 10.08.98 13:10 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 10.08.98 13:10 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOLOGIN_H__9B505286_3038_11D2_BC01_00A024D29804__INCLUDED_)
#define AFX_AUTOLOGIN_H__9B505286_3038_11D2_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAutoLoginApp:
// See AutoLogin.cpp for the implementation of this class
//

class CAutoLoginApp : public CWinApp
{
public:
	CAutoLoginApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoLoginApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAutoLoginApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOLOGIN_H__9B505286_3038_11D2_BC01_00A024D29804__INCLUDED_)
