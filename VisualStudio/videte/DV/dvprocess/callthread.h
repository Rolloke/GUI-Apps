// CallProcess.h: interface for the CCallThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALLPROCESS_H__9A914184_CE4C_11D2_B598_004005A19028__INCLUDED_)
#define AFX_CALLPROCESS_H__9A914184_CE4C_11D2_B598_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"

class CCallThread : public CWK_Thread  
{
	// construction / destruction
public:
	// call and storing process
	CCallThread();
	virtual ~CCallThread();

public:
	enum AlarmCallState 
	{ 
		ACS_SLEEPING,
		ACS_INITIALIZED, 
		ACS_TRYING, 
		ACS_PREPARED,
		ACS_SUCCESS, 
		ACS_FAILED
	};

	// operations
public:
	void   AddAlarmOutputID(CSecID id);

	inline AlarmCallState		GetState();
	inline CConnectionRecord*	GetConnectionRecord();
	inline CInputClient*		GetInputClient();
	inline COutputClient*		GetOutputClient();

	void InputDisconnected(CInputClient* pInputClient);
	void OutputDisconnected(COutputClient* pOutputClient);
	void DialingNumbersChanged();

	// overrides
public:
	virtual	BOOL StartThread();
	virtual	BOOL StopThread();
	virtual	BOOL Run(LPVOID lpContext);


	// implementation
private:
	void HandleAlarmCall();
	BOOL AlreadyConnected();
	void PrepareAlarmCall();
	void DoAlarmCall();
	void ResetAlarmHostList();
	CString GetRealNumber(const CString& sNumber,BOOL bTCPPrefix);
	void EmptyAlarmIDQueue();

	// data member
private:
	//
	CDWordArray			m_AlarmIDs;
	CCriticalSection	m_csAlarmIDs;
	HANDLE				m_hSemaphore;
	CStringArray		m_saHosts;
	int					m_iCurrentHost;
	CSecID				m_idCurrent;	
	AlarmCallState		m_State;
	DWORD				m_dwFailedTime;
	BOOL				m_bRuntimeErrorNoHostSent;

	CConnectionRecord*	m_pConnectionRecord;
	CInputClient*		m_pInputClient;
	COutputClient*		m_pOutputClient;
};
//////////////////////////////////////////////////////////////////////
inline CConnectionRecord* CCallThread::GetConnectionRecord()
{
	return m_pConnectionRecord;
}
//////////////////////////////////////////////////////////////////////
inline CInputClient* CCallThread::GetInputClient()
{
	return m_pInputClient;
}
//////////////////////////////////////////////////////////////////////
inline COutputClient* CCallThread::GetOutputClient()
{
	return m_pOutputClient;
}
//////////////////////////////////////////////////////////////////////
inline CCallThread::AlarmCallState CCallThread::GetState()
{
	return m_State;
}

#endif // !defined(AFX_CALLPROCESS_H__9A914184_CE4C_11D2_B598_004005A19028__INCLUDED_)
