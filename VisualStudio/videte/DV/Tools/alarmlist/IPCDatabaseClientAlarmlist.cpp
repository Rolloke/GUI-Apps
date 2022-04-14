	// IPCDatabaseAlarmlist.cpp: implementation of the CIPCDatabaseClientAlarmlist class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alarmlist.h"
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

CIPCDatabaseClientAlarmlist::CIPCDatabaseClientAlarmlist(const char* shmName, CServerAlarmlist* pServer)
 : CIPCDatabase(shmName,FALSE)
{
	m_pServer = pServer;
	StartThread();
	m_AlarmTime.GetLocalTime();
}

CIPCDatabaseClientAlarmlist::~CIPCDatabaseClientAlarmlist()
{
	StopThread();
	m_Archives.SafeDeleteAll();
	m_Sequences.SafeDeleteAll();
	DeleteAlarmObjectList();
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
	WK_TRACE("OnRequestDisconnect\n");
	if (m_pServer)
	{
		m_pServer->GetAlarmlistDlg()->PostMessage(WM_NOTIFY_DISCONNECT);
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnConfirmConnection()
{
	WK_TRACE("OnConfirmConnection\n");

	if (m_pServer)
	{
		m_pServer->GetAlarmlistDlg()->NotifyConnect(TRUE,0);
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
	m_Archives.Lock();
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
	m_Sequences.Lock();

	// Die angeforderten Sequenzen in die Sequenzliste hängen.
	for (int i=0;i<iNumRecords;i++)
	{
		TRACE("\tSeq %d of Archive %04x %s\n",data[i].GetSequenceNr(),data[i].GetArchiveNr(),
				data[i].GetTime().GetDateTime());
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
	DeleteAlarmObjectList();

	m_AlarmTime = time;

	TRACE("RequestAlarmListArchives %s\n",time.GetDate());

	// RequestCounter rücksetzen
	m_dwNumberOfRequests = 0;

	m_Sequences.Lock();

	// Für das gewünschte Datum alle Records anfordern
	for (int nI = 0; nI < m_Sequences.GetSize(); nI++)
	{	
		CIPCSequenceRecord* pSequenz = m_Sequences.GetAt(nI);
		if (pSequenz && pSequenz->GetTime().GetDate()==m_AlarmTime.GetDate())
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
		MessageBeep(0);
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnConfirmRecords(WORD  wArchivNr, 
									 WORD  wSequenceNr, 
									 CIPCFields fields,
									 CIPCFields records)
{
	int nTypeLen = 0;
	if (fields.GetAt(0)->GetName() == "DVR_TYP")
		nTypeLen = fields.GetAt(0)->GetMaxLength();
	int nDateLen = 0;
	if (fields.GetAt(1)->GetName() == "DVR_DATE")
		nDateLen = fields.GetAt(1)->GetMaxLength();
	int nTimeLen = 0;
	if (fields.GetAt(2)->GetName() == "DVR_TIME")
		nTimeLen = fields.GetAt(2)->GetMaxLength();

	if ((nTypeLen > 0) && (nDateLen > 0) && (nTimeLen > 0))
	{
		for (int nI = 0; nI < records.GetSize(); nI++)
		{
			CIPCField* pField = records.GetAt(nI);
			if (pField)
			{
				CString sRecord		= pField->GetValue();
				CString sType		= sRecord.Left(nTypeLen);
				CString sDate		= sRecord.Mid(nTypeLen, nDateLen);
				CString sTime		= sRecord.Mid(nTypeLen+nDateLen, nTimeLen);
				CString sArchiveName= "";
				CIPCArchivRecord*	pArchive =  m_Archives.GetArchiv(wArchivNr);
				if (pArchive)
					sArchiveName = pArchive->GetName();
				
				CSystemTime	AlarmTime;
	
				TRACE("Nr.%lu Archive:0x%x Name:%s Sequenz:%u Type:%s Date:%s Time:%s\n", nI, wArchivNr, sArchiveName, wSequenceNr, sType, sDate, sTime);

				if (AlarmTime.SetDBTime(sTime))
				{
					if (AlarmTime.SetDBDate(sDate))
					{
						m_csList.Lock();

						if (sType == "1")
							m_AlarmObjectList.AddHead(new CAlarmObject(LOBYTE(wArchivNr), sArchiveName, 0, AlarmTime));
						if (sType == "2")
							m_AlarmObjectList.AddHead(new CAlarmObject(LOBYTE(wArchivNr), sArchiveName, 1, AlarmTime));
						if (sType == "A")
							m_AlarmObjectList.AddHead(new CAlarmObject(LOBYTE(wArchivNr), sArchiveName, 2, AlarmTime));
				
						m_csList.Unlock();
					}
				}
			}
		}
	}
	if (--m_dwNumberOfRequests == 0)
	{
		if (m_pServer)
			m_pServer->GetAlarmlistDlg()->PostMessage(WM_NOTIFY_ALARMLIST_UPDATE);
		MessageBeep(0);
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnIndicateNewSequence(const CIPCSequenceRecord& data)
{
	// Die neue Sequenz in die Sequenzliste hängen...
	m_Sequences.Lock();
	m_Sequences.Add(new CIPCSequenceRecord(data));
	m_Sequences.Unlock();

	//... und die Alarmeliste neu anfordern
	RequestAlarmListArchives(m_AlarmTime);
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	// Dies Sequenz aus der Sequenzliste entfernen....
	m_Sequences.Lock();
	m_Sequences.RemoveSequence(wArchivNr, wSequenceNr);
	m_Sequences.Unlock();

	//... und die Alarmeliste neu anfordern
	RequestAlarmListArchives(m_AlarmTime);
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnIndicateNewArchiv(const CIPCArchivRecord& data)
{
	// Für das neue Archive die Sequenzliste anfordern...
	DoRequestSequenceList(data.GetID());

	//... und die Alarmeliste neu anfordern
	RequestAlarmListArchives(m_AlarmTime);
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnIndicateRemoveArchiv(WORD wArchivNr)
{
	// Archiveliste neu anfordern....
	// (Es würde genügen die zu diesem Archive gehörenden Sequenzen aus der
	//  Sequenzliste zu entfernen)
	DoRequestAlarmListArchives();

	//... und die Alarmeliste neu anfordern
	RequestAlarmListArchives(m_AlarmTime);
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::DeleteAlarmObjectList()
{
	m_csList.Lock();

	int nCount = m_AlarmObjectList.GetCount();
	for (int nI = 0; nI < nCount; nI++)
		delete(m_AlarmObjectList.RemoveHead());
		
	m_AlarmObjectList.RemoveAll();

	m_csList.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseClientAlarmlist::OnIndicateError(DWORD dwCmd, 
												 CSecID id, 
												 CIPCError error, 
												 int iErrorCode,
												 const CString &sErrorMessage)
{
	WK_TRACE("CIPCDatabaseClientAlarmlist::OnIndicateError %s\n",NameOfEnum(error));
}

//////////////////////////////////////////////////////////////////////
CSystemTime CIPCDatabaseClientAlarmlist::GetFirstAlarmTime()
{		   
	CSystemTime FirstAlarmTime;
	FirstAlarmTime.GetLocalTime();

	// Für das gewünschte Datum alle Records anfordern
	for (int nI = 0; nI < m_Sequences.GetSize(); nI++)
	{	
		CIPCSequenceRecord* pSequenz = m_Sequences.GetAt(nI);
		if (pSequenz && pSequenz->GetTime().GetDate()<FirstAlarmTime.GetDate())
			FirstAlarmTime = pSequenz->GetTime();

	}

	return FirstAlarmTime;
}