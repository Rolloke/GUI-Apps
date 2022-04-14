// CARAPreview.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARADoc.h"
#include "CaraFrameWnd.h"
#include "CaraPreview.h"
#include "CaraView.h"
#include "resource.h"
#include "CMarginDialogParams.h"
#include "PictureLabel.h"
#include "LineLabel.h"
#include "RectLabel.h"
#include "TextLabel.h"
#include "PlotLabel.h"
#include "CurveLabel.h"
#include "BezierLabel.h"
#include "CircleLabel.h"
#include "PasteSelectivDlg.h"
#include "CatchGridDlg.h"
#include "TextEdit.h"
#include "CaraPrev.h"
#include "CaraMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ARROWCURSOR              0
#define ZOOMCURSOR               1
#define TRACKCURSOR              2
#define COPYCURSOR               3
#define OPENHANDCURSOR           4
#define SIZENSCURSOR             5
#define SIZEALLCURSOR            6
#define PENCURSOR                7
#define NODROPCURSOR             8
#define CROSSCURSOR              9
#define CREATE_LINE             10
#define CREATE_RECT             11
#define CREATE_BEZIER           12
#define CREATE_CIRCLE           13
#define CREATE_TEXT             14
#define REGKEY_PREVIEW          "Preview"
#define REGKEY_GRID             "Grid"
#define REGKEY_BARSTATE         "Barstate"
#define REGKEY_LABEL            "Label"
#define REGKEY_PRINTERCOLOR     "PrinterColor"

/////////////////////////////////////////////////////////////////////////////
// CCARAPreview


#define CBRS_ANY  (CBRS_BORDER_ANY|CBRS_ALIGN_ANY)

IMPLEMENT_DYNCREATE(CCARAPreview, CPreviewView)

CCARAPreview::CCARAPreview()
{
   m_nMouseMode   = ID_FUNC_PICK;
   ChangeCursor(m_nMouseMode);
   m_nSelect          = 0xF;
   m_nPreviewDCInit   = 0;
   m_pDC              = NULL;
   m_bCatchGrid       = false;
   m_bCopyLabel       = false;
   m_ptCatchGridOrg.x =  0;
   m_ptCatchGridOrg.y =  0;
   m_szCatchGrid.cx   = 10;
   m_szCatchGrid.cy   = 10;
   m_hActCursor       = NULL;
   m_OldMousePoint.x  = 0;
   m_OldMousePoint.y  = 0;

   m_nBarState.tb1CBRS    = (unsigned char)(CBRS_TOP >> 8);
   m_nBarState.tb1visible = true;
   m_nBarState.tb2CBRS    = (unsigned char)(CBRS_ALIGN_LEFT >> 8);
   m_nBarState.tb2visible = true;
   m_nBarState.menuvisible= true;

   CWinApp *pApp =AfxGetApp();
   BYTE * byteptr=NULL;
   UINT   size;
   pApp->GetProfileBinary(REGKEY_PREVIEW, REGKEY_GRID, &byteptr, &size);
   if (byteptr)
   {
      if (size == sizeof(GridStates))
      {
         m_bCatchGrid              = ((GridStates*)byteptr)->bCatchGrid;
         m_ptCatchGridOrg          = ((GridStates*)byteptr)->ptCatchGridOrg;
         m_szCatchGrid             = ((GridStates*)byteptr)->szCatchGrid;
      }
      delete[] byteptr;
   }

   CLabel::gm_bColorMatching = (pApp->GetProfileInt(REGKEY_PREVIEW, REGKEY_PRINTERCOLOR, 1) != 0) ? true : false;
   
   pApp->GetProfileBinary(REGKEY_PREVIEW, REGKEY_LABEL, &byteptr, &size);
   if (byteptr)
   {
      if (size == sizeof(LabelStates))
      {
         CTextLabel   text;
         CLineLabel   line;
         CRectLabel   rect;
         CBezierLabel bezier;
         CCircleLabel circle;
         LabelStates *pls = (LabelStates *)byteptr;
         CLabel::SetSaveGlobal(true);
         text.SetLogFont(   &pls->TextLF);
         text.SetTextColor(  pls->TextColor);
         text.SetBkColor(    pls->TextBkColor);
         text.SetTextAlign(  pls->TextAlign);
         text.SetLogPen(    &pls->TextLP);
         text.SetDrawFrame(  false);
         text.SetDrawBkGnd(  pls->TextBkGnd ? true : false);
         text.SetLogBrush(  NULL);
         line.SetLogPen(    &pls->LineLP);
         rect.SetLogPen(    &pls->RectLP);
         rect.SetBkColor(    pls->RectBkColor);
         rect.SetLogBrush(  &pls->RectBrush);
         bezier.SetLogPen(  &pls->BezierLP);
         circle.SetLogPen(  &pls->CircleLP);
         circle.SetBkColor(  pls->CircleBkColor);
         circle.SetLogBrush(&pls->CircleBrush);
         CLabel::SetSaveGlobal(false);
         COLORREF ShapePenColor = CLabel::GetShapePenColor();
         if (pls->ShapePenColor != ShapePenColor)
         {
            CLabel::SetShapePen(pls->ShapePenColor);
         }
      }
      delete[] byteptr;
   }
   m_pMainMenu = NULL;
}

CCARAPreview::~CCARAPreview()
{
   CWinApp *pApp =AfxGetApp();
   pApp->WriteProfileBinary(REGKEY_PREVIEW, REGKEY_GRID, (BYTE*)&m_bCatchGrid, sizeof(GridStates));
   pApp->WriteProfileInt(REGKEY_PREVIEW, REGKEY_PRINTERCOLOR, CLabel::gm_bColorMatching);
   UINT nBarState;
   memcpy(&nBarState, &m_nBarState, sizeof(UINT));
   pApp->WriteProfileInt(REGKEY_PREVIEW, REGKEY_BARSTATE, nBarState);
   ASSERT(m_nPreviewDCInit == 0);
   CTextLabel   text;
   CLineLabel   line;
   CRectLabel   rect;
   CBezierLabel bezier;
   CCircleLabel circle;
   LabelStates ls;
   ls.TextLF        = text.GetLogFont();
   ls.TextColor     = text.GetTextColor();
   ls.TextBkColor   = text.GetBkColor();
   ls.TextAlign     = text.GetTextAlign();
   ls.TextLP        = text.GetLogPen();
   ls.TextFrame     = text.DoDrawFrame();
   ls.TextBkGnd     = text.DoDrawBkGnd();
   ls.LineLP        = line.GetLogPen();
   ls.RectLP        = rect.GetLogPen();
   ls.RectBrush     = rect.GetLogBrush();
   ls.RectBkColor   = rect.GetBkColor();
   ls.BezierLP      = bezier.GetLogPen();
   ls.CircleLP      = circle.GetLogPen();
   ls.CircleBrush   = circle.GetLogBrush();
   ls.CircleBkColor = circle.GetBkColor();
   ls.ShapePenColor = CLabel::GetShapePenColor();
   pApp->WriteProfileBinary(REGKEY_PREVIEW, REGKEY_LABEL, (BYTE*)&ls, sizeof(LabelStates));
   if (m_pMainMenu)
      delete m_pMainMenu;
}


BEGIN_MESSAGE_MAP(CCARAPreview, CPreviewView)
	//{{AFX_MSG_MAP(CCARAPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_WM_DESTROY()
	ON_COMMAND(AFX_ID_PREVIEW_CLOSE, OnPreviewClose)
	ON_COMMAND(ID_VIEW_PRINTER_COLORS, OnColormatching)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PRINTER_COLORS, OnUpdateColorMatching)
	ON_COMMAND(IDX_CONTEXT_MENU, OnContextMenuAcc)
	ON_COMMAND(ID_VIEW_MENU, OnViewMenu)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MENU, OnUpdateViewMenu)
	ON_COMMAND(IDX_ADD_PAGE, OnAddPage)
	ON_COMMAND(IDX_REMOVE_PAGE, OnRemovePage)
	//}}AFX_MSG_MAP
   // Commands
   // Edit
 	ON_COMMAND(ID_EDIT_COPY          , OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT           , OnEditCut)
	ON_COMMAND(ID_EDIT_CLEAR         , OnEditDelete)
	ON_COMMAND(ID_EDIT_PASTE         , OnEditPaste)
	ON_COMMAND(ID_EDIT_PASTE_SELECTIV, OnEditPasteSelectiv)
	ON_COMMAND(ID_EDIT_PASTE_SPECIAL , OnEditPasteSpecial)
   // View
  	ON_COMMAND(ID_VIEW_TB            , OnViewTB)
	ON_COMMAND(ID_VIEW_TB_TOP        , OnViewTBTop)
	ON_COMMAND(ID_VIEW_TB_BOTTOM     , OnViewTBBottom)
	ON_COMMAND(ID_VIEW_TB1           , OnViewTB1)
	ON_COMMAND(ID_VIEW_TB1_LEFT      , OnViewTB1Left)
	ON_COMMAND(ID_VIEW_TB1_RIGHT     , OnViewTB1Right)
	ON_COMMAND(ID_VIEW_TB1_TOP       , OnViewTB1Top)
	ON_COMMAND(ID_VIEW_TB1_BOTTOM    , OnViewTB1Bottom)
   // Options
	ON_COMMAND(ID_OPTIONS_PICK       , OnOptionsPick)
	ON_COMMAND(ID_OPTIONS_TEXT       , OnOptionsText)
	ON_COMMAND(ID_OPTIONS_LINE       , OnOptionsLine)
	ON_COMMAND(ID_OPTIONS_RECT       , OnOptionsRectangle)
	ON_COMMAND(ID_OPTIONS_BEZIER     , OnOptionsBezier)
   ON_COMMAND(ID_OPTIONS_CIRCLE     , OnOptionsCircle)
	ON_COMMAND(ID_OPTIONS_GRID       , OnOptionsGrid)
   ON_COMMAND(ID_FUNC_PICK + ID_TB_RIGHT_EXT       , OnOptionsPick)
	ON_COMMAND(ID_FUNC_TEXT + ID_TB_RIGHT_EXT       , OnOptionsText)
	ON_COMMAND(ID_FUNC_LINE + ID_TB_RIGHT_EXT       , OnOptionsLine)
	ON_COMMAND(ID_FUNC_RECTANGLE + ID_TB_RIGHT_EXT  , OnOptionsRectangle)
	ON_COMMAND(ID_FUNC_BEZIER + ID_TB_RIGHT_EXT     , OnOptionsBezier)
   ON_COMMAND(ID_FUNC_CIRCLE + ID_TB_RIGHT_EXT     , OnOptionsCircle)
	ON_COMMAND(ID_FUNC_CATCH_GRID + ID_TB_RIGHT_EXT , OnOptionsGrid)
	ON_COMMAND(ID_FUNC_BEZIERCIRCLE+ ID_TB_RIGHT_EXT, OnOptionsCircle)
   // Functions
   ON_COMMAND(ID_FUNC_PICK          , OnFuncPick)
	ON_COMMAND(ID_FUNC_TEXT          , OnFuncText)
	ON_COMMAND(ID_FUNC_LINE          , OnFuncLine)
	ON_COMMAND(ID_FUNC_RECTANGLE     , OnFuncRectangle)
	ON_COMMAND(ID_FUNC_BEZIER        , OnFuncBezier)
	ON_COMMAND(ID_FUNC_CIRCLE        , OnFuncCircle)
	ON_COMMAND(ID_FUNC_BEZIERCIRCLE  , OnFuncBeziercircle)

	ON_COMMAND(ID_FUNC_UP            , OnFuncUp)
   ON_COMMAND(ID_FUNC_DOWN          , OnFuncDown)
	ON_COMMAND(ID_FUNC_HIGHEST       , OnFuncHighest)
	ON_COMMAND(ID_FUNC_LOWEST        , OnFuncLowest)
	ON_COMMAND(ID_FUNC_GROUP         , OnFuncGroup)
	ON_COMMAND(ID_FUNC_UNGROUP       , OnFuncUngroup)
	ON_COMMAND(ID_FUNC_CATCH_GRID    , OnFuncCatchGrid)
 
	ON_COMMAND(IDX_NO_LOGO, OnNoLogo)
   // Update commands
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_SELECTIV,OnUpdateEditPasteSelective)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_SPECIAL, OnUpdateEditPasteSpecial)

   ON_UPDATE_COMMAND_UI(ID_VIEW_TB        , OnUpdateViewTB)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TB_TOP    , OnUpdateViewTBTop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TB_BOTTOM , OnUpdateViewTBBottom)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TB1       , OnUpdateViewTB1)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TB1_LEFT  , OnUpdateViewTB1Left)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TB1_TOP   , OnUpdateViewTB1Top)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TB1_RIGHT , OnUpdateViewTB1Right)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TB1_BOTTOM, OnUpdateViewTB1Bottom)

   ON_UPDATE_COMMAND_UI(ID_FUNC_PICK, OnUpdateFunction)
	ON_UPDATE_COMMAND_UI(ID_FUNC_GROUP, OnUpdateFuncGroup)
	ON_UPDATE_COMMAND_UI(ID_FUNC_UNGROUP, OnUpdateFuncUngroup)
	ON_UPDATE_COMMAND_UI(ID_FUNC_CATCH_GRID, OnUpdateFuncCatchGrid)
	ON_UPDATE_COMMAND_UI(ID_FUNC_UP, OnUpdateFuncUp)
	ON_UPDATE_COMMAND_UI(ID_FUNC_DOWN, OnUpdateFuncDown)
	ON_UPDATE_COMMAND_UI(ID_FUNC_UNGROUP, OnUpdateFuncUngroup)
	ON_UPDATE_COMMAND_UI(ID_FUNC_TEXT, OnUpdateFunction)
	ON_UPDATE_COMMAND_UI(ID_FUNC_LINE, OnUpdateFunction)
	ON_UPDATE_COMMAND_UI(ID_FUNC_RECTANGLE, OnUpdateFunction)
	ON_UPDATE_COMMAND_UI(ID_FUNC_CIRCLE, OnUpdateFunction)
	ON_UPDATE_COMMAND_UI(ID_FUNC_BEZIERCIRCLE, OnUpdateFunction)
	ON_UPDATE_COMMAND_UI(ID_FUNC_BEZIER, OnUpdateFunction)
	ON_UPDATE_COMMAND_UI(ID_FUNC_HIGHEST, OnUpdateFuncUp)
	ON_UPDATE_COMMAND_UI(ID_FUNC_LOWEST, OnUpdateFuncDown)
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CCARAPreview 
extern "C" HINSTANCE g_hCaraPreviewInstance;

void CCARAPreview::OnInitialUpdate()
{
   CPreviewView::OnInitialUpdate();
}

void CCARAPreview::OnDraw(CDC* pDC)
{
   if (pDC->m_hAttribDC == NULL) return;
   ASSERT_VALID(pDC);

   // don't do anything if not fully initialized
   if (m_pPrintView == NULL || m_dcPrint.m_hDC == NULL) return;

   CPoint ViewportOrg = pDC->GetViewportOrg();
   CPen rectPen;
   rectPen.CreatePen(PS_SOLID, 2, GetSysColor(COLOR_WINDOWFRAME));
   CPen shadowPen;
   shadowPen.CreatePen(PS_SOLID, 3, GetSysColor(COLOR_BTNSHADOW));

   m_pPreviewInfo->m_bContinuePrinting = TRUE;                 // do this once each paint

   UINT nPage = 0;
   int nSavedState = m_dcPrint.SaveDC();                       // Save pristine state of DC
   m_pDC = pDC;
   GetPreviewDC();
   m_pPreviewInfo->m_nCurPage = m_nCurrentPage + nPage;
   SMarginDlgParam *pMdp = (SMarginDlgParam*)m_pPreviewInfo->m_lpUserData;

   // Only call PrepareDC if within page range, otherwise use default
   // rect to draw page rectangle
   if (m_nCurrentPage + nPage <= m_pPreviewInfo->GetMaxPage())
      m_pPrintView->OnPrepareDC(m_pPreviewDC, m_pPreviewInfo);
   
   if (m_pPreviewInfo->m_rectDraw.IsRectNull())
   {
      m_pPreviewInfo->m_rectDraw.SetRect(0, 0, m_pPreviewDC->GetDeviceCaps(HORZRES),
                                               m_pPreviewDC->GetDeviceCaps(VERTRES));
   }

   // Draw empty page on screen
   pDC->SaveDC();                                              // save the output dc state

   CSize* pRatio = &m_pPageInfo[nPage].sizeScaleRatio;
   CRect* pRect  = &m_pPageInfo[nPage].rectScreen;
   CRect* pRectDraw = NULL;

   if (pRatio->cx == 0)                                        // page position has not been determined
   {   
      PositionPage(nPage);                                     // compute page position
      if (m_nZoomState != ZOOM_OUT)  ViewportOrg = -GetDeviceScrollPosition();
   }

   pDC->SetMapMode(MM_TEXT);                                   // Page Rectangle is in screen device coords
   pDC->SetViewportOrg(ViewportOrg);
   pDC->SetWindowOrg(0, 0);
   // erase background to white (most paper is white)
   CRect rectFill = *pRect;
   rectFill.left += 1;
   rectFill.top += 1;
   rectFill.right -= 2;
   rectFill.bottom -= 2;
   // evtl. einen Brush mit Gitterpunkten einfügen ?
   // GetBrushOrg(), SetBrushOrg(), UnrealizeObject(),
   ::FillRect(pDC->m_hDC, rectFill, (HBRUSH)GetStockObject(WHITE_BRUSH));

   pDC->RestoreDC(-1);                                         // restore to synchronized state

   m_pPreviewDC->SetScaleRatio(pRatio->cx, pRatio->cy);        // Set scale ratio for this page

   CSize PrintOffset(pRect->left, pRect->top);
   PrintOffset += CSize(1, 1);
   PrintOffset += (CSize)ViewportOrg;                          // For Scrolling
   m_pPreviewDC->SetTopLeftOffset(PrintOffset);

   if (pMdp)
   {
      CRect rcPage(pMdp->rcPageLoMetric);
      CRect rcDraw(pMdp->rcDrawLoMetric);
      CRect rcPaint(rcPage.left, rcPage.top, rcPage.right, rcDraw.top);
      CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH));
      m_pPreviewDC->FillRect(&rcPaint, pBrush);
      rcPaint.top    = rcDraw.bottom;
      rcPaint.bottom = rcPage.bottom;
      m_pPreviewDC->FillRect(&rcPaint, pBrush);
      rcPaint.top    = rcDraw.top;
      rcPaint.bottom = rcDraw.bottom;
      rcPaint.right  = rcDraw.left;
      m_pPreviewDC->FillRect(&rcPaint, pBrush);
      rcPaint.left   = rcDraw.right;
      rcPaint.right  = rcPage.right;
      m_pPreviewDC->FillRect(&rcPaint, pBrush);
      pBrush->Detach();
   }

   if (m_bSizeChanged)
   {
      CSize szPick(3, 3);
      pDC->DPtoLP(&szPick);
      CLabel::SetPickPointSize(szPick);
      m_bSizeChanged = false;
   }


   CCaraView *pView = (CCaraView*) m_pPrintView;
   if (pView->m_nCurrentPage != -1) pView->m_nCurrentPage = m_nCurrentPage;
   pView->OnPrint(m_pPreviewDC, m_pPreviewInfo);
   ReleasePreviewDC();
//   TRACE("Page :%d\n", m_pPreviewInfo->m_nCurPage);


   m_dcPrint.RestoreDC(nSavedState);                           // restore to untouched state

   rectPen.DeleteObject();
   shadowPen.DeleteObject();
}

CDC *CCARAPreview::GetPreviewDC()
{
   if (m_nPreviewDCInit++ > 0)
   {
      ASSERT(m_pDC != NULL);
      return m_pDC;
   }
   if (!m_pDC) m_pDC = GetDC();
   if (m_pDC)
   {
      OnPrepareDC(m_pDC);
      if (m_pPreviewDC)
      {
         m_pPreviewDC->SetOutputDC(m_pDC->GetSafeHdc());
         m_pPreviewDC->SelectStockObject(ANSI_VAR_FONT);
         return m_pDC;
      }
   }
   return false;
}

HDC CCARAPreview::GetAttribDC()
{
   if (m_pPreviewDC)
   {
      ASSERT(m_pPreviewDC->m_hAttribDC != NULL);
      return m_pPreviewDC->m_hAttribDC;
   }
   ASSERT(false);
   return NULL;
}

void CCARAPreview::ReleasePreviewDC()
{
   if (--m_nPreviewDCInit > 0) return;
   if (m_pPreviewDC)
   {
      m_pPreviewDC->ReleaseOutputDC();
   }
   if (m_pDC)
   {
      ReleaseDC(m_pDC);
   }
   else ASSERT(false);
   m_pDC = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Diagnose CCARAPreview

#ifdef _DEBUG
void CCARAPreview::AssertValid() const
{
	CPreviewView::AssertValid();
}

void CCARAPreview::Dump(CDumpContext& dc) const
{
	CPreviewView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCARAPreview 
/******************************************************************************
* Name       : ChangeCursor                                                   *
* Definition : void ChangeCursor(UINT);                                       *
* Zweck      : Anpassen des Cursors an MausModi, Flags und Cursorposition.    *
* Aufruf     : ChangeCursor(nCursor);                                         *
* Parameter  :                                                                *
* nFlags (E) : CursorID                                              (UINT)   *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCARAPreview::ChangeCursor(UINT nCursor)
{
   static UINT oldCursor=100;

   if (oldCursor != nCursor)
   {
//      TRACE("ChangeCursor %d\n", nCursor);
      switch (nCursor)
      {
         case ZOOMCURSOR:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_ZOOMCURSOR));
            break;
         case TRACKCURSOR:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_TRACKCURSOR));
            break;
         case COPYCURSOR:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_COPYCURSOR));
            break;
         case OPENHANDCURSOR:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_OPENHANDCURSOR));
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
         case SIZEALLCURSOR:
            m_hActCursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
            break;
         case PENCURSOR:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_EDITCURSOR));
            break;
         case NODROPCURSOR:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_NODROPCURSOR));
            break;
         case ID_FUNC_LINE:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_CREATE_LINE));
            break;
         case ID_FUNC_RECTANGLE:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_CREATE_RECT));
            break;
         case ID_FUNC_BEZIER:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_CREATE_BEZIER));
            break;
         case ID_FUNC_CIRCLE:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_CREATE_CIRCLE));
            break;
         case ID_FUNC_BEZIERCIRCLE:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_CREATE_BCIRCLE));
            break;
         case ID_FUNC_TEXT:
            m_hActCursor = LoadCursor(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDC_CREATE_TEXT));
            break;
         case ID_FUNC_PICK:
         default :
            m_hActCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
            break;
      }
      oldCursor  = nCursor;
      SetCursor(m_hActCursor);
   }
}

bool CCARAPreview::AllocMainMenu()
{
   if (!m_pMainMenu)
   {
      CMenu Menu, Menu1, *pSub;
      MENUITEMINFO mii;
      TCHAR text[128];
      if (!Menu.LoadMenu(IDR_PREVIEW_CONTEXTMENU)) return false;// Context Menu laden
      m_pMainMenu = new CCaraMenu;                             // Main Menu erzeugen
      if (!m_pMainMenu) return false;
      pSub = Menu.GetSubMenu(0);                               // Submenu mit den Menueinträgen holen
      if (!pSub) return false;
      UINT i, nCount = pSub->GetMenuItemCount();               // Anzahl der Untermenues ermitteln
      if (nCount == 0) return false;
      if (!Menu1.CreateMenu()) return false;                   // Neues Menu erzeugen
      mii.cbSize = sizeof(MENUITEMINFO);
      mii.fMask  = MIIM_TYPE;
      mii.fType  = MFT_STRING;
      mii.dwTypeData = text;
      for (i=0; i<nCount; i++)
      {
         mii.cch    = 128;                                     // Textlänge initialisieren
         ::GetMenuItemInfo(pSub->m_hMenu, 0, true,  &mii);     // Einträge ermitteln und einfügen
         Menu1.InsertMenu(i, MF_BYPOSITION|MF_POPUP, (UINT) ::GetSubMenu(pSub->m_hMenu, 0), (LPCTSTR)mii.dwTypeData);
         pSub->RemoveMenu(0, MF_BYPOSITION);                   // aus dem alten Menu entfernen
      }
      if (CCaraMenu::gm_nOwnerdraw)                            // Bitmaps eintragen, wenn erwünscht
      {
         m_pMainMenu->AddToolBar(&m_PreviewToolbar);
         m_pMainMenu->SetMenuBitmaps(&Menu1);
      }
      m_pMainMenu->m_hMenu = Menu1.Detach();                   // Menü anhängen und aus dem Erzeugten entfernen
   }
   return true;
}

void CCARAPreview::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
   CPreviewView::OnActivateView(bActivate, pActivateView, pDeactiveView);
   if (bActivate)
   {
      CCaraFrameWnd *pWnd = (CCaraFrameWnd*)AfxGetMainWnd();
      if (pWnd)
      {
         pWnd->SetPreviewPaneInfo();
         CString str;
         str.LoadString(AFX_IDS_IDLEMESSAGE);
         pWnd->SetStatusPaneText(0, (TCHAR*)LPCTSTR(str));
         if (m_pToolBar)
         {
            pWnd->ShowControlBar(m_pToolBar, (m_nBarState.tb1visible) ? true: false, false);
         }
         m_PreviewToolbar.SendMessage(WM_IDLEUPDATECMDUI, (WPARAM)TRUE);
         if (m_nBarState.menuvisible && AllocMainMenu())
         {
            if (!::SetMenu(pWnd->m_hWnd, m_pMainMenu->m_hMenu))
            {
               m_nBarState.menuvisible = false;
               ::DestroyMenu(m_pMainMenu->Detach());
            }
         }
      }
   }
}

void CCARAPreview::OnDeactivateView()
{
   if (m_pMainMenu)
   {
      ::DestroyMenu(m_pMainMenu->Detach());
      delete m_pMainMenu;
      m_pMainMenu = NULL;
   }
}

void CCARAPreview::OnRButtonDown(UINT nFlags, CPoint point) 
{
   CPreviewView::OnRButtonDown(nFlags, point);
}

void CCARAPreview::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   CCARAPreview::OnLButtonDown(nFlags, point);
   CLabel *pl = m_PickList.GetLabel(0);
   if (pl)
   {
      if (pl->IsKindOf(RUNTIME_CLASS(CTextLabel)))
      {
         CTextEdit TEdlg;
         TEdlg.SetTextLabel((CTextLabel*)pl);
         CDC *pDC = GetPreviewDC();
         CSize szWnd = pDC->GetWindowExt();
         CSize szVp  = pDC->GetViewportExt();
         TEdlg.m_fRatio = (float)szVp.cx / (float)szWnd.cx;
         pl->GetRgn(RGN_OR, m_pPreviewDC, &m_PickUpdateRgn, INFLATE_RECT);
         ReleasePreviewDC();
         if (TEdlg.DoModal() == IDOK)
         {
            CDC *pDC = GetPreviewDC();
            if (TEdlg.m_bCalcRect)
            {
               ((CTextLabel*)pl)->InvalidateTextRect();
            }
            ProcessLabel prl = {NULL, 0, m_pPreviewDC, &((SMarginDlgParam*)m_pPreviewInfo->m_lpUserData)->rcDrawLoMetric};
            m_PickList.ProcessWithLabels(RUNTIME_CLASS(CTextLabel), &prl, CLabelContainer::ClipLabel);
            pl->GetRgn(RGN_OR, m_pPreviewDC, &m_PickUpdateRgn, INFLATE_RECT);
            ReleasePreviewDC();
         }
         UpdateLabelRgn(m_PickUpdateRgn, pDC);
         m_PickList.SetPicked(false);
         m_PickList.RemoveAll();
      }
   }
}
void CCARAPreview::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
   CCARAPreview::OnRButtonDown(nFlags, point);
}
void CCARAPreview::OnRButtonUp(UINT nFlags, CPoint point) 
{
   CPreviewView::OnRButtonUp(nFlags, point);
}
void CCARAPreview::OnLButtonDown(UINT nFlags, CPoint point)
{
   SetCapture();
   switch (m_nMouseMode)
   {
      case ID_FUNC_PICK:
         OnLButtonDownPick(nFlags, point);
      break;
      default:
         OnLButtonDownNew(nFlags, point);
      break;
   }
   CView::OnLButtonDown(nFlags, point);
}
void CCARAPreview::OnMouseMove(UINT nFlags, CPoint point)
{
   int nResult = 0;
   CDC *pDC = GetPreviewDC();

   CPoint ptPick = point;
   pDC->DPtoLP(&ptPick);
   CatchGrid(ptPick);
   CPoint ptDiff = ptPick - m_OldMousePoint;                             // Mausbewegungsvector berechnen
   m_OldMousePoint = ptPick;

   m_pPrintView->OnPrepareDC(m_pPreviewDC, m_pPreviewInfo);
   if (nFlags&MK_LBUTTON)
   {
      SMarginDlgParam *pMdp = (SMarginDlgParam*)m_pPreviewInfo->m_lpUserData;
      nResult = m_PickList.EditLabelPoints(&ptPick, &ptDiff, m_pPreviewDC, (CRect*)&pMdp->rcDrawLoMetric);
   }
   CCaraFrameWnd *pFrame =(CCaraFrameWnd*) AfxGetApp()->m_pMainWnd;
   if (pFrame)
   {
      TCHAR text[64];
      TCHAR szList[4];
      TCHAR szDecimal[4];
      GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szList, 4);
      GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDecimal, 4);
      ptPick.y = -ptPick.y;
      wsprintf(text, _T("(%d%c%d%c %d%c%d) mm"), ptPick.x/10, szDecimal[0], abs(ptPick.x%10), szList[0], ptPick.y/10, szDecimal[0], abs(ptPick.y%10));
      pFrame->SetStatusPaneText(1, text);

      if (nResult == 2) // Labelpunkte verändern
      {
         CRect rect = m_PickList.GetRect(pDC);
         ptPick.x = abs(rect.Width());
         ptPick.y = abs(rect.Height());
         wsprintf(text, _T(">>%d%c%d%c %d%c%d<< mm"), ptPick.x/10, szDecimal[0], abs(ptPick.x%10), szList[0], ptPick.y/10, szDecimal[0], abs(ptPick.y%10));
         pFrame->SetStatusPaneText(2, text);
      }
   }
   ReleasePreviewDC();

   CPreviewView::OnMouseMove(nFlags, point);
}
void CCARAPreview::OnLButtonUp(UINT nFlags, CPoint point)
{
   if (GetCapture() == this)
      ReleaseCapture();
   ChangeCursor(m_nMouseMode);
   switch (m_nMouseMode)
   {
      case ID_FUNC_PICK:
         OnLButtonUpPick(nFlags, point);
      break;
      default:
         OnLButtonUpNew(nFlags, point);
      break;
   }
   CCaraFrameWnd *pFrame =(CCaraFrameWnd*) AfxGetApp()->m_pMainWnd;
   if (pFrame) pFrame->SetStatusPaneText(2, "");
}

void CCARAPreview::OnLButtonDownPick(UINT nFlags, CPoint point)
{
   CLabelContainer *plc = GetDocumentLabels();
   if (plc)
   {
      int rValue;
      CDC *pDC = GetPreviewDC();
      CPoint ptPick = point;
      pDC->DPtoLP(&ptPick);

      if (m_PickList.SetMatchPoint(&ptPick))                   // Picken eines Labelpunktes
      {
         ChangeCursor(CROSSCURSOR);
      }
      else
      {
         rValue = m_PickList.PickLabel(&ptPick, plc, m_pPreviewDC, ((nFlags&MK_SHIFT)!=0) ? true : false, RGN_OR, &m_PickUpdateRgn);
         if (rValue&CL_NEWLABELPICKED)
         {
            m_PickList.GetRgn(RGN_OR, m_pPreviewDC, &m_PickUpdateRgn, INFLATE_RECT);
            UpdateLabelRgn(m_PickUpdateRgn, pDC);
            TRACE("New Label picked\n");
         }
         if (rValue&CL_LABELUNPICKED)
         {
            UpdateLabelRgn(m_PickUpdateRgn, pDC);
            TRACE("Label unpicked\n");
         }
         if      (rValue&CL_TRACKPICKRECT)
         {
            ChangeCursor(CROSSCURSOR);
            TRACE("Track Pickrect\n");
         }
         else if (rValue&CL_LABELPICKED)
         {
            ChangeCursor(SIZEALLCURSOR);
            TRACE("Label Picked\n");
         }
         if (((nFlags & MK_CONTROL) != 0) && (m_PickList.GetCount() > 0))
         {
            OnEditCopy();
            ChangeCursor(COPYCURSOR);
            m_bCopyLabel = true;
         }
      }
      ReleasePreviewDC();
   }
}
void CCARAPreview::OnLButtonUpPick(UINT nFlags, CPoint point)
{
   if (m_bCopyLabel)
   {
      OnEditPaste();
      m_bCopyLabel = false;
   }

   CLabelContainer *plc = GetDocumentLabels();
   if (plc)
   {
      int rValue;
      CDC *pDC = GetPreviewDC();
      CPoint ptPick(point);
      pDC->DPtoLP(&ptPick);
      CatchGrid(ptPick);
      rValue = m_PickList.OnPickRect(&ptPick, plc, m_pPreviewDC, RGN_OR, &m_PickUpdateRgn);

      if (rValue&CL_NEWLABELPICKED)
      {
         TRACE("New Label Picked Lb-up\n");
         m_PickList.GetRgn(RGN_OR, m_pPreviewDC, &m_PickUpdateRgn, INFLATE_RECT);
         UpdateLabelRgn(m_PickUpdateRgn, pDC);
      }
      if (rValue&CL_LABELUNPICKED)
      {
         TRACE("Label unpicked Lb-up\n");
         UpdateLabelRgn(m_PickUpdateRgn, pDC);
      }

      if (rValue&CL_LABELCHANGED)
      {
//         CCaraWinApp *pApp = (CCaraWinApp*)AfxGetApp();
//         pApp->SetUndoCmd(LAST_CMD_MOVE, pl, NULL, 0, NULL);
         m_pDocument->SetModifiedFlag(true);
      }

      if (m_pPreviewInfo->m_lpUserData)
      {
         ProcessLabel pl = {NULL, 1, m_pPreviewDC, &((SMarginDlgParam*)m_pPreviewInfo->m_lpUserData)->rcDrawLoMetric};
         plc->ProcessWithLabels(NULL, &pl, CLabelContainer::ClipLabel);
         pl.nPos = 0;
         plc->ProcessWithLabels(RUNTIME_CLASS(CTextLabel), &pl, CTextLabel::ClipTextRect);
         if (pl.nPos) m_PickList.RemoveAll();
      }
      ReleasePreviewDC();
   }
}


void CCARAPreview::OnLButtonDownNew(UINT nFlags, CPoint point)
{
   if (m_PickList.GetCount()) {TRACE("Liste enthält Label\n"); m_PickList.RemoveAll();}

   CDC *pDC = GetPreviewDC();
   CPoint ptPick = point;
   pDC->DPtoLP(&ptPick);
   CatchGrid(ptPick);

   CLabel *pLabel = NULL;

   switch(m_nMouseMode)				                           // Modus auswerten
   {
     case ID_FUNC_RECTANGLE:                                // Rechteckobjekt erstellen
        pLabel = new CRectLabel(&ptPick);
        break;
     case ID_FUNC_CIRCLE:                                   // Kreisobjekt erstellen
     {
        CRect rect(ptPick, CSize(0, 0));
        pLabel = new CCircleLabel(&rect);
     }  break;
     case ID_FUNC_BEZIERCIRCLE:                             // Bezierkreisobjekt erstellen
        pLabel = new CCircleLabel(&ptPick);
        break;
     case ID_FUNC_BEZIER:                                   // Bezierobjekt erstellen
        pLabel = new CBezierLabel(&ptPick);
        break;
     case ID_FUNC_LINE:                                     // Linienobjekt erstellen
        pLabel = new CLineLabel(&ptPick);
        break;
     case ID_FUNC_TEXT:
     {
        CRect rect(ptPick, CSize(200, -100));
        pLabel = new CTextLabel(&rect, "Text");
        pLabel->SetChanged(true);
        pLabel->SetDrawPickPoints(false);
     } break;
     default:
        break;
   }

   if (pLabel)
   {
      ASSERT_VALID(pLabel);
      ChangeCursor(CROSSCURSOR);
      pLabel->SetPicked(true);
      pLabel->SetPointPicked(true);
      pLabel->SetDrawPickPoints(true);
      pLabel->DrawShape(m_pPreviewDC, true);
      m_PickList.InsertLabel(pLabel);
   }

   ReleasePreviewDC();
}
void CCARAPreview::OnLButtonUpNew(UINT nFlags, CPoint point)
{
   CLabelContainer *plc = GetDocumentLabels();
   if (plc)
   {
      ASSERT(m_PickList.GetCount()==1);
      CLabel *pl  = m_PickList.GetLabel(0);
      m_PickList.RemoveAll();

      if (pl)
      {
         CDC *pDC = GetPreviewDC();

         if (pl->IsChanged() && plc->InsertLabel(pl))
         {
            CCaraWinApp *pApp = (CCaraWinApp*)AfxGetApp();
            pApp->SetUndoCmd(ID_EDIT_PASTE, pl, NULL, 0, NULL);
            m_pDocument->SetModifiedFlag(true);
            if (m_pPreviewInfo->m_lpUserData)
            {
               ProcessLabel prl = {NULL, 1, m_pPreviewDC, &((SMarginDlgParam*)m_pPreviewInfo->m_lpUserData)->rcDrawLoMetric};
               plc->ProcessWithLabels(NULL, &prl, CLabelContainer::ClipLabel);
               prl.nPos = 0;
               plc->ProcessWithLabels(RUNTIME_CLASS(CTextLabel), &prl, CTextLabel::ClipTextRect);
               if (prl.nPos) m_PickList.RemoveAll();
               else
               {
                  pl->GetRgn(RGN_OR, m_pPreviewDC, &m_PickUpdateRgn, INFLATE_RECT);
                  UpdateLabelRgn(m_PickUpdateRgn, pDC);
               }
            }
         }
         else
         {
            delete pl;
         }
         ReleasePreviewDC();
      }
   }
   else m_PickList.DeleteAll();
}

BOOL CCARAPreview::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
   if (m_hActCursor)
   {
      SetCursor(m_hActCursor);
      return true;
   }
   else return CView::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CCARAPreview::OnPreparePrinting(CPrintInfo* pInfo) 
{
   if (pInfo) return CPreviewView::OnPreparePrinting(pInfo);
   else
   {
      BOOL bResult = m_pPrintView->DoPreparePrinting(m_pPreviewInfo);
      if (bResult)
      {
         InvalidateRect(NULL);
      }
      return bResult;
   }
}

CSize CCARAPreview::CalcScaleRatio(CSize windowSize, CSize actualSize)
{
   TRACE("Wnd(%d, %d), Act(%d, %d)\n", windowSize.cx, windowSize.cy, actualSize.cx, actualSize.cy);
   return CPreviewView::CalcScaleRatio(windowSize, actualSize);
}

void CCARAPreview::OnSize(UINT nType, int cx, int cy) 
{
   CPreviewView::OnSize(nType, cx, cy);
   m_bSizeChanged = true;
}

int CCARAPreview::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CPreviewView::OnCreate(lpCreateStruct) == -1)
      return -1;

   CWinApp *pApp =AfxGetApp();
   UINT nBarState;
   memcpy(&nBarState, &m_nBarState, sizeof(DWORD));
   nBarState = pApp->GetProfileInt(REGKEY_PREVIEW, REGKEY_BARSTATE, nBarState);
   memcpy(&m_nBarState, &nBarState, sizeof(DWORD));

   UINT nStyle = 0;
   nStyle = m_nBarState.tb2CBRS << 8;
   if (m_nBarState.tb2visible) nStyle |= WS_VISIBLE;

   if (!m_PreviewToolbar.Create((CFrameWnd*)AfxGetApp()->m_pMainWnd,WS_CHILD|CBRS_SIZE_DYNAMIC|nStyle|CBRS_TOOLTIPS|CBRS_FLYBY, IDR_PREVIEWBAR) ||
                                !m_PreviewToolbar.LoadToolBar(IDR_PREVIEWBAR))
   {
         TRACE0("Failed to create toolbar\n");
         return -1;      // Fehler beim Erzeugen
   }
   m_PreviewToolbar.EnableDocking(CBRS_ALIGN_ANY);
   m_PreviewToolbar.GetToolBarCtrl().ModifyStyle(0,TBSTYLE_FLAT);

   if (m_pToolBar)
   {
      HICON hIcon;
      int nIDItem[] = {ID_FILE_PRINT_SETUP, AFX_ID_PREVIEW_PRINT, AFX_ID_PREVIEW_ZOOMOUT, AFX_ID_PREVIEW_ZOOMIN, AFX_ID_PREVIEW_CLOSE},
          nIDIcon[] = {IDI_PRT_SETUP      , IDI_PRINT           , IDI_ZOOM_OUT          , IDI_ZOOM_IN          , IDI_PREVIEW_CLOSE};        // Icons für die Buttons
      int   i, nCount = sizeof(nIDItem) / sizeof(int);

      for (i=0; i<nCount; i++)
      {
         hIcon = (HICON) LoadImage(g_hCaraPreviewInstance, MAKEINTRESOURCE(nIDIcon[i]), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
         ASSERT(hIcon != NULL);
         m_pToolBar->SendDlgItemMessage(nIDItem[i], BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
      }
//      m_pToolBar->m_sizeDefault.cy -= 16;
      m_pToolBar->EnableDocking(CBRS_ALIGN_ANY);
      nStyle = m_pToolBar->GetBarStyle();
      nStyle &= ~(CBRS_ALIGN_ANY|CBRS_BORDER_ANY);
      nStyle |= (m_nBarState.tb1CBRS << 8);
      m_pToolBar->SetBarStyle(nStyle);
   }

   CCaraFrameWnd *pWnd = (CCaraFrameWnd*)AfxGetMainWnd();
   if (pWnd) pWnd->EnableDocking(CBRS_ALIGN_ANY);

   return 0;
}

// Update Funktionen für die Toolbar
void CCARAPreview::OnUpdateFunction(CCmdUI* pCmdUI) { pCmdUI->SetCheck(pCmdUI->m_nID==m_nMouseMode);}
void CCARAPreview::OnUpdateFuncUp(CCmdUI* pCmdUI) 
{
   if (m_PickList.GetCount() == 1)
   {
      CLabelContainer *plc = GetDocumentLabels();
      if ((plc!=NULL) && (!plc->IsLast(m_PickList.GetLabel(0))))
      {
         pCmdUI->Enable(true);
         return;
      }
   }
   pCmdUI->Enable(false);
}
void CCARAPreview::OnUpdateFuncDown(CCmdUI* pCmdUI) 
{
   if (m_PickList.GetCount() == 1)
   {
      CLabelContainer *plc = GetDocumentLabels();
      if ((plc!=NULL) && (!plc->IsFirst(m_PickList.GetLabel(0))))
      {
         pCmdUI->Enable(true);
         return;
      }
   }
   pCmdUI->Enable(false);
}
void CCARAPreview::OnUpdateFuncGroup(CCmdUI* pCmdUI) {pCmdUI->Enable(m_PickList.GetCount() > 1);}
void CCARAPreview::OnUpdateFuncUngroup(CCmdUI* pCmdUI) 
{
   if (m_PickList.GetCount()==1)
   {
      CLabel *pl = m_PickList.GetLabel(0);
      if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
      {
         if (pl->GetCount() > 1)
         {
            pCmdUI->Enable(true);
            return;
         }
      }
   }
   pCmdUI->Enable(false);
}

void CCARAPreview::OnUpdateFuncCatchGrid(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(m_bCatchGrid);
}

void CCARAPreview::OnOptionsLine()
{
   CCaraViewProperties dlg(IDS_PROPERTIES, AfxGetMainWnd());
   CLineLabel line;
   line.SetPoint(CPoint(100,-100),1);
   dlg.InitDialog(0, &line);
   dlg.SetApplyNowButton(false);
   CLabel::SetSaveGlobal(true);
   dlg.DoModal();
   CLabel::SetSaveGlobal(false);
}
void CCARAPreview::OnOptionsRectangle()
{
   CCaraViewProperties dlg(IDS_PROPERTIES, AfxGetMainWnd());
   CRectLabel rect;
   rect.SetPoint(CPoint(100,-100),1);
   dlg.InitDialog(0, &rect);
   dlg.SetApplyNowButton(false);
   CLabel::SetSaveGlobal(true);
   dlg.DoModal();
   CLabel::SetSaveGlobal(false);
}
void CCARAPreview::OnOptionsText()
{
   CCaraViewProperties dlg(IDS_PROPERTIES, AfxGetMainWnd());
   CTextLabel text(&CPoint(0,0), "Text");
   dlg.InitDialog(0, &text);
   dlg.SetApplyNowButton(false);
   CLabel::SetSaveGlobal(true);
   dlg.DoModal();
   CLabel::SetSaveGlobal(false);
}
void CCARAPreview::OnOptionsBezier()
{
   CCaraViewProperties dlg(IDS_PROPERTIES, AfxGetMainWnd());
   CBezierLabel bezier(&CPoint(0, 0));
   bezier.SetMatchPoint(1);
   bezier.SetPoint(CPoint(100,-100));
   dlg.InitDialog(0, &bezier);
   dlg.SetApplyNowButton(false);
   CLabel::SetSaveGlobal(true);
   dlg.DoModal();
   CLabel::SetSaveGlobal(false);
}
void CCARAPreview::OnOptionsCircle()
{
   CCaraViewProperties dlg(IDS_PROPERTIES, AfxGetMainWnd());
   CCircleLabel circle(&CPoint(0, 0));
   circle.SetMatchPoint(1);
   circle.SetPoint(CPoint(100,-100));
   dlg.InitDialog(0, &circle);
   dlg.SetApplyNowButton(false);
   CLabel::SetSaveGlobal(true);
   dlg.DoModal();
   CLabel::SetSaveGlobal(false);
}
void CCARAPreview::OnOptionsPick() 
{
   CString string;
   COLORREF oldcolor = CLabel::GetShapePenColor();
   CColorDialog cd(oldcolor, 0, this);
   cd.m_cc.lpCustColors = g_CustomColors;
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   cd.m_cc.Flags       |=  CC_FULLOPEN;

   string.LoadString(IDS_PICKCOLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal()==IDOK)
   {
      COLORREF color = cd.GetColor();
      if (oldcolor != color) 
      {
         CLabel::SetShapePen(color);
         if (m_PickList.GetCount())
         {
            InvalidateRect(NULL);
         }
      }
   }
   g_AfxCommDlgProc = NULL;
}

void CCARAPreview::OnUpdateViewTB(CCmdUI* pCmdUI)        {pCmdUI->SetCheck(m_pToolBar && m_pToolBar->IsVisible());}
void CCARAPreview::OnUpdateViewTBTop(CCmdUI* pCmdUI)     {pCmdUI->SetCheck(m_pToolBar && ((m_pToolBar->GetBarStyle()&CBRS_ALIGN_TOP) != 0));}
void CCARAPreview::OnUpdateViewTBBottom(CCmdUI* pCmdUI)  {pCmdUI->SetCheck(m_pToolBar && ((m_pToolBar->GetBarStyle()&CBRS_ALIGN_BOTTOM) != 0));}

void CCARAPreview::OnUpdateViewTB1(CCmdUI* pCmdUI)       {pCmdUI->SetCheck(m_PreviewToolbar.IsVisible());}
void CCARAPreview::OnUpdateViewTB1Left(CCmdUI* pCmdUI)   {pCmdUI->SetCheck((m_PreviewToolbar.GetBarStyle()&CBRS_ALIGN_LEFT  ) != 0);}
void CCARAPreview::OnUpdateViewTB1Top(CCmdUI* pCmdUI)    {pCmdUI->SetCheck((m_PreviewToolbar.GetBarStyle()&CBRS_ALIGN_TOP   ) != 0);}
void CCARAPreview::OnUpdateViewTB1Right(CCmdUI* pCmdUI)  {pCmdUI->SetCheck((m_PreviewToolbar.GetBarStyle()&CBRS_ALIGN_RIGHT ) != 0);}
void CCARAPreview::OnUpdateViewTB1Bottom(CCmdUI* pCmdUI) {pCmdUI->SetCheck((m_PreviewToolbar.GetBarStyle()&CBRS_ALIGN_BOTTOM) != 0);}

void CCARAPreview::OnViewTB()
{
   CCaraFrameWnd *pWnd = (CCaraFrameWnd*)AfxGetMainWnd();
   if (pWnd)
   {
      m_nBarState.tb1visible = !m_pToolBar->IsVisible();
      pWnd->ShowControlBar(m_pToolBar, m_nBarState.tb1visible, false);
   }
}
void CCARAPreview::OnViewTBTop()
{
   CCaraFrameWnd *pWnd = (CCaraFrameWnd*)AfxGetMainWnd();
   if (pWnd)
   {
      m_pToolBar->SetBarStyle((m_pToolBar->GetBarStyle()&~CBRS_ANY)|(CBRS_ALIGN_TOP|CBRS_BORDER_TOP));
      pWnd->ShowControlBar(m_pToolBar, true, false);
      m_nBarState.tb1CBRS = (CBRS_ALIGN_TOP|CBRS_BORDER_TOP) >> 8;
   }
}
void CCARAPreview::OnViewTBBottom()
{
   CCaraFrameWnd *pWnd = (CCaraFrameWnd*)AfxGetMainWnd();
   if (pWnd)
   {
      m_pToolBar->SetBarStyle((m_pToolBar->GetBarStyle()&~CBRS_ANY)|(CBRS_ALIGN_BOTTOM|CBRS_BORDER_BOTTOM));
      pWnd->ShowControlBar(m_pToolBar, true, false);
      m_nBarState.tb1CBRS = (CBRS_ALIGN_BOTTOM|CBRS_BORDER_BOTTOM) >> 8;
   }
}

void CCARAPreview::OnViewTB1Left()
{
   CCaraFrameWnd *pWnd = (CCaraFrameWnd*)AfxGetMainWnd();
   if (pWnd)
   {
      m_PreviewToolbar.SetBarStyle((m_PreviewToolbar.GetBarStyle()&~CBRS_ALIGN_ANY)|CBRS_ALIGN_LEFT);
      pWnd->ShowControlBar(&m_PreviewToolbar, true, false);
      m_nBarState.tb2CBRS = CBRS_ALIGN_LEFT >> 8;
   }
}
void CCARAPreview::OnViewTB1Right()
{
   CCaraFrameWnd *pWnd = (CCaraFrameWnd*)AfxGetMainWnd();
   if (pWnd)
   {
      m_PreviewToolbar.SetBarStyle((m_PreviewToolbar.GetBarStyle()&~CBRS_ALIGN_ANY)|CBRS_ALIGN_RIGHT);
      pWnd->ShowControlBar(&m_PreviewToolbar, true, false);
      m_nBarState.tb2CBRS = CBRS_ALIGN_RIGHT >> 8;
   }
}
void CCARAPreview::OnViewTB1Top()
{
   CCaraFrameWnd *pWnd = (CCaraFrameWnd*)AfxGetMainWnd();
   if (pWnd)
   {
      m_PreviewToolbar.SetBarStyle((m_PreviewToolbar.GetBarStyle()&~CBRS_ALIGN_ANY)|CBRS_ALIGN_TOP);
      pWnd->ShowControlBar(&m_PreviewToolbar, true, false);
      m_nBarState.tb2CBRS = CBRS_ALIGN_TOP >> 8;
   }
}
void CCARAPreview::OnViewTB1Bottom()
{
   CCaraFrameWnd *pWnd = (CCaraFrameWnd*)AfxGetMainWnd();
   if (pWnd)
   {
      m_PreviewToolbar.SetBarStyle((m_PreviewToolbar.GetBarStyle()&~CBRS_ALIGN_ANY)|CBRS_ALIGN_BOTTOM);
      pWnd->ShowControlBar(&m_PreviewToolbar, true, false);
      m_nBarState.tb2CBRS = CBRS_ALIGN_BOTTOM >> 8;
   }
}
void CCARAPreview::OnViewTB1()
{
   CCaraFrameWnd *pWnd = (CCaraFrameWnd*)AfxGetMainWnd();
   if (pWnd)
   {
      m_nBarState.tb2visible = !m_PreviewToolbar.IsVisible();
      pWnd->ShowControlBar(&m_PreviewToolbar, m_nBarState.tb2visible, false);
   }
}

void CCARAPreview::OnFuncCircle()      {SetMouseMode(ID_FUNC_CIRCLE);}
void CCARAPreview::OnFuncLine()        {SetMouseMode(ID_FUNC_LINE);}
void CCARAPreview::OnFuncPick()        {SetMouseMode(ID_FUNC_PICK);}
void CCARAPreview::OnFuncRectangle()   {SetMouseMode(ID_FUNC_RECTANGLE);}
void CCARAPreview::OnFuncText()        {SetMouseMode(ID_FUNC_TEXT);}
void CCARAPreview::OnFuncBezier()      {SetMouseMode(ID_FUNC_BEZIER);}
void CCARAPreview::OnFuncBeziercircle(){SetMouseMode(ID_FUNC_BEZIERCIRCLE);}
void CCARAPreview::SetMouseMode(UINT nMode)
{
   if (m_nMouseMode == nMode) return;

   if (m_nMouseMode == ID_FUNC_PICK)
   {
      CDC *pDC = GetPreviewDC();
      if (m_PickList.PickLabel(NULL, NULL, pDC, false, RGN_OR, &m_PickUpdateRgn))
      {
         UpdateLabelRgn(m_PickUpdateRgn, pDC);
      }

      ReleasePreviewDC();
   }
   m_nMouseMode = nMode;
   ChangeCursor(m_nMouseMode);
}

void CCARAPreview::OnFuncGroup() 
{
   CLabelContainer *pDocLabel = GetDocumentLabels();           // DokumentenContainer
   if (pDocLabel)
   {
      CDC *pDC = GetPreviewDC();
      if (m_PickList.GetCount() > 1)
      {
         CLabelContainer *plc       = new CLabelContainer();   // einen Container anlegen
         if (plc)
         {
            CLabel *pl;
            int i, nTemp, nPos = pDocLabel->GetCount();        // Position auf max. setzen
            m_PickList.DrawShape(pDC, false);
            for (i=0; (pl = m_PickList.GetLabel(i))!= NULL; i++)// für alle gepickten Label
            {
               nTemp = pDocLabel->GetIndex(pl);                // Position des Label im DokumentenContainer ermitteln
               if (nTemp < nPos) nPos = nTemp;                 // Position des ersten Labels merken
               VERIFY(plc->InsertLabel(pl, 0));                // Label in die Gruppe einfügen
               pl->ResetPickStates();                          // Label unpicken
               pl->SetDrawPickPoints(false);                   // Pickpunkte nicht mitzeichnen
            }
            if (pDocLabel->InsertLabel(plc, nPos))             // den Container in das Document einfügen 
            {
               VERIFY(pDocLabel->RemoveList(&m_PickList));     // alle gepickten Label aus dem Document entfernen
               m_PickList.RemoveAll();                         // diese aus der Pickliste entfernen
               plc->SetDeleteContent(true);                    // Elemente werden von dem Container gelöscht !
               CRect rcGroup = plc->GetRect(pDC);              // umschließendes Rechteck berechnen
               ((CLabel*)plc)->SetPoint(rcGroup.TopLeft()    , 0);
               ((CLabel*)plc)->SetPoint(rcGroup.BottomRight(), 1);
               ProcessLabel prl = {plc, 0, NULL, NULL};        // Skalierungsart bestimmen
               plc->ProcessWithLabels(NULL, &prl, CLabelContainer::CheckScaling);
               if (prl.nPos & SCALE_PROPORTIONAL) plc->SetScaleSize(rcGroup.Size());
               plc->SetDrawPickPoints(true);                   // Pickpunkte des Containers zeichen
               plc->SetPicked(true);
               m_PickList.InsertLabel(plc);
               m_PickList.DrawShape(pDC, false);
               m_pDocument->SetModifiedFlag(true);
            }
            else                                               // nicht eingefügt
            {
               delete plc;                                     // GruppenContainer löschen
            }
         }
      }
      ReleasePreviewDC();
   }
}

void CCARAPreview::OnFuncUngroup() 
{
   CLabelContainer *pDocContainer = GetDocumentLabels();
   if (pDocContainer)
   {
      CDC *pDC = GetPreviewDC();
      if (m_PickList.GetCount() == 1)
      {
         m_PickList.DrawShape(pDC, false);
         CLabel *pl = m_PickList.GetLabel(0);
         if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))        // Ist es ein LabelContainer
         {
            CLabelContainer *plc = (CLabelContainer*) pl;
            m_PickList.RemoveAll();                               // Label der Pickliste entfernen
            int nPos = pDocContainer->GetIndex(pl);
            if (nPos != -1 && pDocContainer->RemoveLabel(pl))
            {
               plc->ResetPickStates();                            // alle Label unpicken
               for (int i=0; (pl = plc->GetLabel(i))!= NULL; i++)
               { 
                  pDocContainer->InsertLabel(pl, nPos++);         // das Label aus dem Container in das Document einfügen
                  if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer))) continue;
                  if (pl->IsKindOf(RUNTIME_CLASS(CTextLabel)) && 
                      !((CTextLabel*)pl)->IsMultiLineText())        continue;
                  pl->SetDrawPickPoints(true);                    // Zeichen der Pickpunkte wieder einschalten
               }
               plc->SetPicked(true);
               m_PickList.InsertList(plc);
               m_PickList.DrawShape(pDC, true);
               plc->RemoveAll();                                  // alle Label aus dem Container entfernen
               delete plc;                                        // den Container löschen
               m_pDocument->SetModifiedFlag(true);
            }
         }
      }
      ReleasePreviewDC();
   }
}
void CCARAPreview::OnFuncUp() 
{
   if (m_PickList.GetCount() == 1)
   {
      CLabelContainer *plc = GetDocumentLabels();
      if (plc)
      {
         CLabel *pl = m_PickList.GetLabel(0);

         CRgn rgn;
         CDC *pDC = GetPreviewDC();

         if (plc->SwapUp(pl, pDC, &rgn))
            UpdateLabelRgn(rgn, pDC);

         m_pDocument->SetModifiedFlag(true);

         ReleasePreviewDC();
      }
   }
}
void CCARAPreview::OnFuncDown() 
{
   if (m_PickList.GetCount() == 1)
   {
      CLabelContainer *plc = GetDocumentLabels();
      if (plc)
      {
         CLabel *pl = m_PickList.GetLabel(0);

         CRgn rgn;
         CDC *pDC = GetPreviewDC();
         if (plc->SwapDown(pl, pDC, &rgn))
            UpdateLabelRgn(rgn, pDC);

         m_pDocument->SetModifiedFlag(true);

         ReleasePreviewDC();
      }
   }
}
void CCARAPreview::OnFuncHighest() 
{
   if (m_PickList.GetCount() == 1)
   {
      CLabelContainer *plc = GetDocumentLabels();
      if (plc)
      {
         CLabel *pl = m_PickList.GetLabel(0);

         CRgn rgn;
         CDC *pDC = GetPreviewDC();

         if (plc->SwapLast(pl, pDC, &rgn))
            UpdateLabelRgn(rgn, pDC);

         m_pDocument->SetModifiedFlag(true);

         ReleasePreviewDC();
      }
   }
}
void CCARAPreview::OnFuncLowest() 
{
   if (m_PickList.GetCount() == 1)
   {
      CLabelContainer *plc = GetDocumentLabels();
      if (plc)
      {
         CLabel *pl = m_PickList.GetLabel(0);

         CRgn rgn;
         CDC *pDC = GetPreviewDC();

         if (plc->SwapFirst(pl, pDC, &rgn))
            UpdateLabelRgn(rgn, pDC);

         m_pDocument->SetModifiedFlag(true);

         ReleasePreviewDC();
      }
   }
}

void CCARAPreview::OnFuncCatchGrid() {m_bCatchGrid = !m_bCatchGrid;}

void CCARAPreview::OnOptionsGrid() 
{
   CCatchGridDlg dlg;
   dlg.m_fGridOrgX  = m_ptCatchGridOrg.x * 0.1f;
   dlg.m_fGridOrgY  = m_ptCatchGridOrg.y * 0.1f;
   dlg.m_fGridstepX = m_szCatchGrid.cx   * 0.1f;
   dlg.m_fGridstepY = m_szCatchGrid.cy   * 0.1f;

   if (dlg.DoModal()==IDOK)
   {
      m_ptCatchGridOrg.x = (int) (dlg.m_fGridOrgX  * 10.0f);
      m_ptCatchGridOrg.y = (int) (dlg.m_fGridOrgY  * 10.0f);
      m_szCatchGrid.cx   = (int) (dlg.m_fGridstepX * 10.0f);
      m_szCatchGrid.cy   = (int) (dlg.m_fGridstepY * 10.0f);
   }
}

void CCARAPreview::OnContextMenu(CWnd* pWnd, CPoint point)
{
   CLabelContainer *plc  = GetDocumentLabels();
   CDC *pDC = GetPreviewDC();
   ProcessLabel pl1   = {NULL, 0, m_pPreviewDC, NULL};

   if (m_PickList.GetCount() == 1)
   {
      pl1.pl = m_PickList.GetLabel(0);
   }

   if ((point.x != -1) && (point.x != -1) && (plc != NULL))
   {
      CPoint ptPick(point);
      ScreenToClient(&ptPick);
      pDC->DPtoLP(&ptPick);
      if (pl1.pl && (pl1.pl->IsOnLabel(pDC, &ptPick, NULL)==CLabel::IsOutside()))
      {
         pl1.pl = NULL;
      }
      if (pl1.pl == NULL)
      {
         pl1.pParam2 = &ptPick;
         CSize        szPP = CLabel::GetPickPointSize();
         pl1.nPos = szPP.cx * szPP.cy;
         plc->ProcessWithLabels(NULL, &pl1, CLabelContainer::HitLabel);
      }
   }

   if (pl1.pl)
   {
      pl1.pl->GetRgn(RGN_OR, m_pPreviewDC, &m_PickUpdateRgn);
      CCaraViewProperties dlg(IDS_PROPERTIES, AfxGetMainWnd());
      dlg.InitDialog(0, pl1.pl);
      dlg.SetApplyNowButton(false);
      dlg.m_pView = this;
      if (dlg.DoModal()==IDOK)
      {
         if (m_pPreviewInfo->m_lpUserData)
         {
            ProcessLabel pl2 = {NULL, 0, m_pPreviewDC, &((SMarginDlgParam*)m_pPreviewInfo->m_lpUserData)->rcDrawLoMetric};
            if (pl1.pl && pl1.pl->IsPicked()) pl2.nPos = 1;
            plc->ProcessWithLabels(NULL, &pl2, CLabelContainer::ClipLabel);
            pl2.nPos = 0;
            plc->ProcessWithLabels(RUNTIME_CLASS(CTextLabel), &pl2, CTextLabel::ClipTextRect);
            if (pl2.nPos) m_PickList.RemoveAll();
         }
         if (pl1.pl->GetRgn(RGN_OR, m_pPreviewDC, &m_PickUpdateRgn))
            UpdateLabelRgn(m_PickUpdateRgn, pDC);
         m_pDocument->SetModifiedFlag(true);
      }
   }
   else
   {
      CPtrList TB;
      TB.AddTail((void*)&m_PreviewToolbar);
      if ((point.x == -1) && (point.x == -1))
      {
         CRect rcClient;
         GetClientRect(&rcClient);
         point.x = rcClient.left;
         point.y = rcClient.top;
         ClientToScreen(&point);
      }
      CCaraMenu::ContextMenu(IDR_PREVIEW_CONTEXTMENU, &TB, TPM_LEFTALIGN, point.x, point.y, AfxGetMainWnd());
   }
   ReleasePreviewDC();
}

void CCARAPreview::UpdateLabelRgn(CRgn & rgn, CDC *pDC)
{
   if (HRGN(rgn)!= NULL)
   {
      InvalidateRgn(&rgn, false);                           // und updaten
      rgn.DeleteObject();                                   // Objekt löschen
   }
}
void CCARAPreview::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(m_PickList.GetCount() >= 1);
}
void CCARAPreview::OnUpdateEditCut(CCmdUI* pCmdUI)    
{
   pCmdUI->Enable(m_PickList.GetCount() >= 1);
}
void CCARAPreview::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_PickList.GetCount() >= 1);
}
void CCARAPreview::OnUpdateEditPasteSelective(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(true);
}
void CCARAPreview::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
   ((CCaraFrameWnd*)AfxGetMainWnd())->OnUpdateEditPaste(pCmdUI);
}
void CCARAPreview::OnUpdateEditPasteSpecial(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable((m_nMouseMode==ID_FUNC_PICK) && (m_PickList.GetCount()==0));
}
int CCARAPreview::SetChanged(CLabel*pl, void*)
{
   if (pl) pl->SetChanged(true);
   return 0;
}

void CCARAPreview::OnEditCopy() 
{
   CCaraWinApp *pApp = (CCaraWinApp*)AfxGetApp();
   CWnd *pWnd        = pApp->m_pMainWnd;
   if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CCaraFrameWnd)))
   {
      CCaraFrameWnd *pCFW = (CCaraFrameWnd*)pWnd;
      CLabel *pl = NULL;
      int nCount = m_PickList.GetCount();
      if (nCount == 1) pl = m_PickList.GetLabel(0);
      else if (nCount > 1)
      {
         pl = &m_PickList;
      }
      if (pl)
      {
         bool bContainer = (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)) != 0);
         if (bContainer) ((CLabelContainer*)pl)->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), NULL, SetChanged);
         else            pl->SetChanged(true);

         CDC *pDC = GetDC();
         pDC->SetMapMode(m_pPreviewDC->GetMapMode());
         pApp->InsertIntoClipboard(pl, pDC, true, true);
         ReleaseDC(pDC);

         if (bContainer) ((CLabelContainer*)pl)->ProcessWithLabels(RUNTIME_CLASS(CPlotLabel), NULL, SetChanged);
         else            pl->SetChanged(true);
      }
   }
}

void CCARAPreview::OnEditCut() 
{
   OnEditCopy();
   OnEditDelete();
}

void CCARAPreview::OnEditDelete() 
{
   if (m_pDocument)
   {
      CLabelContainer *plc = GetDocumentLabels();
      if (plc)
      {
         CLabel *pl = NULL;
         while ((pl = m_PickList.GetLabel(0)) != NULL)
         {
            if (plc->RemoveLabel(pl))
            {
               m_pDocument->UpdateAllViews(NULL, UPDATE_PREVIEW_LABEL|UPDATE_DELETE_LABEL, pl);
               m_pDocument->SetModifiedFlag(true);
               CCaraWinApp *pApp = (CCaraWinApp*)AfxGetApp();
               if (pl->DoUndoCmd()) pApp->SetUndoCmd(ID_EDIT_CLEAR, pl, NULL, 0, NULL);
               delete pl;
            }
         }
      }
   }
}
void CCARAPreview::OnEditPasteSelectiv() 
{
   CPasteSelectivDlg dlg;
   if (dlg.DoModal()==IDOK)
   {
      switch(dlg.m_nSelect)
      {
         case 0: m_nSelect = CARA_LABEL; break;
         case 1: m_nSelect = META_LABEL; break;
         case 2: m_nSelect = DIB_LABEL;  break;
         case 3: m_nSelect = BMP_FILE;   break;
      }
      OnEditPaste();
      m_nSelect = 0xF;
   }
}

void CCARAPreview::OnEditPasteSpecial() 
{
   m_nSelect = SPECIAL_LABEL;
   OnEditPaste();
   m_nSelect = 0xF;
}

void CCARAPreview::OnEditPaste()
{
   CCaraWinApp *pApp = (CCaraWinApp*)AfxGetApp();
   if (pApp->m_pMainWnd && pApp->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CCaraFrameWnd)))
   {
      CCaraFrameWnd   *pCFW   = (CCaraFrameWnd*)pApp->m_pMainWnd;
      CLabel          *pl     = pCFW->GetFromClipboard(m_nSelect);
      CLabelContainer *plcDoc = GetDocumentLabels();
      if (pl && plcDoc)
      {
         pl->SetPreviewMode(true);
         pl->SetDrawPickPoints(true);
         CDC *pDC = GetPreviewDC();
         if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)) && !((CLabelContainer*)pl)->GetDeleteContent())
         {
            CLabelContainer* plcPaste = (CLabelContainer*)pl;
            CLabel *plPaste;
            while (plPaste = plcPaste->GetLabel(0))
            {
               PasteLabel(pApp, plcDoc, plPaste);
               plcPaste->RemoveLabel(plPaste);
            }
            delete pl;
         }
         else PasteLabel(pApp, plcDoc, pl);

         ReleasePreviewDC();
      }
   }
}
void CCARAPreview::PasteLabel(CCaraWinApp *pApp, CLabelContainer *plc, CLabel *pl)
{
   if (pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))     // keine CCurveLabel einfügen
   {
      delete pl;
      return;
   }
   if (plc->InsertLabel(pl))
   {
      if (m_pPreviewInfo->m_lpUserData)
      {
         CRect rcClient, rcLabel;
         CRect rcClip = ((SMarginDlgParam*)m_pPreviewInfo->m_lpUserData)->rcDrawLoMetric;
         ProcessLabel prl = {NULL, 0, m_pPreviewDC, &rcClip};
         CDC *pDC = GetPreviewDC();
         rcLabel = pl->GetRect(pDC);
         GetClientRect(&rcClient);
         pDC->DPtoLP(&rcClient);
         ReleasePreviewDC();
         CPoint ptOff = rcClient.CenterPoint() - rcLabel.CenterPoint();
         pl->MoveLabel(ptOff);
         plc->ProcessWithLabels(NULL, &prl, CLabelContainer::ClipLabel);
         prl.nPos = 0;
         plc->ProcessWithLabels(RUNTIME_CLASS(CTextLabel), &prl, CTextLabel::ClipTextRect);
      }

      pApp->SetUndoCmd(ID_EDIT_PASTE, pl, NULL, 0, NULL);

      m_pDocument->UpdateAllViews(NULL, UPDATE_PREVIEW_LABEL, pl);
      m_pDocument->SetModifiedFlag(true);

      if (!pl->IsKindOf(RUNTIME_CLASS(CTextLabel)) && !((CTextLabel*)pl)->IsMultiLineText())
         pl->SetDrawPickPoints(true);
   }
}

void CCARAPreview::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   if (lHint&UPDATE_PRINT_SETTINGS)
   {
      SMarginDlgParam *pMdp = (SMarginDlgParam*)m_pPreviewInfo->m_lpUserData;
      CCaraView *pView = (CCaraView*) m_pPrintView;

      if (pMdp && pView)
      {
         pView->GetDocument()->GetLabelContainer()->ProcessWithLabels(RUNTIME_CLASS(CPictureLabel), NULL, CPictureLabel::DeleteBWBmp);
         m_pPreviewInfo->m_rectDraw = pMdp->rcPageLoMetric;
         if (m_pPreviewInfo->m_pPD->m_pd.hDC)
         {
            ::DeleteDC(m_pPreviewInfo->m_pPD->m_pd.hDC);
            m_pPreviewInfo->m_pPD->m_pd.hDC = NULL;
         }
         pView->OnPreparePrinting(m_pPreviewInfo);

         m_dcPrint.Detach();

         m_dcPrint.Attach(m_pPreviewInfo->m_pPD->m_pd.hDC);
         m_pPreviewDC->SetAttribDC(m_pPreviewInfo->m_pPD->m_pd.hDC);

         m_dcPrint.SaveDC();     // Save pristine state of DC

         HDC hDC = ::GetDC(m_hWnd);

         if (m_pPreviewDC)
         {
            m_pPreviewDC->SetOutputDC(hDC);
         }
         pView->OnBeginPrinting(m_pPreviewDC, m_pPreviewInfo);
         if (m_pPreviewDC)
         {
            m_pPreviewDC->ReleaseOutputDC();
         }
         ::ReleaseDC(m_hWnd, hDC);

         m_dcPrint.RestoreDC(-1);    // restore to untouched state

         m_sizePrinterPPI.cx = m_dcPrint.GetDeviceCaps(LOGPIXELSX);
         m_sizePrinterPPI.cy = m_dcPrint.GetDeviceCaps(LOGPIXELSY);

         PositionPage(0);                                     // compute page position
         InvalidateRect(NULL);
      }
      return;
   }

   CDC *pDC = GetPreviewDC();
   if (lHint & UPDATE_PREVIEW_LABEL)
   {
      if (pHint && pHint->IsKindOf(RUNTIME_CLASS(CLabel)))
      {
         CRgn UpdateRgn;
         ((CLabel*)pHint)->GetRgn(RGN_OR, pDC, &UpdateRgn, INFLATE_RECT);
         UpdateLabelRgn(UpdateRgn, pDC);
         ((CLabel*)pHint)->SetChanged(true);
         if (lHint & UPDATE_DELETE_LABEL)
         {
            m_PickList.RemoveLabel((CLabel*)pHint);
         }
      }
   }
   else if (lHint & UPDATE_INSERT_LABEL_INTO)
   {
      if (pHint && pHint->IsKindOf(RUNTIME_CLASS(CLabel)))
      {
         CRgn UpdateRgn;
         ((CLabel*)pHint)->GetRgn(RGN_OR, pDC, &UpdateRgn, INFLATE_RECT);
         ((CLabel*)pHint)->SetChanged(true);
         UpdateLabelRgn(UpdateRgn, pDC);
      }
   }
   else if (lHint & UPDATE_CURVE_UNDO)
   {
      if (pHint && pHint->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
      {
         CRgn rgn;
         if (((CCurveLabel*)pHint)->GetUndoRgn(RGN_COPY, pDC, &rgn))
         {
            ((CLabel*)pHint)->SetChanged(true);
            UpdateLabelRgn(rgn, pDC);
         }
      }
   }
   else if ((lHint & UPDATE_INSERT_NEWDOC) || (lHint & UPDATE_DELETE_CONTENTS))
   {
      InvalidateRect(NULL);
   }
   ReleasePreviewDC();
}

void CCARAPreview::CatchGrid(CPoint & ptCatch)
{
   if (m_bCatchGrid)
   {
      int nround;
      if (m_szCatchGrid.cx > 0)
      {
         ptCatch.x -= m_ptCatchGridOrg.x;
         nround = (ptCatch.x % m_szCatchGrid.cx) << 1;
         ptCatch.x /= m_szCatchGrid.cx;
         if (nround >= m_szCatchGrid.cx) ptCatch.x++;
         ptCatch.x *= m_szCatchGrid.cx;
         ptCatch.x += m_ptCatchGridOrg.x;
      }
      if (m_szCatchGrid.cy > 0)
      {
         ptCatch.y += m_ptCatchGridOrg.y;
         nround = (ptCatch.y % m_szCatchGrid.cy) << 1;
         ptCatch.y /= m_szCatchGrid.cy;
         if (nround <= -m_szCatchGrid.cy) ptCatch.y--;
         ptCatch.y *= m_szCatchGrid.cy;
         ptCatch.y -= m_ptCatchGridOrg.y;
      }
   }
}

LRESULT CCARAPreview::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
   if (lParam != 0)
   {
      AfxGetApp()->WinHelp(lParam);
   }

   return true;
}

BOOL CCARAPreview::DestroyWindow() 
{
   m_pDocument->SetTitle(m_pDocument->GetTitle());
   return CPreviewView::DestroyWindow();
}

void CCARAPreview::OnPreviewClose()
{
   if (m_pToolBar)
   {
      int nIDItem[] = {ID_FILE_PRINT_SETUP, AFX_ID_PREVIEW_PRINT, AFX_ID_PREVIEW_ZOOMOUT, AFX_ID_PREVIEW_ZOOMIN, AFX_ID_PREVIEW_CLOSE};
      int   i, nCount = sizeof(nIDItem) / sizeof(int);
      HICON hIcon;
      for (i=0; i<nCount; i++)                        // Icons der Buttons entfernen
      {
         hIcon = (HICON) m_pToolBar->SendDlgItemMessage(nIDItem[i], BM_SETIMAGE, IMAGE_ICON, NULL);
         VERIFY(DestroyIcon(hIcon));                              // und löschen
      }
   }

   CPreviewView::OnPreviewClose();
}

void CCARAPreview::OnNoLogo() 
{
   CCARADoc *pDoc = (CCARADoc*) GetDocument();
   if (pDoc->m_pCaraLogo)
   {
      pDoc->m_pCaraLogo->SetVisible(!pDoc->m_pCaraLogo->IsVisible());
      InvalidateRect(NULL);
   }
}

HENHMETAFILE CCARAPreview::GetPreviewMetaFile()
{
   HENHMETAFILE hmeta = NULL;
   CDC *pDC = GetDC();
   pDC->SetMapMode(m_pPreviewDC->GetMapMode());
   CMetaFileDC  MetaDC;
   SMarginDlgParam *pMdp = (SMarginDlgParam*)m_pPreviewInfo->m_lpUserData;
   if (MetaDC.CreateEnhanced(pDC, NULL, NULL, NULL))
   {
      MetaDC.SetAttribDC(pDC->GetSafeHdc());
      MetaDC.SetMapMode(m_pPreviewDC->GetMapMode());
      CCaraView *pView = (CCaraView*) m_pPrintView;
      MetaDC.m_bPrinting = true;
      MetaDC.SelectStockObject(HOLLOW_BRUSH);
      MetaDC.SelectStockObject(BLACK_PEN);
      CLabel::Rectangle(&MetaDC, &pMdp->rcDrawLoMetric, false);
      m_PickList.SetPicked(false);
      pView->OnPrint(&MetaDC, m_pPreviewInfo);
      m_PickList.SetPicked(true);
      hmeta = MetaDC.CloseEnhanced();
   }
   ReleaseDC(pDC);
   return hmeta;
}

CLabelContainer * CCARAPreview::GetDocumentLabels()
{
   ASSERT(m_pDocument != NULL);
   if (m_pDocument)
   {
      ASSERT_KINDOF(CCARADoc, m_pDocument);
      if (m_pPrintView)
      {
         ASSERT_KINDOF(CCaraView, m_pPrintView);
         if (((CCaraView*)m_pPrintView)->m_nCurrentPage != -1)
         {
            CLabel *pl = ((CCARADoc *)m_pDocument)->GetLabelContainer()->GetLabel(((CCaraView*)m_pPrintView)->m_nCurrentPage-1);
            if (pl && pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
            {
               TRACE("Seite %d\n", ((CCaraView*)m_pPrintView)->m_nCurrentPage);
               return (CLabelContainer*)pl;
            }
         }
      }
      return ((CCARADoc *)m_pDocument)->GetLabelContainer();
   }
   return NULL;
}


void CCARAPreview::OnDestroy()
{
   m_PickList.ResetStates();
	CPreviewView::OnDestroy();
}

void CCARAPreview::OnColormatching() 
{
   CLabel::gm_bColorMatching = !CLabel::gm_bColorMatching;
   InvalidateRect(NULL);
}

void CCARAPreview::OnUpdateColorMatching(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(CLabel::gm_bColorMatching ? 1 : 0);
}

void CCARAPreview::OnContextMenuAcc() 
{
   OnContextMenu(this, CPoint(-1, -1));
}

LRESULT CCARAPreview::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
   return HID_BASE_RESOURCE + IDR_PREVIEW_CONTEXTMENU;
}

void CCARAPreview::OnViewMenu() 
{
   CCaraFrameWnd *pWnd = (CCaraFrameWnd*)AfxGetMainWnd();
   if (pWnd)
   {
      if (m_nBarState.menuvisible)
      {
         ::SetMenu(pWnd->m_hWnd, NULL);
         m_nBarState.menuvisible = false;
      }
      else if (AllocMainMenu())
      {
         if (::SetMenu(pWnd->m_hWnd, m_pMainMenu->m_hMenu))
            m_nBarState.menuvisible = true;
      }
   }	
}

void CCARAPreview::OnUpdateViewMenu(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(true);
   pCmdUI->SetCheck(m_nBarState.menuvisible != 0);
}


CCaraMenu* CCARAPreview::GetMainMenu() const
{
   return m_pMainMenu;
}

void CCARAPreview::OnAddPage() 
{
   if (m_pPreviewInfo && m_pPrintView && m_pDocument)
   {
      ASSERT_KINDOF(CCaraView, m_pPrintView);
      ASSERT_KINDOF(CCARADoc , m_pDocument);
      CCaraView *pView = (CCaraView*) m_pPrintView;
      CCARADoc  *pDoc  = (CCARADoc *) m_pDocument;
      SMarginDlgParam *pMdp = (SMarginDlgParam*)m_pPreviewInfo->m_lpUserData;
      pView->m_nPages++;
      m_pPreviewInfo->SetMaxPage(pView->m_nPages);
      if (pView->m_nPages > 1)
      {
         CLabelContainer *pNew = NULL;                      // alle Label in die erste Seite einfügen
         CLineLabel      *pll  = NULL;
         CTextLabel      *ptl  = NULL;
         CString str;
         pView->m_nCurrentPage = 0;
         CLabelContainer* pDocLabels = pDoc->GetLabelContainer();
         ASSERT(pDocLabels!=NULL);
         if (pView->m_nPages == 2)                             // von einer Seite auf zwei Seiten umschalten
         {
            pNew = new CLabelContainer;
            pNew->SetDeleteContent(true);
            pNew->SetVisible(true);
            str.Format(AFX_IDS_PRINTPAGENUM, 1);
            ptl = new CTextLabel(&CPoint(pMdp->rcDrawLoMetric.right, pMdp->rcDrawLoMetric.bottom), str);
            ptl->SetTextAlign(TA_RIGHT|TA_BOTTOM);
            ptl->SetDrawPickPoints(true);
            pNew->InsertLabel(ptl);
            pll = new CLineLabel(&CPoint(pMdp->rcDrawLoMetric.left, pMdp->rcDrawLoMetric.bottom));
            pll->SetPoint(CPoint(pMdp->rcDrawLoMetric.right, pMdp->rcDrawLoMetric.bottom), 1);
            pll->SetDrawPickPoints(true);
            pNew->InsertLabel(pll);
            pNew->InsertList(pDocLabels, false);
            pDocLabels->RemoveAll();
            pDocLabels->InsertLabel(pNew);
         }
         pNew = new CLabelContainer;
         pNew->SetDeleteContent(true);
         pNew->SetVisible(true);
         str.Format(AFX_IDS_PRINTPAGENUM, pView->m_nPages);
         ptl = new CTextLabel(&CPoint(pMdp->rcDrawLoMetric.right, pMdp->rcDrawLoMetric.bottom), str);
         ptl->SetDrawPickPoints(true);
         ptl->SetTextAlign(TA_RIGHT|TA_BOTTOM);
         pNew->InsertLabel(ptl);
         pll = new CLineLabel(&CPoint(pMdp->rcDrawLoMetric.left, pMdp->rcDrawLoMetric.bottom));
         pll->SetDrawPickPoints(true);
         pll->SetPoint(CPoint(pMdp->rcDrawLoMetric.right, pMdp->rcDrawLoMetric.bottom), 1);
         pNew->InsertLabel(pll);
         pDocLabels->InsertLabel(pNew);

         CDC *pDC = GetPreviewDC();
         ProcessLabel prl = {NULL, 0, m_pPreviewDC, &pMdp->rcDrawLoMetric};
         pDocLabels->ProcessWithLabels(NULL, &prl, CLabelContainer::ClipLabel);
         ReleasePreviewDC();
         SendMessage(WM_COMMAND, MAKELONG(AFX_ID_PREVIEW_NEXT, 1), NULL);
      }
   }
}

void CCARAPreview::OnRemovePage() 
{
   if (m_pPreviewInfo && m_pPrintView && m_pDocument)
   {
      ASSERT_KINDOF(CCaraView, m_pPrintView);
      ASSERT_KINDOF(CCARADoc , m_pDocument);
      CCaraView *pView = (CCaraView*) m_pPrintView;
      if (pView->m_nPages > 1)
      {
         pView->m_nCurrentPage = pView->m_nPages;
         CLabel *pl = ((CCARADoc *)m_pDocument)->GetLabelContainer()->GetLabel(pView->m_nCurrentPage-1);
         if (pl && pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
         {
            if (((CCARADoc *)m_pDocument)->GetLabelContainer()->RemoveLabel(pl))
            {
               delete pl;
            }
         }
         pView->m_nPages--;
         m_pPreviewInfo->SetMaxPage(pView->m_nPages);
         if (pView->m_nCurrentPage >= pView->m_nPages)
         {
            SendMessage(WM_COMMAND, MAKELONG(AFX_ID_PREVIEW_PREV, 1), NULL);
         }
         if (pView->m_nPages == 1)
         {
            pView->m_nCurrentPage = -1;
         }
      }
   }
}
