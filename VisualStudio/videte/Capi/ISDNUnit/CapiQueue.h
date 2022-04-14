/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CapiQueue.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CapiQueue.h $
// CHECKIN:		$Date: 2.09.03 8:12 $
// VERSION:		$Revision: 20 $
// LAST CHANGE:	$Modtime: 1.09.03 8:49 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef GF_CapiQueue_H
#define GF_CapiQueue_H


#include "wk.h"
#include "SecID.h"
#include "Cipc.h"
#include "hdr.h"
#include "CapiQueueRecord.h"
#include "CIPCPipeISDN.h"	// for CIPCType

class CISDNConnection;



////////////////////////////////////////////////////
// forward declaration
class CIPCCmdRecord;
class CIPCExtraMemory;
class CCapiQueue;
class CCapi;

// const DWORD MAX_BLOCK_SIZE = CIPC_MAX_DATA;

// 1000 seems to give to performance
// TESTS FTP mit 2000 und 24576
// >>> 2000 <<<
// 10.04, 23:07:07:steps(20):Baudschnitt= 45487 [b/s]
// 10.04, 23:08:06:steps(20):Baudschnitt= 115974 [b/s]
// MIT 1000 
// 10.04, 23:10:36:steps(20):Baudschnitt= 75562 [b/s]
// 10.04, 23:12:04:steps(20):Baudschnitt= 113055 [b/s]
// >>> 500 <<<
// 10.04, 23:13:34:steps(20):Baudschnitt= 70861 [b/s]
// 10.04, 23:14:46:steps(20):Baudschnitt= 112768 [b/s]

const DWORD MAX_BLOCK_SIZE = 1000;

////////////////////////////////////////////////////
class CCapiQueue : public CObject {
public:
	 CCapiQueue(CCapi* pCapi);
	~CCapiQueue();
	//
	void Reset();
	//
	void SendData(const CISDNConnection *pConnection);
	//
	inline CCapiQueueRecord* GetReceivedCmd();
	// queue will take responsiblity to delete newRecord

	void AddToSendQueue(CCapiQueueRecord *pNewRecord, BOOL bRemoveOldest=FALSE);
	void PrependToSendQueue(CCapiQueueRecord *pNewRecord);
	void AddToReceiveQueue(const CIPCHDR* pCipcHdr);
	//
	inline static void SplitTypeFromService(BYTE &byService, CIPCType &ct);
	//
	inline int GetSendSize() const;
	//
	static BOOL m_bDoLogSend;
	static BOOL m_bDoLogReceive;
private:
	CCapi*					m_pCapi;
	// Send
	CCapiQueueRecords		m_sendQueue;
	CCapiQueueRecord*		m_pSendRecord;
	DWORD					m_dwSendIncreaseCounter;
	DWORD					m_dwSendBlockCounter;
	// Receive
	CCapiQueueRecords		m_receiveQueue;
	CCapiQueueRecord*		m_pReceiveRecord;
	DWORD					m_dwReceiveIncreaseCounter;
	// for bitrate log
	DWORD	m_dwLastSendTick;
	DWORD	m_dwSendStatBlockCount;
	DWORD	m_dwSendStat;	// accumulative data 'per second'
	// 

};


inline void CCapiQueue::SplitTypeFromService(BYTE &byService, CIPCType &ct)
{
	BOOL bIsServer = (byService & SERVER_BIT)!=0;
	BYTE bTypeBits = (BYTE)(byService & TYPE_MASK);

	byService &= ~SERVER_BIT;
	byService &= ~TYPE_MASK;

	switch (bTypeBits) 
	{
		case INPUT_BIT:
			if (bIsServer) 
			{
				ct = CIPC_INPUT_SERVER;
			} 
			else
			{
				ct = CIPC_INPUT_CLIENT;
			}
			break;
		case OUTPUT_BIT:
			if (bIsServer)
			{
				ct = CIPC_OUTPUT_SERVER;
			}
			else
			{
				ct = CIPC_OUTPUT_CLIENT;
			}
			break;
		case DATABASE_BIT:
			if (bIsServer)
			{
				ct = CIPC_DATABASE_SERVER;
			}
			else
			{
				ct = CIPC_DATABASE_CLIENT;
			}
			break;
		case ALARM_BIT:
			if (bIsServer)
			{
				ct = CIPC_ALARM_SERVER;
			}
			else
			{
				ct = CIPC_ALARM_CLIENT;
			}
			break;
		case AUDIO_BIT:
			if (bIsServer)
			{
				ct = CIPC_AUDIO_SERVER;
			}
			else
			{
				ct = CIPC_AUDIO_CLIENT;
			}
			break;
		default:
			WK_TRACE_ERROR(_T("Invalid typeBits %d\n"),bTypeBits);
	}
}

////////////////////////////////////////////////////
// application will take responsiblity to delete Record if no longer in use
inline CCapiQueueRecord* CCapiQueue::GetReceivedCmd()
{
	CCapiQueueRecord* pResult = NULL;
	pResult = m_receiveQueue.SafeGetAndRemoveHead();
	if (pResult) {
		m_dwReceiveIncreaseCounter--;
	}

	return pResult;
}
inline int CCapiQueue::GetSendSize() const
{
	return m_sendQueue.GetCount();
}

#endif



