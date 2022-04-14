// RecordKebaPc400.cpp: implementation of the CRecordKebaPc400 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RecordKebaPc400.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRecordKebaPc400::CRecordKebaPc400(SDIControlType type,
								   int iCom,
								   BOOL bTraceAscii,
								   BOOL bTraceHex,
								   BOOL bTraceEvents)
	: CSDIControlRecord(type, iCom, 0,NULL,bTraceAscii, bTraceHex, bTraceEvents)
{
	m_eTyp = TYPE_UNKNOWN;
	m_eStatus = STATUS_CARD_READER;
	m_iFailureCounter = 0;
	m_wAlarm = 0;
}
/////////////////////////////////////////////////////////////////////////////
CRecordKebaPc400::~CRecordKebaPc400()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordKebaPc400::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_KEBA_PC400) {
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
void CRecordKebaPc400::InitializeResponses()
{
	BYTE byXORCheckSum = 0;
	int i=0;

	// ACK response initialisieren
	m_byaACK.RemoveAll();
	m_byaACK.Add(ASCII_ACK);
// Reicht wenn es einmal berechnet wurde
#if 0
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
	m_byaNAK.Add(ASCII_NAK);
// Reicht wenn es einmal berechnet wurde
#if 0
	byXORCheckSum = 28;
#else 
	byXORCheckSum = 0;
	for (i=0 ; i<m_byaNAK.GetSize() ; i++) {
		byXORCheckSum = (BYTE)(byXORCheckSum ^ m_byaNAK.GetAt(i));
	}
#endif
	m_byaNAK.Add(byXORCheckSum);
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
	CheckAndStoreReadData(pData, dwLen);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordKebaPc400::CheckForNewAlarmData()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::CheckAndStoreReadData(LPBYTE lpB, DWORD dwLen)
{
	DWORD dw = 0;
	while (lpB && dw<dwLen) {
		if (m_sWholeData.GetLength() == m_iMaxBufferLen) {
			// OOPS da fehlen wohl ein paar (mehrere) Trennzeichen
			// todo erst mal BufferOverrun ausschalten,
			// die Daten kann man allerdings vergessen, also naechstes Paket abwarten
			ClearActualData();
			m_eStatus = STATUS_CARD_READER;
		}

		// Solange kein Trennzeichen kommt, in den Buffer sichern
		if (lpB[dw]==ASCII_SPACE || lpB[dw]==ASCII_CR || lpB[dw]==ASCII_LF) {
			if ( m_sWholeData.GetLength() == 0 && lpB[dw]==ASCII_SPACE ) {
				// keine Daten bis jetzt
				// einfach weitermachen
			}
			else {
				switch (m_eStatus) {
					case STATUS_CARD_READER:
						OnStatusKennung();
						break;
					case STATUS_WEEKDAY:
						OnStatusWochentag();
						break;
					case STATUS_DATE:
						OnStatusDatum();
						break;
					case STATUS_TIME:
						OnStatusZeit();
						break;
					case STATUS_SEPERATOR:
						OnStatusTrenner();
						break;
					case STATUS_BANKCODE:
						OnStatusBLZ();
						break;
					case STATUS_SEPERATOR_ACCOUNT:
						OnStatusTrennerKonto();
						break;
					case STATUS_ACCOUNT:
						OnStatusKonto();
						break;
					case STATUS_CREDIT:
						OnStatusKredit();
						break;
					case STATUS_LF:
						OnStatusLF(lpB[dw]);
						break;
					default: // OOPS was soll das sein
						WK_TRACE_ERROR(_T("%s CheckAndStoreReadData WRONG STATUS\n"),
										CSDIControl::NameOfEnum(m_Type));
						break;
				} // switch
			}
		}
		else {
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
		dw++;

		if (m_eStatus == STATUS_OK) {
			SendACK();
			TransmitDataToSecurity();
			IncreaseStatus();
			m_iFailureCounter = 0;
			ClearAllData();
		}
		else if (m_eStatus == STATUS_FAILED) {
			WK_TRACE_WARNING(_T("%s Data INCORRECT\n"), CSDIControl::NameOfEnum(m_Type));
			TransmitDataToSecurity();
			// Ist das zugehoerige LF schon gekommen
			if ( CheckForLF() ) {
				SendNAK();
				IncreaseStatus();
				IncreaseFailureCounter();			}
			// sonst darauf warten
			else {
				m_eStatus = STATUS_LF;
			}
			ClearAllData();
		}
	} // while
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::OnStatusKennung()
{
	// Kennung 4 Byte
	// korrekte Kennung empfangen
	if ( m_sWholeData.GetLength() == 4 && IsKennung() ) {
		IncreaseStatus();
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::OnStatusWochentag()
{
	// Wochentag 2 Byte
	// ist es nur der Wochentag
	if (m_sWholeData.GetLength() == 2) {
		IncreaseStatus();
		ClearActualData();
	}
	// Datenlaenge ist viel zu groß oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = STATUS_FAILED;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::OnStatusDatum()
{
	// Datum 8 Byte im Format DD_MM_YY
	// ist es nur das Datum
	if (m_sWholeData.GetLength() == 8) {
		m_sSingleData = m_sWholeData;
		StoreDate(SDI_DATA_FORMAT_DD_MM_YY);
		IncreaseStatus();
		ClearActualData();
	}
	// Datenlaenge ist viel zu groß oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = STATUS_FAILED;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::OnStatusZeit()
{
	// Zeit 8 Byte im Format HH_MM_SS
	// ist es nur die Zeit
	if (m_sWholeData.GetLength() == 8) {
		m_sSingleData = m_sWholeData;
		StoreTime(SDI_DATA_FORMAT_HH_MM_SS);
		IncreaseStatus();
		ClearActualData();
	}
	// Datenlaenge ist viel zu groß oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = STATUS_FAILED;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::OnStatusTrenner()
{
	// trenner 1 oder 4 Byte
	// ist es nur der Trenner
	// OOPS Protokoll wird geaendert, kommt nicht mehr
	// kommt immer Trenner_BLZ
	if (m_sWholeData.GetLength() == 1) {
		m_eTyp = TYPE_CREDIT;
		IncreaseStatus();
		ClearActualData();
	}
	// ist es BLZ und der Trenner
	else if (m_sWholeData.GetLength() == 4) {
		m_eTyp = TYPE_EC;
		IncreaseStatus();
		ClearActualData();
	}
	// Datenlaenge ist viel zu groß oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = STATUS_FAILED;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::OnStatusBLZ()
{
	// BLZ 8 Byte
	// ist es nur die BLZ
	if (m_sWholeData.GetLength() == 8) {
		m_sSingleData = m_sWholeData;
		StoreBankCode();
		IncreaseStatus();
		ClearActualData();
	}
	// Datenlaenge ist viel zu groß oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = STATUS_FAILED;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::OnStatusTrennerKonto()
{
	// Trenner Konto 4 Byte
	// ist es der Trenner_Konto
	if (m_sWholeData.GetLength() == 4) {
		IncreaseStatus();
		ClearActualData();
	}
	// Datenlaenge ist viel zu groß oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = STATUS_FAILED;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::OnStatusKonto()
{
	// Konto 16 Byte oder 10 Byte
	// ist es das Konto
	if (   (m_sWholeData.GetLength() >= 10)
		&& (m_sWholeData.GetLength() <= 16)
		)
	{
		m_sSingleData = m_sWholeData;
		StoreAccount();
		IncreaseStatus();
		ClearActualData();
	}
	// Datenlaenge ist viel zu groß oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = STATUS_FAILED;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::OnStatusKredit()
{
	// Kredit 16 Byte
	// ist es die Kreditkartennummer
	// OOPS Protokoll wird geaendert, kommt nicht mehr
	if (m_sWholeData.GetLength() == 16) {
		StoreAccount();
		IncreaseStatus();
		ClearActualData();
	}
	// Datenlaenge ist viel zu groß oder Daten fehlen,
	// diese Daten koennen nicht mehr zugeordnet werden
	// sicherheitshalber einen Alarm annehmen
	else {
		m_eStatus = STATUS_FAILED;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::OnStatusLF(BYTE byte)
{
	// Daten waren defekt
	// Auf das LF warten, dabei gibt es zwei Moeglichkeiten:
	// Empfangen wurde nur ein Zeichen - das LF
	// oder mehrere sind im Buffer
	if ( m_sWholeData.GetLength() == 0 && byte==ASCII_LF
		|| CheckForLF() )
	{
		SendNAK();
		IncreaseStatus();
		IncreaseFailureCounter();
	}
	// Falls das LF verlorenging, wurde bereits die naechste Kennung empfangen?
	else if ( m_sWholeData.GetLength() == 4 && IsKennung() ) {
		// Status zweimal erhoehen, da bereits KENNUNG empfangen
		IncreaseStatus();
		IncreaseStatus();
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::TransmitDataToSecurity()
{
	IndicateAlarm(m_wAlarm);
}
/////////////////////////////////////////////////////////////////////////////
void CRecordKebaPc400::IncreaseStatus()
{
	switch (m_eStatus) {
		case STATUS_CARD_READER:
			m_eStatus = STATUS_WEEKDAY;
			break;
		case STATUS_WEEKDAY:
			m_eStatus = STATUS_DATE;
			break;
		case STATUS_DATE:
			m_eStatus = STATUS_TIME;
			break;
		case STATUS_TIME:
			m_eStatus = STATUS_SEPERATOR;
			break;
		case STATUS_SEPERATOR:
			if (m_eTyp == TYPE_CREDIT) {
				m_eStatus = STATUS_CREDIT;
			}
			else if (m_eTyp == TYPE_EC) {
				m_eStatus = STATUS_BANKCODE;
			}
			else {
				// OOPS darf gar nicht sein, sicherheitshalber neue Kennung abwarten
				m_eStatus = STATUS_CARD_READER;
			}
			break;
		case STATUS_BANKCODE:
			m_eStatus = STATUS_SEPERATOR_ACCOUNT;
			break;
		case STATUS_SEPERATOR_ACCOUNT:
			m_eStatus = STATUS_ACCOUNT;
			break;
		case STATUS_ACCOUNT:
			m_eStatus = STATUS_OK;
			break;
		case STATUS_CREDIT:
			m_eStatus = STATUS_OK;
			break;
		case STATUS_OK:
			m_eStatus = STATUS_CARD_READER;
			break;
		case STATUS_FAILED:
			m_eStatus = STATUS_CARD_READER;
			break;
		case STATUS_LF:
			m_eStatus = STATUS_CARD_READER;
			break;
		default:
			// OOPS darf gar nicht sein, sicherheitshalber neue Kennung abwarten
			WK_TRACE_ERROR(_T("%s IncreaseStatus WRONG STATUS %d\n"),
							CSDIControl::NameOfEnum(m_Type), m_eStatus);
			m_eStatus = STATUS_CARD_READER;
			break;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordKebaPc400::IsKennung()
{
	BOOL bReturn = FALSE;
	if (m_sWholeData == _T("L(1)")) {
		m_wAlarm = 1;
		bReturn = TRUE;
	}
	else if (m_sWholeData == _T("L(2)")) {
		m_wAlarm = 2;
		bReturn = TRUE;
	}
	else {
		m_wAlarm = 0;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordKebaPc400::CheckForLF()
{
	BOOL bFound = FALSE;
	for (int i=0 ; !bFound && i<m_sWholeData.GetLength() ; i++) {
		if ( m_sWholeData.GetAt(i) == ASCII_LF ) {
			bFound = TRUE;
		}
	}
	return bFound;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordKebaPc400::IncreaseFailureCounter()
{
	BOOL bReturn = TRUE;
	m_iFailureCounter++;
	if (m_iFailureCounter > 3) {
		m_iFailureCounter = 0;
		bReturn = FALSE;
	}
	return bReturn;
}
