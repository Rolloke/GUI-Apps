// ProductView.h : main header file for the PRODUCTVIEW application
//

#if !defined(AFX_PRODUCTVIEW_H__3688BAF5_1955_11D1_B8C8_0060977A76F1__INCLUDED_)
#define AFX_PRODUCTVIEW_H__3688BAF5_1955_11D1_B8C8_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CProductViewApp:
// See ProductView.cpp for the implementation of this class
//

class CProductViewApp : public CWinApp
{
public:
	CProductViewApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProductViewApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CProductViewApp)
	afx_msg void OnHiddenWrite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	inline BOOL IsSaveEnabled() const;
	inline BOOL IsCryptEnabled() const;
	inline BOOL IsShowEnabled() const;
	inline const CString &GetFileToUse() const;
private:
	void ParseArgs(const CStringArray &saParameters);
	//
	BOOL m_bSaveEnabled;
	BOOL m_bCryptEnabled;
	BOOL m_bShowEnabled;
	CString m_sFileToUse;
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CProductViewApp::IsSaveEnabled() const
{
	return m_bSaveEnabled;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CProductViewApp::IsCryptEnabled() const
{
	return m_bCryptEnabled;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CProductViewApp::IsShowEnabled() const
{
	return m_bShowEnabled;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString &CProductViewApp::GetFileToUse() const
{
	return m_sFileToUse;
}
/////////////////////////////////////////////////////////////////////////////
extern CProductViewApp theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRODUCTVIEW_H__3688BAF5_1955_11D1_B8C8_0060977A76F1__INCLUDED_)
