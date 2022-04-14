// ICPCONClient.h : main header file for the RELAISSWITCHBOX application
//

#if !defined(AFX_CIPCCONCLIENT_H__8EC51C87_7443_4473_AC73_AA6A91BF3F4C__INCLUDED_)
#define AFX_CIPCCONCLIENT_H__8EC51C87_7443_4473_AC73_AA6A91BF3F4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CIPCCONClient:
// See RelaisSwitchBox.cpp for the implementation of this class
//
class CSwitchPanel;
class CIPCCONClient : public CWinApp
{
public:
	CIPCCONClient();
	virtual ~CIPCCONClient();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPCCONClient)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIPCCONClient)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	public:
	static LONG ReadRegistryString(HKEY, LPCTSTR, CString &);
	CString m_sRegistryLocation;
	CString m_sRegistryLocationPorts;
	BOOL m_bIsInMenuLoop;
	private:
	CSwitchPanel *m_pSwitchPanel;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CIPCCONCLIENT_H__8EC51C87_7443_4473_AC73_AA6A91BF3F4C__INCLUDED_)
