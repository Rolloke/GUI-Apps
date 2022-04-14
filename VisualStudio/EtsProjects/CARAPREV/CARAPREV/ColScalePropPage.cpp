// ColScalePropPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ColorLabel.h"
#include "TextLabel.h"
#include "FontPropPage.h"
#include "ColScalePropPage.h"
#include "CaraViewProperties.h"
#include "CaraPreview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CColScalePropPage 

IMPLEMENT_DYNCREATE(CColScalePropPage, CCaraPropertyPage)

CColScalePropPage::CColScalePropPage() : CCaraPropertyPage(CColScalePropPage::IDD)
{
	//{{AFX_DATA_INIT(CColScalePropPage)
	m_nRound       = 0;
	m_bShowText    = FALSE;
	m_nNumMode     = -1;
	m_nStepWidth   = 0;
	//}}AFX_DATA_INIT
   m_bValuesChanged = false;
   ZeroMemory(&m_Font, sizeof(LOGFONT));
   m_TextColor     = 0;
   m_dValue        = 1.2345;
   m_pLabel        = NULL;
   m_pTextLabel    = NULL;
}

CColScalePropPage::~CColScalePropPage()
{
   if (m_pTextLabel) delete m_pTextLabel;
}

void CColScalePropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColScalePropPage)
	DDX_Control(pDX, IDC_PREVIEWSTD, m_Preview);
	DDX_Check(pDX, IDC_CSCALE_SHOWTEXT, m_bShowText);
	DDX_Text(pDX, IDC_CSCALE_STEPWIDTH, m_nStepWidth);
	//}}AFX_DATA_MAP

   CWnd *pParent  = GetParent();
   if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CCaraViewProperties)))
   {
      CView *pView = ((CCaraViewProperties*)pParent)->m_pView;
      if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARAPreview)))
      {
         CDC dc;
         dc.Attach(((CCARAPreview*)pView)->GetAttribDC());
         CSize szMin = m_pLabel->GetMinScaleSize(&dc);
         CSize szMax = m_pLabel->GetRect(&dc).Size();
         dc.Detach();
         if (m_pLabel->IsHorizontal())
         {
         	DDV_MinMaxInt(pDX, m_nStepWidth, abs(szMin.cx), abs(szMax.cx));
         }
         else
         {
         	DDV_MinMaxInt(pDX, m_nStepWidth, abs(szMin.cy), abs(szMax.cy));
         }
      }
   }
}


BEGIN_MESSAGE_MAP(CColScalePropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CColScalePropPage)
	ON_BN_CLICKED(IDC_CSCALE_FONT, OnCscaleFont)
	ON_BN_CLICKED(IDC_CSCALE_SHOWTEXT, OnChangeValues)
	ON_BN_CLICKED(IDC_CSCALE_NUM_REP, OnCscaleNumRep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CColScalePropPage 

BOOL CColScalePropPage::OnApply() 
{
	return CCaraPropertyPage::OnApply();
}

BOOL CColScalePropPage::OnKillActive() 
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
      if      (GetDlgItem(IDC_CSCALE_FONT)==pFocus) OnCscaleFont();
      else if (RequestSaveChanges(false)) return false;

      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
   return false;
}

BOOL CColScalePropPage::OnSetActive() 
{
	if (m_pLabel)
   {
      m_bValuesChanged = false;
      m_Preview.SetBkColor(RGB(255,255,255));
      UpdateData(false);
      Updatepreview();
   }
	return CPropertyPage::OnSetActive();
}

void CColScalePropPage::OnOK() 
{
   if (m_pLabel && UpdateData(true))
   {
      m_pLabel->SetLogFont(m_Font);
      m_pLabel->SetRound(m_nRound);
      m_pLabel->SetNumMode(m_nNumMode);
      m_pLabel->SetTextColor(m_TextColor);
      m_pLabel->SetStepWidth(m_nStepWidth);
      m_pLabel->SetShowScale((m_bShowText!=0) ? true : false);
      SetModified(false);
   }
   CPropertyPage::OnOK();
}

void CColScalePropPage::OnCscaleFont() 
{
   LOGFONT lf = m_Font;
   CCaraViewProperties PropSheet(IDS_PROPERTIES, this);
   CString string;
   string.LoadString(IDS_SCALEFONT);
   CTextLabel TextLabel(&CPoint(0,0), (TCHAR*)LPCTSTR(string));
   TextLabel.SetTextAlign(0);
   TextLabel.SetExtraSpacing(0);
   TextLabel.SetTextColor(m_TextColor);
   TextLabel.SetLogFont(&m_Font);
   TextLabel.SetDrawFrame(false);
   TextLabel.SetDrawBkGnd(false);

   PropSheet.InitDialog(TXT_FONT|TXT_HEIGHT|TXT_WIDTH|TXT_WEIGHT|TXT_ITALIC|TXT_COLOR, &TextLabel);
   PropSheet.SetApplyNowButton(false);
   if (PropSheet.DoModal()==IDOK)
   {
      bool bChanged = false;
      m_Font = TextLabel.GetLogFont();
      if (memcmp(&lf, &m_Font, sizeof(LOGFONT))) bChanged = true;
      COLORREF color = TextLabel.GetTextColor();
      if (m_TextColor != color)
      {
         m_TextColor = color;
         bChanged = true;
      }
      if (bChanged) OnChangeValues();
   }
}

void CColScalePropPage::OnChangeValues() 
{
   Updatepreview();
   SetModified(true);
}

void CColScalePropPage::SetLabel(CLabel *pl)
{
   if (pl)
   {
      ASSERT_VALID(pl);
      m_bValuesChanged = true;
      m_Preview.SetLabel(pl);
      m_Preview.SetLogFont(&m_Font, &m_TextColor, NULL, NULL);

      if (pl->IsKindOf(RUNTIME_CLASS(CColorLabel)))
      {
         m_pLabel     = (CColorLabel*)pl;
         m_Font       = m_pLabel->GetLogFont();
         m_nRound     = m_pLabel->GetRound();
         m_nNumMode   = m_pLabel->GetNumMode();
         m_TextColor  = m_pLabel->GetTextColor();
         m_nStepWidth = m_pLabel->GetStepWidth();
         m_bShowText  = m_pLabel->GetShowScale();
         m_dValue     = (m_pLabel->GetMinLevel() + m_pLabel->GetMaxLevel()) * 0.5;
      }
      else m_pLabel   = NULL;
   }
   else m_pLabel = NULL;
}

void CColScalePropPage::Updatepreview()
{
   if (UpdateData(true))
   {
      TCHAR text[256];
      ETSDIV_NUTOTXT ntx;
      ntx.fvalue = m_dValue;
      ntx.nmaxl  = 256;
      ntx.nmode  = m_nNumMode;
      ntx.nround = m_nRound;
      ntx.dZero  = 0;

      if (CEtsDiv::NumberToText(&ntx, text))
      {
         if (m_pTextLabel == NULL)
         {
            m_pTextLabel = new CTextLabel;
            m_Preview.SetLabel(m_pTextLabel);
         }
         m_pTextLabel->SetText(text);
         m_Preview.InvalidateRect(NULL);
      }
   }
}

BOOL CColScalePropPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
   ::SetWindowLong(m_hWnd, GWL_ID, CColScalePropPage::IDD);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CColScalePropPage::OnCscaleNumRep() 
{
   ETSDIV_NUTOTXT ntx = {0,m_nNumMode, m_nRound, 0, 0};
   if (CEtsDiv::NumberRepresentationDlg(&ntx, m_hWnd))
   {
      OnChangeValues();
      m_nNumMode = ntx.nmode;
      m_nRound   = ntx.nround;
   }
}
