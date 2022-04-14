// NetUnit.h : main header file for the NETUNIT application
//

     

#if !defined(AFX_NETUNIT_H__ABCDD07A_5A48_11D5_8700_004005A26A3B__INCLUDED_)
#define AFX_NETUNIT_H__ABCDD07A_5A48_11D5_8700_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "NetUnitDlg.h"		// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CNetUnitApp:
// See NetUnit.cpp for the implementation of this class
//

 
class CNetUnitApp : public CWinApp
{
//Konstruktor / Destruktor/////////////////////////////////////////////////////////////////////////
public:
	CNetUnitApp();

//Methoden/////////////////////////////////////////////////////////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
#if _MFC_VER >= 0x0710
	int SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif

	//{{AFX_MSG(CNetUnitApp)
	afx_msg void OnAppExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
extern CNetUnitApp theApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETUNIT_H__ABCDD07A_5A48_11D5_8700_004005A26A3B__INCLUDED_)




