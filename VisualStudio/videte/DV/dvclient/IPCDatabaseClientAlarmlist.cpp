	// IPCDatabaseAlarmlist.cpp: implementation of the CIPCDatabaseClientAlarmlist class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "cpanel.h"
//#include "alarmlist.h"
#include "IPCDatabaseClientAlarmlist.h"
#include "AlarmObject.h"
#include "ServerAlarmlist.h"
#include "alarmlistDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDatabaseClientAlarmlist::CIPCDatabaseClientAlarmlist(LPCTSTR shmName, CServerAlarmlist* pServer)
 : CIPCDatabase(shmName,FALSE)
{
	m_pServer = pServer;
	m_AlarmTime.GetLocalTime();
	m_dwNumberOfRequests = 0;
	m_dwNumberOfArchives = 0;

	StartThread();
}

CIPCDatabaseClientAlarmlist::~CIPCDatabaseClientAlarmlist()
{
	StopThread();
	m_Archives.SafeDeleteAll();
	m_Sequences.SafeDeleteAll();
	m_AlarmObjectList.SafeDeleteAll();
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnRequestDisconnect()
{
	DelayedDelete();
	if (m_pServer)
	{
		CAlarmlistDlg* pDlg = m_pServer->GetAlarmlistDlg();
		if (WK_IS_WINDOW(pDlg))
			pDlg->NotifyDisconnect();
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnConfirmConnection()
{
	if (m_pServer)
	{
		CAlarmlistDlg* pDlg = m_pServer->GetAlarmlistDlg();
		if (pDlg)
			pDlg->NotifyConnect(TRUE, 0);
	}

	// Archive zunächst einmal löschen
	m_Archives.SafeDeleteAll();

	// Verbindung zur Datenbank steht. Dann zunächst einmal die Archiveliste anfordern
	DoRequestAlarmListArchives();
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnConfirmAlarmListArchives(int iNumRecords, 
											   const CIPCArchivRecord data[])
{
	// Sequenzliste zunächst einmal löschen
	m_Sequences.SafeDeleteAll();
	
	// Anzahl der Archive
	m_dwNumberOfArchives = iNumRecords;

	// Für jedes Archive die Sequenzlisten anfordern
	m_Archives.Lock(_T(__FUNCTION__));
	// Archive zunächst einmal löschen
	m_Archives.SafeDeleteAll();
	for (int nI=0;nI<iNumRecords;nI++) 
	{
		m_Archives.Add( new CIPCArchivRecord(data[nI]));
		DoRequestSequenceList(data[nI].GetID());
	}
	m_Archives.Unlock();
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnConfirmSequenceList(WORD wArchivNr, 
														int iNumRecords, 
														const CIPCSequenceRecord data[])
{
	m_Sequences.Lock(_T(__FUNCTION__));
	// Die angeforderten Sequenzen in die Sequenzliste hängen.
	for (int i=0;i<iNumRecords;i++)
	{
/*
		TRACE(_T("\tSeq %d of Archive %04x %s\n"),data[i].GetSequenceNr(),data[i].GetArchiveNr(),
				data[i].GetTime().GetDateTime());
*/
		m_Sequences.Add(new CIPCSequenceRecord(data[i]));
	}
	m_Sequences.Unlock();

	// Sequenzliste komplett?
	if (--m_dwNumberOfArchives == 0)
		RequestAlarmListArchives(m_AlarmTime);
}

//////////////////////////////////////////////////////////////////////
// Startet die Datenbanksuche zu einem bestimten Datum
void CIPCDatabaseClientAlarmlist::RequestAlarmListArchives(CSystemTime time)
{
	// CAVEAT: Wird sowohl vom Panel-Thread als auch vom eigenen CIPC-Thread aufgerufen
	// Thread safe???
	// 
	
//	TRACE(_T("RequestAlarmListArchives %s\n"), time.GetDate());
	if (m_dwNumberOfRequests)
	{
		WK_TRACE_ERROR(_T("RequestAlarmListArchives %s while %lu remaining\n"),
								time.GetDate(), m_dwNumberOfRequests);
	}

	m_AlarmObjectList.SafeDeleteAll();

	m_AlarmTime = time;

	m_Sequences.Lock(_T(__FUNCTION__));
	// Für das gewünschte Datum alle Records anfordern
	for (int nI = 0; nI < m_Sequences.GetSize(); nI++)
	{	
		CIPCSequenceRecord* pSequenz = m_Sequences.GetAtFast(nI);
		if (pSequenz && pSequenz->GetTime().GetDBDate()==m_AlarmTime.GetDBDate())
		{
			// Zahl der Requests zählen
			m_dwNumberOfRequests ++;
			DoRequestRecords(pSequenz->GetArchiveNr(), pSequenz->GetSequenceNr(), 0, 0);
		}
	}
	m_Sequences.Unlock();

	// Keine Records für dieses Datum
	if (m_dwNumberOfRequests == 0)
	{
		if (m_pServer)
			m_pServer->GetAlarmlistDlg()->PostMessage(WM_NOTIFY_ALARMLIST_UPDATE);
//		theApp.MessageBeep(DEFAULT_BEEP);
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnConfirmRecords(WORD  wArchivNr, 
									 WORD  wSequenceNr, 
									 CIPCFields fields,
									 CIPCFields records)
{
	int nTypeLen = 0;
	if (fields.GetAtFast(0)->GetName() == _T("DVR_TYP"))
		nTypeLen = fields.GetAtFast(0)->GetMaxLength();
	int nDateLen = 0;
	if (fields.GetAtFast(1)->GetName() == _T("DVR_DATE"))
		nDateLen = fields.GetAtFast(1)->GetMaxLength();
	int nTimeLen = 0;
	if (fields.GetAtFast(2)->GetName() == _T("DVR_TIME"))
		nTimeLen = fields.GetAtFast(2)->GetMaxLength();

	if ((nTypeLen > 0) && (nDateLen > 0) && (nTimeLen > 0))
	{
		for (int nI = 0; nI < records.GetSize(); nI++)
		{
			CIPCField* pField = records.GetAtFast(nI);
			if (pField)
			{
				CString sRecord	= pField->GetValue();
				CString sType		= sRecord.Left(nTypeLen);
				CString sDate		= sRecord.Mid(nTypeLen, nDateLen);
				CString sTime		= sRecord.Mid(nTypeLen+nDateLen, nTimeLen);
				CString sArchiveName= _T("");
				CIPCArchivRecord*	pArchive =  m_Archives.GetArchiv(wArchivNr);
				if (pArchive)
					sArchiveName = pArchive->GetName();
				
				CSystemTime	AlarmTime;
	
///				TRACE(_T("Nr.%lu Archive:0x%x Name:%s Sequenz:%u Type:%s Date:%s Time:%s\n"), nI, wArchivNr, sArchiveName, wSequenceNr, sType, sDate, sTime);

				if (AlarmTime.SetDBTime(sTime))
				{
					if (AlarmTime.SetDBDate(sDate))
					{
						m_AlarmObjectList.Lock(_T(__FUNCTION__));

						if (sType == _T("1"))
							m_AlarmObjectList.AddHead(new CAlarmObject(LOBYTE(wArchivNr), sArchiveName, 0, AlarmTime));
						if (sType == _T("2"))
							m_AlarmObjectList.AddHead(new CAlarmObject(LOBYTE(wArchivNr), sArchiveName, 1, AlarmTime));
						if (sType == _T("A"))
							m_AlarmObjectList.AddHead(new CAlarmObject(LOBYTE(wArchivNr), sArchiveName, 2, AlarmTime));
						if (sType == _T("V"))
							m_AlarmObjectList.AddHead(new CAlarmObject(LOBYTE(wArchivNr), sArchiveName, 3, AlarmTime));
				
						m_AlarmObjectList.Unlock();
					}
				}
			}
		}
	}
	if (--m_dwNumberOfRequests == 0)
	{
		if (m_pServer)
			m_pServer->GetAlarmlistDlg()->PostMessage(WM_NOTIFY_ALARMLIST_UPDATE);
//		theApp.MessageBeep(DEFAULT_BEEP);
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnIndicateNewSequence(const CIPCSequenceRecord& data)
{
	// Die neue Sequenz in die Sequenzliste hängen...
	m_Sequences.SafeAdd(new CIPCSequenceRecord(data));

	//... und die Alarmeliste neu anfordern
	RequestAlarmListArchives(m_AlarmTime);
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	// Dies Sequenz aus der Sequenzliste entfernen....
	m_Sequences.RemoveSequence(wArchivNr, wSequenceNr);

	//... und die Alarmeliste neu anfordern
	RequestAlarmListArchives(m_AlarmTime);
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnIndicateNewArchiv(const CIPCArchivRecord& data)
{
	m_Archives.Lock(_T(__FUNCTION__));
	m_Archives.Add( new CIPCArchivRecord(data));
	m_Archives.Unlock();

	// Für das neue Archive die Sequenzliste anfordern...
	DoRequestSequenceList(data.GetID());
	m_dwNumberOfArchives++;
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnIndicateRemoveArchiv(WORD wArchivNr)
{
	// Archiveliste neu anfordern....
	// (Es würde genügen die zu diesem Archive gehörenden Sequenzen aus der
	//  Sequenzliste zu entfernen)
	DoRequestAlarmListArchives();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnIndicateError(DWORD dwCmd, 
												 CSecID id, 
												 CIPCError error, 
												 int iErrorCode,
												 const CString &sErrorMessage)
{
	if (   dwCmd == CIPC_DB_REQUEST_ALARM_LIST_ARCHIVES
		&& error == CIPC_ERROR_ACCESS_DENIED
		&& iErrorCode == DBEC_ARCHIVE_DURING_RESET)
	{
		DoRequestAlarmListArchives();
	}
	else
	{
		WK_TRACE(_T("CIPCDatabaseClientAlarmlist::OnIndicateError %s, %s, %08lx, %d %s\n"),
					NameOfCmd(dwCmd),
					NameOfEnum(error),
					id.GetID(),
					iErrorCode,
					sErrorMessage);
	}
}

//////////////////////////////////////////////////////////////////////
CSystemTime CIPCDatabaseClientAlarmlist::GetFirstAlarmTime()
{		   
	CSystemTime FirstAlarmTime;
	FirstAlarmTime.GetLocalTime();

	// Für das gewünschte Datum alle Records anfordern
	CAutoCritSec acs(&m_Sequences.m_cs);
	for (int nI = 0; nI < m_Sequences.GetSize(); nI++)
	{	
		CIPCSequenceRecord* pSequenz = m_Sequences.GetAtFast(nI);
		if (pSequenz && pSequenz->GetTime().GetDBDate()<FirstAlarmTime.GetDBDate())
			FirstAlarmTime = pSequenz->GetTime();

	}

	return FirstAlarmTime;
}