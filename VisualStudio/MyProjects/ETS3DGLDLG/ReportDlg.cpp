// ReportDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ETS3DGLDLG.h"
#include "ReportDlg.h"
#include "Ets3dGLRegKeys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CReportDlg 


CReportDlg::CReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReportDlg)
	m_bReportETSCDPL = FALSE;
	m_bReportAC_Com = FALSE;
	//}}AFX_DATA_INIT
	m_bReportCarawalk  = REPORT_KEY_DEFAULT;
	m_bReportEts2DView = REPORT_KEY_DEFAULT;
	m_bReportEts3DView = REPORT_KEY_DEFAULT;
	m_bReportEts3DGL   = REPORT_KEY_DEFAULT;
	m_bReportEtsPrtD   = REPORT_KEY_DEFAULT;
	m_bReportCaraSdb   = REPORT_KEY_DEFAULT;
	m_bReportCaraBox   = REPORT_KEY_DEFAULT;
   m_bReportAC_Com    = REPORT_KEY_DEFAULT;
   m_bReportETSCDPL   = REPORT_KEY_DEFAULT;

   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   pApp->SetProfileName(NULL, CETS3DGL_REGKEY);
   m_bReportEts3DGL   = pApp->GetProfileInt("", REPORT_KEY, m_bReportEts3DGL);
   pApp->SetProfileName(NULL, CARAWALK_REGKEY);
   m_bReportCarawalk  = pApp->GetProfileInt("", REPORT_KEY, m_bReportCarawalk);
   pApp->SetProfileName(NULL, ETS2DV_REGKEY);
   m_bReportEts2DView = pApp->GetProfileInt("", REPORT_KEY, m_bReportEts2DView);
   pApp->SetProfileName(NULL, ETS3DV_REGKEY);
   m_bReportEts3DView = pApp->GetProfileInt("", REPORT_KEY, m_bReportEts3DView);
   pApp->SetProfileName(NULL, ETSPRTD_REGKEY);
   m_bReportEtsPrtD   = pApp->GetProfileInt("", REPORT_KEY, m_bReportEtsPrtD);
   pApp->SetProfileName(NULL, CARASDB_REGKEY);
   m_bReportCaraSdb   = pApp->GetProfileInt("", REPORT_KEY, m_bReportCaraSdb);
   pApp->SetProfileName(NULL, CARABOX_REGKEY);
   m_bReportCaraBox   = pApp->GetProfileInt("", REPORT_KEY, m_bReportCaraBox);
   pApp->SetProfileName(NULL, AC_COM_REGKEY);
   m_bReportAC_Com   = pApp->GetProfileInt("", REPORT_KEY, m_bReportAC_Com);
   pApp->SetProfileName(NULL, ETSCDPL_REGKEY);
   m_bReportETSCDPL   = pApp->GetProfileInt("", REPORT_KEY, m_bReportETSCDPL);

   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME2);
}

void CReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReportDlg)
	DDX_Check(pDX, IDC_REPORT_CARAWALK, m_bReportCarawalk);
	DDX_Check(pDX, IDC_REPORT_ETS2DVIEW, m_bReportEts2DView);
	DDX_Check(pDX, IDC_REPORT_ETS3DGL, m_bReportEts3DGL);
	DDX_Check(pDX, IDC_REPORT_ETS3DVIEW, m_bReportEts3DView);
	DDX_Check(pDX, IDC_REPORT_ETSPRTD, m_bReportEtsPrtD);
	DDX_Check(pDX, IDC_REPORT_CARASDB, m_bReportCaraSdb);
	DDX_Check(pDX, IDC_REPORT_CARABOX, m_bReportCaraBox);
	DDX_Check(pDX, IDC_REPORT_ETSCDPL, m_bReportETSCDPL);
	DDX_Check(pDX, IDC_REPORT_AC_COM,  m_bReportAC_Com);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReportDlg, CDialog)
	//{{AFX_MSG_MAP(CReportDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CReportDlg 

void CReportDlg::OnOK() 
{
   if (UpdateData(true))
   {
      CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
      pApp->SetProfileName(NULL, CETS3DGL_REGKEY);
      pApp->WriteProfileInt("", REPORT_KEY, m_bReportEts3DGL);
      pApp->SetProfileName(NULL, CARAWALK_REGKEY);
      pApp->WriteProfileInt("", REPORT_KEY, m_bReportCarawalk);
      pApp->SetProfileName(NULL, ETS2DV_REGKEY);
      pApp->WriteProfileInt("", REPORT_KEY, m_bReportEts2DView);
      pApp->SetProfileName(NULL, ETS3DV_REGKEY);
      pApp->WriteProfileInt("", REPORT_KEY, m_bReportEts3DView);
      pApp->SetProfileName(NULL, ETSPRTD_REGKEY);
      pApp->WriteProfileInt("", REPORT_KEY, m_bReportEtsPrtD);
      pApp->SetProfileName(NULL, CARASDB_REGKEY);
      pApp->WriteProfileInt("", REPORT_KEY, m_bReportCaraSdb);
      pApp->SetProfileName(NULL, CARABOX_REGKEY);
      pApp->WriteProfileInt("", REPORT_KEY, m_bReportCaraBox);
      pApp->SetProfileName(NULL, AC_COM_REGKEY);
      pApp->WriteProfileInt("", REPORT_KEY, m_bReportAC_Com);
      pApp->SetProfileName(NULL, ETSCDPL_REGKEY);
      pApp->WriteProfileInt("", REPORT_KEY, m_bReportETSCDPL);
   	CDialog::OnOK();
   }	
}

BOOL CReportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);		// Kleines Symbol verwenden
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

CReportDlg::~CReportDlg()
{
	if (m_hIcon != INVALID_HANDLE_VALUE)
      ::DestroyIcon(m_hIcon);
}
