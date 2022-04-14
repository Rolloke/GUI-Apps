/* GlobalReplace: CRecordNCR --> CRecordNcr_1_1 */
/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordNcr_1_1.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordNcr_1_1.cpp $
// CHECKIN:		$Date: 2.01.06 10:05 $
// VERSION:		$Revision: 42 $
// LAST CHANGE:	$Modtime: 2.01.06 10:05 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "RecordNcr_1_1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CRecordNcr_1_1::CRecordNcr_1_1(SDIControlType type,
											 int iCom,
											 BOOL bTraceAscii,
											 BOOL bTraceHex,
											 BOOL bTraceEvents)
	: CSDIControlRecord(type, iCom, 0,NULL,bTraceAscii, bTraceHex, bTraceEvents)
{
	m_eStatus = NCR_STX;
	m_eMeldung = MELDUNG_DIAGNOSE;
	m_sPreviousTAN = CString('0', 10);
}
/////////////////////////////////////////////////////////////////////////////
CRecordNcr_1_1::~CRecordNcr_1_1()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordNcr_1_1::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_NCR_VER_1_1) {
		// Basisklassenfunktion aufrufen
		if ( CreateRecord() ) {
			// RS232 oeffnen
			if ( OpenCom(CBR_1200, 7, ODDPARITY, ONESTOPBIT) ) {
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
void CRecordNcr_1_1::InitializeResponses()
{
	BYTE byXORCheckSum = 0;
	int i=0;

	// ACK response initialisieren
	m_byaACK.RemoveAll();
	m_byaACK.Add(ASCII_ACK);
	m_byaACK.Add(ASCII_LF);
// Reicht wenn es einmal berechnet wurde
#if 0
	byXORCheckSum = 12;
#else 
	byXORCheckSum = 0;
	for (i=0 ; i<m_byaACK.GetSize() ; i++) {
		byXORCheckSum = (BYTE)(byXORCheckSum ^ m_byaACK.GetAt(i));
	}
#endif
	m_byaACK.Add(byXORCheckSum);

	// NAK response initialisieren
	m_byaNAK.RemoveAll();
	m_byaNAK.Add(ASCII_NAK);
	m_byaNAK.Add(ASCII_LF);
// Reicht wenn es einmal berechnet wurde
#if 0
	byXORCheckSum = 31;
#else 
	byXORCheckSum = 0;
	for (i=0 ; i<m_byaNAK.GetSize() ; i++) {
		byXORCheckSum = (BYTE)(byXORCheckSum ^ m_byaNAK.GetAt(i));
	}
#endif
	m_byaNAK.Add(byXORCheckSum);

	// Status response initialisieren
	m_byaStatusResponse.RemoveAll();
	m_byaStatusResponse.Add(ASCII_STX);
	for (i=0 ; i<4 ; i++) {
		m_byaStatusResponse.Add(' ');
	}
	m_byaStatusResponse.Add(ASCII_US);
	for (i=0 ; i<6 ; i++) {
		m_byaStatusResponse.Add(' ');
	}
	m_byaStatusResponse.Add(ASCII_US);
	for (i=0 ; i<3 ; i++) {
		m_byaStatusResponse.Add(' ');
	}
	m_byaStatusResponse.Add(ASCII_US);
	for (i=0 ; i<4 ; i++) {
		m_byaStatusResponse.Add(' ');
	}
	m_byaStatusResponse.Add(ASCII_US);
	for (i=0 ; i<5 ; i++) {
		m_byaStatusResponse.Add(' ');
	}
	m_byaStatusResponse.Add(ASCII_US);
	for (i=0 ; i<4 ; i++) {
		m_byaStatusResponse.Add(' ');
	}
	m_byaStatusResponse.Add(ASCII_ETX);
// Reicht wenn es einmal berechnet wurde
#if 0
	byXORCheckSum = 28;
#else 
	byXORCheckSum = 0;
	// Checksumme ohne STX, daher mit 1 anfangen!
	for (i=1 ; i<m_byaStatusResponse.GetSize() ; i++) {
		byXORCheckSum = (BYTE)(byXORCheckSum ^ m_byaStatusResponse.GetAt(i));
	}
#endif
	m_byaStatusResponse.Add(byXORCheckSum);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordNcr_1_1::CheckForNewAlarmData()
{
	// z.Z. wird immer ein neuer Alarm angenommen
	// Bei Diagnose sind eh nur die Daten des Datensatzes relevant
	// Bei Karte einführen muss eine Kto/BLZ mitkommen, sonst sind es eh alte Daten
	// Bei Karte entnehmen kommt laut Protokoll immer Kto/BLZ mit
	BOOL bReturn = TRUE;
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordNcr_1_1::OnReceivedData(LPBYTE lpB, DWORD dwBufferLen)
{
	DWORD dw = 0;

	while (lpB && dw<dwBufferLen) {
		if (m_iLenDataActual > m_iMaxBufferLen) {
			// OOPS da fehlen wohl ein paar (mehrere) Trennzeichen
			// todo erst mal BufferOverrun ausschalten,
			// die Daten kann man allerdings vergessen, also naechstes Paket abwarten
			ClearAllData();
			m_eStatus = NCR_STX;
		}

		// Solange kein Trennzeichen kommt, in den Buffer sichern
		if (lpB[dw] == ASCII_STX || lpB[dw] == ASCII_US || lpB[dw]==ASCII_ETX) {
			if (lpB[dw] == ASCII_STX) {
				ClearAllData();
				m_eMeldung = MELDUNG_STARTKOMMANDO;
				m_eStatus = NCR_FELD_1;
			}
			else if (lpB[dw] == ASCII_US || lpB[dw]==ASCII_ETX) {
				// Die gehoeren mit zur Checksumme
				m_iLenDataActual++;
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);

				switch (m_eStatus)
				{
					// -------------------- Feld 1 -------------------
					// bei Start und Diagnose Uhrzeit 4 Byte, bei Status A 1 Byte
					case NCR_FELD_1:
						// ist es eine Statusabfrage
						if (m_sWholeData.GetLength()==1 && m_sWholeData == "A") {
							m_bStatusRequest = TRUE;
							m_eMeldung = MELDUNG_STATUSABFRAGE;
							m_eStatus = NCR_ETX;
						}
						// oder eine Zeit
						else if (m_sWholeData.GetLength()==4) {
							m_sSingleData = m_sWholeData;
							m_sSingleData.MakeReverse();
							StoreTime(SDI_DATA_FORMAT_HHMM);
							m_eMeldung = MELDUNG_STARTKOMMANDO;
							m_eStatus = NCR_FELD_2;
						}
						// Datenlaenge ist zu groß,
						// gehe davon aus das ein Trennzeichen fehlt oder fehlerhaft
						else if (m_sWholeData.GetLength()==10
								|| m_sWholeData.GetLength()==11)
						{
							m_sSingleData = m_sWholeData.Left(4);
							m_sSingleData.MakeReverse();
							StoreTime(SDI_DATA_FORMAT_HHMM);
							m_sSingleData = m_sWholeData.Right(6);
							m_sSingleData.MakeReverse();
							StoreDate(SDI_DATA_FORMAT_YYMMDD);
							m_eMeldung = MELDUNG_STARTKOMMANDO;
							m_eStatus = NCR_FELD_3;
						}
						// Datenlaenge passt nicht,
						// diese Daten koennen nicht mehr zu geordnet werden
						// sicherheitshalber einen Alarm annehmen und auf Ende warten
						else {
							m_eMeldung = MELDUNG_STARTKOMMANDO;
							m_eStatus = NCR_ETX;
						}
						ClearActualData();
						break;

					// -------------------- Feld 2 -------------------
					// bei Start und Diagnose Datum 6 Byte, bei Status ist das Feld leer
					case NCR_FELD_2:
						// ist es ein Datum
						if (m_sWholeData.GetLength()==6) {
							m_sSingleData = m_sWholeData;
							m_sSingleData.MakeReverse();
							StoreDate(SDI_DATA_FORMAT_YYMMDD);
							m_eStatus = NCR_FELD_3;
						}
						// Datenlaenge ist zu groß,
						// gehe davon aus das ein Trennzeichen fehlt und Startkommando
						// oder das ein Trennzeichen fehlerhaft und Startkommando
						// OOPS koennte aber auch sein, dass ein Trennzeichen fehlt und Diagnose
						else if (m_sWholeData.GetLength()==9
								|| m_sWholeData.GetLength()==10)
						{
							m_sSingleData = m_sWholeData.Left(6);
							m_sSingleData.MakeReverse();
							StoreDate(SDI_DATA_FORMAT_YYMMDD);
							m_sSingleData = m_sWholeData.Right(3);
							m_sSingleData.MakeReverse();
							StoreTAN();
							m_eStatus = NCR_FELD_4;
						}
						// Datenlaenge ist zu groß,
						// gehe davon aus das ein Trennzeichen fehlerhaft und Diagnose 
						else if (m_sWholeData.GetLength()==11) {
							m_sSingleData = m_sWholeData.Left(6);
							m_sSingleData.MakeReverse();
							StoreDate(SDI_DATA_FORMAT_YYMMDD);
							m_sSingleData = m_sWholeData.Right(4);
							m_sSingleData.MakeReverse();
							StoreTAN();
							m_eMeldung = MELDUNG_DIAGNOSE;
							m_eStatus = NCR_FELD_4;
						}
						// Datenlaenge passt nicht,
						// diese Daten koennen nicht mehr zu geordnet werden
						// sicherheitshalber einen Alarm annehmen und auf Ende warten
						else {
							m_eStatus = NCR_ETX;
						}
						ClearActualData();
						break;

					// -------------------- Feld 3 -------------------
					// bei Start und Diagnose TAN 3 oder 4 Byte, bei Status existiert es nicht
					case NCR_FELD_3:
						// ist es eine TAN vom Startkommando
						if (m_sWholeData.GetLength()==3) {
							m_sSingleData = m_sWholeData;
							m_sSingleData.MakeReverse();
							StoreTAN();
							m_eMeldung = MELDUNG_STARTKOMMANDO;
							m_eStatus = NCR_FELD_4;
						}
						// ist es eine TAN von der Diagnose-Startmeldung
						else if (m_sWholeData.GetLength()==4) {
							m_sSingleData = m_sWholeData;
							m_sSingleData.MakeReverse();
							StoreTAN();
							m_eMeldung = MELDUNG_DIAGNOSE;
							m_eStatus = NCR_FELD_4;
						}
						// Datenlaenge ist zu groß,
						// gehe davon aus das ein Trennzeichen fehlt und Terminal folgt
						// oder gehe davon aus das ein Trennzeichen fehlerhaft und Terminal folgt
						// Der Fall, das zwei aufeinanderfolgende Trennzeichen fehlen und BLZ folgt,
						// ist eher unwahrscheinlich
						else if (m_sWholeData.GetLength()==10
								|| m_sWholeData.GetLength()==11)
						{
							m_sSingleData = m_sWholeData.Left(4);
							m_sSingleData.MakeReverse();
							StoreTAN();
							m_sSingleData = m_sWholeData.Right(6);
							m_sSingleData.MakeReverse();
							StoreWorkstationID();
							m_eMeldung = MELDUNG_DIAGNOSE;
							m_eStatus = NCR_ETX;
						}
						// Datenlaenge ist zu groß,
						// gehe davon aus das ein Trennzeichen fehlt,
						// ein Trennzeichen fehlerhaft und BLZ folgt
						// oder gehe davon aus das zwei Trennzeichen fehlerhaft
						// und BLZ folgt
						else if (m_sWholeData.GetLength()==12
								|| m_sWholeData.GetLength()==13) {
							m_sSingleData = m_sWholeData.Left(3);
							m_sSingleData.MakeReverse();
							StoreTAN();
							m_sSingleData = m_sWholeData.Right(8);
							m_sSingleData.MakeReverse();
							StoreBankCode();
							m_eMeldung = MELDUNG_STARTKOMMANDO;
							m_eStatus = NCR_FELD_6;
						}
						// Datenlaenge passt nicht,
						// diese Daten koennen nicht mehr zu geordnet werden
						// sicherheitshalber einen Alarm annehmen und auf Ende warten
						else {
							m_eMeldung = MELDUNG_STARTKOMMANDO;
							m_eStatus = NCR_ETX;
						}
						ClearActualData();
						break;

					// -------------------- Feld 4 -------------------
					// bei Start leer, bei Diagnose Terminal 6 Byte, bei Status existiert es nicht
					case NCR_FELD_4:
						// ist es das Leerfeld vom Startkommando
						if (m_sWholeData.GetLength()==0) {
							m_eMeldung = MELDUNG_STARTKOMMANDO;
							m_eStatus = NCR_FELD_5;
						}
						// ist es die Terminalnummer von der Diagnose-Meldung
						else if (m_sWholeData.GetLength()==6 && lpB[dw]==ASCII_ETX) {
							m_sSingleData = m_sWholeData;
							m_sSingleData.MakeReverse();
							StoreWorkstationID();
							m_eMeldung = MELDUNG_DIAGNOSE;
							m_eStatus = NCR_CRC;
						}
						// ist es vielleicht schon die Bankleitzahl Feld 5
						else if (m_sWholeData.GetLength()==8) {
							m_sSingleData = m_sWholeData;
							m_sSingleData.MakeReverse();
							StoreBankCode();
							m_eMeldung = MELDUNG_STARTKOMMANDO;
							m_eStatus = NCR_FELD_6;
						}
						// Datenlaenge passt nicht,
						// diese Daten koennen nicht mehr zu geordnet werden
						// sicherheitshalber einen Alarm annehmen und auf Ende warten
						else {
							m_eMeldung = MELDUNG_STARTKOMMANDO;
							m_eStatus = NCR_ETX;
						}
						ClearActualData();
						break;

					// -------------------- Feld 5 -------------------
					// bei Start BLZ 8 Byte, bei Status und Diagnose existiert es nicht
					case NCR_FELD_5:
						// ist es die Bankleitzahl
						if (m_sWholeData.GetLength()==8) {
							m_sSingleData = m_sWholeData;
							m_sSingleData.MakeReverse();
							StoreBankCode();
							m_eMeldung = MELDUNG_STARTKOMMANDO;
							m_eStatus = NCR_FELD_6;
						}
						// Datenlaenge passt nicht,
						// diese Daten koennen nicht mehr zu geordnet werden
						// sicherheitshalber einen Alarm annehmen und auf Ende warten
						else {
							m_eMeldung = MELDUNG_STARTKOMMANDO;
							m_eStatus = NCR_ETX;
						}
						ClearActualData();
						break;

					// -------------------- Feld 6 -------------------
					// bei Start Konto 10 Byte, bei Status und Diagnose existiert es nicht
					case NCR_FELD_6:
						if (lpB[dw]==ASCII_ETX) {
							if (m_sWholeData.GetLength()==10) {
								m_sSingleData = m_sWholeData;
								m_sSingleData.MakeReverse();
								StoreAccount();
								m_eMeldung = MELDUNG_STARTKOMMANDO;
								m_eStatus = NCR_CRC;
							}
						}
						break;

					// ------------ ETX -----------------------
					case NCR_ETX:
						if (lpB[dw]==ASCII_ETX)
						{
							m_eStatus = NCR_CRC;
						}
						break;

					// ----------- Checksumme -----------------------
					case NCR_CRC:
						break;

					default: // OOPS was soll das sein
						WK_TRACE_ERROR(_T("%s COM %dw WRONG Status\n"),
								CSDIControl::NameOfEnum(m_Type), GetCOMNumber() );
						break;
				} // switch
			} // else if Trennzeichen oder ETX
		}
		else {
			// Alle anderen Zeichen werden zwischengespeichert
			// ausser es wird die CRC erwartet
			if (NCR_CRC != m_eStatus) {
				m_iLenDataActual++;
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);
				// Eine ASCII_NUL NIEMALS in den Buffer sichern!
				// Erstens hat sie hier nichts zu suchen,
				// zweitens wuerde sie die CString-Operationen voellig durcheinander brimngen!
				// Wird hier durch ASCII_SPACE ersetzt
				if (lpB[dw]==ASCII_NUL) {
					CString sMsg;
					sMsg.Format( _T("ERROR Ascii NULL|COM%i"), GetCOMNumber() );
					WK_STAT_PEAK(_T("AsciiNull"), 1, sMsg);
					m_sWholeData += ASCII_SPACE;
				}
				else {
					m_sWholeData += lpB[dw];
				}
			}
			else {
				if (m_byXORCheckSum != lpB[dw]) {
					WK_TRACE(_T(" Typ %s COM %d Prüfsumme fehlerhaft %d!=%d\n"),
								CSDIControl::NameOfEnum(m_Type),
								GetCOMNumber(),
								m_byXORCheckSum,
								lpB[dw]);

					// Falsche Checksumme, Daten nochmal anfordern
					SendNAK();
				}
				else {
					// Daten Ok
					SendACK();
				}
				// OOPS z.Z. wird immer ein Alarm angenommen
				m_eStatus = NCR_OK;			
				// STX wird auch als CheckSum angesehen, der Zaehler wird allgemein erhoeht
				// die Laenge der uebertragenen Daten wird oben abgeprueft,
				// sodass nicht ueber den BufferRand hinaus gelesen wird
				if (m_eStatus == NCR_OK) {
					TransmitDataToSecurity();
					if (m_bStatusRequest) {
						RespondStatusRequest();
					}
					ClearAllData();
					m_eStatus = NCR_STX;
				}
			}
		}
		dw++;
	} // while
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordNcr_1_1::CheckForNewTAN()
{
	BOOL bReturn = FALSE;
	CIPCField* pField = m_DataFieldArrayActual.GetCIPCField(CIPCField::m_sfTaNr);
	if (pField) {
		CString sActualTAN = pField->GetValue();
		CString sNullen(_T('0'), 10);
		if ( sActualTAN == sNullen.Right(sActualTAN.GetLength())
			 || sActualTAN == m_sPreviousTAN.Right(sActualTAN.GetLength())
			 )
		{
			// TAN ist 0 bzw. hat sich nicht geaendert
		}
		else {
			// TAN hat sich geaendert
			bReturn = TRUE;
			m_sPreviousTAN = sActualTAN;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordNcr_1_1::TransmitDataToSecurity()
{
	// Es werden DIAGNOSE und STARTKOMMANDO unterschieden
	// Unter STARTKOMMANDO werden bei Karteneingabe und Kartenentnahme
	// dieselben Strings gesendet.
	// Anhand der TAN (gleich oder geaendert) wird weiter differenziert.
	WORD wAlarm = 0;

	switch (m_eMeldung) {
		case MELDUNG_DIAGNOSE:
			wAlarm = 1;
			break;
		case MELDUNG_STARTKOMMANDO:
			wAlarm = 2;
			if ( CheckForNewTAN() ) {
				wAlarm = 3;
			}
			break;
		default: // Kann gar nicht sein
			break;
	}

	if (wAlarm) {
		IndicateAlarm(wAlarm);
	}
}
