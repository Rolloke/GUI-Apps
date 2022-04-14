/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaFirmware535
// FILE:		$Workfile: CInt64.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/Include/CInt64.h $
// CHECKIN:		$Date: 11.05.04 10:46 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 11.05.04 10:45 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CINT64_H__
#define __CINT64_H__

#include "Helper.h"

class CInt64  
{
	// Construction/Destruction
public:
	CInt64();
	CInt64(const CInt64& source);
	CInt64(int i);
	CInt64(DWORD dwLo);
	CInt64(DWORD dwLo, DWORD dwHi);
	CInt64(const LARGE_INTEGER& ul);
	
	// operators
public:
	const CInt64 & operator =(const CInt64 &source);

	const CInt64 & operator +=(const CInt64 &source);
	const CInt64 & operator -=(const CInt64 &source);
	const CInt64 & operator *=(const CInt64 &source);
	const CInt64 & operator /=(const CInt64 &source);
	
	CInt64 operator +(const CInt64 &source);
	CInt64 operator -(const CInt64 &source);
	CInt64 operator *(const CInt64 &source);
	CInt64 operator /(const CInt64 &source);

//	inline const CInt64 operator %(const CInt64 &source);

	const CInt64& operator ++();
	const CInt64& operator --();
	CInt64  operator ++(int);
	CInt64  operator --(int);

	LARGE_INTEGER GetInt64() const;

	BOOL operator < (const CInt64& s);
	BOOL operator > (const CInt64& s);
	BOOL operator == (const CInt64& s);
	BOOL operator != (const CInt64& s);
	BOOL operator >= (const CInt64& s);
	BOOL operator <= (const CInt64& s);

	operator DWORD();
	operator const char*();
private:
	LARGE_INTEGER m_Data64;
	char m_sBuffer[22];

};
#endif //__CINT64_H__
