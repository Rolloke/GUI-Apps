/* GlobalReplace: CRecordIBM --> CRecordIbm */
/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordIbm.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordIbm.cpp $
// CHECKIN:		$Date: 2.01.06 10:05 $
// VERSION:		$Revision: 38 $
// LAST CHANGE:	$Modtime: 2.01.06 10:03 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIUnit.h"
#include "SDIInput.h"
#include "wkclasses\ComParameters.h"
#include "RecordConfigurable.h"
#include "RecordIbm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CRecordIbm::CRecordIbm(SDIControlType type,
											 int iCom,
											 BOOL bTraceAscii,
											 BOOL bTraceHex,
											 BOOL bTraceEvents)
	: CRecordConfigurable(type, iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
	m_eStatus = IBM_STX;
	m_iPhotoStep = 0;
	m_bNewAlarm = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CRecordIbm::~CRecordIbm()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordIbm::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_IBM || m_Type == SDICT_SNI_IBM_EMU) {
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
void CRecordIbm::InitializeResponses()
{
	BYTE byXORCheckSum = 0;
	int i=0;

	// ACK response initialisieren
	m_byaACK.RemoveAll();
	m_byaACK.Add(ASCII_STX);
	m_byaACK.Add(ASCII_ACK);
	m_byaACK.Add(ASCII_LF);
	m_byaACK.Add(ASCII_ETX);
// Reicht wenn es einmal berechnet wurde
#if 1
	byXORCheckSum = 15;
#else 
	byXORCheckSum = 0;
	for (i=0 ; i<m_byaACK.GetSize() ; i++) {
		byXORCheckSum = (BYTE)(byXORCheckSum ^ m_byaACK.GetAt(i));
	}
#endif
	m_byaACK.Add(byXORCheckSum);

	// NAK response initialisieren
	m_byaNAK.RemoveAll();
	m_byaNAK.Add(ASCII_STX);
	m_byaNAK.Add(ASCII_NAK);
	m_byaNAK.Add(ASCII_LF);
	m_byaNAK.Add(ASCII_ETX);
// Reicht wenn es einmal berechnet wurde
#if 1
	byXORCheckSum = 28;
#else 
	byXORCheckSum = 0;
	for (i=0 ; i<m_byaNAK.GetSize() ; i++) {
		byXORCheckSum = (BYTE)(byXORCheckSum ^ m_byaNAK.GetAt(i));
	}
#endif
	m_byaNAK.Add(byXORCheckSum);

	// Status response initialisieren
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
	// Most significant digit of year set to '0'
// Ist nicht notwendig
//	sBuffer.SetAt(sBuffer.GetLength()-1,'0');
	szBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(szBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	m_byaStatusResponse.Add(ASCII_US);
// Extended status ist optional, wir verringern die Datenmenge
/*	sBuffer = "00000000";
	szBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(szBuffer[i]);
	}
*/
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = "05100";
	szBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(szBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = "1000";
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
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordIbm::CheckForNewAlarmData()
{
	BOOL bReturn = m_bNewAlarm;
	if (TRUE == m_bNewAlarm) {
		m_bNewAlarm = FALSE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordIbm::OnReceivedData(LPBYTE lpB, DWORD dwBufferLen)
{
	DWORD dw = 0;

	while (lpB && dw<dwBufferLen)
	{
		// Zuerst auf Sonderzeichen auswerten, um variable Datenfelder
		// und Fehler bei der Uebertragung abzufangen
		// Startzeichen STX
		if (lpB[dw] == ASCII_STX) {
			// Sonderfall Checksumme
			if (IBM_CRC == m_eStatus) {
				// Weitermachen
			}
			else if (IBM_STX != m_eStatus) {
				// OOPS Fehlerbehandlung: Hier ist was schiefgelaufen
				// hier beginnt der naechte Datensatz, deshalb neu anfangen
				m_eStatus = IBM_STX;
			}
		}
		// oder Endezeichen ETX
		else if (lpB[dw] == ASCII_ETX) {
			// Sonderfall Checksumme
			if (IBM_CRC == m_eStatus) {
				// Weitermachen
			}
			else {
				// EndeGelaende, ein Datensatz ist fertig
				m_eStatus = IBM_ETX;
			}
		}
		else {
			// Alle anderen Zeichen werden zwischengespeichert
			// ausser es wird die CRC erwartet
			if (IBM_CRC != m_eStatus) {
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
		case IBM_STX:
			if (lpB[dw] == ASCII_STX) {
				ClearAllData();
				m_eStatus = IBM_LENGTH;
			}
			else {
				// Zeichen verwerfen
			}
			break;

		// ------------------ Datenlaenge 3 Byte ----------------------
		case IBM_LENGTH:
			// ist es ueberhaupt eine Zahl
			if ( isdigit( lpB[dw] ) ) {
				m_iDataLen = m_iDataLen * 10;
				m_iDataLen += lpB[dw] - 0x30;
			}
			// Es kann aber auch ein StatusRequest sein...
			else if (m_iLenDataActual==1 && lpB[dw] == 'A') {
				m_bStatusRequest = TRUE;
				m_eStatus = IBM_ETX;
			}
			else {
				// OOPS was dann?
				// evtl. Daten verwerfen und auf STX warten ?
				SendNAK();
				m_eStatus = IBM_STX;
			}
			if ( m_iLenDataActual==3 ) {
				m_iDataLen = _ttoi(m_sWholeData);
				ClearActualData();
				m_eStatus = IBM_FORMAT;
			}
			break;

		// ------------------ Format 1 Byte ----------------------
		case IBM_FORMAT:
			// Format wird ignoriert
			ClearActualData();
			m_eStatus = IBM_WSID;
			break;

		// ------------------ Workstation ID 5 Byte -------
		case IBM_WSID:
			if ( m_iLenDataActual==5 ) {
				m_sSingleData = m_sWholeData;
				StoreWorkstationID();
				ClearActualData();
				m_eStatus = IBM_DATE_WS;
			}
			break;

		// ------------------ Datum 6 Byte -------------------
		case IBM_DATE_WS:
			if ( m_iLenDataActual==6 ) {
				m_sSingleData = m_sWholeData;
				StoreDate(SDI_DATA_FORMAT_MMDDYY);
				ClearActualData();
				m_eStatus = IBM_TIME_WS;
			}
			break;

		// -------------------- Zeit 6 Byte -------------------
		case IBM_TIME_WS:
			if ( m_iLenDataActual==6 ) {
				m_sSingleData = m_sWholeData;
				StoreTime(SDI_DATA_FORMAT_HHMMSS);
				ClearActualData();
				m_eStatus = IBM_TAN_WS;
			}
			break;

		// ----------------- Workstation Sequenz 4 Byte --------------------
		case IBM_TAN_WS:
			if ( m_iLenDataActual==4 ) {
				m_sSingleData = m_sWholeData;
				StoreTAN();
				ClearActualData();
				m_eStatus = IBM_PHOTO_STEP;
			}
			break;

		// ----------------- Photo Step 2 Byte --------------------
		case IBM_PHOTO_STEP:
			if ( m_iLenDataActual==2 ) {
				IsPhotoStepValid();
				ClearActualData();
				m_eStatus = IBM_STATE_WS;
			}
			break;

		// ----------------- Status des IBM 1 Byte --------------------
		case IBM_STATE_WS:
			// Status des IBM wird ignoriert
			ClearActualData();
			m_eStatus = IBM_DATA;
			break;

		// ----------------- variable Daten --------------------
		case IBM_DATA:
			if ( m_iReceivedTotal > (m_iDataLen) ) {
				// Dies muesste eigentlich ein ETX sein!!!
				WK_TRACE_WARNING(_T("%s Received Data %i will exceed announced Len %i\n"),
						 CSDIControl::NameOfEnum(m_Type), m_iReceivedTotal, m_iDataLen);
				m_eStatus = IBM_CRC;
			}
			break;

		// ----------------- ETX ----------------------------
		case IBM_ETX:
			// Ist es wirklich ein ETX?
			// Beim StatusRequest koennen evtl. auch noch andere Zeichen kommen...
			if (lpB[dw] == ASCII_ETX) {
				// ETX gehoert noch mit in die Checksumme
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);
				// das naechste Zeichen ist die CRC
				m_eStatus = IBM_CRC;
			}
			else {
				// ansonsten wird das Zeichen ignoriert
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);
				if ( m_iReceivedTotal ==  9 ) {
					// Dies muesste eigentlich ein ETX sein!!!
					WK_TRACE_WARNING(_T("%s Received Data %i will exceed StatusRequestLen\n"),
								CSDIControl::NameOfEnum(m_Type), m_iReceivedTotal);
					m_eStatus = IBM_CRC;
				}
			}
			break;

		// ----------------- Checksumme --------------------
		case IBM_CRC:
			if ( m_byXORCheckSum != lpB[dw] ) {
				WK_TRACE_WARNING(_T("%s Checksum NOT OK\n"),
									CSDIControl::NameOfEnum(m_Type) );
				SendNAK();
				m_eStatus = IBM_STX;
			}
			else {
				TRACE(_T("%s Checksum OK\n"),
						CSDIControl::NameOfEnum(m_Type));
				m_eStatus = IBM_OK;
			}
			break;

		default: // was kann das sein?
			WK_TRACE_ERROR(_T("%s WRONG Status %i\n"),
							CSDIControl::NameOfEnum(m_Type), m_eStatus);
			m_eStatus = IBM_STX;
			break;
		} // switch

		// wenn ein Datensatz abgeschlossen ist, Meldung an Server
		if (m_eStatus == IBM_OK) {
			// ist es eine Statusanforderung
			if (m_bStatusRequest) {
				RespondStatusRequest();
			}
			else {
				SendACK();
				if ( m_iReceivedTotal != m_iDataLen ) {
					WK_TRACE_ERROR(_T("%s Received Data %i NOT announced Len %i\n"),
							CSDIControl::NameOfEnum(m_Type), m_iReceivedTotal, m_iDataLen);
					// was wenn nicht, erst mal ignorieren
				}
				CheckAndStoreVariableData();
				TransmitDataToSecurity();
			}
			m_eStatus = IBM_STX;			
			ClearAllData();
		}

		dw++;
	} // while
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordIbm::IsPhotoStepValid()
{
	BOOL bReturn = FALSE;
	// Es werden alle PhotoSteps erfasst.
	// Jeder PhotoSteps wird als Alarm gewertet werden
	int iPhotoStep = _ttoi(m_sWholeData);
	if ( iPhotoStep>=1 && iPhotoStep<=16 ) {
		// ist der aktuelle Photostep kleiner als der vorhergehende,
		// so wird ein neuer Transaktionszyklus eingeleitet
		if (iPhotoStep < m_iPhotoStep) {
			m_bNewAlarm = TRUE;
		}
		m_iPhotoStep = iPhotoStep;
		bReturn = TRUE;
	}
	else {
		WK_TRACE_ERROR(_T("%s PhotoStep Invalid %i\n"),
						CSDIControl::NameOfEnum(m_Type), m_iPhotoStep);
		m_iPhotoStep = 0;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordIbm::TransmitDataToSecurity()
{
	IndicateAlarm((WORD)m_iPhotoStep);
}
