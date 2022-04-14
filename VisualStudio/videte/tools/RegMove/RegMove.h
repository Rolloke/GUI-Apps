// RegMove.h : main header file for the REGMOVE application
//

#if !defined(AFX_REGMOVE_H__FC9AA3B7_E599_11D1_B512_00C095EC9EFA__INCLUDED_)
#define AFX_REGMOVE_H__FC9AA3B7_E599_11D1_B512_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CRegMoveApp:
// See RegMove.cpp for the implementation of this class
//

class CRegMoveApp : public CWinApp
{
public:
	CRegMoveApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegMoveApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
private:
	BOOL	DeleteAllSubKey(HKEY hSecurityKey, LPCSTR lpszSection);
	void	MoveRegistry( HKEY hSourceKey, HKEY hDestKey);
	HKEY	GetWKKey();
	HKEY	GetDVRTKey();
	void	ExportRegistry( HKEY hKey, LPCTSTR pszRegPath , CFile& file);
	CString DuplicateBackSlash(CString s);

	void	ExportRegistryToFile(HKEY hKey, const CString &sDestKey, const CString& sFileName);
	void	MoveWK2DVRT();
	void	ExportWK2DVRT(const CString& sFileName);
	void	ExportWK(const CString& sFileName);
	void	ExportDVRT(const CString& sFileName);
	void	ImportReg();
	BOOL	DeleteSettings(const CString& sPathName);
	void	ImportSettings(const CString& sPathName);
	void	WriteProtocol(const CString& sSection, const CString sFileName);

	//{{AFX_MSG(CRegMoveApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGMOVE_H__FC9AA3B7_E599_11D1_B512_00C095EC9EFA__INCLUDED_)
