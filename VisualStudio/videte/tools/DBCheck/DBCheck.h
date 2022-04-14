// DBCheck.h : main header file for the DBCHECK application
//

#if !defined(AFX_DBCHECK_H__044CFDA7_3F7A_4E28_9B6E_333AE9DBD50F__INCLUDED_)
#define AFX_DBCHECK_H__044CFDA7_3F7A_4E28_9B6E_333AE9DBD50F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDBCheckApp:
// See DBCheck.cpp for the implementation of this class
//
#include "QueryParameter.h"
#include "DBCheckDoc.h"

class CIPCServerControlDBCheck;

class CDBCheckApp : public CWinApp
{
public:
	CDBCheckApp();
	virtual ~CDBCheckApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBCheckApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Attributes
public:
 	CHostArray &GetHosts();
	WORD        GetNewServerID();
	inline CQueryParameter& GetQueryParameter();

public:
	bool IsLocked();
	CServer* GetServerByIndex(int);
//   CAutoLogout m_AutoLogout;
//	BOOL		m_bRunAnyLinkUnit;
//	CString		m_sSyncTime;

//	CWK_Dongle::OemCode m_OemCode;
//	BOOL		 m_bIsDemo;
//	BOOL		 m_bAllowMiCo;
//	BOOL		 m_bAllowCoCo;
//	BOOL		 m_bAllowPT;
//	BOOL		 m_bAllowBackup;
//	BOOL		 m_bReadOnlyModus;
//	BOOL		 m_bBackupModus;
//	BOOL		 m_bSearchMaskAtStart;
	CUser*		 m_pUser;
	CPermission* m_pPermission;
	CIPCServerControlDBCheck * m_pCIPCServerControlDBCheck;

	CErrorMessages m_ErrorMessages;

protected:
   BOOL LoginAsSuperVisor();
	BOOL Login();

private:
 	CHostArray        m_Hosts;
	CQueryParameter   m_QueryParameter;
   int               m_nIDs;

// Implementation
	//{{AFX_MSG(CDBCheckApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CDBCheckApp theApp;


/////////////////////////////////////////////////////////////////////////////
inline CQueryParameter& CDBCheckApp::GetQueryParameter()
{
	return m_QueryParameter;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBCHECK_H__044CFDA7_3F7A_4E28_9B6E_333AE9DBD50F__INCLUDED_)
