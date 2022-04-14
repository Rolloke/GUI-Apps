/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: cvideoout.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/cvideoout.cpp $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "tm.h"
#include "CVideoOut.h"

CVideoOut* CVideoOut::m_pThis = NULL;

///////////////////////////////////////////////////////////////////////////////////
CVideoOut::CVideoOut()
{
	m_pThis					= this;
	m_nVoInst				= 0;
	m_videoStandard			= vasPAL;
	m_outAdapterType		= vaaCVBS;
	m_voISRCount			= 0;
	m_nVoNum				= 0;
}

///////////////////////////////////////////////////////////////////////////////////
CVideoOut::~CVideoOut()
{
	voClose(m_nVoInst);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoOut::voOpenAPI()
{
	BOOL bResult = FALSE;
    tmLibdevErr_t err;

    if (err = voOpen(&m_nVoInst) == TMLIBAPP_OK)
	{
		memset((char *) (&m_voInstSup), 0, sizeof (voInstanceSetup_t));

		m_voInstSup.interruptPriority	= intPRIO_6;
		m_voInstSup.isr					= voTestISR;
		m_voInstSup.videoStandard		= m_videoStandard;
		m_voInstSup.adapterType			= m_outAdapterType;

		/* this function call calculates the ddsFrequency register according to */
		/* the chip version. See formula on VO, Figure 7.6 in the Databook for  */
		/* TM1000. See respective sections for TM1100, TM1300, and TM2700       */
		voFrequencyToDDS(27000000.0, &m_voInstSup.ddsFrequency);

		m_voInstSup.hbeEnable		= TRUE;
		m_voInstSup.underrunEnable	= TRUE;

		if (err = voInstanceSetup(m_nVoInst, &m_voInstSup) == TMLIBAPP_OK)
			bResult = TRUE;
		else
			WK_TRACE_ERROR("TARGET: voInstanceSetup failed Status[%d]\n", err);

	}
	else
		WK_TRACE_ERROR("TARGET: voOpen failed Status[0x%x]\n", err);

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
void CVideoOut::voTestISR()
{
#pragma TCS_handler
	AppModel_suspend_scheduling();	// turned off task scheduling
    AppModel_run_on_sstack((AppModel_Fun)_voTestISR, (Pointer)m_pThis);
	AppModel_resume_scheduling();	// resume task scheduling
}

///////////////////////////////////////////////////////////////////////////////////
void CVideoOut::_voTestISR(Pointer pParam)
{
	CVideoOut* pThis = (CVideoOut*)	pParam;

    UInt32 vo_status = MMIO(VO_STATUS);
    Int    votmpNum;

    pThis->m_voISRCount++;

    if (voYTR(vo_status))
        voAckYTR_ACK();
    if (voURUN(vo_status))
        voAckURUN_ACK();
    if (voHBE(vo_status))
        voAckHBE_ACK();
    if (voBUF2EMPTY(vo_status))
        voAckBFR2_ACK();
	
	if (voBUF1EMPTY(vo_status))
	{    /* bfr1 empty */
        if (voFIELD2(vo_status))
		{
			// Ist dieser Buffer bereit für die VideoOut-Unit?
			if (pThis->m_genBuf[pThis->m_nVoNum].flag == VID_RDY_VO)
			{
				/* from f1 to f2 */
				voYUVChangeBuffer(pThis->m_nVoInst,
					(Char *) pThis->m_genBuf[pThis->m_nVoNum].videoBuffer.Y + pThis->m_genBuf[pThis->m_nVoNum].videoBuffer.yPitch,
					(Char *) pThis->m_genBuf[pThis->m_nVoNum].videoBuffer.U + pThis->m_genBuf[pThis->m_nVoNum].videoBuffer.uvPitch,
					(Char *) pThis->m_genBuf[pThis->m_nVoNum].videoBuffer.V + pThis->m_genBuf[pThis->m_nVoNum].videoBuffer.uvPitch);
			}
		}
        else
		{
			// Nächsten Buffer holen
            votmpNum = (pThis->m_nVoNum + 1) % NUM_BUF_ENTRIES;
            
			// Ist dieser Buffer bereit für die VideoOut-Unit?
			if (pThis->m_genBuf[votmpNum].flag == VID_RDY_VO)
			{
				// Ja, dann schalte VideoOut-Unit auf diesen neuen Buffer
				voYUVChangeBuffer(pThis->m_nVoInst,
						  pThis->m_genBuf[votmpNum].videoBuffer.Y,
						  pThis->m_genBuf[votmpNum].videoBuffer.U,
						  pThis->m_genBuf[votmpNum].videoBuffer.V);

				// und gebe den vorherigen Buffer wieder für die VideoIn-Unit frei. 
                pThis->m_genBuf[pThis->m_nVoNum].flag = VID_RDY_VI;
                
				// Und merke dir den neuen VideoOut Buffer index
				pThis->m_nVoNum = votmpNum;
            }
        }
		voAckBFR1_ACK();
    }
}


///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoOut::voYUVAPI(voYUVModes_t mode, exVideoBuffer* pVBuff, Int imageVertOffset, Int imageHorzOffset)
{
	BOOL	bResult = FALSE;
    tmLibdevErr_t err;

	if (!pVBuff)
		return bResult;

	m_genBuf				= pVBuff;
	m_nVoNum				= 0;

    memset((char *) (&m_voYUVSup), 0, sizeof (voYUVSetup_t));
    m_voYUVSup.mode				= mode;
    m_voYUVSup.buf1emptyEnable	= True;
    m_voYUVSup.yThresholdEnable = False;
    m_voYUVSup.yThreshold		= 0;
    m_voYUVSup.yBase			= pVBuff->videoBuffer.Y;
    m_voYUVSup.uBase			= pVBuff->videoBuffer.U;
    m_voYUVSup.vBase			= pVBuff->videoBuffer.V;
    m_voYUVSup.imageVertOffset	= imageVertOffset;
    m_voYUVSup.imageHorzOffset	= imageHorzOffset;
    m_voYUVSup.imageHeight		= (pVBuff->videoBuffer.nHeight >> 1);
    m_voYUVSup.yStride			= (2*pVBuff->videoBuffer.yPitch);
    m_voYUVSup.uStride			= (2*pVBuff->videoBuffer.uvPitch);
    m_voYUVSup.vStride			= (2*pVBuff->videoBuffer.uvPitch);

    switch (mode)
	{
		case vo422_COSITED_UNSCALED:
		case vo422_INTERSPERSED_UNSCALED:
		case vo420_UNSCALED:
			m_voYUVSup.imageWidth = pVBuff->videoBuffer.nWidth;
			break;
		case vo422_COSITED_SCALED:
		case vo422_INTERSPERSED_SCALED:
		case vo420_SCALED:
		default:
			m_voYUVSup.imageWidth = pVBuff->videoBuffer.nWidth << 1;
			break;
    }

    if (err = voYUVSetup(m_nVoInst, &m_voYUVSup) == TMLIBAPP_OK)
    {
		if (err = voStart(m_nVoInst) == TMLIBAPP_OK)
			bResult = TRUE;
		else
			WK_TRACE_ERROR("TARGET: voStart failed Status[0x%x]\n", err);
	}
	else
		WK_TRACE_ERROR("TARGET: voYUVSetup failed Status[0x%x]\n", err);

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoOut::voStop()
{
	BOOL bResult = FALSE;

    tmLibdevErr_t err;

    if (err = ::voStop(m_nVoInst) == TMLIBAPP_OK)
		bResult = TRUE;
	else
 		WK_TRACE_ERROR("TARGET: viStop failed Status[0x%x]\n", err);

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
void CVideoOut::SetAdapterType(tmVideoAnalogAdapter_t outAdapterType)
{
	m_outAdapterType = outAdapterType;
}

