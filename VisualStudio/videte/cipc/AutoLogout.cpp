// AutoLogout.cpp: implementation of the CAutoLogout class.
//
//////////////////////////////////////////////////////////////////////

#include "stdcipc.h"
#include "AutoLogout.h"

static TCHAR BASED_CODE szSection[] = _T("AutoLogout");
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoLogout::CAutoLogout()
{
	m_bAutoApplication = TRUE;
	m_bAutoConnection = TRUE;
	m_dwApplicationLogoutTime = 10;
	m_dwApplicationConfirmTime = 30;
	m_dwConnectionLogoutTime = 5;
	m_dwConnectionConfirmTime = 30;
	m_dwLastApplication = GetTickCount();
	m_dwLastConnection = GetTickCount();
}
//////////////////////////////////////////////////////////////////////
CAutoLogout::~CAutoLogout()
{

}
//////////////////////////////////////////////////////////////////////
void CAutoLogout::SetValues(	BOOL	bAutoApplication,
								BOOL	bAutoConnection,
								DWORD   dwApplicationLogoutTime,
								DWORD	dwApplicationConfirmTime,
								DWORD   dwConnectionLogoutTime,
								DWORD	dwConnectionConfirmTime)
{
	m_bAutoApplication = bAutoApplication;
	m_bAutoConnection = bAutoConnection;
	m_dwApplicationLogoutTime = dwApplicationLogoutTime;
	m_dwApplicationConfirmTime = dwApplicationConfirmTime;
	m_dwConnectionLogoutTime = dwConnectionLogoutTime;
	m_dwConnectionConfirmTime = dwConnectionConfirmTime;
}
//////////////////////////////////////////////////////////////////////
void CAutoLogout::Load(CWK_Profile& wkProfile)
{
	m_bAutoApplication = wkProfile.GetInt(szSection,_T("AutoApplication"),TRUE);
	m_bAutoConnection = wkProfile.GetInt(szSection,_T("AutoConnection"),TRUE);
	if (m_bAutoApplication)
	{
		m_dwApplicationLogoutTime = wkProfile.GetInt(szSection,_T("ApplicationLogoutTime"),10);
		m_dwApplicationConfirmTime = wkProfile.GetInt(szSection,_T("ApplicationConfirmTime"),30);
	}
	if (m_bAutoConnection)
	{
		m_dwConnectionLogoutTime = wkProfile.GetInt(szSection,_T("ConnectionLogoutTime"),5);
		m_dwConnectionConfirmTime = wkProfile.GetInt(szSection,_T("ConnectionConfirmTime"),30);
	}
	m_dwLastApplication = GetTickCount();
	m_dwLastConnection = GetTickCount();
}
//////////////////////////////////////////////////////////////////////
void CAutoLogout::Save(CWK_Profile& wkProfile)
{
	wkProfile.WriteInt(szSection,_T("AutoApplication"),m_bAutoApplication);
	wkProfile.WriteInt(szSection,_T("AutoConnection"),m_bAutoConnection);
	if (m_bAutoApplication)
	{
		wkProfile.WriteInt(szSection,_T("ApplicationLogoutTime"),m_dwApplicationLogoutTime);
		wkProfile.WriteInt(szSection,_T("ApplicationConfirmTime"),m_dwApplicationConfirmTime);
	}
	if (m_bAutoConnection)
	{
		wkProfile.WriteInt(szSection,_T("ConnectionLogoutTime"),m_dwConnectionLogoutTime);
		wkProfile.WriteInt(szSection,_T("ConnectionConfirmTime"),m_dwConnectionConfirmTime);
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CAutoLogout::IsApplicationOver()
{
	if (m_bAutoApplication)
	{
		DWORD dwT = GetTimeSpan(m_dwLastApplication);
		if (dwT > (m_dwApplicationLogoutTime*60*1000))
		{
			ResetApplication();
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CAutoLogout::IsConnectionOver()
{
	if (m_bAutoConnection)
	{
		DWORD dwT = GetTimeSpan(m_dwLastConnection);
		if (dwT > (m_dwConnectionLogoutTime*60*1000))
		{
			ResetConnection();
			return TRUE;
		}
	}
	return FALSE;
}
