/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: CJpeg.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CJpeg.h $
// CHECKIN:		$Date: 13.12.01 11:51 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 13.12.01 10:41 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CJPEG_H__863432B8_23DB_4F8D_8783_277DCCF48307__INCLUDED_)
#define AFX_CJPEG_H__863432B8_23DB_4F8D_8783_277DCCF48307__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CICP.h"
#include "helper.h"	// Added by ClassView

class CPerfMon;
class CMDPoints;
class CJpeg  
{
public:
	CJpeg();
	virtual ~CJpeg();

	void SetQuality(int nQuality);
	void SetBytesPerFrame(int nBytesPerFrame);
	BOOL Compress(BOOL bDeinterlace, const exVideoBuffer*, CMDPoints& Points, DataPacket* pPacket, DWORD dwSMBufferSize);

protected:
	int  Compress(BOOL bDeinterlace, BYTE* stream, int nStreamSize, const exVideoBuffer* pExVideoBuffer);

private:
	BOOL AllocTempBuffer(const exVideoBuffer *pExVideoBuffer);
	UInt8* allocSz(int bufSz) const;
	void FreeTempBuffer() const;

private:
	exVideoBuffer m_TempVideoBuffer;
	CICP		m_ICP;

	int			m_nBytesPerFrame;
	bool		m_bAutoBitrateControl;
	int			m_nQuality;
	int			m_hJpeg;
	CPerfMon*	m_pPerf1;
	CPerfMon*	m_pPerf2;
	CPerfMon*	m_pPerf3;
};

#endif // !defined(AFX_CJPEG_H__863432B8_23DB_4F8D_8783_277DCCF48307__INCLUDED_)
