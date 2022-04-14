/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcInputRecord.h $
// ARCHIVE:		$Archive: /Project/CIPC/CipcInputRecord.h $
// CHECKIN:		$Date: 7.07.04 17:01 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 7.07.04 16:46 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __CIPC_INPUT_RECORD_H
#define __CIPC_INPUT_RECORD_H

#include "wk.h"
#include "SecID.h"

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
/*  CIPCInputRecord | The class encapslated a CIPC input object,
in most cases this is an alarm detector. The exception is the virtual
comm port CIPCInputRecord for RS232 pan tilt zoom control or transparent data
transmission. An input has a name, an id and a state. The state of the input
is wether ok, alarm or defect or deactivated.
*/
/////////////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCInputRecord : public CObject
{
	DECLARE_DYNAMIC(CIPCInputRecord)	// runtime class identification	
	//  construction/destruction
public:
	//!ic! default constructor
	inline CIPCInputRecord();
	//!ic! assignment operator
	inline const CIPCInputRecord & operator = (const CIPCInputRecord& rec);
	//!ic! copy constructor
	inline CIPCInputRecord(const CIPCInputRecord& rec);
	
	//  attributes
public:
	//!ic! returns the name of the input
	inline const CString &GetName() const;
	//!ic! returns the id of the input
	inline CSecID GetID() const;
	//!ic! returns the flags of the input
	inline BYTE GetFlags() const;
	//!ic! is hardware state OK
	inline BOOL	GetIsOkay() const;
	//!ic! is enabled by sysadmin
	inline BOOL	GetIsEnabled() const;
	//!ic! is in alarm state
	inline BOOL	GetIsActive() const;
	//!ic! needs a user confirmation
	inline BOOL	GetNeedsConfirmations() const;
	//!ic! returns the edge of the input 0==negative edge, 1==positive edge
	inline BOOL	GetEdge() const;	
	//!ic! is virtual comm port
	inline BOOL IsCommPort() const;
	//!ic! is temporary deactivation allowed
	inline BOOL	GetIsTempDeactivateAllowed() const;
	
	//  operations
public:
	//!ic! sets name id and flags
	inline void Set(const CString &s, CSecID id, BYTE b);
	//!ic! sets flags only, i.e. to change state
	inline void SetFlags(BYTE bNewFlags);
	//
	inline BOOL operator != (const CIPCInputRecord& or);

private:
	// data:
	CString m_sName;
	CSecID	m_id;
	BYTE	m_bFlags;
};
///////////////////////////////////////////////////////////////////
//  CIPCInputRecord hardware ok
#define IREC_STATE_OKAY					((BYTE)0x01)
//  CIPCInputRecord enabled by sysadmin
#define IREC_STATE_ENABLED				((BYTE)0x02)
//  CIPCInputRecord in alarm/active state
#define IREC_STATE_ALARM				((BYTE)0x04)
//  CIPCInputRecord positive edge ?
#define IREC_STATE_EDGE					((BYTE)0x08)
//  CIPCInputRecord needs a user confirmation
#define IREC_STATE_NEEDS_CONFIRMATION	((BYTE)0x10)
//  CIPCInputRecord is virtual commport id
#define IREC_IS_COMM_PORT				((BYTE)0x20)
//  CIPCInputRecord is temporary deactivation allowed
#define IREC_STATE_TEMP_DEACTIVATE		((BYTE)0x40)
///////////////////////////////////////////////////////////////////
/*Function: 
default construtor: empty name, SECID_NO_ID, empty flags (disabled, not ok, not active)
*/
inline CIPCInputRecord::CIPCInputRecord()
{
	// m_sName;
	// m_id;
	m_bFlags=0;	// not enabled
}
/*Function: 
copy constructor 
*/
inline const CIPCInputRecord & CIPCInputRecord::operator=(const CIPCInputRecord& rec)
{
	m_sName  = rec.m_sName;
	m_id     = rec.m_id;
	m_bFlags = rec.m_bFlags;
	return *this;
}
/*Function: 
assignment operator
*/
inline CIPCInputRecord::CIPCInputRecord(const CIPCInputRecord& rec)
{
	m_sName  = rec.m_sName;
	m_id     = rec.m_id;
	m_bFlags = rec.m_bFlags;
}

/*Function: 
returns the name of the CIPCInputRecord
@rdesc a CString reference to the name of the CIPCInputRecord
*/
inline const CString &CIPCInputRecord::GetName() const
{
	return m_sName;
}
/*Function: 
returns the id of alarm detector
@rdesc a CSecID, id of alarm detector
 <c CSecID>
*/
inline CSecID CIPCInputRecord::GetID() const
{
	return m_id;
}
/*Function: 
returns hardware state of alarm detector
@rdesc TRUE if the hardware state is OK, otherwise FALSE
*/
inline BOOL	CIPCInputRecord::GetIsOkay() const
{
	return (m_bFlags & IREC_STATE_OKAY)!=0;
}
/*Function: 
returns configuration state of alarm detector
@rdesc TRUE if the hardware configuration is enabled, otherwise FALSE
*/
inline BOOL	CIPCInputRecord::GetIsEnabled() const
{
	return (m_bFlags & IREC_STATE_ENABLED)!=0;
}
/*Function: 
returns alarm state of alarm detector
@rdesc TRUE if the current state is ALARM, otherwise FALSE for OK state
or disabled detector
*/
inline BOOL	CIPCInputRecord::GetIsActive() const
{
	return ((m_bFlags & IREC_STATE_ENABLED)!=0)
			&& ((m_bFlags & IREC_STATE_ALARM)!=0);
}
/*Function: 
returns TRUE if alarm detector has confirmation function. In
some cases alarm state goes only from ALARM to OK if user confirms
the alarm.
@rdesc TRUE if the alarm detector needs a user confirmation, otherwise FALSE
*/
inline BOOL	CIPCInputRecord::GetNeedsConfirmations() const
{
	return (m_bFlags &IREC_STATE_NEEDS_CONFIRMATION)!=0;
}
/*Function: is it the virtual comm port input. There's only
one comm port, if the sysadmin has activated a pan tilt zoom
camera of transparent RS232 transmission.
@rdesc returns TRUE only for the comm port input*/
inline BOOL	CIPCInputRecord::IsCommPort() const
{
	return (m_bFlags & IREC_IS_COMM_PORT)!=0;
}
/*Function: 
has the input a negative (FALSE) or positive(TRUE) edge
@rdesc returns FALSE for a negative edge, TRUE for a positive edge
*/
inline BOOL	CIPCInputRecord::GetEdge() const
{
	return (m_bFlags & IREC_STATE_EDGE)!=0;
}
/*Function: 
is the temporary deactivation of the input allowed
@rdesc returns TRUE if the temporary deactivation is allowed, otherwise FALSE
*/
inline BOOL	CIPCInputRecord::GetIsTempDeactivateAllowed() const
{
	return (m_bFlags & IREC_STATE_TEMP_DEACTIVATE)!=0;
}
/*Function: 
the state of the input as flags of BYTE size
@rdesc a BYTE, each bit representing one flag of the input state
*/
inline BYTE CIPCInputRecord::GetFlags() const
{
	return m_bFlags;
}
/*Function: 
Set's name, id, and flags of the input
param: const CString & | s | the name of the input
param: CSecID | id | the input sec id of the input
param: BYTE | b | the flags of the input, defining the state
 <c CSecID>, <mf CIPCInputRecord.SetFlags>
*/
inline void CIPCInputRecord::Set(const CString &s, CSecID id, BYTE b)
{
	m_sName=s;
	m_id=id;
	m_bFlags=b;
}
/*Function: 
Set's  flags of the input
param: BYTE | b | the flags of the input, defining the state
 <mf CIPCInputRecord.Set>
*/
inline void CIPCInputRecord::SetFlags(BYTE b)
{
	m_bFlags=b;
}

inline BOOL CIPCInputRecord::operator != (const CIPCInputRecord& or)
{
	return    m_sName != or.m_sName
		   || m_id != or.m_id
		   || m_bFlags != or.m_bFlags;

}
#endif

