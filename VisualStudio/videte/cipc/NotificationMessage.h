// NotificationMessage.h: interface for the CNotificationMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTIFICATIONMESSAGE_H__50B5B754_641F_11D2_B5DB_00C095EC9EFA__INCLUDED_)
#define AFX_NOTIFICATIONMESSAGE_H__50B5B754_641F_11D2_B5DB_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SecID.h"
#include "SystemTime.h"

#define NM_CODEPAGE	 _T("NotificationCodePage")
#define NM_PARAMETER _T("NotificationURL")
#define NM_MESSAGE	 _T("NotificationMessage")

/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CNotificationMessage  
{
	// construction / destruction
public:
	CNotificationMessage();
	virtual ~CNotificationMessage();

	// attributes
public:
	inline		 CSecID	  GetID() const;
	inline const CString& GetName() const;
	inline const CString& GetMessage() const;
	inline const CString& GetURL() const;
	inline const CSystemTime& GetTimeStamp() const;

	// operations
public:
	inline void SetName(const CString& sName);
	inline void SetMessage(const CString& sMessage);
	inline void SetURL(const CString& sURL);
	inline void SetTimeStamp(const CSystemTime& sTimeStamp);

	static CString GetWWWRoot();

	// implementation
protected:
	CNotificationMessage(CSecID id);
	BOOL Load(int i,CWK_Profile& wkProfile);
	void Save(int i,CWK_Profile& wkProfile);

	// data member
private:
	CSecID			m_ID;
	CString			m_sName;
	CString			m_sMessage;
	CString			m_sURL;
	CSystemTime		m_stTimeStamp;

	// for load/save protected construction
	friend class CNotificationMessageArray;
};
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CNotificationMessageArray : public CTypedPtrArray<CPtrArray,CNotificationMessage*>
{
public:
	CNotificationMessageArray();
	virtual ~CNotificationMessageArray();

	// operations
public:
	void	Load(CWK_Profile& wkProfile);
	void	Save(CWK_Profile& wkProfile);
	CNotificationMessage*  AddNotificationMessage();
	void	DeleteNotificationMessage(CNotificationMessage* pNotificationMessage);
	CNotificationMessage*  GetNotificationMessage(const CSecID& id);
	void    DeleteAll();
	inline CNotificationMessage*  GetAtFast(int nPos) const;
};
/////////////////////////////////////////////////////////////////////////////
inline CNotificationMessage*  CNotificationMessageArray::GetAtFast(int nPos) const
{
	ASSERT(nPos >= 0 && nPos < m_nSize);
	return (CNotificationMessage* ) m_pData[nPos];
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CNotificationMessage::GetID() const
{
	return m_ID;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CNotificationMessage::GetName() const
{
	return m_sName;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CNotificationMessage::GetMessage() const
{
	return m_sMessage;
}
/////////////////////////////////////////////////////////////////////////////
inline const CString& CNotificationMessage::GetURL() const
{
	return m_sURL;
}
/////////////////////////////////////////////////////////////////////////////
inline const CSystemTime& CNotificationMessage::GetTimeStamp() const
{
	return m_stTimeStamp;
}
/////////////////////////////////////////////////////////////////////////////
inline void CNotificationMessage::SetName(const CString& sName)
{
	m_sName = sName;
}
/////////////////////////////////////////////////////////////////////////////
inline void CNotificationMessage::SetMessage(const CString& sMessage)
{
	m_sMessage = sMessage;
}
/////////////////////////////////////////////////////////////////////////////
inline void CNotificationMessage::SetURL(const CString& sURL)
{
	m_sURL = sURL;
}
inline void CNotificationMessage::SetTimeStamp(const CSystemTime& sTimeStamp)
{
	m_stTimeStamp = sTimeStamp;
}
/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_NOTIFICATIONMESSAGE_H__50B5B754_641F_11D2_B5DB_00C095EC9EFA__INCLUDED_)
