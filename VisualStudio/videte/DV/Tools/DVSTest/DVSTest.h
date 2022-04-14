// DVSTest.h : main header file for the DVSTEST application
//

#if !defined(AFX_DVSTEST_H__C13369D5_8673_11D5_9A29_004005A19028__INCLUDED_)
#define AFX_DVSTEST_H__C13369D5_8673_11D5_9A29_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDVSTestApp:
// See DVSTest.cpp for the implementation of this class
//

class CDVSTestApp : public CWinApp
{
public:
	CDVSTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVSTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDVSTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString m_sStart;
	CString m_sEnd;
	CString m_sPath;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVSTEST_H__C13369D5_8673_11D5_9A29_004005A19028__INCLUDED_)
