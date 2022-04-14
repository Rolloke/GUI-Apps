// DCClient.h : main header file for the DCCLIENT application
//

#if !defined(AFX_DCCLIENT_H__A043C751_8985_43B1_A500_BF0CF9964C5E__INCLUDED_)
#define AFX_DCCLIENT_H__A043C751_8985_43B1_A500_BF0CF9964C5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "DCClientDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDCClientApp:
// See DCClient.cpp for the implementation of this class
//

class CDCClientApp : public CWinApp
{
public:
	CDCClientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDCClientApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDCClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//members
public:

	CDCClientDlg*	m_pDlg;
	CDCClientDlg*	GetDCClientDlg();
};


/////////////////////////////////////////////////////////////////////////////
extern CDCClientApp theApp;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DCCLIENT_H__A043C751_8985_43B1_A500_BF0CF9964C5E__INCLUDED_)
