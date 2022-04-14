// DVHook.h : main header file for the DVHOOK application
//

#if !defined(AFX_DVHOOK_H__43389135_6C29_11D4_8F46_004005A11E32__INCLUDED_)
#define AFX_DVHOOK_H__43389135_6C29_11D4_8F46_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDVHookApp:
// See DVHook.cpp for the implementation of this class
//

class CDVHookApp : public CWinApp
{
public:
	CDVHookApp();
	virtual	~CDVHookApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVHookApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDVHookApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void GetLanguageDll();
	CString InitHookValue(CWK_Profile &prof, LPCTSTR strProg, LPCTSTR strCommand, bool bForce=false);
protected:
	bool PrepareHook();
	CString m_sHomePath;
	CString m_sLanguage;
	HINSTANCE m_hLangDll;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVHOOK_H__43389135_6C29_11D4_8F46_004005A11E32__INCLUDED_)
