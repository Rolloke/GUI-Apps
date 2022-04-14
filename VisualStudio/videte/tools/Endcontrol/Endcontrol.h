// Endcontrol.h : main header file for the ENDCONTROL application
//

#if !defined(AFX_ENDCONTROL_H__08D44055_0786_11D3_8962_004005A1D890__INCLUDED_)
#define AFX_ENDCONTROL_H__08D44055_0786_11D3_8962_004005A1D890__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEndcontrolApp:
// See Endcontrol.cpp for the implementation of this class
//

class CEndcontrolApp : public CWinApp
{
public:
	BOOL m_bIamTheClone;
	CEndcontrolApp();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEndcontrolApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEndcontrolApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENDCONTROL_H__08D44055_0786_11D3_8962_004005A1D890__INCLUDED_)
