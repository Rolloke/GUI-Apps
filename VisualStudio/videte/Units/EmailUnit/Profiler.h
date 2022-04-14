// Profiler.h: interface for the CProfiler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROFILER_H__D2110B13_1B58_11D4_AA05_004005A26A3B__INCLUDED_)
#define AFX_PROFILER_H__D2110B13_1B58_11D4_AA05_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProfiler  
{

public:

	BOOL GetPOPAuth();
	UINT GetPOPPort();
	CString GetPOPUser();
	CString GetPOPServer();
	CString GetPassword();
	CString GetUser();
	CString GetDFUE();
	BOOL ConnectionIsLAN();
	int GetPort();
	CString GetServer();
	CString GetEmail();
	CProfiler();
	virtual ~CProfiler();
	CString GetPOPPassword();

private:

	CString m_sPassword;
	CString m_sUser;
	BOOL m_bLAN;
	CString m_sDFUE;
	int m_iPort;
	CString m_sServer;
	CString m_sEmail;
	CWK_Profile cwk_Profile;
};

#endif // !defined(AFX_PROFILER_H__D2110B13_1B58_11D4_AA05_004005A26A3B__INCLUDED_)
