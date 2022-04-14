/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: VideoInlayWindow.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/VideoInlayWindow.cpp $
// CHECKIN:		$Date: 3.02.05 16:45 $
// VERSION:		$Revision: 50 $
// LAST CHANGE:	$Modtime: 3.02.05 16:36 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VideoInlayWindow.h"
#include "picturedef.h"
#include "TashaUnitDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CVideoInlayWindow

BEGIN_MESSAGE_MAP(CVideoInlayWindow, CFrameWnd)
	//{{AFX_MSG_MAP(CVideoInlayWindow)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define ID_DECODE_VIDEO 0

//IMPLEMENT_DYNCREATE(CVideoInlayWindow, CFrameWnd)

/////////////////////////////////////////////////////////////////////////////
CVideoInlayWindow::CVideoInlayWindow(CCodec *pCodec, CRect& rc, BOOL bPrewiew)
{
	m_lpBMI				= NULL;
	m_pCodec			= pCodec;
	m_bMultyView		= FALSE;
	m_wSource			= 0;
	m_pMainWnd			= (CTashaUnitDlg*)AfxGetMainWnd();

	memset(&m_PaintMask, 255, sizeof(m_PaintMask));

   	m_PaintMask.nDimH	= MD_RESOLUTION_H/8;
	m_PaintMask.nDimV	= MD_RESOLUTION_V/8;
	m_PaintMask.bValid	= TRUE;

	m_nLastPaintPosX	= -1;
	m_nLastPaintPosY	= -1;

//	CRect rc(50,50,50+MAX_HSIZE/2,50+MAX_VSIZE/2);
	CString sTitle(_T("TashaUnit"));

	if (!Create(NULL, sTitle,	WS_OVERLAPPED | WS_THICKFRAME |
												WS_MAXIMIZEBOX|	WS_SYSMENU,
												rc, NULL, NULL))
	{	
		WK_TRACE(_T("VideoInlayWindow Create Window failed\n"));
		return;
	}

	m_lpBMI = (LPBITMAPINFO)new BYTE[MAX_HSIZE*MAX_VSIZE*sizeof(WORD)+sizeof(BITMAPINFO)+1024];

	for (int nI = 0; nI < CHANNEL_COUNT; nI++)
	{
		m_pMPEG4[nI] = new CMPEG4Decoder();
		m_pMPEG4[nI]->Init(NULL,0,0,0);
		m_dwFrameCounter[nI]	= 0;
		m_dwDataLen[nI]			= 0;
		m_dwStartTime[nI]		= GetTickCount();
	}

	// Hintergrundfarbe setzen
	COLORREF ColRef = RGB(0,0,0);
	DeleteObject((HGDIOBJ)SetClassLong(m_hWnd, GCL_HBRBACKGROUND, (DWORD)CreateSolidBrush(ColRef)));

	ShowWindow(SW_NORMAL);

}

/////////////////////////////////////////////////////////////////////////////
CVideoInlayWindow::~CVideoInlayWindow()
{
	for (int nI = 0; nI < CHANNEL_COUNT; nI++)
		WK_DELETE(m_pMPEG4[nI]);

	DrawDibClose(m_hDrawDib);
	WK_DELETE(m_lpBMI);
}

////////////////////////////////////////////////////////////////////////////
BOOL CVideoInlayWindow::DrawImage(DATA_PACKET *pPacket, BOOL bDrawAdaptiveMask/*=FALSE*/, BOOL bDrawPermanentMask/*= FALSE*/, BOOL bDrawDiffImage/*=FALSE*/)
{
	BOOL bResult = FALSE;
	if (pPacket)
	{
		if (pPacket->eType == eTypeYUV422)
			bResult = DrawYUV422Image(pPacket, bDrawAdaptiveMask, bDrawPermanentMask, bDrawDiffImage);
		else if (pPacket->eType == eTypeJpeg)
			bResult = DrawJpegImage(pPacket, bDrawAdaptiveMask, bDrawPermanentMask, bDrawDiffImage);
		else if(pPacket->eType == eTypeMpeg4)
			bResult = DrawMpeg4Image(pPacket, bDrawAdaptiveMask, bDrawPermanentMask, bDrawDiffImage);
		else
			WK_TRACE_WARNING(_T("CVideoInlayWindow::DrawImage Unknown Imagetype (%d)\n"), pPacket->eType);
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////
BOOL CVideoInlayWindow::DrawYUV422Image(DATA_PACKET *pPacket, BOOL bDrawAdaptiveMask/*=FALSE*/, BOOL bDrawPermanentMask/*= FALSE*/, BOOL bDrawDiffImage/*=FALSE*/)
{
	BOOL bResult = FALSE;
	int nHSize	 = 0;
	int nVSize   = 0;

	LPBITMAPINFO lpBMI = CreateDIB(pPacket);
	if (lpBMI)
		bResult = m_Jpeg.SetDIBData(lpBMI);

	if (bResult)
	{	
		CRect rcWnd(0,0,0,0);
		GetClientRect(rcWnd);
		CDC* pDC = GetDC();
		if (pDC)
		{
			if (m_bMultyView)
			{	
				nHSize = rcWnd.Width() / 3;
				nVSize = rcWnd.Height() / 3;
				rcWnd = CRect(0, 0, nHSize, nVSize);
			}

			// Offscreensurface anlegen
			CDC memDC;
			memDC.CreateCompatibleDC(pDC);

			// Kompatible Bitmap mit dem Offscreensurface verbinden
			CBitmap Bitmap;
			Bitmap.CreateCompatibleBitmap(pDC, rcWnd.Width(), rcWnd.Height());
			CBitmap* pOldBitmap = memDC.SelectObject(&Bitmap);

			CString sText;
			sText.Format(_T("%02d:%02d:%02d.%03d"), pPacket->TimeStamp.wHour,
												pPacket->TimeStamp.wMinute,
												pPacket->TimeStamp.wSecond,
												pPacket->TimeStamp.wMSecond);
			
			// Die Titelzeile der ausgewählten Kamera hervorheben
			m_wSource = pPacket->wSource;
			if (m_pMainWnd->GetCurentCam() == m_wSource)
				m_Jpeg.SetBackgroundColor(RGB(0,0,192));
			else
				m_Jpeg.SetBackgroundColor(RGB(192,192,192));
			
			// Titelzeile schreiben
			m_Jpeg.SetOSDText1(rcWnd.left,rcWnd.top, sText);

			// Bild blitten
			m_Jpeg.OnDraw(&memDC, rcWnd);

			if (bDrawAdaptiveMask)
			{
				if (pPacket->AdaptiveMask.bValid == TRUE)
				{
					for (int nY = 0; nY < pPacket->AdaptiveMask.nDimV; nY++)
					{
						for (int nX = 0; nX < pPacket->AdaptiveMask.nDimH; nX++)
							DrawAdaptiveMask(&memDC, rcWnd, nX, nY, pPacket->AdaptiveMask.byMask[nY][nX]);
					}
				}
			}

			if (bDrawPermanentMask)
			{
				if (pPacket->PermanentMask.bValid == TRUE)
				{
					for (int nY = 0; nY < pPacket->PermanentMask.nDimV; nY++)
					{
						for (int nX = 0; nX < pPacket->PermanentMask.nDimH; nX++)
							DrawPermanentMask(&memDC, rcWnd, nX, nY, pPacket->PermanentMask.byMask[nY][nX]);
					}
				}
			}


			if (pPacket->bMotion)
			{
				DrawCross(&memDC, rcWnd, pPacket->Point[0].cx,  pPacket->Point[0].cy, pPacket->Point[0].nValue, TRUE);	
				DrawCross(&memDC, rcWnd, pPacket->Point[1].cx,  pPacket->Point[1].cy, pPacket->Point[1].nValue, FALSE);	
				DrawCross(&memDC, rcWnd, pPacket->Point[2].cx,  pPacket->Point[2].cy, pPacket->Point[2].nValue, FALSE);	
				DrawCross(&memDC, rcWnd, pPacket->Point[3].cx,  pPacket->Point[3].cy, pPacket->Point[3].nValue, FALSE);	
				DrawCross(&memDC, rcWnd, pPacket->Point[4].cx,  pPacket->Point[4].cy, pPacket->Point[4].nValue, FALSE);	
			}

			if (bDrawDiffImage)
			{
				if (pPacket->Diff.bValid == TRUE)
				{
					for (int nY = 0; nY < pPacket->Diff.nDimV; nY++)
					{
						for (int nX = 0; nX < pPacket->Diff.nDimH; nX++)
							DrawDiffImage(&memDC, rcWnd, nX, nY, pPacket->Diff.byMask[nY][nX]);
					}
				}
			}
			// Und das Offscreensurface zeichnen
	//		m_wSource = pPacket->wSource;
			rcWnd.OffsetRect(CPoint(m_wSource%3*nHSize, m_wSource/3*nVSize));
			pDC->BitBlt(rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), &memDC, 0, 0, SRCCOPY);

			memDC.SelectObject(pOldBitmap);
			Bitmap.DeleteObject();
			memDC.DeleteDC();

			ReleaseDC(pDC);
		}
	}

	return bResult;
}
////////////////////////////////////////////////////////////////////////////
BOOL CVideoInlayWindow::DrawJpegImage(DATA_PACKET *pPacket, BOOL bDrawAdaptiveMask/*=FALSE*/, BOOL bDrawPermanentMask/*= FALSE*/, BOOL bDrawDiffImage/*=FALSE*/)
{
	BOOL bResult = FALSE;
	int nHSize	 = 0;
	int nVSize   = 0;

	bResult = m_Jpeg.DecodeJpegFromMemory(&pPacket->pImageData,pPacket->dwImageDataLen); 

	if (bResult)
	{	
		CRect rcWnd(0,0,0,0);
		GetClientRect(rcWnd);
		CDC* pDC = GetDC();
		if (pDC)
		{
			if (m_bMultyView)
			{	
				nHSize = rcWnd.Width() / 3;
				nVSize = rcWnd.Height() / 3;
				rcWnd = CRect(0, 0, nHSize, nVSize);
			}

			// Offscreensurface anlegen
			CDC memDC;
			memDC.CreateCompatibleDC(pDC);

			// Kompatible Bitmap mit dem Offscreensurface verbinden
			CBitmap Bitmap;
			Bitmap.CreateCompatibleBitmap(pDC, rcWnd.Width(), rcWnd.Height());
			CBitmap* pOldBitmap = memDC.SelectObject(&Bitmap);

			CString sText;
			sText.Format(_T("%02d:%02d:%02d.%03d"), pPacket->TimeStamp.wHour,
												pPacket->TimeStamp.wMinute,
												pPacket->TimeStamp.wSecond,
												pPacket->TimeStamp.wMSecond);
			
			// Die Titelzeile der ausgewählten Kamera hervorheben
			m_wSource = pPacket->wSource;
			if (m_pMainWnd->GetCurentCam() == m_wSource)
				m_Jpeg.SetBackgroundColor(RGB(0,0,192));
			else
				m_Jpeg.SetBackgroundColor(RGB(192,192,192));
			
			// Titelzeile schreiben
			m_Jpeg.SetOSDText1(rcWnd.left,rcWnd.top, sText);

			// Bild blitten
			m_Jpeg.OnDraw(&memDC, rcWnd);

			if (bDrawAdaptiveMask)
			{
				if (pPacket->AdaptiveMask.bValid == TRUE)
				{
					for (int nY = 0; nY < pPacket->AdaptiveMask.nDimV; nY++)
					{
						for (int nX = 0; nX < pPacket->AdaptiveMask.nDimH; nX++)
							DrawAdaptiveMask(&memDC, rcWnd, nX, nY, pPacket->AdaptiveMask.byMask[nY][nX]);
					}
				}
			}

			if (bDrawPermanentMask)
			{
				if (pPacket->PermanentMask.bValid == TRUE)
				{
					for (int nY = 0; nY < pPacket->PermanentMask.nDimV; nY++)
					{
						for (int nX = 0; nX < pPacket->PermanentMask.nDimH; nX++)
							DrawPermanentMask(&memDC, rcWnd, nX, nY, pPacket->PermanentMask.byMask[nY][nX]);
					}
				}
			}


			if (pPacket->bMotion)
			{
				DrawCross(&memDC, rcWnd, pPacket->Point[0].cx,  pPacket->Point[0].cy, pPacket->Point[0].nValue, TRUE);	
				DrawCross(&memDC, rcWnd, pPacket->Point[1].cx,  pPacket->Point[1].cy, pPacket->Point[1].nValue, FALSE);	
				DrawCross(&memDC, rcWnd, pPacket->Point[2].cx,  pPacket->Point[2].cy, pPacket->Point[2].nValue, FALSE);	
				DrawCross(&memDC, rcWnd, pPacket->Point[3].cx,  pPacket->Point[3].cy, pPacket->Point[3].nValue, FALSE);	
				DrawCross(&memDC, rcWnd, pPacket->Point[4].cx,  pPacket->Point[4].cy, pPacket->Point[4].nValue, FALSE);	
			}

			if (bDrawDiffImage)
			{
				if (pPacket->Diff.bValid == TRUE)
				{
					for (int nY = 0; nY < pPacket->Diff.nDimV; nY++)
					{
						for (int nX = 0; nX < pPacket->Diff.nDimH; nX++)
							DrawDiffImage(&memDC, rcWnd, nX, nY, pPacket->Diff.byMask[nY][nX]);
					}
				}
			}
			// Und das Offscreensurface zeichnen
	//		m_wSource = pPacket->wSource;
			rcWnd.OffsetRect(CPoint(m_wSource%3*nHSize, m_wSource/3*nVSize));
			pDC->BitBlt(rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), &memDC, 0, 0, SRCCOPY);

			memDC.SelectObject(pOldBitmap);
			Bitmap.DeleteObject();
			memDC.DeleteDC();

			ReleaseDC(pDC);
		}
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////
BOOL CVideoInlayWindow::DrawMpeg4Image(DATA_PACKET *pPacket, BOOL bDrawAdaptiveMask/*=FALSE*/, BOOL bDrawPermanentMask/*= FALSE*/, BOOL bDrawDiffImage/*=FALSE*/)
{
	BOOL bResult	= FALSE;
	int nHSize		= 0;
	int nVSize		= 0;

	CRect rcWnd(0,0,0,0);
	GetClientRect(rcWnd);
	CDC* pDC = GetDC();
	
	if (pDC && pPacket)
	{
		if (m_bMultyView)
		{	
			nHSize = rcWnd.Width() / 3;
			nVSize = rcWnd.Height() / 3;
			rcWnd = CRect(0, 0, nHSize, nVSize);
		}

		// Offscreensurface anlegen
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);

		// Kompatible Bitmap mit dem Offscreensurface verbinden
		CBitmap Bitmap;
		Bitmap.CreateCompatibleBitmap(pDC, rcWnd.Width(), rcWnd.Height());
		CBitmap* pOldBitmap = memDC.SelectObject(&Bitmap);
	
		// Die Titelzeile der ausgewählten Kamera hervorheben
		m_wSource = pPacket->wSource;
		if (m_pMPEG4[m_wSource]->DecodeMpeg4FromMemory((BYTE*)&pPacket->pImageData, pPacket->dwImageDataLen))
		{
			if (m_pMainWnd->GetCurentCam() == m_wSource)
				m_pMPEG4[m_wSource]->SetBackgroundColor(RGB(0,0,192));
			else
				m_pMPEG4[m_wSource]->SetBackgroundColor(RGB(192,192,192));
		
			WORD wSource = pPacket->wSource;
			
			// Anhand der Datenmenge die Bitrate bestimmen
			m_dwDataLen[wSource] += pPacket->dwImageDataLen;

			m_dwFrameCounter[wSource]++;
			if (m_dwFrameCounter[wSource] > 1000)
			{
				m_dwStartTime[wSource]		= GetTickCount();
				m_dwFrameCounter[wSource]	= 0;
				m_dwDataLen[wSource]		= 0;
			}

			DWORD dwTimeDiff = GetTickCount() - m_dwStartTime[wSource];
			CString sFps;
			DWORD   dwBitrate = 0;
			if ((dwTimeDiff != 0) && (m_dwFrameCounter[wSource]>10))
			{
				DWORD dwFC = m_dwFrameCounter[wSource];
				
				// TODO: Der MPeg4-Decoder scheint ein Speicher oder Stackproblem zu verursachen.
				// Nach dem Dekodieren eines P-Frames scheitert der erste Typecast von DWORD nach double
				// daher als Notlösung fDummy als erste double Variable hinzugefügt.
				volatile double fDummy = 0;
				double fTimeDiff	= (double)dwTimeDiff;
				double fFC			= (double)dwFC;
				double fFps			= 1000.0*fFC / fTimeDiff;
				dwBitrate			= 8*m_dwDataLen[wSource]/dwTimeDiff;

				//TRACE(_T("Type=%d dwFC=%lu fFC=%f dwTimeDiff=%lu fTimeDiff=%f -> fFps=%.02f\n"), pPacket->eSubType, dwFC, fFC, dwTimeDiff, fTimeDiff, fFps);
				
				sFps.Format(_T("%.02f"), fFps);
				fDummy = fTimeDiff;	// Verhindert das in Release 'fDummy' vom Compiler entfernt wird.
			} 

			CString sText;
			sText.Format(_T("%02d:%02d:%02d.%03d/%sfps/%luKBit/%s"),
												pPacket->TimeStamp.wHour,
												pPacket->TimeStamp.wMinute,
												pPacket->TimeStamp.wSecond,
												pPacket->TimeStamp.wMSecond,
												sFps,
												dwBitrate,
												pPacket->eSubType == eSubIFrame ? _T("I") : _T("P"));
			// Titelzeile schreiben
			m_pMPEG4[m_wSource]->SetOSDText1(rcWnd.left,rcWnd.top, sText);

			// Bild blitten
			m_pMPEG4[m_wSource]->OnDraw(&memDC, rcWnd);
		}

		if (bDrawAdaptiveMask)
		{
			if (pPacket->AdaptiveMask.bValid == TRUE)
			{
				for (int nY = 0; nY < pPacket->AdaptiveMask.nDimV; nY++)
				{
					for (int nX = 0; nX < pPacket->AdaptiveMask.nDimH; nX++)
						DrawAdaptiveMask(&memDC, rcWnd, nX, nY, pPacket->AdaptiveMask.byMask[nY][nX]);
				}
			}
		}

		if (bDrawPermanentMask)
		{
			if (pPacket->PermanentMask.bValid == TRUE)
			{
				for (int nY = 0; nY < pPacket->PermanentMask.nDimV; nY++)
				{
					for (int nX = 0; nX < pPacket->PermanentMask.nDimH; nX++)
						DrawPermanentMask(&memDC, rcWnd, nX, nY, pPacket->PermanentMask.byMask[nY][nX]);
				}
			}
		}


		if (pPacket->bMotion)
		{
			DrawCross(&memDC, rcWnd, pPacket->Point[0].cx,  pPacket->Point[0].cy, pPacket->Point[0].nValue, TRUE);	
			DrawCross(&memDC, rcWnd, pPacket->Point[1].cx,  pPacket->Point[1].cy, pPacket->Point[1].nValue, FALSE);	
			DrawCross(&memDC, rcWnd, pPacket->Point[2].cx,  pPacket->Point[2].cy, pPacket->Point[2].nValue, FALSE);	
			DrawCross(&memDC, rcWnd, pPacket->Point[3].cx,  pPacket->Point[3].cy, pPacket->Point[3].nValue, FALSE);	
			DrawCross(&memDC, rcWnd, pPacket->Point[4].cx,  pPacket->Point[4].cy, pPacket->Point[4].nValue, FALSE);	
		}

		if (bDrawDiffImage)
		{
			if (pPacket->Diff.bValid == TRUE)
			{
				for (int nY = 0; nY < pPacket->Diff.nDimV; nY++)
				{
					for (int nX = 0; nX < pPacket->Diff.nDimH; nX++)
						DrawDiffImage(&memDC, rcWnd, nX, nY, pPacket->Diff.byMask[nY][nX]);
				}
			}
		}
		
		// Und das Offscreensurface zeichnen
		rcWnd.OffsetRect(CPoint(m_wSource%3*nHSize, m_wSource/3*nVSize));
		pDC->BitBlt(rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(pOldBitmap);
		Bitmap.DeleteObject();
		memDC.DeleteDC();
 
		ReleaseDC(pDC);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVideoInlayWindow::DrawCross(CDC* pDC, const CRect& rect, int nX, int nY, int nVal, BOOL bMain)
{
	BOOL bResult = FALSE;
	
	if (pDC && (nVal > 0))
	{
		int x,y;
		if ((nX != -1) && (nY != -1))
		{
			x = (nX * rect.Width()) / 1000 + rect.left;
			y = (nY * rect.Height()) / 1000 + rect.top;

			CPen pen;
			CPen* pOldPen;

			if (bMain)
			{
				pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
				pOldPen = pDC->SelectObject(&pen);
				pDC->MoveTo(rect.left,y);
				pDC->LineTo(rect.right,y);
				pDC->MoveTo(x,rect.top);
				pDC->LineTo(x,rect.bottom);

				int r = 4;

				pDC->Arc(x-r,y-r,x+r+1,y+r+1,x-r,y,x-r,y);
			}
			else
			{
				pen.CreatePen(PS_SOLID,1,RGB(255,0,0));
				pOldPen = pDC->SelectObject(&pen);
				int r = 4;

				pDC->Arc(x-r,y-r,x+r+1,y+r+1,x-r,y,x-r,y);
			}

			pDC->SelectObject(pOldPen);
			pen.DeleteObject();
		
			bResult = TRUE;
		}
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVideoInlayWindow::DrawAdaptiveMask(CDC* pDC, const CRect& rect, int nX, int nY, BYTE byValue)
{
	// TODO: Die Maske wird später noch im BF533 invertiert. So daß
	// 0 transparent und 255 deckend ist
	BYTE byV = 255-byValue;

	if (byV > 0)
	{
		int x,y;
		x = ((nX*8+4) * rect.Width()) / MD_RESOLUTION_H + rect.left;
		y = ((nY*8+4) * rect.Height()) / MD_RESOLUTION_V + rect.top;

		int rX = (4*rect.Width()/MD_RESOLUTION_H)+1;
		int rY = (4*rect.Height()/MD_RESOLUTION_V)+1;

		CRect rc(x-rX,y-rY,x+rX+1,y+rY+1);
		pDC->FillSolidRect(rc, RGB(0, byV, 0));
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVideoInlayWindow::DrawPermanentMask(CDC* pDC, const CRect& rect, int nX, int nY, BYTE byValue)
{
	// TODO: Die Maske wird später noch im BF533 invertiert. So daß
	// 0 transparent und 255 deckend ist
	BYTE byV = 255-byValue;

	if (byV > 0)
	{
		int x,y;
		x = ((nX*8+4) * rect.Width()) / MD_RESOLUTION_H + rect.left;
		y = ((nY*8+4) * rect.Height()) / MD_RESOLUTION_V + rect.top;

		int rX = (4*rect.Width()/MD_RESOLUTION_H)+1;
		int rY = (4*rect.Height()/MD_RESOLUTION_V)+1;

		CRect rc(x-rX,y-rY,x+rX+1,y+rY+1);
		pDC->FillSolidRect(rc, RGB(byV, 0, 0));
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVideoInlayWindow::DrawDiffImage(CDC* pDC, const CRect& rect, int nX, int nY, BYTE byValue)
{
	int x,y;
	
	byValue = min(8*byValue, 255);

	x = rect.right  - ((((45-nX)*2+1) * rect.Width()) / MD_RESOLUTION_H);
	y = rect.bottom - ((((36-nY)*2+1) * rect.Height()) / MD_RESOLUTION_V);

	int rX = (1*rect.Width()/MD_RESOLUTION_H)+1;
	int rY = (1*rect.Height()/MD_RESOLUTION_V)+1;

	CRect rc(x-rX,y-rY,x+rX+1,y+rY+1);
	pDC->FillSolidRect(rc, RGB(byValue, byValue, byValue));

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
LPBITMAPINFO CVideoInlayWindow::CreateDIB(const DATA_PACKET* pPacket)
{
	if (m_lpBMI)
	{
		m_lpBMI->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		m_lpBMI->bmiHeader.biWidth			= pPacket->wSizeH;
		m_lpBMI->bmiHeader.biHeight			= pPacket->wSizeV;
		m_lpBMI->bmiHeader.biPlanes			= 1;
		m_lpBMI->bmiHeader.biBitCount		= pPacket->wBytesPerPixel*8; 
		m_lpBMI->bmiHeader.biCompression	= mmioFOURCC('Y','4','2','2');
		m_lpBMI->bmiHeader.biSizeImage		= pPacket->dwImageDataLen;

		memcpy(m_lpBMI->bmiColors, &pPacket->pImageData+1, pPacket->dwImageDataLen-1);
	}

	return m_lpBMI;
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::SetPreviewMode(BOOL bMultyView)
{
	m_bMultyView = bMultyView;
	if (m_bMultyView)
		InvalidateRect(NULL, TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::EnableNoiseReduction()
{
	m_Jpeg.EnableNoiseReduction();
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::DisableNoiseReduction()
{
	m_Jpeg.DisableNoiseReduction();
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_nLastPaintPosX	= -1;
	m_nLastPaintPosY	= -1;

	OnMouseMove(nFlags, point);
	CFrameWnd::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_nLastPaintPosX	= -1;
	m_nLastPaintPosY	= -1;

	OnMouseMove(nFlags, point);
	CFrameWnd::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::OnMouseMove(UINT nFlags, CPoint point) 
{
	int		nX		= point.x;
	int		nY		= point.y;
	int		nHSize	= 0;
	int		nVSize	= 0;
	WORD	wSource = 0;

	CRect	rcClient;
	CRect	rcWnd;
	GetClientRect(rcClient);

	if (m_bMultyView)
	{	
		// Größese eines Teilbildes
		nHSize = rcClient.Width() / 3;
		nVSize = rcClient.Height() / 3;
		
		// Berechnet anhand der Mausposition über welchem 'Channel' ers ich befindet
		wSource	 = (3*point.x / rcClient.Width()) + 3*(3*point.y / rcClient.Height());
		
		// Ursprung auf auf die linke obere Ecke des Teilbildes setzen
		nX -= wSource%3*nHSize;
		nY -= wSource/3*nVSize;
	}
	else
	{
		nHSize = rcClient.Width();
		nVSize = rcClient.Height();
		wSource	 = m_wSource;
	}

	// Pixelkoordinaten in Blöcke umrechnen
	nX = nX*m_PaintMask.nDimH/nHSize;
	nY = nY*m_PaintMask.nDimV/nVSize;


	if ((nX >= 0) && (nX <m_PaintMask.nDimH) && (nY>=0) && (nY<m_PaintMask.nDimV) && (wSource < CHANNEL_COUNT))
	{
		if (nFlags & MK_LBUTTON)
		{
			if (wSource == m_pMainWnd->GetCurentCam())
			{
				// Block nur übermitteln, wenn nicht schon zuvor geschehen
				if ((nX != m_nLastPaintPosX) || (nY != m_nLastPaintPosY))
				{
					m_nLastPaintPosX = nX;
					m_nLastPaintPosY = nY;
					m_PaintMask.byMask[nY][nX] = 0;
					m_pCodec->DoRequestChangePermanentMask(0, wSource, nX, nY, 0);
				}
			}
			else
				m_pMainWnd->OnCheckCam(wSource);  // Schalte auf die 'angeklickte' Kamera

		}
		
		if (nFlags & MK_RBUTTON)
		{
			// Block nur übermitteln, wenn nicht schon zuvor geschehen
			if ((nX != m_nLastPaintPosX) || (nY != m_nLastPaintPosY))
			{
				m_nLastPaintPosX = nX;
				m_nLastPaintPosY = nY;
				m_PaintMask.byMask[nY][nX] = 255;
				m_pCodec->DoRequestChangePermanentMask(0, wSource, nX, nY, 255);
			}
		}
	}	
//	CFrameWnd::OnMouseMove(nFlags, point);
}

