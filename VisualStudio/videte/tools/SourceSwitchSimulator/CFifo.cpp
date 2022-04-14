// CFifo.cpp: implementation of the CFifo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Simulator.h"
#include "CFifo.h"
#include "CSignal.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFifo::CFifo()
{
	m_bEnableFifo = FALSE;
}

//////////////////////////////////////////////////////////////////////
CFifo::~CFifo()
{

}

//////////////////////////////////////////////////////////////////////
void CFifo::Push(CValue *pValue)
{
	TRACE("Fill=%d\n", m_Values.GetSize());
	if (m_Values.GetSize() >= 20)
		return;

	int nLast = m_Values.GetSize()-1;
	
	if (m_bEnableFifo == TRUE)
	{
		if (pValue && pValue->GetInt() == HI_LEVEL)
		{
			if (m_Values.GetSize() == 0)	
				m_bEnableFifo = FALSE;
			else if (m_Values.GetAt(nLast)->GetInt() == LO_LEVEL)
				m_bEnableFifo = FALSE;
		}
	}

	if (m_bEnableFifo == FALSE)
	{
		if (pValue && (pValue->GetInt() == HI_LEVEL) && (pValue->GetField() == ODD))
		{
			if (m_Values.GetSize() == 0)
				m_bEnableFifo = TRUE;
			else if (m_Values.GetAt(nLast)->GetInt() == LO_LEVEL)
				m_bEnableFifo = TRUE;
		}
	}
	
	if (m_bEnableFifo)
		m_Values.Add(pValue);
	else
		delete pValue;

}

//////////////////////////////////////////////////////////////////////
CValue* CFifo::Pop()
{
	int nCount = m_Values.GetSize();
	if (nCount == 0)
		return NULL;

	CValue* pValue = m_Values.GetAt(0);
	m_Values.RemoveAt(0);

	return pValue;
}

//////////////////////////////////////////////////////////////////////
void CFifo::Clear()
{

}

//////////////////////////////////////////////////////////////////////
CValue* CFifo::Get()
{
	int nCount = m_Values.GetSize();
	if (nCount == 0)
		return NULL;

	CValue* pValue = m_Values.GetAt(0);

	return pValue;
}

//////////////////////////////////////////////////////////////////////
int CFifo::GetSize()
{
	return m_Values.GetSize();
}
