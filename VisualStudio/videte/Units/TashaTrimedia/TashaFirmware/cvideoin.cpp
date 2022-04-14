/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: cvideoin.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/cvideoin.cpp $
// CHECKIN:		$Date: 14.12.01 12:40 $
// VERSION:		$Revision: 18 $
// LAST CHANGE:	$Modtime: 14.12.01 12:39 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "tm.h"
#include "CVideoIn.h"

CVideoIn* CVideoIn::m_pThis = NULL;

///////////////////////////////////////////////////////////////////////////////////
CVideoIn::CVideoIn(ULONG SemBufferMMReady)
{
	m_pThis				= this;
	m_nViInst			= 0;
	m_videoStandard		= vasPAL;
	m_inAdapterType		= vaaCVBS;
	m_bInvertField		= FALSE;
	m_nViNum			= 0;

	m_bCaptureFrame		= TRUE;
	m_bFirstField		= FALSE;

	m_genBuf			= NULL;
	m_SemBufferMMReady	= SemBufferMMReady;

	m_wSource			= VIDEO_SOURCE0;

	for (WORD wSource = 0; wSource < MAX_CAMERAS; wSource++)
	{
		m_ColorSettings[wSource].wBrightness = (MAX_BRIGHTNESS - MIN_BRIGHTNESS) / 2;
		m_ColorSettings[wSource].wContrast   = (MAX_CONTRAST   - MIN_CONTRAST) / 2;
		m_ColorSettings[wSource].wSaturation = (MAX_SATURATION - MIN_SATURATION) / 2;
	}
}

///////////////////////////////////////////////////////////////////////////////////
CVideoIn::~CVideoIn()
{
	viClose(m_nViInst);				  
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoIn::viOpenAPI()
{
	BOOL bResult	= FALSE;
    tmLibdevErr_t err;
	
    if (err = viOpen(&m_nViInst) == TMLIBAPP_OK)
	{
		memset((char *) (&m_viInstSup), 0, sizeof (viInstanceSetup_t));

		m_viInstSup.interruptPriority	= intPRIO_3;
		m_viInstSup.isr					= viTestISR;
		m_viInstSup.videoStandard		= m_videoStandard;
		m_viInstSup.adapterType			= m_inAdapterType;

		if (err = viInstanceSetup(m_nViInst, &m_viInstSup) == TMLIBAPP_OK)
			bResult  = TRUE;
		else
	        WK_TRACE_ERROR("TARGET: viInstanceSetup failed Status[0x%x\\n", err);
	}
	else
        WK_TRACE_ERROR("TARGET: viOpen failed Status[0x%x]\n", err);

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
void CVideoIn::viTestISR()
{
#pragma TCS_handler
	AppModel_suspend_scheduling();	// turned off task scheduling
	AppModel_run_on_sstack((AppModel_Fun)_viTestISR, (Pointer)m_pThis);
	AppModel_resume_scheduling();	// resume task scheduling
}

///////////////////////////////////////////////////////////////////////////////////
void CVideoIn::_viTestISR(Pointer pParam)
{
	CVideoIn* pThis = (CVideoIn*)pParam;

    UInt32 vi_status	= 0;
    Int    nOddField	= 0;
    Int    nVitmpNum	= 0;
    UInt32 mmioBase		= 0;

	mmioBase = VI1_MMIO_BASE;

    vi_status = MMIO(mmioBase + VI_STATUS_OFFSET);

    nOddField = viExtractODD(vi_status);

    /* Should the field ID be inverted? */
    if (pThis->m_bInvertField)
        nOddField = !nOddField;

    if ((vi_status & 0x1) == 0)
	{
	    /*
         * This is due to hardware bug 21390, a spurious interrupt
         * may occur. Since we have only enabled capture_enable (0x1)
         * we should not check on threshold_reached (0x2)
         */
        return;
    }
    if (viHBE(vi_status))
	{
        /*
         * Highway banmdwidth, VI needs more bandwidth. You can play
         * with the highway arbitration setting
         */
        viAckHBE_ACKM(mmioBase);
        return;
    }

    if (!pThis->m_bCaptureFrame)
	{
		// Nur ein Halbbild
        nVitmpNum = (pThis->m_nViNum + 1) % NUM_BUF_ENTRIES;

		// Ist dieser Buffer bereit für die VideoIn-Unit?
        if (nOddField && (pThis->m_genBuf[nVitmpNum].flag == VID_RDY_VI))
		{
			// Ja, dann schalte die VideoIn-Unit auf diesen neuen Buffer
            viYUVChangeBufferM(mmioBase, pThis->m_nViInst,
                              pThis->m_genBuf[nVitmpNum].videoBuffer.Y,
                              pThis->m_genBuf[nVitmpNum].videoBuffer.U,
                              pThis->m_genBuf[nVitmpNum].videoBuffer.V);

			// und gebe den vorherigen Buffer wieder für die VideoIn-Unit frei. 
            pThis->m_genBuf[pThis->m_nViNum].flag = VID_RDY_MM;
       		
			// Bufferhandling thread melden, das ein neues Bild zur Verfügung stehet
			sm_v(pThis->m_SemBufferMMReady);

			// Und merke dir den neuen VideoIn Buffer index
			pThis->m_nViNum = nVitmpNum;
        }
        viAckCAP_ACKM(mmioBase);
    }
	else	// Beide Halbbilder
	{
		// Erstes Field?
		if (pThis->m_bFirstField)
		{
			pThis->m_bFirstField = FALSE;

			// Erstes Field sollte ein Oddfield sein
			if (!nOddField)
			{
				/* skip even field to get sync */
				pThis->m_bFirstField = TRUE;
			}
			else
			{
				/* always start with odd field */
				viYUVChangeBufferM(mmioBase, m_nViInst,
								  pThis->m_genBuf[pThis->m_nViNum].videoBuffer.Y + pThis->m_genBuf[pThis->m_nViNum].videoBuffer.yPitch,
								  pThis->m_genBuf[pThis->m_nViNum].videoBuffer.U + pThis->m_genBuf[pThis->m_nViNum].videoBuffer.uvPitch,
								  pThis->m_genBuf[pThis->m_nViNum].videoBuffer.V + pThis->m_genBuf[pThis->m_nViNum].videoBuffer.uvPitch);
			
				DP(("yPitch=%d uvPitch=%d\n", pThis->m_genBuf[pThis->m_nViNum].videoBuffer.yPitch, pThis->m_genBuf[pThis->m_nViNum].videoBuffer.uvPitch));
			}
		}
		else
		{
			// Nächsten Buffer holen
			nVitmpNum = (pThis->m_nViNum + 1) % NUM_BUF_ENTRIES;
        
			// Ist dieser Buffer bereit für die VideoIn-Unit?
			if (pThis->m_genBuf[nVitmpNum].flag == VID_RDY_VI)
			{
				// Ja, dann schalte die VideoIn-Unit auf diesen neuen Buffer
				viYUVChangeBufferM(mmioBase, pThis->m_nViInst,
								  pThis->m_genBuf[nVitmpNum].videoBuffer.Y,
								  pThis->m_genBuf[nVitmpNum].videoBuffer.U,
								  pThis->m_genBuf[nVitmpNum].videoBuffer.V);

				// und gebe den vorherigen Buffer wieder für die VideoIn-Unit frei. 
				pThis->m_genBuf[pThis->m_nViNum].flag = VID_RDY_MM;

				// Bufferhandling thread melden, das ein neues Bild zur Verfügung stehet
				sm_v(pThis->m_SemBufferMMReady);

				// Und merke dir den neuen VideoIn Buffer index
				pThis->m_nViNum = nVitmpNum;

				pThis->m_bFirstField = TRUE;		
			}
		}
	    viAckCAP_ACKM(mmioBase);    /* read comments above on the acknowledge */
	}
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoIn::viYUVAPI(viYUVModes_t mode, Bool bCaptueFrame, exVideoBuffer* pVBuff, Int nStartX, Int nStartY)
{
	BOOL	bResult = FALSE;
    UInt startX, startY, endX, endY;
    tmLibdevErr_t err;

	if (!pVBuff)
		return bResult;

    memset((char *) (&m_viYUVSup), 0, sizeof (viYUVSetup_t));

	m_genBuf		= pVBuff;
	m_nViNum		= 0;
	m_bFirstField	= TRUE;
	m_bCaptureFrame	= bCaptueFrame;

    m_viYUVSup.thresholdReachedEnable	= False;
    m_viYUVSup.captureCompleteEnable	= True;
    m_viYUVSup.cositedSampling			= True;
    m_viYUVSup.mode						= mode;
    m_viYUVSup.yThreshold				= 0;

    /* Try to get horizontal and vertical start position from vi */
    if (viGetDefaultAcquisitionWnd(m_nViInst, &startX, &startY, &endX, &endY) == TMLIBDEV_OK)
    {
        if (mode == viHALFRES)          
            m_viYUVSup.startX = (startX + 3) & 0xFC; // Make sure startX is a multiple of 4
        else
            m_viYUVSup.startX = startX;

        m_viYUVSup.startY = startY;
    }
    else
    {      
        m_viYUVSup.startX = nStartX;	// If not available by vi, use defaults
        m_viYUVSup.startY = nStartY;
    }

    /* Using a small vertical offset means we have to invert the field ID... */
    if (m_viYUVSup.startY < 7)
        m_bInvertField = TRUE;
    else
        m_bInvertField = FALSE;
 
    m_viYUVSup.width = pVBuff->videoBuffer.nWidth;
    m_viYUVSup.yBase = pVBuff->videoBuffer.Y;
    m_viYUVSup.uBase = pVBuff->videoBuffer.U;
    m_viYUVSup.vBase = pVBuff->videoBuffer.V;

    if (m_bCaptureFrame)
	{
		// Vollbild
        m_viYUVSup.height = (pVBuff->videoBuffer.nHeight >> 1);
        m_viYUVSup.yDelta = (2*pVBuff->videoBuffer.yPitch  - pVBuff->videoBuffer.nWidth) + 1;
        m_viYUVSup.uDelta = (2*pVBuff->videoBuffer.uvPitch - (pVBuff->videoBuffer.nWidth>>1)) + 1;
        m_viYUVSup.vDelta = (2*pVBuff->videoBuffer.uvPitch - (pVBuff->videoBuffer.nWidth>>1)) + 1;
    }
    else	
	{
    	// Halbbild
		m_viYUVSup.height = pVBuff->videoBuffer.nHeight;
        m_viYUVSup.yDelta = (pVBuff->videoBuffer.yPitch  - pVBuff->videoBuffer.nWidth) + 1;
        m_viYUVSup.uDelta = ((pVBuff->videoBuffer.yPitch - pVBuff->videoBuffer.nWidth) >> 1) + 1;
        m_viYUVSup.vDelta = ((pVBuff->videoBuffer.yPitch - pVBuff->videoBuffer.nWidth) >> 1) + 1;
	}

    if (err = viYUVSetup(m_nViInst, &m_viYUVSup) == TMLIBAPP_OK)
    {
		if (err = viStart(m_nViInst) == TMLIBAPP_OK)
			bResult = TRUE;
		else
			WK_TRACE_ERROR("TARGET: viStart failed Status[0x%x]\n", err);
	}
	else
		WK_TRACE_ERROR("TARGET: viYUVSetup failed Status[0x%x]\n", err);

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoIn::viStop()
{
	BOOL bResult = FALSE;

    tmLibdevErr_t err;

    if (err = ::viStop(m_nViInst))
 		WK_TRACE_ERROR("TARGET: viStop failed Status[0x%x]\n", err);
	else
		bResult = TRUE;

	return bResult;
}


///////////////////////////////////////////////////////////////////////////////////
void CVideoIn::SetAdapterType(tmVideoAnalogAdapter_t inAdapterType)
{
	m_inAdapterType = inAdapterType;
}

///////////////////////////////////////////////////////////////////////////////////
void CVideoIn::SetVideoStandard(tmVideoAnalogStandard_t videoStandard)
{
	m_videoStandard = videoStandard;	
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoIn::SetBrightness(WORD wSource, int nValue)
{
	tmLibdevErr_t err = TMLIBAPP_OK;

	// Wenn die Kamera gerade aktiv ist, kann der Wert gleich gesetzt werden.
	if (wSource == m_wSource)
	{
		err = viSetBrightness(m_nViInst, nValue);
		if (err != TMLIBAPP_OK)
			WK_TRACE_ERROR("TARGET: viSetBrightness failed Status[0x%x]\n", err);
	}
	
	m_ColorSettings[wSource].wBrightness = nValue;

	return (err == TMLIBAPP_OK);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoIn::SetContrast(WORD wSource, int nValue)
{
	tmLibdevErr_t err = TMLIBAPP_OK;

	// Wenn die Kamera gerade aktiv ist, kann der Wert gleich gesetzt werden.
	if (wSource == m_wSource)
	{
		err = viSetContrast(m_nViInst, nValue);
		if (err != TMLIBAPP_OK)
			WK_TRACE_ERROR("TARGET: viSetContrast failed Status[0x%x]\n", err);
	}

	m_ColorSettings[wSource].wContrast = nValue;

	return (err == TMLIBAPP_OK);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoIn::SetSaturation(WORD wSource, int nValue)
{
	tmLibdevErr_t err = TMLIBAPP_OK;

	// Wenn die Kamera gerade aktiv ist, kann der Wert gleich gesetzt werden.
	if (wSource == m_wSource)
	{
		err = viSetSaturation(m_nViInst, nValue);
		if (err != TMLIBAPP_OK)
			WK_TRACE_ERROR("TARGET: viSetSaturation failed Status[0x%x]\n", err);
	}

	m_ColorSettings[wSource].wSaturation = nValue;

	return (err == TMLIBAPP_OK);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoIn::GetBrightness(WORD wSource, int& nValue)
{
	tmLibdevErr_t err = TMLIBAPP_OK;

//	err = viGetVideoColor(m_nViInst, vctBrightness, (UInt*)&nValue);
//	nValue = nValue * 100 / MAX_BRIGHTNESS;
//	if (err != TMLIBAPP_OK)
//		WK_TRACE_ERROR("TARGET: viGetVideoColor(vctBrightness) failed Status[0x%x]\n", err);

	nValue = m_ColorSettings[wSource].wBrightness;

	return (err == TMLIBAPP_OK);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoIn::GetContrast(WORD wSource, int& nValue)
{
	tmLibdevErr_t err = TMLIBAPP_OK;

//	err = viGetVideoColor(m_nViInst, vctContrast, (UInt*)&nValue);
//	nValue = nValue * 100 / MAX_CONTRAST;
//	if (err != TMLIBAPP_OK)
//		WK_TRACE_ERROR("TARGET: viGetVideoColor(vctContrast) failed Status[0x%x]\n", err);

	nValue = m_ColorSettings[wSource].wContrast;

	return (err == TMLIBAPP_OK);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoIn::GetSaturation(WORD wSource, int& nValue)
{
	tmLibdevErr_t err = TMLIBAPP_OK;
//	err = viGetVideoColor(m_nViInst, vctSaturation, (UInt*)&nValue);
//	nValue = nValue * 100 / MAX_SATURATION;
//	if (err != TMLIBAPP_OK)
//		WK_TRACE_ERROR("TARGET: viGetVideoColor(vctSaturation) failed Status[0x%x]\n", err);

	nValue = m_ColorSettings[wSource].wSaturation;

	return (err == TMLIBAPP_OK);
}
			 
///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoIn::SetInputSource(const SOURCE_SELECTION &SrcSel)
{
	// Die Umwschaltung funktioniert SO nicht. Der Code ist lediglich
	// ein Platzhalter!
	tmLibdevErr_t err = TMLIBAPP_OK;
//	tmLibdevErr_t err = viSetAnalogInput(m_nViInst,vaaCVBS, SrcSel.wSource);
	if (err != TMLIBAPP_OK)
		WK_TRACE_ERROR("TARGET: viSetAnalogInput failed Status[0x%x]\n", err);
	
	m_wSource = SrcSel.wSource;

	// Kameraspezifische Einstellungen setzen
	if (SrcSel.wBrightness != (WORD)-1)
		SetBrightness(SrcSel.wSource, SrcSel.wBrightness);
	else
		SetBrightness(SrcSel.wSource, m_ColorSettings[SrcSel.wSource].wBrightness);
	
	if (SrcSel.wContrast != (WORD)-1)
		SetContrast(SrcSel.wSource, SrcSel.wContrast);
	else
		SetContrast(SrcSel.wSource, m_ColorSettings[SrcSel.wSource].wContrast);

	if (SrcSel.wSaturation != (WORD)-1)
		SetSaturation(SrcSel.wSource, SrcSel.wSaturation);
	else
		SetSaturation(SrcSel.wSource, m_ColorSettings[SrcSel.wSource].wSaturation);

	return (err == TMLIBAPP_OK);
}

///////////////////////////////////////////////////////////////////////////////////
BOOL CVideoIn::GetInputSource(UInt& nSource)
{
	// Der Code ist lediglich ein Platzhalter!
	tmVideoAnalogAdapter_t adapter;
	
	tmLibdevErr_t err = viGetAnalogInput(m_nViInst, &adapter, &nSource);
	if (err != TMLIBAPP_OK)
		WK_TRACE_ERROR("TARGET: viGetAnalogInput failed Status[0x%x]\n", err);

	return (err == TMLIBAPP_OK);
}
