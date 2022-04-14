#include "stdafx.h"

#include "CapiQueueRecord.h"
#include "CipcExtraMemory.h"
#include "hdr.h"
#include "SecID.h"
#include "capi4.h"
#include "CapiQueue.h"
#include "CIPCPipeISDN.h"
#include "wkclasses\logziplib\logzip.h"

#include "ISDNUnit.h"

#include "CipcDatabase.h"
#include "CipcOutput.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////
DWORD CCapiQueueRecord::m_dwRecordCounter = 0;
DWORD CCapiQueueRecord::m_dwRecordCounterMax = 0;
CWK_Average<int> CCapiQueueRecord::gm_avgCompression;
////////////////////////////////////////////////////
CCapiQueueRecord::CCapiQueueRecord()
{
	Init();
}

////////////////////////////////////////////////////
// only for received data
CCapiQueueRecord::CCapiQueueRecord(const CIPCHDR* pCipcHdr)
{
	Init();

	m_CipcHdr = *pCipcHdr;	// OOPS needed

	// copy data all or only first block
	BYTE byService = pCipcHdr->byService;

	CIPCType cipcType;
	CCapiQueue::SplitTypeFromService(byService,cipcType);
	m_byService = byService;
	
	const BYTE* pFullData = (const BYTE*)pCipcHdr;
	const BYTE* pRealData = (const BYTE*)pFullData+CIPC_HDRLEN;

	if (byService==CIPC_DATA_SMALL)
	{
		m_dwDataLength = pCipcHdr->wDataLen;
		m_pData = new BYTE[m_dwDataLength];
			 // destination, source   , len
		
		MoveMemory(	m_pData, pRealData, m_dwDataLength);
		m_dwReceivePos += m_dwDataLength;
	}
	else if (byService==CIPC_DATA_LARGE_START)
	{
		const DWORD *pDWData = (const DWORD *)pRealData;
		m_dwDataLength = pDWData[0];

		m_pData = new BYTE[m_dwDataLength];	// full size (for multiple records)
		MoveMemory(	m_pData, // destination
					pRealData+sizeof(DWORD),	// source
					pCipcHdr->wDataLen-sizeof(DWORD)
					); 
		m_dwReceivePos += pCipcHdr->wDataLen-sizeof(DWORD);
	}
	else 
	{
		WK_TRACE_ERROR(_T("invalid queue record service %d\n"),byService);
	}
}


CCapiQueueRecord::CCapiQueueRecord(DWORD dwDataLen, const BYTE *pData)	// for raw data
{
	Init();

	m_byService = CIPC_DATA_SMALL;	// OOPS what is correct here ?

	// NOT YET check blocksize
	
	m_dwDataLength = dwDataLen;
	m_pData = new BYTE[m_dwDataLength];

	MoveMemory(	m_pData, // destination
				pData,	 // source
				m_dwDataLength	// len
				);
	m_dwReceivePos = m_dwDataLength;

}

////////////////////////////////////////////////////
// only called from CIPCDataRequest
void CCapiQueueRecord::FillFromCipcData(CSecID idFrom, CSecID idTo,
								   CIPCType cipcType,
								   DWORD dwCmd,
								   DWORD dwParam1, DWORD dwParam2,
								   DWORD dwParam3, DWORD dwParam4,
								   const CIPCExtraMemory *pExtraMem,
								   BOOL bCompressed/*=FALSE*/
								   )
{
	if (theApp.TraceSentCommands())
	{
		WK_TRACE(_T("CIPCDataRequest from %x to %x %s\n"),
					idFrom.GetID(),
					idTo.GetID(),
					CIPC::NameOfCmd(dwCmd)
					);
		TRACE(_T("cmd:%s, %x, %x, %x, %x, %x\n"), CIPC::NameOfCmd(dwCmd), dwParam1, dwParam2, dwParam3, dwParam4, pExtraMem);
	}
	Init();
	m_byService = CIPC_INVALID_SERVICE;	// USED as mark for data
	m_cipcType = cipcType;
	ResetCipcHeader(m_CipcHdr, m_byService ,m_cipcType);

	if (idTo==SECID_NO_ID) 
	{
		// WK_TRACE(_T("OOPS NOID!?\n")); is allowed for PIPE_CONF etc.
	}
	DWORD dwOffset=0;

	m_dwDataLength = sizeof(DWORD)*6+2;	// minimum [0..5] plau 2 id bytes
	if (pExtraMem) 
	{
		m_dwDataLength += pExtraMem->GetLength();
	}

	if (m_dwDataLength > MAX_BLOCK_SIZE) 
	{
		dwOffset        = sizeof(DWORD);
		m_dwDataLength += dwOffset;
		// DROPPED WK_TRACE(_T("LargeData %d includes size\n"),m_dwDataLength);
	}
	
	m_pData = new BYTE[m_dwDataLength];
	
	// copy dwords
	BYTE *pBYTEData = (BYTE *)(m_pData+dwOffset);	;
	DWORD *pDWData = (DWORD*)(m_pData+dwOffset+2);	// CAST

#if 0
	// old style
	pDWData[0] = idFrom.GetID();
	pDWData[1] = idTo.GetID();
	pDWData[2] = dwCmd;
	pDWData[3] = dwParam1;
	pDWData[4] = dwParam2;
	pDWData[5] = dwParam3;
	pDWData[6] = dwParam4;
	pDWData[7] = 0;	// ExtraMemoryLen
	
	if (pExtraMem) 
	{
		pDWData[7]=pExtraMem->GetLength();
		CopyMemory(&(pDWData[8]),		// destination
					pExtraMem->GetAddress(),	// source
					pExtraMem->GetLength()		// length
					);
	}
#else
	pBYTEData[0] = (BYTE)(idFrom.GetID()&255);
	pBYTEData[1] = (BYTE)(idTo.GetID()&255);	
	pDWData[0] = dwCmd;
	pDWData[1] = dwParam1;
	pDWData[2] = dwParam2;
	pDWData[3] = dwParam3;
	pDWData[4] = dwParam4;
	pDWData[5] = 0;	// ExtraMemoryLen

	if (pExtraMem) 
	{
		pDWData[5]=pExtraMem->GetLength();
		CopyMemory(&(pDWData[6]),		// destination
					pExtraMem->GetAddress(),	// source
					pExtraMem->GetLength()		// length
					);
	}
#endif

	if (bCompressed)
	{
		DWORD dwNewLen = dwOffset +					// optional data length for transmission of data parts
			             sizeof(DWORD)+				// original uncompressed data length
			MulDiv(m_dwDataLength, 101, 100) + 8;	// buffer needed for compression
														
		BYTE* pData = new BYTE[dwNewLen];
		DWORD *pdwData = (DWORD*)pData;
		if (dwOffset)
		{
			m_dwDataLength -= dwOffset;
			pdwData[0] = 0;
			pdwData[1] = m_dwDataLength;
		}
		else
		{
			pdwData[0] = m_dwDataLength;
		}
		dwNewLen -= (sizeof(DWORD)+dwOffset);		// 

		int nRes = CLogZip::CompressData(&pData[sizeof(DWORD)+dwOffset], &dwNewLen, (const BYTE*)&m_pData[dwOffset], m_dwDataLength, bCompressed);
		int nRate = MulDiv(dwNewLen, 100, m_dwDataLength);
		if (nRes == 0 && nRate < 100)
		{
//			TRACE(_T("Shrinked:%d\n"), nRate);
			m_CipcHdr.m_byID = dwOffset ? ID_CIPC_COMP_SEG : ID_CIPC_COMP;
			swap(m_pData, pData);
			m_dwDataLength = dwNewLen + sizeof(DWORD) + dwOffset;
			gm_avgCompression.AddValue(nRate);
		}
		else
		{
			if (dwOffset)
			{
				m_dwDataLength += dwOffset;
			}
			gm_avgCompression.AddValue(100);
		}
		delete pData;
	}

	if (m_dwDataLength>MAX_BLOCK_SIZE) 
	{
		DWORD *pDWDataHead = (DWORD*)(m_pData);	// CAST
		// CAVEAT m_dwDataLength is the length + the leading size info
		pDWDataHead[0]=m_dwDataLength-dwOffset;
	}
}

////////////////////////////////////////////////////
void CCapiQueueRecord::FillFromCmdRecord(CSecID idFrom, CSecID idTo,
								   CIPCType cipcType,
								   const CIPCCmdRecord &c,
								   BOOL bCompressed/*=FALSE*/
								   )
{
	FillFromCipcData(	idFrom,idTo,cipcType,
						c.m_dwCmd,
						c.m_dwParam1, c.m_dwParam2,
						c.m_dwParam3, c.m_dwParam4,
						c.m_pExtraMemory,
						bCompressed
						);
}

void CCapiQueueRecord::AppendData(const CIPCHDR* pCipcHdr)
{
	// NOT YET do some checks

	// copy Data
	const BYTE* pData = (const BYTE*)pCipcHdr;
	pData += CIPC_HDRLEN;	// skip header

	if (m_dwReceivePos+pCipcHdr->wDataLen<=m_dwDataLength)
	{
		MoveMemory(m_pData+m_dwReceivePos, pData, pCipcHdr->wDataLen); 
		m_dwReceivePos += pCipcHdr->wDataLen;
	}
	else
	{
		WK_TRACE_ERROR(_T("data overflow received %d len %d + %d\n"),
			m_dwReceivePos,
			m_dwDataLength,
			pCipcHdr->wDataLen
			);
		// OOPS what to do now
	}
}

void CCapiQueueRecord::FinalizeData(const CIPCHDR* pCipcHdr)
{
	// NOT YET do some checks
	// copy Data
	const BYTE* pData = (const BYTE*)pCipcHdr;
	pData += CIPC_HDRLEN;
	if (m_dwReceivePos+pCipcHdr->wDataLen<=m_dwDataLength)
	{
		MoveMemory(m_pData+m_dwReceivePos, pData, pCipcHdr->wDataLen); 
		m_dwReceivePos += pCipcHdr->wDataLen;
		// OOPS somthing more ?
	}
	else
	{
		WK_TRACE_ERROR(_T("data overflow, can't finalize\n"));
		// OOPS what to do now
	}
}

////////////////////////////////////////////////////
void CCapiQueueRecord::GetCipcCmd(BYTE& byCmdFrom,
								  CSecID& idFrom,
								  CSecID& idTo,
								  DWORD& dwExtraMemoryLen,
								  BYTE*& pExtraMem,
								  DWORD& dwCmd,
								  DWORD& dwParam1,
								  DWORD& dwParam2,
								  DWORD& dwParam3,
								  DWORD& dwParam4
								  )
{
	BYTE byService = m_CipcHdr.byService;
	CIPCType ct = CIPC_NONE;
	CCapiQueue::SplitTypeFromService(byService,ct);

	byCmdFrom = (BYTE)ct;	// OOPS direct cast
#if 1

	if (m_CipcHdr.m_byID == ID_CIPC_COMP || m_CipcHdr.m_byID == ID_CIPC_COMP_SEG)
	{
		static const int nMaxLen = 4*1024*1024; // 4 MB
		DWORD *pdwData = (DWORD*)m_pData;
		DWORD dwNewLen = pdwData[0];
		DWORD dwOffset = 0;
		if (m_CipcHdr.m_byID == ID_CIPC_COMP_SEG)
		{
			if (dwNewLen == 0)
			{
				dwNewLen = pdwData[1];
				dwOffset = sizeof(DWORD);
			}
		}
		dwNewLen += 16;
		if (dwNewLen < nMaxLen)
		{
			BYTE *pData = new BYTE[dwNewLen];
			int nRes = CLogZip::UnCompressData(pData, &dwNewLen, &m_pData[sizeof(DWORD)+dwOffset], m_dwDataLength);
			int nRate = MulDiv(m_dwDataLength, 100, dwNewLen);
			gm_avgCompression.AddValue(nRate);
			swap(pData, m_pData);
			delete pData;
			m_dwDataLength = dwNewLen;
			if (nRes != 0)
			{
				dwCmd = 0;
				return ;
			}
		}
		else
		{
			dwCmd = 0;
			return ;
		}
	}

	const BYTE * pBYTEData = (const BYTE *)m_pData;	// CAST
	const DWORD* pDWData = (const DWORD*)(m_pData+2);	// CAST

	idFrom.Set(0x1000,pBYTEData[0]);
	idTo.Set(0x1000,pBYTEData[1]);
	dwCmd	= pDWData[0];
	dwParam1= pDWData[1];
	dwParam2= pDWData[2];
	dwParam3= pDWData[3];
	dwParam4= pDWData[4];
	dwExtraMemoryLen = pDWData[5];
	pExtraMem = NULL;

	if (dwExtraMemoryLen)
	{
		pExtraMem = (BYTE*)&(pDWData[6]);
	}

#else
	// old style
	DWORD* pDWData = (DWORD*)m_pData;	// CAST
	idFrom	= pDWData[0];
	idTo	= pDWData[1];
	dwCmd	= pDWData[2];
	dwParam1= pDWData[3];
	dwParam2= pDWData[4];
	dwParam3= pDWData[5];
	dwParam4= pDWData[6];
	dwExtraMemoryLen = pDWData[7];
	pExtraMem = NULL;
	
	if (dwExtraMemoryLen) 
	{
		pExtraMem = (BYTE*)&(pDWData[8]);
	}
	// end of old style
#endif

	if (theApp.TraceReceivedCommands())
	{
		WK_TRACE(_T("CIPCDataReceived from %x to %x len %d %s\n"), idFrom.GetID(), idTo.GetID(), dwExtraMemoryLen, CIPC::NameOfCmd(dwCmd));
		TRACE(_T("rec:%s, %x, %x, %x, %x, %x\n"), CIPC::NameOfCmd(dwCmd), dwParam1, dwParam2, dwParam3, dwParam4, dwExtraMemoryLen);
	}

}

