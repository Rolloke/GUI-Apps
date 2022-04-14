// actualimage.h : main header file for the ACTUALIMAGE application
//

#if !defined(AFX_ACTUALIMAGE_H__B9800EC3_F63C_11D2_8951_004005A1D890__INCLUDED_)
#define AFX_ACTUALIMAGE_H__B9800EC3_F63C_11D2_8951_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


/////////////////////////////////////////////////////////////////////////////
// CActualimageApp:
// See actualimage.cpp for the implementation of this class
//

class CActualimageApp : public CWinApp
{
public:
	CActualimageApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActualimageApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL


	// Implementation
protected:
	BOOL RequestAndSaveImages();

	//{{AFX_MSG(CActualimageApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_sPassword;
	CString m_sPermission;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTUALIMAGE_H__B9800EC3_F63C_11D2_8951_004005A1D890__INCLUDED_)
