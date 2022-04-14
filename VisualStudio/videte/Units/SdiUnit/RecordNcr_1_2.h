/* GlobalReplace: RecordNCRNew --> RecordNcr_1_2 */
/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordNcr_1_2.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordNcr_1_2.h $
// CHECKIN:		$Date: 21.12.05 15:28 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 21.12.05 15:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#if !defined(AFX_SDICONTROLRECORD_NCRNEW_H__1BC4B6C3_850A_11D2_B042_004005A1D890__INCLUDED_)
#define AFX_SDICONTROLRECORD_NCRNEW_H__1BC4B6C3_850A_11D2_B042_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecordConfigurable.h"

/////////////////////////////////////////////////////////////////////////////
class CRecordNcr_1_2 : public CRecordConfigurable
{
// Status der gerade bearbeiteten Daten
enum eStatusNCRNew
{
	NCR_NEW_STX,
	NCR_NEW_DATA,
	NCR_NEW_ETX,
	NCR_NEW_CRC,
	NCR_NEW_OK,
};

// Typ der Meldung
enum eMelder
{
	MELDER_DIAGNOSE=1,
	MELDER_START_SAME_TAN=2,
	MELDER_START_NEW_TAN=3,
};

// Construction
public:
	CRecordNcr_1_2(SDIControlType type,
							 int iCom,
							 BOOL bTraceAscii,
							 BOOL bTraceHex,
							 BOOL bTraceEvents);

// Overrides
public:
	virtual	~CRecordNcr_1_2();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:
	virtual void	InitializeResponses();
	virtual BOOL	CheckForNewAlarmData();
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);

// implementation
private:
	void	TransmitDataToSecurity();
	BOOL	CheckForNewTAN();

// variables
private:
	eStatusNCRNew	m_eStatus;			// Status des gerade bearbeiteten Datums
	eMelder			m_eMelder;			// Melder, der den Alarm ausxloesen soll
	CString			m_sPreviousTAN;		// Zuletzt uebermittelte TAN
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONTROLRECORD_NCRNEW_H__1BC4B6C3_850A_11D2_B042_004005A1D890__INCLUDED_)
