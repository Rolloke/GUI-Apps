/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: CICP.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CICP.h $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CICP_H__
#define __CICP_H__

class CICP  
{
public:
	CICP();
	virtual ~CICP();
	BOOL Scale(exVideoBuffer* pExVideoBufferOut, const exVideoBuffer* pExVideoBufferIn);
	BOOL Deinterlace(exVideoBuffer *pVideoBufferOut, const exVideoBuffer *pVideoBufferIn);
	BOOL YUV422toYUV411(exVideoBuffer *pExVideoBufferOut, const exVideoBuffer *pExVideoBufferIn);

protected:
	virtual BOOL SetupICP(exVideoBuffer *pVideoBufferIn, const exVideoBuffer *pVideoBufferOut);

private:
	static tmLibappErr_t tmalVtransICPMemallocFunction (Int instId, 
							 UInt32 flags, ptsaMemallocArgs_t args);

	static tmLibappErr_t tmalVtransICPMemfreeFunction (Int instId,
							UInt32 flags, ptsaMemfreeArgs_t args);

	static tmLibappErr_t tmalVtransICPError(Int instId,
							UInt32 flags, ptsaErrorArgs_t args);

	static tmLibappErr_t tmalVtransICPProgress(Int instId,
							UInt32 flags, ptsaProgressArgs_t args);

	static tmLibappErr_t tmalVtransICPCompletion(Int instId,
							UInt32 flags, ptsaCompletionArgs_t args);

private:
	Int								m_icpInstance;
	tmalVtransICPInstanceSetup_t	m_InstSetup;
	tsaDefaultInstanceSetup_t		m_DefaultSetup;
	BOOL							m_bFormatUpdate;

	tmVideoFormat_t					m_inputFormat;
	tmAvHeader_t					m_inputHeader;
	ptmAvPacket_t					m_pInputPacket;

	tmVideoFormat_t					m_outputFormat;
	tmAvHeader_t					m_outputHeader;
	ptmAvPacket_t					m_pOutputPacket;
	static ULONG					m_dwScalingComplete;
	
	BOOL							m_bDeinterlace;
};

#endif // __CICP_H__
