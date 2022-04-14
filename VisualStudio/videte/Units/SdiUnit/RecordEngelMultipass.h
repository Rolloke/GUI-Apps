/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordEngelMultipass.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordEngelMultipass.h $
// CHECKIN:		$Date: 21.12.05 15:28 $
// VERSION:		$Revision: 20 $
// LAST CHANGE:	$Modtime: 21.12.05 15:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef RecordEngelMultipass_H
#define RecordEngelMultipass_H

#include "SDIControlRecord.h"

/////////////////////////////////////////////////////////////////////////////
class CRecordEngelMultipass : public CSDIControlRecord
{
// Typ des Kommandos
enum eTypEngel
{
	ENGEL_TYP_UNKNOWN,
	ENGEL_TYP_E,
	ENGEL_TYP_L,
};

// Status der gerade bearbeiteten Daten
enum eStatusEngel
{
	ENGEL_KENNUNG,
	ENGEL_DATUM,
	ENGEL_ZEIT,
	ENGEL_BLZ,
	ENGEL_KONTO,
	ENGEL_SONDER,
	ENGEL_KREDIT,
	ENGEL_CR,
	ENGEL_OK,
};

// construction
public:
	CRecordEngelMultipass(SDIControlType type,
									 int iCom,
									 BOOL bTraceAscii,
									 BOOL bTraceHex,
									 BOOL bTraceEvents);

// Overrides
public:
	virtual ~CRecordEngelMultipass();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL	CheckForNewAlarmData();

// implementation
public:
private:
	void	OnStatusKennung();
	void	OnStatusDatum();
	void	OnStatusZeit();
	void	OnStatusBLZ();
	void	OnStatusKonto();
	void	OnStatusSonder();
	void	OnStatusKredit();
	void	IncreaseStatus();
	void	TransmitDataToSecurity();

// data
public:
protected:
private:
	eTypEngel		m_eTyp;
	eStatusEngel	m_eStatus;
};
/////////////////////////////////////////////////////////////////////////////
#endif
