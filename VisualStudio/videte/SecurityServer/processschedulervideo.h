/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: processschedulervideo.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/processschedulervideo.h $
// CHECKIN:		$Date: 26.04.06 14:35 $
// VERSION:		$Revision: 15 $
// LAST CHANGE:	$Modtime: 26.04.06 14:33 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef _CProcessSchedulerVideo_H
#define _CProcessSchedulerVideo_H

#include "ProcessSchedulerVideoBase.h"

#include "ProcessMacro.h"
#include "Activation.h"

	#include "BackupProcess.h"
	#include "CallProcess.h"
	#include "RecordingProcess.h"
		#include "MDProcess.h"
	#include "MultipleCallProcess.h"
	#include "ActualImageProcess.h"
	#include "GuardTourProcess.h"

#include "CipcOutputServer.h"
#include "IPCServerToVision.h"

class CRequestCollectors;

class CProcessSchedulerVideo : public CProcessSchedulerVideoBase
{
	// construction/destruction
public:
	//
	CProcessSchedulerVideo(CIPCOutputServer *pCipc);
	virtual ~CProcessSchedulerVideo();

	// attributed
public:
	virtual	CIPC* GetParentCIPC();
	virtual int		GetMaxFps();
	virtual int		GetMaxNumCameraSwitches();

	// operations
public:
	//

	// processes
public:
	//
	virtual BOOL StartNewProcess(CInput* pInput, 
								 CActivation& record,
								 BOOL& bRefThereWasAProcess,
								 CProcess*& pProcess,
								 int iNumAlreadyRecorded = -1);

	// answers from unit
public:
	// this is called from the CIPC connection
	virtual void OnIndicateVideo(const CIPCPictureRecord& pict,
								 DWORD dwMDX,
								 DWORD dwMDY,
								 DWORD dwUserData);
	virtual void OnUnitConfirmReset();
	virtual void OnUnitDisconnect();
	virtual void OnVideoSignal(CSecID id, int iErrorCode);

protected:
	virtual	void TerminateAllProcesses();
	virtual void Run();

private:

	//
	//
	void CalcEncodingRequests();
	void DistributeEncodingRequests();
	void OptimizeEncodingRequests();
	void SendEncodingRequests();
	void DoEncodeRequest(CProcess *pProcess, int iNumPics, int &iNumPossible);

	//
	BOOL HasSingleClientEncodeProcess() const;
	void DoSyncMsg();
	void SendSync(BOOL bDidRequest);
	//


public:
	// Guard tour stuff
	BOOL RemoveGuardTourProcess(CGuardTourProcess* pProcess);
	CIPCServerToVision* GetCIPCServerToVision(CSecID idHost, BOOL& bCreated);
	void DeleteDisconnectedCIPCServerToVision();

	// data
public:
	// debug options:
	static int  m_iTraceH263Data;
	static BOOL m_bTraceJpegData;
	static BOOL ms_bTraceDroppedRequest;
	static BOOL m_bTraceSync;
	//

	// data
private:
	//
	//
	BOOL	m_bSyncDone;
	HANDLE	m_hSyncSemaphore;
	DWORD	m_tickSyncSend;
	//
	int m_iNumOutstandingSyncs;
	int m_iDroppedProcesses;
	//
	//
	int m_iDeltaOffset;	// offsets virtual seconds versus real seconds

	//
	CompressionType m_CompressionType;

	// NEW 16.10.98
	// active Requests are the ones still to send
	// prior to sending they morph to m_pRunningRequests
	// actually it's a triple, OlRequests is still kept for pics, which arrive after the Sync
	CRequestCollectors *m_pActiveRequests;
	CRequestCollectors *m_pRunningRequests;
	CRequestCollectors *m_pOldRequests1;
	CRequestCollectors *m_pOldRequests2;
	CRequestCollectors *m_pOldRequests3;
	int m_iNumCameras;
	int m_iNumSwitches;
	int	m_iMaxFps;
	int	m_iMaxNumCameraSwitches;
	int m_bBothFields;

	// Guard Tour Process CIPCConnections To Vision
	CIPCServerToVisionArraySafe	m_CIPCServerToVisions;

	friend class CRequestCollectors;
};
////////////////////////////////////////////////////////////////
#endif
