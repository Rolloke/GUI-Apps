/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: CMotionDetection.cpp $
// ARCHIVE:		$Archive: /Project/Units/QUnit/CMotionDetection.cpp $
// CHECKIN:		$Date: 1.12.05 16:09 $
// VERSION:		$Revision: 13 $
// LAST CHANGE:	$Modtime: 1.12.05 15:28 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <direct.h>
#include "CMotionDetection.h"
#include "CMDConfigDlg.h"
#include "CMDPoints.h"
#include "CUDP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CMotionDetection::CMotionDetection(CUDP* pUDP, const CString& sIniName)
{
	int nI;
	_TCHAR szBuffer[255]		={0};
	
	m_pUDP					= pUDP;
	m_bOK					= FALSE;
	m_nHStart				= 0;
	m_nHEnd					= 0;
	m_nVStart				= 0;
	m_nVEnd					= 0;
	m_sIniName				= sIniName;
	m_pDCInlayWnd			= NULL;
	m_pDCDiffWnd			= NULL;
	m_pMDConfigDlg			= NULL;
	m_bShowMask				= FALSE;
	m_pFilterFrameBuffer	= NULL;
	
	// Außmaße de Videoinlays für die Bewegungserkennung.
	m_nCaptureSizeH = CAPTURE_SIZE_H;
	m_nCaptureSizeV = CAPTURE_SIZE_V;
	
	// Zweidimensionales Array zur Aufname des Differenzbildes anlegen
	m_nDiffs = new (int* [m_nCaptureSizeH/8]);		// Allocate row pointers
	for (nI = 0; nI < m_nCaptureSizeH/8; nI++)
		m_nDiffs[nI] = new int[m_nCaptureSizeV/8];   // Allocate rows

	_TCHAR szPath[_MAX_PATH];
	CString sHomePath = _T("");
	HINSTANCE hInstance = AfxGetInstanceHandle( );
	if (hInstance)
	{
		GetModuleFileName(hInstance, szPath, sizeof(szPath));
		sHomePath = szPath;
		nI = sHomePath.ReverseFind(_T('\\'));
		if (nI != -1)
			sHomePath = sHomePath.Left(nI+1);
	}

	// Pfad zum Speichern der Masken-dateien einlesen
	ReadConfigurationString(_T("MotionDetection"), _T("MaskPath"), _T("MDMask\\"), szBuffer,  255, m_sIniName); 
	m_sMaskPath = sHomePath+szBuffer;
	if (!m_sMaskPath.IsEmpty())
		_tmkdir(m_sMaskPath);
	
	for (nI = 0; nI < m_pUDP->GetAvailableCameras(); nI++)
	{
		m_Mask[nI].nAdaptiveMask = new (int* [m_nCaptureSizeH/8]);
		m_Mask[nI].nPermanentMask = new (int* [m_nCaptureSizeH/8]);
		for (int nX = 0; nX < m_nCaptureSizeH/8; nX++)
		{
			m_Mask[nI].nAdaptiveMask[nX] = new int[m_nCaptureSizeV/8];   // Allocate rows
			m_Mask[nI].nPermanentMask[nX] = new int[m_nCaptureSizeV/8];   // Allocate rows
		}

		m_wField[nI]			= 0;  
		m_dwTimeStamp[nI]		= 0;
		m_pFrameBuffers[nI][0]	= NULL;
		m_pFrameBuffers[nI][1]	= NULL;
		m_bValidFrame[nI][0]	= FALSE;
		m_bValidFrame[nI][1]	= FALSE;
		m_bDIBIndication[nI]	= FALSE;		
		LoadParameter(nI);
		LoadMask(nI);
	}

	// Absolutwert-Tabelle
	for (nI = -255; nI <= 255; nI++)
		m_nBetrag[nI+255] = abs(nI);

	// Pointer zeigen auf die Mitte der Tabelle, damit auch negative Tabellenindexe
	// erlaubt sind.
	m_pBetrag	= &m_nBetrag[255];

	// Divisionstabelle für das Rechteckfilter.
	for (nI = 0; nI < 9*256; nI++)
		m_byDiv9[nI] = (BYTE)(nI/9);

	// DIB für das Originalbild
	ZeroMemory(&m_bmiInlay, sizeof(BITMAPINFO));
	m_bmiInlay.bmiHeader.biBitCount = 24;
	m_bmiInlay.bmiHeader.biWidth = m_nCaptureSizeH;
	m_bmiInlay.bmiHeader.biHeight= -m_nCaptureSizeV;
	m_bmiInlay.bmiHeader.biPlanes= 1;
	m_bmiInlay.bmiHeader.biCompression = BI_RGB;
	m_bmiInlay.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmiInlay.bmiHeader.biSizeImage = m_nCaptureSizeH*m_nCaptureSizeV*m_bmiInlay.bmiHeader.biBitCount/8;
	m_pDibImageDataInlay = new BYTE[m_bmiInlay.bmiHeader.biSizeImage];

	// DIB für das Differenzbild
	ZeroMemory(&m_bmiDiff, sizeof(BITMAPINFO));
	m_bmiDiff.bmiHeader.biBitCount = 24;
	m_bmiDiff.bmiHeader.biWidth = m_nCaptureSizeH;
	m_bmiDiff.bmiHeader.biHeight= -m_nCaptureSizeV;
	m_bmiDiff.bmiHeader.biPlanes= 1;
	m_bmiDiff.bmiHeader.biCompression = BI_RGB;
	m_bmiDiff.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmiDiff.bmiHeader.biSizeImage = m_nCaptureSizeH*m_nCaptureSizeV*m_bmiInlay.bmiHeader.biBitCount/8;
	m_pDibImageDataDiff = new BYTE[m_bmiDiff.bmiHeader.biSizeImage];
}

//////////////////////////////////////////////////////////////////////
CMotionDetection::~CMotionDetection()
{
	_TCHAR szBuffer[255];

	for (int nI = 0; nI < m_pUDP->GetAvailableCameras(); nI++)
	{
		wsprintf(szBuffer, _T("Mapping\\Source%02u\\MD"), nI);
		WriteConfigurationInt(szBuffer, _T("Histogramm"), m_bHistogramm[nI], m_sIniName);	
		WriteConfigurationInt(szBuffer, _T("TPFilter"), m_bFilter[nI], m_sIniName);	

		SaveMask(nI, AM_ADAPTIVE);
		SaveMask(nI, AM_PERMANENT);
		WK_DELETE(m_pFrameBuffers[nI][0]);
		WK_DELETE(m_pFrameBuffers[nI][1]);

		// Masken-Arrays freigeben
		for (int nX = 0; nX < m_nCaptureSizeH/8; nX++)
		{
			delete []  m_Mask[nI].nAdaptiveMask[nX];          //  Deallocate rows
			delete []  m_Mask[nI].nPermanentMask[nX];         //  Deallocate rows
		}
		delete [] m_Mask[nI].nAdaptiveMask;					//  Deallocate row pointers
		delete [] m_Mask[nI].nPermanentMask;				//  Deallocate row pointers
	}

	WK_DELETE(m_pMDConfigDlg);
	WK_DELETE(m_pDCInlayWnd);
	WK_DELETE(m_pDCDiffWnd);

	WK_DELETE(m_pDibImageDataInlay);
	WK_DELETE(m_pDibImageDataDiff);

	WK_DELETE(m_pFilterFrameBuffer);

	// Differenzbild-Array freigeben
	for (int nX = 0; nX < m_nCaptureSizeH/8; nX++)
		delete []  m_nDiffs[nX];            //  Deallocate rows
	delete [] m_nDiffs;						//  Deallocate row pointers

	m_bOK	= FALSE;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::OpenConfigDlg(HWND hWndParent)
{
	if (!m_pMDConfigDlg)
		return FALSE;

	return m_pMDConfigDlg->ShowConfigDlg(hWndParent);
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::Init()
{
	m_bOK	= TRUE;

	// Dialog Motiondetection anlegen
	if (!m_pMDConfigDlg)
	{
		m_pMDConfigDlg = new CMDConfigDlg(this, m_pUDP);
	}	

	if (!m_pDCInlayWnd)
		m_pDCInlayWnd = new CWindowDC(m_pMDConfigDlg->GetInlayWnd());
	
	if (!m_pDCDiffWnd)
		m_pDCDiffWnd  = new CWindowDC(m_pMDConfigDlg->GetDiffWnd());

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::IsValid() const
{
	return m_bOK;
}

//////////////////////////////////////////////////////////////////////
// Randbedingung: siSrc muß ein geradzahlig vielfaches von siDst sein!
BOOL CMotionDetection::ScaleDownYUV422(QIMAGE* pQImage, Y8* pFrame)
{
	YUV422* pSrc = (YUV422*)pQImage->lpBMI->bmiColors;
	
	if (!pFrame || !pSrc)
		return FALSE;

	int 	nHScale = (int)((1<<16)*((double)pQImage->nWidth / (double)m_nCaptureSizeH));
	double	fVScale	= (double)pQImage->nHeight / (double)m_nCaptureSizeV / 2.0;
	int		nSrcIndex[768];

	// Indextabelle erzeugen
	for (int nX=0; nX<m_nCaptureSizeH; nX++)
		nSrcIndex[nX] = (int)(nX * nHScale / 2) / (1<<16);

	for (int nY=0; nY<m_nCaptureSizeV; nY++)
	{
		YUV422* pSrcBase = pSrc + pQImage->nWidth * (DWORD)((double)nY * fVScale);
		Y8*		pDstBase = pFrame + nY * m_nCaptureSizeH;
		for (int nX=0; nX<m_nCaptureSizeH; nX++)
			pDstBase[nX].bY = pSrcBase[nSrcIndex[nX]].bY1;
	}

	return TRUE;
}
// Alternative Methode: Das aktuelle Frame wird nicht mit dem vorherigen verglichen, sondern
// mit einem Background Image. Dieses Backgroundimage wird aus dem gewichteten aktuellen und
// dem gewichteten aktuellen Backgroundimage gewonnen. Yb'= (Yc*W + Yb*(100-W))/100
// Yb=aktuelles Backgroundimage, Yc=aktuelles Image (Current image), Yb'=neues Backgroundimage
// W=Wichtungsfaktor (1...99)
// #define MD_TEST
//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::MotionCheck(QIMAGE* pQImage)
{
	if (!pQImage->lpBMI->bmiColors)
		return FALSE;

	int nCamera = pQImage->nCamera;

	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::MotionCheck\tWrong Camera (%d)\n"), nCamera+1);
		return FALSE;
	}

	// MotionDetection aktiviert?
	if (!m_bActivateMD[nCamera])
		return FALSE;

	// Alle 250ms neuen Vergleich durchführen
	if (GetTimeSpan(m_dwTimeStamp[nCamera]) < (DWORD)m_nMinTime[nCamera])
		return FALSE;

	// Liegt der letzte Grab schon länger als 30 Sekunden zurück, dann lösche ihn, da zu alt
	if (GetTimeSpan(m_dwTimeStamp[nCamera]) > (DWORD)m_nMaxTime[nCamera])
		m_bValidFrame[nCamera][m_wField[nCamera]] = FALSE;

#ifndef MD_TEST
	// Die beiden Framebuffer immer im Wechsel ansprechen.
	if (m_wField[nCamera] == 0)
		m_wField[nCamera] = 1;
	else if (m_wField[nCamera] == 1)
		m_wField[nCamera] = 0;
#endif

#ifdef MD_TEST
	m_wField[nCamera] = 0;
	// Wurde bereits ein Framebuffer angelegt.
	if (m_pFrameBuffers[nCamera][0] == NULL)
		m_pFrameBuffers[nCamera][0] = new Y8[m_nCaptureSizeH * m_nCaptureSizeV+4];
	// Wurde bereits ein Framebuffer angelegt.
	if (m_pFrameBuffers[nCamera][1] == NULL)
		m_pFrameBuffers[nCamera][1] = new Y8[m_nCaptureSizeH * m_nCaptureSizeV+4];
#else
	// Wurde bereits ein Framebuffer angelegt.
	if (m_pFrameBuffers[nCamera][m_wField[nCamera]] == NULL)
	{
		m_pFrameBuffers[nCamera][0] = new Y8[m_nCaptureSizeH * m_nCaptureSizeV+4];
		m_pFrameBuffers[nCamera][1] = new Y8[m_nCaptureSizeH * m_nCaptureSizeV+4];
	}
#endif
	
	// Das Frame herunterskalieren und YUV422 nach Y8 wandeln (GreyScale)
	if (!ScaleDownYUV422(pQImage, m_pFrameBuffers[nCamera][m_wField[nCamera]]))
		return FALSE;
	
#ifdef MD_TEST
	m_bValidFrame[nCamera][0] = TRUE;
	if (m_bValidFrame[nCamera][1] == TRUE)
	{
		static int nCount = 1;
		Y8* pCurFrame   = m_pFrameBuffers[nCamera][0];
		Y8* pBgrndFrame = m_pFrameBuffers[nCamera][1];
		for (int nY = 0; nY < m_nCaptureSizeV; nY++)
		{
			Y8*		pSrcBase = pCurFrame   + nY * m_nCaptureSizeH;
			Y8*		pDstBase = pBgrndFrame + nY * m_nCaptureSizeH;
			for (int nX=0; nX<m_nCaptureSizeH; nX++)
				pDstBase[nX].bY = (pSrcBase[nX].bY*nCount + ((100-nCount)*pDstBase[nX].bY)+50)/100;
		}
	}
	else
	{
		memcpy(m_pFrameBuffers[nCamera][1], m_pFrameBuffers[nCamera][0], m_nCaptureSizeH*m_nCaptureSizeV*sizeof(Y8));
		m_bValidFrame[nCamera][1] = TRUE;
	}
#else
	m_bValidFrame[nCamera][m_wField[nCamera]] = TRUE;
#endif

	// Zeitpunkt des Grabs vermerken
	m_dwTimeStamp[nCamera] = GetTickCount();

	// Histogrammausgleich durchführen?
	if (m_bHistogramm[nCamera])
		Histogramm(m_pFrameBuffers[nCamera][m_wField[nCamera]]);
	
	// Tiefpassfilterung durchführen?
	if (m_bFilter[nCamera])
		Filter(m_pFrameBuffers[nCamera][m_wField[nCamera]]);

	ShowOriginalFrame(nCamera);

	// Wurden schon 2 Frames gegrapt?
	if (!m_bValidFrame[nCamera][0] || !m_bValidFrame[nCamera][1])
		return FALSE;

	return CompareImages(nCamera, pQImage->pPMDoints);
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::CompareImages(int nCamera, CMDPoints* pPoints)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::CompareImages\tWrong Camera (%d)\n"), nCamera+1);
		return FALSE;
	}

	int	 nX				= 0;
	int	 nY				= 0;
	int	 nX1			= 0;
	int	 nY1			= 0;
	int  nMittelDiff	= 0;
	int  nMask			= 0;

	// Altes Differenzbild löschen
	for (nX1 = 0; nX1 < m_nCaptureSizeH/8; nX1++ )
	{
		for (nY1 = 0; nY1 < m_nCaptureSizeV/8; nY1++)
			m_nDiffs[nX1][nY1] = 0;
	} 

	// Differenzbild ermitteln
	// dy = Betrag(Y2-Y1)
	BYTE* p1 = (BYTE*)&m_pFrameBuffers[nCamera][0][0]; // Pointer auf 1. Frame
	BYTE* p2 = (BYTE*)&m_pFrameBuffers[nCamera][1][0]; // Pointer auf 2. Frame

	nY1 = 0;
	for (nY=0; nY < m_nCaptureSizeV; nY++, nY1 = nY/8)
	{
		nX1 = 0;
		for (nX=0; nX < m_nCaptureSizeH; nX+=8, nX1 = nX/8)
		{
			m_nDiffs[nX1][nY1] += Betrag(*p2++,*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++,*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++,*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++,*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++,*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++,*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++,*p1++);
			m_nDiffs[nX1][nY1] += Betrag(*p2++,*p1++);
		}
	}

	// Mittler Helligkeit eines 8x8-Blockes bestimmen
	for (nY1 = 0; nY1 < m_nCaptureSizeV/8; nY1++)
	{			  
		for (nX1=0; nX1 < m_nCaptureSizeH/8; nX1++)
		{
			nMittelDiff += m_nDiffs[nX1][nY1];		
		}
	}
	nMittelDiff /= ((m_nCaptureSizeH/8) * (m_nCaptureSizeV/8));

	// Den Block mit der Maximaldifferenz ermitteln und Maske auf-/abbauen
	for (nY1 = 0; nY1 < m_nCaptureSizeV/8; nY1++)
	{
		for (nX1=0; nX1 < m_nCaptureSizeH/8; nX1++)
		{
			// Mittlere Helligkeit subtrahieren. (Nur den AC-Anteil bewerten)
			m_nDiffs[nX1][nY1] = max(m_nDiffs[nX1][nY1] - nMittelDiff, 0);

			if (m_bUseAdaptiveMask[nCamera])
			{
				// Adaptive Bewegungsstörmaske aufbauen/abbauen.
				if (m_nDiffs[nX1][nY1] > m_nUpperMaskTreshHold[nCamera])
				{
					m_Mask[nCamera].nAdaptiveMask[nX1][nY1] += m_nMaskInc[nCamera];
					m_Mask[nCamera].nAdaptiveMask[nX1][nY1]  = min(m_Mask[nCamera].nAdaptiveMask[nX1][nY1], UPPER_BOUNDARY_MASK);
				}
				else if (m_nDiffs[nX1][nY1] <= m_nLowerMaskTreshHold[nCamera])
				{
					m_Mask[nCamera].nAdaptiveMask[nX1][nY1] -= m_nMaskDec[nCamera];
					m_Mask[nCamera].nAdaptiveMask[nX1][nY1]  = max(m_Mask[nCamera].nAdaptiveMask[nX1][nY1], LOWER_BOUNDARY_MASK);
				}
			}

			// Differenz mit den Masken wichten
			nMask =  min(m_Mask[nCamera].nPermanentMask[nX1][nY1] + m_Mask[nCamera].nAdaptiveMask[nX1][nY1], UPPER_BOUNDARY_MASK);
			m_nDiffs[nX1][nY1] = (m_nDiffs[nX1][nY1]*(UPPER_BOUNDARY_MASK-nMask))/UPPER_BOUNDARY_MASK;
		
			// Liegt die Differenz oberhalb des Grenzwertes, dann Koordinaten zurückliefern
			if (m_nDiffs[nX1][nY1] > m_nAlarmTreshHold[nCamera])
			{
				// Ort der Bewegung zurückmelden
				CPoint  Point((nX1*8+4)*1000 / m_nCaptureSizeH,
							  (nY1*8+4)*1000 / m_nCaptureSizeV);
				pPoints->Add(Point, m_nDiffs[nX1][nY1]);
			}
		}
	}								   

	// Nach Stärke sortieren
	if (pPoints->GetSize() > 1)
		pPoints->SortByValue();

	if (m_pMDConfigDlg->IsVisible())
	{
		// Differenzbild ausgeben
		ShowDifferenceFrame(nCamera);

		// Den Ort der größten Differenz durch ein Fadenkreuz kennzeichnen
		if (pPoints->GetSize() > 0)
		{
			if (nCamera == m_pMDConfigDlg->GetSelectedCamera())
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
						NewBrush.CreateSolidBrush(RGB(0,0,255));
						m_pDCInlayWnd->SelectObject(NewBrush);

						int nBlkW = (int)(4.0 * ((float)rectInlay.Width()  / (float)m_nCaptureSizeH));
						int nBlkH = (int)(4.0 * ((float)rectInlay.Height() / (float)m_nCaptureSizeV));
						
						for (int nI = 0; nI < min(pPoints->GetSize(), 5); nI++)
						{											   
							int nMaxDiffX 	= pPoints->GetPointAt(nI).x * rectInlay.Width() / 1000;
							int nMaxDiffY	= pPoints->GetPointAt(nI).y * rectInlay.Height() / 1000;
							
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

	return (pPoints->GetSize() > 0);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::LoadParameter(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::LoadParameter\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	_TCHAR szBuffer[255]		={0};
	wsprintf(szBuffer, _T("Mapping\\Source%02u\\MD"), nCamera);
	
	// Histogramm? Ja/Nein.
	m_bHistogramm[nCamera]	= ReadConfigurationInt(szBuffer, _T("Histogramm"), 0, m_sIniName);	

	// Filter? Ja/Nein.
	m_bFilter[nCamera]		= ReadConfigurationInt(szBuffer, _T("TPFilter"), 1, m_sIniName);	

	// Mindest-, und Maximalzeit zwischen zwei Differentbildern
	m_nMinTime[nCamera] = ReadConfigurationInt(szBuffer, _T("MinTime"), 25,   m_sIniName); 
	m_nMaxTime[nCamera] = ReadConfigurationInt(szBuffer, _T("MaxTime"), 30000, m_sIniName); 

	// Die MD-Parameter in Abhängigkeit der Sensitivity-level laden
	SetMDMaskSensitivity(nCamera, GetMDMaskSensitivity(nCamera));
	SetMDAlarmSensitivity(nCamera, GetMDAlarmSensitivity(nCamera));
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::SaveMask(int nCamera, MaskType eMaskType)
{									  
	BOOL bResult = FALSE;

	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::SaveMask\tWrong Camera (%d)\n"), nCamera+1);
		return FALSE;
	}

	CIPCActivityMask Mask(eMaskType);
	GetMask(nCamera, Mask);

	CString sFileName;
	if (eMaskType == AM_ADAPTIVE)
		sFileName.Format(_T("%sMDA%02u.msk"), m_sMaskPath, nCamera);
	else if (eMaskType == AM_PERMANENT)
		sFileName.Format(_T("%sMDP%02u.msk"), m_sMaskPath, nCamera);
	else
		WK_TRACE_ERROR(_T("SaveMask: Unknown Masktype (0x%x)\n"), eMaskType);

	if (Mask.SaveMaskToFile(sFileName))
		bResult = TRUE;

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::LoadMask(int nCamera)
{
	BOOL bResult = FALSE;

	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::LoadMask\tWrong Camera (%d)\n"), nCamera+1);
		return FALSE;
	}

	// Alte Maske löschen
	ClearMask(nCamera);

	CString sFileName;
	sFileName.Format(_T("%sMotionD%u.msk"), m_sMaskPath, nCamera);

	TRY
	{
		// Gibt es das File überhaupt?
		if (DoesFileExist(sFileName))
		{
			{	// Eigener Scope, da ansonsten 	DeleteFile fehlschlägt
				CFile file(sFileName, CFile::modeRead);
				file.Read(&m_Mask[nCamera], (DWORD)file.GetLength());
			}
			// Alte Maske löschen, da in einem neuen Format gespeichert wird.
			DeleteFile(sFileName);
			
			// Und gleich unter dem neuen Format sichern
			SaveMask(nCamera, AM_PERMANENT);
			SaveMask(nCamera, AM_ADAPTIVE);
			bResult = TRUE;
		}
		else
			bResult = LoadMaskX(nCamera);
	}
	CATCH( CFileException, e )
	{
		bResult = FALSE;
	}
	END_CATCH

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::LoadMaskX(int nCamera)
{
	BOOL bResult = FALSE;

	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::LoadMaskX\tWrong Camera (%d)\n"), nCamera+1);
		return FALSE;
	}

	// Alte Maske löschen
	ClearMask(nCamera);

	CIPCActivityMask MaskAdaptive(AM_ADAPTIVE);
	CIPCActivityMask MaskPermanent(AM_PERMANENT);

	CString sFileName;
	sFileName.Format(_T("%sMDA%02u.msk"), m_sMaskPath, nCamera);
	if (MaskAdaptive.LoadMaskFromFile(sFileName))
	{
		if (SetMask(nCamera, MaskAdaptive))
		{
			sFileName.Format(_T("%sMDP%02u.msk"), m_sMaskPath, nCamera);
			if (MaskPermanent.LoadMaskFromFile(sFileName))
			{
				if (SetMask(nCamera, MaskPermanent))
					bResult = TRUE;
				else
					WK_TRACE_ERROR(_T("LoadMaskX: Failed to set permanent mask\n"));
			}
			else
				WK_TRACE_ERROR(_T("LoadMaskX: Failed to load permanent mask\n"));
		}
		else
			WK_TRACE_ERROR(_T("LoadMaskX: Failed to set adaptive mask\n"));
	}
	else
		WK_TRACE_ERROR(_T("LoadMaskX: Failed to load adaptive mask\n"));

	return bResult;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ClearMask(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ClearMask\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	// Alte Masken löschen
	ClearAdaptiveMask(nCamera);
	ClearPermanentMask(nCamera);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ClearAdaptiveMask(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ClearAdaptiveMask\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	// Alte Maske löschen
	for (int nY=0; nY < m_nCaptureSizeV/8; nY++)
	{
		for (int nX=0; nX < m_nCaptureSizeH/8; nX++)
			m_Mask[nCamera].nAdaptiveMask[nX][nY]  = LOWER_BOUNDARY_MASK;
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ClearPermanentMask(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ClearPermanentMask\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	// Alte Maske löschen
	for (int nY=0; nY < m_nCaptureSizeV/8; nY++)
	{
		for (int nX=0; nX < m_nCaptureSizeH/8; nX++)
			m_Mask[nCamera].nPermanentMask[nX][nY] = LOWER_BOUNDARY_MASK;
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::InvertPermanentMask(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::InvertPermanentMask\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	// Permanente Maske invertieren
	for (int nY=0; nY < m_nCaptureSizeV/8; nY++)
	{
		for (int nX=0; nX < m_nCaptureSizeH/8; nX++)
		{
			m_Mask[nCamera].nPermanentMask[nX][nY] = UPPER_BOUNDARY_MASK - m_Mask[nCamera].nPermanentMask[nX][nY];
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::InvertAdaptiveMask(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::InvertAdaptiveMask\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	// Adaptive Maske invertieren
	for (int nY=0; nY < m_nCaptureSizeV/8; nY++)
	{
		for (int nX=0; nX < m_nCaptureSizeH/8; nX++)
		{
			m_Mask[nCamera].nAdaptiveMask[nX][nY]  = UPPER_BOUNDARY_MASK - m_Mask[nCamera].nAdaptiveMask[nX][nY];
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::SetPixelInMask(int nCamera, int nXM, int nYM, BOOL bFlag)
{
	COLORREF col;

	if (nCamera >= m_pUDP->GetAvailableCameras())
		return;

	CRect rectClient;
	m_pMDConfigDlg->GetInlayWnd()->GetClientRect(rectClient);	
	
	nXM = nXM * m_nCaptureSizeH / rectClient.Width();
	nYM = nYM * m_nCaptureSizeV / rectClient.Height();

	nXM = min(max(nXM, 3), m_nCaptureSizeH-4);
	nYM = min(max(nYM, 3), m_nCaptureSizeV-4);

	for (int nY = nYM-3; nY <= nYM+3; nY++)
	{
		for (int nX = nXM-3; nX <= nXM+3; nX++)
		{
			if (bFlag)
				m_Mask[nCamera].nPermanentMask[nX/8][nY/8] = UPPER_BOUNDARY_MASK;
			else
				m_Mask[nCamera].nPermanentMask[nX/8][nY/8] = LOWER_BOUNDARY_MASK;

			// Farbe der Maske im Differenzbild.
			if (m_Mask[nCamera].nPermanentMask[nX/8][nY/8] != 0)
				col = RGB(m_Mask[nCamera].nPermanentMask[nX/8][nY/8]*255/UPPER_BOUNDARY_MASK, 0, 0);
			else if (m_Mask[nCamera].nAdaptiveMask[nX/8][nY/8] != 0)
				col = RGB(0, m_Mask[nCamera].nAdaptiveMask[nX/8][nY/8]*255/UPPER_BOUNDARY_MASK, 0);
			else
				col = RGB(0, 0, 0);				
		}
	}							    
	if (m_bShowMask)					  
		ShowOriginalFrame(nCamera);
	ShowDifferenceFrame(nCamera);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleShowMask()
{
	m_bShowMask = !m_bShowMask;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleAdaptiveMaskActivation(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ToggleAdaptiveMaskActivation\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	if (m_bUseAdaptiveMask[nCamera])
		DeactivateAdaptiveMask(nCamera);
	else
		ActivateAdaptiveMask(nCamera);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ActivateAdaptiveMask(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ActivateAdaptiveMask\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	m_bUseAdaptiveMask[nCamera] = TRUE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateAdaptiveMask(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::DeactivateAdaptiveMask\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	m_bUseAdaptiveMask[nCamera] = FALSE;
	
	ClearAdaptiveMask(nCamera);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleMDActivation(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ToggleMDActivation\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	if (m_bActivateMD[nCamera])
		DeactivateMotionDetection(nCamera);
	else
		ActivateMotionDetection(nCamera);
}	
//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::ActivateMotionDetection(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ActivateMotionDetection\tWrong Camera (%d)\n"), nCamera+1);
		return FALSE;
	}

	m_bActivateMD[nCamera] = TRUE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::DeactivateMotionDetection(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::DeactivateMotionDetection\tWrong Camera (%d)\n"), nCamera+1);
		return FALSE;
	}

	m_bActivateMD[nCamera] = FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleHistogramm(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ToggleHistogramm\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	if (m_bHistogramm[nCamera])
		DeactivateHistogramm(nCamera);
	else
		ActivateHistogramm(nCamera);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ActivateHistogramm(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ActivateHistogramm\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	m_bHistogramm[nCamera] = TRUE;

	// Einstellung sofort in der Registry sichern
	_TCHAR szBuffer[255];
	wsprintf(szBuffer, _T("Mapping\\Source%02u\\MD"), nCamera);
	WriteConfigurationInt(szBuffer, _T("Histogramm"), m_bHistogramm[nCamera], m_sIniName);	
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateHistogramm(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::DeactivateHistogramm\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}
	m_bHistogramm[nCamera] = FALSE;

	// Einstellung sofort in der Registry sichern
	_TCHAR szBuffer[255];
	wsprintf(szBuffer, _T("Mapping\\Source%02u\\MD"), nCamera);
	WriteConfigurationInt(szBuffer, _T("Histogramm"), m_bHistogramm[nCamera], m_sIniName);	
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleFilter(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ToggleFilter\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	if (m_bFilter[nCamera])
		DeactivateFilter(nCamera);
	else
		ActivateFilter(nCamera);
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ActivateFilter(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ActivateFilter\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	m_bFilter[nCamera] = TRUE;

	// Einstellung sofort in der Registry sichern
	_TCHAR szBuffer[255];
	wsprintf(szBuffer, _T("Mapping\\Source%02u\\MD"), nCamera);
	WriteConfigurationInt(szBuffer, _T("TPFilter"), m_bFilter[nCamera], m_sIniName);	
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateFilter(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::DeactivateFilter\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}
	m_bFilter[nCamera] = FALSE;

	// Einstellung sofort in der Registry sichern
	_TCHAR szBuffer[255];
	wsprintf(szBuffer, _T("Mapping\\Source%02u\\MD"), nCamera);
	WriteConfigurationInt(szBuffer, _T("TPFilter"), m_bFilter[nCamera], m_sIniName);	
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
void CMotionDetection::ShowOriginalFrame(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ShowOriginalFrame\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	Y8*			pSrc = m_pFrameBuffers[nCamera][m_wField[nCamera]];
	DWORD		dwIndex = 0;
	COLORREF	col		= 0;
	int			nPMask  = 0;
	int			nAMask	= 0;

	if (pSrc && m_pMDConfigDlg && m_pMDConfigDlg->IsVisible())
	{
		// Nur das Bild der aktuell gewählten Kamera zeigen
		if (m_pMDConfigDlg->GetSelectedCamera() == nCamera)
		{
			// Frame in den Bilschirmspeicher kopieren, damit sichtbar.
			for (int nY = 0; nY < m_nCaptureSizeV; nY++)
			{
				for (int nX = 0; nX < m_nCaptureSizeH; nX++)
				{
					dwIndex = nX+nY*m_nCaptureSizeH;
					col = RGB(pSrc[dwIndex].bY, pSrc[dwIndex].bY, pSrc[dwIndex].bY);
					if (m_bShowMask)
					{		  							 
						// Farbe der Maske im Originalbild
						nAMask = m_Mask[nCamera].nAdaptiveMask[nX/8][nY/8]  * 255 / UPPER_BOUNDARY_MASK;
						nPMask = m_Mask[nCamera].nPermanentMask[nX/8][nY/8] * 255 / UPPER_BOUNDARY_MASK;
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
			CRect rectFrom(0,0, m_nCaptureSizeH, m_nCaptureSizeV);
			m_pMDConfigDlg->GetInlayWnd()->GetClientRect(rectTo);	
			rectTo.OffsetRect(CPoint(3,3));
			m_pDCInlayWnd->SetStretchBltMode(COLORONCOLOR);
			StretchDIBits(m_pDCInlayWnd->m_hDC,		// handle to device context
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
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ShowDifferenceFrame(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::ShowDifferenceFrame\tWrong Camera (%d)\n"), nCamera+1);
		return;
	}

	COLORREF	col;
	int			nDiff = 0;
	int			nPMask  = 0;
	int			nAMask	= 0;

	if (m_pMDConfigDlg && m_pMDConfigDlg->IsVisible())
	{
		// Nur das Bild der aktuell gewählten Kamera zeigen
		if (m_pMDConfigDlg->GetSelectedCamera() == nCamera)
		{
			// Differenzbild ausgeben
			for (int nY=0; nY < m_nCaptureSizeV; nY++)
			{
				for (int nX=0; nX < m_nCaptureSizeH; nX++)
				{
					nDiff = min(m_nDiffs[nX/8][nY/8]/16, 255);

					// Farbe der Maske im Differenzbild
					nAMask = m_Mask[nCamera].nAdaptiveMask[nX/8][nY/8]  * 255 / UPPER_BOUNDARY_MASK;
					nPMask = m_Mask[nCamera].nPermanentMask[nX/8][nY/8] * 255 / UPPER_BOUNDARY_MASK;
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
			CRect rectFrom(0,0, m_nCaptureSizeH, m_nCaptureSizeV);
			m_pMDConfigDlg->GetDiffWnd()->GetClientRect(rectTo);	
			rectTo.OffsetRect(CPoint(3,3));

			m_pDCInlayWnd->SetStretchBltMode(COLORONCOLOR);
			StretchDIBits(m_pDCDiffWnd->m_hDC,		// handle to device context
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
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::SetPixel(int nID, int nX, int nY, COLORREF col)
{
	BYTE*	pDIBAddress = NULL;

	if (nID == IDC_INLAY_WND)
		pDIBAddress = m_pDibImageDataInlay;
	if (nID == IDC_DIFF_WND)
		pDIBAddress = m_pDibImageDataDiff;

	if (pDIBAddress)
	{
		DWORD dwIndex = 3 * (nY * m_nCaptureSizeH + nX);
		pDIBAddress[dwIndex++] = GetBValue(col);
		pDIBAddress[dwIndex++] = GetGValue(col);
		pDIBAddress[dwIndex++] = GetRValue(col);
	}
}

//////////////////////////////////////////////////////////////////////
// Wertebereich der Tabelle nY1, nY2 -255....255
inline int CMotionDetection::Betrag(int nY1, int nY2)
{
	return m_pBetrag[nY1-nY2];
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::SetMDMaskSensitivity(int nCamera, const CString &sLevel)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::SetMDMaskSensitivity\tWrong Camera (%d)\n"), nCamera+1);
		return FALSE;
	}

	// Einstellung sofort in der Registry sichern
	_TCHAR szBuffer[255];
	wsprintf(szBuffer, _T("Mapping\\Source%02u\\MD"), nCamera);
	WriteConfigurationString(szBuffer, _T("MaskSensitivity"), sLevel, m_sIniName); 

	if (sLevel == CSD_HIGH)
	{
		// Den oberen Mask-Treshhold festlegen
		m_nUpperMaskTreshHold[nCamera]= UPPER_BOUNDARY_LUMI*10/1000;
		// Den unteren Mask-Treshhold festlegen
		m_nLowerMaskTreshHold[nCamera]= UPPER_BOUNDARY_LUMI*5/1000;
		// Den Maskenincrement festlegen
		m_nMaskInc[nCamera]	= UPPER_BOUNDARY_MASK*20/1000;
		// Den Maskendecrement festlegen
		m_nMaskDec[nCamera]	= UPPER_BOUNDARY_MASK*5/1000;
		// Adaptive Maske aktivieren
		ActivateAdaptiveMask(nCamera);
	}
	else if (sLevel == CSD_NORMAL)
	{
		// Den oberen Mask-Treshhold festlegen
		m_nUpperMaskTreshHold[nCamera]= UPPER_BOUNDARY_LUMI*20/1000;
		// Den unteren Mask-Treshhold festlegen
		m_nLowerMaskTreshHold[nCamera]= UPPER_BOUNDARY_LUMI*10/1000;
		// Den Maskenincrement festlegen
		m_nMaskInc[nCamera]	= UPPER_BOUNDARY_MASK*20/1000;
		// Den Maskendecrement festlegen
		m_nMaskDec[nCamera]	= UPPER_BOUNDARY_MASK*5/1000;
		// Adaptive Maske aktivieren
		ActivateAdaptiveMask(nCamera);
	}
	else if (sLevel == CSD_LOW)
	{
		// Den oberen Mask-Treshhold festlegen
		m_nUpperMaskTreshHold[nCamera]= UPPER_BOUNDARY_LUMI*40/1000;
		// Den unteren Mask-Treshhold festlegen
		m_nLowerMaskTreshHold[nCamera]= UPPER_BOUNDARY_LUMI*20/1000;
		// Den Maskenincrement festlegen
		m_nMaskInc[nCamera]	= UPPER_BOUNDARY_MASK*20/1000;
		// Den Maskendecrement festlegen
		m_nMaskDec[nCamera]	= UPPER_BOUNDARY_MASK*5/1000;
		// Adaptive Maske aktivieren
		ActivateAdaptiveMask(nCamera);
	}
	else if (sLevel == CSD_OFF)
	{
		// Den oberen Mask-Treshhold festlegen
		m_nUpperMaskTreshHold[nCamera]= UPPER_BOUNDARY_LUMI*20/1000;
		// Den unteren Mask-Treshhold festlegen
		m_nLowerMaskTreshHold[nCamera]= UPPER_BOUNDARY_LUMI*10/1000;
		// Den Maskenincrement festlegen
		m_nMaskInc[nCamera]	= UPPER_BOUNDARY_MASK*20/1000;
		// Den Maskendecrement festlegen
		m_nMaskDec[nCamera]	= UPPER_BOUNDARY_MASK*5/1000;
		// Adaptive Maske deaktivieren
		DeactivateAdaptiveMask(nCamera);
	}
	else
		WK_TRACE_WARNING(_T("Unknown MDMaskSensitivity\n"));

	if (m_pMDConfigDlg)
		m_pMDConfigDlg->EnableDlgCtrl(); 

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::SetMDAlarmSensitivity(int nCamera, const CString &sLevel)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::SetMDAlarmSensitivity\tWrong Camera (%d)\n"), nCamera+1);
		return FALSE;
	}

	// Einstellung sofort in der Registry sichern
	_TCHAR szBuffer[255];
	wsprintf(szBuffer, _T("Mapping\\Source%02u\\MD"), nCamera);
	WriteConfigurationString(szBuffer, _T("AlarmSensitivity"), sLevel, m_sIniName); 

	if (sLevel == CSD_HIGH)
	{
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[nCamera] = UPPER_BOUNDARY_LUMI*50/1000;
		ActivateMotionDetection(nCamera);
	}
	else if (sLevel == CSD_NORMAL)
	{
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[nCamera] = UPPER_BOUNDARY_LUMI*150/1000;
		ActivateMotionDetection(nCamera);
	}
	else if (sLevel == CSD_LOW)
	{						 
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[nCamera] = UPPER_BOUNDARY_LUMI*250/1000;
		ActivateMotionDetection(nCamera);
	}
	else if (sLevel == CSD_OFF)
	{						 
		// Den Alarm-Treshhold festlegen
		m_nAlarmTreshHold[nCamera] = UPPER_BOUNDARY_LUMI*250/1000;
		DeactivateMotionDetection(nCamera);
	}
	else
		WK_TRACE_WARNING(_T("Unknown MDAlarmSensitivity\n"));

	if (m_pMDConfigDlg)
		m_pMDConfigDlg->EnableDlgCtrl();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
CString CMotionDetection::GetMDMaskSensitivity(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::GetMDMaskSensitivity\tWrong Camera (%d)\n"), nCamera+1);
		return CSD_NORMAL;
	}

	_TCHAR szSection[255] = {0};
	_TCHAR szBuffer[255]	= {0};
	wsprintf(szSection, _T("Mapping\\Source%02u\\MD"), nCamera);

	ReadConfigurationString(szSection, _T("MaskSensitivity"), CSD_OFF, szBuffer, 16, m_sIniName);
	return szBuffer;
}

/////////////////////////////////////////////////////////////////////////////
CString CMotionDetection::GetMDAlarmSensitivity(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::GetMDAlarmSensitivity\tWrong Camera (%d)\n"), nCamera+1);
		return CSD_NORMAL;
	}

	_TCHAR szSection[255] = {0};
	_TCHAR szBuffer[255]	= {0};
	wsprintf(szSection, _T("Mapping\\Source%02u\\MD"), nCamera);

	ReadConfigurationString(szSection, _T("AlarmSensitivity"), CSD_NORMAL, szBuffer, 16, m_sIniName);
	return szBuffer;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::GetMask(int nCamera, CIPCActivityMask& mask)
{
	mask.Create(mask.GetType(), m_nCaptureSizeH/8, m_nCaptureSizeV/8);

	for (int nY = 0; nY < m_nCaptureSizeV/8; nY++)
	{
		for (int nX = 0; nX < m_nCaptureSizeH/8; nX++)
		{
			if (mask.GetType() == AM_ADAPTIVE)
				mask.SetAt(nX, nY, (WORD)m_Mask[nCamera].nAdaptiveMask[nX][nY]);
			else if (mask.GetType() == AM_PERMANENT)
				mask.SetAt(nX, nY, (WORD)m_Mask[nCamera].nPermanentMask[nX][nY]);
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::SetMask(int nCamera, const CIPCActivityMask& mask)
{
	if ((mask.GetSize().cx != m_nCaptureSizeH/8) ||	(mask.GetSize().cy != m_nCaptureSizeV/8))
	{
		WK_TRACE(_T("SetMask: Size of the Masks are different...scaling permanent and adaptive mask\n"));

		float fX = 0.0;
		float fY = 0.0;
		float fdX = (float)mask.GetSize().cx / (float)(m_nCaptureSizeH/8);
		float fdY = (float)mask.GetSize().cy / (float)(m_nCaptureSizeV/8);

		for (int nY = 0; nY < m_nCaptureSizeV/8; nY++, fX = 0.0)
		{
			for (int nX = 0; nX < m_nCaptureSizeH/8; nX++)
			{
				if (mask.GetType() == AM_ADAPTIVE)
					m_Mask[nCamera].nAdaptiveMask[nX][nY]  = mask.GetAt((int)fX, (int)fY);
				else if (mask.GetType() == AM_PERMANENT)
					m_Mask[nCamera].nPermanentMask[nX][nY] = mask.GetAt((int)fX,(int) fY);
				fX += fdX;
			}
			fY += fdY;
		}
	}
	else
	{
		for (int nY = 0; nY < m_nCaptureSizeV/8; nY++)
		{
			for (int nX = 0; nX < m_nCaptureSizeH/8; nX++)
			{
				if (mask.GetType() == AM_ADAPTIVE)
					m_Mask[nCamera].nAdaptiveMask[nX][nY]  = mask.GetAt(nX, nY);
				else if (mask.GetType() == AM_PERMANENT)
					m_Mask[nCamera].nPermanentMask[nX][nY] = mask.GetAt(nX, nY);
			}
		}
	}

	// Und gleich sichern
	return SaveMask(nCamera, mask.GetType());
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::EnableDIBIndication(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::EnableDIBIndication\tWrong Camera (%d)\n"), nCamera+1);
		return FALSE;
	}

	m_bDIBIndication[nCamera] = TRUE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::DisableDIBIndication(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::DisableDIBIndication\tWrong Camera (%d)\n"), nCamera+1);
		return FALSE;
	}

	m_bDIBIndication[nCamera] = FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
HANDLE CMotionDetection::CreateDIB(int nCamera)
{
	if (nCamera >= m_pUDP->GetAvailableCameras())
	{
		WK_TRACE_ERROR(_T("CMotionDetection::CreateDIB\tWrong Camera (%d)\n"), nCamera+1);
		return NULL;
	}

	if (m_bValidFrame[nCamera][m_wField[nCamera]] == FALSE)
		return NULL;

	DWORD  dwSizeImage = 2 *  m_nCaptureSizeH*m_nCaptureSizeV;
	HANDLE hDIB = GlobalAlloc(GHND, dwSizeImage + sizeof(BITMAPINFO));
	if (!hDIB)
	{
		WK_TRACE_ERROR(_T("CMotionDetection::CreateDIB GlobalAlloc failed\n")); 
		return NULL;
	}

	LPBITMAPINFO lpBMI				= (LPBITMAPINFO) GlobalLock(hDIB);
	lpBMI->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	lpBMI->bmiHeader.biWidth		= m_nCaptureSizeH;
	lpBMI->bmiHeader.biHeight		= m_nCaptureSizeV;
	lpBMI->bmiHeader.biPlanes		= 1;
	lpBMI->bmiHeader.biBitCount		= 16;
	lpBMI->bmiHeader.biCompression	= mmioFOURCC('Y','4','2','2');
	lpBMI->bmiHeader.biSizeImage	= dwSizeImage;

	m_csMD.Lock();
	memcpy(lpBMI->bmiColors, m_pFrameBuffers[nCamera][m_wField[nCamera]], dwSizeImage);
//	CMemCopy cpy;
//	cpy.FastCopy(lpBMI->bmiColors, m_pFrameBuffers[nCamera][m_wField[nCamera]], dwSizeImage);
	m_csMD.Unlock();

	GlobalUnlock(hDIB);

	return hDIB;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::Histogramm(Y8* pSrc)
{
	long	lHistY[256] = {0};
	long	lSummeY = 0;
	int		nI = 0, nX=0, nY=0;
	DWORD	dwIndex = 0;

	if (!pSrc)
		return FALSE;

	// Häufigkeitsverteilung ermitteln
	for (nY = 0; nY < m_nCaptureSizeV; nY++)
	{
		for (nX = 0; nX < m_nCaptureSizeH; nX++)
		{
			dwIndex = nX+nY*m_nCaptureSizeH;
			lHistY[pSrc[dwIndex].bY]++;			
		}
	}
	
	// Verteilungsfunktion ermitteln
	for (nI = 0; nI < 256; nI++)
	{
		lSummeY+= lHistY[nI];
		lHistY[nI] = lSummeY;
	}

	// Verteilungsfunktion normieren
	for (nI = 0; nI < 256; nI++)
	{
		lHistY[nI] = lHistY[nI] * 255 / lSummeY;
	}

	for (nY = 0; nY < m_nCaptureSizeV; nY++)
	{
		for (nX = 0; nX < m_nCaptureSizeH; nX++)
		{
			dwIndex = nX+nY*m_nCaptureSizeH;
			pSrc[dwIndex].bY = (BYTE)lHistY[pSrc[dwIndex].bY];
		}
	}

	return TRUE;
}

// Rechteckfilter mit folgender Matrix: 
//					  | 1 | 1 | 1 |
//	P'	=	P x 1/9 * | 1 | 1 | 1 |
//				      | 1 | 1 | 1 |
// Die Randelemente werden nicht berücksichtigt.
// Die Filterung benötigt etwa 4,4ms auf einem PII 300 (ca. 46clk/pixel)
/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::Filter(Y8* pSrc)
{
	Y8*	p1 = NULL;
	Y8*	p2 = NULL;
	Y8*	p3 = NULL;
	Y8*	p4 = NULL;
	Y8*	p5 = NULL;
	Y8*	p6 = NULL;
	Y8*	p7 = NULL;
	Y8*	p8 = NULL;
	Y8*	p9 = NULL;
	
	int		nY		= 0;	
	DWORD	dwI		= 0;
	DWORD	dwIndex = 0;

	if (!pSrc)
		return FALSE;

	// Frame-Hilfsbuffer
	if (m_pFilterFrameBuffer == NULL)
		m_pFilterFrameBuffer = new Y8[m_nCaptureSizeH*m_nCaptureSizeV+16];

	FastZeroMem(m_pFilterFrameBuffer, m_nCaptureSizeH*m_nCaptureSizeV*sizeof(Y8));

	// Tiefpassfilterung durchführen
	for (nY = 1; nY < m_nCaptureSizeV-1; nY++)
	{
		dwIndex = nY*m_nCaptureSizeH;
		for (dwI = dwIndex+1; dwI < dwIndex+m_nCaptureSizeH-1; dwI++)
		{
			p1 = &pSrc[dwI-1-m_nCaptureSizeH];
			p2 = &pSrc[dwI-0-m_nCaptureSizeH];
			p3 = &pSrc[dwI+1-m_nCaptureSizeH];
			p4 = &pSrc[dwI-1];
			p5 = &pSrc[dwI];
			p6 = &pSrc[dwI+1];
			p7 = &pSrc[dwI-1+m_nCaptureSizeH];
			p8 = &pSrc[dwI-0+m_nCaptureSizeH];
			p9 = &pSrc[dwI+1+m_nCaptureSizeH];

			m_pFilterFrameBuffer[dwI].bY = m_byDiv9[(p1->bY+p2->bY+p3->bY+p4->bY+p5->bY+p6->bY+p7->bY+p8->bY+p9->bY)];
		}
	}

	// Und zurück in Quellframe
	memcpy(pSrc, m_pFilterFrameBuffer, m_nCaptureSizeH*m_nCaptureSizeV*sizeof(Y8));
//	CMemCopy cpy;
//	cpy.FastCopy(pSrc, m_pFilterFrameBuffer, m_nCaptureSizeH*m_nCaptureSizeV*sizeof(Y8));

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
// Die Länge sollte 64Bit-alligned sein
void CMotionDetection::FastZeroMem(void* pDestination, DWORD dwLength)
{
	DWORD	*pDest		= (DWORD*)pDestination;
	DWORD	dwLen		= (dwLength + 8) >> 3; // Es werden 64Bit auf einmal gelöscht.

	if (!pDest || !dwLen)
		return;

	_asm{
			PXOR	MM0, MM0
			MOV		EDI, pDest
			MOV		ECX, dwLen
		TOP:
			MOVQ	[EDI],MM0	
			ADD		EDI,8	
			DEC		ECX			// Ende erreicht?
			JNZ		TOP			//    "
			EMMS
	}
}

