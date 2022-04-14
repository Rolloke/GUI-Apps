#include "StdAfx.h"
#include "WndImage.h"
#include "idipclient.h"
#include "ViewIdipClient.h"

IMPLEMENT_DYNAMIC(CWndImage, CWndSmall)

/////////////////////////////////////////////////////////////////////////////
CWndImage::CWndImage(CServer* pServer)
	: CWndSmall(pServer)
{
	m_pMpeg4Decoder	= NULL;
	m_pJpeg			= NULL;
	m_pH263			= NULL;
	m_pPTDecoder	= NULL;
}
/////////////////////////////////////////////////////////////////////////////
CWndImage::~CWndImage(void)
{
	WK_DELETE(m_pMpeg4Decoder);
	WK_DELETE(m_pJpeg);
	WK_DELETE(m_pH263);
	WK_DELETE(m_pPTDecoder);
}

BEGIN_MESSAGE_MAP(CWndImage, CWndSmall)
	//{{AFX_MSG_MAP(CWndPlay)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
UINT CWndImage::GetToolBarID()
{
	CRuntimeClass* pClass = GetRuntimeClass();
	WK_TRACE_WARNING(_T("CWndImage::GetToolBarID not overwritten for %s\n"), pClass->m_lpszClassName);
	return (UINT)(-1);
}
/////////////////////////////////////////////////////////////////////////////
CSize CWndImage::GetPictureSize()
{
	return CSize(0,0);
}
/////////////////////////////////////////////////////////////////////////////
void CWndImage::GetVideoClientRect(LPRECT lpRect)
{
	GetVideoClientRect(lpRect, GetPictureSize(), m_b1to1);
}
/////////////////////////////////////////////////////////////////////////////
void CWndImage::GetVideoClientRect(LPRECT lpRect, CSize picturesize, BOOL b1to1)
{
	int w,h;
	BOOL bKeepProportion = theApp.m_bKeepProportion;
	if (IsWndPlay())
	{
		bKeepProportion = TRUE;
	}
	GetFrameRect(lpRect);
	CSize s = picturesize;
	if (s.cx > (s.cy<<1))
	{
		s.cy <<= 1;
	}

	w = lpRect->right - lpRect->left;
	h = lpRect->bottom - lpRect->top;

	if (   b1to1 && s.cx<w && s.cy<h)
	{
		lpRect->left   = (lpRect->left + (w - s.cx))>>1;
		lpRect->top    = (lpRect->top  + (h - s.cy))>>1;
		lpRect->right  =  lpRect->left + s.cx;
		lpRect->bottom =  lpRect->top  + s.cy;
	}
	else if (bKeepProportion)
	{
		int xoff = 0;
		int yoff = 0;
		int w_m_s_cy = w*s.cy;
		int h_m_s_cx = h*s.cx;
		if (w_m_s_cy > h_m_s_cx)
		{
			// zu breit
			yoff = 0;
			xoff =  (w-h_m_s_cx/s.cy)>>1;
		}
		else if (w_m_s_cy < h_m_s_cx)
		{
			// zu hoch
			xoff = 0;
			yoff =  (h-w_m_s_cy/s.cx)>>1;
		}
		else
		{
			// passt
			xoff = 0;
			yoff = 0;
		}

		lpRect->top += yoff;
		lpRect->bottom -= yoff;
		lpRect->left += xoff;
		lpRect->right -= xoff;
	}
} 
/////////////////////////////////////////////////////////////////////////////
void CWndImage::DrawCinema(CDC* pDC, LPRECT lpRectKeyColor /*= NULL*/)
{
	if (theApp.m_bKeepProportion == 0)
	{
		return;
	}
	CRect vr;
	CRect cr;
	CRect fr;

	if (lpRectKeyColor)
	{
		vr = lpRectKeyColor;
	}
	else
	{
		GetVideoClientRect(vr);
	}

	GetFrameRect(fr);

	cr = fr;
	if (vr.Width()+1<fr.Width())
	{
		// links und rechts einen Rahmen
		cr.right = vr.left;
		pDC->FillRect(cr,&m_pViewIdipClient->m_CinemaBrush); // links
		cr.right = fr.right;
		cr.left = vr.right;
		pDC->FillRect(cr,&m_pViewIdipClient->m_CinemaBrush); // rechts
	}
	cr = fr;
	if (vr.Height()<fr.Height())
	{
		// oben und unten einen Rahmen
		cr.bottom = vr.top;
		pDC->FillRect(cr,&m_pViewIdipClient->m_CinemaBrush); // oben
		cr.top = vr.bottom;
		cr.bottom = fr.bottom;
		pDC->FillRect(cr,&m_pViewIdipClient->m_CinemaBrush); // unten
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndImage::DrawCross(CDC* pDC, WORD wX, WORD wY, CRect*prcVC/*=NULL*/)
{
	if (theApp.m_bTargetDisplay)
	{
		// Den Zoom berücksichtigen.
		if (!(m_rcZoom.IsRectEmpty()))
		{
			int x,y;
			CSize ImageSize(GetPictureSize());
			x = (wX * ImageSize.cx) / 1000 - m_rcZoom.left;
			y = (wY * ImageSize.cy) / 1000 - m_rcZoom.top;
			
			wX = (WORD)(1000 * x / m_rcZoom.Width());
			wY = (WORD)(1000 * y / m_rcZoom.Height());
		}
		
		CRect rect;
		if (prcVC)
		{
			rect = *prcVC;
		}
		else
		{
			GetVideoClientRect(rect);
		}
		int x,y;
		x = (wX * rect.Width()) / 1000 + rect.left;
		y = (wY * rect.Height()) / 1000 + rect.top;
		
		CPen pen;
		CPen* pOldPen;
		if (IsWndPlay() && m_pServer->CanRectangleQuery())
		{
			pen.CreatePen(PS_SOLID,1,RGB(255,255,0));
		}
		else
		{
			pen.CreatePen(PS_SOLID,1,RGB(255,255,255));
		}
		pOldPen = pDC->SelectObject(&pen);
		
		pDC->MoveTo(rect.left,y);
		pDC->LineTo(rect.right,y);
		
		pDC->MoveTo(x,rect.top);
		pDC->LineTo(x,rect.bottom);
		
		int r = 4;
		
		pDC->Arc(x-r,y-r,x+r+1,y+r+1,x-r,y,x-r,y);
		
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndImage::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CWndSmall::OnPrint(pDC, pInfo);
}
/////////////////////////////////////////////////////////////////////////////
void CWndImage::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pPictureRecord != NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CWndImage::OnEditCopy() 
{
	CWnd *pWnd = GetFocus();
	if (pWnd)
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CEdit)))
		{
			int nStart, nEnd;
			((CEdit*)pWnd)->GetSel(nStart, nEnd);
			if (nStart < nEnd)
			{
				((CEdit*)pWnd)->Copy();
                return;
			}
		}
	}
	CAutoCritSec acs(&m_csPictureRecord);
	if (m_pPictureRecord) 
	{
		if (  (   m_pPictureRecord->GetCompressionType()==COMPRESSION_JPEG
			   || m_pPictureRecord->GetCompressionType()==COMPRESSION_RGB_24
			   || m_pPictureRecord->GetCompressionType()==COMPRESSION_YUV_422)
			&& m_pJpeg)
		{
			CSize s = GetPictureSize();
			HBITMAP hBitmap = m_pJpeg->CreateBitmap();
			if (hBitmap)
			{
				if (OpenClipboard())
				{
					EmptyClipboard();
					if (!SetClipboardData (CF_BITMAP, (HANDLE) hBitmap))
						DeleteObject(hBitmap);
					CloseClipboard();
				}
			}
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_MPEG4 && m_pMpeg4Decoder)
		{
			CDib* pDib;
			pDib = m_pMpeg4Decoder->GetDib();
			if (pDib)
			{
				pDib->CopyToClipBoard();
				WK_DELETE(pDib);
			}
		}
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_H263 && m_pH263)
		{
			CDib* pDib;
			pDib = m_pH263->GetDib();
			if (pDib)
			{
				pDib->CopyToClipBoard();
				WK_DELETE(pDib);
			}
		}
#ifndef _DTS
		else if (m_pPictureRecord->GetCompressionType()==COMPRESSION_PRESENCE && m_pPTDecoder)
		{
			CDib dib(m_pPTDecoder->GetBitmapHandle());
			dib.IncreaseTo24Bit();
			dib.CopyToClipBoard();
		}
#endif
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndImage::OnSize(UINT nType, int cx, int cy)
{
	CWndSmall::OnSize(nType, cx, cy);
}
