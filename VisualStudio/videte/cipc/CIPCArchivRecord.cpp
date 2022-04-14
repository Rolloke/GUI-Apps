// CIPCArchivRecord.cpp: implementation of the CIPCArchivRecord1 class.
//
//////////////////////////////////////////////////////////////////////
#include "stdcipc.h"

#include "CIPC.h"
#include "CIPCExtraMemory.h"
#include "ArchivDefines.h"
#include "CIPCArchivRecord.h"

/*{CIPCArchivRecord Overview|Overview,CIPCArchivRecord}
 {Members|CIPCArchivRecord}, {CIPCDatabase}

NOT YET DOCUMENTED
*/

/*
 
 {overview|Overview,CIPCArchivRecord},	{CIPCDataBase}
*/

/*
{CIPCArchivRecord::CIPCArchivRecord()}
default constructor setting SECID_NO_SUBID.
*/

// INLINED functions documented here!

/*{access:}
{BYTE CIPCArchivRecord::GetFlags() const}
{CAR_IS_ALARM}{CAR_IS_RING}{CAR_IS_SAFE_RING}
 returns the flags of the archive.
bitflags are:
 CAR_IS_ALARM
 CAR_IS_RING
 CAR_IS_SAFE_RING

{WORD CIPCArchivRecord::GetID() const}
 returns the id of the archive.

{BOOL CIPCArchivRecord::IsRing() const}
 returns the TRUE if the archive is a ring.

{const CString & CIPCArchivRecord::GetName() const}
 returns the name of the archive.
*/

/*Function:
 returns the id of the associated ring archive
or SECID_NO_SUBID.
*/

/*{ArchivSize	CIPCArchivRecord::BenutzterPlatz() const}
NOT YET documented
*/
/*{DWORD CIPCArchivRecord::SizeInMB() const}
NOT YET documented
*/

/*{checks:}*/
/*{BOOL CIPCArchivRecord::IsAlarm() const}
 returns TRUE if the archive is an alarm archive.

*/

/*Function:  returns TRUE if the archive is a SafeRing.*/
//
/*Function: allows to modify flags of an archive record. */
CIPCArchivRecord::CIPCArchivRecord()
{
	// m_sName
	m_bFlags = 0;
	m_wID = SECID_NO_SUBID;
	m_wSafeRingID = SECID_NO_SUBID;
	m_dwSizeInMB	=	0;
	m_AsBenutzt		=	0;
}

CIPCArchivRecord::CIPCArchivRecord(const CIPCArchivRecord&src)
{
	m_sName			= src.m_sName;
	m_wID			= src.m_wID;
	m_wSafeRingID	= src.m_wSafeRingID;
	m_dwSizeInMB	= src.m_dwSizeInMB;
	m_AsBenutzt		= src.m_AsBenutzt;
	m_bFlags		= src.m_bFlags;
}

void CIPCArchivRecord::SetFlags(BYTE bNewFlags)
{ 
	// NOT YET, check for valid flags
	m_bFlags = bNewFlags;
}
/*Function: allows to set the name of the archive */
void CIPCArchivRecord::SetName(const CString &sName)
{
	m_sName = sName;
}
/*Function: allows to set the id of archive*/
void CIPCArchivRecord::SetID(WORD wID)
{
	m_wID = wID;
}
/*Function: allows to set the id of the associated safe ring */
void CIPCArchivRecord::SetSafeRingID(WORD wSafeID)
{
	m_wSafeRingID = wSafeID;
}
/*Function:
*/
void CIPCArchivRecord::SetSizeInMB(DWORD dwSize) 
{ 
	m_dwSizeInMB = dwSize; 
}

/*Function:
*/
void CIPCArchivRecord::SetBenutzterPlatz(ArchivSize as)
{
	m_AsBenutzt = as;
}
/////////////////////////////////////////////////////////////////////////////////
CIPCExtraMemory* CIPCArchivRecord::BubbleFromArchivRecords(CIPC *pCipc,
														   int iNumRecords,
														   const CIPCArchivRecord data[])
{
	int i;
	DWORD dwSum=0;
#ifdef _UNICODE
	ASSERT(pCipc != NULL);
	BOOL bUnicode = (pCipc->GetCodePage() == CODEPAGE_UNICODE);
#endif
	int  *pnLen = (int*) _alloca(sizeof(int)*iNumRecords);
	// calc size of bubble
	for (i=0;i<iNumRecords;i++) 
	{
#ifdef _UNICODE
		const CWK_String &name = data[i].GetName();
		pnLen[i] = name.CopyToMemory(bUnicode, NULL, -1, pCipc->GetCodePage());
#else
		pnLen[i] = data[i].GetName().GetLength();
#endif
		WK_ASSERT(pnLen[i]<=255);
		dwSum += 2*sizeof(WORD) // ids
				+sizeof(BYTE)	// flags
				+sizeof(DWORD)	// sizeMB
				+8*sizeof(BYTE)	// size used is an int64
				+1				// _tcslen
				+pnLen[i];
	}
	
	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	if (pBubble->Create(pCipc,dwSum))
	{
		BYTE *pByte = (BYTE*) pBubble->GetAddressForWrite();
		if (pByte)
		{
			for (i=0;i<iNumRecords;i++) 
			{
				const CIPCArchivRecord &arc=data[i];
				WORD_TO_MEMORY(pByte, arc.GetID());
				WORD_TO_MEMORY(pByte, arc.GetSafeRingID());
				//
				*pByte++ = arc.GetFlags();
				//
				DWORD_TO_MEMORY(pByte, arc.SizeInMB());
				// scan int64
				_int64 tmpSize = arc.BenutzterPlatz();
				for (int b=0;b<8;b++) 
				{
					*pByte++ = (BYTE)(tmpSize & 255);	// read one byte
					tmpSize >>= 8;	// and shift
				}
				//
				const CWK_String &name = arc.GetName();
#ifdef _UNICODE
				*pByte++ = (BYTE)pnLen[i];
				name.CopyToMemory(bUnicode, pByte, -1, pCipc->GetCodePage());
				pByte += pnLen[i];
#else
				*pByte++ = (BYTE)pnLen[i];
				CopyMemory(pByte, LPCTSTR(name), pnLen[i]);
				pByte += pnLen[i];
#endif
			}
			DWORD dwDelta=pByte-(BYTE*)pBubble->GetAddressForWrite();
			WK_ASSERT(dwDelta==dwSum);
		}
		else
		{
			// TODO
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in BubbleFromArchivRecords\n"));
		WK_DELETE(pBubble);
	}
	return pBubble;
}
/////////////////////////////////////////////////////////////////////////////////
CIPCArchivRecord* CIPCArchivRecord::ArchivRecordsFromBubble(int iNumRecords,
															const CIPCExtraMemory *pExtraMem)
{
	CIPCArchivRecord *pData = NULL;
	const BYTE *pByte = (const BYTE *)pExtraMem->GetAddress();
	if (pByte)
	{
		pData = new CIPCArchivRecord[iNumRecords];
#ifdef _UNICODE
		CWK_String tmpString;
		ASSERT(pExtraMem->GetCIPC() != NULL);
		BOOL bUnicode = (pExtraMem->GetCIPC()->GetCodePage() == CODEPAGE_UNICODE);
#else
		char tmpString[256];
#endif
		// loop over records
		for (int i=0;i<iNumRecords;i++) 
		{
			CIPCArchivRecord & rec = pData[i];
			rec.SetID((WORD)(((WORD)pByte[0])<<8 | pByte[1]));
			pByte += 2;
			rec.SetSafeRingID((WORD)(((WORD)pByte[0])<<8 | pByte[1]));
			pByte += 2;
			rec.SetFlags( *pByte++ );
			//
			rec.SetSizeInMB(
				  (((DWORD)pByte[0])<<24)
				| (((DWORD)pByte[1])<<16)
				| (((DWORD)pByte[2])<<8)
				| (((DWORD)pByte[3]))
				);
			pByte += 4;

			_int64 tmpSize=0;
			for (int b=0;b<8;b++) 
			{
				_int64 tmpByte = *pByte++;
				tmpSize |= (tmpByte << (b*8));
			}
			rec.SetBenutzterPlatz(tmpSize);
			//
			BYTE bLen = *pByte++;
#ifdef _UNICODE
			tmpString.InitFromMemory(bUnicode, (void*)pByte, bLen, pExtraMem->GetCIPC()->GetCodePage());
//			TRACE(_T("ArchivRecordsFromBubble%s\n"), tmpString);
			rec.SetName(tmpString);
			pByte += bLen;
#else
			int j=0;
			for (j=0;j<bLen && j<255;j++) 
			{
				tmpString[j] = *pByte++;
			}
			tmpString[j]=0;
			rec.SetName(tmpString);
#endif
//			WK_TRACE(_T("ArchivRecordsFromBubble Name:%s\n"), tmpString);
		}	// end of loop over records
	}

	return pData;
}
/////////////////////////////////////////////////////////////////////////////
// CIPCArchivRecords
/////////////////////////////////////////////////////////////////////////////
CIPCInt64 CIPCArchivRecords::GetSizeBytes()
{
	CIPCInt64 r = 0;

	int i,c;
	CIPCArchivRecord* pArchiv;

	Lock();

	c = GetSize();
	for (i=0;i<c;i++)
	{
		pArchiv = GetAtFast(i);
		r += pArchiv->BenutzterPlatz();
	}

	Unlock();

	return r;
}
/////////////////////////////////////////////////////////////////////////////
CIPCArchivRecord* CIPCArchivRecords::GetArchiv(WORD wArchivNr)
{
	int i,c;
	CIPCArchivRecord* pArchiv = NULL;

	Lock();

	c = GetSize();
	for (i=0;i<c;i++)
	{
		pArchiv = GetAtFast(i);
		if (pArchiv->GetID()==wArchivNr)
		{
			Unlock();
			return pArchiv;
		}
	}

	Unlock();

	return pArchiv;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCArchivRecords::RemoveArchiv(WORD wArchivNr)
{
	int i,c;
	CIPCArchivRecord* pArchiv;

	Lock();

	c = GetSize();
	for (i=0;i<c;i++)
	{
		pArchiv = GetAtFast(i);
		if (pArchiv->GetID()==wArchivNr)
		{
			RemoveAt(i);
			WK_DELETE(pArchiv);
			Unlock();
			return TRUE;
		}
	}

	Unlock();

	return FALSE;
}
