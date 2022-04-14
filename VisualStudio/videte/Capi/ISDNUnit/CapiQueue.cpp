#include "stdafx.h"

#include "CapiQueue.h"
#include "CipcExtraMemory.h"
#include "hdr.h"
#include "SecID.h"
#include "capi4.h"

#include "ISDNConnection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MIN_QUEUE_SIZE	2

BOOL CCapiQueue::m_bDoLogSend=FALSE;
BOOL CCapiQueue::m_bDoLogReceive=FALSE;



////////////////////////////////////////////////////
CCapiQueue::CCapiQueue(CCapi* pCapi)
{
	ASSERT(pCapi);
	m_pCapi					= pCapi;
	m_pSendRecord			= NULL;
	m_pReceiveRecord		= NULL;
	m_dwSendIncreaseCounter	= 0;
	m_dwReceiveIncreaseCounter = 0;
	m_dwSendBlockCounter	= 0;
	m_dwSendStatBlockCount = 0;
	
	m_dwLastSendTick = GetTickCount();
	m_dwSendStat = 0;
}
////////////////////////////////////////////////////
CCapiQueue::~CCapiQueue()
{
	Reset();
}

////////////////////////////////////////////////////
void CCapiQueue::Reset()
{
	m_sendQueue.Lock();
	m_receiveQueue.Lock();

	m_sendQueue.SafeDeleteAll();
	WK_DELETE(m_pSendRecord);
	
	m_receiveQueue.SafeDeleteAll();
	WK_DELETE(m_pReceiveRecord);

	m_dwSendIncreaseCounter	= 0;
	m_dwReceiveIncreaseCounter = 0;
	m_dwSendBlockCounter = 0;

	m_receiveQueue.Unlock();
	m_sendQueue.Unlock();
}

////////////////////////////////////////////////////
// queue will take responsiblity to delete newRecord
void CCapiQueue::PrependToSendQueue(CCapiQueueRecord *newRecord)
{
	if (newRecord==NULL) 
	{
		return;
	}
	m_sendQueue.Lock();
	m_sendQueue.AddHead(newRecord);
	m_sendQueue.Unlock();
}
////////////////////////////////////////////////////
// queue will take responsiblity to delete newRecord
void CCapiQueue::AddToSendQueue(CCapiQueueRecord *newRecord, BOOL bRemoveOldest)
{
	if (newRecord==NULL) 
	{
		return;
	}

	m_sendQueue.SafeAddTail(newRecord);

#if 1
	// NOT YET use bRemoveOldest
#else
	if ( bRemoveOldest ) 
	{
		BOOL bFound = FALSE;
		DWORD* pDW = (DWORD*)(newRecord->m_pData);
		DWORD dwNewCmd = pDW[2] OLD;
		CCapiQueueRecord* pQueueRecord = NULL;
		DWORD dwCmdInQ = 0;
		// Queue bis auf die beiden letzten Elemente durchsuchen
		for ( int i=0 ; !bFound && i<(m_sendQueue.GetCount()-MIN_QUEUE_SIZE) ; i++)
		{
			pQueueRecord = m_sendQueue.GetAt(i);
			pDW = (DWORD*)(pQueueRecord->m_pData);
			dwCmdInQ = pDW[2] OLD;
			if (dwNewCmd == dwCmdInQ)
			{
				m_sendQueue.RemoveAt(i);
				m_dwSendIncreaseCounter--;
				WK_STAT_PEAK("Reset",1,"OldestDroppped");
				WK_DELETE(pQueueRecord);
				bFound = TRUE;
			}
		}
	}
	else
	{  // end of removeOldest
	}
#endif

}

////////////////////////////////////////////////////
// application will take responsiblity to delete newRecord (GetReceivedCmd())
void CCapiQueue::AddToReceiveQueue(const CIPCHDR* pCipcHdr)
{

	if (m_bDoLogReceive)
	{
		WK_STAT_PEAK(_T("Reset"),pCipcHdr->wDataLen,_T("ReceivedData") );
	}

	BYTE byService = pCipcHdr->byService;
	CIPCType cipcType;
	CCapiQueue::SplitTypeFromService(byService,cipcType);
	// Fängt ein neues Kommando an ?
	switch(byService) 
	{
		case CIPC_DATA_SMALL:
			if (m_pReceiveRecord) 
			{
				WK_TRACE_ERROR(_T("ERROR: CCapiQueue::AddToReceiveQueue() New but old Record not finished\n"));
				WK_DELETE(m_pReceiveRecord);
			}
			// create record
			m_pReceiveRecord = new CCapiQueueRecord(pCipcHdr);
		break;
		case CIPC_DATA_LARGE_START:
			if (m_pReceiveRecord)
			{
				WK_TRACE_ERROR(_T("ERROR: CCapiQueue::AddToReceiveQueue() New but old Record not finished\n"));
				WK_DELETE(m_pReceiveRecord);
			}
			m_pReceiveRecord = new CCapiQueueRecord(pCipcHdr);
		break;
		case CIPC_DATA_LARGE:
			if (m_pReceiveRecord)
			{
				m_pReceiveRecord->AppendData(pCipcHdr);
			} else {
				WK_TRACE_ERROR(_T("no receive record for DATA_LARGE\n"));
			}
		break;
		case CIPC_DATA_LARGE_END:
			if (m_pReceiveRecord)
			{
				m_pReceiveRecord->FinalizeData(pCipcHdr);
			}
			else
			{
				WK_TRACE_ERROR(_T("no receive record for DATA_LARGE_END\n"));
			}
		break;
		default:
			WK_TRACE_ERROR(_T("Receive: invalid service %d\n"),byService);
	}


	if ( m_pReceiveRecord )
	{
		if (   (byService == CIPC_DATA_SMALL)
			|| (byService == CIPC_DATA_LARGE_END)
			)
		{
			if ( m_pReceiveRecord->AllDataReceived() )
			{
				m_receiveQueue.SafeAddTail(m_pReceiveRecord);
				m_dwReceiveIncreaseCounter++;
				m_pReceiveRecord = NULL;
			}
			else
			{
				WK_TRACE_ERROR(_T("invalid end of data, service is %d, receive %d len %d\n"),
					byService, 
					m_pReceiveRecord->m_dwReceivePos, 
					m_pReceiveRecord->m_dwDataLength
					);
				WK_DELETE(m_pReceiveRecord);
			}
		}
		else if (byService == CIPC_DATA_LARGE)
		{
			// keep on collection multiple blocks
			// OOPS error if CIPC_DATA_SMALL
			// gf todo
			// Indicate data flow to avoid timeout interruption at longer transmissions/small bandwidth
		}
		else
		{
			// keep on collection multiple blocks
			// OOPS error if CIPC_DATA_SMALL
		}
	}
}


////////////////////////////////////////////////////
void CCapiQueue::SendData(const CISDNConnection *pConnection)
{
	BOOL bDataSendOk = TRUE;

	while ( bDataSendOk )
	{	// keep on sending ?
		// already a send job? if not check for one in the queue
		if (m_pSendRecord==NULL)
		{
			// activate new send job
			m_pSendRecord = m_sendQueue.SafeGetAndRemoveHead();
			if (m_pSendRecord)
			{
				m_dwSendIncreaseCounter--;
			}
		}

		// something to send ?
		if (m_pSendRecord)
		{
			DWORD dwLeftToSend;
			dwLeftToSend= m_pSendRecord->GetDataLength() - m_pSendRecord->m_dwSendPos;
			BYTE byService;

			if (dwLeftToSend > MAX_BLOCK_SIZE)
			{
				// multi blocks
				if (m_pSendRecord->m_dwSendPos==0)
				{
					// first time
					byService = CIPC_DATA_LARGE_START;
				}
				else
				{
					// continuation but not the last block
					byService = CIPC_DATA_LARGE;
				}
				m_pSendRecord->m_CipcHdr.wDataLen = MAX_BLOCK_SIZE;

			}
			else 
			{	// <= MAX_BLOCK_SIZE
				if (dwLeftToSend==0)
				{
					WK_TRACE_ERROR(_T("Nothing to send !?\n"));
				} 
				if (m_pSendRecord->m_byService==CIPC_INVALID_SERVICE
					|| m_pSendRecord->m_byService==CIPC_DATA_SMALL
					|| m_pSendRecord->m_byService==CIPC_DATA_LARGE_START
					|| m_pSendRecord->m_byService==CIPC_DATA_LARGE
					|| m_pSendRecord->m_byService==CIPC_DATA_LARGE_END
					)
				{
					// single or last block
					m_pSendRecord->m_CipcHdr.wDataLen = (WORD)dwLeftToSend;
					if (m_pSendRecord->GetDataLength()>MAX_BLOCK_SIZE) {
						byService = CIPC_DATA_LARGE_END;
					}
					else
					{
						byService = CIPC_DATA_SMALL;
					}
				} 
				else
				{
					byService = m_pSendRecord->m_byService;
					// OOPS
					WK_TRACE(_T("Service is %d len %d\n"),byService,dwLeftToSend);
					m_pSendRecord->m_CipcHdr.wDataLen = (WORD)dwLeftToSend;
				}
			}

			m_pSendRecord->m_CipcHdr.byService = byService;

			CCapiQueueRecord::AddTypeToService(
							m_pSendRecord->m_CipcHdr.byService,
							m_pSendRecord->m_cipcType
							);
			switch(m_pSendRecord->m_CipcHdr.m_byID)
			{
			case ID_CIPC: case ID_CIPC_COMP: case ID_CIPC_COMP_SEG:
					break;
				default:
					m_pSendRecord->m_CipcHdr.m_byID = ID_CIPC;
					break;
			}

			// try to send data, according to the protocol
			BOOL bSendOkay = FALSE;
			if (pConnection->IsISTConnection())
			{
				if (m_pSendRecord->m_CipcHdr.wDataLen==0)
				{
					WK_TRACE_ERROR(_T("Can't send zero length data\n"));
					bDataSendOk=TRUE;
					bSendOkay = FALSE;
				}
				else 
				{
					bSendOkay = m_pCapi->SendData(
						(BYTE*)&m_pSendRecord->m_CipcHdr, 
						&m_pSendRecord->m_pData[m_pSendRecord->m_dwSendPos]
						);
				}
			}
			else if (pConnection->IsPTConnection())
			{
				bSendOkay = 
					m_pCapi->SendRawData(
						&(m_pSendRecord->m_pData[m_pSendRecord->m_dwSendPos]),
						(WORD) dwLeftToSend
					);						
			}
			else
			{

			}

			if (bSendOkay)
			{
				m_dwSendStatBlockCount++;	
				m_dwSendBlockCounter++;
				m_pSendRecord->IncrementSendPos(m_pSendRecord->m_CipcHdr.wDataLen);
				if (m_bDoLogSend)
				{
					m_dwSendStat += m_pSendRecord->m_CipcHdr.wDataLen;
					WK_STAT_PEAK(_T("Reset"),m_pSendRecord->m_CipcHdr.wDataLen,_T("SendData") );
					DWORD ct = GetTickCount();
					DWORD dwDelta = ct-m_dwLastSendTick;
					if (dwDelta>1000)
					{

						if (m_dwSendStat)
						{
							if (m_bDoLogSend)
							{
								DWORD dwRate = (1000*m_dwSendStat) / dwDelta;
								WK_STAT_LOG(_T("Reset"),dwRate,_T("Bitrate"));
								WK_STAT_LOG(_T("Reset"),m_dwSendStatBlockCount,_T("BlocksSend"));
							}
							m_dwSendStat =0;	// reset
							m_dwSendStatBlockCount =0;	// reset
							m_dwLastSendTick=ct;
						}
						else
						{
							m_dwSendStat += m_pSendRecord->m_CipcHdr.wDataLen;
						}
					}
				}
				else
				{
				}
				// all send ?
				if (m_pSendRecord->IsAllSend())
				{ // DONE
					WK_DELETE(m_pSendRecord);
					// exit loop after ONE record is send. There are more things to do
					// in other _T('threads')
					bDataSendOk = FALSE;
				}
				else
				{
					// keep on sending continuation blocks
				}
			}
			else
			{
				// WK_STAT_PEAK(_T("Reset"),1,_T("SendFailed"));
				bDataSendOk = FALSE;
			}
		}
		else
		{
			bDataSendOk = FALSE;	// terminate loops
		}
	}	// end of loop while(bDataSendOk)
}

