// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 30.12.05 10:01 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#pragma once
#include "sdicontrolrecord.h"

class CRecordSchneiderIntercom : public CSDIControlRecord
{
public:
	CRecordSchneiderIntercom(SDIControlType type,
							int iCom,
							BOOL bTraceAscii,
							BOOL bTraceHex,
							BOOL bTraceEvents);
	virtual ~CRecordSchneiderIntercom();

	// Overrides
public:
	virtual BOOL Create(CWK_Profile& wkp, const CString& sSection);

protected:
	virtual void OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL CheckForNewAlarmData();
	virtual void InitializeResponses();
			void ScanData();

protected:
	CSDIAlarmNumbers m_AlarmNumbers;
	CString m_sData;
};
