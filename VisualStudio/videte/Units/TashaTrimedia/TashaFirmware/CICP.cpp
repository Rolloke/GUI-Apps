/////////////////////////////////////////////////////////////////////////////
// PROJECT:		Tashafirmare
// FILE:		$Workfile: CICP.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CICP.cpp $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 11.12.01 8:10 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#include "tm.h"
#include "CICP.h"

ULONG	CICP::m_dwScalingComplete = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CICP::CICP()
{
	m_pInputPacket		= NULL;
	m_pOutputPacket		= NULL; 
	m_icpInstance		= 0;
	m_bFormatUpdate		= TRUE;
	m_bDeinterlace		= FALSE;
}

//////////////////////////////////////////////////////////////////////
CICP::~CICP()
{
	if (m_icpInstance)
		tmalVtransICPClose(m_icpInstance);
  
	free (m_pInputPacket);
	free (m_pOutputPacket);
}

//////////////////////////////////////////////////////////////////////
BOOL CICP::Scale(exVideoBuffer *pVideoBufferOut, const exVideoBuffer *pVideoBufferIn)
{
    tmLibdevErr_t	err;
	BOOL			bResult = FALSE;

	if (!m_icpInstance)
	{
		// ICP nur einmal initialisieren
		if (!SetupICP(pVideoBufferOut, pVideoBufferIn))
			return bResult;
	}

	if (m_icpInstance)
	{
		m_inputFormat.description			=	(m_bDeinterlace  ? vdfInterlaced : 0);

		memset(m_pInputPacket, 0, sizeof(tmAvPacket_t) + 2*sizeof(tmAvBufferDescriptor_t));
		m_pInputPacket->header					=	&m_inputHeader;
		m_pInputPacket->allocatedBuffers		=	3;
		m_pInputPacket->buffersInUse			=	3;
		m_pInputPacket->buffers[0].data			=	pVideoBufferIn->videoBuffer.Y;
		m_pInputPacket->buffers[1].data			=	pVideoBufferIn->videoBuffer.U;
		m_pInputPacket->buffers[2].data			=	pVideoBufferIn->videoBuffer.V;
		m_pInputPacket->buffers[0].bufSize		=	pVideoBufferIn->videoBuffer.nHeight * pVideoBufferIn->videoBuffer.yPitch;
		m_pInputPacket->buffers[0].dataSize		=	pVideoBufferIn->videoBuffer.nHeight * pVideoBufferIn->videoBuffer.yPitch;
		m_pInputPacket->buffers[1].bufSize		=	pVideoBufferIn->videoBuffer.nHeight * pVideoBufferIn->videoBuffer.yPitch;
		m_pInputPacket->buffers[1].dataSize		=	pVideoBufferIn->videoBuffer.nHeight * pVideoBufferIn->videoBuffer.uvPitch;
		m_pInputPacket->buffers[2].bufSize		=	pVideoBufferIn->videoBuffer.nHeight * pVideoBufferIn->videoBuffer.uvPitch;
		m_pInputPacket->buffers[2].dataSize		=	pVideoBufferIn->videoBuffer.nHeight * pVideoBufferIn->videoBuffer.uvPitch;

		memset(m_pOutputPacket, 0, sizeof(tmAvPacket_t));
		if ((pVideoBufferOut->videoBuffer.U == NULL) && (pVideoBufferOut->videoBuffer.V == NULL))
		{
			m_pOutputPacket->header					=	&m_outputHeader;
			m_pOutputPacket->allocatedBuffers		=	1;
			m_pOutputPacket->buffersInUse			=	1;
			m_pOutputPacket->buffers[0].data		=	pVideoBufferOut->videoBuffer.Y;
			m_pOutputPacket->buffers[0].bufSize		=	pVideoBufferOut->videoBuffer.nHeight * pVideoBufferOut->videoBuffer.yPitch;
			m_pOutputPacket->buffers[0].dataSize	=	pVideoBufferOut->videoBuffer.nHeight * pVideoBufferOut->videoBuffer.yPitch;
		}
		else
		{
			m_pOutputPacket->header					=	&m_outputHeader;
			m_pOutputPacket->allocatedBuffers		=	3;
			m_pOutputPacket->buffersInUse			=	3;
			m_pOutputPacket->buffers[0].data		=	pVideoBufferOut->videoBuffer.Y;
			m_pOutputPacket->buffers[1].data		=	pVideoBufferOut->videoBuffer.U;
			m_pOutputPacket->buffers[2].data		=	pVideoBufferOut->videoBuffer.V;
			m_pOutputPacket->buffers[0].bufSize		=	pVideoBufferOut->videoBuffer.nHeight * pVideoBufferOut->videoBuffer.yPitch;
			m_pOutputPacket->buffers[0].dataSize	=	pVideoBufferOut->videoBuffer.nHeight * pVideoBufferOut->videoBuffer.yPitch;
			m_pOutputPacket->buffers[1].bufSize		=	pVideoBufferOut->videoBuffer.nHeight * pVideoBufferOut->videoBuffer.yPitch;
			m_pOutputPacket->buffers[1].dataSize	=	pVideoBufferOut->videoBuffer.nHeight * pVideoBufferOut->videoBuffer.uvPitch;
			m_pOutputPacket->buffers[2].bufSize		=	pVideoBufferOut->videoBuffer.nHeight * pVideoBufferOut->videoBuffer.uvPitch;
			m_pOutputPacket->buffers[2].dataSize	=	pVideoBufferOut->videoBuffer.nHeight * pVideoBufferOut->videoBuffer.uvPitch;
		}

		tsaControlArgs_t controlArgs;
		controlArgs.command =  VTRANS_CONFIG_DEINTERLACE_ENABLE;
		controlArgs.parameter = (Pointer) &m_outputFormat;
		err = tmalVtransICPInstanceConfig(m_icpInstance, &controlArgs);
		if (err == TMLIBAPP_OK)
		{
			err = tmalVtransICPProcessFrame(m_icpInstance, m_pInputPacket, NULL, m_pOutputPacket, m_bFormatUpdate);
			if (err == TMLIBAPP_OK)
			{
				m_bFormatUpdate = FALSE;
				bResult = TRUE;
			}
			else
				WK_TRACE_ERROR("TARGET: tmalVtransICPProcessFrame failed Status[0x%x]\n", err);
		}
		else
			WK_TRACE_ERROR("TARGET: tmalVtransICPInstanceConfig failed Status[0x%x]\n", err);

		// Scalierung fertig?
		sm_p(m_dwScalingComplete, SM_WAIT, 0);
	}
	
	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CICP::Deinterlace(exVideoBuffer *pVideoBufferOut, const exVideoBuffer *pVideoBufferIn)
{
	BOOL bResult = FALSE;
	m_bDeinterlace				=	TRUE;
	bResult = Scale(pVideoBufferOut, pVideoBufferIn);
	m_bDeinterlace				=	FALSE;
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CICP::YUV422toYUV411(exVideoBuffer *pExVideoBufferOut, const exVideoBuffer *pExVideoBufferIn)
{
	memcpy(pExVideoBufferOut->videoBuffer.Y, pExVideoBufferIn->videoBuffer.Y, pExVideoBufferIn->videoBuffer.nHeight*pExVideoBufferIn->videoBuffer.yPitch);
	for (int nY = 0; nY < pExVideoBufferIn->videoBuffer.nHeight; nY+=2)
	{
		memcpy(pExVideoBufferOut->videoBuffer.U + (nY>>1) * pExVideoBufferIn->videoBuffer.uvPitch,
			   pExVideoBufferIn->videoBuffer.U + nY * pExVideoBufferIn->videoBuffer.uvPitch,
			   pExVideoBufferOut->videoBuffer.uvPitch);
		memcpy(pExVideoBufferOut->videoBuffer.V + (nY>>1) * pExVideoBufferIn->videoBuffer.uvPitch,
			   pExVideoBufferIn->videoBuffer.V + nY * pExVideoBufferIn->videoBuffer.uvPitch,
			   pExVideoBufferOut->videoBuffer.uvPitch);
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CICP::SetupICP(exVideoBuffer *pVideoBufferOut, const exVideoBuffer *pVideoBufferIn)
{
	BOOL	bResult = FALSE;
    tmLibdevErr_t err;
 	
	if (m_icpInstance==0)
	{
		// Input Format
		memset(&m_inputFormat, 0, sizeof(tmVideoFormat_t));
		m_inputFormat.size					=	sizeof(tmVideoFormat_t);
		m_inputFormat.hash					=	0;
		m_inputFormat.referenceCount		=	0;
		m_inputFormat.dataClass				=	avdcVideo;
		m_inputFormat.dataType				=	vtfYUV;
		m_inputFormat.dataSubtype			=	vdfYUV422Planar;
		m_inputFormat.description			=	(m_bDeinterlace  ? vdfInterlaced : 0);
		m_inputFormat.imageWidth			=	pVideoBufferIn->videoBuffer.nWidth;
		m_inputFormat.imageHeight			=	pVideoBufferIn->videoBuffer.nHeight;
		m_inputFormat.imageStride			=	pVideoBufferIn->videoBuffer.yPitch;
		m_inputFormat.imageUVStride			=	pVideoBufferIn->videoBuffer.uvPitch;
		m_inputFormat.activeVideoStartX		=	0;
		m_inputFormat.activeVideoStartY		=	0;
		m_inputFormat.activeVideoEndX		=	pVideoBufferIn->videoBuffer.nWidth-1;
		m_inputFormat.activeVideoEndY		=	pVideoBufferIn->videoBuffer.nHeight-1;
		m_inputFormat.videoStandard			=	vasPAL;

		memset(&m_inputHeader, 0, sizeof(tmAvHeader_t));
		m_inputHeader.id					=	0;
		m_inputHeader.flags					=	0;
		m_inputHeader.userSender			=	0;
		m_inputHeader.userReceiver			=	0;
		m_inputHeader.userPointer			=	(Pointer) 0;
		m_inputHeader.time.ticks			=	0;
		m_inputHeader.time.hiTicks			=	0;
		m_inputHeader.format				=	&m_inputFormat;

		// Output Format
		memset(&m_outputFormat, 0, sizeof(tmVideoFormat_t));
		m_outputFormat.size					=	sizeof(tmVideoFormat_t);
		m_outputFormat.hash					=	0;
		m_outputFormat.referenceCount		=	0;
		m_outputFormat.dataClass			=	avdcVideo;
		m_outputFormat.dataType				=	vtfYUV;
		m_outputFormat.dataSubtype			=	vdfYUV422Planar;
		m_outputFormat.description			=	0;
		m_outputFormat.imageWidth			=	pVideoBufferOut->videoBuffer.nWidth;
		m_outputFormat.imageHeight			=	pVideoBufferOut->videoBuffer.nHeight;
		m_outputFormat.imageStride			=	pVideoBufferOut->videoBuffer.yPitch;
		m_outputFormat.imageUVStride		=	pVideoBufferOut->videoBuffer.uvPitch;

		memset(&m_outputHeader, 0, sizeof(tmAvHeader_t));
		m_outputHeader.id					=	0;
		m_outputHeader.flags				=	0;
		m_outputHeader.userSender			=	0;
		m_outputHeader.userReceiver			=	0;
		m_outputHeader.userPointer			=	(Pointer) 0;
		m_outputHeader.time.ticks			=	0;
		m_outputHeader.time.hiTicks			=	0;
		m_outputHeader.format				=	&m_outputFormat;

		err = tmalVtransICPOpen(&m_icpInstance);
		if (err == TMLIBAPP_OK)
		{
			memset(&m_DefaultSetup, 0, sizeof(m_DefaultSetup));
			m_DefaultSetup.progressFunc		= (tsaProgressFunc_t)	tmalVtransICPProgress;
			m_DefaultSetup.completionFunc	= (tsaCompletionFunc_t) tmalVtransICPCompletion;
			m_DefaultSetup.errorFunc		= (tsaErrorFunc_t)		tmalVtransICPError;
			m_DefaultSetup.memallocFunc		= (tsaMemallocFunc_t)	tmalVtransICPMemallocFunction;
			m_DefaultSetup.memfreeFunc		= (tsaMemfreeFunc_t)	tmalVtransICPMemfreeFunction;
			m_DefaultSetup.datainFunc		= (tsaDatainFunc_t)		NULL;
			m_DefaultSetup.dataoutFunc		= (tsaDataoutFunc_t)	NULL;
			m_DefaultSetup.controlFunc		= (tsaControlFunc_t)	NULL;

			memset(&m_InstSetup, 0, sizeof(m_InstSetup));
			m_InstSetup.defaultSetup		= &m_DefaultSetup;
			m_InstSetup.antiflickerEnable	= FALSE;
			m_InstSetup.bitMaskEnable		= FALSE;
			m_InstSetup.deinterlaceEnable	= TRUE; //FALSE;
			m_InstSetup.notConservative		= FALSE;
			m_InstSetup.outputDest			= tmalVtransICPSDRAM;
			m_InstSetup.outputPCIAddr		= NULL;
			m_InstSetup.overlayEnable		= FALSE;
			m_InstSetup.outputFormat		= m_outputFormat;
			
			err = tmalVtransICPInstanceSetup(m_icpInstance, &m_InstSetup);
			if (err == TMLIBAPP_OK)
			{
				m_pInputPacket	= (tmAvPacket_t *) malloc(sizeof(tmAvPacket_t) + 2*sizeof(tmAvBufferDescriptor_t));
				m_pOutputPacket = (tmAvPacket_t *) malloc(sizeof(tmAvPacket_t));
				sm_create("ScalingComplete", 0, SM_PRIOR, &m_dwScalingComplete);
				bResult			= TRUE;
			}
			else
				WK_TRACE_ERROR("TARGET: tmalVtransICPInstanceSetup failed Status[0x%x]\n", err);
		}
		else
			WK_TRACE_ERROR("TARGET: tmalVtransICPOpen failed Status[0x%x]\n", err);
	}
	
	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
tmLibappErr_t CICP::tmalVtransICPMemallocFunction (Int instId, UInt32 flags, ptsaMemallocArgs_t args)
{
	DP(("tmalVtransICPMemallocFunction[%x]\n", instId));

	args->memHandle = (Pointer)_cache_malloc(args->sizeInBytes, -1); 

	if (args->memHandle) 
		return TMLIBAPP_OK;
	else 
		return TMLIBAPP_ERR_MEMALLOC_FAILED;
}

///////////////////////////////////////////////////////////////////////////////////
tmLibappErr_t CICP::tmalVtransICPMemfreeFunction (Int instId, UInt32 flags, ptsaMemfreeArgs_t args)
{
	DP(("tmalVtransICPMemfreeFunction[%x]\n", instId));

	_cache_free(args->memHandle); 

	return TMLIBAPP_OK;
}

///////////////////////////////////////////////////////////////////////////////////
tmLibappErr_t CICP::tmalVtransICPError(Int instId, UInt32 flags, ptsaErrorArgs_t args) 
{
	DP(("tmalVtransICPError[%d]: entered\n", instId));

	tmAssert(False,args->errorCode);

	return TMLIBAPP_OK;
}

///////////////////////////////////////////////////////////////////////////////////
tmLibappErr_t CICP::tmalVtransICPProgress(Int instId, UInt32 flags, ptsaProgressArgs_t args)
{
	DP(("tmalVtransICPProgress[%d]: inside callback!\n", instId));

	return (TMLIBAPP_OK);
}

///////////////////////////////////////////////////////////////////////////////////
tmLibappErr_t CICP::tmalVtransICPCompletion(Int instId, UInt32 flags, ptsaCompletionArgs_t args)
{
	DP(("tmalVtransICPCompletion[%d]: inside callback!\n", instId));
	
	// Semaphore setzen
	sm_v(m_dwScalingComplete);
	
	return (TMLIBAPP_OK);
}

