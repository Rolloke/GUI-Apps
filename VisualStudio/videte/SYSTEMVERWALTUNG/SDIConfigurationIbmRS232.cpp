/* GlobalReplace: CSDIConfigurationRS232 --> CSDIConfigurationIbmRS232 */
// SDIConfigurationRS232.cpp : implementation file
//

#include "stdafx.h"
#include "SDIConfigurationDialog.h"
#include "SDIConfigurationIbmRS232.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigurationIbmRS232
CSDIConfigurationIbmRS232::CSDIConfigurationIbmRS232(CSDIConfigurationDialog* pParent, 
													 int iCom,
													 BOOL bTraceAscii/*=TRUE*/,
													 BOOL bTraceHex/*=FALSE*/,
													 BOOL bTraceEvents/*=FALSE*/)
	: CSDIConfigurationRS232(pParent, iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
	m_eStatus = IBM_STX;
	InitializeResponses();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationIbmRS232::InitializeResponses()
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
	m_byaStatusResponse.RemoveAll();
	m_byaStatusResponse.Add(ASCII_STX);
	CTime Time;
	CWK_String sBuffer = Time.Format(_T("%H%M"));
	sBuffer.MakeReverse();
	LPCSTR pszBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) 
	{
		m_byaStatusResponse.Add(pszBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = Time.Format(_T("%y%m%d"));
	sBuffer.MakeReverse();
	// Most significant digit of year set to '0'
// Ist nicht notwendig
//	sBuffer.SetAt(sBuffer.GetLength()-1,'0');
	pszBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) 
	{
		m_byaStatusResponse.Add(pszBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	m_byaStatusResponse.Add(ASCII_US);
// Extended status ist optional, wir verringern die Datenmenge
/*	sBuffer = "00000000";
	pszBuffer = (LPCSTR)sBuffer;
	for (i=1 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(pszBuffer[i]);
	}
*/
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = "05100";
	pszBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(pszBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = _T("1000");
	pszBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(pszBuffer[i]);
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
void CSDIConfigurationIbmRS232::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	if (m_bReadInData == FALSE) {
		return;
	}

	DWORD dw = 0;
	while (pData && dw<dwLen)
	{
		// Zuerst auf Sonderzeichen auswerten, um variable Datenfelder
		// und Fehler bei der Uebertragung abzufangen
		// Startzeichen STX
		if (pData[dw] == ASCII_STX) {
			// Sonderfall Checksumme
			if (IBM_CRC == m_eStatus) {
				// Weitermachen
			}
			else if (IBM_STX != m_eStatus) {
				// OOPS Fehlerbehandlung: Hier ist was schiefgelaufen
				// hier beginnt der naechste Datensatz, deshalb neu anfangen
				m_eStatus = IBM_STX;
			}
		}
		// oder Endezeichen ETX
		else if (pData[dw] == ASCII_ETX) {
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
				if (pData[dw]==ASCII_NUL) {
					CString sMsg;
					sMsg.Format( _T("ERROR Ascii NULL|COM%i"), GetCOMNumber() );
					WK_STAT_PEAK(_T("Reset"), 1, sMsg);
					m_sBuffer += ASCII_SPACE;
				}
				else {
					m_sBuffer += pData[dw];
					m_sDataComplete += pData[dw];
				}
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ pData[dw]);
			}
		}

		switch (m_eStatus)
		{
		// -------- STX Startsymbol	1 Byte --------------------------------
		case IBM_STX:
			if (pData[dw] == ASCII_STX) {
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
			if ( isdigit( pData[dw] ) ) {
				m_iDataLen = m_iDataLen * 10;
				m_iDataLen += pData[dw] - 0x30;
			}
			// Es kann aber auch ein StatusRequest sein...
			else if (m_iLenDataActual==1 && pData[dw] == _T('A')) {
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
				ClearActualData();
				// Die Laenge beruecksichtigt nur die folgende Daten?
				// FALSCH, Laengenfeld gehoert mit zur Gesamtlaenge
				// Daher m_iReceivedTotal NICHT zuruecksetzen!
				// m_iReceivedTotal = 0;
				m_eStatus = IBM_DATA;
			}
			break;

		// ----------------- Daten --------------------
		case IBM_DATA:
			if ( m_iReceivedTotal == IBM_MSG_HEADER_LEN ) {
				// Ab hier beginnen die variablen Daten!
				ClearActualData();
			}
			if ( m_iReceivedTotal > (m_iDataLen) ) {
				// Dies muesste eigentlich ein ETX sein!!!
				WK_TRACE_WARNING(_T("Received Data %i will exceed announced Len %i\n"),
										m_iReceivedTotal, m_iDataLen);
				m_eStatus = IBM_CRC;
			}
			break;

		// ----------------- ETX ----------------------------
		case IBM_ETX:
			// Ist es wirklich ein ETX?
			// Beim StatusRequest koennen evtl. auch noch andere Zeichen kommen...
			if (pData[dw] == ASCII_ETX) {
				// ETX gehoert noch mit in die Checksumme
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ pData[dw]);
				// das naechste Zeichen ist die CRC
				m_eStatus = IBM_CRC;
			}
			else {
				// ansonsten wird das Zeichen ignoriert
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ pData[dw]);
				if ( m_iReceivedTotal >=  9 ) {
					// Dies muesste eigentlich ein ETX sein!!!
					WK_TRACE_WARNING(_T("Received Data %i will exceed StatusRequestLen\n"),
										m_iReceivedTotal);
					m_eStatus = IBM_CRC;
				}
			}
			break;

		// ----------------- Checksumme --------------------
		case IBM_CRC:
			if ( m_byXORCheckSum != pData[dw] ) {
				WK_TRACE_WARNING(_T("Checksum NOT OK\n"));
				SendNAK();
				m_eStatus = IBM_STX;
			}
			else {
				TRACE(_T("Checksum OK\n"));
				m_eStatus = IBM_OK;
			}
			break;

		default: // was kann das sein?
			WK_TRACE_ERROR(_T("WRONG Status %i\n"), m_eStatus);
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
					WK_TRACE_ERROR(_T("Received Data %i NOT announced Len %i\n"),
										m_iReceivedTotal, m_iDataLen);
					// was wenn nicht, erst mal ignorieren
				}
				// Daten komplett
				OnDataComplete();
			}
			m_eStatus = IBM_STX;			
		}
		dw++;
	} // while
}
