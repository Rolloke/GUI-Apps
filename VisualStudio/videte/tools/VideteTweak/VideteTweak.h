// VideteTweak.h : main header file for the VIDETETWEAK application
//

#if !defined(AFX_VIDETETWEAK_H__54FF0D9C_2D19_4687_B59D_7926188AA712__INCLUDED_)
#define AFX_VIDETETWEAK_H__54FF0D9C_2D19_4687_B59D_7926188AA712__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVideteTweakApp:
// See VideteTweak.cpp for the implementation of this class
//
#define REG_CLOSE_KEY(hKey) {if (hKey) {RegCloseKey(hKey);hKey = NULL;}}

class CVideteTweakApp : public CWinApp
{
public:
	CVideteTweakApp();

public:
	HKEY RegOpenKey(HKEY hKey, LPCTSTR sSection, REGSAM rsDesired);
	CString RegReadString(HKEY hKey, LPCTSTR sValue, LPCTSTR sDefault);
	int RegReadInt(HKEY hKey, LPCTSTR sValue, int nDefault);
	BOOL RegWriteString(HKEY hKey, LPCTSTR sName, LPCTSTR sValue);
	BOOL RegWriteInt(HKEY hKey, LPCTSTR sName, int nValue);
	BOOL RegWriteStringInt(HKEY hKey, LPCTSTR sName, int nValue);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideteTweakApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVideteTweakApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
extern CVideteTweakApp theApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDETETWEAK_H__54FF0D9C_2D19_4687_B59D_7926188AA712__INCLUDED_)
