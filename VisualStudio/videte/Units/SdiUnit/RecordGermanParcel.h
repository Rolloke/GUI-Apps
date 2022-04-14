/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordGermanParcel.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordGermanParcel.h $
// CHECKIN:		$Date: 21.12.05 15:28 $
// VERSION:		$Revision: 19 $
// LAST CHANGE:	$Modtime: 21.12.05 15:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef RecordGermanParcel_H
#define RecordGermanParcel_H

/////////////////////////////////////////////////////////////////////////////
#include "SDIControlRecord.h"

/////////////////////////////////////////////////////////////////////////////
class CRecordGermanParcel : public CSDIControlRecord
{

// construction
public:
	CRecordGermanParcel(SDIControlType type,
								   int iCom,
								   BOOL bTraceAscii,
								   BOOL bTraceHex,
								   BOOL bTraceEvents);

// Overrides
public:
	virtual ~CRecordGermanParcel();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
	virtual BOOL	CheckForNewAlarmData();

// access

// implementation
protected:
	void	CheckData();
	void	StoreParcel();
	void	TransmitDataToSecurity();

// data
protected:
	BOOL	m_bInclusiveCheckDigit;
	int		m_iWeight;
	int		m_iIncreaseSum;
	int		m_iModulo;
};
/////////////////////////////////////////////////////////////////////////////
#endif // RecordGermanParcel_H
