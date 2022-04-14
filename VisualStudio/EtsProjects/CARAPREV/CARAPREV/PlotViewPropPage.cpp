// PlotViewPropPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "PlotViewPropPage.h"
#include "CaraViewProperties.h"
#include "CARAPREV.h"
#include "FontPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CPlotViewPropPage 

IMPLEMENT_DYNCREATE(CPlotViewPropPage, CCaraPropertyPage)

CPlotViewPropPage::CPlotViewPropPage() : CCaraPropertyPage(CPlotViewPropPage::IDD)
{
   memset(&m_CurveLogFont, 0, sizeof(LOGFONT));
   memset(&m_GridTextLogFont, 0, sizeof(LOGFONT));
   memset(&m_HeadingLogFont, 0, sizeof(LOGFONT));
   m_bValuesChanged  = false;
   m_pPlot         = NULL;
   m_GridTextColor = 0;
   m_GridLineColor = 0;
   m_HeadingColor  = 0;
 
   //{{AFX_DATA_INIT(CPlotViewPropPage)
   m_nXNumMode       = 0;
   m_nYNumMode       = 0;
   m_nXRound         = 2;
   m_nYRound         = 2;
   m_fXGridStep      = 10.0f;
   m_fYGridStep      = 10.0f;
   m_bDrawCurveFrame = FALSE;
	m_bShowGridX = FALSE;
	m_bShowGridY = FALSE;
	m_bShowTextX = FALSE;
	m_bShowTextY = FALSE;
	//}}AFX_DATA_INIT
}

CPlotViewPropPage::~CPlotViewPropPage()
{
}

void CPlotViewPropPage::DoDataExchange(CDataExchange* pDX)
{
   int   nXRound     = m_nXRound;
   int   nYRound     = m_nYRound;
   double dXGridStep = (double)m_fXGridStep;
   double dYGridStep = (double)m_fYGridStep;

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlotViewPropPage)
	DDX_Check(pDX, IDC_PLT_CURVETEXTFRAME, m_bDrawCurveFrame);
	DDX_Check(pDX, IDC_PLT_SHOWGRID_X, m_bShowGridX);
	DDX_Check(pDX, IDC_PLT_SHOWGRID_Y, m_bShowGridY);
	DDX_Check(pDX, IDC_PLT_SHOWTEXT_X, m_bShowTextX);
	DDX_Check(pDX, IDC_PLT_SHOWTEXT_Y, m_bShowTextY);
	//}}AFX_DATA_MAP

   DDX_TextVar(pDX, IDC_PLT_GRIDSTEPX, FLT_DIG+1, dXGridStep);
	DDV_MinMaxDouble(pDX, dXGridStep, 5.0, 100.0);
	DDX_TextVar(pDX, IDC_PLT_GRIDSTEPY, FLT_DIG+1,  dYGridStep);
	DDV_MinMaxDouble(pDX, dYGridStep, 5.0, 100.0);

   m_nXRound    = nXRound;
   m_nYRound    = nYRound;
   m_fXGridStep = (float)dXGridStep;
   m_fYGridStep = (float)dYGridStep;
}


BEGIN_MESSAGE_MAP(CPlotViewPropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPlotViewPropPage)
	ON_BN_CLICKED(IDC_CURVE_FONT, OnCurveFont)
	ON_BN_CLICKED(IDC_PLT_GRID_FONT, OnGridFont)
	ON_BN_CLICKED(IDC_PLT_HEADINGFONT, OnHeadingFont)
	ON_BN_CLICKED(IDC_PLT_GRIDCOLOR, OnGridColor)
	ON_EN_UPDATE(IDC_PLT_GRIDSTEPX, OnChangeValues)
	ON_BN_CLICKED(IDC_PLT_SHOWGRID_X, OnShowgridX)
	ON_BN_CLICKED(IDC_PLT_SHOWGRID_Y, OnShowgridY)
	ON_BN_CLICKED(IDC_PLT_SHOWTEXT_X, OnShowtextX)
	ON_BN_CLICKED(IDC_PLT_SHOWTEXT_Y, OnShowtextY)
	ON_EN_UPDATE(IDC_PLT_GRIDSTEPY, OnChangeValues)
	ON_BN_CLICKED(IDC_PLT_CURVETEXTFRAME, OnChangeValues)
	ON_BN_CLICKED(IDC_PLT_NUM_REP_X, OnPltNumRepX)
	ON_BN_CLICKED(IDC_PLT_NUM_REP_Y, OnPltNumRepY)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPlotViewPropPage 
void CPlotViewPropPage::OnHeadingFont()
{
   LOGFONT lf = m_HeadingLogFont;
   CCaraViewProperties PropSheet(IDS_PROPERTIES, this);
   CString string;
   string.LoadString(IDS_HEADINGFONT);
   CTextLabel TextLabel(&CPoint(0,0), (TCHAR*)LPCTSTR(string));
   TextLabel.SetTextAlign(0);
   TextLabel.SetExtraSpacing(0);
   TextLabel.SetTextColor(m_HeadingColor);
   TextLabel.SetLogFont(&m_HeadingLogFont);
   TextLabel.SetDrawFrame(false);
   TextLabel.SetDrawBkGnd(false);

   CWnd *pParent  = GetParent();
   if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CCaraViewProperties)))
   {
      PropSheet.m_pView = ((CCaraViewProperties*)pParent)->m_pView;
   }
   PropSheet.InitDialog(TXT_FONT|TXT_HEIGHT|TXT_WIDTH|TXT_WEIGHT|TXT_ITALIC|TXT_COLOR, &TextLabel);
   PropSheet.SetApplyNowButton(false);
   if (PropSheet.DoModal()==IDOK)
   {
      m_HeadingLogFont = TextLabel.GetLogFont();
      if (memcmp(&lf, &m_HeadingLogFont, sizeof(LOGFONT))) OnChangeValues();
      COLORREF color = TextLabel.GetTextColor();
      if (m_HeadingColor != color)
      {
         m_HeadingColor = color;
         OnChangeValues();
      }
   }
}
void CPlotViewPropPage::OnGridFont()
{
   LOGFONT lf = m_GridTextLogFont;
   CCaraViewProperties PropSheet(IDS_PROPERTIES, this);
   CString string;
   string.LoadString(IDS_GRIDFONT);
   CTextLabel TextLabel(&CPoint(0,0), (TCHAR*)LPCTSTR(string));
   TextLabel.SetTextAlign(0);
   TextLabel.SetExtraSpacing(0);
   TextLabel.SetTextColor(m_GridTextColor);
   TextLabel.SetLogFont(&m_GridTextLogFont);
   TextLabel.SetDrawFrame(false);
   TextLabel.SetDrawBkGnd(false);

   CWnd *pParent  = GetParent();
   if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CCaraViewProperties)))
   {
      PropSheet.m_pView = ((CCaraViewProperties*)pParent)->m_pView;
   }
   PropSheet.InitDialog(TXT_FONT|TXT_HEIGHT|TXT_WIDTH|TXT_WEIGHT|TXT_ITALIC|TXT_COLOR, &TextLabel);
   PropSheet.SetApplyNowButton(false);
   if (PropSheet.DoModal()==IDOK)
   {
      m_GridTextLogFont = TextLabel.GetLogFont();
      if (memcmp(&lf, &m_GridTextLogFont, sizeof(LOGFONT))) OnChangeValues();
      COLORREF color = TextLabel.GetTextColor();
      if (m_GridTextColor != color)
      {
         m_GridTextColor = color;
         OnChangeValues();
      }
   }
}
void CPlotViewPropPage::OnCurveFont()
{
   LOGFONT lf = m_CurveLogFont;
   CCaraViewProperties PropSheet(IDS_PROPERTIES, this);
   CString string;
   string.LoadString(IDS_CURVEFONT);
   CTextLabel TextLabel(&CPoint(0,0), (TCHAR*)LPCTSTR(string));
   TextLabel.SetTextAlign(0);
   TextLabel.SetExtraSpacing(0);
   TextLabel.SetTextColor(0);
   TextLabel.SetLogFont(&m_CurveLogFont);
   TextLabel.SetDrawFrame(false);
   TextLabel.SetDrawBkGnd(false);

   CWnd *pParent  = GetParent();
   if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CCaraViewProperties)))
   {
      PropSheet.m_pView = ((CCaraViewProperties*)pParent)->m_pView;
   }
   PropSheet.InitDialog(TXT_FONT|TXT_HEIGHT|TXT_WIDTH|TXT_WEIGHT|TXT_ITALIC, &TextLabel);
   PropSheet.SetApplyNowButton(false);
   if (PropSheet.DoModal()==IDOK)
   {
      m_CurveLogFont = TextLabel.GetLogFont();
      if (memcmp(&lf, &m_CurveLogFont, sizeof(LOGFONT))) OnChangeValues();
   }
}

void CPlotViewPropPage::OnGridColor()
{
   CString string;
   CColorDialog cd(m_GridLineColor, 0, this);
   cd.m_cc.lpCustColors = g_CustomColors;
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   cd.m_cc.Flags       |=  CC_FULLOPEN;

   string.LoadString(IDS_GRIDCOLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal()==IDOK)
   {
      COLORREF color = cd.GetColor();
      if (m_GridLineColor != color)
      {
         m_GridLineColor = color;
         OnChangeValues();
      }
   }
   g_AfxCommDlgProc = NULL;
}

void CPlotViewPropPage::SetPlot(CPlotLabel *ppl)
{
   m_pPlot = ppl;
   if (ppl)
   {
      ASSERT_VALID(ppl);
      m_bValuesChanged = true;
   }
}

BOOL CPlotViewPropPage::OnSetActive()
{
   if (m_bValuesChanged && m_pPlot)
   {
      WORD wStep;
      m_HeadingLogFont  = m_pPlot->GetHeadingLogFont();
      m_HeadingColor    = m_pPlot->GetHeadingColor();
      m_GridTextLogFont = m_pPlot->GetGridTextLogFont();
      m_GridTextColor   = m_pPlot->GetGridTextColor();
      m_GridLineColor   = m_pPlot->GetGridLineColor();
      m_CurveLogFont    = m_pPlot->GetCurveLogFont();
      m_nXRound         = m_pPlot->GetXRound();
      m_nYRound         = m_pPlot->GetYRound();
      m_nXNumMode       = m_pPlot->GetXNumModeEx();
      m_nYNumMode       = m_pPlot->GetYNumModeEx();
      wStep             = m_pPlot->GetXGridStep();
      m_bShowGridX      = (wStep & HIDE_GRID_LINE) ? false : true;
      m_bShowTextX      = (wStep & HIDE_GRID_TEXT) ? false : true;
      m_fXGridStep      = 0.1f*(float)(wStep&GRID_LINE_MASK);
      wStep             = m_pPlot->GetYGridStep();
      m_bShowGridY      = (wStep & HIDE_GRID_LINE) ? false : true;
      m_bShowTextY      = (wStep & HIDE_GRID_TEXT) ? false : true;
      m_fYGridStep      = 0.1f*(float)(wStep&GRID_LINE_MASK);
      m_bDrawCurveFrame = m_pPlot->DoDrawFrame();
      UpdateData(false);
      m_bPolar = (m_pPlot->GetXDivision()==ETSDIV_POLAR) ? true :false;
      GetDlgItem(IDC_PLT_GRIDSTEPX)->EnableWindow(m_bShowGridX && !m_bPolar);
      GetDlgItem(IDC_PLT_NUM_REP_X)->EnableWindow(m_bShowTextX && !m_bPolar);

   	GetDlgItem(IDC_PLT_GRIDSTEPY)->EnableWindow(m_bShowGridY);
      GetDlgItem(IDC_PLT_NUM_REP_Y   )->EnableWindow(m_bShowTextY);
      m_bValuesChanged = false;
   }
	
   return CPropertyPage::OnSetActive();
}

BOOL CPlotViewPropPage::OnApply()
{
   return CCaraPropertyPage::OnApply();
}

BOOL CPlotViewPropPage::OnKillActive() 
{
   int nResult = CCaraPropertyPage::OnKillActive();
   if (nResult & CHANGE_PAGE)
   {
      if (nResult & CHANGES_REJECTED)
         SetPlot(m_pPlot);
      return true;
   }
   else
   {
      CWnd *pFocus  = GetFocus();
      if      (GetDlgItem(IDC_PLT_GRID_FONT  )==pFocus) OnGridFont();
      else if (GetDlgItem(IDC_PLT_HEADINGFONT)==pFocus) OnHeadingFont();
      else if (GetDlgItem(IDC_CURVE_FONT     )==pFocus) OnCurveFont();
      else if (GetDlgItem(IDC_PLT_GRIDCOLOR  )==pFocus) OnGridColor();
      else if (RequestSaveChanges(false)) return false;

      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
   return false;
}

void CPlotViewPropPage::OnOK()
{
   CopyValues();
   CPropertyPage::OnOK();
}

void CPlotViewPropPage::OnChangeValues()
{
   SetModified(true);
}

void CPlotViewPropPage::CopyValues()
{
   if (m_pPlot)
   {
      m_pPlot->SetDrawFrame(m_bDrawCurveFrame ? true : false);
      m_pPlot->SetHeadingLogFont(m_HeadingLogFont);
      m_pPlot->SetHeadingColor(m_HeadingColor);
      m_pPlot->SetGridTextLogFont(m_GridTextLogFont);
      m_pPlot->SetGridTextColor(m_GridTextColor);
      m_pPlot->SetGridLineColor(m_GridLineColor);
      m_pPlot->SetCurveLogFont(m_CurveLogFont);
      m_pPlot->SetXRound(m_nXRound);
      m_pPlot->SetYRound(m_nYRound);
      m_pPlot->SetXNumModeEx(m_nXNumMode);
      m_pPlot->SetYNumModeEx(m_nYNumMode);
      WORD wStep;
      wStep = (WORD)(m_fXGridStep * 10.0);
      if (!m_bShowGridX) wStep |= HIDE_GRID_LINE;
      if (!m_bShowTextX) wStep |= HIDE_GRID_TEXT;
      m_pPlot->SetXGridStep(wStep);
      wStep = (WORD)(m_fYGridStep * 10.0);
      if (!m_bShowGridY) wStep |= HIDE_GRID_LINE;
      if (!m_bShowTextY) wStep |= HIDE_GRID_TEXT;
      m_pPlot->SetYGridStep(wStep);
      SetModified(false);
   }
}

void CPlotViewPropPage::OnShowgridX()
{
   UpdateData(true);
   GetDlgItem(IDC_PLT_GRIDSTEPX)->EnableWindow(m_bShowGridX && !m_bPolar);
   SetModified(true);
}

void CPlotViewPropPage::OnShowgridY()
{
   UpdateData(true);
	GetDlgItem(IDC_PLT_GRIDSTEPY)->EnableWindow(m_bShowGridY);
   SetModified(true);
}

void CPlotViewPropPage::OnShowtextX()
{
   UpdateData(true);
	SetModified(true);
	GetDlgItem(IDC_PLT_NUM_REP_X)->EnableWindow(m_bShowTextX && !m_bPolar);
}

void CPlotViewPropPage::OnShowtextY()
{
   UpdateData(true);
   SetModified(true);
	GetDlgItem(IDC_PLT_NUM_REP_Y)->EnableWindow(m_bShowTextY);
}


BOOL CPlotViewPropPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
   ::SetWindowLong(m_hWnd, GWL_ID, CPlotViewPropPage::IDD);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CPlotViewPropPage::OnPltNumRepX() 
{
   ETSDIV_NUTOTXT ntx = {0,m_nXNumMode, m_nXRound, 0, 0};
   if (CEtsDiv::NumberRepresentationDlg(&ntx, m_hWnd))
   {
      OnChangeValues();
      m_nXNumMode = ntx.nmode;
      m_nXRound   = ntx.nround;
   }
}

void CPlotViewPropPage::OnPltNumRepY() 
{
   ETSDIV_NUTOTXT ntx = {0,m_nYNumMode, m_nYRound, 0, 0};
   if (CEtsDiv::NumberRepresentationDlg(&ntx, m_hWnd))
   {
      OnChangeValues();
      m_nYNumMode = ntx.nmode;
      m_nYRound   = ntx.nround;
   }
}
