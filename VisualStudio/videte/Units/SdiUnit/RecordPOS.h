// RecordPOS.h: interface for the CRecordScanner class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "sdicontrolrecord.h"

class CRecordPOS : public CSDIControlRecord
{
public:
	CRecordPOS(SDIControlType type,int iCom,BOOL bTraceAscii,BOOL bTraceHex,BOOL bTraceEvents);
	virtual ~CRecordPOS();
	
	// Overrides
public:
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL	CheckForNewAlarmData();
			void	MySendAlarm();
};
