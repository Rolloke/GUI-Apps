/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcOutputUnit.h $
// ARCHIVE:		$Archive: /Project/CIPC/CipcOutputUnit.h $
// CHECKIN:		$Date: 15.07.98 10:21 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 2.06.98 17:29 $
// BY AUTHOR:	$Author: Hedu $
// $Nokeywords:$


#ifndef _CIPCOutputUnit_h
#define _CIPCOutputUnit_h

#include "CIPCOutput.h"
#include "VideoJob.h"

class CIPCOutputUnit : public CIPCOutput
{
public:
	CIPCOutputUnit(const char *shmName, CompressionType ct,
						BOOL bJoinEncodeDeocdeJobs=TRUE
						);
	virtual ~CIPCOutputUnit();
	//
	inline void LockJobs();
	inline void UnlockJobs();
	// access:
	inline WORD GetGroupID() const;
	inline CompressionType GetCompressionType() const;
	// active jobs:
	inline CVideoJob * GetCurrentJob();
	inline CVideoJob * GetCurrentDecodeJob();
	//
	void ScheduleJobs(	BOOL bDoEncodeQueue=TRUE,
						BOOL bDoDecodeQueue=TRUE
						);
	void ClearAllQueues();
	//
	void DeleteCurrentJob();
	void DeleteCurrentDecodeJob();
	// timeouts;
	void SetEncoderIdleTimeout(DWORD dwTimeout);
	inline DWORD GetEncodeIdleTimeout() const;
protected:
	virtual void HandleEmptyJobQueue()=0;
	virtual void HandleEncodeJob()=0;
	virtual void HandleEncoderStop()=0;	// OBSOLETE ??? OOPS
	virtual void HandleDecodeJob()=0;
	virtual void HandleDecoderStop()=0;
	virtual void HandleLocalVideoJob()=0;
	// overridden cipc fns:
	virtual void OnRequestSync(DWORD dwTickSend,DWORD dwType, DWORD dwUser);
	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestNewJpegEncoding(CSecID camID, 
										Resolution res, 
										Compression comp,
										int iNumPictures,
										DWORD dwUserID
										);
	// Jpeg decoding
	void OnRequestJpegDecoding(WORD wGroupID,
								Resolution res,
								const CIPCExtraMemory &compressedData,
								DWORD dwUserData
						);
	// local video:
	void OnRequestLocalVideo(CSecID camID, 
										Resolution res,
										const CRect &rect,
										BOOL bActive
										);
private:
	CCriticalSection m_csJobs;
	WORD m_wGroupID;
	CompressionType m_compressionType;
	BOOL m_bJoinEncodeDecodeJobs;
	//
	CVideoJobs	m_JobQueue;	// encode or mix encde/decode
	CVideoJobs	m_decodeJobQueue;
	//
	CVideoJob *m_pCurrentEncodeJob;
	CVideoJob *m_pCurrentDecodeJob;
	//
	HANDLE m_hSyncEvent;
	//
	DWORD m_dwLastJobTime;
	BOOL m_bHadAtLeastOneJob;
	//
	CTimedMessage m_tmErrorMessage;
	//
	DWORD m_dwEncodeIdleTimeout;
	DWORD m_dwSleepAfterSync;
};

inline WORD CIPCOutputUnit::GetGroupID() const
{
	return m_wGroupID;
}
inline CompressionType CIPCOutputUnit::GetCompressionType() const
{
	return m_compressionType;
}

inline void CIPCOutputUnit::LockJobs()
{
	m_csJobs.Lock();
}

inline void CIPCOutputUnit::UnlockJobs()
{
	m_csJobs.Unlock();
}

inline DWORD CIPCOutputUnit::GetEncodeIdleTimeout() const
{
	return m_dwEncodeIdleTimeout;
}


inline CVideoJob * CIPCOutputUnit::GetCurrentJob()
{
	LockJobs();
	CVideoJob *pJob =  m_pCurrentEncodeJob;
	UnlockJobs();
	return pJob;
}

inline CVideoJob * CIPCOutputUnit::GetCurrentDecodeJob()
{
	LockJobs();
	CVideoJob *pJob =  m_pCurrentDecodeJob;
	UnlockJobs();
	return pJob;
}

#endif

