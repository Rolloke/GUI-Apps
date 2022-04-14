// SDIAlarmType.cpp: implementation of the CSDIAlarmType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "SDIAlarmType.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSDIAlarmType::CSDIAlarmType(eSDIAlarmType eAlarmType, int iAlarm)
{
	m_eAlarmType = eAlarmType;
	m_iAlarm = iAlarm;
}
//////////////////////////////////////////////////////////////////////
CSDIAlarmType::~CSDIAlarmType()
{
}
//////////////////////////////////////////////////////////////////////
CString CSDIAlarmType::GetAlarmTypeString()
{
	CString sReturn;
	switch (m_eAlarmType) {
		case SDI_ALARM_PORTRAIT:
			sReturn.LoadString(IDS_PORTRAIT);
			break;
		case SDI_ALARM_MONEY:
			sReturn.LoadString(IDS_HAND_TO_MONEY);
			break;
		default:
		case SDI_ALARM_UNKNOWN:
			{
				BOOL b = FALSE;
				WK_ASSERT(b);
			}
			break;
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
//////////////////////////////////////////////////////////////////////
CSDIAlarmTypeArray::CSDIAlarmTypeArray()
{
	SetAutoDelete(TRUE);
}
//////////////////////////////////////////////////////////////////////
BOOL CSDIAlarmTypeArray::AddOnlyOnce(CSDIAlarmType* pAlarmTypeNew)
{
	BOOL bReturn = FALSE;
	int iCounter = GetSize();
	CSDIAlarmType* pAlarmType = NULL;
	BOOL bFound = FALSE;
	for (int i=0 ; !bFound && i<iCounter ; i++) {
		pAlarmType = GetAt(i);
		if (   pAlarmType->GetAlarmType() == pAlarmTypeNew->GetAlarmType()
			|| pAlarmType->GetAlarm() == pAlarmTypeNew->GetAlarm() )
		{
			bFound = TRUE;
		}
	}
	if (!bFound) {
		Add(pAlarmTypeNew);
		bReturn = TRUE;
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////
int CSDIAlarmTypeArray::AddOrReplaceAllOnlyOnce(CSDIAlarmType* pAlarmTypeNew)
{
	int iReturn = -1;
	int iCounter = GetSize();
	CSDIAlarmType* pAlarmType = NULL;
	BOOL bFound = FALSE;
	for (int i=0 ; !bFound && i<iCounter ; i++) {
		pAlarmType = GetAt(i);
		if (pAlarmType) {
			if (	pAlarmType->GetAlarmType() == pAlarmTypeNew->GetAlarmType()
				|| pAlarmType->GetAlarm() == pAlarmTypeNew->GetAlarm() )
			{
				bFound = TRUE;
				RemoveAt(i);
				WK_DELETE(pAlarmType);
				iReturn = Add(pAlarmTypeNew);
			}
		}
	}
	if (!bFound) {
		iReturn = Add(pAlarmTypeNew);
	}
	return iReturn;
}
