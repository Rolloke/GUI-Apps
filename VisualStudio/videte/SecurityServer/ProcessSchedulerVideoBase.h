// ProcessSchedulerVideoBase.h: interface for the CProcessScheduler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSSCHEDULERVIDEOBASE_H__)
#define AFX_PROCESSSCHEDULERVIDEOBASE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "processscheduler.h"
#include "ClientOverlayProcess.h"

class CIPCOutputServer;

class CProcessSchedulerVideoBase : public CProcessScheduler
{
	// construction / destruction
public:
	CProcessSchedulerVideoBase(CIPCOutputServer* pCIPCOutputServer);
	virtual ~CProcessSchedulerVideoBase();


	// attributes
public:
	virtual BOOL	IsImageTimeout();
	virtual int		GetMaxFps();
	virtual int		GetMaxNumCameraSwitches();
	inline CIPCOutputServer* GetParent();

	// overrides
public:
	virtual BOOL StartNewProcess(CInput* pInput, 
								 CActivation& record,
								 BOOL& bRefThereWasAProcess,
								 CProcess*& pProcess,
								 int iNumAlreadyRecorded = -1);
	virtual void TerminateClientProcesses(CSecID idClient);

	// operations
public:
	void DoClientVideoOut(CSecID id);
	void DoActivityVideoOut(CSecID id);
	virtual void DoClientEncoding(CSecID idClient, 
									CProcessPtr &pProcess, // CAVEAT by ref, is set in call
									CSecID camID,
									Resolution res, 
									Compression comp, 
									CompressionType ct,
									int iNumPictures,
									DWORD dwTimeSlice,
									DWORD dwBitrate);
	void ClientRelayProcess(CSecID idClient,
							CProcessPtr &pRefEncodeProcess, // CAVEAT by ref, is set in call
							CSecID relayID,
							BOOL bRequestedState);
	void HandleLocalVideo(CSecID idClient,
						  CSecID camID, 
						  Resolution res,
						  const CRect &rect,
						  BOOL bActive);
	virtual void OnUnitDisconnect();
	// this is called from the CRequestCollectors
	virtual void AnswerRequest(const CIPCPictureRecord &pictRect,
								DWORD dwMDX,
								DWORD dwMDY,
								DWORD pid,
								CRequestCollector* pRequest);
	virtual void OnVideoSignal(CSecID id, int iErrorCode);


	// implementation
protected:
	virtual BOOL TerminateRelayProcess(CProcess *pProcess, BOOL bShutDown=FALSE);

	// data member
protected:
	CIPCOutputServer*    m_pCIPCOutputServer;
	volatile DWORD m_dwLastPicture;
	CCriticalSection m_csLastPicture;
	int   m_iNumLongPSequences;
	CClientOverlayProcess* m_pLocalVideoProcess;

private:
	int m_iNumRelayProcesses;

};
////////////////////////////////////////////////////////////////
inline CIPCOutputServer* CProcessSchedulerVideoBase::GetParent()
{
	return m_pCIPCOutputServer;
}
#endif