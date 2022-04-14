/////////////////////////////////////////////////////////////////////////////
// PROJECT:		Tashafirmare
// FILE:		$Workfile: CMotionDetection.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CMotionDetection.cpp $
// CHECKIN:		$Date: 13.12.01 12:19 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 13.12.01 12:07 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
#include "tm.h"
#include "CMotionDetection.h"
#include "CMDPoints.h"
#include "CPerfMon.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CMotionDetection::CMotionDetection()
{
	for (int nI = 0; nI < MAX_CAMERAS; nI++)
	{
		m_wField[nI]			= 0;  
		m_dwTimeStamp[nI]		= 0;
		m_pFrameBuffers[nI][0]	= NULL;
		m_pFrameBuffers[nI][1]	= NULL;
		m_bValidFrame[nI][0]	= FALSE;
		m_bValidFrame[nI][1]	= FALSE;
		m_bUseAdaptiveMask[nI]	= FALSE;
		m_bActivateMD[nI]		= TRUE;
		LoadParameter(nI);
		LoadMask(nI);
	}
	m_pVBuffer			= NULL;
	m_bShowOriginal		= FALSE;
	m_bShowDifference	= FALSE;
	m_pPerf1			= new CPerfMon("MotionDetection");
	m_pPerf2			= new CPerfMon("Scaling");
	m_pPerf3			= new CPerfMon("TP-Filter");
}

//////////////////////////////////////////////////////////////////////
CMotionDetection::~CMotionDetection()
{
	char szBuffer[255];

	for (int nI = 0; nI < MAX_CAMERAS; nI++)
	{
		SaveMask(nI);
		_cache_free((Pointer)m_pFrameBuffers[nI][0]); // WK_DELETE(m_pFrameBuffers[nI][0]);
		_cache_free((Pointer)m_pFrameBuffers[nI][1]); // WK_DELETE(m_pFrameBuffers[nI][1]);
	}

	WK_DELETE(m_pPerf1);
	WK_DELETE(m_pPerf2);
	WK_DELETE(m_pPerf3);
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::MotionCheck(WORD wSubID, const exVideoBuffer* pVBuffer, CMDPoints& Points)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE("TARGET: CMotionDetection::MotionCheck\tWrong CamID (%d)\n", wSubID);
		return FALSE;
	}

	// MotionDetection aktiviert?
	if (!m_bActivateMD[wSubID])
		return FALSE;

	// Alle m_nMinTime ms (default=250ms) neuen Vergleich durchführen
	if (GetTimeSpan(m_dwTimeStamp[wSubID]) < (DWORD)m_nMinTime[wSubID])
		return FALSE;

	// Liegt der letzte Grab schon länger als 30 Sekunden zurück, dann lösche ihn, da zu alt
	if (GetTimeSpan(m_dwTimeStamp[wSubID]) > (DWORD)m_nMaxTime[wSubID])
		m_bValidFrame[wSubID][m_wField[wSubID]] = FALSE;

	// Die beiden Framebuffer immer im Wechsel ansprechen.
	if (m_wField[wSubID] == 0)
		m_wField[wSubID] = 1;
	else if (m_wField[wSubID] == 1)
		m_wField[wSubID] = 0;

	// Wurde bereits ein Framebuffer angelegt.
	if (m_pFrameBuffers[wSubID][m_wField[wSubID]] == NULL)
	{
		m_pFrameBuffers[wSubID][0] = (BYTE*)_cache_malloc(CAPTURE_SIZE_H * CAPTURE_SIZE_V+2*64, -1); //new BYTE[CAPTURE_SIZE_H * CAPTURE_SIZE_V+4];
		m_pFrameBuffers[wSubID][1] = (BYTE*)_cache_malloc(CAPTURE_SIZE_H * CAPTURE_SIZE_V+2*64, -1); //new BYTE[CAPTURE_SIZE_H * CAPTURE_SIZE_V+4];
	}

	// Wird zur Anzeige des Differenzbildes benötigt.
	m_pVBuffer			=	pVBuffer;

	if (!m_pVBuffer)
		return FALSE;
	
	// Ein Frame grabben
	if (!CaptureFrame(wSubID, m_pFrameBuffers[wSubID][m_wField[wSubID]], m_pVBuffer))
		return FALSE;
	
	// Bild als gültig markieren
	m_bValidFrame[wSubID][m_wField[wSubID]] = TRUE;

	// Zeitpunkt des Grabs vermerken
	m_dwTimeStamp[wSubID] = GetTickCount();
	
	if (m_bShowOriginal)
		ShowOriginalFrame(wSubID);

	// Wurden schon 2 Frames gegrapt?
	if (!m_bValidFrame[wSubID][0] || !m_bValidFrame[wSubID][1])
		return FALSE;

	m_pPerf1->Start();				  
	BOOL bRet = CompareImages(wSubID, Points);
	m_pPerf1->Stop();
	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::CaptureFrame(WORD wSubID, BYTE* pOut, const exVideoBuffer* pVBufferIn)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::CaptureFrame\tWrong CamID (%d)\n", wSubID);
		return FALSE;
	}

	m_pPerf2->Start();
	int		nVScale	= pVBufferIn->videoBuffer.yPitch * pVBufferIn->videoBuffer.nHeight / CAPTURE_SIZE_V;
	int		nHScale = (int)((1<<16)*((float)pVBufferIn->videoBuffer.nWidth / (float)CAPTURE_SIZE_H));
	for (int nY = 0; nY < CAPTURE_SIZE_V; nY++)
	{
		BYTE* pSrcBase  = pVBufferIn->videoBuffer.Y + (nY * nVScale);
		BYTE* pDestBase = pOut + (nY * CAPTURE_SIZE_H);
		for (int nX = 0; nX < CAPTURE_SIZE_H;nX++)
			pDestBase[nX] = *(pSrcBase + (int)((nX * nHScale)>>16));
	}
	m_pPerf2->Stop();

	if (GetFilterStatus(wSubID))
	{
		m_pPerf3->Start();

		exVideoBuffer* pVBIn		= new  exVideoBuffer;
		pVBIn->videoBuffer.nWidth	= CAPTURE_SIZE_H;
		pVBIn->videoBuffer.nHeight	= CAPTURE_SIZE_V;
		pVBIn->videoBuffer.yPitch	= CAPTURE_SIZE_H;
		pVBIn->videoBuffer.uvPitch	= 0;
		pVBIn->videoBuffer.Y		= pOut;
		pVBIn->videoBuffer.U		= NULL;
		pVBIn->videoBuffer.V		= NULL;

		exVideoBuffer* pVBOut		= new  exVideoBuffer;
		pVBOut->videoBuffer.nWidth	= CAPTURE_SIZE_H;
		pVBOut->videoBuffer.nHeight	= CAPTURE_SIZE_V;
		pVBOut->videoBuffer.yPitch	= CAPTURE_SIZE_H;
		pVBOut->videoBuffer.uvPitch	= 0;
		pVBOut->videoBuffer.Y		= new BYTE[CAPTURE_SIZE_H*CAPTURE_SIZE_V];
		pVBOut->videoBuffer.U		= NULL;
		pVBOut->videoBuffer.V		= NULL;


		TPFilter(pVBOut, pVBIn);

		memcpy(pOut, pVBOut->videoBuffer.Y, CAPTURE_SIZE_H*CAPTURE_SIZE_V);

		WK_DELETE(pVBOut->videoBuffer.Y);
		WK_DELETE(pVBIn);
		WK_DELETE(pVBOut);

		m_pPerf3->Stop();
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::CompareImages(WORD wSubID, CMDPoints& Points)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::CompareImages\tWrong CamID (%d)\n", wSubID);
		return FALSE;
	}

	memset(&m_nDiffs[0][0], 0, sizeof(int) * CAPTURE_SIZE_H * CAPTURE_SIZE_V / 64);
	int	 nX				= 0;
	int	 nY				= 0;
	int	 nX1			= 0;
	int	 nY1			= 0;
	int  nMittelDiff	= 0;
	int  nMask			= 0;

	// Differenzbild ermitteln
	// dy = abs(y2-y1)
	BYTE* p1 = (BYTE*)&m_pFrameBuffers[wSubID][0][0]; // Pointer auf 1. Frame
	BYTE* p2 = (BYTE*)&m_pFrameBuffers[wSubID][1][0]; // Pointer auf 2. Frame

	nY1 = 0;
	for (nY=0; nY < CAPTURE_SIZE_V; nY++, nY1 = nY/8)
	{
		nX1 = 0;
		for (nX=0; nX < CAPTURE_SIZE_H; nX+=8, nX1 = nX/8)
		{
			m_nDiffs[nX1][nY1] += abs(*p2++-*p1++);
			m_nDiffs[nX1][nY1] += abs(*p2++-*p1++);
			m_nDiffs[nX1][nY1] += abs(*p2++-*p1++);
			m_nDiffs[nX1][nY1] += abs(*p2++-*p1++);
			m_nDiffs[nX1][nY1] += abs(*p2++-*p1++);
			m_nDiffs[nX1][nY1] += abs(*p2++-*p1++);
			m_nDiffs[nX1][nY1] += abs(*p2++-*p1++);
			m_nDiffs[nX1][nY1] += abs(*p2++-*p1++);
		}
	}
	
	// Mittler Helligkeit eines 8x8-Blockes bestimmen
	for (nY1 = 0; nY1 < CAPTURE_SIZE_V/8; nY1++)
	{			  
		for (nX1=0; nX1 < CAPTURE_SIZE_H/8; nX1++)
		{
			nMittelDiff += m_nDiffs[nX1][nY1];		
		}
	}
	nMittelDiff /= ((CAPTURE_SIZE_H/8) * (CAPTURE_SIZE_V/8));

	int nMaxMotion = -9999;

	// Den Block mit der Maximaldifferenz ermitteln und Maske auf-/abbauen
	for (nY1 = 0; nY1 < CAPTURE_SIZE_V/8; nY1++)
	{
		for (nX1=0; nX1 < CAPTURE_SIZE_H/8; nX1++)
		{
			// Mittlere Helligkeit subtrahieren. (Nur den AC-Anteil bewerten)
			m_nDiffs[nX1][nY1] = MAX(m_nDiffs[nX1][nY1] - nMittelDiff, 0);
								  
			if (m_bUseAdaptiveMask[wSubID])
			{
				// Adaptive Bewegungsstörmaske aufbauen/abbauen.
				if (m_nDiffs[nX1][nY1] > m_nUpperMaskTreshHold[wSubID])
				{
					m_Mask[wSubID].nAdaptiveMask[nX1][nY1] += m_nMaskInc[wSubID];
					m_Mask[wSubID].nAdaptiveMask[nX1][nY1]  = MIN(m_Mask[wSubID].nAdaptiveMask[nX1][nY1], UPPER_BOUNDARY_MASK);
				}
				else if (m_nDiffs[nX1][nY1] <= m_nLowerMaskTreshHold[wSubID])
				{
					m_Mask[wSubID].nAdaptiveMask[nX1][nY1] -= m_nMaskDec[wSubID];
					m_Mask[wSubID].nAdaptiveMask[nX1][nY1]  = MAX(m_Mask[wSubID].nAdaptiveMask[nX1][nY1], LOWER_BOUNDARY_MASK);
				}
			}

			// Differenz mit den Masken wichten
			nMask =  MIN(m_Mask[wSubID].nPermanentMask[nX1][nY1] + m_Mask[wSubID].nAdaptiveMask[nX1][nY1], UPPER_BOUNDARY_MASK);
			m_nDiffs[nX1][nY1] = (m_nDiffs[nX1][nY1]*(UPPER_BOUNDARY_MASK-nMask))/UPPER_BOUNDARY_MASK;
		
			// Liegt die Differenz oberhalb des Grenzwertes, dann Koordinaten zurückliefern
			if (m_nDiffs[nX1][nY1] > m_nAlarmTreshHold[wSubID])
			{
				if (m_nDiffs[nX1][nY1] > nMaxMotion)
				{
					// Ort der Bewegung zurückmelden
					CPoint  Point((nX1*8+4)*1000 / CAPTURE_SIZE_H,
								  (nY1*8+4)*1000 / CAPTURE_SIZE_V);
					Points.Add(Point, m_nDiffs[nX1][nY1]);
				}
			}
		}
	}								   

	// Differenzbild verkleinert zeigen
	if (m_bShowDifference)
		ShowDifferenceFrame(wSubID);


	// Nach Stärke sortieren
	if (Points.GetSize() > 1)
		Points.SortByValue();
/*
	if (m_pMDConfigDlg->IsVisible())
	{
		// Differenzbild ausgeben
		ShowDifferenceFrame(wSubID);

		// Den Ort der größten Differenz durch ein Fadenkreuz kennzeichnen
		if (Points.GetSize() > 0)
		{
			if (wSubID == m_pMDConfigDlg->GetSelectedCamera())
			{							  
				CRect rectInlay;
				if (m_pMDConfigDlg)
				{
					CWnd* pWnd = m_pMDConfigDlg->GetInlayWnd();
					if (pWnd)
					{
						m_pMDConfigDlg->GetInlayWnd()->GetClientRect(rectInlay);	
						rectInlay.OffsetRect(CPoint(3,3));

						CBrush NewBrush;
						NewBrush.CreateSolidBrush(RGB(255,0,0));
						m_pDCInlayWnd->SelectObject(NewBrush);

						int nBlkW = 4.0 * ((float)rectInlay.Width()  / (float)CAPTURE_SIZE_H);
						int nBlkH = 4.0 * ((float)rectInlay.Height() / (float)CAPTURE_SIZE_V);
						
						for (int nI = 0; nI < Points.GetSize(); nI++)
						{											   
							int nMaxDiffX 	= Points.GetPointAt(nI).x * rectInlay.Width() / 1000;
							int nMaxDiffY	= Points.GetPointAt(nI).y * rectInlay.Height() / 1000;
							
							m_pDCInlayWnd->SelectStockObject(WHITE_PEN);
							CRect rc(nMaxDiffX-nBlkW, nMaxDiffY-nBlkH, nMaxDiffX+nBlkW, nMaxDiffY+nBlkH); 
							m_pDCInlayWnd->FrameRect(rc, &NewBrush);
						}
						NewBrush.DeleteObject();
					}
				}
			}
		}
	}
*/

	return (Points.GetSize() > 0);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::LoadParameter(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::LoadParameter\tWrong CamID (%d)\n", wSubID);
		return;
	}

//	char szBuffer[255]		={0};
//	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
	
	// Histogramm? Ja/Nein.
	m_bHistogramm[wSubID]	= 0; //ReadConfigurationInt(szBuffer, "Histogramm", 0, m_sIniName);	

	// Filter? Ja/Nein.
	m_bFilter[wSubID]		= 1; //ReadConfigurationInt(szBuffer, "TPFilter", 1, m_sIniName);	

	// Mindest-, und Maximalzeit zwischen zwei Differentbildern
	m_nMinTime[wSubID] = 0; //ReadConfigurationInt(szBuffer, "MinTime", 250,   m_sIniName); 
	m_nMaxTime[wSubID] = 30000; //ReadConfigurationInt(szBuffer, "MaxTime", 30000, m_sIniName); 

	// Die MD-Parameter in Abhängigkeit der Sensitivity-level laden
	SetMDMaskSensitivity(wSubID, GetMDMaskSensitivity(wSubID));
	SetMDAlarmSensitivity(wSubID, GetMDAlarmSensitivity(wSubID));
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::SaveMask(WORD wSubID) const
{									  
	BOOL bRet = TRUE;
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::SaveMask\tWrong CamID (%d)\n", wSubID);
		return FALSE;
	}

/*
	CString sFileName;
	sFileName.Format("%sMotionD%u.msk", m_sMaskPath, wSubID);
	TRY
	{
		CFile file(sFileName, CFile::modeWrite | CFile::modeCreate);
		file.Write(&m_Mask[wSubID], sizeof(m_Mask[wSubID]));
	}
	CATCH( CFileException, e )
	{
		bRet = FALSE;
	}
	END_CATCH
*/
	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::LoadMask(WORD wSubID)
{
	BOOL bRet = TRUE;

	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::LoadMask\tWrong CamID (%d)\n", wSubID);
		return FALSE;
	}


	// Alte Maske löschen
	ClearMask(wSubID);
/*
	CString sFileName;
	sFileName.Format("%sMotionD%u.msk", m_sMaskPath, wSubID);
	TRY
	{
		CFile file(sFileName, CFile::modeRead);
		file.Read(&m_Mask[wSubID], file.GetLength());
	}
	CATCH( CFileException, e )
	{
		bRet = FALSE;
	}
	END_CATCH
*/
	return bRet;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ClearMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ClearMask\tWrong CamID (%d)\n", wSubID);
		return;
	}

	// Alte Masken löschen
	ClearAdaptiveMask(wSubID);
	ClearPermanentMask(wSubID);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ClearAdaptiveMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ClearAdaptiveMask\tWrong CamID (%d)\n", wSubID);
		return;
	}

	// Alte Maske löschen
	for (int nY=0; nY < CAPTURE_SIZE_V/8; nY++)
	{
		for (int nX=0; nX < CAPTURE_SIZE_H/8; nX++)
			m_Mask[wSubID].nAdaptiveMask[nX][nY]  = LOWER_BOUNDARY_MASK;
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ClearPermanentMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ClearPermanentMask\tWrong CamID (%d)\n", wSubID);
		return;
	}

	// Alte Maske löschen
	for (int nY=0; nY < CAPTURE_SIZE_V/8; nY++)
	{
		for (int nX=0; nX < CAPTURE_SIZE_H/8; nX++)
			m_Mask[wSubID].nPermanentMask[nX][nY] = LOWER_BOUNDARY_MASK;
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::InvertPermanentMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::InvertPermanentMask\tWrong CamID (%d)\n", wSubID);
		return;
	}

	// Permanente Maske invertieren
	for (int nY=0; nY < CAPTURE_SIZE_V/8; nY++)
	{
		for (int nX=0; nX < CAPTURE_SIZE_H/8; nX++)
		{
			m_Mask[wSubID].nPermanentMask[nX][nY] = UPPER_BOUNDARY_MASK - m_Mask[wSubID].nPermanentMask[nX][nY];
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::InvertAdaptiveMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::InvertAdaptiveMask\tWrong CamID (%d)\n", wSubID);
		return;
	}

	// Adaptive Maske invertieren
	for (int nY=0; nY < CAPTURE_SIZE_V/8; nY++)
	{
		for (int nX=0; nX < CAPTURE_SIZE_H/8; nX++)
		{
			m_Mask[wSubID].nAdaptiveMask[nX][nY]  = UPPER_BOUNDARY_MASK - m_Mask[wSubID].nAdaptiveMask[nX][nY];
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::SetPixelInMask(WORD wSubID, int nXM, int nYM, BOOL bFlag)
{
/*	COLORREF col;

	if (wSubID >= MAX_CAMERAS)
		return;

	CRect rectClient;
	m_pMDConfigDlg->GetInlayWnd()->GetClientRect(rectClient);	
	
	nXM = nXM * CAPTURE_SIZE_H / rectClient.Width();
	nYM = nYM * CAPTURE_SIZE_V / rectClient.Height();

	nXM = min(max(nXM, 3), CAPTURE_SIZE_H-4);
	nYM = min(max(nYM, 3), CAPTURE_SIZE_V-4);

	for (int nY = nYM-3; nY <= nYM+3; nY++)
	{
		for (int nX = nXM-3; nX <= nXM+3; nX++)
		{
			if (bFlag)
				m_Mask[wSubID].nPermanentMask[nX/8][nY/8] = UPPER_BOUNDARY_MASK;
			else
				m_Mask[wSubID].nPermanentMask[nX/8][nY/8] = LOWER_BOUNDARY_MASK;

			// Farbe der Maske im Differenzbild.
			if (m_Mask[wSubID].nPermanentMask[nX/8][nY/8] != 0)
				col = RGB(m_Mask[wSubID].nPermanentMask[nX/8][nY/8]*255/UPPER_BOUNDARY_MASK, 0, 0);
			else if (m_Mask[wSubID].nAdaptiveMask[nX/8][nY/8] != 0)
				col = RGB(0, m_Mask[wSubID].nAdaptiveMask[nX/8][nY/8]*255/UPPER_BOUNDARY_MASK, 0);
			else
				col = RGB(0, 0, 0);				
		}
	}							    
	if (m_bShowMask)					  
		ShowOriginalFrame(wSubID);
	ShowDifferenceFrame(wSubID);
*/
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleShowMask()
{
	m_bShowMask = !m_bShowMask;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleAdaptiveMaskActivation(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ToggleAdaptiveMaskActivation\tWrong CamID (%d)\n", wSubID);
		return;
	}

	if (m_bUseAdaptiveMask[wSubID])
		DeactivateAdaptiveMask(wSubID);
	else
		ActivateAdaptiveMask(wSubID);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ActivateAdaptiveMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ActivateAdaptiveMask\tWrong CamID (%d)\n", wSubID);
		return;
	}

	m_bUseAdaptiveMask[wSubID] = TRUE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateAdaptiveMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::DeactivateAdaptiveMask\tWrong CamID (%d)\n", wSubID);
		return;
	}

	m_bUseAdaptiveMask[wSubID] = FALSE;
	
	ClearAdaptiveMask(wSubID);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleMDActivation(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ToggleMDActivation\tWrong CamID (%d)\n", wSubID);
		return;
	}

	if (m_bActivateMD[wSubID])
		DeactivateMotionDetection(wSubID);
	else
		ActivateMotionDetection(wSubID);
}	
//////////////////////////////////////////////////////////////////////
void CMotionDetection::ActivateMotionDetection(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ActivateMotionDetection\tWrong CamID (%d)\n", wSubID);
		return;
	}

	m_bActivateMD[wSubID] = TRUE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateMotionDetection(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::DeactivateMotionDetection\tWrong CamID (%d)\n", wSubID);
		return;
	}

	m_bActivateMD[wSubID] = FALSE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleHistogramm(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ToggleHistogramm\tWrong CamID (%d)\n", wSubID);
		return;
	}

	if (m_bHistogramm[wSubID])
		DeactivateHistogramm(wSubID);
	else
		ActivateHistogramm(wSubID);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ActivateHistogramm(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ActivateHistogramm\tWrong CamID (%d)\n", wSubID);
		return;
	}

	m_bHistogramm[wSubID] = TRUE;

	// Einstellung sofort in der Registry sichern
//	char szBuffer[255];
//	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
//	WriteConfigurationInt(szBuffer, "Histogramm", m_bHistogramm[wSubID], m_sIniName);	
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateHistogramm(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::DeactivateHistogramm\tWrong CamID (%d)\n", wSubID);
		return;
	}
	m_bHistogramm[wSubID] = FALSE;

	// Einstellung sofort in der Registry sichern
//	char szBuffer[255];
//	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
//	WriteConfigurationInt(szBuffer, "Histogramm", m_bHistogramm[wSubID], m_sIniName);	
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleFilter(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ToggleFilter\tWrong CamID (%d)\n", wSubID);
		return;
	}

	if (m_bFilter[wSubID])
		DeactivateFilter(wSubID);
	else
		ActivateFilter(wSubID);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ActivateFilter(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ActivateFilter\tWrong CamID (%d)\n", wSubID);
		return;
	}

	m_bFilter[wSubID] = TRUE;

	// Einstellung sofort in der Registry sichern
//	char szBuffer[255];
//	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
//	WriteConfigurationInt(szBuffer, "TPFilter", m_bFilter[wSubID], m_sIniName);	
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateFilter(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::DeactivateFilter\tWrong CamID (%d)\n", wSubID);
		return;
	}
	m_bFilter[wSubID] = FALSE;

	// Einstellung sofort in der Registry sichern
//	char szBuffer[255];
//	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
//	WriteConfigurationInt(szBuffer, "TPFilter", m_bFilter[wSubID], m_sIniName);	
}


//////////////////////////////////////////////////////////////////////
void CMotionDetection::ShowOriginalFrame(WORD wSubID) const
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ShowOriginalFrame\tWrong CamID (%d)\n", wSubID);
		return;
	}

	BYTE* pSrc = m_pFrameBuffers[wSubID][m_wField[wSubID]];
	BYTE* pYDst = m_pVBuffer->videoBuffer.Y;
	BYTE* pUDst = m_pVBuffer->videoBuffer.U;
	BYTE* pVDst = m_pVBuffer->videoBuffer.V;
	for (int nY = 0; nY < CAPTURE_SIZE_V; nY++)
	{
		memcpy(pYDst, pSrc, CAPTURE_SIZE_H);
		memset(pUDst, 0x80, CAPTURE_SIZE_H>>1);
		memset(pVDst, 0x80, CAPTURE_SIZE_H>>1);

		pSrc += CAPTURE_SIZE_H;
		pYDst += m_pVBuffer->videoBuffer.yPitch;
		pUDst += m_pVBuffer->videoBuffer.uvPitch;
		pVDst += m_pVBuffer->videoBuffer.uvPitch;
	}


/*
	BGR24*		pSrc = m_pFrameBuffers[wSubID][m_wField[wSubID]];
	DWORD		dwIndex = 0;
	COLORREF	col		= 0;
	int			nPMask  = 0;
	int			nAMask	= 0;

	if (pSrc && m_pMDConfigDlg && m_pMDConfigDlg->IsVisible())
	{
		// Nur das Bild der aktuell gewählten Kamera zeigen
		if (m_pMDConfigDlg->GetSelectedCamera() == wSubID)
		{
			// Frame in den Bilschirmspeicher kopieren, damit sichtbar.
			for (int nY = 0; nY < CAPTURE_SIZE_V; nY++)
			{
				for (int nX = 0; nX < CAPTURE_SIZE_H; nX++)
				{
					dwIndex = nX+nY*CAPTURE_SIZE_H;
					col = RGB(pSrc[dwIndex].bR, pSrc[dwIndex].bG, pSrc[dwIndex].bB);
					if (m_bShowMask)
					{		  							 
						// Farbe der Maske im Originalbild
						nAMask = m_Mask[wSubID].nAdaptiveMask[nX/8][nY/8]  * 255 / UPPER_BOUNDARY_MASK;
						nPMask = m_Mask[wSubID].nPermanentMask[nX/8][nY/8] * 255 / UPPER_BOUNDARY_MASK;
						if (nPMask != 0)
							col = RGB(min(GetRValue(col)+nPMask, 255), GetGValue(col), GetBValue(col));
						else if (nAMask != 0)
							col = RGB(GetRValue(col), min(GetGValue(col)+nAMask, 255), GetBValue(col));
					}				
					SetPixel(IDC_INLAY_WND, nX, nY, col);
				}
			}

			// Bild blitten
			CRect rectTo;
			CRect rectFrom(0,0, CAPTURE_SIZE_H, CAPTURE_SIZE_V);
			m_pMDConfigDlg->GetInlayWnd()->GetClientRect(rectTo);	
			rectTo.OffsetRect(CPoint(3,3));
			m_pDCInlayWnd->SetStretchBltMode(COLORONCOLOR);
			int iRet = StretchDIBits(m_pDCInlayWnd->m_hDC,				// handle to device context
						  rectTo.left,				// x-coordinate of upper-left corner of dest. rect.
						  rectTo.top,               // y-coordinate of upper-left corner of dest. rect.
						  rectTo.Width(),			// width of destination rectangle
						  rectTo.Height(),          // height of destination rectangle
						  rectFrom.left,			// x-coordinate of upper-left corner of source rect.
						  rectFrom.top, 	        // y-coordinate of upper-left corner of source rect.
						  rectFrom.Width(), 		// width of source rectangle
						  rectFrom.Height(),		// height of source rectangle
						  m_pDibImageDataInlay,			// address of bitmap bits
						  (BITMAPINFO*)&m_bmiInlay,	// address of bitmap data
						  DIB_RGB_COLORS,           // usage
						  SRCCOPY			        // raster operation code
						  );		
		}
	}
*/
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ShowDifferenceFrame(WORD wSubID) const
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::ShowDifferenceFrame\tWrong CamID (%d)\n", wSubID);
		return;
	}

	int colU, colV;
	for (int nY1 = 0; nY1 < CAPTURE_SIZE_V/2; nY1++)
	{			  
		BYTE* pYBase = m_pVBuffer->videoBuffer.Y + nY1 * m_pVBuffer->videoBuffer.yPitch;
		BYTE* pUBase = m_pVBuffer->videoBuffer.U + nY1 * m_pVBuffer->videoBuffer.uvPitch;
		BYTE* pVBase = m_pVBuffer->videoBuffer.V + nY1 * m_pVBuffer->videoBuffer.uvPitch;
		for (int nX1=0; nX1 < CAPTURE_SIZE_H/2; nX1++)
		{
			// Farbe der Maske im Differenzbild
			int nAMask		= m_Mask[wSubID].nAdaptiveMask[nX1>>2][nY1>>2]  * 127 / UPPER_BOUNDARY_MASK;
			int nPMask		= m_Mask[wSubID].nPermanentMask[nX1>>2][nY1>>2] * 127 / UPPER_BOUNDARY_MASK;
			pYBase[nX1]		= MIN((m_nDiffs[nX1>>2][nY1>>2]) / 10, 255);
			pVBase[nX1>>1]	= nPMask+128;
			pUBase[nX1>>1]	= nAMask+128;
		}
	}

/*
	COLORREF	col;
	int			nDiff = 0;
	int			nPMask  = 0;
	int			nAMask	= 0;

	if (m_pMDConfigDlg && m_pMDConfigDlg->IsVisible())
	{
		// Nur das Bild der aktuell gewählten Kamera zeigen
		if (m_pMDConfigDlg->GetSelectedCamera() == wSubID)
		{
			// Differenzbild ausgeben
			for (int nY=0; nY < CAPTURE_SIZE_V; nY++)
			{
				for (int nX=0; nX < CAPTURE_SIZE_H; nX++)
				{
					nDiff = min(m_nDiffs[nX/8][nY/8]/16, 255);

					// Farbe der Maske im Differenzbild
					nAMask = m_Mask[wSubID].nAdaptiveMask[nX/8][nY/8]  * 255 / UPPER_BOUNDARY_MASK;
					nPMask = m_Mask[wSubID].nPermanentMask[nX/8][nY/8] * 255 / UPPER_BOUNDARY_MASK;
					if (nPMask != 0)
						col = RGB(min(nDiff + nPMask, 255), nDiff, nDiff);
					else if (nAMask != 0)
						col = RGB(nDiff, min(nDiff + nAMask, 255), nDiff);
					else
						col = RGB(nDiff, nDiff, nDiff);

					// Differenzbild und Maske zeigen
					SetPixel(IDC_DIFF_WND, nX, nY, col);
				}
			}

			// Bild blitten
			CRect rectTo;
			CRect rectFrom(0,0, CAPTURE_SIZE_H, CAPTURE_SIZE_V);
			m_pMDConfigDlg->GetDiffWnd()->GetClientRect(rectTo);	
			rectTo.OffsetRect(CPoint(3,3));

			m_pDCInlayWnd->SetStretchBltMode(COLORONCOLOR);
			int iRet = StretchDIBits(m_pDCDiffWnd->m_hDC,				// handle to device context
						  rectTo.left,				// x-coordinate of upper-left corner of dest. rect.
						  rectTo.top,               // y-coordinate of upper-left corner of dest. rect.
						  rectTo.Width(),			// width of destination rectangle
						  rectTo.Height(),          // height of destination rectangle
						  rectFrom.left,			// x-coordinate of upper-left corner of source rect.
						  rectFrom.top, 	        // y-coordinate of upper-left corner of source rect.
						  rectFrom.Width(), 		// width of source rectangle
						  rectFrom.Height(),		// height of source rectangle
						  m_pDibImageDataDiff,			// address of bitmap bits
						  (BITMAPINFO*)&m_bmiDiff,	// address of bitmap data
						  DIB_RGB_COLORS,           // usage
						  SRCCOPY			        // raster operation code
						  );		
		}
	}
*/
}

/////////////////////////////////////////////////////////////////////////////
void CMotionDetection::SetMDMaskSensitivity(WORD wSubID, eMDLevel eLevel)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::SetMDMaskSensitivity\tWrong CamID (%d)\n", wSubID);
		return ;
	}

	// Einstellung sofort in der Registry sichern
//	char szBuffer[255];
//	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
//	WriteConfigurationString(szBuffer, "MaskSensitivity", sLevel, m_sIniName); 

	if (eLevel == CSD_HIGH)
	{
		// Den oberen Mask-Treshhold festlegen
		m_nUpperMaskTreshHold[wSubID]= UPPER_BOUNDARY_LUMI*10/1000;
		// Den unteren Mask-Treshhold festlegen
		m_nLowerMaskTreshHold[wSubID]= UPPER_BOUNDARY_LUMI*5/1000;
		// Den Maskenincrement festlegen
		m_nMaskInc[wSubID]	= UPPER_BOUNDARY_MASK*20/1000;
		// Den Maskendecrement festlegen
		m_nMaskDec[wSubID]	= UPPER_BOUNDARY_MASK*5/1000;
		// Adaptive Maske aktivieren
		ActivateAdaptiveMask(wSubID);
	}
	else if (eLevel == CSD_NORMAL)
	{
		// Den oberen Mask-Treshhold festlegen
		m_nUpperMaskTreshHold[wSubID]= UPPER_BOUNDARY_LUMI*20/1000;
		// Den unteren Mask-Treshhold festlegen
		m_nLowerMaskTreshHold[wSubID]= UPPER_BOUNDARY_LUMI*10/1000;
		// Den Maskenincrement festlegen
		m_nMaskInc[wSubID]	= UPPER_BOUNDARY_MASK*20/1000;
		// Den Maskendecrement festlegen
		m_nMaskDec[wSubID]	= UPPER_BOUNDARY_MASK*5/1000;
		// Adaptive Maske aktivieren
		ActivateAdaptiveMask(wSubID);
	}
	else if (eLevel == CSD_LOW)
	{
		// Den oberen Mask-Treshhold festlegen
		m_nUpperMaskTreshHold[wSubID]= UPPER_BOUNDARY_LUMI*40/1000;
		// Den unteren Mask-Treshhold festlegen
		m_nLowerMaskTreshHold[wSubID]= UPPER_BOUNDARY_LUMI*20/1000;
		// Den Maskenincrement festlegen
		m_nMaskInc[wSubID]	= UPPER_BOUNDARY_MASK*20/1000;
		// Den Maskendecrement festlegen
		m_nMaskDec[wSubID]	= UPPER_BOUNDARY_MASK*5/1000;
		// Adaptive Maske aktivieren
		ActivateAdaptiveMask(wSubID);
	}
	else if (eLevel == CSD_OFF)
	{
		// Den oberen Mask-Treshhold festlegen
		m_nUpperMaskTreshHold[wSubID]= UPPER_BOUNDARY_LUMI*20/1000;
		// Den unteren Mask-Treshhold festlegen
		m_nLowerMaskTreshHold[wSubID]= UPPER_BOUNDARY_LUMI*10/1000;
		// Den Maskenincrement festlegen
		m_nMaskInc[wSubID]	= UPPER_BOUNDARY_MASK*20/1000;
		// Den Maskendecrement festlegen
		m_nMaskDec[wSubID]	= UPPER_BOUNDARY_MASK*5/1000;
		// Adaptive Maske deaktivieren
		DeactivateAdaptiveMask(wSubID);
	}
	else
		WK_TRACE_WARNING("TARGET: Unknown MDMaskSensitivity (Level=0x%x)\n", eLevel);
}

/////////////////////////////////////////////////////////////////////////////
void CMotionDetection::SetMDAlarmSensitivity(WORD wSubID, eMDLevel eLevel)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::SetMDAlarmSensitivity\tWrong CamID (%d)\n", wSubID);
		return;
	}

	// Einstellung sofort in der Registry sichern
//	char szBuffer[255];
//	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
//	WriteConfigurationString(szBuffer, "AlarmSensitivity", sLevel, m_sIniName); 

	if (eLevel == CSD_HIGH)
	{
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[wSubID] = UPPER_BOUNDARY_LUMI*50/1000;
		ActivateMotionDetection(wSubID);
	}
	else if (eLevel == CSD_NORMAL)
	{
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[wSubID] = UPPER_BOUNDARY_LUMI*150/1000;
		ActivateMotionDetection(wSubID);
	}
	else if (eLevel == CSD_LOW)
	{						 
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[wSubID] = UPPER_BOUNDARY_LUMI*250/1000;
		ActivateMotionDetection(wSubID);
	}
	else if (eLevel == CSD_OFF)
	{						 
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[wSubID] = UPPER_BOUNDARY_LUMI*250/1000;
		DeactivateMotionDetection(wSubID);
	}
	else
		WK_TRACE_WARNING("TARGET: Unknown MDAlarmSensitivity (Level:9x%x)\n", eLevel);
}

/////////////////////////////////////////////////////////////////////////////
eMDLevel CMotionDetection::GetMDMaskSensitivity(WORD wSubID) const
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::GetMDMaskSensitivity\tWrong CamID (%d)\n", wSubID);
		return CSD_NORMAL;
	}

//	char szSection[255] = {0};
//	char szBuffer[255]	= {0};
//	wsprintf(szSection, "Mapping\\Source%02u\\MD", wSubID);

//	ReadConfigurationString(szSection, "MaskSensitivity", CSD_NORMAL, szBuffer, 16, m_sIniName);
	eMDLevel Level = CSD_OFF;

	return Level;
}

/////////////////////////////////////////////////////////////////////////////
eMDLevel CMotionDetection::GetMDAlarmSensitivity(WORD wSubID) const
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("TARGET: CMotionDetection::GetMDAlarmSensitivity\tWrong CamID (%d)\n", wSubID);
		return CSD_NORMAL;
	}

//	char szSection[255] = {0};
//	char szBuffer[255]	= {0};
//	wsprintf(szSection, "Mapping\\Source%02u\\MD", wSubID);

//	ReadConfigurationString(szSection, "AlarmSensitivity", CSD_NORMAL, szBuffer, 16, m_sIniName);
	eMDLevel Level = CSD_HIGH;

	return Level;
}


// Rechteckfilter mit folgender Matrix: 
//					  | 1 | 1 | 1 |
//	P'	=	P x 1/9 * | 1 | 1 | 1 |
//				      | 1 | 1 | 1 |
// Die Randelemente werden nicht berücksichtigt.
// Die Filterung benötigt etwa 4,4ms auf einem PII 300 (ca. 46clk/pixel)
// Die Filterung benötigt etwa 4,3ms auf einem Trimedia1300 180MHz (ca. 32clk/pixel)
/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::TPFilter(const exVideoBuffer* pVBufferOut, const exVideoBuffer* pVBufferIn)
{
	int		nY		= 0;	
	DWORD	dwI		= 0;
	DWORD	dwIndex = 0;

	if (!pVBufferIn || !pVBufferOut)
		return FALSE;

	// Divisionstabelle für das Rechteckfilter.
	BYTE	byDiv9[9*256];
	for (int nI = 0; nI < 9*256; nI++)
		byDiv9[nI] = nI/9;

	int		nOffsets[9];
	
	//	FastZeroMem(pDest, CAPTURE_SIZE_H*CAPTURE_SIZE_V*sizeof(BGR24));

	if (pVBufferIn->videoBuffer.Y != NULL)
	{
		int nStride		= pVBufferIn->videoBuffer.yPitch;
		BYTE*	pSrc	= pVBufferIn->videoBuffer.Y;
		BYTE*	pDest	= pVBufferOut->videoBuffer.Y;

		nOffsets[0] = -1-nStride;
		nOffsets[1] = -0-nStride;
		nOffsets[2] = +1-nStride;
		nOffsets[3] = -1;
		nOffsets[4] = 0;
		nOffsets[5] = +1;
		nOffsets[6] = -1+nStride;
		nOffsets[7] = -0+nStride;
		nOffsets[8] = +1+nStride;

		// Tiefpassfilterung durchführen
		for (nY = 1; nY < pVBufferIn->videoBuffer.nHeight-1; nY++)
		{
			dwIndex = nY*nStride;
			for (dwI = dwIndex+1; dwI < dwIndex+pVBufferIn->videoBuffer.nWidth-1; dwI++)
			{
#if(0)
				pDest[dwI] = byDiv9[(pSrc[dwI+nOffsets[0]]+
									 pSrc[dwI+nOffsets[1]]+
									 pSrc[dwI+nOffsets[2]]+
									 pSrc[dwI+nOffsets[3]]+
									 pSrc[dwI+nOffsets[4]]+
									 pSrc[dwI+nOffsets[5]]+
									 pSrc[dwI+nOffsets[6]]+
									 pSrc[dwI+nOffsets[7]]+
									 pSrc[dwI+nOffsets[8]])];
#else
				pDest[dwI] = byDiv9[(pSrc[dwI-1-nStride]+
									 pSrc[dwI-0-nStride]+
									 pSrc[dwI+1-nStride]+
									 pSrc[dwI-1]+
									 pSrc[dwI]+
									 pSrc[dwI+1]+
									 pSrc[dwI-1+nStride]+
									 pSrc[dwI-0+nStride]+
									 pSrc[dwI+1+nStride])];
#endif
			}
		}
	}

	if (pVBufferIn->videoBuffer.U != NULL)
	{
		int nStride = pVBufferIn->videoBuffer.uvPitch;
		BYTE*	pSrc	= pVBufferIn->videoBuffer.U;
		BYTE*	pDest	= pVBufferOut->videoBuffer.U;
		
		// Tiefpassfilterung durchführen
		for (nY = 1; nY < pVBufferIn->videoBuffer.nHeight-1; nY++)
		{
			dwIndex = nY*nStride;
			for (dwI = dwIndex+1; dwI < dwIndex+pVBufferIn->videoBuffer.nWidth-1; dwI++)
			{
				pDest[dwI] = byDiv9[(pSrc[dwI-1-nStride]+
									 pSrc[dwI-0-nStride]+
									 pSrc[dwI+1-nStride]+
									 pSrc[dwI-1]+
									 pSrc[dwI]+
									 pSrc[dwI+1]+
									 pSrc[dwI-1+nStride]+
									 pSrc[dwI-0+nStride]+
									 pSrc[dwI+1+nStride])];
			}
		}
	}

	if (pVBufferIn->videoBuffer.V != NULL)
	{
		int nStride = pVBufferIn->videoBuffer.uvPitch;
		BYTE*	pSrc	= pVBufferIn->videoBuffer.V;
		BYTE*	pDest	= pVBufferOut->videoBuffer.V;
		
		// Tiefpassfilterung durchführen
		for (nY = 1; nY < pVBufferIn->videoBuffer.nHeight-1; nY++)
		{
			dwIndex = nY*nStride;
			for (dwI = dwIndex+1; dwI < dwIndex+pVBufferIn->videoBuffer.nWidth-1; dwI++)
			{
				pDest[dwI] = byDiv9[(pSrc[dwI-1-nStride]+
									 pSrc[dwI-0-nStride]+
									 pSrc[dwI+1-nStride]+
									 pSrc[dwI-1]+
									 pSrc[dwI]+
									 pSrc[dwI+1]+
									 pSrc[dwI-1+nStride]+
									 pSrc[dwI-0+nStride]+
									 pSrc[dwI+1+nStride])];
			}
		}
	}
	return TRUE;
}

