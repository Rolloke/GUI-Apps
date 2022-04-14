#ifndef __WK_CRITICALSECTION_H__
#define __WK_CRITICALSECTION_H__

class CWK_Criticalsection : public CCriticalSection
{
	DECLARE_DYNAMIC(CWK_Criticalsection)
// construction / destruction
public:
	CWK_Criticalsection();
	CWK_Criticalsection(LPCTSTR sName);
	virtual ~CWK_Criticalsection();
// attributes
public:
	BOOL IsLocked();

// operations
#if(_WIN32_WINNT >= 0x0400)
	BOOL TryLock();
#endif
#if (_WIN32_WINNT >= 0x0403)
	DWORD SetSpinCount(DWORD dwSpinCount);
#endif

	virtual BOOL Lock();
	BOOL Lock(DWORD dwTimeout);
	BOOL Lock(LPCTSTR sPosition=NULL);
	BOOL Lock(LPCTSTR sFile, int nLine);
	virtual BOOL Unlock();

// debug
	static void SetTraceLocks(BOOL bTrace, DWORD dwLockTimeout);
	static void TraceLockedSections(CDWordArray*pLockedThreads=NULL);

private:
	volatile UINT m_nLockedThreadID;
	volatile UINT m_nLockCount;
	CString m_sLockedPosition;
	CString m_sObjectName;

	static CPtrList gm_CriticalSections;
	static BOOL		gm_bTraceLocks;
	static DWORD	gm_dwLockTimeout;
};

class CAutoCritSecEx
{
public:
	CAutoCritSecEx(CWK_Criticalsection*pCS, LPCTSTR sPosition)
	{
		m_pCS = pCS;
		pCS->Lock(sPosition);
	}
	CAutoCritSecEx(CCriticalSection*pCS)
	{
		m_pCS = pCS;
		m_pCS->Lock();
	}
	~CAutoCritSecEx()
	{
		if (m_pCS)
		{
			m_pCS->Unlock();
		}
	}
	void Unlock()
	{
		if (m_pCS)
		{
			m_pCS->Unlock();
		}
		m_pCS = NULL;
	}
	
private:
	CCriticalSection*m_pCS;
};

#endif // __WK_CRITICALSECTION_H__