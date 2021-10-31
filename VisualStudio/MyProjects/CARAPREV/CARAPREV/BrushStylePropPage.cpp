// BrushStylePropPage.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "BrushStylePropPage.h"
#include "RectLabel.h"
#include "TextLabel.h"
#include "CircleLabel.h"
#include "CARAPREV.h"
#include "Dibsection.h"
#include "ETS3DGLRegKeys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGKEY_DEFAULT_BRUSH_BMP "BrushBmp"

/////////////////////////////////////////////////////////////////////////////
// Eigenschaftenseite CBrushStylePropPage 
//extern "C" __declspec(dllexport) void     *g_AfxCommDlgProc;
LOGBRUSH CBrushStylePropPage::gm_LogBrushes[NO_OF_BRUSHES] = 
{
   {BS_HOLLOW , 0, 0            },
   {BS_HATCHED, 0, HS_BDIAGONAL },
   {BS_HATCHED, 0, HS_CROSS     },
   {BS_HATCHED, 0, HS_DIAGCROSS },
   {BS_HATCHED, 0, HS_FDIAGONAL },
   {BS_HATCHED, 0, HS_HORIZONTAL},
   {BS_HATCHED, 0, HS_VERTICAL  },
   {BS_SOLID  , 0, 0            }
};

IMPLEMENT_DYNCREATE(CBrushStylePropPage, CCaraPropertyPage)

CBrushStylePropPage::CBrushStylePropPage() : CCaraPropertyPage(CBrushStylePropPage::IDD)
{
   m_bValuesChanged = false;
   m_pLabel        = NULL;
   m_LogBrush.lbStyle = BS_HOLLOW;
   m_LogBrush.lbColor = 0;
   m_LogBrush.lbHatch = 0;
   m_cBkColor = RGB(255,255,255);
#ifdef _DEBUG
   m_pDib              = NULL;
#endif
   //{{AFX_DATA_INIT(CBrushStylePropPage)
	//}}AFX_DATA_INIT
}

CBrushStylePropPage::~CBrushStylePropPage()
{
#ifdef _DEBUG
   DeleteLogBrushBmp();
#endif
}

#ifdef _DEBUG
void CBrushStylePropPage::DeleteLogBrushBmp()
{
   if (m_pDib)
   {
      delete m_pDib;
      m_pDib = NULL;
   }
}
#endif

void CBrushStylePropPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CBrushStylePropPage)
   DDX_Control(pDX, IDC_BRUSH_BKGNDCOLOR, m_BkColor);
   DDX_Control(pDX, IDC_BRUSH_COLOR, m_BrushColor);
   DDX_Control(pDX, IDC_BRUSH_STYLE, m_BrushStyle);
   DDX_Control(pDX, IDC_PREVIEWSTD, m_Preview);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrushStylePropPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBrushStylePropPage)
   ON_BN_CLICKED(IDC_BRUSH_COLOR, OnBrushcolor)
   ON_LBN_SELCHANGE(IDC_BRUSH_STYLE, OnSelchangeBrushstyle)
   ON_BN_CLICKED(IDC_BRUSH_BKGNDCOLOR, OnBkcolor)
	//}}AFX_MSG_MAP
#ifdef _DEBUG
   ON_BN_CLICKED(IDC_BRUSH_BITMAP, OnBrushbitmap)
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CBrushStylePropPage 

#ifdef _DEBUG
void CBrushStylePropPage::OnBrushbitmap() 
{
   static TCHAR         filterlist[] = "File (*.bmp)\0*.bmp\0\0";

   CFileDialog fd(true);
   fd.m_ofn.nFilterIndex   = 1;
   fd.m_ofn.nFileExtension = 1;
   fd.m_ofn.Flags         |= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
   fd.m_ofn.lpstrFilter    = filterlist;
   fd.m_ofn.nMaxCustFilter = 1;

   if (fd.DoModal()== IDOK)
   {
      DeleteLogBrushBmp();
      m_pDib = new CDibSection;
      CFile file(fd.GetPathName(), CFile::modeRead);
      if (m_pDib->LoadFromFile((HANDLE)file.m_hFile))
      {
         m_LogBrush.lbStyle = BS_PATTERN;
         m_LogBrush.lbHatch = (long) m_pDib->GetBitmapHandle();
         m_LogBrush.lbColor = (long) m_pDib;
         m_BrushStyle.SetCurSel(-1);
         m_Preview.InvalidateRect(NULL);
         SetModified(true);
      }
   }
}
#endif

void CBrushStylePropPage::OnBrushcolor() 
{
   CString string;
   CColorDialog cd(m_LogBrush.lbColor, 0, this);
   cd.m_cc.lpCustColors = g_CustomColors;
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   cd.m_cc.Flags       |=  CC_FULLOPEN;

   string.LoadString(IDS_BRUSHCOLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal()==IDOK)
   {
      COLORREF color = cd.GetColor();
      if (m_LogBrush.lbColor != color) 
      {
         m_LogBrush.lbColor = color;
         SetModified(true);
         m_Preview.InvalidateRect(NULL);
      }
   }
   g_AfxCommDlgProc = NULL;
}

void CBrushStylePropPage::OnSelchangeBrushstyle() 
{
   int index = m_BrushStyle.GetCurSel();
   if (index != LB_ERR)
   {
      LOGBRUSH lb = *((LOGBRUSH*)m_BrushStyle.GetItemData(index));
      
      if ((m_LogBrush.lbStyle != lb.lbStyle) || (m_LogBrush.lbHatch != lb.lbHatch))
      {
         m_LogBrush.lbStyle = lb.lbStyle;
         m_LogBrush.lbHatch = lb.lbHatch; 
         SetModified(true);
         m_bDrawBkGnd = (m_LogBrush.lbStyle == BS_HOLLOW) ? false : true;
         CheckStyle(index);
         m_Preview.InvalidateRect(NULL);
      }
   }
}

BOOL CBrushStylePropPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();
   int i;
   TCHAR szText[16];
   ::SetWindowLong(m_hWnd, GWL_ID, CBrushStylePropPage::IDD);
   for (i=0; i<NO_OF_BRUSHES; i++)
   {
      wsprintf(szText, "%d", i+1);
      m_BrushStyle.AddString(szText);
      m_BrushStyle.SetItemData(i, (DWORD)&gm_LogBrushes[i]);
      m_BrushStyle.SetItemHeight(i, 40);
   }
   m_Preview.SetLogBrush(&m_LogBrush);
   m_Preview.SetpBkColor(&m_cBkColor);
   m_Preview.SetpbDrawBkGnd(&m_bDrawBkGnd);
   m_Preview.SetBkColor(GetPreviewBkGndColor());

   if (AfxGetApp()->GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_BRUSH_BMP, 0))
   {
      GetDlgItem(IDC_BRUSH_BITMAP)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_BRUSH_BITMAP)->EnableWindow(true);
   }
   return TRUE;
   // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CBrushStylePropPage::CheckStyle(UINT nStyle)
{
   if ((nStyle >= NO_OF_BRUSHES) /*|| (nStyle < 0)*/) return;
                                         // 0     , 1           , 2       , 3           , 4           , 5            , 6          , 7
                                         // HOLLOW, HATCHED     , HATCHED , HATCHED     , HATCHED     , HATCHED      , HATCHED    , SOLID
                                         //         HS_BDIAGONAL, HS_CROSS, HS_DIAGCROSS, HS_FDIAGONAL, HS_HORIZONTAL, HS_VERTICAL,
   const static bool bBrush[NO_OF_BRUSHES]={false , true        , true    , true        , true        , true         , true       , true  };  // Color
   const static bool bBkGnd[NO_OF_BRUSHES]={false , true        , true    , true        , true        , true         , true       , false }; // BkColor

   m_BrushColor.EnableWindow(bBrush[nStyle]);
   m_BkColor.EnableWindow(   bBkGnd[nStyle]);
}

BOOL CBrushStylePropPage::OnSetActive() 
{
   if (m_bValuesChanged)
   {
      int i, count;
      count = m_BrushStyle.GetCount();
      for (i=0; i< count; i++)
      {
         if ((m_LogBrush.lbStyle == ((LOGBRUSH*)m_BrushStyle.GetItemDataPtr(i))->lbStyle) &&
             (m_LogBrush.lbHatch == ((LOGBRUSH*)m_BrushStyle.GetItemDataPtr(i))->lbHatch))
         {
            m_BrushStyle.SetCurSel(i);
            break;
         }
      }
      if (i == count) m_BrushStyle.SetCurSel(-1);
      else            CheckStyle(i);

      UpdateData(false);
      m_bValuesChanged = false;
   }
   
   return CPropertyPage::OnSetActive();
}

BOOL CBrushStylePropPage::OnKillActive() 
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
      if      (GetDlgItem(IDC_BRUSH_COLOR     )==pFocus) OnBrushcolor();
      else if (GetDlgItem(IDC_BRUSH_BKGNDCOLOR)==pFocus) OnBkcolor();
#ifdef _DEBUG
      else if (GetDlgItem(IDC_BRUSH_BITMAP    )==pFocus) OnBrushbitmap();
#endif
      else if (RequestSaveChanges(false)) return false;

      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
   return false;
}

void CBrushStylePropPage::OnOK()
{
   if (m_pLabel)
   {
      m_pLabel->SetBkColor(m_cBkColor);
      m_pLabel->SetDrawBkGnd((m_bDrawBkGnd!=0) ? true : false);

      if      (m_pLabel->IsKindOf(RUNTIME_CLASS(CRectLabel)))
         ((CRectLabel*   )m_pLabel)->SetLogBrush(&m_LogBrush);
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CTextLabel)))
         ((CTextLabel*   )m_pLabel)->SetLogBrush(&m_LogBrush);
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CCircleLabel)))
         ((CCircleLabel* )m_pLabel)->SetLogBrush(&m_LogBrush);

#ifdef _DEBUG
      if (m_LogBrush.lbStyle == BS_PATTERN)     // wurde eine Bitmap übergeben
      {
         if (m_LogBrush.lbHatch == 0)           // und daraus ein Hintergrund erzeugt ?
         {                                      // Alten Bitmaphandle an lbHatch zurückgeben
            m_LogBrush.lbHatch = (long) m_pDib->GetBitmapHandle();
            m_LogBrush.lbColor = (long) m_pDib; // Zeiger auf DibSection übergeben
            m_pDib = NULL;                      // und die DibSection nicht löschen !
         }
      }
#endif

      SetModified(false);
   }
   CPropertyPage::OnOK();
}

BOOL CBrushStylePropPage::OnApply()
{
   return CCaraPropertyPage::OnApply();
}

void CBrushStylePropPage::SetLabel(CLabel *pl)
{
   if (pl)
   {
      ASSERT_VALID(pl);
      m_pLabel         = pl;
      m_bValuesChanged = true;
      m_Preview.SetLabel(pl);

      if      (m_pLabel->IsKindOf(RUNTIME_CLASS(CRectLabel)))
         m_LogBrush = ((CRectLabel*  )m_pLabel)->GetLogBrush();
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CTextLabel)))
         m_LogBrush = ((CTextLabel*  )m_pLabel)->GetLogBrush();
      else if (m_pLabel->IsKindOf(RUNTIME_CLASS(CCircleLabel)))
         m_LogBrush = ((CCircleLabel*)m_pLabel)->GetLogBrush();

      m_cBkColor   = m_pLabel->GetBkColor();
      m_bDrawBkGnd = m_pLabel->DoDrawBkGnd();
   }
   else m_pLabel = NULL;
}

void CBrushStylePropPage::OnBkcolor() 
{
   CString string;
   CColorDialog cd(m_cBkColor, 0, this);
   cd.m_cc.lpCustColors = g_CustomColors;
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   cd.m_cc.Flags       |=  CC_FULLOPEN;

   string.LoadString(IDS_BKCOLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal()==IDOK)
   {
      COLORREF color = cd.GetColor();
      if (m_cBkColor != color) 
      {
         m_cBkColor = color;
         m_bDrawBkGnd = (m_LogBrush.lbStyle == BS_HOLLOW) ? false : true;
         SetModified(true);
         m_Preview.InvalidateRect(NULL);
      }
   }
   g_AfxCommDlgProc = NULL;
}

