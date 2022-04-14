// BitmapTextButton.cpp : implementation file
//

#include "stdafx.h"
#include "securitylauncher.h"
#include "BitmapTextButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapTextButton

CBitmapTextButton::CBitmapTextButton()
{
}

CBitmapTextButton::~CBitmapTextButton()
{
}


BEGIN_MESSAGE_MAP(CBitmapTextButton, CButton)
	//{{AFX_MSG_MAP(CBitmapTextButton)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapTextButton message handlers

void CBitmapTextButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	ASSERT(lpDIS != NULL);

	// draw the whole button
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	BOOL b = lpDIS->itemState & ODS_SELECTED;
	CRect rect;
	CPen white,high,black,face,shadow;
	CPen* pOldPen;
	rect.CopyRect(&lpDIS->rcItem);

	white.CreatePen(PS_SOLID,1,RGB(255,255,255));
	high.CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DLIGHT));
	black.CreatePen(PS_SOLID,1,RGB(0,0,0));
	face.CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DFACE));
	shadow.CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));

	pOldPen = pDC->SelectObject(b ? &black : &white);
	pDC->MoveTo(rect.left,rect.bottom-1);
	pDC->LineTo(rect.left,rect.top);
	pDC->LineTo(rect.right,rect.top);

	pDC->SelectObject(b ? &white : &black);
	pDC->MoveTo(rect.left,rect.bottom-1);
	pDC->LineTo(rect.right-1,rect.bottom-1);
	pDC->LineTo(rect.right-1,rect.top);

	rect.DeflateRect(1,1);

	pDC->SelectObject(b ? &shadow : &high);
	pDC->MoveTo(rect.left,rect.bottom-1);
	pDC->LineTo(rect.left,rect.top);
	pDC->LineTo(rect.right,rect.top);

	pDC->SelectObject(b ? &high : &shadow);
	pDC->MoveTo(rect.left,rect.bottom-1);
	pDC->LineTo(rect.right-1,rect.bottom-1);
	pDC->LineTo(rect.right-1,rect.top);


	white.DeleteObject();
	high.DeleteObject();
	black.DeleteObject();
	face.DeleteObject();
	shadow.DeleteObject();

	pDC->SelectObject(pOldPen);

	rect.DeflateRect(1,1);

	CBrush bface;
	
	bface.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	pDC->FillRect(rect,&bface);
	face.DeleteObject();

	int off = b ? 2 : 0; 
	CPoint pt(rect.left+off,rect.top+off);
	m_Images.Draw(pDC,theApp.m_pDongle->GetOemCode()-1,pt,ILD_TRANSPARENT);

	CString t;
	GetWindowText(t);
	CSize z = pDC->GetOutputTextExtent(t);
	int x,y;

	rect.left += 16;
	x = (rect.Width()-z.cx) / 2 + off;
	y = (rect.Height()-z.cy) / 2 + off;
	int iBkMode = pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(x+18,y+2,t);
	pDC->SetBkMode(iBkMode);
}

int CBitmapTextButton::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_Images.Create(IDB_BUTTON_SYMBOLS,16,0,RGB(128,255,255));

	return 0;
}
