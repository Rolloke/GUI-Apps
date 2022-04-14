// ICPCONUnit.h : main header file for the ICPCONUNIT application
//

#if !defined(AFX_ICPCONUNIT_H__FF2F49DC_E2FD_4945_A266_F435D50F9602__INCLUDED_)
#define AFX_ICPCONUNIT_H__FF2F49DC_E2FD_4945_A266_F435D50F9602__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

class CICPCONUnitDlg;

/////////////////////////////////////////////////////////////////////////////
// CICPCONUnitApp:
// See ICPCONUnit.cpp for the implementation of this class
//

class CICPCONUnitApp : public CWinApp
{
public:
	CICPCONUnitApp();
	virtual ~CICPCONUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CICPCONUnitApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
public:
	static LONG ReadRegistryString(HKEY, LPCTSTR, CString &);
#if _MFC_VER >= 0x0710
	int SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif

protected:
	//{{AFX_MSG(CICPCONUnitApp)
	afx_msg void OnAppExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:

	CICPCONUnitDlg*m_pDlg;
public:
	
	void _cdecl OutputDebug(LPCTSTR lpszFormat, ...);

	CString m_sRegistryLocation;
	CString m_sRegistryLocationModules;
	CString m_sRegistryLocationPorts;
	
	BOOL    m_bIsInMenuLoop;
	BOOL	  m_bStandalone;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICPCONUNIT_H__FF2F49DC_E2FD_4945_A266_F435D50F9602__INCLUDED_)
