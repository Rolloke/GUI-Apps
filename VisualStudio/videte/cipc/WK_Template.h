/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: WK_Template.h $
// ARCHIVE:		$Archive: /Project/CIPC/WK_Template.h $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 56 $
// LAST CHANGE:	$Modtime: 19.01.06 16:07 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _WK_TEMPLATE_H
#define _WK_TEMPLATE_H

#include <math.h>

#ifdef _DEBUG
#if _MSC_VER >= 1300 
#define TRACE_LOCKS 1
#endif
#endif

#ifdef TRACE_LOCKS
 #include "wkclasses\WK_Utilities.h"
 #include "wkclasses\wk_Trace.h"
#endif

// HEDU 040396
// HEDU 040497 m_bAutoDelete added

// erzeuge eine type-safe subclass from CPtrArray mit
// neuem Namen CNAME, Elemente haben den Type TYPE
// z.B.: WK_PTR_ARRAY(CUserPtrArray,PUSER);
// NOTE: TYPE darf nicht von der Form 'MyType *' sein, da alles ein macro ist
// NOTE: am besten typedef MyType *MyTypePtr;

// direct subclass from templated-class
#define WK_PTR_ARRAY(CNAME,TYPE) class CNAME : public CTypedPtrArray<CPtrArray,TYPE> { \
  public: \
	CNAME() { m_bAutoDelete=FALSE; } \
	virtual ~CNAME() { \
		if (m_bAutoDelete) { DeleteAll(); } \
	} \
	void DeleteAll() { \
		for (int i=0;i<GetSize();i++) { \
			if (GetAtFast(i)) { \
				delete (TYPE) GetAtFast(i); \
			} \
		} \
		RemoveAll(); \
	}\
	int Index(const TYPE p) { \
		for (int i=0;i<GetSize();i++) { \
			if (GetAtFast(i)==p) { \
				return (i); \
			} \
		} \
		return -1; \
	}\
	void Remove(const TYPE p) {\
		for (int i=0;i<GetSize();i++) { \
			if (GetAtFast(i)==p) { \
				RemoveAt(i); \
				return; \
			} \
		} \
	}\
	void Delete(const TYPE p) {\
		for (int i=0;i<GetSize();i++) { \
			if (GetAtFast(i)==p) { \
				delete (TYPE) GetAtFast(i); \
				RemoveAt(i); \
				return; \
			} \
		} \
	} \
	void SetAutoDelete(BOOL bAutoDelete) { \
		m_bAutoDelete = bAutoDelete; \
	} \
	TYPE GetAtFast(int nPos) const { \
		ASSERT(nPos >= 0 && nPos < m_nSize); \
		return (TYPE) m_pData[nPos]; \
	} \
protected: \
	BOOL	m_bAutoDelete;\
};
#ifdef TRACE_LOCKS
#define WK_PTR_ARRAY_CS(CNAME,TYPE,CNAME_CS) \
	WK_PTR_ARRAY(CNAME,TYPE) \
	class CNAME_CS : public CNAME { \
		public: \
			CNAME_CS() { \
				m_nCurrentThread = (UINT)-1; \
				m_hCurrentThread = NULL; \
				m_nLockCount = 0; } \
			virtual ~CNAME_CS() { \
				if (m_bAutoDelete) { SafeDeleteAll(); } \
			} \
			void Lock(LPCTSTR pszFkt = NULL) { \
				if (m_nCurrentThread != (UINT)-1 && m_nCurrentThread != 0 && m_nCurrentThread != GetCurrentThreadId()) { \
				WK_TRACE(_T("%s Locked by (%x):%s, entered by %s in %s\n"), _T(#CNAME_CS), m_nCurrentThread, m_sCurrentThread, XTIB::GetThreadName(), pszFkt ? pszFkt : _T("?")); } \
				m_cs.Lock(); \
				if (0 == m_nLockCount++) { \
				m_sCurrentThread = XTIB::GetThreadName(); \
				if (pszFkt) m_sCurrentThread = m_sCurrentThread + _T(" in ") + pszFkt; \
				if (m_nCurrentThread != (UINT)-1) m_nCurrentThread = GetCurrentThreadId(); \
				m_hCurrentThread = GetCurrentThread(); } } \
			void Unlock() { \
			 if (0 == --m_nLockCount && m_nCurrentThread != (UINT)-1) { \
				m_nCurrentThread = 0; \
				m_hCurrentThread = NULL; \
				m_sCurrentThread.Empty(); } \
				m_cs.Unlock(); } \
			void SafeAdd(TYPE p) { Lock(_T(__FUNCTION__)); Add(p); Unlock(); } \
			void SafeRemove(TYPE p) { Lock(_T(__FUNCTION__)); Remove(p); Unlock(); } \
			void SafeDelete(TYPE p) { Lock(_T(__FUNCTION__)); Delete(p); Unlock(); } \
			void SafeRemoveAll() { Lock(_T(__FUNCTION__)); RemoveAll(); Unlock(); } \
			void SafeDeleteAll() { Lock(_T(__FUNCTION__)); DeleteAll(); Unlock(); } \
		public: \
			CCriticalSection m_cs; \
			CString m_sCurrentThread; \
			UINT	m_nCurrentThread; \
			HANDLE	m_hCurrentThread; \
			UINT	m_nLockCount; \
	};
#else
#define WK_PTR_ARRAY_CS(CNAME,TYPE,CNAME_CS) \
	WK_PTR_ARRAY(CNAME,TYPE) \
	class CNAME_CS : public CNAME { \
		public: \
			virtual ~CNAME_CS() { \
				if (m_bAutoDelete) { SafeDeleteAll(); } \
			} \
			void Lock(LPCTSTR sFkt=NULL) { m_cs.Lock(); sFkt=NULL;} \
			void Unlock() { m_cs.Unlock(); } \
			void SafeAdd(TYPE p) { Lock(); Add(p); Unlock(); } \
			void SafeRemove(TYPE p) { Lock(); Remove(p); Unlock(); } \
			void SafeDelete(TYPE p) { Lock(); Delete(p); Unlock(); } \
			void SafeRemoveAll() { Lock(); RemoveAll(); Unlock(); } \
			void SafeDeleteAll() { Lock(); DeleteAll(); Unlock(); } \
		public: \
			CCriticalSection m_cs; \
	};
#endif

#define WK_PTR_LIST(CNAME, TYPE) class CNAME : public CTypedPtrList<CPtrList,TYPE> { \
	public: \
		CNAME() { m_bAutoDelete=FALSE; } \
		virtual ~CNAME() { \
			if (m_bAutoDelete) { DeleteAll(); } \
		} \
		void DeleteAll() { \
			while (IsEmpty()==FALSE) { \
				delete (TYPE) GetTail(); \
				RemoveTail(); \
			} \
		} \
		void SetAutoDelete(BOOL bAutoDelete) { \
				m_bAutoDelete = bAutoDelete; \
			} \
protected: \
	BOOL	m_bAutoDelete;\
};
#ifdef TRACE_LOCKS
#define WK_PTR_LIST_CS(CNAME,TYPE,CNAME_CS) \
	WK_PTR_LIST(CNAME,TYPE) \
	class CNAME_CS : public CNAME { \
		public: \
			CNAME_CS() { \
				m_nCurrentThread = (UINT)-1; \
				m_hCurrentThread = NULL; \
				m_nLockCount = 0; } \
			virtual ~CNAME_CS() { \
				if (m_bAutoDelete) { SafeDeleteAll(); } \
			} \
			void Lock(LPCTSTR pszFkt = NULL) { \
				if (m_nCurrentThread != (UINT)-1 && m_nCurrentThread != 0 && m_nCurrentThread != GetCurrentThreadId()) { \
				WK_TRACE(_T("%s Locked by (%x):%s, entered by %s in %s\n"), _T(#CNAME_CS), m_nCurrentThread, m_sCurrentThread, XTIB::GetThreadName(), pszFkt ? pszFkt : _T("?")); } \
				m_cs.Lock(); \
				if (0 == m_nLockCount++) { \
				m_sCurrentThread = XTIB::GetThreadName(); \
				if (pszFkt) m_sCurrentThread = m_sCurrentThread + _T(" in ") + pszFkt; \
				if (m_nCurrentThread != (UINT)-1) m_nCurrentThread = GetCurrentThreadId(); \
				m_hCurrentThread = GetCurrentThread(); } } \
			void Unlock() { \
				if (0 == --m_nLockCount && m_nCurrentThread != (UINT)-1) { \
				m_nCurrentThread = 0; \
				m_hCurrentThread = NULL; \
				m_sCurrentThread.Empty(); } \
				m_cs.Unlock(); } \
			void SafeDeleteAll() { Lock(_T(__FUNCTION__)); DeleteAll(); Unlock(); } \
			void SafeAddTail(TYPE p) { \
				Lock(_T(__FUNCTION__)); AddTail(p); Unlock(); \
			} \
			TYPE SafeGetHead() { \
				TYPE p; \
				Lock(_T(__FUNCTION__)); \
				if (IsEmpty()) p=NULL; \
				else { \
					p=GetHead();\
				} \
				Unlock(); \
				return p; \
			} \
			TYPE SafeGetAndRemoveHead() { \
				TYPE p; \
				Lock(_T(__FUNCTION__)); \
				if (IsEmpty()) p=NULL; \
				else { \
					p=GetHead(); RemoveHead(); \
				} \
				Unlock(); \
				return p; \
			} \
	public: \
		CCriticalSection m_cs; \
		CString m_sCurrentThread; \
		UINT	m_nCurrentThread; \
		HANDLE	m_hCurrentThread; \
		UINT	m_nLockCount; \
	};
#else
#define WK_PTR_LIST_CS(CNAME,TYPE,CNAME_CS) \
	WK_PTR_LIST(CNAME,TYPE) \
	class CNAME_CS : public CNAME { \
		public: \
			virtual ~CNAME_CS() { \
				if (m_bAutoDelete) { SafeDeleteAll(); } \
			} \
			void Lock(LPCTSTR sFkt=NULL) { m_cs.Lock(); sFkt=NULL;} \
			void Unlock() { m_cs.Unlock(); } \
			void SafeDeleteAll() { Lock(); DeleteAll(); Unlock(); } \
			void SafeAddTail(TYPE p) { \
				Lock(); AddTail(p); Unlock(); \
			} \
			TYPE SafeGetHead() { \
				TYPE p; \
				Lock(); \
				if (IsEmpty()) p=NULL; \
				else { \
					p=GetHead();\
				} \
				Unlock(); \
				return p; \
			} \
			TYPE SafeGetAndRemoveHead() { \
				TYPE p; \
				Lock(); \
				if (IsEmpty()) p=NULL; \
				else { \
					p=GetHead(); RemoveHead(); \
				} \
				Unlock(); \
				return p; \
			} \
	public: \
		CCriticalSection m_cs; \
	};
#endif
#define WK_PTR_LIST_SEMA(CNAME,TYPE,CNAME_CS) \
	WK_PTR_LIST(CNAME,TYPE) \
	class CNAME_CS : public CNAME { \
		public: \
			CNAME_CS() \
			{ \
				CString sSemaName; \
				sSemaName.Format(_T("QueueSema%x%x"),(DWORD)this,GetTickCount()); \
				m_hSemaphore = CreateSemaphore( NULL, 0, 5000, sSemaName); \
				if (m_hSemaphore==NULL) { \
					TRACE(_T("failed to create m_hSemaphore\n")); \
				} \
			} \
			virtual ~CNAME_CS() { \
				if (m_bAutoDelete) { SafeDeleteAll(); } \
				if (m_hSemaphore) { CloseHandle(m_hSemaphore); m_hSemaphore = NULL; } \
			} \
			void IncreaseSemaphore() \
			{ \
				if (m_hSemaphore) { \
					ReleaseSemaphore(m_hSemaphore, 1, NULL); \
				} \
			} \
			void Lock() { m_cs.Lock(); } \
			void Unlock() { m_cs.Unlock(); } \
			void SafeDeleteAll() { Lock(); DeleteAll(); Unlock(); } \
			void SafeAddTail(TYPE p) { \
				Lock(); AddTail(p); IncreaseSemaphore(); Unlock(); \
			} \
			TYPE SafeGetHead() { \
				TYPE p; \
				Lock(); \
				if (IsEmpty()) p=NULL; \
				else { \
					p=GetHead(); \
				} \
				Unlock(); \
				return p; \
			} \
			TYPE SafeGetAndRemoveHead() { \
				TYPE p; \
				Lock(); \
				if (IsEmpty()) p=NULL; \
				else { \
					p=GetHead(); RemoveHead(); \
				} \
				Unlock(); \
				return p; \
			} \
		HANDLE GetSemaphore() const { return m_hSemaphore; } \
private: \
	HANDLE m_hSemaphore;\
	CCriticalSection m_cs; \
};

template <class T> void swap(T&Val1, T&Val2)
{
	T tempVal = Val1;
	Val1 = Val2;
	Val2 = tempVal;
}

#define ARBITRARY_AVG_VAL -1

template <class TavgVal>

class CWK_Average
{
public:
	//////////////////////////////////////////////////////////////////////////
	// Konstruction / Destruction
/*********************************************************************************************
 Class      : CWK_Average
 Function   : CWK_Average()
 Description: Creates an array to store values of arbitrary types.
              Sample: CWK_Average<double> ArrayWithFloatingPointValues

 Parameters: None

 Result type:  ()
 created: June, 16 2005
 <TITLE CWK_Average>
*********************************************************************************************/
	CWK_Average()
	{
		m_nNoOfAvgValues = ARBITRARY_AVG_VAL;
		m_nCurrentValue	 = 0;
		m_nCurrenSize	 = 0;
		m_AvgSum		 = (TavgVal)0;
		m_pAvgValues     = NULL;
	};
	//////////////////////////////////////////////////////////////////////////
	~CWK_Average()
	{
		RemoveAll();
	};

	//////////////////////////////////////////////////////////////////////////
	// Implementation
/*********************************************************************************************
 Class      : CWK_Average
 Function   : void RemoveAll()
 Description: Frees all Data

 Parameters: None

 Result type:  (void)
 created: June, 16 2005
 <TITLE RemoveAll>
*********************************************************************************************/
	void RemoveAll()
	{
		if (m_pAvgValues)
		{
			free(m_pAvgValues);
		}
		m_nCurrenSize   = 0;
		m_nCurrentValue = 0;
		m_pAvgValues    = NULL;
		m_AvgSum		 = (TavgVal)0;
	};

/*********************************************************************************************
 Class      : CWK_Average
 Function   : void AddValue(TavgVal val)
 Description: Adds a single value to the array

 Parameters:
  val: (E): Value to add  (TavgVal)

 Result type:  (void)
 created: June, 16 2005
 <TITLE AddValue>
*********************************************************************************************/
	void AddValue(TavgVal val)
	{
		m_AvgSum  = (TavgVal)(m_AvgSum + val);		// add the current value
		if (m_nNoOfAvgValues == ARBITRARY_AVG_VAL)
		{											// if the size is arbitrary
			m_nCurrenSize++;						// allocate the desired memory just in time
			int nSize = sizeof(TavgVal)*m_nCurrenSize;
			if (m_pAvgValues == NULL)
			{
				m_pAvgValues = (TavgVal*) malloc(nSize);
			}
			else
			{
				m_pAvgValues = (TavgVal*) realloc(m_pAvgValues, nSize);
			}
			if (m_pAvgValues)
			{
				m_pAvgValues[m_nCurrenSize-1] = val;// insert the current value
			}
		}
		else
		{
			if (m_pAvgValues == NULL)
			{
				int nSize = sizeof(TavgVal)*m_nNoOfAvgValues;
				m_pAvgValues = (TavgVal*) malloc(nSize);
			}
			if (m_nCurrenSize < m_nNoOfAvgValues)	// not full
			{
				m_pAvgValues[m_nCurrentValue] = val;// insert the value
				m_nCurrenSize++;					// increase current size
				m_nCurrentValue++;					// increase current value
			}
			else									// full
			{										// subtract the last value
				if (m_nCurrentValue >= m_nCurrenSize)// last value
				{
					m_nCurrentValue = 0;			// go to first
				}
				m_AvgSum = (TavgVal)(m_AvgSum - m_pAvgValues[m_nCurrentValue]);
				m_pAvgValues[m_nCurrentValue] = val;// overwrite the value
				m_nCurrentValue++;
			}
		}
	};
	//////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CWK_Average
 Function   : void SetNoOfAvgValues(int nValues)
 Description: Sets the number of values in the array. If the last value in the array is set,
              the first value is overwritten.
              If ARBITRARY_AVG_VAL is set, the number of values is arbitrary.

 Parameters:
  nValues: (): Number of values in the array.  (int)

 Result type:  (void)
 created: June, 16 2005
 <TITLE SetNoOfAvgValues>
*********************************************************************************************/
	void SetNoOfAvgValues(int nValues)
	{
		RemoveAll();
		if (nValues < 1)	// number of values is arbitrary
		{
			m_nNoOfAvgValues = ARBITRARY_AVG_VAL;
		}
		else				// certain number of values
		{					// are allocated at the beginning to avoid memory fragmentation
			m_nNoOfAvgValues = nValues;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// Access
/*********************************************************************************************
 Class      : CWK_Average
 Function   : TavgVal GetAtFast(int nPos)
 Description: retrieves a value in the array

 Parameters:
  nPos: (E): index of the value  (int)

 Result type:  (TavgVal)
 created: June, 16 2005
 <TITLE GetAtFast>
*********************************************************************************************/
	TavgVal GetAtFast(int nPos) const
	{
		ASSERT(nPos >= 0 && nPos < m_nCurrenSize);
		return m_pAvgValues[nPos];
	};
/*********************************************************************************************
 Class      : CWK_Average
 Function   : TavgVal GetSum()
 Description: retrieves the sum of the values in the array.

 Parameters: None

 Result type:  (double), (TavgVal)
 created: June, 16 2005
 <TITLE  GetSum()>
*********************************************************************************************/
	TavgVal GetSum() const
	{
		return m_AvgSum;
	};
/*********************************************************************************************
 Class      : CWK_Average
 Function   : double GetAverage(), TavgVal GetAverageT()
 Description: retrieves the average of the values in the array.

 Parameters: None

 Result type:  (double), (TavgVal)
 created: June, 16 2005
 <TITLE  GetAverage()>
*********************************************************************************************/
	double GetAverage()
	{
		if (m_nCurrenSize > 1)	// calculate the average value
		{
			double dSum = double(m_AvgSum), dDiv = double(m_nCurrenSize);
			return dSum / dDiv;
		}
		return (double)m_AvgSum;
	};
	TavgVal GetAverageT()
	{
		if (m_nCurrenSize > 1)	// calculate the average value
		{
			TavgVal dDiv = TavgVal(m_nCurrenSize);
			return m_AvgSum / dDiv;
		}
		return m_AvgSum;
	};
//////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CWK_Average
 Function   : double GetStandardDeviation()
 Description: calculates the standard deviation of the values in the array.

 Parameters: None

 Result type:  (double)
 created: June, 16 2005
 <TITLE double GetStandardDeviation>
*********************************************************************************************/
	double GetStandardDeviation()
	{
		if (m_nCurrenSize > 2)
		{
			return sqrt(GetQDiff() / (double)(m_nCurrenSize-1));
		}
		else if (m_nCurrenSize > 1)
		{
			return sqrt(GetQDiff());
		}
		return (double)m_AvgSum;
	};
/*********************************************************************************************
 Class      : CWK_Average
 Function   : double GetVariance()
 Description: calculates the variance of the values in the array

 Parameters: None

 Result type:  (double)
 created: June, 16 2005
 <TITLE double GetVariance>
*********************************************************************************************/
	double GetVariance()
	{
		if (m_nCurrenSize > 1)
		{
			return sqrt(GetQDiff() / (double)(m_nCurrenSize));
		}
		return (double)m_AvgSum;
	};
/*********************************************************************************************
 Class      : CWK_Average
 Function   : double GetQDiff(), TavgVal GetQDiffT()
 Description: calculates the square difference sum of the values in the array

 Parameters: None

 Result type:  (double), (TavgVal)
 created: June, 16 2005
 <TITLE double GetVariance>
*********************************************************************************************/
	double GetQDiff()
	{
		int i;
		double dSum = 0, dDiff, dAvg = GetAverage();
		for (i=0; i<m_nCurrenSize; i++)
		{
			dDiff = (double)GetAtFast(i) - dAvg;
			dSum += dDiff*dDiff;
		}
		return dSum;
	};
	TavgVal GetQDiffT()
	{
		int i;
		TavgVal dSum = 0, dDiff, dAvg = GetAverage();
		for (i=0; i<m_nCurrenSize; i++)
		{
			dDiff = GetAtFast(i) - dAvg;
			dSum += dDiff*dDiff;
		}
		return dSum;
	};
	//////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CWK_Average
 Function   : int GetActualValues()
 Description: retrieves the actual number of values in the array

 Parameters: None

 Result type:  (int)
 created: June, 16 2005
 <TITLE GetActualValues>
*********************************************************************************************/
	int     GetActualValues()  { return m_nCurrenSize;};
//////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CWK_Average
 Function   : int GetNoOfAvgValues()
 Description: retrieves the array size

 Parameters: None

 Result type:  (DWORD  )
 created: June, 16 2005
 <TITLE GetNoOfAvgValues>
*********************************************************************************************/
	DWORD   GetNoOfAvgValues() { return m_nNoOfAvgValues;};

protected:
	//////////////////////////////////////////////////////////////////////////

private:
	//////////////////////////////////////////////////////////////////////////
	int      m_nNoOfAvgValues;
	int		 m_nCurrentValue;
	int		 m_nCurrenSize;
	TavgVal  m_AvgSum;
	TavgVal *m_pAvgValues;
};

#endif