/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CapiQueueRecord.h $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/CapiQueueRecord.h $
// CHECKIN:		$Date: 7.09.05 11:34 $
// VERSION:		$Revision: 19 $
// LAST CHANGE:	$Modtime: 7.09.05 11:31 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef GF_CapiQueueRecord_H
#define GF_CapiQueueRecord_H

#include "wk.h"
#include "SecID.h"
#include "Cipc.h"
#include "hdr.h"
#include "CIPCPipeISDN.h"

////////////////////////////////////////////////////
// forward declaration
class CIPCCmdRecord;
class CIPCExtraMemory;
class CCapiQueue;
class CCapi;

const BYTE TYPE_MASK=(BYTE)(7<<4);		// 0x60 : 0b0111:0000

const BYTE SERVER_BIT=128;				// 0x80 : 0b1000:0000
const BYTE INPUT_BIT=(BYTE)(0<<5);		// 0x00 : 0b0000:0000
const BYTE OUTPUT_BIT=(BYTE)(1<<5);		// 0x20 : 0b0010:0000
const BYTE DATABASE_BIT=(BYTE)(2<<5);	// 0x40 : 0b0100:0000
const BYTE ALARM_BIT=(BYTE)(3<<5);		// 0x60 : 0b0110:0000
const BYTE AUDIO_BIT=(BYTE)(1<<4);		// 0x10 : 0b0001:0000


class CCapiQueueRecord 
{
friend CCapiQueue;
public:

	// construtor form CIPC data
	CCapiQueueRecord();

	CCapiQueueRecord(const CIPCHDR* pCipcHdr);	// for received data
	CCapiQueueRecord(DWORD dwDataLen, const BYTE *pData);	// for raw data
	/*
	DROPPED
	CCapiQueueRecord(CSecID idFrom,
					CSecID idTo,
					BYTE byService,
					CIPCType cipcType
					);
					*/
	void FillFromCipcData(CSecID idFrom,
					CSecID idTo,
					CIPCType cipcType,
					DWORD dwCmd,
					DWORD dwParam1,
					DWORD dwParam2,
					DWORD dwParam3,
					DWORD dwParam4,
					const CIPCExtraMemory *pExtraMem,
					BOOL bCompressed = FALSE
					);
	void FillFromCmdRecord(CSecID idFrom,
					CSecID idTo,
					CIPCType cipcType,
					const CIPCCmdRecord &CmdRecord,
					BOOL bCompressed = FALSE
					);
	inline ~CCapiQueueRecord();

	void GetCipcCmd(BYTE& byCmdFrom,
					CSecID& idFrom,
					CSecID& idTo,
					DWORD& dwExtraMemoryLen,
					BYTE*& pExtraMem,
					DWORD& dwCmd,
					DWORD& dwParam1,
					DWORD& dwParam2,
					DWORD& dwParam3,
					DWORD& dwParam4
					);
	//
	BOOL inline IsAllSend() const;
	inline DWORD GetDataLength();
	//
	inline void IncrementSendPos(DWORD dwAdd);
	//
	// Get related data from friend CCapiQueue
	// DROPPED BOOL AddData(CIPCHDR* pCipcHdr);
	inline BOOL AllDataReceived();
	//
	void AppendData(const CIPCHDR* pCipcHdr);
	void FinalizeData(const CIPCHDR* pCipcHdr);
	//
	inline static void AddTypeToService(BYTE &byService, CIPCType ct);

private:
	inline void Init();

private:
	//
	static DWORD m_dwRecordCounter;
	static DWORD m_dwRecordCounterMax;

	// send related data for friend CCapiQueue
	CIPCHDR m_CipcHdr;
	BYTE *m_pData;
	CIPCType m_cipcType;
	//
	DWORD m_dwDataLength;
	DWORD m_dwSendPos;
	DWORD m_dwSendLength;
	DWORD m_dwReceivePos;
	//
	BYTE m_byService;
public:
	static 	CWK_Average<int> gm_avgCompression;
};

typedef CCapiQueueRecord * CCapiQueueRecordPtr;

WK_PTR_ARRAY(CCapiQueueRecordArray,CCapiQueueRecordPtr);
WK_PTR_LIST_CS(CCapiQueueRecordList,CCapiQueueRecordPtr,CCapiQueueRecords);


///////////////////////////////////////////////////


inline void CCapiQueueRecord::AddTypeToService(BYTE &byService, CIPCType ct)
{
// input output database alarm

	switch(ct) 
	{
		case CIPC_NONE:
			// OOPS NOP
			break;
		case CIPC_INPUT_CLIENT:
			byService |= INPUT_BIT;
			break;
		case CIPC_OUTPUT_CLIENT:
			byService |=  OUTPUT_BIT;
			break;
		case CIPC_DATABASE_CLIENT:
			byService |=  DATABASE_BIT;
			break;
		case CIPC_ALARM_CLIENT:
			byService |=  ALARM_BIT;
			break;
		// server types
		case CIPC_INPUT_SERVER:
			byService |= SERVER_BIT;
			byService |=  INPUT_BIT;
			break;
		case CIPC_OUTPUT_SERVER:
			byService |= SERVER_BIT;
			byService |=  OUTPUT_BIT;
			break;
		case CIPC_DATABASE_SERVER:
			byService |= SERVER_BIT;
			byService |= DATABASE_BIT;
			break;
		case CIPC_ALARM_SERVER:
			byService |= SERVER_BIT;
			byService |= ALARM_BIT;
			break;
		case CIPC_AUDIO_SERVER:
			byService |= SERVER_BIT;
			byService |= AUDIO_BIT;
			break;
		case CIPC_AUDIO_CLIENT:
			byService |= AUDIO_BIT;
			break;

		default:
			WK_TRACE_ERROR(_T("AddType: invalid service %d\n"), byService);
	}
}


inline void ResetCipcHeader(CIPCHDR &ch, BYTE byService,
							 CIPCType ct)
{
	ch.m_byID = ID_CIPC;
	ch.byChannelNr = 0;	// overriden in CCapi
	ch.byNr = 0;	// overridden in CCapi
	ch.byService	= byService;
	ch.wDataLen	= 0;
	CCapiQueueRecord::AddTypeToService(ch.byService,ct);

}


inline void CCapiQueueRecord::Init()
{
	m_pData=NULL;
	m_dwSendPos=0;
	m_dwSendLength=0;
	m_dwReceivePos = 0;
	m_cipcType = CIPC_NONE;
	m_byService = CIPC_INVALID_SERVICE;
	// OOPS
	ResetCipcHeader(m_CipcHdr, m_byService ,m_cipcType);
}

inline DWORD CCapiQueueRecord::GetDataLength()
{
	return m_dwDataLength;
}

inline BOOL CCapiQueueRecord::AllDataReceived()
{
	if (m_dwReceivePos == m_dwDataLength)
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}


inline CCapiQueueRecord::~CCapiQueueRecord()
{
	m_dwRecordCounter--;
	WK_DELETE_ARRAY(m_pData);
}

BOOL inline CCapiQueueRecord::IsAllSend() const
{
	if (m_dwDataLength==m_dwSendPos) {
		return TRUE;
	} else {
		return FALSE;
	}

}

inline void CCapiQueueRecord::IncrementSendPos(DWORD dwAdd)
{	
	// NOT YET some checks
	m_dwSendPos+=dwAdd;
}

#endif