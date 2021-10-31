// ViewParams.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA3DV.h"
#include "ViewParams.h"
#include "..\ColorArray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CURRENT_COLOR_OFF_X 8
#define CURRENT_COLOR_OFF_Y 8
#define CURRENT_COLOR_CX    10
#define CURRENT_COLOR_CY    10
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CViewParams 
extern "C" __declspec(dllimport) BOOL DDX_TextVar(CDataExchange*, int, int, double&, bool bThrow=true);

CViewParams::CViewParams(CWnd* pParent /*=NULL*/)
	: CNonModalDlg(CViewParams::IDD, pParent)
{
   //{{AFX_DATA_INIT(CViewParams)
   m_dObjectZoom        = 0.0;
   m_dValueZoom         = 0.0;
   m_nProjection        = 1;
   m_bShadingSmooth     = FALSE;
   m_nNumColors         = 0;
   m_bInterpolate       = FALSE;
   m_bInterpolateColor  = FALSE;
   m_strZ_Unit          = _T("[]");
   m_dMaxLevel          = 0.0;
   m_dMinLevel          = 0.0;
	//}}AFX_DATA_INIT
   m_Colors.Attach(new CColorArray);
}

void CViewParams::DoDataExchange(CDataExchange* pDX)
{
   double dObjectZoom = m_dObjectZoom;
   double dValueZoom  = m_dValueZoom;
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CViewParams)
   DDX_Control(pDX, IDC_VIEW_BKGND_COLOR,   m_BkGndColor);
   DDX_Control(pDX, IDC_VIEW_OBJECT_COLOR,  m_ObjectColor);
   DDX_Control(pDX, IDC_VIEW_COLORVIEW2,    m_ShadeColors);
   DDX_Control(pDX, IDC_VIEW_COLORVIEW,     m_Colors);
   DDX_Control(pDX, IDC_VIEW_COLSCALE, m_Scale);
   DDX_Control(pDX, IDC_VIEW_DETAIL_BUDDY,  m_cDetail);
   DDX_Radio(pDX, IDC_VIEW_PROJECTION1, m_nProjection);
   DDX_Check(pDX, IDC_VIEW_SHADING, m_bShadingSmooth);
   DDX_Check(pDX, IDC_VIEW_INTERPOLATE, m_bInterpolate);
   DDX_Check(pDX, IDC_VIEW_INTERPOLATE_COLOR, m_bInterpolateColor);
   DDX_Text(pDX, IDC_VIEW_Z_UNIT, m_strZ_Unit);
	//}}AFX_DATA_MAP

   DDX_TextVar(pDX, IDC_VIEW_ZMIN, FLT_DIG+1, m_dMinLevel);
   DDX_TextVar(pDX, IDC_VIEW_ZMAX, FLT_DIG+1, m_dMaxLevel);

   DDX_TextVar(pDX, IDC_VIEW_OBJECT_ZOOM, FLT_DIG+1, dObjectZoom);
   DDV_MinMaxDouble(pDX, dObjectZoom, 1.e-002, 100.);

   DDX_TextVar(pDX, IDC_VIEW_VALUE_ZOOM, FLT_DIG+1, dValueZoom);
   DDV_MinMaxDouble(pDX, dValueZoom, 1.e-002, 100.);

   DDX_Text(pDX, IDC_VIEW_NUMCOLORS, m_nNumColors);
   DDV_MinMaxInt(pDX, m_nNumColors, MIN_3D_COLORS, MAX_3D_COLORS);

   m_dObjectZoom = dObjectZoom;
   m_dValueZoom  = dValueZoom;
}


BEGIN_MESSAGE_MAP(CViewParams, CDialog)
	//{{AFX_MSG_MAP(CViewParams)
	ON_BN_CLICKED(IDC_VIEW_INTERPOLATE_COLOR, OnInterpolateColor)
	ON_EN_CHANGE(IDC_VIEW_DETAIL, OnChangeDetail)
	ON_EN_KILLFOCUS(IDC_VIEW_NUMCOLORS, OnKillfocusNumcolors)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_EN_KILLFOCUS(IDC_VIEW_ZMAX, OnKillfocusZValue)
	ON_BN_CLICKED(IDC_VIEW_INTERPOLATE, OnInterpolate)
	ON_EN_CHANGE(IDC_VIEW_OBJECT_ZOOM, OnChangeZoom)
	ON_BN_CLICKED(IDC_VIEW_PROJECTION1, OnProjection)
	ON_EN_CHANGE(IDC_VIEW_ZMAX, OnChangeMinMax)
	ON_BN_CLICKED(IDC_VIEW_SHADING, OnShading)
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_INITMENUPOPUP()
	ON_WM_SYSCOMMAND()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_BN_CLICKED(IDC_VIEW_PROJECTION2, OnProjection)
	ON_BN_CLICKED(ID_APPLY_NOW, OnApplyNowCmd)
	ON_EN_CHANGE(IDC_VIEW_VALUE_ZOOM, OnChangeZoom)
	ON_EN_CHANGE(IDC_VIEW_ZMIN, OnChangeMinMax)
	ON_EN_KILLFOCUS(IDC_VIEW_ZMIN, OnKillfocusZValue)
   ON_MESSAGE(WM_COLORVIEW_MOUSE_EVENT, OnColorViewMouseEvent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CViewParams 
void CViewParams::OnApplyNowCmd()
{
   OnApplyNow();
}

void CViewParams::OnOK() 
{
   CWnd * pWnd = GetFocus();
   if (GetDlgItem(IDOK) == pWnd)
   {
      GetParent()->SendMessage(WM_APPLY_TO_VIEW, INVALIDATE_VIEW, NULL);
      if (OnApplyNow())
      {	
         CWnd::DestroyWindow();
      }
   }
   else if (GetDlgItem(IDC_VIEW_NUMCOLORS)==pWnd)
   {
      OnKillfocusNumcolors();
   }
   else if ((GetDlgItem(IDC_VIEW_ZMIN)==pWnd) ||
            (GetDlgItem(IDC_VIEW_ZMAX)==pWnd))
   {
       OnKillfocusZValue();
   }
   else
   {
      pWnd = GetNextDlgTabItem(pWnd);
      if (pWnd) pWnd->SetFocus();
   }
}

BOOL CViewParams::OnInitDialog() 
{
   CNonModalDlg::OnInitDialog();
   if (!IsWindow(m_CurrentColor.m_hWnd))
   { 
      if (m_CurrentColor.Create(NULL, WS_CHILD, CRect(0,0,CURRENT_COLOR_CX,CURRENT_COLOR_CY), this))
      {
         m_CurrentColor.SetColorMode(0);
         m_CurrentColor.SetArraySize(2);
      }
      else return false;
   }

   if (m_Colors.GetNumColor() == 0)
   {
      m_Colors.CreateColorArray();
      m_Colors.SetColorMode(CV_PROTECTED);
   }
   OnInterpolateColor();

   m_cDetail.SetBuddy(GetDlgItem(IDC_VIEW_DETAIL));
   m_cDetail.SetRange(0,5);
   m_cDetail.SetPos(m_nDetail);

   InitSysColors();
   m_Scale.m_ScalePainter.m_Div.f1 = m_dMinLevel;
   m_Scale.m_ScalePainter.m_Div.f2 = m_dMaxLevel;
   m_Scale.m_ScalePainter.m_Div.stepwidth = MulDiv(m_Scale.GetMinSize().cx, 3, 2);
   return true;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

// Mouse - Messages
LRESULT CViewParams::OnColorViewMouseEvent(WPARAM wParam, LPARAM lParam)
{
   UINT nFlags = LOWORD(wParam);
   COLORREF CurrentColor=0;
   switch(HIWORD(wParam))
   {
      case  CV_LBUTTONDOWN:                                    // Farbe Picken
         CurrentColor = ((CColorView*)lParam)->GetColor(CURRENT_DOWN);
         TRACE("Farbe gepickt: RGB(%d, %d, %d)\n", GetRValue(CurrentColor),GetGValue(CurrentColor),GetBValue(CurrentColor));
         m_CurrentColor.SetColor(CurrentColor, 0);
         m_CurrentColor.SetColor(CurrentColor, 1);
         if (CurrentColor != 0xffffffff) ShowCurrentColor();
         break;
      case  CV_LBUTTONUP:                                      // Farbe bzw. Farben setzen
         CurrentColor = m_CurrentColor.GetColor(1);
         m_CurrentColor.SetColor(0xffffffff, 1);
         if (CurrentColor != 0xffffffff)
         {
            TRACE("Farbe gepickt: RGB(%d, %d, %d)\n", GetRValue(CurrentColor),GetGValue(CurrentColor),GetBValue(CurrentColor));
            ((CColorView*)lParam)->SetColor(CurrentColor);
         }
         if      (lParam == (LPARAM)&m_BkGndColor ) SetChanged(VIEWPARAMS_BKGNDCOLOR, 0);
         else if (lParam == (LPARAM)&m_ObjectColor) SetChanged(VIEWPARAMS_OBJECTCOLOR, 0);
         else if (lParam == (LPARAM)&m_ShadeColors) SetChanged(VIEWPARAMS_SHADECOLORS, 0);
         m_ShadeColors.ResetMouseEvent();
         break;
      case  CV_RBUTTONDOWN:                                    // Farbe Picken
         CurrentColor = ((CColorView*)lParam)->GetColor(CURRENT_DOWN);
         m_CurrentColor.SetColor(CurrentColor, 0);
         m_CurrentColor.SetColor(CurrentColor, 1);
         if (CurrentColor != 0xffffffff) ShowCurrentColor();
         break;
      case  CV_RBUTTONUP:                                      // Farben setzen
         CurrentColor = m_CurrentColor.GetColor(1);
         m_CurrentColor.SetColor(0xffffffff, 1);
         if (CurrentColor != 0xffffffff) ((CColorView*)lParam)->SetColor(CurrentColor, -2);
         if      (lParam == (LPARAM)&m_BkGndColor ) SetChanged(VIEWPARAMS_BKGNDCOLOR, 0);
         else if (lParam == (LPARAM)&m_ObjectColor) SetChanged(VIEWPARAMS_OBJECTCOLOR, 0);
         else if (lParam == (LPARAM)&m_ShadeColors) SetChanged(VIEWPARAMS_SHADECOLORS, 0);
         break;
      case  CV_MOUSEMOVE:
         break;
   }
   return 0;
}
void CViewParams::OnMouseMove(UINT nFlags, CPoint point) 
{
   if (nFlags & (MK_LBUTTON|MK_RBUTTON)) 
   {
      CPoint screenpoint(point);
      ClientToScreen(&screenpoint);
      CWnd *pWnd = CWnd::WindowFromPoint(screenpoint);
      if (pWnd == &m_ShadeColors)
      {
         if (m_ShadeColors.GetCurrentIndexDown() != -1)
         {
            m_ShadeColors.ScreenToClient(&screenpoint);
            m_CurrentColor.SetColor(m_ShadeColors.GetColor(m_ShadeColors.SetCurrentFromPoint(screenpoint)));
         }
      }
      if ((pWnd == &m_Colors) && (nFlags&MK_SHIFT))
      {
         m_Colors.ScreenToClient(&screenpoint);
         COLORREF color = m_Colors.GetColor(m_Colors.SetCurrentFromPoint(screenpoint));
         m_CurrentColor.SetColor(color);
         if (nFlags & MK_LBUTTON)
         {
            m_ShadeColors.SetColor(color, -3);
            SetChanged(VIEWPARAMS_SHADECOLORS, 0);
         }
         if (nFlags & MK_RBUTTON)
         {
            m_ShadeColors.SetColor(color, -4);
            SetChanged(VIEWPARAMS_SHADECOLORS, 0);
         }
      }
   }
   if (m_CurrentColor.IsWindowVisible())
      m_CurrentColor.MoveWindow(point.x-CURRENT_COLOR_OFF_X, point.y-CURRENT_COLOR_OFF_Y, CURRENT_COLOR_CX, CURRENT_COLOR_CY, true);
   CDialog::OnMouseMove(nFlags, point);
}
void CViewParams::OnLButtonUp(UINT nFlags, CPoint point) 
{
   ReleaseCapture();
   m_CurrentColor.ShowWindow(SW_HIDE);
   CDialog::OnLButtonUp(nFlags, point);
   ClientToScreen(&point);
   CWnd * pWnd = WindowFromPoint(point);
   if (pWnd && (pWnd != this))
   {
      pWnd->ScreenToClient(&point);
      pWnd->SendMessage(WM_LBUTTONUP, nFlags, MAKELONG(point.x, point.y));
   }
}
void CViewParams::OnRButtonUp(UINT nFlags, CPoint point) 
{
   ReleaseCapture();
   m_CurrentColor.ShowWindow(SW_HIDE);
   CDialog::OnRButtonUp(nFlags, point);
   ClientToScreen(&point);
   CWnd * pWnd = WindowFromPoint(point);
   if (pWnd && (pWnd != this))
   {
      pWnd->ScreenToClient(&point);
      pWnd->SendMessage(WM_RBUTTONUP, nFlags, MAKELONG(point.x, point.y));
   }
}
void CViewParams::ShowCurrentColor()
{
   CPoint point;
   GetCursorPos(&point);
   ScreenToClient(&point);
   SetCapture();
   m_CurrentColor.MoveWindow(point.x-CURRENT_COLOR_OFF_X, point.y-CURRENT_COLOR_OFF_Y, CURRENT_COLOR_CX, CURRENT_COLOR_CY, false);
   m_CurrentColor.ShowWindow(SW_SHOW);
}

void CViewParams::OnKillfocusNumcolors() 
{
   if (UpdateData(true))
   {
      if (m_nNumColors != m_ShadeColors.GetNumColor())
      {
         bool bIndexMode = false;
         if ((!m_bInterpolateColor) && (m_nNumColors > m_ShadeColors.GetNumColor()))
         {
            m_ShadeColors.SetColorMode(CV_INTERPOLATE);
            bIndexMode = true;
         }
         m_ShadeColors.SetArraySize(m_nNumColors);
         if (bIndexMode) m_ShadeColors.SetColorMode(0);
         m_ShadeColors.InvalidateRect(NULL);
         SetChanged(VIEWPARAMS_SHADECOLORS, 0);
      }
   }	
   else UpdateData(false);
}

void CViewParams::OnKillfocusZValue()
{
   if (UpdateData(true))
   {
      bool bChanged = false;
      if (m_Scale.m_ScalePainter.m_Div.f1 != m_dMinLevel)
      {
         m_Scale.m_ScalePainter.m_Div.f1 = m_dMinLevel;
         bChanged = true;
      }
      if (m_Scale.m_ScalePainter.m_Div.f2 != m_dMaxLevel)
      {
         m_Scale.m_ScalePainter.m_Div.f2 = m_dMaxLevel;
         bChanged = true;
      }
      if (bChanged)
      {
         m_Scale.m_ScalePainter.m_Div.stepwidth = MulDiv(m_Scale.GetMinSize().cx, 3, 2);
         m_Scale.InvalidateRect(NULL);
         SetChanged(VIEWPARAMS_MINMAXLEVEL, 0);
      }
   }
}
void CViewParams::OnInterpolateColor() 
{
   if (UpdateData(true))
   {
      m_ShadeColors.SetColorMode((m_bInterpolateColor) ? CV_INTERPOLATE : 0);
   }	
   else UpdateData(false);
}
void CViewParams::OnChangeDetail()
{
   int nMin, nMax;
   m_cDetail.GetRange(nMin, nMax);
   m_nDetail = m_cDetail.GetPos();
   if (m_nDetail >= 65535)                                     // ist das Ergebnis größer als ein 16 Bit Wert
   {
      if (m_nDetail==65536) m_nDetail = nMax;
      else                  m_nDetail = nMin;
      m_cDetail.SetPos(m_nDetail);
   }
   if (m_nDetail == 0) GetDlgItem(IDC_VIEW_INTERPOLATE)->EnableWindow(false);
   else                GetDlgItem(IDC_VIEW_INTERPOLATE)->EnableWindow(true);
   SetChanged(VIEWPARAMS_DETAIL, 0);
}

void CViewParams::OnInterpolate() {SetChanged(VIEWPARAMS_INTERPOLATE, 0);}
void CViewParams::OnChangeZoom()  {SetChanged(VIEWPARAMS_ZOOM, 0);}
void CViewParams::OnProjection()  {SetChanged(VIEWPARAMS_PROJECTION, 0);}
void CViewParams::OnChangeMinMax(){SetChanged(VIEWPARAMS_MINMAXLEVEL, 0);}
void CViewParams::OnShading()     {SetChanged(VIEWPARAMS_SHADING, 0);}

void CViewParams::InitSysColors()
{
   NONCLIENTMETRICS ncm;
   ncm.cbSize = sizeof(NONCLIENTMETRICS);
   ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
   CFont *pFont = GetFont();
   if (pFont)
      pFont->GetLogFont(&m_Scale.m_ScalePainter.m_LogFont);
   else
      m_Scale.m_ScalePainter.m_LogFont   = ncm.lfStatusFont;
   m_Scale.m_ScalePainter.m_TextColor = ::GetSysColor(COLOR_BTNTEXT);
}
