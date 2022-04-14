// BackupThread.cpp: implementation of the CBackupThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbs.h"
#include "Mainfrm.h"
#include "BackupThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBackupThread::CBackupThread(CBackup* pBackup) : CWK_Thread(_T("BackupThread"), NULL)
{
	m_pBackup = pBackup;
	m_bCancelled = FALSE;
	m_iCurrentSequence = 0;

	m_dwLastAction = GetTickCount();
	m_bFinished = FALSE;
	m_dwTimeOut = 30*1000;
	m_dwID = 0;
	m_dwTimeOutHandled = 0;
}
//////////////////////////////////////////////////////////////////////
CBackupThread::~CBackupThread()
{
	WK_DELETE(m_pBackup);
}
//////////////////////////////////////////////////////////////////////
BOOL CBackupThread::Run(LPVOID lpContext)
{
	BOOL bRet = FALSE;
	if (m_pBackup->GetFlags()==0)
	{
		// old fashioned backup
		bRet = RunCopy();
	}
	else
	{
		// new backup by time
		if (m_pBackup->GetFlags() == BBT_CALCULATE)
		{
			bRet = RunCalculate();
		}
		else if (m_pBackup->GetFlags() == BBT_CALCULATE_END)
		{
			bRet = RunCalculateEnd();
		}
		else if (   m_pBackup->GetFlags() & BBT_EXECUTE
			     || m_pBackup->GetFlags() & BBT_EXECUTE_AUTO)
		{
			bRet = RunCopy();
		}
	}

	if (!bRet)
	{
		CWnd* pWnd = AfxGetMainWnd();
		if (WK_IS_WINDOW(pWnd))
		{
			pWnd->PostMessage(WM_USER,WPARAM_BACKUP_FINISHED);
		}
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CBackupThread::RunCopy()
{
	// return TRUE for next call of run
	m_dwLastAction = GetTickCount();


	// always check if client still exits
	CClient* pClient = theApp.m_Clients.GetClient(m_pBackup->GetClientID());
	if (pClient==NULL)
	{
		WK_TRACE_ERROR(_T("backup stopped by disconnect or cancel\n"));
		return FALSE;
	}
	CIPCDatabaseServer* pCIPCDatabase = pClient->GetCIPCDatabaseServer();

	// does Client cancelled the backup ?
	if (m_bCancelled)
	{
		if(m_pBackup->GetNrOfIDs() == 0)
		{
			//no sequence was copied to backup drive
			//clean up the whole backup (e.g. delete Z:\dbb000c)
			m_pBackup->CleanUp();
		}
		else
		{
			//some of the sequences were copied
			//update the necessary .dbi and .dbf files (sequence.dbf, backup.dbi and e.g. 0078.dbf)
			m_pBackup->CleanUpPartly();
		}

		pCIPCDatabase->DoConfirmCancelBackup(m_pBackup->GetUserData());
		return FALSE;
	}

	// are we ready ?
	if (m_iCurrentSequence >= m_pBackup->GetNrOfIDs())
	{
		if(m_pBackup->GetFlags() == BBT_EXECUTE_AUTO) //automatic partly backup
		{
			//check if at least one sequence was copied
			//get the path e.g. Z:\dbb0001\arch001 and check if the folder "archxxx" exists
			//if not, delete dbbxxxx
			WORD wArchiveNr   = HIWORD(m_dwID);
			BOOL bNothingCopied = FALSE;

			// is the Archive there
			CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchiveNr);
			if (pArchiv)
			{
				//check if archxxx - folder exists on backup drive
				//if not, nothing was copied -> delete it
				CString sDir;
				sDir.Format(_T("%s\\arch%04lx"),m_pBackup->GetDirectory(),pArchiv->GetNr());
				if(!DoesFileExist(sDir))
				{
					WK_TRACE_ERROR(_T("Directory %s does not exist, nothing was copied during backup\n"), sDir);
					m_pBackup->CleanUp();
					bNothingCopied = TRUE;
				}
			}
			
			if(!bNothingCopied)
			{
				// write Sequence Map 
				m_pBackup->WriteCameraMaps();
				// write Sequence Map 
				m_pBackup->WriteSequenceMap();
				// end everything is ok
			}
		}
		else
		{
			// write Sequence Map 
			m_pBackup->WriteCameraMaps();
			// write Sequence Map 
			m_pBackup->WriteSequenceMap();
			// end everything is ok
		}

		// signal the end
		pCIPCDatabase->DoConfirmBackup(m_pBackup->GetUserData(),
									   m_pBackup->GetDestinationPath(),
									   MAKELONG(0,0));
		WK_TRACE(_T("backup finished\n"));
		m_bFinished = TRUE;
		return FALSE;
	}

	// Initialize, create directory and so on
	if (!m_pBackup->Initialized())
	{
		if (m_pBackup->Initialize())
		{
			m_dwLastAction = GetTickCount();
			return TRUE;
		}
		else
		{
			CString sDirectory, sErrorMessage;
			CString sKamera = _T("--");
			CString sBackupError = GetLastErrorString();
			CString sError = _T("backup initializing failed. \n");
			WK_TRACE_ERROR(sError);
			DoIndicateError(2,m_pBackup->GetUserData());

			//something during copy failed
			//cancel the copy process of all following sequences and archives
			Cancel();

			//create RTE
			if(m_pBackup)
			{
				sDirectory = m_pBackup->GetDestinationPath();
			}

			//send RTE
			sErrorMessage.Format(IDS_AUTO_BACKUP_FAILED ,sKamera, sDirectory);
			sErrorMessage = sErrorMessage + _T(" ") +sBackupError;

			CWK_RunTimeError e(WAI_DATABASE_SERVER,
				REL_ERROR,
				RTE_STORAGE,
				sErrorMessage,0,0);
			e.Send();

			//return FALSE;
			return TRUE; //return TRUE because cancelation will be handled on the next call of
						//this function "RunCopy" in "if (m_bCancelled)"
		}
	}

	// do the real copy
	m_dwID = m_pBackup->GetIDNr(m_iCurrentSequence);
	WORD wArchiveNr   = HIWORD(m_dwID);
	WORD wSequenceNr = LOWORD(m_dwID);

	// increase index
	m_iCurrentSequence++;

	// id wrong try the next
	if (m_dwID == 0)
	{
		WK_TRACE_ERROR(_T("backup Archiv nr or Sequence nr not found\n"));
		return TRUE;
	}

	// is the Archive still there
	CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchiveNr);
	if (pArchiv==NULL)
	{
		WK_TRACE_ERROR(_T("Archiv for backup not found %d\n"),wArchiveNr);
		return TRUE;
	}

	// is the Sequence still there
	CSequence* pSequence = pArchiv->GetSequenceID(wSequenceNr);
	if (pSequence==NULL)
	{
		WK_TRACE_ERROR(_T("Sequence for backup not found %d\n"),wSequenceNr);
		m_dwLastAction = GetTickCount();
		return TRUE;
	}

	// everything OK

	// calc the backup timeout limit
	DWORD dwSizeBytes = (DWORD)pSequence->GetSizeBytes().GetInt64();
	// 150 kB/s = 150*1024B/1000ms = 153,6 Byte / ms
	// assume minimum speed of 50 Byte / ms
	// safety reserve of 60 s
	// now calc timeout in ms (!)
	m_dwTimeOut = dwSizeBytes / 50 + 60 * 1000;
	m_dwLastAction = GetTickCount();

	// call the backup
	BOOL bRet = TRUE;
	BOOL bSequenceNotInTimeSpan = FALSE;
	BOOL bErrorSharingViolation = FALSE;

	//automatic backup from a whole archive -> m_pBackup->GetFlags() == 0
	//automatic backup from some sequences -> m_pBackup->GetFlags() == BBT_EXECUTE_AUTO
	if (m_pBackup->GetFlags() == 0) 
	{
		bRet = pSequence->Backup(*m_pBackup, bErrorSharingViolation);
	}
	else
	{
		bRet = pSequence->BackupByTime(*m_pBackup, bSequenceNotInTimeSpan, bErrorSharingViolation);
	}
	
	if (bRet) //copy of backup sequences success
	{
		pSequence->DoRequestCloseSequence(m_pBackup->GetUserData());
		pClient = theApp.m_Clients.GetClient(m_pBackup->GetClientID());
		if (pClient==NULL)
		{
			WK_TRACE_ERROR(_T("backup stopped by disconnect or cancel\n"));
			return FALSE;
		}
		pCIPCDatabase->DoConfirmBackup(m_pBackup->GetUserData(),
									   m_pBackup->GetDestinationPath(),
									   m_dwID);
		m_dwLastAction = GetTickCount();
		// to give other threads more breath
		Sleep(50);
		// check whether it was an Archiv to backup
		// and it's was the last Sequence
		if (m_pBackup->WasLastSequenceOfArchive(wArchiveNr,wSequenceNr))
		{

			WK_TRACE(_T("last sequence of %04hx backup complete\n"),wArchiveNr);
			pCIPCDatabase->DoConfirmBackup(m_pBackup->GetUserData(),
										   m_pBackup->GetDestinationPath(),
										   MAKELONG(0,wArchiveNr));
		}
	}

	//automatic backup from some sequences -> m_pBackup->GetFlags() == BBT_EXECUTE_AUTO
	//if copying of one sequence failed, it could be, because the sequence is not in the
	//time span for automatic partly backup(e.g. the last 24 hours). That means do not
	//cancel the whole backup, just wait until all sequences were checked against the time span
	//if a copy failed because of e.g. not enough space on target device, cancel the automatic backup
	else if(m_pBackup->GetFlags() == BBT_EXECUTE_AUTO)
	{
		if(!bSequenceNotInTimeSpan)
		{
			//if one sequence couldn`t be copied because of 
			//ERROR_SHARING-VIOLATION (Der Prozess kann nicht auf die Datei zugreifen,
			//da sie von einem anderen Prozess verwendet wird)
			//go on copying the next sequences and do not cancel the backup of the whole archive
			if(!bErrorSharingViolation)
			{
				WK_TRACE(_T("Cancel partly automatic backup\n"));
				CancelAutomaticBackup(pSequence);
			}
		}
	}
	//automatic backup from a whole archive -> m_pBackup->GetFlags() == 0,
	//if copying of one sequence failed, stop the whole copy process
	else if (m_pBackup->GetFlags() == 0)
	{
		//if one sequence couldn`t be copied because of 
		//ERROR_SHARING-VIOLATION (Der Prozess kann nicht auf die Datei zugreifen,
		//da sie von einem anderen Prozess verwendet wird)
		//go on copying the next sequences and do not cancel the backup of the whole archive
		if(!bErrorSharingViolation)
		{
			WK_TRACE(_T("Cancel automatic backup\n"));
			CancelAutomaticBackup(pSequence);
		}
	}
	else
	{
		// OK nothing todo, because sequence is out of time 
	}


	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CBackupThread::CancelAutomaticBackup(CSequence* pSequence)
{
	//automatic backup
	m_dwLastAction = GetTickCount();
	CString sBackupError = GetLastErrorString();
	//3 => backup copy error
	DoIndicateError(3,m_dwID);

	//something during copy failed
	//set the flag for cancelation of the copy process to stop copying
	//all following sequences and archives
	Cancel();

	//delete the current sequence (no copy success) and all further sequences from the sequence list
	//and delete this not complete copied sequence (e.g. 000c0003.dbd AND 000c0003.dbf)
	//from the backup archive on the backup drive. 

	if(pSequence)
	{
		//delete current sequence from sequence list
		WORD wArchive = pSequence->GetArchivNr();
		WORD wSequence = pSequence->GetNr();

		for (int i=0;i<m_pBackup->GetNrOfIDs();i++)
		{
			DWORD dwID = m_pBackup->GetIDNr(i);
			WORD wArchiveNr   = HIWORD(dwID);
			WORD wSequenceNr = LOWORD(dwID);

			if(    wArchive == wArchiveNr
				&& wSequence == wSequenceNr)
			{
				//shrink the sequence list to all complete copied sequences
				m_pBackup->RemoveAllFurtherIDsFromList(i);
				break;
			}
		}
	}	

	//create RTE
	CString sDirectory;
	CString sKamera;
	if(m_pBackup)
	{
		sDirectory = m_pBackup->GetDestinationPath();
		if(pSequence)
		{
			CArchiv* pArchive = pSequence->GetArchiv();
			if(pArchive)
			{
				sKamera = pArchive->GetName();
			}
		}
	}

	//send rte
	CString sErrorMessage;
	sErrorMessage.Format(IDS_AUTO_BACKUP_FAILED ,sKamera, sDirectory);
	sErrorMessage = sErrorMessage + _T(" ") +sBackupError;

	CWK_RunTimeError e(WAI_DATABASE_SERVER,
		REL_ERROR,
		RTE_STORAGE,
		sErrorMessage,0,0);
	e.Send();
}
//////////////////////////////////////////////////////////////////////
BOOL CBackupThread::RunCalculate()
{
	CIPCInt64 iSum = 0;
	CIPCInt64 iOne = 0;
	DWORD dwTick = GetTickCount();
	CClient* pClient = NULL;

	for (int i=0;i<m_pBackup->GetNrOfIDs();i++)
	{
		DWORD dwID = m_pBackup->GetIDNr(i);
		WORD wArchiveNr   = HIWORD(dwID);
		WORD wSequenceNr = LOWORD(dwID);

		CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchiveNr);
		
		if (   pArchiv
			&& !pArchiv->IsBackup())
		{
			if (wSequenceNr != 0)
			{
				CSequence* pSequence = pArchiv->GetSequenceID(wSequenceNr);
				if (   pSequence
					&& !pSequence->IsBackup())
				{
					iOne = pSequence->GetSizeBytes(m_pBackup->GetStartTime(),
												   m_pBackup->GetEndTime());
					if (iOne>0)
					{
						pClient = theApp.m_Clients.GetClient(m_pBackup->GetClientID());
						if (   pClient
							&& pClient->IsConnected()
							&& !m_bCancelled)
						{
							pClient->GetCIPCDatabaseServer()->DoConfirmBackupByTime((WORD)m_pBackup->GetUserData(),
								dwID,
								m_pBackup->GetStartTime(),
								m_pBackup->GetEndTime(),
								(WORD)BBT_CALCULATE,
								iOne);
						}
						iSum += iOne;
						if (   m_pBackup->GetMaximumInMB()>0
							&& iSum.GetInMB()>m_pBackup->GetMaximumInMB())
						{
							break;
						}
					}
				}
			}
			else
			{
				// calculate the entire archive
				iOne = pArchiv->GetSizeBytes(m_pBackup->GetStartTime(),
												 m_pBackup->GetEndTime(),
												 iSum,
												 m_pBackup->GetMaximumInMB(),
												 m_bCancelled);
				if (iOne>0)
				{
					pClient = theApp.m_Clients.GetClient(m_pBackup->GetClientID());
					if (   pClient
						&& pClient->IsConnected())
					{
						pClient->GetCIPCDatabaseServer()->DoConfirmBackupByTime((WORD)m_pBackup->GetUserData(),
																				 dwID,
																				m_pBackup->GetStartTime(),
																				m_pBackup->GetEndTime(),
																				 (WORD)BBT_CALCULATE,
																				 iOne);
					}
					if (   m_pBackup->GetMaximumInMB()>0
						&& iSum.GetInMB()>m_pBackup->GetMaximumInMB())
					{
						break;
					}
				}
			}
		}

		if (m_bCancelled)
		{
			pClient = theApp.m_Clients.GetClient(m_pBackup->GetClientID());
			if (   pClient
				&& pClient->IsConnected())
			{
				pClient->GetCIPCDatabaseServer()->DoConfirmCancelBackup(m_pBackup->GetUserData());
			}
			break;
		}
	}

	pClient = theApp.m_Clients.GetClient(m_pBackup->GetClientID());
	if (   pClient
		&& pClient->IsConnected())
	{
		pClient->GetCIPCDatabaseServer()->DoConfirmBackupByTime((WORD)m_pBackup->GetUserData(),
																 0,
																m_pBackup->GetStartTime(),
																m_pBackup->GetEndTime(),
																 (WORD)BBT_CALCULATE,
																 iSum);
	}
	WK_TRACE(_T("CALCULATE BACKUP in %d ms %s\n"),GetTimeSpan(dwTick),iSum.Format(TRUE));
	// Run only one time
	return FALSE;
}
DWORD GetDifference(DWORD dw1, DWORD dw2)
{
	if (dw1>dw2)
	{
		return dw1 - dw2;
	}
	else
	{
		return dw2 - dw1;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CBackupThread::RunCalculateEnd()
{
	CIPCInt64 s1=0,s2 = 0;
	CIPCInt64 one = 0;
	CIPCInt64 max = m_pBackup->GetMaximumInMB();
	max *= 1024*1024;
	DWORD dwTick = GetTickCount();
	CClient* pClient = theApp.m_Clients.GetClient(m_pBackup->GetClientID());
	CDWordArray dwIDs;
	CSystemTime start,t1,t2;
	CSequences sequences;


	// first check the IDs no Sequence allowed
	for (int i=0;i<m_pBackup->GetNrOfIDs();i++)
	{
		DWORD dwID = m_pBackup->GetIDNr(i);
		WORD wArchiveNr   = HIWORD(dwID);
		WORD wSequenceNr = LOWORD(dwID);
		if (wSequenceNr !=0)
		{
			pClient->GetCIPCDatabaseServer()->DoIndicateError(CIPC_DB_REQUEST_BACKUP_BY_TIME,dwID,CIPC_ERROR_INVALID_VALUE,0);
			return FALSE;
		}
		TRACE(_T("RunCalculateEnd add id %08lx\n"),dwID);
		dwIDs.Add(dwID);
		CArchiv* pArchive = theApp.m_Archives.GetArchiv(wArchiveNr);
		if (   pArchive
			&& !pArchive->IsBackup())
		{
			for (int j=0;j<pArchive->GetNrSequences();j++)
			{
				CSequence* pSequence = pArchive->GetSequenceIndex(j);
				if (!pSequence->IsBackup())
				{
					sequences.Add(pSequence);
				}
				if (m_bCancelled)
				{
					pClient = theApp.m_Clients.GetClient(m_pBackup->GetClientID());
					if (   pClient
						&& pClient->IsConnected())
					{
						pClient->GetCIPCDatabaseServer()->DoConfirmCancelBackup(m_pBackup->GetUserData());
					}
					break;
				}
			}
		}
		if (m_bCancelled)
		{
			pClient = theApp.m_Clients.GetClient(m_pBackup->GetClientID());
			if (   pClient
				&& pClient->IsConnected())
			{
				pClient->GetCIPCDatabaseServer()->DoConfirmCancelBackup(m_pBackup->GetUserData());
			}
			break;
		}
	}

	WK_TRACE(_T("CALCULATE BACKUP END sequences evaluated in %d ms\n"),GetTimeSpan(dwTick));
	sequences.Sort(m_bCancelled);
	WK_TRACE(_T("CALCULATE BACKUP END sequences sorted in %d ms\n"),GetTimeSpan(dwTick));

	start = m_pBackup->GetStartTime();

	for (i=0;i<sequences.GetSize();i++)
	{
		CSequence* pSequence = sequences.GetAtFast(i);
		// cut only in front
		one = pSequence->GetSizeBytes(start,pSequence->GetLastTime());
		s1 = s2;
		s2 += one;
		if (m_bCancelled)
		{
			pClient = theApp.m_Clients.GetClient(m_pBackup->GetClientID());
			if (   pClient
				&& pClient->IsConnected())
			{
				pClient->GetCIPCDatabaseServer()->DoConfirmCancelBackup(m_pBackup->GetUserData());
			}
			break;
		}
		pClient->GetCIPCDatabaseServer()->DoConfirmBackupByTime((WORD)m_pBackup->GetUserData(),
												  MAKELONG(pSequence->GetNr(),pSequence->GetArchivNr()),
												  pSequence->GetFirstTime(),
												  pSequence->GetLastTime(),
												  (WORD)BBT_CALCULATE_END,
												  one);
		if (s2 > max)
		{
			t2 = pSequence->GetLastTime();
			break;
		}
		t1 = pSequence->GetFirstTime();
		TRACE(_T("%d Sequences %04hx,%04hx %s Bytes %s\n"),i,
			pSequence->GetArchivNr(),pSequence->GetNr(),s2.Format(TRUE),t1.GetDateTime());
	}

	if (s2 < max)
	{
		t2.GetLocalTime();
	}

	WK_TRACE(_T("CALCULATE BACKUP END in %d ms MinMB: %s  MaxMB: %s , %s->%s\n"),
		GetTimeSpan(dwTick),
		s1.Format(TRUE),
		s2.Format(TRUE),
		t1.GetDateTime(),
		t2.GetDateTime());

	pClient->GetCIPCDatabaseServer()->DoConfirmBackupByTime((WORD)m_pBackup->GetUserData(),
											  0,
											  t1,
											  t2,
											  (WORD)m_pBackup->GetFlags(),
											  s2);

	sequences.RemoveAll();

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CBackupThread::Cancel()
{
	WK_TRACE(_T("backup cancelled\n"));
	m_bCancelled = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CBackupThread::DoIndicateError(int iUnitCode, DWORD dwID)
{
	CClient* pClient;
	pClient = theApp.m_Clients.GetClient(m_pBackup->GetClientID());
	if (pClient)
	{
		CIPCDatabaseServer* pCIPCDatabase = pClient->GetCIPCDatabaseServer();
		pCIPCDatabase->DoIndicateError(
						 CIPC_DB_REQUEST_BACKUP, 
						 dwID, 
						 CIPC_ERROR_BACKUP, 
						 iUnitCode,
						 GetLastErrorString());
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CBackupThread::IsTimedOut()
{
	if (!m_bFinished)
	{
		DWORD dwDiff = GetTimeSpan(m_dwLastAction);
		if (dwDiff>m_dwTimeOut)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CBackupThread::HandleTimeOut()
{
	if (m_dwTimeOutHandled == 0)
	{
		WK_TRACE(_T("backup time out %d\n"),m_pBackup->GetClientID());
		DoIndicateError(4,m_dwID);
		m_dwTimeOutHandled = GetTickCount();
	}
	else
	{
		if (GetTimeSpan(m_dwTimeOutHandled)>5*1000)
		{
			WK_TRACE(_T("reset by backup timeout\n"));
			theApp.DoReset();
		}
	}
}
