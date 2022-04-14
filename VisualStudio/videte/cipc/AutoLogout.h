// AutoLogout.h: interface for the CAutoLogout class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOLOGOUT_H__4AED47C2_2554_11D1_93E1_00C095ECA33E__INCLUDED_)
#define AFX_AUTOLOGOUT_H__4AED47C2_2554_11D1_93E1_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class AFX_EXT_CLASS CAutoLogout  
{
	// construction / destruction
public:
	CAutoLogout();
	virtual ~CAutoLogout();

	// attributes
public:
	inline BOOL		GetAutoApplication() const;
	inline BOOL		GetAutoConnection() const;
	inline DWORD	GetApplicationLogoutTime() const;
	inline DWORD	GetApplicationConfirmTime() const;
	inline DWORD	GetConnectionLogoutTime() const;
	inline DWORD	GetConnectionConfirmTime() const;

		   BOOL		IsApplicationOver();
		   BOOL		IsConnectionOver();
	// operations
public:
	inline void	ResetApplication();
	inline void	ResetConnection();

	void SetValues(	BOOL	bAutoApplication,
					BOOL	bAutoConnection,
					DWORD   dwApplicationLogoutTime,
					DWORD	dwApplicationConfirmTime,
					DWORD   dwConnectionLogoutTime,
					DWORD	dwConnectionConfirmTime);
	// serialization
public:
	void Load(CWK_Profile& wkProfile);
	void Save(CWK_Profile& wkProfile);

	// data
private:
	BOOL	m_bAutoApplication;
	BOOL	m_bAutoConnection;
	DWORD   m_dwApplicationLogoutTime;
	DWORD	m_dwApplicationConfirmTime;
	DWORD   m_dwConnectionLogoutTime;
	DWORD	m_dwConnectionConfirmTime;

	DWORD	m_dwLastApplication;
	DWORD	m_dwLastConnection;
};
//////////////////////////////////////////////////////////////////////
inline BOOL	CAutoLogout::GetAutoApplication() const
{
	return m_bAutoApplication;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CAutoLogout::GetAutoConnection() const
{
	return m_bAutoConnection;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CAutoLogout::GetApplicationLogoutTime() const
{
	return m_dwApplicationLogoutTime;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CAutoLogout::GetApplicationConfirmTime() const
{
	return m_dwApplicationConfirmTime;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CAutoLogout::GetConnectionLogoutTime() const
{
	return m_dwConnectionLogoutTime;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CAutoLogout::GetConnectionConfirmTime() const
{
	return m_dwConnectionConfirmTime;
}
//////////////////////////////////////////////////////////////////////
inline void CAutoLogout::ResetApplication()
{
	m_dwLastApplication = GetTickCount();
}
//////////////////////////////////////////////////////////////////////
inline void CAutoLogout::ResetConnection()
{
	m_dwLastConnection = GetTickCount();
}
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_AUTOLOGOUT_H__4AED47C2_2554_11D1_93E1_00C095ECA33E__INCLUDED_)
