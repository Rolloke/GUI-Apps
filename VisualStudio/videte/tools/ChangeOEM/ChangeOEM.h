// ChangeOEM.h : main header file for the CHANGEOEM application
//
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ChangeOEM.h $
// ARCHIVE:		$Archive: /Project/Tools/ChangeOEM/ChangeOEM.h $
// CHECKIN:		$Date: 10.05.99 11:19 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 10.05.99 11:19 $
// BY AUTHOR:	$Author: Tomas $
// $Nokeywords:$
#if !defined(AFX_CHANGEOEM_H__0BFB6915_012F_11D3_895D_004005A1D890__INCLUDED_)
#define AFX_CHANGEOEM_H__0BFB6915_012F_11D3_895D_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CChangeOEMApp:
// See ChangeOEM.cpp for the implementation of this class
//

class CChangeOEMApp : public CWinApp
{
public:
	CChangeOEMApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeOEMApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CChangeOEMApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEOEM_H__0BFB6915_012F_11D3_895D_004005A1D890__INCLUDED_)
