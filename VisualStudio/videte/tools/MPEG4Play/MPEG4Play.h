// MPEG4Play.h : main header file for the MPEG4PLAY application
//

#if !defined(AFX_MPEG4PLAY_H__B74BB5DB_C3FF_44A1_9963_71DDE930F86B__INCLUDED_)
#define AFX_MPEG4PLAY_H__B74BB5DB_C3FF_44A1_9963_71DDE930F86B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMPEG4PlayApp:
// See MPEG4Play.cpp for the implementation of this class
//

class CMPEG4PlayApp : public CWinApp
{
public:
	CMPEG4PlayApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMPEG4PlayApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMPEG4PlayApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MPEG4PLAY_H__B74BB5DB_C3FF_44A1_9963_71DDE930F86B__INCLUDED_)
