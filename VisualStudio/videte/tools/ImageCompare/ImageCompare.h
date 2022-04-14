/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ImageCompare
// FILE:		$Workfile: ImageCompare.h $
// ARCHIVE:		$Archive: /Project/Tools/ImageCompare/ImageCompare.h $
// CHECKIN:		$Date: 27.04.99 10:34 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 27.04.99 10:31 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGECOMPARE_H__C2210975_F324_11D2_8D05_004005A11E32__INCLUDED_)
#define AFX_IMAGECOMPARE_H__C2210975_F324_11D2_8D05_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CImageCompareApp:
// See ImageCompare.cpp for the implementation of this class
//

class CImageCompareApp : public CWinApp
{
public:
	CImageCompareApp();
	~CImageCompareApp();

	const CUser* GetUser(){return &m_User;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageCompareApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CImageCompareApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// implementation	
private:
	BOOL Login();
	void RegisterWindowClass();

	// data member
private:
	CUser m_User;

};
/////////////////////////////////////////////////////////////////////////////
extern CImageCompareApp theApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGECOMPARE_H__C2210975_F324_11D2_8D05_004005A11E32__INCLUDED_)
