// MultipleCallProcess.h: interface for the CMultipleCallProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTIPLECALLPROCESS_H__D3F03194_F70F_11D2_B5D1_004005A19028__INCLUDED_)
#define AFX_MULTIPLECALLPROCESS_H__D3F03194_F70F_11D2_B5D1_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"

class CCallThread;
class CIPCInputServerCall;
class CIPCOutputServerCall;

class CMultipleCallProcess : public CProcess  
{
public:
	CMultipleCallProcess(CProcessScheduler* pScheduler,CActivation *pActivation, SecProcessType type);
	virtual ~CMultipleCallProcess();

	// attributes
public:
	CHost* GetCurrentHost();
	CHost* GetValidHost(BOOL bNext);
	inline const CString& GetName();
	BOOL IsConnected() const;
	BOOL IsDisconnected() const;
	BOOL IsValidHost(CHost* pHost) const;

	inline CIPCOutputServerCall* GetCIPCOutputServerCall() const;
	inline CIPCInputServerCall* GetCIPCInputServerCall() const;

	// overrides
public:
	virtual void Run();
	virtual void OnPause();

	// implementation
protected:
	void OnInitialized();
	void OnStart();
	void OnFetching();
	void OnConnecting();
	void OnConnection();
	void OnRunningLevel1();
	void OnRunningLevel2();
	void OnRunningLevel3();
	void OnConfirm();
	void NextCall();
	void CleanUpCIPC();
	void CheckError();

	// currents state
private:
	enum MultipleCallState
	{
		MCS_INITIALIZED,
		MCS_START,
		MCS_FETCHING,
		MCS_CONNECTING,
		MCS_CONNECTED,
		MCS_RUNNING_LEVEL_1,
		MCS_RUNNING_LEVEL_2,
		MCS_RUNNING_LEVEL_3,
		MCS_CONFIRMED,
		MCS_FINISHED,
		MCS_FAILED
	};

	// data member
private:
	int					 m_iFailedHosts;
	int					 m_iCurrentHost;
	int					 m_iTimeOutCounter;
	int					 m_iStartUpCounter;
	int					 m_iConnectionTimeOutCounter;
	CHostArray			 m_Hosts;
	MultipleCallState	 m_eState;
	CCallThread*		 m_pCallThread;
	CIPCInputServerCall* m_pCIPCInputServerCall;
	CIPCOutputServerCall* m_pCIPCOutputServerCall;
	CStringArray		 m_FailedHosts;
	BOOL				m_bProtokolInitialised;
};

inline const CString& CMultipleCallProcess::GetName()
{
	return m_Hosts.GetLocalHost()->GetName();
}
inline CIPCOutputServerCall* CMultipleCallProcess::GetCIPCOutputServerCall() const
{
	return m_pCIPCOutputServerCall;
}
inline CIPCInputServerCall* CMultipleCallProcess::GetCIPCInputServerCall() const
{
	return m_pCIPCInputServerCall;
}

#endif // !defined(AFX_MultipleCallProcess_H__D3F03194_F70F_11D2_B5D1_004005A19028__INCLUDED_)
