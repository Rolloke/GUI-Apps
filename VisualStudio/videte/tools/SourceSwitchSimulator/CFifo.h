// CFifo.h: interface for the CFifo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CFIFO_H__8CF9009D_AB9B_47B1_A0D2_E08FDF94EB9F__INCLUDED_)
#define AFX_CFIFO_H__8CF9009D_AB9B_47B1_A0D2_E08FDF94EB9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CValue.h"
#include "CValues.h"
class CFifo  
{
public:
	int GetSize();
	CValue* Get();
	CFifo();
	virtual ~CFifo();

	void	Push(CValue* pValue);
	CValue* Pop();
	void	Clear();

private:
	CValues m_Values;
	BOOL	m_bEnableFifo;
};

#endif // !defined(AFX_CFIFO_H__8CF9009D_AB9B_47B1_A0D2_E08FDF94EB9F__INCLUDED_)
