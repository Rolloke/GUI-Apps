// IPCInt64.h: interface for the CIPCInt64 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCINT64_H__E03A4AB5_3E39_11D2_B59B_00C095EC9EFA__INCLUDED_)
#define AFX_CIPCINT64_H__E03A4AB5_3E39_11D2_B59B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class AFX_EXT_CLASS CIPCInt64  
{
	// Construction/Destruction
public:
	inline CIPCInt64();
	inline CIPCInt64(const CIPCInt64& source);
	inline CIPCInt64(int i);
	inline CIPCInt64(DWORD dw);
	inline CIPCInt64(__int64 i);
	inline CIPCInt64(unsigned __int64 i);
	inline CIPCInt64(const ULARGE_INTEGER& ul);
	
	// operators
public:
	inline const CIPCInt64 & operator =(const CIPCInt64 &source);
	inline const CIPCInt64 & operator +=(const CIPCInt64 &source);
	inline const CIPCInt64 & operator -=(const CIPCInt64 &source);
	inline const CIPCInt64 & operator *=(const CIPCInt64 &source);
	inline __int64 GetInt64() const;

	inline BOOL operator < (const CIPCInt64& s);
	inline BOOL operator > (const CIPCInt64& s);
	inline BOOL operator == (const CIPCInt64& s);
	inline BOOL operator != (const CIPCInt64& s);
	inline BOOL operator >= (const CIPCInt64& s);
	inline BOOL operator <= (const CIPCInt64& s);

	inline DWORD	GetInMB() const;
	CString Format(BOOL bPoints) const;

	CString FormatInGB(int precision, BOOL bPoints) const;
private:
	__int64	m_Data64;
};

//////////////// inlined functions ////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
inline CIPCInt64::CIPCInt64()
{
	m_Data64 = 0;
}
//////////////////////////////////////////////////////////////////////
inline CIPCInt64::CIPCInt64(const CIPCInt64& source)
{
	m_Data64 = source.m_Data64;
}

inline const CIPCInt64 & CIPCInt64::operator =(const CIPCInt64 &source)
{
	m_Data64 = source.m_Data64;
	return *this;
}


inline const CIPCInt64 & CIPCInt64::operator +=(const CIPCInt64 &source)
{
	m_Data64 += source.m_Data64;
	return *this;
}

inline const CIPCInt64 & CIPCInt64::operator -=(const CIPCInt64 &source)
{
	m_Data64 -= source.m_Data64;
	return *this;
}

inline const CIPCInt64 & CIPCInt64::operator *=(const CIPCInt64 &source)
{
	m_Data64 *= source.m_Data64;
	return *this;
}



//////////////////////////////////////////////////////////////////////
inline CIPCInt64::CIPCInt64(__int64 i)
{
	m_Data64 = i;
}
//////////////////////////////////////////////////////////////////////
inline CIPCInt64::CIPCInt64(unsigned __int64 i)
{
	m_Data64 = i;
}
//////////////////////////////////////////////////////////////////////
inline CIPCInt64::CIPCInt64(int i)
{
	m_Data64 = (__int64)i;
}
//////////////////////////////////////////////////////////////////////
inline CIPCInt64::CIPCInt64(DWORD dw)
{
	m_Data64 = (__int64)dw;
}
//////////////////////////////////////////////////////////////////////
inline CIPCInt64::CIPCInt64(const ULARGE_INTEGER& ul)
{
	m_Data64 = ul.QuadPart;
}
//////////////////////////////////////////////////////////////////////
inline __int64 CIPCInt64::GetInt64() const
{
	return m_Data64;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CIPCInt64::GetInMB() const
{
	return (DWORD)(m_Data64 / (__int64)(1024*1024));
}
//////////////////////////////////////////////////////////////////////
inline BOOL CIPCInt64::operator < (const CIPCInt64& s)
{
	return m_Data64 < s.m_Data64;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CIPCInt64::operator > (const CIPCInt64& s)
{
	return m_Data64 > s.m_Data64;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CIPCInt64::operator == (const CIPCInt64& s)
{
	return m_Data64 == s.m_Data64;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CIPCInt64::operator != (const CIPCInt64& s)
{
	return m_Data64 != s.m_Data64;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CIPCInt64::operator >= (const CIPCInt64& s)
{
	return m_Data64 >= s.m_Data64;
}
//////////////////////////////////////////////////////////////////////
inline BOOL CIPCInt64::operator <= (const CIPCInt64& s)
{
	return m_Data64 <= s.m_Data64;
}

#endif // !defined(AFX_INT64_H__E03A4AB5_3E39_11D2_B59B_00C095EC9EFA__INCLUDED_)
