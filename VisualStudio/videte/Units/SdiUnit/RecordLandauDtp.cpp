// RecordLandauDtp.cpp: implementation of the CRecordLandauDtp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SDIUnit.h"
#include "SDIInput.h"
#include "RecordLandauDtp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRecordLandauDtp::CRecordLandauDtp(SDIControlType type,
												int iCom,
												BOOL bTraceAscii,
												BOOL bTraceHex,
												BOOL bTraceEvents)
	: CSDIControlRecord(type, iCom, 0,NULL,bTraceAscii, bTraceHex, bTraceEvents)
{
	m_iMaxBufferLen = 100;
	m_eStatus = STATUS_STX;
	m_iInterfaceActual = -1;
	m_iInterfaceLast = -1;
	m_iCamera = -1;
	m_bUpdateOnly = FALSE;
	m_bPreviousCameraAlarmUpdate = FALSE;
}
//////////////////////////////////////////////////////////////////////
CRecordLandauDtp::~CRecordLandauDtp()
{
	Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordLandauDtp::Create(CWK_Profile& wkp, const CString& sSection)
{
	BOOL bReturn = FALSE;
	// Stimmt der Typ
	if (m_Type == SDICT_LANDAU_DTP) {
		// Basisklassenfunktion aufrufen
		if ( CreateRecord() ) {
			// RS232 oeffnen
			if ( OpenCom(CBR_9600, 8, NOPARITY, ONESTOPBIT) ) {
				bReturn = TRUE;
			}
			else {
				WK_TRACE_ERROR(_T("COM%i %s Create OpenCom FAILED\n"),
								GetCOMNumber(), CSDIControl::NameOfEnum(m_Type));
			}
		}
		else {
			WK_TRACE_ERROR(_T("COM%i %s Create FAILED\n"),
								GetCOMNumber(), CSDIControl::NameOfEnum(m_Type));
		}
	}
	else {
		WK_TRACE_ERROR(_T("COM%i Create WRONG Type %s\n"),
								GetCOMNumber(), CSDIControl::NameOfEnum(m_Type));
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::InitializeResponses()
{
	// ACK response initialisieren
	m_byaACK.RemoveAll();
	m_byaACK.Add(ASCII_STX);
	m_byaACK.Add(ASCII_ACK);
	m_byaACK.Add(ASCII_ETX);

	// NAK response initialisieren
	m_byaNAK.RemoveAll();
	m_byaNAK.Add(ASCII_STX);
	m_byaNAK.Add(ASCII_NAK);
	m_byaNAK.Add(ASCII_ETX);
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnReceivedData(LPBYTE lpB, DWORD dwBufferLen)
{
	DWORD dw = 0;
	while (lpB && dw<dwBufferLen) {
		if (m_sSingleData.GetLength() == m_iMaxBufferLen) {
			// OOPS da fehlen wohl ein paar (mehrere) Trennzeichen
			// todo erst mal BufferOverrun ausschalten,
			// die Daten kann man allerdings vergessen, also naechstes Paket abwarten
			WK_TRACE_ERROR(_T("COM%i %s OnReceivedData Buffer Overrun\n"),
							GetCOMNumber(), CSDIControl::NameOfEnum(m_Type));
			ClearActualData();
			m_eStatus = STATUS_STX;
		}

		// Checksumme ist Sonderfall
		if (STATUS_CRC == m_eStatus)
		{
			OnStatusCRC(lpB[dw]);
		}
		else
		{
			// Solange kein Trennzeichen kommt, in den Buffer sichern
			if (lpB[dw]==ASCII_STX || lpB[dw]==ASCII_US || lpB[dw]==ASCII_ETX) {
				if (lpB[dw]==ASCII_US || lpB[dw]==ASCII_ETX)
				{
					// Die gehoeren mit zur Checksumme
					m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);
				}
				switch (m_eStatus) {
					case STATUS_STX:
						OnStatusSTX(lpB[dw]);
						break;
					case STATUS_INTERFACE:
						OnStatusInterface();
						break;
					case STATUS_DATE:
						OnStatusDate();
						break;
					case STATUS_TIME:
						OnStatusTime();
						break;
					case STATUS_TAN:
						OnStatusTAN();
						break;
					case STATUS_WSID:
						OnStatusWSID();
						break;
					case STATUS_BANKCODE:
						OnStatusBankCode();
						break;
					case STATUS_ACCOUNT:
						OnStatusAccount();
						break;
					case STATUS_VALUE:
						OnStatusValue();
						break;
					case STATUS_CAMERA:
						OnStatusCamera();
						break;
					default: // OOPS was soll das sein
						WK_TRACE_ERROR(_T("COM%i %s OnReceivedData WRONG STATUS %i\n"),
									GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_eStatus);
						m_eStatus = STATUS_STX;
						break;
				} // switch
			}
			else {
				// Chechsumme berechnen
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[dw]);
				// Eine ASCII_NUL NIEMALS in den Buffer sichern!
				// Erstens hat sie hier nichts zu suchen,
				// zweitens wuerde sie die CString-Operationen voellig durcheinander brimngen!
				// Wird hier durch ASCII_SPACE ersetzt
				if (lpB[dw]==ASCII_NUL) {
					CString sMsg;
					sMsg.Format( _T("ERROR Ascii NULL|COM%i"), GetCOMNumber() );
					WK_STAT_PEAK(_T("AsciiNull"), 1, sMsg);
					m_sSingleData += ASCII_SPACE;
				}
				else {
					m_sSingleData += lpB[dw];
				}
			}
		}
		dw++;

		if (m_eStatus == STATUS_OK) {
			TransmitDataToSecurity();
			ClearAllData();
			m_eStatus = STATUS_STX;
		}
		else if (m_eStatus == STATUS_FAILED) {
			WK_TRACE_WARNING(_T("COM%i %s Data INCORRECT\n"),
								GetCOMNumber(), CSDIControl::NameOfEnum(m_Type));
			// OOPS z.Z. wird immer ein Alarm angenommen
			TransmitDataToSecurity();
			ClearAllData();
			m_eStatus = STATUS_STX;
		}
	} // while
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordLandauDtp::CheckForNewAlarmData()
{
	// must be overwritten, but will be never used here!
	WK_TRACE_ERROR(_T("COM%i %s CheckForNewAlarmData BUT WHY???\n"),
				GetCOMNumber(), CSDIControl::NameOfEnum(m_Type));
	ASSERT(0);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnStatusSTX(BYTE byte)
{
	if (ASCII_STX == byte)
	{
		m_iInterfaceActual = -1;
		m_iCamera = -1;
		m_eStatus = STATUS_INTERFACE;
		ClearAllData();
	}
	else
	{
		// possible after STATUS_FAILED, waiting for real STX
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnStatusInterface()
{
	if (m_sSingleData.GetLength() == 1)
	{
		TCHAR chInterface = m_sSingleData.GetAt(0);
		// valid are 0 - 3
		if (0x30 <= chInterface && chInterface <= 0x33)
		{
			m_iInterfaceActual = _ttoi(&chInterface);
			m_iInterfaceLast = m_iInterfaceActual;
			m_eStatus = STATUS_DATE;
		}
		// space is also valid as update
		else if (ASCII_SPACE == chInterface)
		{
			// update the data for the last actual interface 
			m_iInterfaceActual = m_iInterfaceLast;
			m_bUpdateOnly = TRUE;
			m_eStatus = STATUS_DATE;
		}
		else
		{
			WK_TRACE_ERROR(_T("COM%i %s Interface WRONG Hex:%02x Char:%c\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), chInterface, chInterface);
			m_iInterfaceActual = -1;
			m_eStatus = STATUS_FAILED;
		}
	}
	else
	{
		if (m_sSingleData.GetLength() < 1)
		{
			WK_TRACE_ERROR(_T("COM%i %s Interface too less data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		else
		{
			WK_TRACE_ERROR(_T("COM%i %s Interface too much data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		m_eStatus = STATUS_FAILED;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnStatusDate()
{
	if (m_sSingleData.GetLength() == 6)
	{
		StoreDate(SDI_DATA_FORMAT_DDMMYY);
		m_eStatus = STATUS_TIME;
	}
	else
	{
		if (m_sSingleData.GetLength() < 6)
		{
			WK_TRACE_ERROR(_T("COM%i %s Date too less data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		else
		{
			WK_TRACE_ERROR(_T("COM%i %s Date too much data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		m_eStatus = STATUS_FAILED;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnStatusTime()
{
	if (m_sSingleData.GetLength() == 4)
	{
		StoreTime(SDI_DATA_FORMAT_HHMM);
		m_eStatus = STATUS_TAN;
	}
	else
	{
		if (m_sSingleData.GetLength() < 4)
		{
			WK_TRACE_ERROR(_T("COM%i %s Time too less data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		else
		{
			WK_TRACE_ERROR(_T("COM%i %s Time too much data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		m_eStatus = STATUS_FAILED;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnStatusTAN()
{
	if (m_sSingleData.GetLength() == 4)
	{
		StoreTAN();
		m_eStatus = STATUS_WSID;
	}
	else
	{
		if (m_sSingleData.GetLength() < 4)
		{
			WK_TRACE_ERROR(_T("COM%i %s TAN too less data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		else
		{
			WK_TRACE_ERROR(_T("COM%i %s TAN too much data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		m_eStatus = STATUS_FAILED;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnStatusWSID()
{
	if (m_sSingleData.GetLength() == 6)
	{
		StoreWorkstationID();
		m_eStatus = STATUS_BANKCODE;
	}
	else
	{
		if (m_sSingleData.GetLength() < 6)
		{
			WK_TRACE_ERROR(_T("COM%i %s WSID too less data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		else
		{
			WK_TRACE_ERROR(_T("COM%i %s WSID too much data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		m_eStatus = STATUS_FAILED;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnStatusBankCode()
{
	if (m_sSingleData.GetLength() == 8)
	{
		StoreBankCode();
		m_eStatus = STATUS_ACCOUNT;
	}
	else
	{
		if (m_sSingleData.GetLength() < 8)
		{
			WK_TRACE_ERROR(_T("COM%i %s BankCode too less data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		else
		{
			WK_TRACE_ERROR(_T("COM%i %s BankCode too much data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		m_eStatus = STATUS_FAILED;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnStatusAccount()
{
	if (m_sSingleData.GetLength() == 10)
	{
		StoreAccount();
		m_eStatus = STATUS_VALUE;
	}
	else
	{
		if (m_sSingleData.GetLength() < 10)
		{
			WK_TRACE_ERROR(_T("COM%i %s Account too less data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		else
		{
			WK_TRACE_ERROR(_T("COM%i %s Account too much data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		m_eStatus = STATUS_FAILED;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnStatusValue()
{
	if (m_sSingleData.GetLength() == 4)
	{
		StoreValue();
		m_eStatus = STATUS_CAMERA;
	}
	else
	{
		if (m_sSingleData.GetLength() < 4)
		{
			WK_TRACE_ERROR(_T("COM%i %s Value too less data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		else
		{
			WK_TRACE_ERROR(_T("COM%i %s Value too much data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		m_eStatus = STATUS_FAILED;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnStatusCamera()
{
	if (m_sSingleData.GetLength() == 1)
	{
		TCHAR chCamera = m_sSingleData.GetAt(0);
		// valid are 1 and 2
		if (0x31 == chCamera || 0x32 == chCamera)
		{
			m_iCamera = _ttoi(&chCamera);
			m_eStatus = m_eStatus = STATUS_CRC;
		}
		// 0 and space are also valid as update
		else if (0x30 == chCamera || ASCII_SPACE == chCamera)
		{
			// update the data for both cameras 
			m_iCamera = 0;
			m_bUpdateOnly = TRUE;
			m_eStatus = m_eStatus = STATUS_CRC;
		}
		else
		{
			WK_TRACE_ERROR(_T("COM%i %s Camera WRONG Hex:%02x Char:%c\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), chCamera, chCamera);
			m_iCamera = -1;
			m_eStatus = STATUS_FAILED;
		}
	}
	else
	{
		if (m_sSingleData.GetLength() < 1)
		{
			WK_TRACE_ERROR(_T("COM%i %s Camera too less data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		else
		{
			WK_TRACE_ERROR(_T("COM%i %s Camera too much data len %i\n"),
					GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_sSingleData.GetLength());
		}
		m_eStatus = STATUS_FAILED;
	}
	ClearActualData();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::OnStatusCRC(BYTE byte)
{
	if (m_byXORCheckSum != byte) {
		// Falsche Checksumme
		WK_TRACE(_T("COM%i %s Prüfsumme fehlerhaft %d!=%d\n"),
					GetCOMNumber(),
					CSDIControl::NameOfEnum(m_Type),
					m_byXORCheckSum,
					byte);
		m_eStatus = STATUS_FAILED;
	}
	else {
		// Daten Ok
		m_eStatus = STATUS_OK;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::TransmitDataToSecurity()
{
	if (IsInterfaceOK())
	{
		if (IsCameraOK() && IsCameraExplicit())
		{
			WORD wAlarm = (WORD)(m_iInterfaceActual * 2 + m_iCamera);
			if (m_bUpdateOnly)
			{
				UpdateAlarm(wAlarm);
				// gf OOPS todo
				// Soll bei Kamera 2 auch der Alarm von Kamera 1 upgedatet werden?
				if (m_bPreviousCameraAlarmUpdate && (m_iCamera==2))
				{
					UpdateAlarm(wAlarm--);
				}
			}
			else
			{
				IndicateAlarm(wAlarm);
			}
		}
		else
		{
			// no valid camera, but update interface data
			UpdateInterfaceAlarms();
		}
	}
	else {
		WK_TRACE_ERROR(_T("COM%i %s WRONG Interface, NO alarm\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::IndicateAlarm(WORD wAlarm)
{
	// Bei Kamera 2 kann ein Update vom vorherigen Kamera 1-Alarm sinnvoll sein
	if (m_bPreviousCameraAlarmUpdate && (m_iCamera==2))
	{
		RefreshDataForSend(TRUE);
		UpdateAlarm(wAlarm--);
	}
	// Bei Kamera 1 wird immer ein ganz neuer Alarm angenommen und die alten Daten werden geloescht
	else
	{
		RefreshDataForSend(FALSE);
	}
	// Alarm wird als 1-32 uebermittelt
	if ( 1<=wAlarm && wAlarm<=32) {
		// Beim Alarm 1 muss nicht geschoben werden
		DWORD dwAlarmMask = 1L << (wAlarm-1);
		// Alarme indizieren
		GetInput()->IndicateAlarm(dwAlarmMask, m_DataFieldArrayToSend, TRUE);
		if (theApp.TraceiSTData()) {
			WK_TRACE(_T("COM%i %s Indicate Alarm:%u %s\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type),
						wAlarm,
						(LPCTSTR)m_DataFieldArrayToSend.ToString() );
		}

		// Nach einer gewissen Zeit Alarm wieder resetten
		Sleep(10);
		CIPCFields cfEmpty;
		GetInput()->IndicateAlarm(dwAlarmMask, cfEmpty, FALSE);
	}
	else {
		WK_TRACE_ERROR(_T("COM%i %s WRONG Alarm %u <1 or >32, not indicated\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type),
						wAlarm);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::UpdateAlarm(WORD wAlarm)
{
	RefreshDataForSend(TRUE);
	// Alarm wird als 1-32 uebermittelt
	if ( 1<=wAlarm && wAlarm<=32) {
		// Alarm wird als 1- uebermittelt, SubID beginnt aber bei 0
		WORD wSubID = (WORD)(wAlarm - 1);
		GetInput()->UpdateAlarm(wSubID, m_DataFieldArrayToSend);
		if (theApp.TraceiSTData()) {
			WK_TRACE(_T("COM%i %s Update Alarm:%u %s\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type),
						wAlarm,
						(LPCTSTR)m_DataFieldArrayToSend.ToString() );
		}
	}
	else {
		WK_TRACE_ERROR(_T("COM%i %s WRONG Alarm %u <1 or >32, not updated\n"),
						GetCOMNumber(),
						CSDIControl::NameOfEnum(m_Type),
						wAlarm);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::UpdateInterfaceAlarms()
{
	RefreshDataForSend(TRUE);
	for (int iCamera = 1 ; iCamera <= 2 ; iCamera++)
	{
		WORD wAlarm = (WORD)(m_iInterfaceActual * 2 + iCamera);
		// Alarm wird als 1-32 uebermittelt
		if ( 1<=wAlarm && wAlarm<=32) {
			// Alarm wird als 1- uebermittelt, SubID beginnt aber bei 0
			WORD wSubID = (WORD)(wAlarm - 1);
			GetInput()->UpdateAlarm(wSubID, m_DataFieldArrayToSend);
			if (theApp.TraceiSTData()) {
				WK_TRACE(_T("COM%i %s Update Alarm:%u %s\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type),
							wAlarm,
							(LPCTSTR)m_DataFieldArrayToSend.ToString() );
			}
		}
		else {
			WK_TRACE_ERROR(_T("COM%i %s WRONG Alarm %u <1 or >32, not updated\n"),
							GetCOMNumber(),
							CSDIControl::NameOfEnum(m_Type),
							wAlarm);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::RefreshDataForSend(BOOL bUpdateOnly)
{
	if (bUpdateOnly) {
		m_DataFieldArrayForInterface[m_iInterfaceActual].AddNoDuplicates(m_DataFieldArrayActual);
	}
	else {
		m_DataFieldArrayForInterface[m_iInterfaceActual] = m_DataFieldArrayActual;
	}
	m_DataFieldArrayToSend = m_DataFieldArrayForInterface[m_iInterfaceActual];
	SetStandardDataMembers();
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::SetStandardDataMembers()
{
	// Wenn Betrag vorhanden
	if (NULL != m_DataFieldArrayToSend.GetCIPCField(CIPCField::m_sfAmnt))
	{
		// Currency auf Standard setzen, falls nicht vorhanden
		if ( NULL == m_DataFieldArrayToSend.GetCIPCField(CIPCField::m_sfCurr) ) 
		{
			CIPCField* pField = new CIPCField(CIPCField::m_sfCurr,
												m_sCurrency.Left(3),
												'C');
			m_DataFieldArrayToSend.SafeAdd(pField);

		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordLandauDtp::ClearAllData()
{
	CSDIControlRecord::ClearAllData();
	m_iInterfaceActual = -1;
	m_iCamera = -1;
	m_bUpdateOnly = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordLandauDtp::IsInterfaceOK()
{
	BOOL bReturn = FALSE;
	if (0 <= m_iInterfaceActual && m_iInterfaceActual <= 3)
	{
		bReturn = TRUE;
	}
	else
	{
		WK_TRACE_ERROR(_T("COM%i %s Interface NOT OK %i\n"),
				GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_iInterfaceActual);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordLandauDtp::IsCameraOK()
{
	BOOL bReturn = FALSE;
	if (0 <= m_iCamera && m_iCamera <= 2)
	{
		bReturn = TRUE;
	}
	else
	{
		WK_TRACE_ERROR(_T("COM%i %s Camera NOT OK %i\n"),
				GetCOMNumber(), CSDIControl::NameOfEnum(m_Type), m_iInterfaceActual);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordLandauDtp::IsCameraExplicit()
{
	BOOL bReturn = FALSE;
	if (1 <= m_iCamera && m_iCamera <= 2)
	{
		bReturn = TRUE;
	}
	else
	{
		// not explicit, update all
	}
	return bReturn;
}
