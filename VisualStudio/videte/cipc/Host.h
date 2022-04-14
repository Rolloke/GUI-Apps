/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Host.h $
// ARCHIVE:		$Archive: /Project/CIPC/Host.h $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 47 $
// LAST CHANGE:	$Modtime: 19.01.06 21:26 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _HOST_HEADER_H_
#define _HOST_HEADER_H_

#include "SecID.h"

class CPermission;
//
/////////////////////////////////////////////////////////////////////////////
// 
//Contains hostName (alias comment), number and id.
class AFX_EXT_CLASS CHost 
{
	friend class CHostArray;
	// construction
public:
	//  public
	//!ic! default constructor
	CHost();
	//!ic! constructor from strings
	CHost(LPCTSTR pszName, LPCTSTR pszNumber);
	//!ic! destructor
	virtual ~CHost();

	//  access
	// returns the Host name.
	inline const CString& GetName() const;
	// returns the number (ISDN, TCP/IP ....)
	inline const CString& GetNumber() const;
	// type as string.
	const CString  GetTyp() const;
	// returns the number without the leading type xxx:
	// Only for non routed addresses!.
	const CString  GetClearNumber() const;
	// returns the ID.
	inline CSecID	   GetID() const;
	
	inline CSecID GetAlarmArchive() const;
	inline CSecID GetNormalArchive() const;
	inline CSecID GetSearchArchive() const;

	inline BOOL				IsPINRequired() const;
	inline const CString&	GetPIN() const;
	inline DWORD			GetAutoDisconnectTime() const;

	// types
	BOOL IsISDN() const; // Is Connection ISDN
	BOOL IsTCPIP() const;// Is Connection TCPIP
	BOOL IsDUN() const;// Is Connection DUN
	BOOL IsB3() const;// Is Connection B3
	BOOL IsB6() const;// Is Connection B6
	BOOL IsSMS() const;// Is Connection SMS
	BOOL IsTOBS() const;// Is Connection TOBS
	BOOL IsEMail() const;// Is Connection EMail
	BOOL IsFAX() const;// Is Connection FAX
	BOOL IsRouting() const;// Is Connection Routing

	BOOL IsValid() const;
	BOOL IsRoutingValid() const;

	const CString GetLastNumber() const;
	const CString GetCleanedUpNumber() const;
	static CString GetCleanedUpNumber(const CString& sCheck);


public:
	//!ic!erations
	// sets the name.
	void SetName(const CString& sName);
	// sets the number.
	void SetNumber(const CString& sName);

//!ic!atic functions
	// helper function, which splits a route into strings
	static void SplitRouteFromString(
							const CString &sPath, 
							CStringArray &result
						);

	inline void SetAlarmArchive(CSecID id);
	inline void SetNormalArchive(CSecID id);
	inline void SetSearchArchive(CSecID id);
	inline void SetPINRequired(BOOL bPinRequired);
	inline void SetPIN(const CString &sPIN);
	inline void SetAutoDisconnectTime(DWORD dwAutoDisconnectTime);

protected:
	// from id
	CHost(CSecID id);
	// loads profile
	BOOL Load(int i,CWK_Profile& wkProfile);
	// saves profile
#ifdef _UNICODE
	void Save(int i, CWK_Profile& wkProfile, bool bUseBinaryFormat);
#else
	void Save(int i, CWK_Profile& wkProfile);
#endif
	static BOOL IsNumberValid(const CString& sNumber);
	static BOOL IsISDN(const CString& sNumber);
	static BOOL IsTCPIP(const CString& sNumber);
	static BOOL IsDUN(const CString& sNumber);
	static BOOL IsB3(const CString& sNumber);
	static BOOL IsB6(const CString& sNumber);
	static BOOL IsTOBS(const CString& sNumber);
	static BOOL IsSMS(const CString& sNumber);
	static BOOL IsEMail(const CString& sNumber);
	static BOOL IsFAX(const CString& sNumber);

private:
	void	Init();

	// data
private:
	CSecID		m_ID;
	CString		m_sName;
	CString		m_sNumber;

	CSecID		m_idAlarmArchive;
	CSecID		m_idNormalArchive;
	CSecID		m_idSearchArchive;

	BOOL		m_bPINRequired;
	CString		m_sPIN;
	DWORD		m_dwAutoDisconnectTime;

	// for load/save protected construction
	friend class CHostArray;
};
/////////////////////////////////////////////////////////////////////////////
/*Function: returns the host's name*/
inline const CString& CHost::GetName() const
{
	return (m_sName);
}
/////////////////////////////////////////////////////////////////////////////
/*Function: return's the host's number*/
inline const CString& CHost::GetNumber() const
{
	return (m_sNumber);
}
/////////////////////////////////////////////////////////////////////////////
/*Function: returns the host's unique ID, only local unique*/
inline CSecID CHost::GetID() const
{
	return (m_ID);
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CHost::GetAlarmArchive() const
{
	return m_idAlarmArchive;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CHost::GetNormalArchive() const
{
	return m_idNormalArchive;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CHost::GetSearchArchive() const
{
	return m_idSearchArchive;
}
/////////////////////////////////////////////////////////////////////////////
inline void CHost::SetAlarmArchive(CSecID id)
{
	m_idAlarmArchive = id;
}
/////////////////////////////////////////////////////////////////////////////
inline void CHost::SetNormalArchive(CSecID id)
{
	m_idNormalArchive = id;
}
/////////////////////////////////////////////////////////////////////////////
inline void CHost::SetSearchArchive(CSecID id)
{
	m_idSearchArchive = id;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CHost::IsPINRequired() const
{
	return m_bPINRequired;
}
/////////////////////////////////////////////////////////////////////////////
inline void CHost::SetPINRequired(BOOL bPINRequired)
{
	m_bPINRequired = bPINRequired;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CHost::GetPIN() const
{
	return m_sPIN;
}
/////////////////////////////////////////////////////////////////////////////
inline void CHost::SetPIN(const CString& sPIN)
{
	m_sPIN = sPIN;
}
/////////////////////////////////////////////////////////////////////////////
inline DWORD CHost::GetAutoDisconnectTime() const
{
	return m_dwAutoDisconnectTime;
}
/////////////////////////////////////////////////////////////////////////////
inline void CHost::SetAutoDisconnectTime(DWORD dwAutoDisconnectTime)
{
	m_dwAutoDisconnectTime = dwAutoDisconnectTime;
}

typedef CHost * CHostPtr;
WK_PTR_ARRAY(CHostArrayPlain, CHostPtr)
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
// 
// A pointer array of hosts.
//  <c CHost>
// class AFX_EXT_CLASS CHostArray : public CTypedPtrArray<CPtrArray,CHost*>
class AFX_EXT_CLASS CHostArray : public CHostArrayPlain
{
public:
	CHostArray();
	virtual ~CHostArray();
public:
	//  operations
	//!ic! returns the standard name.
	static LPCTSTR GetLocalHostNameStandard();
	void	Load(CWK_Profile& wkProfile);
	void	Save(CWK_Profile& wkProfile);
	CHost*  AddHost();
	void	DeleteHost(CHost* pHost);
	CHost*  GetHost(const CSecID& id);
	CHost*  GetHostForArchive(const CSecID& id);
	void	Sort();

	//  access
	inline CHost*  GetAtFast(int nPos) const;
	CHost*  GetLocalHost();
	CString GetLocalHostName() const;
	CSecID  GetLocalHostID() const;
	void    SetLocalHostName(CString sName);
	//
	BOOL	CheckHost(const CString &sHost,
					  const CPermission *pPermission) const;
	void	Add(CHost*pHost);
private:
	CHost	m_LocalHost;
#ifdef _UNICODE
public:
	BOOL	m_bUseBinaryFormat;
#endif
};
/////////////////////////////////////////////////////////////////////////////
inline CHost*  CHostArray::GetAtFast(int nPos) const
{
	ASSERT(nPos >= 0 && nPos < m_nSize);
	return (CHost* ) m_pData[nPos];
}
#endif

