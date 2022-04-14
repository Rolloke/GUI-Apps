// LineTracker.cpp : implementation file
//

#include "stdafx.h"
#include "GraphCreator.h"
#include "LineTracker.h"

#include <afximpl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLineTracker

CLineTracker::CLineTracker()
{
	m_bDrawn = false;
}
/////////////////////////////////////////////////////////////////////////////
CLineTracker::CLineTracker(LPCRECT lpSrcRect, UINT nStyle) : CRectTracker(lpSrcRect, nStyle)
{
	m_bDrawn = false;
}
/////////////////////////////////////////////////////////////////////////////
CLineTracker::~CLineTracker()
{
}
/////////////////////////////////////////////////////////////////////////////
void CLineTracker::DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd)
{
	int nROP=0;
	if (m_bDrawn)
	{
		nROP = pDC->SetROP2(R2_NOTXORPEN);
		pDC->MoveTo(m_rcOld.left, m_rcOld.top);
		pDC->LineTo(m_rcOld.right, m_rcOld.bottom);
	}
	if (lpRect)
	{
		pDC->MoveTo(lpRect->left, lpRect->top);
		pDC->LineTo(lpRect->right, lpRect->bottom);
		m_rcOld = *lpRect;
		m_bDrawn = true;
	}
	else
	{
		m_bDrawn = FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLineTracker::AdjustRect(int nHandle, LPRECT lpRect)
{
	
}

/////////////////////////////////////////////////////////////////////////////
// CLineTracker message handlers

