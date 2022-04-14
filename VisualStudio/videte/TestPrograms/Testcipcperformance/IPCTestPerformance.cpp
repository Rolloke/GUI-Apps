#include "StdAfx.h"
#include "ipctestperformance.h"
#include "TestCIPCPerformanceDlg.h"


unsigned long CIPCTestPerformance::gm_CRC_Table[256];

CIPCExtraMemory* BubbleFromFieldsAndBubble(CIPC *pCipc,
 										   const CIPCFields& f,
										   const CIPCExtraMemory* pMem)
{
	CIPCExtraMemory* pBubble = NULL;
#ifdef _UNICODE
	DWORD dwLen1 = f.GetBubbleLength(pCipc->GetCodePage());
#else
	DWORD dwLen1 = f.GetBubbleLength();
#endif			
	DWORD dwLen2 = pMem->GetLength();
	DWORD dwSum = dwLen1 + dwLen2 + 2 * sizeof(DWORD);
	const void* pSrc2 = pMem->GetAddress();

	pBubble = new CIPCExtraMemory();
	if (pBubble->Create(pCipc,dwSum))
	{
		BYTE* pByte = (BYTE*)pBubble->GetAddressForWrite();

		if (pByte)
		{
			*pByte++ = HIBYTE(HIWORD(dwLen1));
			*pByte++ = LOBYTE(HIWORD(dwLen1));
			*pByte++ = HIBYTE(LOWORD(dwLen1));
			*pByte++ = LOBYTE(LOWORD(dwLen1));
#ifdef _UNICODE
			DWORD dwDelta = f.ToMemory(pByte, pCipc->GetCodePage());
#else
			DWORD dwDelta = f.ToMemory(pByte);
#endif			
			WK_ASSERT(dwDelta == dwLen1);
			*pByte++ = HIBYTE(HIWORD(dwLen2));
			*pByte++ = LOBYTE(HIWORD(dwLen2));
			*pByte++ = HIBYTE(LOWORD(dwLen2));
			*pByte++ = LOBYTE(LOWORD(dwLen2));
			CopyMemory(pByte,pSrc2,dwLen2);
		}
		else
		{
			WK_DELETE(pBubble);
			WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in BubbleFromFieldsAndPicture\n"));
		}
	}

	return pBubble;
}

CIPCTestPerformance::CIPCTestPerformance(LPCTSTR lpSMName, BOOL bMaster, CTestCIPCPerformanceDlg*pDlg):
	CIPC(lpSMName, bMaster)
{
	m_pDlg = pDlg;
	if (gm_CRC_Table[0] == 0)
	{
		unsigned long shf;   /* crc shift register                        */
		int i;               /* counter for all possible eight bit values */
		int k;               /* counter for bit being shifted into crc    */
		for (i=0; i<256; i++)
		{
			shf = i;
			for (k=0; k<8; k++)
			{
				shf = (shf & 1) ? ((shf >> 1) ^ 0xedb88320L) : (shf >> 1);
			}

			gm_CRC_Table[i] = shf;
		}
	}
	m_dwTick = GetTickCount();
   	StartThread();
}

CIPCTestPerformance::~CIPCTestPerformance(void)
{
}

BOOL CIPCTestPerformance::HandleCmd(DWORD dwCmd, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, DWORD dwParam4, const CIPCExtraMemory *pExtraMem)
{
	switch (dwCmd)
	{
#ifdef TEST_DV_STORAGE
		case CIPC_DB_NEW_CONFIRM_SAVE:
			OnConfirmNewSavePicture((WORD)dwParam1,CSecID(dwParam2));
		break;
#endif
#ifdef TEST_PERFORMANCE
		case CIPC_TEST_CMD_REQUEST_TRANSMIT_DATA:
		{
			const void *pData = NULL;
			UINT nSize  = 0;
			if (pExtraMem)
			{
				pData = pExtraMem->GetAddress();
				nSize = pExtraMem->GetLength();
				if (dwParam4)
				{
					DWORD dwCheckSum = CalcCheckSum(nSize, pData);
					if (dwParam4 != dwCheckSum)
					{
						WK_TRACE(_T("Checksum error\n"));
						pData = NULL;
						nSize = CIPC_ERROR_USER_DEFINED;
					}
				}
			}
			OnRequestTransmitData(dwParam1, dwParam2, dwParam3, nSize, pData);
		}	break;
		case CIPC_TEST_CMD_CONFIRM_TRANSMIT_DATA:
			OnConfirmTransmitData(dwParam1);
			break;
#endif
		default:
			return CIPC::HandleCmd(dwCmd, dwParam1, dwParam2, dwParam3, dwParam4, pExtraMem);
	}
	return TRUE;
}
#ifdef TEST_PERFORMANCE
void CIPCTestPerformance::DoRequestTransmitData(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, UINT nSize, const void*pData)
{
	DWORD dwChecksum = 0;
	if (dwParam1)
	{
		dwChecksum = CalcCheckSum(nSize, pData);
	}
	WriteCmdWithData(nSize, pData, CIPC_TEST_CMD_REQUEST_TRANSMIT_DATA, dwParam1, dwParam2, dwParam3, dwChecksum);
}

void CIPCTestPerformance::OnRequestTransmitData(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, UINT nSize, const void*pData)
{
	DWORD dwError = 0;
	if (pData == NULL)
	{
		dwError = nSize;
	}
	DoConfirmTransmitData(dwError);
}

void CIPCTestPerformance::DoConfirmTransmitData(DWORD dwError)
{
	WriteCmd(CIPC_TEST_CMD_CONFIRM_TRANSMIT_DATA, dwError);

}

void CIPCTestPerformance::OnConfirmTransmitData(DWORD dwError)
{
	m_pDlg->m_nBubblesPerSecondRes++;
}
#endif

#ifdef TEST_DV_STORAGE
void CIPCTestPerformance::DoRequestNewSavePicture(WORD wArchivNr, const CIPCPictureRecord &pict, const CIPCFields& fields)
{
	if (pict.GetBubble())
	{
		CIPCExtraMemory *pBubble = BubbleFromFieldsAndBubble(this,
															  fields,
															  pict.GetBubble());
		WriteCmdWithExtraMemory(pBubble,CIPC_DB_NEW_REQUEST_SAVE,
								wArchivNr,fields.GetSize());
	}
	else
	{
		OnCommandSendError(CIPC_DB_NEW_REQUEST_SAVE);
	}
}

void CIPCTestPerformance::OnConfirmNewSavePicture(WORD wArchivNr, CSecID camID)
{
	m_pDlg->m_nBubblesPerSecondRes += 50;
}
#endif

void CIPCTestPerformance::OnReadChannelFound()
{
	WK_TRACE(_T("%s%d OnReadChannelFound %d ms\n"),GetShmName(),GetIsMaster(),GetTimeSpan(m_dwTick));
	WK_TRACE(_T("%s%d OnReadChannelFound %s\n"),GetShmName(),GetIsMaster(),NameOfEnum(GetIPCState()));
	m_dwTick = GetTickCount();
#ifdef TEST_DV_STORAGE
	DoRequestConnection();
#endif
#ifdef TEST_PERFORMANCE
	if (GetIsMaster())
	{
		DoRequestConnection();
	}
#endif

}
void CIPCTestPerformance::OnConfirmConnection()
{
	CIPC::OnConfirmConnection();
	WK_TRACE(_T("%s%d OnConfirmConnection %d ms\n"),GetShmName(),GetIsMaster(),GetTimeSpan(m_dwTick));
	WK_TRACE(_T("%s%d OnConfirmConnection %s\n"),GetShmName(),GetIsMaster(),NameOfEnum(GetIPCState()));
}
void CIPCTestPerformance::OnRequestConnection()
{
	CIPC::OnRequestConnection();
	WK_TRACE(_T("%s%d OnRequestConnection %d ms\n"),GetShmName(),GetIsMaster(),GetTimeSpan(m_dwTick));
	WK_TRACE(_T("%s%d OnRequestConnection %s\n"),GetShmName(),GetIsMaster(),NameOfEnum(GetIPCState()));
}


DWORD CIPCTestPerformance::CalcCheckSum(UINT nSize, const void *pData)
{
	DWORD dwCheckSum = 0xffffffff;
	unsigned char *buffer = (unsigned char *) pData;
	if (pData == NULL)
	{
		nSize = 0;
	}
	/* Update the crc with each byte */
	for (UINT i=0; i<nSize; i++)
	{
		dwCheckSum = gm_CRC_Table[buffer[i] ^ ((unsigned char) dwCheckSum)] ^ (dwCheckSum >> 8);
	}
	return dwCheckSum;
}
