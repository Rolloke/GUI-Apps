// PTZTest.h : main header file for the PTZTEST application
//

#if !defined(AFX_PTZTEST_H__7E7AB982_84EE_4C1B_BF44_53F23FB9BAB3__INCLUDED_)
#define AFX_PTZTEST_H__7E7AB982_84EE_4C1B_BF44_53F23FB9BAB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
#define RESPONSE_IGNORE	0
#define RESPONSE_ACK	1
#define RESPONSE_NAK	2

/////////////////////////////////////////////////////////////////////////////
class CPTZTestApp : public CWinApp
{
public:
	CPTZTestApp();

// Access
	int	GetResponse();

// Operation
	int	SetResponse(int iState);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPTZTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPTZTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CCriticalSection	m_csAck;
	int					m_iResponse;
};


/////////////////////////////////////////////////////////////////////////////
extern CPTZTestApp theApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PTZTEST_H__7E7AB982_84EE_4C1B_BF44_53F23FB9BAB3__INCLUDED_)
