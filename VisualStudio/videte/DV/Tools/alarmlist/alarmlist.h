// alarmlist.h : main header file for the ALARMLIST application
//

#if !defined(AFX_ALARMLIST_H__B0D14215_49F3_11D5_99F0_004005A19028__INCLUDED_)
#define AFX_ALARMLIST_H__B0D14215_49F3_11D5_99F0_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAlarmlistApp:
// See alarmlist.cpp for the implementation of this class
//

class CAlarmlistApp : public CWinApp
{
public:
	CAlarmlistApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmlistApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAlarmlistApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMLIST_H__B0D14215_49F3_11D5_99F0_004005A19028__INCLUDED_)
