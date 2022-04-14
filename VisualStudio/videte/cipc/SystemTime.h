// SystemTime.h: interface for the CSystemTime class.
//
//////////////////////////////////////////////////////////////////////
#include "wkclasses\WK_Profile.h"
#include "CipcExtraMemory.h"

#if !defined(AFX_SYSTEMTIME_H__50B5B756_641F_11D2_B5DB_00C095EC9EFA__INCLUDED_)
#define AFX_SYSTEMTIME_H__50B5B756_641F_11D2_B5DB_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wk_template.h"

class AFX_EXT_CLASS CSystemTime : public SYSTEMTIME  
{
	// construction / destruction
public:
	CSystemTime();
	CSystemTime(DWORD dwHours);
	CSystemTime(const CSystemTime& s);

	CSystemTime(WORD wDay, WORD wMonth, WORD wYear,
			    WORD wHour, WORD wMinute, WORD wSecond, WORD wMilliseconds = 0);
	~CSystemTime();

	// attributes
public:
	inline WORD	GetDay() const;
	inline WORD	GetDayOfWeek() const;
	inline WORD	GetMonth() const;
	inline WORD	GetYear() const;
	inline WORD	GetHour() const;
	inline WORD	GetMinute() const;
	inline WORD	GetSecond() const;
	inline WORD	GetMilliseconds() const;

	// operators
public:
	BOOL operator <  (const CSystemTime& s);
	BOOL operator >  (const CSystemTime& s);
	BOOL operator <= (const CSystemTime& s);
	BOOL operator >= (const CSystemTime& s);
	BOOL operator == (const CSystemTime& s);

	CSystemTime operator + (const CSystemTime& timeSpan) const;
	CSystemTime operator - (const CSystemTime& timeSpan) const;

	// operations
public:
	void	GetLocalTime();
	BOOL	IncrementTime(DWORD dwDays, DWORD dwHours, DWORD dwMinutes,
						  DWORD dwSeconds, DWORD dwMilliSeconds);
	BOOL	DecrementTime(DWORD dwDays, DWORD dwHours, DWORD dwMinutes,
						  DWORD dwSeconds, DWORD dwMilliSeconds);
	CString	GetTime() const;
	CString	GetDate() const;
	CString	GetDateTime() const;
	CString	GetLocalizedTime() const; 
	CString	GetLocalizedDate(BOOL bShort=TRUE) const;
	CString	GetLocalizedDateTime(BOOL bShort=TRUE) const;
	CString	ToString();
	BOOL    FromString(const CString& s);
	CTime	GetCTime();
	BOOL    FromTime(CTime t);

	CString	GetDBTime() const;
	CString	GetDBDate() const;
	BOOL	SetDBTime(const CString& sDBTime);
	BOOL	SetDBDate(const CString& sDBDate);

	static  DWORD GetBubbleLength();
	void    WriteIntoBubble(BYTE*& pByte) const;
	void    ReadFromBubble(const BYTE*& pByte);

	operator LARGE_INTEGER() const;
	CSystemTime (const LARGE_INTEGER& li);
private:
};
inline WORD	CSystemTime::GetDay() const
{
	return wDay;
}
inline WORD	CSystemTime::GetMonth() const
{
	return wMonth;
}
inline WORD	CSystemTime::GetYear() const
{
	return wYear;
}
inline WORD	CSystemTime::GetHour() const
{
	return wHour;
}
inline WORD	CSystemTime::GetMinute() const
{
	return wMinute;
}
inline WORD	CSystemTime::GetSecond() const
{
	return wSecond;
}
inline WORD	CSystemTime::GetDayOfWeek() const
{
	return wDayOfWeek;
}
inline WORD	CSystemTime::GetMilliseconds() const
{
	return wMilliseconds;
}
typedef CSystemTime* CSystemTimePtr;
WK_PTR_ARRAY_CS(CSystemTimeArray, CSystemTimePtr, CSystemTimeArrayCS);
//////////////////////////////////////////////////////////////////////
//********************************************************************
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CSystemTimeSpan
{
	// construction
public:
	CSystemTimeSpan();
	CSystemTimeSpan(const CSystemTime& s,const CSystemTime& e);

	// attributes
public:
	inline CSystemTime& GetStartTime();
	inline CSystemTime& GetEndTime();
	inline BOOL IsIncluded(const CSystemTime& s);

	// operations
public:
	inline void SetTimes(const CSystemTime& s,const CSystemTime& e);
	inline void SetStartTime(const CSystemTime& s);
	inline void SetEndTimes(const CSystemTime& e);

	// data member
private:
	CSystemTime m_stStart;
	CSystemTime m_stEnd;
};
///////////////////////////////////////////////////////////////////////////
inline CSystemTime& CSystemTimeSpan::GetStartTime()
{
	return m_stStart;
}
///////////////////////////////////////////////////////////////////////////
inline CSystemTime& CSystemTimeSpan::GetEndTime()
{
	return m_stEnd;
}
///////////////////////////////////////////////////////////////////////////
inline BOOL CSystemTimeSpan::IsIncluded(const CSystemTime& s)
{
	CSystemTime st(s);
	return (m_stStart <= st) && (st<=m_stEnd);
}
///////////////////////////////////////////////////////////////////////////
inline void CSystemTimeSpan::SetTimes(const CSystemTime& s,const CSystemTime& e)
{
	m_stStart = s;
	m_stEnd = e;
}
///////////////////////////////////////////////////////////////////////////
inline void CSystemTimeSpan::SetStartTime(const CSystemTime& s)
{
	m_stStart = s;
}
///////////////////////////////////////////////////////////////////////////
inline void CSystemTimeSpan::SetEndTimes(const CSystemTime& e)
{
	m_stEnd = e;
}
///////////////////////////////////////////////////////////////////////////
typedef CSystemTimeSpan* CSystemTimeSpanPtr;
WK_PTR_ARRAY_CS(CSystemTimeSpanArrayPlain, CSystemTimeSpanPtr, CSystemTimeSpanArray)

class AFX_EXT_CLASS CSystemTimeSpans : public CSystemTimeSpanArray
{
	// construction/destruction
public:
	CSystemTimeSpans();

	// attributes
public:
	BOOL IsIncluded(const CSystemTime& s);

	// operations
public:
	void Load(CWK_Profile& wkp, const CString& sSection);
	void Save(CWK_Profile& wkp, const CString& sSection);

	// bubble code
public:
	void FromArray(int iNumRecords,const CSystemTimeSpan data[]);
	CIPCExtraMemory*  BubbleFromSystemTimeSpans(CIPC *pCipc) const;
	static CSystemTimeSpan* SystemTimeSpansFromBubble(int iNumRecords,
													   const CIPCExtraMemory *pExtraMem);
};


#endif // !defined(AFX_SYSTEMTIME_H__50B5B756_641F_11D2_B5DB_00C095EC9EFA__INCLUDED_)
