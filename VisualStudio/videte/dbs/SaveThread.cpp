// SaveThread.cpp: implementation of the CSaveThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBS.h"
#include "SaveThread.h"
#include "Mainfrm.h"
#include "OscopeView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
	#define _MAX_DATA_QUEUE_LENGTH	1000
#else
	#define _MAX_DATA_QUEUE_LENGTH	1000
#endif
#define _NR_OF_DATA_PER_RUN	50
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSaveThread::CSaveThread()
	: CWK_Thread(_T("SaveThread"),NULL)	, m_MBPerSecond(1000)
{
	m_dwLastTrace = GetTickCount();
	m_dwCurrentTick = GetTickCount();
	m_dwStatTime = 60 * 60 * 1000;
	m_bIsRunning = FALSE;
	m_currentTime.GetLocalTime(); 

	m_nBuffer = 0;
	m_bRun = FALSE;
	m_dwStoredWithoutConfirm = 0;

	m_dwAlarmAndPreAlarmDefinedInMB = 0;
	m_dwAlarmAndPreAlarmSafetyInMB = 0;
	m_dwMustBeFreeMB = 500;

	m_dwLastCheckandDelete = 0;

}
//////////////////////////////////////////////////////////////////////
CSaveThread::~CSaveThread()
{
	DWORD dwSize = m_DataQueues[0].GetCount()+m_DataQueues[1].GetCount();
	if (dwSize>0)
		WK_TRACE(_T("thread %s has still %d pictures\n"),m_sName,dwSize);
	EmptyDataQueue();
}
//////////////////////////////////////////////////////////////////////
BOOL CSaveThread::StartThread()
{
	m_bRun = TRUE;
	DWORD dwTotalMB = theApp.m_Drives.GetTotalHDSizeMB();
	if (dwTotalMB/50>m_dwMustBeFreeMB)
	{
		m_dwMustBeFreeMB = dwTotalMB/50;
		WK_TRACE(_T("new must be free %d MB\n"),m_dwMustBeFreeMB);
	}
	CheckAndDelete(TRUE);
	return CWK_Thread::StartThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CSaveThread::StopThread()
{
	m_bRun = FALSE;
	EmptyDataQueue();
	return CWK_Thread::StopThread(10000);
}
//////////////////////////////////////////////////////////////////////
void CSaveThread::Reset()
{
	int iNrOfDynamicArchives = theApp.m_Archives.GetNrOfDynamicArchives();
	int iNrOfFixedArchives = theApp.m_Archives.GetNrOfFixedArchives();

	if (iNrOfFixedArchives>0)
	{
		m_dwAlarmAndPreAlarmDefinedInMB = theApp.m_Archives.GetFixedSizeDefinedMB();
		m_dwAlarmAndPreAlarmSafetyInMB /= iNrOfFixedArchives;
		m_dwAlarmAndPreAlarmSafetyInMB /= 20;
	}
	else
	{
		m_dwAlarmAndPreAlarmSafetyInMB = 0;
	}

	DWORD dwSequenceSizeMB = theApp.m_Archives.GetAverageDynamicSequenceSizeMB();
	m_dwMustBeFreeMB = dwSequenceSizeMB * iNrOfDynamicArchives;
	DWORD dw5PercentOfHarddiskCapacity = theApp.m_Drives.GetAvailableMB() / 20;
	if (m_dwMustBeFreeMB < dw5PercentOfHarddiskCapacity)
	{
		m_dwMustBeFreeMB = dw5PercentOfHarddiskCapacity;
	}
	m_dwStoredWithoutConfirm = 0;
	WK_TRACE(_T("%d MB must be free for %d ring archive(s), Sequence size is %d MB\n"),
		m_dwMustBeFreeMB, iNrOfDynamicArchives,dwSequenceSizeMB);
}
//////////////////////////////////////////////////////////////////////
CStatistics& CSaveThread::GetStatistics()
{
	return m_MBPerSecond;
}
//////////////////////////////////////////////////////////////////////
void CSaveThread::EmptyDataQueue()
{
	CAutoCritSec acs(&m_csQueue);
	m_DataQueues[0].DeleteAll();
	m_DataQueues[1].DeleteAll();
	acs.Unlock();
}
//////////////////////////////////////////////////////////////////////
CString CSaveThread::Format()
{
	CString s;
	s.Format(_T("%s %d Queue %.3f MB/s %.3f FPS "),
		m_sName,
		m_DataQueues[0].GetCount()+m_DataQueues[1].GetCount(),
		m_MBPerSecond.GetMBperSecond(),
		m_MBPerSecond.GetFPS());
	return s;
}
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CSaveThread::TraceQueue(int iMax/*=0*/)
{
	CSaveDataPtrList *pList = &m_DataQueues[m_nBuffer];

	if (iMax == 0)
	{
		iMax = pList->GetCount();
	}
	CString sTrace,s;
	POSITION pos = pList->GetHeadPosition();

	while (pos && iMax--)
	{
		CSaveData* pSaveData = pList->GetNext(pos);
		s.Format(_T("%d,%02d,%03d|"),
			pSaveData->GetArchiv()->GetNr(),
			pSaveData->GetTimeStamp().GetSecond(),
			pSaveData->GetTimeStamp().GetMilliseconds()
			);
		sTrace += s;
	}

	TRACE(_T("QUEUE:%s\n"),sTrace);
}
#endif
//////////////////////////////////////////////////////////////////////
BOOL CSaveThread::Run(LPVOID lpContext)
{
	CCallStackEntry cse(_T("Run"));
	int i = 0, nCount, nMax;

	m_csQueue.Lock();
	m_nBuffer = !m_nBuffer;
	m_csQueue.Unlock();

	CSaveDataPtrList* pList = &m_DataQueues[m_nBuffer];

	while (m_bRun)
	{
		m_dwCurrentTick = GetTickCount();
		pList = &m_DataQueues[m_nBuffer];
		nCount = pList->GetCount();
		nMax = 2000;

		if (nCount)
		{
			// _NR_OF_DATA_PER_RUN Bilder in einem Run auswerten,
			// damit das Wait auf Ende 10ms nicht zu sehr bremst
			for (i=0; i<nCount; i++)
			{
				if (!m_bRun)
				{
					return FALSE;
				}
				if (CIPCExtraMemory::GetCreatedBubbles() > nMax)
				{
					WK_TRACE(_T("TOO MANY Bubbles created: Breaking at %d of %d\n"), i, nCount);
					nCount = i;
					break;
				}
				// take FirstOut picture of queue
				BOOL bNewSequenceCreated = FALSE;
				CSaveData* pSaveData = pList->RemoveHead();
				if (   pSaveData 
					&& pSaveData->Save(bNewSequenceCreated))
				{
					const CIPCPictureRecord* pPict = pSaveData->GetPictureRecord();
					if (pPict && pPict->GetPictureType() == SPT_GOP_PICTURE)
					{
						for (DWORD j=0;j<pPict->GetNumberOfPictures();j++)
						{
							m_MBPerSecond.AddValue(pPict->GetDataLength(j));
						}
					}
					else
					{
						m_MBPerSecond.AddValue(pSaveData->GetDataLength());
					}
					if (m_dwStoredWithoutConfirm++ == 50)
					{
						if (   theApp.GetCIPCDatabaseToServer()
							&& theApp.GetCIPCDatabaseToServer()->GetIPCState() == CIPC_STATE_CONNECTED)
						{
							theApp.GetCIPCDatabaseToServer()->DoConfirmNewSavePicture(
								pSaveData->GetArchiv()->GetNr(),
								pSaveData->GetDataID());
							m_dwStoredWithoutConfirm = 0;
						}
					}
				}
				else
				{
					// data could not be saved
					// may be an alarm archive and full
					// everything is handled in CArchiv
				}
				if (pSaveData && pSaveData->GetFileError())
				{
					DWORD dwFileError = LOWORD(pSaveData->GetFileError());
					WORD  wDrive      = HIWORD(pSaveData->GetFileError());
					CString sDrive;
					if (pSaveData->GetArchiv())
					{
						sDrive = pSaveData->GetArchiv()->GetFixedDrive();
					}
					if (wDrive)
					{
						sDrive.Format(_T("%c:\\"), wDrive);
					}
					if (!sDrive.IsEmpty())
					{
						if (theApp.HandleDriveError(dwFileError, sDrive))
						{
							m_bRun = FALSE;
						}
					}
				}

				WK_DELETE(pSaveData);
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
		if (theApp.m_Archives.CloseAll())
		{
			CSystemTime t;
			t.GetLocalTime();
			if (t.GetDay() != m_currentTime.GetDay())
			{
				// it's a new day
				theApp.ReloadSequenceMap(FALSE);
			}
			m_currentTime = t;
		}

		if (nCount)
		{
			COscopeView*pView = theApp.GetMainFrame()->GetOscopeView();
			if (pView)
			{
				pView->AppendValue(0, m_MBPerSecond.GetFPS(), TRUE);
				pView->AppendValue(1, m_MBPerSecond.GetMBperSecond(), TRUE);
			}
		}
		m_dwCurrentTick = GetTickCount();
		m_csQueue.Lock();
		m_nBuffer = !m_nBuffer;
		m_csQueue.Unlock();
		Sleep(5);

		if (   m_dwLastCheckandDelete == 0
			|| GetTimeSpan(m_dwLastCheckandDelete)>1000*120)
		{
			CheckAndDelete();
			m_dwLastCheckandDelete = GetTickCount();
		}
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////
void CSaveThread::CheckAndDelete(BOOL bForce /*= FALSE*/)
{
	if (   !theApp.IsResetting()
		|| bForce)
	{
			
		// let space for Alarm/PreAlarm
		DWORD dwAlarmAndPreAlarmUsedInMB = theApp.m_Archives.GetFixedSizeUsedMB();
		DWORD dwNecessaryForAlarm = 0;

		if (m_dwAlarmAndPreAlarmDefinedInMB > dwAlarmAndPreAlarmUsedInMB)
		{
			dwNecessaryForAlarm = m_dwAlarmAndPreAlarmDefinedInMB - dwAlarmAndPreAlarmUsedInMB;
		}
		dwNecessaryForAlarm += m_dwAlarmAndPreAlarmSafetyInMB;

		// check disk space
		theApp.m_Drives.Lock();
		theApp.m_Drives.CheckSpace();
		DWORD dwFreeMB = theApp.m_Drives.GetFreeMB();
		theApp.m_Drives.Unlock();

		if (theApp.TraceDelete())
		{
			WK_TRACE(_T("Check and delete %d MB free\n"),dwFreeMB);
		}

		// adjust space for Alarm/PreAlarm
		while ((dwFreeMB + 100) < dwNecessaryForAlarm)
		{
			WK_TRACE(_T("not enough space for Alarm/PreAlarm %d>%d MB, delete something\n"),dwNecessaryForAlarm,dwFreeMB);

			theApp.m_Archives.DeleteOldestDynamicSequence();

			theApp.m_Drives.Lock();
			theApp.m_Drives.CheckSpace();
			dwFreeMB = theApp.m_Drives.GetFreeMB();
			theApp.m_Drives.Unlock();
		}

		dwFreeMB = dwFreeMB - dwNecessaryForAlarm; 

		while (dwFreeMB < m_dwMustBeFreeMB)
		{
			if (theApp.TraceDelete())
			{
				WK_TRACE(_T("Check and delete %d MB free\n"),dwFreeMB);
			}
			theApp.m_Drives.Lock();
			theApp.m_Drives.CheckSpace();
			dwFreeMB = theApp.m_Drives.GetFreeMB() - dwNecessaryForAlarm;
			theApp.m_Drives.Unlock();
			theApp.m_Archives.DeleteOldestDynamicSequence();
		}
	}
}
