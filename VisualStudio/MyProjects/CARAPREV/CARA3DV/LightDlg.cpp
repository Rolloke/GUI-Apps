// LightDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA3DV.h"
#include "LightDlg.h"
#include "CARA3DVDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

#define LIGHT_PARAMETER  0x00000001
#define LIGHT_ANGLES_NEW 0x00000002

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CLightDlg 
extern "C" __declspec(dllimport) void     *g_AfxCommDlgProc;
extern "C" __declspec(dllimport) COLORREF  g_CustomColors[16];
extern "C" __declspec(dllimport) UINT CALLBACK CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllimport) BOOL DDX_TextVar(CDataExchange*, int, int, double&, bool bThrow=true);

CLightDlg::CLightDlg(CWnd* pParent /*=NULL*/)
	: CNonModalDlg(CLightDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CLightDlg)
   m_fLightDistance    = 100.0;
   m_fLightAnglePhi    =  0.0;
   m_fLightAngleTheta  = 30.0;
   m_fSpotCutOff       = 30.0f;
   m_fSpotExponent     = 0.0f;
   m_bLightOn          = TRUE;
   //}}AFX_DATA_INIT
   m_fLightIntens      = 0.63f;
   m_fMaxDim           =  4.0;
   m_pLights           = NULL;
   m_nSelectedLight    = 1;
}


void CLightDlg::DoDataExchange(CDataExchange* pDX)
{
   double dLightDistance   = (double)m_fLightDistance;
   double dLightAngleTheta = m_fLightAngleTheta;
   double dLightAnglePhi   = m_fLightAnglePhi;
   double dSpotCutOff      = m_fSpotCutOff   * 2.0;
   double dSpotExponent    = m_fSpotExponent * 0.78125;

   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CLightDlg)
	DDX_Control(pDX, IDC_LIGHT_COMBO, m_cLightCombo);
   DDX_Control(pDX, IDC_LIGHT_INTENS0, m_cLightIntens);
   DDX_Check(pDX, IDC_LIGHT_ON, m_bLightOn);
	//}}AFX_DATA_MAP


   DDX_TextVar(pDX, IDC_LIGHT_THETA, FLT_DIG+1, dLightAngleTheta);
   DDV_MinMaxDouble(pDX, dLightAngleTheta, -90.0, 90.0);

   DDX_TextVar(pDX, IDC_LIGHT_EXPONENT, FLT_DIG+1, dSpotExponent);
   DDV_MinMaxDouble(pDX, dSpotExponent, 0.0, 100.0);

   DDX_TextVar(pDX, IDC_LIGHT_DISTANCE, FLT_DIG+1, dLightDistance);
   DDV_MinMaxDouble(pDX, dLightDistance, 0.0, 10000.0);

   DDX_TextVar(pDX, IDC_LIGHT_PHI, FLT_DIG+1, dLightAnglePhi);
   DDX_TextVar(pDX, IDC_LIGHT_CUTOFF, FLT_DIG+1, dSpotCutOff);

   if ((pDX->m_bSaveAndValidate) && (m_nSelectedLight != 0))
   {
      SetChanged(0, LIGHT_ANGLES_NEW);
      if (dSpotCutOff != 360.0)
      {
         DDV_MinMaxDouble(pDX, dSpotCutOff, 0.0, 180.0);
      }
      int nfactor = (int) (dLightAnglePhi / 360.0);
      if (nfactor)
      {
         dLightAnglePhi -= 360.0f * nfactor;
         SetChanged(LIGHT_ANGLES_NEW, 0);
      }
      if (dLightAnglePhi > 180.0)
      {
         dLightAnglePhi = -(360.0f - dLightAnglePhi);
         SetChanged(LIGHT_ANGLES_NEW, 0);
      }
      if (dLightAnglePhi < -180.0)
      {
         dLightAnglePhi = 360.0f + dLightAnglePhi;
         SetChanged(LIGHT_ANGLES_NEW, 0);
      }
   }

   m_fLightAnglePhi   = (float)dLightAnglePhi;
   m_fLightDistance   = (float)dLightDistance;
   m_fLightAngleTheta = (float)dLightAngleTheta;
   m_fSpotCutOff      = (float)(dSpotCutOff   * 0.5);
   m_fSpotExponent    = (float)(dSpotExponent * 1.28);
}


BEGIN_MESSAGE_MAP(CLightDlg, CDialog)
	//{{AFX_MSG_MAP(CLightDlg)
	ON_EN_KILLFOCUS(IDC_LIGHT_DISTANCE, OnKillfocusLightDistance)
	ON_BN_CLICKED(IDC_LIGHTDLG_DEFAULT, OnLightdlgDefault)
	ON_EN_KILLFOCUS(IDC_LIGHT_CUTOFF, OnKillfocusSpotCutoff)
	ON_CBN_SELCHANGE(IDC_LIGHT_COMBO, OnSelchangeLightCombo)
	ON_BN_CLICKED(IDC_LIGHT_COLOR, OnLightColor)
	ON_BN_CLICKED(ID_APPLY_NOW, OnApplyNowCmd)
	ON_EN_KILLFOCUS(IDC_LIGHT_PHI, OnKillfocusLightPhi)
	ON_EN_KILLFOCUS(IDC_LIGHT_THETA, OnKillfocusLightTheta)
	ON_BN_CLICKED(IDC_LIGHT_ON, OnLightOn)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_LIGHT_INTENS0, OnReleasedcaptureLightIntens0)
	ON_EN_CHANGE(IDC_LIGHT_EXPONENT, OnChange)
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_INITMENUPOPUP()
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_INITMENUPOPUP()
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_EN_CHANGE(IDC_LIGHT_THETA, OnChange)
	ON_EN_CHANGE(IDC_LIGHT_PHI, OnChange)
	ON_EN_CHANGE(IDC_LIGHT_DISTANCE, OnChange)
	ON_EN_CHANGE(IDC_LIGHT_CUTOFF, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CLightDlg 
void CLightDlg::OnOK() 
{
   CWnd * pWnd = GetFocus();
   if (GetDlgItem(IDOK) == pWnd)
   {
      GetParent()->SendMessage(WM_APPLY_TO_VIEW, INVALIDATE_VIEW, NULL);
      if (CLightDlg::OnApplyNow())
      {	
         CWnd::DestroyWindow();
      }
   }
   else
   {
      if (GetDlgItem(IDC_LIGHT_DISTANCE) == pWnd)
         OnKillfocusLightDistance();
      pWnd = GetNextDlgTabItem(pWnd);
      if (pWnd) pWnd->SetFocus();
   }
}

void CLightDlg::OnApplyNowCmd()
{
   OnApplyNow();
}

bool CLightDlg::OnApplyNow()
{
   bool bResult = CNonModalDlg::OnApplyNow();
   return bResult;
}

BOOL CLightDlg::OnInitDialog()
{
   m_nSelectedLight = 1;            // 0 entspricht dem Umgebungslicht
   SelectLight(m_nSelectedLight-1); // kann vorher initialisiert werden
   CNonModalDlg::OnInitDialog();
   int i;
   m_cLightIntens.SetRange(0, 100, true);
   for (i=10; i<=90; i+=10) m_cLightIntens.SetTic(i);
   m_cLightIntens.SetPos((int)(m_fLightIntens*100.0f));
   char text[64];
   CString format;
   format.LoadString(IDS_AMBIENT_COMBO);
   m_cLightCombo.AddString(format);
   format.LoadString(IDS_LIGHTN_COMBO);
   for (i=0; i< NO_OF_LIGHTS; i++)
   {
      wsprintf(text, format, i+1);
      m_cLightCombo.AddString(text);
   }
   m_cLightCombo.SetCurSel(m_nSelectedLight);

   UpdateLightCtrls();
   return TRUE;   // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CLightDlg::OnKillfocusLightDistance() 
{
   float fLightDistance = m_fLightDistance;
   if (UpdateData(true) && (fLightDistance != m_fLightDistance))
   {
      UpdateLightCtrls();
      SetChanged(LIGHT_PARAMETER, 0);
   }
   else UpdateData(false);
}

void CLightDlg::UpdateLightCtrls()
{
   m_cLightIntens.SetPos((int)(m_fLightIntens*100.0f));
   if (m_nSelectedLight == 0)
   {
      GetDlgItem(IDC_LIGHT_CUTOFF    )->EnableWindow(false);
      GetDlgItem(IDC_LIGHT_EXPONENT  )->EnableWindow(false);
      GetDlgItem(IDC_LIGHTDLG_DEFAULT)->EnableWindow(false);
      GetDlgItem(IDC_LIGHT_DISTANCE  )->EnableWindow(false);

      GetDlgItem(IDC_LIGHT_PHI       )->EnableWindow(false);
      GetDlgItem(IDC_LIGHT_THETA     )->EnableWindow(false);
      GetDlgItem(IDC_LIGHTDLG_DEFAULT)->EnableWindow(false);
   }
   else
   {
      GetDlgItem(IDC_LIGHT_PHI       )->EnableWindow(true);
      GetDlgItem(IDC_LIGHT_THETA     )->EnableWindow(true);
      GetDlgItem(IDC_LIGHTDLG_DEFAULT)->EnableWindow(true);
      GetDlgItem(IDC_LIGHT_DISTANCE  )->EnableWindow(true);
      if (m_fLightDistance == 0.0)
      {
         GetDlgItem(IDC_LIGHT_CUTOFF    )->EnableWindow(false);
         GetDlgItem(IDC_LIGHT_EXPONENT  )->EnableWindow(false);
         GetDlgItem(IDC_LIGHTDLG_DEFAULT)->EnableWindow(false);
      }
      else
      {
         GetDlgItem(IDC_LIGHT_CUTOFF    )->EnableWindow(true);
         GetDlgItem(IDC_LIGHT_EXPONENT  )->EnableWindow(true);
         GetDlgItem(IDC_LIGHTDLG_DEFAULT)->EnableWindow(m_fSpotCutOff != 180.0);
      }
   }
}

void CLightDlg::OnKillfocusSpotCutoff() 
{
   float fSpotCutoff = m_fSpotCutOff;
   if (UpdateData(true) && (fSpotCutoff != m_fSpotCutOff))
   {
      SetChanged(LIGHT_PARAMETER, 0);
      GetDlgItem(IDC_LIGHTDLG_DEFAULT)->EnableWindow(m_fSpotCutOff != 180.0);
   }
}

void CLightDlg::OnLightdlgDefault() 
{
   if (UpdateData(true) && (m_fSpotCutOff > 1.0e-3))
   {
      m_fLightDistance = (float)GetOptDistance(m_fMaxDim, m_fSpotCutOff);
      SetChanged(LIGHT_PARAMETER, 0);
      UpdateData(false);
   }
}

void CLightDlg::SelectLight(int nLight)
{
   float pfLC[3];
   if (nLight == -1)
   {
      memcpy(pfLC, m_pfAmbient, sizeof(float)*3);
      m_bLightOn = (m_pfAmbient[3] != 0) ? true : false;
   }
   else if ((m_pLights != NULL) && (nLight >= 0) && (nLight < NO_OF_LIGHTS))
   {
      m_bLightOn         = m_pLights[nLight].glLight.m_bEnabled;
      m_fLightAnglePhi   = m_pLights[nLight].fPhi;
      m_fLightAngleTheta = m_pLights[nLight].fTheta;
      m_fLightDistance   = m_pLights[nLight].fDistance;
      m_fSpotCutOff      = m_pLights[nLight].glLight.m_fSpotCutOff;
      m_fSpotExponent    = m_pLights[nLight].glLight.m_fSpotExponent;
      memcpy(pfLC, m_pLights[nLight].glLight.m_pfAmbientColor, sizeof(float)*3);
   }
   m_fLightIntens = max(pfLC[0]       , pfLC[1]);
   m_fLightIntens = max(m_fLightIntens, pfLC[2]);
   if (m_fLightIntens > 0.0f)
   {
      CGlLight::MultnFloat(3, pfLC, (float)(1.0 / m_fLightIntens));
      m_cLightColor = CGlLight::FloatsToColor(pfLC);
   }
   else 
   {
      m_cLightColor = RGB(255, 255, 255);
   }
}

void CLightDlg::OnSelchangeLightCombo() 
{
   int nSelectedLight = m_cLightCombo.GetCurSel();
   if (nSelectedLight != m_nSelectedLight)
   {
      UpdateData(true);
      if (m_dwChanges & LIGHT_PARAMETER)
      {
         if (AfxMessageBox(IDS_APPLY_LIGHT_CHANGES, MB_YESNO|MB_ICONQUESTION, 0)==IDYES)
         {
            OnApplyNow();
         }
         else m_dwChanges = 0;
      }
      m_nSelectedLight = nSelectedLight;
      SelectLight(m_nSelectedLight-1);
      UpdateLightCtrls();
      UpdateData(false);
   }
}

void CLightDlg::OnLightColor() 
{
   CString string;
   CColorDialog cd(m_cLightColor, 0, this);
   cd.m_cc.lpCustColors = g_CustomColors;
   g_AfxCommDlgProc     = cd.m_cc.lpfnHook;
   cd.m_cc.lpfnHook     = CCHookProc;
   cd.m_cc.Flags       |=  CC_FULLOPEN;

   string.LoadString(IDS_LIGHTCOLOR);
   cd.m_cc.lCustData = (long) LPCTSTR(string);

   if (cd.DoModal()==IDOK)
   {
      COLORREF color = cd.GetColor();
      if (m_cLightColor != color) 
      {
         SetChanged(LIGHT_PARAMETER, 0);
         m_cLightColor = color;
      }
   }
   g_AfxCommDlgProc = NULL;
}

void CLightDlg::ReturnLightParam(Cara3DLight *pLight)
{
   if (pLight)
   {
      pLight->glLight.m_bEnabled      = (m_bLightOn != 0) ? true : false;
      pLight->fPhi                    = m_fLightAnglePhi;
      pLight->fTheta                  = m_fLightAngleTheta;
      pLight->fDistance               = m_fLightDistance;
      pLight->glLight.m_fSpotCutOff   = m_fSpotCutOff;
      pLight->glLight.m_fSpotExponent = m_fSpotExponent;
      float pfLightColor[4] = {0,0,0,1};
      CGlLight::ColorToFloat(m_cLightColor, pfLightColor);
      CGlLight::MultnFloat(3, pfLightColor, m_fLightIntens);
      pLight->glLight.SetAmbientColor(pfLightColor);
      pLight->glLight.SetDiffuseColor(pfLightColor);
   }
}

void CLightDlg::ReturnAmbientLight(float*pfLightColor)
{
   CGlLight::ColorToFloat(m_cLightColor, pfLightColor);
   CGlLight::MultnFloat(3, pfLightColor, m_fLightIntens);
   if (m_bLightOn) pfLightColor[3] = 1.0;
   else            pfLightColor[3] = 0.0;
}

void CLightDlg::OnKillfocusLightPhi() 
{
   float fLightAnglePhi = m_fLightAnglePhi;
   if (UpdateData(true) && (fLightAnglePhi != m_fLightAnglePhi))
   {
      SetChanged(LIGHT_PARAMETER, 0);
      if (m_dwChanges & LIGHT_ANGLES_NEW)
      {
         UpdateData(false);
         SetChanged(0, LIGHT_ANGLES_NEW);
      }
   }
}

void CLightDlg::OnKillfocusLightTheta() 
{
   float fLightAngleTheta = m_fLightAngleTheta;
   if (UpdateData(true) && (fLightAngleTheta != m_fLightAngleTheta))
   {
      SetChanged(LIGHT_PARAMETER, 0);
      if (m_dwChanges & LIGHT_ANGLES_NEW)
      {
         UpdateData(false);
         SetChanged(0, LIGHT_ANGLES_NEW);
      }
   }
}

void CLightDlg::OnLightOn() 
{
   SetChanged(LIGHT_PARAMETER, 0);
}

void CLightDlg::OnReleasedcaptureLightIntens0(NMHDR* pNMHDR, LRESULT* pResult) 
{
   float fLightIntens = 0.01f * m_cLightIntens.GetPos();
   if (m_fLightIntens != fLightIntens)
   {
      m_fLightIntens = fLightIntens;
      SetChanged(LIGHT_PARAMETER, 0);
   }
	*pResult = 0;
}

double CLightDlg::GetOptDistance(double dMaxDim, double dSpotCutoff)
{
   return dMaxDim / tan(dSpotCutoff * M_PI_D_180);
}

void CLightDlg::OnChange() 
{
   SetChanged(LIGHT_PARAMETER, 0);
}
