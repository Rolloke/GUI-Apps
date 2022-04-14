/* GlobalReplace: SDIControlRecord_ --> Record */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: RecordConfigurable.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/RecordConfigurable.cpp $
// CHECKIN:		$Date: 2.01.06 10:05 $
// VERSION:		$Revision: 25 $
// LAST CHANGE:	$Modtime: 2.01.06 10:05 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$


#include "stdafx.h"
#include "SDIUnit.h"
#include "SDIInput.h"
#include "SDIControlRecord.h"
#include "RecordConfigurable.h"

// Einige Standardstrings definieren
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CRecordConfigurable::CRecordConfigurable(SDIControlType type,
															   int iCom,
															   BOOL bTraceAscii,
															   BOOL bTraceHex,
															   BOOL bTraceEvents)
	: CSDIControlRecord(type, iCom, 0,NULL,bTraceAscii, bTraceHex, bTraceEvents)
{
	m_pProtocolArray = NULL;	// is always created at record create
	m_pProtocol = NULL;			// may be NULL
	m_pProtocolData = NULL;		// may be NULL
	m_bStatusRequest = FALSE;
	m_wProtocolPositionAsAlarm = 0; // 0 indicates an invalid alarm
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRecordConfigurable::CreateRecord()
{
	BOOL bReturn = FALSE;
	// Basisklassenfunktion aufrufen
	if ( CSDIControlRecord::CreateRecord() ) {

		// variables Datenprotokoll einlesen
		CWK_Profile wkProfile;
		CString sSection;
		sSection.Format(szSectionFormat, GetCOMNumber() );

		// Unterstuetzte Protokolle einlesen
		m_pProtocolArray = new CSDIProtocolArray(sSection);
		if (m_pProtocolArray) {
			m_pProtocolArray->SetAutoDelete(TRUE);
			m_pProtocolArray->LoadFromRegistry(wkProfile);

			// Einstellungen der Protokolle tracen
			TraceProtocolData();

			bReturn = TRUE;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
CRecordConfigurable::~CRecordConfigurable()
{
	// virtual class has to close WK_RS232,
	// otherwise possible GP in virtual OnReceivedData()
	if (IsOpen())
	{
		WK_TRACE_ERROR(_T("%s RS232 NOT closed, object destroyed\n"),
						CSDIControl::NameOfEnum(m_Type));
		// Try to close Com
		Close();
	}
	WK_DELETE(m_pProtocolArray);
}
/////////////////////////////////////////////////////////////////////////////
void CRecordConfigurable::TraceProtocolData()
{
	CSDIProtocol* pProtocol = NULL;
	for (int i=0 ; i<m_pProtocolArray->GetSize() ; i++) {
		pProtocol = m_pProtocolArray->GetAt(i);
		WK_TRACE(_T("COM%i Protokoll<%s>\n"),
					GetCOMNumber(), (LPCTSTR)pProtocol->GetSample() );
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordConfigurable::CheckAndStoreVariableData()
{
	// Welches Protokoll passt zu den Daten
	CheckForProtocol();
	if (m_pProtocol) {
		// Wie werden die Daten erkannt
		switch ( CheckForProtocolDataIdentification() ) {
			case SDI_PROTOCOL_FORMAT_POSITION:
				CheckForProtocolDataByPosition();
				break;
			case SDI_PROTOCOL_FORMAT_SEPERATOR:
				CheckForProtocolDataBySeperator();
				break;
			case SDI_PROTOCOL_FORMAT_TAG:
				CheckForProtocolDataByTag();
				break;
			case SDI_PROTOCOL_FORMAT_UNKNOWN:
			default:
				WK_TRACE_ERROR(_T("%s Protocol format UNKNOWN\n"),
								CSDIControl::NameOfEnum(m_Type) );
				break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordConfigurable::CheckForProtocol()
{
	// das Protokoll ist noch unbekannt
	m_pProtocol = NULL;
	BOOL bFound = FALSE;
	int iNrCriteriaToCheckFor = 0;
	int iNrCriteriaPerformed = 0;
	m_wProtocolPositionAsAlarm = 0; // 0 indicates an invalid alarm
	for (int i=0 ; !bFound && i<m_pProtocolArray->GetSize() ; i++)
	{
		iNrCriteriaToCheckFor = 0;
		iNrCriteriaPerformed = 0;
		m_pProtocol = m_pProtocolArray->GetAt(i);
		// Soll nach Gesamtlaenge geprueft werden
		DWORD dwLength = m_pProtocol->GetTotalLength();
		if ( 0 < dwLength )
		{
			iNrCriteriaToCheckFor++;
			if ( (DWORD)m_sWholeData.GetLength() == dwLength )
			{
				// Laenge stimmt
				iNrCriteriaPerformed++;
			}
		}
		// Soll nach IdentifyString geprueft werden
		CString sIdentify = m_pProtocol->GetIdentifyString();
		if ( !sIdentify.IsEmpty() )
		{
			iNrCriteriaToCheckFor++;
			// GetPositionIdentifyString() beginnt bei 1, String-Index aber bei 0!!!
			if ( (DWORD)m_sWholeData.GetLength() >= m_pProtocol->GetPositionIdentifyString() )
			{
				int iRealIndex = m_sWholeData.Find(sIdentify) + 1;
				int iTempIndex = iRealIndex;
				// Wurde der Substring ueberhaupt gefunden
				if (iRealIndex > 0)
				{
					CString sRestData;
					// Solange die gefundene Position kleiner ist als die gesuchte
					while ( (DWORD)iRealIndex < m_pProtocol->GetPositionIdentifyString() )
					{
						sRestData = m_sWholeData.Mid(iRealIndex);
						iTempIndex = sRestData.Find(sIdentify);
						if (iTempIndex != -1)
						{
							iRealIndex += iTempIndex + 1;
						}
						else
						{
							break;
						}
					}
					if ( (DWORD)iRealIndex == m_pProtocol->GetPositionIdentifyString() )
					{
						// String gefunden und Position stimmt
						iNrCriteriaPerformed++;
					}
				}
			}
		}
		// Sind alle Kriterien erfuellt
		bFound = (iNrCriteriaPerformed == iNrCriteriaToCheckFor);
	}
	if (bFound)
	{
		m_wProtocolPositionAsAlarm = (WORD)i;
		WK_TRACE(_T("Found protocol %02u %s\n"), m_wProtocolPositionAsAlarm, m_pProtocol->GetName() );
	}
	else
	{
		m_pProtocol = NULL;
		WK_TRACE(_T("%s NO matching protocol found\n"),
						CSDIControl::NameOfEnum(m_Type) );
	}
}
/////////////////////////////////////////////////////////////////////////////
eSDIProtocolFormat CRecordConfigurable::CheckForProtocolDataIdentification()
{
	eSDIProtocolFormat eFormat = SDI_PROTOCOL_FORMAT_UNKNOWN;
	if (m_pProtocol) {
		eFormat = m_pProtocol->GetProtocolFormat();
	}
	return eFormat;
}
/////////////////////////////////////////////////////////////////////////////
void CRecordConfigurable::CheckForProtocolDataByPosition()
{
	if (m_pProtocol) {
		int iPosition = 0;
		int iLength = 0;
		int iSeperator = -1;
		for (int i=0 ; i< m_pProtocol->GetDataArray().GetSize() ; i++) {
			m_pProtocolData = m_pProtocol->GetDataArray().GetAt(i);
			iPosition = (int)m_pProtocolData->GetPosition();
			iLength = (int)m_pProtocolData->GetLength();
			// Ist die gefragte Position ueberhaupt in m_sWholeData vorhanden?
			// Soll heissen, ist m_sWholeData ueberhaupt lang genug?
			// iPosition beginnt bei 1!
			if ( iPosition <= m_sWholeData.GetLength() ) {
				m_sSingleData = m_sWholeData.Mid(iPosition-1);
				if (-1 == iLength  ) {
					iSeperator = m_sSingleData.Find( m_pProtocolData->GetSeperator() );
					if (-1 != iSeperator) {
						iLength = iSeperator;
					}
				}
				if (0 < iLength ) {
					m_sSingleData = m_sSingleData.Left(iLength);
					StoreData( m_pProtocolData->GetID() );
				}
				else {
					// Keine Daten zum Speichern, weil keine Laenge gegeben
				}
			}
			else {
				// Keine Daten zum Speichern
				// Diese Position ist im String nicht mehr enthalten
				// d.h. der String ist kuerzer
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordConfigurable::CheckForProtocolDataBySeperator()
{
	// NOT YET IMPLEMENTED
	WK_TRACE_ERROR(_T("%s CheckForProtocolDataBySeperator NOT YET IMPLEMENTED\n"),
					CSDIControl::NameOfEnum(m_Type) );
	if (m_pProtocol) {
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordConfigurable::CheckForProtocolDataByTag()
{
	// NOT YET IMPLEMENTED
	WK_TRACE_ERROR(_T("%s CheckForProtocolDataByTag NOT YET IMPLEMENTED\n"),
					CSDIControl::NameOfEnum(m_Type) );
	if (m_pProtocol) {
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordConfigurable::StoreData(CSecID idParam)
{
	if (   m_pProtocol
		&& m_pProtocol->GetReadReverse()
		)
	{
		m_sSingleData.MakeReverse();
	}
	// First check for old datafield params ...
	DWORD dwID = idParam.GetID();
	switch (dwID)
	{
		case PARAM_ACCOUNT:
			StoreAccount();
			break;
		case PARAM_BANKCODE:
			StoreBankCode();
			break;
		case PARAM_VALUE:
			StoreValue();
			break;
		case PARAM_CURRENCY:
			StoreCurrency();
			break;
		case PARAM_WORKSTATION:
			StoreWorkstationID();
			break;
		case PARAM_TRANSAKTION:
			StoreTAN();
			break;
		case PARAM_DATE:
			if (m_pProtocolData)
			{
				StoreDate(m_pProtocolData->GetFormat());
			}
			else
			{
				WK_TRACE_ERROR(_T("StoreData no ProtocolData"));
			}
			break;
		case PARAM_TIME:
			if (m_pProtocolData)
			{
				StoreTime(m_pProtocolData->GetFormat());
			}
			else
			{
				WK_TRACE_ERROR(_T("StoreData no ProtocolData"));
			}
			break;
		case PARAM_UNKNOWN:
		default:
			// Wird ignoriert
			break;
	}
	// .. then check for new datafield params
	if (HIWORD(dwID) == SECID_PARAMS_NEW)
	{
		StoreNewData(LOWORD(dwID));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordConfigurable::StoreNewData(WORD wID)
{
	if (m_pProtocolData)
	{
		CString sFieldName;
		sFieldName.Format(_T("DBD_%04u"), wID);
		if (m_pProtocolData->IsDate())
		{
			StoreDate(sFieldName, m_pProtocolData->GetFormat());
		}
		else if (m_pProtocolData->IsTime())
		{
			StoreTime(sFieldName, m_pProtocolData->GetFormat());
		}
		else
		{
			StoreNewDataBaseFields(sFieldName);
		}
	}
}
