// CIPCInt64.cpp: implementation of the CIPCInt64 class.
//
//////////////////////////////////////////////////////////////////////

#include "StdCipc.h"
#include "CIPCInt64.h"

//////////////////////////////////////////////////////////////////////
CString CIPCInt64::Format(BOOL bPoints) const
{
	BOOL bMinus = FALSE;
	__int64 t = m_Data64;
	CString r;
	int c;

	if (t<0)
	{
		t = -t;
		bMinus = TRUE;
	}
	if (t==0)
	{
		r = _T("0");
	}
	else
	{
		while(t>0)
		{
			c = (int)(t%10);
			t = t/10;
			r = (char)(c+'0') + r;
		}
	}

	if (bMinus)
	{
		r = '-' + r;
	}

	if (bPoints)
	{
		CString s3;
		CString s;

		while (r.GetLength())
		{
			s3 = r.Right(3);
			if (s.IsEmpty())
			{
				s = s3;
			}
			else
			{
				s  = s3 + '.' + s;
			}
			r  = r.Left(r.GetLength()-3); 
		}
		r = s;
	}

	return r;
}
//////////////////////////////////////////////////////////////////////
CString CIPCInt64::FormatInGB(int iPrecision, BOOL bPoints) const
{
	CString sResult;

	double d = (double)m_Data64;
	d /= (double)(1024*1024);


	CString sFormat;
	sFormat.Format(_T("%%.%df"),iPrecision);
	sResult.Format(sFormat,d);

	// NOT YET points

	return sResult;
}

