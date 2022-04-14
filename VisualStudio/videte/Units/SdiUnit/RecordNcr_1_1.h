/* GlobalReplace: CRecordNCR --> CRecordNcr_1_1 */
/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordNcr_1_1.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordNcr_1_1.h $
// CHECKIN:		$Date: 21.12.05 15:28 $
// VERSION:		$Revision: 26 $
// LAST CHANGE:	$Modtime: 21.12.05 15:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef RecordNCR_H
#define RecordNCR_H

#include "SDIControlRecord.h"

// Allgemeine Hinweise

/////////////////////////////////////////////////////////////////////////////
class CRecordNcr_1_1 : public CSDIControlRecord
{
// Status der gerade bearbeiteten Daten
enum eStatusNCR
{
	NCR_STX=0,
	NCR_FELD_1=1,
	NCR_FELD_2=2,
	NCR_FELD_3=3,
	NCR_FELD_4=4,
	NCR_FELD_5=5,
	NCR_FELD_6=6,
	NCR_ETX=7,
	NCR_CRC=8,
	NCR_OK=9,
};

// Typ der Meldung
enum eMeldung
{
	MELDUNG_STATUSABFRAGE=0,
	MELDUNG_DIAGNOSE=1,
	MELDUNG_STARTKOMMANDO=2,
};

// construction
public:
	CRecordNcr_1_1(SDIControlType type,
						  int iCom,
						  BOOL bTraceAscii,
						  BOOL bTraceHex,
						  BOOL bTraceEvents);

// Overrides
public:
	virtual	~CRecordNcr_1_1();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:
	virtual void	InitializeResponses();
	virtual BOOL	CheckForNewAlarmData();
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
private:

// implementation
public:
protected:
private:
	BOOL	CheckForNewTAN();
	void	TransmitDataToSecurity();

// variables
public:
protected:
private:
	eStatusNCR		m_eStatus;
	eMeldung		m_eMeldung;
	CString			m_sPreviousTAN;
};
/////////////////////////////////////////////////////////////////////////////
#endif
