/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CipcExtraMemory.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CipcExtraMemory.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 105 $
// LAST CHANGE:	$Modtime: 19.01.06 20:30 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"

#include "Cipc.h"
#include "CipcChannel.h"
#include "CipcExtraMemory.h"
#include "WK_Timer.h"
#include "CIPCField.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

volatile LONG CIPCExtraMemory::m_iNumCreated=0;
int CIPCExtraMemory::m_iMaxCreated=0;
volatile DWORD CIPCExtraMemory::m_dwGlobalCounter=0;
//
//
// PART: CIPCExtraMemory
//
//
/*
{CIPCExtraMemory Overview|Overview,CIPCExtraMemory}
@BOLD Related topics: @NORMAL 
{Members|CIPCExtraMemory}, {CIPC}

The class CIPCExtraMemory handles memory for the
interprocess communication of CIPC. A sending process
creates a chunk of memory (also called memory bubble). To exchange
this memory the mechanism of memory mapped files is used.
This allows to transfer a handle to a memory bubble and do a mapping
in different processes. In the SystemMonitor this mapping is
visible as 'swapped-in pages' (Seiteneinlagerungen), Thus
there is the strange effect, that there are swapped-in pages,
without any swapped-out ones.

Within one process a reference counting mechanism is used, so
it is possible transfer memory bubbles between thread, without copying.

@LISTHEAD{Sending CIPCExtraMemory:} First step is to create a memory
bubble via 
{CIPCExtraMemory(CIPC *ptr, DWORD len)|CIPCExtraMemory::CIPCExtraMemory}.
Usually this will happen within a CIPC derived class, so ptr
will be this.
This creates a writable chunk of memory, which can be filled as needed.
Second step is to feed this bubble into one of the
@CW{DoXXXExtra(...)}, which will send it to the receiver. The write action
will take responsibility for the memory bubble. The
DoXXXExtra functions take a reference to a pointer as parameter
@BOLD and set the pointer to NULL!@NORMAL

@LISTHEAD{Receiving CIPCExtraMemory:}
On the receiver side the @CW{OnXXX(...)} functions are called 
with a @CW{const void *pData}. If there is need to handle the memory
further it is possible to get the memory bubble via
{CIPC::CopyExtraMemory}(const void *pData). It returns
a pointer to a CIPCExtraMemory, which has ReadOnly access! 
Once the memory is known
as CIPCExtraMemory the copy constructor can be used to get as many
copies as need. Copying does not take time, since it is based on
reference counting. The caller is responsible to delete the created
CIPCExtraMemory.

 {Members|CIPCExtraMemory}
*/

void CIPCExtraMemory::CheckMax()
{
	if (m_iNumCreated>m_iMaxCreated) 
	{
		m_iMaxCreated=m_iNumCreated;
		if (m_iMaxCreated>250 && (m_iMaxCreated % 50)== 0) 
		{
			WK_TRACE(_T("CIPC %s:Now there are %d bubbles\n"),
				m_pCipc->GetShmName(),m_iMaxCreated);
		}
	}
}
//////////////////////////////////////////////////////
CIPCExtraMemory::CIPCExtraMemory()
{
	m_pCipc = NULL;
	m_pRecord = NULL;
	m_bReadOnly = FALSE;
}

//////////////////////////////////////////////////////
/*  
{Overview|Overview,CIPCExtraMemory},
{CIPC}
*/
/*Function: creates a writable bubble of size dwLength */
BOOL CIPCExtraMemory::Create(const CIPC *pOneCipc, 
							 DWORD dwLength, 
							 const void *pData/*=NULL*/)
{
	if (Allocate(pOneCipc,dwLength))
	{
		if (pData)
		{
			return SetMemoryFrom(pData,dwLength);
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}
/*Function: creates a writable bubble filled with the given string*/
#ifdef _UNICODE
BOOL CIPCExtraMemory::Create(const CIPC *pOneCipc, const CString &sString)
{
	CWK_String swkString(sString);
	BOOL bRet = FALSE;
	BOOL bUnicode = (pOneCipc->GetCodePage() == CODEPAGE_UNICODE) ? TRUE : FALSE;
	if (Allocate(pOneCipc, swkString.CopyToMemory(bUnicode, NULL, -1, pOneCipc->GetCodePage())))
	{
		m_pRecord->Lock();
		if (sString.GetLength())
		{
			DWORD nLen = swkString.CopyToMemory(bUnicode, GetAddressForWrite(), -1, pOneCipc->GetCodePage());
			bRet = (nLen == m_pRecord->m_dwExtraMemoryLen) ? TRUE:FALSE;
		}
		else
		{
			bRet = TRUE;
		}
		m_pRecord->Unlock();
	}
	return bRet;
}
#else
BOOL CIPCExtraMemory::Create(const CIPC *pOneCipc, LPCTSTR szBuffer)
{
	return Create(pOneCipc, strlen(szBuffer), szBuffer);
}
/*Function: creates a writable bubble filled with the given string*/
BOOL CIPCExtraMemory::Create(const CIPC *pOneCipc, const CString &sString)
{
	return Create(pOneCipc, sString.GetLength(), LPCTSTR(sString));
}
#endif

// NOT YET magic byte
/*Function: creates a writable bubble filled with the given strings */
BOOL CIPCExtraMemory::Create(const CIPC *pOneCipc, const CStringArray &someStrings)
{
	// NOT YET can't handle 0 strings, seems to be in, not tested
	BOOL bRet = FALSE;
	int i;
	int iNumStrings=someStrings.GetSize();
	DWORD dwLength = 0;
	DWORD *pHeader = new DWORD[max(1,iNumStrings)];	// make sure it's an array
#ifdef _UNICODE
	ASSERT(pOneCipc!=NULL);
	BOOL bUnicode = (pOneCipc->GetCodePage() == CODEPAGE_UNICODE) ? TRUE : FALSE;
#endif	
	// calc sum and fill pHeader
	for (i=0;i<iNumStrings;i++) 
	{
		const CWK_String & p = someStrings[i];
#ifdef _UNICODE
		pHeader[i] = p.CopyToMemory(bUnicode, NULL, -1, pOneCipc->GetCodePage());
		dwLength  += pHeader[i];
#else
		pHeader[i] = p.GetLength();
		dwLength  += pHeader[i];
#endif
	}
	// extra space for header, which contains the length of all strings
	// plus the number of each string
	dwLength += sizeof(DWORD)*(iNumStrings+1);

	if (Allocate(pOneCipc,dwLength))
	{
		// fill memory with strings and write len into the header
		char  *pWrite   = (char *)GetAddressForWrite();
		char  *pTemp;
		DWORD *pDWWrite = (DWORD *)GetAddressForWrite();
		DWORD dwOffset=0;
//		int   nLen;

		pDWWrite[0]=iNumStrings;	// first field
		
		for (i=0;i<iNumStrings;i++) 
		{
			const CWK_String & p = someStrings[i];
			if (pHeader[i]) 
			{
				pDWWrite[i+1] = pHeader[i];	// skip num field
#ifdef _UNICODE
				pTemp = pWrite+sizeof(DWORD)*(iNumStrings+1)+dwOffset;
				p.CopyToMemory(bUnicode, (void*)pTemp, -1, pOneCipc->GetCodePage());
				dwOffset += pHeader[i];
#else
				pTemp = pWrite+sizeof(DWORD)*(iNumStrings+1)+dwOffset;
				CopyMemory(pTemp, p, pHeader[i]);
				dwOffset += pHeader[i];
#endif
			} 
			else 
			{	// empty string
				pDWWrite[i+1] = 0;
			}
		}	// end of fill memory loop
		bRet = TRUE;
	}
	WK_DELETE_ARRAY(pHeader);
	return bRet;
}


BOOL CIPCExtraMemory::Allocate(const CIPC *pOneCipc, DWORD dwLength)
{
	CString sName;

	m_pCipc = pOneCipc;
	
	InterlockedIncrement((LONG*)&m_iNumCreated); // m_iNumCreated++;
	DWORD dwCounter = InterlockedIncrement((LONG*)&m_dwGlobalCounter);

	CheckMax();

	// INIT
	m_bReadOnly = FALSE;
	m_pRecord = NULL;

	if (pOneCipc == NULL) 
	{
		WK_TRACE(_T("CIPCExtraMemory from NULL CIPC\n"));
	}
	else
	{
		if (dwLength==0) 
		{
			WK_TRACE(_T("CIPCExtraMemory from zero length for %s\n"), pOneCipc->GetShmName());
		}
		else
		{
			
			m_pRecord = new CIPCExtraMemoryRecord;
			
			m_pRecord->Lock();
			// create a unique name, NAME[0/1]MemTICK

			int iNumRetries = 0;
			while (   m_pRecord->m_pExtraMemory == NULL
				   && iNumRetries<5)

			{
				sName.Format(_T("%s%08lxMem%d:%d"), (LPCTSTR)pOneCipc->GetShmName(),
											pOneCipc->GetIsMaster(), 
											dwCounter,
											GetTickCount());
				m_pRecord->m_hExtraMemory = CreateFileMapping(INVALID_HANDLE_VALUE, 
															NULL, 
															PAGE_READWRITE, 
															0, 
															dwLength, 
															(LPCTSTR)sName);

				if (m_pRecord->m_hExtraMemory) 
				{
					m_pRecord->m_iRefCount=1;
					m_pRecord->m_dwExtraMemoryLen = dwLength;

					// and do the real mapping
					m_pRecord->m_pExtraMemory = MapViewOfFile(m_pRecord->m_hExtraMemory, 
																FILE_MAP_WRITE,
																0, 0,	// offsets
																m_pRecord->m_dwExtraMemoryLen);
					if (m_pRecord->m_pExtraMemory == NULL)
					{
						m_pRecord->m_iRefCount = 0;
						m_pRecord->m_dwExtraMemoryLen = 0;
						WK_CLOSE_HANDLE(m_pRecord->m_hExtraMemory);
					}
					else 
					{
						m_pRecord->m_bIsMapped = TRUE;
					}
				} 
				else 
				{	// no handle
					WK_TRACE(_T("CIPC %s:FAILED CMD=%s CreateFileMapping %s,%d, %s\n"),
						m_pCipc->GetShmName(),
						CIPC::NameOfCmd(m_pCipc->GetLastCmd()),
						sName,
						dwLength,
						GetLastErrorString());
				}
				iNumRetries++;
			}
			m_pRecord->Unlock();
		} // 0 Len
	} // NULL CIPC

	BOOL bRet = m_pRecord && 
				m_pRecord->m_hExtraMemory &&
				m_pRecord->m_pExtraMemory &&
				m_pRecord->m_bIsMapped;

	if (!bRet)
	{
		WK_TRACE(_T("CIPC %s: FAILED to map extra memory RW of size %d, error %s\n"),
			m_pCipc->GetShmName(), 
			dwLength,
			LPCTSTR(GetLastErrorString()));
		WK_DELETE(m_pRecord);
		InterlockedDecrement((LONG*)&m_iNumCreated); // m_iNumCreated--;
	}

	return bRet;
}

//////////////////////////////////////////////////////
CIPCExtraMemory::CIPCExtraMemory(const CIPCChannel *pReadChannel)
{
	if (pReadChannel==NULL) 
	{
		WK_TRACE(_T("CIPCExtraMemory: internal error no readChannel\n"));
		return;	// EXIT
	}

	m_pCipc = pReadChannel->m_pParent;
	InterlockedIncrement((LONG*)&m_iNumCreated); // m_iNumCreated++;
	CheckMax();
	// INIT
	m_bReadOnly=FALSE;
	
	m_pRecord = new CIPCExtraMemoryRecord;
	m_pRecord->Lock();
	m_pRecord->m_iRefCount=1;
	// the writing process has created a file mapping,
	// which handle is stored in the channel hExtraMemory
	// using DuplicateHandle, the reading process gains access 
	// to that memory. 
	if (pReadChannel->m_hWriteProcess) 
	{
		int iNumRetries=0;
		BOOL bCopyOkay=FALSE;
		while (bCopyOkay==FALSE && iNumRetries<5) 
		{
			bCopyOkay = DuplicateHandle(
				pReadChannel->m_hWriteProcess,		// source process
				pReadChannel->m_pSharedMemoryStruct->hExtraMemory,	// source handle
				GetCurrentProcess(),				// target process
				&m_pRecord->m_hExtraMemory,	// target handle
				FILE_MAP_READ,		// 
				FALSE,						// inherit flag
				0							// options
				);		
			if (bCopyOkay==FALSE)
			{
				WK_TRACE(_T("CIPC %s: FAILED to copy extra mem handle error %s try %d\n"),
								m_pCipc->GetShmName(),
								LPCTSTR(GetLastErrorString()), iNumRetries);
				Sleep(3);
				iNumRetries++;
			}
		}	// end of retry loop
		if (bCopyOkay) 
		{
			// and do the real mapping
			m_pRecord->m_dwExtraMemoryLen = pReadChannel->m_pSharedMemoryStruct->dwExtraMemUsed;
			DoMapping();
		} 
		else 
		{
			/// message already printed in retry loop
		}
	} 
	else
	{
		WK_TRACE(_T("CIPC %s:NO write-process handle!?\n"),m_pCipc->GetShmName());
	}
	m_pRecord->Unlock();
}
//////////////////////////////////////////////////////
/*Function:T{fast!} copy constructor using reference counting. Thre created
bubble is read-only */
CIPCExtraMemory::CIPCExtraMemory(const CIPCExtraMemory &src)
{
	WK_ASSERT(&src);
	WK_ASSERT(src.m_pRecord);

	src.m_pRecord->Lock();
	//
	m_pCipc = src.m_pCipc;
	InterlockedIncrement((LONG*)&m_iNumCreated); // m_iNumCreated++;
	CheckMax();
	m_bReadOnly=TRUE;
	// ref counting
	m_pRecord = src.m_pRecord;

	m_pRecord->m_iRefCount++;
	// that's it
	src.m_pRecord->Unlock();
}
/* 
 See also {CIPC::CopyExtraMemory}(const void *pData)
*/
//////////////////////////////////////////////////////
/*
Function:
The associated memory is released if the last reference is deleted.
 {CIPCExtraMemory::CIPCExtraMemory}
*/
CIPCExtraMemory::~CIPCExtraMemory()
{
	InterlockedDecrement((LONG*)&m_iNumCreated); // m_iNumCreated--;
	if (m_pRecord) 
	{
		m_pRecord->Lock();
		m_pRecord->m_iRefCount--;
		if (m_pRecord->m_iRefCount==0) 
		{
			if (m_pRecord->m_pExtraMemory) 
			{
				UnmapViewOfFile(m_pRecord->m_pExtraMemory);
			}
			if (m_pRecord->m_hExtraMemory) 
			{
				WK_CLOSE_HANDLE(m_pRecord->m_hExtraMemory);
			}		
			m_pRecord->Unlock();
			WK_DELETE(m_pRecord);
		}	// end of refCount==0
		else
		{
			m_pRecord->Unlock();
		}
	}
}
//////////////////////////////////////////////////////
void CIPCExtraMemory::DoMapping() const
{
	if (   m_pRecord
		&& m_pRecord->m_bIsMapped==FALSE) 
	{
		m_pRecord->Lock();
		// do the mapping
		m_pRecord->m_pExtraMemory = MapViewOfFile(m_pRecord->m_hExtraMemory,
													FILE_MAP_READ,
													0, 
													0,	// offsets
													m_pRecord->m_dwExtraMemoryLen);
		if (m_pRecord->m_pExtraMemory==NULL) 
		{
			WK_TRACE(_T("CIPC %s:FAILED to map extra memory RO %08lx error %s %d bytes\n"), 
				m_pCipc->GetShmName(),
				m_pRecord->m_hExtraMemory,
				LPCTSTR(GetLastErrorString()),
				m_pRecord->m_dwExtraMemoryLen);
		} 
		else 
		{
			m_pRecord->m_bIsMapped=TRUE;
		}

		m_pRecord->Unlock();
	}
}
//////////////////////////////////////////////////////
const void* CIPCExtraMemory::GetAddress() const 
{ 
	if (m_pRecord)
	{
		if (!m_pRecord->m_bIsMapped) 
		{
			DoMapping();
		}
		return m_pRecord->m_pExtraMemory; 
	}
	else
	{
		return NULL;
	}
}
//////////////////////////////////////////////////////
/*{DWORD CIPCExtraMemory::GetLength() const}
Returns the size of the associated memory chunk.
 {CIPCExtraMemory::GetAddress}, {CIPCExtraMemory::GetAddressForWrite}

{const void *CIPCExtraMemory::GetAddress() const}
Returns the address of the associated memory chunk for reading.
It is a @BOLD const void * @NORMAL, and it is meant as such.

To get a writable pointer use {CIPCExtraMemory::GetAddressForWrite}()
 {CIPCExtraMemory::GetLength}
*/
//////////////////////////////////////////////////////
/*Function:
Returns the address of the associated memory chunk. If
the chunk is read-only, @CW{NULL} is returned!

 {CIPCExtraMemory::GetAddress}, {CIPCExtraMemory::GetLength}
*/
void* CIPCExtraMemory::GetAddressForWrite() const
{ 
	if (m_bReadOnly==FALSE)
	{
		return m_pRecord->m_pExtraMemory; 
	} 
	else
	{
		WK_TRACE(_T("CIPC %s: FAILED GetAddressForWrite on ReadOnly memory!\n"),
					m_pCipc->GetShmName());
		return NULL;
	}
}
/*Function:
Copies dwDataLen bytes from pData into the extra memory.
Fails if there is no data or dwDataLen is larger than the allocated extra memory.
*/
BOOL CIPCExtraMemory::SetMemoryFrom(const void *pData, DWORD dwDataLen)
{
	BOOL bRet = FALSE;

	if (m_pRecord!=NULL)
	{
		m_pRecord->Lock();
		if (dwDataLen>0)
		{
			if (pData!=NULL)
			{
				if (dwDataLen == m_pRecord->m_dwExtraMemoryLen)
				{
					if (!m_bReadOnly)
					{
						if (!IsBadReadPtr(pData,dwDataLen))
						{
							if (!IsBadWritePtr(m_pRecord->m_pExtraMemory,
											   m_pRecord->m_dwExtraMemoryLen)
								)
							{
								CopyMemory(m_pRecord->m_pExtraMemory,pData,dwDataLen);
								bRet = TRUE;
							}
							else
							{
								WK_TRACE_ERROR(_T("SetMemoryFrom BadWritePtr\n"));
							}
						}
						else
						{
							WK_TRACE_ERROR(_T("SetMemoryFrom BadReadPtr\n"));
						}
					}
					else
					{
						WK_TRACE_ERROR(_T("SetMemoryFrom read only\n"));
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("SetMemoryFrom wrong size\n"));
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("SetMemoryFrom no data\n"));
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("SetMemoryFrom data len 0\n"));
		}
		m_pRecord->Unlock();
	}
	else
	{
		WK_TRACE_ERROR(_T("SetMemoryFrom no record\n"));
	}

	if (!bRet)
	{
		if (m_pCipc)
		{
			WK_TRACE(_T("CIPC %s, %s\n"), m_pCipc->GetShmName(), CIPC::NameOfCmd(m_pCipc->GetLastCmd()));
		}
	}

	return bRet;
}

/*Function: NOT YET DOCUMENTED */
CString CIPCExtraMemory::GetString() const
{
	int nLen = GetLength();
#ifdef _UNICODE
	CWK_String result;
	ASSERT(m_pCipc != NULL);
	BOOL bUnicode = (m_pCipc->GetCodePage() == CODEPAGE_UNICODE) ? TRUE : FALSE;
	result.InitFromMemory(bUnicode, GetAddress(), nLen, m_pCipc->GetCodePage());
//	TRACE(_T("GetString:%s\n"), result);
#else
	CString result((LPCTSTR)GetAddress(), nLen);
#endif
//	WK_TRACE(_T("CIPCExtraMemory::GetString(%s)\n"), result);
	return result;
}

// NOT YET magic byte
/*Function: NOT YET DOCUMENTED */
CStringArray * CIPCExtraMemory::GetStrings() const
{
	CStringArray *pResult = new CStringArray;

	if (GetAddress()==NULL)
	{
		WK_TRACE(_T("No input for GetStrings()\n"));
	}
	else if (GetLength()==0)
	{
		WK_TRACE(_T("Empty input for GetStrings()\n"));
	} 
	else
	{
//		char *tmp = new char[GetLength()+1];	// max string size
		const DWORD *pDWHeader = (const DWORD *)GetAddress();
		const char *pRead = (const char *)GetAddress();
		int iNumStrings = pDWHeader[0];
		DWORD dwOffset=0;

		const char *pTemp;
		CWK_String newString;
#ifdef _UNICODE
		ASSERT(m_pCipc != NULL);
		BOOL bUnicode = (m_pCipc->GetCodePage() == CODEPAGE_UNICODE) ? TRUE : FALSE;
#endif
	
		for (int i=0;i<iNumStrings;i++) 
		{
			DWORD len = pDWHeader[i+1];	// skip num field
			if (len) 
			{
#ifdef _UNICODE
				pTemp = pRead+sizeof(DWORD)*(iNumStrings+1)+dwOffset;
				newString.InitFromMemory(bUnicode, (void*)pTemp, len, m_pCipc->GetCodePage());
//				TRACE(_T("%s\n"), newString);
				dwOffset += pDWHeader[i+1];
// gf todo subject to change
#else
				pTemp = pRead+sizeof(DWORD)*(iNumStrings+1)+dwOffset;
				CopyMemory(newString.GetBufferSetLength(len), pTemp, len);
				newString.ReleaseBuffer();
				dwOffset += pDWHeader[i+1];
#endif
				pResult->Add(newString);
//				WK_TRACE(_T("CIPCExtraMemory::GetStrings(%d, %s)\n"), i, (LPCTSTR)newString);
/*
				CopyMemory(tmp,
						pRead+sizeof(DWORD)*(iNumStrings+1)+dwOffset,
						len);
				tmp[len]=0;	// terminate string
				dwOffset += pDWHeader[i+1];
				// create CString and add to result
				CString newString = tmp;
				pResult->Add(newString);
*/
			}
			else
			{
				pResult->Add(_T(""));
			}
		}

//		WK_DELETE_ARRAY(tmp);
	}
	
	return pResult;
}

///////////////////////////////////////////////
// bubble help code
void CIPCExtraMemory::BYTE2Memory(BYTE*& pByte, BYTE b)
{
	*pByte++ = b;
}
///////////////////////////////////////////////
BYTE CIPCExtraMemory::Memory2BYTE(const BYTE*& pByte)
{
	BYTE b = (BYTE)pByte[0];
	pByte ++;
	return b;
}
///////////////////////////////////////////////
void CIPCExtraMemory::WORD2Memory(BYTE*& pByte, WORD w)
{
	*pByte++ = HIBYTE(w);
	*pByte++ = LOBYTE(w);
}
///////////////////////////////////////////////
WORD CIPCExtraMemory::Memory2WORD(const BYTE*& pByte)
{
	WORD w = (WORD)(((WORD)pByte[0])<<8 | pByte[1]);
	pByte += 2;
	return w;
}
///////////////////////////////////////////////
void CIPCExtraMemory::DWORD2Memory(BYTE*& pByte, DWORD dw)
{
	*pByte++ = HIBYTE(HIWORD(dw));
	*pByte++ = LOBYTE(HIWORD(dw));
	*pByte++ = HIBYTE(LOWORD(dw));
	*pByte++ = LOBYTE(LOWORD(dw));
}
///////////////////////////////////////////////
DWORD CIPCExtraMemory::Memory2DWORD(const BYTE*& pByte)
{
	DWORD dw = (((DWORD)pByte[0])<<24) | (((DWORD)pByte[1])<<16)
		| (((DWORD)pByte[2])<<8) | (((DWORD)pByte[3]));
	pByte += 4;
	return dw;
}
///////////////////////////////////////////////
void CIPCExtraMemory::ULARGE_INTEGER2Memory(BYTE*& pByte, ULARGE_INTEGER ul)
{
	DWORD2Memory(pByte,ul.LowPart);
	DWORD2Memory(pByte,ul.HighPart);
}
///////////////////////////////////////////////
ULARGE_INTEGER CIPCExtraMemory::Memory2ULARGE_INTEGER(const BYTE*& pByte)
{
	ULARGE_INTEGER ul;

	ul.LowPart = Memory2DWORD(pByte);
	ul.HighPart = Memory2DWORD(pByte);

	return ul;
}
///////////////////////////////////////////////
#ifdef _UNICODE
/*********************************************************************************************
 Class      : CIPCExtraMemory
 Function   : CString2Memory
 Description: Copies string data to a memory block

 Parameters:   
  pByte    : (E): Pointer to Memory receiving the strings data  (BYTE*&)
  s        : (E): Source string  (const CString&)
  wCodePage: (E): Codepage for conversion from/to Unicode  (WORD)

 Result type:  (void)
 created: June, 24 2003
 <TITLE CString2Memory>
*********************************************************************************************/
WORD CIPCExtraMemory::CString2Memory(BYTE*& pByte, const CString &s, WORD wCodePage)
{	
	CWK_String swk(s);
	BYTE*pbLen = pByte;						// save pointer for string length
	if (pByte) pByte += sizeof(WORD);	// increment string pointer by WORD size

	WORD w = (WORD) swk.CopyToMemory(wCodePage == CODEPAGE_UNICODE, pByte, -1, wCodePage);
	if (pByte) 
	{
		WORD2Memory(pbLen, w);				// copy the lenght
		pByte += w;								// increment the string pointer by copied bytes
	}
	return w;
}
#else
void CIPCExtraMemory::CString2Memory(BYTE*& pByte, const CString &s)
{	
	WORD w = (WORD)(s.GetLength());
	WORD2Memory(pByte, w);
	CopyMemory(pByte, LPCTSTR(s), w);
	pByte += w;
}	
#endif

/*********************************************************************************************
 Class      : CIPCExtraMemory
 Function   : Memory2CString
 Description: Converts the data in the memory block to a string.

 Parameters:   
  pByte   : (E): Pointer to memory with string data.			(const BYTE*&)
                 The first WORD contains the string length. The memory pointer pByte is
					  incremented by the number of bytes of the memory block.
  wCodePage:(E): Determines whether the string data contains a Unicode or MBCS string. (WORD)

 Result type:  (CString)
 <TITLE Memory2CString>
*********************************************************************************************/
#ifdef _UNICODE
CString CIPCExtraMemory::Memory2CString(const BYTE*& pByte, WORD wCodePage)
{
	WORD w = Memory2WORD(pByte);
	CWK_String sTemp;
	if (w>0)
	{
		sTemp.InitFromMemory(wCodePage == CODEPAGE_UNICODE, pByte, w, wCodePage);
		pByte += w;
	}
	return sTemp;
}
#else
CString CIPCExtraMemory::Memory2CString(const BYTE*& pByte)
{
	WORD w = Memory2WORD(pByte);
	CWK_String sTemp;
	if (w>0)
	{
		CopyMemory(sTemp.GetBuffer(w), pByte, w);
		sTemp.ReleaseBuffer();
		pByte += w;
	}
	return sTemp;
}
#endif
int CIPCExtraMemory::GetCreatedBubbles()
{
	return m_iNumCreated;
}
