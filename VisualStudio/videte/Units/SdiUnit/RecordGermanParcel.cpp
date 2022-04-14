/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordGermanParcel.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordGermanParcel.cpp $
// CHECKIN:		$Date: 2.01.06 10:05 $
// VERSION:		$Revision: 29 $
// LAST CHANGE:	$Modtime: 2.01.06 10:03 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIUnit.h"
#include "SDIInput.h"
#include "RecordGermanParcel.h"

#define PARCEL_LEN			11
#define BARCODE_LEN			PARCEL_LEN + 1

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CRecordGermanParcel::CRecordGermanParcel(SDIControlType type,
											int iCom,
											BOOL bTraceAscii,
											BOOL bTraceHex,
											BOOL bTraceEvents)
	: CSDIControlRecord(type, iCom, 0,NULL,bTraceAscii, bTraceHex, bTraceEvents)
{
	// Die Paketnummern werden mit Pruefziffer angeliefert
	// Die Paketnummer wird mit 31313131313 gewichtet und die Quersumme berechnet
	// Die Quersumme wird um 1 erhoeht
	// Die letzte Ziffer der Quersumme (Modulo10) wird von 10 abgezogen
	// Das Ergebnis ergibt die Pruefziffer
	m_bInclusiveCheckDigit = TRUE;
	m_iWeight = 3;
	m_iIncreaseSum = 1;
	m_iModulo = 10;
}
/////////////////////////////////////////////////////////////////////////////
CRecordGermanParcel::~CRecordGermanParcel()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordGermanParcel::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_GERMAN_PARCEL) {
		// Basisklassenfunktion aufrufen
		if ( CreateRecord() ) {
			// RS232 oeffnen
			if ( OpenCom(CBR_9600, 8, NOPARITY, ONESTOPBIT) ) {
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
void CRecordGermanParcel::OnReceivedData(LPBYTE lpB, DWORD dwBufferLen)
{
	DWORD dw = 0;
	while (lpB && dw<dwBufferLen) {
		if (m_sSingleData.GetLength() == m_iMaxBufferLen) {
			// OOPS da fehlen wohl ein paar (mehrere) Trennzeichen
			// todo erst mal BufferOverrun ausschalten,
			// die Daten kann man allerdings vergessen, also naechstes Paket abwarten
			// Fehler, zu viele Zeichen
			CString sMsg;
			WK_TRACE_ERROR(_T("COM%i %s Daten %s Len %i Buffer-Overrun, no CR\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type),
						(LPCTSTR)m_sSingleData,
						m_sSingleData.GetLength());
			sMsg.Format( _T("Buffer-Overrun|COM%i"), GetCOMNumber() );
			WK_STAT_PEAK(_T("BufferOverrun"), 1, sMsg);
			// Vor dem Loeschen des Buffers die letzten (BARCODE_LEN) Zeichen sichern,
			// falls sie doch noch zu einem korrekten Datensatz gehoeren
			CString sSave = m_sSingleData.Right(BARCODE_LEN);
			ClearAllData();
			m_sSingleData = sSave;
		}

		// Wenn ein Trennzeichen kommt, ist ein Datensatz komplett
		if (lpB[dw]==ASCII_CR) {
			CheckData();
			// Immer Alarm liefern, auch wenn die Daten korrupt sind,
			// um wenigstens Bilder aufzunehmen
			StoreParcel();
			TransmitDataToSecurity();
			ClearAllData();
		}
		else {
			// Sonst das Zeichen in den Buffer sichern, ABER:
			// Eine ASCII_NUL NIEMALS in den Buffer sichern!
			// Erstens hat sie hier nichts zu suchen,
			// zweitens wuerde sie die CString-Operationen voellig durcheinander brimngen!
			if (lpB[dw]==ASCII_NUL) {
				CString sMsg;
				sMsg.Format( _T("ERROR Ascii NULL|COM%i"), GetCOMNumber() );
				WK_STAT_PEAK(_T("AsciiNull"), 1, sMsg);
				// Wird hier durch Leerzeichen ersetzt
				m_sSingleData += ASCII_SPACE; 
			}
			else {
				m_sSingleData += lpB[dw]; 
			}
		}
		dw++;
	} // while
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordGermanParcel::CheckForNewAlarmData()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordGermanParcel::CheckData()
{
	// Einmal Daten komplett ausgeben
/*	WK_TRACE(_T("COM%i %s Daten %s Len %i\n"),
				GetCOMNumber(),
				NameOfEnum(m_Type),
				(LPCSTR)m_sSingleData,
				m_sSingleData.GetLength() );
*/
	// Die Paketnummer hat eine bestimmte Laenge, nur die ist gueltig
	int iValidLen = PARCEL_LEN;
	if (m_bInclusiveCheckDigit) {
		iValidLen = BARCODE_LEN;
	}
	if (m_sSingleData.GetLength() == iValidLen) {
		// Ggf. Pruefziffer kontrollieren
		if (m_bInclusiveCheckDigit) {
			// Prüfziffer extrahieren
			const int iCheckDigitInput = _ttoi( m_sSingleData.Right(1) );
			// Paketnummer extrahieren
			m_sSingleData = m_sSingleData.Left( m_sSingleData.GetLength()-1 );
			// wir drehen die Paketnummer um,
			// dann koennen wir die Wichtung immer mit der richtigen Wichtungsziffer beginnen
			CString sParcelReverse = m_sSingleData;
			sParcelReverse.MakeReverse();
			// Prüfsumme berechnen
			int iSum = 0;
			int iDigit = 0;
			int iWeight = m_iWeight;
			CString sChar(' ');
			for (int i=0 ; i<sParcelReverse.GetLength() ; i++) {
				sChar = sParcelReverse.GetAt(i);
				iDigit = _ttoi(sChar);
				iSum += iDigit * iWeight;
				iWeight = (iWeight == m_iWeight) ? 1 : m_iWeight;
			}
			// Ggf. Summe erhoehen
			iSum += m_iIncreaseSum;
			// Pruefziffer berechnen
			int iRemains = iSum % m_iModulo;
			int iCheckDigit = (10-iRemains) % 10;

			// Pruefziffer vergleichen
			if ( iCheckDigitInput == iCheckDigit ) {
				// Pruefziffer korrekt, Daten OK
			}
			else {
				// Fehler, Prüfziffer stimmt nicht ueberein
				WK_TRACE_ERROR(_T("COM%i %s Daten %s Len %i Daten/Prüfziffer falsch %i != %i berechnet\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type),
							(LPCTSTR)m_sSingleData,
							m_sSingleData.GetLength(),
							iCheckDigitInput,
							iCheckDigit);
			}
		}
		else {
			// Keine Prüfziffer, muessen die Daten ungeprueft uebernehmen
		}
	}
	else if (m_sSingleData.GetLength() > iValidLen) {
			// Fehler, zu viele Zeichen
			WK_TRACE_ERROR(_T("COM%i %s Daten %s Len %i statt %i zu lang\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type),
						(LPCTSTR)m_sSingleData,
						m_sSingleData.GetLength(),
						iValidLen);
	}
	else {
		// Fehler, zu wenig Zeichen
		WK_TRACE_ERROR(_T("COM%i %s Daten %s Len %i statt %i zu kurz\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type),
						(LPCTSTR)m_sSingleData,
						m_sSingleData.GetLength(),
						iValidLen);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordGermanParcel::StoreParcel()
{
	// Die Paketnummer hat eine bestimmte Laenge, nur die ist gueltig
	int iValidLen = PARCEL_LEN;
	if (m_bInclusiveCheckDigit) {
		iValidLen = BARCODE_LEN;
	}
	if (m_sSingleData.GetLength() > iValidLen) {
		// Empfangene Daten sind groesser als erlaubt
		// Sicherheitshalber die letzten Zeichen speichern
		WK_TRACE_ERROR(_T("%s Account len %i>%i, last valid stored\n"),
						CSDIControl::NameOfEnum(m_Type),
						m_sSingleData.GetLength(),
						iValidLen);
		m_sSingleData.Right(iValidLen);
	}
	StoreAccount();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordGermanParcel::TransmitDataToSecurity()
{
	IndicateAlarm(1);
}
