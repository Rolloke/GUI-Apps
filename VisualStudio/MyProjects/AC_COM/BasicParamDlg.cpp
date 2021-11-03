// BasicParamDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AC_COM.h"
#include "BasicParamDlg.h"
#include "ETS3DGLRegKeys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CBasicParamDlg 


CBasicParamDlg::CBasicParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBasicParamDlg::IDD, pParent)
{
   CWinApp *pApp = AfxGetApp();
   BYTE * byteptr=NULL;
   UINT   size = 0;
   pApp->GetProfileBinary(REGKEY_DEFAULT, FR_RANGE, &byteptr, &size);
   if (byteptr)
   {
      if (size == (sizeof(RANGE)*2))
      {
         m_Range1 = ((RANGE*)byteptr)[0];
         m_Range2 = ((RANGE*)byteptr)[1];
         m_nDivision            = pApp->GetProfileInt(REGKEY_DEFAULT, FR_DIVISION, 1);
         m_nPollMeasurementTime = pApp->GetProfileInt(REGKEY_DEFAULT, POLL_TIME_M, 10000);
         m_nPortTrials          = pApp->GetProfileInt(REGKEY_DEFAULT, PORT_TRIALS, 3);
         m_bTrack9_ACD2         = pApp->GetProfileInt(REGKEY_DEFAULT, TRACK9 , 1);
         m_bTrack10_ACD3        = pApp->GetProfileInt(REGKEY_DEFAULT, TRACK10, 0);
         m_nEventTime           = pApp->GetProfileInt(REGKEY_DEFAULT, PORT_EVENT_TIME, 500);
      }
      delete[] byteptr;
   }

   if (size != sizeof(RANGE)*2)
      OnBsBtnDefault();
}

void CBasicParamDlg::DoDataExchange(CDataExchange* pDX)
{
   double dTemp;
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CBasicParamDlg)
	DDX_Text(pDX, IDC_BS_EDT_TRIALS, m_nPortTrials);
	DDV_MinMaxInt(pDX, m_nPortTrials, 1, 30);
	DDX_Check(pDX, IDC_BS_CK_TRACK10NEW, m_bTrack10_ACD3);
	DDX_Check(pDX, IDC_BS_CK_TRACK9OLD, m_bTrack9_ACD2);
	DDX_Text(pDX, IDC_BS_EDT_EVENT_TIME, m_nEventTime);
	DDV_MinMaxLong(pDX, m_nEventTime, 100, 5000);
	//}}AFX_DATA_MAP
   
   dTemp = 0.001 * m_nPollMeasurementTime;
	DDX_Text(pDX, IDC_BS_EDT_POLLTIME, dTemp);
   m_nPollMeasurementTime = (int) (dTemp * 1000);
	DDV_MinMaxInt(pDX, m_nPollMeasurementTime, 1000, 30000);

	DDX_TextVar(pDX, IDC_BS_EDT_X_FROM1, DBL_DIG, m_Range1.x1);
	DDX_TextVar(pDX, IDC_BS_EDT_X_TO1  , DBL_DIG, m_Range1.x2);
	DDX_TextVar(pDX, IDC_BS_EDT_Y_FROM1, DBL_DIG, m_Range1.y1);
	DDX_TextVar(pDX, IDC_BS_EDT_Y_TO1  , DBL_DIG, m_Range1.y2);

	DDX_TextVar(pDX, IDC_BS_EDT_X_FROM2, DBL_DIG, m_Range2.x1);
	DDX_TextVar(pDX, IDC_BS_EDT_X_TO2  , DBL_DIG, m_Range2.x2);
	DDX_TextVar(pDX, IDC_BS_EDT_Y_FROM2, DBL_DIG, m_Range2.y1);
	DDX_TextVar(pDX, IDC_BS_EDT_Y_TO2  , DBL_DIG, m_Range2.y2);

   if (pDX->m_bSaveAndValidate)
   {
      pDX->m_bSaveAndValidate = false;
      if (m_Range1.x1 > m_Range1.x2)
      {
         Swap(m_Range1.x1, m_Range1.x2);
	      DDX_Text(pDX, IDC_BS_EDT_X_FROM1, m_Range1.x1);
	      DDX_Text(pDX, IDC_BS_EDT_X_TO1, m_Range1.x2);
      }
      if (m_Range1.y1 > m_Range1.y2)
      {
         Swap(m_Range1.y1, m_Range1.y2);
	      DDX_Text(pDX, IDC_BS_EDT_Y_FROM1, m_Range1.y1);
	      DDX_Text(pDX, IDC_BS_EDT_Y_TO1, m_Range1.y2);
      }
      if (m_Range2.x1 > m_Range2.x2)
      {
         Swap(m_Range2.x1, m_Range2.x2);
	      DDX_Text(pDX, IDC_BS_EDT_X_FROM2, m_Range2.x1);
	      DDX_Text(pDX, IDC_BS_EDT_X_TO2, m_Range2.x2);
      }
      if (m_Range2.y1 > m_Range2.y2)
      {
         Swap(m_Range2.y1, m_Range2.y2);
	      DDX_Text(pDX, IDC_BS_EDT_Y_FROM2, m_Range2.y1);
	      DDX_Text(pDX, IDC_BS_EDT_Y_TO2, m_Range2.y2);
      }
      pDX->m_bSaveAndValidate = true;
   }
	DDX_Radio(pDX, IDC_BS_R_LIN, m_nDivision);
}


BEGIN_MESSAGE_MAP(CBasicParamDlg, CDialog)
	//{{AFX_MSG_MAP(CBasicParamDlg)
   ON_MESSAGE(WM_HELP, OnHelp)
	ON_BN_CLICKED(IDC_BS_BTN_DEFAULT, OnBsBtnDefault)
	ON_BN_CLICKED(IDC_BS_CK_TRACK9OLD, OnBsCkTrack9TestCD)
	ON_BN_CLICKED(IDC_BS_CK_TRACK10NEW, OnBsCkTrack10TestCD)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDHELP, CDialog::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CBasicParamDlg 

void CBasicParamDlg::Swap(double &d1, double &d2)
{
   double dT = d1;
               d1 = d2;
                    d2 = dT;
}

void CBasicParamDlg::OnBsBtnDefault()
{
	//{{AFX_DATA_INIT(CBasicParamDlg)
	m_nDivision                = 1;        // logarithmisch in Dekaden
	m_nPortTrials              =       3;  // 3 Wiederholungen
	m_bTrack10_ACD3            = FALSE;
	m_bTrack9_ACD2             = TRUE;
	m_nEventTime               = 500;
	//}}AFX_DATA_INIT
	m_nPollMeasurementTime   =   10000;  // 10 Sekunden
	m_Range1.x1              =    20.0;  // Hz
	m_Range1.x2              = 20000.0;  // Hz
	m_Range1.y1              =    40.0;  // dB
	m_Range1.y2              =   120.0;  // dB
	m_Range2.x1              =   100.0;  // Hz
	m_Range2.x2              = 12500.0;  // Hz
	m_Range2.y1              =     0.0;  // ms
	m_Range2.y2              =  1000.0;  // ms
   if (m_hWnd)
   {
      UpdateData(false);
   }
}

void CBasicParamDlg::OnOK() 
{
   m_nDivision = -1;
   CDialog::OnOK();
   if (m_nDivision != -1)
   {
      CWinApp *pApp = AfxGetApp();
      RANGE r[2];
      r[0] = m_Range1;
      r[1] = m_Range2;
      pApp->WriteProfileBinary(REGKEY_DEFAULT, FR_RANGE, (BYTE*)r, sizeof(RANGE)*2);
      pApp->WriteProfileInt(REGKEY_DEFAULT, FR_DIVISION, m_nDivision);
      pApp->WriteProfileInt(REGKEY_DEFAULT, POLL_TIME_M, m_nPollMeasurementTime);
      pApp->WriteProfileInt(REGKEY_DEFAULT, PORT_TRIALS, m_nPortTrials);
      pApp->WriteProfileInt(REGKEY_DEFAULT, TRACK9 , m_bTrack9_ACD2);
      pApp->WriteProfileInt(REGKEY_DEFAULT, TRACK10, m_bTrack10_ACD3);
      pApp->WriteProfileInt(REGKEY_DEFAULT, PORT_EVENT_TIME, m_nEventTime);
   }
}

void CBasicParamDlg::OnBsCkTrack9TestCD() 
{
   CDataExchange dx(this, true);
   if (!(CAC_COMApp::gm_dwMessageBoxFlags&MBF_WARN_TRACK9) || AfxMessageBox(IDS_TRACK_9_OF_THE_TESTCD, MB_ICONINFORMATION|MB_OKCANCEL|MB_HELP, IDC_BS_CK_TRACK9OLD)==IDOK)
   {
   	DDX_Check(&dx, IDC_BS_CK_TRACK9OLD, m_bTrack9_ACD2);
      if (m_bTrack9_ACD2) m_bTrack10_ACD3 = false;
      dx.m_bSaveAndValidate = false;
   	DDX_Check(&dx, IDC_BS_CK_TRACK10NEW, m_bTrack10_ACD3);
   }
   else
   {
      dx.m_bSaveAndValidate = false;
   	DDX_Check(&dx, IDC_BS_CK_TRACK9OLD, m_bTrack9_ACD2);
   }
}

void CBasicParamDlg::OnBsCkTrack10TestCD() 
{
   CDataExchange dx(this, true);
   if (!(CAC_COMApp::gm_dwMessageBoxFlags&MBF_WARN_TRACK10) || AfxMessageBox(IDS_TRACK_10_OF_THE_TESTCD, MB_ICONINFORMATION|MB_OKCANCEL|MB_HELP, IDC_BS_CK_TRACK10NEW)==IDOK)
   {
   	DDX_Check(&dx, IDC_BS_CK_TRACK10NEW, m_bTrack10_ACD3);
      if (m_bTrack10_ACD3) m_bTrack9_ACD2 = false;
      dx.m_bSaveAndValidate = false;
   	DDX_Check(&dx, IDC_BS_CK_TRACK9OLD, m_bTrack9_ACD2);
   }
   else
   {
      dx.m_bSaveAndValidate = false;
   	DDX_Check(&dx, IDC_BS_CK_TRACK10NEW, m_bTrack10_ACD3);
   }
}

LRESULT CBasicParamDlg::OnHelp(WPARAM wParam, LPARAM lParam)
{
   HELPINFO *pHI  = (HELPINFO*) lParam;

   if (pHI->dwContextId == 0)
   {
      CDialog::OnHelp();
      return true;
   }

   if (pHI->iContextType == HELPINFO_WINDOW)                   // Hilfe für ein Control oder window
   {
      CDialog::WinHelp(HID_BASE_CONTROL + pHI->iCtrlId, HELP_CONTEXT);
   }
   else                                                        // Hilfe für einen Menüpunkt
   {
      CDialog::WinHelp(pHI->dwContextId, HELP_FINDER);
   }

   return true;
}
