/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: processschedulervideo.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/processschedulervideo.cpp $
// CHECKIN:		$Date: 4.05.06 14:14 $
// VERSION:		$Revision: 45 $
// LAST CHANGE:	$Modtime: 4.05.06 14:14 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"

#include "ProcessSchedulerVideo.h"
#include "ProcessMacro.h"
#include "ProcessListView.h"
#include "CIPCOutputServer.h"
#include "OutputList.h"
#include "CIPCOutputServerClient.h"
#include "CIPCDatabaseClientServer.h"

#include "sec3.h"
#include "input.h"
#include "RequestCollectors.h"

#include "ClientEncodingProcess.h"
#include "ClientOverlayProcess.h"
#include "ClientRelayProcess.h"
#include "GuardTourProcess.h"
#include "UploadProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define SECID_GROUP_MACROS (SECID_GROUP_PROCESS+123)

int CProcessSchedulerVideo::m_iTraceH263Data=0;	// 1 Intra frames, 2 all data
BOOL CProcessSchedulerVideo::m_bTraceJpegData=FALSE;	
BOOL CProcessSchedulerVideo::m_bTraceSync=FALSE;
//
BOOL CProcessSchedulerVideo::ms_bTraceDroppedRequest = FALSE;


static CTimedMessage timedMessageDatabaseActive(500);	// only used when TraceSavePicture is active!

/////////////////////////////////////////////
// PART CProcessSchedulerVideo
/////////////////////////////////////////////
/*@TOPIC{CProcessSchedulerVideo Overview|CProcessSchedulerVideo,Overview}
@RELATED @LINK{members|CProcessSchedulerVideo},
*/
/*
@MLIST @RELATED @LINK{CProcess}, @LINK{Notes|notes,CProcessSchedulerVideo}
*/
/*@TOPIC{CProcesScheduler notes|notes,CProcessSchedulerVideo}
@CW{m_pLocalVideoProcess} is a pseudo process, which
is @BW{NOT} kept in m_processes or the priority arrays.
Thus it only needs a plai WK_DELETE.

m_iNumClientProcess does not count m_pLocalVideoProcess.
*/
/*@MHEAD{constructor:}*/
/*@MD NYD */
CProcessSchedulerVideo::CProcessSchedulerVideo(CIPCOutputServer *pCipc)
: CProcessSchedulerVideoBase(pCipc)
{
	CWK_Profile wkp;

	m_sParentShm = pCipc->GetShmName();

	m_iNumCameras = 0;
	m_iNumSwitches  = 0;
	m_iMaxFps = 25;
	m_iMaxNumCameraSwitches = 16;
	m_bBothFields = FALSE;

	m_iNumStoringProcesses = 0;
	m_iDroppedProcesses = 0;
	m_iNumClientProcesses = 0;
	m_bShutdownInProgress=FALSE;
	m_dwLastGridTick=0;
	m_hSyncSemaphore=NULL;
	m_tickSyncSend=0;
	m_iNumOutstandingSyncs=0;

	m_pActiveRequests = NULL;
	m_pRunningRequests = NULL;
	m_pOldRequests1 = NULL;
	m_pOldRequests2 = NULL;
	m_pOldRequests3 = NULL;

	// CProcesses m_processes;
	// CProcesses m_priorityProcesses[NUM_PRIORITIES] array of arrays

	m_bParentIsMiCo  = FALSE;
	m_bParentIsCoCo  = FALSE;
	m_bParentIsSaVic = FALSE;
	m_bParentIsAudio = FALSE;
	m_bParentIsTasha = FALSE;
	m_CompressionType = COMPRESSION_UNKNOWN;

	if (   m_sParentShm==SM_COCO_OUTPUT_CAM0
		|| m_sParentShm==SM_COCO_OUTPUT_CAM1) 
	{
		m_bParentIsCoCo=TRUE;
	} 
	else if (   m_sParentShm==SM_MICO_OUTPUT_CAMERAS
			 || m_sParentShm==SM_MICO_OUTPUT_CAMERAS2
			 || m_sParentShm==SM_MICO_OUTPUT_CAMERAS3
			 || m_sParentShm==SM_MICO_OUTPUT_CAMERAS4
		) 
	{
		m_bParentIsMiCo=TRUE;
	} 
	else if (   m_sParentShm==SM_SAVIC_OUTPUT_CAMERAS
			 || m_sParentShm==SM_SAVIC_OUTPUT_CAMERAS2
			 || m_sParentShm==SM_SAVIC_OUTPUT_CAMERAS3
			 || m_sParentShm==SM_SAVIC_OUTPUT_CAMERAS4
		) 

	{
		m_bParentIsSaVic = TRUE;
	}

	if (   m_bParentIsCoCo
		|| m_bParentIsSaVic
		|| m_bParentIsMiCo
		) 
	{	
		// CreateEvent(securityAttr, bManualReset, bInitialState );
		CString sEventName;
		if (   m_sParentShm==SM_COCO_OUTPUT_CAM0
			|| m_sParentShm==SM_COCO_OUTPUT_CAM1) 
		{
			sEventName = _T("CoCoSyncEvent");
			m_iMaxNumCameraSwitches = 4;
		} 
		else if (   m_sParentShm==SM_MICO_OUTPUT_CAMERAS
			     || m_sParentShm==SM_MICO_OUTPUT_CAMERAS2
			     || m_sParentShm==SM_MICO_OUTPUT_CAMERAS3
			     || m_sParentShm==SM_MICO_OUTPUT_CAMERAS4
			) 
		{
			m_iMaxNumCameraSwitches = 16;
			m_iMaxFps = 25;
			m_bBothFields = FALSE;
			sEventName = _T("MiCoSyncEvent");
			if (m_sParentShm==SM_MICO_OUTPUT_CAMERAS)
			{
				m_bBothFields = wkp.GetInt(_T("JaCobUnit\\Device1\\General"),_T("StoreField"),0) == 2;
			}
			else if (m_sParentShm==SM_MICO_OUTPUT_CAMERAS2) 
			{
				sEventName += _T("2");
				m_bBothFields = wkp.GetInt(_T("JaCobUnit\\Device2\\General"),_T("StoreField"),0) == 2;
			} 
			else if (m_sParentShm==SM_MICO_OUTPUT_CAMERAS3) 
			{
				sEventName += _T("3");
				m_bBothFields = wkp.GetInt(_T("JaCobUnit\\Device3\\General"),_T("StoreField"),0) == 2;
			} 
			else if (m_sParentShm==SM_MICO_OUTPUT_CAMERAS4) 
			{
				sEventName += _T("4");
				m_bBothFields = wkp.GetInt(_T("JaCobUnit\\Device4\\General"),_T("StoreField"),0) == 2;
			}
			if (m_bBothFields)
			{
				m_iMaxNumCameraSwitches=(m_iMaxNumCameraSwitches*3/2);
				m_iMaxFps*=2;
			}
		}
		else if (   m_sParentShm==SM_SAVIC_OUTPUT_CAMERAS
			     || m_sParentShm==SM_SAVIC_OUTPUT_CAMERAS2
			     || m_sParentShm==SM_SAVIC_OUTPUT_CAMERAS3
			     || m_sParentShm==SM_SAVIC_OUTPUT_CAMERAS4
			) 
		{
			sEventName = _T("SaVicSyncEvent");
			m_iMaxNumCameraSwitches = 12;
			if (m_sParentShm == SM_SAVIC_OUTPUT_CAMERAS)
			{
				m_iMaxFps = wkp.GetInt(_T("SaVicUnit\\Device1\\FrontEnd"),_T("Fps"),m_iMaxFps);
			}
			else if (m_sParentShm==SM_SAVIC_OUTPUT_CAMERAS2) 
			{
				sEventName += _T("2");
				m_iMaxFps = wkp.GetInt(_T("SaVicUnit\\Device2\\FrontEnd"),_T("Fps"),m_iMaxFps);
			} 
			else if (m_sParentShm==SM_SAVIC_OUTPUT_CAMERAS3) 
			{
				sEventName += _T("3");
				m_iMaxFps = wkp.GetInt(_T("SaVicUnit\\Device3\\FrontEnd"),_T("Fps"),m_iMaxFps);
			} 
			else if (m_sParentShm==SM_SAVIC_OUTPUT_CAMERAS4) 
			{
				sEventName += _T("4");
				m_iMaxFps = wkp.GetInt(_T("SaVicUnit\\Device4\\FrontEnd"),_T("Fps"),m_iMaxFps);
			}
		}
		m_hSyncSemaphore = CreateSemaphore(NULL, 0, 100, sEventName);	// no manual reset
		if (m_hSyncSemaphore == NULL)
		{
			WK_TRACE_ERROR(_T("Failed to create sync event %s\n"),GetLastErrorString());
		}

		m_bSyncDone = TRUE;
	
		m_CompressionType = m_pCIPCOutputServer->GetCompressionType();
	}	// end of create SyncEvent

#ifdef TRACE_LOCKS
	int nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
	m_CIPCServerToVisions.m_nCurrentThread = nCurrentThread;
#endif
	StartThread();
}
/*@MD NYD */
CProcessSchedulerVideo::~CProcessSchedulerVideo()
{
	if (IsRunning())
	{
		if (!StopThread())
		{
			ExitProcess(0xEEEE);
		}
	}

	Lock(_T(__FUNCTION__));
	WK_DELETE(m_pActiveRequests);
	WK_DELETE(m_pRunningRequests);
	WK_DELETE(m_pOldRequests1);
	WK_DELETE(m_pOldRequests2);
	WK_DELETE(m_pOldRequests3);
	Unlock();

	WK_CLOSE_HANDLE(m_hSyncSemaphore);
}
/////////////////////////////////////////////////////////////////////////
CIPC* CProcessSchedulerVideo::GetParentCIPC()
{
	return m_pCIPCOutputServer;
}
/////////////////////////////////////////////////////////////////////////
int CProcessSchedulerVideo::GetMaxFps()
{
	return m_iMaxFps;
}
/////////////////////////////////////////////////////////////////////////
int	CProcessSchedulerVideo::GetMaxNumCameraSwitches()
{
	return m_iMaxNumCameraSwitches;
}
/////////////////////////////////////////////////////////////////////////
void CProcessSchedulerVideo::DoSyncMsg()
{
	m_csCurrentTime.Lock();
	DWORD dwDeltaSync = m_dwCurrentTime-m_tickSyncSend;
	m_csCurrentTime.Unlock();
	
	if (m_bSyncDone==FALSE) 
	{
		if (m_bTraceSync || dwDeltaSync>2500) 
		{
			if (m_bTraceSync==FALSE && HasSingleClientEncodeProcess()) 
			{ 
				// don't trace long syncs from single clients,
				// only if directly requested
			} 
			else 
			{
				CString sSyncMsg;
				// direct trace request or not a single client process
				m_csCurrentTime.Lock();
				if (m_dwCurrentTime< m_tickSyncSend) 
				{
					sSyncMsg = _T("SmallSyncEvent|");
				} 
				else 
				{
					sSyncMsg = _T("SyncEvent|");
				}
				m_csCurrentTime.Unlock();
				sSyncMsg += m_sParentShm;
				WK_STAT_LOG(_T("Process"),dwDeltaSync,sSyncMsg);

			}
		}
	}
	
	m_bSyncDone=TRUE;	// reset
}


// called from SchedulerThreadFunction
//
/*@MD NYD */
void CProcessSchedulerVideo::Run()
{
	if (m_bShutdownInProgress)
	{
		TerminateAllProcesses();
		return;
	}

	if (PreWaitSecondGrid())
	{
		return;
	}

	// and now a special wait, not a simple timeout of 2500, 
	// other conditions might change as well
	int nWait=0;

	int iMaxWait=25;	// default 25 * 20 miliseconds max

	// NOTE outputDummy has no sync handle
	if (m_hSyncSemaphore) 
	{
		// Beim ersten Aufruf gibt es noch keine
		// Solange noch Syncs ausstehen auf diese warten
		while (   (m_iNumOutstandingSyncs > 0)
			   && IsRunning()
			   && m_pCIPCOutputServer->GetIPCState()==CIPC_STATE_CONNECTED
			   && nWait < iMaxWait	// 25 * 20 ms --> 1.0 seconds
			) 
		{
			DWORD dwRet = WaitForSingleObject(m_hSyncSemaphore,20);
			if (dwRet==WAIT_OBJECT_0) 
			{
				if (m_bTraceSync) 
				{
					// WK_STAT_PEAK(_T("Process"),1,_T("Sync|Received"));
				}
				m_iNumOutstandingSyncs--;
				if (theApp.m_bTraceVeryOld)
				{
					TRACE(_T("RECEIVE SYNC %04x\n"),m_pCIPCOutputServer->GetGroupID());
					TRACE(_T("\n"));
				}
//				WK_TRACE(_T(">>>>ReceiveSync Outstanding Syncs=%d (%lu)\n"), m_iNumOutstandingSyncs, WK_GetTickCount());
				// if there is long P-sequence, now it's over
				// either normal via outstandingPics==0 or terminated
				// via 0 job request
				if (m_iNumLongPSequences) 
				{
					m_iNumLongPSequences--;
					TRACE(_T("m_iNumLongPSequences=%d\n"),m_iNumLongPSequences);
				}
			} 
			else if (dwRet==WAIT_TIMEOUT) 
			{
				nWait++;
			}
			else
			{
				WK_TRACE_ERROR(_T("Wait Error for Sync Semaphore\n"));
			}
		}
		if (nWait == iMaxWait)
		{
			WK_TRACE(_T("time out waiting for sync %d\n"),WK_GetTimeSpan(m_dwCurrentTime));
		}

		// refesh time after the wait for sync!
		m_csCurrentTime.Lock();
		m_dwCurrentTime = WK_GetTickCount();
		m_csCurrentTime.Unlock();

	} 
	else 
	{	// no m_hSyncSemaphore, assume okay
		m_bSyncDone = TRUE;
	}

	DoSyncMsg();
	PostWaitSecondGrid();
	
	Lock(_T(__FUNCTION__));	

	// check for valid/connected unit
	if (   m_pCIPCOutputServer
		&& m_pCIPCOutputServer!=theApp.GetOutputDummy()
		&& m_pCIPCOutputServer->GetIPCState()!=CIPC_STATE_CONNECTED
		&& m_processes.GetSize() ) 
	{
		WK_TRACE_ERROR(_T("Cancel processes, unit %s disappeared!\n"),m_pCIPCOutputServer->GetShmName());
		TerminateAllProcesses();
		Unlock();
		return;
	}

	CProcessScheduler::Run();
	
	// ML Den nächsten Videojob erst abschicken, wenn alle Syncs erhalten.
	if (   (m_processes.GetSize() > 0)
		&& (m_iNumOutstandingSyncs == 0)) 
	{
		if (   m_CompressionType == COMPRESSION_H263
			|| m_CompressionType == COMPRESSION_JPEG
			|| m_CompressionType == COMPRESSION_YUV_422) 
		{
			CalcEncodingRequests();
			// DistributeEncodingRequests();
			OptimizeEncodingRequests();
			SendEncodingRequests();
		} 
		else 
		{
			// OOPS no valid compression
		}
	} 
	else
	{
		// NOT YET avoid each time!
		if (   m_pLocalVideoProcess 
			&& theApp.GetServerControl()->CanDoOverlay(m_pLocalVideoProcess->GetClientID())
			) 
		{
			theApp.GetServerControl()->SetOverlayClient(m_pLocalVideoProcess->GetClientID(),
														m_pCIPCOutputServer->GetGroupID());
			
			m_pCIPCOutputServer->DoRequestLocalVideo(m_pLocalVideoProcess->GetCamID(),
													 RESOLUTION_2CIF,
													 m_pLocalVideoProcess->GetOverlayRect(),
													 TRUE);
		}
	}

	Unlock();	

	DeleteDisconnectedCIPCServerToVision();
	m_dwLastGridTick = m_dwCurrentTime;
}	// end of ::Run
/*@MD NYD */
void CProcessSchedulerVideo::TerminateAllProcesses()
{
	CProcessScheduler::TerminateAllProcesses();
	WK_DELETE(m_pLocalVideoProcess);
}



/*@MD NYD 
@ARGS
@ITEM pInput
@ITEM @CW{bRefThereWasAProcess}, set if there was a process created, but then deleted (by timer
or whatever). This allows the Input, to adjust its keepAlive counter, even if NULL is returned
The process will call RemoveProcess, which decrements the counter in the input.

Only if m_pCIPCOutputServer is connected, exception theApp.GetOutputDummy(), because that one is
never connected.
*/
BOOL CProcessSchedulerVideo::StartNewProcess(CInput *pInput, 
												  CActivation &activation, 
												  BOOL &bRefThereWasAProcess,
												  CProcess*& pProcess,
												  int iNumAlreadyRecorded /*= -1*/)
{
	BOOL bHandled = FALSE;
	bRefThereWasAProcess = FALSE;

	if (!CanStartNewProcess()) 
	{
		return TRUE;
	}

	pProcess = NULL;

	// first call base class for common processes
	bHandled = CProcessSchedulerVideoBase::StartNewProcess(pInput,activation,bRefThereWasAProcess,pProcess, iNumAlreadyRecorded);

	if (!bHandled)
	{
		const CProcessMacro &processMacro = activation.GetMacro();
		Lock(_T(__FUNCTION__));
		switch (processMacro.GetType())
		{
		case PMT_RECORD:
			if (CheckProcessForTimer(activation)) 
			{
				// create...
				if (processMacro.GetID() == SECID_PROCESS_CAMERA_MD_OK)
				{
					pProcess = new CMDProcess(this,&activation,m_CompressionType);
				}
				else
				{
					pProcess = new CRecordingProcess(this,&activation,m_CompressionType,iNumAlreadyRecorded);
				}

				bRefThereWasAProcess=TRUE;
				// add to 'container' and the according priority array
				AddProcess(pProcess);
				m_iNumStoringProcesses++;
			} 
			bHandled = TRUE;
			break;
		case PMT_UPLOAD:
			if (CheckProcessForTimer(activation)) 
			{
				// create...
				pProcess = new CUploadProcess(this,&activation,m_CompressionType,iNumAlreadyRecorded);
				bRefThereWasAProcess=TRUE;
				AddProcess(pProcess);
			} 
			bHandled = TRUE;
			break;
		case PMT_ALARM_CALL:
			{
				// check preconditions
				BOOL bDoCallProcess=TRUE;
				if (pInput->IsActive()==FALSE) 
				{
					WK_TRACE_ERROR(_T("Not supported: Alarm Call process in OK state\n"));
					bDoCallProcess = FALSE;
				}
				else if (pInput->IsInAlarmOffSpan())
				{
					WK_TRACE(_T("Alarm Call deactivated by special time span\n"));
					bDoCallProcess = FALSE;
				}
				else if (activation.GetMacro().GetIDs().GetSize() == 0)
				{
					WK_TRACE(_T("Alarm Call deactivated no host defined\n"));
					bDoCallProcess = FALSE;
				}

				if (bDoCallProcess) 
				{
					if (   activation.GetInput() 
						&& activation.GetActiveProcess()) 
					{
						WK_TRACE_ERROR(_T("alarm call still have process in link!?\n"));
					}
					else
					{
						if (CheckProcessForTimer(activation)) 
						{
							// create...
							pProcess = new CCallProcess(this,&activation,m_CompressionType);
							bRefThereWasAProcess=TRUE;
							AddProcess(pProcess);
						} 
					}
				} 
				else 
				{
					// invalid combination or no host or already dialing
					// see above
				}
			} 
			bHandled = TRUE;
			break;
		case PMT_BACKUP: 
			if (CheckProcessForTimer(activation)) 
			{
				// start a backup process
				// check preconditions
				pProcess = new CBackupProcess(this,&activation);
				bRefThereWasAProcess=TRUE;
				AddProcess(pProcess);
			} 
			bHandled = TRUE;
			break;
		case PMT_ACTUAL_IMAGE:
			if (CheckProcessForTimer(activation)) 
			{
				pProcess = new CActualImageProcess(this,&activation);
				bRefThereWasAProcess=TRUE;
				AddProcess(pProcess);
			} 
			bHandled = TRUE;
			break;
		case PMT_IMAGE_CALL:
			if (CheckProcessForTimer(activation)) 
			{
				pProcess = new CMultipleCallProcess(this,&activation,SPT_IMAGE_CALL);
				bRefThereWasAProcess=TRUE;
				AddProcess(pProcess);
			} 
			bHandled = TRUE;
			break;
		case PMT_CHECK_CALL:
			if (CheckProcessForTimer(activation)) 
			{
				pProcess = new CMultipleCallProcess(this,&activation,SPT_CHECK_CALL);
				bRefThereWasAProcess=TRUE;
				AddProcess(pProcess);
			} 
			bHandled = TRUE;
			break;
		case PMT_GUARD_TOUR:
			if (CheckProcessForTimer(activation)) 
			{
				pProcess = new CGuardTourProcess(this,&activation);
				bRefThereWasAProcess=TRUE;
				AddProcess(pProcess);
			} 
			bHandled = TRUE;
			break;
		default:
			WK_TRACE_ERROR(_T("Could not start process type %d for %s input %s\n"),processMacro.GetType(),
				processMacro.GetName(),
				pInput->GetName()
				);
			WK_TRACE_ERROR(_T("Invalid type or non matched hardware\n"));
			break;
		}
		Unlock();
	}
	return bHandled;
}

/*@MD NYD */
void CProcessSchedulerVideo::DoEncodeRequest(CProcess *pProcess, 
										int iNumPics,
										int &iNumPossible
										)
{
//	ASSERT(pProcess->GetState()==SPS_ACTIVE_RECORDING);
	ASSERT(m_pActiveRequests);

	pProcess->StatLog(-4);	// show record peak (if CProcess traceLevel is enabled)
	pProcess->m_iNumPicturesRequested += iNumPics;

	if (   ParentIsMiCo()
		|| ParentIsSaVic()) 
	{
		int iNumPicsUsed = m_pActiveRequests->AddNewRequest(pProcess,iNumPics);
		iNumPossible -= iNumPicsUsed;
	} 
	else if (ParentIsAudio())
	{
		// TODO AUDIO
	}
	else
	{
		WK_TRACE_ERROR(_T("unknown unit type\n"));
	}
	
	if (m_bTraceEncodeRequests) {
		CString sMsg;
		sMsg.Format(_T("EncodeRequest|%x"),pProcess->GetCamID().GetID());
		WK_STAT_PEAK(_T("Requests"),iNumPics,sMsg);
	}
	// long slice ? so change to idle state
	if (pProcess->GetMacro().GetRecordSlice() > 1000) {
			pProcess->SetState(SPS_ACTIVE_IDLE,m_dwCurrentTime);
	}
}

void CProcessSchedulerVideo::SendSync(BOOL bDidRequest)
{
	// sync mechanism
	// NOT YET take care of nested syncs when a timeout occured
	// NEW 23.11.98, send sync request first, BEFORE the jobs are sent
	if (m_hSyncSemaphore && bDidRequest) 
	{
		m_bSyncDone = FALSE;
		m_tickSyncSend = WK_GetTickCount();
		if (m_bTraceSync) 
		{
			// WK_STAT_PEAK(_T("Process"),1,_T("Sync|Send"));
		}
		m_iNumOutstandingSyncs++;
//		WK_TRACE(_T(">>>>SendSync TickSend=%lu) %d\n"), m_tickSyncSend,m_iNumOutstandingSyncs);
		// Vordrängeln mit 1234
		if (theApp.m_bTraceVeryOld)
		{
			TRACE(_T("SEND SYNC %04x\n"),m_pCIPCOutputServer->GetGroupID());
		}
		m_pCIPCOutputServer->DoRequestSync(m_tickSyncSend,1234,m_iNumOutstandingSyncs);
	}
}

int GetReducedFPS(int iPriority, int iFPS, int iIteration, BOOL bUseBothFields)
{
	static int iReductionP1 [25][5] = { {25,12,6,3,2}, {24,12,6,3,2}, {23,12,6,3,2}, {22,12,6,3,2}, {21,12,6,3,2}, {20,8,5,3,2}, {19,8,5,3,2}, {18,8,5,3,2}, {17,8,5,3,2}, {16,8,4,3,2}, {15,6,4,3,2}, {14,6,4,3,2}, {13,6,4,2,1}, {12,6,4,2,1}, {11,5,4,3,2},{10,5,3,2,1},{9,4,3,2,1},{8,4,3,2,1},{7,4,3,2,1},{6,4,3,2,1},{5,4,3,2,1},{4,4,3,2,1},{3,3,2,1,1},{2,2,2,1,1},{1,1,1,1,1},};
	static int iReductionP2 [25][5] = { {25,12,6,3,2}, {24,12,6,3,2}, {23,12,6,3,2}, {22,12,6,3,2}, {21,12,6,3,2}, {20,8,5,3,2}, {19,8,5,3,2}, {18,8,5,3,2}, {17,8,4,3,2}, {16,8,4,3,2}, {15,6,4,3,2}, {14,6,4,3,2}, {13,6,4,2,1}, {12,6,4,2,1}, {11,5,4,2,1},{10,5,3,2,1},{9,4,3,2,1},{8,4,3,2,1},{7,4,3,2,1},{6,4,3,2,1},{5,4,3,1,1},{4,4,2,1,1},{3,2,2,1,1},{2,2,2,1,1},{1,1,1,1,1},};
	static int iReductionP3 [25][5] = { {25,12,6,3,2}, {24,12,6,3,2}, {23,12,6,3,2}, {22,12,6,3,2}, {21,12,6,3,2}, {20,8,5,3,2}, {19,8,4,3,2}, {18,8,4,3,2}, {17,8,4,3,2}, {16,8,4,3,2}, {15,6,4,2,1}, {14,6,4,2,1}, {13,6,3,2,1}, {12,6,3,2,1}, {11,5,3,2,1},{10,5,3,2,1},{9,4,3,2,1},{8,4,3,2,1},{7,3,3,2,1},{6,4,3,2,1},{5,3,2,1,1},{4,3,2,1,1},{3,2,1,1,1},{2,2,1,1,1},{1,1,1,1,1},};
	static int iReductionP4 [25][5] = { {25,12,6,3,2}, {24,12,6,3,2}, {23,12,6,3,2}, {22,12,6,3,2}, {21,12,6,3,2}, {20,6,4,3,2}, {19,6,4,3,2}, {18,8,4,3,2}, {17,8,4,3,2}, {16,8,4,3,1}, {15,6,3,2,1}, {14,6,3,2,1}, {13,6,3,2,1}, {12,6,3,2,1}, {11,5,3,2,1},{10,5,3,2,1},{9,4,3,2,1},{8,4,3,2,1},{7,3,2,1,1},{6,4,2,1,1},{5,3,2,1,1},{4,3,2,1,1},{3,2,1,1,1},{2,1,1,1,1},{1,1,1,1,1},};
	static int iReductionP5 [25][5] = { {25,12,6,3,2}, {24,12,6,3,2}, {23,12,6,3,2}, {22,12,6,3,2}, {21,12,6,3,2}, {20,6,4,3,2}, {19,6,4,3,2}, {18,8,4,3,2}, {17,8,4,3,2}, {16,8,4,3,1}, {15,6,3,2,1}, {14,6,3,2,1}, {13,6,3,2,1}, {12,6,3,2,1}, {11,5,3,2,1},{10,5,3,2,1},{9,4,3,2,1},{8,4,3,2,1},{7,3,2,1,1},{6,2,1,1,1},{5,2,1,1,1},{4,2,1,1,1},{3,1,1,1,1},{2,1,1,1,1},{1,1,1,1,1},};

	if (bUseBothFields)
	{
		iFPS /= 2;
		if (iFPS==0)
		{
			iFPS = 1;
		}
	}

	int iRet = 1;
	switch (iPriority)
	{
	case 0:
		iRet = iReductionP1[25-iFPS][iIteration];
		break;
	case 2:
		iRet = iReductionP2[25-iFPS][iIteration];
		break;
	case 3:
		iRet = iReductionP3[25-iFPS][iIteration];
		break;
	case 4:
		iRet = iReductionP4[25-iFPS][iIteration];
		break;
	case 5:
		iRet = iReductionP5[25-iFPS][iIteration];
		break;
	}
	if (bUseBothFields)
	{
		iRet *= 2;
	}
	return iRet;
}

void CProcessSchedulerVideo::CalcEncodingRequests()
{
	if (   ParentIsAudio()
		|| m_CompressionType == COMPRESSION_H263
		|| m_CompressionType == COMPRESSION_MPEG4
		|| m_pCIPCOutputServer == NULL)
	{
		TRACE(_T("no DistributeEncodingRequests\n"));
		return;
	}

	int iNrOfCameras = 0;
	int iNrOfFrames = 0;
	int iNrOfRequests = 0;
	int* pNrOfFrames = new int[m_pCIPCOutputServer->GetSize()];
	ZeroMemory(pNrOfFrames,m_pCIPCOutputServer->GetSize()*sizeof(int));

	for (int i=0;i<m_processes.GetSize();i++)
	{
		CProcess* pProcess = m_processes.GetAtFast(i);
		if (pProcess->IsActiveRecordingProcess())
		{
			pNrOfFrames[pProcess->GetCamID().GetSubID()] += pProcess->GetMacro().GetPicsPerSlice();
			iNrOfFrames += pProcess->GetMacro().GetPicsPerSlice();
			iNrOfRequests++;
		}
	}

//	CString sTraceFrames;

	for (int i=0;i<m_pCIPCOutputServer->GetSize();i++)
	{
		if (pNrOfFrames[i]>0)
		{
			iNrOfCameras++;
		}
/*
		if (CRequestCollectors::ms_bTraceRequestOptimization)
		{
			CString s;
			s.Format(_T("(%d,%d),"),i,pNrOfFrames[i]);
			sTraceFrames += s;
		}
*/
	}

	int iNrOfFramesMaxAll = GetMaxFps();
	int iNrOfFramesMaxSingle = iNrOfFramesMaxAll;

	if (iNrOfCameras > 1)
	{
		iNrOfFramesMaxAll = GetMaxNumCameraSwitches() - m_pCIPCOutputServer->GetNrOfActiveMDDetectors()/2;
		iNrOfFramesMaxSingle = iNrOfFramesMaxAll/2;
	}

	m_iNumCameras = iNrOfCameras;
	iNrOfFramesMaxAll = max(iNrOfFramesMaxAll,iNrOfCameras);
	m_iNumSwitches = iNrOfFramesMaxAll;


/*
	if (CRequestCollectors::ms_bTraceRequestOptimization)
	{
		TRACE(_T("NrOfCameras=%d, NrOfFrames=%d, NrOfRequests=%d,NrOfFramesMaxAll=%d\n"),
			iNrOfCameras,iNrOfFrames,iNrOfRequests,iNrOfFramesMaxAll);
		TRACE(_T("NrOfFrames %s\n"),sTraceFrames);
	}
*/

	int iNumPossible = iNrOfFramesMaxAll;
	int iNumProcessesDone = 0;
	BOOL bDone = FALSE;
	int iIteration = 0;

	while (   !bDone 
		   && iIteration<5)
	{
		iNumProcessesDone = 0;
		iNumPossible = iNrOfFramesMaxAll;

//		TRACE(_T("iIteration=%d\n"),iIteration);

		WK_DELETE(m_pActiveRequests);
		m_pActiveRequests = new CRequestCollectors(m_pCIPCOutputServer);

		while (   iNumPossible>0 
			   && iNumProcessesDone<iNrOfRequests) 
		{
			for (int iPriority=0;iPriority<NUM_PRIORITIES && iNumPossible>0;iPriority++)
			{
				for (i=0;i<m_priorityProcesses[iPriority].GetSize() && iNumPossible>0;i++) 
				{
					CProcess *pProcess = m_priorityProcesses[iPriority][i];
					if (pProcess->IsActiveRecordingProcess()) 
					{
						int iNumPictRequested = pProcess->GetMacro().GetPicsPerSlice();
						int iNumPictReduced = GetReducedFPS(iPriority,iNumPictRequested,iIteration,m_bBothFields);
						if (iNumPictReduced<iNumPictRequested)
						{
							TRACE(_T("reducing from %d->%d\n"),iNumPictRequested,iNumPictReduced);
							iNumPictRequested = iNumPictReduced;
						}

						iNumPictRequested = max(1,iNumPictRequested);
						iNumPictRequested = min(iNumPictRequested,iNrOfFramesMaxSingle);

						if (iNumPictRequested<=iNumPossible)
						{
							DoEncodeRequest(pProcess,iNumPictRequested,iNumPossible);
							iNumProcessesDone++;
						}
						else
						{
							iNumPossible = 0;
							break;
						}
					}
				}
			}
		}
		bDone = (iNumProcessesDone == iNrOfRequests);
		if (!bDone)
		{
			iIteration++;
		}
	}

	delete [] pNrOfFrames;

}


/*@MD NYD Also for H263! */
void CProcessSchedulerVideo::DistributeEncodingRequests()
{
	if (ParentIsAudio())
	{
		TRACE(_T("no DistributeEncodingRequests for audio\n"));
		return;
	}
	int i;
	
	int	  iPriority=0;	// start at highest priority
	int   iNumPossible = m_pCIPCOutputServer->GetFramesPerSecond();
	int	  iNumTotal = m_processes.GetSize();
	int   iNumProcessesDone=0;
	int   iNumWanted = 0;
	int   iNumProcesses = 0;
	BOOL  bDidRequest=FALSE;
	double dScale = 1.0;

	// special modification for H263 client processes
	// to get a long sequence of P-frames

	if (m_CompressionType==COMPRESSION_H263)
	{
		if (   m_iNumClientProcesses == 1
			&& m_iNumStoringProcesses == 0
			) 
		{
			if (m_iNumLongPSequences==0)
			{
				iNumPossible = CIPCOutputServerClient::m_iCoCoClientPicsPerSecond;
				m_iNumLongPSequences++;
				TRACE(_T("m_iNumLongPSequences=%d\n"),m_iNumLongPSequences);
			}
			else
			{
				return;
			}
		}
		else
		{
			for (i=0;i<m_processes.GetSize();i++)
			{
				CProcess* pProcess = m_processes[i];
				if (   pProcess
					&& (   pProcess->IsClientEncodingProcess()
						|| pProcess->IsActiveRecordingProcess()
						)
					)
				{
					if (pProcess->GetMacro().GetNumPictures()<=12)
						iNumWanted += pProcess->GetMacro().GetNumPictures();
					else
						iNumWanted += 12;
					iNumProcesses++;
				}
			}
			TRACE(_T("%d Wanted from %d processes\n"),iNumWanted,iNumProcesses);
			dScale = (double)iNumPossible / (double)(iNumWanted+iNumProcesses);
			if (dScale>1.0)
			{
				dScale = 1.0;
			}
			TRACE(_T("Scale is %2.2f %d switches\n"),dScale,iNumProcesses-1);
		}
	}

	BOOL bSingleCamera = TRUE;
	if (m_processes.GetSize())
	{
		CSecID idCamera = SECID_NO_ID;
		for (i=0;i<m_processes.GetSize() && bSingleCamera;i++)
		{
			CProcess* pProcess = m_processes[i];
			if (     pProcess
				&& (   pProcess->IsClientEncodingProcess()
				    || pProcess->IsActiveRecordingProcess()
				   )
				)
			{
				if (idCamera == SECID_NO_ID)
				{
					idCamera = pProcess->GetCamID();
				}
				else
				{
					bSingleCamera = pProcess->GetCamID() == idCamera;
				}
			}
		}
	}

	int iNumPicsWanted = 0;
	int iNumPictProcessesAll = 0;
	int iNumPictProcessesScheduled = 0;
	for (i=0;i<m_processes.GetSize();i++)
	{
		CProcess* pProcess = m_processes[i];
		if (   pProcess->IsActiveRecordingProcess()
			|| pProcess->IsClientEncodingProcess()) 
		{
			iNumPictProcessesAll++;
			iNumPicsWanted += pProcess->GetMacro().GetPicsPerSlice();
		}
	}
//	TRACE(_T("iNumPictProcessesAll=%d, iNumPicsWanted=%d\n"),iNumPictProcessesAll,iNumPicsWanted);

	// are there still record time to schedule and processes left
	while (   iNumPossible>0 
		   && iNumProcessesDone<iNumTotal 
		   && iPriority<NUM_PRIORITIES)
	{
		// create if not already done so
		// only used for mico/jacob
		if (m_pActiveRequests==NULL) 
		{
			m_pActiveRequests = new CRequestCollectors(m_pCIPCOutputServer);
		}

		int iNumActive=0;	// how many processes per level
		int iPicsWanted=0;	// sum of active processes
		// OOPS NOT YET int iNumSingles=0;	// processes that need only one picture
		for (i=0;i<m_priorityProcesses[iPriority].GetSize();i++) 
		{
			CProcess *pProcess = m_priorityProcesses[iPriority][i];
			iNumProcessesDone++;
			if (   pProcess->IsActiveRecordingProcess()
				|| pProcess->IsClientEncodingProcess()) 
			{
				iNumActive++;
				int d = min(1,(int)(pProcess->GetMacro().GetPicsPerSlice()*dScale));
				iPicsWanted += d;
			}
		}	// end of loop over processes per priorities
 									

		if (iNumActive) 
		{	
			// the hard part, schedule the requests...
			// splits into 'all fits' and 'real work'
//			TRACE(_T("iPicsWanted=%d, iNumPossible=%d\n"),iPicsWanted,iNumPossible);

			if (   iPicsWanted <= iNumPossible
				|| bSingleCamera) 
			{ 
				// all fits, that's simple
//				TRACE(_T("all fits, that's simple\n"));
				for (i=0;i<m_priorityProcesses[iPriority].GetSize();i++) 
				{
					CProcess *pProcess=m_priorityProcesses[iPriority][i];
					if (   pProcess->IsActiveRecordingProcess()
						|| pProcess->IsClientEncodingProcess()) 
					{
						int iNumPictRequested = (int)(pProcess->GetMacro().GetPicsPerSlice()*dScale);
						if (iNumPictRequested==0)
						{
							iNumPictRequested = 1;
						}
						DoEncodeRequest(pProcess,iNumPictRequested,iNumPossible);
						iNumPictProcessesScheduled++;
						bDidRequest=TRUE;
					}
				}

			// NOT YET check for single process, to make life easier
			} 
			else 
			{	
				// more than possible
				// NOT YET shift within recordSlice instead of cut down
				// slice the rest, min one picture
				TRACE(_T("more than possible\n"));
				DWORD dwMaxPossible = max(1,iNumPossible/iNumActive);
				// try all processes, but limit is iNumPossible
				for (i=0;iNumPossible>0 && i<m_priorityProcesses[iPriority].GetSize();i++) 
				{
					CProcess *pProcess=m_priorityProcesses[iPriority][i];
					if (   pProcess->IsActiveRecordingProcess()
						|| pProcess->IsClientEncodingProcess()) 
					{
						int iWanted=min(pProcess->GetMacro().GetPicsPerSlice(),dwMaxPossible);
						int iNumPictRequested = (int)(iWanted*dScale);
						if (iNumPictRequested == 0)
						{
							iNumPictRequested = 1;
						}
						DoEncodeRequest(pProcess,iNumPictRequested, iNumPossible);
						iNumPictProcessesScheduled++;
						bDidRequest=TRUE;
					}
				}
			}	// end of hard part
		} 
		else 
		{
			// no requests on this level
		}
		iPriority++;	// and next priority level
	}	// end of big loop over prioities

	if (ms_bTraceDroppedRequest)
	{
		int iDropped = iNumPictProcessesAll-iNumPictProcessesScheduled;
		if (iDropped != m_iDroppedProcesses)
		{
			m_iDroppedProcesses = iDropped;
			WK_STAT_LOG(_T("Process"),m_iDroppedProcesses,m_sParentShm);
		}
	}
}
void CProcessSchedulerVideo::OptimizeEncodingRequests()
{
	if (m_pActiveRequests)
	{
		m_pActiveRequests->OptimizeRequests(m_iNumCameras,m_iNumSwitches);
	}
}
void CProcessSchedulerVideo::SendEncodingRequests()
{
	// sync mechanism
	// NOT YET take care of nested syncs when a timeout occured
	// NEW 23.11.98, send sync request first, BEFORE the jobs are sent
	// SendSync(bDidRequest);

	if (   ParentIsMiCo()
		|| ParentIsSaVic()) 
	{
		// now drop the existing old request, they should already be answered
		// and take the newly calculated ones as the running requests
		Lock(_T(__FUNCTION__));
		WK_DELETE(m_pOldRequests3);	// these are realy done now
		m_pOldRequests3 = m_pOldRequests2;
		m_pOldRequests2 = m_pOldRequests1;
		m_pOldRequests1 = m_pRunningRequests;
		m_pRunningRequests = m_pActiveRequests;
		m_pActiveRequests = NULL;	// reset pointer
		if (m_pRunningRequests) 
		{
			m_pRunningRequests->SendRequestsToUnit(m_pCIPCOutputServer);
		}
		else
		{
			TRACE(_T("no running requests %s\n"),m_pCIPCOutputServer->GetShmName());
		}
		Unlock();
	}
	SendSync(TRUE); // ML Test 05.03.99
}
/*@MHEAD{pictures:}*/
/*@MD NYD */
void CProcessSchedulerVideo::OnIndicateVideo(const CIPCPictureRecord &pict,DWORD dwMDX,DWORD dwMDY,DWORD dwUserID)
{
	m_csLastPicture.Lock();
	m_dwLastPicture = WK_GetTickCount();
	m_csLastPicture.Unlock();

	CSecID pid(dwUserID);
	
	if (m_bTraceJpegData) 
	{
		CString sMsg;
		sMsg.Format(_T("JpegData|%x"),pict.GetCamID().GetID());
		WK_STAT_LOG(_T("Requests"),-1,sMsg);
	}
	
	BOOL bUsed=FALSE;
	Lock(_T(__FUNCTION__));
	
	// first try the current requests
	if (m_pRunningRequests) 
	{
		m_pRunningRequests->Lock(_T(__FUNCTION__));
		bUsed = m_pRunningRequests->OnIndicateVideo(pict,0,0,dwUserID);
		m_pRunningRequests->Unlock();
	}
	else
	{
		WK_TRACE_ERROR(_T("NO RUNNING REQUESTS\n"));
	}
	
	// if not found there try the old requests
	if (!bUsed && m_pOldRequests1) 
	{
		// this is not seldom because of some
		// by cipc delayed picture records
		m_pOldRequests1->Lock(_T(__FUNCTION__));
		bUsed = m_pOldRequests1->OnIndicateVideo(pict,0,0,dwUserID);
		m_pOldRequests1->Unlock();
	}
	
	// if not found there try the very old requests
	if (!bUsed && m_pOldRequests2) 
	{
		m_pOldRequests2->Lock(_T(__FUNCTION__));
		bUsed = m_pOldRequests2->OnIndicateVideo(pict,0,0,dwUserID);
		m_pOldRequests2->Unlock();
	}
	
	// if not found there try the very old requests
	if (!bUsed && m_pOldRequests3) 
	{
		m_pOldRequests3->Lock(_T(__FUNCTION__));
		bUsed = m_pOldRequests3->OnIndicateVideo(pict,0,0,dwUserID);
		m_pOldRequests3->Unlock();
	}

	if (!bUsed) 
	{
		// process identified by pid not found
		// might already have terminated
		
		WK_STAT_PEAK(_T("Process"),1,_T("PictDataUnused"));
		WK_TRACE(_T("PictDataUnused for id %08x %d Queue\n"),dwUserID,
			m_pCIPCOutputServer->GetReceiveQueueLength());
		//WK_TRACE(_T("%s had %d records in the receive queue\n"),m_pCIPCOutputServer->GetShmName(),m_pCIPCOutputServer->GetReceiveQueueLength());
		
		if (m_pOldRequests3) 
		{
			m_pOldRequests3->DumpIDs(_T("Oldests"));
		}
		else
		{
			WK_TRACE(_T("no Oldests\n"));
		}
		if (m_pOldRequests2) 
		{
			m_pOldRequests2->DumpIDs(_T("VeryOld"));
		}
		else
		{
			WK_TRACE(_T("no VERY OLDs\n"));
		}
		
		if (m_pOldRequests1) 
		{
			m_pOldRequests1->DumpIDs(_T("Old"));
		}
		else
		{
			WK_TRACE(_T("no OLDs\n"));
		}
		
		if (m_pRunningRequests) 
		{
			m_pRunningRequests->DumpIDs(_T("Running"));
		}
		else
		{
			WK_TRACE(_T("no RUNNINGs\n"));
		}
	}
	Unlock();
}
BOOL CProcessSchedulerVideo::HasSingleClientEncodeProcess() const
{
	BOOL bResult = FALSE;
	// if there are any processes and ONE client process
	// and the one client process is the only one one the highest
	// priority
	if (   m_processes.GetSize() 
		&& m_iNumClientProcesses==1) 
	{
		BOOL bDone=FALSE;
		// search top down over all priorities
		for (int i=0;bDone==FALSE && i<NUM_PRIORITIES;i++)	 
		{
			const CProcesses &onePriority = m_priorityProcesses[i];
			int iNum = onePriority.GetSize();
			switch (iNum) 
			{
				case 0:
					// search on
					break;
				case 1:
					{
						// check for client
						const CProcess *pProcess = onePriority[0];
						bResult = pProcess->IsClientEncodingProcess();
						bDone = TRUE;
					}
					break;
				default:
					// more than one
					bResult = FALSE;
					bDone = TRUE;
			}
		}
	}

	return bResult;
}


/*@MD */




/*@MD */


/////////////////////////////////////////////////////////////
void CProcessSchedulerVideo::OnUnitConfirmReset()
{
	TRACE(_T("OnUnitConfirmReset %s\n"),m_pCIPCOutputServer->GetShmName());
	m_iNumOutstandingSyncs = 0;
}
/////////////////////////////////////////////////////////////
void CProcessSchedulerVideo::OnUnitDisconnect()
{
	TRACE(_T("OnUnitDisconnect %s\n"),m_pCIPCOutputServer->GetShmName());
	m_iNumOutstandingSyncs = 0;
	CProcessSchedulerVideoBase::OnUnitDisconnect();
}
/////////////////////////////////////////////////////////////
// 0 nicht gefunden
// 1 gefunden und evtl gelöscht
BOOL CProcessSchedulerVideo::RemoveGuardTourProcess(CGuardTourProcess* pProcess)
{
	BOOL bRet = FALSE;
	m_CIPCServerToVisions.Lock(_T(__FUNCTION__));

	for (int i=m_CIPCServerToVisions.GetSize()-1;i>=0;i--) 
	{
		 CIPCServerToVision* pCIPCServerToVision = m_CIPCServerToVisions.GetAtFast(i);
		 if (pCIPCServerToVision->RemoveProcess(pProcess))
		 {
			 if (pCIPCServerToVision->GetNrOfProcesses() == 0)
			 {
				 if (pCIPCServerToVision->GetIPCState()!=CIPC_STATE_CONNECTED)
				 {
					 WK_TRACE(_T("+++++++++ deleting unconnected ServerControl %08lx %s\n"),
						      pCIPCServerToVision->GetDestination().GetID(),
							  NameOfEnum(pCIPCServerToVision->GetIPCState()));
				 }
				 WK_DELETE(pCIPCServerToVision);
				 m_CIPCServerToVisions.RemoveAt(i);
			 }
			 bRet = TRUE;
		 }
	}

	m_CIPCServerToVisions.Unlock();

	return bRet;
}
/////////////////////////////////////////////////////////////
CIPCServerToVision* CProcessSchedulerVideo::GetCIPCServerToVision(CSecID idHost, BOOL& bCreated)
{
	CIPCServerToVision* pCIPC = NULL;
	m_CIPCServerToVisions.Lock(_T(__FUNCTION__));

	for (int i=0;i<m_CIPCServerToVisions.GetSize();i++) 
	{
		 CIPCServerToVision* pCIPCServerToVision = m_CIPCServerToVisions.GetAtFast(i);
		 if (pCIPCServerToVision->GetDestination() == idHost)
		 {
			 pCIPC = pCIPCServerToVision;
			 bCreated = FALSE;
			 break;
		 }
	}

	if (pCIPC == NULL)
	{
		CString sShmName;
		sShmName.Format(_T("ServerToClient%08lx"),GetTickCount());
		pCIPC = new CIPCServerToVision(sShmName,idHost);
		m_CIPCServerToVisions.Add(pCIPC);
		bCreated = TRUE;
	}

	m_CIPCServerToVisions.Unlock();

	return pCIPC;
}
/////////////////////////////////////////////////////////////
void CProcessSchedulerVideo::DeleteDisconnectedCIPCServerToVision()
{
	m_CIPCServerToVisions.Lock(_T(__FUNCTION__));

	for (int i=m_CIPCServerToVisions.GetSize()-1;i>=0;i--) 
	{
		 CIPCServerToVision* pCIPCServerToVision = m_CIPCServerToVisions.GetAtFast(i);
		 if (pCIPCServerToVision->GetIsMarkedForDelete())
		 {
			 if (pCIPCServerToVision->GetNrOfProcesses())
			 {
				 pCIPCServerToVision->RemoveProcesses();
				 WK_TRACE_ERROR(_T("pCIPCServerToVision still processes inside %08lx\n"),pCIPCServerToVision);
			 }
			 WK_DELETE(pCIPCServerToVision);
			 m_CIPCServerToVisions.RemoveAt(i);
		 }
	}

	m_CIPCServerToVisions.Unlock();
}
/////////////////////////////////////////////////////////////
void CProcessSchedulerVideo::OnVideoSignal(CSecID id, int iErrorCode)
{
}
