// ETSPaintView.cpp : Implementierung der Klasse CETSPaintView
//

#include "stdafx.h"
#include "ETSPaint.h"

#include "ETSPaintDoc.h"
#include "ETSPaintView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DRAW_BLACK_AND_WHITE 0x00000001

/////////////////////////////////////////////////////////////////////////////
// CETSPaintView

IMPLEMENT_DYNCREATE(CETSPaintView, CScrollView)

BEGIN_MESSAGE_MAP(CETSPaintView, CScrollView)
	//{{AFX_MSG_MAP(CETSPaintView)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_VIEW_NORMAL, OnViewNormal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NORMAL, OnUpdateViewNormal)
	ON_COMMAND(ID_VIEW_400_PERCENT, OnView400Percent)
	ON_COMMAND(ID_VIEW_200_PERCENT, OnView200Percent)
	ON_COMMAND(ID_VIEW_STRETCHED, OnViewStretched)
	ON_COMMAND(ID_VIEW_ZOOM, OnViewZoom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOM, OnUpdateViewZoom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_200_PERCENT, OnUpdateView200Percent)
	ON_UPDATE_COMMAND_UI(ID_VIEW_400_PERCENT, OnUpdateView400Percent)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STRETCHED, OnUpdateViewStretched)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_VIEW_BW, OnViewBw)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BW, OnUpdateViewBw)
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CETSPaintView Konstruktion/Destruktion

CETSPaintView::CETSPaintView()
{
   m_nViewFactor = 100;
   m_cLeftColor  = RGB(255,0  ,0  );
   m_cRightColor = RGB(255,255,255);
   m_dwDrawFlags = 0;
}

CETSPaintView::~CETSPaintView()
{
}

BOOL CETSPaintView::PreCreateWindow(CREATESTRUCT& cs)
{

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CETSPaintView Zeichnen

void CETSPaintView::OnDraw(CDC* pDC)
{
	CETSPaintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   const BITMAPINFO *pBi = pDoc->m_Dibsection.GetBitmapInfo();
   if (pBi)
   {
      RECT rc = {0, 0, m_sizeTotal.cx, m_sizeTotal.cy};
      if (m_nViewFactor == 100)
      {
//         pDoc->m_Dibsection.BitBlt(pDC->m_hDC, 0, 0, m_sizeTotal.cx, m_sizeTotal.cy, SRCCOPY);
         pDoc->m_Dibsection.SetDIBitsToDevice(pDC->m_hDC, rc, (m_dwDrawFlags&DRAW_BLACK_AND_WHITE)? true : false); 
      }
      else
      {
         pDC->SaveDC();
         if (m_nViewFactor < 100)
            pDC->SetStretchBltMode(HALFTONE);
//         pDoc->m_Dibsection.StretchBlt(pDC->m_hDC, 0, 0, m_sizeTotal.cx, m_sizeTotal.cy, SRCCOPY);
         pDoc->m_Dibsection.StretchDIBits(pDC->m_hDC, rc, (m_dwDrawFlags&DRAW_BLACK_AND_WHITE)? true : false);
         pDC->RestoreDC(-1);
      }
   }
}

void CETSPaintView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
   OnUpdate(NULL, VIEW_SIZE, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CETSPaintView Drucken

BOOL CETSPaintView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CETSPaintView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CETSPaintView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CETSPaintView Diagnose

#ifdef _DEBUG
void CETSPaintView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CETSPaintView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CETSPaintDoc* CETSPaintView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CETSPaintDoc)));
	return (CETSPaintDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CETSPaintView Nachrichten-Handler

BOOL CETSPaintView::OnEraseBkgnd(CDC* pDC) 
{
	CETSPaintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   const BITMAPINFO *pBi = pDoc->m_Dibsection.GetBitmapInfo();
   if (pBi)
   {
      CBrush brush;
      brush.CreateSolidBrush(RGB(255,255,255));
      HRGN hrgn1 = ::CreateRectRgn(0, 0, 0, 0);
      HRGN hrgn2 = ::CreateRectRgn(0, 0, pBi->bmiHeader.biWidth, pBi->bmiHeader.biHeight);
      HRGN hrgn3 = ::CreateRectRgn(0, 0, m_sizeTotal.cx, m_sizeTotal.cy);
      ::CombineRgn(hrgn1, hrgn2, hrgn3, RGN_DIFF);
      ::DeleteObject(hrgn2);
      ::DeleteObject(hrgn3);
      pDC->FillRgn(CRgn::FromHandle(hrgn1), &brush);
      ::DeleteObject(hrgn1);
      return 0;
   }
	return CScrollView::OnEraseBkgnd(pDC);
}

void CETSPaintView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
   if (lHint & VIEW_SIZE)
   {
	   CETSPaintDoc* pDoc = GetDocument();
	   ASSERT_VALID(pDoc);
      if (pDoc->m_Dibsection.GetBitmapInfo())
      {
         m_sizeTotal.cx = MulDiv(pDoc->m_Dibsection.GetWidth() , m_nViewFactor, 100);
         m_sizeTotal.cy = MulDiv(pDoc->m_Dibsection.GetHeight(), m_nViewFactor, 100);
      }
      else m_sizeTotal.cx = m_sizeTotal.cy = 100;
	   SetScrollSizes(MM_TEXT, m_sizeTotal);
      InvalidateRect(NULL);
   }
}

void CETSPaintView::OnViewStretched() 
{
	m_nViewFactor = 0;
   OnUpdate(NULL, VIEW_SIZE, NULL);
}

void CETSPaintView::OnViewNormal() 
{
	m_nViewFactor = 100;
   OnUpdate(NULL, VIEW_SIZE, NULL);
}

void CETSPaintView::OnView200Percent() 
{
	m_nViewFactor = 200;
   OnUpdate(NULL, VIEW_SIZE, NULL);
}

void CETSPaintView::OnView400Percent()
{
	m_nViewFactor = 400;
   OnUpdate(NULL, VIEW_SIZE, NULL);
}

void CETSPaintView::OnViewZoom()
{
	m_nViewFactor = 800;
   OnUpdate(NULL, VIEW_SIZE, NULL);
}

void CETSPaintView::OnViewBw()
{
   if (m_dwDrawFlags & DRAW_BLACK_AND_WHITE)
      m_dwDrawFlags &= ~DRAW_BLACK_AND_WHITE;
   else
   {
	   CETSPaintDoc* pDoc = GetDocument();
	   ASSERT_VALID(pDoc);
      const BITMAPINFO *pBi = pDoc->m_Dibsection.GetBitmapInfo();
      if (pBi)
      {
         pDoc->m_Dibsection.CreateBmpBW(256, pBi->bmiHeader.biWidth, pBi->bmiHeader.biHeight, NULL);
      }
      m_dwDrawFlags |= DRAW_BLACK_AND_WHITE;
   }
   InvalidateRect(NULL);
}

void CETSPaintView::OnSize(UINT nType, int cx, int cy) 
{
   if (m_nViewFactor == 0)
   {
      m_sizeTotal.cx = cx;
      m_sizeTotal.cy = cy;
	   SetScrollSizes(MM_TEXT, m_sizeTotal);
   }
	CScrollView::OnSize(nType, cx, cy);
}

void CETSPaintView::OnUpdateViewStretched(CCmdUI* pCmdUI) {pCmdUI->SetRadio(m_nViewFactor == 0);}
void CETSPaintView::OnUpdateViewNormal(CCmdUI* pCmdUI)    {pCmdUI->SetRadio(m_nViewFactor == 100);}
void CETSPaintView::OnUpdateView200Percent(CCmdUI* pCmdUI){pCmdUI->SetRadio(m_nViewFactor == 200);}
void CETSPaintView::OnUpdateView400Percent(CCmdUI* pCmdUI){pCmdUI->SetRadio(m_nViewFactor == 400);}
void CETSPaintView::OnUpdateViewZoom(CCmdUI* pCmdUI) 
{
   BOOL bRadio = true;
   switch (m_nViewFactor)
   {
      case 100: case 200: case 400: case 0: bRadio = false;
   }
  	pCmdUI->SetRadio(bRadio);
}

void CETSPaintView::OnUpdateViewBw(CCmdUI* pCmdUI) {pCmdUI->SetCheck((m_dwDrawFlags & DRAW_BLACK_AND_WHITE) ? 1 : 0);}

void CETSPaintView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_StartPoint = point;
	CETSPaintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   const BITMAPINFO *pBi = pDoc->m_Dibsection.GetBitmapInfo();
   if (pBi)
   {
      CPoint pixel = PixelCoord(&point, pBi);
      pDoc->m_Dibsection.SetPixel(pixel.x, pixel.y, m_cLeftColor);
      InvalidatePart(point);
   }

	CScrollView::OnLButtonDown(nFlags, point);
}

void CETSPaintView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CScrollView::OnLButtonUp(nFlags, point);
}

void CETSPaintView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_StartPoint = point;
	CETSPaintDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
   const BITMAPINFO *pBi = pDoc->m_Dibsection.GetBitmapInfo();
   if (pBi)
   {
      CPoint pixel = PixelCoord(&point, pBi);
      pDoc->m_Dibsection.SetPixel(pixel.x, pixel.y, m_cRightColor);
      InvalidatePart(point);
   }
	
	CScrollView::OnRButtonDown(nFlags, point);
}

CPoint CETSPaintView::PixelCoord(CPoint *pPoint, const BITMAPINFO *pBi)
{
   CPoint pixel;
   if      (m_nViewFactor == 100) pixel = *pPoint;
   else if (m_nViewFactor ==   0) ;
   else
   {
      pixel.x = MulDiv(pPoint->x, 100, m_nViewFactor);
      pixel.y = MulDiv(pPoint->y, 100, m_nViewFactor);
   }
   pixel.y = pBi->bmiHeader.biHeight-pixel.y;
   return pixel;
}

void CETSPaintView::InvalidatePart(CPoint point)
{
   int nInflate = MulDiv(2, m_nViewFactor, 100);
   CRect rc(point.x, point.y, point.x, point.y);
   rc.InflateRect(nInflate, nInflate);
   InvalidateRect(&rc);
}

void CETSPaintView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	
	CScrollView::OnRButtonUp(nFlags, point);
}

void CETSPaintView::OnMouseMove(UINT nFlags, CPoint point) 
{
	
	CScrollView::OnMouseMove(nFlags, point);
}



