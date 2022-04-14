// TimerThread.cpp: implementation of the CTimerThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "sec3.h"
#include "TimerThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTimerThread::CTimerThread()
	: CWK_Thread(_T("TimerThread"))
{
	m_dwStatTime = 8*60*60*1000;
	m_iUpdateStoringFlagCounter = 0;
}
//////////////////////////////////////////////////////////////////////
CTimerThread::~CTimerThread()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CTimerThread::Run(LPVOID lpContext)
{
	// special reset delay
	BOOL bDidUpdate = FALSE;
	CTime ct = CTime::GetCurrentTime();
	
	// since a timer might change, we have to lock them, 
	// but do not try to start processes within that loop, dead lock danger
	theApp.GetTimers().Lock(_T(__FUNCTION__));

	CSecTimerArray newValidTimers;
	newValidTimers.SetAutoDelete(FALSE);

	CSecTimerArray newInvalidTimers;
	newInvalidTimers.SetAutoDelete(FALSE);
	
// GF new 03.12.2002
// The timers are updated individually at UpdateNextModeChange
	CSecTimer *pTimer=NULL;
	CTime ctNextModeChange;
	
	for (int i=0;i<theApp.GetTimers().GetSize();i++) 
	{
		pTimer = theApp.GetTimers().GetAtFast(i);

		// check if current/previous mode change is reached
		ctNextModeChange = pTimer->GetNextModeChange();
		if (   bDidUpdate // always try after update
			|| (ctNextModeChange <= ct)
			) 
		{
			if (theApp.m_bTraceTimer)
			{
				WK_TRACE(_T("Timer %s mode change reached %s\n"),
							pTimer->GetName(), ctNextModeChange.Format(_T("%a %d.%m.%Y %H:%M:%S")));
			}
			// Update timer, may be still valid (daily, mo-fr, sa-su...)
			// plus one second to get really the next mode change, not the same
			pTimer->UpdateNextModeChange(ct+CTimeSpan(0,0,0,1));
			if (theApp.m_bTraceTimer)
			{
				ctNextModeChange = pTimer->GetNextModeChange();
				WK_TRACE(_T("Timer %s next mode change %s\n"),
							pTimer->GetName(), ctNextModeChange.Format(_T("%a %d.%m.%Y %H:%M:%S")));
			}

			int iValue = 0;
			if (pTimer->IsIncluded(ct))
			{
				newValidTimers.Add(pTimer);
				iValue=1;
			}
			else
			{
				newInvalidTimers.Add(pTimer);
			}
			CString sMsg;
			sMsg.Format(_T("Timer|%s"), pTimer->GetName());
			WK_STAT_LOG(_T("Timer"), iValue, sMsg);
		}
	}
	theApp.GetTimers().Unlock();
	
	// now start processes, that is after the Lock to avoid a deadlock
	// NOT YET make sure processes are stopped, before starting new ones
	for (int t=0;t<newValidTimers.GetSize();t++) 
	{
		pTimer = newValidTimers[t];
		theApp.StartProcessesByTimer(pTimer);
	}
	
	// make sure there is no dead 'storing'
	if (++m_iUpdateStoringFlagCounter>10) 
	{
		m_iUpdateStoringFlagCounter=0;
		theApp.ModifyStoringFlag(FALSE);
	}

	if (theApp.HasAlarmOffSpanFeature())
	{
		CSystemTime st;
		st.GetLocalTime();
		theApp.GetInputGroups().RecalcAlarmOffSpans(st);
	}

	for (int t=0;t<newInvalidTimers.GetSize();t++) 
	{
		pTimer = newInvalidTimers[t];
		theApp.GetServerControl()->DisconnectTimedoutClients(pTimer->GetID());
	}
	
	Sleep(500);	// in TimerThread

	theApp.OnTimerThreadRun();

	return TRUE;
}
