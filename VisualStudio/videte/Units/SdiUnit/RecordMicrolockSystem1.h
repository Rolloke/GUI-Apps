// RecordMicrolockSystem1.h: interface for the CRecordMicrolockSystem1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDMICROLOCKSYSTEM1_H__55A0DEB8_2E4A_44FD_AB82_11AC43ED3FEB__INCLUDED_)
#define AFX_RECORDMICROLOCKSYSTEM1_H__55A0DEB8_2E4A_44FD_AB82_11AC43ED3FEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecordConfigurable.h"

//////////////////////////////////////////////////////////////////////
class CRecordMicrolockSystem1 : public CRecordConfigurable  
{
public:
	CRecordMicrolockSystem1(SDIControlType type,
							int iCom,
							BOOL bTraceAscii,
							BOOL bTraceHex,
							BOOL bTraceEvents);

// Overrides
public:
	virtual ~CRecordMicrolockSystem1();
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
#endif // !defined(AFX_RECORDMICROLOCKSYSTEM1_H__55A0DEB8_2E4A_44FD_AB82_11AC43ED3FEB__INCLUDED_)
