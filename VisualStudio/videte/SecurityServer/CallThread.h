// CallThread.h: interface for the CCallThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALLTHREAD_H__EF225B63_F7E8_11D2_B5D7_004005A19028__INCLUDED_)
#define AFX_CALLTHREAD_H__EF225B63_F7E8_11D2_B5D7_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMultipleCallProcess;

class CCallThread : public CWK_Thread  
{
public:
	CCallThread(CMultipleCallProcess* pProcess);
	virtual ~CCallThread();

	// attributes
public:
	CIPCFetchResult*	GetInputResult();
	CIPCFetchResult*	GetOutputResult();

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);
			void DeleteInputResult();
			void DeleteOutputResult();
			void Lock();
			void Unlock();

	// implementation
private:
	BOOL DoFetch();

	// data member
private:
	CMultipleCallProcess*	m_pProcess;
	CIPCFetchResult*		m_pInputResult;
	CIPCFetchResult*		m_pOutputResult;
	CCriticalSection		m_cs;
	CString					m_sHost;
};

#endif // !defined(AFX_CALLTHREAD_H__EF225B63_F7E8_11D2_B5D7_004005A19028__INCLUDED_)
