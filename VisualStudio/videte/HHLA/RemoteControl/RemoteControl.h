// RemoteControl.h : main header file for the REMOTECONTROL application
//

#if !defined(AFX_REMOTECONTROL_H__57580DB5_8B70_11D2_8CA3_004005A11E32__INCLUDED_)
#define AFX_REMOTECONTROL_H__57580DB5_8B70_11D2_8CA3_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "wk_trace.h"

/////////////////////////////////////////////////////////////////////////////
// CRemoteControlApp:
// See RemoteControl.cpp for the implementation of this class
//

class CWK_Dongle;
class CRemoteControlApp : public CWinApp
{
public:
	CRemoteControlApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteControlApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRemoteControlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	WK_ApplicationId		m_AppID;	
	CWK_Dongle*				m_pDongle;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTECONTROL_H__57580DB5_8B70_11D2_8CA3_004005A11E32__INCLUDED_)
