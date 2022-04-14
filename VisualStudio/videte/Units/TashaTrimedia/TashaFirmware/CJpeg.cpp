/////////////////////////////////////////////////////////////////////////////
// PROJECT:		Tashafirmare
// FILE:		$Workfile: CJpeg.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CJpeg.cpp $
// CHECKIN:		$Date: 13.12.01 11:51 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 13.12.01 10:41 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#include "Tm.h"
#include "CJpeg.h"
#include "CPerfMon.h"
#include "CMDPoints.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
CJpeg::CJpeg()
{
	// JPeg-Encoder öffnen
	m_hJpeg	= mjpg_enc_open();
	m_nQuality = 50;

	SetBytesPerFrame(25000);

	m_TempVideoBuffer.videoBuffer.Y = NULL;
	m_TempVideoBuffer.videoBuffer.U = NULL;
	m_TempVideoBuffer.videoBuffer.V = NULL;
	m_pPerf1 = new CPerfMon("Compression");
	m_pPerf2 = new CPerfMon("YUV422->YUV411");
	m_pPerf3 = new CPerfMon("Deinterlace");
}

///////////////////////////////////////////////////////////////////////////////////
CJpeg::~CJpeg()
{
	mjpg_enc_close(m_hJpeg);
	WK_DELETE(m_pPerf1);
	WK_DELETE(m_pPerf2);
	WK_DELETE(m_pPerf3);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CJpeg::Compress(BOOL bDeinterlace, const exVideoBuffer* pExVideoBuffer, CMDPoints& Points, DataPacket* pDataPacket, DWORD dwSMBufferSize)
{
	BOOL bResult = FALSE;

	// Ist der Eintrag frei?
	if (!pDataPacket->bValid)
	{
		// Kompression durchführen
		int nJpegSize = Compress(bDeinterlace, (BYTE *)&pDataPacket->dwData, dwSMBufferSize, pExVideoBuffer);
	
		// Fehler bei der Kompression aufgetreten?
		if ((nJpegSize > 0) && (nJpegSize <= dwSMBufferSize))
		{
			pDataPacket->eType			= eTypeJpeg;
			pDataPacket->eSubType		= eSubTypeUnvalid;
			pDataPacket->wSource		= pExVideoBuffer->wSource;

			// Die 5 größten Differenzen zurückliefern
			for (int nI = 0; nI < min(Points.GetSize(), 5) ; nI++)
			{
				pDataPacket->Point[nI].cx		= Points.GetPointAt(nI).m_cx;
				pDataPacket->Point[nI].cy		= Points.GetPointAt(nI).m_cy;
				pDataPacket->Point[nI].nValue	= Points.GetValueAt(nI);
			}
			pDataPacket->bMotion = (Points.GetSize() != 0);
			pDataPacket->TimeStamp.wDay		= 14;
			pDataPacket->TimeStamp.wMonth	= 11;
			pDataPacket->TimeStamp.wYear	= 2001;
			pDataPacket->TimeStamp.wHour	= 8;
			pDataPacket->TimeStamp.wMinute	= 20;
			pDataPacket->TimeStamp.wSecond	= 5;
			pDataPacket->TimeStamp.wMSecond	= 300;
			pDataPacket->TimeStamp.bValid	= FALSE;
			pDataPacket->dwProzessID		= pExVideoBuffer->dwProzessID;
			pDataPacket->dwDataLen			= nJpegSize;	
			pDataPacket->bValid				= TRUE;
			bResult = TRUE;
		}
		else
			WK_TRACE_ERROR("TARGET: Jpegsize greater than Buffersize (%d>%d)\n", nJpegSize, dwSMBufferSize); 
	}
	else
		WK_TRACE_WARNING("TARGET: Queue is full!\n");

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
int CJpeg::Compress(BOOL bDeinterlace, BYTE *stream, int nStreamSize, const exVideoBuffer *pExVideoBuffer)
{
	int nJpegSize = 0;
	unsigned long dwStart = 0;

	// Einen Hilfsvideo Buffer anlegen. Dies geschied jedoch nur EINMAL.
	AllocTempBuffer(pExVideoBuffer);

	if (bDeinterlace)
	{
		m_pPerf3->Start();
		m_ICP.Deinterlace(&m_TempVideoBuffer, pExVideoBuffer);
		m_pPerf3->Stop();

		m_pPerf2->Start();
		m_ICP.YUV422toYUV411(&m_TempVideoBuffer, &m_TempVideoBuffer);
		m_pPerf2->Stop();
	}
	else
	{
		m_pPerf2->Start();
		m_ICP.YUV422toYUV411(&m_TempVideoBuffer, pExVideoBuffer);
		m_pPerf2->Stop();
	}

	m_pPerf1->Start();
	nJpegSize  = mjpg_enc_frame(m_hJpeg, stream, nStreamSize, &m_TempVideoBuffer.videoBuffer);
	m_pPerf1->Stop();

	if (m_bAutoBitrateControl)
	{
		if (nJpegSize > m_nBytesPerFrame)
		{
			m_nQuality--;
			m_nQuality = MAX(m_nQuality,0);
			mjpg_enc_set_quality( m_hJpeg, m_nQuality );
		}
		else if (nJpegSize < m_nBytesPerFrame)
		{
			m_nQuality++;
			m_nQuality = MIN(m_nQuality,100);
			mjpg_enc_set_quality( m_hJpeg, m_nQuality );
		}
	}

	return nJpegSize;
}

///////////////////////////////////////////////////////////////////////////////////
void CJpeg::SetBytesPerFrame(int nBytesPerFrame)
{
	m_bAutoBitrateControl	= true;
	m_nBytesPerFrame		= nBytesPerFrame;
}

///////////////////////////////////////////////////////////////////////////////////
void CJpeg::SetQuality(int nQuality)
{
	m_bAutoBitrateControl	= FALSE;
	m_nQuality				= nQuality;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CJpeg::AllocTempBuffer(const exVideoBuffer *pExVideoBuffer)
{
	// Den Tempbuffer genau EINMAL anlegen. Er wird aus performance Gründen recycled 
	if ((m_TempVideoBuffer.videoBuffer.Y == NULL) &&
		(m_TempVideoBuffer.videoBuffer.U == NULL) &&
		(m_TempVideoBuffer.videoBuffer.V == NULL))
	{
		int szY	 = (pExVideoBuffer->videoBuffer.yPitch  * (pExVideoBuffer->videoBuffer.nHeight + 4));
		int szUV = (pExVideoBuffer->videoBuffer.uvPitch * (pExVideoBuffer->videoBuffer.nHeight + 4));

		m_TempVideoBuffer.videoBuffer.Y			= allocSz(szY);
		m_TempVideoBuffer.videoBuffer.U			= allocSz(szUV);
		m_TempVideoBuffer.videoBuffer.V			= allocSz(szUV);
		m_TempVideoBuffer.videoBuffer.nWidth	= pExVideoBuffer->videoBuffer.nWidth;
		m_TempVideoBuffer.videoBuffer.nHeight	= pExVideoBuffer->videoBuffer.nHeight;
		m_TempVideoBuffer.videoBuffer.yPitch	= pExVideoBuffer->videoBuffer.yPitch;
		m_TempVideoBuffer.videoBuffer.uvPitch	= pExVideoBuffer->videoBuffer.uvPitch;
		m_TempVideoBuffer.flag					= VID_RDY_MM;
	}
	
	return (m_TempVideoBuffer.videoBuffer.Y && m_TempVideoBuffer.videoBuffer.U && m_TempVideoBuffer.videoBuffer.V);
}

///////////////////////////////////////////////////////////////////////////////////
UInt8* CJpeg::allocSz(int bufSz)  const
{
    UInt8*      temp	= Null;

    if ((temp = (UInt8*) _cache_malloc(bufSz, -1)) != Null)
	{
	    memset((Pointer)temp, 0, bufSz);
		_cache_copyback((Pointer)temp, bufSz);
	}
	else
        WK_TRACE_ERROR("TARGET: _cache_malloc failed\n");

    return temp;
}

///////////////////////////////////////////////////////////////////////////////////
void CJpeg::FreeTempBuffer() const
{
	if (m_TempVideoBuffer.videoBuffer.Y)
	    _cache_free((Pointer) m_TempVideoBuffer.videoBuffer.Y);
	if (m_TempVideoBuffer.videoBuffer.U)
	    _cache_free((Pointer) m_TempVideoBuffer.videoBuffer.U);
	if (m_TempVideoBuffer.videoBuffer.V)
		_cache_free((Pointer) m_TempVideoBuffer.videoBuffer.V);
}


