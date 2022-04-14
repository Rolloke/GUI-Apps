/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordKebaPasador.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordKebaPasador.cpp $
// CHECKIN:		$Date: 2.01.06 10:05 $
// VERSION:		$Revision: 32 $
// LAST CHANGE:	$Modtime: 2.01.06 10:03 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIUnit.h"
#include "SDIInput.h"
#include "RecordKebaPasador.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CRecordKebaPasador::CRecordKebaPasador(SDIControlType type,
										 int iCom,
										 BOOL bTraceAscii,
										 BOOL bTraceHex,
										 BOOL bTraceEvents)
	: CSDIControlRecord(type, iCom, 0,NULL,bTraceAscii, bTraceHex, bTraceEvents)
{
	m_eStatus = STATUS_DATE_TIME;
	m_TypKarte = TYPE_EC;
	m_bKontoFound = FALSE;

	// Einlesen der cfg-Datei
	CWKDebugOptions debugOptions;
	// check for C:\SDIUnit.cfg first, then in current directory
	BOOL bFoundFile = debugOptions.ReadFromFile(_T("c:\\SDIUnit.cfg"));
	if (bFoundFile==FALSE) {
		bFoundFile = debugOptions.ReadFromFile(_T("SDIUnit.cfg"));
	}

	// Setzen der Variablen
	m_bECCheckTrack2 = debugOptions.GetValue(_T("PasadorECCheckTrack2"),FALSE);
	m_bExtraTrace = debugOptions.GetValue(_T("ExtraDebugTrace"),FALSE);

	WK_TRACE(_T("PasadorECCheckTrack2=%d\n"),m_bECCheckTrack2);
	WK_TRACE(_T("ExtraDebugTrace=%d\n"),m_bExtraTrace);
}
/////////////////////////////////////////////////////////////////////////////
CRecordKebaPasador::~CRecordKebaPasador()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordKebaPasador::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_KEBA_PASADOR) {
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
void CRecordKebaPasador::OnReceivedData(LPBYTE lpB, DWORD dwBufferLen)
{
	DWORD dw = 0;

	while (lpB && dw<dwBufferLen)
	{
		if (m_iMaxBufferLen <= m_sWholeData.GetLength() ) {
			// OOPS da fehlen wohl ein paar (mehrere) Trennzeichen
			// todo erst mal BufferOverrun ausschalten,
			// die Daten kann man allerdings vergessen, also naechstes Paket abwarten
			ClearAllData();
			m_bKontoFound = FALSE;
			m_eStatus = STATUS_DATE_TIME;
		}

		// Solange kein Trennzeichen kommt, in den Buffer sichern
		if (lpB[dw]==ASCII_CR || lpB[dw]==ASCII_LF)
		{
			if ( m_sWholeData.GetLength()==0 && lpB[dw]==ASCII_LF ) {
				// keine Daten bis jetzt
				// einfach weitermachen
			}
			else {
				CheckData();
			}
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
				m_sWholeData += ASCII_SPACE; 
			}
			else {
				m_sWholeData += lpB[dw]; 
			}
		}
		dw++;
	} // while
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPasador::CheckData()
{
	if (m_bExtraTrace) {
		WK_TRACE(_T("COM%i %s Debug: CR_LF Found Len:%i %s\n"),
				GetCOMNumber(),
				CSDIControl::NameOfEnum(m_Type),
				m_sWholeData.GetLength(),
				(LPCTSTR)ConvertToAsciiTrace(m_sWholeData) );
	}

//*********************************************************************************************
	// Wir erwarten das Datum und die Zeit
	if ( STATUS_DATE_TIME == m_eStatus ) {
		CheckDateTime();
	}
//*********************************************************************************************
	if ( STATUS_TRACK_2 == m_eStatus ) {
		CheckTrack2();
	}
//*********************************************************************************************
	if ( STATUS_TRACK_3 == m_eStatus ) {
		CheckTrack3();
	}
//*********************************************************************************************
	// Ist ein kompletter Datensatz abgearbeitet?
	if ( STATUS_OK == m_eStatus ) {
		DataCheckedOK();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPasador::CheckDateTime()
{
	if (m_bExtraTrace) {
		WK_TRACE(_T("COM%i %s Debug: Looking for DateTime\n"),
				GetCOMNumber(),
				CSDIControl::NameOfEnum(m_Type) );
	}
	if ( m_sWholeData.GetLength() == 17 ) {
		if (m_bExtraTrace) {
			WK_TRACE(_T("COM%i %s Debug: DateTime Found\n"),
					GetCOMNumber(),
					CSDIControl::NameOfEnum(m_Type) );
		}
		// Das Datum sind die ersten 8 Zeichen
		m_sSingleData = m_sWholeData.Left(8);
		StoreDate(SDI_DATA_FORMAT_DD_MM_YY);
		// Die Zeit sind die 
		m_sSingleData = m_sWholeData.Right(8);
		StoreTime(SDI_DATA_FORMAT_HH_MM_SS);
		ClearActualData();
		m_eStatus = STATUS_TRACK_2;
	}
	else {
		// Falsche Laenge
		WK_TRACE_ERROR(_T("COM%i %s TimeDate wrong Len:%i\n"),
				GetCOMNumber(),
				CSDIControl::NameOfEnum(m_Type),
				m_sWholeData.GetLength() );
		// Auf Track2 als Anfangsbedingung pruefen
		int iTrack2 = m_sWholeData.Find(_T("Track2: "));
		if ( -1 != iTrack2 ) {
			// Track2 gefunden, dort weitermachen
			if (m_bExtraTrace) {
				WK_TRACE(_T("COM%i %s Debug: Goto Track2\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type) );
			}
			m_eStatus = STATUS_TRACK_2;
		}
		else {
			// Auf Track3 als Anfangsbedingung pruefen
			int iTrack3 = m_sWholeData.Find(_T("Track3: "));
			if ( -1 != iTrack3 ) {
				// Track3 gefunden, dort weitermachen
				if (m_bExtraTrace) {
					WK_TRACE(_T("COM%i %s Debug: Goto Track3\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type) );
				}
				m_eStatus = STATUS_TRACK_3;
			}
			else {
				// Die Daten kann man vergessen,
				// warten wir auf das naechste Datum
				if (m_bExtraTrace) {
					WK_TRACE(_T("COM%i %s Debug: Try next DateTime\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type) );
				}
				ClearAllData();
			}
		}
	}
	m_TypKarte = TYPE_EC;
	m_bKontoFound = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPasador::CheckTrack2()
{
	if (m_bExtraTrace) {
		WK_TRACE(_T("COM%i %s Debug: Looking for Track2 Len:%i %s\n"),
				GetCOMNumber(),
				CSDIControl::NameOfEnum(m_Type),
				m_sWholeData.GetLength(),
				(LPCTSTR)ConvertToAsciiTrace(m_sWholeData) );
	}
	// Wenn ueberhaupt Daten im Buffer sind
	if ( 0< m_sWholeData.GetLength() ) {
		int iTrack2 = m_sWholeData.Find(_T("Track2: "));
		// Wenn kein Track2 gefunden
		if (-1 == iTrack2) {
			// Daten korrupt
			if (m_bExtraTrace) {
				WK_TRACE(_T("COM%i %s Debug: NO Track2 Found\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type) );
			}
			int iTrack3 = m_sWholeData.Find(_T("Track3: "));
			// Wenn kein Track3 gefunden
			if (-1 == iTrack3) {
				// Keine gueltigen Daten mehr
				if (m_bExtraTrace) {
					WK_TRACE(_T("COM%i %s Debug: NO Track3 Found\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type) );
				}
				ClearActualData();
				m_eStatus = STATUS_TRACK_3;
			}
			else {
				// Neue Anfangsbedingung
				if (m_bExtraTrace) {
					WK_TRACE(_T("COM%i %s Debug: Track3 Found Goto\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type) );
				}
				m_sWholeData = m_sWholeData.Mid(iTrack3);
				m_eStatus = STATUS_TRACK_3;
			}
		}
		else {
			// Wir haben eine korrekte Anfangsbedingung
			m_sWholeData = m_sWholeData.Mid(iTrack2+8);
			if (m_bExtraTrace) {
				WK_TRACE(_T("COM%i %s Debug: Track2 Found Len:%i %s\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type),
						m_sWholeData.GetLength(),
						(LPCTSTR)ConvertToAsciiTrace(m_sWholeData) );
			}
			if ( m_sWholeData.GetLength() >= 18) {
				// Haben wir eine Kreditkarte vor uns?
				// Dann Spur2 Kartennummer auswerten
				// Eurocard 16 stellig mit 5 am Anfang
				if ( m_sWholeData.GetAt(17) == 'D'
					 && m_sWholeData.GetAt(1) == '5') {
					if (m_bExtraTrace) {
						WK_TRACE(_T("COM%i %s Found Eurocard\n"),
									GetCOMNumber(),
									CSDIControl::NameOfEnum(m_Type));
					}
					m_TypKarte = TYPE_CREDIT;
					m_sSingleData = m_sWholeData.Mid(1, 16);
					StoreAccount();
				}
				// Visa 16 stellig mit 4 am Anfang
				if ( m_sWholeData.GetAt(17) == _T('D')
					 && m_sWholeData.GetAt(1) == _T('4')) {
					if (m_bExtraTrace) {
						WK_TRACE(_T("COM%i %s Found Visa\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type));
					}
					m_TypKarte = TYPE_CREDIT;
					m_sSingleData = m_sWholeData.Mid(1, 16);
				}
				// AmEx ist nur 15 stellig mit 3 am Anfang
				else if ( m_sWholeData.GetAt(16) == _T('D')
						  && m_sWholeData.GetAt(1) == _T('3') )
				{
					if (m_bExtraTrace) {
						WK_TRACE(_T("COM%i %s Found Amex\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type));
					}
					m_TypKarte = TYPE_CREDIT;
					m_sSingleData = m_sWholeData.Mid(1, 15);
					StoreAccount();
				}
				// oder ist es eine EC-Karte
				else {
					if (m_bExtraTrace) {
						WK_TRACE(_T("COM%i %s Found EC card\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type));
					}
					m_TypKarte = TYPE_EC;

	//*************************************************************************************
					// Soll bei EC-Karten auch Track2 ausgewertet werden
					if ( m_bECCheckTrack2 ) {
						if (m_bExtraTrace) {
							WK_TRACE(_T("COM%i %s Looking for Konto on Track2\n"),
								GetCOMNumber(),
								CSDIControl::NameOfEnum(m_Type));
						}
						// Kontonummer wird vom Trennzeichen (D) begrenzt
						int iTrenner = m_sWholeData.Find(_T("D"));
						// Wenn Trenner OK
						if ( -1 != iTrenner ) {
							// Zwischen Kontonummer und Trennzeichen steht aber
							// die Pruefziffer!
							int iStart = 9;
							int iLen = iTrenner-1-iStart;
							if (m_bExtraTrace) {
								WK_TRACE(_T("COM%i %s Debug: Konto Found Len %i\n"),
										GetCOMNumber(),
										CSDIControl::NameOfEnum(m_Type),
										iLen );
							}
							m_sSingleData = m_sWholeData.Mid(iStart, iLen);
							StoreAccount();
							m_bKontoFound = TRUE;
						}
						else {
							// Fehler, kein Trenner, Daten defekt
							// Versuche es in Spur 3
							if (m_bExtraTrace) {
								WK_TRACE(_T("COM%i %s Debug: No Seperator Found Goto Track3\n"),
										GetCOMNumber(),
										CSDIControl::NameOfEnum(m_Type) );
							}
						}
					}
					//*********************************************************
					// oder soll bei EC-Karten nur Track3 ausgewertet werden
					else {
						// Nichts zu tun, Weitermachen
					}
	//*************************************************************************************
					ClearActualData();
					m_eStatus = STATUS_TRACK_3;
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPasador::CheckTrack3()
{
	if (m_bExtraTrace) {
		WK_TRACE(_T("COM%i %s Debug: Looking for Track3 Len:%i %s\n"),
				GetCOMNumber(),
				CSDIControl::NameOfEnum(m_Type),
				m_sWholeData.GetLength(),
				(LPCTSTR)ConvertToAsciiTrace(m_sWholeData) );
	}
	// Wenn ueberhaupt Daten im Buffer sind
	if ( 0< m_sWholeData.GetLength() ) {
		int iTrack3 = m_sWholeData.Find(_T("Track3: "));
		// Wenn kein Track3
		if (-1 == iTrack3) {
			// keine gueltige Anfangsbedingung
			// Sicherheitshalber Alarm annehmen
			if (m_bExtraTrace) {
				WK_TRACE(_T("COM%i %s Debug: NO Track3 Found Alarm\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type) );
			}
			m_eStatus = STATUS_OK;
		}
		else {
			// Wenn es sich um eine Kreditkarte handelt
			if (TYPE_CREDIT == m_TypKarte ) {
				if (m_bExtraTrace) {
					WK_TRACE(_T("COM%i %s Debug: BLZ Kredit Alarm\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type) );
				}
				m_eStatus = STATUS_OK;
			}
			else {
				// Wenn es sich um eine EC-Karte handelt
				m_sWholeData = m_sWholeData.Mid(iTrack3);
				if (m_bExtraTrace) {
					WK_TRACE(_T("COM%i %s Debug: Looking for BLZ Len:%i %s\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type),
							m_sWholeData.GetLength(),
							(LPCTSTR)ConvertToAsciiTrace(m_sWholeData) );
				}
				// BLZ wird vom Trennzeichen (D) begrenzt
				int iTrenner = m_sWholeData.Find(_T("D"));
				// Wenn kein Trenner
				if (-1 == iTrenner) {
					// keine gueltigen Daten
					// Sicherheitshalber Alarm annehmen
					if (m_bExtraTrace) {
						WK_TRACE(_T("COM%i %s Debug: No Seperator Found Alarm\n"),
								GetCOMNumber(),
								CSDIControl::NameOfEnum(m_Type) );
					}
					m_eStatus = STATUS_OK;
				}
				else {
					// Wenn Trenner
					int iStart = 13;
					int iLen = iTrenner - iStart;
					if (m_bExtraTrace) {
						WK_TRACE(_T("COM%i %s Debug: BLZ Found Len:%i\n"),
								GetCOMNumber(),
								CSDIControl::NameOfEnum(m_Type),
								iLen );
					}
					m_sSingleData = m_sWholeData.Mid(iStart, iLen);
					StoreBankCode();
					// Wenn die BLZ gespeichert wurde,
					// ueberpruefen, ob noch Konto gesichert werden muss
					if ( !m_bKontoFound ) {
						// Das Konto muss noch gespeichert werden
						m_sWholeData = m_sWholeData.Mid(iTrenner+1);
						if (m_bExtraTrace) {
							WK_TRACE(_T("COM%i %s Debug: Looking for Konto Len:%i %s\n"),
									GetCOMNumber(),
									CSDIControl::NameOfEnum(m_Type),
									m_sWholeData.GetLength(),
									(LPCTSTR)ConvertToAsciiTrace(m_sWholeData) );
						}
						// Es wird ggf. durch einen Trenner begrenzt
						iTrenner = m_sWholeData.Find(_T("D"));
						// Wenn Trenner gefunden
						if ( -1 != iTrenner ) {
							// Zwischen Kontonummer und Trennzeichen steht aber
							// die Pruefziffer!
							iStart = 0;
							iLen = iTrenner - 1 - iStart;
						}
						else {
							// Kein Trenner gefunden, kann z.B. bei deutschen
							// Karten vorkommen, daher die restlichen Ziffern
							// als Kontonummer interpretieren
							m_sWholeData = m_sWholeData.SpanIncluding(_T("0123456789"));
							iLen = m_sWholeData.GetLength();
						}
						// Konto auf jeden Fall speichern
						if (m_bExtraTrace) {
							WK_TRACE(_T("COM%i %s Debug: Found Konto Len:%i Alarm\n"),
									GetCOMNumber(),
									CSDIControl::NameOfEnum(m_Type),
									iLen );
						}
						m_sSingleData = m_sWholeData.Mid(0, iLen);
						StoreAccount();
					}
					m_eStatus = STATUS_OK;
				}
			}
		}
	}
	else {
		// keine Daten
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPasador::DataCheckedOK()
{
	TransmitDataToSecurity();
	// Warten wir auf die naechsten Daten
	ClearAllData();
	m_bKontoFound = FALSE;
	m_eStatus = STATUS_DATE_TIME;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordKebaPasador::CheckForNewAlarmData()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPasador::TransmitDataToSecurity()
{
	IndicateAlarm(1L);
}
