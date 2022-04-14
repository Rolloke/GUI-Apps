// CallProcess.h: interface for the CCallProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALLPROCESS_H__9A914184_CE4C_11D2_B598_004005A19028__INCLUDED_)
#define AFX_CALLPROCESS_H__9A914184_CE4C_11D2_B598_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"

class CCallProcess : public CProcess  
{
	// construction / destruction
public:
	// call and storing process
	CCallProcess(CProcessScheduler* pScheduler,CActivation *pActivation, CompressionType ct);
	virtual ~CCallProcess();

	// attributes
public:
	inline CSecID	GetCurrentHostID() const;
	inline CIPCOutputServerClient* GetOutputClient() const;

	// overrides
public:
	virtual void Run();
	virtual void OnPause();
	virtual void OnStateChanging(SecProcessState newState,BOOL bShutDown);

private:
	enum AlarmCallState 
	{ 
		ACS_INITIALIZED, 
		ACS_TRYING, 
		ACS_SUCCESS, 
		ACS_FAILED
	};
	static LPCTSTR NameOfEnum(AlarmCallState state);


	// implementation
private:
	void NextHost();
	void PrepareAlarmCall(BOOL& bAlreadyConnected, BOOL& bCameraSelected);	// returns TRUE if already connected
	BOOL DoAlarmCall();			// does FetchAlarmConnection within thread
	void StopAlarmCallThread();
	void CleanUpCIPC();

	// call thread procedure
	static UINT AlarmCallThreadFunction(LPVOID pParam);
	static volatile BOOL ms_bFetching;

	// data member
private:
	//
	AlarmCallState	m_alarmCallState;
	CWinThread*		m_pThread;

	BOOL  m_bThreadRun;
	BOOL  m_bGotAlarmConnection;
	DWORD m_tickLastAlarmCallTry;
	DWORD m_tickLastAlarmCallFailure;
	int	   m_iCurrentCalledHostIndex;
	CSecID m_idCurrentHost;
	
	//
	CIPCFetch				m_Fetch;
	//
	CConnectionRecord*		m_pAlarmCallRecord;
	//
	CIPCInputServerClient*	m_pInputClient;
	CIPCOutputServerClient*	m_pOutputClient;
};
///////////////////////////////////////////////////////////////////////
inline CSecID CCallProcess::GetCurrentHostID() const
{
	return m_idCurrentHost;
}
///////////////////////////////////////////////////////////////////////
inline CIPCOutputServerClient* CCallProcess::GetOutputClient() const
{
	return m_pOutputClient;
}

#endif // !defined(AFX_CALLPROCESS_H__9A914184_CE4C_11D2_B598_004005A19028__INCLUDED_)
