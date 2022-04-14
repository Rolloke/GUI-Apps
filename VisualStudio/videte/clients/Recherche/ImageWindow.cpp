// ImageWindow.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"
#include "RechercheView.h"
#include "ImageWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageWindow

CImageWindow::CImageWindow(CServer* pServer)
	: CSmallWindow(pServer)
{
	m_pFieldDialog = NULL;
	m_pNavigationDialog = NULL;
}

CImageWindow::~CImageWindow()
{
}


BEGIN_MESSAGE_MAP(CImageWindow, CSmallWindow)
	//{{AFX_MSG_MAP(CImageWindow)
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CImageWindow::IsImageWindow()
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CImageWindow message handlers
/////////////////////////////////////////////////////////////////////////////
void CImageWindow::DrawCinema(CDC* pDC)
{
	CRect vr;
	CRect cr;
	CRect fr;
	CBrush b;
	b.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

	GetVideoClientRect(vr);
	GetFrameRect(fr);

	cr = fr;
	if (vr.Width()+1<fr.Width())
	{
		// links und rechts einen Rahmen
		cr.right = vr.left;
		pDC->FillRect(cr,&b); // links
		cr.right = fr.right;
		cr.left = vr.right;
		pDC->FillRect(cr,&b); // rechts
	}
	cr = fr;
	if (vr.Height()<fr.Height())
	{
		// oben und unten einen Rahmen
		cr.bottom = vr.top;
		pDC->FillRect(cr,&b); // oben
		cr.top = vr.bottom;
		cr.bottom = fr.bottom;
		pDC->FillRect(cr,&b); // unten
	}
	b.DeleteObject();
}
/////////////////////////////////////////////////////////////////////////////
void CImageWindow::GetVideoClientRect(LPRECT lpRect)
{
	GetFrameRect(lpRect);
	int w,h;

	w = lpRect->right - lpRect->left;
	h = lpRect->bottom - lpRect->top;

	CSize s = GetPictureSize();

	if (   (s.cx==0)
		|| (s.cy==0)
		)
	{
		s.cx = 352;
		s.cy = 288;
	}

	if (m_b1to1 && m_pRechercheView->Is1x1())
	{
		lpRect->left = lpRect->left + (w - s.cx)/2;
		lpRect->top = lpRect->top + (h - s.cy)/2;
		lpRect->right = lpRect->left + s.cx;
		lpRect->bottom = lpRect->top + s.cy;
	}
	else
	{
		int xoff = 0;
		int yoff = 0;
		if ( (w*s.cy) > (h*s.cx))
		{
			// zu breit
			yoff = 0;
			xoff =  (w-(s.cx*h)/s.cy)/2;
		}
		else if ( (w*s.cy) < (h*s.cx))
		{
			// zu hoch
			xoff = 0;
			yoff =  (h-(s.cy*w)/s.cx)/2;
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
void CImageWindow::OnSize(UINT nType, int cx, int cy) 
{
	CSmallWindow::OnSize(nType, cx, cy);
	
	if (WK_IS_WINDOW(m_pNavigationDialog))
	{
		m_pNavigationDialog->MoveWindow(0,
									    cy-NAVIGATION_HEIGHT,
										cx,
										NAVIGATION_HEIGHT,
										TRUE);
	}
	if (WK_IS_WINDOW(m_pFieldDialog))
	{
		m_pFieldDialog->MoveWindow(0,//cx-FIELD_WIDTH,
								   m_ToolBarSize.cy,
								   FIELD_WIDTH,
								   cy-NAVIGATION_HEIGHT-m_ToolBarSize.cy,
								   TRUE);
	}
	
}
/////////////////////////////////////////////////////////////////////////////
int CImageWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSmallWindow::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pNavigationDialog = new CNavigationDialog(this);
	m_pFieldDialog = new CFieldDialog(this);
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CImageWindow::UpdateDialogs()
{
	if (WK_IS_WINDOW(m_pFieldDialog))
	{
		m_pFieldDialog->Update();
	}
	if (WK_IS_WINDOW(m_pNavigationDialog))
	{
		m_pNavigationDialog->Update();
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CImageWindow::FormatData(const CString& sD,const CString& sS)
{
	CString result;

	int i,c;
	CString name,value;
	CIPCField* pField;

	c = m_Fields.GetSize();

	for (i=0;i<c;i++)
	{
		pField = m_Fields.GetAt(i);
		name = theApp.GetMappedString(pField->GetName());
		if (name.IsEmpty())
		{
			if	(   (pField->GetName() == CIPCField::m_sfStNm)
				 || (pField->GetName() == CIPCField::m_sfCaNm)
				 || (pField->GetName() == CIPCField::m_sfInNm)
				 || (pField->GetName() == CIPCField::m_sfCurr)
				)
			{
				if (m_pServer)
				{
					name = m_pServer->GetFieldName(pField->GetName());
				}
			}
		}
		value = pField->ToString();
		if (!name.IsEmpty() && !value.IsEmpty())
		{
			value.TrimLeft();
			value.TrimRight();
			if (!value.IsEmpty())
			{
				if (!result.IsEmpty())
				{
					result += sD;
				}
				result += name + sS + value;
			}
		}
	}

	return result;
}
/////////////////////////////////////////////////////////////////////////////
int CImageWindow::PrintInfoRectLandscape(CDC* pDC,CRect rect, CFont* pFont)
{
	CFont* pFontOld;
	CString sTitle,sData;
	CSize size;
	int h = 0;
	
	sTitle = theApp.GetStationName();
	if (sTitle.IsEmpty())
	{
		sTitle = COemGuiApi::GetProducerName();
	}

	pFontOld = pDC->SelectObject(pFont);
	sTitle += _T(" ") + FormatPicture();
	size = pDC->GetOutputTextExtent(sTitle);
	h += size.cy+1;
	pDC->TextOut(rect.left+1,
				 rect.top+1,
				 sTitle);
	sData = FormatData(_T(", "),_T(": "));
	size = pDC->GetOutputTextExtent(sTitle);
	pDC->TextOut(rect.left+1,
				 rect.top+h+2,
				 sData);
	h += size.cy+3;
	pDC->SelectObject(pFontOld);
	return h;
}
/////////////////////////////////////////////////////////////////////////////
void CImageWindow::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CRect ir;
	CRect pr;
	CRect rect(pInfo->m_rectDraw);
	int iSpace = pDC->GetDeviceCaps(LOGPIXELSX) / 10;
	
	int iX = rect.Width()/25;
	int iY = rect.Height()/25;

	if (rect.Width()>rect.Height())
	{
		// Landscape = Videoprinter
		CFont font;
		int h;
		ir = rect;
		ir.bottom = ir.top + iY * 2;
		font.CreatePointFont(50,_T("Arial"),pDC);
		h = PrintInfoRectLandscape(pDC,ir,&font);
		pr = rect;
		pr.top += h;
		PrintPicture(pDC,pr);
	}
	else
	{
		rect.DeflateRect(iX,iY);
		// Portraet = normaler Drucker
		CFont font;
		ir = rect;
		ir.bottom = ir.top + iY * 2 - iSpace;
		font.CreatePointFont(120,_T("Arial"),pDC);
		PrintTitleRectPortrat(pDC,ir,&font);
		pr = rect;
		pr.top = ir.bottom + iSpace;
		font.DeleteObject();

		if (IsDibWindow() || (IsDisplayWindow() && GetNrDibs()==1))
		{
			int h = PrintPicture(pDC,pr);
			CFont font;
			ir = rect;
			ir.top = pr.top + h + iSpace;
			font.CreatePointFont(DW_INFO_FONT_SIZE,_T("Arial"),pDC);
			PrintInfoRectPortrat(pDC,ir,&font,iSpace,
								 FormatPicture(TRUE),
								 FormatData(_T("\n"),_T(":\t")),
								 FormatComment());
			font.DeleteObject();
		}
		else
		{
			PrintDIBs(pDC,pr,iSpace);
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
void CImageWindow::PrintTitleRectPortrat(CDC* pDC,CRect rect, CFont* pFont)
{
	CFont* pFontOld;
	CString sTitle;
	CSize size;

	sTitle = theApp.GetStationName();
	if (sTitle.IsEmpty())
	{
		sTitle = COemGuiApi::GetProducerName();
	}

	pDC->Rectangle(rect);
	pFontOld = pDC->SelectObject(pFont);
	size = pDC->GetOutputTextExtent(sTitle);
	pDC->TextOut(rect.left+(rect.Width()-size.cx)/2,
				 rect.top+(rect.Height()-size.cy)/2,
				 sTitle);
	pDC->SelectObject(pFontOld);
}
/////////////////////////////////////////////////////////////////////////////
void CImageWindow::PrintInfoRectPortrat(CDC* pDC,CRect rect, 
									  CFont* pFont, int iSpace,
									  const CString& sPicture,
									  const CString& sData,
									  const CString& sComment)
{
	CFont* pFontOld;
	int h;
	CRect r;
	
	pFontOld = pDC->SelectObject(pFont);

	r = rect;
	r.right = rect.left + (rect.Width()- iSpace)/2;
	pDC->Rectangle(r);
	r.DeflateRect(iSpace,iSpace);
	h = pDC->DrawText(sPicture,r,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);
	r.top += h+iSpace;
	pDC->DrawText(sData,r,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);

	r = rect;
	r.left = rect.left + (rect.Width() + iSpace)/2;
	pDC->Rectangle(r);
	r.DeflateRect(iSpace,iSpace);
	pDC->DrawText(sComment,r,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);

	pDC->SelectObject(pFontOld);
}
