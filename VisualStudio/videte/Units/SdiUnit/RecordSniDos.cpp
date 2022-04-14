/* GlobalReplace: CRecordSNI --> CRecordSniDosChase */
/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordSniDos.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordSniDos.cpp $
// CHECKIN:		$Date: 2.01.06 10:05 $
// VERSION:		$Revision: 50 $
// LAST CHANGE:	$Modtime: 2.01.06 10:05 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"

#include "RecordSniDos.h"

static TCHAR szFormat[] = _T("~%02d:");

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CRecordSniDosChase::CRecordSniDosChase(SDIControlType type,
											 int iCom,
											 BOOL bTraceAscii,
											 BOOL bTraceHex,
											 BOOL bTraceEvents)
	: CSDIControlRecord(type, iCom, 0,NULL,bTraceAscii, bTraceHex, bTraceEvents)
{
	m_eStatus = SNI_CHASE_ROUTINE;
	m_iMaxBufferLen = 1024;
}
/////////////////////////////////////////////////////////////////////////////
CRecordSniDosChase::~CRecordSniDosChase()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordSniDosChase::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_SNI_DOS_CHASE) {
		// Basisklassenfunktion aufrufen
		if ( CreateRecord() ) {
			// RS232 oeffnen
			if ( OpenCom(CBR_2400, 8, NOPARITY, ONESTOPBIT) ) {
				bReturn = TRUE;
			}
			else {
				WK_TRACE_ERROR(_T("%s Create OpenCom FAILED\n"),
								CSDIControl::NameOfEnum(m_Type));
			}
		}
		else {
			WK_TRACE_ERROR(_T("%s Create FAILED\n"), CSDIControl::NameOfEnum(m_Type));
		}
	}
	else {
		WK_TRACE_ERROR(_T("Create WRONG Type %s\n"), CSDIControl::NameOfEnum(m_Type));
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosChase::InitializeResponses()
{
	// Status response initialisieren
	// Vorerst wir >Alles ok< vorgespiegelt
	// OOPS todo muss evtl. noch veraendert werden,
	// z.B. um Kameraausfaelle anzuzeigen

	m_byaStatusResponse.RemoveAll();
	m_byaStatusResponse.Add(ASCII_STX);
	m_byaStatusResponse.Add('0');
	m_byaStatusResponse.Add('1');
	m_byaStatusResponse.Add('4');
	m_byaStatusResponse.Add('1');
	m_byaStatusResponse.Add('1');
	m_byaStatusResponse.Add('1');
	m_byaStatusResponse.Add('1');
	m_byaStatusResponse.Add('1');
	m_byaStatusResponse.Add('1');
	m_byaStatusResponse.Add('1');
	m_byaStatusResponse.Add('1');
	m_byaStatusResponse.Add('0');
	m_byaStatusResponse.Add('0');
	m_byaStatusResponse.Add('0');
	m_byaStatusResponse.Add('0');
	m_byaStatusResponse.Add('0');
	m_byaStatusResponse.Add('0');
	m_byaStatusResponse.Add(ASCII_ETX);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordSniDosChase::CheckForNewAlarmData()
{
	return (m_eStatus == SNI_CHASE_ROUTINE);
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosChase::OnReceivedData(LPBYTE lpB, DWORD dwBufferLen)
{
	for (DWORD i=0;i<dwBufferLen;i++)
	{
		OnReceivedData(lpB[i]);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosChase::OnReceivedData(char c)
{
	if (c == ASCII_STX)
	{
		ClearActualData();
	}
	else if (c == ASCII_ETX)
	{
		CheckLine();
	}
	else if (isprint(c))
	{
		m_sWholeData += c;
	}
	else
	{
		WK_TRACE(_T("invalid char in SNI DOS CHASE %02lx\n"),(int)c);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosChase::CheckLine()
{
	CString sTag;
	CString sData;
	
	
	switch (m_eStatus)
	{
	case SNI_CHASE_ROUTINE:
		sData = GetData(SNI_UEBERWACHUNGSMODUS);
		if (sData == _T("1"))
		{
			OnCardIn();
			// Melder "Karte ein" auslösen
			TransmitDataToSecurity(1);
			m_eStatus = SNI_CHASE_CARD;
		}
		else
		{
			// nur Routinemeldung der Uhrzeit des GA
			// ignorieren
			WK_TRACE(_T("ignoring routine line\n"));
			ClearAllData();
		}
		break;
	case SNI_CHASE_CARD:
		sTag.Format(szFormat,(int)SNI_KONTO);
		if (-1!=m_sWholeData.Find(sTag))
		{
			OnPortrait();
			// Melder "Portrait" auslösen und "Karte ein" updaten
			TransmitDataToSecurity(2);
			m_eStatus = SNI_CHASE_PORTRAIT;
		}
		else
		{
			m_eStatus = SNI_CHASE_ROUTINE;
		}
		break;
	case SNI_CHASE_PORTRAIT:
		sTag.Format(szFormat,(int)SNI_BETRAG);
		if (-1!=m_sWholeData.Find(sTag))
		{
			OnHandToMoney();
			m_eStatus = SNI_CHASE_TRANSACTION;
		}
		else
		{
			m_eStatus = SNI_CHASE_ROUTINE;
		}
		break;
	case SNI_CHASE_TRANSACTION:
		sData = GetData(SNI_UEBERWACHUNGSMODUS);
		if (sData == _T("2"))
		{
			TransmitDataToSecurity(3);
			ClearAllData();
		}
		m_eStatus = SNI_CHASE_ROUTINE;
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosChase::OnCardIn()
{
	// GA Nr
	m_sSingleData = GetData(SNI_GA_KENNZEICHEN);
	if (!m_sSingleData.IsEmpty())
	{
		StoreWorkstationID();
	}
	// Datum
	m_sSingleData = GetData(SNI_DATUM);
	if (!m_sSingleData.IsEmpty())
	{
		StoreDate(SDI_DATA_FORMAT_YYMMDD);
	}
	// Zeit
	m_sSingleData = GetData(SNI_ZEIT);
	if (!m_sSingleData.IsEmpty())
	{
		StoreTime(SDI_DATA_FORMAT_HHMMSS);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosChase::OnPortrait()
{
	// GA Nr
	m_sSingleData = GetData(SNI_GA_KENNZEICHEN);
	if (!m_sSingleData.IsEmpty())
	{
		StoreWorkstationID();
	}
	// TA Nr
	m_sSingleData = GetData(SNI_TAN);
	if (!m_sSingleData.IsEmpty())
	{
		StoreTAN();
	}
	// Datum
	m_sSingleData = GetData(SNI_DATUM);
	if (!m_sSingleData.IsEmpty())
	{
		StoreDate(SDI_DATA_FORMAT_YYMMDD);
	}
	// Zeit
	m_sSingleData = GetData(SNI_ZEIT);
	if (!m_sSingleData.IsEmpty())
	{
		StoreTime(SDI_DATA_FORMAT_HHMMSS);
	}
	// BLZ
	m_sSingleData = GetData(SNI_BLZ);
	if (!m_sSingleData.IsEmpty())
	{
		StoreBankCode();
	}
	// Kontonummer
	m_sSingleData = GetData(SNI_KONTO);
	if (!m_sSingleData.IsEmpty())
	{
		StoreAccount();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosChase::OnHandToMoney()
{
	// GA Nr
	m_sSingleData = GetData(SNI_GA_KENNZEICHEN);
	if (!m_sSingleData.IsEmpty())
	{
		StoreWorkstationID();
	}
	// TA Nr
	m_sSingleData = GetData(SNI_TAN);
	if (!m_sSingleData.IsEmpty())
	{
		StoreTAN();
	}
	// Datum
	m_sSingleData = GetData(SNI_DATUM);
	if (!m_sSingleData.IsEmpty())
	{
		StoreDate(SDI_DATA_FORMAT_YYMMDD);
	}
	// Zeit
	m_sSingleData = GetData(SNI_ZEIT);
	if (!m_sSingleData.IsEmpty())
	{
		StoreTime(SDI_DATA_FORMAT_HHMMSS);
	}
	// Betrag
	m_sSingleData = GetData(SNI_BETRAG);
	if (!m_sSingleData.IsEmpty())
	{
		StoreValue();
	}
	// Währung
	m_sSingleData = GetData(SNI_WAEHRUNG);
	if (!m_sSingleData.IsEmpty())
	{
		StoreCurrency();
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CRecordSniDosChase::GetData(eKennungSNI tag)
{
	CString r;
	CString sTag;
	int p;

	sTag.Format(szFormat,(int)tag);
	p = m_sWholeData.Find(sTag);
	if (p!=-1)
	{
		CString t = m_sWholeData.Mid(p+sTag.GetLength());
		for (int i=0;i<t.GetLength();i++)
		{
			if (isalnum(t[i]))
			{
				r += t[i];
			}
			else
			{
				break;
			}
		}
	}

	return r;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosChase::TransmitDataToSecurity(WORD wAlarm)
{
	IndicateAlarm(wAlarm);
}
/////////////////////////////////////////////////////////////////////////////
/* NOT USED
void CRecordSniDosChase::StoreData(eKennungSNI tag)
{
	switch (tag)
	{
		// ------------- Ueberwachungsmodus 1 Byte ---------
		case SNI_UEBERWACHUNGSMODUS:
			// wird ignoriert
			break;
		// ------------- Fehlermeldung ---------
		case SNI_FEHLERMELDUNG:
			// wird ignoriert
			break;
		// ------------------ Datum 6 Byte ----------------------
		case SNI_DATUM:
			if ( m_sSingleData.GetLength() == 6 ) {
				StoreDate(SDI_DATA_FORMAT_YYMMDD);
			}
			else {
			}
			break;

		// -------------------- Zeit 6 Byte -------------------
		case SNI_ZEIT:
			if ( m_sSingleData.GetLength() == 6 ) {
				StoreTime(SDI_DATA_FORMAT_HHMMSS);
			}
			else {
			}
			break;

		// ------------- CSG-Nr. (GA-Kennzeichen) ---------
		case SNI_GA_KENNZEICHEN:
			if ( m_sSingleData.GetLength() > 0 ) {
				StoreWorkstationID();
			}
			else {
			}
			break;

		// ----------------- Transaktionsnummer variabel --------------------
		case SNI_TAN:
			if ( m_sSingleData.GetLength() > 0 ) {
				StoreTAN();
			}
			else {
			}
			break;

		// ------------ Bankleitzahl 8 Byte -----------------------
		case SNI_BLZ:
			if ( m_sSingleData.GetLength() == 8 ) {
				StoreBankCode();
			}
			else {
			}
			break;

		// ----------- Kontonummer 10 Byte -----------------------
		case SNI_KONTO:
			if ( m_sSingleData.GetLength() == 10 ) {
				StoreAccount();
			}
			else {
			}
			break;

		// ------------- Kartenfolgenummer 1 Byte ---------
		case SNI_KARTENFOLGENR:
			// wird ignoriert
			break;

			// ------------- PAN_2 variabel ---------
		case SNI_PAN_2:
			// wird ignoriert
			break;

		// ----------------- Betrag variabel --------------------
		case SNI_BETRAG:
			if ( m_sSingleData.GetLength() > 0 ) {
				StoreValue();
			}
			else {
			}
			break;

			// --------------- Waehrung 3 Byte -------------------------
		case SNI_WAEHRUNG:
			if ( m_sSingleData.GetLength() == 3 ) {
				StoreCurrency();
			}
			else {
			}
			break;

			// ------------- Status Auszahlung 1 Byte ---------
		case SNI_AUSZAHLUNG:
			// Tja, was machen wir damit
			// Soll bei >Kein Geld ausgezahlt< auch kein Alarm gegeben werden?
			// Erst mal doch, also z.Z. ignorieren
			break;

		default: // was kann das sein?
			break;
	} // switch
	ClearActualData();
}
*/
/////////////////////////////////////////////////////////////////////////////
/* NOT USED
BOOL CRecordSniDosChase::IstKennungGueltig(int iKennung)
{
	// Die Kennungen fuer Individuelle Nutzung und Texte werden ignoriert,
	// die folgenden Daten damit verworfen
	if (   iKennung>=0 && iKennung<=1
		|| iKennung>=9 && iKennung<=13
		|| iKennung>=20 && iKennung<=23
		|| iKennung>=30 && iKennung<=32
		)
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}
*/