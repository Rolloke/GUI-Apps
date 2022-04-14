// SDIConfigurationRondoRS232.cpp: implementation of the CSDIConfigurationRondoRS232 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wkclasses\wk_ascii.h"
#include "wkclasses\wk_time.h"
#include "SDIConfigurationDialog.h"
#include "SDIConfigurationRondoRS232.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSDIConfigurationRondoRS232::CSDIConfigurationRondoRS232(CSDIConfigurationDialog* pParent, 
														 int iCom,
														 BOOL bTraceAscii/*=TRUE*/,
														 BOOL bTraceHex/*=FALSE*/,
														 BOOL bTraceEvents/*=FALSE*/)
	: CSDIConfigurationRS232(pParent, iCom, bTraceAscii, bTraceHex, bTraceEvents)
{

}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigurationRondoRS232::~CSDIConfigurationRondoRS232()
{

}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigurationRondoRS232::InitializeResponses()
{
	BYTE byXORCheckSum = 0;
	int i=0;

	// ACK response initialisieren
	m_byaACK.Add(ASCII_ACK);

	// NAK response initialisieren
	m_byaNAK.Add(ASCII_NAK);

	// KameraStatus response initialisieren
	CWKTime wkTime;
	CString sBuffer;
	m_byaStatusResponse.RemoveAll();
	m_byaStatusResponse.Add(ASCII_STX);
	m_byaStatusResponse.Add(_T('3'));		// Anzahl der angeschlossenen Kameras
	m_byaStatusResponse.Add(ASCII_US);
	sBuffer = _T("VMnxxx");					// Siehe Protokoll, z.Z.
	for (i=0 ; i<sBuffer.GetLength() ; i++) {
		m_byaStatusResponse.Add(sBuffer.GetAt(i));
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
void CSDIConfigurationRondoRS232::OnReceivedData(LPBYTE lpB, DWORD dwBufferLen)
{
	DWORD dw = 0;

	while (lpB && dw<dwBufferLen)
	{
		// Zuerst auf Sonderzeichen auswerten, um variable Datenfelder
		// und Fehler bei der Uebertragung abzufangen
		// Startzeichen STX
		if (lpB[dw] == ASCII_STX) {
			// Sonderfall Checksumme
			if (STATUS_CRC == m_eStatus) {
				// Weitermachen
			}
			else if (STATUS_STX != m_eStatus) {
				// OOPS Fehlerbehandlung: Hier ist was schiefgelaufen
				// hier beginnt der naechte Datensatz, deshalb neu anfangen
				m_eStatus = STATUS_STX;
			}
		}
		// oder Endezeichen ETX
		else if (lpB[dw] == ASCII_ETX) {
			// Sonderfall Checksumme
			if (STATUS_CRC == m_eStatus) {
				// Weitermachen
			}
			else {
				// EndeGelaende, ein Datensatz ist fertig
				m_eStatus = STATUS_ETX;
			}
		}
		else {
			// Alle anderen Zeichen werden zwischengespeichert
			// ausser es wird die CRC erwartet
			if (STATUS_CRC != m_eStatus) {
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
					m_sBuffer += ASCII_SPACE;
				}
				else {
					m_sBuffer += lpB[dw];
				}
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);
			}
		}

		switch (m_eStatus)
		{
		// -------- STX Startsymbol	1 Byte --------------------------------
		case STATUS_STX:
			if (lpB[dw] == ASCII_STX) {
				ClearAllData();
				m_eStatus = STATUS_COMMAND;
			}
			else {
				// Zeichen verwerfen
			}
			break;

		// -------- Kommando 1 Byte --------------------------------
		case STATUS_COMMAND:
			if ( lpB[dw] == _T('E') ) {
				// Dies ist eine Statusabfrage
				ClearActualData();
				m_bStatusRequest = TRUE;
				m_eStatus = STATUS_ETX;
			}
			else {
				m_eStatus = STATUS_DATA;
			}
			break;

		// ----------------- variable Daten --------------------
		case STATUS_DATA:
			break;

		// ----------------- ETX ----------------------------
		case STATUS_ETX:
			// Ist es wirklich ein ETX?
			if (lpB[dw] == ASCII_ETX) {
				// ETX gehoert noch mit in die Checksumme
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);
				// das naechste Zeichen ist die CRC
				m_eStatus = STATUS_CRC;
			}
			else {
				// ansonsten wird das Zeichen ignoriert
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);
				WK_TRACE_ERROR(_T("COM %i Expected ETX not arrived\n"),
								GetCOMNumber() );
				m_eStatus = STATUS_CRC;
			}
			break;

		// ----------------- Checksumme --------------------
		case STATUS_CRC:
			if ( m_byXORCheckSum != lpB[dw] ) {
				WK_TRACE_WARNING(_T("Com %i Checksum NOT OK\n"),
									GetCOMNumber() );
				SendNAK();
				m_eStatus = STATUS_STX;
			}
			else {
				TRACE(_T("Com %i Checksum OK\n"), GetCOMNumber());
				m_eStatus = STATUS_OK;
			}
			break;

		default: // was kann das sein?
			WK_TRACE_ERROR(_T("Com %i WRONG Status %i\n"),
					 GetCOMNumber(), m_eStatus);
			m_eStatus = STATUS_STX;
			break;
		} // switch

		// wenn ein Datensatz abgeschlossen ist, Meldung an Server
		if (m_eStatus == STATUS_OK) {
			SendACK();
			// ist es eine Statusanforderung
			if (m_bStatusRequest) {
				RespondStatusRequest();
			}
			else {
				// Daten komplett
				OnDataComplete();
			}
			m_eStatus = STATUS_STX;			
			ClearAllData();
		}

		dw++;
	} // while
}
