/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordKebaPasador.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordKebaPasador.h $
// CHECKIN:		$Date: 21.12.05 15:28 $
// VERSION:		$Revision: 20 $
// LAST CHANGE:	$Modtime: 21.12.05 15:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef RecordKebaPasador_H
#define RecordKebaPasador_H

#include "SDIControlRecord.h"

/////////////////////////////////////////////////////////////////////////////
class CRecordKebaPasador : public CSDIControlRecord
{
// Status der gerade bearbeiteten Daten
enum eStatus
{
	STATUS_DATE_TIME,
	STATUS_TRACK_2,
	STATUS_TRACK_3,
	STATUS_OK,
};

// Typ der Karten-Daten
enum eType
{
	TYPE_EC,
	TYPE_CREDIT,
};

// construction
public:
	CRecordKebaPasador(SDIControlType type,
								  int iCom,
								  BOOL bTraceAscii,
								  BOOL bTraceHex,
								  BOOL bTraceEvents);

// Overrides
public:
	virtual ~CRecordKebaPasador();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:

// implementation
public:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL	CheckForNewAlarmData();
private:
	void	CheckData();
	void	CheckDateTime();
	void	CheckTrack2();
	void	CheckTrack3();
	void	DataCheckedOK();
	void	TransmitDataToSecurity();

// variables
public:
protected:
private:
	eStatus		m_eStatus;
	eType		m_TypKarte;
	BOOL		m_bKontoFound;
	BOOL		m_bECCheckTrack2;
	BOOL		m_bExtraTrace;
};
/////////////////////////////////////////////////////////////////////////////
#endif
