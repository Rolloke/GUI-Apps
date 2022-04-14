// EULA.h : main header file for the EULA application
//

#if !defined(AFX_EULA_H__CD0CAF27_0C8E_11D5_99B0_004005A19028__INCLUDED_)
#define AFX_EULA_H__CD0CAF27_0C8E_11D5_99B0_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEULAApp:
// See EULA.cpp for the implementation of this class
//

class CEULAApp : public CWinApp
{
public:
	CEULAApp();
	BOOL	IsDTS();
	CString GetEulaRTF();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEULAApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEULAApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL	m_bIsDTS;
	CString m_sEulaRTF;

};

extern CEULAApp theApp;
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EULA_H__CD0CAF27_0C8E_11D5_99B0_004005A19028__INCLUDED_)
