/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCOutputUnit.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCOutputUnit.cpp $
// CHECKIN:		$Date: 28.07.98 12:12 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 28.07.98 12:05 $
// BY AUTHOR:	$Author: Hedu $
// $Nokeywords:$

#include "stdcipc.h"

#include "CipcOutputUnit.h"
#include "WK_Names.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CIPCOutputUnit::CIPCOutputUnit(const char *shmName,
							   CompressionType ct,
							   BOOL bJoinEncodeDecodeJobs /*=TRUE*/
							   )
	: CIPCOutput(shmName, FALSE)	// as slave
{
	m_wGroupID = SECID_NO_GROUPID;
	m_compressionType = ct;
	m_bJoinEncodeDecodeJobs = bJoinEncodeDecodeJobs;
	//
	m_hSyncEvent = NULL;
	//
	m_pCurrentEncodeJob=NULL;
	m_pCurrentDecodeJob=NULL;
	//
	m_dwLastJobTime = GetTickCount();
	m_bHadAtLeastOneJob=FALSE;

	m_dwEncodeIdleTimeout = 30000;

	m_dwSleepAfterSync = 10;	// might increase server reactions

	CString sShmName(shmName);
	CString sEventName;
	if (sShmName==SM_COCO_OUTPUT_CAM0
		|| sShmName==SM_COCO_OUTPUT_CAM1) {
		sEventName = "CoCoSyncEvent";
	} else if (sShmName==SM_MICO_OUTPUT_CAMERAS) {
		sEventName = "MiCoSyncEvent";
	} else if (sShmName==SM_SWCODEC_OUTPUT) {
		sEventName = "SWCodecSyncEvent";
	} else {
		// no sync event
	}

	if (sShmName.GetLength()) {
		m_hSyncEvent = CreateEvent(NULL, TRUE, FALSE, sEventName);
		if (m_hSyncEvent) {
			ResetEvent(m_hSyncEvent);
		} else {
			WK_TRACE("Failed to create sync event\n");
		}
	}
}


CIPCOutputUnit::~CIPCOutputUnit()
{
	StopThread();
	ClearAllQueues();
}

void CIPCOutputUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(wGroupID);
}

void CIPCOutputUnit::OnRequestNewJpegEncoding(CSecID camID, 
										Resolution res, 
										Compression comp,
										int iNumPictures,
										DWORD dwUserID
										)
{
	CVideoJob *pJob;
	pJob = new CVideoJob(camID, res, comp, m_compressionType, dwUserID);
	pJob->m_pCipc = this;
	pJob->m_iOutstandingPics = iNumPictures;
	m_JobQueue.SafeAddTail(pJob);
}


void CIPCOutputUnit::OnRequestJpegDecoding(WORD wGroupID,
										Resolution res,
										const CIPCExtraMemory &compressedData,
										DWORD dwUserData
									)
{
	CVideoJob *pJob;
	pJob = new CVideoJob(&compressedData, dwUserData);
	pJob->m_resolution = res;
	pJob->m_pCipc = this;
	if (m_bJoinEncodeDecodeJobs) {
		m_JobQueue.SafeAddTail(pJob);
	} else {
		m_decodeJobQueue.SafeAddTail(pJob);
	}
}

void CIPCOutputUnit::OnRequestLocalVideo(CSecID camID, 
										Resolution res,
										const CRect &rect,
										BOOL bActive
										)
{
	CVideoJob *pJob;
	pJob = new CVideoJob(camID, res);
	pJob->m_pCipc = this;
	m_JobQueue.SafeAddTail(pJob);
}

void CIPCOutputUnit::OnRequestSync(DWORD dwTickSend,DWORD dwType, DWORD dwUser)
{
	CVideoJob *pJob;
	pJob = new CVideoJob(dwTickSend,dwType,dwUser);	// VJA_SYNC
	pJob->m_pCipc = this;

	m_JobQueue.SafeAddTail(pJob);
}


void CIPCOutputUnit::ScheduleJobs(
								BOOL bDoEncodeQueue,
								BOOL bDoDecodeQueue
								)
{
 	LockJobs();

	if (m_bJoinEncodeDecodeJobs) {
		// never touch decode Queue, if jon is active
		bDoDecodeQueue = FALSE;
	}

	if (bDoEncodeQueue && m_pCurrentEncodeJob == NULL) { 	// no current job ?
		// ############ Encoder Jobs
		m_pCurrentEncodeJob = m_JobQueue.SafeGetAndRemoveHead();
		if (m_pCurrentEncodeJob) {	// found a new job
			m_dwLastJobTime =  GetTickCount();
			m_bHadAtLeastOneJob = TRUE;

			switch (m_pCurrentEncodeJob->m_action) {
				case VJA_ENCODE:
					HandleEncodeJob();
					break;
				case VJA_STOP_ENCODE:
					HandleEncoderStop();	// OBSOLETE ??? OOPS
					break;
				case VJA_VIDEO:
					HandleLocalVideoJob();
					break;
				case VJA_SYNC:
					DeleteCurrentJob();	// job done 
					if (m_hSyncEvent) {
						SetEvent(m_hSyncEvent);
						Sleep(m_dwSleepAfterSync);
					} else {
						m_tmErrorMessage.Message("No Sync event !?\n");
					}
					break;
				// for mixed queues, decode jobs might also be here
				case VJA_DECODE:
					HandleDecodeJob();
					DeleteCurrentDecodeJob(); // job done
					break;
				case VJA_STOP_DECODE:
					HandleDecoderStop();
					DeleteCurrentDecodeJob(); // job done
					break;
				default:	
					m_tmErrorMessage.Message("ERROR:\tInvalid Encode job type %d\n",
												m_pCurrentEncodeJob->m_action);
					DeleteCurrentJob();	// job done 
			}	// end of job switch
		} else {	// no new job found
			if (m_bHadAtLeastOneJob) {
				DWORD dwCurrentTime= GetTickCount();
				if (dwCurrentTime  -  m_dwLastJobTime > m_dwEncodeIdleTimeout) {
						HandleEmptyJobQueue();
						WK_TRACE("JobQueue empty\n");
						m_bHadAtLeastOneJob = FALSE;
				}
				m_dwLastJobTime = dwCurrentTime;
			}					   
		}	
	}

	if (bDoDecodeQueue) {
	// ############ Decoder Jobs
		if (m_pCurrentDecodeJob == NULL) {
			m_pCurrentDecodeJob = m_decodeJobQueue.SafeGetAndRemoveHead();
			if (m_pCurrentDecodeJob) {
				switch (m_pCurrentDecodeJob->m_action)
				{
					case VJA_DECODE:
						HandleDecodeJob();
						DeleteCurrentDecodeJob(); // job done
						break;
					case VJA_STOP_DECODE:
						HandleDecoderStop();
						DeleteCurrentDecodeJob(); // job done
						break;
					default:
						m_tmErrorMessage.Message("ERROR:\tInvalid Decode job type %d\n",
														m_pCurrentDecodeJob->m_action);
						DeleteCurrentDecodeJob(); // job done
						break;
				}	// end of job switch
			}	// end of job found
		} else {
			// already have active decoder job
		}
	} else {
		// do not work on decode queue
	}
	UnlockJobs();
}


void CIPCOutputUnit::DeleteCurrentJob()
{
	LockJobs();
	WK_DELETE(m_pCurrentEncodeJob);
	UnlockJobs();
}
void CIPCOutputUnit::DeleteCurrentDecodeJob()
{
	LockJobs();
	WK_DELETE(m_pCurrentDecodeJob);
	UnlockJobs();
}
void CIPCOutputUnit::ClearAllQueues()
{
	LockJobs();
	m_JobQueue.SafeDeleteAll();
	m_decodeJobQueue.SafeDeleteAll();
	UnlockJobs();
}
