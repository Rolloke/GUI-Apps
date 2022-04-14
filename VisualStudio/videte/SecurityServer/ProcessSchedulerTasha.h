// ProcessSchedulerTasha.h: interface for the CProcessScheduler_Tasha_Q_IP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSSCHEDULERTASHA_H__3B9526FD_1B32_4B7D_886C_AD229573733A__INCLUDED_)
#define AFX_PROCESSSCHEDULERAUDIO_H__3B9526FD_1B32_4B7D_886C_AD229573733A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProcessSchedulerVideoBase.h"

class CIPCAudioServer;
class CRequestCollectors;

class CProcessScheduler_Tasha_Q_IP : public CProcessSchedulerVideoBase  
{
public:
	CProcessScheduler_Tasha_Q_IP(CIPCOutputServer *pCipc, int iSize);
	virtual ~CProcessScheduler_Tasha_Q_IP();

	// attributes
public:
	virtual	CIPC* GetParentCIPC();
	virtual int	  GetMaxNumCameraSwitches();
	virtual int	  GetMaxFps();

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
	virtual void DoClientEncoding(CSecID idClient, 
									CProcessPtr &pProcess, // CAVEAT by ref, is set in call
									CSecID camID,
									Resolution res, 
									Compression comp, 
									CompressionType ct,
									int iNumPictures,
									DWORD dwTimeSlice,
									DWORD dwBitrate);

	// answers from unit
public:
	virtual void OnIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData);
	virtual void OnVideoSignal(CSecID id, int iErrorCode);
	virtual void AnswerRequest(const CIPCPictureRecord &pictRect,
								DWORD dwMDX,
								DWORD dwMDY,
								DWORD pid,
								CRequestCollector* pRequest);
			void RemoveRequest(CProcess* pProcess);
			void AddRequest(CProcess* pProcess);

			// misc
public:
	virtual void TraceRequests();

protected:
	virtual void Run();
			void CheckClientTimeout();
			void CheckProcessRequests();
			void CheckRequests();
			int GetRCIndex(CSecID id);

private:
	CRequestCollectors** m_pRequestCollectors;
	int					 m_iSize;
};

#endif // !defined(AFX_PROCESSSCHEDULERAUDIO_H__3B9526FD_1B32_4B7D_886C_AD229573733A__INCLUDED_)
