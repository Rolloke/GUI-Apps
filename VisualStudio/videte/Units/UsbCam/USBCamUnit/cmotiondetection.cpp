/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: cmotiondetection.cpp $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/USBCamUnit/cmotiondetection.cpp $
// CHECKIN:		$Date: 17.11.03 14:01 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 28.04.03 12:51 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <direct.h>
#include "CMotionDetection.h"
#include "CMDConfigDlg.h"
#include "Memcopy.h"
#include "USBcamProfile.h"
#include "CMDPoints.h"

//#include "MemCopy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define ML_TRACE_WARNING WK_TRACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CMotionDetection::CMotionDetection(const CString& sIniName)
{
	int nI;
	char szBuffer[255]		={0};

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
	m_nCaptureSizeH = ReadConfigurationInt("MotionDetection", "CaptureSizeH", CAPTURE_SIZE_H, m_sIniName);
	m_nCaptureSizeV = ReadConfigurationInt("MotionDetection", "CaptureSizeV", CAPTURE_SIZE_V, m_sIniName);
	
	// Zweidimensionales Array zur Aufname des Differenzbildes anlegen
	m_nDiffs = new (int* [m_nCaptureSizeH/8]);		// Allocate row pointers
	for (nI = 0; nI < m_nCaptureSizeH/8; nI++)
		m_nDiffs[nI] = new int[m_nCaptureSizeV/8];   // Allocate rows

	_TCHAR szPath[_MAX_PATH];
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
		m_byDiv9[nI] = nI/9;

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

//	m_pPerf1 = new CWK_PerfMon("Scale down");
//	m_pPerf2 = new CWK_PerfMon("Compare");
}

//////////////////////////////////////////////////////////////////////
CMotionDetection::~CMotionDetection()
{
	_TCHAR szBuffer[255];

	for (int nI = 0; nI < MAX_CAMERAS; nI++)
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

//	WK_DELETE(m_pPerf1);
//	WK_DELETE(m_pPerf2);

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
BOOL CMotionDetection::OpenConfigDlg(WORD wSubID)
{
	if (!m_pMDConfigDlg)
		return FALSE;

	return m_pMDConfigDlg->ShowConfigDlg(wSubID);
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::Init()
{
	m_bOK	= TRUE;

	// Dialog Motiondetection anlegen
	if (!m_pMDConfigDlg)
	{
		m_pMDConfigDlg = new CMDConfigDlg(this);
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
BOOL CMotionDetection::ScaleDownYUV422(CAPTURE_STRUCT& Capture, Y8* pFrame)
{
//	if (m_pPerf1) m_pPerf1->Start();

	YUV422* pSrc = (YUV422*)Capture.Buffer.pLinAddr;
	
	if (!pFrame || !pSrc)
		return FALSE;

	int 	nHScale = (int)((1<<16)*((float)Capture.wSizeH / (float)m_nCaptureSizeH));
	int		nVScale	= Capture.wSizeH * Capture.wSizeV / m_nCaptureSizeV;
	int		nSrcIndex[768];

	// Indextabelle erzeugen
	for (int nX=0; nX<m_nCaptureSizeH; nX++)
		nSrcIndex[nX] = (int)(nX * nHScale / 2) / (1<<16);

	for (int nY=0; nY<m_nCaptureSizeV; nY++)
	{
		YUV422* pSrcBase = pSrc + nY * nVScale / 2;
		Y8*		pDstBase = pFrame + nY * m_nCaptureSizeH;
		for (int nX=0; nX<m_nCaptureSizeH; nX++)
			pDstBase[nX].bY = pSrcBase[nSrcIndex[nX]].bY1;
	}

//	if (m_pPerf1) m_pPerf1->Stop();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::MotionCheck(CAPTURE_STRUCT& Capture, CMDPoints &Points)
{
	if (!Capture.Buffer.pLinAddr)
		return FALSE;

	WORD wSubID = Capture.wSource;

	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::MotionCheck\tWrong Camera (%d)\n", wSubID+1);
		return FALSE;
	}

	// MotionDetection aktiviert?
	if (!m_bActivateMD[wSubID])
		return FALSE;

	// Alle 250ms neuen Vergleich durchführen
	if (GetTimeSpan(m_dwTimeStamp[wSubID]) < (DWORD)m_nMinTime[wSubID])
		return REQUEST_TOO_EARLY;

	// Liegt der letzte Grab schon länger als 30 Sekunden zurück, dann lösche ihn, da zu alt
	if (GetTimeSpan(m_dwTimeStamp[wSubID]) > (DWORD)m_nMaxTime[wSubID])
		m_bValidFrame[wSubID][m_wField[wSubID]] = PICTURE_TOO_OLD;

	// Die beiden Framebuffer immer im Wechsel ansprechen.
	if (m_wField[wSubID] == 0)
		m_wField[wSubID] = 1;
	else if (m_wField[wSubID] == 1)
		m_wField[wSubID] = 0;

	// Wurde bereits ein Framebuffer angelegt.
	if (m_pFrameBuffers[wSubID][m_wField[wSubID]] == NULL)
	{
		m_pFrameBuffers[wSubID][0] = new Y8[m_nCaptureSizeH * m_nCaptureSizeV+4];
		m_pFrameBuffers[wSubID][1] = new Y8[m_nCaptureSizeH * m_nCaptureSizeV+4];
	}

	// Das Frame herunterskalieren und YUV422 nach Y8 wandeln (GreyScale)
	if (!ScaleDownYUV422(Capture, m_pFrameBuffers[wSubID][m_wField[wSubID]]))
		return SCALE_DOWN_ERROR;

	m_bValidFrame[wSubID][m_wField[wSubID]] = TRUE;

	// Zeitpunkt des Grabs vermerken
	m_dwTimeStamp[wSubID] = GetTickCount();

	// Soll die DIB verschickt werden?
	if (m_bDIBIndication[wSubID])
	{
		// DIB anlegen...
		HANDLE hDIB = CreateDIB(wSubID);
		if (hDIB)
		{
			// ... und ab damit
			GlobalFree(hDIB);
		}
	}
	
	// Histogrammausgleich durchführen?
	if (m_bHistogramm[wSubID])
		Histogramm(m_pFrameBuffers[wSubID][m_wField[wSubID]]);
	
	// Tiefpassfilterung durchführen?
	if (m_bFilter[wSubID])
		Filter(m_pFrameBuffers[wSubID][m_wField[wSubID]]);

	ShowOriginalFrame(wSubID);

	// Wurden schon 2 Frames gegrapt?
	if (!m_bValidFrame[wSubID][0] || !m_bValidFrame[wSubID][1])
		return FALSE;

	BOOL bRet = CompareImages(wSubID, Points);

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::CompareImages(WORD wSubID, CMDPoints &Points)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::CompareImages\tWrong Camera (%d)\n", wSubID+1);
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
	BYTE* p1 = (BYTE*)&m_pFrameBuffers[wSubID][0][0]; // Pointer auf 1. Frame
	BYTE* p2 = (BYTE*)&m_pFrameBuffers[wSubID][1][0]; // Pointer auf 2. Frame

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
		
			// Liegt die Differenz oberhalb des Grenzwertes, dann Koordinaten zurückliefern
			if (m_nDiffs[nX1][nY1] > m_nAlarmTreshHold[wSubID])
			{
				// Ort der Bewegung zurückmelden
				CPoint  Point((nX1*8+4)*1000 / m_nCaptureSizeH,
							  (nY1*8+4)*1000 / m_nCaptureSizeV);
				Points.Add(Point, m_nDiffs[nX1][nY1]);
			}
		}
	}								   

	// Nach Stärke sortieren
	if (Points.GetSize() > 1)
		Points.SortByValue();

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

						int nBlkW = 4.0 * ((float)rectInlay.Width()  / (float)m_nCaptureSizeH);
						int nBlkH = 4.0 * ((float)rectInlay.Height() / (float)m_nCaptureSizeV);
						
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

	return (Points.GetSize() > 0) ? MD_POINTS_RECEIVED: FALSE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::LoadParameter(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::LoadParameter\tWrong Camera (%d)\n", wSubID+1);
		return;
	}

	CHAR szBuffer[255]		={0};
	wsprintf(szBuffer, _T("Mapping\\Source%02u\\MD"), wSubID);
	
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
BOOL CMotionDetection::SaveMask(WORD wSubID, MaskType eMaskType)
{									  
	BOOL bResult = FALSE;

	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::SaveMask\tWrong Camera (%d)\n", wSubID+1);
		return FALSE;
	}

	CIPCActivityMask Mask(eMaskType);
	GetMask(wSubID, Mask);

	CString sFileName;
	if (eMaskType == AM_ADAPTIVE)
		sFileName.Format("%sMDA%02u.msk", m_sMaskPath, wSubID);
	else if (eMaskType == AM_PERMANENT)
		sFileName.Format("%sMDP%02u.msk", m_sMaskPath, wSubID);
	else
		WK_TRACE_ERROR("SaveMask: Unknown Masktype (0x%x)\n", eMaskType);

	if (Mask.SaveMaskToFile(sFileName))
		bResult = TRUE;

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::LoadMask(WORD wSubID)
{
	BOOL bResult = FALSE;

	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::LoadMask\tWrong Camera (%d)\n", wSubID+1);
		return FALSE;
	}

	// Alte Maske löschen
	ClearMask(wSubID);

	CString sFileName;
	sFileName.Format("%sMotionD%u.msk", m_sMaskPath, wSubID);

	TRY
	{
		// Gibt es das File überhaupt?
		if (DoesFileExist(sFileName))
		{
			{	// Eigener Scope, da ansonsten 	DeleteFile fehlschlägt
				CFile file(sFileName, CFile::modeRead);
				file.Read(&m_Mask[wSubID], file.GetLength());
			}
			// Alte Maske löschen, da in einem neuen Format gespeichert wird.
			DeleteFile(sFileName);
			
			// Und gleich unter dem neuen Format sichern
			SaveMask(wSubID, AM_PERMANENT);
			SaveMask(wSubID, AM_ADAPTIVE);
			bResult = TRUE;
		}
		else
			bResult = LoadMaskX(wSubID);
	}
	CATCH( CFileException, e )
	{
		bResult = FALSE;
	}
	END_CATCH

	return bResult;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::LoadMaskX(WORD wSubID)
{
	BOOL bResult = FALSE;

	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::LoadMaskX\tWrong Camera (%d)\n", wSubID+1);
		return FALSE;
	}

	// Alte Maske löschen
	ClearMask(wSubID);

	CIPCActivityMask MaskAdaptive(AM_ADAPTIVE);
	CIPCActivityMask MaskPermanent(AM_PERMANENT);

	CString sFileName;
	sFileName.Format("%sMDA%02u.msk", m_sMaskPath, wSubID);
	if (MaskAdaptive.LoadMaskFromFile(sFileName))
	{
		if (SetMask(wSubID, MaskAdaptive))
		{
			sFileName.Format("%sMDP%02u.msk", m_sMaskPath, wSubID);
			if (MaskPermanent.LoadMaskFromFile(sFileName))
			{
				if (SetMask(wSubID, MaskPermanent))
					bResult = TRUE;
				else
					WK_TRACE_ERROR("LoadMaskX: Failed to set permanent mask\n");
			}
			else
				WK_TRACE_ERROR("LoadMaskX: Failed to load permanent mask\n");
		}
		else
			WK_TRACE_ERROR("LoadMaskX: Failed to set adaptive mask\n");
	}
	else
		WK_TRACE_ERROR("LoadMaskX: Failed to load adaptive mask\n");

	return bResult;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ClearMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::ClearMask\tWrong Camera (%d)\n", wSubID+1);
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
		WK_TRACE_ERROR("CMotionDetection::ClearAdaptiveMask\tWrong Camera (%d)\n", wSubID+1);
		return;
	}

	// Alte Maske löschen
	for (int nY=0; nY < m_nCaptureSizeV/8; nY++)
	{
		for (int nX=0; nX < m_nCaptureSizeH/8; nX++)
			m_Mask[wSubID].nAdaptiveMask[nX][nY]  = LOWER_BOUNDARY_MASK;
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ClearPermanentMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::ClearPermanentMask\tWrong Camera (%d)\n", wSubID+1);
		return;
	}

	// Alte Maske löschen
	for (int nY=0; nY < m_nCaptureSizeV/8; nY++)
	{
		for (int nX=0; nX < m_nCaptureSizeH/8; nX++)
			m_Mask[wSubID].nPermanentMask[nX][nY] = LOWER_BOUNDARY_MASK;
	}
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::InvertPermanentMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::InvertPermanentMask\tWrong Camera (%d)\n", wSubID+1);
		return;
	}

	// Permanente Maske invertieren
	for (int nY=0; nY < m_nCaptureSizeV/8; nY++)
	{
		for (int nX=0; nX < m_nCaptureSizeH/8; nX++)
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
		WK_TRACE_ERROR("CMotionDetection::InvertAdaptiveMask\tWrong Camera (%d)\n", wSubID+1);
		return;
	}

	// Adaptive Maske invertieren
	for (int nY=0; nY < m_nCaptureSizeV/8; nY++)
	{
		for (int nX=0; nX < m_nCaptureSizeH/8; nX++)
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
		WK_TRACE_ERROR("CMotionDetection::ToggleAdaptiveMaskActivation\tWrong Camera (%d)\n", wSubID+1);
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
		WK_TRACE_ERROR("CMotionDetection::ActivateAdaptiveMask\tWrong Camera (%d)\n", wSubID+1);
		return;
	}

	m_bUseAdaptiveMask[wSubID] = TRUE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::DeactivateAdaptiveMask(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::DeactivateAdaptiveMask\tWrong Camera (%d)\n", wSubID+1);
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
		WK_TRACE_ERROR("CMotionDetection::ToggleMDActivation\tWrong Camera (%d)\n", wSubID+1);
		return;
	}

	if (m_bActivateMD[wSubID])
		DeactivateMotionDetection(wSubID);
	else
		ActivateMotionDetection(wSubID);
}	
//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::ActivateMotionDetection(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::ActivateMotionDetection\tWrong Camera (%d)\n", wSubID+1);
		return FALSE;
	}

	m_bActivateMD[wSubID] = TRUE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::DeactivateMotionDetection(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::DeactivateMotionDetection\tWrong Camera (%d)\n", wSubID+1);
		return FALSE;
	}

	m_bActivateMD[wSubID] = FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ToggleHistogramm(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::ToggleHistogramm\tWrong Camera (%d)\n", wSubID+1);
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
		WK_TRACE_ERROR("CMotionDetection::ActivateHistogramm\tWrong Camera (%d)\n", wSubID+1);
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
		WK_TRACE_ERROR("CMotionDetection::DeactivateHistogramm\tWrong Camera (%d)\n", wSubID+1);
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
		WK_TRACE_ERROR("CMotionDetection::ToggleFilter\tWrong Camera (%d)\n", wSubID+1);
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
		WK_TRACE_ERROR("CMotionDetection::ActivateFilter\tWrong Camera (%d)\n", wSubID+1);
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
		WK_TRACE_ERROR("CMotionDetection::DeactivateFilter\tWrong Camera (%d)\n", wSubID+1);
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
		WK_TRACE_ERROR("CMotionDetection::ShowOriginalFrame\tWrong Camera (%d)\n", wSubID+1);
		return;
	}

	Y8*			pSrc = m_pFrameBuffers[wSubID][m_wField[wSubID]];
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
			for (int nY = 0; nY < m_nCaptureSizeV; nY++)
			{
				for (int nX = 0; nX < m_nCaptureSizeH; nX++)
				{
					dwIndex = nX+nY*m_nCaptureSizeH;
					col = RGB(pSrc[dwIndex].bY, pSrc[dwIndex].bY, pSrc[dwIndex].bY);
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
			CRect rectFrom(0,0, m_nCaptureSizeH, m_nCaptureSizeV);
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
}

//////////////////////////////////////////////////////////////////////
void CMotionDetection::ShowDifferenceFrame(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::ShowDifferenceFrame\tWrong Camera (%d)\n", wSubID+1);
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
			for (int nY=0; nY < m_nCaptureSizeV; nY++)
			{
				for (int nX=0; nX < m_nCaptureSizeH; nX++)
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
			CRect rectFrom(0,0, m_nCaptureSizeH, m_nCaptureSizeV);
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
BOOL CMotionDetection::SetMDMaskSensitivity(WORD wSubID, const CString &sLevel)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::SetMDMaskSensitivity\tWrong Camera (%d)\n", wSubID+1);
		return FALSE;
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
		ML_TRACE_WARNING("Unknown MDMaskSensitivity\n");

	if (m_pMDConfigDlg)
		m_pMDConfigDlg->EnableDlgCtrl(); 

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::SetMDAlarmSensitivity(WORD wSubID, const CString &sLevel)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::SetMDAlarmSensitivity\tWrong Camera (%d)\n", wSubID+1);
		return FALSE;
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
		ML_TRACE_WARNING("Unknown MDAlarmSensitivity\n");

	if (m_pMDConfigDlg)
		m_pMDConfigDlg->EnableDlgCtrl();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
CString CMotionDetection::GetMDMaskSensitivity(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::GetMDMaskSensitivity\tWrong Camera (%d)\n", wSubID+1);
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
		WK_TRACE_ERROR("CMotionDetection::GetMDAlarmSensitivity\tWrong Camera (%d)\n", wSubID+1);
		return CSD_NORMAL;
	}

	char szSection[255] = {0};
	char szBuffer[255]	= {0};
	wsprintf(szSection, "Mapping\\Source%02u\\MD", wSubID);

	ReadConfigurationString(szSection, "AlarmSensitivity", CSD_NORMAL, szBuffer, 16, m_sIniName);
	return szBuffer;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::GetMask(WORD wSubID, CIPCActivityMask& mask)
{
	mask.Create(mask.GetType(), m_nCaptureSizeH/8, m_nCaptureSizeV/8);

	for (int nY = 0; nY < m_nCaptureSizeV/8; nY++)
	{
		for (int nX = 0; nX < m_nCaptureSizeH/8; nX++)
		{
			if (mask.GetType() == AM_ADAPTIVE)
				mask.SetAt(nX, nY, m_Mask[wSubID].nAdaptiveMask[nX][nY]);
			else if (mask.GetType() == AM_PERMANENT)
				mask.SetAt(nX, nY, m_Mask[wSubID].nPermanentMask[nX][nY]);
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::SetMask(WORD wSubID, const CIPCActivityMask& mask)
{
	if ((mask.GetSize().cx != m_nCaptureSizeH/8) ||	(mask.GetSize().cy != m_nCaptureSizeV/8))
	{
		WK_TRACE("SetMask: Size of the Masks are different...scaling permanent and adaptive mask\n");

		float fX = 0.0;
		float fY = 0.0;
		float fdX = (float)mask.GetSize().cx / (float)(m_nCaptureSizeH/8);
		float fdY = (float)mask.GetSize().cy / (float)(m_nCaptureSizeV/8);

		for (int nY = 0; nY < m_nCaptureSizeV/8; nY++, fX = 0.0)
		{
			for (int nX = 0; nX < m_nCaptureSizeH/8; nX++)
			{
				if (mask.GetType() == AM_ADAPTIVE)
					m_Mask[wSubID].nAdaptiveMask[nX][nY]  = mask.GetAt(fX, fY);
				else if (mask.GetType() == AM_PERMANENT)
					m_Mask[wSubID].nPermanentMask[nX][nY] = mask.GetAt(fX, fY);
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
					m_Mask[wSubID].nAdaptiveMask[nX][nY]  = mask.GetAt(nX, nY);
				else if (mask.GetType() == AM_PERMANENT)
					m_Mask[wSubID].nPermanentMask[nX][nY] = mask.GetAt(nX, nY);
			}
		}
	}

	// Und gleich sichern
	SaveMask(wSubID, mask.GetType());

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::EnableDIBIndication(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::EnableDIBIndication\tWrong Camera (%d)\n", wSubID+1);
		return FALSE;
	}

	m_bDIBIndication[wSubID] = TRUE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::DisableDIBIndication(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::DisableDIBIndication\tWrong Camera (%d)\n", wSubID+1);
		return FALSE;
	}

	m_bDIBIndication[wSubID] = FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
HANDLE CMotionDetection::CreateDIB(WORD wSubID)
{
	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR("CMotionDetection::CreateDIB\tWrong Camera (%d)\n", wSubID+1);
		return NULL;
	}

	if (m_bValidFrame[wSubID][m_wField[wSubID]] == FALSE)
		return NULL;

	DWORD  dwSizeImage = 2 *  m_nCaptureSizeH*m_nCaptureSizeV;
	HANDLE hDIB = GlobalAlloc(GHND, dwSizeImage + sizeof(BITMAPINFO));
	if (!hDIB)
	{
		WK_TRACE_ERROR("CMotionDetection::CreateDIB GlobalAlloc failed\n"); 
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
//	memcpy(lpBMI->bmiColors, m_pFrameBuffers[wSubID][m_wField[wSubID]], dwSizeImage);
	CMemCopy cpy;
	cpy.FastCopy(lpBMI->bmiColors, m_pFrameBuffers[wSubID][m_wField[wSubID]], dwSizeImage);
	m_csMD.Unlock();

	GlobalUnlock(hDIB);

	return hDIB;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMotionDetection::Histogramm(Y8* pSrc)
{
	long	lHistY[256] = {0};
	long	nLum=0, nR=0, nG=0, nB=0;
	long	lSummeY = 0;
	long	nMax = 0;
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
			pSrc[dwIndex].bY = lHistY[pSrc[dwIndex].bY];
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
//	memcpy(pSrc, m_pFilterFrameBuffer, m_nCaptureSizeH*m_nCaptureSizeV*sizeof(Y8));
	CMemCopy cpy;
	cpy.FastCopy(pSrc, m_pFilterFrameBuffer, m_nCaptureSizeH*m_nCaptureSizeV*sizeof(Y8));

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
