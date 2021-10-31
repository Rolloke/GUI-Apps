// PresentationDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ETS3DGLDLG.h"
#include "PresentationDlg.h"
#include "Ets3dGLRegKeys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPresentationDlg 


CPresentationDlg::CPresentationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPresentationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPresentationDlg)
   m_fPrinterOffsetX = 0;
   m_fPrinterOffsetY = 0;
	//}}AFX_DATA_INIT

   OnDefault();
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   if (pApp->SetProfileName(ETS2DV_MODULEFILENAME, ETS2DV_REGKEY))
   {
      DWORD dwOffsetCorrection = pApp->GetProfileInt(PRINTER, OFFSET_CORRECTION, 0);
      m_fPrinterOffsetX = 0.1f * (short)LOWORD(dwOffsetCorrection);
      m_fPrinterOffsetY = 0.1f * (short)HIWORD(dwOffsetCorrection);
      m_n2DVMenuScale = pApp->GetProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_MENU, m_n2DVMenuScale);
      if (m_n2DVMenuScale != 0) m_b2DVMenuBmp = true;
      m_bFillPath = pApp->GetProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_FILLPATH, m_bFillPath) ? true : false;
      m_n2DVRecentFiles = pApp->GetProfileInt(REGKEY_DEFAULT , REGKEY_DEFAULT_MAX_MRU_FILES, m_n2DVRecentFiles);
   }
   else m_b2DVMenuBmp = NOT_AVAILABLE;

   if (pApp->SetProfileName(ETS3DV_MODULEFILENAME, ETS3DV_REGKEY))
   {
      m_n3DVMenuScale = pApp->GetProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_MENU, m_n3DVMenuScale);
      if (m_n3DVMenuScale != 0) m_b3DVMenuBmp = true;
      m_bFillPath = pApp->GetProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_FILLPATH, m_bFillPath) ? true : false;
      m_n3DVRecentFiles = pApp->GetProfileInt(REGKEY_DEFAULT , REGKEY_DEFAULT_MAX_MRU_FILES, m_n3DVRecentFiles);
   }
   else m_b3DVMenuBmp = NOT_AVAILABLE;
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME4);
}

void CPresentationDlg::OnDefault() 
{
	m_b2DVMenuBmp     = true;
	m_n2DVMenuScale   = SETTINGS_MENU_DEFAULT;
   m_n2DVRecentFiles = 6;
	m_b3DVMenuBmp     = true;
	m_n3DVMenuScale   = SETTINGS_MENU_DEFAULT;
   m_n3DVRecentFiles = 6;
	m_bFillPath       = SETTINGS_FILLPATH_DEFAULT;
}

void CPresentationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPresentationDlg)
	DDX_Check(pDX, IDC_2DV_MENU_BMP, m_b2DVMenuBmp);
	DDX_Text(pDX, IDC_2DV_MENU_SCALE, m_n2DVMenuScale);
	DDV_MinMaxInt(pDX, m_n2DVMenuScale, 60, 140);
	DDX_Check(pDX, IDC_3DV_MENU_BMP, m_b3DVMenuBmp);
	DDX_Text(pDX, IDC_3DV_MENU_SCALE, m_n3DVMenuScale);
	DDV_MinMaxInt(pDX, m_n3DVMenuScale, 60, 140);
	DDX_Check(pDX, IDC_FILL_PATH, m_bFillPath);
	DDX_Text(pDX, IDC_2DV_MENU_RECENT_FILES, m_n2DVRecentFiles);
	DDV_MinMaxInt(pDX, m_n2DVRecentFiles, 1, 16);
	DDX_Text(pDX, IDC_3DV_MENU_RECENT_FILES, m_n3DVRecentFiles);
	DDV_MinMaxInt(pDX, m_n3DVRecentFiles, 1, 16);
	//}}AFX_DATA_MAP
	DDX_TextVar(pDX, IDC_2_3DV_PRINTER_OFFSET_X, FLT_DIG, m_fPrinterOffsetX);
	DDV_MinMaxFloat(pDX, m_fPrinterOffsetX, -50.f, 50.f);
	DDX_TextVar(pDX, IDC_2_3DV_PRINTER_OFFSET_Y, FLT_DIG, m_fPrinterOffsetY);
	DDV_MinMaxFloat(pDX, m_fPrinterOffsetY, -50.f, 50.f);
}


BEGIN_MESSAGE_MAP(CPresentationDlg, CDialog)
	//{{AFX_MSG_MAP(CPresentationDlg)
	ON_BN_CLICKED(IDC_DEFAULT, OnDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPresentationDlg 

void CPresentationDlg::OnOK() 
{
   if (UpdateData(true))
   {
      int nValue = 0;
      CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
      short nOffsetX = (short)(m_fPrinterOffsetX * 10.0f);
      short nOffsetY = (short)(m_fPrinterOffsetY * 10.0f);
      if (pApp->SetProfileName(ETS2DV_MODULEFILENAME, ETS2DV_REGKEY))
      {
         if (m_b2DVMenuBmp != 0) nValue = m_n2DVMenuScale;
         pApp->WriteProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_MENU        , nValue);
         pApp->WriteProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_FILLPATH    , m_bFillPath);
         pApp->WriteProfileInt(REGKEY_DEFAULT , REGKEY_DEFAULT_MAX_MRU_FILES, m_n2DVRecentFiles);
         pApp->WriteProfileInt(PRINTER, OFFSET_CORRECTION, MAKELONG(nOffsetX, nOffsetY));
      }
      if (pApp->SetProfileName(ETS3DV_MODULEFILENAME, ETS3DV_REGKEY))
      {
         if (m_b3DVMenuBmp != 0) nValue = m_n3DVMenuScale;
         pApp->WriteProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_MENU        , nValue);
         pApp->WriteProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_FILLPATH    , m_bFillPath);
         pApp->WriteProfileInt(REGKEY_DEFAULT , REGKEY_DEFAULT_MAX_MRU_FILES, m_n3DVRecentFiles);
         pApp->WriteProfileInt(PRINTER, OFFSET_CORRECTION, MAKELONG(nOffsetX, nOffsetY));
      }
   	CDialog::OnOK();
   }	
}

BOOL CPresentationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   if (m_b2DVMenuBmp == NOT_AVAILABLE)
   {
      GetDlgItem(IDC_2DV_MENU_BMP)->EnableWindow(false);
      GetDlgItem(IDC_2DV_MENU_SCALE)->EnableWindow(false);
   }
	
   if (m_b3DVMenuBmp == NOT_AVAILABLE)
   {
      GetDlgItem(IDC_3DV_MENU_BMP)->EnableWindow(false);
      GetDlgItem(IDC_3DV_MENU_SCALE)->EnableWindow(false);
   }

	SetIcon(m_hIcon, TRUE);		// Kleines Symbol verwenden
   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


CPresentationDlg::~CPresentationDlg()
{
	if (m_hIcon != INVALID_HANDLE_VALUE)
      ::DestroyIcon(m_hIcon);
}
