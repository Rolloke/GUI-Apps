
#ifndef _CPTAlarm_H
#define _CPTAlarm_H

#include "WK_Template.h"

class CPTAlarm {
friend class CPTBox;	// OOPS should be Set....
public:
	inline CPTAlarm(WORD wx);
	//
	inline const CString &GetName() const;
	inline CSecID GetID() const;
	//
	inline BOOL IsActive() const;
private:
	CString m_sName;
	CSecID m_id;
	//
	BOOL m_bIsActive;
};

typedef CPTAlarm* CPTAlarmPtr;
WK_PTR_ARRAY_CS(CPTAlarmsPlain,CPTAlarmPtr,CPTAlarms);

///////////////// inlined fns ////////////////////

inline CPTAlarm::CPTAlarm(WORD wx)
{
	m_id.Set(SECID_GROUP_INPUT,wx);
	m_bIsActive=FALSE;
	m_sName = "Unbekannt";

}
inline const CString &CPTAlarm::GetName() const
{
	return m_sName;
}
//
inline BOOL CPTAlarm::IsActive() const
{
	return m_bIsActive;
}

inline CSecID CPTAlarm::GetID() const
{
	return m_id;
}

#endif
