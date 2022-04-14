// NetUnit.h : main header file for the NETUNIT application
//

#if !defined(AFX_NETUNIT_H__210DC3AD_2FB7_49D4_929C_AF728A0B2257__INCLUDED_)
#define AFX_NETUNIT_H__210DC3AD_2FB7_49D4_929C_AF728A0B2257__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNetUnitApp:
// See NetUnit.cpp for the implementation of this class
//

class CNetUnitApp : public CWinApp
{
public:
	CNetUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetUnitApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNetUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETUNIT_H__210DC3AD_2FB7_49D4_929C_AF728A0B2257__INCLUDED_)
