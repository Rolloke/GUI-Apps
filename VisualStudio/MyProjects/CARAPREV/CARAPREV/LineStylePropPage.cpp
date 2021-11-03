// LineStylePropPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "LineStylePropPage.h"
#include "LineLabel.h"
#include "RectLabel.h"
#include "TextLabel.h"
#include "BezierLabel.h"
#include "CircleLabel.h"
#include "PlotLabel.h"
#include "CARAPREV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CLineStylePropPage 

IMPLEMENT_DYNCREATE(CLineStylePropPage, CCaraPropertyPage)

CLineStylePropPage::CLineStylePropPage() : CCaraPropertyPage(CLineStylePropPage::IDD)
{
   m_pLabel             = NULL;
   m_bValuesChanged      = false;
   m_LogPen.lopnStyle   = PS_SOLID;
   m_LogPen.lopnWidth.x = 0;
   m_LogPen.lopnWidth.y = 0;
   m_LogPen.lopnColor   = 0;

   //{{AFX_DATA_INIT(CLineStylePropPage)
   m_nLineWidth = 0;
   //}}AFX_DATA_INIT
}

CLineStylePropPage::~CLineStylePropPage()
{
}

void CLineStylePropPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CLineStylePropPage)
   DDX_Control(pDX, IDC_FRAME_LINESTYLE, m_LineStyle);
   DDX_Control(pDX, IDC_PREVIEWSTD, m_Preview);
   DDX_Text(pDX, IDC_FRAME_LINEWIDTH, m_nLineWidth);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLineStylePropPage, CPropertyPage)
   //{{AFX_MSG_MAP(CLineStylePropPage)
   ON_BN_CLICKED(IDC_FRAME_LINECOLOR, OnLinecolor)
   ON_LBN_SELCHANGE(IDC_FRAME_LINESTYLE, OnSelchangeLinestyle)
	ON_EN_CHANGE(IDC_FRAME_LINEWIDTH, OnChangeFrameLinewidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CLineStylePropPage 
BOOL CLineStylePropPage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   ::SetWindowLong(m_hWnd, GWL_ID, CLineStylePropPage::IDD);

   int i, count;
   char szText[16];
   DWORD penstyle[] = {PS_NULL, PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT, PS_DASHDOTDOT};
   count = sizeof(penstyle) / sizeof(DWORD);
   for (i=0; i<count; i++)
   {
      wsprintf(szText, "%d", i+1);
      m_LineStyle.AddString(szText);
      m_LineStyle.SetItemData(i, penstyle[i]);
   }

   m_Preview.SetLogPen(&m_LogPen);
   CWnd *pParent  = GetParent();
   m_Preview.SetBkColor(GetPreviewBkGndColor());
   GetDlgItem(IDC_FRAME_LINEWIDTH_SPIN)->SendMessage(UDM_SETRANGE32, 0, 100);
   return true;
   // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CLineStylePropPage::OnLinecolor() 
{
   CString string;
   CColorDialog cd(m_LogPen.lopnColor, 0, this);
   cd.m_cc.lpCustColors = CPlotLabel::gm_StdPltSets.sm_CurveColors;
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   cd.m_cc.Flags       |=  CC_FULLOPEN;

   string.LoadString(IDS_LINECOLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal()==IDOK)
   {
      COLORREF color = cd.GetColor();
      if (m_LogPen.lopnColor != color)
      {
         m_LogPen.lopnColor = color;
         SetModified(true);
         m_Preview.InvalidateRect(NULL);
      }
   }
   g_AfxCommDlgProc = NULL;
}

void CLineStylePropPage::OnSelchangeLinestyle() 
{
   int index = m_LineStyle.GetCurSel();
   if (index != LB_ERR)
   {
      m_LogPen.lopnStyle = m_LineStyle.GetItemData(index);
      SetModified(true);
      m_Preview.InvalidateRect(NULL);
   }
}
 
BOOL CLineStylePropPage::OnSetActive() 
{
   if (m_bValuesChanged)
   {
      int i, count;
      count = m_LineStyle.GetCount();
      for (i=0; i< count; i++)
      {
         if (m_LogPen.lopnStyle == (UINT) m_LineStyle.GetItemData(i))
         {
            m_LineStyle.SetCurSel(i);
            break;
         }
      }
      if (i == count) m_LineStyle.SetCurSel(-1);

      m_nLineWidth = m_LogPen.lopnWidth.x;
      if (m_nLineWidth>1)
      {
         SIZE sz = {m_nLineWidth, m_nLineWidth};
         CDC *pDC = GetDC();
		   pDC->SetMapMode(MM_LOMETRIC);
         pDC->LPtoDP(&sz);
         ReleaseDC(pDC);
         m_nLineWidth = sz.cx;
      }

      if (m_nLineWidth > 1)
      {
         m_LineStyle.ShowWindow(SW_HIDE);
         m_LineStyle.EnableWindow(false);
      }
      else
      {
         m_LineStyle.ShowWindow(SW_SHOW);
         m_LineStyle.EnableWindow(true);
      }

      m_bValuesChanged = false;
      UpdateData(false);
   }
   return CPropertyPage::OnSetActive();
}
 
BOOL CLineStylePropPage::OnKillActive() 
{
   int nResult = CCaraPropertyPage::OnKillActive();
   if (nResult & CHANGE_PAGE)
   {
      if (nResult & CHANGES_REJECTED)
         SetLabel(m_pLabel);
      return true;
   }
   else
   {
      CWnd *pFocus  = GetFocus();
      if      (GetDlgItem(IDC_FRAME_LINECOLOR)==pFocus) OnLinecolor();
      else if (RequestSaveChanges(false)) return false;

      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
   return false;
}

void CLineStylePropPage::OnOK() 
{
   if (m_pLabel)
   {
      if      (m_pLabel->IsKindOf(RUNTIME_CLASS(CLineLabel)))
         ((CLineLabel*        )m_pLabel)->SetLogPen(&m_LogPen);
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CRectLabel)))
         ((CRectLabel*        )m_pLabel)->SetLogPen(&m_LogPen);
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CTextLabel)))
         ((CTextLabel*        )m_pLabel)->SetLogPen(&m_LogPen);
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CBezierLabel)))
         ((CBezierLabel*      )m_pLabel)->SetLogPen(&m_LogPen);
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CCircleLabel)))
         ((CCircleLabel*)m_pLabel)->SetLogPen(&m_LogPen);
      SetModified(false);
   }
   CPropertyPage::OnOK();
}

BOOL CLineStylePropPage::OnApply() 
{
   return CCaraPropertyPage::OnApply();
}

void CLineStylePropPage::SetLabel(CLabel *pl)
{
   if (pl)
   {
      ASSERT_VALID(pl);
      m_pLabel        = pl;
      if      (m_pLabel->IsKindOf(RUNTIME_CLASS(CLineLabel)))
         m_LogPen = ((CLineLabel*  )m_pLabel)->GetLogPen();
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CRectLabel)))
         m_LogPen = ((CRectLabel*  )m_pLabel)->GetLogPen();
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CTextLabel)))
         m_LogPen = ((CTextLabel*  )m_pLabel)->GetLogPen();
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CBezierLabel)))
         m_LogPen = ((CBezierLabel*)m_pLabel)->GetLogPen();
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CCircleLabel)))
         m_LogPen = ((CCircleLabel*)m_pLabel)->GetLogPen();
      m_Preview.SetLabel(pl);
      m_bValuesChanged = true;
   }
   else m_pLabel = NULL;
}

void CLineStylePropPage::OnChangeFrameLinewidth() 
{
   if (m_hWnd && m_LineStyle.m_hWnd)
   {
      CDataExchange dx(this, true);
      DDX_Text(&dx, IDC_FRAME_LINEWIDTH, m_nLineWidth);
      int nLineWidth = m_nLineWidth;
      if (m_nLineWidth > 1)
      {
         SIZE sz = {m_nLineWidth, m_nLineWidth};
         CDC *pDC = GetDC();
		   pDC->SetMapMode(MM_LOMETRIC);
         pDC->DPtoLP(&sz);
         ReleaseDC(pDC);
         nLineWidth = sz.cx;
      }

      if (m_LogPen.lopnWidth.x != nLineWidth)
      {
         m_LogPen.lopnWidth.x = nLineWidth;
         m_LogPen.lopnWidth.y = nLineWidth;
         SetModified(true);
         m_Preview.InvalidateRect(NULL);
      }

      if (m_nLineWidth > 1)
      {
         m_LineStyle.ShowWindow(SW_HIDE);
         m_LineStyle.EnableWindow(false);
      }
      else
      {
         m_LineStyle.ShowWindow(SW_SHOW);
         m_LineStyle.EnableWindow(true);
      }
   }
}
