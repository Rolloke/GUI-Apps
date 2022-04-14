// StandardDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA3DV.h"
#include "StandardDlg.h"
#include "..\CaraViewProperties.h"
#include "..\FontPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" __declspec(dllimport) BOOL DDX_TextVar(CDataExchange*, int, int, double&, bool bThrow=true);

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CStandardDlg 

CStandardDlg::CStandardDlg(CWnd* pParent /*=NULL*/)
	: CNonModalDlg(CStandardDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CStandardDlg)
   m_dGridStepX       = 1.0;
   m_dGridStepLevel   = 1.0;
   m_dGridStepZ       = 1.0;
   m_nNumMode         = 0;
   m_nRounding        = 0;
   m_nNumModeZ        = 0;
   m_nRoundingZ       = 0;
   m_bEffect          = FALSE;
	m_bIntelliGrid     = FALSE;
	//}}AFX_DATA_INIT
   memset(&m_GridFont, 0, sizeof(LOGFONT));
}

void CStandardDlg::DoDataExchange(CDataExchange* pDX)
{
   double dGridStepX       = m_dGridStepX;
   double dGridStepLevel   = m_dGridStepLevel;
   double dGridStepZ       = m_dGridStepZ;
   int    nRounding        = m_nRounding;
   int    nRoundingZ       = m_nRoundingZ;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStandardDlg)
	DDX_Check(pDX, IDC_STD_REDUCE, m_bEffect);
	DDX_Check(pDX, IDC_STD_INTELLI_GRID, m_bIntelliGrid);
	//}}AFX_DATA_MAP

	DDX_TextVar(pDX, IDC_STD_GRID_STEP_X, FLT_DIG+1, dGridStepX);
	DDV_MinMaxDouble(pDX, dGridStepX, 0.0001, 10000.);

	DDX_TextVar(pDX, IDC_STD_GRID_STEP_Y, FLT_DIG+1, dGridStepLevel);
	DDV_MinMaxDouble(pDX, dGridStepLevel, 1.0e-6, 1.0e6);

	DDX_TextVar(pDX, IDC_STD_GRID_STEP_Z, FLT_DIG+1, dGridStepZ);
	DDV_MinMaxDouble(pDX, dGridStepZ, 0.0001, 10000.);

   m_dGridStepX       = dGridStepX;
   m_dGridStepLevel   = dGridStepLevel;
   m_dGridStepZ       = dGridStepZ;
   m_nRounding        = nRounding;
   m_nRoundingZ       = nRoundingZ;
}


BEGIN_MESSAGE_MAP(CStandardDlg, CDialog)
	//{{AFX_MSG_MAP(CStandardDlg)
	ON_BN_CLICKED(IDC_STD_GRAFIC_FONT, OnGridFont)
	ON_EN_CHANGE(IDC_STD_SZPRINTBMP, OnOutputParam)
	ON_EN_CHANGE(IDC_STD_GRID_STEP_X      , OnChangeGridStep)
	ON_BN_CLICKED(IDC_STD_REDUCE, OnReduce)
	ON_BN_CLICKED(ID_APPLY_NOW, OnApplyNowCmd)
	ON_BN_CLICKED(IDC_STD_INTELLI_GRID, OnStdIntelliGrid)
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_INITMENUPOPUP()
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_EN_CHANGE(IDC_STD_GRID_STEP_Y      , OnChangeGridStep)
	ON_EN_CHANGE(IDC_STD_GRID_STEP_Z      , OnChangeGridStep)
	ON_BN_CLICKED(IDC_STD_DOUBLE_BUFFER   , OnOutputParam)
	ON_BN_CLICKED(IDC_STD_BMP         , OnOutputParam)
	ON_BN_CLICKED(IDC_STD_NUM_MODE0, OnStdNumMode)
	ON_BN_CLICKED(IDC_STD_NUM_MODEZ0, OnStdNumModeZ)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CStandardDlg 
void CStandardDlg::OnOK() 
{
   CWnd * pWnd = GetFocus();
   if (GetDlgItem(IDOK) == pWnd)
   {
      GetParent()->SendMessage(WM_APPLY_TO_VIEW, INVALIDATE_VIEW, NULL);
      if (CStandardDlg::OnApplyNow())
      {	
         CWnd::DestroyWindow();
      }
   }
   else
   {
      if (GetDlgItem(IDC_STD_GRAFIC_FONT) == pWnd) OnGridFont();
      pWnd = GetNextDlgTabItem(pWnd);
      if (pWnd) pWnd->SetFocus();
   }
}


BOOL CStandardDlg::OnInitDialog() 
{
   CNonModalDlg::OnInitDialog();
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CStandardDlg::OnGridFont() 
{
   LOGFONT lf = m_GridFont;
   CCaraViewProperties m_PropSheet(IDS_PROPERTIES, this);
   CWnd *pParent  = GetParent();
   if (pParent && pParent->IsKindOf(RUNTIME_CLASS(CCaraViewProperties)))
   {
      m_PropSheet.m_pView = ((CCaraViewProperties*)pParent)->m_pView;
   }
   CString string;
   string.LoadString(IDS_GRAFIC_FONT);
   CTextLabel TextLabel(&CPoint(0,0), (char*)LPCTSTR(string));
   TextLabel.SetTextAlign(0);
   TextLabel.SetExtraSpacing(0);
   TextLabel.SetTextColor(0);
   TextLabel.SetLogFont(&lf);

   m_PropSheet.InitDialog(TXT_FONT|TXT_HEIGHT|TXT_WIDTH|TXT_WEIGHT|TXT_ITALIC, &TextLabel);
   m_PropSheet.SetApplyNowButton(false);
   if (m_PropSheet.DoModal()==IDOK)
   {
      m_GridFont = TextLabel.GetLogFont();
      SetChanged(STDDLG_GRIDFONT, 0);
   }
}


void CStandardDlg::OnOutputParam() 
{
   SetChanged(STDDLG_OUTPUTPARAM, 0);
}

void CStandardDlg::OnChangeGridStep() 
{
   SetChanged(STDDLG_GRIDSTEP, 0);
}

/*
void CStandardDlg::OnNumMode() 
{
   SetChanged(STDDLG_GRIDROUND, 0);
}

void CStandardDlg::OnChangeRounding() 
{
   SetChanged(STDDLG_GRIDROUND, 0);
}
*/
void CStandardDlg::OnReduce() 
{
   SetChanged(STDDLG_EFFECTS, 0);
}

void CStandardDlg::OnApplyNowCmd()
{
   OnApplyNow();
}

bool CStandardDlg::OnApplyNow()
{
   bool bResult = CNonModalDlg::OnApplyNow();
   if (bResult)
      SetChanged(STDDLG_UPDATEGRIDSTEP, 0);
   return bResult;
}

void CStandardDlg::OnStdIntelliGrid() 
{
   SetChanged(STDDLG_GRID_TYPE, 0);
}

void CStandardDlg::OnStdNumMode() 
{
   ETSDIV_NUTOTXT ntx = {0,m_nNumMode, m_nRounding, 0, 0};
   if (CEtsDiv::NumberRepresentationDlg(&ntx, m_hWnd))
   {
      SetChanged(STDDLG_GRIDROUND, 0);
      m_nNumMode  = ntx.nmode;
      m_nRounding = ntx.nround;
   }
}

void CStandardDlg::OnStdNumModeZ() 
{
   ETSDIV_NUTOTXT ntx = {0,m_nNumModeZ, m_nRoundingZ, 0, 0};
   if (CEtsDiv::NumberRepresentationDlg(&ntx, m_hWnd))
   {
      SetChanged(STDDLG_GRIDROUND, 0);
      m_nNumModeZ  = ntx.nmode;
      m_nRoundingZ = ntx.nround;
   }
}
