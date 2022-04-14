// ProcessScheduler.h: interface for the CProcessScheduler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSSCHEDULER_H__D33472DA_E7C6_49D6_986B_1AF440A0CB05__INCLUDED_)
#define AFX_PROCESSSCHEDULER_H__D33472DA_E7C6_49D6_986B_1AF440A0CB05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"

class CRequestCollector;
class CCallProcess;

class CProcessScheduler  
{
public:
	CProcessScheduler();
	virtual ~CProcessScheduler();

	// attributes
public:
	virtual	CIPC* GetParentCIPC();
			BOOL  IsRunning();
	inline  int	  GetNrOfProcesses();
	CProcess*     GetProcess(DWORD dwID);

	inline BOOL ParentIsAudio() const;
	inline BOOL ParentIsMiCo() const;
	inline BOOL ParentIsSaVic() const;
	inline BOOL ParentIsTasha() const;
	inline BOOL ParentIsQ() const;
	inline BOOL ParentIsIP() const;

		   Compression GetDefaultCompression(CSecID idCam);
		   Resolution  GetDefaultResolution(CSecID idCam);

	virtual BOOL IsTimeout();
	virtual BOOL IsImageTimeout();
	
	// operations
public:
	virtual void SetShutdownInProgress();

	void StartThread();
	BOOL StopThread();

	inline void Lock( LPCTSTR szMsg=NULL );
	inline void Unlock( LPCTSTR szMsg=NULL );

	// client processes
public:


	// processes
public:
	BOOL CanStartNewProcess();
	virtual BOOL StartNewProcess(CInput* pInput, 
								 CActivation& record,
								 BOOL& bRefThereWasAProcess,
								 CProcess*& pProcess,
  								 int iNumAlreadyRecorded = -1);
	CProcess* StopProcess(CProcess* pProcess);
	void KillProcesses(CInput *pInput);	// if disconnected or destructor
	void KillProcessByTimer(const CSecTimer *pTimer);
	virtual void TerminateClientProcesses(CSecID idClient)=0;
	BOOL FindCallProcess(CIPCOutputServerClient* pCIPCOutputServerClient, CCallProcess* pCallCheck);
	void TerminateCallProcesses(CIPCOutputServerClient* pCIPCOutputServerClient);

	// data
public:
	virtual void OnIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData);
	virtual void AnswerRequest(const CIPCPictureRecord &pictRect,
								DWORD dwMDX,
								DWORD dwMDY,
								DWORD pid,
								CRequestCollector* pRequest);
	virtual void OnUnitConfirmReset();
	virtual void OnUnitDisconnect();

	// misc
public:
	virtual void TraceProcesses(BOOL bForce=FALSE);
	virtual void TraceRequests();

protected:
			void CalcDefaultCompression();
			BOOL CheckProcessForTimer(const CActivation& activation);
//			BOOL CheckProcessForTimer(const CProcess *pProcess);
			void AddProcess(CProcess* pProcess);
	virtual	BOOL TerminateProcess(CProcess *pProcess, BOOL bShutDown=FALSE);
	virtual BOOL TerminateRelayProcess(CProcess *pProcess, BOOL bShutDown=FALSE);
	virtual	void TerminateAllProcesses();
			BOOL CheckDoubleRecordingProcesses(CSecID idCam, WORD wArchive);

	virtual void Run();
			BOOL PreWaitSecondGrid();
			void PostWaitSecondGrid();
			int  SetRunTick();
			int  GetDeltaTick();
			void SaveAlarm(CInput* pInput, WORD wArchiveNr, CSecID idOutput, CSecID idArchive);

private:
	static UINT SchedulerThreadFunction(LPVOID pParam);

	// data member
protected:
	CString		m_sParentShm;
	DWORD		m_dwLastGridTick;
	int			m_iDeltaOffset;	// offsets virtual seconds versus real seconds
	volatile DWORD	m_dwCurrentTime;
	CCriticalSection m_csCurrentTime;

	// CAVEAT duplicate pointers in m_processes and m_priorityProcesses
	CProcesses m_processes;		// the real 'container'
	CProcesses m_priorityProcesses[NUM_PRIORITIES];	// utility arrays

	volatile int	m_iNumStoringProcesses;
	volatile int	m_iNumClientProcesses;
	volatile BOOL	m_bShutdownInProgress;

	BOOL m_bParentIsCoCo;	// set in Init
	BOOL m_bParentIsMiCo;
	BOOL m_bParentIsSaVic;
	BOOL m_bParentIsAudio;
	BOOL m_bParentIsTasha;
	BOOL m_bParentIsQ;
	BOOL m_bParentIsIP;

	// data member
private:
	DWORD		m_dwIdleSleep;	// for ->Run()
	BOOL		m_bThreadActive;
	CWinThread* m_pThread;

public:
	static BOOL m_bTraceEncodeUnitRequests;
	static BOOL m_bTraceEncodeRequests;
	static BOOL m_bTraceInactiveByTimer;
	static BOOL m_bTraceInactiveByLock;
	static BOOL	m_bTracePictDataWithoutProcess;
	static BOOL m_bTraceVirtualSeconds;
	static BOOL m_bTraceVideoout;
};
////////////////////////////////////////////////////////////////
inline void CProcessScheduler::Lock( LPCTSTR szMsg)	
{ 
	m_processes.Lock(szMsg);
}
////////////////////////////////////////////////////////////////
inline void CProcessScheduler::Unlock( LPCTSTR szMsg)	
{ 
	if (szMsg) 
	{
		WK_TRACE(_T("UNLOCK: %s\n"),szMsg);
	}
	m_processes.Unlock();
}
////////////////////////////////////////////////////////////////
inline BOOL CProcessScheduler::ParentIsAudio() const
{
	return m_bParentIsAudio;
}
////////////////////////////////////////////////////////////////
inline BOOL CProcessScheduler::ParentIsTasha() const
{
	return m_bParentIsTasha;
}
////////////////////////////////////////////////////////////////
inline BOOL CProcessScheduler::ParentIsMiCo() const
{
	return m_bParentIsMiCo;
}
////////////////////////////////////////////////////////////////
inline BOOL CProcessScheduler::ParentIsSaVic() const
{
	return m_bParentIsSaVic;
}
////////////////////////////////////////////////////////////////
inline BOOL CProcessScheduler::ParentIsQ() const
{
	return m_bParentIsQ;
}
////////////////////////////////////////////////////////////////
inline BOOL CProcessScheduler::ParentIsIP() const
{
	return m_bParentIsIP;
}
/////////////////////////////////////////////////////////////////////////
inline int CProcessScheduler::GetNrOfProcesses()
{
	return m_processes.GetSize();
}

#endif // !defined(AFX_PROCESSSCHEDULER_H__D33472DA_E7C6_49D6_986B_1AF440A0CB05__INCLUDED_)
