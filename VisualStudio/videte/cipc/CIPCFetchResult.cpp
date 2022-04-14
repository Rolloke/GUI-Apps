/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCFetchResult.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCFetchResult.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 26 $
// LAST CHANGE:	$Modtime: 19.01.06 16:10 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"

#include "CIPCFetchResult.h"
#include "wkclasses\WK_Trace.h"
#include "CIPCExtraMemory.h"
#include "WK_Magic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*{CIPCFetchResult Overview|Overview,CIPFetchResult}
 {members|CIPCFetchResult},
{CIPCServerControl} 
*/
/*
  {CIPCServerControl}
*/

/* BubbleFormat
DWORD WK_MAGIC_DW 0x12344321
DWORD m_error
DWORD m_iErrorCode
DWORD m_dwOptions
DWORD m_dwConnectTime
DWORD m_assignedID
DWORD _tcslen(m_sShmname)
DWORD _tcslen(m_sErrorMessage)
BYTE shmName[len1]
BYTE errorMsg[len2]
*/

DWORD CIPCFetchResult::GetMinBubbleSize()
{
	return (sizeof(DWORD)*8);
}

/*Function: NYD
*/
CIPCExtraMemory *CIPCFetchResult::CreateBubble(CIPC *pCipc)
{
	if (pCipc==NULL) 
	{
		WK_TRACE_ERROR(_T("CreateBubble from FetchResult without cipc\n"));
		return NULL;	// <<<EXIT>>>
	}

#ifdef _UNICODE
	CWK_String sShmName        = m_sShmName;	// force ansi strings for connection
	CWK_String sErrorOrVersion = m_sErrorOrVersion;
	int len1 = sShmName.CopyToMemory(FALSE, NULL);
	int len2 = sErrorOrVersion.CopyToMemory(FALSE, NULL);
#else
	int len1 = m_sShmName.GetLength();
	int len2 = m_sErrorOrVersion.GetLength();
#endif
	CIPCExtraMemory *pResult = new CIPCExtraMemory();
		
	if (pResult->Create(pCipc, GetMinBubbleSize()+len1+len2))
	{
		// fill bubble memory
		DWORD *pDW = (DWORD *)pResult->GetAddressForWrite();
		pDW[0] = WK_MAGIC_DW_FETCH_RESULT;
		pDW[1] = (DWORD)m_error;
		pDW[2] = (DWORD)m_iErrorCode;
		pDW[3] = (DWORD)m_dwOptions;
		pDW[4] = (DWORD)MAKELONG(m_wCodePage, m_wReserved);
		pDW[5] = m_assignedID.GetID();
		pDW[6] = len1;
		pDW[7] = len2;
		BYTE *pByte = (BYTE*)(&pDW[8]);
#ifdef _UNICODE
		sShmName.CopyToMemory(FALSE, (void*)pByte);
		pByte+=len1;
		sErrorOrVersion.CopyToMemory(FALSE, (void*)pByte);
		pByte+=len2;
#else
		int i;
		for (i=0;i<len1;i++) 
		{
			*pByte = m_sShmName[i];
			pByte++;
		}
		for (i=0;i<len2;i++) 
		{
			*pByte = m_sErrorOrVersion[i];
			pByte++;
		}
#endif
	}
	else
	{
		WK_DELETE(pResult);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in BubbleFromDrives\n"));
	}


	return pResult;
}

// from bubble
/*Function: NYD */
CIPCFetchResult::CIPCFetchResult(const CIPCExtraMemory &mem)
{
	Init();
	if (mem.GetLength()<GetMinBubbleSize())
	{
		WK_TRACE_ERROR(_T("CIPCFetchResult bubbleSize %d < %d\n"),
			mem.GetLength(),
			GetMinBubbleSize()
			);
	} 
	else
	{
		const DWORD *pDW = (const DWORD *)mem.GetAddress();
		if (pDW[0]!=WK_MAGIC_DW_FETCH_RESULT)
		{
			WK_TRACE_ERROR(_T("CIPCFetchResult invalid magic found %x expected %x\n"),
				pDW[0],
				WK_MAGIC_DW_FETCH_RESULT
				);
		}
		else
		{
			// preconditions fine, unpack...
			m_error = (CIPCError) pDW[1];
			m_iErrorCode = (int)pDW[2];
			m_dwOptions = pDW[3];
			m_wCodePage = LOWORD(pDW[4]);
			m_wReserved = HIWORD(pDW[4]);
			m_assignedID.Set(pDW[5]);
			int len1 = pDW[6];
			int len2 = pDW[7];
			const BYTE *pByte = (const BYTE *)(&pDW[8]);
#ifdef _UNICODE
			CWK_String str;
			str.InitFromMemory(FALSE, pByte, len1);
			TRACE(_T("CIPCFetchResult: %s, "), str);
			pByte+=len1;
			m_sShmName = str;

			str.InitFromMemory(FALSE, pByte, len2);
			TRACE(_T("%s\n"), str);
			pByte+=len2;
			m_sErrorOrVersion = str;
#else
			CopyMemory(m_sShmName.GetBufferSetLength(len1), pByte, len1);
			pByte += len1;
			m_sShmName.ReleaseBuffer();
			CopyMemory(m_sErrorOrVersion.GetBufferSetLength(len2), pByte, len2);
			pByte += len2;
			m_sErrorOrVersion.ReleaseBuffer();
//			WK_TRACE(_T("CIPCFetchResult(%s, %s)\n"), m_sShmName, m_sErrorOrVersion);
/*
			int i=0;
			if (len1)
			{
				for (i=0;i<len1;i++)
				{
					m_sShmName += pByte[i];
				}
			}
			if (len2)
			{
				for (i=0;i<len2;i++)
				{
					m_sErrorOrVersion += pByte[len1+i];
				}
			}
*/
#endif
		}
	}
}


void CIPCFetchResult::Init()
{
	// m_sShmName
	m_wReserved = 0;
	m_dwOptions = 0;
	m_error = CIPC_ERROR_UNKNOWN;
	m_iErrorCode = 0;
	// m_sErrorMessage
	// m_assignedID
#ifdef _UNICODE
	m_wCodePage = CODEPAGE_UNICODE;
#else
	m_wCodePage = (WORD)GetACP();
#endif
}

/*Function: NOT YET DOCUMENTED */
CIPCFetchResult::CIPCFetchResult()
{
	Init();
}

/*Function: NOT YET DOCUMENTED */
CIPCFetchResult::CIPCFetchResult(const CString &sShmName, 
								 CIPCError cipcError, 
								 int iErrorCode,
								 const CString& sErrorOrVersion,
								 DWORD dwOptions,
								 CSecID assignedID
#ifdef _UNICODE
								 , WORD wCodePage
#endif
								 )
{
	Init();
	m_sShmName = sShmName;
	m_dwOptions = dwOptions;
	m_error = cipcError;
	m_iErrorCode = iErrorCode;
	m_sErrorOrVersion = sErrorOrVersion;
	m_assignedID = assignedID;
#ifdef _UNICODE
	m_wCodePage = wCodePage;
#endif
}

/*Function: NOT YET DOCUMENTED */
BOOL CIPCFetchResult::IsOkay()	const
{
	return (m_error==CIPC_ERROR_OKAY);
}
/*Function: NOT YET DOCUMENTED */
CIPCError CIPCFetchResult::GetError() const
{
	return m_error;
}
/*Function: NOT YET DOCUMENTED */
int	CIPCFetchResult::GetErrorCode() const
{
	return m_iErrorCode;
}
/*Function: NOT YET DOCUMENTED */
const CString& CIPCFetchResult::GetErrorMessage() const
{
	return m_sErrorOrVersion;
}
const CString& CIPCFetchResult::GetServerVersion() const
{
	return m_sErrorOrVersion;
}
const CString& CIPCFetchResult::GetBitrate() const
{
	return m_sShmName;
}

/*Function: NOT YET DOCUMENTED */
const CString & CIPCFetchResult::GetShmName() const
{
	return m_sShmName;
}
/*Function: NOT YET DOCUMENTED */
DWORD CIPCFetchResult::GetOptions() const
{
	return m_dwOptions;	
}
/*Function: NOT YET DOCUMENTED */
CSecID	CIPCFetchResult::GetAssignedID() const
{
	return m_assignedID;
}

#ifdef _UNICODE
WORD CIPCFetchResult::GetCodePage()
{
	return m_wCodePage;
}
#endif