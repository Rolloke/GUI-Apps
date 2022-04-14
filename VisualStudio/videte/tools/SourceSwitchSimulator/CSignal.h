// CSignal.h: interface for the CSignal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSIGNAL_H__D27A3DAC_5A93_4B4E_83CF_3D9D24737973__INCLUDED_)
#define AFX_CSIGNAL_H__D27A3DAC_5A93_4B4E_83CF_3D9D24737973__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CValue.h"

#define LO_LEVEL	0
#define MID_LEVEL	2
#define HI_LEVEL	5

#define UNDEFINE	0
#define ODD			1
#define EVEN		2

class CSignal  
{
public:
	int GetSyncCounter();
	BOOL IsVSync();
	int GetField();
	CSignal();
	CSignal(int nPeriode, int nTimeOffset, int nField, int nLo=LO_LEVEL, int nHi=HI_LEVEL, COLORREF col=RGB(0,0,0));
	virtual ~CSignal();

	void    Init(int nPeriode, int nTimeOffset, int nField, int nLo=LO_LEVEL, int nHi=HI_LEVEL, COLORREF col=RGB(0,0,0));
	void    Clock();
	CValue* Out();
	DWORD	GetUNR(){return m_dwUNR;};

	CSignal* operator&(CSignal&);
  
private:
	DWORD		m_dwUNR;
	COLORREF	m_Col;
	int			m_nPeriode;
	int			m_nOffset;
	int			m_nTime;
	int			m_nHiTime;
	int			m_nLo;
	int			m_nHi;
	int			m_nField;
	int			m_nSyncCounter;
	BOOL		m_bVSync;
};

typedef CSignal * CSignalPtr;
WK_PTR_ARRAY_CS(CSignalPtrArray, CSignalPtr, CSignalPtrArrayCS)

#endif // !defined(AFX_CSIGNAL_H__D27A3DAC_5A93_4B4E_83CF_3D9D24737973__INCLUDED_)

						 