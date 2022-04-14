// RecordScanner.h: interface for the CRecordScanner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDSCANNER_H__9D158064_9951_4A24_9F68_147C74CE88E8__INCLUDED_)
#define AFX_RECORDSCANNER_H__9D158064_9951_4A24_9F68_147C74CE88E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDIControlRecord.h"

class CRecordScanner : public CSDIControlRecord  
{
public:
	CRecordScanner(SDIControlType type,
								   int iCom,
								   BOOL bTraceAscii,
								   BOOL bTraceHex,
								   BOOL bTraceEvents);
	virtual ~CRecordScanner();

// Overrides
public:
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL	CheckForNewAlarmData();
};

#endif // !defined(AFX_RECORDSCANNER_H__9D158064_9951_4A24_9F68_147C74CE88E8__INCLUDED_)
