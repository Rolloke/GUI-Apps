// CDecoder.h: interface for the CDecoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDECODER_H__F549E727_C023_4FD9_9370_500CAD563DBE__INCLUDED_)
#define AFX_CDECODER_H__F549E727_C023_4FD9_9370_500CAD563DBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CSignal.h"

class CDecoder  
{
public:
	void Clock();
	CDecoder();
	virtual ~CDecoder();

	CSignal& Out();
	void In(CSignal & Signal);

private:
	CSignal*	m_pSignal;
	int			m_nReSyncTime;
	CSignal		m_SignalNoSync;
};

#endif // !defined(AFX_CDECODER_H__F549E727_C023_4FD9_9370_500CAD563DBE__INCLUDED_)
