// TimerThread.h: interface for the CTimerThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMERTHREAD_H__F99089E3_B3C1_11D4_9950_004005A19028__INCLUDED_)
#define AFX_TIMERTHREAD_H__F99089E3_B3C1_11D4_9950_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTimerThread : public CWK_Thread  
{
public:
	CTimerThread();
	virtual ~CTimerThread();

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);

private:
	CTime m_lastTimerUpdate;
	int   m_iUpdateStoringFlagCounter;
};

#endif // !defined(AFX_TIMERTHREAD_H__F99089E3_B3C1_11D4_9950_004005A19028__INCLUDED_)
