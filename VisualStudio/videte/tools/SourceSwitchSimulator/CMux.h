// CMux.h: interface for the CMux class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMUX_H__97630321_8BDD_4213_8742_3169DF6DAAD1__INCLUDED_)
#define AFX_CMUX_H__97630321_8BDD_4213_8742_3169DF6DAAD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CSignal.h"

class CMux  
{
public:
	CMux();
	virtual ~CMux();
	Init(int nOutPorts);

	void SwitchSignalToOutput(CSignal& Signal, int nOutPort);
	CSignal& Out(int nOutPort);

private:
	int					m_nOutPorts;
	CSignalPtrArrayCS 	m_OSignal;
	
	BYTE*		m_pByte;
};

#endif // !defined(AFX_CMUX_H__97630321_8BDD_4213_8742_3169DF6DAAD1__INCLUDED_)
