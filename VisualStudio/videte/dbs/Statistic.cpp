// Statistic.cpp: implementation of the CStatistic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dbs.h"
#include "Statistic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStatistic::CStatistic()
{
	m_cs.Lock();
	m_fDataLen = 0.0;
	m_dwCount = 0;
	m_dwTick = GetTickCount();
	m_cs.Unlock();
}
//////////////////////////////////////////////////////////////////////
CStatistic::~CStatistic()
{

}
//////////////////////////////////////////////////////////////////////
double CStatistic::GetFPS()
{
	m_cs.Lock();
	double fTick = (double)(GetTickCount() - m_dwTick);
	double r = ((double)m_dwCount * 1000.0)/fTick;
	m_cs.Unlock();
	return r;
}
//////////////////////////////////////////////////////////////////////
double CStatistic::GetMBperSecond()
{
	m_cs.Lock();

	double fTick = (double)(GetTickCount() - m_dwTick);
	double r = 0.0;

	if (fTick>0)
	{
		r = (m_fDataLen*1000.0)/(fTick*1024.0*1024.0);
	}
	else
	{
		r = 0.0;
	}
	m_cs.Unlock();
	return r;
}
//////////////////////////////////////////////////////////////////////
void CStatistic::AddBytes(DWORD dwBytes)
{
	m_cs.Lock();
	DWORD dwTick = GetTickCount();
	if (dwTick - m_dwTick > 30 * 1000)
	{
		m_fDataLen = 0;
		m_dwTick = dwTick;
		m_dwCount = 0;
	}
	m_fDataLen += (double)dwBytes;
	m_dwCount++;
	m_cs.Unlock();
}
