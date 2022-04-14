// TimerEditor.h : main header file for the TIMEREDITOR application
//

#if !defined(AFX_TIMEREDITOR_H__57CDD638_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
#define AFX_TIMEREDITOR_H__57CDD638_260F_11D2_B8FB_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorApp:
// See TimerEditor.cpp for the implementation of this class
//

class CTimerEditorApp : public CWinApp
{
public:
	CTimerEditorApp();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerEditorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTimerEditorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEREDITOR_H__57CDD638_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
