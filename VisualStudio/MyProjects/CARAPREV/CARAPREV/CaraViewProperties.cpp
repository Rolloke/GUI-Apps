// CaraViewProperties.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARAPREV.h"
#include "CaraWinApp.h"

#include "CaraViewProperties.h"
#include "LineStylePropPage.h"
#include "BrushStylePropPage.h"
#include "FontPropPage.h"
#include "PlotRangePropPage.h"
#include "CurveValuePropPage.h"
#include "CurveColorPropPage.h"
#include "PlotViewPropPage.h"
#include "PicturePropPage.h"
#include "ColScalePropPage.h"
#include "PropertyPreview.h"

#include "Label.h"
#include "LineLabel.h"
#include "RectLabel.h"
#include "TextLabel.h"
#include "PlotLabel.h"
#include "BezierLabel.h"
#include "CircleLabel.h"
#include "PictureLabel.h"
#include "ColorLabel.h"

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaraViewProperties

IMPLEMENT_DYNAMIC(CCaraViewProperties, CPropertySheet)

CCaraViewProperties::CCaraViewProperties(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   Init();
}

CCaraViewProperties::CCaraViewProperties(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   Init();
}

void CCaraViewProperties::Init()
{
   m_pLineStyle      = new CLineStylePropPage;
   m_pBrushStyle     = new CBrushStylePropPage;

   m_pFontProps      = NULL;
   m_pPlotRange      = NULL;
   m_pCurveValues    = NULL;
   m_pCurveColors    = NULL;
   m_pPlotViewProps  = NULL;
   m_pPictureProps   = NULL;
   m_pColScaleProps  = NULL;

   m_pView           = NULL;
   m_BkColor         = RGB(255,255,255);

   // LineStyle Preview initialisieren
   m_pLineStyle->m_Preview.SetpBkColor(&m_pBrushStyle->m_cBkColor);
   m_pLineStyle->m_Preview.SetpbDrawBkGnd(&m_pBrushStyle->m_bDrawBkGnd);
   m_pLineStyle->m_Preview.SetLogBrush(&m_pBrushStyle->m_LogBrush);
   // BrushStyle Preview initialisieren
   m_pBrushStyle->m_Preview.SetLogPen(&m_pLineStyle->m_LogPen);
}

CCaraViewProperties::~CCaraViewProperties()
{
   if (m_pLineStyle)       delete m_pLineStyle;
   if (m_pBrushStyle)      delete m_pBrushStyle;
   if (m_pFontProps)       delete m_pFontProps;
   if (m_pPlotRange)       delete m_pPlotRange;
   if (m_pCurveValues)     delete m_pCurveValues;
   if (m_pCurveColors)     delete m_pCurveColors;
   if (m_pPlotViewProps)   delete m_pPlotViewProps;
   if (m_pPictureProps)    delete m_pPictureProps;
   if (m_pColScaleProps)   delete m_pColScaleProps;
}

void CCaraViewProperties::CleanUpLabel()
{
   if (m_pLineStyle)       m_pLineStyle->SetLabel(NULL);
   if (m_pBrushStyle)      m_pBrushStyle->SetLabel(NULL);
   if (m_pFontProps)       m_pFontProps->SetLabel(NULL);
   if (m_pPlotRange)       m_pPlotRange->SetPlot(NULL);
   if (m_pCurveValues)     m_pCurveValues->SetPlot(NULL);
   if (m_pPlotViewProps)   m_pPlotViewProps->SetPlot(NULL);
   if (m_pPictureProps)    m_pPictureProps->SetLabel(NULL);
   if (m_pColScaleProps)   m_pColScaleProps->SetLabel(NULL);
}

BEGIN_MESSAGE_MAP(CCaraViewProperties, CPropertySheet)
	//{{AFX_MSG_MAP(CCaraViewProperties)
	ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCaraViewProperties 
void CCaraViewProperties::RemoveAllPages()
{
   while (GetPageCount()) RemovePage(0);
}

BOOL CCaraViewProperties::InitDialog(int nMode, CLabel *pl)
{
   RemoveAllPages();
   m_psh.dwFlags &= ~PSH_NOAPPLYNOW;
   if (pl)
   {
      if (pl->IsKindOf(RUNTIME_CLASS(CLineLabel)))
      {
         SetTitle(IDS_PROPERTY_LINES);
         if (!m_pLineStyle) return false;
         AddPage(m_pLineStyle);                                // Linienstil
         m_pLineStyle->SetLabel(pl);
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CPlotLabel)))
      {
         CPlotLabel *ppl = (CPlotLabel*) pl;
         if (nMode == 1)                                       // 1: Standardeinstellungen
         {
            m_psh.dwFlags |= PSH_NOAPPLYNOW;
            SetTitle(IDS_PROPERTY_PLOT_STD);
            if (!m_pPlotViewProps)
            {
               m_pPlotViewProps = new CPlotViewPropPage;
               if (!m_pPlotViewProps) return false;
            }
            AddPage(m_pPlotViewProps);                         // Darstellungseigenschaften
            m_pPlotViewProps->SetPlot(ppl);
            if (!m_pCurveColors)
            {
               m_pCurveColors = new CCurveColorPropPage;
               if (!m_pCurveColors) return false;
            }
            AddPage(m_pCurveColors);                           // Darstellungseigenschaften
            m_pCurveColors->SetColors(CPlotLabel::gm_StdPltSets.sm_CurveColors);
            if (!m_pBrushStyle) return false;
            AddPage(m_pBrushStyle);                            // Hintergrund des Plots
            m_pBrushStyle->SetLabel(pl);
            if (!m_pLineStyle) return false;
            AddPage(m_pLineStyle);                             // Linien des Plots
            m_pLineStyle->SetLabel(pl);
         }
         else                                                  // 2: Kurven und Ploteinstellungen
         {
            SetTitle(IDS_PROPERTY_PLOT);
            if (!m_pPlotRange)
            {
               m_pPlotRange = new CPlotRangePropPage;
               if (!m_pPlotRange) return false;
            }
            AddPage(m_pPlotRange);                             // Weltkorrdinatenbereich
            m_pPlotRange->SetPlot(ppl);
            if (!m_pPlotViewProps)
            {
               m_pPlotViewProps = new CPlotViewPropPage;
               if (!m_pPlotViewProps) return false;
            }
            AddPage(m_pPlotViewProps);                         // Darstellungseigenschaften
            m_pPlotViewProps->SetPlot(ppl);
            if (!m_pCurveValues)
            {
               m_pCurveValues = new CCurveValuePropPage;
               if (!m_pCurveValues) return false;
            }
            AddPage(m_pCurveValues);                           // Kurvenwerte
            m_pCurveValues->SetPlot(ppl);
            if (!m_pBrushStyle) return false;
            AddPage(m_pBrushStyle);                            // Hintergrund des Plots
            m_pBrushStyle->SetLabel(pl);
            if (!m_pLineStyle) return false;
            AddPage(m_pLineStyle);                             // Linien des Plots
            m_pLineStyle->SetLabel(pl);
            if (nMode==3) SetActivePage(m_pCurveValues);       // 3: Kurveneinstellung
         }
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CPictureLabel)))
      {
         SetTitle(IDS_PROPERTY_PICTURE);
         if (!m_pPictureProps)
         {
            m_pPictureProps = new CPicturePropPage;
            if (!m_pPictureProps) return false;
         }
         AddPage(m_pPictureProps);                             // Rand um das Rechteck
         m_pPictureProps->SetLabel(pl);
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CColorLabel)))
      {
         SetTitle(IDS_PROPERTY_COLORSCALE);
         if (!m_pColScaleProps)
         {
            m_pColScaleProps = new CColScalePropPage;
            if (!m_pColScaleProps) return false;
         }
         AddPage(m_pColScaleProps);                            // Rand um das Rechteck
         m_pColScaleProps->SetLabel(pl);
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CRectLabel)))
      {
         SetTitle(IDS_PROPERTY_RECTANGLE);
         if (!m_pLineStyle) return false;
         AddPage(m_pLineStyle);                                // Rand um das Rechteck
         m_pLineStyle->SetLabel(pl);
         if (!m_pBrushStyle) return false;
         AddPage(m_pBrushStyle);                               // Hintergrund des Rechtecks
         m_pBrushStyle->SetLabel(pl);
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CBezierLabel)))
      {
         SetTitle(IDS_PROPERTY_BEZIER);
         AddPage(m_pLineStyle);                                // Linienstil um die Bezierlinie
         m_pLineStyle->SetLabel(pl);
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CCircleLabel)))
      {
         SetTitle(IDS_PROPERTY_CIRCLE);
         if (!m_pLineStyle) return false;
         AddPage(m_pLineStyle);                                // Rand um den Bezierkreis
         m_pLineStyle->SetLabel(pl);
         if (!m_pBrushStyle) return false;
         AddPage(m_pBrushStyle);                               // Hintergrund des Bezierkreises
         m_pBrushStyle->SetLabel(pl);
      }
      else if (pl->IsKindOf(RUNTIME_CLASS(CTextLabel)))
      {
         SetTitle(IDS_PROPERTY_TEXT);
         if (!m_pFontProps)
         {
            m_pFontProps = new CFontPropPage;
            if (!m_pFontProps) return false;
         }
         AddPage(m_pFontProps);                                // Schriftart des Textes
         m_pFontProps->SetLabel(pl);
         m_pFontProps->SetMode(nMode);
/*
         if (!m_pLineStyle) return false;
         AddPage(m_pLineStyle);                                // Rand um das Rechteck
         m_pLineStyle->SetLabel(pl);
         m_pLineStyle->m_Preview.SetLogFont(&m_pFontProps->m_LogFont, &m_pFontProps->m_Color, &m_pFontProps->m_nAlign, &m_pFontProps->m_nExtra);
         m_pLineStyle->m_Preview.SetpBkColor(&m_pFontProps->m_BKColor);
         m_pLineStyle->m_Preview.SetZoom(&m_pFontProps->m_nZoomFactor);
         m_pLineStyle->m_Preview.SetpbDrawBkGnd(&m_pFontProps->m_bBkGround);
*/
      }
   }
   else
   {
   }
   return false;
}

void CCaraViewProperties::SetTitle(UINT nID)
{
   CString str;
   
   if (str.LoadString(nID))
   {
      CPropertySheet::SetTitle(str, 0);
   }
}

BOOL CCaraViewProperties::IsModified()
{
   CButton *pButton = (CButton*) GetDlgItem(ID_APPLY_NOW);
   if (pButton) return pButton->IsWindowEnabled();
   return false;
}

BOOL CCaraViewProperties::IsApplyButtonVisible()
{
   CButton *pButton = (CButton*) GetDlgItem(ID_APPLY_NOW);
   if (pButton) return pButton->IsWindowVisible();
   return false;
}

int CCaraViewProperties::DoModal() 
{
   EnableStackedTabs(false);
//   m_psh.nPages
   return CPropertySheet::DoModal();
}

BOOL CCaraViewProperties::OnInitDialog() 
{
   BOOL bResult = CPropertySheet::OnInitDialog();
   CButton *pButton = (CButton*) GetDlgItem(ID_APPLY_NOW);
   if (pButton) pButton->EnableWindow(false);
   CWnd::ModifyStyleEx(0, WS_EX_CONTEXTHELP);
   return bResult;
}

LRESULT CCaraViewProperties::OnHelp(WPARAM wParam, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;

   CCaraWinApp *pApp = (CCaraWinApp*)AfxGetApp();
   const char * pszDefault = NULL;
   if (!pApp->m_strPreviewHelpPath.IsEmpty())
   {
      pszDefault = pApp->m_pszHelpFilePath;
      pApp->m_pszHelpFilePath = LPCTSTR(pApp->m_strPreviewHelpPath);
   }

   if ((pHI->iContextType == HELPINFO_WINDOW) && (pHI->dwContextId != 0))
   {                                                           // Hilfe für ein Control oder window
      CPropertySheet::WinHelp(pHI->dwContextId, HELP_CONTEXTPOPUP);
   }
   else                                                        // Hilfe für einen Menüpunkt
   {
      UINT nCmd = HELP_CONTEXT;
      if (pHI->dwContextId == 0)                               // wenn keine Kontext ID vorhanden ist 
      {

         HWND hwndParent = ::GetParent((HWND)pHI->hItemHandle);
         if (m_hWnd != hwndParent)
         {
            pHI->dwContextId = HID_BASE_RESOURCE +::GetWindowLong(hwndParent, GWL_ID);
         }
         else if (pHI->iCtrlId)
         {
            pHI->dwContextId = HID_BASE_RESOURCE + pHI->iCtrlId;
            nCmd = HELP_CONTEXTPOPUP;
         }
      }
      REPORT("HelpID %x", pHI->dwContextId);
      CPropertySheet::WinHelp(pHI->dwContextId, nCmd);
   }

   if (pszDefault) pApp->m_pszHelpFilePath = pszDefault;

   return true;
}

void CCaraViewProperties::SetApplyNowButton(bool bApply)
{
   if (bApply) m_psh.dwFlags &= ~PSH_NOAPPLYNOW;
   else        m_psh.dwFlags |=  PSH_NOAPPLYNOW;
}

void CCaraViewProperties::BuildPropPageArray()
{
   int i, nCount = GetPageCount();
   for (i=0; i<nCount; i++)
   {
      CPropertyPage *pP = GetPage(i);
      if (pP) pP->m_psp.dwFlags |= PSP_HASHELP;
   }
   CPropertySheet::BuildPropPageArray();
}
