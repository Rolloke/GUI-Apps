/* GlobalReplace: RecordKebaPC2000 --> RecordKebaPc2000 */
/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordKebaPC2000.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordKebaPC2000.cpp $
// CHECKIN:		$Date: 2.01.06 10:05 $
// VERSION:		$Revision: 31 $
// LAST CHANGE:	$Modtime: 2.01.06 10:03 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIUnit.h"
#include "SDIInput.h"
#include "RecordKebaPc2000.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CRecordKebaPc2000::CRecordKebaPc2000(SDIControlType type,
									   int iCom,
									   BOOL bTraceAscii,
									   BOOL bTraceHex,
									   BOOL bTraceEvents)
	: CRecordKebaPc400(type, iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
	m_eTyp = TYPE_UNKNOWN;
	m_eStatus = STATUS_CARD_READER;
	m_iFailureCounter = 0;
	m_wAlarm = 0;
	m_bConfigMode = FALSE;
	m_bConfigTransmit = FALSE;
	m_dwCounterForMissedConfigAck = 0;
}
//////////////////////////////////////////////////////////////////////
CRecordKebaPc2000::~CRecordKebaPc2000()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordKebaPc2000::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_KEBA_PC2000) {
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
void CRecordKebaPc2000::OnXModemOK()
{
	if ( m_bConfigMode && (STATUS_CONFIG_START==m_eStatus) ) {
		m_eStatus = STATUS_CONFIG_END;
	}
	else {
		WK_TRACE_WARNING(_T("OnXModemOK() Typ=%s COM=%i WRONG Status\n"),
							CSDIControl::NameOfEnum(m_Type), GetCOMNumber() );
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc2000::OnXModemFailed()
{
	if ( m_bConfigMode && (STATUS_CONFIG_START==m_eStatus) ) {
		m_bConfigMode = FALSE;
		m_eStatus = STATUS_CARD_READER;
		GetInput()->ConfirmSetValue(SDI_VALUE_CONFIG_FAILED_XMODEM_FAILED);
	}
	else {
		WK_TRACE_WARNING(_T("OnXModemFailed() Typ=%s COM=%i WRONG Status\n"),
									CSDIControl::NameOfEnum(m_Type),GetCOMNumber() );
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc2000::OnTimeOutTransparent()
{
	if ( m_bConfigMode && (STATUS_CONFIG_START==m_eStatus) ) {
		m_bConfigMode = FALSE;
		m_eStatus = STATUS_CARD_READER;
		GetInput()->ConfirmSetValue(SDI_VALUE_CONFIG_FAILED_NOT_STARTED);
	}
	else {
		// Interessiert nicht
//			WK_TRACE_WARNING(_T("OnTimeOutTransparent() Typ=%s COM=%i WRONG Status\n"),
		//								NameOfEnum(m_Type),GetCOMNumber() );
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc2000::SwitchToConfigMode(const CString& sPath, BOOL bTransmit)
{
	m_bConfigMode = TRUE;
	m_bConfigTransmit = bTransmit;
	m_sConfigFilePath = sPath;
	if ((STATUS_CARD_READER==m_eStatus) && (0==m_iFailureCounter)) {
		StartConfigMode();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc2000::StartConfigMode()
{
	m_eStatus = STATUS_CONFIG_START;
	m_dwCounterForMissedConfigAck = 0;
	BYTE byBuffer[] = "KONFIG\r";
	Write(&byBuffer[0], 7);
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc2000::ResetToNormalMode()
{
	m_bConfigMode = FALSE;
	m_eStatus = STATUS_CARD_READER;
	m_iFailureCounter = 0;
	m_dwCounterForMissedConfigAck = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc2000::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	// Wenn wir am Anfang der Konfiguration stehen
	if ( m_bConfigMode && (STATUS_CONFIG_START==m_eStatus) ) {
		// wurde die Konfigurations-Anfrage angenommen
		if ( (1==dwLen) && (ASCII_ACK==pData[0]) ) {
			// Config Start ok
			if ( StartXModemMode(m_sConfigFilePath, m_bConfigTransmit) ) {
				GetInput()->ConfirmSetValue(SDI_VALUE_CONFIG_STARTED);
			}
			else {
				WK_TRACE_WARNING(_T("%s ReceiveRS232Data START XModem FAILURE Len:%u\n"),
									CSDIControl::NameOfEnum(m_Type), dwLen);
				GetInput()->ConfirmSetValue(SDI_VALUE_CONFIG_FAILED_NOT_STARTED);
				ResetToNormalMode();
			}
		}
		// oder wurde die Konfigurations-Anfrage abgewiesen
		else if (ASCII_NAK==pData[0]) {
			// Config Start not allowed
			GetInput()->ConfirmSetValue(SDI_VALUE_CONFIG_FAILED_NOT_ALLOWED);
			ResetToNormalMode();
			// ggf folgende Daten weiterleiten
			if ( dwLen > 1) {
				CheckAndStoreReadData(&pData[1], dwLen-1);
			}
		}
		else {
			// Evtl. Fehler in der Uebertragung
			// OOPS gf todo was ist zu tun
			WK_TRACE_WARNING(_T("%s ReceiveRS232Data CONFIG START FAILURE Len:%u\n"),
								CSDIControl::NameOfEnum(m_Type), dwLen);
			GetInput()->ConfirmSetValue(SDI_VALUE_CONFIG_FAILED_NOT_STARTED);
			ResetToNormalMode();
			// sicherheitshalber die Daten weiterleiten
			CheckAndStoreReadData(pData, dwLen);
		}
	}
	// Wenn die Konfiguration uebertragen wurde, Bestaetigung abwarten
	else if ( m_bConfigMode && (STATUS_CONFIG_END==m_eStatus) ) {
		// Wegen Schliessen und Oeffnen der COM-Schnittstelle am Keba PC2000 koennen
		// vor dem ACK/NAK undefinierte Zeichen uebertragen werden
		for (DWORD dw=0 ; dw<dwLen ; dw++) {
			if (ASCII_ACK==pData[dw]) {
				// Config ok, alles klar
				GetInput()->ConfirmSetValue(SDI_VALUE_CONFIG_OK);
				ResetToNormalMode();
				// ggf folgende Daten weiterleiten
				if ( dwLen > dw+1) {
					CheckAndStoreReadData(&pData[dw+1], dwLen-dw-1);
				}
			}
			else if (ASCII_NAK==pData[dw]) {
				// Config not ok, bis zu dreimal wiederholen
				if ( IncreaseFailureCounter() ) {
					GetInput()->ConfirmSetValue(SDI_VALUE_CONFIG_FAILED_NEXT_TRY);
					StartConfigMode();
				}
				else {
					GetInput()->ConfirmSetValue(SDI_VALUE_CONFIG_FAILED_NOT_OK);
					ResetToNormalMode();
					// ggf folgende Daten weiterleiten
					if ( dwLen > dw+1) {
						CheckAndStoreReadData(&pData[dw+1], dwLen-dw-1);
					}
				}
			}
			else {
				m_dwCounterForMissedConfigAck++;
			}
		}
		// Wenn nach soundso vielen Zeichen immer noch kein ACK/NAK dabei war
		if (m_dwCounterForMissedConfigAck>20) {
			// Evtl. Fehler in der Uebertragung
			// OOPS gf todo was ist zu tun
			WK_TRACE_WARNING(_T("%s ReceiveRS232Data CONFIG NO ACK/NAK\n"),
								CSDIControl::NameOfEnum(m_Type));
			GetInput()->ConfirmSetValue(SDI_VALUE_CONFIG_FAILED_NOT_OK);
			ResetToNormalMode();
		}
	}
	// sonst sind wir im ganz normalen Daten-Mode
	else {
		// Daten ueberpruefen
		CheckAndStoreReadData(pData, dwLen);
		// Pruefe, ob Konfiguration gewuenscht und erlaubt
		if ( m_bConfigMode && (STATUS_CARD_READER==m_eStatus) && (0==m_iFailureCounter) ) {
			StartConfigMode();
		}
	}
}
