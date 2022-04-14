/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CircularBuffer.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDll/CircularBuffer.h $
// CHECKIN:		$Date: 26.08.04 9:21 $
// VERSION:		$Revision: 19 $
// LAST CHANGE:	$Modtime: 24.08.04 12:20 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIRCULARBUFFER_H__FE51675A_25C8_40E3_8B00_C05742EFACC4__INCLUDED_)
#define AFX_CIRCULARBUFFER_H__FE51675A_25C8_40E3_8B00_C05742EFACC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "memcopy.h"

class CWK_PerfMon;
class CCircularBuffer  
{
public:
	CCircularBuffer(int nChannel, DWORD dwStartMarker, DWORD dwEndMarker, int nMaxBufferSize);
	virtual ~CCircularBuffer();

	BOOL Add(void* pBuffer, int nBufferLen);
	void* GetNextFrame();
	void Flush();
	int FastFind(int nStart, BYTE* pSrc, DWORD dwLen);

private:
	CCriticalSection	m_csCirBuff;

	int					m_nCurrentBufferLen;	// Anzhal der Datenbytes, die sich aktuell im Buffer befinden
	int					m_nAllocSize;			// Buffergröße des allozierten Speichers
	int					m_nFrameStart;			// Index auf das 
	int					m_nChannel;				// Channelnummer. (Nur zu Debug zwecken)
	DWORD				m_dwStartMarker;
	DWORD				m_dwEndMarker;
	BYTE*				m_pBuffer;
	CMemCopy			m_MemCpy;
	DWORD				m_dwLastSuccessTC;		// Zeitpunkt des letzten erfolgreichen 'Add'
};

#endif // !defined(AFX_CIRCULARBUFFER_H__FE51675A_25C8_40E3_8B00_C05742EFACC4__INCLUDED_)
