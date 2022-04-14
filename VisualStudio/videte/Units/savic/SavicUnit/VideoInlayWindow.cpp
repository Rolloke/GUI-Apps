/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: VideoInlayWindow.cpp $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/VideoInlayWindow.cpp $
// CHECKIN:		$Date: 20.06.03 13:15 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 20.06.03 12:20 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VideoInlayWindow.h"

#define MAX_HSIZE 352
#define MAX_VSIZE 300

/////////////////////////////////////////////////////////////////////////////
// CVideoInlayWindow

BEGIN_MESSAGE_MAP(CVideoInlayWindow, CFrameWnd)
	//{{AFX_MSG_MAP(CVideoInlayWindow)
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_MOVING()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//IMPLEMENT_DYNCREATE(CVideoInlayWindow, CFrameWnd)

/////////////////////////////////////////////////////////////////////////////
CVideoInlayWindow::CVideoInlayWindow(CCodec *pCodec, BOOL bPrewiew)
{
	m_pCodec			= pCodec;

	CRect rc(50,50,50+MAX_HSIZE,50+MAX_VSIZE);
	CString sTitle(bPrewiew ? _T("SaVicUnit Preview") : _T("SaVicUnit VideoInlay"));

	if (!Create(NULL, sTitle,	WS_OVERLAPPED | WS_THICKFRAME |
												WS_MAXIMIZEBOX|	WS_SYSMENU,
												rc, NULL, NULL))
	{	
		WK_TRACE(_T("VideoInlayWindow Create Window failed\n"));
		return;
	}

	if (m_pCodec)
		m_pCodec->VideoInlayActivate();

	// Hintergrundfarbe auf die KeyColor setzen
	COLORREF ColRef = RGB(255,0,255);
	DeleteObject((HGDIOBJ)SetClassLong(m_hWnd, GCL_HBRBACKGROUND, (DWORD)CreateSolidBrush(ColRef)));

	ShowWindow(SW_NORMAL);
	GetClientRect(&rc);
	ClientToScreen(&rc);
	m_pCodec->VideoInlaySetOutputWindow(rc);

}

/////////////////////////////////////////////////////////////////////////////
CVideoInlayWindow::~CVideoInlayWindow()
{
	if (m_pCodec)
		m_pCodec->VideoInlayDeactivate();
}

////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::OnMove(int x, int y) 
{
	if (m_pCodec)
	{
		CRect rc;
		GetClientRect(&rc);
		ClientToScreen(&rc);
//		m_pCodec->VideoInlaySetOutputWindow(rc);
	}
	CFrameWnd::OnMove(x, y);
}

////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::OnMoving( UINT nSide, LPRECT lpRect ) 
{
	if (m_pCodec)
	{
		CRect rc;
		GetClientRect(&rc);
		ClientToScreen(&rc);
		rc = *lpRect;
		rc.DeflateRect(4,32,4,4);
		m_pCodec->VideoInlaySetOutputWindow(rc);
	}
	CFrameWnd::OnMoving(nSide, lpRect);
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::OnSize(UINT nType, int cx, int cy) 
{
	if (m_pCodec)
	{						   
		CRect rc;
		GetClientRect(&rc);
		ClientToScreen(&rc);
//		m_pCodec->VideoInlaySetOutputWindow(rc);
	}
	CFrameWnd::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////
void CVideoInlayWindow::OnSizing( UINT nSide, LPRECT lpRect )
{
	if (m_pCodec)
	{						   
		CRect rc;
		GetClientRect(&rc);
		ClientToScreen(&rc);
		rc = *lpRect;
		rc.DeflateRect(4,32,4,4);
		m_pCodec->VideoInlaySetOutputWindow(rc);
	}
	CFrameWnd::OnSizing(nSide, lpRect);
}
