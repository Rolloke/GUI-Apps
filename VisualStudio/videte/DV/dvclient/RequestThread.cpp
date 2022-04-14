// RequestThread.cpp: implementation of the CRequestThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "RequestThread.h"
#include "MainFrame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRequestThread::CRequestThread()
: CWK_Thread("ClientRequestThread",NULL)
{
	m_iTimeOut = 0;
	SetTimeOutValue(5);
}
//////////////////////////////////////////////////////////////////////
CRequestThread::~CRequestThread()
{
//	TRACE("CRequestThread delete\n");
}
//////////////////////////////////////////////////////////////////////
BOOL CRequestThread::Run(LPVOID lpContext)
{
	CMainFrame* pMF = theApp.GetMainFrame();

	XTIB::SetThreadName(_T("CRequestThread::Run"));

	if (pMF && pMF->m_hWnd)
	{
		// warte auf OnIdle Aufruf
		HANDLE hHandles[2] = {theApp.m_IdleEvent,m_StopEvent};
		DWORD dwWait = WaitForMultipleObjects(2,hHandles,FALSE,10);
		
		if (dwWait == WAIT_OBJECT_0+1)
		{
			m_StopEvent.SetEvent();
			return FALSE;
		}
		else
		{
			if (dwWait == WAIT_TIMEOUT)
			{
				m_iTimeOut++;
			}
			if (   (dwWait == WAIT_OBJECT_0)
				|| (m_iTimeOut > m_nTimeOutValue))
			{
				// bei OnIdle oder spätestens nach 10 * 20 = 200 ms
				// wieder ein Request
				if (pMF && pMF->m_hWnd)
				{
					//if live window is the bigone use a direct request to speed up
					//the live view, if database windows is the bigone use posted request
					//to avoid a deadlock
					pMF->RequestLive();	// Livewindows are directly requested
					pMF->DoRequest();	// others are requested by PostMessage
				}
				m_iTimeOut = 0;
			}
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}

bool CRequestThread::SetTimeOutValue(int nNewValue /*=-1*/)
{
	if (nNewValue == -1)
	{
		m_nTimeOutValue = 50;	// 500 ms
		return true;
	}
	else if (IsBetween(nNewValue, 1, 150)) // 10 - 1500 ms
	{
		m_nTimeOutValue = nNewValue;
		return true;
	}
	return false;
}
