// CurveColorPropPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CaraWinApp.h"
#include "CurveColorPropPage.h"
#include "CARAPREV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CCurveColorPropPage 

IMPLEMENT_DYNCREATE(CCurveColorPropPage, CCaraPropertyPage)

CCurveColorPropPage::CCurveColorPropPage() : CCaraPropertyPage(CCurveColorPropPage::IDD)
{
   m_pColors = NULL;
   m_bColorsChanged = false;

	//{{AFX_DATA_INIT(CCurveColorPropPage)
	//}}AFX_DATA_INIT
}

CCurveColorPropPage::~CCurveColorPropPage()
{
}

void CCurveColorPropPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CCurveColorPropPage)
   //}}AFX_DATA_MAP
   for (int i=0; i<NUM_CURVE_COLORS; i++)
   {
      DDX_Control(pDX, IDC_CURVECOLOR1+i, m_Preview[i]);
   }
}


BEGIN_MESSAGE_MAP(CCurveColorPropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCurveColorPropPage)
	ON_BN_CLICKED(IDC_CURVECOLOR_BKGND, OnViewbackground)
   ON_CONTROL_RANGE(BN_CLICKED, IDC_CURVECOLOR1, IDC_CURVECOLOR16, OnButtoncolor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCurveColorPropPage 
BOOL CCurveColorPropPage::OnKillActive() 
{
   if (CCaraPropertyPage::OnKillActive()) return true;
   else
   {
      CWnd *pFocus  = GetFocus();
      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
   return false;
}

void CCurveColorPropPage::OnOK()
{
   if (m_pColors)
   {
      for (int i=0; i<NUM_CURVE_COLORS; i++) m_pColors[i] = m_CurvePen[i].lopnColor;

      ((CCaraWinApp*)AfxGetApp())->SetBackgroundColor(m_Backgroundcolor);
      SetModified(false);
   }
}

BOOL CCurveColorPropPage::OnApply()
{
   return CCaraPropertyPage::OnApply();
}

BOOL CCurveColorPropPage::OnSetActive()
{
   if (m_pColors && m_bColorsChanged)
   {
      LOGPEN logpen = {PS_SOLID, {0,0}, 0};
      for (int i=0; i<NUM_CURVE_COLORS; i++)
      {
         logpen.lopnColor = m_pColors[i];
         m_CurvePen[i] = logpen;
         m_Preview[i].SetBkColor(m_Backgroundcolor);
      }
   }
   return CPropertyPage::OnSetActive();
}

void CCurveColorPropPage::OnViewbackground()
{
   CString string;
   CColorDialog cd(m_Backgroundcolor, 0, this);
   cd.m_cc.lpCustColors = g_CustomColors;
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   cd.m_cc.Flags       |=  CC_FULLOPEN;

   string.LoadString(IDS_BACKGROUNDCOLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal()==IDOK)
   {
      COLORREF color = cd.GetColor();
      if (m_Backgroundcolor != color)
      {
         m_Backgroundcolor = color;
         for (int i=0; i<NUM_CURVE_COLORS; i++)
         {
            m_Preview[i].SetBkColor(m_Backgroundcolor);
            m_Preview[i].InvalidateRect(NULL);
         }
         SetModified(true);
      }
   }
   g_AfxCommDlgProc = NULL;
}

void CCurveColorPropPage::OnButtoncolor(UINT ID) 
{
   ID-=IDC_CURVECOLOR1;
   if ((ID>=0) && (ID < NUM_CURVE_COLORS))
   {
      CString string;
      CColorDialog cd(m_CurvePen[ID].lopnColor, 0, this);
      cd.m_cc.lpCustColors = g_CustomColors;
      g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
      cd.m_cc.lpfnHook     = CCHookProc;
      cd.m_cc.Flags       |=  CC_FULLOPEN;

      string.Format(IDS_CURVECOLOR, ID+1);
      cd.m_cc.lCustData = (long) LPCTSTR(string);

      if (cd.DoModal()==IDOK)
      {
         COLORREF color = cd.GetColor();
         if (m_CurvePen[ID].lopnColor != color)
         {
            m_CurvePen[ID].lopnColor = color;
            SetModified(true);
            m_Preview[ID].InvalidateRect(NULL);
         }
      }
      g_AfxCommDlgProc = NULL;
   }
}

BOOL CCurveColorPropPage::OnInitDialog() 
{
   CDialog::OnInitDialog();
   ::SetWindowLong(m_hWnd, GWL_ID, CCurveColorPropPage::IDD);
   m_nZoom = 160;
   for (int i=0; i<NUM_CURVE_COLORS; i++)
   {
      m_Preview[i].SetLogPen(&m_CurvePen[i]);
      m_Preview[i].SetLabel(&m_Bezier);
      m_Preview[i].SetZoom(&m_nZoom);
   }

   CLabel * pl = &m_Bezier;
   pl->SetPoint(CPoint(  0,   0), 0);
   pl->SetPoint(CPoint( 50,  40), 1);
   pl->SetPoint(CPoint( 50, -40), 2);
   pl->SetPoint(CPoint(100,   0), 3);
   m_Backgroundcolor = ((CCaraWinApp*) AfxGetApp())->GetBackgroundColor();
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CCurveColorPropPage::SetColors(COLORREF *pcolor)
{
   if (!pcolor) return;
   m_pColors = pcolor;
   m_bColorsChanged = true;
}



