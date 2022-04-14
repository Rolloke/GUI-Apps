/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordConfigurable.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordConfigurable.h $
// CHECKIN:		$Date: 27.08.03 10:17 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 27.08.03 9:43 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#ifndef RecordConfigurable_H
#define RecordConfigurable_H

#include "SDIControlRecord.h"

// Allgemeine Hinweise

/////////////////////////////////////////////////////////////////////////////
class CRecordConfigurable : public CSDIControlRecord
{
// construction
public:
	CRecordConfigurable(SDIControlType type,
								   int iCom,
								   BOOL bTraceAscii,
								   BOOL bTraceHex,
								   BOOL bTraceEvents);
	~CRecordConfigurable();

// Overrides
public:
protected:
	virtual	void	StoreData(CSecID idParam);
	virtual	void	StoreNewData(WORD wID);
private:

// implementation
public:
protected:
	BOOL	CreateRecord();
	void	TraceProtocolData();
	void	CheckAndStoreVariableData();
	void	CheckForProtocol();
	eSDIProtocolFormat	CheckForProtocolDataIdentification();
	void	CheckForProtocolDataByPosition();
	void	CheckForProtocolDataBySeperator();
	void	CheckForProtocolDataByTag();
private:

// variables
public:
protected:
	CSDIProtocolArray*	m_pProtocolArray;			// Welche Protokolle sind eingestellt?
	CSDIProtocol*		m_pProtocol;				// Aktuell zu erkennendes Protokoll
	CSDIProtocolData*	m_pProtocolData;			// Aktuell zu bearbeitendes Datum
	WORD				m_wProtocolPositionAsAlarm;	// 0 indicates an invalid alarm
private:
};
/////////////////////////////////////////////////////////////////////////////
#endif // RecordConfigurable_H
