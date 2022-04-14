// ProductVersion.h : main header file for the PRODUCTVERSION application
//

#if !defined(AFX_PRODUCTVERSION_H__207CC326_56E2_11D1_B8CD_0060977A76F1__INCLUDED_)
#define AFX_PRODUCTVERSION_H__207CC326_56E2_11D1_B8CD_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CProductVersionApp:
// See ProductVersion.cpp for the implementation of this class
//

class CProductVersionApp : public CWinApp
{
public:
	CProductVersionApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProductVersionApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CProductVersionApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_sDirForCreate;
	CString m_sFileToRead;
private:
	void ParseArgs(const CStringArray &saParameters);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRODUCTVERSION_H__207CC326_56E2_11D1_B8CD_0060977A76F1__INCLUDED_)
