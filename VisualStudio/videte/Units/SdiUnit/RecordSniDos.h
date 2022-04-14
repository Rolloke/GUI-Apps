/* GlobalReplace: CRecordSNI --> CRecordSniDos */
/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordSniDos.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordSniDos.h $
// CHECKIN:		$Date: 21.12.05 15:29 $
// VERSION:		$Revision: 25 $
// LAST CHANGE:	$Modtime: 21.12.05 15:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#if !defined(AFX_RECORDSNIDOSCHASE_H)
#define AFX_RECORDSNIDOSCHASE_H

#include "SDIControlRecord.h"

// Allgemeine Hinweise
//	Dieser GA liefert 3 Alarme: Karteneingabe, Portrait, Geldausgabe

/////////////////////////////////////////////////////////////////////////////
class CRecordSniDosChase : public CSDIControlRecord
{
	// der aktuelle Zustand
private:
	enum eStatusSNIChase
	{
		SNI_CHASE_ROUTINE,
		SNI_CHASE_CARD,
		SNI_CHASE_PORTRAIT,
		SNI_CHASE_TRANSACTION,
	};

	// Die positiven Werte entsprechen den Datenkennungen des Protokolls
private:
	enum eKennungSNI
	{
		SNI_UNKNOWN=-1,
		SNI_STATUSANFORDERUNG=0,
		SNI_UEBERWACHUNGSMODUS=1,
		SNI_FEHLERMELDUNG=9,
		SNI_DATUM=10,
		SNI_ZEIT=11,
		SNI_GA_KENNZEICHEN=12,
		SNI_TAN=13,
		SNI_BLZ=20,
		SNI_KONTO=21,
		SNI_KARTENFOLGENR=22,
		SNI_PAN_2=23,
		SNI_BETRAG=30,
		SNI_WAEHRUNG=31,
		SNI_AUSZAHLUNG=32,
	};

	// construction/destruction
public:
	CRecordSniDosChase(SDIControlType type,
					   int iCom,
					   BOOL bTraceAscii,
					   BOOL bTraceHex,
					   BOOL bTraceEvents);

	// Overrides
public:
	virtual			~CRecordSniDosChase();
	virtual BOOL	Create(CWK_Profile& wkp, const CString& sSection);
protected:
	virtual void	InitializeResponses();
	virtual void	OnReceivedData(LPBYTE pData, DWORD dwLen);
			void	OnReceivedData(char c);
	virtual BOOL	CheckForNewAlarmData();

	// implementation
private:
	void CheckLine();
	void OnCardIn();
	void OnPortrait();
	void OnHandToMoney();
	CString GetData(eKennungSNI tag);
	void TransmitDataToSecurity(WORD wAlarm);
//	void StoreData(eKennungSNI tag);			NOT USED
//	BOOL IstKennungGueltig(int iKennung);		NOT USED

	// variables
private:
	eStatusSNIChase	m_eStatus;
};
/////////////////////////////////////////////////////////////////////////////
#endif
