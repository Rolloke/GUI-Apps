// CSignal.cpp: implementation of the CSignal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Simulator.h"
#include "CSignal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CSignal::CSignal()
{
	m_nPeriode		= 0;
	m_nTime			= 0;
	m_nHiTime		= 0;
	m_nOffset		= 0;
	m_nLo			= 0;
	m_Col			= RGB(0,0,0);
	m_dwUNR			= rand() * 100000 / RAND_MAX;
	m_nField		= ODD;
	m_bVSync		= FALSE;
	m_nSyncCounter	= 0;
}

//////////////////////////////////////////////////////////////////////
CSignal::CSignal(int nPeriode, int nTimeOffset, int nField /*=ODD*/, int nLo/*=LO_LEVEL*/, int nHi/*=HI_LEVEL*/, COLORREF col/*RGB(0,0,0)*/)
{
	Init(nPeriode, nTimeOffset, nField, nLo, nHi, col);
}

//////////////////////////////////////////////////////////////////////
CSignal::~CSignal()
{

}

//////////////////////////////////////////////////////////////////////
CValue* CSignal::Out()
{
	int nResult = LO_LEVEL;
	int nTmp = m_nTime % m_nPeriode;

	if ((nTmp >= 0) && (nTmp < m_nHiTime))
		nResult = HI_LEVEL;

	return new CValue(nResult, m_nField, m_nSyncCounter, m_Col);
}

//////////////////////////////////////////////////////////////////////
void CSignal::Clock()
{
	m_nTime++;

	if ((m_nTime % m_nPeriode) == 0)
	{
		m_bVSync = TRUE;
		m_nSyncCounter++;
	}
	else
	{
		m_bVSync = FALSE;
	}	
	if (m_bVSync)
	{
		if (m_nField == ODD)
			m_nField = EVEN;
		else if (m_nField == EVEN)
			m_nField = ODD;
	}
}

//////////////////////////////////////////////////////////////////////
void CSignal::Init(int nPeriode, int nTimeOffset, int nField/*=ODD*/,int nLo/*=LO_LEVEL*/, int nHi/*=HI_LEVEL*/, COLORREF col/*=RGB(0,0,0)*/)
{
	m_nPeriode	= nPeriode;
	m_nTime		= nTimeOffset;
	m_nHiTime	= 1;			
	m_nLo		= nLo;
	m_nHi		= nHi;
	m_Col		= col;
	m_nField	= nField;
}

//////////////////////////////////////////////////////////////////////
CSignal* CSignal::operator & (CSignal&) 
{
	return this;
}

//////////////////////////////////////////////////////////////////////
int CSignal::GetField()
{
	return m_nField;
}

//////////////////////////////////////////////////////////////////////
BOOL CSignal::IsVSync()
{
	return m_bVSync;
}

//////////////////////////////////////////////////////////////////////
int CSignal::GetSyncCounter()
{
	return m_nSyncCounter;
}
