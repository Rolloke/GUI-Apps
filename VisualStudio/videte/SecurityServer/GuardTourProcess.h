// GuardTourProcess.h: interface for the CGuardTourProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUARDTOURPROCESS_H__4DC11C2E_2C63_402A_A682_3AC490889CED__INCLUDED_)
#define AFX_GUARDTOURPROCESS_H__4DC11C2E_2C63_402A_A682_3AC490889CED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"

class CIPCServerToVision;
class CGuardTourThread;

class CGuardTourProcess : public CProcess  
{
// Construction/Destruction
public:
	CGuardTourProcess(CProcessScheduler* pScheduler,CActivation *pActivation);
	virtual ~CGuardTourProcess();

	// overrides
public:
	virtual void Run();
	virtual void OnPause();
	virtual	BOOL Terminate(BOOL bShutDown=FALSE);

	void ControlDisconnected(CIPCServerToVision* pCIPC);
	inline CIPCServerToVision* GetServerToVision();


	// implementation
protected:
	// states
	void OnInitialized();
	void OnWaitingForClient();
	void OnClientStarted();
	void OnStartFetchThread();
	void OnFetching();
	void OnWaitingForConnect();
	void OnConnected();
	void OnFinished();

	// helper
	void CleanUpGuardTour();
	void DoDisconnect();

	// currents state
private:
	enum GuardTourState
	{
		GTS_INITIALIZED,
		GTS_WAITING_FOR_CLIENT,
		GTS_CLIENT_STARTED,
		GTS_START_FETCH_THREAD,
		GTS_FETCHING,
		GTS_WAITING_FOR_CONNECT,
		GTS_CONNECTED,
		GTS_FINISHED,
	};

	static LPCTSTR NameOfEnum(GuardTourState gts);

	// data member
private:
	GuardTourState		m_State;
	CIPCServerToVision* m_pCIPCServerToVision;
	CConnectionRecord*  m_pConnectionRecord;
	CGuardTourThread*	m_pGuardTourThread;
	int					m_iCurrentHost;
	DWORD				m_dwCurrentTry;
	int					m_iCurrentCamera;
	CWordArray			m_wHosts;
	CStringArray		m_sCameras;
	CWordArray			m_wCameras;
	DWORD				m_dwCameraSwitchTime;
	DWORD				m_dwStartWaitingForConnect;
};
///////////////////////////////////////////////////////////////////////
inline CIPCServerToVision* CGuardTourProcess::GetServerToVision()
{
	return m_pCIPCServerToVision;
}

#endif // !defined(AFX_GUARDTOURPROCESS_H__4DC11C2E_2C63_402A_A682_3AC490889CED__INCLUDED_)
