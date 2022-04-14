// WndImageRecherche.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "ViewIdipClient.h"
#include "WndImageRecherche.h"
#include "DlgNavigation.h"
#include "DlgField.h"
#include "QueryRectTracker.h"
#include "Mainfrm.h"

#include "WndDib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CWndImageRecherche, CWndImage)

/////////////////////////////////////////////////////////////////////////////
// CWndImageRecherche
CWndImageRecherche::CWndImageRecherche(CServer* pServer)
	: CWndImage(pServer)
{
	m_pDlgField = NULL;
	m_pDlgNavigation = NULL;
}

CWndImageRecherche::~CWndImageRecherche()
{
}


BEGIN_MESSAGE_MAP(CWndImageRecherche, CWndImage)
	//{{AFX_MSG_MAP(CWndImageRecherche)
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CWndImageRecherche::IsImageWindow()
{
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CWndImageRecherche message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CWndImageRecherche::CanShowImageInOriginalSize()
{
	//kann das Originalbild in voller Größe angezeigt werden?
	BOOL bRet = FALSE;

	CRect rcFrame;
	GetFrameRect(rcFrame);
	CSize sz = GetPictureSize();

	if(sz.cx > 0 && sz.cy > 0)
	{
		if(rcFrame.Width() >= sz.cx && rcFrame.Height() >= sz.cy)
		{
			bRet = TRUE;
		}
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CWndImageRecherche::OnSize(UINT nType, int cx, int cy) 
{
	if (cx == 0 && cy == 0)
	{
		CRect rc;
		GetClientRect(&rc);
		cx = rc.Width();
		cy = rc.Height();
	}
	CWndImage::OnSize(nType, cx, cy);

	
	BOOL bFieldsVisible = (cx > FIELD_WIDTH*2);
	if (!bFieldsVisible)
	{
		CAutoCritSec acs(&m_csPictureRecord);
		if (m_pPictureRecord == NULL && m_pAudioRecord != NULL)
		{
			bFieldsVisible = TRUE;
		}
	}
	
	EnableDlgFields(bFieldsVisible);

	if (m_pDlgNavigation)
	{
		m_pDlgNavigation->MoveWindow(0 , cy-NAVIGATION_HEIGHT,
									 cx, NAVIGATION_HEIGHT, TRUE);
	}
	if (   m_pDlgField
		&& bFieldsVisible)
	{
		m_pDlgField->MoveWindow(0, m_wndToolBar.GetSize().cy, FIELD_WIDTH,	//cx-FIELD_WIDTH,
								   cy-NAVIGATION_HEIGHT-m_wndToolBar.GetSize().cy,
								   TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CWndImageRecherche::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWndImage::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pDlgNavigation = new CDlgNavigation(this);
	m_pDlgField = new CDlgField(this);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndImageRecherche::UpdateDialogs()
{
	if (   m_pDlgField
		&& m_pDlgField->IsWindowVisible())
	{
		m_pDlgField->Update();
	}
	if (   m_pDlgNavigation
		&& m_pDlgNavigation->IsWindowVisible())
	{
		m_pDlgNavigation->Update();
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CWndImageRecherche::FormatData(const CString& sD,const CString& sS)
{
	CString result;

	int i,c;
	CString name,value;
	CIPCField* pField;

	m_Fields.Lock();
	c = m_Fields.GetSize();

	for (i=0;i<c;i++)
	{
		pField = m_Fields.GetAtFast(i);
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
	m_Fields.Unlock();

	return result;
}
/////////////////////////////////////////////////////////////////////////////
int CWndImageRecherche::PrintInfoRectLandscape(CDC* pDC,CRect rect, CFont* pFont)
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
void CWndImageRecherche::OnPrint(CDC* pDC, CPrintInfo* pInfo)
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
		font.CreatePointFont(100, GetVarFontFaceName(),pDC);
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
		font.CreatePointFont(120, GetVarFontFaceName(), pDC);
		PrintTitleRectPortrat(pDC,ir,&font);
		pr = rect;
		pr.top = ir.bottom + iSpace;
		font.DeleteObject();

		if (GetType() == WST_DIB || (IsWndPlay() && GetNrDibs()==1))
		{
			int h = PrintPicture(pDC,pr);
			CFont font;
			ir = rect;
			ir.top = pr.top + h + iSpace;
			font.CreatePointFont(DW_INFO_FONT_SIZE, GetVarFontFaceName(), pDC);
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
void CWndImageRecherche::PrintTitleRectPortrat(CDC* pDC,CRect rect, CFont* pFont)
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
void CWndImageRecherche::PrintInfoRectPortrat(CDC* pDC,CRect rect, 
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

	DRAWTEXTPARAMS dtP = {sizeof(DRAWTEXTPARAMS), 6, 0, 0, 0};
	h = pDC->DrawTextEx(InsertSpacesForTabbedOutput(sPicture),r,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS|DT_TABSTOP, &dtP);
	r.top += h+iSpace;
	pDC->DrawTextEx(InsertSpacesForTabbedOutput(sData),r,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS|DT_TABSTOP, &dtP);

	r = rect;
	r.left = rect.left + (rect.Width() + iSpace)/2;
	pDC->Rectangle(r);
	r.DeflateRect(iSpace,iSpace);
	pDC->DrawTextEx(sComment,r,DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS|DT_TABSTOP, &dtP);

	pDC->SelectObject(pFontOld);
}
/////////////////////////////////////////////////////////////////////////////
void CWndImageRecherche::GetFrameRect(LPRECT lpRect)
{
	CWndSmall::GetFrameRect(lpRect);
	if (m_pDlgField && m_pDlgField->IsWindowVisible())
	{
		lpRect->left += FIELD_WIDTH;
	}
	if (m_pDlgNavigation && m_pDlgNavigation->IsWindowVisible())
	{
		lpRect->bottom -= NAVIGATION_HEIGHT;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndImageRecherche::EnableDlgFields(BOOL bEnable)
{
	if (m_pDlgField)
	{
		//DlgField bei Archivfenstern anzeigen oder verstecken
		if (!bEnable)
		{
			m_pDlgField->ShowWindow(SW_HIDE);
			m_pDlgNavigation->ShowWindow(SW_HIDE);
		}
		else if (!m_pDlgField->IsWindowVisible() && bEnable)
		{
			m_pDlgField->Update();
			m_pDlgField->ShowWindow(SW_SHOW);
			m_pDlgNavigation->Update();
			m_pDlgNavigation->ShowWindow(SW_SHOW);
		}

		//DlgField Slider bei Sync Wiedergabe verstecken bzw. wieder anzeigen
		else if(   ((CMainFrame*)theApp.GetMainFrame())->IsSyncMode()
				|| bEnable)
		{
			//Slider wird in Update versteckt
			m_pDlgNavigation->Update();
		}
	}
}
void CWndImageRecherche::EnableNavigationSlider(BOOL bEnable)
{
	if (m_pDlgNavigation)
	{
		m_pDlgNavigation->EnableSlider(bEnable);
	}
}
