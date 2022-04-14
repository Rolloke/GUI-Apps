// CircularBuffer.cpp: implementation of the CCircularBuffer class.
//
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
	m_nChannel				= nChannel;
	m_pBuffer				= new BYTE[m_nAllocSize+1024];
}

//////////////////////////////////////////////////////////////////////
CCircularBuffer::~CCircularBuffer()
{
	WK_DELETE(m_pBuffer);
}

//////////////////////////////////////////////////////////////////////
BOOL CCircularBuffer::Add(void* pBuffer, int nBufferLen)
{
	BOOL bResult = FALSE;
	m_csCirBuff.Lock();

	// Ist der Buffer schon groﬂ genug?
	if (m_nCurrentBufferLen + nBufferLen < m_nAllocSize)
	{
		MemCpy.FastCopy(&m_pBuffer[m_nCurrentBufferLen], pBuffer, nBufferLen);
//		memcpy(&m_pBuffer[m_nCurrentBufferLen], pBuffer, nBufferLen);
		m_nCurrentBufferLen += nBufferLen;
		bResult = TRUE;
	}
	else
	{
		WK_TRACE_WARNING("Circularbuffer is full! dropping data (Channel=%d)\n", m_nChannel);
	}

	m_csCirBuff.Unlock();

	return bResult;
}

//////////////////////////////////////////////////////////////////////
void* CCircularBuffer::GetNextFrame()
{
	DATA_PACKET*	pPacket		= NULL;
	void*			pBuff		= NULL;
	BOOL			bFoundMarker= FALSE;
	int				nFrameEnd	= 0;

	m_csCirBuff.Lock();

	// Suche den Magic Header, der einen Framebeginn markiert
	for (int nI = m_nFrameStart; (nI < m_nCurrentBufferLen) && (!bFoundMarker); nI++)
	{	
		pPacket = (DATA_PACKET*)&m_pBuffer[nI];
		if (pPacket->dwStartMarker == m_dwStartMarker)
		{
			bFoundMarker  = TRUE;
			m_nFrameStart = nI;
		}
	}

	// Magic Header gefunden?
	if (bFoundMarker)
	{	
		// Gefundenes Frame(segment) an den Anfang des Buffers schieben.
		memmove(&m_pBuffer[0], &m_pBuffer[m_nFrameStart], m_nCurrentBufferLen-m_nFrameStart);
		pPacket	 = (DATA_PACKET*)&m_pBuffer[0];

		m_nCurrentBufferLen -= m_nFrameStart;
		m_nFrameStart = 0;
		
		// Ist der Header schon komplett im Buffer?
		if (sizeof(DATA_PACKET) <= m_nCurrentBufferLen)
		{
			// Ist der Endmarker ebenfalls vorhanden, wenn nicht, dann wurde der Header schon vom DMA
			// Kontroller ¸berschrieben!
			if (pPacket->dwEndMarker != m_dwEndMarker)
			{
				WK_TRACE_WARNING("Endmarker not found...discard data\n");
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
				
					// Adresse des gefundenen Frames zur¸ckliefern.
					pBuff = pPacket; 	
				}
			}
		}
	}
	else
	{
		m_nCurrentBufferLen = 0;
		m_nFrameStart		= 0;
	}
	m_csCirBuff.Unlock();

	return pBuff;
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
