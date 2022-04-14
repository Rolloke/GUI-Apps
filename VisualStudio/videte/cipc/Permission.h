/* GlobalReplace: @[mM][dD] -->  */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Permission.h $
// ARCHIVE:		$Archive: /Project/CIPC/Permission.h $
// CHECKIN:		$Date: 12.06.06 14:00 $
// VERSION:		$Revision: 46 $
// LAST CHANGE:	$Modtime: 12.06.06 14:00 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _PERMISSION_HEADER_H_
#define _PERMISSION_HEADER_H_

#include "SecID.h"

class CPermissionArray;

// Permissions for archives access only
#define WK_ALLOW_PICTURE	0x0001
#define WK_ALLOW_DATA		0x0002
#define WK_ALLOW_DELETE		0x0004

// Permissions for general user access
#define WK_ALLOW_HARDWARE		0x00000001
#define WK_ALLOW_INPUT			0x00000002
#define WK_ALLOW_OUTPUT			0x00000004
#define WK_ALLOW_TIMER			0x00000008
#define WK_ALLOW_PROZESS		0x00000010
#define WK_ALLOW_LINK			0x00000020
#define WK_ALLOW_USER			0x00000040
#define WK_ALLOW_PERMISSION		0x00000080
#define WK_ALLOW_HOST			0x00000100
#define WK_ALLOW_ARCHIV			0x00000200
#define WK_ALLOW_BACKUP			0x00000400
#define WK_ALLOW_SDICONFIG		0x00000800
#define WK_ALLOW_SHUTDOWN		0x00001000
#define WK_ALLOW_DATA_EXPORT	0x00002000
#define WK_ALLOW_SETTINGS		0x00004000
#define WK_ALLOW_SECURITY		0x00008000
#define WK_ALLOW_LOGVIEW		0x00010000
#define WK_ALLOW_DATE_TIME		0x00020000
#define WK_ALLOW_IMAGECOMPARE	0x00040000
#define WK_ALLOW_ALARM_OFF_SPAN	0x00080000
#define WK_ALLOW_PTZ_CONFIG		0x00100000
// CAVE: adjust WK_SV_ACCESS if any relevant flag is appended 

// Permission summary for general SV access
// since some flags does not regard SV anyway
// e.g ARCHIV, BACKUP, SDICONFIG, SHUTDOWN, LOGVIEW, DATE_TIME, IMAGECOMPARE, ALARM_OFF_SPAN, PTZ_CONFIG
#define WK_SV_ACCESS			0x0000c3ff


/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CPermission 
{
	// construction
public:
	CPermission();
	CPermission(const CPermission& perm);
	virtual ~CPermission();
	CPermission(CSecID id);	

protected:

	// access
public:
	inline const CString& GetName() const; // returns the permission's name
	inline const CString& GetPassword() const;// returns the permission's password
	inline const CString& GetComment() const;// returns the permission's comment
	inline		 CSecID	GetID() const;// returns the permission's unique ID
	inline		 int	GetPriority() const;// returns the permission's priority
				 BOOL   IsOutputAllowed(CSecID id);
				 BOOL   IsArchivAllowed(CSecID id, UINT uFlags);
				 UINT	GetArchivFlags(CSecID id);

				 int	GetNumberOfArchivs();
				 CSecID GetArchiv(int nIndex);
				 int	GetNumberOfOutputs();
				 CSecID GetOutput(int nIndex);
	inline		 BOOL	IsSuperVisor();
	inline		 BOOL	IsSpecialReceiver();
	inline	     CSecID GetTimer() const;

	//!ic! operations
public:
	//!ic! name & password
	void SetName(const CString& sName); // sets the permission's name
	void SetPassword(const CString&  sPassword);// sets the permission's password
	void SetComment(const CString&  sComment);// sets the permission's comment
	void SetPriority(int iPriority);// sets the permission's priority
	void SetTimer(CSecID id);

	//!ic! permissions
	DWORD GetFlags() const;
	void  AddFlags(DWORD dwFlags);
	void  SetFlags(DWORD dwFlags);
	void  DeleteFlags();
	void  AddOutput(CSecID id);
	void  DeleteOutput(CSecID id);
	void  AddArchiv(CSecID id, UINT uFlags);
	void  SetArchivFlags(CSecID id, UINT uFlags);
	void  DeleteArchiv(CSecID id);
	void  DeleteAllArchivs();
	void  DeleteAllOutputs();
	void  DeleteAll();

	//!ic! serialization
	BOOL Load(int i,CWK_Profile& wkProfile);
#ifdef _UNICODE
	void Save(int i,CWK_Profile& wkProfile, bool bUseBinaryFormat);
#else
	void Save(int i,CWK_Profile& wkProfile);
#endif
	//!ic! data
private:
	CSecID			m_ID;
	CString			m_Name;
	CString			m_Password;
	CString			m_Comment;

	//!ic! rights
	CDWordArray		m_OutputIDs;				
	CDWordArray		m_ArchivIDs;
	DWORD			m_Flags;
	int				m_Priority;
	CSecID			m_idTimer;

	friend class CPermissionArray;
};
class CConnectionRecord;
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CPermissionArray : public CTypedPtrArray<CPtrArray,CPermission*>
{
public:
	CPermissionArray();
	virtual ~CPermissionArray();
public:
	//!ic! operations
	void Load(CWK_Profile& wkProfile);
	void Save(CWK_Profile& wkProfile);
	void DeleteAll();

	CPermission*  AddPermission();
	void		  DeletePermission(CPermission* pPermission);
	//
	inline CPermission*  GetAtFast(int nPos) const;
	CPermission*  GetPermission(const CSecID& id);
	CPermission*  GetPermission(const CString& sName);
	CPermission*  GetSuperVisor();
	CPermission*  GetSpecialReceiver();
	//
	CPermission* CheckPermission(const CConnectionRecord& c) const;
#ifdef _UNICODE
public:
	BOOL	m_bUseBinaryFormat;
#endif
};

/////////////////////////////////////////////////////////////////////////////
inline const CString& CPermission::GetName() const
{
	return m_Name;
}
inline const CString& CPermission::GetPassword() const
{
	return m_Password;
}
inline const CString& CPermission::GetComment() const
{
	return m_Comment;
}
inline CSecID CPermission::GetID() const
{
	return (m_ID);
}
inline int CPermission::GetPriority() const
{
	return (m_Priority);
}
/* returns the permission's flags for SystemVerwaltung

  WK_ALLOW_HARDWARE	

  WK_ALLOW_INPUT		

  WK_ALLOW_OUTPUT		

  WK_ALLOW_TIMER		

  WK_ALLOW_PROZESS	

  WK_ALLOW_LINK		

  WK_ALLOW_USER		

  WK_ALLOW_PERMISSION	

  WK_ALLOW_HOST		

  WK_ALLOW_ARCHIV

  WK_ALLOW_BACKUP
 */
inline DWORD CPermission::GetFlags() const
{
	return (m_Flags);
}
/* not yet documented*/
inline BOOL	CPermission::IsSuperVisor()
{
	return m_ID.GetID()== SECID_SUPERVISOR_PERMISSION;
}
/* not yet documented*/
inline BOOL	CPermission::IsSpecialReceiver()
{
	return m_ID.GetID()== SECID_SPECIAL_RECEIVER_PERMISSION;
}
/* not yet documented*/
inline void CPermission::AddFlags(DWORD dwFlags)
{
	m_Flags = dwFlags;
}
/* not yet documented*/
inline void CPermission::SetFlags(DWORD dwFlags)
{
	m_Flags = dwFlags;
}
/* not yet documented*/
inline void CPermission::DeleteFlags()
{
	m_Flags = 0L;
}
/////////////////////////////////////////////////////////////////////////////
inline CSecID CPermission::GetTimer() const
{
	return m_idTimer;
}
/////////////////////////////////////////////////////////////////////////////
inline CPermission*  CPermissionArray::GetAtFast(int nPos) const
{
	ASSERT(nPos >= 0 && nPos < m_nSize);
	return (CPermission* ) m_pData[nPos];
}
/////////////////////////////////////////////////////////////////////////////
#endif

