/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: CircularBuffer.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaDll/CircularBuffer.cpp $
// CHECKIN:		$Date: 11.11.04 12:20 $
// VERSION:		$Revision: 50 $
// LAST CHANGE:	$Modtime: 11.11.04 11:06 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CircularBuffer.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CCircularBuffer::CCircularBuffer(int nChannel, DWORD dwStartMarker, DWORD dwEndMarker, int nMaxBufferSize)
{
	m_nAllocSize			= nMaxBufferSize;
	m_nCurrentBufferLen		= 0;
	m_nFrameStart			= 0;
	m_dwStartMarker			= dwStartMarker;
	m_dwEndMarker			= dwEndMarker;
	m_dwLastSuccessTC		= WK_GetTickCount();
	m_nChannel				= nChannel;
	m_pBuffer				= new BYTE[m_nAllocSize+1024];
}

//////////////////////////////////////////////////////////////////////
CCircularBuffer::~CCircularBuffer()
{
	WK_DELETE(m_pBuffer);
}

//////////////////////////////////////////////////////////////////////
// CPU Auslstung auf 2.4GHz Pentium 4 ca. 134 ms/min pro channel
BOOL CCircularBuffer::Add(void* pBuffer, int nBufferLen)
{
	BOOL bResult = FALSE;
	m_csCirBuff.Lock();

	// Ist der Buffer schon groﬂ genug?
	if (m_nCurrentBufferLen + nBufferLen < m_nAllocSize)
	{
		m_MemCpy.FastCopy(&m_pBuffer[m_nCurrentBufferLen], pBuffer, nBufferLen);
		m_nCurrentBufferLen += nBufferLen;
		m_dwLastSuccessTC = WK_GetTickCount();
		bResult = TRUE;
	}
	else
	{
		// Nur ausgeben, wenn f¸r 10 Sekunden der Buffer voll ist.
		// Ansonsten w¸rden bis zu 200 Messages in der Sekunde im Logfile landen.
		if (WK_GetTimeSpan(m_dwLastSuccessTC) > 10000)
		{
			ML_TRACE_WARNING(_T("Circularbuffer is full for 10 seconds! dropping data (Channel=%d)\n"), m_nChannel);
			m_nCurrentBufferLen = 0;
			m_nFrameStart		= 0;
			m_dwLastSuccessTC = WK_GetTickCount();
		}
	}

/* F¸llstand ausgeben
	static double fAvWatermark	= 0.0;
	static DWORD  dwCounter		= 0;

	double fWatermark = 100.0 * ((double)m_nCurrentBufferLen / (double)m_nAllocSize);
	fAvWatermark += fWatermark;
	dwCounter++;

	if (dwCounter > 25)
	{
		TRACE(_T("Source=%d, Watermark=%d%%\n"),m_nChannel, (int)(fAvWatermark/dwCounter+0.5));
	
		dwCounter = 0;
		fAvWatermark = 0.0;
	}
*/
	m_csCirBuff.Unlock();

	return bResult;
}


//////////////////////////////////////////////////////////////////////
// CPU Auslstung auf 2.4GHz Pentium 4 ca.
// memmove: 16,4 ms/min pro channel
// Search:  259 ms/min pro cahnnel
void* CCircularBuffer::GetNextFrame()
{
	DATA_PACKET*	pPacket		= NULL;
	void*			pBuff		= NULL;
	int				nFrameEnd	= 0;
	m_csCirBuff.Lock();

	if (m_nCurrentBufferLen > 0)
	{
		// Suche den Magic Header, der einen Framebeginn markiert
		int nI = FastFind(m_nFrameStart, m_pBuffer, m_nCurrentBufferLen);
		if (nI >= 0)  
		{
			m_nFrameStart = nI;

			// Wenn der Framestart schon am Beginn ist, brauch der Frame nicht verschoben werden.
			if (m_nFrameStart > 0)
			{
				// Gefundenes Frame(segment) an den Anfang des Buffers schieben.
				memmove(&m_pBuffer[0], &m_pBuffer[m_nFrameStart], m_nCurrentBufferLen-m_nFrameStart);

				m_nCurrentBufferLen -= m_nFrameStart;
				m_nFrameStart = 0;
			}

			pPacket	 = (DATA_PACKET*)&m_pBuffer[0];
			
			// Ist der Header schon komplett im Buffer?
			if (sizeof(DATA_PACKET) <= m_nCurrentBufferLen)
			{
				// Ist der Endmarker ebenfalls vorhanden, wenn nicht, dann wurde der Header schon vom DMA
				// Kontroller ¸berschrieben!
				if (pPacket->dwEndMarker != m_dwEndMarker)
				{
					TRACE(_T("Endmarker not found...discard data (Slave=%d, StreamID=%d)\n"), pPacket->wSource, pPacket->dwStreamID);
					m_nCurrentBufferLen = 0;
					m_nFrameStart		= 0;
				}
				else
				{
					// Das Ende des Frames ermitteln
					nFrameEnd   = sizeof(DATA_PACKET) + pPacket->dwImageDataLen;

					// Ist das Frame schon komplett im Buffer?
					if ((DWORD)nFrameEnd <= (DWORD)m_nCurrentBufferLen)
					{
						// Die n‰chste Suche kann ab dem Ende des aktuellen Frames erfolgen
						m_nFrameStart = nFrameEnd;

						pPacket->dwStartMarker = 0; // Marker lˆschen
					
						// Adresse des gefundenen Frames zur¸ckliefern.
						pBuff = pPacket; 	
					}
				}
			}
		}
		else
		{
			// Es wurde im gesamten Buffer kein Startmarker gefunden, dann die letzten
			// 4 Bytes, die ja ein Fragment des n‰chsten Startmarkers enthalten kˆnnten, an
			// den Anfang des Buffers kopieren, und die L‰nge auf 4Bytes setzen.
			if (m_nCurrentBufferLen > 4)
			{
				memmove(&m_pBuffer[0], &m_pBuffer[m_nCurrentBufferLen-4], 4);
				m_nCurrentBufferLen = 4;
			}
			else if (m_nCurrentBufferLen > 3)
			{
				memmove(&m_pBuffer[0], &m_pBuffer[m_nCurrentBufferLen-3], 3);
				m_nCurrentBufferLen = 3;
			}
			else if (m_nCurrentBufferLen > 2)
			{
				memmove(&m_pBuffer[0], &m_pBuffer[m_nCurrentBufferLen-2], 2);
				m_nCurrentBufferLen = 2;
			}
			else if (m_nCurrentBufferLen > 1)
			{
				memmove(&m_pBuffer[0], &m_pBuffer[m_nCurrentBufferLen-1], 1);
				m_nCurrentBufferLen = 1;
			}
			else
			{
				m_nCurrentBufferLen = 0;
			}
			m_nFrameStart		= 0;
		}
	}

	m_csCirBuff.Unlock();
	return pBuff;
}

#define MAGIC_MARKER_START1 ((MAGIC_MARKER_START >> 24) & 0x000000ff)
#define MAGIC_MARKER_START2 ((MAGIC_MARKER_START >> 16) & 0x000000ff)
#define MAGIC_MARKER_START3 ((MAGIC_MARKER_START >> 8) & 0x000000ff)
#define MAGIC_MARKER_START4 ((MAGIC_MARKER_START >> 0) & 0x000000ff)

//////////////////////////////////////////////////////////////////////
int CCircularBuffer::FastFind(int nStart, BYTE* pSrc, DWORD dwLen)
{
	int nIndex = -1;
	if (nStart < (int)dwLen)
	{
		__asm{
			mov ecx, nStart
			mov esi, pSrc
		
		SearchLoop:
			cmp [esi+ecx+0], MAGIC_MARKER_START4
			jne cont
			cmp [esi+ecx+1], MAGIC_MARKER_START3
			jne cont
			cmp [esi+ecx+2], MAGIC_MARKER_START2
			jne cont
			cmp [esi+ecx+3], MAGIC_MARKER_START1
			jne cont
			jmp FoundMarker

		cont:
			inc ecx
			cmp ecx, dwLen
			jne SearchLoop
			
			jmp NotFound
		FoundMarker:
			mov nIndex, ecx
		NotFound:
		}
	}

	return nIndex-4;
}

//////////////////////////////////////////////////////////////////////
void CCircularBuffer::Flush()
{
	m_csCirBuff.Lock();
	m_nCurrentBufferLen = 0;
	m_nFrameStart		= 0;
	m_csCirBuff.Unlock();
}

// #############################################################################################
// FS  = m_dwFrameSize
// S   = dwStart
// BL  = m_dwCurrentBufferLen
// xxx = Frame
// yyy = Frame
//
// 0		  FS		2*FS	  3*FS		 4*FS		5*FS	    6*FS
// ---------------------------------------------------------------------------------------------
// |..........|..xxxxxxxx|xxyyyyyyyy|yyxxxxxxxx|xxyyyyyyyy|yyxxxxxxxx|xx........................
// ---------------------------------------------------------------------------------------------
//				 S			S		   S		  S			 S		    L
// (CCircularBuffer::GetNextFrame(DWORD dwMarker))
//		memmove(&m_pBuffer[0], &m_pBuffer[dwStart], m_dwCurrentBufferLen-dwStart);
//		m_dwCurrentBufferLen -= dwStart;
//
//
// 0		  FS		2*FS	  3*FS		 4*FS		5*FS	    6*FS ....
// ---------------------------------------------------------------------------------------------
// |xxxxxxxxxx|yyyyyyyyyy|xxxxxxxxxx|yyyyyyyyyy|xxxxxxxxxx|.....................................
// ---------------------------------------------------------------------------------------------
// S		  S			 S			S		   S		  L
//
//	(CCircularBuffer::Add(void* pBuffer))
// 		memcpy(&m_pBuffer[m_dwCurrentBufferLen], pBuffer, m_dwFrameSize);
//		m_dwCurentBufferLen += m_dwFrameSize;
//
//
// 0		  FS		2*FS	  3*FS		 4*FS		5*FS	    6*FS ....
// ---------------------------------------------------------------------------------------------
// |xxxxxxxxxx|yyyyyyyyyy|xxxxxxxxxx|yyyyyyyyyy|xxxxxxxxxx|yyyyyyyyyy|..........................
// ---------------------------------------------------------------------------------------------
// S		  S			 S			S		   S		  S			 L
//
//
// #############################################################################################

