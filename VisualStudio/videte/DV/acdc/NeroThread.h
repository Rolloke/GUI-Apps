// CNeroThread.h: interface for the CNeroThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEROTHREAD_H__B69D0FDF_33AB_4090_95DF_DA2F8143C438__INCLUDED_)
#define AFX_NEROTHREAD_H__B69D0FDF_33AB_4090_95DF_DA2F8143C438__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NeroBurn.h"
#include "Producer.h"


#include "wkclasses/wk_thread.h"


class CNeroBurn;

class CNeroThread : public CWK_Thread  
{
//functions
public:
	virtual BOOL StartThread();
	virtual BOOL StopThread();
	virtual BOOL Run(LPVOID lpContext);
	CNeroThread();
	virtual ~CNeroThread();

	void SetMainFunktion(MainFunction mf);
	CNeroBurn* GetNeroBurn();

//members
private:

	CNeroBurn* m_pNeroBurn;
	MainFunction m_MainFunktion;
	CProducer* m_pProducer;
};

#endif // !defined(AFX_NEROTHREAD_H__B69D0FDF_33AB_4090_95DF_DA2F8143C438__INCLUDED_)
