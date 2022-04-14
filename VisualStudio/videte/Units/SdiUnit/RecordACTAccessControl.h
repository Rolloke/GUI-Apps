// RecordACTAccessControl.h: interface for the CRecordACTAccessControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDACTACCESSCONTROL_H__96AEC8CD_C936_473E_9DEC_6B3728328ACD__INCLUDED_)
#define AFX_RECORDACTACCESSCONTROL_H__96AEC8CD_C936_473E_9DEC_6B3728328ACD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecordConfigurable.h"

class CRecordACTAccessControl : public CRecordConfigurable  
{
public:
	CRecordACTAccessControl(SDIControlType type,
							int iCom,
							BOOL bTraceAscii,
							BOOL bTraceHex,
							BOOL bTraceEvents);

// Overrides
public:
	virtual ~CRecordACTAccessControl();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:

// implementation
public:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL	CheckForNewAlarmData();
private:
	void	TransmitDataToSecurity();

// variables
public:
protected:
private:
	CString	m_sEndOfLine;
	int		m_iEndOfLineLen;
};
//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_RECORDACTACCESSCONTROL_H__96AEC8CD_C936_473E_9DEC_6B3728328ACD__INCLUDED_)
