// CaraView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CaraView.h"
#include "CaraWinApp.h"
#include "CaraDoc.h"
#include "resource.h"
#include "CaraPreview.h"
#include "TextLabel.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaraView

IMPLEMENT_DYNCREATE(CCaraView, CView)

CCaraView::CCaraView()
{
   m_nMapMode = MM_LOMETRIC;
   m_nCurrentPage = -1;
   m_nPages       =  1;
}

CCaraView::~CCaraView()
{
}

inline CCARADoc* CCaraView::GetDocument()
{
   return (CCARADoc*) m_pDocument;
}

BEGIN_MESSAGE_MAP(CCaraView, CScrollView)
	//{{AFX_MSG_MAP(CCaraView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CCaraView 

void CCaraView::OnDraw(CDC* pDC)
{
   CCARADoc *pDoc = (CCARADoc*)GetDocument();
   CLabelContainer *pDocLabels = pDoc->GetLabelContainer();

   int nSave = pDC->SaveDC();
   if (m_nCurrentPage == -1)
   {
      pDocLabels->Draw(pDC);
   }
   else
   {
      CLabel *pl = pDocLabels->GetLabel(m_nCurrentPage-1);
      if (pl)
      {
         pl->Draw(pDC);
         TRACE("Draw page %d\n", m_nCurrentPage);
      }
   }
   if (pDC->IsPrinting() && pDoc->m_pCaraLogo) pDoc->m_pCaraLogo->Draw(pDC);
   pDC->RestoreDC(nSave);
}

/////////////////////////////////////////////////////////////////////////////
// Diagnose CCaraView

#ifdef _DEBUG
void CCaraView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CCaraView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCaraView 

void CCaraView::OnFilePrintPreview()
{
   if (((CCaraWinApp*)AfxGetApp())->GetPrinter())
   {
      TRACE("OnFilePrintPreview()\n");
      CPrintPreviewState* pState = new CPrintPreviewState;

      if (DoPrintPreview(IDD_PREVIEW_DLG_BAR, this, RUNTIME_CLASS(CCARAPreview), pState))
      {
         CFrameWnd* pParent = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
         if (pParent)
         {
            pParent->m_hAccelTable = NULL;
            pParent->LoadAccelTable(MAKEINTRESOURCE(IDR_PREVIEWACCELERATORS));
         }
         return ;
      }
      else
      {
         TRACE0("Error: DoPrintPreview failed.\n");
         AfxMessageBox(AFX_IDP_COMMAND_FAILURE|MB_ICONINFORMATION);
         delete pState;      // preview failed to initialize, delete State now
      }
   }
   return ;
}

BOOL CCaraView::OnPreparePrinting(CPrintInfo* pInfo) 
{
   pInfo->SetMaxPage(m_nPages);
   if (m_nPages > 1)
   {
      m_nCurrentPage = 0;
   }

   ASSERT(pInfo != NULL);
   ASSERT(pInfo->m_pPD != NULL);

   if (pInfo->m_pPD->m_pd.nMinPage > pInfo->m_pPD->m_pd.nMaxPage)
      pInfo->m_pPD->m_pd.nMaxPage = pInfo->m_pPD->m_pd.nMinPage;

   CCaraWinApp* pApp = (CCaraWinApp*) AfxGetApp();

   if (pInfo->m_bPreview || pInfo->m_bDirect || (pInfo->m_bDocObject && !(pInfo->m_dwFlags & 2)))
   {
      if (pInfo->m_bDirect)
      {
         if (pInfo->m_pPD->m_pd.hDC != NULL)
         {
            ::DeleteDC(pInfo->m_pPD->m_pd.hDC);
            pInfo->m_pPD->m_pd.hDC = NULL;
         }

         if (pApp->m_pPrtD == NULL)
         {
            pApp->GetCaraPrinterDefaults(pInfo);
         }
      }
      if (pInfo->m_pPD->m_pd.hDC == NULL)
      {
         if (!pApp->GetCaraPrinterDefaults(pInfo))
         {
            return false;
         }

         if (pInfo->m_pPD->m_pd.hDC == NULL)
         {
            if ((pInfo->m_pPD->m_pd.hDC = pApp->CreateCaraPrinterDC((pInfo->m_bPreview!=0)))== NULL)
               return FALSE;
         }
      }
      pInfo->m_pPD->m_pd.nFromPage = (WORD)pInfo->GetMinPage();
      pInfo->m_pPD->m_pd.nToPage = (WORD)pInfo->GetMaxPage();
   }
   else
   {
      pInfo->m_pPD->m_pd.nFromPage = (WORD)pInfo->GetMinPage();
      pInfo->m_pPD->m_pd.nToPage = (WORD)pInfo->GetMaxPage();
      if (pApp->DoCaraPrintDialog() == IDOK)
      {
         pApp->GetCaraPrinterDefaults(pInfo);
         if (pInfo->m_pPD->m_pd.hDC == NULL)
         {
            if ((pInfo->m_pPD->m_pd.hDC = pApp->CreateCaraPrinterDC())== NULL)
               return FALSE;
         }
      }
      else
      {
         return FALSE;
      }
   }

   ASSERT(pInfo->m_pPD != NULL);
   ASSERT(pInfo->m_pPD->m_pd.hDC != NULL);
   if (pInfo->m_pPD->m_pd.hDC == NULL) return FALSE;

   pInfo->m_nNumPreviewPages = pApp->m_nNumPreviewPages;
   return TRUE;
}

void CCaraView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
   if (pInfo)
   {
      CCARADoc   *pDoc = (CCARADoc*)    GetDocument();
      CCaraWinApp*pApp = (CCaraWinApp*) AfxGetApp();
      if (pInfo->m_bDocObject)
      {
         pDC->Attach(pInfo->m_pPD->m_pd.hDC);
		   pDC->m_bPrinting = TRUE;
      }

      SMarginDlgParam *pMdp = (SMarginDlgParam*)pInfo->m_lpUserData;
      ASSERT(pMdp!=NULL);

      pInfo->m_rectDraw = pMdp->rcPageLoMetric;

      OnPrepareDC(pDC, pInfo);
      CLabelContainer *plc = pDoc->GetLabelContainer();

      if (pMdp->nScaleLabel&SCALE_LABEL)
      {
         plc->MoveLabel(-((CRect*)&pMdp->rcOldDrawLoMetric)->CenterPoint());
         plc->ScaleLabel(CSize(pMdp->rcDrawLoMetric.right     - pMdp->rcDrawLoMetric.left,
                               pMdp->rcDrawLoMetric.bottom    - pMdp->rcDrawLoMetric.top),
                         CSize(pMdp->rcOldDrawLoMetric.right  - pMdp->rcOldDrawLoMetric.left,
                               pMdp->rcOldDrawLoMetric.bottom - pMdp->rcOldDrawLoMetric.top));
         plc->MoveLabel(((CRect*)&pMdp->rcDrawLoMetric)->CenterPoint());
         pMdp->nScaleLabel &= ~SCALE_LABEL;                    // nicht noch einmal skalieren
      }

      ProcessLabel pl = {NULL, 0, pDC, &pMdp->rcDrawLoMetric};
      plc->ProcessWithLabels(RUNTIME_CLASS(CTextLabel), &pl, CTextLabel::ClipTextRect);
      pl.nPos = 0;
      pl.pl   = NULL;
      plc->ProcessWithLabels(NULL, &pl, CLabelContainer::ClipLabel);

      pDoc->CreateCaraLogo();

      if (pDoc->m_pCaraLogo)
      {
         SMarginDlgParam *pMdp = (SMarginDlgParam*)pInfo->m_lpUserData;
         ASSERT(pMdp!=NULL);
         CRect rcLogo = pDoc->m_pCaraLogo->GetRect(pDC);
         CSize szLogo = rcLogo.Size();
         CSize szWnd  = pDC->GetViewportExt();
         if ((szLogo.cy > 0) && (szWnd.cy < 0))
         {
            swap(rcLogo.top, rcLogo.bottom);
         }
         CPoint ptShift(pMdp->rcDrawLoMetric.right - rcLogo.right-4, pMdp->rcDrawLoMetric.top - rcLogo.top-4);
         rcLogo.OffsetRect(ptShift);
         pDoc->m_pCaraLogo->SetRect(rcLogo);
      }
   }
   CScrollView::OnBeginPrinting(pDC, pInfo);
}

void CCaraView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
   pDC->SetMapMode(m_nMapMode);
    
   if (pInfo && !pInfo->m_bPreview)
   {
      SMarginDlgParam *pMdp = (SMarginDlgParam*)pInfo->m_lpUserData;
      if (pMdp)
      {
         if ((pMdp->OffsetDev.x == 0) && (pMdp->OffsetDev.y == 0))
         {
            TRACE("P_DC-Offset : (%d, %d)\n", pMdp->OffsetLoMetric.x, pMdp->OffsetLoMetric.y);
            pMdp->OffsetDev = pMdp->OffsetLoMetric;
            pDC->LPtoDP(&pMdp->OffsetDev);
            pMdp->OffsetDev.y = -pMdp->OffsetDev.y;
         }
         pDC->SetViewportOrg(-pMdp->OffsetDev.x, -pMdp->OffsetDev.y);
      }
      CView::OnPrepareDC(pDC, pInfo);
   }
   else CScrollView::OnPrepareDC(pDC, pInfo);
}

void CCaraView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
/*
   SMarginDlgParam *pMdp = (SMarginDlgParam*)pInfo->m_lpUserData;
   pDC->SelectStockObject(HOLLOW_BRUSH);
   pDC->SelectStockObject(BLACK_PEN);
   CLabel::Rectangle(pDC, &pMdp->rcDrawLoMetric);
*/
   bool bFillPath      = CLabel::gm_bFillPath;
   bool bColorMatching = CLabel::gm_bColorMatching;
   if (!pInfo->m_bPreview)
   {
      CLabel::gm_bFillPath      = true;
      CLabel::gm_bColorMatching = false;
   }
   CScrollView::OnPrint(pDC, pInfo);
   CLabel::gm_bFillPath      = bFillPath;
   CLabel::gm_bColorMatching = bColorMatching;
}

void CCaraView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
   if (pInfo->m_pPD->m_pd.hDC)
   {
      CCaraWinApp *pApp = (CCaraWinApp*) AfxGetApp();
      pApp->EndPrinting();
      ASSERT(pDC->m_hAttribDC == pInfo->m_pPD->m_pd.hDC);
      pInfo->m_pPD->m_pd.hDC = NULL;
      pDC->Detach();
   }
   CScrollView::OnEndPrinting(pDC, pInfo);
}

void CCaraView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView) 
{
   GetDocument()->SetTitle(NULL);
   if (pView->IsKindOf(RUNTIME_CLASS(CCARAPreview)))
   {
      ((CCARAPreview*)pView)->OnDeactivateView();
   }
	CScrollView::OnEndPrintPreview(pDC, pInfo, point, pView);
}

CSize CCaraView::GetScrollBarSize()
{
   CSize szB(::GetSystemMetrics(SM_CXVSCROLL), ::GetSystemMetrics(SM_CYHSCROLL));
   return szB;
}

CSize CCaraView::GetScreenSize()
{
   CSize szB(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
   return szB;
}
