// ProcessSchedulerAudio.h: interface for the CProcessSchedulerAudio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSSCHEDULERAUDIO_H__3B9526FD_1B32_4B7D_886C_AD229573733A__INCLUDED_)
#define AFX_PROCESSSCHEDULERAUDIO_H__3B9526FD_1B32_4B7D_886C_AD229573733A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProcessScheduler.h"

class CIPCAudioServer;
class CRequestCollectors;
class CAudioProcess;

class CProcessSchedulerAudio : public CProcessScheduler  
{
public:
	CProcessSchedulerAudio(CIPCAudioServer *pCipc);
	virtual ~CProcessSchedulerAudio();

	// attributes
public:
	virtual	CIPC* GetParentCIPC();

	// operations
public:
	//
	virtual BOOL StartNewProcess(CInput* pInput, 
								 CActivation& record,
								 BOOL& bRefThereWasAProcess,
								 CProcess*& pProcess,
								 int iNumAlreadyRecorded = -1);
	virtual	BOOL TerminateProcess(CProcess *pProcess, BOOL bShutDown=FALSE);
	virtual void TerminateClientProcesses(CSecID idClient);

	// client stuff
	void DoClientEncoding(CSecID idClient, 
						  CProcess*& pRefEncodeProcess, // CAVEAT by ref, is set in call
						  CSecID mediaID,
						  DWORD dwFlags,
						  BOOL bStart);

	// answers from unit
public:
	void OnIndicateMediaData(const CIPCMediaSampleRecord& rec, CSecID secID, DWORD dwUserData);
	// this is called from the CRequestCollectors
	void AnswerRequest(const CIPCMediaSampleRecord& rec, CSecID idMedia, DWORD pid);

protected:
	virtual void Run();

private:
	CIPCAudioServer*    m_pCIPCAudioServer;
	CRequestCollectors* m_pRequestCollectors;
};

#endif // !defined(AFX_PROCESSSCHEDULERAUDIO_H__3B9526FD_1B32_4B7D_886C_AD229573733A__INCLUDED_)
