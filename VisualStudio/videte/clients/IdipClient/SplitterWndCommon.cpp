// SplitterWndCommon.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "SplitterWndCommon.h"
#include ".\splitterwndcommon.h"


// CSplitterWndCommon

IMPLEMENT_DYNAMIC(CSplitterWndCommon, CSplitterWnd)
CSplitterWndCommon::CSplitterWndCommon()
{
}

CSplitterWndCommon::~CSplitterWndCommon()
{
}


BEGIN_MESSAGE_MAP(CSplitterWndCommon, CSplitterWnd)
END_MESSAGE_MAP()

// CSplitterWndCommon message handlers

void CSplitterWndCommon::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
	// this is a mfc source copy and the middle color
	// was changed to frame color

	if (pDC == NULL)
	{
		RedrawWindow(rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
		return;
	}
	ASSERT_VALID(pDC);

	// otherwise, actually draw
	CRect rect = rectArg;
	switch (nType)
	{
	case splitBorder:
		pDC->Draw3dRect(rect, afxData.clrBtnShadow, afxData.clrBtnHilite);
		rect.InflateRect(-CX_BORDER, -CY_BORDER);
		pDC->Draw3dRect(rect, afxData.clrWindowFrame, afxData.clrBtnFace);
		return;

	case splitIntersection:
		break;

	case splitBox:
		pDC->Draw3dRect(rect, afxData.clrBtnFace, afxData.clrWindowFrame);
		rect.InflateRect(-CX_BORDER, -CY_BORDER);
		pDC->Draw3dRect(rect, afxData.clrBtnHilite, afxData.clrBtnShadow);
		rect.InflateRect(-CX_BORDER, -CY_BORDER);
		break;

	case splitBar:
		break;

	default:
		ASSERT(FALSE);  // unknown splitter type
	}

	// fill the middle
	COLORREF clr = afxData.clrWindowFrame; // this was changed to mfc source
	pDC->FillSolidRect(rect, clr);
}
