// WatchCD.h : main header file for the WATCHCD application
//

#if !defined(AFX_WATCHCD_H__218741A4_E856_11D3_AE43_004005A1D890__INCLUDED_)
#define AFX_WATCHCD_H__218741A4_E856_11D3_AE43_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWatchCDApp:
// See WatchCD.cpp for the implementation of this class
//

class CWatchCDApp : public CWinApp
{
public:
	CWatchCDApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchCDApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWatchCDApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
public:
	void CheckDirectCDVersion();  //liefert die installierte DirectCD Version. 3 oder 5
	int GetDirectCDVersion();

private:

	int m_iDirectCDVersion; //DirectCD Version 
};

extern CWatchCDApp theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHCD_H__218741A4_E856_11D3_AE43_004005A1D890__INCLUDED_)
