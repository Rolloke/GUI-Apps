// SDIConfigurationNcrRS232.cpp : implementation file
//

#include "stdafx.h"
#include "SDIConfigurationDialog.h"
#include "SDIConfigurationNcrRS232.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigurationNcrRS232

CSDIConfigurationNcrRS232::CSDIConfigurationNcrRS232(CSDIConfigurationDialog* pParent, 
													 int iCom,
													 BOOL bTraceAscii/*=TRUE*/,
													 BOOL bTraceHex/*=FALSE*/,
													 BOOL bTraceEvents/*=FALSE*/)
	: CSDIConfigurationRS232(pParent, iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
	m_eStatus = NCR_STX;
	InitializeResponses();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationNcrRS232::InitializeResponses()
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
	pszBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++)
	{
		m_byaStatusResponse.Add(pszBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = "111";	// Transaktionsnummer 3 Byte
	pszBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(pszBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = "2222";	// Terminalnummer 4 Byte
	pszBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(pszBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = "99999";	// Anzahl der Bilder 5 Byte
	pszBuffer = (LPCSTR)sBuffer;
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(pszBuffer[i]);
	}
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = "    ";	// Frei 4 Byte
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
	m_byaStatusResponse.Add(ASCII_LF);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationNcrRS232::OnReceivedData(LPBYTE pData, DWORD dwLen)
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
			if (NCR_CRC == m_eStatus) {
				// Weitermachen
			}
			else if (NCR_STX != m_eStatus) {
				// OOPS Fehlerbehandlung: Hier ist was schiefgelaufen
				// hier beginnt der naechste Datensatz, deshalb neu anfangen
				m_eStatus = NCR_STX;
			}
		}
		// oder Endezeichen ETX
		else if (pData[dw] == ASCII_ETX) {
			// Sonderfall Checksumme
			if (NCR_CRC == m_eStatus) {
				// Weitermachen
			}
			else {
				// EndeGelaende, ein Datensatz ist fertig
				m_eStatus = NCR_ETX;
			}
		}
		else {
			// Alle anderen Zeichen werden zwischengespeichert
			// ausser es wird die CRC erwartet
			if (NCR_CRC != m_eStatus) {
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
		case NCR_STX:
			if (pData[dw] == ASCII_STX) {
				ClearAllData();
				m_eStatus = NCR_DATA;
			}
			else {
				// Zeichen verwerfen
			}
			break;

		// ----------------- Daten --------------------
		case NCR_DATA:
			if (m_iLenDataActual == 1) {
				if ( pData[dw] == _T('A') ) {
					// Dies ist eine Statusabfrage
					m_bStatusRequest = TRUE;
				}
			}
			break;

		// ----------------- ETX ----------------------------
		case NCR_ETX:
			// Ist es wirklich ein ETX?
			if (pData[dw] == ASCII_ETX) {
				// ETX gehoert noch mit in die Checksumme
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ pData[dw]);
				// das naechste Zeichen ist die CRC
				m_eStatus = NCR_CRC;
			}
			else {
				// ansonsten wird das Zeichen ignoriert
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ pData[dw]);
			}
			break;

		// ----------------- Checksumme --------------------
		case NCR_CRC:
			if ( m_byXORCheckSum != pData[dw] ) {
				WK_TRACE_WARNING(_T("Checksum NOT OK\n"));
				SendNAK();
				m_eStatus = NCR_STX;
			}
			else {
				TRACE(_T("Checksum OK\n"));
				m_eStatus = NCR_OK;
			}
			break;

		default: // was kann das sein?
			WK_TRACE_ERROR(_T("WRONG Status %i\n"), m_eStatus);
			m_eStatus = NCR_STX;
			break;
		} // switch

		// wenn ein Datensatz abgeschlossen ist, Meldung an Server
		if (m_eStatus == NCR_OK) {
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
			m_eStatus = NCR_STX;			
		}
		dw++;
	} // while
}
