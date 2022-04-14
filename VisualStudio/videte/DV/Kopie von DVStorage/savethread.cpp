// SaveThread.cpp: implementation of the CSaveThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVStorage.h"
#include "SaveThread.h"

#include "wk_timer.h"
extern CWK_Timer theTimer;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


LARGE_INTEGER liSaved, liArrived;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSaveThread::CSaveThread()
	: CWK_Thread(_T("SaveThread"),NULL),m_Statistics(1000)
{
	// CWK_Thread members
	m_dwStatTime = 60 * 60 * 1000;	
	m_dwWaitTime = 1;
	m_bIsRunning = FALSE;

	// Own members
	m_dwAlarmAndPreAlarmDefinedInMB = 0;
	m_iNrOfRingArchives = 0;
	m_dwMustBeFreeMB = 100;
	m_dwStoredWithoutConfirm = 0;
	m_dwAlarmAndPreAlarmSafetyInMB = 0;
	
	m_bVerify = FALSE;
	m_wArchive = 0;
	m_wSequence = 0;
	m_nBuffer = 0;

	liArrived.QuadPart = 0;
	liSaved.QuadPart = 0;
}
//////////////////////////////////////////////////////////////////////
CSaveThread::~CSaveThread()
{
	DWORD dwSize = m_DataQueues[0].GetCount() + m_DataQueues[1].GetCount();
	if (dwSize>0)
	{
		WK_TRACE(_T("thread %s has still %d pictures\n"), m_sName, dwSize);
	}

	double dSaved = 100.0 * (double)liSaved.QuadPart / (double)liArrived.QuadPart;
	WK_TRACE(_T("Percent Saved: %f"), dSaved);
	EmptyDataQueue();
}
//////////////////////////////////////////////////////////////////////
void CSaveThread::Reset()
{
	m_dwAlarmAndPreAlarmDefinedInMB = theApp.m_Archives.GetAlarmAndPreAlarmDefinedMB();
	if (theApp.m_Archives.GetNrOfAlarmArchives()>0)
	{
		m_dwAlarmAndPreAlarmSafetyInMB = theApp.m_Archives.GetAlarmAndPreAlarmDefinedMB();
		m_dwAlarmAndPreAlarmSafetyInMB /= theApp.m_Archives.GetNrOfAlarmArchives();
		m_dwAlarmAndPreAlarmSafetyInMB /= 20;
	}
	else
	{
		m_dwAlarmAndPreAlarmSafetyInMB = 0;
	}
	
	theApp.m_Drives.Initialize();

	m_iNrOfRingArchives = theApp.m_Archives.GetNrOfRingArchives();
	DWORD dwSequenceSizeMB = theApp.m_Archives.GetSequenceSizeInMB();
	m_dwMustBeFreeMB = dwSequenceSizeMB * m_iNrOfRingArchives;
	DWORD dw5PercentOfHarddiskCapacity = theApp.m_Drives.GetAvailableMB() / 20;
	if (m_dwMustBeFreeMB < dw5PercentOfHarddiskCapacity)
	{
		m_dwMustBeFreeMB = dw5PercentOfHarddiskCapacity;
	}
	m_dwStoredWithoutConfirm = 0;
	WK_TRACE(_T("%d MB must be free on %d drive(s) for %d ring archive(s), Sequence size is %d MB\n"),
		m_dwMustBeFreeMB, theApp.m_Drives.GetNrOfDBDrives(), m_iNrOfRingArchives,dwSequenceSizeMB);
}
//////////////////////////////////////////////////////////////////////
void CSaveThread::EmptyDataQueue()
{
	m_csQueue.Lock();
	m_DataQueues[0].DeleteAll();
	m_DataQueues[1].DeleteAll();
	m_csQueue.Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CSaveThread::StartThread()
{
	Reset();
	m_bVerify = !theApp.IsFastResetting();
	m_bRun = TRUE;
	return CWK_Thread::StartThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CSaveThread::StopThread()
{
	m_bRun = FALSE;
	EmptyDataQueue();
	return CWK_Thread::StopThread();
}
/////////////////////////////////////////////////////////////////////////
void CSaveThread::SortQueue(int iMax/*=0*/)
{
	CImageDataPtrList *pList = &m_DataQueues[m_nBuffer];
	// nur die halbe queue sortieren
	if (iMax == 0)
	{
		iMax = pList->GetCount();
	}
	int iCounter = 0;
	int iCurrent = 0;
	POSITION posCurrent = NULL;
	POSITION posIteration = NULL;
	WORD wArchivNr;
	CImageData* pImageData = NULL;

	if (pList->GetCount()>0)
	{
//		TraceQueue(iMax);

		iCurrent = 1;
		posCurrent = pList->GetHeadPosition();

		while (posCurrent && iCounter<iMax)
		{
//			TRACE(_T("iCounter=%d,iMax=%d\n"),iCounter,iMax);
			pImageData = pList->GetNext(posCurrent);
			wArchivNr = pImageData->GetCollectionID().GetSubID();
//			TRACE(_T("A=%02x\n"),wArchivNr&0x0F);
			posIteration = posCurrent;
			iCounter++;
			while (posIteration && iCounter<iMax)
			{
				POSITION posRemove = posIteration;
				
				pImageData = pList->GetNext(posIteration);
				// posIteration ist schon ein weiter als das posRemove!
				iCounter++;

				if (pImageData->GetCollectionID().GetSubID() == wArchivNr)
				{
					// element von posIteration vor posCurrent einfügen
					if (posCurrent != posRemove)
					{
						pList->InsertBefore(posCurrent,pImageData);
						pList->RemoveAt(posRemove);
					}
					iCurrent++;
				}

			}
			// ein archiv fertig sortiert weiter geht es an posCurrent mit
			// dem nächsten Archiv
			iCounter = iCurrent+1;
		}
#ifdef _DEBUG
//		TraceQueue(iMax);
#endif
	}
}
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CSaveThread::TraceQueue(int iMax/*=0*/)
{
	CImageDataPtrList *pList = &m_DataQueues[m_nBuffer];

	if (iMax == 0)
	{
		iMax = pList->GetCount();
	}
	CString sTrace,s;
	POSITION pos = pList->GetHeadPosition();

	while (pos && iMax--)
	{
		CImageData* pImageData = pList->GetNext(pos);
		s.Format(_T("%01x,%02d,%03d|"),
			pImageData->GetCollectionID().GetSubID()&0x000F,
			pImageData->GetTimeStamp().GetSecond(),
			pImageData->GetTimeStamp().GetMilliseconds()
			);
		sTrace += s;
	}

	TRACE(_T("QUEUE:%s\n"),sTrace);
}
#endif
//////////////////////////////////////////////////////////////////////
BOOL CSaveThread::Run(LPVOID lpContext)
{
	int i = 0, nCount, nMax;

	LARGE_INTEGER liOld, liTick;
	// check times for the several parts inside the saving loop
	DWORD dwStore=0;

	m_csQueue.Lock();
	m_nBuffer = !m_nBuffer;
	m_csQueue.Unlock();

	CImageDataPtrList *pList = &m_DataQueues[m_nBuffer];
	
	liOld = theTimer.GetMicroTicks();

	while (m_bRun)
	{
		pList = &m_DataQueues[m_nBuffer];
		nCount = pList->GetCount();
		if (theApp.m_Dlg.m_hWnd)
		{
			theApp.m_Dlg.PostMessage(WM_USER, IDC_TXTINT_SAVE_BUFFER, m_nBuffer);
			theApp.m_Dlg.PostMessage(WM_USER, IDC_TXTINT_SAVE_BUFFER_COUNT, nCount);
		}
		nMax = 2000;

		if (nCount)
		{
			liArrived.QuadPart += nCount;
			dwStore = 0;

			if (nCount > nMax)
			{
				WK_TRACE_ERROR(_T("TOO MANY images in queue %d %s\n"),pList->GetCount(),m_sName);
				nCount = nMax;
			}

			for (i=0; i<nCount; i++) 
			{
				if (!m_bRun)
				{
					return FALSE;
				}
				if (CIPCExtraMemory::GetCreatedBubbles() > 1000)
				{
					WK_TRACE(_T("TOO MANY Bubbles created: Breaking at %d of %d\n"), i, nCount);
					nCount = i;
					break;
				}
				// take FirstOut picture of queue
				CImageData* pImageData = pList->RemoveHead();

				BOOL bNewSequenceCreated = FALSE;
				if (pImageData)
				{
					liOld = theTimer.GetMicroTicks();
					CAutoCritSec acs(&theApp.m_Archives.m_cs);
					CCollection* pCollection = theApp.m_Archives.GetCollection(pImageData->GetCollectionID().GetSubID());
					if (pCollection)
					{
						if (pCollection->Store(*pImageData, bNewSequenceCreated))
						{
							Sleep(0); // RKE: does sleep make sense here?
							ADD_MICRO_TIME_SPAN(dwStore, liTick, liOld);
							// OK confirm it to CIPC
							int i, nNumPics = 0;
							const CIPCPictureRecord*pPict = pImageData->GetPictureRecord();
							if (pPict)
							{
								nNumPics = pPict->GetNumberOfPictures();
								if (nNumPics == 0)
								{
									nNumPics = 1;
								}
								if (m_dwStoredWithoutConfirm++ == 50)
								{
									if (   theApp.GetCIPCDatabaseStorage()
										&& theApp.GetCIPCDatabaseStorage()->GetIPCState() == CIPC_STATE_CONNECTED)
									{
										theApp.GetCIPCDatabaseStorage()->DoConfirmNewSavePicture(pCollection->GetNr(),
											pImageData->GetPictureRecord()->GetCamID());
										m_dwStoredWithoutConfirm = 0;
									}
								}
							}
							for (i=0; i<nNumPics; i++)
							{
								m_Statistics.AddValue(pImageData->GetLength(i));
							}
						}
						else
						{
							bNewSequenceCreated = TRUE;
						}
					}
					acs.Unlock();
				}

				WK_DELETE(pImageData);

				if (bNewSequenceCreated)
				{
					CheckAndDelete();
				}
			}

			if (pList->GetCount())
			{
				pList->DeleteAll();
			}
		}
		else
		{
			Verify();
		}

		theApp.m_Archives.CloseAll();

		if (nCount)
		{
			liSaved.QuadPart += nCount;
			if (theApp.m_Dlg.m_hWnd)
			{
				dwStore /= nCount;
				LPARAM *plParam;
				float fMBperSecond = (float)m_Statistics.GetMBperSecond();
				float fFPS         = (float)m_Statistics.GetFPS();
//				DWORD dwAverage    = m_Statistics.GetAverageValue();
//				int   nBitrate	   = m_Statistics.GetBitrate();
				theApp.m_Dlg.PostMessage(WM_USER, IDC_TXTINT_STORE, (LPARAM)dwStore);
				plParam = (LPARAM*)&fMBperSecond;
				theApp.m_Dlg.PostMessage(WM_USER, IDC_TXTINT_BITRATEMB, *plParam);
				plParam = (LPARAM*)&fFPS;
				theApp.m_Dlg.SendMessage(WM_USER, IDC_TXTINT_FPS, *plParam);
//				theApp.m_Dlg.SendMessage(WM_USER, 16, (LPARAM)dwAverage);
//				theApp.m_Dlg.SendMessage(WM_USER, 17, (LPARAM)nBitrate);

			}
		}
		m_csQueue.Lock();
		m_nBuffer = !m_nBuffer;
		m_csQueue.Unlock();
		Sleep(5);
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////
void CSaveThread::CheckAndDelete()
{
	if (   !theApp.IsResetting()
		&& !theApp.IsFastResetting())
	{
		// let space for Alarm/PreAlarm
		DWORD dwAlarmAndPreAlarmUsedInMB = theApp.m_Archives.GetAlarmAndPreAlarmUsedMB();
		DWORD dwNecessaryForAlarm = 0;
		
		if (m_dwAlarmAndPreAlarmDefinedInMB > dwAlarmAndPreAlarmUsedInMB)
		{
			dwNecessaryForAlarm = m_dwAlarmAndPreAlarmDefinedInMB - dwAlarmAndPreAlarmUsedInMB;
		}
		dwNecessaryForAlarm += m_dwAlarmAndPreAlarmSafetyInMB;

		// check disk space

		CAutoCritSec acsD(&theApp.m_Drives.m_cs);
		theApp.m_Drives.CheckSpace();
		DWORD dwFreeMB = theApp.m_Drives.GetFreeMB();
		acsD.Unlock();

		// adjust space for Alarm/PreAlarm
		while ((dwFreeMB + 10) < dwNecessaryForAlarm)
		{
			WK_TRACE(_T("not enough space for Alarm/PreAlarm %d>%d MB, delete something\n"),dwNecessaryForAlarm,dwFreeMB);
			
			theApp.m_Archives.DeleteOldestRingSequence();

			theApp.m_Drives.Lock();
			theApp.m_Drives.CheckSpace();
			dwFreeMB = theApp.m_Drives.GetFreeMB();
			theApp.m_Drives.Unlock();
		}

		dwFreeMB = dwFreeMB - dwNecessaryForAlarm; 

		while (dwFreeMB < m_dwMustBeFreeMB)
		{
			theApp.m_Drives.Lock();
			theApp.m_Drives.CheckSpace();
			dwFreeMB = theApp.m_Drives.GetFreeMB() - dwNecessaryForAlarm;
			theApp.m_Drives.Unlock();
			theApp.m_Archives.DeleteOldestRingSequence();
		}
	}
}
/////////////////////////////////////////////////////////////////////////
void CSaveThread::Verify()
{
	if (m_bVerify)
	{
		if (   m_wArchive == 0
			&& m_wSequence == 0)
		{
			WK_TRACE(_T("starting verify\n"));
		}
		CTape* pTape = theApp.m_Archives.GetNextSequence(m_wArchive,m_wSequence);
		if (pTape)
		{
			pTape->Verify();
			m_wArchive = pTape->GetArchiveNr();
			m_wSequence = pTape->GetNr();
		}
		else
		{
			theApp.GetSequenceMap().Flush();
			WK_TRACE(_T("verify finished\n"));
			m_bVerify = FALSE;
			m_wSequence = 0;
			m_wArchive = 0;
		}
	}
}
