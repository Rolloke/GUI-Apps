// Profiler.cpp: implementation of the CProfiler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "emailunit.h"
#include "Profiler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static TCHAR BASED_CODE szModules[]   = _T("SecurityLauncher\\Modules");
static TCHAR BASED_CODE szSection[]   = _T("EmailUnit");

CProfiler::CProfiler()
{

}

CProfiler::~CProfiler()
{

}

CString CProfiler::GetEmail()
{
	return cwk_Profile.GetString(szSection,_T("EMAIL"),_T(""));
}

CString CProfiler::GetServer()
{
	return cwk_Profile.GetString(szSection,_T("SMTP"),_T(""));
}

int CProfiler::GetPort()
{
	return cwk_Profile.GetInt(szSection,_T("PORT"),0);
}

BOOL CProfiler::ConnectionIsLAN()
{
	int iLAN = cwk_Profile.GetInt(szSection,_T("CONTYPEDFUE"),0);

	return !iLAN;
}

CString CProfiler::GetDFUE()
{
	return 	cwk_Profile.GetString(szSection,_T("DFUE"),m_sDFUE);
}

CString CProfiler::GetUser()
{
	m_sUser=cwk_Profile.GetString(szSection,_T("USER"),m_sUser);
	return m_sUser;
}

CString CProfiler::GetPassword()
{
	m_sPassword = cwk_Profile.GetString(szSection,_T("PASSWORD"),m_sPassword);
	cwk_Profile.Decode(m_sPassword);
	return m_sPassword;

}

CString CProfiler::GetPOPServer()
{
  	return 	cwk_Profile.GetString(szSection,_T("POPSERVER"),_T(""));
}

CString CProfiler::GetPOPUser()
{
	return 	cwk_Profile.GetString(szSection,_T("POPUSER"),_T(""));

}

CString CProfiler::GetPOPPassword()
{
	m_sPassword = cwk_Profile.GetString(szSection,_T("POPPASSWORD"),_T(""));
	cwk_Profile.Decode(m_sPassword);
	return m_sPassword;
}

UINT CProfiler::GetPOPPort()
{
	return cwk_Profile.GetInt(szSection,_T("POPPORT"),0);
}

BOOL CProfiler::GetPOPAuth()
{
   return cwk_Profile.GetInt(szSection,_T("POPAuth"),0);
}
