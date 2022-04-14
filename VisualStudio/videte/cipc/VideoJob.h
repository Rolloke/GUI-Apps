

#ifndef _CVideoJob_H
#define _CVideoJob_H

#include "wk.h"
#include "SecID.h"
#include "PictureDef.h"
#include "CipcOutput.h"
#include "CIPCExtraMemory.h"
#include "PictureRecord.h"


class CIPC;
class CIPCOutput;
class CIPCPictureRecord;

enum VideoJobAction {
	VJA_NONE,
	VJA_ENCODE,
	VJA_DECODE,
	VJA_VIDEO,
	VJA_STOP_ENCODE,
	VJA_STOP_DECODE,
	VJA_SYNC,
	VJA_IDLE
};

class AFX_EXT_CLASS CVideoJob 
{
public:
	// VJA_NONE
	inline CVideoJob();
	// VJA_STOP
	inline CVideoJob(CSecID camID, VideoJobAction stopAction);
	// VJA_ENCODE
	inline CVideoJob(CSecID camID, 
				Resolution res, 
				Compression comp, CompressionType ct,
				DWORD dwUserData=0,
				DWORD dwBitrateScale=0,
				DWORD dwRecordTime=0
				);	// CAVEAT set m_iOutstandingPics by hand, default is 1 <<<
	// VJA_ENCODE with next camID
	inline CVideoJob(
				CSecID camID, 
				CSecID nextCamID,
				Resolution res, 
				Compression comp, CompressionType ct,
				DWORD dwUserData=0,
				DWORD dwBitrateScale=0,
				DWORD dwRecordTime=0
				);	// CAVEAT set m_iOutstandingPics by hand, default is 1 <<<

	// VJA_DECODE
	inline CVideoJob(const CIPCExtraMemory *pEncodedSource, DWORD dwUserData);
	inline CVideoJob(const CRect &rect, const CIPCPictureRecord &pict,
						DWORD dwUserData);
	// VJA_VIDEO
	inline CVideoJob(CSecID camID, Resolution res );	// B&W
	inline CVideoJob(CSecID camID, const CRect & rect );	// color
	// VJA_SYNC
	inline CVideoJob(DWORD dwTick, DWORD dwType, DWORD dwUser);

	// VJA_IDLE
	inline CVideoJob(int iNumPics);

	inline virtual ~CVideoJob();
	//
	VideoJobAction m_action;
	//
	CSecID	m_camID;	// SECID_NO_ID, for decoding
	// DROPPED DWORD	m_dwStartTime;	// time job starting execution
	// DROPPED DWORD	m_dwStopTime;	// time job finished execution
	// DROPPED DWORD	m_dwSwitchTime;
	DWORD	m_dwUserData;
	//
	CIPCExtraMemory *m_pEncodedSource;
	CIPCPictureRecord *m_pSource;
	//
	Resolution		m_resolution;
	Compression		m_compression;
	CompressionType m_compressionType;
	CRect			m_overlayRect;
	//
	CIPCOutput		*m_pCipc;
	CIPCExtraMemory *m_pDestination;
	//
	//DWORD m_dwBitrate;  // OOPS ML
	 DWORD m_dwBitrateScale;
	
	// DROPPED DWORD m_dwRecordTime;
	//
	DWORD m_dwSyncTick,m_dwSyncType,m_dwSyncUser;
	int	m_iOutstandingPics;
	int	m_iNumIdlePics;
	CSecID m_camIDNext;
private:
	inline void Init();
};

typedef CVideoJob * CVideoJobPtr;

WK_PTR_LIST_CS(CVideoJobList, CVideoJobPtr, CVideoJobs)
WK_PTR_LIST_SEMA(CSemaVideoJobList, CVideoJobPtr, CSemaVideoJobs)

inline void CVideoJob::Init()
{
	m_action = VJA_NONE;
	// m_camID;	// SECID_NO_ID, for decoding
	// m_camIDNext;	// SECID_NO_ID

	// DROPPED m_dwStartTime = 0;
	// DROPPED m_dwStopTime = 0;
	// DROPPED m_dwSwitchTime=0;
	m_dwUserData=0;
	//
	m_pEncodedSource = NULL;
	m_pSource = NULL;
	//
	m_resolution = RESOLUTION_NONE;
	m_compression = COMPRESSION_NONE;
	m_compressionType = COMPRESSION_UNKNOWN;
	// m_overlayRect;
	m_pCipc = NULL;
	m_pDestination = NULL;
	//
//	m_dwBitrate=0;
	m_dwBitrateScale=0;
	// DROPPED m_dwRecordTime=0;
	//
	m_dwSyncTick= m_dwSyncType= m_dwSyncUser = 0;
	m_iOutstandingPics=1;
	m_iNumIdlePics = 0;
}

inline CVideoJob::CVideoJob()
{
	Init();
}
inline CVideoJob::CVideoJob(CSecID camID, VideoJobAction stopAction)
{
	Init();
	m_camID = camID;
	m_action = stopAction;
}

// VJA_ENCODE
inline CVideoJob::CVideoJob(CSecID camID, 
				Resolution res, 
				Compression comp, CompressionType ct,
				DWORD dwUserData,
				DWORD dwBitrate,
				DWORD /*dwRecordTime*/
				)
{
	Init();
	m_action = VJA_ENCODE;
	m_camID = camID;
	m_resolution = res;
	m_compression = comp;
	m_compressionType = ct;
	m_dwUserData = dwUserData;
//	m_dwBitrate = dwBitrate; // OOPS ML
	m_dwBitrateScale = dwBitrate;
	// DROPPED m_dwRecordTime = dwRecordTime;
}

// VJA_ENCODE
inline CVideoJob::CVideoJob(
				CSecID camID,
				CSecID camIDNext,
				Resolution res, 
				Compression comp, CompressionType ct,
				DWORD dwUserData,
				DWORD dwBitrate,
				DWORD /*dwRecordTime*/
				)
{
	Init();
	m_action = VJA_ENCODE;
	m_camID = camID;
	m_camIDNext = camIDNext;
	m_resolution = res;
	m_compression = comp;
	m_compressionType = ct;
	m_dwUserData = dwUserData;
//	m_dwBitrate = dwBitrate; // OOPS ML
	m_dwBitrateScale = dwBitrate;
	// DROPPED m_dwRecordTime = dwRecordTime;
}

// VJA_DECODE
inline CVideoJob::CVideoJob(const CIPCExtraMemory *pEncodedSource, DWORD dwUserData)
{
	Init();
	m_action = VJA_DECODE;
	m_pEncodedSource  = new CIPCExtraMemory(*pEncodedSource);
	m_dwUserData = dwUserData;
}
// VJA_DECODE
inline CVideoJob::CVideoJob(const CRect &rect, 
							const CIPCPictureRecord &pict,
							DWORD dwUserData)
{
	Init();
	m_action = VJA_DECODE;
	m_overlayRect = rect;
	m_pSource = new CIPCPictureRecord(pict);
	m_dwUserData = dwUserData;
}

// VJA_VIDEO
inline CVideoJob::CVideoJob(CSecID camID, Resolution res )	// B&W
{
	Init();
	m_action = VJA_VIDEO;
	m_camID = camID;
	m_resolution = res;
	m_compressionType = COMPRESSION_RGB_24;
}

inline CVideoJob::CVideoJob(CSecID camID, const CRect & rect)	// color
{
	Init();
	m_action = VJA_VIDEO;
	m_overlayRect = rect;
	m_camID = camID;
}

inline CVideoJob::CVideoJob(DWORD dwTick, DWORD /*dwType*/, DWORD /*dwUser*/)
{
	Init();
	m_action = VJA_SYNC;
	m_dwSyncTick = dwTick;

	// DROPPED m_dwSyncTick = dwTick;
	// DROPPED m_dwSyncType = dwType;
	// DROPPED m_dwSyncUser = dwUser;
	// DROPPED m_dwRecordTime = 0;	// make sure nobody make use of that time
}

inline CVideoJob::~CVideoJob()
{
	WK_DELETE(m_pEncodedSource);
	WK_DELETE(m_pSource);
}

inline CVideoJob::CVideoJob(int iNumPics)
{
	Init();
	m_action = VJA_IDLE;
	m_iNumIdlePics = iNumPics;
}
#endif
