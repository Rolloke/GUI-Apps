// SaveThread.h: interface for the CSaveThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAVETHREAD_H__6275DED6_3B22_11D2_B590_00C095EC9EFA__INCLUDED_)
#define AFX_SAVETHREAD_H__6275DED6_3B22_11D2_B590_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SaveData.h"
WK_PTR_LIST(CSaveDataPtrList, CSaveDataPtr);

class CSaveThread : public CWK_Thread  
{
public:
	CSaveThread();
	virtual ~CSaveThread();

	// attributes
public:
	CString		 Format();
	inline DWORD GetRunTick();
	CStatistics& GetStatistics();

	// operations
public:
	inline  void AddSaveData(CSaveData* pSaveData);
	virtual	BOOL StartThread();
	virtual	BOOL StopThread();
	virtual	BOOL Run(LPVOID lpContext);
		    void Reset();

protected:
			void CheckAndDelete(BOOL bForce = FALSE);
			void EmptyDataQueue();
#ifdef _DEBUG
			void TraceQueue(int iMax=0);
#endif

private:
	CCriticalSection m_csQueue;
	CSaveDataPtrList m_DataQueues[2];
	DWORD			m_dwLastTrace;
	CStatistics		m_MBPerSecond;
	DWORD			m_dwCurrentTick;
	CSystemTime     m_currentTime;
	volatile int  m_nBuffer;
	volatile BOOL  m_bRun;
	
	DWORD m_dwStoredWithoutConfirm;
	DWORD m_dwAlarmAndPreAlarmDefinedInMB;
	DWORD m_dwAlarmAndPreAlarmSafetyInMB;
	DWORD m_dwMustBeFreeMB;

	DWORD m_dwLastCheckandDelete;
};
///////////////////////////////////////////////////
inline DWORD CSaveThread::GetRunTick()
{
	return m_dwCurrentTick;
}
//////////////////////////////////////////////////////////////////////
inline void CSaveThread::AddSaveData(CSaveData* pSaveData)
{
	CAutoCritSec acs(&m_csQueue);
	m_DataQueues[!m_nBuffer].AddTail(pSaveData);
}

#endif // !defined(AFX_SAVETHREAD_H__6275DED6_3B22_11D2_B590_00C095EC9EFA__INCLUDED_)
