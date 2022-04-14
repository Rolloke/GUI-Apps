// ChildView.cpp : Implementierung der Klasse CChildView
//

#include "stdafx.h"
#include "TestMotionDetection.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CChildView

CChildView::CChildView()
{
	m_nTimer = 0;
	m_ptPosition.x = 0;
	m_ptPosition.y = 0;
	m_bBlack = FALSE;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_APPLY_NOW, OnApplyNow)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CChildView Meldungshandler

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

//	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // Gerätekontext zum Zeichnen
	OnDraw(&dc);
}

void CChildView::OnDraw(CDC*pDC)
{
	switch (theApp.m_nType)
	{
		case MOVING_BLOCK_RANDOM:
			DrawMovingBlock(pDC);
			break;
		case COLORS_RGBCMY:	
			DrawColorsRGBCMY(pDC);
			break;
		case SHADE_WHITE_TO_BLACK:
			DrawShadeWhiteToBlack(pDC);
			break;
		case BLINK_WHITE_AND_BLACK:
			DrawBlinkWhiteAndBlack(pDC);
			break;
		case HORZ_AND_VERT_LINES:
			DrawLines(pDC);
			break;
		case TEST_PICTURE5:
			DrawMovingBlock(pDC);
			break;
		case TEST_PICTURE6:
			TestPicture6(pDC);
			break;
		case TEST_PICTURE7:
			TestPicture7(pDC);
			break;
		case TEST_PICTURE8:
			TestPicture8(pDC);
			break;
		case TEST_PICTURE9:
			TestPicture9(pDC);
			break;
	}
}

void CChildView::DrawBackGround(CDC*pDC, CRect&rc)
{
	CBrush brush(theApp.m_BackGroundColor);
	pDC->FillRect(&rc, &brush);
}

void CChildView::DrawMovingBlock(CDC*pDC)
{
	pDC->SaveDC();
	CRect rc(m_ptPosition, theApp.m_szBlock);
	pDC->SelectObject(GetStockObject(BLACK_BRUSH));
	pDC->SelectObject(GetStockObject(BLACK_PEN));
	pDC->Rectangle(&rc);
	pDC->RestoreDC(-1);
}

void CChildView::DrawColorsRGBCMY(CDC*pDC)
{
	CRect rc;
	const int nColors = 6;
	int i, nWidth;
	COLORREF colors[nColors] = 
	{
		RGB(255,0,0),	// Red
		RGB(0,255,0),	// Green
		RGB(0,0,255),	// Blue
		RGB(0,255,255),	// Cyan
		RGB(255,0,255),	// Magenta
		RGB(255,255,0)	// Yellow
	};
	pDC->SaveDC();
	pDC->SelectObject(GetStockObject(NULL_PEN));
	GetClientRect(&rc);

	nWidth = rc.right / nColors;
	i = rc.right % nColors;
	if (i)
	{
		nWidth += 1;
		rc.left -= 1;
	}
	rc.right = nWidth;
	for (i=0; i<nColors; i++)
	{
		{
			CBrush br(colors[i]);
			pDC->SelectObject(&br);
			pDC->Rectangle(&rc);
		}
		rc.OffsetRect(nWidth, 0);
	}
	pDC->RestoreDC(-1);
}

void CChildView::DrawShadeWhiteToBlack(CDC*pDC)
{
	CRect rc;
	int nColors, i, nWidth, nStep, nColor;

	pDC->SaveDC();
	pDC->SelectObject(GetStockObject(NULL_PEN));
	GetClientRect(&rc);

	nColors = rc.right / theApp.m_szBlock.cx;
	nWidth = rc.right;
	rc.left -= 1;
	nStep = 255 / (nColors-1);
	rc.right = theApp.m_szBlock.cx;
	for (i=0; i<nColors; i++)
	{
		if (i==nColors-1)
		{
			rc.right = nWidth;
		}
		{
			nColor = 255 - i*nStep;
			COLORREF color = RGB(nColor,nColor,nColor);
			CBrush br(color);
			pDC->SelectObject(&br);
			pDC->Rectangle(&rc);
		}
		rc.OffsetRect(theApp.m_szBlock.cx, 0);
	}
	pDC->RestoreDC(-1);
}

void CChildView::DrawBlinkWhiteAndBlack(CDC*pDC)
{
	CRect rc;
	COLORREF color = m_bBlack ? RGB(0,0,0) : RGB(255,255,255);
	CBrush br(color);
	GetClientRect(&rc);
	m_bBlack = !m_bBlack;

	pDC->SaveDC();
	pDC->SelectObject(GetStockObject(NULL_PEN));
	pDC->SelectObject(&br);
	pDC->Rectangle(&rc);
	pDC->RestoreDC(-1);
}

void CChildView::DrawLines(CDC*pDC)
{
	CRect rc;
	int i;

	pDC->SaveDC();
	pDC->SelectObject(GetStockObject(BLACK_PEN));
	GetClientRect(&rc);

	for (i=0; i<rc.right; i+=theApp.m_szBlock.cx)
	{
		pDC->MoveTo(i, rc.top);
		pDC->LineTo(i, rc.bottom);
	}

	for (i=0; i<rc.bottom; i+=theApp.m_szBlock.cy)
	{
		pDC->MoveTo(rc.left, i);
		pDC->LineTo(rc.right, i);
	}
	pDC->RestoreDC(-1);
}

void CChildView::TestPicture5(CDC*pDC)
{
}

void CChildView::TestPicture6(CDC*pDC)
{
}

void CChildView::TestPicture7(CDC*pDC)
{
}

void CChildView::TestPicture8(CDC*pDC)
{
}

void CChildView::TestPicture9(CDC*pDC)
{
}

void CChildView::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_nTimer)
	{
		switch (theApp.m_nType)
		{
			case MOVING_BLOCK_RANDOM:
			{
				CRect rc;
				GetClientRect(&rc);
				rc.right  -= theApp.m_szBlock.cx;
				rc.bottom -= theApp.m_szBlock.cy;
				int nX = rand();
				int nY = rand();

				CRect rcOld(m_ptPosition, theApp.m_szBlock);
				m_ptPosition.x = MulDiv(nX, rc.right, RAND_MAX);
				m_ptPosition.y = MulDiv(nY, rc.bottom, RAND_MAX);
				CRect rcNew(m_ptPosition, theApp.m_szBlock);
				InvalidateRect(&rcOld);
				InvalidateRect(&rcNew);
				
				WK_TRACE(_T("Motion:(%d, %d)\n"), m_ptPosition.x, m_ptPosition.y);
			} break;
			case BLINK_WHITE_AND_BLACK:
				InvalidateRect(NULL);
				break;
			case TEST_PICTURE5:
			{
				CRect rc;
				GetClientRect(&rc);
				rc.right  -= theApp.m_szBlock.cx;
				rc.bottom -= theApp.m_szBlock.cy;
				CRect rcOld(m_ptPosition, theApp.m_szBlock);
				CPoint ptOld = m_ptPosition;
				m_ptPosition += m_ptDirection;

				if (!rc.PtInRect(m_ptPosition))
				{
					CPoint ptTL = rc.TopLeft() - m_ptPosition;
					CPoint ptBR = m_ptPosition - rc.BottomRight();
					if (ptBR.y > 0 || ptTL.y > 0)
					{
						m_ptDirection.y = -m_ptDirection.y;
					}
					if (ptBR.x > 0 || ptTL.x > 0)
					{
						m_ptDirection.x = -m_ptDirection.x;
					}
				}

				CRect rcNew(m_ptPosition, theApp.m_szBlock);
				InvalidateRect(&rcOld);
				InvalidateRect(&rcNew);
			}break;
		}
	}

	CWnd::OnTimer(nIDEvent);
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	OnApplyNow();

	return 0;
}

void CChildView::OnDestroy()
{
	if (m_nTimer)
	{
		KillTimer(m_nTimer);
	}
	CWnd::OnDestroy();
}

void CChildView::OnApplyNow()
{
	if (m_nTimer)
	{
		KillTimer(m_nTimer);
	}
	switch (theApp.m_nType)
	{
		case MOVING_BLOCK_RANDOM:
		case BLINK_WHITE_AND_BLACK:	
			m_nTimer = (UINT)SetTimer(10, theApp.m_nTimeOut, NULL);
			break;
		case TEST_PICTURE5:
		{
			CRect rc;
			m_nTimer = (UINT)SetTimer(10, theApp.m_nTimeOut, NULL);
			GetClientRect(&rc);
			rc.right  -= theApp.m_szBlock.cx;
			rc.bottom -= theApp.m_szBlock.cy;
			int nX = rand();
			int nY = rand();

			CRect rcOld(m_ptPosition, theApp.m_szBlock);
			m_ptPosition.x = MulDiv(nX, rc.right, RAND_MAX);
			m_ptPosition.y = MulDiv(nY, rc.bottom, RAND_MAX);

			nX = rand();
			nY = rand();
			m_ptDirection.x = MulDiv(nX, theApp.m_szBlock.cx, RAND_MAX);
			m_ptDirection.y = MulDiv(nY, theApp.m_szBlock.cy, RAND_MAX);
		}	break;
		case COLORS_RGBCMY:
		case SHADE_WHITE_TO_BLACK:
		case HORZ_AND_VERT_LINES:
		case TEST_PICTURE6:
		case TEST_PICTURE7:
		case TEST_PICTURE8:
		case TEST_PICTURE9:
			break;
	}
	InvalidateRect(NULL);
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	switch (theApp.m_nType)
	{
		case COLORS_RGBCMY:
		case SHADE_WHITE_TO_BLACK:
		case BLINK_WHITE_AND_BLACK:
			return TRUE;
		case TEST_PICTURE5:// undefined draw background
		case TEST_PICTURE6:
		case TEST_PICTURE7:
		case TEST_PICTURE8:
		case TEST_PICTURE9:
			break;
	}
	if (theApp.m_BackGroundColor == DEFAULT_COLOR)
	{
		return CWnd::OnEraseBkgnd(pDC);
	}
	else
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		DrawBackGround(pDC, rcClient);
		return TRUE;
	}
}
