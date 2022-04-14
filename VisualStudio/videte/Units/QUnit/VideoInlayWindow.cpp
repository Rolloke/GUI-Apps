/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: VideoInlayWindow.cpp $
// ARCHIVE:		$Archive: /Project/Units/QUnit/VideoInlayWindow.cpp $
// CHECKIN:		$Date: 27.03.06 9:30 $
// VERSION:		$Revision: 16 $
// LAST CHANGE:	$Modtime: 24.03.06 17:43 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <VImage\CJpeg.h>
#include <math.h>
#include "VideoInlayWindow.h"
#include "CUDP.h"
#include "QUnitDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CVideoInlayWindow

BEGIN_MESSAGE_MAP(CVideoInlayWindow, CFrameWnd)
	//{{AFX_MSG_MAP(CVideoInlayWindow)
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_MOVING()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//IMPLEMENT_DYNCREATE(CVideoInlayWindow, CFrameWnd)

/////////////////////////////////////////////////////////////////////////////
CVideoInlayWindow::CVideoInlayWindow(CUDP* pUDP, CQUnitDlg* pMainWnd)
{
	m_pJpeg				= new CJpeg();
	m_pMainWnd			= pMainWnd;
	m_dwVideoStatus		= 0;
	m_nActiveCamera		= 0;
	m_pUDP				= pUDP;
	m_bMulti			= FALSE;
	m_nVisibleCameras = 0;

	CRect rc(250,50,1200,850);

	if (!Create(NULL, _T("Q-Unit Preview"),	WS_OVERLAPPED | WS_THICKFRAME | WS_MAXIMIZEBOX|	WS_SYSMENU,
												rc, NULL, NULL))
	{	
		WK_TRACE_ERROR(_T("VideoInlayWindow Create Window failed\n"));
		return;
	}

	// Zuordnung der dargestellten Kammera und ihrer Position.
	for (int nI = 0; nI < m_pUDP->GetAvailableCameras(); nI++)
		m_nCameraPos[nI] = 0;

	// Hintergrundfarbe auf Schwarz setzen
	DeleteObject((HGDIOBJ)SetClassLong(m_hWnd, GCL_HBRBACKGROUND, (DWORD)CreateSolidBrush(RGB(0,0,0))));

	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

	ShowWindow(SW_NORMAL);
	GetClientRect(&rc);
	ClientToScreen(&rc);
	CenterWindow();
}

/////////////////////////////////////////////////////////////////////////////
CVideoInlayWindow::~CVideoInlayWindow()
{
	delete m_pJpeg;
	m_pJpeg = NULL;
}

////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::OnMove(int x, int y) 
{
	CFrameWnd::OnMove(x, y);
}

////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::OnMoving( UINT nSide, LPRECT lpRect ) 
{
	CFrameWnd::OnMoving(nSide, lpRect);
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CVideoInlayWindow::DrawImage(QIMAGE* pQImage, BOOL bMulti)
{
	CRect rcClient(0,0,0,0);
	GetClientRect(rcClient);
	CRect rcWnd = rcClient;
	
	m_bMulti		= bMulti;

	int nCamera		= pQImage->nCamera;
//	int nWidth		= pQImage->nWidth;
//	int nHeight		= pQImage->nHeight;

	if (bMulti)
	{
		int nPos = 0;
	
		// Ermittelt die 'Fenster'-Position der aktuell darzustellenden Kamera, sowie die insgesamte
		// Anzahl von Kameras mit Videosignal.
		m_nVisibleCameras = 0;
		for (int nI = 0; nI < m_pUDP->GetAvailableCameras(); nI++)
		{
			BOOL bPresent = FALSE;
			if (m_pUDP->DoRequestGetVideoState(nI, bPresent))
			{
				if (bPresent)
				{
					m_nVisibleCameras++;
					if (nI < nCamera)
						nPos++;
				}
			}
		}
		m_nCameraPos[nPos] = nCamera;

		int nCol = (int)(sqrt((double)m_nVisibleCameras)+0.5);
		int nHOffset = rcClient.Width()/nCol;
		int nVOffset = rcClient.Height()/nCol;

		rcWnd.right = nHOffset;
		rcWnd.bottom = nVOffset;	
		rcWnd.OffsetRect(CPoint((nPos%nCol)*nHOffset, (nPos/nCol)*nVOffset));
	}
	else if (nCamera != m_nActiveCamera)
	{
		return FALSE;	
	}

	CDC* pDC = GetDC();
	if (pQImage->pPMDoints && pQImage->pPMDoints->GetSize()>0)
	{
		// Die Titelzeile der ausgewählten Kamera hervorheben
		if (nCamera == m_nActiveCamera)
			m_pJpeg->SetBackgroundColor(RGB(192,0,0));
		else
			m_pJpeg->SetBackgroundColor(RGB(128,0,0));
	}
	else
	{
		// Die Titelzeile der ausgewählten Kamera hervorheben
		if (nCamera == m_nActiveCamera)
			m_pJpeg->SetBackgroundColor(RGB(0,192,0));
		else
			m_pJpeg->SetBackgroundColor(RGB(0,128,0));
	}

	CString sText;
	sText.Format(_T("Cam:%02d, fps=%.02f"), nCamera+1, pQImage->fFramerate);
	m_pJpeg->SetOSDText1(rcWnd.left, rcWnd.top, sText);

	sText.Format(_T("Res.:%dx%d"), pQImage->nWidth, pQImage->nHeight);
	CFont Font2;
	Font2.CreatePointFont(10*10, _T("Courier New"));

	m_pJpeg->SetFont2(Font2);
	m_pJpeg->SetOSDText2(rcWnd.left, rcWnd.bottom-16, sText);

	if (pQImage->ct == COMPRESSION_YUV_422)
		m_pJpeg->SetDIBData(pQImage->lpBMI);
	else if (pQImage->ct == COMPRESSION_JPEG)
		m_pJpeg->DecodeJpegFromMemory((BYTE*)pQImage->lpBMI->bmiColors, pQImage->lpBMI->bmiHeader.biSizeImage);

	m_pJpeg->OnDraw(pDC, rcWnd);

	
	if (pQImage->pPMDoints && pQImage->pPMDoints->GetSize()>0)
	{
		DrawCross(pDC, rcWnd, pQImage->pPMDoints->GetPointAt(0).x,
							  pQImage->pPMDoints->GetPointAt(0).y,
							  pQImage->pPMDoints->GetValueAt(0), TRUE);
	}

	ReleaseDC(pDC);

	return TRUE;
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
void CVideoInlayWindow::OnSizing( UINT nSide, LPRECT lpRect )
{
	CFrameWnd::OnSizing(nSide, lpRect);
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::SetActiveCamera(int nActiveCamera)
{
	m_nActiveCamera = nActiveCamera;
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	int		nHSize	= 0;
	int		nVSize	= 0;

	CRect	rcClient;
	CRect	rcWnd;
	GetClientRect(rcClient);

	if (m_bMulti)
	{
		int nCol = (int)(sqrt((double)m_nVisibleCameras)+0.5);

		// Größe eines Teilbildes
		nHSize = rcClient.Width() / nCol;
		nVSize = rcClient.Height() / nCol;
		
		// Berechnet anhand der Mausposition über welchem 'Channel' sie sich befindet
		int nPos = (nCol*point.x / rcClient.Width()) + nCol*(nCol*point.y / rcClient.Height());
		m_nActiveCamera = m_nCameraPos[nPos];
	}
	else
	{
		
	}

	if ((m_nActiveCamera >= 0) && (m_nActiveCamera < m_pUDP->GetAvailableCameras()))
	{
		if (m_pMainWnd)
			m_pMainWnd->SetActiveCamera(m_nActiveCamera);
	}
	
	CFrameWnd::OnLButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::SetDeinterlace(BOOL bDeInt)
{
	if (m_pJpeg)
	{
		m_pJpeg->SetDeInterlace(bDeInt);
	}
}
