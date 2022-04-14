// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 28.12.05 20:43 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#pragma once
#include "sdicontrolrecord.h"

class CRecordPM100Alarm : public CSDIControlRecord
{
	// construction
public:
	CRecordPM100Alarm(SDIControlType type,
				 int iPort,
				 LPCTSTR szIPAddress,
				 BOOL bTraceAscii,
				 BOOL bTraceHex);
	virtual ~CRecordPM100Alarm();


	// Overrides
public:
	virtual BOOL Create(CWK_Profile& wkp, const CString& sSection);

protected:
	virtual void OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL CheckForNewAlarmData();
	virtual void InitializeResponses();

protected:
	CSDIAlarmNumbers m_AlarmNumbers;

};
