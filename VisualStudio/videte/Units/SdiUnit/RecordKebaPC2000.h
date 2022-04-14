/* GlobalReplace: RecordKebaPC2000 --> RecordKebaPc2000 */
/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordKebaPC2000.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordKebaPC2000.h $
// CHECKIN:		$Date: 21.12.05 15:28 $
// VERSION:		$Revision: 19 $
// LAST CHANGE:	$Modtime: 21.12.05 15:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef RecordKebaPc2000_H
#define RecordKebaPc2000_H

#include "RecordKebaPc400.h"

/////////////////////////////////////////////////////////////////////////////
class CRecordKebaPc2000 : public CRecordKebaPc400
{
// construction
public:
	CRecordKebaPc2000(SDIControlType type,
								 int iCom,
								 BOOL bTraceAscii,
								 BOOL bTraceHex,
								 BOOL bTraceEvents);

// Overrides
public:
	virtual ~CRecordKebaPc2000();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
	virtual	void	SwitchToConfigMode(const CString& sPath, BOOL bReceive);
protected:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);

	virtual void	OnTimeOutTransparent();
	virtual void	OnXModemOK();
	virtual void	OnXModemFailed();

// implementation
public:
protected:
private:
	void	StartConfigMode();
	void	ResetToNormalMode();

// variables
public:
protected:
	eType		m_eTyp;
	eStatus		m_eStatus;
	int			m_iFailureCounter;
	WORD		m_wAlarm;
private:
	BOOL		m_bConfigMode;
	BOOL		m_bConfigTransmit;
	CString		m_sConfigFilePath;
	DWORD		m_dwCounterForMissedConfigAck;
};
/////////////////////////////////////////////////////////////////////////////
#endif
