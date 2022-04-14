/* GlobalReplace: RecordNCRNew --> RecordNcr_1_2 */
/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordNcr_1_2.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordNcr_1_2.cpp $
// CHECKIN:		$Date: 21.12.05 15:28 $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 21.12.05 15:27 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIUnit.h"
#include "SDIInput.h"
#include "wkclasses\ComParameters.h"
#include "SDIControlRecord.h"
#include "RecordConfigurable.h"
#include "RecordNcr_1_2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecordNcr_1_2
CRecordNcr_1_2::CRecordNcr_1_2(SDIControlType type,
												   int iCom,
												   BOOL bTraceAscii,
												   BOOL bTraceHex,
												   BOOL bTraceEvents)
	: CRecordConfigurable(type, iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
	m_eStatus = NCR_NEW_STX;
	m_eMelder = MELDER_DIAGNOSE;
	m_sPreviousTAN = CString('0', 10);
}
/////////////////////////////////////////////////////////////////////////////
CRecordNcr_1_2::~CRecordNcr_1_2()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordNcr_1_2::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_NCR_VER_1_2) {
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
void CRecordNcr_1_2::InitializeResponses()
{
	BYTE byXORCheckSum = 0;
	int i=0;

	// ACK response initialisieren
	m_byaACK.RemoveAll();
	m_byaACK.Add(ASCII_ACK);
	m_byaACK.Add(ASCII_LF);

	// NAK response initialisieren
	m_byaNAK.RemoveAll();
	m_byaNAK.Add(ASCII_NAK);
	m_byaNAK.Add(ASCII_LF);

	// KameraStatus response initialisieren
	// OOPS ToDo erstmal Dummy schicken, Daten werden vom GA eh nicht ueberprueft
	CWKTime wkTime;
	CWK_String sBuffer;
	LPCSTR szBuffer;

	m_byaStatusResponse.RemoveAll();
	m_byaStatusResponse.Add(ASCII_STX);
	sBuffer = wkTime.GetTimeReversed_MMHH();
	szBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(szBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = wkTime.GetDateReversed_DDMMYY();
	szBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(szBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = "111";	// Transaktionsnummer 3 Byte
	szBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(szBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = "2222";	// Terminalnummer 4 Byte
	szBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(szBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = "99999";	// Anzahl der Bilder 5 Byte
	szBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(szBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = "    ";	// Frei 4 Byte
	szBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(szBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_ETX);

	byXORCheckSum = 0;
	// Checksumme ohne STX, daher mit 1 anfangen!
	for (i=1 ; i<m_byaStatusResponse.GetSize() ; i++) {
		byXORCheckSum = (BYTE)(byXORCheckSum ^ m_byaStatusResponse.GetAt(i));
	}

	m_byaStatusResponse.Add(byXORCheckSum);
	m_byaStatusResponse.Add(ASCII_LF);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordNcr_1_2::CheckForNewAlarmData()
{
	// z.Z. wird immer ein neuer Alarm angenommen
	// Bei Diagnose sind eh nur die Daten des Datensatzes relevant
	// Bei Karte einführen muss eine Kto/BLZ mitkommen, sonst sind es eh alte Daten
	// Bei Karte entnehmen kommt laut Protokoll immer Kto/BLZ mit
	BOOL bReturn = TRUE;
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordNcr_1_2::OnReceivedData(LPBYTE lpB, DWORD dwBufferLen)
{
	DWORD dw = 0;

	while (lpB && dw<dwBufferLen)
	{
		// Zuerst auf Sonderzeichen auswerten, um variable Datenfelder
		// und Fehler bei der Uebertragung abzufangen
		// Startzeichen STX
		if (lpB[dw] == ASCII_STX) {
			// Sonderfall Checksumme
			if (NCR_NEW_CRC == m_eStatus) {
				// Weitermachen
			}
			else if (NCR_NEW_STX != m_eStatus) {
				// OOPS Fehlerbehandlung: Hier ist was schiefgelaufen
				// hier beginnt der naechte Datensatz, deshalb neu anfangen
				m_eStatus = NCR_NEW_STX;
			}
		}
		// oder Endezeichen ETX
		else if (lpB[dw] == ASCII_ETX) {
			// Sonderfall Checksumme
			if (NCR_NEW_CRC == m_eStatus) {
				// Weitermachen
			}
			else {
				// EndeGelaende, ein Datensatz ist fertig
				m_eStatus = NCR_NEW_ETX;
			}
		}
		else {
			// Alle anderen Zeichen werden zwischengespeichert
			// ausser es wird die CRC erwartet
			if (NCR_NEW_CRC != m_eStatus) {
				m_iReceivedTotal++;
				m_iLenDataActual++;
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
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);
			}
		}

		switch (m_eStatus)
		{
		// -------- STX Startsymbol	1 Byte --------------------------------
		case NCR_NEW_STX:
			if (lpB[dw] == ASCII_STX) {
				ClearAllData();
				m_eStatus = NCR_NEW_DATA;
			}
			else {
				// Zeichen verwerfen
			}
			break;

		// ----------------- variable Daten --------------------
		case NCR_NEW_DATA:
			if (m_iLenDataActual == 1) {
				if ( lpB[dw] == 'A' ) {
					// Dies ist eine Statusabfrage
					m_bStatusRequest = TRUE;
				}
			}
			break;

		// ----------------- ETX ----------------------------
		case NCR_NEW_ETX:
			// Ist es wirklich ein ETX?
			if (lpB[dw] == ASCII_ETX) {
				// ETX gehoert noch mit in die Checksumme
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);
				// das naechste Zeichen ist die CRC
				m_eStatus = NCR_NEW_CRC;
			}
			else {
				// ansonsten wird das Zeichen ignoriert
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);
				WK_TRACE_ERROR(_T("%s Expected ETX not arrived\n"),
								CSDIControl::NameOfEnum(m_Type) );
				m_eStatus = NCR_NEW_CRC;
			}
			break;

		// ----------------- Checksumme --------------------
		case NCR_NEW_CRC:
			if ( m_byXORCheckSum != lpB[dw] ) {
				WK_TRACE_WARNING(_T("%s Checksum NOT OK\n"),
									CSDIControl::NameOfEnum(m_Type) );
				SendNAK();
				m_eStatus = NCR_NEW_STX;
			}
			else {
				TRACE(_T("%s Checksum OK\n"), CSDIControl::NameOfEnum(m_Type));
				m_eStatus = NCR_NEW_OK;
			}
			break;

		default: // was kann das sein?
			WK_TRACE_ERROR(_T("%s WRONG Status %i\n"),
					 CSDIControl::NameOfEnum(m_Type), m_eStatus);
			m_eStatus = NCR_NEW_STX;
			break;
		} // switch

		// wenn ein Datensatz abgeschlossen ist, Meldung an Server
		if (m_eStatus == NCR_NEW_OK) {
			// ist es eine Statusanforderung
			if (m_bStatusRequest) {
				RespondStatusRequest();
			}
			else {
				SendACK();
				CheckAndStoreVariableData();
				TransmitDataToSecurity();
			}
			m_eStatus = NCR_NEW_STX;			
			ClearAllData();
		}

		dw++;
	} // while
}
/////////////////////////////////////////////////////////////////////////////
void CRecordNcr_1_2::TransmitDataToSecurity()
{
	if ( m_sWholeData.GetLength() == 23) {
		m_eMelder = MELDER_DIAGNOSE;
	}
	else {
		if ( CheckForNewTAN() ) {
			m_eMelder = MELDER_START_NEW_TAN;
		}
		else {
			m_eMelder = MELDER_START_SAME_TAN;
		}
	}
	IndicateAlarm( (WORD)m_eMelder );
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordNcr_1_2::CheckForNewTAN()
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
