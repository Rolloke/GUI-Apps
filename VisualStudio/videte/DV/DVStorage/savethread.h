// SaveThread.h: interface for the CSaveThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAVETHREAD_H__6275DED6_3B22_11D2_B590_00C095EC9EFA__INCLUDED_)
#define AFX_SAVETHREAD_H__6275DED6_3B22_11D2_B590_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ImageData.h"
typedef CImageData* CImageDataPtr;
WK_PTR_LIST(CImageDataPtrList, CImageDataPtr);

class CSaveThread : public CWK_Thread  
{
public:
	CSaveThread();
	virtual ~CSaveThread();

	// attributes
public:
	inline int GetNrOfDBDrives() const;
	inline CStatistics& GetStatistics();

	// operations
public:
			void Reset();
	virtual	BOOL StartThread();
	virtual	BOOL StopThread();
	virtual	BOOL Run(LPVOID lpContext);
	inline void AddData(CImageData* pImageData);

protected:
			void CheckAndDelete();
			void EmptyDataQueue();
			void Verify();
			void SortQueue(int iMax=0);
#ifdef _DEBUG
			void TraceQueue(int iMax=0);
#endif

private:
	CCriticalSection m_csQueue;
	CImageDataPtrList m_DataQueues[2];
	DWORD m_dwAlarmAndPreAlarmDefinedInMB;
	DWORD m_dwAlarmAndPreAlarmSafetyInMB;
	DWORD m_dwMustBeFreeMB;
	int	  m_iNrOfRingArchives;
	DWORD m_dwStoredWithoutConfirm;

	BOOL  m_bVerify;
	volatile int  m_nBuffer;
	volatile BOOL  m_bRun;
	WORD  m_wArchive;
	WORD  m_wSequence;
	CStatistics m_Statistics;
};
//////////////////////////////////////////////////////////////////////
inline CStatistics& CSaveThread::GetStatistics()
{
	return m_Statistics;
}
//////////////////////////////////////////////////////////////////////
inline void CSaveThread::AddData(CImageData* pImageData)
{
	CAutoCritSec acs(&m_csQueue);
	m_DataQueues[!m_nBuffer].AddTail(pImageData);
}
//////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_SAVETHREAD_H__6275DED6_3B22_11D2_B590_00C095EC9EFA__INCLUDED_)
