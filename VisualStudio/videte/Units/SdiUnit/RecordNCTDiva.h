// RecordNCTDiva.h: interface for the CRecordNCTDiva class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDNCTDIVA_H__5A6B9C9A_FEC5_4624_A088_D9991F8C9816__INCLUDED_)
#define AFX_RECORDNCTDIVA_H__5A6B9C9A_FEC5_4624_A088_D9991F8C9816__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecordConfigurable.h"

class CRecordNCTDiva : public  CRecordConfigurable  
{
public:
	CRecordNCTDiva(SDIControlType type,
							int iCom,
							BOOL bTraceAscii,
							BOOL bTraceHex,
							BOOL bTraceEvents);

// Overrides
public:
	virtual ~CRecordNCTDiva();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:

// implementation
public:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL	CheckForNewAlarmData();
private:
	void	HandleData();
	void	TransmitDataToSecurity();

// variables
public:
protected:
private:
	// Daten, die über einen Vorgang erhalten bleiben sollen
};

#endif // !defined(AFX_RECORDNCTDIVA_H__5A6B9C9A_FEC5_4624_A088_D9991F8C9816__INCLUDED_)
