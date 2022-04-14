// PlotRangePropPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARAPREV.h"
#include "PlotRangePropPage.h"
#include "CurveLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CPlotRangePropPage 
#define LIN_MIN  -1.0e+13                          // Minimaler X- bzw. Y-Wert bei linearer Darstellung
#define LOG_MIN   1.0e-13                          // Minimaler X- bzw. Y-Wert bei logarithmischer Darstellung

#define POLAR_DIAGRAM   0
#define X_Y_DIAGRAM     1

IMPLEMENT_DYNCREATE(CPlotRangePropPage, CCaraPropertyPage)

CPlotRangePropPage::CPlotRangePropPage() : CCaraPropertyPage(CPlotRangePropPage::IDD)
{
	//{{AFX_DATA_INIT(CPlotRangePropPage)
	m_dRangex2    = 0.2;
	m_dRangex1    = 0.1;
	m_dRangey1    = 0.1;
	m_dRangey2    = 0.2;
	m_sUnit_x     = _T("");
	m_sUnit_y     = _T("");
	m_sPlotHeading = _T("");
	m_nDivision_x = 0;
	m_nDivision_y = 0;
	//}}AFX_DATA_INIT
	m_nDiagramType= POLAR_DIAGRAM;
   m_nAllowedCurves = 0;
   m_Acc.x          = 4;
   m_Acc.y          = 4;
}

CPlotRangePropPage::~CPlotRangePropPage()
{
}

void CPlotRangePropPage::DoDataExchange(CDataExchange* pDX)
{
   static double max = 1.0e13;                                 // Maximaler X- bzw. Y-Wert
   double min_x, min_y;

   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPlotRangePropPage)
   DDX_Text(pDX, IDC_PLT_UNITX,     m_sUnit_x);
   DDX_Text(pDX, IDC_PLT_UNITY,     m_sUnit_y);
   DDX_Text(pDX, IDC_PLT_PLOTHEADING, m_sPlotHeading);
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_PLT_POLAR, m_nDiagramType);

	DDX_Radio(pDX, IDC_PLT_XDIVLIN, m_nDivision_x);
   DDX_Division(pDX, IDC_PLT_XDIVLIN, min_x);

   DDX_Radio(pDX, IDC_PLT_YDIVLIN,  m_nDivision_y);
   DDX_Division(pDX, IDC_PLT_YDIVLIN, min_y);

   if (pDX->m_bSaveAndValidate)
   {
      DDX_TextVar( pDX, IDC_PLT_RANGEX1, DBL_DIG, m_dRangex1);
      MinMaxDouble(pDX, IDC_PLT_RANGEX1, min_x, max, m_dRangex1);
      DDX_TextVar( pDX, IDC_PLT_RANGEX2, DBL_DIG, m_dRangex2);
      MinMaxDouble(pDX, IDC_PLT_RANGEX2, min_x, max, m_dRangex2);
      DDX_TextVar( pDX, IDC_PLT_RANGEY1, DBL_DIG, m_dRangey1);
      MinMaxDouble(pDX, IDC_PLT_RANGEY1, min_y, max, m_dRangey1);
      DDX_TextVar( pDX, IDC_PLT_RANGEY2, DBL_DIG, m_dRangey2);
      MinMaxDouble(pDX, IDC_PLT_RANGEY2, min_y, max, m_dRangey2);
   }
   else
   {
      MinMaxDouble(pDX, IDC_PLT_RANGEX1, min_x, max, m_dRangex1);
      DDX_TextVar( pDX, IDC_PLT_RANGEX1, DBL_DIG, m_dRangex1);
      MinMaxDouble(pDX, IDC_PLT_RANGEX2, min_x, max, m_dRangex2);
      DDX_TextVar( pDX, IDC_PLT_RANGEX2, DBL_DIG, m_dRangex2);
      MinMaxDouble(pDX, IDC_PLT_RANGEY1, min_y, max, m_dRangey1);
      DDX_TextVar( pDX, IDC_PLT_RANGEY1, DBL_DIG, m_dRangey1);
      MinMaxDouble(pDX, IDC_PLT_RANGEY2, min_y, max, m_dRangey2);
      DDX_TextVar( pDX, IDC_PLT_RANGEY2, DBL_DIG, m_dRangey2);
   }
}


BEGIN_MESSAGE_MAP(CPlotRangePropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPlotRangePropPage)
	ON_EN_UPDATE(IDC_PLT_RANGEX1,      OnSetModified)
	ON_EN_KILLFOCUS(IDC_PLT_RANGEX1,   OnCheckRange)
	ON_EN_UPDATE(IDC_PLT_UNITX,        OnChangeValues)
	ON_BN_CLICKED(IDC_PLT_SETDEFAULTRANGE, OnSetDefaultrange)
	ON_BN_CLICKED(IDC_PLT_DETERMINRANGE, OnDeterminRange)
	ON_EN_KILLFOCUS(IDC_PLT_RANGEX2,   OnCheckRange)
	ON_EN_KILLFOCUS(IDC_PLT_RANGEY1,   OnCheckRange)
	ON_EN_KILLFOCUS(IDC_PLT_RANGEY2,   OnCheckRange)
	ON_EN_UPDATE(IDC_PLT_RANGEX2,      OnSetModified)
	ON_EN_UPDATE(IDC_PLT_RANGEY1,      OnSetModified)
	ON_EN_UPDATE(IDC_PLT_RANGEY2,      OnSetModified)
	ON_EN_UPDATE(IDC_PLT_UNITY,        OnChangeValues)
	ON_EN_UPDATE(IDC_PLT_PLOTHEADING,  OnChangeValues)
	ON_BN_CLICKED(IDC_PLT_XDIVFRQ,     OnChangeValues)
	ON_BN_CLICKED(IDC_PLT_XDIVLIN,     OnChangeValues)
	ON_BN_CLICKED(IDC_PLT_XDIVLOG,     OnChangeValues)
	ON_BN_CLICKED(IDC_PLT_YDIVFRQ,     OnChangeValues)
	ON_BN_CLICKED(IDC_PLT_YDIVLIN,     OnChangeValues)
	ON_BN_CLICKED(IDC_PLT_YDIVLOG,     OnChangeValues)
	ON_BN_CLICKED(IDC_PLT_POLAR, OnDiagramType)
	ON_BN_CLICKED(IDC_PLT_X_Y_DIAG, OnDiagramType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPlotRangePropPage 
BOOL CPlotRangePropPage::OnSetActive()
{
   if (m_bValuesChanged && m_pPlot)
   {
      m_sPlotHeading = m_pPlot->GetHeading();
      m_sUnit_x      = m_pPlot->GetXUnit();
      m_sUnit_y      = m_pPlot->GetYUnit();
      RANGE range    = m_pPlot->GetRange();
      m_dRangex1     = range.x1;
      m_dRangex2     = range.x2;
      m_dRangey1     = range.y1;
      m_dRangey2     = range.y2;
      m_nDivision_x  = m_pPlot->GetXDivision()-1;
      if (m_nDivision_x == -1)
      {
         m_nDivision_x  = 0;
         m_nDiagramType = POLAR_DIAGRAM;
      }
      else
      {
         m_nDiagramType = X_Y_DIAGRAM;
      }
      m_nDivision_y  = m_pPlot->GetYDivision()-1;
      CheckDivision();
      m_bEdit        = m_pPlot->IsEditable();
      ((CEdit*) GetDlgItem(IDC_PLT_UNITX))->SetReadOnly(!m_bEdit);
      ((CEdit*) GetDlgItem(IDC_PLT_UNITY))->SetReadOnly(!m_bEdit);

      m_Acc.x = m_pPlot->GetXRound() + 2;
      m_Acc.y = m_pPlot->GetYRound() + 2;

      UpdateData(false);
      m_bValuesChanged = false;
   }
   return CPropertyPage::OnSetActive();
}

void CPlotRangePropPage::SetPlot(CPlotLabel *ppl)
{
   m_pPlot = ppl;
   if (ppl)
   {
      ASSERT_VALID(ppl);
      CLabelContainer *plc = m_pPlot->GetCurveList();
      m_nAllowedCurves = 0;
      plc->ProcessWithLabels(RUNTIME_CLASS(CCurveLabel), &m_nAllowedCurves, CPlotLabel::CheckInsertCurves);
      m_bValuesChanged = true;
   }
}

void CPlotRangePropPage::OnCheckRange()           // wird aufgerufen bei Killfocus
{
   if (UpdateData(true))
   {
      CheckDivision();
   }
}

void CPlotRangePropPage::CheckDivision()
{
   bool bUpdate    = false;
   bool bXdivLin   = true;
   bool bXdivLog   = true;
   bool bPolarPlot = true;
   bool bYdivLog   = true;

   if (m_nAllowedCurves & NO_LOGARITHMIC_CURVES)
   {
      bXdivLog = false;
   }
   else if (m_nAllowedCurves & NO_POLAR_CURVES)
   {
      bPolarPlot = false;
   }
   if (m_dRangex1 < LOG_MIN)
   {
      bXdivLog = false;
      if (m_nDivision_x+1 > CCURVE_LINEAR)
      {
         m_nDivision_x = CCURVE_LINEAR-1;
         bUpdate = true;
      }
   }
   if (m_nDiagramType == POLAR_DIAGRAM)
   {
      bXdivLin = bXdivLog = false;
      m_nDivision_x = -1;
      bUpdate = true;
   }
   else if (m_nDivision_x == -1)
   {
      m_nDivision_x = 0;
      bUpdate = true;
   }
   GetDlgItem(IDC_PLT_XDIVLIN)->EnableWindow(bXdivLin);
   GetDlgItem(IDC_PLT_XDIVLOG)->EnableWindow(bXdivLog);
   GetDlgItem(IDC_PLT_XDIVFRQ)->EnableWindow(bXdivLog);
   GetDlgItem(IDC_PLT_POLAR  )->EnableWindow(bPolarPlot);

   if (m_dRangey1 < LOG_MIN)
   {
      bYdivLog = false;
      if ((m_nDivision_y+1) > CCURVE_LINEAR)
      {
         m_nDivision_y = CCURVE_LINEAR-1;
         bUpdate = true;
      }
   }
   GetDlgItem(IDC_PLT_YDIVLOG)->EnableWindow(bYdivLog);
   GetDlgItem(IDC_PLT_YDIVFRQ)->EnableWindow(bYdivLog);

   if (bUpdate)
   {
      CDataExchange dx(this, false);
   	DDX_Radio(&dx, IDC_PLT_XDIVLIN, m_nDivision_x);
   	DDX_Radio(&dx, IDC_PLT_YDIVLIN, m_nDivision_y);
   }
}

void CPlotRangePropPage::DDX_Division(CDataExchange *, int nIDC, double &min_x)
{
   int            div;
   switch(nIDC)
   {
      case IDC_PLT_XDIVLIN: div = m_nDivision_x+1; break;
      case IDC_PLT_YDIVLIN: div = m_nDivision_y+1; break;
   }

   switch(div)
   {
      case CCURVE_LOGARITHMIC: case CCURVE_FREQUENCY:  min_x = LOG_MIN; break;
      case CCURVE_LINEAR:      case CCURVE_POLAR:      min_x = LIN_MIN; break;
   }
}
void CPlotRangePropPage::MinMaxDouble(CDataExchange*pDX, int nID, double dmin, double dmax, double &dval)
{
   ASSERT(dmin < dmax);
   if (_isnan(dval)) dval = (dmin + dmax) * 0.5;
   bool bChanged = false;
   if      (dval < dmin) dval = dmin, bChanged = true;
   else if (dval > dmax) dval = dmax, bChanged = true;

   if (bChanged && pDX->m_bSaveAndValidate)
   {
      pDX->m_bSaveAndValidate = false;
      DDX_Text(pDX, nID, dval);
      pDX->m_bSaveAndValidate = true;
   }
}


BOOL CPlotRangePropPage::OnKillActive() 
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
      if (RequestSaveChanges(false)) return false;

      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
   return false;
}

void CPlotRangePropPage::OnOK()
{
   if (m_pPlot)
   {
      if (m_nDiagramType == X_Y_DIAGRAM)
         m_pPlot->SetXDivision(m_nDivision_x+1);
      else
         m_pPlot->SetXDivision(0);
      m_pPlot->SetYDivision(m_nDivision_y+1);
      if (m_dRangex1  > m_dRangex2)  swap(m_dRangex1,  m_dRangex2);
      if (m_dRangey1  > m_dRangey2)  swap(m_dRangey1,  m_dRangey2);
      RANGE range = {m_dRangex1, m_dRangex2, m_dRangey1, m_dRangey2};
      m_pPlot->SetRange(&range);
      m_pPlot->SetHeading((TCHAR*) LPCTSTR(m_sPlotHeading));
      if (m_bEdit)
      {
         m_sUnit_x.TrimLeft();
         if (m_sUnit_x.Find(_T("["))==-1) m_sUnit_x = _T("[") + m_sUnit_x;
         m_sUnit_x.TrimRight();
         if (m_sUnit_x.Find(_T("]"))==-1) m_sUnit_x = m_sUnit_x + _T("]");
         m_pPlot->SetXUnit((TCHAR*) LPCTSTR(m_sUnit_x));
         m_sUnit_y.TrimLeft();
         if (m_sUnit_y.Find(_T("["))==-1) m_sUnit_y = _T("[") + m_sUnit_y;
         m_sUnit_y.TrimRight();
         if (m_sUnit_y.Find(_T("]"))==-1) m_sUnit_y = m_sUnit_y + _T("]");
         m_pPlot->SetYUnit((TCHAR*) LPCTSTR(m_sUnit_y));
      }
      m_pPlot->SetChanged(true);
      SetModified(false);
   }
   CPropertyPage::OnOK();
}

BOOL CPlotRangePropPage::OnApply()
{
   return CCaraPropertyPage::OnApply();
}

void CPlotRangePropPage::OnChangeValues()
{
   if (UpdateData(true))
   {
      SetModified(true);
   }
}

void CPlotRangePropPage::OnSetModified()
{
   SetModified(true);
}

void CPlotRangePropPage::OnSetDefaultrange()
{
   if (UpdateData(true))
   {
      RANGE range = {m_dRangex1, m_dRangex2, m_dRangey1, m_dRangey2};
      m_pPlot->SetDefaultRange(&range);
      m_pPlot->SetXDivision(m_nDivision_x+1);
      m_pPlot->SetYDivision(m_nDivision_y+1);
      m_pPlot->SetNewDefaults();
   }
}

void CPlotRangePropPage::OnDeterminRange() 
{
   if (m_pPlot)
   {
      RANGE range    = m_pPlot->DeterminRange();
      m_dRangex1     = range.x1;
      m_dRangex2     = range.x2;
      m_dRangey1     = range.y1;
      m_dRangey2     = range.y2;
      UpdateData(false);
      SetModified(true);
   }
}

BOOL CPlotRangePropPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
   ::SetWindowLong(m_hWnd, GWL_ID, CPlotRangePropPage::IDD);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CPlotRangePropPage::OnDiagramType() 
{
   CDataExchange dx(this, true);
	DDX_Radio(&dx, IDC_PLT_POLAR, m_nDiagramType);
   CheckDivision();
   SetModified(true);
}
