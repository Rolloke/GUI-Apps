// ProcessScheduler.cpp: implementation of the CProcessScheduler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "ProcessListView.h"
#include "ProcessScheduler.h"

#include "recordingprocess.h"
#include "callprocess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL CProcessScheduler::m_bTraceEncodeUnitRequests=FALSE;
BOOL CProcessScheduler::m_bTraceEncodeRequests=FALSE;
BOOL CProcessScheduler::m_bTraceInactiveByTimer=FALSE;
BOOL CProcessScheduler::m_bTraceInactiveByLock=FALSE;
BOOL CProcessScheduler::m_bTracePictDataWithoutProcess = FALSE;
BOOL CProcessScheduler::m_bTraceVirtualSeconds=FALSE;
BOOL CProcessScheduler::m_bTraceVideoout=FALSE;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CProcessScheduler::CProcessScheduler()
{
	m_bThreadActive = FALSE;
	m_pThread = NULL;
	m_dwIdleSleep = 200;
	m_dwLastGridTick=0;
	m_iDeltaOffset = 0;	// init
	m_bShutdownInProgress = FALSE;
	m_iNumStoringProcesses = 0;
	m_iNumClientProcesses = 0;

	m_bParentIsCoCo = FALSE;
	m_bParentIsMiCo = FALSE;
	m_bParentIsSaVic = FALSE;
	m_bParentIsAudio = FALSE;
	m_bParentIsTasha = FALSE;
	m_bParentIsQ = FALSE;
	m_bParentIsIP = FALSE;
}
//////////////////////////////////////////////////////////////////////
CProcessScheduler::~CProcessScheduler()
{

	TerminateAllProcesses();
}
//////////////////////////////////////////////////////////////////////
CIPC* CProcessScheduler::GetParentCIPC()
{
	return NULL;
}
/////////////////////////////////////////////////////////////////////////
void CProcessScheduler::Run()
{
	Lock(_T(__FUNCTION__));

	int iDeltaTick = GetDeltaTick();

	// calc the offset from the real second
	// to do the CalcMode for the real second
	// that is a small virtual second should be calculated for the next second.
	int iTickOffset = 1000-iDeltaTick;	
	// 1000 - 996 = 4
	// 1000 - 1010 = - 10
	if (iTickOffset<0) 
	{
		iTickOffset=0;
	}

	// loop over all processes
	for (int i=0;i<m_processes.GetSize();i++) 
	{
		CProcess *pProcess = m_processes[i];

		// calc current process state
		// UNUSED SecProcessState oldState = pProcess->GetState();
		pProcess->CalcMode(m_dwCurrentTime+iTickOffset);	// fake the next second
		SecProcessState newState = pProcess->GetState();

		if (newState==SPS_TERMINATED) 
		{
			// first set dwStopTick, once set keep terminated a certain time 
			// to collect remaining data
			if (m_dwCurrentTime - pProcess->GetTickTerminated()> 800
				|| pProcess->GetType()!=SPT_ENCODE	// NEW 060597 only for encoding processes
				) 
			{		// CAVEAT ^^^ != ^^^, so all non-encoding processes are terminated at once
				// wait half a second to collect remaining data
				int oldNum=m_processes.GetSize();
				TerminateProcess(pProcess);	// modifies m_processes!
				if (m_processes.GetSize()<oldNum) 
				{
					i--;	// adjust loop
				}
			}
		} 
		else if (   (newState==SPS_ACTIVE_RECORDING)
				 || (newState==SPS_ACTIVE_IDLE) 
				)
		{
			if (   (pProcess->GetType()==SPT_ALARM_CALL) 
				|| (pProcess->GetType()==SPT_BACKUP)
				|| (pProcess->GetType()==SPT_ACTUAL_IMAGE)
				|| (pProcess->GetType()==SPT_IMAGE_CALL)
				|| (pProcess->GetType()==SPT_CHECK_CALL)
				|| (pProcess->GetType()==SPT_GUARD_TOUR)
				|| (pProcess->GetType()==SPT_VIDEO_OUT)
				)
			{
				pProcess->Run();
			}
			else if (    pProcess->IsRecordingProcess()
				     && !pProcess->IsClientProcess())
			{
				CheckDoubleRecordingProcesses(pProcess->GetCamID(),pProcess->GetActivation()->GetArchiveID().GetSubID());
			}
		} // end  of not dropped
	}	// end of loop over m_processes

	Unlock();
}
/////////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler::IsTimeout()
{
	BOOL bTimeout = FALSE;
	m_csCurrentTime.Lock();
	bTimeout = WK_GetTimeSpan(m_dwCurrentTime) > (30 * 1000);
	m_csCurrentTime.Unlock();
	return bTimeout = FALSE;
}
//////////////////////////////////////////////////////////////////////
int CProcessScheduler::SetRunTick()
{
	m_csCurrentTime.Lock();
	m_dwCurrentTime = WK_GetTickCount();
	int iDelta = (int) (m_dwCurrentTime-m_dwLastGridTick);
	m_csCurrentTime.Unlock();
	return iDelta;
}
//////////////////////////////////////////////////////////
BOOL CProcessScheduler::PreWaitSecondGrid()
{
	int iDelta = SetRunTick();

	// cut unreasonable delta offsets
	if (m_iDeltaOffset>400) {
		m_iDeltaOffset=400;
	} else if (m_iDeltaOffset<-400) {
		m_iDeltaOffset=-400;
	}


	// as long as one second is not 'reached' keep on sleeping
	// narrowing the sleep interval while approaching one second
	if (iDelta<=500)  
	{
		m_dwIdleSleep=100;
		return TRUE;	// <<< EXIT >>>
	} 
	else 
	{
		// UNUSED int iModifiedDelta = (int)dwDelta - m_iDeltaOffset;
		if (m_processes.GetSize()==0 ) 
		{
			if (iDelta<800-m_iDeltaOffset) 
			{
				m_dwIdleSleep=100;
				return TRUE;	// <<< EXIT >>>
			} 
			else if (iDelta<975-m_iDeltaOffset) 
			{
				m_dwIdleSleep = 50;
				return TRUE;	// <<< EXIT >>>
			} 
			else 
			{
				// fall through
			}
		} 
		else 
		{
			// here dwDelta>500 and m_iDeltaOffset<=400
			if (iDelta<800-m_iDeltaOffset) 
			{
				m_dwIdleSleep = 20;
				return TRUE;	// <<< EXIT >>>
			} 
			else if (iDelta<975-m_iDeltaOffset) 
			{
				m_dwIdleSleep = 5;
				return TRUE;	// <<< EXIT >>>
			} 
			else 
			{
				// close enough to one second, fall through in the action part
				// there is still a final sleep below, right after waiting
				// for the sync
			}
		}	// end of there are some processes
	}

	return FALSE;
}
//////////////////////////////////////////////////////////
void CProcessScheduler::PostWaitSecondGrid()
{
	// >>> the final Wait/Sleep to touch ONE second
	int iWait = min(1000,1000-m_iDeltaOffset);
	while ((int)(m_dwCurrentTime-m_dwLastGridTick) < iWait) 
	{
		if (m_processes.GetSize()) 
		{
			WK_Sleep(2);	// with active processes
		} 
		else 
		{
			WK_Sleep(20); // without active processes
			Sleep(0);
		}
		m_csCurrentTime.Lock();
		m_dwCurrentTime = WK_GetTickCount();	
		m_csCurrentTime.Unlock();
	}

}
//////////////////////////////////////////////////////////
int CProcessScheduler::GetDeltaTick()
{
	m_csCurrentTime.Lock();
	int iDeltaTick = (int)(m_dwCurrentTime-m_dwLastGridTick);
	m_csCurrentTime.Unlock();

	if (m_bTraceVirtualSeconds && m_processes.GetSize()) 
	{
		CString sVirtualSecond(_T("VirtualSecond|"));
		sVirtualSecond += m_sParentShm;

		WK_STAT_LOG(_T("Process"),iDeltaTick,sVirtualSecond);
	}

	CString sDeltaMsg;
	if (m_bTraceVirtualSeconds) 
	{
		sDeltaMsg= _T("DeltaOffset|");
		sDeltaMsg += m_sParentShm;
	}

	if (iDeltaTick <1000 ) 
	{
		// under flow
		m_iDeltaOffset -= 1000-iDeltaTick;
	} 
	else if (iDeltaTick>1000 && m_processes.GetSize()) 
	{
		m_iDeltaOffset += iDeltaTick-1000;
		if (m_iDeltaOffset>400) 
		{
			m_iDeltaOffset=400;
		}
		if (m_bTraceVirtualSeconds && m_iDeltaOffset>0) 
		{
			WK_STAT_LOG(_T("Process"),m_iDeltaOffset,sDeltaMsg);
		}
	} 
	else 
	{
		if (m_iDeltaOffset>75) 
		{
			// print old value for the log and set to zero
			// WK_STAT_LOG(_T("Process"),m_iDeltaOffset,sDeltaMsg);
			if (m_bTraceVirtualSeconds) 
			{
				WK_STAT_LOG(_T("Process"),0,sDeltaMsg);
			}
		}
		if (m_processes.GetSize()==0)  
		{
			m_iDeltaOffset = 0;
		}
	}
	return iDeltaTick;
}
//////////////////////////////////////////////////////////////////////
CProcess* CProcessScheduler::GetProcess(DWORD dwID)
{
	return m_processes.GetByID(dwID);
}
//////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler::CanStartNewProcess()
{
	if (m_bShutdownInProgress) 
	{
		return FALSE;	// EXIT! 
	}

	CIPC* pParent = GetParentCIPC();

	if (   pParent->GetIPCState()!=CIPC_STATE_CONNECTED
		&& pParent!=theApp.GetOutputDummy()) 
	{
		WK_TRACE_ERROR(_T("Can't activate process while not connected to %s!\n"),
			m_sParentShm);
		return FALSE;
	}

	return TRUE;

}
//////////////////////////////////////////////////////////////////////
UINT CProcessScheduler::SchedulerThreadFunction(LPVOID pParam)
{
	CProcessScheduler* pProcessScheduler = (CProcessScheduler*)pParam;
	CString s = pProcessScheduler->m_sParentShm;
	CString sSchedulerName = pProcessScheduler->m_sParentShm + _T("Scheduler");

	XTIB::SetThreadName(sSchedulerName);

	WK_TRACE(_T("START scheduler thread %s %08lx\n"),s,GetCurrentThreadId());

	CSystemTime t;
	t.GetLocalTime();
	Sleep(1000-t.GetMilliseconds());

	while (pProcessScheduler->m_bThreadActive) 
	{
		pProcessScheduler->Run();	// modified m_dwIdleSleep
		Sleep(pProcessScheduler->m_dwIdleSleep);
	}
	WK_TRACE(_T("STOP scheduler thread %s %08lx\n"),s,GetCurrentThreadId());
	return 0;
}
/////////////////////////////////////////////////////////////////
void CProcessScheduler::StartThread()
{
	m_bThreadActive=TRUE;
	m_dwLastGridTick = WK_GetTickCount();
	m_pThread = AfxBeginThread(SchedulerThreadFunction, this);
	m_pThread->m_bAutoDelete = FALSE;
}
/////////////////////////////////////////////////////////////////
BOOL CProcessScheduler::StopThread()
{
	BOOL bRet = TRUE;
	m_bThreadActive=FALSE;
	if (m_pThread) 
	{
		DWORD dwThreadEnd = WaitForSingleObject(m_pThread->m_hThread,5000);
		if (dwThreadEnd==WAIT_TIMEOUT) 
		{
			WK_TRACE_ERROR(_T("OOPS, scheduler thread did not terminate!? %s\n"),m_sParentShm);
			TerminateThread(m_pThread->m_hThread,0xEEEE);
			bRet = FALSE; 
		} 
		else if (dwThreadEnd!=WAIT_OBJECT_0) 
		{
			WK_TRACE(_T("scheduler dwThreadEnd = %d !?\n"),dwThreadEnd);
		}
		WK_DELETE(m_pThread);
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////
void CProcessScheduler::SetShutdownInProgress()
{
	m_bShutdownInProgress = TRUE;
}
/////////////////////////////////////////////////////////////////
void CProcessScheduler::KillProcesses(CInput *pInput)
{
	Lock(_T(__FUNCTION__));
	for (int i=m_processes.GetSize()-1;i>=0;i--) 
	{
		CProcess *pProcess = m_processes[i];
		if (   pProcess->GetAlarmInput()==pInput 
			|| pInput==NULL) 
		{
			StopProcess(pProcess);
			// no need to adjust loop, terminate does the real remove
		}
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
CProcess* CProcessScheduler::StopProcess(CProcess* pProcess)
{
	CProcess* pReturn = NULL;

	Lock(_T(__FUNCTION__));
	if (TerminateProcess(pProcess,FALSE))
	{
		theApp.ModifyStoringFlag(FALSE);
	}
	else
	{
		pReturn = pProcess;
	}

	Unlock();

	return pReturn;
}
/////////////////////////////////////////////////////////////////////////
void CProcessScheduler::KillProcessByTimer(const CSecTimer *pTimer)
{
	if (pTimer==NULL) {
		WK_TRACE_ERROR(_T("NULL timer for KillProcessByTimer\n"));
		return;
	}
	
	Lock(_T(__FUNCTION__));
	for (int i=m_processes.GetSize()-1;i>=0;i--) 
	{
		CProcess *pProcess = m_processes[i];
		if (pProcess->GetTimer()==pTimer) 
		{
			// OOPS Stop and Kill ?
			if (m_bTraceInactiveByLock) 
			{
				CString sMsg;
				sMsg.Format(_T("InactiveByLock"));	// NOT YET input timer name
				WK_STAT_PEAK(_T("Process"),2,sMsg);
			}
			TerminateProcess(pProcess);
		}
	}
	Unlock();
}
///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler::TerminateRelayProcess(CProcess *pProcess, BOOL bShutDown/*=FALSE*/)
{
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler::TerminateProcess(CProcess *pProcess, BOOL bShutDown)
{
	if (pProcess==NULL) 
	{
		return FALSE;	// EXIT
	}

	BOOL bRet = FALSE;

	Lock(_T(__FUNCTION__));

	pProcess->Lock(_T(__FUNCTION__));

	BOOL bIsNoRelayOrTerminated = TRUE;;
	if (pProcess->IsRelayProcess()) 
	{
		// relay prozesse werden extra behandelt
		bIsNoRelayOrTerminated = TerminateRelayProcess(pProcess,bShutDown);
	}

	// If it is relay and not terminated, do not remove process!
	if (bIsNoRelayOrTerminated)
	{
		// die aktuellen Zähler korrigieren
		if (pProcess->IsClientProcess()) 
		{
			m_iNumClientProcesses--;
			if (m_iNumClientProcesses<0) 
			{
				WK_TRACE_ERROR(_T("internal error, m_iNumClientProcesses\n"));
				m_iNumClientProcesses=0;
			}
		} 
		else 
		{
			if (pProcess->GetMacro().GetType()==PMT_RECORD) 
			{
				m_iNumStoringProcesses--;
				if (m_iNumStoringProcesses<0) 
				{
					WK_TRACE_ERROR(_T("internal error, m_iNumStoringProcesses\n"));
					m_iNumStoringProcesses=0;
				}
				CheckDoubleRecordingProcesses(pProcess->GetCamID(),pProcess->GetActivation()->GetArchiveID().GetSubID());
			}
		}

		// remove in the 'container' and delete
		if (m_processes.Index(pProcess)!=-1) 
		{
			if (pProcess->Terminate(bShutDown))
			{
				// remove reference in priority arrays
				for (int i=0;i<NUM_PRIORITIES;i++) 
				{
					m_priorityProcesses[i].Remove(pProcess);	// nop if not found
				}
				pProcess->Unlock();
				m_processes.Remove(pProcess);
				WK_DELETE(pProcess);
				bRet = TRUE;
			}
			else
			{
				pProcess->Unlock();
			}
		} 
		else 
		{
			pProcess->Unlock();
			WK_TRACE_ERROR(_T("internal error, could not find process (delete)\n"));
		}
	}
	else
	{
		pProcess->Unlock();
	}

	Unlock();

	TraceProcesses();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////////
void CProcessScheduler::TraceProcesses(BOOL bForce/*=FALSE*/)
{
	if (   CProcess::m_bTraceProcesses
		|| bForce)
	{
		Lock(_T(__FUNCTION__));
		
		for (int i=0;i<m_processes.GetSize();i++)
		{
			WK_TRACE(_T("%d,%08lx <%s>\n"),i,m_processes[i]->GetID().GetID(),
				m_processes[i]->GetDescription(TRUE));
			WK_TRACE(_T("%d,%08lx <%s>\n"),i,m_processes[i]->GetID().GetID(),
				m_processes[i]->Format());
		}
		
		Unlock();
	}
}
/////////////////////////////////////////////////////////////////////////////////
void CProcessScheduler::TraceRequests()
{

}
/////////////////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler::CheckProcessForTimer(const CActivation& activation)
{
	BOOL bTimerOkay=TRUE;
	const CSecTimer* pSecTimer = theApp.GetTimers().GetTimerByID(activation.m_idTimer);


	if (pSecTimer) 
	{
		theApp.GetTimers().Lock(_T(__FUNCTION__));
		// time check for all
		if (pSecTimer->IsIncluded(CTime::GetCurrentTime())) 
		{
			bTimerOkay=TRUE;
		} 
		else 
		{
			bTimerOkay=FALSE;
			if (m_bTraceInactiveByTimer) 
			{
				CString sMsg;
				sMsg.Format(_T("InactiveByTimer|%s|%s"),pSecTimer->GetName(),activation.GetMacro().GetName());
				WK_STAT_PEAK(_T("Process"),1,sMsg);
			}
		}
		if (pSecTimer->GetLockIDs().GetSize()>0) 
		{
			// has lock[s]
			const CDWordArray &lockIDs = pSecTimer->GetLockIDs();
			for (int lx=0;lx<lockIDs.GetSize() && bTimerOkay;lx++) 
			{
				const CInput *pInput;
				pInput = theApp.GetInputGroups().GetInputByID(lockIDs[lx]);
				if (pInput) 
				{
					// now check the state of the input
					if (pInput->IsActive()==FALSE) 
					{
						// OK means not active
						bTimerOkay=FALSE;
						if (m_bTraceInactiveByLock) 
						{
							CString sMsg;
							sMsg.Format(_T("InactiveByLock"));	// NOT YET input timer name
							WK_STAT_PEAK(_T("Process"),1,sMsg);
						}
					} 
					else 
					{
						// ALARM state not blocked
					}
				} 
				else 
				{
					WK_TRACE(_T("Input %x not found !?\n"),lockIDs[lx]);
					bTimerOkay=FALSE;
					if (m_bTraceInactiveByLock) 
					{
						CString sMsg;
						sMsg.Format(_T("InactiveByLock"));	// NOT YET input timer name
						WK_STAT_PEAK(_T("Process"),3,sMsg);
					}
				}
			}
		}
		theApp.GetTimers().Unlock();
	} 
	else 
	{
		// has no timer at all
	}

	return bTimerOkay;
}
/*@MD Checks time and Lock[s]*/
/*
BOOL CProcessScheduler::CheckProcessForTimer(const CProcess *pProcess)
{
	BOOL bTimerOkay=TRUE;
	if (pProcess->GetTimer()) 
	{
		theApp.GetTimers().Lock(_T(__FUNCTION__));
		const CSecTimer &timer = *(pProcess->GetTimer());
		// time check for all
		if (timer.IsIncluded(CTime::GetCurrentTime())) {
			bTimerOkay=TRUE;
		} else {
			bTimerOkay=FALSE;
			if (m_bTraceInactiveByTimer) {
				CString sMsg;
				sMsg.Format(_T("InactiveByTimer|%s|%s"),
					timer.GetName(),
					pProcess->GetMacro().GetName());
				WK_STAT_PEAK(_T("Process"),1,sMsg);
			}
		}
		if (timer.GetLockIDs().GetSize()==0) {
			// no lock, plain time check
		} else {
			// has lock[s]
			const CDWordArray &lockIDs = timer.GetLockIDs();
			for (int lx=0;lx<lockIDs.GetSize() && bTimerOkay;lx++) 
			{
				const CInput *pInput;
				pInput = theApp.GetInputGroups().GetInputByID(lockIDs[lx]);
				if (pInput) {
					// now check the state of the input
					if (pInput->IsActive()==FALSE) {
						// OK means not active
						bTimerOkay=FALSE;
						if (m_bTraceInactiveByLock) {
							CString sMsg;
							sMsg.Format(_T("InactiveByLock"));	// NOT YET input timer name
							WK_STAT_PEAK(_T("Process"),1,sMsg);
						}
					} else {
						// ALARM state not blocked
					}
				} else {
					WK_TRACE(_T("Input %x not found !?\n"),lockIDs[lx]);
					bTimerOkay=FALSE;
					if (m_bTraceInactiveByLock) {
						CString sMsg;
						sMsg.Format(_T("InactiveByLock"));	// NOT YET input timer name
						WK_STAT_PEAK(_T("Process"),3,sMsg);
					}
				}
			}
		}
		theApp.GetTimers().Unlock();
	} else {
		// has no timer at all
	}

	return bTimerOkay;
}*/
/////////////////////////////////////////////////////////////////////////////////////////////////
void CProcessScheduler::AddProcess(CProcess* pProcess)
{
	Lock(_T(__FUNCTION__));
	// add to 'container' and the according priority array
	m_processes.Add(pProcess);
	ASSERT(pProcess->GetActivation()->GetPriority()<NUM_PRIORITIES);
	m_priorityProcesses[pProcess->GetActivation()->GetPriority()].Add(pProcess);
	UpdateProcessListView(TRUE,pProcess);
	Unlock();

	TraceProcesses();
}
/////////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler::IsRunning()
{
	return m_pThread != NULL;
}
/////////////////////////////////////////////////////////////////////////
void CProcessScheduler::TerminateAllProcesses()
{
	Lock(_T(__FUNCTION__));
	while (m_processes.GetSize()) 
	{
		TerminateProcess(m_processes[0],TRUE);
	}
	Unlock();
}
/////////////////////////////////////////////////////////////////////////
void CProcessScheduler::OnIndicateVideo(const CIPCPictureRecord& pict,
							 DWORD dwMDX,
							 DWORD dwMDY,
							 DWORD dwUserData)
{
}
/////////////////////////////////////////////////////////////////////////
void CProcessScheduler::AnswerRequest(const CIPCPictureRecord &pictRect,
									  DWORD dwMDX,
									  DWORD dwMDY,
									  DWORD pid,
									  CRequestCollector* pRequest)
{
}
/////////////////////////////////////////////////////////////////////////
void CProcessScheduler::OnUnitConfirmReset()
{
}
/////////////////////////////////////////////////////////////////////////
void CProcessScheduler::OnUnitDisconnect()
{
}
/////////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler::IsImageTimeout()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler::StartNewProcess(CInput *pInput, 
								  CActivation &record,
								  BOOL & bRefThereWasAProcess,
								  CProcess*& pProcess,
								  int iNumAlreadyRecorded /*= -1*/)
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////
void CProcessScheduler::SaveAlarm(CInput* pInput, WORD wArchiveNr, CSecID idOutput, CSecID idArchive)
{
	CIPCDatabaseClientServer* pDatabase = theApp.GetDatabase();
	if (pDatabase)
	{
		CSystemTime st;
		st.GetLocalTime();

		CIPCFields fields;
		CString s;
		CSecID idInput = pInput->GetID();

		fields.Add(new CIPCField(_T("DBP_TYP"),pInput->IsMD() ? _T('A'):_T('1'),_T('C')));
		fields.Add(new CIPCField(_T("DBP_TIME"),st.GetDBTime(),_T('C')));
		fields.Add(new CIPCField(_T("DBP_DATE"),st.GetDBDate(),_T('C')));
		s.Format(_T("%03d"),st.GetMilliseconds());
		fields.Add(new CIPCField(_T("DBP_MS"),s,_T('C')));

		if (idOutput!=SECID_NO_ID)
		{
			s.Format(_T("%08lx"),idOutput.GetID());
			fields.Add(new CIPCField(_T("DBP_CANR"),s,_T('C')));
		}

		if (idArchive!=SECID_NO_ID)
		{
			s.Format(_T("%08lx"),idArchive.GetID());
			fields.Add(new CIPCField(_T("DBP_ARCNR"),s,_T('C')));
		}

		if (idInput!=SECID_NO_ID)
		{
			s.Format(_T("%08lx"),idInput.GetID());
			fields.Add(new CIPCField(_T("DBP_INPNR"),s,_T('C')));
		}

		if (!pInput->IsMD())
		{
			pInput->CopyFields(fields);
		}


		theApp.SaveAlarmData(fields,wArchiveNr);
	}
}
////////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler::CheckDoubleRecordingProcesses(CSecID idCam, WORD wArchive)
{
	BOOL bRet = FALSE;

	Lock(_T(__FUNCTION__));

	if (m_iNumStoringProcesses>1)
	{
		// at least two processes

		// first look for all processes with the same camera and archive
		CProcesses recording_processes;
		CRecordingProcess* pProcessMax = NULL;
		DWORD iMax = 0;

		for (int i=0;i<m_processes.GetSize();i++)
		{
			CProcess* pProcess = m_processes.GetAtFast(i);
			if (   pProcess
				&& pProcess->IsRecordingProcess()
				&& !pProcess->IsClientEncodingProcess()
				&& !pProcess->IsMDProcess()
				&& pProcess->IsActiveRecordingProcess())
			{
				if (   pProcess->GetCamID() == idCam
					&& pProcess->GetActivation()->GetArchiveID().GetSubID() == wArchive)
				{
					recording_processes.Add(pProcess);
					if (pProcess->GetMacro().GetPicsPerSlice()>iMax)
					{
						iMax = pProcess->GetMacro().GetPicsPerSlice();
						pProcessMax = (CRecordingProcess*)pProcess;
					}
				}
			}
		}

		if (pProcessMax)
		{
			pProcessMax->SetDoStoring(TRUE);
			pProcessMax->CleanNotStoringProcesses();
		}

		if (recording_processes.GetSize()>1)
		{
			// more than one
			// only the max fps
			for (int i=0;i<recording_processes.GetSize();i++)
			{
				CProcess* pProcess = recording_processes.GetAtFast(i);
				if (   pProcess
					&& !pProcess->IsMDProcess()
					&& !pProcess->IsClientEncodingProcess()
					&& pProcess->IsRecordingProcess()
					)
				{
					CRecordingProcess* pRecordingProcess = (CRecordingProcess*)pProcess;
					if (pRecordingProcess != pProcessMax)
					{
						pRecordingProcess->SetDoStoring(FALSE);
						pProcessMax->AddNotStoringProcess(pRecordingProcess);
					}
				}
			}
		}
	}

	Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////
Compression CProcessScheduler::GetDefaultCompression(CSecID camID)
{
	Compression defaultCompression = COMPRESSION_NONE;

	if (  !ParentIsQ()
		&&!ParentIsSaVic())
	{
		m_processes.Lock(_T(__FUNCTION__));
		for (int i=0;i<m_processes.GetSize();i++)
		{
			CProcess* pProcess = m_processes.GetAt(i);
			if (   pProcess->IsRecordingProcess()
				&& pProcess->GetActivation()->GetOutputID() == camID)
			{
				if (defaultCompression == COMPRESSION_NONE)
				{
					defaultCompression = pProcess->GetMacro().GetCompression();
				}
				else
				{
					if (defaultCompression != pProcess->GetMacro().GetCompression())
					{
						defaultCompression = COMPRESSION_12;
						break;
					}
				}
			}
		}
		if (defaultCompression == COMPRESSION_NONE)
		{
			defaultCompression = COMPRESSION_12;
		}
		m_processes.Unlock();
	}

	return defaultCompression;
}
//////////////////////////////////////////////////////////////////////////////////////////
Resolution CProcessScheduler::GetDefaultResolution(CSecID camID)
{
	m_processes.Lock(_T(__FUNCTION__));
	Resolution defaultResolution = RESOLUTION_NONE;

	for (int i=0;i<m_processes.GetSize();i++)
	{
		CProcess* pProcess = m_processes.GetAt(i);
		if (   pProcess->IsRecordingProcess()
			//&& pProcess->GetActivation()->GetOutputID() == camID
			)
		{
			if (defaultResolution == RESOLUTION_NONE)
			{
				defaultResolution = pProcess->GetMacro().GetResolution();
			}
			else
			{
				if (defaultResolution != pProcess->GetMacro().GetResolution())
				{
					defaultResolution = RESOLUTION_2CIF;
					break;
				}
			}
		}
	}
	if (defaultResolution == RESOLUTION_NONE)
	{
		defaultResolution = RESOLUTION_2CIF;
	}

	m_processes.Unlock();

	return defaultResolution;
}
//////////////////////////////////////////////////////////////////////////
void CProcessScheduler::TerminateCallProcesses(CIPCOutputServerClient* pCIPCOutputServerClient)
{
	for (int i=0;i<m_processes.GetSize();i++)
	{
		CProcess* pProcess = m_processes.GetAt(i);
		if (   pProcess->IsCallProcess()
			&& pProcess->GetState() != SPS_TERMINATED)
		{
			CCallProcess* pCallProcess = (CCallProcess*)pProcess;
			if (pCallProcess->GetOutputClient() == pCIPCOutputServerClient)
			{
				TerminateProcess(pCallProcess);
				break;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CProcessScheduler::FindCallProcess(CIPCOutputServerClient* pCIPCOutputServerClient, CCallProcess* pCallCheck)
{
	BOOL bFound = FALSE;
	for (int i=0;i<m_processes.GetSize() && !bFound;i++)
	{
		CProcess* pProcess = m_processes.GetAt(i);
		if (   pProcess->IsCallProcess()
			&& pProcess->GetState() != SPS_TERMINATED
			&& pProcess != pCallCheck)
		{
			CCallProcess* pCallProcess = (CCallProcess*)pProcess;
			bFound = pCallProcess->GetOutputClient() == pCIPCOutputServerClient;
		}
	}
	return bFound;
}

