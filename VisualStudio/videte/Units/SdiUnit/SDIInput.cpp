/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIInput.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/SDIInput.cpp $
// CHECKIN:		$Date: 7/20/06 2:20p $
// VERSION:		$Revision: 25 $
// LAST CHANGE:	$Modtime: 7/20/06 12:41p $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "messages.h"
#include "SDIUnit.h"
#include "SDIInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
extern CSDIUnitApp theApp;

//////////////////////////////////////////////////////////////////////////////////////
// Constructor
CSDIInput::CSDIInput(LPCTSTR shmName, CSDIControlRecord* pControlRecord)
	: CIPCInput(shmName,FALSE)
{
	m_wGroupID = SECID_NO_GROUPID;
	m_dwOpenMask = 0;
	m_dwAlarmState = 0;
	m_pControlRecord = pControlRecord;
	m_dwServerData = SECID_NO_ID;
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
// Destructor
CSDIInput::~CSDIInput()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
// Initializing and connection code
//////////////////////////////////////////////////////////////////////////////////////
// 1. called overridden method
//
//////////////////////////////////////////////////////////////////////////////////////
// 2. called overridden method
//
void CSDIInput::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID; 
	DoConfirmSetGroupID(wGroupID);
};
//////////////////////////////////////////////////////////////////////////////////////
// 3. called overridden method
//
void CSDIInput::OnRequestHardware(WORD wGroupNr)
{
	// OOPS todo COM abfragen
	DoConfirmHardware(m_wGroupID,0);
}
//////////////////////////////////////////////////////////////////////////////////////
// 4. called overridden method
//
void CSDIInput::OnRequestSetEdge(WORD wGroupID,DWORD dwEdgeMask)	// 1 positive, 0 negative
{
	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}
//////////////////////////////////////////////////////////////////////////////////////
// 5. called overridden method
//
void CSDIInput::OnRequestSetFree(WORD wGroupID,DWORD dwOpenMask)	// 1 open, 0 closed
{
	m_dwOpenMask = dwOpenMask;
	DoConfirmFree(m_wGroupID, m_dwOpenMask);
}
//////////////////////////////////////////////////////////////////////////////////////
// 5. called overridden method
//
void CSDIInput::OnRequestReset(WORD wGroupID)
{
	WK_TRACE(_T("CSDIInput[%s]::OnRequestReset %u %u\n"),GetShmName(),wGroupID,m_wGroupID);
	DoConfirmReset(m_wGroupID);
	m_dwAlarmState = 0;
}
//////////////////////////////////////////////////////////////////////////////////////
// Running Application code
//////////////////////////////////////////////////////////////////////////////////////
void CSDIInput::OnRequestAlarmState(WORD wGroupID)
{
	DoConfirmAlarmState(m_wGroupID,m_dwAlarmState);
}
//////////////////////////////////////////////////////////////////////////////////////
// Disconnecting code
//
void CSDIInput::OnRequestDisconnect()
{
	WK_TRACE(_T("CSDIInput[%s]::OnRequestDisconnect\n"),GetShmName());
	DelayedDelete();
	if ( AfxGetMainWnd() ) {
		AfxGetMainWnd()->PostMessage(WM_RESET_SDI,0,0);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIInput::OnRequestGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						DWORD dwServerData
					)
{
	WK_TRACE(_T("CSDIInput[%s]::OnRequestGetValue Key:%s\n"),GetShmName(), sKey);
	// OOPS gf todo muessen wir mitteilen
	if (sKey==SDI_KEY_CONTROLRECORD_TYP_COMMENT) {
		// Typ und Kommentar des ControlRecords liefern
		CString sValue = CSDIControl::NameOfEnum(m_pControlRecord->GetType());
		sValue += _T("=");
		sValue += m_pControlRecord->GetComment();
		DoConfirmGetValue(id, sKey, sValue, dwServerData);
	}
	else if (sKey==SDI_KEY_CONTROLRECORD_TYP) {
		// Typ des ControlRecords liefern
		CString sTyp = CSDIControl::NameOfEnum(m_pControlRecord->GetType());
		DoConfirmGetValue(id, sKey, sTyp, dwServerData);
	}
	else if (sKey==SDI_KEY_CONTROLRECORD_COMMENT) {
		// Kommentar des ControlRecords liefern
		CString sComment = m_pControlRecord->GetComment();
		DoConfirmGetValue(id, sKey, sComment, dwServerData);
	}
	else {
		WK_TRACE_WARNING(_T("OnRequestGetValue Key NOT HANDLED\n"));
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIInput::OnRequestSetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						)
{
	// id wird ignoriert, da nur die Gruppe interessiert
	m_dwServerData = dwServerData;
	m_sKey = sKey;

	// OOPS gf todo einfach setzen ohne Confirm
	if (SDI_KEY_START_XMODEM_TRANSMITTER==sKey) {
		// XModem-Modus Transmitter starten
		m_pControlRecord->SwitchToConfigMode(sValue, TRUE);
	}
	else if (SDI_KEY_START_XMODEM_RECEIVER==sKey) {
		// XModem-Modus Receiver starten
		m_pControlRecord->SwitchToConfigMode(sValue, FALSE);
	}
	else {
		WK_TRACE(_T("CSDIInput[%s]::OnRequestSetValue NOT HANDLED Key:%s Value:%s dwServer:%08x\n"),
						GetShmName(), sKey, sValue, dwServerData);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIInput::IndicateAlarm(DWORD dwMask, const CIPCFields& DataFields, BOOL bAlarm/*=TRUE*/)
{
	DWORD dwOpenAlarms = m_dwOpenMask & dwMask;
	DWORD dwOldState = m_dwAlarmState;
	if (bAlarm)
	{
		m_dwAlarmState |= dwOpenAlarms;
	}
	else
	{
		m_dwAlarmState &= ~dwOpenAlarms;
	}

	DWORD dwChangeMask = dwOldState^m_dwAlarmState;

	DoIndicateAlarmFieldsState(m_wGroupID,
								m_dwAlarmState,				// 1 high, 0 low
								dwChangeMask,				// 1 changed, 0 unchanged
								DataFields
								);
	TRACE(_T("CSDIInput[%s] Alarms indicated %s\n"),
				GetShmName(), (LPCTSTR)BinaryString(dwOpenAlarms, 32) );
	if (theApp.TraceRS232Events()) {
		WK_STAT_PEAK(_T("Alarm"), 1, _T("IndicateAlarm"));
	}

	DWORD dwSkippedMask = (m_dwOpenMask ^ dwMask) & dwMask;
	if (0 != dwSkippedMask) {
		TRACE(_T("CSDIInput[%s] Alarms not indicated %s\n"),
					GetShmName(), (LPCTSTR)BinaryString(dwSkippedMask, 32) );
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIInput::IndicateAlarm(WORD wSubID, BOOL bAlarm, const CIPCFields& DataFields)
{
	CSecID id(m_wGroupID,wSubID);
	DoIndicateAlarm(id,bAlarm,DataFields);
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CSDIInput::GetAlarmState(WORD wSubID)
{
	DWORD dwMask = 1 << (wSubID);
	return (m_dwAlarmState & dwMask) >0;
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIInput::UpdateAlarm(WORD wSubID, const CIPCFields& DataFields)
{
	DWORD dwMask = 1 << (wSubID);
	DWORD dwOpenAlarms = m_dwOpenMask & dwMask;
	if (0 != dwOpenAlarms) {
		CSecID id(m_wGroupID, wSubID);
		DoIndicateAlarmFieldsUpdate(id, DataFields);
		if (theApp.TraceRS232Events()) {
			WK_STAT_PEAK(_T("Alarm"), 1, _T("UpdateAlarm"));
			TRACE(_T("CSDIInput[%s] Alarm updated %d\n"), GetShmName(), wSubID );
		}
	}
	else {
		TRACE(_T("CSDIInput[%s] Alarm not updated %d\n"), GetShmName(), wSubID );
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIInput::ConfirmSetValue(const CString &sValue)
{
	// OOPS gf todo jetzt haben wir den Wert
	if (SECID_NO_ID != m_dwServerData) {
		CSecID id(m_wGroupID, SECID_NO_SUBID);
		DoConfirmSetValue(id, m_sKey, sValue, m_dwServerData);
		// Wir merken uns weiterhin die Werte, damit auch mehrere Confirms durchkommen
//		m_dwServerData = SECID_NO_ID;
//		m_sKey = _T("");
	}
	else {
		WK_TRACE_ERROR(_T("CSDIInput[%s]::ConfirmSetValue(%s) NO SERVER DATA\n"),
							GetShmName(), sValue);
	}
}
