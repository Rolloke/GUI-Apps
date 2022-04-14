// SMSUnit.h : main header file for the SMSUNIT application
//

#if !defined(AFX_SMSUNIT_H__60D2705A_9E3A_11D2_B54A_004005A19028__INCLUDED_)
#define AFX_SMSUNIT_H__60D2705A_9E3A_11D2_B54A_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CIPCServerControlSMS;

/////////////////////////////////////////////////////////////////////////////
// CSMSUnitApp:
// See SMSUnit.cpp for the implementation of this class
//

class CSMSUnitApp : public CWinApp
{
public:
	CSMSUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMSUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
#if _MFC_VER >= 0x0710
	int SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif

public:
	//{{AFX_MSG(CSMSUnitApp)
	afx_msg void OnFileReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// implementation
private:
	void OnConnectionRecord();
	void RegisterWindowClass();
	void LoadSettings();

public:
	CConnectionRecords		m_aConnectionRecords;
	CString	m_sD1;
	CString	m_sD2;
	CString	m_sEPlus;
	CString m_sPrefix;
	CString m_sOwnNumber;
	CIPCServerControlSMS*	m_pCIPCServerControlSMS;

private:
	BOOL	m_bResetting;
};

extern CSMSUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMSUNIT_H__60D2705A_9E3A_11D2_B54A_004005A19028__INCLUDED_)
