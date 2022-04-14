// Statistic.h: interface for the CStatistic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATISTIC_H__834A84A3_8069_11D2_B50B_004005A19028__INCLUDED_)
#define AFX_STATISTIC_H__834A84A3_8069_11D2_B50B_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStatistic  
{
	// constructor
public:
	CStatistic();
	virtual ~CStatistic();
	// attributes
public:
	double GetFPS();
	double GetMBperSecond();
	// operations
public:
	void AddBytes(DWORD dwBytes);
private:
	double		 m_fDataLen;
	DWORD		 m_dwTick;
	DWORD		 m_dwCount;
	CCriticalSection m_cs;
};

#endif // !defined(AFX_STATISTIC_H__834A84A3_8069_11D2_B50B_004005A19028__INCLUDED_)
