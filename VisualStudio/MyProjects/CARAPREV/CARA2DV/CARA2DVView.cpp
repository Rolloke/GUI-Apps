// CARA2DVView.cpp : Implementierung der Klasse CCARA2DVView
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "CARA2DVDoc.h"
#include "CARA2DVView.h"
#include "MainFrm.h"
#include "CEtsHelp.h"
#include "..\Resource.h"
#include "..\PictureLabel.h"
#include "..\CaraMenu.h"
#include "..\ListDlg.h"

#include "ETS3DGLRegKeys.h"
#include "Dibsection.h"

#include <conio.h>
#include <process.h>

#include "Etsbind.h"

#ifdef ETS_OLE_SERVER
#include "IpFrame.h"
#include "SrvrItem.h"
#endif

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"
#include ".\cara2dvview.h"

#define MOUSE_HOVERTIMER 20            // Timer ID
#define MOUSE_HOVERTIME  200           // Wiederholungszeit in ms
#define SET_TOOLTIP       5            // Tooltip setzen nach   5 * 200 ms = 1 s
#define END_TOOLTIP      25            // Tooltip beenden nach 25 = 200 ms = 5 s

//#define OLE_DOC_UPDATE_TIMER   40      // Timer ID
//#define OLE_DOC_UPDATE_TIME  5000      // Zeit zum Updaten 5 s

#define PICKSIZE         3

#define TILE_VERTICAL    0x00010000
#define TILE_HORIZONTAL  0x00020000
#define TILE_BY_MOUSE    0x00040000

#define DO_RENDER          0x00000001
#define CAN_RENDER         0x00000002
#define NEW_RENDER         0x00000004
#define RENDERING_NOW      0x00000008
#define CRITICAL_SECT_INIT 0x00000010
#define USE_FIRST_DC       0x00000040

#define REGKEY_DEFAULT_BKGND_BMP "BkGndBmp"

#define IDC_SET_CURVE_VISIBLE           1302

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVView

IMPLEMENT_DYNCREATE(CCARA2DVView, CCaraView)

BEGIN_MESSAGE_MAP(CCARA2DVView, CCaraView)
	//{{AFX_MSG_MAP(CCARA2DVView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FUNC_DEFAULTRANGE, OnDefaultrange)
	ON_COMMAND(ID_FUNC_REZOOM, OnRezoom)
	ON_COMMAND(ID_FUNC_ZOOM, OnZoom)
	ON_COMMAND(ID_FUNC_REFERENCE_SELECT, OnReferenceSelect)
	ON_COMMAND(ID_FUNC_ZERO_REFERENCE, OnZeroReference)
	ON_COMMAND(ID_OPTIONS_PLOT, OnOptionsPlot)
	ON_COMMAND(ID_OPTIONS_STANDARD, OnOptionsStandard)
	ON_COMMAND(ID_OPTIONS_SETSTANDARDS, OnOptionsSetstandards)
	ON_COMMAND(ID_OPERATION_AVERAGE, OnOperationAverage)
	ON_COMMAND(ID_OPERATION_SPLINE, OnOperationSpline)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditDelete)
	ON_COMMAND(ID_FUNC_DESTROYRANGELIST, OnDestroyrangelist)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_FUNC_NUMEDITMODE, OnNumeditmode)
	ON_COMMAND(ID_FUNC_LINEEDITMODE, OnLineeditmode)
	ON_COMMAND(ID_FUNC_EDITSTEPMINUS, OnMinus)
	ON_COMMAND(ID_FUNC_EDITSTEPPLUS, OnPlus)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_COMMAND(ID_FUNC_DELETERANGE, OnFuncDeleterange)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_KEYUP()
	ON_COMMAND(ID_OPERATION_DERIVE, OnOperationDerive)
   ON_COMMAND(ID_OPERATION_FFT, OnOperationFFT)
	ON_COMMAND(ID_OPERATION_INTEGRATE, OnOperationIntegrate)
	ON_COMMAND(ID_OPERATION_CREATECURVE, OnOperationCreatecurve)
	ON_COMMAND(ID_OPERATION_FITCURVE, OnOperationFitcurve)
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CCaraView::OnFilePrintPreview)
	ON_COMMAND(ID_FUNC_REZOOM+ID_TB_RIGHT_EXT, OnSelectZoom)
	ON_COMMAND(ID_FUNC_ZOOM+ID_TB_RIGHT_EXT, OnSelectZoom)
   ON_COMMAND_RANGE(ID_VIEW_SET_SIZE_NORMAL, ID_VIEW_SET_SIZE_Y_400_PC, OnViewSetSize)
	ON_COMMAND_RANGE(ID_FUNC_PICKMODE, ID_FUNC_EDITMODE, ChangeMouseMode)
   ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
   ON_MESSAGE(WM_RESETPICKSTATES, OnResetPickStates)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
   ON_COMMAND(IDX_CONTEXT_SET_PLOT_MARKER, OnSetPlotMarker)
   ON_UPDATE_COMMAND_UI(IDX_CONTEXT_SET_PLOT_MARKER, OnUpdateSetPlotMarker)
   ON_COMMAND(ID_CALC_AVERAGE, OnCalcAverage)
   ON_UPDATE_COMMAND_UI(ID_CALC_AVERAGE, OnUpdateCalcAverage)
   ON_COMMAND(ID_CALC_INTEGRAL, OnCalcIntegral)
   ON_UPDATE_COMMAND_UI(ID_CALC_INTEGRAL, OnUpdateCalcIntegral)
   ON_COMMAND(ID_CALC_SLOPE, OnCalcSlope)
   ON_UPDATE_COMMAND_UI(ID_CALC_SLOPE, OnUpdateCalcSlope)
   ON_COMMAND(ID_CALC_ZERO, OnCalcZero)
   ON_UPDATE_COMMAND_UI(ID_CALC_ZERO, OnUpdateCalcZero)
   ON_COMMAND_RANGE(ID_FUNC_RIGHT_ZOOM, ID_FUNC_DOWN_ZOOM, OnFuncPanZoom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVView Konstruktion/Destruktion
extern "C" __declspec(dllimport) HINSTANCE g_hCaraPreviewInstance;

CCARA2DVView::CCARA2DVView()
{
   BEGIN("CCARA2DVView");
   m_pPropSheet        = NULL;
   m_pActualPlot       = NULL;
   m_nMouseMode        = 0;
   m_nMouseFlags       = 0;
   m_OldMousePoint     = CPoint(0,0);
   m_nArrowCursorStep  = 1;
   m_dStepValue        = 0.0;
   m_bKeyControl       = false;
//   m_nMouseHover       = -1;
   m_lTile             = TILE_HORIZONTAL;
   m_ptViewSize.x      = 100;
   m_ptViewSize.y      = 100;
   ChangeCursor(ARROWCURSOR);
   m_pHilightedLabel   = NULL;

   m_hBkGndBmp         = NULL;
   m_hRenderBmp[0]     = NULL;
   m_hRenderBmp[1]     = NULL;
   m_hRenderDC[0]      = NULL;
   m_hRenderDC[1]      = NULL;
   m_hRenderThread     = NULL;
   m_hRenderEvent      = NULL;
   m_nRenderFlag       = 0;
   if (AfxGetApp()->GetProfileInt(REGKEY_DEFAULT, "RenderMode", 0) != 0)
      m_nRenderFlag       = DO_RENDER;


   CString strBkGndBmp = AfxGetApp()->GetProfileString(REGKEY_DEFAULT, REGKEY_DEFAULT_BKGND_BMP);
   if (!strBkGndBmp.IsEmpty())
   {
	   TRY
	   {
         CFile file(strBkGndBmp, CFile::modeRead);
         CDibSection dib;
         if (dib.LoadFromFile((HANDLE)file.m_hFile))
         {
            m_hBkGndBmp = dib.Detach();
         }
      }
	   CATCH_ALL(e)
	   {
      }
   	END_CATCH_ALL
   }
}

CCARA2DVView::~CCARA2DVView()
{
   if (m_pPropSheet)      delete m_pPropSheet;
   if (m_hBkGndBmp) ::DeleteObject(m_hBkGndBmp);
}

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVView Zeichnen
void CCARA2DVView::OnSize(UINT nType, int cx, int cy)
{
#ifdef ETS_OLE_SERVER
   CCaraView::OnSize(nType, cx, cy);
   if ((cx == 0) || (cy == 0)) return;
   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);

   CSize szPick(PICKSIZE, PICKSIZE);
   pDC->DPtoLP(&szPick);
   CLabel::SetPickPointSize(szPick);
   
   ResetPickedLabels();

   CPoint size  = CLabel::PixelToLogical(pDC, cx, cy);

   CCARA2DVApp *pApp = (CCARA2DVApp*) AfxGetApp();
   CCARA2DVDoc *pDoc = GetDocument();
   bool bSetSize = true;
   if (pApp->m_CmdInfo.m_bRunEmbedded)
   {
      if (m_bIPframeRecalcLayOut && pDoc->IsEmbeddedItemValid())
      {
         COleServerSrvrItem *pItem = pDoc->GetEmbeddedItem();
         pItem->m_sizeExtent.cx = cx;
         pItem->m_sizeExtent.cy = cy;
         pDC->DPtoHIMETRIC(&pItem->m_sizeExtent);
         pDoc->SetModifiedFlag(true);
         SetPlotSizes(CRect(0, 0, size.x, size.y), pDC);
      }
      else bSetSize = false;
   }
   else
   {
      CSize szScroll;
      szScroll.cx = abs(MulDiv(size.x, m_ptViewSize.x, 100));
      szScroll.cy = abs(MulDiv(size.y, m_ptViewSize.y, 100));
      SetScrollSizes(m_nMapMode, szScroll);
      ResizePlotsToView();
   }
   ReleaseDC(pDC);

#else

   CCaraView::OnSize(nType, cx, cy);
   if ((cx == 0) || (cy == 0)) return;
   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);

   CSize szPick(PICKSIZE, PICKSIZE);
   pDC->DPtoLP(&szPick);
   CLabel::SetPickPointSize(szPick);
   
   ResetPickedLabels();

   CPoint size  = CLabel::PixelToLogical(pDC, cx, cy);

   CSize szScroll;
   
   szScroll.cx = abs(MulDiv(size.x, m_ptViewSize.x, 100));
   szScroll.cy = abs(MulDiv(size.y, m_ptViewSize.y, 100));
   SetScrollSizes(m_nMapMode, szScroll);
   ResizePlotsToView();

   ReleaseDC(pDC);
#endif
}

BOOL CCARA2DVView::OnEraseBkgnd(CDC* pDC) 
{
   if (m_nRenderFlag & CAN_RENDER) return 1;
   CCARA2DVDoc*     pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   // löschen des Hintergrundes
   CRect clientrect;
   if (pDC->IsKindOf(RUNTIME_CLASS(CPaintDC))) clientrect = ((CPaintDC*)pDC)->m_ps.rcPaint;
   else GetClientRect(&clientrect);

   CBrush bkbrush;
   LOGBRUSH logbrush = {BS_SOLID, ((CCARA2DVApp*)AfxGetApp())->GetBackgroundColor(), 0};
   if (m_hBkGndBmp)
   {
      logbrush.lbStyle = BS_PATTERN;
      logbrush.lbHatch = (long)m_hBkGndBmp;
   }
   bkbrush.CreateBrushIndirect(&logbrush);
   pDC->SaveDC();
   pDC->SelectObject(bkbrush);
   pDC->PatBlt(clientrect.left, clientrect.top, clientrect.Width(), clientrect.Height(), PATCOPY);
   // Updaten der Labels falls nötig
   OnPrepareDC(pDC);

   pDoc->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), pDC, UpdateChanges);
   pDC->RestoreDC(-1);

   return true;   
}

int CCARA2DVView::UpdateChanges(CLabel *pl, void *pParam)
{
   if (pl && pParam && pl->IsVisible()) ((CPlotLabel*)pl)->UpdateChanges((CDC*)pParam);
   return 0;
}

void CCARA2DVView::OnDraw(CDC* pDC)
{
   CCARA2DVDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);

   if (pDC->IsPrinting())
   {
      CCaraView::OnDraw(pDC);
   }
   else
   {
      if (m_nRenderFlag & CAN_RENDER)
      {
         ::EnterCriticalSection(&m_csRenderDC);

         ASSERT_KINDOF(CPaintDC, pDC);
         CPaintDC *pP = (CPaintDC*) pDC;

         int nDC = (m_nRenderFlag & USE_FIRST_DC) ? 1 : 0;
         pDC->SetMapMode(MM_TEXT);
         ::BitBlt(pDC->m_hDC,
                     pP->m_ps.rcPaint.left, pP->m_ps.rcPaint.top,
                     pP->m_ps.rcPaint.right-pP->m_ps.rcPaint.left, 
                     pP->m_ps.rcPaint.bottom-pP->m_ps.rcPaint.top,
                     m_hRenderDC[nDC],
                     pP->m_ps.rcPaint.left, pP->m_ps.rcPaint.top,
                     SRCCOPY);

         ::LeaveCriticalSection(&m_csRenderDC);
         OnPrepareDC(pDC);
         if (m_pActualPlot)
         {
            m_pActualPlot->SetShapeDrawn(false);
            m_pActualPlot->DrawShape(pDC, true);
         }
         if (m_PickList.GetCount())
         {
            m_PickList.SetShapeDrawn(false);
            m_PickList.DrawShape(pDC, true);
         }
      }
      else
      {
         pDC->SaveDC();
         pDoc->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), pDC, CCARA2DVView::DrawLabel);
         pDC->RestoreDC(-1);
      }
   }
}

int CCARA2DVView::DrawLabel(CLabel *pl, void *p)
{
   pl->Draw((CDC*)p);
   return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CCARA2DVView Drucken
void CCARA2DVView::OnFilePrint()
{
	CPrintInfo printInfo;
	ASSERT(printInfo.m_pPD != NULL);    // must be set
	if (LOWORD(GetCurrentMessage()->wParam) == ID_FILE_PRINT_DIRECT)
	{
		CCommandLineInfo* pCmdInfo = AfxGetApp()->m_pCmdInfo;

		if (pCmdInfo != NULL)
		{
			if (pCmdInfo->m_nShellCommand == CCommandLineInfo::FilePrintTo)
			{
            // DC initialisieren
			}
		}

		printInfo.m_bDirect = TRUE;
	}

	if (OnPreparePrinting(&printInfo))
	{
      CDC dcPrint;
		ASSERT(printInfo.m_pPD->m_pd.hDC != NULL);
		OnBeginPrinting(&dcPrint, &printInfo);
		OnPrepareDC(&dcPrint, &printInfo);
      if (m_nCurrentPage != -1)
      {
         int nFrom = printInfo.GetFromPage();
         int nTo   = printInfo.GetToPage();
         for (m_nCurrentPage=nFrom; m_nCurrentPage<=nTo; m_nCurrentPage++)
         {
            if ((m_nCurrentPage>nFrom) && (m_nCurrentPage<=nTo))
            {
               dcPrint.StartPage();
               dcPrint.EndPage();
            }
		      OnPrint(&dcPrint, &printInfo);
         }
      }
      else
      {
         OnPrint(&dcPrint, &printInfo);
      }

		OnEndPrinting(&dcPrint, &printInfo);
   }
}

void CCARA2DVView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
   ResetPickedLabels();
   SMarginDlgParam *pMdp = (SMarginDlgParam*)pInfo->m_lpUserData;
   ASSERT(pMdp!=NULL);
   CCARA2DVDoc *pDoc = GetDocument();

   CCaraView::OnBeginPrinting(pDC, pInfo);

   if (!pDoc->RestorePlotRects(pDC, pMdp->rcDrawLoMetric))
   {
      SetPlotSizes(pMdp->rcDrawLoMetric, pDC);           // neue Abmessungen setzen
   }
}

void CCARA2DVView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
   CCaraView::OnEndPrinting(pDC, pInfo);
   GetDocument()->SavePlotRects();
   ResizePlotsToView();
}

void CCARA2DVView::ResizePlotsToView()
{
   CDC *pdc = GetDC();
   CRect rcClient;
   if ((m_ptViewSize.x == 100) && (m_ptViewSize.y == 100))
   {
      GetClientRect(&rcClient);
      if (m_nRenderFlag & DO_RENDER)
         m_nRenderFlag |= CAN_RENDER;
   }
   else
   {
      ScrollToPosition(CPoint(0,0));
      rcClient.SetRect(0, 0, m_totalDev.cx, m_totalDev.cy);
      if (m_nRenderFlag & DO_RENDER)
         m_nRenderFlag &= ~CAN_RENDER;
   }
   OnPrepareDC(pdc);
   pdc->DPtoLP(&rcClient);
   SetPlotSizes(rcClient, pdc);
   ReleaseDC(pdc);

   if (m_nRenderFlag & CAN_RENDER)
   {
      m_nRenderFlag |= NEW_RENDER;
      ::SetEvent(m_hRenderEvent);
   }

   InvalidateRect(NULL);
}

void CCARA2DVView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView)
{
   CCaraView::OnEndPrintPreview(pDC, pInfo, point, pView);
   CCARA2DVDoc     *pDoc = GetDocument();
   pDoc->DeletePlotMenu();
   pDoc->InvalidatePlotMenu();
//   pDoc->BuildNewPlotMenu();
   CMainFrame *pWnd = (CMainFrame*)AfxGetApp()->m_pMainWnd;
   if (pWnd)
   {
      pWnd->SetMainPaneInfo();
   }

   CDC *pdc = GetDC();
   OnPrepareDC(pdc);
   CSize szPick(PICKSIZE, PICKSIZE);
   pdc->DPtoLP(&szPick);
   CLabel::SetPickPointSize(szPick);
   ReleaseDC(pdc);
}

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVView Diagnose

#ifdef _DEBUG
void CCARA2DVView::AssertValid() const
{
	CCaraView::AssertValid();
}

void CCARA2DVView::Dump(CDumpContext& dc) const
{
	CCaraView::Dump(dc);
}

CCARA2DVDoc* CCARA2DVView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCARA2DVDoc)));
	return (CCARA2DVDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVView Nachrichten-Handler
void CCARA2DVView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   OnLButtonDown(nFlags, point);
   CCaraView::OnLButtonDblClk(nFlags, point);
}
 
void CCARA2DVView::OnLButtonDown(UINT nFlags, CPoint point)
{
   if (m_bKeyControl)
   {
      ResetPickedLabels();
      m_bKeyControl = false;
   }
   m_nMouseFlags = nFlags|(m_nMouseFlags&0xff00);
   switch(m_nMouseMode)
   {
      case  ID_FUNC_EDITMODE:
         OnLButtonDownEdit(nFlags, point);
         break;
      case  ID_FUNC_ZOOMMODE:
         OnLButtonDownZoom(nFlags, point);
         break;
      case  ID_FUNC_PICKMODE:
         OnLButtonDownPick(nFlags, point);
         break;
   }
   CCaraView::OnLButtonDown(nFlags, point);
}

void CCARA2DVView::TestCurveEditMode()
{
   if (!((CCARA2DVApp*)AfxGetApp())->m_bEditable) return;
   CLabel *pl = m_PickList.GetLabel(0);
   if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
   {
      CCurveLabel *pcl = (CCurveLabel*)pl;
      if (pcl->IsSortable() && !pcl->IsSorted()) pcl->TestXSorted();
      if (pcl->IsSorted())
      {
         m_nMouseFlags &= ~MK_NO_LINE_EDIT;
      }
      else
      {
         m_nMouseFlags &= ~MK_LINEMODE;
         m_nMouseFlags |= MK_NO_LINE_EDIT;
      }
   }
}

LRESULT CCARA2DVView::OnResetPickStates(WPARAM wParam, LPARAM lParam)
{
   ResetPickedLabels();
   return 1;
}

LRESULT CCARA2DVView::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
   CCaraWinApp::gm_hCurrentTmeWnd = NULL;
   ChangeCursor(ARROWCURSOR);
   SetStatusPaneText(0, NULL);
   SetStatusPaneText(1, "");
   SetStatusPaneText(2, "");
   SetStatusPaneText(3, "");
   return 1;
}

void CCARA2DVView::OnMouseMove(UINT nFlags, CPoint point)
{
   if (m_hWnd != CCaraWinApp::gm_hCurrentTmeWnd)
   {
      TRACKMOUSEEVENT tme;
      tme.cbSize      = sizeof(TRACKMOUSEEVENT);
      tme.dwFlags     = TME_LEAVE;
      tme.hwndTrack   = m_hWnd;
      tme.dwHoverTime = HOVER_DEFAULT;
      _TrackMouseEvent(&tme);
      CCaraWinApp::gm_hCurrentTmeWnd = m_hWnd;
   }

   if (m_bKeyControl && (m_OldMousePoint != point))
   {
      ResetPickedLabels();
      m_bKeyControl = false;
   }
   m_nMouseFlags = nFlags|(m_nMouseFlags&0xff00);
   switch(m_nMouseMode)
   {
      case  ID_FUNC_EDITMODE:
         OnMouseMoveEdit(nFlags, point);
         break;
      case  ID_FUNC_ZOOMMODE:
         OnMouseMoveZoom(nFlags, point);
         break;
      case  ID_FUNC_PICKMODE:
         OnMouseMovePick(nFlags, point);
         break;
   }
   m_OldMousePoint = point;
   CCaraView::OnMouseMove(nFlags, point);
}
void CCARA2DVView::OnLButtonUp(UINT nFlags, CPoint point)
{
   switch(m_nMouseMode)
   {
      case  ID_FUNC_EDITMODE:
         OnLButtonUpEdit(nFlags, point);
         break;
      case  ID_FUNC_ZOOMMODE:
         OnLButtonUpZoom(nFlags, point);
         break;
      case  ID_FUNC_PICKMODE:
         OnLButtonUpPick(nFlags, point);
         break;
   }
   m_nMouseFlags = nFlags|(m_nMouseFlags&0xff00);
   CCaraView::OnLButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// Mausfunktionen zum editieren von Kurven
void CCARA2DVView::OnLButtonDownEdit(UINT nFlags, CPoint point)
{
   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);
   pDC->DPtoLP(&point);

   HitPlot(&point, pDC);                                       // aktuellen Plot ermitteln
   if (m_pActualPlot)
   {
      if (m_PickList.PickLabel(&point, m_pActualPlot->GetCurveList(), pDC, false, RGN_COPY, &m_UpdateRgn, true) & CL_LABELPICKED)
      {
         TestCurveEditMode();
         if (m_PickList.GetCount() == 1)
         {
            CLabel *pl = m_PickList.GetLabel(0);
            if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
            {
               CCurveLabel *pcl = (CCurveLabel*)pl;
               CRect rect = pcl->GetRect(pDC);
               if (!rect.PtInRect(point) && (pcl->IsOnLabel(pDC, &point) != CLabel::IsOutside()))//pcl->SetMatchPoint(&point))
               {
                  pcl->SetPointPicked(true);
                  if (m_nMouseFlags & MK_NUMERICMODE)
                  {
                     SCurve dp1 = pcl->GetDPoint();
                     SetStepValue(true);
                     SCurve dp2 = pcl->GetDPoint();
                     if (m_dStepValue)
                     {
                        int  nfactor = CEtsDiv::Round((dp1.y - dp2.y) / m_dStepValue);
                        if (nfactor)
                        {
                           dp2.y = dp2.y + (m_dStepValue * nfactor);
                           pcl->SetY_Value(pcl->GetMatchPoint(), dp2.y);
                        }
                     }
                  }
                  if (m_nMouseFlags & MK_LINEMODE)
                  {
                     pcl->SetLineStartPoint();
                     ChangeCursor(SIZEALLCURSOR);
                  }
                  else
                  {
                     pcl->SetShapeDrawn(false);
                     pcl->DrawShape(pDC, true);
                     ChangeCursor(SIZENSCURSOR);
                  }
                  pcl->GetRgn(RGN_COPY, pDC, &m_UpdateRgn);

                  SetMouseCapture();
               }
            }
         }
         else ChangeCursor(PENCURSOR);
      }
   }

   ReleaseDC(pDC);
}
void CCARA2DVView::OnMouseMoveEdit(UINT nFlags, CPoint point)
{
   SCurve  *pDP = NULL;
   CDC     *pDC = GetDC();
   CRect rcClient;
   GetClientRect(&rcClient);
   if (point.x > rcClient.right) point.x = rcClient.right;
   if (point.x < rcClient.left ) point.x = rcClient.left;
   OnPrepareDC(pDC);
   pDC->DPtoLP(&point);

   if ((nFlags&MK_LBUTTON) && (m_PickList.GetCount() == 1))    // wenn der Mausbutton gedrückt ist und nur eine Kurve gepickt ist
   {
      CLabel *pl = m_PickList.GetLabel(0);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         CCurveLabel *pcl = (CCurveLabel*) pl;
         if (pcl->IsEditable() && pcl->IsPointPicked())
         {
            pcl->DrawShape(pDC, false);
            if (m_nMouseFlags & MK_NUMERICMODE)
            {
               SCurve dp2, dp1 = pcl->GetDPoint();
               if (m_pActualPlot)
               {
                  double offset = pcl->GetOffset();
                  if (offset != 0) m_pActualPlot->SetOffset(&offset);
                  m_pActualPlot->TransformPoint(&point, (LPDPOINT)&dp2);
                  m_pActualPlot->SetOffset(NULL);
                  if (m_dStepValue)
                  {
                     int  nfactor = CEtsDiv::Round((dp2.y - dp1.y) / m_dStepValue);
                     if (nfactor) dp1.y = dp1.y + (m_dStepValue * nfactor);
                  }
               }
               dp1.x = dp2.x;
               pDP = &dp1;
               pcl->SetDPoint(dp1, point);
            }
            else  pcl->SetPoint(point);
            pcl->DrawShape(pDC, true);
         }         
      }
   }
   else 
      ChangeCursor(PENCURSOR);

   MousePosToStatusbar(pDC, &point, false, (LPDPOINT) pDP);
   ReleaseDC(pDC);
}
void CCARA2DVView::OnLButtonUpEdit(UINT nFlags, CPoint point)
{
   ReleaseMouseCapture();

   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);
   pDC->DPtoLP(&point);

   if (m_pActualPlot)
   {
      CLabel *pl = m_PickList.GetLabel(0);
      if (pl && pl->IsPointPicked() && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         CCurveLabel *pcl = (CCurveLabel*) pl;
         pcl->DrawShape(pDC, false);
         pcl->GetRgn(RGN_OR, pDC, &m_UpdateRgn);
         pcl->SetPointPicked(false);
         ((CCARA2DVApp*) AfxGetApp())->SetUndoCmd(LAST_CMD_CURVE_EDIT, pcl, NULL, 0, NULL);
//         pcl->UpdateLine();
      }
      if (HRGN(m_UpdateRgn))
      {
         InvalidateRgn(&m_UpdateRgn);
         m_UpdateRgn.DeleteObject();
         GetDocument()->SetModifiedFlag(true);
      }
   }

//   m_PickList.DrawShape(pDC, false);
   ReleaseDC(pDC);
   ChangeCursor(PENCURSOR);
}

/////////////////////////////////////////////////////////////////////////////
// Mausfunktionen zum Zoomen eines Plots
void CCARA2DVView::OnLButtonDownZoom(UINT nFlags, CPoint point)
{
   SetMouseCapture();

   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);
   pDC->DPtoLP(&point);

   HitPlot(&point, pDC);
   if (m_pActualPlot)
   {
      if (m_pActualPlot->GetXDivision() == ETSDIV_POLAR)
      {
         m_pActualPlot->ZoomPolarPlot(pDC, point, WM_LBUTTONDOWN);
      }
      else
      {
         m_PickList.RemoveAll();
         ((CLabel*)&m_PickList)->SetPoint(point, 0);
         ((CLabel*)&m_PickList)->SetPoint(point, 1);
         m_PickList.SetChanged(false);
         m_PickList.SetPicked(true);
         m_PickList.DrawShape(pDC, true);
      }
   }

   ReleaseDC(pDC);
   ChangeCursor(ZOOMCURSOR);
}
 
void CCARA2DVView::OnMouseMoveZoom(UINT nFlags, CPoint point)
{
   CPoint ptDiff = point - m_OldMousePoint;

   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);
   pDC->DPtoLP(&point);
   if (m_pHilightedLabel)
   {
      if (!m_pHilightedLabel->IsPicked())
      {
         m_pHilightedLabel->SetPicked(true);
         m_pHilightedLabel->DrawShape(pDC, true);
         ASSERT_KINDOF(CCurveLabel, m_pHilightedLabel);
         ((CCurveLabel*)m_pHilightedLabel)->DrawCursor(pDC, true);
      }
      else
      {
         m_pHilightedLabel->SetPicked(false);
         m_pHilightedLabel->DrawShape(pDC, false);
         ASSERT_KINDOF(CCurveLabel, m_pHilightedLabel);
         ((CCurveLabel*)m_pHilightedLabel)->DrawCursor(pDC, false);
         m_pHilightedLabel = NULL;
      }
   }
   MousePosToStatusbar(pDC, &point, true);

   if (nFlags&MK_LBUTTON)                                // wenn der Mausbutton gedrückt ist
   {
      if (m_pActualPlot && m_pActualPlot->GetXDivision() == ETSDIV_POLAR)
      {
         m_pActualPlot->ZoomPolarPlot(pDC, point, WM_MOUSEMOVE);
      }
      else
      {
         ptDiff = CLabel::PixelToLogical(pDC, ptDiff.x, ptDiff.y);
         m_PickList.EditLabelPoints(&point, &ptDiff, pDC);
      }
   }
/*
   if ((m_nMouseHover != SET_TOOLTIP+1) && (m_nMouseHover != END_TOOLTIP+1))
   {
      if (GetFocus() != this) SetFocus();
      m_nMouseHover = 0;
   }
*/
   ReleaseDC(pDC);
   ChangeCursor(ZOOMCURSOR);
}

void CCARA2DVView::OnLButtonUpZoom(UINT nFlags, CPoint point)
{
   ReleaseMouseCapture();

   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);
   pDC->DPtoLP(&point);

   if (m_pActualPlot)                 // Ist beim Drücken der Maustaste kein Plot ausgewählt worden: Ende
   {
      if (m_pActualPlot->GetXDivision() == ETSDIV_POLAR)
      {
         if (m_pActualPlot->ZoomPolarPlot(pDC, point, WM_LBUTTONUP))
            InvalidateLabelRgn(m_pActualPlot, pDC);
         else
            AfxMessageBox(IDE_RANGE_TOO_SMALL, MB_OK|MB_ICONSTOP);// Fehlermeldung ausgeben
      }
      else if (m_PickList.IsPicked())
      {
         m_PickList.OnPickRect(&point, NULL, pDC, 0, NULL, true);
         m_PickList.SetPicked(false);
         CRect zoomrect = m_PickList.GetRect(pDC);                // das durch PickList gezogene Rechteck ermitteln
         if (!zoomrect.IsRectEmpty())                             // ist das Zoom Rechteck leer: Ende
         {
            CRect viewrect = m_pActualPlot->GetView();            // Plotrechteck ermitteln
            if (zoomrect.IntersectRect(&zoomrect, &viewrect))     // Ist der Mauspunkt nicht im Plotrechteck: Ende
            {
               if (m_pActualPlot->SetRangeInLogical(pDC, zoomrect))// und setzen des Bereichs in logischen Koordinaten
                  InvalidateLabelRgn(m_pActualPlot, pDC);
               else
                  AfxMessageBox(IDE_RANGE_TOO_SMALL, MB_OK|MB_ICONSTOP);// Fehlermeldung ausgeben
            }
         }
      }
   }

   ReleaseDC(pDC);
   ChangeCursor(ZOOMCURSOR);
}
void CCARA2DVView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
   OnRButtonDown(nFlags, point);
   CCaraView::OnRButtonDblClk(nFlags, point);
}
void CCARA2DVView::OnRButtonDown(UINT nFlags, CPoint point) 
{
   if (nFlags & MK_LBUTTON)
   {
      CDC     *pDC = GetDC();
      OnPrepareDC(pDC);
      if (m_nMouseMode == ID_FUNC_ZOOMMODE)                       // Zoomen abbrechen
      {
         if (m_PickList.IsPicked())
         {
            m_PickList.DrawShape(pDC, false);
            m_PickList.SetPicked(false);
         }
      }
      else if (m_nMouseMode == ID_FUNC_EDITMODE)                  // editieren abbrechen
      {
         CLabel *pl = m_PickList.GetLabel(0);
         if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
         {
            CCurveLabel *pcl = (CCurveLabel *) pl;
            pcl->DrawShape(pDC, false);
            pcl->SetPointPicked(false);
            pcl->UndoChanges();
            m_UpdateRgn.DeleteObject();
         }
      }
      ReleaseDC(pDC);
   }
   CCaraView::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// Mausfunktionen zum Picken von Labeln
void CCARA2DVView::OnLButtonDownPick(UINT nFlags, CPoint point)
{
   SetMouseCapture();
   CDC *pDC = GetDC();
   OnPrepareDC(pDC);
   pDC->DPtoLP(&point);
   HitPlot(&point, pDC);
   if (m_pActualPlot)
   {
      CPlotLabel *ppl2 = Hit2ndPlot(m_pActualPlot, &point);
      if (ppl2)
      {
         CRgn rgn;
         m_lTile |= TILE_BY_MOUSE;
         m_PickList.DrawShape(pDC, false);
         m_PickList.ResetPickStates();
         m_PickList.RemoveAll();

         m_pActualPlot->SetPreviewMode(true);
         ppl2->SetPreviewMode(true);
         ppl2->SetPicked(true);

         m_pActualPlot->GetRgn(RGN_COPY, pDC, &rgn, INFLATE_RECT);
         ppl2->GetRgn(RGN_OR, pDC, &rgn, INFLATE_RECT);
         UpdateLabelRgn(rgn, pDC, true);

         m_PickList.InsertLabel(ppl2);
         ChangeCursor((m_lTile & TILE_HORIZONTAL) ? SIZENSCURSOR : SIZEWECURSOR);
      }
      else
      {
         int rValue = m_PickList.PickLabel(&point, m_pActualPlot->GetCurveList(), pDC, ((nFlags&MK_SHIFT) ? true : false), 0, NULL, true);
         TestCurveEditMode();
         if      (rValue&CL_TRACKPICKRECT)
         {
            if (m_pActualPlot->IsOnHeading(pDC, &point))
            {
               m_pActualPlot->SetHeadingPicked(true);
               ChangeCursor(CROSSCURSOR);
            }
            else
            {
               ChangeCursor(CROSSCURSOR);
            }
         }
         else if (rValue&CL_LABELPICKED)   ChangeCursor(TRACKCURSOR);
         else                              ChangeCursor(ARROWCURSOR);
      }
   }
   ReleaseDC(pDC);
}

void CCARA2DVView::OnMouseMovePick(UINT nFlags, CPoint point)
{
   CPoint ptScreen(point);
   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);
   pDC->DPtoLP(&point);
   ClientToScreen(&ptScreen);

   if (nFlags&MK_LBUTTON)                                 // wenn der Mausbutton gedrückt ist
   {
      if (m_lTile & TILE_BY_MOUSE)
      {
         CheckTilePlots(&point, pDC);
      }
      else
      {
         HWND hwnd;
         if ((CheckWindow(ptScreen, hwnd, true) == 1) && (hwnd == m_hWnd))
         {
            CheckCursorPosition(&point, pDC);
         }
      }
   }
   else CheckCursorPosition(&point, pDC);
   ReleaseDC(pDC);
}

void CCARA2DVView::OnLButtonUpPick(UINT nFlags, CPoint point)
{
   CCARA2DVApp *pApp = (CCARA2DVApp*) AfxGetApp();
   CCARA2DVDoc* pDoc = (CCARA2DVDoc*) GetDocument();
   CPoint ptScreen(point);
   HWND hwnd = NULL;
   ClientToScreen(&ptScreen);
   ReleaseMouseCapture();

   ChangeCursor(ARROWCURSOR);

   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);
   pDC->DPtoLP(&point);

   int            nPicked, i;
   nPicked = m_PickList.GetCount();
   if (m_lTile & TILE_BY_MOUSE)
   {
      CPlotLabel *ppl2  = (CPlotLabel*) m_PickList.GetLabel(0);
      CRgn        rgn;
      CheckTilePlots(&point, pDC);
      m_pActualPlot->SetPreviewMode(false);
      ppl2->SetPreviewMode(false);
      ppl2->SetPicked(false);

      m_lTile &= ~TILE_BY_MOUSE;
      m_PickList.RemoveAll();

      m_pActualPlot->GetRgn(RGN_COPY, pDC, &rgn, INFLATE_RECT);
      ppl2->GetRgn(RGN_OR, pDC, &rgn, INFLATE_RECT);
      UpdateLabelRgn(rgn, pDC);
   }
   else if (nPicked || m_pActualPlot)
   {
      int nID = CheckWindow(ptScreen, hwnd, false);
      switch (nID)
      {
         case ID_EDIT_COPY:             OnEditCopy();        break;
         case ID_EDIT_CLEAR:            OnEditDelete();      break;
         case ID_EDIT_CUT:              OnEditCut();         break;
         case ID_FUNC_REFERENCE_SELECT: OnReferenceSelect(); break;
         case 1: hwnd = NULL;
         default : break;
      }
      if (hwnd) ::PostMessage(hwnd, WM_COMMAND, ID_EDIT_PASTE, 0);
      else if (nPicked)                                     // Ist es innerhalb des Clientbereichs
      {
         CPlotLabel *ppl = CheckCursorPosition(&point, pDC);
         if (ppl)
         {
            for (i=0; i<nPicked; i++)
            {
               CLabel * pl = m_PickList.GetLabel(i);
               if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
               {
                  CCurveLabel *pcl = (CCurveLabel*) pl;
                  int nTargFuncNo = pcl->GetTargetFuncNo();
                  
                  if (nTargFuncNo) nFlags &=~MK_CONTROL;       // Zielfunktionen werden niemals kopiert !
                                                               // und niemals verschoben !
                  if (nFlags & MK_CONTROL)                     // Kurve kopieren
                  {
                     CCurveLabel *pclnew = new CCurveLabel(pcl);
                     if (pclnew)
                     {
                        pclnew->CopyValues(pcl);
                        pclnew->SetText(pcl->GetText());
                        pcl = pclnew;
                        pApp->SetUndoCmd(ID_EDIT_PASTE, pcl, NULL, -1, ppl);
                     }
                  }
                  else if (!nTargFuncNo)                       // Kurve verschieben,
                  {                                            // wenn es keine Zielfunktion ist
                     int nPos;
                     if (m_pActualPlot)
                     {
                        pDoc->UpdateAllViews(NULL, UPDATE_DELETE_LABEL, pcl);
                        if (m_pActualPlot->RemoveCurve(pcl, nPos))
                        {
                           pDoc->InvalidatePlotEdit();
                           pDoc->UpdateAllViews(NULL, UPDATE_DELETE_LABEL_FROM, m_pActualPlot);
                           pApp->SetUndoCmd(LAST_CMD_MOVE, pcl, m_pActualPlot, nPos, ppl);
                        }
                     }
                  }
                  ppl->InsertCurve(pcl, false, MAKELONG(-1, DONT_DELETE_INSERTED_CURVE));
               }
            }
            pDoc->UpdateAllViews(NULL, UPDATE_INSERT_LABEL_INTO, ppl);
            pDoc->SetModifiedFlag(true);
         }
      }
      else if (m_pActualPlot)
      {
         CRect rcPlot = m_pActualPlot->GetView();
         CRect rc = m_PickList.GetRect(pDC);
         rc.NormalizeRect();
         if (rcPlot.PtInRect(rc.TopLeft()) && rcPlot.PtInRect(rc.BottomRight()))
         {
            DPOINT dp1, dp2;
            m_pActualPlot->TransformPoint(&rc.TopLeft(), &dp1);
            m_pActualPlot->TransformPoint(&rc.BottomRight(), &dp2);
            if (dp1.x > dp2.x)
            {
               swap(dp1.x, dp2.x);
            }
            m_pActualPlot->SetMarker(0, dp1.x);
            m_pActualPlot->SetMarker(1, dp2.x);
            InvalidateLabelRgn(m_pActualPlot, pDC);
         }
      }
   }

   if (m_pActualPlot)
   {
      if (m_pActualPlot->IsHeadingPicked()) m_pActualPlot->SetHeadingPicked(false);
      else
      {
         m_PickList.OnPickRect(&point, m_pActualPlot->GetCurveList(), pDC, 0, NULL, true);
      }
   }

   ReleaseDC(pDC);
}

void CCARA2DVView::CheckTilePlots(CPoint *pp, CDC *pDC)
{
   CPlotLabel *ppl  = (CPlotLabel*) m_PickList.GetLabel(0);
   bool        bSet = false;
   ASSERT(ppl != NULL);
   ASSERT_KINDOF(CPlotLabel, ppl);
   CRect rc1 = m_pActualPlot->GetRect(),
         rc2 = ppl->GetRect();
   if (m_lTile & TILE_HORIZONTAL)
   {
      if (CLabel::IsBetween(pp->y, rc2.top, rc1.bottom))
      {
         CPoint pt = CLabel::PixelToLogical(pDC, 1, 1);
         rc1.top    = pp->y - pt.y;
         rc2.bottom = pp->y + pt.y;
         bSet = true;
      }
   }
   else
   {
      if (CLabel::IsBetween(pp->x, rc1.left, rc2.right))
      {
         CPoint pt = CLabel::PixelToLogical(pDC, 1, 1);
         rc1.right = pp->x - pt.x;
         rc2.left  = pp->x + pt.x;
         bSet = true;
      }
   }
   if (bSet)
   {
      if (pDC)
      {
         m_pActualPlot->DrawShape(pDC, false);
         ppl->DrawShape(pDC, false);
      }
      m_pActualPlot->SetRect(rc1);
      ppl->SetRect(rc2);
      if (pDC)
      {
         m_pActualPlot->DrawShape(pDC, true);
         ppl->DrawShape(pDC, true);
      }
   }
}

int CCARA2DVView::CheckWindow(CPoint ptScreen, HWND& hWnd, bool bPress)
{
   CCARA2DVApp *pApp = (CCARA2DVApp*) AfxGetApp();
   CMainFrame *pMF = (CMainFrame*)pApp->m_pMainWnd;
   hWnd = NULL;
   int nCount =m_PickList.GetCount();
   if (nCount || m_pActualPlot)
   {
      hWnd = ::WindowFromPoint(ptScreen);
      TRACE("%d, %d, %0x\n", ptScreen.x, ptScreen.y,hWnd);
      if (hWnd)
      {
         if (hWnd != m_hWnd)
         {
            UINT nID = 0xffffffff, nCursor = 0;
            if (pMF->m_wndToolBar.m_hWnd == hWnd)
            {
               pMF->m_wndToolBar.ScreenToClient(&ptScreen);
               nID = pMF->m_wndToolBar.IdFromPoint(ptScreen, bPress);
            }
            else if (pMF->m_wndPlotToolBar.m_hWnd == hWnd)
            {
               pMF->m_wndPlotToolBar.ScreenToClient(&ptScreen);
               nID = pMF->m_wndPlotToolBar.IdFromPoint(ptScreen, bPress);
            }
#ifdef ETS_OLE_SERVER
            else if (pApp->m_pActiveWnd && pApp->m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CInPlaceFrame)) &&
                     (((CInPlaceFrame*)pApp->m_pActiveWnd)->m_wndToolBar.m_hWnd == hWnd))
            {
               ((CInPlaceFrame*)pApp->m_pActiveWnd)->m_wndToolBar.ScreenToClient(&ptScreen);
               nID = ((CInPlaceFrame*)pApp->m_pActiveWnd)->m_wndToolBar.IdFromPoint(ptScreen, bPress);
               ((CInPlaceFrame*)pApp->m_pActiveWnd)->m_wndToolBar.UnpressOld();
            }
#endif
            else
            {
               long value;
               value = (long) GetProp(hWnd, "CARA2DVIEW");
               if   (value != 0x0815)
               {
                  ChangeCursor((nCount != 0) ? NODROPCURVE : NODROPPLOT);
                  hWnd = NULL;
                  return 0;
               }
               else
               {
                  ChangeCursor((nCount != 0) ? COPYCURVE   : COPYPLOT);
                  return ID_EDIT_COPY;
               }
            }
            hWnd = NULL;
            switch (nID)
            {
               case ID_FUNC_REFERENCE_SELECT: case ID_EDIT_CLEAR:
                  nCursor = (nCount != 0) ? TRACKCURVE : TRACKPLOT;
                  break;
               case ID_EDIT_CUT: case ID_EDIT_COPY:
                  nCursor = (nCount != 0) ? COPYCURVE: COPYPLOT;
                  break;
               case 0: case 0xffffffff:
                  pMF->m_wndToolBar.UnpressOld();
                  pMF->m_wndPlotToolBar.UnpressOld();
                  break;
               default:
                  nCursor = (nCount != 0) ? NODROPCURVE: NODROPPLOT;
                  break;
            }
            if (nCursor)
            {
               ChangeCursor(nCursor);
               return nID;
            }
         }
      }
   }
   pMF->m_wndToolBar.UnpressOld();
   pMF->m_wndPlotToolBar.UnpressOld();
   return (hWnd == m_hWnd) ? 1 : 0;
}

void CCARA2DVView::OnContextMenu(CWnd* pWnd, CPoint MenuPosition)
{
   if ((m_nMouseFlags & MK_LBUTTON)==0)                        // nur die rechte Maustaste
   {
      CMenu Menu, *pContext;
      CPoint point;
      CDC *pDC = GetDC();
      if (MenuPosition.x != -1 && MenuPosition.y != -1)
      {
         point = MenuPosition;
         ScreenToClient(&point);
         OnPrepareDC(pDC);
         pDC->DPtoLP(&point);
         HitPlot(&point, pDC);
      }
      CCaraMenu  ContextMenu, CoMenu; 
      int      ncount=0, num, nPlotMenuPos = -1;
      CString  string;
      CString  str3pt = _T("...");
      if (m_pActualPlot && ContextMenu.CreatePopupMenu())
      {
         CCARA2DVDoc*     pDoc = GetDocument();
#ifdef ETS_OLE_SERVER
         CCARA2DVApp *pApp = (CCARA2DVApp*) AfxGetApp();
         if (!pApp->m_CmdInfo.m_bRunAutomated && !pApp->m_CmdInfo.m_bRunEmbedded)
         {
            if (pDoc->GetPlotMenu() && pDoc->GetPlotsMenu()->GetMenuItemCount())
            {
               string.LoadString(IDS_PLOTS);
               ContextMenu.AppendMenu(MF_POPUP, (UINT)pDoc->GetPlotsMenu()->m_hMenu, string);
               nPlotMenuPos = ncount;
               ncount++;
            }
            ContextMenu.InsertMenu(ncount++, MF_BYPOSITION|MF_SEPARATOR);
         }
#else
         if (pDoc->GetPlotMenu() && pDoc->GetPlotMenu()->GetMenuItemCount())
         {
            string.LoadString(IDS_PLOTS);
            ContextMenu.AppendMenu(MF_POPUP, (UINT)pDoc->GetPlotMenu()->m_hMenu, string);
            nPlotMenuPos = ncount;
            ncount++;
         }
#endif
         CLabelContainer  *pCList = m_pActualPlot->GetCurveList();
         num = pCList->GetCount();
         CLabel      *pl;
         if (num > 0)
         {
            string.LoadString(IDS_SET_CURVE_VISIBILITY);
            string += str3pt;
            ContextMenu.AppendODMenuItem(ID_VIEW_CURVE, string, NULL);
            ncount++;
         }
         if (m_pActualPlot->GetNoOfRanges() > 1)
         {
            string.LoadString(IDS_SELECT_ZOOMRANGE);
            string += str3pt;
            ContextMenu.AppendODMenuItem(ID_FUNC_ZOOM+ID_TB_RIGHT_EXT, string, NULL);
            ncount++;
         }

         if (MenuPosition.x != -1 && MenuPosition.y != -1)
            m_PickList.PickLabel(&point, pCList, pDC, false, RGN_COPY, &m_UpdateRgn);

         string.LoadString(IDX_CONTEXT_SET_PLOT_MARKER);
         ContextMenu.AppendODMenuItem(IDX_CONTEXT_SET_PLOT_MARKER, string.Mid(string.Find("\n")+1), NULL);
         ncount++;

         pl = m_PickList.GetLabel(0);
         string.Empty();
         if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
         {
            CString sCurveText((char*) ((CCurveLabel*)pl)->GetText());
            if (!sCurveText.IsEmpty())
            {
               sCurveText.Replace(_T("&"), _T("&&"));
               string = _T("  ") + sCurveText;
           } 
         }
         if (string.IsEmpty())
         {
            string.LoadString(IDS_PROPERTY_PLOT);
            string += str3pt;

            ContextMenu.AppendODMenuItem(ID_OPTIONS_PLOT, string, NULL);
            ncount++;
         }
         else
         {
            Menu.LoadMenu(IDR_CONTEXT_MENUES);
            pContext = Menu.GetSubMenu(1);
            ContextMenu.AppendMenu(MF_POPUP, (UINT_PTR)pContext->m_hMenu, string);
         }

         ContextMenu.InsertMenu(ncount++, MF_BYPOSITION|MF_SEPARATOR);
         string.LoadString(IDS_CANCEL);
         ContextMenu.AppendODMenuItem(IDCANCEL, string, NULL);
         ncount++;
      }  
      else if (ContextMenu.CreatePopupMenu())
      {
         CCARA2DVDoc*     pDoc = GetDocument();
         if (pDoc->GetPlotMenu() && pDoc->GetPlotMenu()->GetMenuItemCount())
         {
            string.LoadString(IDS_PLOTS);
            ContextMenu.AppendMenu(MF_POPUP, (UINT)pDoc->GetPlotMenu()->m_hMenu, string);
            nPlotMenuPos = ncount;
            ncount++;
         }
         string.LoadString(IDS_STANDARD_PROPERTIES);
         ContextMenu.AppendODMenuItem(ID_OPTIONS_STANDARD, string, NULL);
         ncount++;
         ContextMenu.InsertMenu(ncount++, MF_BYPOSITION|MF_SEPARATOR);
         string.LoadString(IDS_CANCEL);
         ContextMenu.AppendODMenuItem(IDCANCEL, string, NULL);
         ncount++;
      }
      else return;
      if (MenuPosition.x == -1 && MenuPosition.y == -1)
      {
         CRect rcClient;
         GetClientRect(&rcClient);
         MenuPosition.x = rcClient.left;
         MenuPosition.y = rcClient.top;
         ClientToScreen(&MenuPosition);
      }
//      ContextMenu.TrackPopupMenu(TPM_LEFTALIGN, MenuPosition.x, MenuPosition.y, AfxGetApp()->m_pMainWnd);
      ContextMenu.TrackPopupMenu(TPM_LEFTALIGN, MenuPosition.x, MenuPosition.y, theApp.m_pMainWnd);
      ContextMenu.DeleteStrings();

      if (nPlotMenuPos != -1)
      {
         ContextMenu.RemoveMenu(nPlotMenuPos, MF_BYPOSITION);
      }
      ContextMenu.DestroyMenu(); 
      ReleaseDC(pDC);
   }
}
void CCARA2DVView::OnOptionsPlot() 
{
   if (m_pActualPlot && GetPropertySheet())
   {
      int nMode = 2;
      if (m_PickList.GetCount()) nMode = 3;
      m_pPropSheet->InitDialog(nMode, m_pActualPlot);
      if (m_pPropSheet->DoModal()==IDOK)
      {
         if (m_pActualPlot->IsChanged())
         {
            GetDocument()->UpdateAllViews(NULL, UPDATE_LABEL_REGION, m_pActualPlot);
            GetDocument()->SetModifiedFlag(true);
         }
      }
   }
}
void CCARA2DVView::OnOptionsStandard()
{
   CPlotLabel  plot;
   CCurveLabel curve;
   CString     text;
   text.LoadString(IDS_STANDARD_PROPERTIES);
   plot.SetPoint(CPoint(  0, 600), 0);
   plot.SetPoint(CPoint(800,  20), 1);
   plot.SetHeading((char*) LPCTSTR(text));
   curve.SetSize(2);
   curve.SetX_Value(0, 0.1);
   curve.SetY_Value(0, 0.2);
   curve.SetX_Value(1, 0.9);
   curve.SetY_Value(1, 0.8);
   curve.SetText("function");
   curve.SetXUnit("x");
   curve.SetYUnit("y");
   plot.InsertCurve(&curve);
   plot.CheckCurveColors();

   if (GetPropertySheet())
   {
      m_pPropSheet->InitDialog(1, &plot);
      if (m_pPropSheet->DoModal()==IDOK)
      {
         plot.SaveStandards();
         ((CCARA2DVApp*)AfxGetApp())->SaveDefaultSettings();
      }
   }
   int nPos;
   plot.RemoveCurve(&curve, nPos);
}
void CCARA2DVView::OnOptionsSetstandards()
{
   CCARA2DVDoc*     pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   ((CCARA2DVApp*)AfxGetApp())->LoadDefaultSettings();
   pDoc->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), NULL, SetStandardOptions);
   pDoc->UpdateAllViews(NULL, 0, NULL);
}
int CCARA2DVView::SetStandardOptions(CLabel*pl, void*)
{
   if (pl) ((CPlotLabel*)pl)->SetStandards();
   return 0;
}
BOOL CCARA2DVView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
   SetCursor(m_hActCursor);
   return true;
}
/////////////////////////////////////////////////////////////////////////////
// CCARA2DVView Nachrichten-Handler für Plot-Toolbar
void CCARA2DVView::ChangeMouseMode(UINT nMode)
{
   BEGIN("ChangeMouseMode");
   ResetPickedLabels();
 
   if (m_nMouseMode == (UINT)nMode) return;
   if (nMode == ID_FUNC_ZOOMMODE)
   {
      KillTimer(MOUSE_HOVERTIMER);
   }
   m_nMouseMode = nMode;
   switch (m_nMouseMode)
   {
      case ID_FUNC_PICKMODE: ChangeCursor(ARROWCURSOR); break;
      case ID_FUNC_ZOOMMODE:
      {
         ChangeCursor(ZOOMCURSOR);
//         SetTimer(MOUSE_HOVERTIMER, MOUSE_HOVERTIME, NULL);
         EnableToolTips(true);
         break;
      }
      case ID_FUNC_EDITMODE: ChangeCursor(PENCURSOR); break;
   }
   SetStatusPaneText(2, "");
   SetStatusPaneText(3, "");
}

void CCARA2DVView::OnSelectZoom()      {if (m_pActualPlot && m_pActualPlot->SelectZoom()) InvalidateLabelRgn(m_pActualPlot);}
void CCARA2DVView::OnRezoom()          {if (m_pActualPlot && m_pActualPlot->ReZoom())     InvalidateLabelRgn(m_pActualPlot);}
void CCARA2DVView::OnZoom()            {if (m_pActualPlot && m_pActualPlot->Zoom())       InvalidateLabelRgn(m_pActualPlot);}
void CCARA2DVView::OnDefaultrange()    {if (m_pActualPlot) {m_pActualPlot->SetDefaultRange(); InvalidateLabelRgn(m_pActualPlot);}}
void CCARA2DVView::OnDestroyrangelist(){if (m_pActualPlot) {m_pActualPlot->SetDefaults(); InvalidateLabelRgn(m_pActualPlot);}}
void CCARA2DVView::OnFuncDeleterange() {if (m_pActualPlot) {m_pActualPlot->DeleteCurrentRange(); InvalidateLabelRgn(m_pActualPlot);}}
void CCARA2DVView::OnFuncPanZoom(UINT nID)
{
   if (m_pActualPlot && m_pActualPlot->PanZoom(nID-ID_FUNC_RIGHT_ZOOM))
   {
      InvalidateLabelRgn(m_pActualPlot);
   }
}
void CCARA2DVView::OnReferenceSelect()
{
   if (m_pActualPlot && (m_pActualPlot->GetReferenceCurve() != m_PickList.GetLabel(0)))
   {
      CLabel      *pl  = m_PickList.GetLabel(0);
      CCurveLabel *pcl = NULL;
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel))) pcl = (CCurveLabel*) pl;
      m_pActualPlot->SetReferenceCurve(pcl);
      m_pActualPlot->SetReferenceOn(pcl!=NULL);
      if (m_pActualPlot->UpdateZoom()) InvalidateLabelRgn(m_pActualPlot);
   }
}
void CCARA2DVView::OnZeroReference() 
{
   if (m_pActualPlot && m_pActualPlot->GetReferenceCurve())
   {
      m_pActualPlot->SetReferenceOn(!m_pActualPlot->IsReferenceOn());
      if (m_pActualPlot->UpdateZoom()) InvalidateLabelRgn(m_pActualPlot);
   }
}
void CCARA2DVView::OnEditCut()
{
   OnEditCopy();
   OnEditDelete();
   CCARA2DVApp* pApp = (CCARA2DVApp*)AfxGetApp();
}
void CCARA2DVView::OnEditDelete()
{
   CCARA2DVApp* pApp = (CCARA2DVApp*)AfxGetApp();
   CCARA2DVDoc* pDoc = (CCARA2DVDoc*)GetDocument();
   if (pApp)
   {
      int nPos, i, count;
      CLabelContainer plotlist;
      if (m_PickList.GetCount())
      {
         while (m_PickList.GetCount())
         {     
            CLabel *pl = m_PickList.GetLabel(0);;
            if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
            {
               if (((CCurveLabel*)pl)->GetTargetFuncNo() != 0) break;
               CPlotLabel *ppl = pDoc->RemoveCurve((CCurveLabel*) pl, nPos);
               if (ppl)
               {
                  pDoc->SetModifiedFlag(true);
                  pApp->SetUndoCmd(ID_EDIT_CLEAR, pl, ppl, nPos, NULL);
                  plotlist.InsertLabel(ppl);
                  m_PickList.RemoveLabel(pl);
                  delete pl;
               }
            }
         }
         count = plotlist.GetCount();
         for (i=0; i<count; i++) pDoc->UpdateAllViews(NULL, UPDATE_DELETE_LABEL_FROM, plotlist.GetLabel(i));
      }
      else if (m_pActualPlot)
      {
         CPlotLabel *ppl = m_pActualPlot;
         if ((pApp->m_nCallingWindowMessage == WM_USER_OPT_UPDATE) && (ppl->GetPlotID() != 0))
            return;
         pDoc->RemovePlot(m_pActualPlot, nPos);
         pApp->SetUndoCmd(ID_EDIT_CLEAR, ppl, pDoc->GetLabelContainer(), nPos, NULL);
         delete ppl;
      }
   }
}

void CCARA2DVView::OnEditCopy()
{
   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);
   CLabel  *pl = NULL;
   bool     bDraw  = true;
   bool     bLabel = true;

   if (m_pActualPlot)
   {
      if (m_PickList.GetCount())                               // einzelne Label in das Clipboard legen
      {
         pl = &m_PickList;
         bDraw = false;                                        // kein Bitmap- oder Metafile-Format ablegen
      }
      else
      {
         pl = m_pActualPlot;                                   // einen Plot in das Clipboard legen
      }
   }
   else
   {
      pl = GetDocument()->GetLabelContainer();                 // Alle Label als Bitmap oder Metafile in das Clipboard legen
      bLabel = false;
   }

   CCARA2DVApp* pApp = (CCARA2DVApp*)AfxGetApp();
   int *pnIDNo = NULL;
   int i, nCount = m_PickList.GetCount(),
       nPlotID   = 0;
   if (pApp->m_nCallingWindowMessage == WM_USER_OPT_UPDATE)
   {
      if (nCount)
      {
         CLabel *pl;
         pnIDNo = new int[nCount];
         for (i=0; i<nCount; i++)
         {
            pl = m_PickList.GetLabel(i);
            ASSERT_KINDOF(CCurveLabel, pl);
            pnIDNo[i] = ((CCurveLabel*)pl)->GetTargetFuncNo();
            ((CCurveLabel*)pl)->SetTargetFuncNo(0);
         }
      }
      if (m_pActualPlot)
      {
         nPlotID = m_pActualPlot->GetPlotID();
         m_pActualPlot->SetPlotID(0);
      }
   }

   if (pApp) pApp->InsertIntoClipboard(pl, pDC, bDraw, bLabel);

   if (pnIDNo)
   {
      CLabel *pl;
      for (i=0; i<nCount; i++)
      {
         pl = m_PickList.GetLabel(i);
         ((CCurveLabel*)pl)->SetTargetFuncNo(pnIDNo[i]);
      }
      delete[] pnIDNo;
   }

   if (m_pActualPlot && (nPlotID != 0))
   {
      m_pActualPlot->SetPlotID(nPlotID);
   }

   ReleaseDC(pDC);
}

void CCARA2DVView::OnEditPaste()
{
//   ASSERT(FALSE);
   CCARA2DVApp   *pApp = (CCARA2DVApp*)AfxGetApp();
   CCaraFrameWnd *pCFW = (CCaraFrameWnd*)pApp->m_pMainWnd;
   //CLabel        *pl   = pCFW->GetFromClipboard(TEXT_CSV);
   CLabel        *pl   = pCFW->GetFromClipboard(CARA_LABEL);
   if (pl == NULL)
   {
       pl  = pCFW->GetFromClipboard(TEXT_CSV);
   }
   CPoint         ptCursor;
   if (::GetCursorPos(&ptCursor))
   {
      CRect rcClient;
      GetClientRect(&rcClient);
      ScreenToClient(&ptCursor);
      if (rcClient.PtInRect(ptCursor))
      {
         CDC *pDC = GetDC();
         OnPrepareDC(pDC);
         pDC->DPtoLP(&ptCursor);
         HitPlot(&ptCursor, pDC);
         ReleaseDC(pDC);
      }
   }
   if (pl)
   {
      pl->SetPreviewMode(false);
      pl->SetDrawPickPoints(false);
      CCARA2DVDoc* pDoc = GetDocument();
      if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
      {
         CLabelContainer *plc = (CLabelContainer*) pl;
         CPlotLabel *ppl;
         int i, count = plc->GetCount();
         for (i=0; i<count; i++)
         {
            pl  = plc->GetLabel(i);
            if (pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
            {
               CCurveLabel *pcl = (CCurveLabel*) pl;
               bool bTargetFunction = (pcl->GetTargetFuncNo() != 0) ? true : false;
               ppl = pDoc->InsertCurve(pcl, m_pActualPlot);
               if (ppl)
               {
                  if (!bTargetFunction)                        // eine Zielfunktion ersetzt die ursprüngliche
                  {                                            // deshalb Undo nur bei nicht-Zielfunktionen
                     pApp->SetUndoCmd(ID_EDIT_PASTE, pl, NULL, -1, ppl);
                  }
                  pDoc->UpdateAllViews(NULL, UPDATE_INSERT_LABEL_INTO, ppl);
               }
            }
         }
         delete plc;
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CPlotLabel)))
      {
         pDoc->InsertPlot((CPlotLabel*)pl);
         pApp->SetUndoCmd(ID_EDIT_PASTE, pl, NULL, -1, pDoc->GetLabelContainer());
         pDoc->UpdateAllViews(NULL, UPDATE_INSERT_LABEL, pl);
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         pDoc->InsertCurve((CCurveLabel*)pl);
         pApp->SetUndoCmd(ID_EDIT_PASTE, pl, NULL, -1, pDoc->GetLabelContainer());
         pDoc->UpdateAllViews(NULL, UPDATE_INSERT_LABEL, pl);
      }
      else delete pl;
   }
   else
   {

   }
}

void CCARA2DVView::OnOperationAverage() 
{
   if (((CCaraWinApp*)AfxGetApp())->IsLightVersion()) return;
   if (m_AvgDlg.IsActive())
   {
      if (m_AvgDlg.IsIconic()) m_AvgDlg.ShowWindow(SW_RESTORE);
      else                     m_AvgDlg.ShowWindow(SW_MINIMIZE);
   }
   else
   {
      m_AvgDlg.m_pCurves = &m_PickList;
      m_AvgDlg.Create(IDD_AVERAGE, AfxGetApp()->m_pMainWnd);
   }
}
void CCARA2DVView::OnOperationSpline() 
{
   if (((CCaraWinApp*)AfxGetApp())->IsLightVersion()) return;
   if (m_SplineDlg.IsActive())
   {
      if (m_SplineDlg.IsIconic()) m_SplineDlg.ShowWindow(SW_RESTORE);
      else                        m_SplineDlg.ShowWindow(SW_MINIMIZE);
   }
   else
   {
      m_SplineDlg.m_pCurves = &m_PickList;
      m_SplineDlg.Create(IDD_SPLINECURVE, AfxGetApp()->m_pMainWnd);
   }
}
void CCARA2DVView::OnOperationDerive() 
{
   if (((CCaraWinApp*)AfxGetApp())->IsLightVersion()) return;
   if (m_DrvDlg.IsActive())
   {
      if (m_DrvDlg.IsIconic()) m_DrvDlg.ShowWindow(SW_RESTORE);
      else                     m_DrvDlg.ShowWindow(SW_MINIMIZE);
   }
   else
   {
      m_DrvDlg.m_pCurves = &m_PickList;
      m_DrvDlg.Create(IDD_DERIVE, AfxGetApp()->m_pMainWnd);
   }
}
void CCARA2DVView::OnOperationFFT() 
{
   if (((CCaraWinApp*)AfxGetApp())->IsLightVersion()) return;
   if (m_FFTDlg.IsActive())
   {
      if (m_FFTDlg.IsIconic()) m_FFTDlg.ShowWindow(SW_RESTORE);
      else                     m_FFTDlg.ShowWindow(SW_MINIMIZE);
   }
   else
   {
      m_FFTDlg.m_pCurves = &m_PickList;
      m_FFTDlg.Create(IDD_FFT, AfxGetApp()->m_pMainWnd);
   }
}
void CCARA2DVView::OnOperationIntegrate() 
{
   if (((CCaraWinApp*)AfxGetApp())->IsLightVersion()) return;
   if (m_IntDlg.IsActive())
   {
      if (m_IntDlg.IsIconic()) m_IntDlg.ShowWindow(SW_RESTORE);
      else                     m_IntDlg.ShowWindow(SW_MINIMIZE);
   }
   else
   {
      m_IntDlg.m_pCurves = &m_PickList;
      m_IntDlg.Create(IDD_INTEGRATE, AfxGetApp()->m_pMainWnd);
   }
}

void CCARA2DVView::OnOperationCreatecurve() 
{
   if (((CCaraWinApp*)AfxGetApp())->IsLightVersion()) return;
   if (m_CreateCurveDlg.IsActive())
   {
      if (m_CreateCurveDlg.IsIconic()) m_CreateCurveDlg.ShowWindow(SW_RESTORE);
      else                             m_CreateCurveDlg.ShowWindow(SW_MINIMIZE);
   }
   else
   {
      m_CreateCurveDlg.m_pCurves = &m_PickList;
      m_CreateCurveDlg.Create(IDD_CREATECURVE, AfxGetApp()->m_pMainWnd);
   }
}
void CCARA2DVView::OnOperationFitcurve() 
{
   if (((CCaraWinApp*)AfxGetApp())->IsLightVersion()) return;
   if (m_FitCurveDlg.IsActive())
   {
      if (m_CreateCurveDlg.IsIconic()) m_FitCurveDlg.ShowWindow(SW_RESTORE);
      else                             m_FitCurveDlg.ShowWindow(SW_MINIMIZE);
   }
   else
   {
      m_FitCurveDlg.m_pCurves = &m_PickList;
      m_FitCurveDlg.Create(IDD_FITCURVE, AfxGetApp()->m_pMainWnd);
   }
}

void CCARA2DVView::OnUpdateMouseMode(CCmdUI* pCmdUI)
{
//   BEGIN("OnUpdateMouseMode");
   pCmdUI->SetCheck(pCmdUI->m_nID==m_nMouseMode);
   CCARA2DVApp* pApp = (CCARA2DVApp*)AfxGetApp();
   switch (pCmdUI->m_nID)
   {
      case ID_FUNC_PICKMODE:
//         pCmdUI->Enable(!pApp->m_bEditMode);
         break;
      case ID_FUNC_EDITMODE:
         pCmdUI->Enable(pApp->m_bEditable);
         if ((m_nMouseMode==ID_FUNC_EDITMODE) && !pApp->m_bEditable)
            ChangeMouseMode(ID_FUNC_PICKMODE);
         break;
      default:
         break;
   }
}


/******************************************************************************
* Name       : ChangeCursor                                                   *
* Definition : void ChangeCursor(UINT);                                       *
* Zweck      : Anpassen des Cursors an MausModi, Flags und Cursorposition.    *
* Aufruf     : ChangeCursor(nCursor);                                         *
* Parameter  :                                                                *
* nFlags (E) : CursorID                                              (UINT)   *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCARA2DVView::ChangeCursor(UINT nCursor)
{
   static UINT oldCursor=100;

   if (oldCursor != nCursor)
   {
//      TRACE("ChangeCursor %d\n", nCursor);
      switch (nCursor)
      {
         case ZOOMCURSOR:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_ZOOMCURSOR);
            break;
         case TRACKCURSOR:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_TRACKCURSOR);
            break;
         case COPYCURSOR:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_COPYCURSOR);
            break;
         case OPENHANDCURSOR:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_OPENHANDCURSOR);
            break;
         case ARROWCURSOR:
            m_hActCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
            break;
         case CROSSCURSOR:
            m_hActCursor = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
            break;
         case SIZENSCURSOR:
            m_hActCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZENS);
            break;
         case SIZEWECURSOR:
            m_hActCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
            break;
         case SIZEALLCURSOR:
            m_hActCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
            break;
         case PENCURSOR:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_EDITCURSOR);
            break;
         case NODROPCURSOR:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_NODROPCURSOR);
            break;
         case COPYCURVE:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_COPYCURVE);
            break;
         case TRACKCURVE:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_TRACKCURVE);
            break;
         case NODROPCURVE:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_NODROPCURVE);
            break;
         case NODROPPLOT:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_NODROPPLOT);
            break;
         case COPYPLOT:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_COPYPLOT);
            break;
         case TRACKPLOT:
            m_hActCursor = AfxGetApp()->LoadCursor(IDC_TRACKPLOT);
            break;
         default :
            m_hActCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
            break;
      }
      oldCursor  = nCursor;
      SetCursor(m_hActCursor);
   }
}


// CCARA2DVView: Bearbeitungsfunktionen für Plots und Kurven
CCurveLabel * CCARA2DVView::Get1stPickedCurve()
{
   CLabel *pl = m_PickList.GetLabel(0);
   if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))   return (CCurveLabel*) pl;
   else                                                  return NULL;
}

CCurveLabel * CCARA2DVView::Get2ndPickedCurve()
{
   CLabel *pl = m_PickList.GetLabel(1);
   if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))   return (CCurveLabel*) pl;
   else                                                  return NULL;
}

/******************************************************************************
* Name       : MousePosToStatusbar                                            *
* Definition : void MousePosToStatusbar(CPoint *, int);                       *
* Zweck      : Ausgabe der Mauscursorposition an die Statusbar in             *
*              Weltkoordinaten.                                               *
* Aufruf     : MousePosToStatusbar(p, n);                                     *
* Parameter  :                                                                *
* pDC    (E) : Zeiger auf DeviceKontextklasse                       (CDC*)    *
* p      (E) : Mauscursorposition in logischen Koordinaten          (CPoint*) *
* bCatchGrid(E): Am Gitter fangen (true, false)                     (bool)    *
* pdp    (E) : Darzustellender Punkt in Weltkoordinaten             (LPDPOINT)*
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCARA2DVView::MousePosToStatusbar(CDC *pDC, CPoint * p, bool bCatchGrid, LPDPOINT pdp)
{
//   BEGIN("MousePosToStatusbar");
   char        text[40] ="x: ";
   CPlotLabel *ppl = NULL;
   ETSDIV_NUTOTXT ntx;

   if ((m_pActualPlot) && ((m_nMouseFlags & MK_LBUTTON) || (pdp!=NULL)))
   {
      ppl = m_pActualPlot;
   }
   else
   {
      CCARA2DVDoc     *pDoc = GetDocument();
      ASSERT_VALID(pDoc);
      CSize        szPP = CLabel::GetPickPointSize();
      ProcessLabel pl   = {NULL, 0, pDC, p};
      pl.nPos = szPP.cx * szPP.cy;
      pDoc->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::HitLabel);
      if (pl.pl) ppl = (CPlotLabel*) pl.pl;
   }

   if (ppl)
   {
      DPOINT dp = {0.0,0.0};
      if      (pdp) dp = *pdp;
      else if (p)
      {
         bool bRef = ppl->IsReferenceOn();
         ppl->SetReferenceOn(false);
         ppl->TransformPoint(p, &dp);
         ppl->SetReferenceOn(bRef);
         if (bCatchGrid)                                       // Am Gitter fangen
         {
            DPOINT dpCatched;
            int rvalue = ppl->CatchGridValue(pDC, p, &dpCatched);
            if (rvalue & 1) dp.x = dpCatched.x;
            if (rvalue & 2) dp.y = dpCatched.y;
         }
      }
      ntx.nround = ppl->GetXRound()+2;                      // Rundungsstellen
      ntx.nmaxl  = ntx.nround + 5;
      ntx.nmode  = ppl->GetXNumModeEx();                    // Darstellungsmodus

      ntx.fvalue = dp.x;
      ntx.dZero  = 0;
      if (CEtsDiv::NumberToText(&ntx, &text[2]))
         SetStatusPaneText(2, text);

      text[0] = 'y';
      ntx.nround = ppl->GetYRound()+2;                      // Rundungsstellen
      ntx.nmaxl  = ntx.nround + 5;
      ntx.nmode  = ppl->GetYNumModeEx();                    // Darstellungsmodus
      if (ppl->NoYEndNulls()) ntx.nmode |= ETSDIV_NM_NO_END_NULLS;
      ntx.fvalue = dp.y;
      if (CEtsDiv::NumberToText(&ntx, &text[2]))
         SetStatusPaneText(3, text);
      SetStatusPaneText(0, (char*)ppl->GetHeading());
   }
   else
   {
      SetStatusPaneText(2, "");
      SetStatusPaneText(3, "");
   }

   if ((ID_FUNC_EDITMODE == m_nMouseMode) && m_PickList.GetCount())
   {
      CLabel *pl = m_PickList.GetLabel(0);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         SCurve dp1, dp2;
         CCurveLabel *pcl = (CCurveLabel*) pl;
         if (pcl->GetLinePoints(dp1,dp2))
         {
            if (ppl)
            {
               if (ppl->GetXDivision()!=ETSDIV_LINEAR)
               {
                  dp1.x = log10(dp1.x);
                  dp2.x = log10(dp2.x);
               }
               if (ppl->GetYDivision()!=ETSDIV_LINEAR)
               {
                  dp1.y = log10(dp1.y);
                  dp2.y = log10(dp2.y);
               }
               double dy = dp2.y-dp1.y,
                      dx = dp2.x-dp1.x,
                      m;
               if ((fabs(dx) > 1.0e-13) && (fabs(dy) > 1.0e-13)) m = dy / dx;
               else                                              m = 0;
               ntx.nround = ppl->GetXRound()+2;             // Rundungsstellen
               ntx.nmaxl  = ntx.nround + 5;
               ntx.nmode  = ppl->GetXNumModeEx();           // Darstellungsmodus

               ntx.dZero  = 0;
               if (((ppl->GetXDivision()==ETSDIV_FREQUENCY)||(ppl->GetXDivision()==ETSDIV_LOGARITHMIC)) && (ppl->GetYDivision()==ETSDIV_LINEAR))
               {
                  m *= log10(2.0);
                  ntx.fvalue = m;
                  CEtsDiv::NumberToText(&ntx, text);
                  const char *pszUnit = ppl->GetYUnit();
                  if (pszUnit) strcat(text, pszUnit);
                  else         strcat(text, "[]");
                  strcat(text, "/Oktave");
               }
               else
               {
                  ntx.fvalue = m;
                  CEtsDiv::NumberToText(&ntx, text);
                  const char *pszUnit = ppl->GetYUnit();
                  if (pszUnit) strcat(text, pszUnit);
                  else         strcat(text, "[]");
                  strcat(text, "/");
                  pszUnit = ppl->GetXUnit();
                  if (pszUnit) strcat(text, pszUnit);
                  else         strcat(text, "[]");
               }
            }
            SetStatusPaneText(1, text);
         }
      }
   }
}

/******************************************************************************
* Name       : InvalidateLabelRgn                                             *
* Definition : InvalidateLabelRgn(CLabel *, CDC *pDC=NULL);                   *
* Zweck      : Macht die Plotregion des Plots ungültig für die Neuzeichnung   *
*              des Bildschirms.                                               *
* Aufruf     : InvalidateLabelRgn(pDC)                                        * 
* Parameter  :                                                                *
* ppl    (E) : Zeiger auf Label                                 (CLabel*)     *
* [pDC]  (E) : Zeiger auf DeviceKontextklasse                   (CDC*)        *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCARA2DVView::InvalidateLabelRgn(CLabel *pl, CDC *pDC)
{
   ASSERT(pl!=NULL);
   if (m_nRenderFlag & CAN_RENDER)
   {
      m_nRenderFlag |= NEW_RENDER;
      ::SetEvent(m_hRenderEvent);
      return;
   }
   BOOL bDC=false;
   if (!pDC)
   {
      pDC = GetDC();
      OnPrepareDC(pDC);
      bDC = true;
   }

   CRgn rgn;
   pl->GetRgn(RGN_COPY, pDC, &rgn);
   InvalidateRgn(&rgn);
   rgn.DeleteObject();
   if (bDC) ReleaseDC(pDC);
}

void CCARA2DVView::UpdateLabelRgn(CRgn & rgn, CDC *pDC, bool bErase)
{
   if (HRGN(rgn)!= NULL)
   {
      InvalidateRgn(&rgn, bErase);                           // und updaten
      rgn.DeleteObject();                                   // Objekt löschen
   }
}

/******************************************************************************
* Name       : CheckCursorPosition                                            *
* Definition : void CheckCursorPosition(CPoint *, CDC *);                     *
* Zweck      : Überprüfen der Cursorposition und Anpassen der Cursor an die   *
*              Position und Flags.                                            *
* Aufruf     : CheckCursorPosition(nFlags, pp, pDC);                          *
* Parameter  :                                                                *
* pp     (E) : Mauscursorposition in logischen Koordinaten          (CPoint*) *
* pDC    (E) : Zeiger auf DeviceKontextklasse                       (CDC*)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
CPlotLabel * CCARA2DVView::CheckCursorPosition(CPoint *pp, CDC *pDC)
{
   CCARA2DVDoc     *pDoc   = GetDocument();
   ASSERT_VALID(pDoc);
   int              nCount = m_PickList.GetCount();
   ProcessLabel     pl     = {NULL, 0, pDC, pp};
   pl.nPos                 = CLabel::MaxPickDistance();
   CLabelContainer *pplots = pDoc->GetLabelContainer();

   pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::HitLabel);

   if (pl.pl)
   {
      CPlotLabel* ppl = (CPlotLabel*) pl.pl;
      if (m_nMouseFlags&MK_LBUTTON)                            // ist die linke Maustaste gedrückt und
      {
         if (nCount)
         {
            pl.pl = m_PickList.GetLabel(0);                    // ein CurveLabel gepickt und nicht Pointpicked
            if (pl.pl && pl.pl->IsVisible() && pl.pl->IsPicked() && !pl.pl->IsPointPicked())
            {
               if (pl.pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
               {
                  if (ppl->InsertCurve((CCurveLabel*)pl.pl, true))// testen ob der CurveLabel eingefügt werden kann
                  {
                     if (m_nMouseFlags & MK_CONTROL) ChangeCursor(COPYCURVE);// CCurveLabel würde kopiert werden
                     else                            ChangeCursor(TRACKCURVE);// CCurveLabel würde verschoben werden
                     return ppl;                               // Plot zurückliefern
                  }
                  else
                  {
                     ChangeCursor(NODROPCURVE);                // CCurveLabel darf nicht kopiert oder verschoben werden
                     return NULL;                              // Null liefern
                  }
               }
            }
         }
         else if (m_pActualPlot!= NULL)
         {
            if (m_pActualPlot->IsHeadingPicked())
            {
               ChangeCursor(NODROPPLOT);
            }
            else 
            {
               CPoint ptDiff = *pp - m_OldMousePoint;
               m_PickList.EditLabelPoints(pp, &ptDiff, pDC, NULL);
            }
            return NULL;
         }
      }
      else
      {
         if (Hit2ndPlot(ppl, pp))
         {
            ChangeCursor((m_lTile & TILE_HORIZONTAL) ? SIZENSCURSOR: SIZEWECURSOR);
            return NULL;
         }
      }
      pl.pl = ppl->GetCurveList()->HitLabel(pp, pDC);
      if (pl.pl && pl.pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         ChangeCursor(OPENHANDCURSOR);
         SetStatusPaneText(0, (char*)((CCurveLabel*)pl.pl)->GetText());
         return ppl;
      }
   }

   if ((m_nMouseFlags&MK_LBUTTON) && (((m_pActualPlot!= NULL) && m_pActualPlot->IsOnHeading(pDC, pp)) || nCount))// ist die linke Maustaste gedrückt und CPlotLabel :
      ChangeCursor((nCount!=0) ? NODROPCURVE:NODROPPLOT);      // CPlotLabel darf nicht ins eigene Fenster kopiert oder verschoben werden
   else
      ChangeCursor(ARROWCURSOR);
   return NULL;
}

CPlotLabel* CCARA2DVView::Hit2ndPlot(CPlotLabel*ppl, CPoint *pp)
{
   int  nVisible = 0;
   CSize szPP = CLabel::GetPickPointSize();
   CLabelContainer  *pplots = GetDocument()->GetLabelContainer();
   pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &nVisible, CLabelContainer::CountVisibleLabel);
   if (nVisible > 1)
   {
      ProcessLabel pl = {ppl, 1, NULL, NULL};
      CRect rc = ppl->GetRect();
      if (m_lTile & TILE_HORIZONTAL)
      {
         if (abs(rc.top - pp->y) < szPP.cy)
         {
            pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::GetNextLabel);
            if (pl.pParam1)
            { 
               ppl = (CPlotLabel*)pl.pParam1;
               rc  = ppl->GetRect();
               if (abs(rc.bottom - pp->y) < szPP.cy)
                  return ppl;
            }
         }
      }
      else
      {
         if (abs(rc.right - pp->x) < szPP.cx)
         {
            pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::GetNextLabel);
            if (pl.pParam1)
            { 
               ppl = (CPlotLabel*)pl.pParam1;
               rc  = ppl->GetRect();
               if (abs(rc.left - pp->x) < szPP.cx)
                  return ppl;
            }
         }
      }
   }
   return NULL;
}

/******************************************************************************
* Name       : HitPlot                                                        *
* Definition : void HitPlot(CPoint *, CDC *);                                 *
* Zweck      : ermittelt den Plot, auf dem sich der Mauscursor befindet       *
               und speichert den Zeiger auf diesen Plot in m_pActualPlot.     *
* Aufruf     : HitPlot(pp, pDC);                                              *
* Parameter  :                                                                *
* pp     (E) : Mauscursorposition in logischen Koordinaten          (CPoint*) *
* pDC    (E) : Zeiger auf DeviceKontextklasse                       (CDC*)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCARA2DVView::HitPlot(CPoint *pp, CDC *pDC)
{
   CCARA2DVDoc     *pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   CLabelContainer  *pplots = pDoc->GetLabelContainer();
   CPlotLabel       *ppl;
   ProcessLabel pl   = {NULL, 0, pDC, pp};
   pl.nPos           = CLabel::MaxPickDistance();
   pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::HitLabel);

   if (pl.pl)
   {
      ppl = (CPlotLabel*) pl.pl;
      if (ppl == m_pActualPlot) return;                           // wenn sich nichts ändert: Ende

      if (m_pActualPlot)                                          // alten Plot zurücksetzen
      {
         m_pActualPlot->ResetPickStates();
         m_pActualPlot->SetPicked(false);
         m_pActualPlot->DrawShape(pDC, false);
      }

      if (ppl)                                                    // neuen Plot setzen
      {
         m_pActualPlot = ppl;
         m_pActualPlot->SetPicked(true);
         m_pActualPlot->DrawShape(pDC, true);
      }
   }
}

/******************************************************************************
* Name       : SetPlotSizes                                                   *
* Definition : void SetPlotSizes(CRect , CDC *);                              *
* Zweck      : Berechnen der Position und Größe der einzelnen sichtbaren Plots*
* Aufruf     : SetPlotSizes(rcPlots, pDC);                                    *
* Parameter  :                                                                *
* rcPlots (E): Rechteck ,in dem die Plots platziert werden sollen     (CRect) *
               (in logischen Koordinaten)                                     *
* pDC     (E): Zeiger auf DeviceKontextklasse                         (CDC*)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCARA2DVView::SetPlotSizes(CRect rcPlots, CDC *pDC)
{
   CCARA2DVDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   CPoint one = CLabel::PixelToLogical(pDC,    1,  1);
   CPoint two = CLabel::PixelToLogical(pDC,    2,  2);
   CRect  plotrect(rcPlots.left+one.x, rcPlots.top+one.y, rcPlots.right-two.x, rcPlots.bottom-one.y);
   long lValue[3] = {(long)&one, (long)&two, (long)&plotrect};

   CLabelContainer  *pplots = pDoc->GetLabelContainer();
   ProcessLabel pl = {NULL, 0, &lValue, &m_lTile};

   int  count    = 0;
   pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &count, CLabelContainer::CountVisibleLabel);
   if (count > 0)
   {
      if (m_lTile & TILE_HORIZONTAL)
      {
         pl.nPos = (rcPlots.bottom - rcPlots.top) / count;
         plotrect.bottom = plotrect.top + pl.nPos - two.y;
      }
      else
      {
         pl.nPos = (rcPlots.right  - rcPlots.left) / count;
         plotrect.right = plotrect.left + pl.nPos - two.x;
      }
      pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, SetPlotRect);
      pl.nPos    = 0;
      pl.pl      = NULL;
      pl.pParam1 = NULL;
      pl.pParam2 = &rcPlots;
      pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::ClipLabel);
   }
}

int CCARA2DVView::SetPlotRect(CLabel *pl, void *pParam)
{
   if (pl && pl->IsVisible() && pParam)
   {
      ProcessLabel *ppl    = (ProcessLabel*) pParam;
      CRect        *pRect  = (CRect *) ((long*)ppl->pParam1)[2];
      CPoint       *pTwo   = (CPoint*) ((long*)ppl->pParam1)[1];
      CPoint       *pOne   = (CPoint*) ((long*)ppl->pParam1)[0];
      long         lTile   = *((long*)ppl->pParam2);
      ((CPlotLabel*)pl)->SetRect(*pRect);
      if (lTile & TILE_HORIZONTAL)
      {
         pRect->top     = pRect->bottom + pOne->y;
         pRect->bottom += ppl->nPos - pOne->y;
      }
      else
      {
         pRect->left    = pRect->right + pTwo->x;
         pRect->right   += ppl->nPos - pOne->x;
      }
   }
   return 0;
}

/******************************************************************************
* Name       : OnUpdate                                                       *
* Definition : virtual void OnUpdate(CView*, LPARAM, CObject*);               *
* Zweck      : Updaten des Views ausgelöst durch die Nachricht von anderen    *
*              Views.                                                         *
* Aufruf     : OnUpdate(pSender, lHint, pHint);                               *
* Parameter  :                                                                *
* pSender (E): Zeiger auf den Sender der Nachricht                 (CView)    *
* lHint   (E): Art der Nachricht                                   (LPARAM)   *
* pHint   (E): verändertes Objekt (Label)                          (CObject*) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCARA2DVView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
   BEGIN("OnUpdate");
   if      (lHint & UPDATE_PREVIEW_LABEL)
   {
      return;
   }
   else if (lHint & UPDATE_DELETE_LABEL_FROM)
   {
      if ((pHint) && (pHint->IsKindOf(RUNTIME_CLASS(CPlotLabel))))
      {
         CPlotLabel *ppl = (CPlotLabel*) pHint;
         ppl->SetChanged(true);
         InvalidateLabelRgn(ppl);
         return;
      }
      else
         m_pActualPlot = NULL;
   }
   else if (lHint & UPDATE_DELETE_LABEL)
   {
      m_PickList.RemoveLabel((CLabel*) pHint);
      if (pHint == m_pActualPlot) m_pActualPlot = NULL;
      if (pHint->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         GetDocument()->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), (void*)pHint, CCARA2DVDoc::RemoveReferenceCurve);
      }
   }
   else if (lHint & UPDATE_DELETE_CONTENTS)
   {
      m_PickList.RemoveAll();
      m_pActualPlot = NULL;
      m_nMouseMode  = 0;
      ChangeCursor(0);
   }
   else if (lHint & UPDATE_INSERT_LABEL_INTO)
   {
      if ((pHint) && (pHint->IsKindOf(RUNTIME_CLASS(CPlotLabel))))
      {
         CPlotLabel *ppl = (CPlotLabel*) pHint;
         ppl->SetChanged(true);
         InvalidateLabelRgn(ppl);
         return;
      }
   }
   else if (lHint & UPDATE_LABEL_REGION)
   {
      InvalidateLabelRgn((CLabel*)pHint);
   }
   else if (lHint & UPDATE_INSERT_LABEL)
   {
      InitMouseMode();
   }
   else if (lHint & UPDATE_INSERT_NEWDOC)
   {
      InitMouseMode();
      GetDocument()->InvalidatePlotMenu();
      GetDocument()->InvalidatePlotEdit();
//    GetDocument()->BuildNewPlotMenu();
   }
   else if (lHint & UPDATE_CURVE_UNDO)
   {
      if (pHint && pHint->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         CDC  *pDC  = GetDC();
         OnPrepareDC(pDC);
         CRgn rgn;
         if (((CCurveLabel*)pHint)->GetUndoRgn(RGN_COPY, pDC, &rgn))
         {
            InvalidateRgn(&rgn);
            rgn.DeleteObject();
         }
         ReleaseDC(pDC);
      }
      return;
   }
   else if (lHint & UPDATE_RESET_PICKSTATES)
   {
      ResetPickedLabels();
      return;
   }
   else if (lHint & UPDATE_PRINT_SETTINGS)
   {
      CView *pView = ((CFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveView();
      if (pView == this)
      {
         SMarginDlgParam *pMdp = (SMarginDlgParam*) pHint;
         if (pMdp)
         {
            if (pMdp->nScaleLabel&SCALE_LABEL)
            {
               CDC *pDC = GetDC();
               OnPrepareDC(pDC);
               CCARA2DVDoc     *pDoc = GetDocument();
               if (pDoc->RestorePlotRects(pDC, pMdp->rcDrawLoMetric))
               {
                  CLabelContainer *plc = pDoc->GetLabelContainer();
                  plc->MoveLabel(-((CRect*)&pMdp->rcOldDrawLoMetric)->CenterPoint());
                  plc->ScaleLabel(CSize(pMdp->rcDrawLoMetric.right     - pMdp->rcDrawLoMetric.left,
                                        pMdp->rcDrawLoMetric.bottom    - pMdp->rcDrawLoMetric.top),
                                  CSize(pMdp->rcOldDrawLoMetric.right  - pMdp->rcOldDrawLoMetric.left,
                                        pMdp->rcOldDrawLoMetric.bottom - pMdp->rcOldDrawLoMetric.top));
                  plc->MoveLabel(((CRect*)&pMdp->rcDrawLoMetric)->CenterPoint());
                  pDoc->SavePlotRects();
               }
               else
               {
                  SetPlotSizes(pMdp->rcDrawLoMetric, pDC);
               }
//               CRect rcClient;
//               GetClientRect(&rcClient);
//               pDC->DPtoLP(&rcClient);
//               SetPlotSizes(rcClient, pDC);
               ReleaseDC(pDC);
               ResizePlotsToView();
               pMdp->nScaleLabel &= ~SCALE_LABEL;
            }
            pMdp->nScaleLabel |= RESTORE_POSITIONS;
         }
      }
      return;
   }

   if ((pSender) && (pSender != this)) return;
   ResizePlotsToView();
}

BOOL CCARA2DVView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   BOOL bResult = true;
   CCARA2DVDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   CLabelContainer  *pplots = pDoc->GetLabelContainer();

   if (pplots)
   {
      UINT count = pplots->GetCount();
      if ((nCode == CN_COMMAND) && (pExtra == NULL))
      {
         if ((ID_VIEW_PLOT <= nID) && (nID <= ID_VIEW_LAST_PLOT) && (nID < ID_VIEW_PLOT + count))
         {
            // Äquivalent zu Funktionsaufruf OnPlot{index}();
            ProcessLabel pl = {NULL, (nID - ID_VIEW_PLOT), NULL, NULL};
            pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::GetLabelFromIndex);
            if (pl.pl)
            {
               if ((pl.pl == m_pActualPlot) && pl.pl->IsVisible())
               {
                  CDC     *pDC = GetDC();
                  OnPrepareDC(pDC);
                  m_PickList.ResetStates();
                  m_PickList.DrawShape(pDC, false);
                  m_PickList.RemoveAll();
                  m_pActualPlot->ResetPickStates();
                  m_pActualPlot->DrawShape(pDC, false);
                  m_pActualPlot = NULL;
                  ReleaseDC(pDC);
               }
               pl.pl->SetVisible(!pl.pl->IsVisible());
               GetDocument()->UpdateAllViews(NULL, 0, NULL);
            }
            return bResult;
         }
         else
         {
            switch (nID)
            {
               case ID_VIEW_CURVE:
               if (m_pActualPlot)
               {
                  CLabelContainer *plc = m_pActualPlot->GetCurveList();
                  if ((plc != NULL) && (plc->GetCount() > 0))
                  {
                     CListDlg LDlg;
                     LDlg.m_pCallBack   = ShowOrHideCurves;
                     LDlg.m_pParam      = this;
                     LDlg.m_strSelected.LoadString(IDS_SET_CURVE_VISIBILITY);
                     LDlg.m_szSize.cx   = 250;
                     LDlg.m_szSize.cy   = 200;
                     LDlg.m_bOkButton   = true;
                     LDlg.DoModal();
                  }
               } break;
               case ID_VIEW_HORIZONTAL:
                  m_lTile &= ~TILE_VERTICAL;
                  m_lTile |=  TILE_HORIZONTAL;
                  ResizePlotsToView();
                  break;
               case ID_VIEW_VERTICAL:
                  m_lTile &= ~TILE_HORIZONTAL;
                  m_lTile |=  TILE_VERTICAL;
                  ResizePlotsToView();
                  break;
               case IDCANCEL:break;
            }
         }
      }
      if (((nCode == CN_UPDATE_COMMAND_UI)||(nCode ==  0xFFFF)) && (pExtra != NULL))
      {
		   CCmdUI* pCmdUI = (CCmdUI*)pExtra;
		   ASSERT(!pCmdUI->m_bContinueRouting);    // idle - not set
         bool bFound = true;
         bool bEnable = true;
         if ((ID_VIEW_PLOT <= nID) && (nID < ID_VIEW_PLOT + count))
         {
            ASSERT(pExtra != NULL);
            // Äquivalent zu Funktionsaufruf OnUpdatePlot{index}(CCmdUI* pCmdUI);
            pCmdUI->Enable(true);
            ProcessLabel pl = {NULL, (nID - ID_VIEW_PLOT), NULL, NULL};
            pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::GetLabelFromIndex);
            if (pl.pl) pCmdUI->SetCheck(pl.pl->IsVisible());
         }
         else if ((ID_OPERATION_AVERAGE <= nID) && (nID <= ID_OPERATION_FFT))
         {
            BOOL bIsWindow = true;
            switch (nID)
            {
               case ID_OPERATION_AVERAGE:    
                  bIsWindow = m_AvgDlg.IsActive();
                  break;
               case ID_OPERATION_SPLINE:
                  bIsWindow = m_SplineDlg.IsActive();
                  break;
               case ID_OPERATION_DERIVE:
                  bIsWindow = m_DrvDlg.IsActive();
                  break;
               case ID_OPERATION_INTEGRATE:
                  bIsWindow = m_IntDlg.IsActive();
                  break;
               case ID_OPERATION_FITCURVE:
                  bIsWindow = m_FitCurveDlg.IsActive();
                  break;
               case ID_OPERATION_FFT:
                  bIsWindow = m_FFTDlg.IsActive();
                  break;
            }
            bEnable = (bIsWindow!=0)?true:false;
            if (m_PickList.GetCount() != 0)
            {
               CLabel *pl = m_PickList.GetLabel(0);
               ASSERT_KINDOF(CCurveLabel, pl);
               CCurveLabel *pcl = (CCurveLabel*) pl;

               if      (pcl->IsInterupted() ) bEnable = false;
               else if (pcl->GetLocus() != 0) bEnable = true;
               else if (!pcl->IsSorted())     bEnable = false;
               else                           bEnable = true;
            }
            pCmdUI->Enable(bEnable);
         }
         else
         {
            switch (nID)
            {
               case ID_EDIT_CLEAR: case ID_EDIT_CUT: 
                  if (((CCARA2DVApp*)AfxGetApp())->m_bEditMode)
                     bEnable = false;
                  else
                     bEnable = (m_pActualPlot!=NULL) ? true : false;
                  pCmdUI->Enable(bEnable);
                  break;
               case ID_EDIT_PASTE:
                  ((CCARA2DVApp*)AfxGetApp())->OnUpdateEditPaste(pCmdUI);
                  break;                  
               case ID_OPTIONS_PLOT:
                  bEnable = (m_pActualPlot!=NULL) ? true : false;
                  pCmdUI->Enable(bEnable);
                  break;
               case ID_FUNC_SAVE_CURVE:
                  pCmdUI->Enable(Get1stPickedCurve() != NULL);
                  break;
               case ID_FUNC_DELETERANGE:     pCmdUI->Enable(m_pActualPlot && m_pActualPlot->CheckDeleteCurrentRange()); break;
               case ID_FUNC_REZOOM:          pCmdUI->Enable(m_pActualPlot && m_pActualPlot->CheckReZoom());             break;
               case ID_FUNC_ZOOM:            pCmdUI->Enable(m_pActualPlot && m_pActualPlot->CheckZoom());               break;
               case ID_FUNC_DEFAULTRANGE:    pCmdUI->Enable(m_pActualPlot && m_pActualPlot->CheckReZoom());             break;
               case ID_FUNC_DESTROYRANGELIST:pCmdUI->Enable(m_pActualPlot && m_pActualPlot->CheckDefault());            break;
               case ID_FUNC_RIGHT_ZOOM:
               case ID_FUNC_LEFT_ZOOM:
               case ID_FUNC_UP_ZOOM:
               case ID_FUNC_DOWN_ZOOM:
                  pCmdUI->Enable(m_pActualPlot != NULL);
                  break;
               case ID_FUNC_EDITSTEPPLUS: case ID_FUNC_EDITSTEPMINUS:
                  pCmdUI->Enable((m_pActualPlot != NULL) && ((CCARA2DVApp*)AfxGetApp())->m_bEditable);
                  break;
               case ID_FUNC_PICKMODE: OnUpdateMouseMode(pCmdUI); break;
               case ID_FUNC_ZOOMMODE: OnUpdateMouseMode(pCmdUI); break;
               case ID_FUNC_EDITMODE: OnUpdateMouseMode(pCmdUI); break;
               case ID_FUNC_REFERENCE_SELECT:
                  pCmdUI->Enable(m_pActualPlot && (m_pActualPlot->GetReferenceCurve()!=m_PickList.GetLabel(0)));
                  break;
               case ID_FUNC_ZERO_REFERENCE:
                  bEnable = (m_pActualPlot && m_pActualPlot->GetReferenceCurve()) ? true : false;
                  pCmdUI->Enable(bEnable);
                  pCmdUI->SetCheck(bEnable && m_pActualPlot && m_pActualPlot->IsReferenceOn());
                  break;
               case ID_FUNC_NUMEDITMODE:
                  bEnable = (m_pActualPlot && ((CCARA2DVApp*)AfxGetApp())->m_bEditable) ? true : false;
                  pCmdUI->Enable(bEnable);
                  pCmdUI->SetCheck(bEnable && ((m_nMouseFlags&MK_NUMERICMODE)!=0));
                  break;
               case ID_FUNC_LINEEDITMODE:
                  bEnable = ((CCARA2DVApp*)AfxGetApp())->m_bEditable;
                  if (m_nMouseFlags & MK_NO_LINE_EDIT) bEnable = false;
                  pCmdUI->Enable(bEnable);
                  pCmdUI->SetCheck(bEnable && ((m_nMouseFlags&MK_LINEMODE)!=0));
                  break;
               case IDCANCEL: pCmdUI->Enable(); break;
               case ID_VIEW_HORIZONTAL:
               case ID_VIEW_VERTICAL:
                  if (pCmdUI->m_pMenu)
                  {
                     int nID = 0;
                     if (m_lTile & TILE_HORIZONTAL) nID = ID_VIEW_HORIZONTAL;
                     else                           nID = ID_VIEW_VERTICAL;
                     pCmdUI->m_pMenu->CheckMenuRadioItem(ID_VIEW_HORIZONTAL, ID_VIEW_VERTICAL, nID, MF_BYCOMMAND);
                  }
                  pCmdUI->Enable(true);
                  break;

               case ID_VIEW_SET_SIZE_NORMAL:
                  if (pCmdUI->m_pMenu!=NULL)
                  {
                     int nID = -1;
                     if ((m_ptViewSize.x == 100) && (m_ptViewSize.y == 100)) nID = ID_VIEW_SET_SIZE_NORMAL;
                     pCmdUI->m_pMenu->CheckMenuRadioItem(ID_VIEW_SET_SIZE_NORMAL, ID_VIEW_SET_SIZE_NORMAL, nID, MF_BYCOMMAND);
                     pCmdUI->Enable(true);
                  }break;
               case ID_VIEW_SET_SIZE_X_200_PC:
               case ID_VIEW_SET_SIZE_X_300_PC:
               case ID_VIEW_SET_SIZE_X_400_PC:
                  if (pCmdUI->m_pMenu)
                  {
                     int nID = 0;
                     switch(m_ptViewSize.x)
                     {  
                        case 100: nID = -1;break;
                        case 200: nID = 0; break;
                        case 300: nID = 1; break;
                        case 400: nID = 2; break;
                     }
                     pCmdUI->m_pMenu->CheckMenuRadioItem(0, 2, nID, MF_BYPOSITION);
                  }
                  pCmdUI->Enable(true);
                  break;
               case ID_VIEW_SET_SIZE_Y_200_PC:
               case ID_VIEW_SET_SIZE_Y_300_PC:
               case ID_VIEW_SET_SIZE_Y_400_PC:
                  if (pCmdUI->m_pMenu)
                  {
                     int nID = 0;
                     switch(m_ptViewSize.y)
                     {  
                        case 100: nID = -1;break;
                        case 200: nID = 0; break;
                        case 300: nID = 1; break;
                        case 400: nID = 2; break;
                     }
                     pCmdUI->m_pMenu->CheckMenuRadioItem(0, 2, nID, MF_BYPOSITION);
                  }
                  pCmdUI->Enable(true);
                  break;
               case ID_VIEW_CURVE:
               case ID_OPERATION_CREATECURVE:
                  pCmdUI->Enable(true);
                  break;
               default: bFound = false;
            }
         }
         if (bFound)
         {
            pCmdUI->m_bContinueRouting = FALSE;
            return true;
         }
      }
   }
   pDoc->CheckEditable();
   return CCaraView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

int CCARA2DVView::ShowOrHideCurves(CListDlg *pDlg, int nReason)
{
   CCARA2DVView    *pThis = (CCARA2DVView*) pDlg->m_pParam;
   CLabelContainer *plc = pThis->m_pActualPlot->GetCurveList();
   CCurveLabel     *pcl;
   int i, nCount = plc->GetCount();
   switch (nReason)
   {
      case ONSIZE:
      {
         LVCOLUMN Column = {LVCF_WIDTH, 0, 0, NULL, 0, 0, 0, 0};
         RECT rc;
         pDlg->m_List.GetClientRect(&rc);
         Column.cx = rc.right;
         pDlg->m_List.SetColumn(0, &Column);
      } break;
      case ONINITDIALOG:
      {
         CRect rc;
         pDlg->m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 
            LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
            LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
         pDlg->m_List.ModifyStyle(0, LVS_NOCOLUMNHEADER|LVS_EDITLABELS, 0);
         pDlg->m_List.GetClientRect(&rc);
         pDlg->m_List.InsertColumn(0, "", LVCFMT_LEFT, rc.right-2, 0);
         pDlg->m_List.SetWindowContextHelpId(IDC_SET_CURVE_VISIBLE|HID_BASE_DISPATCH);
         for (i=0; i<nCount;i++)
         {
            pcl = (CCurveLabel*) plc->GetLabel(i);
            pDlg->m_List.InsertItem(i, (const char*)pcl->GetText());
         }
         for (i=0; i<nCount;i++)
         {
            pcl = (CCurveLabel*) plc->GetLabel(i);
            if (pcl->IsVisible())
               pDlg->m_List.SetCheck(i, TRUE);
         }
      } break;
      case ONBEGINLABELEDITLSTLIST:
      {
         LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pDlg->m_pNMHdr;
         return 0;
      }
      case ONENDLABELEDITLSTLIST:
      {
         LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pDlg->m_pNMHdr;
         pDlg->m_List.SetItemData(pDispInfo->item.iItem, 1);
         return 1;
      }
      case ONENDDRAGLSTLIST:
      if (pDlg->m_pNMHdr)
      {
         NMLISTVIEW* pListView = (NMLISTVIEW*)pDlg->m_pNMHdr;
         if (pDlg->m_nCursel != pListView->iItem)
         {
            BOOL bChanged = FALSE;
            CLabel*pl = plc->GetLabel(pDlg->m_nCursel);
            if (pl)
            {
               plc->RemoveLabel(pl);
               if (pDlg->m_nCursel < pListView->iItem) pListView->iItem--;
               if (plc->InsertLabel(pl, pListView->iItem))
               {
                  bChanged = TRUE;
               }
               else
               {
                  plc->InsertLabel(pl);
               }
            }
            if (bChanged)
            {
               pDlg->m_List.DeleteAllItems();
               for (i=0; i<nCount;i++)
               {
                  pcl = (CCurveLabel*) plc->GetLabel(i);
                  pDlg->m_List.InsertItem(i, (const char*)pcl->GetText());
               }
               for (i=0; i<nCount;i++)
               {
                  pcl = (CCurveLabel*) plc->GetLabel(i);
                  if (pcl->IsVisible())
                     pDlg->m_List.SetCheck(i, TRUE);
               }
               CDC     *pDC = pThis->GetDC();
               pThis->OnPrepareDC(pDC);
               pThis->m_pActualPlot->SetChanged(true);
               pThis->InvalidateLabelRgn(pThis->m_pActualPlot, pDC);
               pThis->GetDocument()->SetModifiedFlag(true);
               pThis->ReleaseDC(pDC);
            }
         }
      }
      return 1;
      case ONOK:
      {
         bool bChanged = false;
         CDC     *pDC = pThis->GetDC();
         pThis->OnPrepareDC(pDC);
         for (i=0; i<nCount;i++)
         {
            pcl = (CCurveLabel*) plc->GetLabel(i);
            bool bVnow = pcl->IsVisible();
            bool bVnew = (pDlg->m_List.GetCheck(i) != 0) ? true : false;
            if (bVnow ^ bVnew)
            {
               bChanged = true;
               pcl->SetVisible(bVnew);
               if (bVnow && plc->IsPicked())
               {
                  pcl->ResetPickStates();
                  pcl->DrawShape(pDC, false);
                  pThis->m_PickList.RemoveLabel(pcl);
               }
            }
            if (pDlg->m_List.GetItemData(i))
            {
               pcl->GetString() = pDlg->m_List.GetItemText(i,0);
               bChanged = true;
            }
         }
         if (bChanged)
         {
            pThis->m_pActualPlot->SetChanged(true);
            pThis->InvalidateLabelRgn(pThis->m_pActualPlot, pDC);
            pThis->GetDocument()->SetModifiedFlag(true);
         }
         pThis->ReleaseDC(pDC);
      } break;
   }
   return 1;
}

void CCARA2DVView::ResetPickedLabels()
{
   int i, count = m_PickList.GetCount();
   if ((count != 0) || (HRGN(m_UpdateRgn) == NULL) || (m_pActualPlot == NULL))
   {
      CDC *pDC    = GetDC();
      OnPrepareDC(pDC);
      for (i=0; i< count; i++)
      {
         CLabel *pl = m_PickList.GetLabel(i);
         if (pl)
         {
            if (pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
            {
               CCurveLabel *pcl = (CCurveLabel*) pl;
               if (pcl->IsPointPicked())
               {
                  pcl->SetShapeDrawn(true);
                  pcl->DrawShape(pDC, false);
                  pcl->SetPointPicked(false);
                  pcl->SetShapeDrawn(true);
               }
            }
            pl->ResetPickStates();
            pl->DrawShape(pDC, false);
         }
      }

      if (HRGN(m_UpdateRgn))
      {
         InvalidateRgn(&m_UpdateRgn);
         m_UpdateRgn.DeleteObject();
      }
      m_PickList.RemoveAll();
      if (m_pActualPlot)
      {
         m_pActualPlot->ResetPickStates();
         m_pActualPlot->DrawShape(pDC, false);
         m_pActualPlot = NULL;
      }
      ReleaseDC(pDC);
   }
}

bool CCARA2DVView::GetPropertySheet()
{
   if (!m_pPropSheet)
   {
      m_pPropSheet = new CCaraViewProperties(IDS_PROPERTY_LINES, AfxGetMainWnd());
      if (!m_pPropSheet) return false;
      m_pPropSheet->m_pView = this;
   }
   return true;
}

inline void CCARA2DVView::SetMouseCapture()
{
   if (GetCapture() != this) SetCapture();
}

inline void CCARA2DVView::ReleaseMouseCapture()
{
   if (GetCapture() == this) ReleaseCapture();
}


int CCARA2DVView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   BEGIN("OnCreate");
   int nResult = CCaraView::OnCreate(lpCreateStruct);
   if (nResult != -1)
   {
      SetProp(m_hWnd, "CARA2DVIEW", (void*)0x0815);
   }
   
   if (m_nRenderFlag & DO_RENDER)                              // Rendern in eine Bitmap
   {
      unsigned int nThreadId = 0;
      try
      {
         HDC  hdc  = ::GetDC(m_hWnd);                          // Haupt Gerätekontext holen
         if (!hdc) throw (int) 1;
         m_hRenderDC[0]  = ::CreateCompatibleDC(hdc);          // Memory DC 1 erzeugen
         if (!m_hRenderDC[0]) throw (int) 2;
         m_hRenderDC[1]  = ::CreateCompatibleDC(hdc);          // Memory DC 2 erzeugen
         if (!m_hRenderDC[1]) throw (int) 2;
         CSize szView = GetScreenSize();                       // Memory Bmp 1 erzeugen
         m_hRenderBmp[0] = ::CreateCompatibleBitmap(hdc, szView.cx, szView.cy);
//         m_hRenderBmp[0] = CreateBitmap(szView.cx, szView.cy, 1, 24, NULL);
         if (!m_hRenderBmp[0]) throw (int) 3;                  // Memory Bmp 2 erzeugen
         m_hRenderBmp[1] = ::CreateCompatibleBitmap(hdc, szView.cx, szView.cy);
//         m_hRenderBmp[1] = CreateBitmap(szView.cx, szView.cy, 1, 24, NULL);
         if (!m_hRenderBmp[1]) throw (int) 3;
         ::SelectObject(m_hRenderDC[0], m_hRenderBmp[0]);// Bmp 1 in DC 1 selektieren
         ::SelectObject(m_hRenderDC[1], m_hRenderBmp[1]);// Bmp 2 in DC 2 selektieren
         ::ReleaseDC(m_hWnd, hdc);                             // Hauptgerätekontext freigeben
         m_hRenderEvent = ::CreateEvent(NULL, true, false, "DoRender");
         InitializeCriticalSection(&m_csRenderDC);
         m_nRenderFlag |= CRITICAL_SECT_INIT;
                                                               // Renderthread erzeugen
         m_hRenderThread = (HANDLE) _beginthreadex(NULL, 0, RenderThread, this, 0, &nThreadId);
         if (!m_hRenderThread) throw (int) 4;
         m_nRenderFlag |= CAN_RENDER;
      }
      catch (int nError)
      {
         DestroyRenderThread();
         REPORT("Error RenderThread %d", nError);
      }
   }

/*
   CCARA2DVApp *pApp = (CCARA2DVApp*) AfxGetApp();
   if (pApp->m_CmdInfo.m_bRunEmbedded)
   {
      SetTimer(OLE_DOC_UPDATE_TIMER, OLE_DOC_UPDATE_TIME, NULL);
   }
*/
   return nResult;
}

void CCARA2DVView::DestroyRenderThread()
{
   DWORD dwResult;
   m_nRenderFlag &= ~DO_RENDER;
   if (m_hRenderThread)
   {
      while ((::GetExitCodeThread(m_hRenderThread, &dwResult) != 0) && (dwResult == STILL_ACTIVE))
      {
         Sleep(20);
      }
      m_hRenderThread = 0;
   }
   m_nRenderFlag &= ~CAN_RENDER;

   if (m_hRenderDC[0])  dwResult = ::DeleteDC(m_hRenderDC[0]);
   if (m_hRenderDC[1])  dwResult = ::DeleteDC(m_hRenderDC[1]);
   if (m_hRenderBmp[0]) dwResult = ::DeleteObject(m_hRenderBmp[0]);
   if (m_hRenderBmp[1]) dwResult = ::DeleteObject(m_hRenderBmp[1]);
   if (m_hRenderEvent)  dwResult = ::CloseHandle(m_hRenderEvent);
   if (m_nRenderFlag & CRITICAL_SECT_INIT)
   {
      ::DeleteCriticalSection(&m_csRenderDC);
      m_nRenderFlag &= ~CRITICAL_SECT_INIT;
   }
   m_hRenderDC[0]  = NULL;
   m_hRenderDC[1]  = NULL;
   m_hRenderBmp[0] = NULL;
   m_hRenderBmp[1] = NULL;
}

unsigned __stdcall CCARA2DVView::RenderThread(void *pParameter)
{
   CCARA2DVView *pThis = (CCARA2DVView*) pParameter;

   do
   {
      ::WaitForSingleObject(pThis->m_hRenderEvent, 1000);
      if (pThis->m_nRenderFlag & NEW_RENDER)
      {
         CCARA2DVDoc* pDoc = pThis->GetDocument();
         CDC   dc;
         CRect rc;
         CBrush bkbrush;
         LOGBRUSH logbrush = {BS_SOLID, ((CCARA2DVApp*)AfxGetApp())->GetBackgroundColor(), 0};

         pThis->m_nRenderFlag &= ~NEW_RENDER;
         ::ResetEvent(pThis->m_hRenderEvent);

         pThis->GetClientRect(&rc);
         int nDC = (pThis->m_nRenderFlag & USE_FIRST_DC) ? 0 : 1;

         pThis->m_nRenderFlag |= RENDERING_NOW;
         dc.Attach(pThis->m_hRenderDC[nDC]);
         dc.SaveDC();

         bkbrush.CreateBrushIndirect(&logbrush);
         dc.SaveDC();
         dc.SelectObject(bkbrush);
         dc.PatBlt(0, 0, rc.Width(), rc.Height(), PATCOPY);
         pThis->OnPrepareDC(&dc);
         pDoc->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &dc, CCARA2DVView::UpdateChanges);
         pDoc->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &dc, CCARA2DVView::DrawLabel);
         dc.RestoreDC(-1);
         dc.Detach();
         pThis->m_nRenderFlag &= ~RENDERING_NOW;
         ::EnterCriticalSection(&pThis->m_csRenderDC);
         {
            if (pThis->m_nRenderFlag & USE_FIRST_DC)
            {
               pThis->m_nRenderFlag &= ~USE_FIRST_DC;
            }
            else
            {
               pThis->m_nRenderFlag |= USE_FIRST_DC;
            }
         }
         ::LeaveCriticalSection(&pThis->m_csRenderDC);
         pThis->InvalidateRect(NULL, true);
      }
   } while (pThis->m_nRenderFlag & DO_RENDER);
   return 0;
}

void CCARA2DVView::OnDestroy() 
{
   DestroyRenderThread();
   RemoveProp(m_hWnd, "CARA2DVIEW");
   KillTimer(MOUSE_HOVERTIMER);
   CCaraView::OnDestroy();
}

void CCARA2DVView::SetStatusPaneText(int nID, char *text)
{
   CCARA2DVApp *pApp = (CCARA2DVApp*)AfxGetApp();
   CMainFrame *pWnd =(CMainFrame*) pApp->m_pMainWnd;
   if (pWnd) pWnd->SetStatusPaneText(nID, text);
#ifdef ETS_OLE_SERVER
   if (pApp->m_pActiveWnd && pApp->m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CInPlaceFrame)))
   {
      ((CInPlaceFrame*)pApp->m_pActiveWnd)->SetStatusPaneText(nID, text);
   }
#endif
}

void CCARA2DVView::SetStepValue(bool bRound)
{
   if (m_pActualPlot)
   {
      char text[40] = "";
      ETSDIV_NUTOTXT ntx;
      int nY = 0;
      double dRound;
      CCurveLabel *pcl = NULL; 
      CLabel *pl = m_PickList.GetLabel(0);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         pcl = (CCurveLabel*) pl;
         if (pcl->IsPointPicked())
            nY = pcl->GetPoint().y;
      }
      m_dStepValue = m_pActualPlot->GetStepValue(m_nArrowCursorStep, nY, dRound);
      if (pcl && bRound) pcl->SetY_Value(pcl->GetMatchPoint(), dRound);

      ntx.nround = m_pActualPlot->GetYRound()+2;               // Rundungsstellen
      ntx.nmaxl  = ntx.nround + 5;
      ntx.nmode  = m_pActualPlot->GetYNumModeEx();             // Darstellungsmodus
      if (m_pActualPlot->NoYEndNulls()) ntx.nmode |= ETSDIV_NM_NO_END_NULLS;
      ntx.fvalue = m_dStepValue;
      ntx.dZero  = 0;
      if (CEtsDiv::NumberToText(&ntx, text))
      {
         const char * pszUnit = m_pActualPlot->GetYUnit();
         if (pszUnit) {strcat(text, " "); strcat(text, pszUnit);}
         SetStatusPaneText(1,text);
      }
   }
   else m_dStepValue = 0;
}

void CCARA2DVView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
/*
   LONG lExtraInfo = GetMessageExtraInfo();
   int nRep = nRepCnt;
   switch(nChar)
   {
      case 40:   mouse_event(MOUSEEVENTF_MOVE, 0,  nRep, 0, lExtraInfo); break; // down
      case 38:   mouse_event(MOUSEEVENTF_MOVE, 0, -nRep, 0, lExtraInfo); break; // up
      case 37:   mouse_event(MOUSEEVENTF_MOVE, -nRep, 0, 0, lExtraInfo); break; //left
      case 39:   mouse_event(MOUSEEVENTF_MOVE,  nRep, 0, 0, lExtraInfo); break; //right
      case 32:   mouse_event(MOUSEEVENTF_LEFTDOWN, 1, 0, 0, lExtraInfo); break; //right
   }
*/

   bool bKeyControl = true;
   switch(nChar)
   {
      case 40:   OnArrowDown(nRepCnt, nFlags);  break;
      case 38:   OnArrowUp(nRepCnt, nFlags);    break;
      case 37:   OnArrowLeft(nRepCnt, nFlags);  break;
      case 39:   OnArrowRight(nRepCnt, nFlags); break;
      case 27:   OnESC(nRepCnt, nFlags);        break;
      case  9:   OnTAB(nRepCnt, nFlags);        break;
      case 13:   OnEnter(nRepCnt, nFlags);      break;
      case 107:  OnPlus(); bKeyControl = false; break;
      case 109:  OnMinus();bKeyControl = false; break;
      default :  bKeyControl = false;           break;
   }
   if (bKeyControl && !m_bKeyControl)
   {
      ResetPickedLabels();
      m_bKeyControl = true;
   }

   if (nChar==17)
   {
      CPoint point;
      if (GetCursorPos((POINT*)&point))
      {
         ScreenToClient((POINT*)&point);
         CDC     *pDC = GetDC();
         OnPrepareDC(pDC);
         m_nMouseFlags |= MK_CONTROL;
         pDC->DPtoLP(&point);
         CheckCursorPosition(&point, pDC);
         ReleaseDC(pDC);
      }
   }

   CCaraView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCARA2DVView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//   if (nChar == 32) mouse_event(MOUSEEVENTF_LEFTUP, 1, 0, 0, GetMessageExtraInfo());
   if (nChar==17)
   {
      CPoint point;
      if (GetCursorPos((POINT*)&point))
      {
         ScreenToClient((POINT*)&point);
         CDC     *pDC = GetDC();
         OnPrepareDC(pDC);
         m_nMouseFlags &= ~MK_CONTROL;
         pDC->DPtoLP(&point);
         CheckCursorPosition(&point, pDC);
         ReleaseDC(pDC);
      }
   }
	
   CCaraView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCARA2DVView::OnArrowDown(UINT nRepCnt, UINT nFlags)
{
   CCARA2DVDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   CDC     *pDC = GetDC();
   CLabel  *pl;
   OnPrepareDC(pDC);
   if (m_pActualPlot)                     
   {
      pl = m_PickList.GetLabel(0);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         CCurveLabel *pcl = (CCurveLabel*)pl;
         if (pcl->IsPointPicked())                             // Wenn  CCurveLabel-Punkt markiert ist,
         {                                                     // editierbar ist und der Editiermodus eingeschaltet ist
            if (pcl->IsEditable() && (m_nMouseMode==ID_FUNC_EDITMODE)) 
            {
               pcl->DrawShape(pDC, false);                     // Cursor löschen
               pcl->GetRgn(RGN_OR, pDC, &m_UpdateRgn);         // alte Region der zu editierenden Kurvenpunkte merken
               CPoint pt = pcl->GetPoint();
               if (m_nMouseFlags&MK_NUMERICMODE)               // mit fester numerischer Schrittweite ändern 
               {
                  if (m_dStepValue==0.0) SetStepValue();
                  SCurve dp = pcl->GetDPoint();
                  dp.y -= m_dStepValue;
                  pcl->SetDPoint(dp, pt);
               }
               else                                               // oder mit x*Pixelschrittweite
               {
                  pt.y -= m_nArrowCursorStep;
                  pcl->SetPoint(pt);
               }
               pcl->GetRgn(RGN_OR, pDC, &m_UpdateRgn);            // neue Region merken
               pcl->DrawShape(pDC, true);                         // Cursor zeichnen und Position auf die Stausbar ausgeben
               MousePosToStatusbar(pDC, NULL, false, (LPDPOINT)&pcl->GetDPoint());
               ReleaseDC(pDC);
               GetDocument()->SetModifiedFlag(true);
               return;                                            // wenn editiert wurde 
            }
            else                                                  // ist er nicht editirbar
            {
               pcl->DrawShape(pDC, false);                        // nur den Cursor löschen
               pcl->SetShapeDrawn(true);                          // Sonderfall: Umriß löschen nochmal ermöglichen
            }
         }
         m_PickList.ResetStates();
         m_PickList.DrawShape(pDC, false);
         m_PickList.RemoveAll();
      }
      m_pActualPlot->SetPicked(false);
      m_pActualPlot->DrawShape(pDC, false);
   }

   CLabelContainer *pPlots = pDoc->GetLabelContainer();
   int index = pPlots->GetIndex(m_pActualPlot);
   do
   {
      pl = pPlots->GetLabel(++index);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CPlotLabel))) m_pActualPlot = (CPlotLabel*) pl;
      else                                               m_pActualPlot = NULL;
   }
   while (m_pActualPlot && !m_pActualPlot->IsVisible());

   if (m_pActualPlot)
   {
      m_pActualPlot->SetPicked(true);
      m_pActualPlot->DrawShape(pDC, true);
   }
   ReleaseDC(pDC);
}

void CCARA2DVView::OnArrowUp(UINT nRepCnt, UINT nFlags)
{
   CCARA2DVDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);
   CLabelContainer *pPlots = pDoc->GetLabelContainer();

   int count = pPlots->GetCount();
   if (!count) return;

   if (m_pActualPlot)
   {
      CLabel *pl = m_PickList.GetLabel(0);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         CCurveLabel *pcl = (CCurveLabel*)pl;
         if (pcl->IsPointPicked())
         {
            if (pcl->IsEditable() && (m_nMouseMode==ID_FUNC_EDITMODE))
            {
               pcl->DrawShape(pDC, false);
               pcl->GetRgn(RGN_OR, pDC, &m_UpdateRgn);
               CPoint pt = pcl->GetPoint();
               if (m_nMouseFlags&MK_NUMERICMODE)
               {
                  if (m_dStepValue==0.0) SetStepValue();
                  SCurve dp = pcl->GetDPoint();
                  dp.y += m_dStepValue;
                  pcl->SetDPoint(dp, pt);
               }
               else
               {
                  pt.y += m_nArrowCursorStep;
                  pcl->SetPoint(pt);
               }
               pcl->GetRgn(RGN_OR, pDC, &m_UpdateRgn);
               pcl->DrawShape(pDC, true);
               MousePosToStatusbar(pDC, NULL, false, (LPDPOINT)&pcl->GetDPoint());
               ReleaseDC(pDC);
               GetDocument()->SetModifiedFlag(true);
               return;
            }
            else
            {
               pcl->DrawShape(pDC, false);
               pcl->SetShapeDrawn(true);
            }
         }
         m_PickList.ResetStates();
         m_PickList.DrawShape(pDC, false);
         m_PickList.RemoveAll();
      }
      m_pActualPlot->SetPicked(false);
      m_pActualPlot->DrawShape(pDC, false);
   }

   int index = pPlots->GetIndex(m_pActualPlot);
   do
   {
      index--;
      if (index == -1) index = count;
      if (index == -2) index = count-1;
      CLabel *pl = pPlots->GetLabel(index);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CPlotLabel))) m_pActualPlot = (CPlotLabel*) pl;
      else                                               m_pActualPlot = NULL;
   }
   while (m_pActualPlot && !m_pActualPlot->IsVisible());

   if (m_pActualPlot)
   {
      m_pActualPlot->SetPicked(true);
      m_pActualPlot->DrawShape(pDC, true);
   }
   ReleaseDC(pDC);
}

void CCARA2DVView::OnArrowLeft(UINT nRepCnt, UINT nFlags)
{
   if (m_pActualPlot != NULL)
   {
      CDC     *pDC = GetDC();
      OnPrepareDC(pDC);
      CLabel *pl = m_PickList.GetLabel(0);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         CCurveLabel *pcl = (CCurveLabel*)pl;
         if (pcl->IsPointPicked())
         {
            bool bDraw = true;
            pcl->DrawShape(pDC, false);
            int nMatchPoint = pcl->GetMatchPoint()-1;
            if (!pcl->SetMatchPoint(nMatchPoint))
               return;
            CPoint pt = pcl->GetPoint(nMatchPoint);
            SCurve dp = pcl->GetDPoint(nMatchPoint);
            if (((m_nMouseFlags&MK_LINEMODE) !=0) && pcl->IsEditingLine())
            {
               SCurve dp2 = pcl->GetDPoint();
               dp2.x = dp.x;
               pcl->SetDPoint(dp2, pt);
               pcl->GetRgn(RGN_OR, pDC, &m_UpdateRgn);
            }
            else if (HRGN(m_UpdateRgn))
            {
               InvalidateRgn(&m_UpdateRgn);
               m_UpdateRgn.DeleteObject();
               bDraw = false;
            }
            if (bDraw) pcl->DrawShape(pDC, true);
            MousePosToStatusbar(pDC, NULL, false, (LPDPOINT)&pcl->GetDPoint());
            if (m_nMouseFlags&MK_NUMERICMODE) m_dStepValue = 0;
            return;
            ReleaseDC(pDC);
         }
         m_PickList.ResetStates();
         m_PickList.DrawShape(pDC, false);
         m_PickList.RemoveAll();
      }
      CLabelContainer *pCurves = m_pActualPlot->GetCurveList();
      int count = pCurves->GetCount();
      int index = pCurves->GetIndex(pl);
      do
      {
         index--;
         if (index == -1) index = count;
         if (index == -2) index = count-1;
         pl = pCurves->GetLabel(index);
      }
      while (pl && !pl->IsVisible());
      if (pl)
      {
         pl->SetPicked(true);
         pl->DrawShape(pDC, true);
         m_PickList.InsertLabel(pl);
         TestCurveEditMode();
      }
      ReleaseDC(pDC);
   }
}

void CCARA2DVView::OnArrowRight(UINT nRepCnt, UINT nFlags)
{
   if (m_pActualPlot != NULL)
   {
      CDC     *pDC = GetDC();
      OnPrepareDC(pDC);
      CLabel *pl = m_PickList.GetLabel(0);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         CCurveLabel *pcl = (CCurveLabel*)pl;
         if (pcl->IsPointPicked())
         {
            bool bDraw = true;
            pcl->DrawShape(pDC, false);
            int nMatchPoint = pcl->GetMatchPoint()+1;
            if (!pcl->SetMatchPoint(nMatchPoint))
               return;
            CPoint pt = pcl->GetPoint(nMatchPoint);
            SCurve dp = pcl->GetDPoint(nMatchPoint);
            if (((m_nMouseFlags&MK_LINEMODE) !=0) && pcl->IsEditingLine())
            {
               SCurve dp2 = pcl->GetDPoint();
               dp2.x = dp.x;
               pcl->SetDPoint(dp2, pt);
               pcl->GetRgn(RGN_OR, pDC, &m_UpdateRgn);
            }
            else if (HRGN(m_UpdateRgn))
            {
               InvalidateRgn(&m_UpdateRgn);
               m_UpdateRgn.DeleteObject();
               bDraw = false;
            }
            if (bDraw) pcl->DrawShape(pDC, true);
            MousePosToStatusbar(pDC, NULL, false, (LPDPOINT)&pcl->GetDPoint());
            if (m_nMouseFlags&MK_NUMERICMODE) m_dStepValue = 0;
            ReleaseDC(pDC);
            return;
         }
         m_PickList.ResetStates();
         m_PickList.DrawShape(pDC, false);
         m_PickList.RemoveAll();
      }
      CLabelContainer *pCurves = m_pActualPlot->GetCurveList();
      int count = pCurves->GetCount();
      int index = pCurves->GetIndex(pl);
      do
      {
         index++;
         pl = pCurves->GetLabel(index);
      }
      while (pl && !pl->IsVisible());
      if (pl)
      {
         pl->SetPicked(true);
         pl->DrawShape(pDC, true);
         m_PickList.InsertLabel(pl);
         TestCurveEditMode();
      }
      ReleaseDC(pDC);
   }
}

void  CCARA2DVView::OnESC(UINT, UINT)
{
   ResetPickedLabels();
}

void  CCARA2DVView::OnTAB(UINT, UINT)
{
   if (m_pActualPlot != NULL)
   {
      CDC     *pDC = GetDC();
      OnPrepareDC(pDC);
      CLabel *pl = m_PickList.GetLabel(0);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         CCurveLabel *pcl = (CCurveLabel*) pl;
         if (pcl->IsPointPicked())
         {
            pcl->SetShapeDrawn(true);
            pcl->DrawShape(pDC, false);
            pcl->SetPointPicked(false);
            pcl->SetShapeDrawn(true);
         }
         else
         {
            CRect rcView = m_pActualPlot->GetView();
            bool  bOK = false;
            if (rcView.PtInRect(pcl->GetPoint())) bOK = true;
            else
            {
               int i, nCount = pcl->GetNumValues();
               for (i=0; i<nCount; i++)
               {
                  if (rcView.PtInRect(pcl->GetPoint(i)))
                  {
                     bOK = true;
                     pcl->SetMatchPoint(i);
                     break;
                  }
               }
            }
            if (bOK)
            {
               pcl->SetPointPicked(true);
               pcl->SetShapeDrawn(false);
               pcl->DrawShape(pDC, true);
               m_dStepValue = 0;
               MousePosToStatusbar(pDC, NULL, false, (LPDPOINT)&pcl->GetDPoint());
            }
         }
      }
      ReleaseDC(pDC);
   }   
}

void  CCARA2DVView::OnPlus()
{
   m_nArrowCursorStep++;
   if (m_nMouseFlags&MK_NUMERICMODE)
   {
      double dStepValue = m_dStepValue;
      do
      {
         SetStepValue();
         if (m_nArrowCursorStep > 500) break;
         m_nArrowCursorStep++;
      }
      while(dStepValue == m_dStepValue);
   }
   else
   {
      char text[30];
      sprintf(text, "%d Pixel", m_nArrowCursorStep);
      SetStatusPaneText(1,text);
   }
}

void  CCARA2DVView::OnMinus()
{
   if (!--m_nArrowCursorStep) m_nArrowCursorStep=1;
   if (m_nMouseFlags&MK_NUMERICMODE)
   {
      double dStepValue = m_dStepValue;
      do
      {
         SetStepValue();
         if (m_nArrowCursorStep <= 1) break;
         m_nArrowCursorStep--;
      }
      while(dStepValue == m_dStepValue);
   }
   else
   {
      char text[30];
      sprintf(text, "%d Pixel", m_nArrowCursorStep);
      SetStatusPaneText(1,text);
   }
}

void CCARA2DVView::OnNumeditmode()
{
   if (m_nMouseFlags&MK_NUMERICMODE)
   {
      m_nMouseFlags^=MK_NUMERICMODE;
      char text[30];
      sprintf(text, "%d Pixel", m_nArrowCursorStep);
      SetStatusPaneText(1,text);
   }
   else
   {
      m_nMouseFlags|=MK_NUMERICMODE;
      SetStepValue();
   }
}

void CCARA2DVView::OnLineeditmode()
{
   if (m_nMouseFlags&MK_LINEMODE) m_nMouseFlags^=MK_LINEMODE;
   else                           m_nMouseFlags|=MK_LINEMODE;
}

void  CCARA2DVView::OnEnter(UINT, UINT)
{
   if (m_pActualPlot && ((m_nMouseFlags&MK_LINEMODE)!=0))
   {
      CCurveLabel *pcl = NULL; 
      CLabel *pl = m_PickList.GetLabel(0);
      if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         pcl = (CCurveLabel*) pl;
         if (pcl->IsPointPicked())
         {
            if (pcl->IsEditingLine())
            {
               pcl->UpdateLine();
               if (HRGN(m_UpdateRgn))
               {
                  InvalidateRgn(&m_UpdateRgn);
                  m_UpdateRgn.DeleteObject();
               }
            }
            else
            {
               pcl->SetLineStartPoint();
            }
         }
      }
   }
}

LRESULT CCARA2DVView::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
   if (lParam != 0)
   {
      AfxGetApp()->WinHelp(lParam);
   }
   return true;
}

LRESULT CCARA2DVView::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
   CDC     *pDC = GetDC();
   if (pDC) 
   {
      CPoint  p(LOWORD(lParam), HIWORD(lParam));
      CPlotLabel      *ppl;
      CCARA2DVDoc     *pDoc = GetDocument();
      ASSERT_VALID(pDoc);
      CLabelContainer  *pCurves;

      OnPrepareDC(pDC);
      pDC->DPtoLP(&p);

      CSize        szPP = CLabel::GetPickPointSize();
      ProcessLabel pl   = {NULL, 0, pDC, &p};
      pl.nPos = szPP.cx * szPP.cy;
      pDoc->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::HitLabel);
      if (pl.pl)
      {
         ppl = (CPlotLabel*) pl.pl;
         pCurves = ppl->GetCurveList();
         pl.pl = pCurves->HitLabel(&p, pDC);
         if (pl.pl && pl.pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
         {
            return HID_BASE_COMMAND + ID_VIEW_CURVE;
         }
         LRESULT lHelpCmd = ppl->GetHelpCmd(pDC, p);
         ReleaseDC(pDC);
         return lHelpCmd;
      }
      ReleaseDC(pDC);
   }
   return HID_BASE_RESOURCE+IDR_MAINFRAME;
}

int CCARA2DVView::GetPickedPlotIndex()
{
   CCARA2DVDoc* pDoc = GetDocument();
   ASSERT_VALID(pDoc);
   ProcessLabel pl = {m_pActualPlot, 0, NULL, NULL};
   pDoc->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::GetIndexFromLabel);
   return (pl.pl==NULL) ? pl.nPos : -1;
}

int CCARA2DVView::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
   int i=-1;
   if (pTI)
   {
      if (m_nMouseFlags&MK_LBUTTON)
         return -1;

      if (m_pHilightedLabel)
      {
         if (m_pHilightedLabel->IsPicked())
            return -1;
         return 0;
      }
      if (m_nMouseMode == ID_FUNC_ZOOMMODE)
      {
         i = 0;
         pTI->uId      = (UINT)m_hWnd;
//         pTI->uFlags  |= TTF_IDISHWND|TTF_FROMVIEW|TTF_ALWAYSTIP;
         pTI->uFlags  |= TTF_IDISHWND|TTF_FROMVIEW;
         pTI->hwnd     = AfxGetMainWnd()->m_hWnd;
         pTI->lpszText = LPSTR_TEXTCALLBACK;
      }
   }
   else
   {
      i = IDS_STATUS_PANE0;
   }
   return i;
}

BOOL CCARA2DVView::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	UINT nID = pNMHDR->idFrom;
   
   CPoint ptCursor;
   ::GetCursorPos(&ptCursor);
   CPoint ptClient(ptCursor);
   ScreenToClient(&ptClient);

   CCARA2DVDoc     *pDoc = GetDocument();
   CLabelContainer  *pplots = pDoc->GetLabelContainer();
   CSize        szPP = CLabel::GetPickPointSize();
   CDC     *pDC = GetDC();
   OnPrepareDC(pDC);
   pDC->DPtoLP(&ptClient);
   ProcessLabel pl   = {NULL, 0, pDC, &ptClient};
   pl.nPos = szPP.cx * szPP.cy;
   pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::HitLabel);

   if (pl.pl)
   {
      CPlotLabel *ppl = (CPlotLabel*) pl.pl;
      pl.pl = NULL;
      ppl->GetCurveList()->ProcessWithLabels(RUNTIME_CLASS(CCurveLabel), &pl, CLabelContainer::HitLabel);
      if (pl.pl)
      {
         int          nP;
         double       dValue;
         SCurve       dp1;
         CCurveLabel *pcl = (CCurveLabel*) pl.pl;
         ppl->TransformPoint(&ptClient, (DPOINT*)&dp1);
         CString strFormat;
         char szList[4];
         GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szList, 4);
         if (pcl->GetLocus()!=0)
         {
            nP = pcl->GetMatchPoint(); 
            if (nP != -1) dp1 = pcl->GetCurveValue(nP);
            else
            {
               dp1.y = pcl->Get_Y_Value(dp1.x, &nP);
               dp1.x = pcl->GetX_Value(nP);
            }
            char *pszLU = pcl->GetLocusUnit();
            if (!pszLU) pszLU = "";
            if (!_isnan(dp1.y))
            {
               strFormat.Format("%%.%dg %%s%c %%.%dg %%s%c %%.%dg %%s",ppl->GetXRound()+4, szList[0], ppl->GetXRound()+4, szList[0], ppl->GetYRound()+4);
               strTipText.Format(strFormat, pcl->GetLocusValue(nP), pszLU, dp1.x, ppl->GetXUnit(), dp1.y, ppl->GetYUnit());
            }
         }
         else
         {
            dValue = dp1.x;
            nP = pcl->GetMatchPoint(); 
            if (nP != -1) dp1 = pcl->GetCurveValue(nP);
            else
            {
               dp1.y = pcl->Get_Y_Value(dp1.x, &nP);
               dp1.x = pcl->GetX_Value(nP);
            }

            if (nP < (pcl->GetNumValues()-1))
            {
               SCurve dp2;
               dp2 = pcl->GetCurveValue(nP+1);
               if ((dValue - dp1.x) > (dp2.x - dValue))
               {
                  pcl->SetMatchPoint(nP+1);
                  dp1 = dp2;
               }
            }
            if (!_isnan(dp1.y))
            {
               strFormat.Format("%%.%dg %%s%c %%.%dg %%s",ppl->GetXRound()+4, szList[0], ppl->GetYRound()+4);
               strTipText.Format(strFormat, dp1.x, ppl->GetXUnit(), dp1.y, ppl->GetYUnit());
            }
         }

         m_pHilightedLabel = pcl;
      }
   }
   ReleaseDC(pDC);

   if (strTipText.IsEmpty())
   {
      return 0;
   }

#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, _countof(pTTTA->szText));
	else
		_mbstowcsz(pTTTW->szText, strTipText, _countof(pTTTW->szText));
#else

	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText, _countof(pTTTA->szText));
	else
		lstrcpyn(pTTTW->szText, strTipText, _countof(pTTTW->szText));

#endif

   return 0;
}
void CCARA2DVView::OnTimer(UINT nIDEvent) 
{

/*
   if (MOUSE_HOVERTIMER == nIDEvent)
   {
      if (m_nMouseHover == SET_TOOLTIP)
      {
         CPoint ptCursor;
         ::GetCursorPos(&ptCursor);
         CPoint ptClient(ptCursor);
         ScreenToClient(&ptClient);

         CCARA2DVDoc     *pDoc = GetDocument();
         CLabelContainer  *pplots = pDoc->GetLabelContainer();
         CSize        szPP = CLabel::GetPickPointSize();
         CDC     *pDC = GetDC();
         OnPrepareDC(pDC);
         pDC->DPtoLP(&ptClient);
         ProcessLabel pl   = {NULL, 0, pDC, &ptClient};
         pl.nPos = szPP.cx * szPP.cy;
         pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &pl, CLabelContainer::HitLabel);

         if (pl.pl)
         {
            CPlotLabel *ppl = (CPlotLabel*) pl.pl;
            pl.pl = NULL;
            ppl->GetCurveList()->ProcessWithLabels(RUNTIME_CLASS(CCurveLabel), &pl, CLabelContainer::HitLabel);
            if (pl.pl)
            {
               int          nP;
               double       dValue;
               SCurve       dp1;
               CCurveLabel *pcl = (CCurveLabel*) pl.pl;
               ppl->TransformPoint(&ptClient, (DPOINT*)&dp1);
               CString str, strFormat;
               char szList[4];
               GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szList, 4);
               if (pcl->GetLocus()!=0)
               {
                  nP = pcl->GetMatchPoint(); 
                  if (nP != -1) dp1 = pcl->GetCurveValue(nP);
                  else
                  {
                     dp1.y = pcl->Get_Y_Value(dp1.x, &nP);
                     dp1.x = pcl->GetX_Value(nP);
                  }
                  char *pszLU = pcl->GetLocusUnit();
                  if (!pszLU) pszLU = "";
                  strFormat.Format("%%.%dg %%s%c %%.%dg %%s%c %%.%dg %%s",ppl->GetXRound()+4, szList[0], ppl->GetXRound()+4, szList[0], ppl->GetYRound()+4);
                  str.Format(strFormat, pcl->GetLocusValue(nP), pszLU, dp1.x, ppl->GetXUnit(), dp1.y, ppl->GetYUnit());
               }
               else
               {
                  dValue = dp1.x;
                  nP = pcl->GetMatchPoint(); 
                  if (nP != -1) dp1 = pcl->GetCurveValue(nP);
                  else
                  {
                     dp1.y = pcl->Get_Y_Value(dp1.x, &nP);
                     dp1.x = pcl->GetX_Value(nP);
                  }

                  if (nP < (pcl->GetNumValues()-1))
                  {
                     SCurve dp2;
                     dp2 = pcl->GetCurveValue(nP+1);
                     if ((dValue - dp1.x) > (dp2.x - dValue))
                     {
                        pcl->SetMatchPoint(nP+1);
                        dp1 = dp2;
                     }
                  }
                  strFormat.Format("%%.%dg %%s%c %%.%dg %%s",ppl->GetXRound()+4, szList[0], ppl->GetYRound()+4);
                  str.Format(strFormat, dp1.x, ppl->GetXUnit(), dp1.y, ppl->GetYUnit());
               }
               pcl->DrawShape(pDC, true);
               pcl->DrawCursor(pDC, true);
               CEtsHelp::CreateContextWnd(LPCTSTR(str), ptCursor.x+10, ptCursor.y-20);
               Sleep(1000);
               pcl->DrawShape(pDC, false);
               pcl->DrawCursor(pDC, false);
            }
         }
         ReleaseDC(pDC);
      }
      else if (m_nMouseHover == END_TOOLTIP)
      {
         SetFocus();
      }
      if (m_nMouseHover <= END_TOOLTIP+1) m_nMouseHover++;
   }
*/
/*
   if (OLE_DOC_UPDATE_TIMER == nIDEvent)
   {
      CCARA2DVDoc *pDoc = GetDocument();
      if (pDoc->m_bOleChanged)
      {
         pDoc->UpdateAllItems(NULL);
         pDoc->m_bOleChanged = false;
      }
   }
*/
	CCaraView::OnTimer(nIDEvent);
}

void CCARA2DVView::InitMouseMode()
{
   if (m_nMouseMode == 0)
   {
      CCARA2DVDoc* pDoc = GetDocument();
      CLabelContainer  *pplots = pDoc->GetLabelContainer();
      int  count    = 0;
      pplots->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), &count, CLabelContainer::CountVisibleLabel);
      if (count)
      {
         CCARA2DVApp* pApp = (CCARA2DVApp*) AfxGetApp();
         if (pApp->m_bEditMode) m_nMouseMode = ID_FUNC_EDITMODE;
         else                   m_nMouseMode = ID_FUNC_PICKMODE;
      }
   }
}

void CCARA2DVView::OnViewSetSize(UINT nId) 
{
   CRect rcClient;
   CSize szView;
   int i, n = 1;

   if (((m_ptViewSize.x == 100) || (m_ptViewSize.y == 100)) && (nId != ID_VIEW_SET_SIZE_NORMAL)) n = 2;

   if (((m_ptViewSize.x != 100) || (m_ptViewSize.y != 100)) && (nId == ID_VIEW_SET_SIZE_NORMAL)) n = 2;

   switch (nId)
   {
      case ID_VIEW_SET_SIZE_NORMAL:   
         m_ptViewSize.x = 100;
         m_ptViewSize.y = 100;
         break;
      case ID_VIEW_SET_SIZE_X_200_PC: m_ptViewSize.x = 200; break;
      case ID_VIEW_SET_SIZE_X_300_PC: m_ptViewSize.x = 300; break;
      case ID_VIEW_SET_SIZE_X_400_PC: m_ptViewSize.x = 400; break;
      case ID_VIEW_SET_SIZE_Y_200_PC: m_ptViewSize.y = 200; break;
      case ID_VIEW_SET_SIZE_Y_300_PC: m_ptViewSize.y = 300; break;
      case ID_VIEW_SET_SIZE_Y_400_PC: m_ptViewSize.y = 400; break;
   }

   for (i=0; i<n; i++)
   {
      CDC *pdc = GetDC();
      OnPrepareDC(pdc);
      GetClientRect(&rcClient);
      CSize szScrollBar = GetScrollBarSize();
      pdc->DPtoLP(&rcClient);
      CPoint ptOff = rcClient.TopLeft();
      rcClient.OffsetRect(-ptOff);
      ReleaseDC(pdc);
   
      szView = rcClient.Size();
      szView.cx = abs(MulDiv(szView.cx, m_ptViewSize.x, 100));
      szView.cy = abs(MulDiv(szView.cy, m_ptViewSize.y, 100));
      SetScrollSizes(m_nMapMode, szView);
   }

   ResizePlotsToView();
}

void CCARA2DVView::ResetPickStatesQuiet()
{
   GetDocument()->GetLabelContainer()->ResetStates();
   m_PickList.ResetStates();
}


void CCARA2DVView::OnInitialUpdate() 
{
	CCaraView::OnInitialUpdate();
	
	EnableToolTips();
}

bool CCARA2DVView::InvalidateView(bool bRender)
{
   if (m_nRenderFlag & CAN_RENDER)
   {
      if (bRender)
      {
         m_nRenderFlag |= NEW_RENDER;
         ::SetEvent(m_hRenderEvent);
      }
      return true;
   }
   return false;
}

void CCARA2DVView::OnSetPlotMarker()
{
   if (m_pActualPlot)
   {
      int    nM;
      CPoint pt;
      DPOINT dp;
      double dM1, dM2;
      BOOL bMarker = m_pActualPlot->GetMarker(0, dM1) && m_pActualPlot->GetMarker(1, dM2) ? TRUE: FALSE;
      RANGE  r   = m_pActualPlot->GetRange();
      GetCursorPos(&pt);
      ScreenToClient(&pt);
      CDC     *pDC = GetDC();
      OnPrepareDC(pDC);
      pDC->DPtoLP(&pt);
      ReleaseDC(pDC);
      m_pActualPlot->TransformPoint(&pt, &dp);
      if (!bMarker)
      {
         dM1 = dM2 = dp.x;
         nM  = -1;
      }
      else if (fabs(dp.x-dM1) < fabs(dp.x-dM2))
      {
         nM = 0;
      }
      else
      {
         nM = 1;
      }
      CMarkerDlg dlg;
      dlg.m_dMin = r.x1;
      dlg.m_dMax = r.x2;
      dlg.m_dMarker = dp.x;
      if (dlg.DoModal() == IDOK)
      {
         if (nM == -1)
         {
            m_pActualPlot->SetMarker(0, dlg.m_dMarker);
            m_pActualPlot->SetMarker(1, dlg.m_dMarker);
         }
         else
         {
            if (nM == 0) dM1 = dlg.m_dMarker;
            else         dM2 = dlg.m_dMarker;
            if (dM1 > dM2)
            {
               swap(dM1, dM2);
            }
            m_pActualPlot->SetMarker(0, dM1);
            m_pActualPlot->SetMarker(1, dM2);
         }
      }
      else
      {
         m_pActualPlot->SetMarker(-1, 0.0);
      }
      InvalidateLabelRgn(m_pActualPlot);
   }
}

void CCARA2DVView::OnUpdateSetPlotMarker(CCmdUI *pCmdUI)
{
   pCmdUI->Enable(m_pActualPlot != NULL);
}

void CCARA2DVView::OnCalcAverage()
{
   CLabel *pl = m_PickList.GetLabel(0);
   if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
   {
      CCurveLabel*pC = (CCurveLabel*) pl;
      CPlotLabel* pP = pC->GetTransformation();
      double dAvg, dAvgSqr, dM1=0.0, dM2=0.0;
      CString str;
      int n;
      if (!pP->GetMarker(0, dM1))
      {
         dM1 = pC->GetX_Value(0);
      }
      if (!pP->GetMarker(1, dM2))
      {
         dM2 = pC->GetX_Value(pC->GetNumValues()-1);
      }
      dAvg    = pC->Average(0, dM1, dM2);
      dAvgSqr = pC->Average(1, dM1, dM2);
      CCurveLabel c1;
      c1.SetSize(2);
      c1.SetX_Value(0, dM1);
      c1.SetX_Value(1, dM2);
      c1.SetY_Value(0, dAvg);
      c1.SetY_Value(1, dAvg);
      c1.SetText("Mittelwert");
      CCurveLabel c2;
      c2.SetText("Quadratischer Mittelwert");
      c2.SetSize(2);
      c2.SetX_Value(0, dM1);
      c2.SetX_Value(1, dM2);
      c2.SetY_Value(0, dAvgSqr);
      c2.SetY_Value(1, dAvgSqr);
      pP->InsertCurve(&c1);
      pP->InsertCurve(&c2);
      InvalidateLabelRgn(pP);
      str.Format(IDS_AVG_VALUES, dM1, dM2, dAvg, dAvgSqr); 
      AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
      pP->RemoveCurve(&c1, n);
      pP->RemoveCurve(&c2, n);
      InvalidateLabelRgn(pP);
   }
}

void CCARA2DVView::OnUpdateCalcAverage(CCmdUI *pCmdUI)
{
   pCmdUI->Enable();
}

void CCARA2DVView::OnCalcIntegral()
{
   CLabel *pl = m_PickList.GetLabel(0);
   if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
   {
      CCurveLabel*pC = (CCurveLabel*) pl;
      CPlotLabel* pP = pC->GetTransformation();
      double dSum, dArea, dM1=0.0, dM2=0.0;
      CString str;
      if (!pP->GetMarker(0, dM1))
      {
         dM1 = pC->GetX_Value(0);
      }
      if (!pP->GetMarker(1, dM2))
      {
         dM2 = pC->GetX_Value(pC->GetNumValues()-1);
      }
      dSum  = pC->Integrate(0, dM1, dM2);
      dArea = pC->Integrate(1, dM1, dM2);
      str.Format(IDS_AREA_VALUES, dM1, dM2, dSum, dArea); 
      AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
   }
}

void CCARA2DVView::OnUpdateCalcIntegral(CCmdUI *pCmdUI)
{
   pCmdUI->Enable();
}

void CCARA2DVView::OnCalcSlope()
{
   CLabel *pl = m_PickList.GetLabel(0);
   if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
   {
      CCurveLabel*pC = (CCurveLabel*) pl;
      CPlotLabel* pP = pC->GetTransformation();
      double dY1, dY2, dSlope1, dSlope2, dM1=0.0, dM2=0.0;
      int n1=0, n2=0;
      CString str;
      if (!pP->GetMarker(0, dM1))
      {
         dM1 = pC->GetX_Value(0);
      }
      if (!pP->GetMarker(1, dM2))
      {
         dM2 = pC->GetX_Value(pC->GetNumValues()-1);
      }
      dSlope1 = pC->Slope(dM1, dY1, &n1);
      dSlope2 = pC->Slope(dM2, dY2, &n2);
      str.Format(IDS_SLOPE_VALUES, dM1, dSlope1, dY1, dM2, dSlope2, dY2); 
      AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
   }
}

void CCARA2DVView::OnUpdateCalcSlope(CCmdUI *pCmdUI)
{
   pCmdUI->Enable();
}

void CCARA2DVView::OnCalcZero()
{
   CLabel *pl = m_PickList.GetLabel(0);
   if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
   {
      CCurveLabel*pC = (CCurveLabel*) pl;
      CPlotLabel* pP = pC->GetTransformation();
      double  dM1=0.0, dM2=0.0, dY1, dY2;
      int n1=0, n2=0;
      CString str;
      if (!pP->GetMarker(0, dM1))
      {
         dM1 = pC->GetX_Value(0);
      }
      if (!pP->GetMarker(1, dM2))
      {
         dM2 = pC->GetX_Value(pC->GetNumValues()-1);
      }
      pC->FindZero(dM1);
      pC->FindZero(dM2);
      if (dM1 > dM2)
      {
         swap(dM1, dM2);
      }
      pP->SetMarker(0, dM1);
      pP->SetMarker(1, dM2);
      dY1 = pC->Get_Y_Value(dM1, &n1);
      dY2 = pC->Get_Y_Value(dM2, &n2);
      InvalidateLabelRgn(pP);
      str.Format(IDS_ZERO_POINTS, dM1, dY1, dM2, dY2);
      AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
   }
}

void CCARA2DVView::OnUpdateCalcZero(CCmdUI *pCmdUI)
{
   pCmdUI->Enable();
}
