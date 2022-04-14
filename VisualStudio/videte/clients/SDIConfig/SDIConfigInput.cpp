/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigInput.cpp $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigInput.cpp $
// CHECKIN:		$Date: 1.03.99 14:29 $
// VERSION:		$Revision: 20 $
// LAST CHANGE:	$Modtime: 1.03.99 14:29 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "SDIConfig.h"

#include "SDIConfigDoc.h"
#include "SDIConfigView.h"

#include "SDIConfigInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bExtraDebugTrace;

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigInput
//////////////////////////////////////////////////////////////////////////////////////
CSDIConfigInput::CSDIConfigInput(const char *shmName, CSecID id, CSDIConfigDoc* pDoc)
	: CIPCInputClient(shmName,FALSE)
{
	m_ID = id;
	m_bInfo = FALSE;
	m_dwBitrate = 65535;
	m_pDoc = pDoc;
	m_pView = pDoc->GetView();
	m_dwTime = GetTickCount();
	m_RecordArray.SetAutoDelete(TRUE);

	StartThread();
	if (g_bExtraDebugTrace) {
		WK_TRACE("CSDIConfigInput THREAD %d started\n", this);
	}

}
//////////////////////////////////////////////////////////////////////////////////////
CSDIConfigInput::~CSDIConfigInput()
{
	m_RecordArray.DeleteAll();
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigInput::Run(DWORD dwTimeOut)
{
	DWORD curTime = GetTickCount();
	if ( (m_bInfo==FALSE) && ((curTime-m_dwTime)>15000) )
	{
		// time out post app to stop connection
		WK_TRACE_WARNING("TIMEOUT for input connection\n");
		m_pDoc->AddStatusLine(IDS_INPUT_NO_INFO);
		m_dwTime = curTime;
	}
	return CIPC::Run(dwTimeOut);
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnReadChannelFound()
{
	m_dwTime = GetTickCount();
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnIndicateError(DWORD dwCmd, CSecID id, 
									 CIPCError error, int iErrorCode,
									 const CString &sErrorMessage)
{
	if (g_bExtraDebugTrace) {
		WK_TRACE("OnIndicateError(%s,%08lx,%s,%d,%s)\n",
				  NameOfCmd(dwCmd),id.GetID(),NameOfEnum(error),
				  iErrorCode,(const char*)sErrorMessage);
	}

	CString sText, sFormat;
	// Bekannte Fehler muessen behandelt werden
	if (CIPC_ERROR_FILE_UPDATE==error) {
		if ( m_pDoc->ResetTransfering(FALSE) ) {
			sFormat.LoadString(IDS_FILE_TRANSFER_FAILED);
			sText.Format(sFormat, sErrorMessage);
			m_pDoc->AddStatusLine(sText);
		}
		return;
	}

	// Wenn der Fehler nicht behandelt wurde, wenigstens Meldung ausgeben
	m_pDoc->AddStatusLine(IDS_SDI_ERROR);
	sText.Format("%s %i %s", NameOfEnum(error), iErrorCode, sErrorMessage);
	m_pDoc->AddStatusLine(sText);
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnRequestDisconnect()
{
	m_pDoc->EndConnection(FALSE);
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnConfirmConnection()
{
	// Nur den User informieren, dass weitere Infos eingeholt werden
	m_pView->PostMessage(WM_COMMAND, MSG_SDI_GET_INFO, 0);
	m_pDoc->AddStatusLine(IDS_FETCH_INFO_INPUTS);
	m_dwTime = GetTickCount();
	DoRequestBitrate();
	DoRequestInfoInputs(SECID_NO_GROUPID);	// all groups
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnConfirmBitrate(DWORD dwBitrate)
{
	m_dwBitrate = (dwBitrate * 85) / 100;
	if (g_bExtraDebugTrace) {
		WK_TRACE("Bitrate = %d\n",m_dwBitrate);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnConfirmInfoInputs(WORD wGroupID, int iNumGroups,
										  int numRecords, const CIPCInputRecord records[])
{
	// Die Auswertung der InputInfos erfolgt in OnAddRecord()
	CIPCInputClient::OnConfirmInfoInputs(wGroupID,iNumGroups,numRecords,records);
	m_bInfo = TRUE;
	// Nur den User informieren, dass Infos eingetroffen sind
	m_pDoc->AddStatusLine(IDS_HAS_INFO_INPUTS);
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnConfirmFileUpdate(const CString &sFileName)
{
	if ( m_pDoc->ResetTransfering(TRUE) ) {
		CString sText, sFormat;
		sFormat.LoadString(IDS_CONFIG_FILE_TRANSFERED);
		sText.Format(sFormat, sFileName);
		m_pDoc->AddStatusLine(sText);
	}
	if (g_bExtraDebugTrace) {
		WK_TRACE("OnConfirmFileUpdate(%s)\n",(const char*)sFileName);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnConfirmGetValue(CSecID id, // might be used as group ID only
										const CString &sKey,
										const CString &sValue,
										DWORD dwServerData)
{
	if (sKey==CSD_APPLICATION_NAME) {
		// Das war eine Anfrage, zu welcher Unit dieser Input gehoert
		if (sValue==NameOfEnum(WAI_SDIUNIT)) {
			// Dieser Input gehoert zur SDIUnit
			// Jetzt muessen wir wissen, welcher Typ von ControlRecord es ist
			DoRequestGetValue(id, SDI_KEY_CONTROLRECORD_TYP_COMMENT);
			if (g_bExtraDebugTrace) {
				WK_TRACE("OnConfirmGetValue %s %s %08lx Get Type\n",
							(LPCSTR)sKey, (LPCSTR)sValue, id.GetID());
			}
		}
		else {
			// Dieser Input gehoert nicht zur SDIUnit
			// Kann ja auch  ein UpdateRecord gewesen sein
			// Rausschmeissen, falls vorhanden
			if (g_bExtraDebugTrace) {
				WK_TRACE("OnConfirmGetValue %s %s %08lx Wrong Value\n",
							(LPCSTR)sKey, (LPCSTR)sValue, id.GetID());
			}
			DeleteSDIInput(id);
		}
	}
	else if (sKey==SDI_KEY_CONTROLRECORD_TYP_COMMENT) {
		// Das war eine Anfrage nach Typ und Kommentar des SDIControlRecord
		int iTrenner = sValue.Find("=");
		CString sType = sValue.Left(iTrenner);
		CString sComment = sValue.Right(sValue.GetLength()-iTrenner-1);
		if (m_pDoc->IsControlRecordOk(CSDIControl::NameToSDIControlType(sType))) {
			// Dieser ControlTyp wird unterstuetzt
			if (g_bExtraDebugTrace) {
				WK_TRACE("OnConfirmGetValue %s %s %s %08lx Supported\n",
							(LPCSTR)sKey, (LPCSTR)sComment, (LPCSTR)sType, id.GetID());
			}
			// Diesen Input hinzufuegen, falls noch nicht vorhanden
			AddSDIInput(id, sComment);
		}
		else {
			// Dieser ControlTyp wird nicht unterstuetzt
			if (g_bExtraDebugTrace) {
				WK_TRACE("OnConfirmGetValue %s %s %s %08lx Not supported\n",
							(LPCSTR)sKey, (LPCSTR)sComment, (LPCSTR)sType, id.GetID());
			}
		}
	}
	else {
		WK_TRACE_WARNING("OnConfirmGetValue Key NOT HANDLED\n");
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnConfirmSetValue(CSecID id, // might be used as group ID only
										const CString &sKey,
										const CString &sValue,
										DWORD dwServerData)
{
	if ( (SDI_KEY_START_XMODEM_TRANSMITTER == sKey)
		  || (SDI_KEY_START_XMODEM_RECEIVER == sKey) )
	{
		if (SDI_VALUE_CONFIG_STARTED == sValue) {
			DWORD dwTime = 0;
			if ( m_pDoc->GetByteRate() ) {
				dwTime = m_pDoc->GetFileSize() / m_pDoc->GetByteRate();
			}
			CTimeSpan timeSpan((time_t)dwTime);
			CString sTime, sFormat, sText;
			sTime = timeSpan.Format("%M min %S s");
			sFormat.LoadString(IDS_CONFIG_TRANSFERING);
			sText.Format(sFormat, sTime);
			m_pDoc->AddStatusLine(sText);
			m_pView->PostMessage(WM_COMMAND, MSG_SDI_CONFIGURATION_START, 0);
		}
		else if (SDI_VALUE_CONFIG_OK == sValue) {
			if ( m_pDoc->ResetConfiguring(TRUE) ) {
				m_pDoc->AddStatusLine(IDS_CONFIG_OK);
			}
		}
		else if (SDI_VALUE_CONFIG_FAILED_NOT_ALLOWED == sValue) {
			if ( m_pDoc->ResetConfiguring(FALSE) ) {
				m_pDoc->AddStatusLine(IDS_CONFIG_FAILED_NOT_ALLOWED);
			}
		}
		else if (SDI_VALUE_CONFIG_FAILED_NOT_STARTED == sValue) {
			if ( m_pDoc->ResetConfiguring(FALSE) ) {
				m_pDoc->AddStatusLine(IDS_CONFIG_FAILED_NOT_STARTED);
			}
		}
		else if (SDI_VALUE_CONFIG_FAILED_NEXT_TRY == sValue) {
			DWORD dwTime = 0;
			if ( m_pDoc->GetByteRate() ) {
				dwTime = m_pDoc->GetFileSize() / m_pDoc->GetByteRate();
			}
			CTimeSpan timeSpan((time_t)dwTime);
			CString sTime, sFormat, sText;
			sTime = timeSpan.Format("%M min %S s");
			sFormat.LoadString(IDS_CONFIG_FAILED_NEXT_TRY);
			sText.Format(sFormat, sTime);
			m_pDoc->AddStatusLine(sText);
			m_pView->PostMessage(WM_COMMAND, MSG_SDI_CONFIGURATION_START, 0);
		}
		else if (SDI_VALUE_CONFIG_FAILED_NOT_OK == sValue) {
			if ( m_pDoc->ResetConfiguring(FALSE) )  {
				m_pDoc->AddStatusLine(IDS_CONFIG_FAILED_NOT_OK);
			}
		}
		else if (SDI_VALUE_CONFIG_FAILED_XMODEM_FAILED == sValue) {
			if ( m_pDoc->ResetConfiguring(FALSE) ) {
				m_pDoc->AddStatusLine(IDS_CONFIG_FAILED_XMODEM_FAILED);
			}
		}
		else {
			WK_TRACE_WARNING("OnConfirmSetValue Key %s Value %s VALUE NOT HANDLED\n",
										(LPCSTR)sKey, (LPCSTR)sValue);
		}
	}
	else {
		WK_TRACE_WARNING("OnConfirmSetValue Key %s Value %s KEY NOT HANDLED\n",
									(LPCSTR)sKey, (LPCSTR)sValue);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnAddRecord(const CIPCInputRecord& record)
{
	if (g_bExtraDebugTrace) {
		WK_TRACE("Add CIPCInputRecord ID %08lx Name %s\n",
					record.GetID().GetID(), (LPCSTR)record.GetName());
	}
	// Jetzt muessen wir wissen, zu welcher Unit der Input gehoert
	DoRequestGetValue(record.GetID(), CSD_APPLICATION_NAME);
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnUpdateRecord(const CIPCInputRecord& record)
{
	if (g_bExtraDebugTrace) {
		WK_TRACE("Update CIPCInputRecord ID %08lx Name %s\n",
					record.GetID().GetID(), (LPCSTR)record.GetName());
	}
	// Kommt auch bei Beenden der Unit, also beim Entfernen der InputRecords
	// dann liefert record.GetIsOkay() FALSE zurueck
	if ( record.GetIsOkay() ) {
		DoRequestGetValue(record.GetID(), CSD_APPLICATION_NAME);
	}
	else {
		DeleteSDIInput(record.GetID());
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::OnDeleteRecord(const CIPCInputRecord& record)
{
	if (g_bExtraDebugTrace) {
		WK_TRACE("Delete CIPCInputRecord ID %08lx Name %s\n",
					record.GetID().GetID(), (LPCSTR)record.GetName());
	}
	// Der Input muss weg
	DeleteSDIInput(record.GetID());
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::AddSDIInput(CSecID id, CString& sComment)
{
	if (g_bExtraDebugTrace) {
		WK_TRACE("CSDIConfigDoc::AddSDIInput %08lx %s\n", id.GetID(), (LPCSTR)sComment);
	}
	// Erst ueberpruefen, ob es diesen Input schon gibt, falls es ein UpdateRecord war
	BOOL bFound = FALSE;
	CSDIConfigRecord* pRecord = FindInputInArray(id);
	if ( pRecord ) {
		bFound = TRUE;
		if ( pRecord->GetComment() != sComment ) {
			pRecord->SetComment(sComment);
			// Sicherheitshalber unzerstoerbare Kopie des Records anlegen
			CSecID idGroup(id.GetGroupID(), SECID_NO_SUBID);

			CSDIConfigRecord* pSafeRecord = new CSDIConfigRecord(idGroup, sComment);
			m_pView->PostMessage(WM_COMMAND, MSG_SDI_UPDATE_CONTROL, (LPARAM)pSafeRecord);
			if (g_bExtraDebugTrace) {
				WK_TRACE("AddSDIInput Update %08lx %s\n", id.GetID(), (LPCSTR)sComment);
			}
		}
	}
	else {
		// Dann ueberpruefen, ob es diese Gruppe schon gibt
		bFound = FindInputGroupInArray( id.GetGroupID() );
		// Erst danach den Input ans Array anhaengen
		CSDIConfigRecord* pRecord = new CSDIConfigRecord(id, sComment);
		m_RecordArray.SafeAdd(pRecord);
		if (g_bExtraDebugTrace) {
			WK_TRACE("AddSDIInput Add to Array %08lx %s\n", id.GetID(), (LPCSTR)sComment);
		}
	}
	if (!bFound) {
		// Wenn es diesen Input bzw. diese Gruppe vorher noch nicht gab,
		// dann View benachrichtigen
		// Sicherheitshalber unzerstoerbare Kopie des Records anlegen
		CSecID idGroup(id.GetGroupID(), SECID_NO_SUBID);
		CSDIConfigRecord* pSafeRecord = new CSDIConfigRecord(idGroup, sComment);
		m_pView->PostMessage(WM_COMMAND, MSG_SDI_ADD_CONTROL, (LPARAM)pSafeRecord);
		if (g_bExtraDebugTrace) {
			WK_TRACE("AddSDIInput Add NEW %08lx %s\n", id.GetID(), (LPCSTR)sComment);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::DeleteSDIInput(CSecID id)
{
//	WK_TRACE("CSDIConfigDoc::DeleteSDIInput %08lx\n", id.GetID());
	// Erst den Input aus dem Array entfernen
	RemoveInputFromArray(id);
	// Dann ueberpruefen, ob es diese Gruppe noch gibt
	BOOL bFound = FindInputGroupInArray( id.GetGroupID() );
	if (!bFound) {
		// Wenn es diese Gruppe nicht mehr gibt, dann View benachrichtigen
		CSecID idGroup(id.GetGroupID(), SECID_NO_SUBID);
		m_pView->PostMessage(WM_COMMAND, MSG_SDI_DELETE_CONTROL, (LPARAM)idGroup.GetID());
		if (g_bExtraDebugTrace) {
			WK_TRACE("DeleteSDIInput %08lx\n", id.GetID());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CSDIConfigRecord* CSDIConfigInput::FindInputInArray(CSecID id)
{
	// Sucht die angegebene ID im m_RecordArray
	BOOL bFound = FALSE;
	CSDIConfigRecord* pRecord = NULL;
	m_RecordArray.Lock();
	for (int i=0 ; i<m_RecordArray.GetSize() && !bFound ; i++) {
		pRecord = m_RecordArray.GetAt(i);
		if ( pRecord->GetID() == id ) {
			bFound = TRUE;
			if (g_bExtraDebugTrace) {
				WK_TRACE("Found Record in Array %08lx\n", id.GetID());
			}
		}
	}
	m_RecordArray.Unlock();
	return (bFound ? pRecord : NULL);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIConfigInput::FindInputGroupInArray(WORD wGroupID)
{
	// Sucht die angegebene GroupID im m_RecordArray
	BOOL bFound = FALSE;
	CSecID id;
	m_RecordArray.Lock();
	for (int i=0 ; i<m_RecordArray.GetSize() && !bFound ; i++) {
		id = (m_RecordArray.GetAt(i))->GetID();
		if (id.GetGroupID() == wGroupID) {
			bFound = TRUE;
			if (g_bExtraDebugTrace) {
				WK_TRACE("DeleteSDIInput %04lx\n", wGroupID);
			}
		}
	}
	m_RecordArray.Unlock();
	return bFound;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIConfigInput::RemoveInputFromArray(CSecID id)
{
	// Sucht die angegebene ID im m_RecordArray
	BOOL bFound = FALSE;
	m_RecordArray.Lock();
	CSDIConfigRecord* pRecord = NULL;
	for (int i=0 ; i<m_RecordArray.GetSize() && !bFound ; i++) {
		pRecord = m_RecordArray.GetAt(i);
		// Wenn die ID gefunden wurde
		if ( id == pRecord->GetID() ) {
			// dann rausschmeissen
			m_RecordArray.Remove(pRecord);
			delete pRecord;
			bFound = TRUE;
		}
	}
	// Wenn er gefunden und entfernt wurde und er der letzte war
	if ( bFound && (0==m_RecordArray.GetSize()) ) {
		// Alle angeschlossenen Geräte sind nicht mehr vorhanden
		m_pDoc->AddStatusLine(IDS_INFO_INPUTS_NOTHING);
	}
	m_RecordArray.Unlock();
}
//////////////////////////////////////////////////////////////////////////////////////
CString CSDIConfigInput::GetCommentFromRecordArrayByGroupID(CSecID id)
{
	// Sucht die angegebene ID(nur GroupID) im m_RecordArray
	BOOL bFound = FALSE;
	m_RecordArray.Lock();
	CSDIConfigRecord* pRecord = NULL;
	CString sComment;
	for (int i=0 ; i<m_RecordArray.GetSize() && !bFound ; i++) {
		pRecord = m_RecordArray.GetAt(i);
		// Wenn die GroupID gefunden wurde
		if ( id.GetGroupID() == pRecord->GetID().GetGroupID() ) {
			sComment = pRecord->GetComment();
			bFound = TRUE;
		}
	}
	m_RecordArray.Unlock();
	return sComment;
}
