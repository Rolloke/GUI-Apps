// DVSoftAlarm.h : main header file for the DVSOFTALARM application
//

#if !defined(AFX_DVSOFTALARM_H__EFE2E08B_584E_45D2_8E7D_64DA9AE2A845__INCLUDED_)
#define AFX_DVSOFTALARM_H__EFE2E08B_584E_45D2_8E7D_64DA9AE2A845__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDVSoftAlarmApp:
// See DVSoftAlarm.cpp for the implementation of this class
//

class CDVSoftAlarmApp : public CWinApp
{
public:
	CDVSoftAlarmApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVSoftAlarmApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDVSoftAlarmApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVSOFTALARM_H__EFE2E08B_584E_45D2_8E7D_64DA9AE2A845__INCLUDED_)
