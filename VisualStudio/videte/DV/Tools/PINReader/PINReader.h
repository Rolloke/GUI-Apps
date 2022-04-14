// PINReader.h : main header file for the PINREADER application
//

#if !defined(AFX_PINREADER_H__4D7DB631_ADED_4EC2_8F64_D2B1CA04582F__INCLUDED_)
#define AFX_PINREADER_H__4D7DB631_ADED_4EC2_8F64_D2B1CA04582F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPINReaderApp:
// See PINReader.cpp for the implementation of this class
//

class CPINReaderApp : public CWinApp
{
public:
	CPINReaderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPINReaderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPINReaderApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PINREADER_H__4D7DB631_ADED_4EC2_8F64_D2B1CA04582F__INCLUDED_)
