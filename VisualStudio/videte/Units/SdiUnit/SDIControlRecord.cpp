/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIControlRecord.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/SDIControlRecord.cpp $
// CHECKIN:		$Date: 7/20/06 2:20p $
// VERSION:		$Revision: 67 $
// LAST CHANGE:	$Modtime: 7/20/06 2:02p $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "resource.h"
#include "SDIUnit.h"
#include "SDIInput.h"
#include "SDIControlRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CSDIControlRecord::CSDIControlRecord(SDIControlType type,
									 int iCom,
									 int iPort,
									 LPCTSTR szIPAddress,
									 BOOL bTraceAscii,
									 BOOL bTraceHex,
									 BOOL bTraceEvents)
	: CWK_RS232(iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
	m_Type = type;
	m_iPort = iPort;
	m_sIPAddress = szIPAddress;
	// m_Inputs;
	m_iMaxBufferLen = 1024;
	m_iReceivedTotal = 0,
	m_iDataLen = 0;
	m_byXORCheckSum = 0;
	m_iLenDataActual = 0;
	m_bStatusRequest = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CSDIControlRecord::~CSDIControlRecord()
{
	// virtual class has to close WK_RS232,
	// otherwise possible GP in virtual OnReceivedData()
	if (IsOpen())
	{
		WK_TRACE_ERROR(_T("%s RS232 NOT closed, object destroyed\n"),
						CSDIControl::NameOfEnum(m_Type));
		// Try to close Com
		CWK_RS232::Close();
	}
	m_Inputs.DeleteAll();
	m_DataFieldArrayActual.DeleteAll();
	m_DataFieldArrayToSend.DeleteAll();
	m_Type = SDICT_UNKNOWN;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::Close()
{
	if (IsOpen())
	{
		CWK_RS232::Close();
	}
	CAsyncSocket::Close();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::InitializeResponses()
{
	// Wird von der virtuellen Klasse ausgewertet, wenn nötig
	// Nichts zu tun
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::SwitchToConfigMode(const CString& sPath, BOOL bReceive)
{
	// Wird von der virtuellen Klasse ausgewertet, wenn nötig
	// Nichts zu tun
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIControlRecord::IsValid() const
{
	return IsOpen();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIControlRecord::UpdateAlarms()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIControlRecord::CreateRecord()
{
	BOOL bReturn = FALSE;
	if ( InitializeStandardMembers() ) 
	{
		// Antworten initialisieren
		InitializeResponses();
		// Input anlegen
		CString sSMName = SM_SDIUNIT_INPUT;
		CString sInterface;
		int iCom = GetCOMNumber();
		int iPort = GetPortNumber();
		if (iCom>0)
		{
			sInterface.Format(_T("%i"), iCom ); 
		}
		else if (iPort>0)
		{
			sInterface.Format(_T("UDP%i"), iPort ); 
		}
		sSMName += sInterface;
		CSDIInput* pInput = new CSDIInput(sSMName, this);
		if ( pInput ) 
		{
			m_Inputs.Add(pInput);
			WK_TRACE(_T("Created SDIControl Typ=%s COM=%d %s\n"),CSDIControl::NameOfEnum(m_Type), GetCOMNumber(), (LPCTSTR)m_sComment);
			Sleep(100);
			bReturn = TRUE;
		}
		else {
			WK_TRACE_ERROR(_T("CreateRecord FAILED, no Input Typ=%s COM=%d %s\n"),
				CSDIControl::NameOfEnum(m_Type), GetCOMNumber(), (LPCTSTR)m_sComment);
		}
	}
	else {
		WK_TRACE_ERROR(_T("CreateRecord FAILED, InitializeStandardMembers Typ=%s COM=%d %s\n"),
			CSDIControl::NameOfEnum(m_Type), GetCOMNumber(), (LPCTSTR)m_sComment);
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIControlRecord::InitializeStandardMembers()
{
	BOOL bReturn = FALSE;
	CWK_Profile wkProfile;
	CString	sSection;
	sSection.Format(szSectionFormat, GetCOMNumber() );
	m_sCurrency = wkProfile.GetString(sSection, szEntryCurrency, _T("   "));
	m_sComment = wkProfile.GetString(sSection, szEntryComment, _T(""));
	bReturn = TRUE;

	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIControlRecord::OpenCom(DWORD dwBaudRate,
								BYTE byDataBits,
								BYTE byParity,
								BYTE byStopBits)
{
	BOOL bReturn = FALSE;
	// ggf. variables Datenprotokoll einlesen und RS232 oeffnen
	CWK_Profile wkProfile;
	CString	sSection;
	sSection.Format(szSectionFormat, GetCOMNumber() );
	CComParameters ComParams(sSection, dwBaudRate, byDataBits, byParity, byStopBits);
	ComParams.LoadFromRegistry(wkProfile);

	WK_TRACE(_T("COM%i should be opened with %s\n"), GetCOMNumber(), ComParams.GetParamsString());
	if ( Open(ComParams) ) {
		Sleep(100);
		bReturn = TRUE;
	}
	else {
		CString sText;
		sText.Format(IDS_SDI_CONTROL_OPEN_COM_FAILED,
						GetCOMNumber(), (int)m_Type, CSDIControl::NameOfEnum(m_Type));
		CWK_RunTimeError rtError(WAI_SDIUNIT, REL_ERROR, RTE_CONFIGURATION, sText);
		rtError.Send();
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::ClearActualData()
{
	m_sWholeData.Empty();
	m_sSingleData.Empty();
	m_iLenDataActual = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::ClearAllData()
{
	m_iReceivedTotal = 0;
	m_iLenDataActual = 0;
	m_byXORCheckSum = 0;
	m_iDataLen = 0;
	m_sWholeData.Empty();
	m_sSingleData.Empty();
	m_DataFieldArrayActual.SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::StoreDate(eSDIDataFormat eFormat)
{
	StoreDate(CIPCField::m_sfDate, eFormat);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::StoreDate(CString sFieldName, eSDIDataFormat eFormat)
{
	CString sDate;	// im Format YYYYMMDD
	switch (eFormat) {
		case SDI_DATA_FORMAT_DDMMYY:
			if (6 <= m_sSingleData.GetLength() ) {
				sDate = GetCenturyFor2DigitYear(m_sSingleData.Mid(4,2))
						+ m_sSingleData.Mid(4,2)
						+ m_sSingleData.Mid(2,2)
						+ m_sSingleData.Left(2);
			}
			break;
		case SDI_DATA_FORMAT_DD_MM_YY:
			if (8 <= m_sSingleData.GetLength() ) {
				sDate = GetCenturyFor2DigitYear(m_sSingleData.Mid(6,2))
						+ m_sSingleData.Mid(6,2)
						+ m_sSingleData.Mid(3,2)
						+ m_sSingleData.Left(2);
			}
			break;
		case SDI_DATA_FORMAT_DDMMYYYY:
			if (8 <= m_sSingleData.GetLength() ) {
				sDate = m_sSingleData.Mid(4,4)
						+ m_sSingleData.Mid(2,2)
						+ m_sSingleData.Left(2);
			}
			break;
		case SDI_DATA_FORMAT_DD_MM_YYYY:
			if (10 <= m_sSingleData.GetLength() ) {
				sDate = m_sSingleData.Mid(6,4)
						+ m_sSingleData.Mid(3,2)
						+ m_sSingleData.Left(2);
			}
			break;
		case SDI_DATA_FORMAT_MMDDYY:
			if (6 <= m_sSingleData.GetLength() ) {
				sDate = GetCenturyFor2DigitYear(m_sSingleData.Mid(4,2))
						+ m_sSingleData.Mid(4,2)
						+ m_sSingleData.Left(2)
						+ m_sSingleData.Mid(2,2);
			}
			break;
		case SDI_DATA_FORMAT_MM_DD_YY:
			if (8 <= m_sSingleData.GetLength() ) {
				sDate = GetCenturyFor2DigitYear(m_sSingleData.Mid(6,2))
						+ m_sSingleData.Mid(6,2)
						+ m_sSingleData.Left(2)
						+ m_sSingleData.Mid(3,2);
			}
			break;
		case SDI_DATA_FORMAT_MMDDYYYY:
			if (8 <= m_sSingleData.GetLength() ) {
				sDate = m_sSingleData.Mid(4,4)
						+ m_sSingleData.Left(2)
						+ m_sSingleData.Mid(2,2);
			}
			break;
		case SDI_DATA_FORMAT_MM_DD_YYYY:
			if (10 <= m_sSingleData.GetLength() ) {
				sDate = m_sSingleData.Mid(6,4)
						+ m_sSingleData.Left(2)
						+ m_sSingleData.Mid(3,2);
			}
			break;
		case SDI_DATA_FORMAT_YYMMDD:
			if (6 <= m_sSingleData.GetLength() ) {
				sDate = GetCenturyFor2DigitYear(m_sSingleData.Left(2))
						+ m_sSingleData.Left(2)
						+ m_sSingleData.Mid(2,2)
						+ m_sSingleData.Mid(4,2);
			}
			break;
		case SDI_DATA_FORMAT_YY_MM_DD:
			if (8 <= m_sSingleData.GetLength() ) {
				sDate = GetCenturyFor2DigitYear(m_sSingleData.Left(2))
						+ m_sSingleData.Left(2)
						+ m_sSingleData.Mid(3,2)
						+ m_sSingleData.Mid(6,2);
			}
			break;
		case SDI_DATA_FORMAT_YYYYMMDD:
			if (8 <= m_sSingleData.GetLength() ) {
				sDate = m_sSingleData.Left(4)
						+ m_sSingleData.Mid(4,2)
						+ m_sSingleData.Mid(6,2);
			}
			break;
		case SDI_DATA_FORMAT_YYYY_MM_DD:
			if (10 <= m_sSingleData.GetLength() ) {
				sDate =   m_sSingleData.Left(4)
						+ m_sSingleData.Mid(5,2)
						+ m_sSingleData.Mid(8,2);
			}
			break;
		case SDI_DATA_FORMAT_DD_MMM_YY:
			if (9 == m_sSingleData.GetLength())
			{
				CString sMonthDigits;
				CString sYear = m_sSingleData.Mid(7,2);
				if (Get2DigitMonthFromString(m_sSingleData.Mid(3,3), sMonthDigits))
				{
					sDate =   GetCenturyFor2DigitYear(sYear)
							+ sYear
							+ sMonthDigits
							+ m_sSingleData.Left(2);
				}
			}
			break;
		case SDI_DATA_FORMAT_DD_MMM_YYYY:
			if (11 == m_sSingleData.GetLength() )
			{
				CString sMonthDigits;
				if (Get2DigitMonthFromString(m_sSingleData.Mid(3,3), sMonthDigits))
				{
					sDate =   m_sSingleData.Mid(7,4)
							+ sMonthDigits
							+ m_sSingleData.Left(2);
				}
			}
			break;
		default:
			WK_TRACE_ERROR(_T("%s Wrong Format for Date\n"),
							CSDIControl::NameOfEnum(m_Type) );
			break;
	}
	if (FALSE==sDate.IsEmpty()) {
		CIPCField cfField(sFieldName, sDate, 'D');
		m_DataFieldArrayActual.AddNoDuplicates(cfField);
	}
	else {
		WK_TRACE_ERROR(_T("%s Date %s not stored %s\n"),
						CSDIControl::NameOfEnum(m_Type), m_sSingleData);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::StoreTime(eSDIDataFormat eFormat)
{
	StoreTime(CIPCField::m_sfTime, eFormat);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::StoreTime(CString sFieldName, eSDIDataFormat eFormat)
{
	CString sTime;	// im Format HHMMSS
	switch (eFormat) {
		case SDI_DATA_FORMAT_HHMM:
			if (4 <= m_sSingleData.GetLength() ) {
				sTime = m_sSingleData.Left(4) + _T("00");
			}
			break;
		case SDI_DATA_FORMAT_HH_MM:
			if (5 <= m_sSingleData.GetLength() ) {
				sTime = m_sSingleData.Left(2) + m_sSingleData.Mid(3,2) + _T("00");
			}
			break;
		case SDI_DATA_FORMAT_HHMMSS:
			if (6 <= m_sSingleData.GetLength() ) {
				sTime = m_sSingleData.Left(6);
			}
			break;
		case SDI_DATA_FORMAT_HH_MM_SS:
			if (8 <= m_sSingleData.GetLength() ) {
				sTime = m_sSingleData.Left(2)
						+ m_sSingleData.Mid(3,2)
						+ m_sSingleData.Mid(6,2);
			}
			break;
		default:
			WK_TRACE_ERROR(_T("%s Wrong Format for Time\n"),
							CSDIControl::NameOfEnum(m_Type) );
			break;
	}
	if (FALSE==sTime.IsEmpty()) {
		CIPCField cfField(CIPCField::m_sfTime, sTime, 'T');
		m_DataFieldArrayActual.AddNoDuplicates(cfField);
	}
	else {
		WK_TRACE_ERROR(_T("%s Time %s not stored\n"),
						CSDIControl::NameOfEnum(m_Type), m_sSingleData);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::StoreAccount()
{
	CIPCField cfField(CIPCField::m_sfKtNr, m_sSingleData, 'C');
	m_DataFieldArrayActual.AddNoDuplicates(cfField);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::StoreBankCode()
{
	CIPCField cfField(CIPCField::m_sfBcNr, m_sSingleData, 'N');
	m_DataFieldArrayActual.AddNoDuplicates(cfField);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::StoreTAN()
{
	CIPCField cfField(CIPCField::m_sfTaNr, m_sSingleData, 'N');
	m_DataFieldArrayActual.AddNoDuplicates(cfField);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::StoreWorkstationID()
{
	CIPCField cfField(CIPCField::m_sfGaNr, m_sSingleData, 'N');
	m_DataFieldArrayActual.AddNoDuplicates(cfField);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::StoreValue()
{
// Non Digits ',' and '.' allowed
	CheckForNonDigitsInValue();
	CIPCField cfField(CIPCField::m_sfAmnt, m_sSingleData, 'C');
	m_DataFieldArrayActual.AddNoDuplicates(cfField);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::StoreCurrency()
{
	CIPCField cfField(CIPCField::m_sfCurr, m_sSingleData, 'C');
	m_DataFieldArrayActual.AddNoDuplicates(cfField);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::StoreNewDataBaseFields(CString sFieldName)
{
	CIPCField cfField(sFieldName, m_sSingleData, 'C');
	m_DataFieldArrayActual.AddNoDuplicates(cfField);
}
///////////////////////////////////////////////////////////////////////////////
//void CSDIControlRecord::StoreProcessID()
//{
//	CIPCField cfField(CIPCField::m_sfProcessId, m_sSingleData, 'N');
//	m_DataFieldArrayActual.AddNoDuplicates(cfField);
//}
///////////////////////////////////////////////////////////////////////////////
//void CSDIControlRecord::StoreFirstName()
//{
//	CIPCField cfField(CIPCField::m_sfFirstName, m_sSingleData, 'C');
//	m_DataFieldArrayActual.AddNoDuplicates(cfField);
//}
///////////////////////////////////////////////////////////////////////////////
//void CSDIControlRecord::StoreLastName()
//{
//	CIPCField cfField(CIPCField::m_sfLastName, m_sSingleData, 'C');
//	m_DataFieldArrayActual.AddNoDuplicates(cfField);
//}
///////////////////////////////////////////////////////////////////////////////
//void CSDIControlRecord::StoreCustomerID()
//{
//	CIPCField cfField(CIPCField::m_sfCustomerId, m_sSingleData, 'N');
//	m_DataFieldArrayActual.AddNoDuplicates(cfField);
//}
///////////////////////////////////////////////////////////////////////////////
//void CSDIControlRecord::StoreBoxID()
//{
//	CheckForNonDigitsInValue();
//	CIPCField cfField(CIPCField::m_sfBoxId, m_sSingleData, 'N');
//	m_DataFieldArrayActual.AddNoDuplicates(cfField);
//}
///////////////////////////////////////////////////////////////////////////////
//void CSDIControlRecord::StoreIstBetrag()
//{
//	CIPCField cfField(CIPCField::m_sfIst, m_sSingleData, 'W');
//	m_DataFieldArrayActual.AddNoDuplicates(cfField);
//}
///////////////////////////////////////////////////////////////////////////////
//void CSDIControlRecord::StoreSollBetrag()
//{
//	CIPCField cfField(CIPCField::m_sfSoll, m_sSingleData, 'W');
//	m_DataFieldArrayActual.AddNoDuplicates(cfField);
//}
///////////////////////////////////////////////////////////////////////////////
//void CSDIControlRecord::StoreDifferenzBetrag()
//{
//	CIPCField cfField(CIPCField::m_sfDifferenz, m_sSingleData, 'W');
//	m_DataFieldArrayActual.AddNoDuplicates(cfField);
//}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::RefreshDataForSend()
{
	if ( CheckForNewAlarmData() ) {
		m_wFormerAlarmsArray.RemoveAll();
		m_DataFieldArrayToSend = m_DataFieldArrayActual;
	}
	else {
		m_DataFieldArrayToSend.AddNoDuplicates(m_DataFieldArrayActual);
	}
	SetStandardDataMembers();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::SetStandardDataMembers()
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
void CSDIControlRecord::IndicateAlarm(WORD wAlarm,int iKind/*=2*/)
{
	// Alarm wird als 1-32 uebermittelt
	if (1 <= wAlarm) 
	{
		// Erstmal Daten aktualisieren
		RefreshDataForSend();

		if (iKind==1 || iKind == 2)
		{
			if (wAlarm<=32)
			{
				// Beim Alarm 1 muss nicht geschiftet werden
				DWORD dwAlarmMask = 1L << (wAlarm-1);
				// Alarme indizieren
				GetInput()->IndicateAlarm(dwAlarmMask, m_DataFieldArrayToSend, TRUE);
			}
			else
			{
				GetInput()->IndicateAlarm(wAlarm-1,TRUE,m_DataFieldArrayToSend);
			}
			if (UpdateAlarms())
			{
				// Vorherige Alarme updaten, es koennten ja neue Daten gekommen sein
				UpdateFormerAlarms();
				// Jetzt ist dieser Alarm auch ein 'vorheriger'
				m_wFormerAlarmsArray.Add(wAlarm);
			}
		}


		if (iKind == 2 || iKind == 0)
		{
			// Nach einer gewissen Zeit Alarm wieder resetten
			Sleep(10);
			CIPCFields cfEmpty;

			if (wAlarm<=32)
			{
				DWORD dwAlarmMask = 1L << (wAlarm-1);
				GetInput()->IndicateAlarm(dwAlarmMask, cfEmpty, FALSE);
			}
			else
			{
				GetInput()->IndicateAlarm(wAlarm-1,FALSE,cfEmpty);
			}
		}
		if (theApp.TraceiSTData()) 
		{
			WK_TRACE(_T("%s Indicate Alarm:%u %s\n"), CSDIControl::NameOfEnum(m_Type),
				wAlarm,
				(LPCTSTR)m_DataFieldArrayToSend.ToString() );
		}
	}
	else 
	{
		WK_TRACE_ERROR(_T("%s WRONG Alarm %u <1 or >32, not indicated\n"),
						CSDIControl::NameOfEnum(m_Type), wAlarm);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::UpdateFormerAlarms()
{
	WORD wAlarm;
	for (int i=0 ; i<m_wFormerAlarmsArray.GetSize() ; i++) {
		wAlarm = m_wFormerAlarmsArray.GetAt(i);
		RealUpdateAlarm(wAlarm);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::UpdateAlarm(WORD wAlarm)
{
	// Alarm wird als 1-32 uebermittelt
	if ( 1<=wAlarm && wAlarm<=32) {
		RefreshDataForSend();
		RealUpdateAlarm(wAlarm);
	}
	else {
		WK_TRACE_ERROR(_T("%s WRONG Alarm %u <1 or >32, not updated\n"),
						CSDIControl::NameOfEnum(m_Type), wAlarm);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::RealUpdateAlarm(WORD wAlarm)
{
	// Alarm wird als 1-32 uebermittelt, SubID beginnt aber bei 0
	WORD wSubID = (WORD)(wAlarm - 1);
	GetInput()->UpdateAlarm(wSubID, m_DataFieldArrayToSend);
	if (theApp.TraceiSTData()) {
		WK_TRACE(_T("%s Update Alarm:%u %s\n"), CSDIControl::NameOfEnum(m_Type),
											wAlarm,
											(LPCTSTR)m_DataFieldArrayToSend.ToString() );
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::SendACK()
{
	Write( (LPVOID)m_byaACK.GetData(), m_byaACK.GetSize() );
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::SendNAK()
{
	Write( (LPVOID)m_byaNAK.GetData(), m_byaNAK.GetSize() );
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::RespondStatusRequest()
{
	// Jede Statusanforderung muss beantwortet werden
	// Vorerst wird >Alles ok< vorgespiegelt
	// OOPS todo muss evtl. noch veraendert werden,
	// z.B. um Kameraausfaelle anzuzeigen
	// OOPS Response wird z.Z. nur einmal bei Programmstart berechnet, um Zeit zu sparen

	Write( (LPVOID)m_byaStatusResponse.GetData(), m_byaStatusResponse.GetSize() );

	m_bStatusRequest = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIControlRecord::CheckForNonDigitsInValue()
{
	if (theApp.IgnoreNonDigitsInValue()) {
		CString sOnlyDigits;
		TCHAR chChar;
		for (int i=0 ; i<m_sSingleData.GetLength() ; i++) {
			chChar = m_sSingleData.GetAt(i);
			if ( isdigit(chChar) ) {
				sOnlyDigits += chChar;
			}
		}
		m_sSingleData = sOnlyDigits;
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CSDIControlRecord::GetCenturyFor2DigitYear(CString s2DigitYear)
{
	int iYear = _ttoi(s2DigitYear);
	CString sCentury = (iYear > 70)?_T("19"):_T("20");
	return sCentury;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIControlRecord::Get2DigitMonthFromString(CString sMonthString, CString& sMonth2Digits)
{
	BOOL bReturn = FALSE;
	sMonthString.MakeUpper();
	if (   sMonthString.Find(_T("JAN")) != -1
		)
	{
		sMonth2Digits = _T("01");
		bReturn = TRUE;
	}
	else if (   (sMonthString.Find(_T("FEB")) != -1)
			)
	{
		sMonth2Digits = _T("02");
		bReturn = TRUE;
	}
	else if (   (sMonthString.Find(_T("MÄR")) != -1)
			 || (sMonthString.Find(_T("MAR")) != -1)
			 || (sMonthString.Find(_T("MRZ")) != -1)
			)
	{
		sMonth2Digits = _T("03");
		bReturn = TRUE;
	}
	else if (   (sMonthString.Find(_T("APR")) != -1)
			)
	{
		sMonth2Digits = _T("04");
		bReturn = TRUE;
	}
	else if (   (sMonthString.Find(_T("MAI")) != -1)
			 || (sMonthString.Find(_T("MAY")) != -1)
			)
	{
		sMonth2Digits = _T("05");
		bReturn = TRUE;
	}
	else if (   (sMonthString.Find(_T("JUN")) != -1)
			)
	{
		sMonth2Digits = _T("06");
		bReturn = TRUE;
	}
	else if (   (sMonthString.Find(_T("JUL")) != -1)
			)
	{
		sMonth2Digits = _T("07");
		bReturn = TRUE;
	}
	else if (   (sMonthString.Find(_T("AUG")) != -1)
			)
	{
		sMonth2Digits = _T("08");
		bReturn = TRUE;
	}
	else if (   (sMonthString.Find(_T("SEP")) != -1)
			)
	{
		sMonth2Digits = _T("09");
		bReturn = TRUE;
	}
	else if (   (sMonthString.Find(_T("OKT")) != -1)
			 || (sMonthString.Find(_T("OCT")) != -1)
			)
	{
		sMonth2Digits = _T("10");
		bReturn = TRUE;
	}
	else if (   (sMonthString.Find(_T("NOV")) != -1)
			)
	{
		sMonth2Digits = _T("11");
		bReturn = TRUE;
	}
	else if (   (sMonthString.Find(_T("DEZ")) != -1)
			 || (sMonthString.Find(_T("DEC")) != -1)
			)
	{
		sMonth2Digits = _T("12");
		bReturn = TRUE;
	}
	if (!bReturn)
	{
		WK_TRACE_ERROR(_T("%s UNKNOWN Format for Month %s\n"),
						CSDIControl::NameOfEnum(m_Type), sMonthString);
	}
	return bReturn;
}

void CSDIControlRecord::OnReceive(int nErrorCode)
{
	// TODO read data from socket
	// call OnReceivedData
	DWORD dwLen = 2048;
	DWORD dwReceived = 0;
	PBYTE pBuffer = new BYTE[dwLen];

	dwReceived = Receive(pBuffer,dwLen);
	if (   0<dwReceived
		&& dwReceived<dwLen)
	{
		if (GetTraceRS232DataAscii() || GetTraceRS232DataHex())
		{
			TraceData(pBuffer,dwReceived,TRUE);
		}
		OnReceivedData(pBuffer,dwReceived);
	}
	else
	{
		DWORD dwError = WSAGetLastError();
		WK_TRACE_ERROR(_T("cannot receive UDP data %d\n"),m_iPort,GetLastErrorString(dwError));
	}


	WK_DELETE_ARRAY(pBuffer);
}
