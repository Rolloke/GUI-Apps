/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SecID.h $
// ARCHIVE:		$Archive: /Project/CIPC/SecID.h $
// CHECKIN:		$Date: 14.12.05 20:22 $
// VERSION:		$Revision: 57 $
// LAST CHANGE:	$Modtime: 14.12.05 20:22 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _SEC_ID_H_
#define _SEC_ID_H_

#include "wk.h"

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
// invalid id, for constructors and checks
//  an invalid sub id (WORD)(-1)
#define SECID_NO_SUBID		((WORD)0xffff)
//  an invalid group id (WORD)(-1)
#define SECID_NO_GROUPID	((WORD)0xffff)
//  an invalid id (DWORD)(-1)
#define SECID_NO_ID			((DWORD)0xffffffff)

// SECID_GROUPS 0x0001 - 0x0FFF for special IDs
// groupID 0x0001 is used for input activations
#define SECID_ACTIVE_OFF		((DWORD)0x00010000)
#define SECID_ACTIVE_ON			((DWORD)0x00010001)
#define SECID_ACTIVE_LOCK		((DWORD)0x00010002)
//  groupID 0x0002 is used for ISDN connections
#define SECID_ISDN_CONNECTION	((WORD)0x0002)
//  groupID 0x0003 is used for ISDN Input Pipes
#define SECID_ISDN_INPUT_PIPE	((WORD)0x0003)
//  groupID 0x0004 is used for ISDN Output Pipes
#define SECID_ISDN_OUTPUT_PIPE	((WORD)0x0004)
//  groupID 0x0005 is used for Input Clients
#define SECID_INPUT_CLIENT	((WORD)0x0005)
//  groupID 0x0006 is used for Output Clients
#define SECID_OUTPUT_CLIENT	((WORD)0x0006)
//  groupID 0x0007 is used for ISDN DataBase Pipes
#define SECID_ISDN_DATABASE_PIPE	((WORD)0x0007)
//  groupID 0x0008 is used for Integrated Data Parameters
#define SECID_PARAMS	((WORD)0x0008)
//  groupID 0x0009 is used for Integrated Data Parameters (New database fields)
#define SECID_PARAMS_NEW	((WORD)0x000a)
//  groupID 0x000a is used for HTML/Text/Windows in clients
#define SECID_HTML_WINDOWS	((WORD)0x000a)
//  groupID 0x000b is used for Audio Clients
#define SECID_AUDIO_CLIENT	((WORD)0x000b)

// SECID_GROUP_XXX defines
// min/max values for IDs, 1..N numbered in the hiword
// Use the check functions: IsXXXID()
// NOTE:order is important:
//		the sequence of the minvalues is used for the maxvalues!
//  the input group id
#define	SECID_GROUP_INPUT              ((WORD)0x1000)
//  the output group id
#define	SECID_GROUP_OUTPUT             ((WORD)0x2000)
//  the process group id
#define SECID_GROUP_PROCESS             ((WORD)0x3000)
//  the timer group id
#define SECID_GROUP_TIMER               ((WORD)0x4000)
//  the user group id
#define SECID_GROUP_USER                ((WORD)0x5000)
//  the archive group id
#define SECID_GROUP_ARCHIVE             ((WORD)0x6000)
//  the permission group id
#define SECID_GROUP_PERMISSION          ((WORD)0x7000)
//  the host group id
#define SECID_GROUP_HOST                ((WORD)0x8000)
//  the notification message group id
#define SECID_GROUP_NOTIFICATIONMESSAGE ((WORD)0x9000)
//  the media group id
#define SECID_GROUP_MEDIA               ((WORD)0xA000)

#define SECID_GROUP_LAST                ((WORD)0xB000)

//NOTE: same order/values as above!!!
#define	SECID_MIN_INPUT              (DWORD(SECID_GROUP_INPUT)<<16)
#define	SECID_MIN_OUTPUT             (DWORD(SECID_GROUP_OUTPUT)<<16)
#define SECID_MIN_PROCESS             (DWORD(SECID_GROUP_PROCESS)<<16)
#define SECID_MIN_TIMER               (DWORD(SECID_GROUP_TIMER)<<16)
#define SECID_MIN_USER                (DWORD(SECID_GROUP_USER)<<16)
#define SECID_MIN_ARCHIVE             (DWORD(SECID_GROUP_ARCHIVE)<<16)
#define SECID_MIN_PERMISSION          (DWORD(SECID_GROUP_PERMISSION)<<16)
#define SECID_MIN_HOST                (DWORD(SECID_GROUP_HOST)<<16)
#define SECID_MIN_NOTIFICATIONMESSAGE (DWORD(SECID_GROUP_NOTIFICATIONMESSAGE)<<16)
#define SECID_MIN_MEDIA               (DWORD(SECID_GROUP_MEDIA)<<16)
#define SECID_MIN_LAST                (DWORD(SECID_GROUP_LAST)<<16)

#define	SECID_MAX_INPUT              ((DWORD)(SECID_MIN_OUTPUT-1))
#define SECID_MAX_OUTPUT              ((DWORD)(SECID_MIN_PROCESS-1))
#define SECID_MAX_PROCESS             ((DWORD)(SECID_MIN_TIMER-1))
#define SECID_MAX_TIMER               ((DWORD)(SECID_MIN_USER-1))
#define SECID_MAX_USER                ((DWORD)(SECID_MIN_ARCHIVE-1))
#define SECID_MAX_ARCHIVE             ((DWORD)(SECID_MIN_PERMISSION-1))
#define SECID_MAX_PERMISSION          ((DWORD)(SECID_MIN_HOST-1))
#define SECID_MAX_HOST                ((DWORD)(SECID_MIN_NOTIFICATIONMESSAGE-1))
#define SECID_MAX_NOTIFICATIONMESSAGE ((DWORD)(SECID_MIN_MEDIA-1))
#define SECID_MAX_MEDIA               ((DWORD)(SECID_MIN_LAST-1))

//  the special CommUnit input id
#define SECID_INPUT_COMMUNIT		SECID_MAX_INPUT

// RelaisProzesse Flanke==EDGE, Pegel==STATE
//  the special relay process with edge control id
#define SECID_PROCESS_RELAY_EDGE_CONTROL	((DWORD)(SECID_MAX_PROCESS-1))
//  the special relay process with state control id
#define SECID_PROCESS_RELAY_STATE_CONTROL	((DWORD)(SECID_MAX_PROCESS-2))
//  the special camera with motion detection ok process id
#define SECID_PROCESS_CAMERA_MD_OK			((DWORD)(SECID_MAX_PROCESS-3))
//  the special audio recording process id
#define SECID_PROCESS_AUDIO					((DWORD)(SECID_MAX_PROCESS-4))
//  the special ptz preset process id
#define SECID_PROCESS_PRESET				((DWORD)(SECID_MAX_PROCESS-5))
//  the special alarm list process id
#define SECID_PROCESS_ALARM_LIST			((DWORD)(SECID_MAX_PROCESS-6))
	
//  the special idipservice user id
#define SECID_SPECIAL_IDIP_SERVICE_USER		((DWORD)(SECID_MAX_USER))

//  the special supervisor permission id
#define SECID_SUPERVISOR_PERMISSION			((DWORD)(SECID_MIN_PERMISSION))
//  the special supervisor permission id
#define SECID_SPECIAL_RECEIVER_PERMISSION	((DWORD)(SECID_MAX_PERMISSION))

//  the special local host id
#define SECID_LOCAL_HOST					((DWORD)(SECID_GROUP_HOST<<16)|(SECID_NO_SUBID-1))
#define SECSUBID_LOCAL_HOST				(SECID_NO_SUBID-1)

/////////////////////////////////////////////////////////////////////////////
/*  CSecID | A CIPC internal id class to identify serveral 
system wide objects without any pointer or name. A CSecID contains
always a group id and a sub id. i.e. a CSecID can identify a host,
the group id will be the same for all hosts, the sub id is the real
number of the host. A CSecID is stored in a DWORD, high WORD for group
id, and low WORD for sub id.
Always use CSecID as parameter by call, because it's only DWORD size,
and a reference would have also DWORD size.
*/
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CSecID 
{
	//  construction/destruction
public:
	//default constructor with SECID_NO_ID
	inline CSecID();
	//two parameter constructor
	inline CSecID(WORD wGroupID, WORD wSubID);
	//one parameter constructor and also id = DWORD assignment
	inline CSecID(DWORD dwFullID);

	//  operations
public:
	//sets full id value
	inline void Set(DWORD dwID);
	//sets group id and sub id value
	inline void Set(WORD wGroupID, WORD wSubID);
	//sets group id value
	inline void SetGroupID(WORD wGroupID);
	//sets sub id value
	inline void SetSubID(WORD wSubID);

	//  attributes
public:
	//returns full id, no cast operator to prevent from unwished automatic casts
	inline DWORD	GetID() const;
	//returns group id
	inline WORD		GetGroupID() const;
	//returns sub id
	inline WORD		GetSubID() const;
	//returns TRUE if it's an input id
	inline BOOL IsInputID() const;
	//returns TRUE if it's an output id
	inline BOOL IsOutputID() const;
	//returns TRUE if it's a process id
	inline BOOL IsProcessID() const;
	//returns TRUE if it's a timer id
	inline BOOL IsTimerID() const;
	//returns TRUE if it's an archive id
	inline BOOL IsArchiveID() const;
	//returns TRUE if it's a user id
	inline BOOL IsUserID() const;
	//returns TRUE if it's a permission id
	inline BOOL IsPermissionID() const;
	//returns TRUE if it's a host id
	inline BOOL IsHostID() const;
	//returns TRUE if it's a notification message id
	inline BOOL IsNotificationMessageID() const;
	//returns TRUE if it's a media id
	inline BOOL IsMediaID() const;
	// .... more ...

	//  operators
public:
	//returns TRUE if both id's are equal
	inline BOOL operator == (const CSecID id) const;
	//returns TRUE if both id's are not equal
	inline BOOL operator != (const CSecID id) const;

	BOOL IsClassOkay();	// NOTE: currently implemented in sec3.cpp

private:
	// private constructor and functions, checking for invalid parameter
	CSecID(WORD wInvalidID);
	void Set(WORD wInvalidID);
	static void InternalErrorIdRange(DWORD dwFullID);
	
	// union of ONE DWORD, and TWO WORDs. HIWORD,LOWORD is deceided at
	// compile time. IsClassOkay checks for correct size and order!
	union 
	{
		DWORD m_dwSystemID;
		struct 
		{
			WORD m_wSubID;		// LOWORD
			WORD m_wGroupID;	// HIWORD
		} m_split;
	}m_id; 
};
inline CSecID::CSecID()
{
	m_id.m_dwSystemID=SECID_NO_ID;
}
inline CSecID::CSecID(WORD wGroupID, WORD wSubID)
{
	m_id.m_split.m_wGroupID=wGroupID;
	m_id.m_split.m_wSubID=wSubID;
}
inline CSecID::CSecID(DWORD dwFullID)
{
	m_id.m_dwSystemID=dwFullID;
}
inline void CSecID::Set(WORD wGroupID, WORD wSubID)
{
	m_id.m_split.m_wGroupID=wGroupID;
	m_id.m_split.m_wSubID=wSubID;
}
inline void CSecID::Set(DWORD dwFullID)
{
	m_id.m_dwSystemID=dwFullID;
}
inline void CSecID::SetGroupID(WORD wGroupID)
{
	m_id.m_split.m_wGroupID=wGroupID;
}
inline void CSecID::SetSubID(WORD wSubID)
{
	m_id.m_split.m_wSubID=wSubID;
}
// acesss:
inline DWORD CSecID::GetID() const
{
	return m_id.m_dwSystemID;
}
inline WORD CSecID::GetGroupID() const
{
	return m_id.m_split.m_wGroupID;
}
inline WORD CSecID::GetSubID() const
{
	return m_id.m_split.m_wSubID;
}

inline BOOL CSecID::operator == (const CSecID x) const
{
	return (m_id.m_dwSystemID == x.m_id.m_dwSystemID);
}
inline BOOL CSecID::operator != (const CSecID x) const
{
	return (m_id.m_dwSystemID != x.m_id.m_dwSystemID);
}

inline BOOL CSecID::IsInputID() const
{
	return (GetID()>= SECID_MIN_INPUT && GetID()<=SECID_MAX_INPUT);
}
inline BOOL CSecID::IsOutputID() const
{
	return (GetID()>= SECID_MIN_OUTPUT && GetID()<=SECID_MAX_OUTPUT);
}
inline BOOL CSecID::IsProcessID() const
{
	return (GetID()>= SECID_MIN_PROCESS && GetID()<=SECID_MAX_PROCESS);
}
inline BOOL CSecID::IsTimerID() const
{
	return (GetID()>= SECID_MIN_TIMER && GetID()<=SECID_MAX_TIMER);
}
inline BOOL CSecID::IsArchiveID() const
{
	return (GetID()>= SECID_MIN_ARCHIVE && GetID()<=SECID_MAX_ARCHIVE);
}
inline BOOL CSecID::IsUserID() const
{
	return (GetID()>= SECID_MIN_USER && GetID()<=SECID_MAX_USER);
}
inline BOOL CSecID::IsPermissionID() const
{
	return (GetID()>= SECID_MIN_PERMISSION && GetID()<=SECID_MAX_PERMISSION);
}
inline BOOL CSecID::IsHostID() const
{
	return (GetID()>= SECID_MIN_HOST && GetID()<=SECID_MAX_HOST);
}
inline BOOL CSecID::IsNotificationMessageID() const
{
	return (GetID()>= SECID_MIN_NOTIFICATIONMESSAGE && GetID()<=SECID_MAX_NOTIFICATIONMESSAGE);
}
inline BOOL CSecID::IsMediaID() const
{
	return (GetID()>= SECID_MIN_MEDIA && GetID()<=SECID_MAX_MEDIA);
}

#endif
