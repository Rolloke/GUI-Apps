/* GlobalReplace: @[mM][dD] --> Function: */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: User.h $
// ARCHIVE:		$Archive: /Project/CIPC/User.h $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 45 $
// LAST CHANGE:	$Modtime: 19.01.06 21:25 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _USER_HEADER_H_
#define _USER_HEADER_H_

#include "SecID.h"
#include "wkclasses\wk_profile.h"
#include "SystemTime.h"

static const TCHAR szIdipServiceUser[]	= _T("idipservice");
static const TCHAR szUserAlarm[]		= _T("Alarm");

/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CUser 
{
	// construction
public:
	CUser();
	CUser(const CUser& user);
	virtual ~CUser();

protected:
	CUser(CSecID id);

	// attributes
public:
	inline BOOL IsIdipServiceUser() const;
	inline BOOL IsUserAlarm() const;

	// access
public:
	inline const CString& GetName() const;// Function: returns the user's name
	inline const CString& GetPassword() const;// Function: return's the user's password, not encrypted
	inline CSecID	   GetID() const;// Function: returns the user's unique ID
	// Function: returns the users permission ID, use {CPermissionArray::GetPermission} to
	// retrieve the {CPermission} object belonging to the user*/
	inline CSecID	   GetPermissionID() const;

	inline DWORD	   GetDisconnectMinutes() const;
	inline int		   GetTryCounter() const;
	inline const CString& GetEndOfTime() const;
	inline const CString& GetLastFailedLoginTime() const;
	inline BOOL			GetDualLogin() const;
	inline WORD			GetTimeLimit() const;
	inline int			GetMaxTries() const;
	inline WORD			GetLockTime() const;
	inline const CSystemTime& GetCreationTime() const;

	// operations
public:
	void SetName(const CString& sName);
	void SetPassword(const CString& sPassword, BOOL bForceToChangePassword = FALSE);
	void SetPermissionID(CSecID id);
	void SetDisconnectMinutes(DWORD dwMinutes);
	void LoginFailed();
	void LoginSuccess();
	BOOL IsLocked();
	BOOL IsPasswordOutOfTime();
	BOOL Load(int i,CWK_Profile& wkProfile);
#ifdef _UNICODE
	void Save(int i, CWK_Profile& wkProfile, bool bUseBinaryFormat);
#else
	void Save(int i, CWK_Profile& wkProfile);
#endif
	void SetDualLogin(BOOL bDual);
	
private:
	void SetEndOfTime(BOOL bForceToChangePassword);
	void Init();

	// data
private:
	CSecID		m_ID;
	CString		m_Name;
	CString 	m_Password;
	CSecID		m_PermissionID;
	DWORD		m_MinutesToDisconnect;
	CString		m_sEndOfTime;			// Ablaufdatum des Paßwortes
	CString		m_sLastFailedLoginTime;	// Zeitpunkt des letzten fehlgeschlagenen Logins
	BOOL		m_bDualLogin;			// TRUE -> Ein 2. Login notwendig
	WORD		m_wTimeLimit;			// Gültigkeitszeitraum des Paßwortes
	int			m_nTryCounter;			// Zahl der Login-Fehlversuche 
	int			m_nMaxTries;			// Maximale Zahl der erlaubten Fehlversuche
	WORD		m_wLockTime;			// Sperrdauer in Stunden

	CSystemTime m_CreationTime;
						   
	friend class CUserArray;
};
/////////////////////////////////////////////////////////////////////////////
inline const CString& CUser::GetName() const
{
	return (m_Name);
}
inline const CString& CUser::GetPassword() const
{
	return (m_Password);
}
inline CSecID CUser::GetID() const
{
	return (m_ID);
}
inline CSecID CUser::GetPermissionID() const
{
	return (m_PermissionID);
}
/*Function: returns the minutes for automatic disconnect in idip! Security 3.0 or PictureHandler*/
inline DWORD CUser::GetDisconnectMinutes() const
{
	return (m_MinutesToDisconnect);
}

/////////////////////////////////////////////////////////////////////////////
inline int CUser::GetTryCounter() const
{
	return (m_nTryCounter);
}

/////////////////////////////////////////////////////////////////////////////
inline const CString& CUser::GetEndOfTime() const
{
	return (m_sEndOfTime);
}

/////////////////////////////////////////////////////////////////////////////
inline const CString& CUser::GetLastFailedLoginTime() const
{
	return (m_sLastFailedLoginTime);
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL CUser::GetDualLogin() const
{
	return m_bDualLogin;
}

/////////////////////////////////////////////////////////////////////////////
inline WORD CUser::GetTimeLimit() const
{
	return m_wTimeLimit;
}

/////////////////////////////////////////////////////////////////////////////
inline int	CUser::GetMaxTries() const	    
{
	return m_nMaxTries;
}
/////////////////////////////////////////////////////////////////////////////
inline WORD	CUser::GetLockTime() const
{
	return m_wLockTime;
}
/////////////////////////////////////////////////////////////////////////////
inline const CSystemTime& CUser::GetCreationTime() const
{
	return m_CreationTime;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CUser::IsIdipServiceUser() const
{
	return (   (m_Name == szIdipServiceUser)
			&& (m_PermissionID == SECID_SUPERVISOR_PERMISSION)
			&& (m_ID == SECID_SPECIAL_IDIP_SERVICE_USER)
			);
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CUser::IsUserAlarm() const
{
	return (   (m_Name == szUserAlarm)
			&& (m_PermissionID == SECID_SUPERVISOR_PERMISSION)
			&& (m_Password.GetLength() == 8)
			&& (m_Password.SpanIncluding(_T("01213456789abcdefABCDEF")) == m_Password)
			);
}
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
typedef CUser * CUserPtr;
WK_PTR_ARRAY(CUserArrayPlain, CUserPtr)

// class AFX_EXT_CLASS CUserArray : public CTypedPtrArray<CPtrArray,CUser*>
class AFX_EXT_CLASS CUserArray : public CUserArrayPlain
{
public:
			CUserArray();
	virtual ~CUserArray();

public:
	// operations
	void	Load(CWK_Profile& wkProfile);
	void	Save(CWK_Profile& wkProfile);
	CUser*  AddUser();
	void	DeleteUser(CUser* pUser);
	CUser*  GetUser(CSecID id);
	void	DeleteAll();

	CSecID	IsValid(const CString& sName, const CString& sPassword);
	CSecID  IsValid(const CString& sName);

private:
	void	AddIdipServiceUser();
#ifdef _UNICODE
public:
	BOOL	m_bUseBinaryFormat;
#endif
};
#endif
