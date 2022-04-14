// IPCDatabaseDBCheck.cpp: implementation of the CIPCDatabaseDBCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DBCheck.h"
#include "IPCDatabaseDBCheck.h"
#include "Server.h"
#include "DBCheckView.h"
#include "LeftView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDatabaseDBCheck::CIPCDatabaseDBCheck(const char* shmName, 
											 CServer* pServer, CSecID id)
	: CIPCDatabaseClient(shmName)
{
	m_pServer         = pServer;
	m_dwServerVersion = 0; // assume iST 3.0
	m_ID = id;
	m_bGotInfo = FALSE;
   m_bLocked  = false;
//	m_bActualizing = FALSE;
//	m_dwLastActualizing = GetTickCount();
	StartThread();
}

CIPCDatabaseDBCheck::~CIPCDatabaseDBCheck()
{
   StopThread();
}
void CIPCDatabaseDBCheck::Lock()
{
	m_cs.Lock();
   m_bLocked = true;

}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDBCheck::Unlock()
{
	m_cs.Unlock();
   m_bLocked = false;
}

bool CIPCDatabaseDBCheck::IsLocked()
{
   return m_bLocked;
}
/*
void CIPCDatabaseDBCheck::OnConfirmConnection()
{
   CIPCDatabaseClient::OnConfirmConnection();
   TRACE("CIPCDatabaseDBCheck::OnConfirmConnection\n");
   DoRequestInfo();
}
*/
void CIPCDatabaseDBCheck::OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[])
{
   Lock();
   CIPCDatabaseClient::OnConfirmInfo(iNumRecords,data);
   CIPCArchivRecordArray &arr = GetRecords();
   CIPCArchivRecordPtr    ptr = NULL;
   for (int i=0;i<arr.GetSize();i++)
   {
      ptr = arr.GetAt(i);
      TRACE("%d %s\n",i, ptr->GetName());
      DoRequestSequenceList(ptr->GetID());
   }
   m_bGotInfo = TRUE;
   Unlock();
}

/*********************************************************************************************
 Klasse  : CIPCDatabaseDBCheck
 Funktion: OnConfirmSequenceList
 Zweck   : 

 Parameter:  
  wArchivNr  : (E): Nummer des Archivs  (WORD)
  iNumRecords: (E): Anzahl der Datensätze  (int)
  data       : (E): Feld mit Datensätzen  (<LINK CIPCSequenceRecord, const CIPCSequenceRecord>)

 Rückgabewert:  (void)
 Author: Rolf Kary-Ehlers
 <TITLE OnConfirmSequenceList>
 <GROUP CIPC-Kommunikation>
 <TOPICORDER 1>
*********************************************************************************************/
void CIPCDatabaseDBCheck::OnConfirmSequenceList(WORD wArchivNr, int iNumRecords, const CIPCSequenceRecord data[])
{
   Lock();
	for (int i=0; i<iNumRecords; i++)
	{
		m_pServer->AddNewSequence(data[i]);
	}
   Unlock();
}

void CIPCDatabaseDBCheck::OnConfirmAlarmListArchives(int iNumRecords, const CIPCArchivRecord data[])
{
   Lock();
   CIPCDatabaseClient::OnConfirmAlarmListArchives(iNumRecords,data);
   for (int i=0;i<GetRecords().GetSize();i++)
   {
      TRACE("%d %s\n",i,GetRecords().GetAt(i)->GetName());
   }
   m_bGotInfo = TRUE;
   Unlock();
}

void CIPCDatabaseDBCheck::OnConfirmFieldInfo(int iNumRecords, const CIPCField data[])
{
   Lock();
	m_pServer->OnConfirmFieldInfo(iNumRecords,data);
   Unlock();
}

void CIPCDatabaseDBCheck::OnConfirmDrives(int iNumDrives, const CIPCDrive drives[])
{
   Lock();
	m_pServer->OnConfirmDrives(iNumDrives,drives);
   Unlock();
}

void CIPCDatabaseDBCheck::OnConfirmRecords(WORD wArchivNr,WORD wSequenceNr,CIPCFields fields,CIPCFields records)
{
   Lock();
   int j, nFields = fields.GetSize(),
       nRecords = records.GetSize(),
       nLen, nPos, *pnPositions = NULL;
   CString str = records[0]->GetValue();
   for (nPos=0; nPos<str.GetLength(); nPos++)
   {
      if (str.GetAt(nPos) != ' ')
         break;
   }
   if (nFields)
   {
      pnPositions = new int[nFields];
      for (j=0; j<nFields; j++, nPos+=nLen)
      {
         nLen           = fields[j]->GetMaxLength();
         pnPositions[j] = nPos;
         TRACE("%s\n", fields[j]->GetName());
      }
   }
   DWORD dwRecordNr = 1;
   CDBCheckView *pCW = m_pServer->GetDocument()->GetDBCheckView();
   if (pCW)
   {
      if (!pCW->ContainsSequence(wArchivNr, wSequenceNr, dwRecordNr))
      {
         pCW->InsertRecord(wArchivNr, wSequenceNr, dwRecordNr, fields, pnPositions, records);
      }
      CLeftView *pLW = m_pServer->GetDocument()->GetLeftView();
      if (pLW)
      {
         if (!pLW->IterateSequences())
         {
            pCW->InsertMaxArchiveTime();
         }
      }
   }
   WK_DELETE(pnPositions);
   Unlock();
}

void CIPCDatabaseDBCheck::OnConfirmRecordNr(WORD wArchivNr,WORD wSequenceNr,
                                            DWORD dwRecordNr,DWORD dwNrOfRecords,const CIPCPictureRecord &pict,
                                            int iNumFields,const CIPCField fields[])
{
   Lock();
   CDBCheckView *pCW = m_pServer->GetDocument()->GetDBCheckView();
   if (pCW)
   {
      if (!pCW->ContainsSequence(wArchivNr, wSequenceNr, dwRecordNr))
      {
         pCW->InsertRecordNr(wArchivNr, wSequenceNr, dwRecordNr, dwNrOfRecords, pict, iNumFields, fields);
      }
      else
      {
         dwRecordNr = dwNrOfRecords;
      }
      if (dwRecordNr < dwNrOfRecords)
      {
         DoRequestRecordNr(wArchivNr, wSequenceNr, dwRecordNr, dwRecordNr+1, (DWORD)-2);
      }
      if (dwRecordNr == dwNrOfRecords)
      {
         CLeftView *pLW = m_pServer->GetDocument()->GetLeftView();
         if (pLW)
         {
            if (!pLW->IterateSequences())
            {
               pCW->InsertMaxArchiveTime();
            }
         }
      }
   }
   Unlock();
}

void CIPCDatabaseDBCheck::OnRequestRecords(WORD  wArchivNr, WORD  wSequenceNr, DWORD dwFirstRecordNr, DWORD dwLastRecordNr)
{
   Lock();
   CIPCDatabaseClient::OnRequestRecords(wArchivNr, wSequenceNr, dwFirstRecordNr, dwLastRecordNr);
   Unlock();
}

void CIPCDatabaseDBCheck::OnIndicateError(DWORD dwCmd, CSecID id, CIPCError error, int iErrorCode,const CString &sErrorMessage)
{
   Lock();
//   CString str;
//   str.Format("ERROR\nCommand:%x, ID:%04x.%04x, Code:%d\n%s", dwCmd, id.GetGroupID(), id.GetSubID(), iErrorCode, sErrorMessage);
//   AfxMessageBox(str, MB_OK|MB_ICONERROR);
   CIPCDatabaseClient::OnIndicateError(dwCmd, id, error, iErrorCode, sErrorMessage);
   Unlock();
}

CIPCArchivRecord* CIPCDatabaseDBCheck::GetArchiv(WORD wArchiv)
{
	Lock();
	const CIPCArchivRecordArray& a = GetRecords();
	int i,c;
	CIPCArchivRecord* pRecord = NULL;
	c = a.GetSize();
	for (i=0;i<c;i++)
	{
		if (a.GetAt(i)->GetSubID() == wArchiv)
		{
			pRecord = a.GetAt(i);
			break;
		}
	}
	Unlock();
	return pRecord;
}

CString	CIPCDatabaseDBCheck::GetArchivName(WORD wArchiv)
{
	Lock();
	const CIPCArchivRecordArray& a = GetRecords();
	int i,c;
	CString s;
	c = a.GetSize();
	for (i=0;i<c;i++)
	{
		if (a.GetAt(i)->GetSubID() == wArchiv)
		{
			s = a.GetAt(i)->GetName();
			break;
		}
	}
	Unlock();
	return s;
}

CCriticalSection& CIPCDatabaseDBCheck::GetCriticalSection()
{
   return m_cs;
}

