/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: ICP.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/ICP.h $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICP_H__C12B0B67_4C4F_423F_93AB_45A0250385B2__INCLUDED_)
#define AFX_ICP_H__C12B0B67_4C4F_423F_93AB_45A0250385B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CICP  
{
public:
	CICP();
	virtual ~CICP();
	virtual BOOL SetupICP(int nInpW, int nInpH, int nInpYStride, int nOutW, int nOutH, int nOutYStride);

private:
	static tmLibappErr_t tmalVtransICPMemallocFunction (Int instId, 
							 UInt32 flags,
							 ptsaMemallocArgs_t args);
	static tmLibappErr_t tmalVtransICPMemfreeFunction (Int instId,
							UInt32 flags,
							ptsaMemfreeArgs_t args);

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

};

#endif // !defined(AFX_ICP_H__C12B0B67_4C4F_423F_93AB_45A0250385B2__INCLUDED_)
