/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordEngelMultipass.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordEngelMultipass.cpp $
// CHECKIN:		$Date: 2.01.06 10:05 $
// VERSION:		$Revision: 35 $
// LAST CHANGE:	$Modtime: 2.01.06 10:03 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "wkclasses\wk_ascii.h"
#include "SDIUnit.h"
#include "SDIInput.h"
#include "RecordEngelMultipass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CRecordEngelMultipass::CRecordEngelMultipass(SDIControlType type,
																   int iCom,
																   BOOL bTraceAscii,
																   BOOL bTraceHex,
																   BOOL bTraceEvents)
	: CSDIControlRecord(type, iCom, 0,NULL,bTraceAscii, bTraceHex, bTraceEvents)
{
	m_eTyp = ENGEL_TYP_UNKNOWN;
	m_eStatus = ENGEL_KENNUNG;
}
/////////////////////////////////////////////////////////////////////////////
CRecordEngelMultipass::~CRecordEngelMultipass()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordEngelMultipass::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_ENGEL_MULTIPASS) {
		// Basisklassenfunktion aufrufen
		if ( CreateRecord() ) {
			// RS232 oeffnen
			if ( OpenCom(CBR_4800, 8, NOPARITY, ONESTOPBIT) ) {
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
void CRecordEngelMultipass::OnReceivedData(LPBYTE lpB, DWORD dwBufferLen)
{
	DWORD dw = 0;

	while (lpB && dw<dwBufferLen) {
		if (m_sWholeData.GetLength() >= m_iMaxBufferLen) {
			// OOPS da fehlen wohl ein paar (mehrere) Trennzeichen
			// todo erst mal BufferOverrun ausschalten,
			// die Daten kann man allerdings vergessen, also naechstes Paket abwarten
			ClearActualData();
			m_eStatus = ENGEL_KENNUNG;
		}
		// Solange kein Trennzeichen kommt, in den Buffer sichern
		if (lpB[dw] == ASCII_SPACE || lpB[dw]==ASCII_CR) {
			switch (m_eStatus)
			{
				case ENGEL_KENNUNG:
					OnStatusKennung();
					break;
				case ENGEL_DATUM:
					OnStatusDatum();
					break;
				case ENGEL_ZEIT:
					OnStatusZeit();
					break;
				case ENGEL_BLZ:
					OnStatusBLZ();
					break;
				case ENGEL_KONTO:
					OnStatusKonto();
					break;
				case ENGEL_SONDER:
					OnStatusSonder();
					break;
				case ENGEL_KREDIT:
					OnStatusKredit();
					break;
				default: // OOPS was soll das sein
					break;
			} // switch
		}
		else {
			// Eine ASCII_NUL NIEMALS in den Buffer sichern!
			// Erstens hat sie hier nichts zu suchen,
			// zweitens wuerde sie die CString-Operationen voellig durcheinander brimngen!
			// Wird hier durch ASCII_SPACE ersetzt
			if (lpB[dw]==ASCII_NUL) {
				CString sMsg;
				sMsg.Format( _T("ERROR Ascii NULL|COM%i"), GetCOMNumber() );
				WK_STAT_PEAK(_T("Ascii-Null"), 1, sMsg);
				m_sWholeData += ASCII_SPACE;
			}
			else {
				m_sWholeData += lpB[dw];
			}
		}
		// Wenn ein kompletter Datensatz fertig ist
		if (m_eStatus == ENGEL_OK) {
			TransmitDataToSecurity();
			ClearAllData();
			m_eStatus = ENGEL_KENNUNG;			
		}
		dw++;
	} // while
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordEngelMultipass::CheckForNewAlarmData()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordEngelMultipass::OnStatusKennung()
{
	// KENNUNG	1 Byte
	// Erst mal gucken, ob eine richtige Kennung kam
	m_eTyp = ENGEL_TYP_UNKNOWN;
	if (m_sWholeData.GetLength() > 0) {
		if (m_sWholeData.GetAt(0) == 'L') {
			m_eTyp = ENGEL_TYP_L;
			m_eStatus = ENGEL_DATUM;
		}
		else if (m_sWholeData.GetAt(0) == 'E') {
			m_eTyp = ENGEL_TYP_E;
			m_eStatus = ENGEL_DATUM;
		}
		else {
			// OOPS was dann
			// Daten verwerfen und weiterhin auf naechstes Kommando warten
		}
	}
	// wenn Kennung erkannt, dann weiter bearbeiten
	if (m_eTyp != ENGEL_TYP_UNKNOWN) {
		// ist es nur die Kennung
		if (m_sWholeData.GetLength() == 1) {
			// OK, weitermachen
		}
		// Datenlaenge ist zu groﬂ,
		// gehe davon aus das ein Trennzeichen fehlt
		else if (m_sWholeData.GetLength() == 9) {
			m_sSingleData = m_sWholeData.Right(8);
			StoreDate(SDI_DATA_FORMAT_DD_MM_YY);
			IncreaseStatus();
		}
		// Datenlaenge ist zu groﬂ,
		// gehe davon aus das ein Trennzeichen fehlerhaft
		else if (m_sWholeData.GetLength() == 10) {
			m_sSingleData = m_sWholeData.Right(8);
			StoreDate(SDI_DATA_FORMAT_DD_MM_YY);
			IncreaseStatus();
		}
		// Datenlaenge ist viel zu groﬂ oder Daten fehlen,
		// diese Daten koennen nicht mehr zugeordnet werden
		// sicherheitshalber einen Alarm annehmen
		else {
			m_eStatus = ENGEL_OK;
		}
		ClearActualData();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordEngelMultipass::OnStatusDatum()
{
	// Datum 8 Byte
	// ist es nur das Datum
	if (m_sWholeData.GetLength() == 8) {
		m_sSingleData = m_sWholeData;
		StoreDate(SDI_DATA_FORMAT_DD_MM_YY);
		IncreaseStatus();
	}
	// Datenlaenge ist zu groﬂ,
	// gehe davon aus das ein Trennzeichen fehlt
	else if (m_sWholeData.GetLength() == 16) {
		m_sSingleData = m_sWholeData.Left(8);
		StoreDate(SDI_DATA_FORMAT_DD_MM_YY);
		IncreaseStatus();
		m_sSingleData = m_sWholeData.Right(8);
		StoreTime(SDI_DATA_FORMAT_HH_MM_SS);
		IncreaseStatus();
	}
	// Datenlaenge ist zu groﬂ,
	// gehe davon aus das ein Trennzeichen fehlerhaft
	else if (m_sWholeData.GetLength() == 17) {
		m_sSingleData = m_sWholeData.Left(8);
		StoreDate(SDI_DATA_FORMAT_DD_MM_YY);
		IncreaseStatus();
		m_sSingleData = m_sWholeData.Right(8);
		StoreTime(SDI_DATA_FORMAT_HH_MM_SS);
		IncreaseStatus();
	}
	// Datenlaenge ist viel zu groﬂ oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = ENGEL_OK;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordEngelMultipass::OnStatusZeit()
{
	// Zeit 8 Byte
	// ist es nur die Zeit
	if (m_sWholeData.GetLength() == 8) {
		m_sSingleData = m_sWholeData;
		StoreTime(SDI_DATA_FORMAT_HH_MM_SS);
		IncreaseStatus();
	}
	// Datenlaenge ist zu groﬂ,
	// gehe davon aus das ein Trennzeichen fehlt und Sonderschluessel folgt 
	else if (m_sWholeData.GetLength() == 12) {
		m_sSingleData = m_sWholeData.Left(8);
		StoreTime(SDI_DATA_FORMAT_HH_MM_SS);
		IncreaseStatus();
		m_sSingleData = m_sWholeData.Right(4);
		StoreBankCode();
		IncreaseStatus();
	}
	// Datenlaenge ist zu groﬂ,
	// gehe davon aus das ein Trennzeichen fehlerhaft und Sonderschluesel folgt 
	else if (m_sWholeData.GetLength() == 13) {
		m_sSingleData = m_sWholeData.Left(8);
		StoreTime(SDI_DATA_FORMAT_HH_MM_SS);
		IncreaseStatus();
		m_sSingleData = m_sWholeData.Right(4);
		StoreBankCode();
		IncreaseStatus();
	}
	// Datenlaenge ist zu groﬂ,
	// gehe davon aus das ein Trennzeichen fehlt und BLZ folgt 
	else if (m_sWholeData.GetLength() == 16) {
		m_sSingleData = m_sWholeData.Left(8);
		StoreTime(SDI_DATA_FORMAT_HH_MM_SS);
		IncreaseStatus();
		m_sSingleData = m_sWholeData.Right(8);
		StoreBankCode();
		IncreaseStatus();
	}
	// Datenlaenge ist zu groﬂ,
	// gehe davon aus das ein Trennzeichen fehlerhaft und BLZ folgt 
	else if (m_sWholeData.GetLength() == 17) {
		m_sSingleData = m_sWholeData.Left(8);
		StoreTime(SDI_DATA_FORMAT_HH_MM_SS);
		IncreaseStatus();
		m_sSingleData = m_sWholeData.Right(8);
		StoreBankCode();
		IncreaseStatus();
	}
	// Datenlaenge ist viel zu groﬂ oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = ENGEL_OK;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordEngelMultipass::OnStatusBLZ()
{
	// BLZ 8 Byte
	// ist es nur die BLZ
	if (m_sWholeData.GetLength() == 8) {
		m_sSingleData = m_sWholeData;
		StoreBankCode();
		IncreaseStatus();
	}
	// Datenlaenge ist zu groﬂ,
	// gehe davon aus das ein Trennzeichen fehlt und Konto folgt
	else if (m_sWholeData.GetLength() == 18) {
		m_sSingleData = m_sWholeData.Right(8);
		StoreBankCode();
		IncreaseStatus();
		m_sSingleData = m_sWholeData.Right(10);
		StoreAccount();
		IncreaseStatus();
	}
	// Datenlaenge ist zu groﬂ,
	// gehe davon aus das ein Trennzeichen fehlerhaft und Konto folgt
	else if (m_sWholeData.GetLength() == 19) {
		m_sSingleData = m_sWholeData.Right(8);
		StoreBankCode();
		IncreaseStatus();
		m_sSingleData = m_sWholeData.Right(10);
		StoreAccount();
		IncreaseStatus();
	}
	// Datenlaenge ist viel zu groﬂ oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = ENGEL_OK;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordEngelMultipass::OnStatusKonto()
{
	// Konto 10 Byte
	// ist es das Konto
	if (m_sWholeData.GetLength() == 10) {
		m_sSingleData = m_sWholeData;
		StoreAccount();
		IncreaseStatus();;
	}
	// Datenlaenge ist viel zu groﬂ oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = ENGEL_OK;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordEngelMultipass::OnStatusSonder()
{
	// Sonder-Schluessel 4 Byte
	// ist es nur der Sonderschluessel
	if (m_sWholeData.GetLength() == 4) {
		m_sSingleData = m_sWholeData;
		StoreBankCode();
		IncreaseStatus();;
	}
	// Datenlaenge ist zu groﬂ
	// speichere nur Sonderschluessel
	// weitere Daten koennen nicht zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_sSingleData = m_sWholeData.Left(4);
		StoreBankCode();
		m_eStatus = ENGEL_OK;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordEngelMultipass::OnStatusKredit()
{
	// Kreditkartennummer variabel, max. 14 Byte
	// ist es die Kreditkarte
	if (m_sWholeData.GetLength() <= 14) {
		m_sSingleData = m_sWholeData;
		StoreAccount();
		IncreaseStatus();
	}
		// Datenlaenge ist viel zu groﬂ
		// diese Daten koennen nicht mehr zugeordnet werden
		// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = ENGEL_OK;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordEngelMultipass::IncreaseStatus()
{
	switch (m_eStatus) {
		case ENGEL_KENNUNG:
			m_eStatus = ENGEL_DATUM;
			break;
		case ENGEL_DATUM:
			m_eStatus = ENGEL_ZEIT;
			break;
		case ENGEL_ZEIT:
			if (m_eTyp == ENGEL_TYP_L) {
				m_eStatus = ENGEL_BLZ;
			}
			else if (m_eTyp == ENGEL_TYP_E) {
				m_eStatus = ENGEL_SONDER;
			}
			else {
				// OOPS darf gar nicht sein, sicherheitshalber neue Kennung abwarten
				m_eStatus = ENGEL_KENNUNG;
			}
			break;
		case ENGEL_BLZ:
			m_eStatus = ENGEL_KONTO;
			break;
		case ENGEL_KONTO:
			m_eStatus = ENGEL_OK;
			break;
		case ENGEL_SONDER:
			m_eStatus = ENGEL_KREDIT;
			break;
		case ENGEL_KREDIT:
			m_eStatus = ENGEL_OK;
			break;
		case ENGEL_OK:
			m_eStatus = ENGEL_KENNUNG;
			break;
		default:
			// OOPS darf gar nicht sein, sicherheitshalber neue Kennung abwarten
			m_eStatus = ENGEL_KENNUNG;
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordEngelMultipass::TransmitDataToSecurity()
{
	IndicateAlarm(1);
}
