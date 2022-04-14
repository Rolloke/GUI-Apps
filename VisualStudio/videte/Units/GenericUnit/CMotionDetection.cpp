/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: CMotionDetection.cpp $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/CMotionDetection.cpp $
// CHECKIN:		$Date: 20.01.06 12:40 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 20.01.06 12:40 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <direct.h>
#include <math.h>
#include "CMotionDetection.h"
#include "CMDConfigDlg.h"
#include "DirectDraw.h"				   
#include "CCodec.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

WORD wMask[] ={0x0000, 0x0001, 0x0003, 0x0007,
			   0x000f, 0x001f, 0x003f, 0x007f,
			   0x00ff, 0x01ff, 0x03ff, 0x07ff,
			   0x0fff, 0x1fff,	0x3fff, 0x7fff,
			   0xffff};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CMotionDetection::CMotionDetection(const CString& sIniName)
{
	int nI;
	char szBuffer[255]		={0};
	
	m_bOK					= FALSE;
	m_sIniName				= sIniName;
	m_pDCInlayWnd			= NULL;
	m_pDCDiffWnd			= NULL;
	m_pMDConfigDlg			= NULL;
	m_hDevice				= NULL;
	m_pCodec				= NULL;
	m_bShowMask				= FALSE;
	m_pDirectDraw			= NULL;
	m_dwScreenBaseAddr		= 0;	// Bildschirmspeicheradresse
	m_pCJpeg				= new CJpeg();

	char szPath[_MAX_PATH];
	CString sHomePath = "";
	HINSTANCE hInstance = AfxGetInstanceHandle( );
	if (hInstance)
	{
		GetModuleFileName(hInstance, szPath, sizeof(szPath));
		sHomePath = szPath;
		nI = sHomePath.ReverseFind('\\');
		if (nI != -1)
			sHomePath = sHomePath.Left(nI+1);
	}

	// Pfad zum Speichern der Masken-dateien einlesen
	ReadConfigurationString(_T("MotionDetection"), _T("MaskPath"), _T(""), szBuffer,  255, m_sIniName); 
	m_sMaskPath = sHomePath+szBuffer;
	if (!m_sMaskPath.IsEmpty())
		_mkdir(m_sMaskPath);
	
	for (nI = 0; nI < MAX_CAMERAS; nI++)
	{
		m_wField[nI]			= 0;  
		m_dwTimeStamp[nI]		= 0;
		m_pFrameBuffers[nI][0]	= NULL;
		m_pFrameBuffers[nI][1]	= NULL;
		m_bValidFrame[nI][0]	= FALSE;
		m_bValidFrame[nI][1]	= FALSE;
		
		LoadParameter(nI);
		LoadMask(nI);
	}

	// Quadrat-Tabelle erstellen
	for (nI = -255; nI <= 255; nI++)
		m_nQuad[nI+255] = nI*nI;

	// Wurzel-Tabelle erstellen
	for (nI = 0; nI <= 3*(255*255); nI++)
		m_nSqrt[nI] = (int)sqrt((double)nI);

	// Pointer zeigen auf die Mitte der Tabelle, damit auch negative Tabellenindexe
	// erlaubt sind.
	m_pQuad	= &m_nQuad[255];
}

//////////////////////////////////////////////////////////////////////
CMotionDetection::~CMotionDetection()
{
	char szBuffer[255];

	for (int nI = 0; nI < MAX_CAMERAS; nI++)
	{
		wsprintf(szBuffer, "Mapping\\Source%02u\\MD", nI);
		WriteConfigurationInt(szBuffer, "Histogramm", m_bHistogramm[nI], m_sIniName);	
		WriteConfigurationInt(szBuffer, "TPFilter", m_bFilter[nI], m_sIniName);	

		SaveMask(nI);
		WK_DELETE(m_pFrameBuffers[nI][0]);
		WK_DELETE(m_pFrameBuffers[nI][1]);
	}

	WK_DELETE(m_pMDConfigDlg);
	WK_DELETE(m_pDCInlayWnd);
	WK_DELETE(m_pDCDiffWnd);
	WK_DELETE(m_pDirectDraw);

	m_bOK	= FALSE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::OpenConfigDlg()
{
	if (m_pMDConfigDlg)
		m_pMDConfigDlg->ShowConfigDlg();
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::Init(CCodec* pCodec)
{
	m_pCodec = pCodec;

	// Informationen über das Ausgabedevice erfragen
	HDC hDC				= CreateIC("DISPLAY", NULL, NULL, NULL);
	m_nHScreenRes		= GetDeviceCaps(hDC, HORZRES);				// Pixel pro Zeile
	m_nVScreenRes		= GetDeviceCaps(hDC, VERTRES);				// Anzahl der Zeilen
	m_nPlanes			= (WORD)GetDeviceCaps(hDC, PLANES);		// Anzahl der Planes
	m_nBitsPerPixel		= (WORD)GetDeviceCaps(hDC, BITSPIXEL);	// Bits pro Pixel
	DeleteDC(hDC);


	// Die direkte Zugriff auf die Bitmap erfolgt nur mit 15/16/24/32-Bit Farbtiefe
	if ((m_nPlanes != 1) ||	(m_nBitsPerPixel != 15 && m_nBitsPerPixel != 16 && m_nBitsPerPixel != 24 && m_nBitsPerPixel != 32))
	{
		WK_TRACE_ERROR("Falsche Farbtiefe (%d Bits)\n", m_nBitsPerPixel);
		return FALSE;
	}

	// Unterscheidung zwischen 15Bit und 16Bit Farbtiefe
	switch (CheckIf15Or16BitsPerPixel(m_nBitsPerPixel))
	{
		case 15:
			m_wRBits		= 5;	
			m_wGBits		= 5;
			m_wBBits		= 5;
			break;
		case 16:
			m_wRBits		= 5;	
			m_wGBits		= 6;
			m_wBBits		= 5;
			break;
		case 24:
			m_wRBits		= 8;	
			m_wGBits		= 8;
			m_wBBits		= 8;
			break;
		case 32:
			m_wRBits		= 8;	
			m_wGBits		= 8;
			m_wBBits		= 8;
			break;
	}
	if (!m_pDirectDraw)
	{
		m_pDirectDraw = new CDirectDraw(m_nHScreenRes, m_nVScreenRes);
		if (!m_pDirectDraw)
			return FALSE;
		if (!m_pDirectDraw->CreatePrimarySurface())
		{
			WK_TRACE_ERROR("Can't create PrimarySurface\n");
			return FALSE;
		}
		WK_TRACE("Using Video-Inlay (Primary Surface)\n");
	}

	if (!m_dwScreenBaseAddr)
	{
		m_dwScreenBaseAddr	= (DWORD)m_pDirectDraw->LockPrimarySurface();
		m_pDirectDraw->UnlockPrimarySurface();
		m_bOK	= TRUE;
	}

	// Dialog Motiondetection anlegen
	if (!m_pMDConfigDlg)
		m_pMDConfigDlg = new CMDConfigDlg(this, pCodec);
	
	if (m_pMDConfigDlg)
	{
		if (!m_pDCInlayWnd)
			m_pDCInlayWnd = new CWindowDC(m_pMDConfigDlg->GetInlayWnd());
		
		if (!m_pDCDiffWnd)
			m_pDCDiffWnd  = new CWindowDC(m_pMDConfigDlg->GetDiffWnd());
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::IsValid() const
{
	return m_bOK;
}


//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::CaptureFrame(WORD wSubID, BGR24* pFrame)
{
	if (!pFrame)
		return FALSE;

	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::CaptureFrame\tWrong CamID (%d)\n", wSubID);
		return FALSE;
	}

	
	CAPTURE_STRUCT Capture;
	Capture.bAutoLuminance	= FALSE;	// Wird nun im Differenzbild erledigt.
	Capture.bHistogramm		= m_bHistogramm[wSubID];
	Capture.bFilter			= m_bFilter[wSubID];
	Capture.pFrameBuffer	= pFrame;

	BOOL bRet = DACaptureFrame(wSubID, Capture);

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::MotionCheck(WORD wSubID, CPoint &Point)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::MotionCheck\tWrong CamID (%d)\n", wSubID);
		return FALSE;
	}

	// MotionDetection aktiviert?
	if (!m_bActivateMD[wSubID])
		return FALSE;

	// Alle 250ms neuen Vergleich durchführen
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
		m_pFrameBuffers[wSubID][0] = new BGR24[CAPTURE_SIZE_H * CAPTURE_SIZE_V+4];
		m_pFrameBuffers[wSubID][1] = new BGR24[CAPTURE_SIZE_H * CAPTURE_SIZE_V+4];
	}

	// Ein Frame grabben
	if (!CaptureFrame(wSubID, m_pFrameBuffers[wSubID][m_wField[wSubID]]))
		return FALSE;

	m_bValidFrame[wSubID][m_wField[wSubID]] = TRUE;

	// Zeitpunkt des Grabs vermerken
	m_dwTimeStamp[wSubID] = GetTickCount();

	ShowOriginalFrame(wSubID);

	// Wurden schon 2 Frames gegrapt?
	if (!m_bValidFrame[wSubID][0] || !m_bValidFrame[wSubID][1])
		return FALSE;

	return CompareImages(wSubID, Point);
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::CompareImages(WORD wSubID, CPoint &Point)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::CompareImages\tWrong CamID (%d)\n", wSubID);
		return FALSE;
	}

	ZeroMemory(&m_nDiffs[0][0], sizeof(int) * CAPTURE_SIZE_H * CAPTURE_SIZE_V / 64);
	int	 nX				= 0;
	int	 nY				= 0;
	int	 nX1			= 0;
	int	 nY1			= 0;
	int  nMittelDiff	= 0;
	int  nMask			= 0;
	int  nMaxDiff		= 0;
	int  nMaxDiffX		= -1;
	int	 nMaxDiffY		= -1;

	// Differenzbild ermitteln
	// dy = Sqrt((r2-r1)^2 + (g2-g1)^2 + (b2-b1)^2)
	BYTE* p1 = (BYTE*)&m_pFrameBuffers[wSubID][0][0]; // Pointer auf 1. Frame
	BYTE* p2 = (BYTE*)&m_pFrameBuffers[wSubID][1][0]; // Pointer auf 2. Frame

	nY1 = 0;
	for (nY=0; nY < CAPTURE_SIZE_V; nY++, nY1 = nY/8)
	{
		nX1 = 0;
		for (nX=0; nX < CAPTURE_SIZE_H; nX+=8, nX1 = nX/8)
		{
			m_nDiffs[nX1][nY1] += Betrag(*p2++-*p1++,*p2++-*p1++,*p2++-*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++-*p1++,*p2++-*p1++,*p2++-*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++-*p1++,*p2++-*p1++,*p2++-*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++-*p1++,*p2++-*p1++,*p2++-*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++-*p1++,*p2++-*p1++,*p2++-*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++-*p1++,*p2++-*p1++,*p2++-*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++-*p1++,*p2++-*p1++,*p2++-*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++-*p1++,*p2++-*p1++,*p2++-*p1++);
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

	// Den Block mit der Maximaldifferenz ermitteln und Maske auf-/abbauen
	for (nY1 = 0; nY1 < CAPTURE_SIZE_V/8; nY1++)
	{
		for (nX1=0; nX1 < CAPTURE_SIZE_H/8; nX1++)
		{
			// Mittlere Helligkeit subtrahieren. (Nur den AC-Anteil bewerten)
			m_nDiffs[nX1][nY1] = max(m_nDiffs[nX1][nY1] - nMittelDiff, 0);

			if (m_bUseAdaptiveMask[wSubID])
			{
				// Adaptive Bewegungsstörmaske aufbauen/abbauen.
				if (m_nDiffs[nX1][nY1] > m_nUpperMaskTreshHold[wSubID])
				{
					m_Mask[wSubID].nAdaptiveMask[nX1][nY1] += m_nMaskInc[wSubID];
					m_Mask[wSubID].nAdaptiveMask[nX1][nY1]  = min(m_Mask[wSubID].nAdaptiveMask[nX1][nY1], UPPER_BOUNDARY_MASK);
				}
				else if (m_nDiffs[nX1][nY1] <= m_nLowerMaskTreshHold[wSubID])
				{
					m_Mask[wSubID].nAdaptiveMask[nX1][nY1] -= m_nMaskDec[wSubID];
					m_Mask[wSubID].nAdaptiveMask[nX1][nY1]  = max(m_Mask[wSubID].nAdaptiveMask[nX1][nY1], LOWER_BOUNDARY_MASK);
				}
			}

			// Differenz mit den Masken wichten
			nMask =  min(m_Mask[wSubID].nPermanentMask[nX1][nY1] + m_Mask[wSubID].nAdaptiveMask[nX1][nY1], UPPER_BOUNDARY_MASK);
			m_nDiffs[nX1][nY1] = (m_nDiffs[nX1][nY1]*(UPPER_BOUNDARY_MASK-nMask))/UPPER_BOUNDARY_MASK;

			if (m_nDiffs[nX1][nY1] > nMaxDiff)
			{						    
				nMaxDiff	= m_nDiffs[nX1][nY1];
				nMaxDiffX 	= nX1*8+4; //+3;
				nMaxDiffY	= nY1*8+4; //+3;
				//TRACE("nMaxDiffX=%d, nX1=%d\n", nMaxDiffX, nX1);

				// Ort der Bewegung zurückmelden
				Point.x = (nMaxDiffX*1000 / CAPTURE_SIZE_H);
				Point.y	= (nMaxDiffY*1000 / CAPTURE_SIZE_V);
			}
		}
	}								   

	if (m_pMDConfigDlg && m_pMDConfigDlg->IsVisible())
	{
		// Differenzbild ausgeben
		ShowDifferenceFrame(wSubID);

		// Liegt die Differenz oberhalb des Grenzwertes, dann zeichne Fadenkreuz
		if (nMaxDiff > m_nAlarmTreshHold[wSubID])
		{
			if (wSubID == m_pMDConfigDlg->GetSelectedCamera())
			{							  
				m_pDCInlayWnd->SelectStockObject(WHITE_PEN);
				m_pDCInlayWnd->MoveTo(nMaxDiffX, 3);
				m_pDCInlayWnd->LineTo(nMaxDiffX, CAPTURE_SIZE_V+3);
				m_pDCInlayWnd->MoveTo(3, nMaxDiffY);
				m_pDCInlayWnd->LineTo(CAPTURE_SIZE_H+3, nMaxDiffY);
				CRect rc(nMaxDiffX-4, nMaxDiffY-4, nMaxDiffX+4, nMaxDiffY+4); 
				m_pDCInlayWnd->Ellipse(rc);			
				// WK_TRACE("Cam:%d MaxDiff=%d\n", wSubID, nMaxDiff);
			}
		}
	}

	return (nMaxDiff > m_nAlarmTreshHold[wSubID]);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::LoadParameter(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::LoadParameter\tWrong CamID (%d)\n", wSubID);
		return;
	}

	char szBuffer[255]		={0};
	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
	
	// Histogramm? Ja/Nein.
	m_bHistogramm[wSubID]	= ReadConfigurationInt(szBuffer, "Histogramm", 0, m_sIniName);	

	// Filter? Ja/Nein.
	m_bFilter[wSubID]		= ReadConfigurationInt(szBuffer, "TPFilter", 1, m_sIniName);	

	// Mindest-, und Maximalzeit zwischen zwei Differentbildern
	m_nMinTime[wSubID] = ReadConfigurationInt(szBuffer, "MinTime", 250,   m_sIniName); 
	m_nMaxTime[wSubID] = ReadConfigurationInt(szBuffer, "MaxTime", 30000, m_sIniName); 

	// Die MD-Parameter in Abhängigkeit der Sensitivity-level laden
	SetMDMaskSensitivity(wSubID, GetMDMaskSensitivity(wSubID));
	SetMDAlarmSensitivity(wSubID, GetMDAlarmSensitivity(wSubID));
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::SaveMask(WORD wSubID)
{									  
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::SaveMask\tWrong CamID (%d)\n", wSubID);
		return FALSE;
	}

	BOOL bRet = TRUE;
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

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::LoadMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::LoadMask\tWrong CamID (%d)\n", wSubID);
		return FALSE;
	}

	BOOL bRet = TRUE;

	// Alte Maske löschen
	ClearMask(wSubID);

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

	return bRet;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ClearMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::ClearMask\tWrong CamID (%d)\n", wSubID);
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
		WK_TRACE_ERROR("CMotionDetection::ClearAdaptiveMask\tWrong CamID (%d)\n", wSubID);
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
		WK_TRACE_ERROR("CMotionDetection::ClearPermanentMask\tWrong CamID (%d)\n", wSubID);
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
		WK_TRACE_ERROR("CMotionDetection::InvertPermanentMask\tWrong CamID (%d)\n", wSubID);
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
		WK_TRACE_ERROR("CMotionDetection::InvertAdaptiveMask\tWrong CamID (%d)\n", wSubID);
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
	COLORREF col;

	if (wSubID >= MAX_CAMERAS)
		return;

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
															 
			if (m_bShowMask)
				SetPixel(IDC_INLAY_WND, nX, nY, col);

 			SetPixel(IDC_DIFF_WND, nX, nY, col);
		}
	}							    
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
		WK_TRACE_ERROR("CMotionDetection::ToggleAdaptiveMaskActivation\tWrong CamID (%d)\n", wSubID);
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
		WK_TRACE_ERROR("CMotionDetection::ActivateAdaptiveMask\tWrong CamID (%d)\n", wSubID);
		return;
	}

	m_bUseAdaptiveMask[wSubID] = TRUE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateAdaptiveMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::DeactivateAdaptiveMask\tWrong CamID (%d)\n", wSubID);
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
		WK_TRACE_ERROR("CMotionDetection::ToggleMDActivation\tWrong CamID (%d)\n", wSubID);
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
		WK_TRACE_ERROR("CMotionDetection::ActivateMotionDetection\tWrong CamID (%d)\n", wSubID);
		return;
	}

	m_bActivateMD[wSubID] = TRUE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateMotionDetection(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::DeactivateMotionDetection\tWrong CamID (%d)\n", wSubID);
		return;
	}

	m_bActivateMD[wSubID] = FALSE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleHistogramm(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::ToggleHistogramm\tWrong CamID (%d)\n", wSubID);
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
		WK_TRACE_ERROR("CMotionDetection::ActivateHistogramm\tWrong CamID (%d)\n", wSubID);
		return;
	}

	m_bHistogramm[wSubID] = TRUE;

	// Einstellung sofort in der Registry sichern
	char szBuffer[255];
	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
	WriteConfigurationInt(szBuffer, "Histogramm", m_bHistogramm[wSubID], m_sIniName);	
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateHistogramm(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::DeactivateHistogramm\tWrong CamID (%d)\n", wSubID);
		return;
	}
	m_bHistogramm[wSubID] = FALSE;

	// Einstellung sofort in der Registry sichern
	char szBuffer[255];
	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
	WriteConfigurationInt(szBuffer, "Histogramm", m_bHistogramm[wSubID], m_sIniName);	
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleFilter(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::ToggleFilter\tWrong CamID (%d)\n", wSubID);
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
		WK_TRACE_ERROR("CMotionDetection::ActivateFilter\tWrong CamID (%d)\n", wSubID);
		return;
	}

	m_bFilter[wSubID] = TRUE;

	// Einstellung sofort in der Registry sichern
	char szBuffer[255];
	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
	WriteConfigurationInt(szBuffer, "TPFilter", m_bFilter[wSubID], m_sIniName);	
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateFilter(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::DeactivateFilter\tWrong CamID (%d)\n", wSubID);
		return;
	}
	m_bFilter[wSubID] = FALSE;

	// Einstellung sofort in der Registry sichern
	char szBuffer[255];
	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
	WriteConfigurationInt(szBuffer, "TPFilter", m_bFilter[wSubID], m_sIniName);	
}

//////////////////////////////////////////////////////////////////////
// This routine tries to determine if the number of bits per pixel is either 15 or 16.
// First, a bitmap compatible to the system screen bitmap is created. Then the physical
// data format of green and violet pixels is compared to the expected values for 15 and
// 16 bit.
int CMotionDetection::CheckIf15Or16BitsPerPixel(int nBitsPerPixel)
{
	HBITMAP hBitmap			= NULL;
	HDC		hMemDC			= NULL;
	WORD	BitmapData[256] ={0};

	// Überprüfung nur bei 15 und 16Bit Farbtiefe
	if ((nBitsPerPixel != 15 && nBitsPerPixel != 16))
		return nBitsPerPixel;
						  
	hBitmap	= CreateCompatibleBitmap(GetDC(NULL), 8, 8);
	hMemDC	= CreateCompatibleDC(NULL);

	if (!hBitmap || !hMemDC)
		return nBitsPerPixel;

	SelectObject(hMemDC,  hBitmap);

	::SetPixel(hMemDC, 0, 0, RGB(0, 255, 0));
	::SetPixel(hMemDC, 1, 0, RGB(255, 0, 255));
	::GetBitmapBits(hBitmap, 32, BitmapData);

	if (BitmapData[0] == 0x03e0 && BitmapData[1] == 0x7c1f)			// Patterns for 15 bit depth.
		nBitsPerPixel = 15;
	else if (BitmapData[0] == 0x07e0 && BitmapData[1] == 0xf81f)	// Patterns for 16 bit depth.
		nBitsPerPixel = 16;

	DeleteDC(hMemDC);
	DeleteObject(hBitmap);

	return nBitsPerPixel;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ShowOriginalFrame(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::ShowOriginalFrame\tWrong CamID (%d)\n", wSubID);
		return;
	}

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
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ShowDifferenceFrame(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::ShowDifferenceFrame\tWrong CamID (%d)\n", wSubID);
		return;
	}

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
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::SetPixel(int nID, int nX, int nY, COLORREF col)
{
	m_pMDConfigDlg->ClientToScreen(nID, nX, nY);
	nX = max(nX, 0);
	nY = max(nY, 0);
	if ((nX >= m_nHScreenRes) || (nY >= m_nVScreenRes))
		return;

	DWORD r	= ((DWORD)GetRValue(col)>>(8-m_wRBits)) & (DWORD)wMask[m_wRBits];
	DWORD g	= ((DWORD)GetGValue(col)>>(8-m_wGBits)) & (DWORD)wMask[m_wGBits];
	DWORD b	= ((DWORD)GetBValue(col)>>(8-m_wBBits)) & (DWORD)wMask[m_wBBits];
	DWORD dwIndex;
	
	switch (m_nBitsPerPixel)
	{
		case 15:
		case 16:
 			((WORD*)m_dwScreenBaseAddr)[nY * m_nHScreenRes  + nX] = (WORD)((r<<(m_wGBits+m_wBBits)) | (g<<m_wRBits) | b);
			break;
		case 24:
			dwIndex = 3 * (nY * m_nHScreenRes + nX);
			((BYTE*)m_dwScreenBaseAddr)[dwIndex++] = (BYTE)b;
			((BYTE*)m_dwScreenBaseAddr)[dwIndex++] = (BYTE)g;
			((BYTE*)m_dwScreenBaseAddr)[dwIndex++] = (BYTE)r;
			break;
		case 32:
			((DWORD*)m_dwScreenBaseAddr)[nY * m_nHScreenRes + nX] = (r<<(m_wGBits+m_wBBits)) | (g<<m_wRBits) | b;
			break;
	}
}

//////////////////////////////////////////////////////////////////////
// Wertebereich der Tabelle -255 <= nR,nG,nB <= +255
inline int CMotionDetection::Betrag(int nR, int nG, int nB)
{
	return m_nSqrt[(m_pQuad[nR] + m_pQuad[nG] + m_pQuad[nB])];
}

/////////////////////////////////////////////////////////////////////////////
void CMotionDetection::SetMDMaskSensitivity(WORD wSubID, const CString &sLevel)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::SetMDMaskSensitivity\tWrong CamID (%d)\n", wSubID);
		return ;
	}

	// Einstellung sofort in der Registry sichern
	char szBuffer[255];
	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
	WriteConfigurationString(szBuffer, "MaskSensitivity", sLevel, m_sIniName); 

	if (sLevel == CSD_HIGH)
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
	else if (sLevel == CSD_NORMAL)
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
	else if (sLevel == CSD_LOW)
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
	else if (sLevel == CSD_OFF)
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
		WK_TRACE_WARNING("Unknown MDMaskSensitivity\n");

	if (m_pMDConfigDlg)
		m_pMDConfigDlg->EnableDlgCtrl();
}

/////////////////////////////////////////////////////////////////////////////
void CMotionDetection::SetMDAlarmSensitivity(WORD wSubID, const CString &sLevel)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::SetMDAlarmSensitivity\tWrong CamID (%d)\n", wSubID);
		return;
	}

	// Einstellung sofort in der Registry sichern
	char szBuffer[255];
	wsprintf(szBuffer, "Mapping\\Source%02u\\MD", wSubID);
	WriteConfigurationString(szBuffer, "AlarmSensitivity", sLevel, m_sIniName); 

	if (sLevel == CSD_HIGH)
	{
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[wSubID] = UPPER_BOUNDARY_LUMI*50/1000;
		ActivateMotionDetection(wSubID);
	}
	else if (sLevel == CSD_NORMAL)
	{
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[wSubID] = UPPER_BOUNDARY_LUMI*150/1000;
		ActivateMotionDetection(wSubID);
	}
	else if (sLevel == CSD_LOW)
	{						 
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[wSubID] = UPPER_BOUNDARY_LUMI*250/1000;
		ActivateMotionDetection(wSubID);
	}
	else if (sLevel == CSD_OFF)
	{						 
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[wSubID] = UPPER_BOUNDARY_LUMI*250/1000;
		DeactivateMotionDetection(wSubID);
	}
	else
		WK_TRACE_WARNING("Unknown MDAlarmSensitivity\n");

	if (m_pMDConfigDlg)
		m_pMDConfigDlg->EnableDlgCtrl();
}

/////////////////////////////////////////////////////////////////////////////
CString CMotionDetection::GetMDMaskSensitivity(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::GetMDMaskSensitivity\tWrong CamID (%d)\n", wSubID);
		return CSD_NORMAL;
	}

	char szSection[255] = {0};
	char szBuffer[255]	= {0};
	wsprintf(szSection, "Mapping\\Source%02u\\MD", wSubID);

	ReadConfigurationString(szSection, "MaskSensitivity", CSD_NORMAL, szBuffer, 16, m_sIniName);
	return szBuffer;
}

/////////////////////////////////////////////////////////////////////////////
CString CMotionDetection::GetMDAlarmSensitivity(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::GetMDAlarmSensitivity\tWrong CamID (%d)\n", wSubID);
		return CSD_NORMAL;
	}

	char szSection[255] = {0};
	char szBuffer[255]	= {0};
	wsprintf(szSection, "Mapping\\Source%02u\\MD", wSubID);

	ReadConfigurationString(szSection, "AlarmSensitivity", CSD_NORMAL, szBuffer, 16, m_sIniName);
	return szBuffer;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::DACaptureFrame(WORD wSubID, CAPTURE_STRUCT &Capture)
{
	BOOL bResult = FALSE;
	JPEG *pJpeg	= NULL;
	
	if (m_pCodec)
	{
		pJpeg = m_pCodec->ReadJpegFile(TRUE);

		if (pJpeg && m_pCJpeg)
		{
			if (m_pCJpeg->DecodeJpegFromMemory((BYTE*)pJpeg->Buffer.pLinAddr, pJpeg->Buffer.dwLen, FALSE))
				bResult = ScaleDown(Capture);
		
			WK_DELETE(pJpeg->Buffer.pLinAddr);
			WK_DELETE(pJpeg);
		}
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::ScaleDown(CAPTURE_STRUCT &Capture)
{
	LPBITMAPINFO lpBMI = NULL;
	BOOL	bResult = FALSE;

	if (m_pCJpeg)
	{
		HANDLE hDib = m_pCJpeg->CreateDIB();
		if (hDib)
		{
			lpBMI = (LPBITMAPINFO)GlobalLock(hDib);
			if (lpBMI)
			{
				CRect rectTo(0,0, CAPTURE_SIZE_H, CAPTURE_SIZE_V);
				CRect rectFrom(0,0, lpBMI->bmiHeader.biHeight, lpBMI->bmiHeader.biHeight);

				double dX = lpBMI->bmiHeader.biWidth  / (double)CAPTURE_SIZE_H;
				double dY = lpBMI->bmiHeader.biHeight / (double)CAPTURE_SIZE_V;
				double fX = 0.0;
				double fY = 0.0;
				BGR24* pSrc = (BGR24*)lpBMI->bmiColors;
				BGR24* pDest= (BGR24*) Capture.pFrameBuffer;

				for (int nY = CAPTURE_SIZE_V-1; nY >= 0; nY--)
				{
					for (int nX = 0; nX < CAPTURE_SIZE_H; nX++)
					{
						DWORD dwOffset = fX+lpBMI->bmiHeader.biWidth*(int)fY;
						fX +=dX;
						pDest[nX+CAPTURE_SIZE_H*nY] = pSrc[dwOffset];
					}
					fX = 0.0;
					fY +=dY;
				}

				GlobalUnlock(hDib);
				bResult = TRUE;
			}
			GlobalFree(hDib);
		}		
	}

	return bResult;
}
