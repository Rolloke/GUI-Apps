// ReserveMem.h : main header file for the RESERVEMEM application
//

#if !defined(AFX_RESERVEMEM_H__C6CB8C23_9081_4CB0_B819_0EAE51E01C24__INCLUDED_)
#define AFX_RESERVEMEM_H__C6CB8C23_9081_4CB0_B819_0EAE51E01C24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CReserveMemApp:
// See ReserveMem.cpp for the implementation of this class
//

class CReserveMemApp : public CWinApp
{
public:
	CReserveMemApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReserveMemApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CReserveMemApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESERVEMEM_H__C6CB8C23_9081_4CB0_B819_0EAE51E01C24__INCLUDED_)
