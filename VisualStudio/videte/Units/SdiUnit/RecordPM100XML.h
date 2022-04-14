// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 29.12.05 13:11 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#pragma once
#include "sdicontrolrecord.h"

class CRecordPM100XML :
	public CSDIControlRecord
{
public:
	CRecordPM100XML(SDIControlType type,
					int iPort,
					LPCTSTR szIPAddress,
					BOOL bTraceAscii,
					BOOL bTraceHex);
	virtual ~CRecordPM100XML(void);

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
