// RecordSniDosCom.cpp: implementation of the CRecordSniDosCom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RecordSniDosCom.h"

//////////////////////////////////////////////////////////////////////
#define SNI_COM_LEN		78
// Positionen der Daten
#define POS_DATE_NORMAL		50
#define POS_TIME_NORMAL		56
#define POS_TAN_NORMAL		62
#define POS_DATE_TA			41
#define POS_TIME_TA			47
#define POS_TAN_TA			53
#define POS_DATE_AUTOR		67
#define POS_TIME_AUTOR		73
#define POS_TAN_AUTOR		79
#define POS_BANKCODE		70
#define POS_ACCOUNT			24
#define POS_VALUE_AUTOR		87
#define POS_VALUE_NORMAL	4
// Laenge der Daten
#define LEN_DATE			6
#define LEN_TIME			6
#define LEN_TAN				4
#define LEN_BANKCODE		8
#define LEN_ACCOUNT			10
#define LEN_VALUE			8

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRecordSniDosCom::CRecordSniDosCom(SDIControlType type,
											 int iCom,
											 BOOL bTraceAscii,
											 BOOL bTraceHex,
											 BOOL bTraceEvents)
	: CSDIControlRecord(type, iCom, 0,NULL,bTraceAscii, bTraceHex, bTraceEvents)
{
	m_eStatus = STATUS_EARLY_TDR;
}
/////////////////////////////////////////////////////////////////////////////
CRecordSniDosCom::~CRecordSniDosCom()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordSniDosCom::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_SNI_DOS_COM) {
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
void CRecordSniDosCom::OnReceivedData(LPBYTE lpB, DWORD dwBufferLen)
{
	DWORD dw = 0;

	while (lpB && dw<dwBufferLen) {
		if (m_sWholeData.GetLength() == m_iMaxBufferLen) {
			// OOPS da fehlen wohl ein paar (mehrere) Trennzeichen
			// todo erst mal BufferOverrun ausschalten,
			// die Daten kann man allerdings vergessen, also naechstes Paket abwarten
			// Fehler, zu viele Zeichen
			CString sMsg;
			WK_TRACE_ERROR(_T("COM%i %s Daten %s Len %i Buffer-Overrun, no CR\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type),
						(LPCTSTR)m_sWholeData,
						m_sWholeData.GetLength());
			sMsg.Format( _T("Buffer-Overrun|COM%i"), GetCOMNumber() );
			WK_STAT_PEAK(_T("BufferOverrun"), 1, sMsg);
			ClearAllData();
		}

		// Wenn ein Trennzeichen kommt, ist ein Datensatz komplett
		if (lpB[dw]==ASCII_CR || lpB[dw]==ASCII_LF) {
			CheckLine();
		}
		else {
			// Sonst das Zeichen in den Buffer sichern, ABER:
			// Eine ASCII_NUL NIEMALS in den Buffer sichern!
			// Erstens hat sie hier nichts zu suchen,
			// zweitens wuerde sie die CString-Operationen voellig durcheinander brimngen!
			if (lpB[dw]==ASCII_NUL) {
				CString sMsg;
				sMsg.Format( _T("ERROR Ascii NULL|COM%i"), GetCOMNumber() );
				WK_STAT_PEAK(_T("Ascii-Null"), 1, sMsg);
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
void CRecordSniDosCom::CheckLine()
{
	// first check for clear identifier to correct errors in status
	int iFound = -1;
	if ((iFound = m_sWholeData.Find(_T(" EARLY TDR "))) != -1) {
		if (m_eStatus != STATUS_EARLY_TDR) {
			WK_TRACE_ERROR(_T("%s CheckData WRONG status %i != %i EARLY TDR\n"),
							CSDIControl::NameOfEnum(m_Type), m_eStatus, STATUS_EARLY_TDR);
		}
		m_eStatus = STATUS_EARLY_TDR;
	}
	else if ((iFound = m_sWholeData.Find(_T(" AUTORISIERUNG "))) != -1) {
		if (m_eStatus != STATUS_AUTORISIERUNG) {
			WK_TRACE_ERROR(_T("%s CheckData WRONG status %i != %i AUTORISIERUNG\n"),
						CSDIControl::NameOfEnum(m_Type), m_eStatus, STATUS_AUTORISIERUNG);
		}
		m_eStatus = STATUS_AUTORISIERUNG;
	}
	else if ((iFound = m_sWholeData.Find(_T(" NORMAL STATUS "))) != -1) {
		if (m_eStatus != STATUS_NORMAL_STATUS) {
			WK_TRACE_ERROR(_T("%s CheckData WRONG status %i != %i NORMAL STATUS\n"),
						CSDIControl::NameOfEnum(m_Type), m_eStatus, STATUS_NORMAL_STATUS);
		}
		m_eStatus = STATUS_NORMAL_STATUS;
	}

	if (m_sWholeData.GetLength() >= SNI_COM_LEN) {
		// now check special data
		CheckData();
	}
	else {
		// less data than normal line
		// can be ignored if register output
		// must be ignored if error in line, position of data is possible corrupted
	}
	ClearAllData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckData()
{
	switch (m_eStatus) {
		case STATUS_EARLY_TDR:
			CheckDataEarlyTdr();
			break;
		case STATUS_EARLY_TDR_1:
			CheckDataEarlyTdr1();
			break;
		case STATUS_EARLY_TDR_2:
			CheckDataEarlyTdr2();
			break;
		case STATUS_TRANSAKTIONSANTWORT_1:
			CheckDataTransaktionsantwort1();
			break;
		case STATUS_AUTORISIERUNG:
			CheckDataAutorisierung();
			break;
		case STATUS_AUTORISIERUNG_1:
			CheckDataAutorisierung1();
			break;
		case STATUS_AUTORISIERUNG_2:
			CheckDataAutorisierung2();
			break;
		case STATUS_TRANSAKTIONSANTWORT_2:
			CheckDataTransaktionsantwort2();
			break;
		case STATUS_NORMAL_STATUS:
			CheckDataNormalStatus();
			break;
		case STATUS_NORMAL_STATUS_1:
			CheckDataNormalStatus1();
			break;
		case STATUS_NORMAL_STATUS_2:
			CheckDataNormalStatus2();
			break;
		case STATUS_TRANSAKTIONSANTWORT_3:
			CheckDataTransaktionsantwort3();
			break;
		default:
			WK_TRACE_ERROR(_T("%s CheckData WRONG status\n"), CSDIControl::NameOfEnum(m_Type));
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataEarlyTdr()
{
	int iFound = m_sWholeData.Find(_T(" EARLY TDR "));
	if (iFound != -1) {
		// date
		m_sSingleData = m_sWholeData.Mid(POS_DATE_NORMAL, LEN_DATE);
		StoreDate(SDI_DATA_FORMAT_YYMMDD);
		// time
		m_sSingleData = m_sWholeData.Mid(POS_TIME_NORMAL, LEN_TIME);
		StoreTime(SDI_DATA_FORMAT_HHMMSS);
		// TAN
		m_sSingleData = m_sWholeData.Mid(POS_TAN_NORMAL, LEN_TAN);
		StoreTAN();
		TransmitDataToSecurity(1);
		m_eStatus = STATUS_EARLY_TDR_1;
	}
	else {
		// ignore, wait for next line
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataEarlyTdr1()
{
	// ignore, wait for next line with next status
	m_eStatus = STATUS_EARLY_TDR_2;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataEarlyTdr2()
{
	// acount
	m_sSingleData = m_sWholeData.Mid(POS_ACCOUNT, LEN_ACCOUNT);
	StoreAccount();
	// bankcode
	m_sSingleData = m_sWholeData.Mid(POS_BANKCODE, LEN_BANKCODE);
	StoreBankCode();
	TransmitDataToSecurity(2);
	m_eStatus = STATUS_TRANSAKTIONSANTWORT_1;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataTransaktionsantwort1()
{
	int iFound = m_sWholeData.Find(_T(" TRANSAKTIONSANTWORT "));
	if (iFound != -1) {
		// date
		m_sSingleData = m_sWholeData.Mid(POS_DATE_TA, LEN_DATE);
		StoreDate(SDI_DATA_FORMAT_YYMMDD);
		// time
		m_sSingleData = m_sWholeData.Mid(POS_TIME_TA, LEN_TIME);
		StoreTime(SDI_DATA_FORMAT_HHMMSS);
		// TAN
		m_sSingleData = m_sWholeData.Mid(POS_TAN_TA, LEN_TAN);
		StoreTAN();
		TransmitDataToSecurity(3);
		m_eStatus = STATUS_AUTORISIERUNG;
	}
	else {
		// ignore, wait for next line
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataAutorisierung()
{
	int iFound = m_sWholeData.Find(_T(" AUTORISIERUNG "));
	if (iFound != -1) {
		// date
		m_sSingleData = m_sWholeData.Mid(POS_DATE_AUTOR, LEN_DATE);
		StoreDate(SDI_DATA_FORMAT_YYMMDD);
		// time
		m_sSingleData = m_sWholeData.Mid(POS_TIME_AUTOR, LEN_TIME);
		StoreTime(SDI_DATA_FORMAT_HHMMSS);
		// TAN
		m_sSingleData = m_sWholeData.Mid(POS_TAN_AUTOR, LEN_TAN);
		StoreTAN();
		// value
		m_sSingleData = m_sWholeData.Mid(POS_VALUE_AUTOR, LEN_VALUE);
		StoreValue();
		TransmitDataToSecurity(4);
		m_eStatus = STATUS_AUTORISIERUNG_1;
	}
	else {
		// ignore, wait for next line
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataAutorisierung1()
{
	// ignore, wait for next line with next status
	m_eStatus = STATUS_AUTORISIERUNG_2;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataAutorisierung2()
{
	// acount
	m_sSingleData = m_sWholeData.Mid(POS_ACCOUNT, LEN_ACCOUNT);
	StoreAccount();
	// bankcode
	m_sSingleData = m_sWholeData.Mid(POS_BANKCODE, LEN_BANKCODE);
	StoreBankCode();
	// value
	m_sSingleData = m_sWholeData.Mid(POS_VALUE_NORMAL, LEN_VALUE);
	StoreValue();
	TransmitDataToSecurity(5);
	m_eStatus = STATUS_TRANSAKTIONSANTWORT_2;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataTransaktionsantwort2()
{
	int iFound = m_sWholeData.Find(_T(" TRANSAKTIONSANTWORT "));
	if (iFound != -1) {
		// date
		m_sSingleData = m_sWholeData.Mid(POS_DATE_TA, LEN_DATE);
		StoreDate(SDI_DATA_FORMAT_YYMMDD);
		// time
		m_sSingleData = m_sWholeData.Mid(POS_TIME_TA, LEN_TIME);
		StoreTime(SDI_DATA_FORMAT_HHMMSS);
		// TAN
		m_sSingleData = m_sWholeData.Mid(POS_TAN_TA, LEN_TAN);
		StoreTAN();
		TransmitDataToSecurity(6);
		m_eStatus = STATUS_NORMAL_STATUS;
	}
	else {
		// ignore, wait for next line
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataNormalStatus()
{
	int iFound = m_sWholeData.Find(_T(" NORMAL STATUS "));
	if (iFound != -1) {
		// date
		m_sSingleData = m_sWholeData.Mid(POS_DATE_NORMAL, LEN_DATE);
		StoreDate(SDI_DATA_FORMAT_YYMMDD);
		// time
		m_sSingleData = m_sWholeData.Mid(POS_TIME_NORMAL, LEN_TIME);
		StoreTime(SDI_DATA_FORMAT_HHMMSS);
		// TAN
		m_sSingleData = m_sWholeData.Mid(POS_TAN_NORMAL, LEN_TAN);
		StoreTAN();
		TransmitDataToSecurity(7);
		m_eStatus = STATUS_NORMAL_STATUS_1;
	}
	else {
		// ignore, wait for next line
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataNormalStatus1()
{
	// ignore, wait for next line with next status
	m_eStatus = STATUS_NORMAL_STATUS_2;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataNormalStatus2()
{
	// acount
	m_sSingleData = m_sWholeData.Mid(POS_ACCOUNT, LEN_ACCOUNT);
	StoreAccount();
	// bankcode
	m_sSingleData = m_sWholeData.Mid(POS_BANKCODE, LEN_BANKCODE);
	StoreBankCode();
	// value
	m_sSingleData = m_sWholeData.Mid(POS_VALUE_NORMAL, LEN_VALUE);
	StoreValue();
	TransmitDataToSecurity(8);
	m_eStatus = STATUS_TRANSAKTIONSANTWORT_3;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::CheckDataTransaktionsantwort3()
{
	int iFound = m_sWholeData.Find(_T(" TRANSAKTIONSANTWORT "));
	if (iFound != -1) {
		// date
		m_sSingleData = m_sWholeData.Mid(POS_DATE_TA, LEN_DATE);
		StoreDate(SDI_DATA_FORMAT_YYMMDD);
		// time
		m_sSingleData = m_sWholeData.Mid(POS_TIME_TA, LEN_TIME);
		StoreTime(SDI_DATA_FORMAT_HHMMSS);
		// TAN
		m_sSingleData = m_sWholeData.Mid(POS_TAN_TA, LEN_TAN);
		StoreTAN();
		TransmitDataToSecurity(9);
		m_eStatus = STATUS_EARLY_TDR;
	}
	else {
		// ignore, wait for next line
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordSniDosCom::TransmitDataToSecurity(WORD wAlarm)
{
	IndicateAlarm(wAlarm);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordSniDosCom::CheckForNewAlarmData()
{
	return (m_eStatus == STATUS_EARLY_TDR);
}
