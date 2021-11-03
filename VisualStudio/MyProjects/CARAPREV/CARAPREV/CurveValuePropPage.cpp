// CurveValuePropPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CurveValuePropPage.h"
#include "CARAPREV.h"
#include "PlotLabel.h"
#include "CurveLabel.h"
#include "CEtsDiv.h"
#include ".\curvevalueproppage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CCurveValuePropPage 

IMPLEMENT_DYNCREATE(CCurveValuePropPage, CCaraPropertyPage)

long CCurveValuePropPage::gm_lOldEditCtrlWndProc = 0;

int CCurveValuePropPage::gm_nCurveValueCtrl[] =
{ 
   IDC_CURVE_INDEX_TXT, IDC_CURVE_INDEX, IDC_CURVE_INDEX_SPIN,
   IDC_CURVE_VALUE_X_TXT, IDC_CURVE_VALUE_X,
   IDC_CURVE_VALUE_Y_TXT, IDC_CURVE_VALUE_Y,
   IDC_CURVE_ORIGIN_TXT, IDC_CURVE_ORIGIN,
   IDC_CURVE_STEP_TXT, IDC_CURVE_STEP,
   IDC_CURVE_OFFSETY_TXT, IDC_CURVE_OFFSETY,
   IDC_CURVE_END_TXT, IDC_CURVE_END,
   0
};

int CCurveValuePropPage::gm_nCurveUnitsCtrl[] =
{
   IDC_CURVE_UNIT_X_TXT, IDC_CURVE_UNIT_X,
   IDC_CURVE_UNIT_Y_TXT, IDC_CURVE_UNIT_Y,
   IDC_CURVE_UNIT_LOCUS_TXT, IDC_CURVE_UNIT_LOCUS,
   0
};

int CCurveValuePropPage::gm_nCurveParamCtrl[] =
{
   IDC_CURVE_EDITABLE,
   IDC_CURVE_EDITMODE1,
   IDC_CURVE_EDITMODE2,
   IDC_CURVE_WIDTH_TXT, IDC_CURVE_WIDTH, IDC_CURVE_WIDTH_SPIN,
   IDC_CURVE_COLOR,
   IDC_COMBO_SCATTER,
   IDC_CURVE_VISIBLE,
   IDC_CURVE_FORMAT_TXT, IDC_CURVE_FORMAT,
   0
};

int CCurveValuePropPage::gm_nCurveValueListCtrl[] =
{
   IDC_CURVE_VALUE_LIST, 0
};

CCurveValuePropPage::CCurveValuePropPage() : CCaraPropertyPage(CCurveValuePropPage::IDD)
{
   //{{AFX_DATA_INIT(CCurveValuePropPage)
   m_sUnit_x        = _T("");
   m_sUnit_y        = _T("");
   m_fXValue        = 0.0;
   m_fYValue        = 0.0;
   m_nCurveWidth    = 0;
   m_bEditable      = FALSE;
   m_nEditMode      = 0;
   m_bVisible       = FALSE;
   m_dOffset        = 0.0;
	m_nCurveIndex    = 0;
	m_dOrigin        = 0.0;
	m_dStep          = 0.0;
	m_sUnitLocus     = _T("");
	m_nFormat        = -1;
	m_dEnd           = 0.0;
	//}}AFX_DATA_INIT
   m_pCurve            = NULL;
   m_pPlot             = NULL;
   m_bValuesChanged    = true;
   m_nCursel           = -1;
   m_nItem             = -1;
   m_nSubItem          = -1;
   m_nNoTabCtrlChange  = 0;
   m_Acc.x             = 4;
   m_Acc.y             = 4;
   m_bGradientChangeable = false;
}

CCurveValuePropPage::~CCurveValuePropPage()
{
}

void CCurveValuePropPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CCurveValuePropPage)
   DDX_Control(pDX, IDC_CURVE_VALUE_LIST, m_cValueList);
   DDX_Control(pDX, IDC_CURVE_LIST, m_cCurveList);
   DDX_Control(pDX, IDC_CURVE_INDEX_SPIN, m_cCurveIndex);
   DDX_Text(pDX, IDC_CURVE_UNIT_X, m_sUnit_x);
   DDX_Text(pDX, IDC_CURVE_UNIT_Y, m_sUnit_y);
   DDX_Text(pDX, IDC_CURVE_WIDTH, m_nCurveWidth);
   DDX_Check(pDX, IDC_CURVE_EDITABLE, m_bEditable);
   DDX_Radio(pDX, IDC_CURVE_EDITMODE1, m_nEditMode);
   DDX_Check(pDX, IDC_CURVE_VISIBLE, m_bVisible);
   DDX_Text(pDX, IDC_CURVE_INDEX, m_nCurveIndex);
   DDX_Text(pDX, IDC_CURVE_UNIT_LOCUS, m_sUnitLocus);
   DDX_CBIndex(pDX, IDC_CURVE_FORMAT, m_nFormat);
   DDX_Control(pDX, IDC_COMBO_SCATTER, m_ComboScatter);
   //}}AFX_DATA_MAP

   DDX_TextVar(pDX, IDC_CURVE_OFFSETY, DBL_DIG, m_dOffset);
   DDX_TextVar(pDX, IDC_CURVE_ORIGIN, DBL_DIG, m_dOrigin);
   DDX_TextVar(pDX, IDC_CURVE_END, DBL_DIG, m_dEnd);

   if (!pDX->m_bSaveAndValidate)
   {
      DDX_TextVar(pDX, IDC_CURVE_VALUE_X, DBL_DIG, m_fXValue);
      DDX_TextVar(pDX, IDC_CURVE_VALUE_Y, DBL_DIG, m_fYValue);
   }
   else if (m_bEditable)
   {
      double  dVal = m_fYValue;
      CString str;
      DDX_Text(pDX, IDC_CURVE_VALUE_Y, str);
      if (str.Find("--") != -1)
      {
         m_fYValue = sqrt((double)-1);
      }
      else if (!DDX_TextVar(pDX, IDC_CURVE_VALUE_Y, m_Acc.y, dVal, false))
      {
         pDX->m_bSaveAndValidate = false;
         DDX_TextVar(pDX, IDC_CURVE_VALUE_Y, DBL_DIG, m_fYValue, false);
         pDX->m_bSaveAndValidate = true;
      }
      else
      {
         m_fYValue = dVal;
      }
   }
   if (m_bGradientChangeable)
   {
      DDX_Text(pDX, IDC_CURVE_STEP, m_dStep);
   }
   else
   {
      if (!pDX->m_bSaveAndValidate)
      {
         CString str = _T("--");
         DDX_Text(pDX, IDC_CURVE_STEP, str);
      }
   }
}


BEGIN_MESSAGE_MAP(CCurveValuePropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCurveValuePropPage)
	ON_EN_CHANGE(IDC_CURVE_INDEX, OnChangeCurveIndex)
	ON_BN_CLICKED(IDC_CURVE_COLOR, OnCurveColor)
	ON_BN_CLICKED(IDC_CURVE_EDITABLE, OnEditable)
	ON_EN_UPDATE(IDC_CURVE_UNIT_X,   OnChangeValues)
	ON_EN_KILLFOCUS(IDC_CURVE_VALUE_Y, OnKillfocusCurveValueY)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CURVE_LIST, OnEndlabeleditCurveList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CURVE_LIST, OnItemchangedCurveList)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_CURVE_LIST, OnBeginlabeleditCurveList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_CURVE_TAB, OnSelchangeCurveTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_CURVE_TAB, OnSelchangingCurveTab)
	ON_CBN_SELCHANGE(IDC_CURVE_FORMAT, OnSelchangeCurveFormat)
	ON_EN_KILLFOCUS(IDC_CURVE_END, OnKillfocusCurveEnd)
	ON_EN_KILLFOCUS(IDC_CURVE_ORIGIN, OnKillfocusCurveOrigin)
	ON_EN_KILLFOCUS(IDC_CURVE_STEP, OnKillfocusCurveStep)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CURVE_VALUE_LIST, OnGetdispinfoCurveValueList)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_CURVE_VALUE_LIST, OnBeginlabeleditCurveValueList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CURVE_VALUE_LIST, OnEndlabeleditCurveValueList)
	ON_NOTIFY(NM_RCLICK, IDC_CURVE_VALUE_LIST, OnRclickCurveValueList)
	ON_EN_UPDATE(IDC_CURVE_UNIT_Y,   OnChangeValues)
	ON_EN_UPDATE(IDC_CURVE_WIDTH,    OnChangeValues)
	ON_BN_CLICKED(IDC_CURVE_EDITMODE2,     OnChangeValues)
	ON_BN_CLICKED(IDC_CURVE_VISIBLE,       OnChangeValues)
	ON_BN_CLICKED(IDC_CURVE_EDITMODE1,     OnChangeValues)
	ON_EN_UPDATE(IDC_CURVE_OFFSETY,    OnChangeValues)
	ON_EN_UPDATE(IDC_CURVE_UNIT_LOCUS, OnChangeValues)
   ON_CBN_SELCHANGE(IDC_COMBO_SCATTER, OnChangeValues)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCurveValuePropPage 
BOOL CCurveValuePropPage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   ::SetWindowLong(m_hWnd, GWL_ID, CCurveValuePropPage::IDD);
   m_cCurveIndex.SetBuddy(GetDlgItem(IDC_CURVE_INDEX));
   SendDlgItemMessage(IDC_CURVE_WIDTH_SPIN, UDM_SETRANGE32, 0, 20);
   CRect rc;
   m_cCurveList.GetClientRect(&rc);
   m_cCurveList.InsertColumn(0, "", LVCFMT_LEFT, rc.right-2, 0);
   m_cCurveList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

   CString string;
   m_cValueList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

   m_cValueList.InsertColumn(0, _T("i"),  LVCFMT_RIGHT, 20);
   
   GetDlgItem(IDC_CURVE_VALUE_X_TXT)->GetWindowText(string);
   m_cValueList.InsertColumn(1, string,  LVCFMT_LEFT, 30);

   GetDlgItem(IDC_CURVE_VALUE_Y_TXT)->GetWindowText(string);
   m_cValueList.InsertColumn(2, string,  LVCFMT_LEFT, 30);

   CWnd * pWnd = GetDlgItem(IDC_CURVE_TAB);
   if (pWnd)
   {
      CString str;
      TCITEMA tcItem;
      tcItem.mask       = TCIF_TEXT|TCIF_PARAM;

      str.LoadString(IDS_CURVE_PARAMS);
      tcItem.pszText    = (TCHAR*)LPCTSTR(str);
      tcItem.cchTextMax = str.GetLength();;
      tcItem.lParam = 0;
      pWnd->SendMessage(TCM_INSERTITEM, (WPARAM)tcItem.lParam++, (LPARAM) &tcItem);

      str.LoadString(IDS_CURVE_VALUES);
      tcItem.pszText    = (TCHAR*)LPCTSTR(str);
      tcItem.cchTextMax = str.GetLength();;
      pWnd->SendMessage(TCM_INSERTITEM, (WPARAM)tcItem.lParam++, (LPARAM) &tcItem);

      str.LoadString(IDS_CURVE_VALUE_LIST);
      tcItem.pszText    = (TCHAR*)LPCTSTR(str);
      tcItem.cchTextMax = str.GetLength();;
      pWnd->SendMessage(TCM_INSERTITEM, (WPARAM)tcItem.lParam++, (LPARAM) &tcItem);

      str.LoadString(IDS_CURVE_UNITS);
      tcItem.pszText    = (TCHAR*)LPCTSTR(str);
      tcItem.cchTextMax = str.GetLength();;
      pWnd->SendMessage(TCM_INSERTITEM, (WPARAM)tcItem.lParam++, (LPARAM) &tcItem);

   }

   OnSelchangeCurveTab(  NULL, (LRESULT*)0); // sichbar
   OnSelchangingCurveTab(NULL, (LRESULT*)1); // nicht sichbar
   OnSelchangingCurveTab(NULL, (LRESULT*)2); // nicht sichbar
   OnSelchangingCurveTab(NULL, (LRESULT*)3); // nicht sichbar

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
void CCurveValuePropPage::SetPlot(CPlotLabel *ppl)
{
   m_pPlot = ppl;
   if (ppl)
   {
      ASSERT_VALID(ppl);
      m_bValuesChanged = true;
   }
}
BOOL CCurveValuePropPage::OnSetActive()
{
   if (m_pPlot)
   {
      int i, count;
      if (m_bValuesChanged)
      {
         SetCurveListContent();
         CLabelContainer *plc   = m_pPlot->GetCurveList();
         count = plc->GetCount();
         for (i=0; i<count; i++) if (plc->GetLabel(i)->IsPicked()) break;
         if (i >= count) i=0;
         m_Acc.x = m_pPlot->GetXRound() + 2;
         m_Acc.y = m_pPlot->GetYRound() + 2;
         m_bValuesChanged = false;
      }
      else
      {
         i = m_cCurveList.GetSelectionMark();
         if (i == LB_ERR) i=0;
      }
      SetCurve(i);

      ((CEdit*)   GetDlgItem(IDC_CURVE_VALUE_X))->SetReadOnly(true);

      UpdateData(false);
   }
   return CPropertyPage::OnSetActive();
}

BOOL CCurveValuePropPage::SetCurve(int index)
{
   CString str;
   int nCurveValues = 0;
   int nColumns = 0, nNeededColumns;
   CRect rcClient;
   LVCOLUMN lvc;
   CCurveLabel* pcl      = (CCurveLabel*) m_pPlot->GetCurveList()->GetLabel(index);
   m_bGradientChangeable = false;
   if (!pcl) return false;                                     // Bei Fehler: Ende

   if (m_pCurve && (pcl != m_pCurve))                          // war vorher eine Kurve selektiert, und wird jetzt eine andere selektiert
   {
      int nR = RequestSaveChanges(true);
      if (nR & CHANGES_REJECTED)
      {
         m_cCurveList.SetItemText(m_nCursel, 0, m_pCurve->GetText());
         m_cCurveList.SetItemData(m_nCursel, 0);
      }
   }

   SetModified(false);
   m_pCurve      = pcl;
   nCurveValues  = m_pCurve->GetNumValues();
   m_nCursel     = index;
   m_cCurveIndex.SetRange32(0, nCurveValues-1);
   m_cCurveIndex.SetPos(nCurveValues/2);
   OnChangeCurveIndex();

   m_cValueList.SetItemCount(nCurveValues);
   CHeaderCtrl* pCtrl = m_cValueList.GetHeaderCtrl();
	ASSERT(pCtrl != NULL);
	if (pCtrl != NULL) nColumns = pCtrl->GetItemCount();
   nNeededColumns = (m_pCurve->GetLocus() == 0) ? 3 : 4;
   
   if (nColumns > nNeededColumns)
   {
      m_cValueList.DeleteColumn(nColumns-1);
      nColumns--;
   }
   else if (nColumns < nNeededColumns)
   {
      CString string;
      string.LoadString(IDS_CURVE_LOCUS_AXIS);
      m_cValueList.InsertColumn(3, string,  LVCFMT_LEFT, 30);
      nColumns++;
   }

   m_cCurveList.SetSelectionMark(index);
   m_cCurveList.SetHotItem(index);
   m_cCurveList.EnsureVisible(index, false);

   lvc.mask = LVCF_WIDTH;
   m_cValueList.GetWindowRect(&rcClient);
   lvc.cx = (rcClient.right-rcClient.left) / nColumns;
   int nScrollBarwidth = ::GetSystemMetrics(SM_CXVSCROLL);
   lvc.cx -= nScrollBarwidth;
   m_cValueList.SetColumn(0, &lvc);
   lvc.cx += nScrollBarwidth;

   lvc.mask |= LVCF_TEXT;
   m_sUnit_x     = _T(m_pCurve->GetXUnit());
   lvc.pszText    = (TCHAR*)LPCTSTR(m_sUnit_x);
   lvc.cchTextMax = m_sUnit_x.GetLength();
   m_cValueList.SetColumn(1, &lvc);

   m_sUnit_y     = _T(m_pCurve->GetYUnit());
   lvc.pszText    = (TCHAR*)LPCTSTR(m_sUnit_y);
   lvc.cchTextMax = m_sUnit_y.GetLength();
   m_cValueList.SetColumn(2, &lvc);

   if (nColumns>3)
   {
      if (m_pCurve->GetLocusUnit())
      {
         m_sUnitLocus  = _T(m_pCurve->GetLocusUnit());
         lvc.pszText    = (TCHAR*)LPCTSTR(m_sUnitLocus);
         lvc.cchTextMax = m_sUnitLocus.GetLength();
      }
      else lvc.mask &= ~LVCF_TEXT;
      m_cValueList.SetColumn(3, &lvc);
   }

   m_nCurveWidth = m_pCurve->GetWidth();
   if (m_nCurveWidth>1)
   {
      SIZE sz = {m_nCurveWidth, m_nCurveWidth};
      CDC *pDC = GetDC();
		pDC->SetMapMode(MM_LOMETRIC);
      pDC->LPtoDP(&sz);
      ReleaseDC(pDC);
      m_nCurveWidth = sz.cx;
   }
   m_bEdit       = !m_pCurve->IsWriteProtect();
   m_bEditable   = m_pCurve->IsEditable() && m_bEdit;
   m_nEditMode   = m_pCurve->IsSplineMode();
   m_bVisible    = m_pCurve->IsVisible();
   m_CurveColor  = m_pCurve->GetColor();
   if (m_pCurve->IsScatter())
   {
      m_ComboScatter.SetCurSel(m_pCurve->GetScatterForm()+1);
   }
   else
   {
      m_ComboScatter.SetCurSel(0);
   }

   if (m_pCurve->GetXDivision() == CCURVE_LINEAR)
      str.LoadString(IDS_CURVE_STEP_LIN);
   else
      str.LoadString(IDS_CURVE_STEP_LOG);
   GetDlgItem(IDC_CURVE_STEP_TXT)->SetWindowText(str);

   m_nFormat     = m_pCurve->GetFormat();
   m_dOffset     = m_pCurve->GetOffset();

	m_dOrigin = m_dOldOrigin = m_pCurve->GetX_Value(0);
	m_dStep   = m_dOldStep   = m_pCurve->GetXStep();
   m_dEnd    = m_pCurve->GetX_Value(nCurveValues-1);
   
   if ((m_nFormat >= CF_SINGLE_DOUBLE) || m_pCurve->GetLocus()) // Single Format || Ortskurve
      m_bGradientChangeable = true;

   ((CEdit*)   GetDlgItem(IDC_CURVE_STEP     ))->SetModify(false);
   ((CEdit*)   GetDlgItem(IDC_CURVE_ORIGIN   ))->SetModify(false);
   ((CEdit*)   GetDlgItem(IDC_CURVE_END      ))->SetModify(false);


   ((CButton*) GetDlgItem(IDC_CURVE_EDITABLE ))->EnableWindow(m_bEdit);

   ((CEdit*)   GetDlgItem(IDC_CURVE_VALUE_Y  ))->SetReadOnly(!m_bEditable);

   ((CButton*) GetDlgItem(IDC_CURVE_EDITMODE1))->EnableWindow(m_bEditable);
   ((CButton*) GetDlgItem(IDC_CURVE_EDITMODE2))->EnableWindow(m_bEditable);

   ((CEdit*)   GetDlgItem(IDC_CURVE_UNIT_X    ))->SetReadOnly(!m_bEditable);
   ((CEdit*)   GetDlgItem(IDC_CURVE_UNIT_Y    ))->SetReadOnly(!m_bEditable);
   ((CEdit*)   GetDlgItem(IDC_CURVE_UNIT_LOCUS))->SetReadOnly(!m_bEditable);

   bool bReadOnly = (!m_bEditable || (m_nFormat < CF_SINGLE_DOUBLE))?true:false;
   ((CEdit*)   GetDlgItem(IDC_CURVE_STEP     ))->SetReadOnly(bReadOnly);
   ((CEdit*)   GetDlgItem(IDC_CURVE_ORIGIN   ))->SetReadOnly(bReadOnly);
   ((CEdit*)   GetDlgItem(IDC_CURVE_END      ))->SetReadOnly(bReadOnly);

   return true;
}

void CCurveValuePropPage::OnChangeCurveIndex()
{
   if (m_pCurve != NULL)
   {
      CDataExchange dx(this, true);
    	DDX_Text(&dx, IDC_CURVE_INDEX, m_nCurveIndex);
      int nValues =  m_pCurve->GetNumValues();
      if (m_nCurveIndex<0) m_nCurveIndex = nValues-1;
      if (m_nCurveIndex >= nValues) m_nCurveIndex = 0;

      m_fXValue = m_pCurve->GetX_Value(m_nCurveIndex);
      m_fYValue = m_pCurve->GetY_Value(m_nCurveIndex);
      dx.m_bSaveAndValidate = false;
      DDX_TextVar(&dx, IDC_CURVE_VALUE_X, DBL_DIG, m_fXValue, false);
      
      CString str;
      DDX_Text(&dx, IDC_CURVE_VALUE_Y, str);
      if (str.Find(_T("--")) != -1)
      {
         m_fYValue = sqrt((double)-1);
      }
      else
      {
         DDX_TextVar(&dx, IDC_CURVE_VALUE_Y, DBL_DIG, m_fYValue, false);
      }
   }
}

void CCurveValuePropPage::OnCurveColor() 
{
   CString string;
   CColorDialog cd(m_CurveColor, 0, this);
   cd.m_cc.lpCustColors = CPlotLabel::gm_StdPltSets.sm_CurveColors;
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   cd.m_cc.Flags       |=  CC_FULLOPEN;

   string.LoadString(IDS_COLOR_OF_CURVE);
   if (m_pCurve)
   {
      string += _T(m_pCurve->GetText());
   }
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal()==IDOK)
   {
      COLORREF color = cd.GetColor();
      if (m_CurveColor != color)
      {
         m_CurveColor = color;
         SetModified(true);
      }
   }
   g_AfxCommDlgProc = NULL;
}

void CCurveValuePropPage::OnEditable() 
{
   if (UpdateData(true))
   {
      ((CEdit*)   GetDlgItem(IDC_CURVE_VALUE_Y  ))->SetReadOnly(!m_bEditable);
      ((CEdit*)   GetDlgItem(IDC_CURVE_STEP     ))->SetReadOnly(!m_bEditable);
      ((CEdit*)   GetDlgItem(IDC_CURVE_ORIGIN   ))->SetReadOnly(!m_bEditable);
      ((CEdit*)   GetDlgItem(IDC_CURVE_END      ))->SetReadOnly(!m_bEditable);
      ((CButton*) GetDlgItem(IDC_CURVE_EDITMODE1))->EnableWindow(m_bEditable);
      ((CButton*) GetDlgItem(IDC_CURVE_EDITMODE2))->EnableWindow(m_bEditable);

      ((CEdit*)   GetDlgItem(IDC_CURVE_UNIT_X    ))->SetReadOnly(!m_bEditable);
      ((CEdit*)   GetDlgItem(IDC_CURVE_UNIT_Y    ))->SetReadOnly(!m_bEditable);
      ((CEdit*)   GetDlgItem(IDC_CURVE_UNIT_LOCUS))->SetReadOnly(!m_bEditable);
      SetModified(true);
   }
   else UpdateData(false);
}

BOOL CCurveValuePropPage::OnKillActive() 
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
      if      (GetDlgItem(IDC_CURVE_COLOR)==pFocus) OnCurveColor();
      else if (RequestSaveChanges(true)) return false;

      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
   return false;
}

void CCurveValuePropPage::OnOK()
{
   if (m_pCurve)
   {
      if (m_bEdit)
      {
         m_pCurve->SetXUnit(m_sUnit_x);
         m_pCurve->SetYUnit(m_sUnit_y);
         if (!m_sUnitLocus.IsEmpty()) m_pCurve->SetLocusUnit(m_sUnitLocus);
         m_pCurve->SetEditable((m_bEditable) ? true : false);
         m_pCurve->SetSplineMode((m_nEditMode) ? true : false);
      }

      m_pCurve->SetVisible(m_bVisible ? true : false);
      if (m_nCurveWidth > 1)
      {
         SIZE sz = {m_nCurveWidth, m_nCurveWidth};
         CDC *pDC = GetDC();
		   pDC->SetMapMode(MM_LOMETRIC);
         pDC->DPtoLP(&sz);
         ReleaseDC(pDC);
         m_pCurve->SetWidth(sz.cx);
      }
      else m_pCurve->SetWidth(m_nCurveWidth);
      m_pCurve->SetColor(m_CurveColor);
      m_pCurve->SetOffset(m_dOffset);
      m_pCurve->SetXOrigin(m_dOrigin);
      m_pCurve->SetXStep(m_dStep);

      int nFormat = m_pCurve->GetFormat();
      if (nFormat != m_nFormat)
      {  // ist das Kurvenformat schon single oder 
         if ((nFormat >= CF_SINGLE_DOUBLE) || m_pCurve->AreXvaluesEqualySpaced())
         {
            CFloatPrecision fp;
            m_pCurve->ChangeFormat(m_nFormat);
         }
         else
         {
            m_nFormat = nFormat;
            if (m_hWnd)
            {
               CDataExchange dx(this, false);
   	         DDX_CBIndex(&dx, IDC_CURVE_FORMAT, m_nFormat);
            }
         }
      }

      CString string, windowtext;
      if (m_nCursel != -1)
      {
         if (m_cCurveList.GetItemData(m_nCursel))
         {
            m_pCurve->GetString() = m_cCurveList.GetItemText(m_nCursel, 0);
         }
      }
      int nScatter = m_ComboScatter.GetCurSel();
      if (nScatter)
      {
         m_pCurve->SetScatter(true);
         m_pCurve->SetScatterForm(nScatter-1);
      }
      else
      {
         m_pCurve->SetScatter(false);
      }
      if (m_pPlot) m_pPlot->SetChanged(true);
      SetModified(false);
   }
   CPropertyPage::OnOK();
}

BOOL CCurveValuePropPage::OnApply()
{
   return CCaraPropertyPage::OnApply();
}

void CCurveValuePropPage::SetCurveListContent()
{
   CLabelContainer  *plc;
   CCurveLabel      *pcl;
   int               i, numCurves;
   m_cCurveList.DeleteAllItems();
   if (m_pPlot)
   {
      plc = m_pPlot->GetCurveList();
      numCurves = plc->GetCount();
      CRect rc;
      m_cCurveList.GetClientRect(&rc);
      if (numCurves > 3) rc.right -= 20;
      else               rc.right -=  2;
      m_cCurveList.SetColumnWidth(0, rc.right);
      for (i=0; i<numCurves; i++)
      {
         pcl = (CCurveLabel*) plc->GetLabel(i);
         if (pcl)
         {
            m_cCurveList.InsertItem(i, (LPCTSTR)pcl->GetText());
         }
      }
   }
}

void CCurveValuePropPage::OnChangeValues()
{
   SetModified(true);
}


void CCurveValuePropPage::OnKillfocusCurveValueY() 
{
   if (m_pCurve && m_bEditable)
   {
      CEdit *pEdit = (CEdit*) GetDlgItem(IDC_CURVE_VALUE_Y);
      if (pEdit->GetModify())
      {
         CDataExchange dx(this, true);                         // Wert holen
	      if (!DDX_TextVar(&dx, IDC_CURVE_VALUE_Y, DBL_DIG, m_fYValue, false))
         {
            if (m_nNoTabCtrlChange & 2) m_nNoTabCtrlChange = 1;// Fehler
            dx.m_bSaveAndValidate = false;
            DDX_TextVar(&dx, IDC_CURVE_VALUE_Y, DBL_DIG, m_fYValue, false);
         }
         else
         {
            m_pCurve->SetY_Value(m_cCurveIndex.GetPos(), m_fYValue);
         }
      }
   }
}

void CCurveValuePropPage::OnBeginlabeleditCurveList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	m_cCurveList.SetItemData(pDispInfo->item.iItem, 1);
	*pResult = 0;
}

void CCurveValuePropPage::OnEndlabeleditCurveList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   BOOL bChanged = 0;
   if (m_pCurve)
   {
      const TCHAR *pnew = pDispInfo->item.pszText,
           *pold = m_pCurve->GetText();
      if ((pnew != NULL) && (pold != NULL) && (strcmp(pnew, pold) !=0))
         bChanged = true;
   }
	m_cCurveList.SetItemData(pDispInfo->item.iItem, bChanged);
   SetModified(bChanged);
	*pResult = 1;
}

void CCurveValuePropPage::OnItemchangedCurveList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != m_nCursel)
   {
      if (SetCurve(pNMListView->iItem))
      {
         UpdateData(false);
      }
   }
	*pResult = 0;
}


void CCurveValuePropPage::OnSelchangeCurveTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_nNoTabCtrlChange & 1)
   {
      if ((pNMHDR != NULL) && (pResult != NULL))
         ::PostMessage(pNMHDR->hwndFrom, TCM_SETCURSEL, HIWORD(m_nNoTabCtrlChange), 0);
      m_nNoTabCtrlChange = 0;
      return;
   }
   int *pnCtrl;
	int i, nCursel;
   if ((pNMHDR != NULL) && (pResult != NULL))
   {
      nCursel= ::SendMessage(pNMHDR->hwndFrom, TCM_GETCURSEL, 0, 0);
	   *pResult = 0;
   }
   else nCursel = (int) pResult;

   switch (nCursel)
   {
      case  0: pnCtrl = (int*)gm_nCurveParamCtrl; break;    // Parameter
      case  1: pnCtrl = (int*)gm_nCurveValueCtrl; break;    // Werte
      case  2: pnCtrl = (int*)gm_nCurveValueListCtrl;break; // Werteliste
      case  3: pnCtrl = (int*)gm_nCurveUnitsCtrl; break;    // Einheiten
      default: return;
   }
   m_nNoTabCtrlChange = MAKELONG(0, nCursel);
   for (i=0; pnCtrl[i] != 0; i++)
   {
      GetDlgItem(pnCtrl[i])->ShowWindow(SW_SHOW);
   }
}

void CCurveValuePropPage::OnSelchangingCurveTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
   m_nNoTabCtrlChange |= 2;
   ::SetFocus(::GetDlgItem(m_hWnd, IDC_CURVE_TAB));
   if (m_nNoTabCtrlChange & 1)
   {
      return;
   }
   int *pnCtrl;
	int i, nCursel;
   if ((pNMHDR != NULL) && (pResult != NULL))
   {
      nCursel= ::SendMessage(pNMHDR->hwndFrom, TCM_GETCURSEL, 0, 0);
	   *pResult = 0;
   }
   else nCursel = (int) pResult;

   switch (nCursel)
   {
      case  0: pnCtrl = (int*)gm_nCurveParamCtrl; break;    // Parameter
      case  1: pnCtrl = (int*)gm_nCurveValueCtrl; break;    // Werte
      case  2: pnCtrl = (int*)gm_nCurveValueListCtrl;break; // Werteliste
      case  3: pnCtrl = (int*)gm_nCurveUnitsCtrl; break;    // Einheiten
      default: return;
   }

   for (i=0; pnCtrl[i] != 0; i++)
   {
      GetDlgItem(pnCtrl[i])->ShowWindow(SW_HIDE);
   }
}
 
void CCurveValuePropPage::OnSelchangeCurveFormat() 
{
   int nFormat = m_nFormat;
   if (AfxMessageBox(IDS_CHANGE_FORMAT_REQUEST, MB_ICONQUESTION|MB_YESNO)	== IDNO)
   {
      CDataExchange dx(this, false);
      m_nFormat = nFormat;
   	  DDX_CBIndex(&dx, IDC_CURVE_FORMAT, m_nFormat);
   }
   else
      SetModified(true);
}

void CCurveValuePropPage::OnKillfocusCurveEnd() 
{
   if (m_bGradientChangeable)
   {
      CEdit *pEdit = (CEdit*) GetDlgItem(IDC_CURVE_END);
      if (pEdit->GetModify())
      {
         CDataExchange dx(this, true);                         // Wert holen
	      if (!DDX_TextVar(&dx, IDC_CURVE_END, DBL_DIG, m_dEnd, false))
         {
            if (m_nNoTabCtrlChange & 2) m_nNoTabCtrlChange |= 1;// Fehler
            dx.m_bSaveAndValidate = false;
            DDX_TextVar(&dx, IDC_CURVE_END, DBL_DIG, m_dEnd, false);
         }
         else
         {
            m_pCurve->SetXOrigin(m_dOrigin);                   // Anfangswerte setzen
            m_pCurve->SetX_Value(m_pCurve->GetNumValues()-1, m_dEnd);
            m_dStep = m_pCurve->GetXStep();                    // Berecheten Wert holen
            dx.m_bSaveAndValidate = false;
	         DDX_TextVar(&dx, IDC_CURVE_STEP, DBL_DIG, m_dStep, false);// und anzeigen
            m_pCurve->SetXOrigin(m_dOldOrigin);                // alte Werte zurücksetzen
            m_pCurve->SetXStep(m_dOldStep);
            pEdit->SetModify(false);                           // Edit Not Modified
            SetModified(true);
         }
      }
   }
}

void CCurveValuePropPage::OnKillfocusCurveOrigin() 
{
   if (m_bGradientChangeable)
   {
      CEdit *pEdit = (CEdit*) GetDlgItem(IDC_CURVE_ORIGIN);
      if (pEdit->GetModify())
      {
         CDataExchange dx(this, true);                         // Wert holen
	      if (!DDX_TextVar(&dx, IDC_CURVE_ORIGIN, DBL_DIG, m_dOrigin, false))
         {
            if (m_nNoTabCtrlChange & 2) m_nNoTabCtrlChange |= 1;// Fehler
            dx.m_bSaveAndValidate = false;
            DDX_TextVar(&dx, IDC_CURVE_ORIGIN, DBL_DIG, m_dOrigin, false);
         }
         else
         {
            m_pCurve->SetXOrigin(m_dOrigin);                   // Anfangswerte setzen
            m_pCurve->SetX_Value(m_pCurve->GetNumValues()-1, m_dEnd);
            m_dEnd = m_pCurve->GetX_Value(m_pCurve->GetNumValues()-1);// Berecheten Wert holen
            dx.m_bSaveAndValidate = false;
	         DDX_TextVar(&dx, IDC_CURVE_END, DBL_DIG, m_dEnd, false); // und anzeigen
            m_pCurve->SetXOrigin(m_dOldOrigin);                // alte Werte zurücksetzen
            m_pCurve->SetXStep(m_dOldStep);
            pEdit->SetModify(false);                           // Edit Not Modified
            SetModified(true);
         }
      }
   }
}

void CCurveValuePropPage::OnKillfocusCurveStep() 
{
   if (m_bGradientChangeable)
   {
      CEdit *pEdit = (CEdit*) GetDlgItem(IDC_CURVE_STEP);
      if (pEdit->GetModify())
      {
         CDataExchange dx(this, true);                         // Wert holen
	      if (!DDX_TextVar(&dx, IDC_CURVE_STEP, DBL_DIG, m_dStep, false))
         {
            if (m_nNoTabCtrlChange & 2) m_nNoTabCtrlChange |= 1;// Fehler
            dx.m_bSaveAndValidate = false;
            DDX_TextVar(&dx, IDC_CURVE_STEP, DBL_DIG, m_dStep, false);
         }
         else
         {
            m_pCurve->SetXOrigin(m_dOrigin);                   // Anfangswerte setzen
            m_pCurve->SetXStep(m_dStep);
            m_dEnd = m_pCurve->GetX_Value(m_pCurve->GetNumValues()-1);// Berecheten Wert holen
            dx.m_bSaveAndValidate = false;
	         DDX_TextVar(&dx, IDC_CURVE_END, DBL_DIG, m_dEnd, false); // und anzeigen
            m_pCurve->SetXOrigin(m_dOldOrigin);                // alte Werte zurücksetzen
            m_pCurve->SetXStep(m_dOldStep);
            pEdit->SetModify(false);                           // Edit Not Modified
            SetModified(true);
         }
      }
   }
}

void CCurveValuePropPage::OnGetdispinfoCurveValueList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if (m_pCurve)
   {
      ETSDIV_NUTOTXT ntx;
      ntx.nmaxl  = pDispInfo->item.cchTextMax;
      ntx.nmode  = ETSDIV_NM_STD;
      ntx.nround = 2;
   	*pResult = 0;
      if(pDispInfo->item.mask & LVIF_TEXT)
      {
         switch (pDispInfo->item.iSubItem)
         {
            case 0: wsprintf(pDispInfo->item.pszText, _T("%d"), pDispInfo->item.iItem+1); return;
            case 1:
               ntx.fvalue = m_pCurve->GetX_Value(pDispInfo->item.iItem);
               ntx.nmode  = m_pPlot->GetXNumModeEx();
               ntx.nround = m_Acc.x;
               break;
            case 2:
               ntx.fvalue = m_pCurve->GetY_Value(pDispInfo->item.iItem);
               ntx.nmode  = m_pPlot->GetYNumModeEx();
               ntx.nround = m_Acc.y;
               break;
            case 3: ntx.fvalue = m_pCurve->GetLocusValue(pDispInfo->item.iItem); break;
         }
         if (::_isnan(ntx.fvalue))
         {
            strcpy(pDispInfo->item.pszText, _T("--"));
            return;
         }
         if (CEtsDiv::NumberToText(&ntx, pDispInfo->item.pszText))
            return;
      }
   }

	*pResult = 1;
}

void CCurveValuePropPage::OnBeginlabeleditCurveValueList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
 	*pResult = 1;
   if (m_pCurve && m_bEditable)
   {
      LVHITTESTINFO lvHTI;
      if (m_nSubItem == -1)
      {
         ::GetCursorPos(&lvHTI.pt);
         m_cValueList.ScreenToClient(&lvHTI.pt);
         m_cValueList.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHTI);
      }
      else
      {
         lvHTI.iItem    = m_nItem;
         lvHTI.flags    = LVHT_ONITEM;
         lvHTI.iSubItem = m_nSubItem;
      }
      if ((lvHTI.iItem != -1) && (lvHTI.flags & LVHT_ONITEM))
      {
         if (lvHTI.iSubItem == 0) return;                      // Subitem 0 ist nur der Index
         if ((m_nFormat >= CF_SINGLE_DOUBLE) &&                // im Singleformat
             (lvHTI.iSubItem == 1)) return;                    // kann der X-Wert nicht editiert werden

         CEdit* pEdit = m_cValueList.GetEditControl();
         if (pEdit)
         {
            if (::GetWindowLong(m_cValueList.m_hWnd, GWL_USERDATA)==0)
            {
               m_nItem    = lvHTI.iItem;
               m_nSubItem = lvHTI.iSubItem;
               gm_lOldEditCtrlWndProc = ::SetWindowLong(pEdit->m_hWnd, GWL_WNDPROC, (long)EditCtrlWndProc);
               ::SetWindowLong(m_cValueList.m_hWnd, GWL_USERDATA, (long)this);
               CString str = m_cValueList.GetItemText(m_nItem, m_nSubItem);
               pEdit->SetWindowText(str);
            	*pResult = 0;
            }
         }
      }
   }
}

LRESULT CALLBACK CCurveValuePropPage::EditCtrlWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (gm_lOldEditCtrlWndProc == 0) return 0;

   if (uMsg == WM_WINDOWPOSCHANGING)
   {
      WINDOWPOS *pwp = (WINDOWPOS*)lParam;
      HWND hwndP = ::GetParent(hwnd);
      CRect rc;
      CCurveValuePropPage *pDlg = (CCurveValuePropPage*) ::GetWindowLong(hwndP, GWL_USERDATA);
      if (pDlg)
      {
         ListView_GetSubItemRect(hwndP, pDlg->m_nItem, pDlg->m_nSubItem, LVIR_BOUNDS, (RECT*)&rc);
         pwp->x = rc.left;
         pwp->y = rc.top;
         pwp->cx= rc.Width();
         pwp->cy= rc.Height();
      }
   }  
   return CallWindowProc((WNDPROC)gm_lOldEditCtrlWndProc, hwnd, uMsg, wParam, lParam);
}

void CCurveValuePropPage::OnEndlabeleditCurveValueList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;
   CEdit* pEdit = m_cValueList.GetEditControl();
   if (pEdit)
   {
      ::SetWindowLong(pEdit->m_hWnd, GWL_WNDPROC, gm_lOldEditCtrlWndProc);
      gm_lOldEditCtrlWndProc = 0;
      if ((m_pCurve != NULL) && (pDispInfo->item.mask & LVIF_TEXT) && (pDispInfo->item.pszText != NULL))
      {
         double dVal = _ttof(pDispInfo->item.pszText);
         if ((dVal == 0.0) && (pDispInfo->item.pszText[0] != '0'))
            *pResult = 1;
         if      (m_nSubItem == 1) m_pCurve->SetX_Value(m_nItem, dVal);
         else if (m_nSubItem == 2) m_pCurve->SetY_Value(m_nItem, dVal);
         m_cValueList.Update(m_nItem);
      }
   }
   m_nSubItem = -1;

   ::SetWindowLong(m_cValueList.m_hWnd, GWL_USERDATA, (long)0);
}

void CCurveValuePropPage::OnRclickCurveValueList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_pCurve && m_bEditable)
   {
      UINT Flags = 0;
      POINT ptCursor, ptScreen;
      CMenu Menu, * pContext = NULL;
      LV_ITEM lvItem = { LVIF_STATE|LVIF_PARAM|LVIF_IMAGE, -1, 0, 0, LVIS_SELECTED, NULL, 0, 0, NULL};

      GetCursorPos(&ptCursor);
      ptScreen = ptCursor;
      m_cValueList.ScreenToClient(&ptCursor);
      ptCursor.x = 2;
      Menu.LoadMenu(IDR_EDIT_CURVE_VALUE_MENU);
      pContext = Menu.GetSubMenu(0);
      if (pContext)
      {
         lvItem.iItem = m_cValueList.HitTest(ptCursor, &Flags);

         if ((lvItem.iItem != -1) && (Flags & LVHT_ONITEMLABEL))
         {
            m_cValueList.GetItem(&lvItem);
            if ((lvItem.state & LVIS_SELECTED)==0)
            {
               lvItem.iItem = -1;
            }
         }
         if (lvItem.iItem != -1)
         {
            m_nItem = lvItem.iItem;
            if (m_nFormat >= CF_SINGLE_DOUBLE)                // im Singleformat
            {
               pContext->EnableMenuItem(IDM_EDIT_CURVE_VALUEX, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
               if (lvItem.iItem < m_pCurve->GetNumValues()-1)
               {
                  pContext->EnableMenuItem(IDM_DELETE_CURVE_VALUE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
                  pContext->EnableMenuItem(IDM_INSERT_CURVE_VALUE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
               }
            }
            switch (pContext->TrackPopupMenu(TPM_RETURNCMD, ptScreen.x, ptScreen.y, this))
            {
               case IDM_EDIT_CURVE_VALUEX:
                  m_nSubItem = 1;
                  m_cValueList.EditLabel(lvItem.iItem);
                  break;
               case IDM_EDIT_CURVE_VALUEY:
                  m_nSubItem = 2;
                  m_cValueList.EditLabel(lvItem.iItem);
                  break;
               case IDM_DELETE_CURVE_VALUE:
               {
                  int i, nValues = m_pCurve->GetNumValues();
                  if (nValues > 2)
                  {
                     if (m_pCurve->GetFormat() <= CF_SINGLE_DOUBLE)
                     {
                        for (i=lvItem.iItem+1; i<nValues; i++)
                           m_pCurve->SetCurveValue(i-1, m_pCurve->GetCurveValue(i));
                     }
                     else
                     {
                        for (i=lvItem.iItem+1; i<nValues; i++)
                           m_pCurve->SetY_Value(i-1, m_pCurve->GetY_Value(i));
                     }
                     m_pCurve->SetSize(--nValues);
                     m_cValueList.SetItemCount(nValues);
                  }
               }break;
               case IDM_INSERT_CURVE_VALUE:
               {
                  int i, nValues = m_pCurve->GetNumValues()+1;
                  if (m_pCurve->SetSize(nValues))
                  {
                     m_cValueList.SetItemCount(nValues);
                     if (m_pCurve->GetFormat() <= CF_SINGLE_DOUBLE)
                     {
                        for (i=nValues-2; i>=lvItem.iItem; i--)
                           m_pCurve->SetCurveValue(i+1, m_pCurve->GetCurveValue(i));
                     }
                     else
                     {
                        for (i=nValues-2; i>=lvItem.iItem; i--)
                           m_pCurve->SetY_Value(i+1, m_pCurve->GetY_Value(i));
                     }
                     m_nSubItem = 2;
                     m_nItem    = lvItem.iItem;
                     m_cValueList.EditLabel(lvItem.iItem);
                  }
               } break;
               break;
            }
         }
      }
      Menu.DestroyMenu();
   }
	*pResult = 0;
}

BOOL CCurveValuePropPage::UpdateData(BOOL bSaveAndValidate)
{
   BOOL bResult = CPropertyPage::UpdateData(bSaveAndValidate);
   if (bSaveAndValidate && !bResult)
   {
      m_nNoTabCtrlChange |= 1;
      HWND hwndFocus = ::GetFocus();
      if (!::IsWindowVisible(hwndFocus))
      {
         int i, nCursel, *pnCtrl, nID = ::GetWindowLong(hwndFocus, GWL_ID);

         for (nCursel=0; nCursel<4; nCursel++)
         {
            switch (nCursel)
            {
               case  0: pnCtrl = (int*)gm_nCurveParamCtrl; break;    // Parameter
               case  1: pnCtrl = (int*)gm_nCurveValueCtrl; break;    // Werte
               case  2: pnCtrl = (int*)gm_nCurveValueListCtrl;break; // Werteliste
               case  3: pnCtrl = (int*)gm_nCurveUnitsCtrl; break;    // Einheiten
            }
            for (i=0; pnCtrl[i]!=0; i++)
            {
               if (pnCtrl[i]==nID) break;
            }
            if (pnCtrl[i])
            {
               m_nNoTabCtrlChange = 0;
               OnSelchangingCurveTab(NULL, (LRESULT*)SendDlgItemMessage(IDC_CURVE_TAB, TCM_GETCURSEL, 0, 0)); // nicht sichbar
               OnSelchangeCurveTab(  NULL, (LRESULT*)nCursel); // sichbar
               m_nNoTabCtrlChange = 1;
               SendDlgItemMessage(IDC_CURVE_TAB, TCM_SETCURSEL, nCursel, 0);
               break;
            }
         }
      }
      m_nNoTabCtrlChange = 0;
   }
   return bResult;
}
