// AlarmObject.cpp: implementation of the CAlarmObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alarmlist.h"
#include "AlarmObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CAlarmObject::CAlarmObject()
{
	m_nAlarmCam		= 0;
	m_nAlarmInput	= 0;
	m_AlarmTime		= 0;
}

//////////////////////////////////////////////////////////////////////
CAlarmObject::CAlarmObject(const CAlarmObject* pAlarm)
{
	m_nAlarmCam		= pAlarm->GetAlarmCam();
	m_nAlarmInput	= pAlarm->GetAlarmInput();
	m_AlarmTime		= pAlarm->GetAlarmTime();
	m_sArchiveName	= pAlarm->GetArchiveName();
}

CAlarmObject::CAlarmObject(int nAlarmCam, const CString& sName, int nAlarmInput, CSystemTime AlarmTime)
{
	m_nAlarmCam		= nAlarmCam;
	m_nAlarmInput	= nAlarmInput;
	m_AlarmTime		= AlarmTime;
	m_sArchiveName	= sName;
}

//////////////////////////////////////////////////////////////////////
CAlarmObject::~CAlarmObject()
{

}

//////////////////////////////////////////////////////////////////////
void CAlarmObject::SetAlarmCam(int nAlarmCam)
{
	m_nAlarmCam		= nAlarmCam;
}

//////////////////////////////////////////////////////////////////////
void CAlarmObject::SetArchiveName(const CString& sName)
{
	m_sArchiveName	= sName;
}

//////////////////////////////////////////////////////////////////////
void CAlarmObject::SetAlarmInput(int nAlarmInput)
{
	m_nAlarmInput	= nAlarmInput;
}

//////////////////////////////////////////////////////////////////////
void CAlarmObject::SetAlarmTime(CSystemTime AlarmTime)
{
	m_AlarmTime		= AlarmTime;
}

//////////////////////////////////////////////////////////////////////
CString CAlarmObject::GetFormatString()
{
	CString sText;
	CString sTime;

	switch (m_nAlarmInput)
	{
		case 0:
			sText.Format("%-2d1 %s", m_nAlarmCam+1, m_AlarmTime.GetTime());
			break;
		case 1:
			sText.Format("%-2d2 %s", m_nAlarmCam+1, m_AlarmTime.GetTime());
			break;
		case 2:
			sText.Format("%-2d+ %s", m_nAlarmCam+1, m_AlarmTime.GetTime());
			break;
	}

	return sText;
}

//////////////////////////////////////////////////////////////////////
CString CAlarmObject::GetFormatPrintString()
{
	CString sText;
	CString sTime;
	CString sCamName = m_sArchiveName + CString(" ", 16);
	sCamName = sCamName.Left(16);

	switch (m_nAlarmInput)
	{
		case 0:
			sText.Format("%s 1 %s %s", sCamName, m_AlarmTime.GetTime(), m_AlarmTime.GetDate());
			break;
		case 1:
			sText.Format("%s 2 %s %s", sCamName, m_AlarmTime.GetTime(), m_AlarmTime.GetDate());
			break;
		case 2:
			sText.Format("%s + %s %s", sCamName, m_AlarmTime.GetTime(), m_AlarmTime.GetDate());
			break;
	}

	return sText;
}

