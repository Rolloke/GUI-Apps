// AlarmObject.h: interface for the CAlarmObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMOBJECT_H__0946BD1C_3F76_4078_A415_D9D2FDB3F796__INCLUDED_)
#define AFX_ALARMOBJECT_H__0946BD1C_3F76_4078_A415_D9D2FDB3F796__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SystemTime.h"

class CAlarmObject  : public CObject
{
public:
	CAlarmObject();
	CAlarmObject(const CAlarmObject* pAlarm);
	CAlarmObject(int nAlarmCam, const CString& sName, int nAlarmInput, CSystemTime AlarmTime);

	
	void SetAlarmCam(int nCam);
	void SetArchiveName(const CString& sName);
	void SetAlarmInput(int nInput);
	void SetAlarmTime(CSystemTime time);
	
	int GetAlarmCam() const {return m_nAlarmCam;};
	int GetAlarmInput() const {return m_nAlarmInput;};
	CString GetArchiveName() const {return m_sArchiveName;};
	CSystemTime GetAlarmTime() const {return m_AlarmTime;};

	CString GetFormatString();
	CString GetFormatPrintString();
	virtual ~CAlarmObject();

private:
	int	m_nAlarmCam;
	int m_nAlarmInput;
	CSystemTime m_AlarmTime;
	CString	m_sArchiveName;
};

#endif // !defined(AFX_ALARMOBJECT_H__0946BD1C_3F76_4078_A415_D9D2FDB3F796__INCLUDED_)
