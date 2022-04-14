/* GlobalReplace: CRecordIBM --> CRecordIbm */
/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordIbm.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordIbm.h $
// CHECKIN:		$Date: 21.12.05 15:28 $
// VERSION:		$Revision: 20 $
// LAST CHANGE:	$Modtime: 21.12.05 15:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef RecordIBM_H
#define RecordIBM_H

#include "RecordConfigurable.h"

// Allgemeine Hinweise
//	Dieser GA liefert z.Z. 16 Alarme

#define IBM_MSG_HEADER_LEN	28

/////////////////////////////////////////////////////////////////////////////
class CRecordIbm : public CRecordConfigurable
{
// Die Photosteps des Protokolls
enum eIBMPhotoStep
{
	IBM_STEP_UNKNOWN=-1,
	IBM_STEP_RESERVED_0=0,
	IBM_STEP_OPEN=1,
	IBM_STEP_CLOSE=2,
	IBM_STEP_BEFOR_PIN=3,
	IBM_STEP_RESERVED_4=4,
	IBM_STEP_RESERVED_5=5,
	IBM_STEP_BEFORE_SEND_AUTHORIZED_TRANSAKTION=6,
	IBM_STEP_AFTER_RECEIVED_AUTHORIZED_TRANSAKTION=7,
	IBM_STEP_RESERVED_8=8,
	IBM_STEP_BEFORE_RETURN_CARD=9,
	IBM_STEP_AFTER_RETURN_CARD=10,
	IBM_STEP_BEFORE_CURRENCY_ISSUED=11,
	IBM_STEP_BEFORE_RETURN_PASSBOOK=12,
	IBM_STEP_AFTER_RETURN_PASSBOOK=13,
	IBM_STEP_RESERVED_14=14,
	IBM_STEP_RESERVED_15=15,
	IBM_STEP_RESERVED_16=16,
};

// Status der gerade bearbeiteten Daten
enum eStatusIBM
{
	IBM_OK=-3,
	IBM_STX=-2,
	IBM_UNKNOWN=-1,
	IBM_LENGTH=0,
	IBM_FORMAT=1,
	IBM_WSID=2,
	IBM_DATE_WS=3,
	IBM_TIME_WS=4,
	IBM_TAN_WS=5,
	IBM_PHOTO_STEP=6,
	IBM_STATE_WS=7,
	IBM_DATA=8,
	IBM_ETX=9,
	IBM_CRC=10,
};

// construction
public:
	CRecordIbm(SDIControlType type,
						  int iCom,
						  BOOL bTraceAscii,
						  BOOL bTraceHex,
						  BOOL bTraceEvents);

// Overrides
public:
	virtual ~CRecordIbm();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:
	virtual void	InitializeResponses();
	virtual BOOL	CheckForNewAlarmData();
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);

// implementation
private:
	BOOL	IsPhotoStepValid();
	void	TransmitDataToSecurity();
// variables
public:
protected:
private:
	eStatusIBM			m_eStatus;			// Status des gerade bearbeiteten Datums
	int					m_iPhotoStep;		// PhotoStep
	BOOL				m_bNewAlarm;		// Beginnt ein neuer Transaktionszyklus
};
/////////////////////////////////////////////////////////////////////////////
#endif // RecordIBM_H
