// Int64.h: interface for the CInt64 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INT64_H__E03A4AB5_3E39_11D2_B59B_00C095EC9EFA__INCLUDED_)
#define AFX_INT64_H__E03A4AB5_3E39_11D2_B59B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CInt64  
{
	// Construction/Destruction
public:
	CInt64();
	CInt64(const CInt64& source);
	CInt64(int i);
	CInt64(DWORD dw);
	CInt64(__int64 i);
	CInt64(unsigned __int64 i);
	CInt64(const ULARGE_INTEGER& ul);
	virtual ~CInt64();

	// operators
public:
	operator __int64() const;

	// operations
public:
	CString Format(BOOL bPoints) const;
	DWORD	GetInMB() const;

private:
	__int64	m_Data;
};

#endif // !defined(AFX_INT64_H__E03A4AB5_3E39_11D2_B59B_00C095EC9EFA__INCLUDED_)
