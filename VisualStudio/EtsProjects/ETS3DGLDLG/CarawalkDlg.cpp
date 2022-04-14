// CarawalkDlg.cpp: Implementierungsdatei
//
#include "stdafx.h"
#include "ETS3DGLDLG.h"
#include "CarawalkDlg.h"
#include "ETS3DGLDLGDlg.h"
#include "Ets3dGLRegKeys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef UINT (CALLBACK* COMMDLGPROC)(HWND, UINT, UINT, LONG);

void     *g_AfxCommDlgProc       = NULL;


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCarawalkDlg 

CCarawalkDlg::CCarawalkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCarawalkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCarawalkDlg)
	m_nPrespectiveAngle = 0;
	m_fAmbientColFact = 0.5f;
	m_fAvgColFrom = 0.2f;
	m_fAvgColTo = 0.7f;
	//}}AFX_DATA_INIT
	m_nDepthBits          = CARAWALK_DEPTH_BITS_DEFAULT;
	m_bGenericAccelerated = CARAWALK_GENERIC_ACC_DEFAULT;
	m_nStencilBits        = CARAWALK_STENCIL_BITS_DEFAULT;
	m_nColorBits          = CARAWALK_COLOR_BITS_DEFAULT;
	m_nAlphaBits          = CARAWALK_ALPHA_BITS_DEFAULT;
	m_nAccumulationBits   = CARAWALK_ACCUM_BITS_DEFAULT;
	m_nRefreshTime        = CARAWALK_REFRESH_TIME_DEFAULT;
   m_nPrespectiveAngle   = CARAWALK_PERSPECTIVE_ANGLE_DEFAULT;
   m_cTexAmbient         = CARAWALK_TEX_AMBIENT_DEFAULT;
   m_cTexDiffuse         = CARAWALK_TEX_DIFFUSE_DEFAULT;
   m_cTexSpecular        = CARAWALK_TEX_SPECULAR_DEFAULT;

   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   if (pApp->SetProfileName(CARAWALK_MODULEFILENAME, CARAWALK_REGKEY))
   {
      m_bGenericAccelerated = (pApp->GetProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_GENERIC_ACC, 0) != 0) ? true : false;
      m_nColorBits          = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_COLOR_BITS  , m_nColorBits);
      m_nDepthBits          = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_DEPTH_BITS  , m_nDepthBits);
      m_nStencilBits        = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_STENCIL_BITS, m_nStencilBits);
      m_nAlphaBits          = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_ALPHA_BITS  , m_nAlphaBits);
      m_nAccumulationBits   = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_ACCUM_BITS  , m_nAccumulationBits);
      m_nRefreshTime        = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_REFRESH_TIME, m_nRefreshTime);
      m_nPrespectiveAngle   = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_PERSPECTIVE_ANGLE, m_nPrespectiveAngle);
      m_cTexAmbient         = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_TEX_AMBIENT, m_cTexAmbient);
      m_cTexDiffuse         = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_TEX_DIFFUSE, m_cTexDiffuse);
      m_cTexSpecular        = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_TEX_SPECULAR, m_cTexSpecular);
      int nAvgB             = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_TEX_AVG_B, CARAWALK_TEX_AVG_B_DEFAULT);
      int nAvgM             = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_TEX_AVG_M, CARAWALK_TEX_AVG_M_DEFAULT);
      int nColFac           = pApp->GetProfileInt(CARAWALK_GL_SETTINGS , CARAWALK_TEX_AMBIENT_FACTOR, CARAWALK_TEX_AMBIENTF_DEFAULT);
      m_fAvgColFrom         = 0.001f * nAvgB;
      m_fAvgColTo           = 0.001f * (nAvgB + nAvgM);
      m_fAmbientColFact     = 0.001f * nColFac;
   }
   else
   {
      m_bGenericAccelerated = NOT_AVAILABLE;
   }
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME1);
}


void CCarawalkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCarawalkDlg)
	DDX_Text(pDX, IDC_DEPTH_BITS, m_nDepthBits);
	DDV_MinMaxInt(pDX, m_nDepthBits, 0, 32);
	DDX_Check(pDX, IDC_GENERIC_ACCELERATED, m_bGenericAccelerated);
	DDX_Text(pDX, IDC_STENCIL_BITS, m_nStencilBits);
	DDV_MinMaxInt(pDX, m_nStencilBits, 0, 32);
	DDX_Text(pDX, IDC_COLOR_BITS, m_nColorBits);
	DDV_MinMaxInt(pDX, m_nColorBits, 0, 32);
	DDX_Text(pDX, IDC_ALPHA_BITS, m_nAlphaBits);
	DDV_MinMaxInt(pDX, m_nAlphaBits, 0, 32);
	DDX_Text(pDX, IDC_ACCUM_BITS, m_nAccumulationBits);
	DDV_MinMaxInt(pDX, m_nAccumulationBits, 0, 32);
	DDX_Text(pDX, IDC_REFRESH_TIME, m_nRefreshTime);
	DDV_MinMaxInt(pDX, m_nRefreshTime, 0, 20000);
	DDX_Text(pDX, IDC_PERSPECTIVE_ANGLE, m_nPrespectiveAngle);
	DDV_MinMaxInt(pDX, m_nPrespectiveAngle, 10, 170);
	//}}AFX_DATA_MAP

	DDX_TextVar(pDX, IDC_AMBIENT_COL_FACTOR, FLT_DIG, m_fAmbientColFact);
	DDV_MinMaxFloat(pDX, m_fAmbientColFact, 0.f, 1.f);

	DDX_TextVar(pDX, IDC_AVG_COL_FROM, FLT_DIG, m_fAvgColFrom);
	DDV_MinMaxFloat(pDX, m_fAvgColFrom, 0.f, 1.f);

	DDX_TextVar(pDX, IDC_AVG_COL_TO, FLT_DIG, m_fAvgColTo);
	DDV_MinMaxFloat(pDX, m_fAvgColTo, 0.f, 1.f);
}


BEGIN_MESSAGE_MAP(CCarawalkDlg, CDialog)
	//{{AFX_MSG_MAP(CCarawalkDlg)
	ON_BN_CLICKED(IDC_TEX_AMBIENT, OnTexAmbient)
	ON_BN_CLICKED(IDC_TEX_DIFFUSE, OnTexDiffuse)
	ON_BN_CLICKED(IDC_TEX_SPECULAR, OnTexSpecular)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCarawalkDlg 

void CCarawalkDlg::OnOK() 
{
   if (UpdateData(true))
   {
      CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
      if (pApp->SetProfileName(CARAWALK_MODULEFILENAME, CARAWALK_REGKEY))
      {
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_GENERIC_ACC , m_bGenericAccelerated);
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_COLOR_BITS  , m_nColorBits);
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_DEPTH_BITS  , m_nDepthBits);
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_STENCIL_BITS, m_nStencilBits);
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_ALPHA_BITS  , m_nAlphaBits);
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_ACCUM_BITS  , m_nAccumulationBits);
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_REFRESH_TIME, m_nRefreshTime);
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_PERSPECTIVE_ANGLE, m_nPrespectiveAngle);
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_TEX_AMBIENT , m_cTexAmbient);
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_TEX_DIFFUSE , m_cTexDiffuse);
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_TEX_SPECULAR, m_cTexSpecular);
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_TEX_AVG_B, (int)(m_fAvgColFrom*1000));
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_TEX_AVG_M, (int)((m_fAvgColTo-m_fAvgColFrom)*1000));
         pApp->WriteProfileInt(CARAWALK_GL_SETTINGS, CARAWALK_TEX_AMBIENT_FACTOR, (int)(m_fAmbientColFact*1000));
      }
   	CDialog::OnOK();
   }	
}

BOOL CCarawalkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);		// Kleines Symbol verwenden
	
	return TRUE;
}

UINT CALLBACK CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   if (uiMsg == WM_INITDIALOG)
   {
      CHOOSECOLOR *pcc = (CHOOSECOLOR*) lParam;
      if (hdlg && pcc && pcc->lCustData)
      {
         char *text = (char*) pcc->lCustData;
         ::SetWindowText(hdlg, text);
         return 0;
      }
   }
   if (g_AfxCommDlgProc)
      return (((COMMDLGPROC) g_AfxCommDlgProc)(hdlg,uiMsg,wParam,lParam));
   else
      return 0;
}

void CCarawalkDlg::OnTexAmbient() 
{
   CColorDialog cd(m_cTexAmbient, 0, this);
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   CString string;
   string.LoadString(IDS_TEX_AMBIENT_COLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal() == IDOK)
   {
      m_cTexAmbient = cd.m_cc.rgbResult;
   }
} 

void CCarawalkDlg::OnTexDiffuse() 
{
   CColorDialog cd(m_cTexDiffuse, 0, this);
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   CString string;
   string.LoadString(IDS_TEX_DIFFUSE_COLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal() == IDOK)
   {
      m_cTexDiffuse = cd.m_cc.rgbResult;
   }
}

void CCarawalkDlg::OnTexSpecular() 
{
   CColorDialog cd(m_cTexSpecular, 0, this);
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   CString string;
   string.LoadString(IDS_TEX_SPECULAR_COLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal() == IDOK)
   {
      m_cTexSpecular = cd.m_cc.rgbResult;
   }
}

CCarawalkDlg::~CCarawalkDlg()
{
	if (m_hIcon != INVALID_HANDLE_VALUE)
      ::DestroyIcon(m_hIcon);
}
