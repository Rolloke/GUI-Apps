// SoftDongle.h : main header file for the SOFTDONGLE application
//

#if !defined(AFX_SOFTDONGLE_H__85E32CB5_02DD_11D3_8D21_004005A11E32__INCLUDED_)
#define AFX_SOFTDONGLE_H__85E32CB5_02DD_11D3_8D21_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSoftDongleApp:
// See SoftDongle.cpp for the implementation of this class
//

class CWK_Dongle;
class CSoftDongleApp : public CWinApp
{
public:
	CSoftDongleApp();
	~CSoftDongleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoftDongleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSoftDongleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CWK_Dongle* m_pDongle;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOFTDONGLE_H__85E32CB5_02DD_11D3_8D21_004005A11E32__INCLUDED_)
