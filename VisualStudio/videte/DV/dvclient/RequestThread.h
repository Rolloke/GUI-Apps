// RequestThread.h: interface for the CRequestThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUESTTHREAD_H__9E2BF085_406C_439A_8104_A3090F44EE3E__INCLUDED_)
#define AFX_REQUESTTHREAD_H__9E2BF085_406C_439A_8104_A3090F44EE3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRequestThread : public CWK_Thread  
{
	// construction/destruction
public:
	CRequestThread();
	virtual ~CRequestThread();
	bool SetTimeOutValue(int nNewValue=-1);

	// overrides
public:
	virtual	BOOL Run(LPVOID lpContext);

	// data
private:
	int m_iTimeOut;
	volatile int m_nTimeOutValue;
};

#endif // !defined(AFX_REQUESTTHREAD_H__9E2BF085_406C_439A_8104_A3090F44EE3E__INCLUDED_)
