// FontPropPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "FontPropPage.h"
#include "TextLabel.h"
#include "PlotLabel.h"
#include "CaraViewProperties.h"
#include "CARAPREV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CFontPropPage 

IMPLEMENT_DYNCREATE(CFontPropPage, CCaraPropertyPage)

CFontPropPage::CFontPropPage() : CCaraPropertyPage(CFontPropPage::IDD)
{
   m_psp.dwFlags &= (0xffff ^ PSP_HASHELP);
   m_Color         = 0;
   m_nAlign        = 0;
   m_pLabel        = NULL;
   m_bValuesChanged = false;
   CTextLabel text;
   m_LogFont       = text.GetLogFont();
   m_nMode         = 0xffff;

	//{{AFX_DATA_INIT(CFontPropPage)
   m_AlignH     = -1;
   m_AlignV     = -1;
   m_bCursiv    = FALSE;
   m_bUnderLine = FALSE;
   m_bLine      = FALSE;
   m_nExtra     = 0;
   m_nHeight    = 0;
   m_nWeight    = 0;
   m_nWidth     = 0;
   m_bBkGround  = FALSE;
   m_nZoom      = 1;
	//}}AFX_DATA_INIT
}

CFontPropPage::~CFontPropPage()
{
}

void CFontPropPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CFontPropPage)
   DDX_Control(pDX, IDC_FONT_FONT, m_cFont);
   DDX_Control(pDX, IDC_PREVIEWSTD, m_Preview);
   DDX_CBIndex(pDX, IDC_FONT_ALIGN_H, m_AlignH);
   DDX_CBIndex(pDX, IDC_FONT_ALIGN_V, m_AlignV);
   DDX_Check(pDX, IDC_FONT_CURSIV, m_bCursiv);
   DDX_Check(pDX, IDC_FONT_UNDERLINE, m_bUnderLine);
   DDX_Check(pDX, IDC_FONT_LINE, m_bLine);
   DDX_Text(pDX, IDC_FONT_EXTRASPACE, m_nExtra);
   DDX_Text(pDX, IDC_FONT_HEIGHT, m_nHeight);
   DDX_CBIndex(pDX, IDC_FONT_WEIGHT, m_nWeight);
   DDX_Text(pDX, IDC_FONT_WIDTH, m_nWidth);
   DDX_Check(pDX, IDC_FONT_BKGND, m_bBkGround);
   DDX_Radio(pDX, IDC_FONT_ZOOM0, m_nZoom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFontPropPage, CPropertyPage)
   //{{AFX_MSG_MAP(CFontPropPage)
   ON_CBN_SELCHANGE(IDC_FONT_FONT, OnSelchangeFont)
   ON_BN_CLICKED(IDC_FONT_TEXTCOLOR, OnTextColor)
   ON_BN_CLICKED(IDC_FONT_BKGNDCOLOR, OnBkgColor)
   ON_CBN_SELCHANGE(IDC_FONT_ALIGN_H, OnSelchangeAlign)
   ON_BN_CLICKED(IDC_FONT_BKGND, OnBkground)
   ON_BN_CLICKED(IDC_FONT_CURSIV, OnCursiv)
   ON_BN_CLICKED(IDC_FONT_LINE, OnLine)
   ON_BN_CLICKED(IDC_FONT_UNDERLINE, OnUnderline)
   ON_EN_KILLFOCUS(IDC_FONT_WIDTH, OnKillfocusWidth)
   ON_EN_KILLFOCUS(IDC_FONT_HEIGHT, OnKillfocusHeight)
   ON_EN_KILLFOCUS(IDC_FONT_EXTRASPACE, OnKillfocusExtra)
   ON_CBN_SELCHANGE(IDC_FONT_WEIGHT, OnSelchangeWeight)
   ON_BN_CLICKED(IDC_FONT_ZOOM0, OnZoom)
   ON_CBN_SELCHANGE(IDC_FONT_ALIGN_V, OnSelchangeAlign)
   ON_BN_CLICKED(IDC_FONT_ZOOM1, OnZoom)
   ON_BN_CLICKED(IDC_FONT_ZOOM2, OnZoom)
   ON_BN_CLICKED(IDC_FONT_ZOOM3, OnZoom)
   ON_BN_CLICKED(IDC_FONT_ZOOM4, OnZoom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFontPropPage 
void CFontPropPage::SetAlign(UINT nAlign)
{
   if      ((nAlign & TA_BASELINE) == TA_BASELINE) m_AlignV = 1;
   else if ((nAlign & TA_BOTTOM)   == TA_BOTTOM)   m_AlignV = 2;
   else                                            m_AlignV = 0;

   if      ((nAlign & TA_CENTER)   == TA_CENTER)   m_AlignH = 1;
   else if ((nAlign & TA_RIGHT)    == TA_RIGHT)    m_AlignH = 2;
   else                                            m_AlignH = 0;
   m_nAlign = nAlign;
}

UINT CFontPropPage::GetAlign()
{
   UINT align;
   switch (m_AlignH)
   {
      case 0: align = TA_LEFT;   break;
      case 1: align = TA_CENTER; break;
      case 2: align = TA_RIGHT;  break;
   }

   switch (m_AlignV)
   {
      case 0: align |= TA_TOP;      break;
      case 1: align |= TA_BASELINE; break;
      case 2: align |= TA_BOTTOM;   break;
   }
   return align;
}

BOOL CFontPropPage::OnSetActive()
{
   if (m_bValuesChanged)
   {
      UpdateData(false);
      m_bValuesChanged = false;
      m_Preview.SetBkColor(RGB(255,255,255));
   }
   return CPropertyPage::OnSetActive();
}

BOOL CFontPropPage::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   ::SetWindowLong(m_hWnd, GWL_ID, CFontPropPage::IDD);
   LOGFONT lf;
   memset(&lf, 0, sizeof(LOGFONT));
//   lf.lfCharSet = DEFAULT_CHARSET;
   lf.lfCharSet = ANSI_CHARSET;
   HDC hdc = ::GetDC(NULL);
   ::EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC) EnumFontFamProc, (LPARAM) this, 0);
   ::ReleaseDC(NULL, hdc);

   int nCur = m_cFont.FindString(0, m_strFullName);
   if (nCur!=CB_ERR) m_cFont.SetCurSel(nCur);

   GetDlgItem(IDC_FONT_BKGNDCOLOR)->EnableWindow(m_bBkGround);

   m_Preview.SetLogFont(&m_LogFont, &m_Color, &m_nAlign, &m_nExtra);
   m_Preview.SetpBkColor(&m_BKColor);
   m_Preview.SetZoom(&m_nZoomFactor);
   m_Preview.SetpbDrawBkGnd(&m_bBkGround);
   OnZoom();

   GetDlgItem(IDC_FONT_FONT      )->EnableWindow((m_nMode&TXT_FONT)!=0);
   GetDlgItem(IDC_FONT_HEIGHT    )->EnableWindow((m_nMode&TXT_HEIGHT)!=0);
   GetDlgItem(IDC_FONT_WIDTH     )->EnableWindow((m_nMode&TXT_WIDTH)!=0);
   GetDlgItem(IDC_FONT_ALIGN_H   )->EnableWindow((m_nMode&TXT_ALIGN)!=0);
   GetDlgItem(IDC_FONT_ALIGN_V   )->EnableWindow((m_nMode&TXT_ALIGN)!=0);
   GetDlgItem(IDC_FONT_EXTRASPACE)->EnableWindow((m_nMode&TXT_EXTRA)!=0);
   GetDlgItem(IDC_FONT_WEIGHT    )->EnableWindow((m_nMode&TXT_WEIGHT)!=0);
   GetDlgItem(IDC_FONT_CURSIV    )->EnableWindow((m_nMode&TXT_ITALIC)!=0);
   GetDlgItem(IDC_FONT_UNDERLINE )->EnableWindow((m_nMode&TXT_UNDERLINE)!=0);
   GetDlgItem(IDC_FONT_LINE      )->EnableWindow((m_nMode&TXT_STRIKEOUT)!=0);
   GetDlgItem(IDC_FONT_TEXTCOLOR )->EnableWindow((m_nMode&TXT_COLOR)!=0);
   GetDlgItem(IDC_FONT_BKGND     )->EnableWindow((m_nMode&TXT_BKCOLOR)!=0);
   GetDlgItem(IDC_FONT_BKGNDCOLOR)->EnableWindow(((m_nMode&TXT_BKCOLOR)!=0) && m_bBkGround);

   return TRUE;
   // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL CFontPropPage::OnKillActive() 
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
      if (RequestSaveChanges(false)) return false;

      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
   return false;
}

void CFontPropPage::OnOK() 
{
   if (m_pLabel)
   {
      m_pLabel->SetBkColor(m_BKColor);
      m_pLabel->SetDrawBkGnd((m_bBkGround!=0) ? true : false);
//      m_pLabel->SetLogBrush(NULL);
      m_pLabel->SetLogFont(&m_LogFont);
      m_pLabel->SetTextColor(m_Color);
      m_pLabel->SetTextAlign(m_nAlign);
      m_pLabel->SetExtraSpacing(m_nExtra);
      SetModified(false);
   }
   CPropertyPage::OnOK();
}

BOOL CFontPropPage::OnApply() 
{
   return CCaraPropertyPage::OnApply();
}
void CFontPropPage::OnZoom() 
{
   if (UpdateData(true))
   {
      switch(m_nZoom)
      {
         case 0: m_nZoomFactor =  50; break;
         case 1: m_nZoomFactor = 100; break;
         case 2: m_nZoomFactor = 200; break;
         case 3: m_nZoomFactor = 500; break;
         case 4: m_nZoomFactor =1000; break;
      }
      CWnd *pParent  = GetParent();
      if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CCaraViewProperties)))
      {
         CView *pView = ((CCaraViewProperties*)pParent)->m_pView;
         if (pView)
         {
            CDC *pDC = pView->GetDC();
            if (pDC)
            {
               pView->OnPrepareDC(pDC);
               CSize sz(m_nZoomFactor, m_nZoomFactor);
               pDC->DPtoLP(&sz);
               m_nZoomFactor = sz.cx;
               pView->ReleaseDC(pDC);
            }
         }
      }
      m_Preview.InvalidateRect(NULL);
   }
}
void CFontPropPage::SetLabel(CLabel *pl)
{
   if (pl && pl->IsKindOf(RUNTIME_CLASS(CTextLabel)))
   {
      ASSERT_VALID(pl);
      m_pLabel        = (CTextLabel*)pl;
      m_bValuesChanged = true;
      m_Preview.SetLabel(pl);
      m_LogFont = m_pLabel->GetLogFont();

      m_bCursiv      =  m_LogFont.lfItalic;
      m_bUnderLine   =  m_LogFont.lfUnderline;
      m_bLine        =  m_LogFont.lfStrikeOut;
      m_nHeight      = -m_LogFont.lfHeight;
      m_nWidth       =  m_LogFont.lfWidth;
      m_BKColor      =  m_pLabel->GetBkColor();
      m_bBkGround    =  m_pLabel->DoDrawBkGnd();

      if      (m_LogFont.lfWeight < 300) m_nWeight = 0;
      else if (m_LogFont.lfWeight > 500) m_nWeight = 2;
      else                               m_nWeight = 1;

      m_Color   = m_pLabel->GetTextColor();
      m_nAlign  = m_pLabel->GetTextAlign();
      SetAlign(m_nAlign);
      m_nExtra  = m_pLabel->GetExtraSpacing();
      m_bBkGround = m_pLabel->DoDrawBkGnd();
   }
   else m_pLabel = NULL;
}

void CFontPropPage::OnSelchangeFont() 
{
   int nWeight = m_LogFont.lfWeight;
   ENUMLOGFONT elf;
   memset(&elf, 0, sizeof(ENUMLOGFONT));
   LOGFONT lf;
   memset(&lf, 0, sizeof(LOGFONT));
   lf.lfCharSet = DEFAULT_CHARSET;

   m_cFont.GetWindowText((TCHAR*)elf.elfFullName, LF_FULLFACESIZE);
   lstrcpyn((TCHAR*)elf.elfLogFont.lfFaceName , (const TCHAR*)elf.elfFullName, LF_FACESIZE);
   HDC hdc = ::GetDC(NULL);
//   ::EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC) FindFontFamProc, (LPARAM) &elf, DEFAULT_CHARSET);
   ::EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC) FindFontFamProc, (LPARAM) &elf, 0);
   memcpy(&m_LogFont, &elf.elfLogFont, sizeof(LOGFONT));

   m_LogFont.lfHeight    = -m_nHeight;
   m_LogFont.lfWidth     =  0;
   m_LogFont.lfItalic    =  m_bCursiv;
   m_LogFont.lfStrikeOut =  m_bLine;
   m_LogFont.lfUnderline =  m_bUnderLine;
   m_LogFont.lfWeight    =  nWeight;

   HFONT hfont    = CreateFontIndirect(&m_LogFont);
   HFONT hOldfont = (HFONT)::SelectObject(hdc, hfont);

   TEXTMETRIC Metrics;
   ::GetTextMetrics(hdc, &Metrics);

   m_LogFont.lfWidth = m_nWidth = Metrics.tmAveCharWidth;

   if (hOldfont) ::SelectObject(hdc, hOldfont);
   ::DeleteObject(hfont);
   ::ReleaseDC(NULL, hdc);

   UpdateData(false);
    
   SetModified(true);
   m_Preview.InvalidateRect(NULL);
}

void CFontPropPage::OnSelchangeAlign()
{
   if (UpdateData(true))
   {
      m_nAlign = GetAlign();
      SetModified(true);
      m_Preview.InvalidateRect(NULL);
   }
}
void CFontPropPage::OnSelchangeWeight() 
{
   if (UpdateData(true))
   {
      switch (m_nWeight)
      {
         case 0: m_LogFont.lfWeight = 100; break;
         case 1: m_LogFont.lfWeight = 400; break;
         case 2: m_LogFont.lfWeight = 700; break;
      }
      SetModified(true);
      m_Preview.InvalidateRect(NULL);
   }	
}

void CFontPropPage::OnTextColor() 
{
   CString string;
   CColorDialog cd(m_Color, 0, this);
   cd.m_cc.lpCustColors = CPlotLabel::gm_StdPltSets.sm_CurveColors;
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   cd.m_cc.Flags       |=  CC_FULLOPEN;

   string.LoadString(IDS_TEXTCOLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal()==IDOK)
   {
      COLORREF color = cd.GetColor();
      if (m_Color != color) 
      {
         m_Color = color;
         SetModified(true);
         m_Preview.InvalidateRect(NULL);
      }
   }
   g_AfxCommDlgProc = NULL;
}
void CFontPropPage::OnBkgColor() 
{
   CString string;
   CColorDialog cd(m_BKColor, 0, this);
   cd.m_cc.lpCustColors = g_CustomColors;
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   cd.m_cc.Flags       |=  CC_FULLOPEN;

   string.LoadString(IDS_BKCOLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal()==IDOK)
   {
      COLORREF color = cd.GetColor();
      if (m_BKColor != color) 
      {
         m_BKColor = color;
         SetModified(true);
         m_Preview.InvalidateRect(NULL);
      }
   }
   g_AfxCommDlgProc = NULL;
}
void CFontPropPage::OnBkground() 
{
   if (UpdateData(true))
   {
      CWnd *pWnd = GetDlgItem(IDC_FONT_BKGNDCOLOR);
      if (pWnd) pWnd->EnableWindow(m_bBkGround);
      SetModified(true);
      m_Preview.InvalidateRect(NULL);
   }
}
void CFontPropPage::OnCursiv() 
{
   if (UpdateData(true))
   {
      m_LogFont.lfItalic = m_bCursiv;
      SetModified(true);
      m_Preview.InvalidateRect(NULL);
   }
}
void CFontPropPage::OnLine() 
{
   if (UpdateData(true))
   {
      m_LogFont.lfStrikeOut = m_bLine;
      SetModified(true);
      m_Preview.InvalidateRect(NULL);
   }
}
void CFontPropPage::OnUnderline() 
{
   if (UpdateData(true))
   {
      m_LogFont.lfUnderline = m_bUnderLine;
      SetModified(true);
      m_Preview.InvalidateRect(NULL);
   }
}
void CFontPropPage::OnKillfocusWidth()
{
   if (UpdateData(true))
   {
      if (m_LogFont.lfWidth == m_nWidth) return; // nichts geändert
      m_LogFont.lfWidth = m_nWidth;
      SetModified(true);
      m_Preview.InvalidateRect(NULL);
   }
}
void CFontPropPage::OnKillfocusHeight() 
{
   if (UpdateData(true))
   {
      if (m_LogFont.lfHeight == -m_nHeight) return; // nichts geändert

      m_LogFont.lfHeight = -m_nHeight;
      m_LogFont.lfWidth  = 0;

      HDC hdc = ::GetDC(NULL);
      HFONT hfont    = CreateFontIndirect(&m_LogFont);
      HFONT hOldfont = (HFONT)::SelectObject(hdc, hfont);

      TEXTMETRIC Metrics;
      ::GetTextMetrics(hdc, &Metrics);
      m_LogFont.lfWidth = m_nWidth = Metrics.tmAveCharWidth;

      if (hOldfont) ::SelectObject(hdc, hOldfont);
      ::ReleaseDC(NULL, hdc);

      UpdateData(false);

      SetModified(true);
      m_Preview.InvalidateRect(NULL);
   }
}
void CFontPropPage::OnKillfocusExtra() 
{
   if (UpdateData(true))
   {
      SetModified(true);
      m_Preview.InvalidateRect(NULL);
   }
}

int CALLBACK CFontPropPage::EnumFontFamProc(ENUMLOGFONT * pelf, NEWTEXTMETRIC * pntm, int nFontType, LPARAM lParam)
{
   ASSERT(lParam!=NULL);
   CFontPropPage *pFPP = (CFontPropPage*)lParam;
   if ((nFontType & (TRUETYPE_FONTTYPE)) && (pelf->elfLogFont.lfCharSet == ANSI_CHARSET))
   {
      pFPP->m_cFont.AddString((LPCTSTR) pelf->elfFullName);
      if (lstrcmp((const TCHAR*) pelf->elfLogFont.lfFaceName, (const TCHAR*)pFPP->m_LogFont.lfFaceName)==0)
      {
         pFPP->m_strFullName = _T(pelf->elfFullName);
      }
   }
   return true;
}

int CALLBACK CFontPropPage::FindFontFamProc(ENUMLOGFONT * pelf, NEWTEXTMETRIC * pntm, int nFontType, LPARAM lParam)
{
   if ((nFontType & (TRUETYPE_FONTTYPE)) && (pelf->elfLogFont.lfCharSet == ANSI_CHARSET))
   {
      ENUMLOGFONT *pelfL = (ENUMLOGFONT*) lParam;
      if (l_tcsclen(pelfL->elfLogFont.lfFaceName)!=0)
      {
         if (lstrcmp((const TCHAR*) pelf->elfFullName, (const TCHAR*)pelfL->elfFullName)==0)
         {
            CopyMemory((void*)lParam, pelf, sizeof(ENUMLOGFONT));
            return false;                                      // Ende der Suche
         }
         else if (lstrcmp((const TCHAR*) pelf->elfLogFont.lfFaceName, (const TCHAR*)pelfL->elfLogFont.lfFaceName)==0)
         {
            CopyMemory((void*)lParam, pelf, sizeof(ENUMLOGFONT));
            return false;                                      // Ende der Suche
         }
      }
   }
   return true;
}

void CFontPropPage::SetMode(UINT nMode)
{
   if (nMode == 0) m_nMode = 0xffff;
   else            m_nMode = nMode;
}
