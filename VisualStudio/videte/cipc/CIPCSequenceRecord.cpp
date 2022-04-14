// CIPCSequenceRecord.cpp: implementation of the CIPCSequenceRecord class.
//
//////////////////////////////////////////////////////////////////////
#include "stdcipc.h"

#include "CIPC.h"
#include "CIPCExtraMemory.h"
#include "CIPCSequenceRecord.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCSequenceRecord::CIPCSequenceRecord()
{
	m_wArchivNr = 0;
	m_wSequenceNr = 0;
	m_dwNrOfPictures = 0;
	m_iSize = 0;
	m_bFlags = 0;
	// m_sName;
	// m_Time;
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecord::CIPCSequenceRecord(const CIPCSequenceRecord& src)
{
	*this = src;
}
//////////////////////////////////////////////////////////////////////
void CIPCSequenceRecord::operator = (const CIPCSequenceRecord& src)
{
	m_wArchivNr = src.m_wArchivNr;
	m_wSequenceNr = src.m_wSequenceNr;
	m_iSize = src.m_iSize;
	m_dwNrOfPictures = src.m_dwNrOfPictures;
	m_bFlags = src.m_bFlags;
	m_sName = src.m_sName;
	m_Time = src.m_Time;
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecord::~CIPCSequenceRecord()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CIPCSequenceRecord::operator < (const CIPCSequenceRecord& s)
{
	return m_Time < s.m_Time;
}
//////////////////////////////////////////////////////////////////////
void CIPCSequenceRecord::SetName(const CString &sName)
{
	m_sName = sName;
}
//////////////////////////////////////////////////////////////////////
void CIPCSequenceRecord::SetArchiveNr(WORD wNr)
{
	m_wArchivNr = wNr;
}
//////////////////////////////////////////////////////////////////////
void CIPCSequenceRecord::SetSequenceNr(WORD wNr)
{
	m_wSequenceNr = wNr;
}
//////////////////////////////////////////////////////////////////////
void CIPCSequenceRecord::SetFlags(BYTE bNewFlags)
{
	m_bFlags = bNewFlags;
}
//////////////////////////////////////////////////////////////////////
void CIPCSequenceRecord::SetNrOfPictures(DWORD dwNrOfPictures)
{
	m_dwNrOfPictures = dwNrOfPictures;
}
//////////////////////////////////////////////////////////////////////
void CIPCSequenceRecord::SetSize(CIPCInt64 iSize)
{
	m_iSize = iSize;
}
//////////////////////////////////////////////////////////////////////
void CIPCSequenceRecord::SetTime(const CSystemTime& t)
{
	m_Time = t;
}
//////////////////////////////////////////////////////////////////////
CIPCExtraMemory* CIPCSequenceRecord::BubbleFromSequenceRecords(CIPC *pCipc,
															   int iNumRecords,
															   const CIPCSequenceRecord records[])
{
	if (iNumRecords == 0)
	{
		return NULL;
	}
	// 2 archiv nr
	// 2 sequence nr
	// 4 nr of pictures
	// 8 size
	// 16 time
	// 1 flags
	// 4 name len
	// name

	int i;
	DWORD dwSum = 0;
	
	// calc size of bubble
#ifdef _UNICODE
	BOOL bUnicode = pCipc->GetCodePage() == CODEPAGE_UNICODE;
	for (i=0;i<iNumRecords;i++) 
	{
		const CWK_String &sName = records[i].GetName();
		dwSum += sizeof(WORD)   // archiv nr
				+sizeof(WORD)   // sequence nr
				+sizeof(DWORD)	// nr of pictures
				+8*sizeof(BYTE)	// size
				+8*sizeof(WORD) // time
				+sizeof(BYTE)	// flags
				+sizeof(DWORD)	// string length
				+sName.CopyToMemory(bUnicode, NULL, -1, pCipc->GetCodePage());
	}
#else
	for (i=0;i<iNumRecords;i++) 
	{
		dwSum += sizeof(WORD)   // archiv nr
				+sizeof(WORD)   // sequence nr
				+sizeof(DWORD)	// nr of pictures
				+8*sizeof(BYTE)	// size
				+8*sizeof(WORD) // time
				+sizeof(BYTE)	// flags
				+sizeof(DWORD)	// string length
				+records[i].GetName().GetLength();
	}
#endif				
	CIPCExtraMemory *pBubble = new CIPCExtraMemory();
	
	if (pBubble->Create(pCipc,dwSum))
	{
		BYTE *pByte = (BYTE*)pBubble->GetAddressForWrite();
		BYTE b;
		CSystemTime t;

		for (i=0;i<iNumRecords;i++) 
		{
			const CIPCSequenceRecord &seq=records[i];
			// archiv nr
			WORD_TO_MEMORY(pByte, seq.GetArchiveNr());
			// sequence nr		
			WORD_TO_MEMORY(pByte, seq.GetSequenceNr());
			// nr of pictures		
			DWORD_TO_MEMORY(pByte, seq.GetNrOfPictures());
			// size
			__int64 tmpSize = seq.GetSize().GetInt64();
			for (int j=0;j<8;j++) {
				*pByte++ = (BYTE)(tmpSize & 255);	// read one byte
				tmpSize >>= 8;	// and shift
			}

			t = seq.GetTime();
			WORD_TO_MEMORY(pByte, t.wYear);
			WORD_TO_MEMORY(pByte, t.wMonth);
			WORD_TO_MEMORY(pByte, t.wDayOfWeek);
			WORD_TO_MEMORY(pByte, t.wDay);
			WORD_TO_MEMORY(pByte, t.wHour);
			WORD_TO_MEMORY(pByte, t.wMinute);
			WORD_TO_MEMORY(pByte, t.wSecond);
			WORD_TO_MEMORY(pByte, t.wMilliseconds);
/*
			*pByte++ = HIBYTE(t.wYear);
			*pByte++ = LOBYTE(t.wYear);
			*pByte++ = HIBYTE(t.wMonth);
			*pByte++ = LOBYTE(t.wMonth);
			*pByte++ = HIBYTE(t.wDayOfWeek);
			*pByte++ = LOBYTE(t.wDayOfWeek);
			*pByte++ = HIBYTE(t.wDay);
			*pByte++ = LOBYTE(t.wDay);
			*pByte++ = HIBYTE(t.wHour);
			*pByte++ = LOBYTE(t.wHour);
			*pByte++ = HIBYTE(t.wMinute);
			*pByte++ = LOBYTE(t.wMinute);
			*pByte++ = HIBYTE(t.wSecond);
			*pByte++ = LOBYTE(t.wSecond);
			*pByte++ = HIBYTE(t.wMilliseconds);
			*pByte++ = LOBYTE(t.wMilliseconds);
*/
			// flags		
			b = seq.GetFlags();
			*pByte++ = b;
			// name
#ifdef _UNICODE
			const CWK_String &sName = seq.GetName();
			BYTE* pbLen = pByte;
			pByte += sizeof(DWORD);
			DWORD dwLen = sName.CopyToMemory(bUnicode, pByte, -1, pCipc->GetCodePage());
			pByte += dwLen;
			DWORD_TO_MEMORY(pbLen, dwLen);
#else
			DWORD dw = seq.GetName().GetLength();
			*pByte++ = HIBYTE(HIWORD(dw));
			*pByte++ = LOBYTE(HIWORD(dw));
			*pByte++ = HIBYTE(LOWORD(dw));
			*pByte++ = LOBYTE(LOWORD(dw));
			CopyMemory(pByte, LPCTSTR(seq.GetName()), dw);
			pByte += dw;
#endif
		}
		DWORD dwDelta=pByte-(BYTE*)pBubble->GetAddressForWrite();
		WK_ASSERT(dwDelta==dwSum);
	}
	else
	{
		WK_DELETE(pBubble);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in BubbleFromSequenceRecords\n"));
	}
	return pBubble;
}
//////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CIPCSequenceRecord::SequenceRecordsFromBubble(int iNumRecords,
										  const CIPCExtraMemory *pExtraMem)
{
	// 2 archiv nr
	// 2 sequence nr
	// 4 nr of pictures
	// 8 size
	// 16 time
	// 1 flags
	// 4 name len
	// name

	CIPCSequenceRecord* pData = NULL;
	const BYTE *pByte = (const BYTE *)pExtraMem->GetAddress();
	DWORD dw;
	WORD  w;
	CSystemTime t;

	if (iNumRecords>0)
	{
		pData = new CIPCSequenceRecord[iNumRecords];
		// loop over records
		for (int i=0;i<iNumRecords;i++) 
		{
			CIPCSequenceRecord & seq = pData[i];
			// archiv nr
			w = (WORD)(((WORD)pByte[0])<<8 | pByte[1]);
			pByte +=2;
			seq.SetArchiveNr(w);
			// sequence nr
			w = (WORD)(((WORD)pByte[0])<<8 | pByte[1]);
			pByte +=2;
			seq.SetSequenceNr(w);
			// nr of pictures
			dw = (((DWORD)pByte[0])<<24) | (((DWORD)pByte[1])<<16)
				| (((DWORD)pByte[2])<<8) | (((DWORD)pByte[3]));
			pByte += 4;
			seq.SetNrOfPictures(dw);
			// size
			_int64 tmpSize=0;
			for (int j=0;j<8;j++) {
				__int64 tmpByte = *pByte++;
				tmpSize |= (tmpByte << (j*8));
			}
			seq.SetSize(tmpSize);
			t.wYear         = (WORD)(((WORD)pByte[ 0])<<8 | pByte[ 1]);
			t.wMonth        = (WORD)(((WORD)pByte[ 2])<<8 | pByte[ 3]);
			t.wDayOfWeek    = (WORD)(((WORD)pByte[ 4])<<8 | pByte[ 5]);
			t.wDay          = (WORD)(((WORD)pByte[ 6])<<8 | pByte[ 7]);
			t.wHour         = (WORD)(((WORD)pByte[ 8])<<8 | pByte[ 9]);
			t.wMinute       = (WORD)(((WORD)pByte[10])<<8 | pByte[11]);
			t.wSecond       = (WORD)(((WORD)pByte[12])<<8 | pByte[13]);
			t.wMilliseconds = (WORD)(((WORD)pByte[14])<<8 | pByte[15]);
			seq.SetTime(t);
			pByte += 16;

			// flags
			seq.SetFlags((BYTE)*pByte++);

			dw = (((DWORD)pByte[0])<<24) | (((DWORD)pByte[1])<<16)
				| (((DWORD)pByte[2])<<8) | (((DWORD)pByte[3]));
			pByte += 4;
			// name
			CWK_String sTemp;
			if (dw>0)
			{
#ifdef _UNICODE
				sTemp.InitFromMemory(pExtraMem->GetCIPC()->GetCodePage() == CODEPAGE_UNICODE, pByte, dw, pExtraMem->GetCIPC()->GetCodePage());
//				TRACE(_T("SequenceRecordsFromBubble:%s\n"), sTemp);
#else
				sTemp.InitFromMemory(pByte, dw);
#endif
				pByte += dw;
			}
			seq.SetName(sTemp);
		}
	}

	return pData;
}
/////////////////////////////////////////////////////////////////////////////
// CIPCSequenceRecords
/////////////////////////////////////////////////////////////////////////////
CIPCInt64 CIPCSequenceRecords::GetSizeBytes()
{
	CIPCInt64 r = 0;

	int i,c;
	CIPCSequenceRecord* pSequence;

	Lock();

	c = GetSize();
	for (i=0;i<c;i++)
	{
		pSequence = GetAtFast(i);
		r += pSequence->GetSize();
	}

	Unlock();

	return r;
}
/////////////////////////////////////////////////////////////////////////////
CIPCSequenceRecord*	CIPCSequenceRecords::GetSequence(WORD wArchivNr, WORD wSequenceNr)
{
	int i,c;
	CIPCSequenceRecord* pSequence;

	Lock();

	c = GetSize();
	for (i=0;i<c;i++)
	{
		pSequence = GetAtFast(i);
		if ((pSequence->GetArchiveNr()==wArchivNr) &&
			(pSequence->GetSequenceNr()==wSequenceNr))
		{
			Unlock();
			return pSequence;
		}
	}

	Unlock();

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCSequenceRecords::RemoveSequence(WORD wArchivNr, WORD wSequenceNr)
{
	int i,c;
	CIPCSequenceRecord* pSequence;

	Lock();

	c = GetSize();
	for (i=0;i<c;i++)
	{
		pSequence = GetAtFast(i);
		if ((pSequence->GetArchiveNr()==wArchivNr) &&
			(pSequence->GetSequenceNr()==wSequenceNr))
		{
			RemoveAt(i);
			WK_DELETE(pSequence);
			Unlock();
			return TRUE;
		}
	}

	Unlock();

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CIPCSequenceRecords::Sort()
{
	int iCur, iMin, iNext, nCount;
	CIPCSequenceRecord* swap;
	
	Lock();
	
	nCount = GetSize();
	for (iCur=0; iCur<nCount; iCur++)
	{
		iMin = iCur;
		for (iNext=iCur; iNext<nCount; iNext++)
		{
			if (*GetAtFast(iNext) < *GetAtFast(iMin))
			{
				iMin = iNext;
			}
		}
		// wenn ein element kleiner als das aktuelle ist, tausche die elemente.
		if (iMin>iCur)
		{
			swap = GetAtFast(iCur);
			SetAt(iCur,GetAtFast(iMin));
			SetAt(iMin,swap);
		}
	}

	Unlock();
}
