// SDIAlarmType.h: interface for the CSDIAlarmType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDIALARMTYPE_H__C3DC48A3_CAEE_11D2_B07D_004005A1D890__INCLUDED_)
#define AFX_SDIALARMTYPE_H__C3DC48A3_CAEE_11D2_B07D_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum eSDIAlarmType
{
	SDI_ALARM_UNKNOWN,
	SDI_ALARM_PORTRAIT,
	SDI_ALARM_MONEY,
};

class CSDIAlarmType  
{
public:
// Construction
	CSDIAlarmType(eSDIAlarmType eAlarmType, int iAlarm);
	virtual ~CSDIAlarmType();

// Attributes
	eSDIAlarmType	GetAlarmType();
	int				GetAlarm();
	CString			GetAlarmTypeString();

// Operations
public:

// Implementation
protected:

// Variables
protected:
	eSDIAlarmType	m_eAlarmType;
	int				m_iAlarm;

};
//////////////////////////////////////////////////////////////////////
inline eSDIAlarmType CSDIAlarmType::GetAlarmType()
{
	return m_eAlarmType;
}
//////////////////////////////////////////////////////////////////////
inline int CSDIAlarmType::GetAlarm()
{
	return m_iAlarm;
}
//////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY(CSDIAlarmTypeArrayBase, CSDIAlarmType*)
//////////////////////////////////////////////////////////////////////
//********************************************************************
//////////////////////////////////////////////////////////////////////
// CSDIProtocolArray
class CSDIAlarmTypeArray : public CSDIAlarmTypeArrayBase
{
// Construction
public:
	CSDIAlarmTypeArray();
// Attributes
public:

// Operations
public:
	BOOL	AddOnlyOnce(CSDIAlarmType* pAlarmTypeNew);
	int		AddOrReplaceAllOnlyOnce(CSDIAlarmType* pAlarmTypeNew);

// Implementation
protected:

// Variables
protected:
};
//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_SDIALARMTYPE_H__C3DC48A3_CAEE_11D2_B07D_004005A1D890__INCLUDED_)
