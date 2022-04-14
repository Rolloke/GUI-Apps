
#ifndef _SocketCmdRecord_h
#define _SocketCmdRecord_h

#include "wk.h"
#include "cipc.h"
#include "CipcExtraMemory.h"

class CSocketCmdRecord 
{
public:
	inline CSocketCmdRecord(WORD wLocalId,
							WORD wRemoteId,
							DWORD dwCmd,
							DWORD dwParam1,
							DWORD dwParam2,
							DWORD dwParam3,
							DWORD dwParam4,
							const CIPCExtraMemory* pExtraMem);
	inline ~CSocketCmdRecord();

// lazy public data
	WORD m_wLocalId;
	WORD m_wRemoteId;
	DWORD m_dwCmd;
	DWORD m_dwParam1;
	DWORD m_dwParam2;
	DWORD m_dwParam3;
	DWORD m_dwParam4;
	CIPCExtraMemory* m_pExtraMem;
	int m_iRetryCount;
};
//////////////////////////////////////////////////////////////////////////////////
typedef CSocketCmdRecord *CSocketCmdRecordPtr;
//////////////////////////////////////////////////////////////////////////////////
WK_PTR_LIST_CS(CSocketCmdRecordsPlain,CSocketCmdRecordPtr,CSocketCmdRecords);
//////////////////////////////////////////////////////////////////////////////////
inline CSocketCmdRecord::CSocketCmdRecord(	
		WORD wLocalId,
		WORD wRemoteId,
		DWORD dwCmd,
		DWORD dwParam1,
		DWORD dwParam2,
		DWORD dwParam3,
		DWORD dwParam4,
		const CIPCExtraMemory* pExtraMem
		)
{
	m_iRetryCount=0;

	m_wLocalId=wLocalId;
	m_wRemoteId=wRemoteId;
	m_dwCmd=dwCmd;
	m_dwParam1=dwParam1;
	m_dwParam2=dwParam2,
	m_dwParam3=dwParam3;
	m_dwParam4=dwParam4,
	m_pExtraMem=NULL;
	if (pExtraMem) {
		m_pExtraMem = new CIPCExtraMemory(*pExtraMem);
	}

}
//////////////////////////////////////////////////////////////////////////////////
inline CSocketCmdRecord::~CSocketCmdRecord()
{
	WK_DELETE(m_pExtraMem);
}

#endif