// RemoteCD.h : main header file for the REMOTECD application
//

#if !defined(AFX_REMOTECD_H__56B74558_4CC6_11D4_AD95_004005A1D890__INCLUDED_)
#define AFX_REMOTECD_H__56B74558_4CC6_11D4_AD95_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRemoteCDApp:
// See RemoteCD.cpp for the implementation of this class
//

class CRemoteCDApp : public CWinApp
{
public:
	CRemoteCDApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteCDApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRemoteCDApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTECD_H__56B74558_4CC6_11D4_AD95_004005A1D890__INCLUDED_)
