/********************************************************************/
/* Funktionen der Klasse CBoxPropertySheet                          */
/********************************************************************/

#include "BoxPropertySheet.h"
#include "CEtsDlg.h"
#include "FileHeader.h"
#include "CCaraBoxView.h"
#include "resource.h"
#include "Cara3DTr.h"
#include "CEtsDiv.h"
#include "BoxFileDlg.h"
#include "PLOT.H"
#include "CURVE.H"
#include "CSortFunctions.h"
#include "CEtsPropertyPage.h"
#include "Debug.h"
#include "BOXGLOBAL.H"
#include "CustErr.h"
#include "CSPLViewDlg.h"
#include "AfxRes.h"
#include "EtsAppWndMsg.h"
#include "BoxCabinetDesign.h"
#include "CProgressDlg.h"
#include "C3DViewDlg.h"

#include <stdio.h>
#include <commctrl.h>
#include <process.h>
#include <time.h>
#include <wininet.h>
#include <prsht.h>

#ifdef _DEBUG_HEAP
 #ifdef Alloc
  #undef Alloc
 #endif
 #define Alloc(p1, p2) Alloc(p1, p2, __FILE__, __LINE__)
 #define new DLL_DEBUG_NEW
#endif

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

extern HINSTANCE g_hInstance;
extern char      g_szBoxPath[_MAX_PATH];
extern int       g_nUserMode;              // Registryparameter

#define NCOUNTBOXPT           0
#define NCOUNTBOXPOLY         1

#define PSBOX_CALE            0
#define NBOXPOLY              2
#define PNCOUNTBOXPOLYPT      3
#define PPNINDEXBOXPOLYPT     4

#define NCOUNTTOTALPT         0
#define NCOUNTCHASSISPOLY     1
#define NCOUNTSPHERES         2
#define NCOUNTSHAPED          3

#define PNCOUNTCHASSISPOLYPT  0
#define PSCHASSIS_CALE        1
#define PPNINDEXCHASSISPOLYPT 2
#define NSTARTINDEX           3
#define IPOLY                 4
#define NBOXPT                5
#define PSSPHEREPOINTS        6
#define PSSPHERERADIUS        7
#define NSPHEREINDEX          8

#define SPL_NO_OF_DISTANCE    4
#define SPL_NO_OF_THETA       31
#define SPL_NO_OF_PHI         36
#define SPL_NO_OF_FREQUENCIES CALCNOOFFREQ

#define PHASE_SPECIFIC_UNIT      "deg"          // Einheit für die Zeitanimation in Grad (0,..,360)
#define PHASEVALUES_AVAILABLE    0x00000001     // Anfangsphasen für die Zeitanimation vorhanden ?
#define LEVELVALUES_LOG          0x00000002     // Amplitudenwerte logarithmiert
#define LEVELVALUES_IN_DB10      0x00000004     // Amplitudenwerte (10 * log(P1/p2))
#define LEVELVALUES_IN_DB20      0x00000008     // Amplitudenwerte (20 * log(P1/p2))
#define NO_VALUE_NORMALIZATION   0x00000010     // Kein Normalisierungsfaktor
#define ONE_NORMALIZATION_FACTOR 0x00000020     // Ein Normalisierungsfaktor für alle Bilder
#define N_NORMALIZATION_FACTORS  0x00000040     // n Normalisierungsfaktoren für n Bilder
#define PHASEVALUES_CALCULATED   0x00020000     // Phasenwerte für die Animation berechnet
#define CALCULATE_PHASEVALUES    0x00040000     // Phasenwerte müssen für die Animation berechnet werden
#define PHI_VALUES_COMPLETE      0x00100000     // Phiwerte von 0,..,360°
#define SPECIFICVALUES_AVAILABLE 0x00200000     // 

#define NO_OF_PHASEVALUES        72             // 5°-Schritte für die Animation
#define ERROR_DIR_DATA           88
#define TERMINATE_THREAD_SAVE    0x00010000

#define WM_ETSVIEW_CLOSED WM_USER+100

//#ifndef _DEBUG
char CBoxPropertySheet::gm_szCARADir[MAX_PATH] = "";
//#endif
double CBoxPropertySheet::gm_dFreq[CALCNOOFFREQ];// CALE Standard-Frequenzen

const double CBoxPropertySheet::gm_dDist[4] = {1.0, 3.0, 5.0, 10.0};
const double CBoxPropertySheet::gm_dDefaultTransmisionRanges[5][2] = 
{
   {   45.0, 20000.0},        // Standard Lautsprecher
   {   55.0, 20000.0},        // Dipolflachmembran
   {   35.0, 20000.0},        // Dipolflachmembran (Hybrid)
   {   35.0,   200.0},        // Subwoofer
   { 3500.0, 20000.0}         // Zusatzhochtöner
};

LONG CBoxPropertySheet::gm_lOldGrpCtrlSCProc = NULL;
LONG CBoxPropertySheet::gm_lOldGrpCtrlCount  = 0;

CBoxPropertySheet::CBoxPropertySheet()
{
   char szDir[MAX_PATH];
   GetCARADirectory(MAX_PATH, szDir);
   m_nFlagChanged = 0;
   InitData();
   m_ppppcSP_3D = NULL;
   m_pCaraBoxV  = NULL;
   m_pFH        = NULL;
   m_hFile      = INVALID_HANDLE_VALUE;

   m_DirectivData.SetDeleteFunction(DeleteDirectivDataFnc);
   m_DirectivData.SetDestroyMode(true);
   m_ChassisData.SetDeleteFunction(DeleteChassisDataFnc);
   m_ChassisData.SetDestroyMode(true);
   m_ProcessHandles.SetDeleteFunction(DeleteProcessHandlesFnc);
   m_ProcessHandles.SetDestroyMode(true);

   for( int i = 0; i < CALCNOOFFREQ; i++ )       // Standardfrequenzen
   {
      gm_dFreq[i] = 5.0*pow( 2.0, (1.0*i/9.0) );
   }

   m_nBufferPointer = 0;
   m_hCalcThread    = INVALID_HANDLE_VALUE;
   m_nCalcWhat      = 0;
   m_hwndChildDlg   = NULL;

   m_dDistance      = 1.0;
   m_p3DViewDlg     = NULL;
}

CBoxPropertySheet::~CBoxPropertySheet()
{
   Close3DView();
   CloseFile();

   if(m_ppppcSP_3D)
   {
      int i, j, k, nPhi;
      for(i=0; i<SPL_NO_OF_DISTANCE; i++)
      {
         if (m_ppppcSP_3D[i])
         {
            for(j=0; j<SPL_NO_OF_THETA; j++)
            {
               if ((j == 0) || (j == SPL_NO_OF_THETA-1)) nPhi = 1; // für Theta=90°,-90° nur Phi=0°
               else                                      nPhi = SPL_NO_OF_PHI;
               if (m_ppppcSP_3D[i][j])
               {
                  for(k=0; k<nPhi; k++)
                  {
                     if (m_ppppcSP_3D[i][j][k]) delete[] m_ppppcSP_3D[i][j][k];
                  }
                  delete[] m_ppppcSP_3D[i][j];
               }
            }
            delete[] m_ppppcSP_3D[i];
         }
      }
      delete[] m_ppppcSP_3D;
   }
   delete m_p3DViewDlg;
}
 
bool CBoxPropertySheet::Allocate3D_SPL()
{
   if( m_ppppcSP_3D == NULL )
   {
      int m, i, j, nPhi;
      m_ppppcSP_3D = new Complex***[SPL_NO_OF_DISTANCE];
      ZeroMemory(m_ppppcSP_3D, sizeof(Complex***) * SPL_NO_OF_DISTANCE);
      for(m=0; m<SPL_NO_OF_DISTANCE; m++)
      {
         m_ppppcSP_3D[m] = new Complex**[SPL_NO_OF_THETA];
         ZeroMemory(m_ppppcSP_3D[m], sizeof(Complex**) * SPL_NO_OF_THETA);
         for(i=0; i<SPL_NO_OF_THETA; i++)
         {
            if((i == 0) || (i == SPL_NO_OF_THETA-1)) nPhi = 1;
            else                                     nPhi = SPL_NO_OF_PHI;
            m_ppppcSP_3D[m][i] = new Complex*[nPhi];
            ZeroMemory(m_ppppcSP_3D[m][i], sizeof(Complex*)*nPhi);
            for(j=0; j<nPhi; j++)
            {
               m_ppppcSP_3D[m][i][j] = new Complex[CALCNOOFFREQ];
            }
         }
      }
   }
   return true;
}

void CBoxPropertySheet::InitData()
{
   CEtsDialog::gm_nIDFmtMinMaxDouble = IDS_FORMAT_MIN_MAX_DOUBLE;
   CEtsDialog::gm_nIDFmtMinMaxInt    = IDS_FORMAT_MIN_MAX_INT;
   CEtsDialog::gm_nIDFmtNoNumber     = IDS_FORMAT_NO_NUMBER;

   if (!(m_nFlagChanged & CARABOX_DONT_DEL_BT))
      ZeroMemory(&m_BoxText, sizeof(BoxText));
   ::LoadString(g_hInstance, IDS_FILTERBOX, m_BoxText.szTradeMark, BOXTEXT_LEN);

   m_nFlagOk      = CARABOX_INIT_STD_FRQ;
   m_nFlagChanged = UPDATE_CABINET_LIST;
   m_nLSType      = STAND_ALONE;    // Default Lautsprechertyp ist Stand Alone
   m_nFileVersion = 0;
   m_nCurrentCab  = 0;
   m_nNoMovements = 0;
   m_BasicData.InitData();
   m_BasicData.dLowLimitFreq  = gm_dDefaultTransmisionRanges[0][0];
   m_BasicData.dHighLimitFreq = gm_dDefaultTransmisionRanges[0][1];
   m_dVolume               = 0.0;

   m_Cabinet.Init();
}

DWORD CBoxPropertySheet::GetCARADirectory(DWORD nBufferLength, LPTSTR lpBuffer)
{
//#ifdef _DEBUG
//   const char szCaraPath[] = "C:\\Programme\\ETS\\Cara_1_1";
//   strcpy(lpBuffer, szCaraPath);
//   return strlen(lpBuffer);
//#else
   if (gm_szCARADir[0] == 0)
   {
      ::GetCurrentDirectory(MAX_PATH, gm_szCARADir);
   }
   strncpy(lpBuffer, gm_szCARADir, nBufferLength);
   return strlen(lpBuffer);
//#endif
}

void CBoxPropertySheet::ResetAllPages()
{
   HWND hWndFocus = ::GetFocus();
   //              hwndFrom, nIdFrom                           , code      , lParam
   PSHNOTIFY psn = {{m_hWnd, ::GetWindowLong(hWndFocus, GWL_ID), PSN_RESET}, 1};
   //          hwnd , message  , wParam, lParam      , time, ptMouse
   MSG msg = {m_hWnd, WM_NOTIFY, 0     , (LPARAM)&psn, 0   , {0, 0,}};
   EnumChildWindows(m_hWnd, NotifyChildPages, (LPARAM)&msg);
}

bool CBoxPropertySheet::CheckAllPages()
{
   HWND hWndFocus = ::GetFocus();
   //              hwndFrom, nIdFrom                           , code        , lParam
   PSHNOTIFY psn = {{m_hWnd, ::GetWindowLong(hWndFocus, GWL_ID), PSN_WIZBACK}, 1};
//   PSHNOTIFY psn = {{m_hWnd, ID_WIZBACK, PSN_WIZBACK}, 1};
   //          hwnd , message  , wParam, lParam      , time, ptMouse
   MSG msg = {m_hWnd, WM_NOTIFY, 0     , (LPARAM)&psn, 0   , {0, 0,}};
   EnumChildWindows(m_hWnd, NotifyChildPages, (LPARAM)&msg);
   return (psn.lParam!=0) ? true : false;
}

BOOL CALLBACK CBoxPropertySheet::NotifyChildPages(HWND hwnd, LPARAM lParam)
{
   if (hwnd && lParam)
   {
      MSG *pmsg = (MSG*)lParam;
      DWORD dwClassS = ::GetClassLong(pmsg->hwnd, GCW_ATOM);
      DWORD dwClassP = ::GetClassLong(hwnd, GCW_ATOM);
      if (dwClassS == dwClassP)
      {
         ::SendMessage(hwnd, pmsg->message, pmsg->wParam, pmsg->lParam);
         if (pmsg->message == WM_NOTIFY)
         {
            if (((PSHNOTIFY*)pmsg->lParam)->lParam == 0)
               return false;
         }
      }
   }
   return (hwnd != 0) ? true : false;
}

bool CBoxPropertySheet::OnInitSheet(HWND hWnd)
{
   if (CEtsPropertySheet::OnInitSheet(hWnd))
   {
      if (g_szBoxPath[0] != 0)
      {
         HICON hIcon = ::LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON));
         if (hIcon) ::SendMessage(hWnd, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIcon);
      }
      if (m_p3DViewDlg == NULL)
      {
         m_p3DViewDlg = new C3DViewDlg;
         m_p3DViewDlg->SetPropertySheet(this);
         m_p3DViewDlg->GetWindowThreadID(hWnd);
         ::LoadString(g_hInstance, IDS_CABINET_LIST_ITEM1, m_Cabinet.m_szName, CAB_NAME_LEN);
      }
      return true;
   }
   return false;
}

void CBoxPropertySheet::OnChangeLSType()
{
   m_nFlagChanged |= CARABOX_DONT_DEL_BT;
   InitData();
   ResetAllPages();
   m_ChassisData.Destroy();
   m_ProcessHandles.Destroy();
}

void CBoxPropertySheet::OnBtnClckNew()
{
   if (!RequestSaveData())
      return;

   InitData();
   ResetAllPages();
   m_ChassisData.Destroy();
   m_ProcessHandles.Destroy();
}

bool CBoxPropertySheet::OnBtnClckOpen()
{
   bool        bReturn = false;
   CBoxFileDlg pfd(g_hInstance, 0, m_hWnd);
   char        szPathDir[MAX_PATH];

   if (!RequestSaveData())
      return false;
   pfd.SetTemplateName(IDD_OPEN_BOX_FILE);
   GetCARADirectory(MAX_PATH, szPathDir);
   strcat(szPathDir,"\\LS_BOX");
   pfd.SetInitialDir(szPathDir);
   pfd.SetFilter(IDS_BOX_FILTER);
   pfd.ModifyFlags(OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST, 0);
   pfd.m_bOpen = true;
   pfd.SetPropertySheet(this);

   if (pfd.GetOpenFileName())
   {
      InitData();
      ResetAllPages();
      m_ChassisData.Destroy();
      m_ProcessHandles.Destroy();
      bReturn = LoadBoxData(pfd.GetFilePathName());
   }
   Close3DView();

   m_hwndChildDlg = NULL;
   return bReturn;
}
void CBoxPropertySheet::OnBtnClckSPLView()
{
   CAutoDisable ad(m_p3DViewDlg->GetWindowHandle());
   CSPLViewDlg dlg(g_hInstance, IDD_SPL_VIEW, m_hWnd);
   dlg.m_nWhat = 0;
   if (CheckCurrentPage() && UpdateZeroSPL() && (m_nFlagOk & CARABOX_SPL_ZERO) && (dlg.DoModal() == IDOK))
   {
      if (m_nLSType & ACTIVE_LS)
      {
         m_nFlagChanged |= CARABOX_FILTER_SPL;
         CalcTransferFunc();
         m_nFlagChanged &= ~CARABOX_FILTER_SPL;
      }
      switch(dlg.m_nWhat)
      {
         case SPL_VIEW_2D_FRQRESPONSE:
            if ((dlg.m_dDistance != m_dDistance) || (m_nLSType & ACTIVE_LS))
            {
               m_dDistance = dlg.m_dDistance;
               m_nFlagChanged &= ~CARABOX_ADOPT_EFFIC;
               CalcZeroSPL();
               ShowZeroSPL();
               SetAdoptDistance();
               CalcZeroSPL();
            }
            else ShowZeroSPL();
            break;
         case SPL_VIEW_2D_POLAR:
            ShowSPLPolar2D();
            break;
         case SPL_VIEW_3D_POLAR_MAGN:
         case SPL_VIEW_3D_POLAR_PHASE:
            if (!(m_nFlagOk&CARABOX_BDD_DATA))
            {
               CalcInThread(CARABOX_BDD_DATA);
            }
            if (m_nFlagOk & CARABOX_BDD_DATA)
            {
               ShowSPLPolar3D(dlg.m_nDistance);
            }
            break;
         default:
            break;
      }
   }
}

void CBoxPropertySheet::WriteFileFH(void *p, DWORD dwSize)
{
   DWORD dwAnzWritten;
   ASSERT(m_hFile != INVALID_HANDLE_VALUE);
   if (m_pFH) m_pFH->CalcChecksum(p, dwSize);
   ::WriteFile(m_hFile, p, dwSize, &dwAnzWritten, NULL);
   if (dwSize != dwAnzWritten)
      throw (DWORD)FILEERR_WRITE;
}

void CBoxPropertySheet::ReadFileFH(void *p, DWORD dwSize)
{
   ASSERT(m_hFile != INVALID_HANDLE_VALUE);
   DWORD dwAnzRead;
   ::ReadFile(m_hFile, p, dwSize, &dwAnzRead, NULL);
   if (m_pFH) m_pFH->CalcChecksum(p, dwSize);
   if (dwSize != dwAnzRead)
      throw (DWORD)FILEERR_READ;
}

void CBoxPropertySheet::DeleteChassisDataFnc(void *ptr)
{
   ChassisData *pCD = (ChassisData*)ptr;
   delete pCD;
}

void CBoxPropertySheet::DeleteProcessHandlesFnc(void *ptr)
{
   PROCESS_INFORMATION *pP = (PROCESS_INFORMATION*)ptr;
   if (pP->hProcess != NULL)
   {
      DWORD   exitcode;
      if(GetExitCodeProcess(pP->hProcess, &exitcode))
      {
         if(exitcode == STILL_ACTIVE)
         {
            EnumThreadWindows(pP->dwThreadId, CBoxPropertySheet::EnumThreadWndProc, NULL);
         }
      }
      CloseHandle(pP->hProcess);
   }
   if (pP->hThread) CloseHandle(pP->hThread);

   delete pP;
}

BOOL CALLBACK CBoxPropertySheet::EnumThreadWndProc(HWND hwnd, LPARAM /*lParam*/)
{
   if (::IsWindow(hwnd))
   {
      DWORD dwResult = 0;
      ::SendMessageTimeout(hwnd, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 1000, &dwResult);
   }
   return (hwnd != NULL) ? true : false;
}

void CBoxPropertySheet::DeleteDirectivDataFnc(void *ptr)
{
   SDirectivData *psDD = (SDirectivData*) ptr;
   int i, j;
   if (psDD->pszDirectivFileName) free(psDD->pszDirectivFileName);
   if (psDD->pDirectivData)
   {
      for(i=0; i<SPL_NO_OF_THETA; i++)
      {
         if (psDD->pDirectivData[i])
         {
            for(j=0; j<SPL_NO_OF_PHI; j++)
            {
               if (psDD->pDirectivData[i][j]) delete[] psDD->pDirectivData[i][j];
            }
            delete[] psDD->pDirectivData[i];
         }
      }
      delete[] psDD->pDirectivData;
   }
   delete psDD;
}

void CBoxPropertySheet::Create3DView()
{
   SendMsgTo3DDlg(WM_COMMAND, IDCANCEL, (LPARAM)m_hWnd);

   if(m_pCaraBoxV == NULL)
   {
      m_pCaraBoxV = new CCaraBoxView();
   }

   if ((m_pCaraBoxV != NULL) && !m_pCaraBoxV->IsWindow())
   {
      ::SetCurrentDirectory(gm_szCARADir);
      m_pCaraBoxV->Create(m_hWnd);
   }
}

bool CBoxPropertySheet::ShowThisBox(char *pszBox)
{
   if (m_pCaraBoxV && m_pCaraBoxV->IsWindow() && (pszBox != NULL))
   {
      m_pCaraBoxV->DrawBox3DView(pszBox);
      return true;
   }
   return false;
}

void CBoxPropertySheet::Close3DView()
{
   if (m_pCaraBoxV != NULL)
   {
      delete m_pCaraBoxV;
      m_pCaraBoxV = NULL;
   }
}

void CBoxPropertySheet::ShowZeroSPL()
{
   const int     nanzPlots = 2;                  // max 2 Plots (Graphikfenster)
   int           nanzPlotsAct = 2;
   int           i, j, k;
   double        dummy_double;
   double        dCorrectAmplGroup = 1;
   Complex       cVal;
   char          szbuffer2[256], *pszName;
   char          szPlotHeadline[nanzPlots][256]; // Plot-Überschriften

   int           fPlotXTeilung[nanzPlots]    = { CCURVE_FREQUENCY, CCURVE_FREQUENCY };
   int           fPlotYTeilung[nanzPlots]    = { CCURVE_LINEAR, CCURVE_LINEAR };
   char         *szPlotXUnits[nanzPlots]     = { "Hz", "Hz" };
                                                 // Vorbelegung für Impedanzen
   char         *szPlotYUnits[nanzPlots]     = { "dB", "deg" };
   RANGE         Range[nanzPlots]            = { {5.0, 40960.0, 40.0, 120.0},
                                                 {5.0, 40960.0, -180.0, 180.0} };
   int           nanzCurves_Plot[nanzPlots];
   char          szText_Curve[64];
   ChassisData  *pCD;

   if (m_BoxText.szFilename[0] != 0) pszName = m_BoxText.szFilename;
   else                              pszName = m_BoxText.szDescrpt;
   szbuffer2[0] = 0;

   LoadString(g_hInstance, IDS_2DVCAPTION, szbuffer2, sizeof(szbuffer2) );
   wsprintf(szPlotHeadline[0], szbuffer2, pszName, (int)m_dDistance);
   LoadString(g_hInstance, IDS_2DVCAPTION2, szbuffer2, sizeof(szbuffer2) );
   wsprintf( szPlotHeadline[1], szbuffer2, pszName, (int)m_dDistance);

   nanzCurves_Plot[0] = 0;
   pCD = (ChassisData*)m_ChassisData.GetFirst();
   while (pCD)
   {
      if (pCD->ShowZeroSpl()) nanzCurves_Plot[0]++;
      pCD = (ChassisData*)m_ChassisData.GetNext();
   }

   nanzCurves_Plot[0] = m_Cabinet.nCountAllChassis;
   if (nanzCurves_Plot[0] > 1) nanzCurves_Plot[0]++; // bei mehreren Chassis Gesamtschall anzeigen
   nanzCurves_Plot[1] = nanzCurves_Plot[0];

   CPlot         Plot[nanzPlots];
   CFileHeader   FileHeader( "PLT", "1999", 110 );

   FileHeader.CalcChecksum(NULL, 0);                           // Prüfsumme zurücksetzen

   for( i = 0; i < nanzPlotsAct; i++ )
   {
      Plot[i].SetSize(nanzCurves_Plot[i]);
      Plot[i].SetHeading(szPlotHeadline[i]);
      Plot[i].SetXDivision(fPlotXTeilung[i]);
      Plot[i].SetYDivision(fPlotYTeilung[i]);
      Plot[i].SetXUnit(szPlotXUnits[i]);
      Plot[i].SetYUnit(szPlotYUnits[i]);
      Plot[i].SetXNumMode(NM_STD);
      Plot[i].SetYNumMode(NM_STD);
      Plot[i].SetDefaultRange(Range[i]);

      pCD = (ChassisData*) m_ChassisData.GetFirst();

      for(j=0; (j<nanzCurves_Plot[i]); pCD = (ChassisData*) m_ChassisData.GetNext())
      {
         if ((pCD != NULL) && pCD->GetNoOfCoupled()) // m_nLSType & DIPOLE_FLAT)
         {
            if (pCD && pCD->IsDummy()) pCD = NULL;
         }
         if (pCD)
         {
            int nIDStr = IDS_BR + pCD->nChassisTyp -1;
            ::LoadString(g_hInstance, nIDStr, szText_Curve, 64);
            strcat(szText_Curve, ":");
            strcat(szText_Curve, pCD->szDescrpt);
            switch(pCD->nChassisTyp)
            {
               case CARABOX_CHASSISTYP_BR:                           // Baßreflex
               case CARABOX_CHASSISTYP_PM:                           // Passivmembran
               case CARABOX_CHASSISTYP_TL:                           // Transmissionline
               case CARABOX_CHASSISTYP_TT1:
               case CARABOX_CHASSISTYP_TT2: dCorrectAmplGroup = m_BasicData.dCorrectAmplTT12; break;
               case CARABOX_CHASSISTYP_MT1: dCorrectAmplGroup = m_BasicData.dCorrectAmplMT1;  break;
               case CARABOX_CHASSISTYP_MT2: dCorrectAmplGroup = m_BasicData.dCorrectAmplMT2;  break;
               case CARABOX_CHASSISTYP_MT3: dCorrectAmplGroup = m_BasicData.dCorrectAmplMT3;  break;
               case CARABOX_CHASSISTYP_HT:  dCorrectAmplGroup = m_BasicData.dCorrectAmplHT;   break;
            }
         }
         else
         {
            wsprintf(szText_Curve, "Box:%s", m_BoxText.szDescrpt);
         }
         CCurve   *pCurve = Plot[i].GetCurve(j++);
         pCurve->SetFormat(CF_SINGLE_DOUBLE);
         pCurve->SetSize(CALCNOOFFREQ);
         pCurve->SetColor(0xFFFFFFFF);
         pCurve->SetDescription(szText_Curve);
         pCurve->SetX_Value(0             , gm_dFreq[0]             );
         pCurve->SetX_Value(CALCNOOFFREQ-1, gm_dFreq[CALCNOOFFREQ-1]);
        
         for(k=0; k<CALCNOOFFREQ; k++)
         {
            switch( i )
            {
               case 0:                           // Amplituden: 1. Kurve im Plot
                  if(pCD) dummy_double = Betrag(dCorrectAmplGroup * pCD->dCorrectAmpl * pCD->cAmplAccel[k] * pCD->cSPZeroStandard[k]);
                  else    dummy_double = Betrag(m_BasicData.cSPTotalZeroStandard[k]);
                  if( dummy_double < 1E-50 ) dummy_double = -999;
                  else   dummy_double = 20*log10(dummy_double/P0);
                  pCurve->SetY_Value(k, dummy_double);
                  break;

               case 1:                           // Phasen: 2. Kurve im Plot
                  dummy_double = 2.0*M_PI*gm_dFreq[k]*m_dDistance/CSCHALL;
                  SinCos(dummy_double, cVal.imagteil, cVal.realteil);// Entfernungskorrektur für die Phase
                  if(pCD)
                  {
                     dummy_double = Phase(pCD->cAmplAccel[k]*pCD->cSPZeroStandard[k]*cVal);
                  }
                  else
                  {
                     dummy_double = Phase(m_BasicData.cSPTotalZeroStandard[k]*cVal);
                  }
                  if( dummy_double < -180.0 )   dummy_double = -180.0;
                  if( dummy_double >  180.0 )   dummy_double =  180.0;
                  pCurve->SetY_Value(k, dummy_double);
                  break;
            }
         }
      }
      Plot[i].GetChecksum( &FileHeader );
   }

   char szPath[MAX_PATH];
   int nLen = GetCARADirectory(MAX_PATH, szPath);
   if (nLen > 0)
   {
      if (szPath[nLen-1] != '\\') strcat(szPath, "\\");
      PROCESS_INFORMATION *ppinfo = new PROCESS_INFORMATION;
      ZeroMemory(ppinfo, sizeof(PROCESS_INFORMATION));

      m_ProcessHandles.ADDHead(ppinfo);
      if (!Call2DView(szPath, nanzPlotsAct, Plot, false, false, m_hWnd, WM_ETSVIEW_CLOSED, 1, NULL, ppinfo))
      {
         m_ProcessHandles.Delete(ppinfo);
      }
   }
}

void CBoxPropertySheet::ShowSPLPolar2D()
{

}

void CBoxPropertySheet::ShowSPLPolar3D(int nDist)
{
   int                 i, j, k, m;
   int                 nHeadlines, nSize, nNoOfFrequencies;
   float               fDummy;
   double              dDummy, dFakt1Volt, dUSchwelle = 0;
   char                szTempPath[MAX_PATH*2];
   char                szCaraViewFileName[MAX_PATH*2];
   char                szProgrPath[MAX_PATH*2];
   char               *pszName;
   STARTUPINFO         startupinfo;
   CFileHeader         FileHeader("DPO", "2000", 110);

   try
   {
      if (m_BoxText.szFilename[0] != 0) pszName = m_BoxText.szFilename;
      else                              pszName = m_BoxText.szDescrpt;

      ZeroMemory(&startupinfo, sizeof(STARTUPINFO));              // StartupInfo Struktur füllen
      startupinfo.cb          = sizeof(STARTUPINFO);
      startupinfo.wShowWindow = SW_SHOW;

      GetTempPath( MAX_PATH*2, szTempPath );                      // Pfad der Übergabedatei basteln
      GetTempFileName(szTempPath, "C3D", 0, szCaraViewFileName);

      ASSERT(m_hFile == INVALID_HANDLE_VALUE);
      CloseFile();
      m_hFile = CreateFile(szCaraViewFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
      if (m_hFile == INVALID_HANDLE_VALUE) throw (DWORD) ERR_INVALID_HANDLE;

      m_pFH = NULL;
      FileHeader.CalcChecksum(NULL, 0);
      WriteFileFH(&FileHeader, sizeof(CFileHeader));
      m_pFH = &FileHeader;

      nHeadlines = 1;

      WriteFileFH(&nHeadlines, sizeof(int));                      // Anzahl der Überschriften
      for (i=0; i<nHeadlines; i++)
      {
         char szText[256], szFormat[256];
         ::LoadString(g_hInstance, IDS_POLARVIEW_HEADLINE, szFormat, 256);
         wsprintf(szText, szFormat, pszName, (int)gm_dDist[nDist]);
         nSize = strlen(szText);
         WriteFileFH(&nSize, sizeof(int));                       // Länge der Überschrift
         WriteFileFH(szText, sizeof(char)*nSize);                // Überschrift
      }
   
      nNoOfFrequencies = 109;                                     // bis 20480 Hz
      WriteFileFH(&nNoOfFrequencies, sizeof(int));
      fDummy = 0.0;
      WriteFileFH(&fDummy, sizeof(float));                        // Delta Theta
      WriteFileFH(&fDummy, sizeof(float));                        // Delta Phi
      nSize = SPL_NO_OF_THETA;
      WriteFileFH(&nSize, sizeof(int));                           // Anzahl Theta
      nSize = SPL_NO_OF_PHI;
      WriteFileFH(&nSize, sizeof(int));                           // Anzahl Phi
      nSize = 2;
      WriteFileFH(&nSize, sizeof(int));                           // Länge der Einheit
      WriteFileFH("dB", sizeof(char)*nSize);                      // Einheit
      nSize = SPECIFICVALUES_AVAILABLE;
      WriteFileFH(&nSize, sizeof(int));                           // Flags

      m = 0;
      WriteFileFH(&gm_dFreq[m], sizeof(double)*nNoOfFrequencies);  // Spezifische Werte für die Filmbilder
      nSize = 2;
      WriteFileFH(&nSize, sizeof(int));                           // Länge der Einheit
      WriteFileFH("Hz", sizeof(char)*nSize);                      // Einheit
   
      GetTempPath( MAX_PATH*2, szTempPath );                      // Pfad der Übergabedatei basteln
      GetTempFileName(szTempPath, "DPD", 0, szProgrPath);
      nSize = strlen(szProgrPath);
      WriteFileFH(&nSize, sizeof(int));                           // Länge des Pfades
      WriteFileFH(szProgrPath, sizeof(char)*nSize);               // Pfad

      SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);               // FilePointer auf Filebeginn setzen
      m_pFH = NULL;
      WriteFileFH(&FileHeader, sizeof(CFileHeader));
      CloseFile();

      m_hFile = CreateFile(szProgrPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
      if (m_hFile == INVALID_HANDLE_VALUE) throw (DWORD) ERR_INVALID_HANDLE;
      nSize = 0;
      i     = nDist;                                              // Entfernung 1 m
      dFakt1Volt = 1.0/sqrt(1.0*m_BasicData.nNennImped);

      for(; m<nNoOfFrequencies; m++)                              // Frequenzen
      {
         for(j=0; j<SPL_NO_OF_THETA; j++)                         // Theta
         {
            for(k=0; k<SPL_NO_OF_PHI; k++)                        // Phi
            {
               dDummy = (20.0*log10(Betrag(m_ppppcSP_3D[i][j][k][m])/(P0*dFakt1Volt)));
               if (dDummy < dUSchwelle)
                  dDummy = dUSchwelle;
               if (dDummy >  100.0)
                  dDummy =  100.0;
               fDummy = (float)dDummy;
               WriteFileBuffered(&fDummy, sizeof(float));
               nSize++;
               if((j == 0) || (j == SPL_NO_OF_THETA-1)) break;// für Theta=90°,-90° nur Phi=0°
            }
         }
      }

      WriteFileBuffered(NULL, 0);
      CloseFile();

      GetCARADirectory(MAX_PATH*2, szProgrPath);                   // 
      wsprintf(szTempPath, "%s\\ETS3DV.EXE %s :H%d :M%d :W%d", szProgrPath, szCaraViewFileName, m_hWnd, WM_ETSVIEW_CLOSED, 1);

      PROCESS_INFORMATION *ppinfo = new PROCESS_INFORMATION;
      ZeroMemory(ppinfo, sizeof(PROCESS_INFORMATION));
      m_ProcessHandles.ADDHead(ppinfo);

      if (!CreateProcess(NULL, szTempPath, NULL, NULL, false, 0, NULL, NULL, &startupinfo, ppinfo))
      {
         m_ProcessHandles.Delete(ppinfo);
      }
   }
   catch (DWORD nError)
   {
      char text[128];
      wsprintf(text, "ShowSPLPolar3D Error : %0x", nError);
      GetActivePage()->MessageBox((long)text, IDS_PROPSHCAPTION, MB_OK|MB_ICONERROR|MBU_TEXTSTRING);
      CloseFile();
   }
}

/********************************************************************/
/*                                                                  */
/*           Berechnet die Filterkoeffizienten für ein              */
/*              (aktives) boxenspezifisches Filter                  */
/*                                                                  */
/********************************************************************/
void CBoxPropertySheet::CalcFilterBox( FilterBox *sFilterStruct, double *pdFreq )
{
   Complex     cSn;                              // = j*2Pi*freq/2Pi*Grenzfreq
   Complex     cFilterKoeff1, cFilterKoeff2;
   double      da1[5][5], da2[5][5], db1[5][5], db2[5][5]; // Koeffizienten[Order][Type] der Filterpolynome
   int         i, j;

   
   for( i = 0; i < 5; i++ )                      // alle Koeff. auf Null setzen, max. Ordnung=4; max. Typ=4
   {                                             // i = Ordnung, j = Typ
      for( j = 0; j < 5; j++ )
      {
         da1[i][j] = 0.0;
         da2[i][j] = 0.0;
         db1[i][j] = 0.0;
         db2[i][j] = 0.0;
      }
   }
   da1[1][0] = da1[1][2] = da1[1][3] = 1.0;      // 1. Ordnung nur für Butterworth, Bessel u. Tschebyscheff

   da1[2][0] = 1.4142;                           // 2. Ordnung, Butterworth
   db1[2][0] = 1.0;
   da1[2][1] = 1.0;                              // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
   db1[2][1] = 1.0;
   da1[2][2] = 1.3617;                           // 2. Ordnung, Bessel
   db1[2][2] = 0.6180;
   da1[2][3] = 1.3614;                           // 2. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
   db1[2][3] = 1.3827;

   da1[3][0] = 1.0;                              // 3. Ordnung, Butterworth
   da2[3][0] = 1.0;
   db2[3][0] = 1.0;
   da1[3][1] = 1.0;                              // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
   da2[3][1] = 1.0;
   db2[3][1] = 1.0;
   da1[3][2] = 0.7560;                           // 3. Ordnung, Bessel
   da2[3][2] = 0.9996;
   db2[3][2] = 0.4772;
   da1[3][3] = 1.8636;                           // 3. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
   da2[3][3] = 0.6402;
   db2[3][3] = 1.1931;

   da1[4][0] = 1.8478;                           // 4. Ordnung, Butterworth
   db1[4][0] = 1.0;
   da2[4][0] = 0.7654;
   db2[4][0] = 1.0;
   da1[4][2] = 1.3397;                           // 4. Ordnung, Bessel
   db1[4][2] = 0.4889;
   da2[4][2] = 0.7743;
   db2[4][2] = 0.3890;
   da1[4][3] = 2.6282;                           // 4. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
   db1[4][3] = 3.4341;
   da2[4][3] = 0.3648;
   db2[4][3] = 1.1509;
   da1[4][4] = 1.4142;                           // 4. Ordnung, Linkwitz-Riley
   db1[4][4] = 1.0;
   da2[4][4] = 1.4142;
   db2[4][4] = 1.0;

   for( i = 0; i < CALCNOOFFREQ; i++ )
   {
      cFilterKoeff1.SetValues(1.0, 0.0);
      cFilterKoeff2.SetValues(1.0, 0.0);

      if( sFilterStruct->dHPFreq > 0.0 )
         cSn.SetValues(0.0, pdFreq[i]/sFilterStruct->dHPFreq);
      else
         cSn.SetValues(0.0, pdFreq[i]); // HPFreq = 1 HZ

      if( (sFilterStruct->nHPType == 1) && (sFilterStruct->dHPQFaktor > 0.0) )
      {
         da1[2][1] = 1.0/sFilterStruct->dHPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         da2[3][1] = 1.0/sFilterStruct->dHPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
      }

      if( (sFilterStruct->nHPOrder > 0) && (sFilterStruct->nHPOrder < 5) &&
          (sFilterStruct->nHPType >= 0) && (sFilterStruct->nHPType  < 5) )
      {
         cFilterKoeff1  = 1.0 + da1[sFilterStruct->nHPOrder][sFilterStruct->nHPType]/cSn +
                                db1[sFilterStruct->nHPOrder][sFilterStruct->nHPType]/(cSn*cSn);
         cFilterKoeff1 *= 1.0 + da2[sFilterStruct->nHPOrder][sFilterStruct->nHPType]/cSn +
                                db2[sFilterStruct->nHPOrder][sFilterStruct->nHPType]/(cSn*cSn);
         cFilterKoeff1  = 1.0/cFilterKoeff1;
      }

      if( sFilterStruct->dTPFreq > 0.0 )
         cSn.SetValues(0.0, pdFreq[i]/sFilterStruct->dTPFreq);
      else
         cSn.SetValues(0.0, pdFreq[i]/1000000.0 ); // TPFreq = 1 MHZ

      if( (sFilterStruct->nTPType == 1) && (sFilterStruct->dTPQFaktor > 0.0) )
      {
         da1[2][1] = 1.0/sFilterStruct->dTPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         da2[3][1] = 1.0/sFilterStruct->dTPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
      }

      if( (sFilterStruct->nTPOrder > 0) && (sFilterStruct->nTPOrder < 5) &&
          (sFilterStruct->nTPType >= 0) && (sFilterStruct->nTPType  < 5) )
      {
         cFilterKoeff2  = 1.0 + da1[sFilterStruct->nTPOrder][sFilterStruct->nTPType]*cSn +
                                db1[sFilterStruct->nTPOrder][sFilterStruct->nTPType]*cSn*cSn;
         cFilterKoeff2 *= 1.0 + da2[sFilterStruct->nTPOrder][sFilterStruct->nTPType]*cSn +
                                db2[sFilterStruct->nTPOrder][sFilterStruct->nTPType]*cSn*cSn;
         cFilterKoeff2  = 1.0/cFilterKoeff2;
      }

      sFilterStruct->cFilterBox[i] = cFilterKoeff1*cFilterKoeff2;
   }
}

void CBoxPropertySheet::GetDirectivityName(double dEffDiameter, ChassisData* pCD)
{
   static const int  nanzDome = 7, nanzFlach = 13, nanzCone = 13, nanzSphere = 6,nanzDipHT = 7,nanzDipTT1 = 3,nanzDipTT2 = 4;
   static const char *szDome[nanzDome]       = {{"KAL_16"},{"KAL_19"},{"KAL_25"},{"KAL_37"},{"DOME_050"},{"DOME_070"},{"DOME_100"}};
   static const int   nDomeDiam[nanzDome]    = {16,      19,        25,        37,        50,          70,          100};
   static const char *szCon[nanzCone]        = {{"CON_025"},{"CON_050"},{"CON_070"},{"CON_090"},{"CON_110"},{"CON_130"},{"CON_150"},{"CON_170"},{"CON_200"},{"CON_230"},{"CON_270"},{"CON_330"},{"CON_450"}};
   static const int   nConDiam[nanzCone]     = {25,       50,         70,         90,         110,        130,        150,        170,        200,        230,        270,        330,        450};
   static const char *szFla[nanzFlach]       = {{"FLA_025"},{"FLA_050"},{"FLA_070"},{"FLA_090"},{"FLA_110"},{"FLA_130"},{"FLA_150"},{"FLA_170"},{"FLA_200"},{"FLA_230"},{"FLA_270"},{"FLA_330"},{"FLA_450"}};
   static const int   nFlaDiam[nanzFlach]    = {25,       50,         70,         90,         110,        130,        150,        170,        200,        230,        270,        330,        450};
   static const char *szSphere[nanzSphere]   = {{"SPHER050"},{"SPHER100"},{"SPHER150"},{"SPHER200"},{"SPHER300"},{"SPHER400"}};
   static const int   nSphereDiam[nanzSphere]= {50,        100,         150,         200,         300,         400};
   static const char *szDipHT[nanzDipHT]     = {{"UU"},{"UM"},{"MU"},{"MM"},{"MO"},{"OM"},{"OO"}};
   static const int   nDipHT[nanzDipHT+1]    = { 0,   14,    29,    43,    57,    71,    86, 100};
   static const char *szDipTT1[nanzDipTT1]   = {{"_U"},{"_M"},{"_O"}};
   static const int   nDipTT1[nanzDipTT1+1]  = { 0,   33,    67, 100};
   static const char *szDipTT2[nanzDipTT2]   = {{"_U"},{"UM"},{"OM"},{"_O"}};
   static const int   nDipTT2[nanzDipTT2+1]   = { 0,    25,  50,    75, 100};
   int j, nIndex = 0;
   CVector2D    vrcCabWall[NO_OF_RECT_POINTS];
   switch (pCD->GetMembrTyp())
   {
      case CARABOX_MEMBRTYP_FLAT2:     // Flache rechteckige Membran 
      case CARABOX_MEMBRTYP_DIPOLE:    // echte DipolChassis
      if ((m_nLSType & (DIPOLE_FLAT|DIPOLE_FLAT_H)) && m_Cabinet.GetCabinetWall(pCD->nCabWall, vrcCabWall))
      {
         int          i, j, nChassis, nCoupled, nDirectivities;
         int         *pnHeightPercent = NULL;
         char       **ppszDirName  = NULL;
         double       dDiff, dHeight;
         ChassisData *pCDC = NULL;
         char         szDir[32] = "DPT510";
         char         szText[128];
         double       dCenterLine    = CCabinet::GetXCenterLine(vrcCabWall);
         double       dCabWallHeight = Vy(vrcCabWall[0])-Vy(vrcCabWall[1]);
         CVector      vPos;
         CalcInverseTran();
         CCabinet *pCchassis = NULL;
         if (pCD->GetCabinet())                                // ist es nicht das Basissegment
         {
            pCchassis = GetCabinet(pCD->GetCabinet());         // Segment für die Verschiebung und Drehung merken
         }

//         ASSERT(pCD->nCabWall == CARABOX_CABWALL_FRONT);
         ASSERT(pCD->GetWall() == CARABOX_CABWALL_FRONT);

         nDirectivities  = nanzDipHT;                          // Richtcharakteristikdateien
         pnHeightPercent = (int*)nDipHT;                       // für Hochtöner und Mitteltöner würde am häufigsten
         ppszDirName     = (char**)szDipHT;                    // in den Abfragen kommen

         if (pCD->dEffBreite > 550)  
         {
            pCD->dEffDiameter = 700.0;
            szDir[3] = '8';
            szDir[5] = '7';
            nDirectivities  = nanzDipTT2;                      // Richtcharakteristikdatei
            pnHeightPercent = (int*)nDipTT2;                   // für Tieftöner mit ca 700 mm Breite
            ppszDirName     = (char**)szDipTT2;                // und einer wahrscheinlichen Höhe >= 1350 mm
         }
         else if (pCD->dEffBreite > 250) 
         {
            pCD->dEffDiameter = 400.0;
            if (dCabWallHeight > 1350.0)
            {
               szDir[5] = '7';
               nDirectivities  = nanzDipTT2;                   // Richtcharakteristikdatei
               pnHeightPercent = (int*)nDipTT2;                // für Tieftöner mit ca 400 mm Breite
               ppszDirName     = (char**)szDipTT2;             // mit einer Höhe >= 1350 mm
            }
            else
            {
               nDirectivities  = nanzDipTT1;                   // Richtcharakteristikdatei
               pnHeightPercent = (int*)nDipTT1;                // für Tieftöner mit ca 400 mm Breite
               ppszDirName     = (char**)szDipTT1;             // mit einer Höhe < 1350 mm
            }
         }
         else if (pCD->dEffBreite > 65)                        // Mitteltöner
         {
            pCD->dEffDiameter = 100.0;                         // mit ca 100 mm Breite
            szDir[2] = 'M';
         }
         else                                                  // Hochtöner
         {
            pCD->dEffDiameter = 30.0;                          // mit ca 30 mm Breite
            szDir[2] = 'H';
         }

         dDiff = pCD->dEffDiameter * 0.25;                     // Die Membranen überlappen sich
         nChassis = 1 + (int)((pCD->dEffHoehe - pCD->dEffDiameter) / dDiff);// Anzahl der Chassis auf der Schallwand
         if (nChassis > 1)                                     // Abstand zwischen den Chassis
            dDiff = (pCD->dEffHoehe - pCD->dEffDiameter) / (double)(nChassis - 1);
         dHeight  = (pCD->dHeightOnCabWall - pCD->dEffHoehe * 0.5) + pCD->dEffDiameter * 0.5;// erste Chassishöhe
         pCD->SetCalculation(false);                           // sichtbare Flachmembran nicht berechnen

         nCoupled = nChassis + 1;                              // sichtbares Chassis hinten + nicht sichtbare zu berechnende
         pCD->SetNoOfCoupledChassis(nCoupled, &m_ChassisData); // Chassisanzahl setzen
         pCD->SetFirstDummy(1);                                // erstes Dummychassis
//         pCD->AdaptCoupledSize();
         pCDC = pCD->GetCoupled(0);                            // erstes Chassis ist hinten
         strcpy(pCDC->szDescrpt, "Back side of ");
         strncat(pCDC->szDescrpt, pCD->szDescrpt, 32);
         pCDC->SetHidden(false);                               // sichbar
         pCDC->SetCalculation(false);                          // sichtbare Flachmembran hinten auch nicht berechnen
         pCDC->SetShowZeroSpl(false);                          // ZeroSpl nicht anzeigen
         pCDC->SetCalcFilter(false);                           // keine Filterberechnung
         pCDC->SetAdoptChassisEffic(false);                    // keine Pegelanpassung
         pCDC->SetCalcTransferFunc(false);                     // keine Transferfunktion berechnen

         for (i=pCD->GetFirstDummy(); i<nCoupled; i++)         // die Parameter der gekoppelten Chassis
         {
            ChassisData *pCDC = pCD->GetCoupled(i);
            if (pCDC)
            {
               pCDC->SetCalculation(true);                     // wird berechnet
               pCDC->SetHidden(true);                          // wird nicht angezeigt
               pCDC->SetDummy(true);                           // ist ein Dummy
               pCDC->SetShowZeroSpl(false);                    // Spl nicht anzeigen (test)
               pCDC->SetAdoptChassisEffic(false);              // keine Anpassung des Pegels mit diesem Chassis
               pCDC->SetCalcFilter(false);                     // keine Filterfunktion
               pCDC->SetCalcTransferFunc(false);               // keine Transferfunktion berechnen
               pCDC->nPolung          = pCD->nPolung;          // Parameter kopieren
               pCDC->nCabWall         = pCD->nCabWall;         // Gehäusewand
               pCDC->SetMembrTyp(CARABOX_MEMBRTYP_FLAT1);      // Membranform
               pCDC->dEffDiameter     = pCD->dEffDiameter;     // Durchmesser
               pCDC->dDistFromCenter  = pCD->dDistFromCenter;  // Position 2D
               pCDC->dHeightOnCabWall = dHeight;
               pCDC->dResFreqMounted  = pCD->dResFreqMounted;  // Chassisparameter
               pCDC->dResFreqFrei     = pCD->dResFreqFrei;
               pCDC->dQFact           = pCD->dQFact;
               pCDC->dInduReal        = pCD->dInduReal;
               pCDC->dInduImag        = pCD->dInduImag;
               pCDC->dInduExpReal     = pCD->dInduExpReal;
               pCDC->dInduExpImag     = pCD->dInduExpImag;
               pCDC->nChassisTyp      = pCD->nChassisTyp;      // Typ
               wsprintf(szText, "Slave %d of %s", i-pCD->GetFirstDummy()+1, pCD->szDescrpt);
               strncpy(pCDC->szDescrpt, szText, 32);
               pCDC->vPosChassis.SetX() = dCenterLine+pCD->dDistFromCenter; // 2D-Position
               pCDC->vPosChassis.SetY() = dHeight;
               pCDC->vPosChassis.SetZ() = 0.0;
               ReTransformVector(pCDC->vPosChassis);           // in 3D transformieren
               if (pCchassis)                                  // Chassis sitzt auf einem Gehäusesegment
               {
                  SetMatrixPtr((Array44*)pCchassis->m_pdTran, false);
                  TransformVector(pCDC->vPosChassis);          // verschieben und drehen wie das Segment
                  SetMatrixPtr(NULL, false);
               }

               strcpy(pCDC->szDirectFileName, szDir);          // Directivity Daten
               for (j=0; j<nDirectivities; j++)                // Abhängig von der Höhe auf der Schallwand
               {
                  if (IsBetween(dCabWallHeight*pnHeightPercent[j]*0.01, dHeight, dCabWallHeight*pnHeightPercent[j+1]*0.01))
                  {
                     strcat(pCDC->szDirectFileName, ppszDirName[j]);
                     break;
                  }
               }
               dHeight += dDiff;
            }
         }
         break;
      }
      case CARABOX_MEMBRTYP_FLAT1:                             // Flache runde Membran 
      {
         for(j=0; j<nanzFlach; j++)
         {
            if(dEffDiameter >= (double)nFlaDiam[j]) nIndex = j;
            else if( j > 0 )
            {
               if( (dEffDiameter - (double)nFlaDiam[j-1])/(double)(nFlaDiam[j] - nFlaDiam[j-1]) > 0.4)
                  nIndex = j;                                  // tendenziell den größeren Diam nehmen
               break;
            }
         }
         wsprintf(pCD->szDirectFileName, "%s", szFla[nIndex]);
      } break;
      case CARABOX_MEMBRTYP_CON:                               // Konusmembran
      {
         for(j=0; j<nanzCone; j++)
         {
            if(dEffDiameter >= (double)nConDiam[j]) nIndex = j;
            else if (j > 0)
            {
               if ((dEffDiameter - (double)nConDiam[j-1])/(double)(nConDiam[j] - nConDiam[j-1]) > 0.4)
                  nIndex = j;                                  // tendenziell den größeren Diam nehmen
               break;
            }
         }
         wsprintf(pCD->szDirectFileName, "%s", szCon[nIndex]);
      } break;
      case CARABOX_MEMBRTYP_SPHERE:                            // Kugellautsprecher
      {
         for(j=0; j<nanzSphere; j++)
         {
            if(dEffDiameter >= (double)nSphereDiam[j]) nIndex = j;
            else if (j > 0)
            {
               if ((dEffDiameter - (double)nSphereDiam[j-1])/(double)(nSphereDiam[j] - nSphereDiam[j-1]) > 0.4)
                  nIndex = j;                                  // tendenziell den größeren Diam nehmen
               break;
            }
         }
         wsprintf(pCD->szDirectFileName, "%s", szSphere[nIndex]);
      } break;
      case CARABOX_MEMBRTYP_DOME:                              // Kalottenmembran
      {
         for(j=0; j<nanzDome; j++)
         {
            if (dEffDiameter >= (double)nDomeDiam[j]) nIndex = j;
            else if (j > 0)
            {
               if ((dEffDiameter - (double)nDomeDiam[j-1])/(double)(nDomeDiam[j] - nDomeDiam[j-1]) > 0.4)
                  nIndex = j;              // tendenziell den größeren Diam nehmen
               break;
            }
         }
         if (nIndex > 3)                  // Standard-Daten für Kalotten > 37mm
         {
            wsprintf( pCD->szDirectFileName, "%s", szDome[nIndex] );
         }
         else
         {
            double       dCabWallWidth;
            m_Cabinet.GetCabinetWall(pCD->nCabWall, vrcCabWall);
            dCabWallWidth  = 0.5 * ((Vx(vrcCabWall[2]) - Vx(vrcCabWall[1])) + (Vx(vrcCabWall[3]) - Vx(vrcCabWall[0])));
            if (dCabWallWidth >= 260) // breite Schallwand
            {
               if( (0.5*m_Cabinet.dHeight - Vz(pCD->vPosChassis)) < 100) // Pos. des Chassis 'oben'
                  wsprintf( pCD->szDirectFileName, "%s%s", szDome[nIndex], "OB" );
               else
                  wsprintf( pCD->szDirectFileName, "%s%s", szDome[nIndex], "MB" );
            }
            else                        // schmale Schallwand
            {
               if( (0.5*m_Cabinet.dHeight - Vz(pCD->vPosChassis)) < 100) // Pos. des Chassis 'oben'
                  wsprintf( pCD->szDirectFileName, "%s%s", szDome[nIndex], "OS" );
               else
                  wsprintf( pCD->szDirectFileName, "%s%s", szDome[nIndex], "MS" );
            }
         }
      }break;
/*
   static const char *szHorn[nanzHorn]       = {{"HORN025"},{"HORN050"},{"HORN070"},{"HORN090"},{"HORN110"},{"HORN130"},{"HORN150"},{"HORN170"},{"HORN200"},{"HORN230"},{"HORN270"},{"HORN330"},{"HORN450"}};
   static const int   nHornDiam[nanzHorn]    = {25,       50,         70,         90,         110,        130,        150,        170,        200,        230,        270,        330,        450};

      case CARABOX_MEMBRTYP_HORN1:                             // Hornlautsprecher
      {
         for(j=0; j<nanzHorn; j++)
         {
            if(dEffDiameter >= (double)nHornDiam[j]) nIndex = j;
            else if (j > 0)
            {
               if ((dEffDiameter - (double)nHornDiam[j-1])/(double)(nHornDiam[j] - nHornDiam[j-1]) > 0.4)
                  nIndex = j;                                  // tendenziell den größeren Diam nehmen
               break;
            }
         }
         wsprintf(pCD->szDirectFileName, "%s", szHorn[nIndex]);
      } break;
*/
   }
}

Complex *** CBoxPropertySheet::ReadSP_Directiv( char *szFilename)
{
   DWORD    nReturn = 0;
   int      i, j, k, nPhi1, nPhi2;
   char     szPathDir[MAX_PATH];
   double   dTheta3DROT, dPhaseCor, dTemp;
   Complex  cPhaseCor;  // double   dSinPhaseCor, dCosPhaseCor;
   Complex  cWert1, cWert2;

   struct   FloatRI
   {
      float    fReal;
      float    fImag;
   }sRealImag[91];

   struct sKopf
   {
      char  text[5][20];
      char  boxname[10];
      short datenkennung;
      short anz_freq;
      short anz_radius;
      short anz_theta;
      short anz_phi;
      double   freq[118];
      double   radius[118];
      double   theta[91];
      double   phi[180];
   }sDirHeader;

   SDirectivData sDD, *psDD=NULL;
   sDD.pDirectivData       = NULL;
   sDD.pszDirectivFileName = szFilename;

   try
   {
      int nDD = m_DirectivData.FindElement(SortDirectivData, &sDD);
      psDD    = (SDirectivData*)m_DirectivData.GetAt(nDD);
      if (psDD) return psDD->pDirectivData;

      int nLen = GetCARADirectory(MAX_PATH, szPathDir);
      if (nLen == 0)                                        throw (DWORD)1;
      if (szPathDir[nLen-1] != '\\') strcat(szPathDir, "\\");
      strcat(szPathDir, "DIRECTIV\\" );
      strcat(szPathDir, szFilename );
      strcat(szPathDir, ".DAT" );
      
      ASSERT(m_hFile == INVALID_HANDLE_VALUE);
      CloseFile();
      m_hFile = CreateFile(szPathDir, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
      if (m_hFile == INVALID_HANDLE_VALUE) throw (DWORD) ERR_INVALID_HANDLE;
      m_pFH   = NULL;

      ReadFileFH(&sDirHeader, sizeof(sKopf));

      psDD = new SDirectivData;
      psDD->pDirectivData       = NULL;
      psDD->pszDirectivFileName = _strdup(szFilename);
      m_DirectivData.ADDHead(psDD);
      
      psDD->pDirectivData = new Complex**[SPL_NO_OF_THETA]; // SPL-Array[Theta=31][Phi=36][Freq=118] zum Einlesen der
      ZeroMemory(psDD->pDirectivData, sizeof(Complex**)*SPL_NO_OF_THETA);
      for(i=0; i<SPL_NO_OF_THETA; i++)
      {
         psDD->pDirectivData[i] = new Complex*[SPL_NO_OF_PHI];
         ZeroMemory(psDD->pDirectivData[i], sizeof(Complex*)*SPL_NO_OF_PHI);
         for(j=0; j<SPL_NO_OF_PHI; j++)
         {
            psDD->pDirectivData[i][j] = new Complex[CALCNOOFFREQ];
         }
      }

      if (sDirHeader.datenkennung == 1100)                     // CAL3DROT-File
      {
         double dCosPhi[SPL_NO_OF_PHI];
         double dCosTheta[SPL_NO_OF_THETA];
         for( k = 0; k < SPL_NO_OF_PHI; k++ )                  // 36 Phi
         {
            dTemp = k*10.0;
            dCosPhi[k] = cos(dTemp*M_PI_D_180);
         }
         for( j = 0; j < SPL_NO_OF_THETA; j++ )                // 31 Theta
         {
            dTemp = 90.0 - j*6.0;
            dCosTheta[j] = cos(dTemp*M_PI_D_180);
         }
         for( i = 0; i < sDirHeader.anz_freq; i++ )            // 118 Frequenzen
         {
            ReadFileFH(&sRealImag[0], sDirHeader.anz_theta*sizeof(FloatRI));
            dPhaseCor = 2.0*M_PI*gm_dFreq[i]*20.0/CSCHALL;
            SinCos(dPhaseCor, cPhaseCor.imagteil, cPhaseCor.realteil);// SinCos(dPhaseCor, dSinPhaseCor, dCosPhaseCor);
            for( j = 0; j < SPL_NO_OF_THETA; j++ )             // 31 Theta
            {
               for( k = 0; k < SPL_NO_OF_PHI; k++ )            // 36 Phi
               {
                  dTheta3DROT = acos(dCosPhi[k]*dCosTheta[j])*M180_D_PI;
                  nPhi1 = (int) (dTheta3DROT/2.0);
                  nPhi2 = nPhi1+1;
                  if(nPhi2>90)   nPhi2 = 90;                   // Werte aus CAL3DROT müssen phasenkorr. werden
                  cWert1.SetValues((double)sRealImag[nPhi1].fReal, (double)sRealImag[nPhi1].fImag);
                  cWert1 *= cPhaseCor;                         // cWert1 *= Complex(dCosPhaseCor, dSinPhaseCor);
                  cWert2.SetValues((double)sRealImag[nPhi2].fReal, (double)sRealImag[nPhi2].fImag);
                  cWert2 *= cPhaseCor;                         // cWert2 *= Complex(dCosPhaseCor, dSinPhaseCor);
                  psDD->pDirectivData[j][k][i] = cWert1 + ((dTheta3DROT-2.0*nPhi1)/2.0)*(cWert2 - cWert1);
               }
            }
         }
      }                                                     // CALBASTA-File 
      else if ((sDirHeader.datenkennung == 2100) ||         // rundes Chassis
               (sDirHeader.datenkennung == 2150))           // rundes Chassis, dipolstrahler
      {
         for( i = 0; i < CALCNOOFFREQ; i++ )                // nur 109 Frequenzen belegt
         {
            for( j = 0; j < sDirHeader.anz_theta; j++ )     // 31 Theta
            {
               if( i < sDirHeader.anz_freq )
               {
                  ReadFileFH(&sRealImag[0], sDirHeader.anz_phi*sizeof(FloatRI));
               }
               for( k = 0; k < sDirHeader.anz_phi; k++ )    // 36 Phi
               {
                  if( i < sDirHeader.anz_freq )             // Daten aus CALBASTA sind bezgl. der Phase schon korrigiert
                     psDD->pDirectivData[j][k][i].SetValues((double) sRealImag[k].fReal, (double) sRealImag[k].fImag);
                  else
                     psDD->pDirectivData[j][k][i].SetValues(0.0, 0.0);
               }
            }
         }
      }
      else throw (DWORD) ERROR_DIR_DATA;
   }
   catch (DWORD nError)
   {
      char text[128];
      wsprintf(text, "ReadSP_Directiv(%s) Error : %0x", szFilename, nError);
      GetActivePage()->MessageBox((long)text, IDS_PROPSHCAPTION, MB_OK|MB_ICONERROR|MBU_TEXTSTRING);
//      REPORT("ReadSP_Directiv Error : %0x", nError);
      if (psDD) m_DirectivData.Delete(psDD);
      psDD = NULL;
      nReturn = nError;
   }

   CloseFile();

   if (psDD) return psDD->pDirectivData;
   throw (DWORD) nReturn;
//   return NULL;
}

void CBoxPropertySheet::GetThetaPhiIndex(double dTheta, double dPhi, int &nTheta1, int &nTheta2, double &dThetaInt, int &nPhi1, int &nPhi2, double &dPhiInt)
{
   int i;
   nTheta1 = nTheta2 = 15;
   for(i = 0; i < SPL_NO_OF_THETA-1; i++)
   {
      if( (dTheta >= (double)(90-6*(i+1))) && (dTheta <= (double)(90-6*i)) )
      {
         nTheta1 = i+1;
         nTheta2 = i;
      }
   }
   dThetaInt = (dTheta-(90.0-6.0*nTheta1))/6.0;
   if (dThetaInt <      1e-13)  nTheta2 = nTheta1;
   if (dThetaInt > (1.0-1e-13)) nTheta1 = nTheta2;

   nPhi1 = nPhi2 = 0;
   for(i = 0; i < SPL_NO_OF_PHI; i++)
   {
      if( i == SPL_NO_OF_PHI-1)
      {
         if( dPhi >= (double)(10*i))
         {
            nPhi1 = i;
            nPhi2 = 0;
         }
      }
      else
      {
         if( (dPhi >= (double)(10*i)) && (dPhi <= (double)(10*(i+1))) )
         {
            nPhi1 = i;
            nPhi2 = i+1;
         }
      }
   }
   dPhiInt = ((dPhi-10.0*nPhi1)/10.0);
   if (dPhiInt <      1e-13)  nPhi2 = nPhi1;
   if (dPhiInt > (1.0-1e-13)) nPhi1 = nPhi2;
//   REPORT("Theta : %3.14f, %2d, %2d %1.4f, Phi : %3.14f, %2d, %2d %1.4f", dTheta, nTheta1, nTheta2, dThetaInt, dPhi, nPhi1, nPhi2, dPhiInt);
}

Complex CBoxPropertySheet::GetSPLDirectiv(Complex *** pSPThetaPhiFreq, int nTheta1, int nTheta2, double dThetaInt, int nPhi1, int nPhi2, double dPhiInt, int nf)
{
   Complex  cTemp, cWertTheta1Phi1, cWertTheta1Phi2, cWertTheta2Phi1, cWertTheta2Phi2;
   if (nPhi1 != nPhi2)                                // interpolieren über Phi
   {
      cWertTheta1Phi1 = pSPThetaPhiFreq[nTheta1][nPhi1][nf] +
                       (pSPThetaPhiFreq[nTheta1][nPhi2][nf]-pSPThetaPhiFreq[nTheta1][nPhi1][nf]) * dPhiInt;
      if (nTheta1 != nTheta2)                         // interpolieren über Theta und Phi
      {
         cWertTheta2Phi1 = pSPThetaPhiFreq[nTheta2][nPhi1][nf] +
                          (pSPThetaPhiFreq[nTheta2][nPhi2][nf]-pSPThetaPhiFreq[nTheta2][nPhi1][nf]) * dPhiInt;
         cTemp = cWertTheta1Phi1 + (cWertTheta2Phi1-cWertTheta1Phi1) * dThetaInt;
      }
      else cTemp = cWertTheta1Phi1;                   // interpolieren nur über Phi
   }
   else if (nTheta1 != nTheta2)                       // interpolieren nur über Theta
   {
      cTemp = pSPThetaPhiFreq[nTheta1][nPhi1][nf] +
             (pSPThetaPhiFreq[nTheta2][nPhi1][nf]-pSPThetaPhiFreq[nTheta1][nPhi1][nf]) * dThetaInt;
   }
   else cTemp = pSPThetaPhiFreq[nTheta1][nPhi1][nf];
//   TRACE("11(%f, %f)\n12(%f, %f)\n21(%f, %f)\n22(%f, %f)\nbt(%f, %f)\n", pSPThetaPhiFreq[nTheta1][nPhi1][nf], pSPThetaPhiFreq[nTheta1][nPhi2][nf], pSPThetaPhiFreq[nTheta2][nPhi1][nf], pSPThetaPhiFreq[nTheta2][nPhi2][nf], cTemp);
   return cTemp;
}

int CBoxPropertySheet::CountChassisPolyPoints(void *p, WPARAM /*wParam*/, LPARAM lParam)
{
   ChassisData       *pCH   = (ChassisData*)p;
   if (pCH->IsHidden()) return true;
   if (pCH->CheckPositionParam()==0)
   {
      if (pCH->GetMembrTyp() == CARABOX_MEMBRTYP_SPHERE)
      {
         ((long*)lParam)[NCOUNTSPHERES]++;
      }
      else if (pCH->IsRect())
      {
         ((long*)lParam)[NCOUNTTOTALPT]    += 4;
         ((long*)lParam)[NCOUNTCHASSISPOLY]++;
      }
      else if (pCH->GetMembrTyp() != 0)
      {
         ((long*)lParam)[NCOUNTTOTALPT]    += 36;
         ((long*)lParam)[NCOUNTCHASSISPOLY]++;
         if ((pCH->GetMembrTyp() == CARABOX_MEMBRTYP_DOME) ||
             (pCH->GetMembrTyp() == CARABOX_MEMBRTYP_CON))
         {
            ((long*)lParam)[NCOUNTSHAPED]++;
         }
      }
      pCH->bDataOk = true;
   }
   else pCH->bDataOk = false;
   return 1;
}

int CBoxPropertySheet::WriteChassisForm(void *p, WPARAM wParam, LPARAM /*lParam*/)
{
   ChassisData *pCH         = (ChassisData*)p;
   if (pCH->bDataOk)
   {
      BYTE bFlag = 0;
      CBoxPropertySheet *pThis = (CBoxPropertySheet*) wParam;
      int nShapePoints = 0;
      if (pCH->GetMembrTyp() == CARABOX_MEMBRTYP_DOME)
      {
         double dRadiusMM = 0.0005 * pCH->dEffDiameter;
         CVector2D vShapePoints[11] =
         {
            CVector2D(0.0, 0.475*dRadiusMM),  // 0
            CVector2D(0.1, 0.47 *dRadiusMM),  // 10
            CVector2D(0.2, 0.46 *dRadiusMM),  // 20
            CVector2D(0.3, 0.435*dRadiusMM),  // 30
            CVector2D(0.4, 0.41 *dRadiusMM),  // 40
            CVector2D(0.5, 0.37 *dRadiusMM),  // 50
            CVector2D(0.6, 0.325*dRadiusMM),  // 60
            CVector2D(0.7, 0.26 *dRadiusMM),  // 70
            CVector2D(0.8, 0.19 *dRadiusMM),  // 80
            CVector2D(0.9, 0.1  *dRadiusMM),  // 80
            CVector2D(1.0, 0.0            )   // 90
         };
         nShapePoints = sizeof(vShapePoints) / sizeof(CVector2D);
         bFlag = 0x04;
         pThis->WriteFileFH(&bFlag, sizeof(BYTE));
         pThis->WriteFileFH(&nShapePoints, sizeof(int));
         pThis->WriteFileFH(vShapePoints, sizeof(CVector2D)*nShapePoints);
         return 1;
      }
      else if (pCH->GetMembrTyp() == CARABOX_MEMBRTYP_CON)
      {
         double dRadiusMM = 0.0005 * pCH->dEffDiameter;
         CVector2D vShapePoints[9] =
         {
            CVector2D(0.0  , 0.1  *dRadiusMM),  
            CVector2D(0.05 , 0.095*dRadiusMM),  
            CVector2D(0.1  , 0.083*dRadiusMM),  
            CVector2D(0.15 , 0.072*dRadiusMM),  
            CVector2D(0.2  , 0.06 *dRadiusMM),  
            CVector2D(0.25 , 0.04 *dRadiusMM),  
            CVector2D(0.3  , 0.0  *dRadiusMM),  
            CVector2D(1.0  , 0.1  *dRadiusMM),  
            CVector2D(1.0  , 0.0            )   
         };
         nShapePoints = sizeof(vShapePoints) / sizeof(CVector2D);
         bFlag = 0x04;
         pThis->WriteFileFH(&bFlag, sizeof(BYTE));
         pThis->WriteFileFH(&nShapePoints, sizeof(int));
         pThis->WriteFileFH(vShapePoints, sizeof(CVector2D)*nShapePoints);
         return 1;

      }
      pThis->WriteFileFH(&bFlag, sizeof(BYTE));
   }
   return 1;
}

int CBoxPropertySheet::GetChassisPolygons(void *p, WPARAM wParam, LPARAM lParam)
{
   ChassisData *pCH                   = (ChassisData*)p;
   if (pCH->bDataOk)
   {
      CBoxPropertySheet *pThis           = (CBoxPropertySheet*) wParam;
      CCabinet    *pC                    = &pThis->m_Cabinet;
      int         *pnCountChassisPolyPt  = (int*)       ((long*)lParam)[PNCOUNTCHASSISPOLYPT];
      PointFloat  *psChassis_CALE        = (PointFloat*)((long*)lParam)[PSCHASSIS_CALE];
      int        **ppnIndexChassisPolyPt = (int**)      ((long*)lParam)[PPNINDEXCHASSISPOLYPT];
      int          i                     = (int)        ((long*)lParam)[IPOLY], j;
      int          nStartIndex           = (int)        ((long*)lParam)[NSTARTINDEX];
      int          nCountBoxPt           = (int)        ((long*)lParam)[NBOXPT];

      CVector2D    vrcWall[NO_OF_RECT_POINTS], vPos;
      double       dCenterLine, dPhi, dFactor = 0.001, dCenterX, dCenterY;
      CVector      vCalePos, vClocation;

      if (pCH->GetMembrTyp() == CARABOX_MEMBRTYP_SPHERE)
      {
         PointFloat *psSphereLocations = (PointFloat*)((long*)lParam)[PSSPHEREPOINTS];
         float      *psSphereRadius    = (float*)     ((long*)lParam)[PSSPHERERADIUS];
         long       *pnIndex           = &((long*)lParam)[NSPHEREINDEX];
         pThis->m_Cabinet.CalcChassisPos3D(pCH);
         psSphereLocations[*pnIndex]   = pC->VectorToBDDpoint(pCH->vPosChassis, 0.001, pCH->GetCabinet());
         psSphereRadius[   *pnIndex]   = (float)(dFactor*pCH->dEffDiameter * 0.5);
         (*pnIndex)++;
         return 1;
      }
      else
      {
         pThis->m_Cabinet.GetCabinetWall(pCH->nCabWall, vrcWall);
         dCenterLine = pThis->m_Cabinet.GetXCenterLine(vrcWall);
         CalcInverseTran();

         pnCountChassisPolyPt[i] = 0;
         if (pCH->IsRect())
            pnCountChassisPolyPt[i] = 4; // 4 Punkte für Rechteckchassis
         else
            pnCountChassisPolyPt[i] = 36;// runde Chassis, aber keine Kugeln

         ppnIndexChassisPolyPt[i] = new int[pnCountChassisPolyPt[i]];

         dCenterX = dCenterLine+pCH->dDistFromCenter;
         dCenterY = pCH->dHeightOnCabWall;

         for(j=0; j<pnCountChassisPolyPt[i]; j++)
         {
            ppnIndexChassisPolyPt[i][j] = nCountBoxPt + nStartIndex + j;
            if (pnCountChassisPolyPt[i] == 4)   // Rechteckchassis
            {
               switch(j)
               {
                  case 0:
                  vCalePos.SetX() = dCenterX + 0.5*pCH->dEffBreite;
                  vCalePos.SetY() = dCenterY - 0.5*pCH->dEffHoehe ;
                  break;
                  case 1:
                  vCalePos.SetX() = dCenterX + 0.5*pCH->dEffBreite;
                  vCalePos.SetY() = dCenterY + 0.5*pCH->dEffHoehe ;
                  break;
                  case 2:
                  vCalePos.SetX() = dCenterX - 0.5*pCH->dEffBreite;
                  vCalePos.SetY() = dCenterY + 0.5*pCH->dEffHoehe ;
                  break;
                  case 3:
                  vCalePos.SetX() = dCenterX - 0.5*pCH->dEffBreite;
                  vCalePos.SetY() = dCenterY - 0.5*pCH->dEffHoehe ;
                  break;
               }
            }
            else                                               // rundes Chassis
            {
               dPhi = ((10*j)*M_PI_D_180);                        // Winkel Phi im Bogenmaß
               vCalePos.SetX() = dCenterX + 0.5*pCH->dEffDiameter*cos(dPhi);
               vCalePos.SetY() = dCenterY + 0.5*pCH->dEffDiameter*sin(dPhi);
            }
            vCalePos.SetZ() = 0.0;                             // Z ist 0 !!
            ReTransformVector(vCalePos);
            psChassis_CALE[nStartIndex+j] = pC->VectorToBDDpoint(vCalePos, 0.001, pCH->GetCabinet());
         }
      }
      ((long*)lParam)[IPOLY]++;
      ((long*)lParam)[NSTARTINDEX] += pnCountChassisPolyPt[i]; // neuer Startindex für Indexliste
   }

   return 1;
}

int CBoxPropertySheet::WriteChassisData(void *p, WPARAM wParam, LPARAM /*lParam*/)
{
   CBoxPropertySheet *pThis = (CBoxPropertySheet*) wParam;
   ChassisData *pCD = (ChassisData*)p;
   if (!pCD->IsDummy())
      pCD->WriteData(pThis);                                   // Daten des Chassis
   return 1;
}

void CBoxPropertySheet::WriteBoxFile()
{
   int         filebegin, fileend;
   DWORD       dwFlags;
   CFileHeader FileHeader( "BOX", "2002", 100);                // Einfachste Box läßt sich unter Format 1.00 speichern

   if (m_Cabinet.m_dwFlags & COMPLEX_CAB_DESIGN)               // Abweichungen von der Quaderform (Pyramide etc.)
   {
      if (m_Cabinet.m_dwFlags & WIDTH_1_2_ON_BACK)             // Trapezförmige Draufsicht
      {                                                        // z.B.: Studiomonitore etc.
         m_nLSType |= WIDTH_ON_BACK;
         FileHeader.SetVersion(210);                           // ab Version 2.10
      }
      else
      {
         m_nLSType &= ~WIDTH_ON_BACK;
         FileHeader.SetVersion(110);                           // ab Version 1.10
      }
   }

   if (m_nLSType & DIPOLE_FLAT  ) FileHeader.SetVersion(210);  // Flächenstrahler ab 2.10
   if (m_nLSType & DIPOLE_FLAT_H) FileHeader.SetVersion(220);  // Flächenstrahler Hybrid ab 2.20

   if ((FileHeader.GetVersion() < 110) && 
       (m_nFlagOk & CARABOX_FILTER_EDITED))                    // Filter editierbar
   {                                                           // Flag ist gesetzt, wenn ein Filter editiert wurde
      FileHeader.SetVersion(110);                              // ab Version 1.10
   }
   if ((FileHeader.GetVersion() < 210) && 
       (m_nFlagOk & CARABOX_FILTER2_EDITED))                   // Filter 2 wurde editiert (Bug in 1.10 dieses Flag wurde nicht gespeichert)
   {
      FileHeader.SetVersion(210);                              // ab Version 2.10 behoben
   }

   m_nLSType &= ~(ZERO_DEG_REF_LEFT|ZERO_DEG_REF_BACK|ZERO_DEG_REF_RIGHT);
   dwFlags = m_Cabinet.m_dwFlags &(ZERO_DEG_REF_LEFT|ZERO_DEG_REF_BACK|ZERO_DEG_REF_RIGHT);
   if (dwFlags)
   {                                                           // Hauptschallwand nicht vorne
      m_nLSType |= dwFlags;
      if (FileHeader.GetVersion() < 210)
         FileHeader.SetVersion(210);                           // ab Version 2.10
   }

   if (FileHeader.GetVersion() < 210)
   {
      if ((LOWORD(m_nLSType) != STAND_ALONE) ||                // Andere LS Typen als STAND_ALONE oder
          (m_BasicData.nXoverTyp == CARABOX_CO_1W))            // 1-Wege Weichen
      {  
         FileHeader.SetVersion(210);                           // ab Version 2.10
      }
      else if (m_nLSType & ACTIVE_LS)                          // Aktive Lautsprecher mit mehreren Frequenzen
      {
         FileHeader.SetVersion(210);                           // ab Version 2.10
      }

      if (FileHeader.GetVersion()<210)                         // Version < 2.10
      {                                                        // Chassistypen prüfen
         m_ChassisData.ProcessWithObjects(ChassisData::CheckChassisVersion, (WPARAM)&FileHeader, 0);
      }
   }
   
   if (FileHeader.GetVersion()<220)                            // Version < 2.20
   {                                                           // Gehäuse prüfen
      int nCabs = m_Cabinet.m_Cabinets.GetCounter();
      if (nCabs == 1)                                          // 1 zusätzliches Gehäuse ?
      {  // kann ein Kugelhalter sein, der auch in der Version 2.10 vorkommt
         if (GetCabinet(1)->nCountHT == 0)                     // wenn es kein Kugelhalter ist
            FileHeader.SetVersion(220);                        // ist es 2.20
      }
      else if (nCabs > 1) FileHeader.SetVersion(220);          // noch mehr Segmente gibt es ab 2.20
   }

   m_nFileVersion = FileHeader.GetVersion();
   CEtsPropertyPage*pPP = GetActivePage();
   if (pPP)
   {  
      PSHNOTIFY shn = {{ m_hWnd, 0, PSN_SETACTIVE}, 0};
      SendMessage(pPP->GetWindowHandle(), WM_NOTIFY, 0, (LPARAM)&shn);
      UpdateWindow(pPP->GetWindowHandle());
   }

   FileHeader.CalcChecksum(NULL, 0);                           // Prüfsumme zurücksetzen
   filebegin = SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT );// Filepointer auf Anfang merken
   m_pFH = NULL;
   WriteFileFH(&FileHeader, sizeof(CFileHeader));
   m_pFH = &FileHeader;

   WriteFileFH(&m_nFlagOk, sizeof(int));                       // EingabeFlag
   if (FileHeader.GetVersion() > 100)                          // über Version 1.00
      WriteFileFH(&m_nLSType, sizeof(int));                    // Lautsprechertyp speichern
   WriteFileFH(&m_BoxText, sizeof(BoxText));                   // Text-Beschreibungsfelder

   m_Cabinet.WriteData(this);                                  // Gehäusedaten

   m_BasicData.WriteData(this);                                // Basisdaten

   m_ChassisData.ProcessWithObjects(WriteChassisData, (WPARAM)this, FileHeader.GetVersion());

   fileend = SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT );  // Filepointer auf 'Hier' merken
   SetFilePointer(m_hFile, filebegin, NULL, FILE_BEGIN );      // FilePointer auf Filebeginn setzen
   m_pFH = NULL;
   WriteFileFH(&FileHeader, sizeof(CFileHeader));              // neue Prüfsumme schreiben
   SetFilePointer(m_hFile, fileend, NULL, FILE_BEGIN );        // FilePointer auf aktuelle Endpos setzen
}

bool CBoxPropertySheet::ReadBoxFile(const char *pszFilePath)
{
   CFileHeader       FileHeader;
   unsigned int      nCRC_Load;                                // Original-CRC-Code im Header für BOX-Daten
   int               i;
   ChassisData      *pCD;
   bool              bCoupled = false;

   m_pFH = NULL;
   ReadFileFH(&FileHeader, sizeof(CFileHeader));
   m_nFileVersion = FileHeader.GetVersion();
   m_pFH = &FileHeader;
   nCRC_Load = FileHeader.GetChecksum();

   if (FileHeader.IsType( "BOX" ) == 0 )                       // Panel Description Data
   {
      throw (DWORD)FILEERR_TYPE;
   }

   FileHeader.CalcChecksum(NULL, 0);                           // Prüfsumme zurücksetzen

   ReadFileFH(&m_nFlagOk, sizeof(int));                        // EingabeFlag
   m_nFlagOk &= ~(CARABOX_SAVE_AS|CARABOX_PASS_CHECK);

   if (FileHeader.GetVersion() > 100)                          // über Version 1.00
      ReadFileFH(&m_nLSType, sizeof(int));                     // Lautsprechertyp lesen

   ReadFileFH(&m_BoxText, sizeof(BoxText));                    // Text-Beschreibungsfelder

   if (m_nFlagOk & CARABOX_BDD_DATA)
   {                                                           // BDD-Daten vorhanden
      char szPathDir[MAX_PATH];
      char szFilePath[MAX_PATH*2];
      GetCARADirectory(MAX_PATH, szPathDir);
      wsprintf(szFilePath, "%s\\LS_BOX\\%s.BOX", szPathDir, m_BoxText.szFilename);
      bool bBoxExists = (::GetFileAttributes(szFilePath) != 0xffffffff) ? true : false;
      if (_stricmp(szFilePath, pszFilePath) == 0)              // Namen sind gleich
      {
         wsprintf(szFilePath, "%s\\LS_BOX\\%s.BDD", szPathDir, m_BoxText.szFilename);
         if (::GetFileAttributes(szFilePath) == 0xffffffff)    // BDD Daten nicht vorhanden
         {
            CCaraSdb::UpdateDataBase(m_BoxText.szFilename);    // damit die Boxendatenbank neu aufgebaut wird
//            m_BoxText.szFilename[0] = 0;
            m_nFlagOk &= ~CARABOX_BDD_DATA;                    // Flag löschen
            SetChanged(CARABOX_BDD_DATA);                      // geändert setzen
         }
      }
      else                                                     // Namen sind ungleich
      {
         SetChanged(CARABOX_DESCRIPTION);
         wsprintf(szFilePath, "%s\\LS_BOX\\%s.BDD", szPathDir, m_BoxText.szFilename);
         if (::GetFileAttributes(szFilePath) == 0xffffffff)    // BDD Daten nicht vorhanden
         {
            CCaraSdb::UpdateDataBase(m_BoxText.szFilename);    // damit die Boxendatenbank neu aufgebaut wird
//            m_BoxText.szFilename[0] = 0;
            m_nFlagOk &= ~CARABOX_BDD_DATA;                    // Flag löschen
            SetChanged(CARABOX_BDD_DATA);                      // geändert setzen
         }
         else if (!bBoxExists)                                 // Box zum anderen BDD-File existiert nicht
         {
            char szFilePath2[MAX_PATH*2];
            int i, j = 0, iPoint = 0;
            for (i=strlen(pszFilePath)-1; i>=0; i--)           // Dateinamen extrahieren
            {
               if      (pszFilePath[i] == '.') iPoint = i;     // von hinten suchen, Punkt merken
               else if (pszFilePath[i] == '\\') break;         // bis zum ersten Backslash
            }
            for (i++; i<iPoint; i++)
               m_BoxText.szFilename[j++] = pszFilePath[i];     // Den Name ohne Endung kopieren
            m_BoxText.szFilename[j++] = 0;                     // Null anhängen
            wsprintf(szFilePath2, "%s\\LS_BOX\\%s.BDD", szPathDir, m_BoxText.szFilename);
            rename(szFilePath, szFilePath2);                   // BDD-Datei umbenennen
         }                                                     // wenn Sie existiert : in Ruhe lassen
      }
   }

   CEtsPropertyPage*pPP = GetActivePage();
   if (pPP)
   {  
      PSHNOTIFY shn = {{ m_hWnd, 0, PSN_SETACTIVE}, 0};
      SendMessage(pPP->GetWindowHandle(), WM_NOTIFY, 0, (LPARAM)&shn);
      UpdateWindow(pPP->GetWindowHandle());
   }

   if (FileHeader.GetVersion() == 100)
   {
      m_nLSType = STAND_ALONE;
      if (m_nFlagOk & CARABOX_BASE_DATA  ) m_nFlagOk |= CARABOX_DESCRIPTION|CARABOX_CABINET;
      if (m_nFlagOk & CARABOX_CHASSIS_POS) m_nFlagOk |= CARABOX_CHASSIS_DATA;
   }
   
   m_Cabinet.ReadData(this);                                   // Gehäusedaten
   if (!m_Cabinet.IsBaseCabinet())
   {
#ifdef _DEBUG
      ASSERT(false);
      m_Cabinet.m_dwFlags &= ~NO_BASE_CABINET;
#else
      throw (DWORD)FILEERR_TYPE;
#endif
   }

   if (m_nLSType & WIDTH_ON_BACK)                              // Trapezförmige Draufsicht in LS-Typ gespeichert
   {
      m_Cabinet.m_dwFlags |= WIDTH_1_2_ON_BACK;
   }

   DWORD dwFlags = m_nLSType &(ZERO_DEG_REF_LEFT|ZERO_DEG_REF_BACK|ZERO_DEG_REF_RIGHT);
   if (dwFlags)
   {                                                           // Hauptschallwand nicht vorne
      m_Cabinet.m_dwFlags |= dwFlags;
   }

   m_BasicData.ReadData(this);                                 // Basisdaten
   if (FileHeader.GetVersion() == 100)
   {
      CSortFunctions::SetSortDirection(true);
      qsort(&m_BasicData.dImpedLowFreqBR, 3, sizeof(double), CSortFunctions::SortDouble);
   }

   for (i=0; i<m_Cabinet.nCountAllChassis; i++)                // Chassisdaten
   {
      pCD = new ChassisData;
      if (pCD)
      {
         m_ChassisData.ADDHead(pCD);
         pCD->ReadData(this);
         if (pCD->IsSphere())                                  // Kugeln
         {
            if (FileHeader.GetVersion() >= 220)                // Abstandshalter suchen
            {
               CCabinet *pC = (CCabinet*)m_Cabinet.m_Cabinets.GetFirst();
               while (pC)
               {
                  if ((pC->nCountHT != 0) && (((int)pCD->m_ppCabinets[0]) == pC->nCountAllChassis))
                  {
                     pCD->m_ppCabinets[0] = pC;
                     break;
                  }
                  pC = (CCabinet*)m_Cabinet.m_Cabinets.GetNext();
               }
            }
            else
            {
               InitSphereFoot(pCD);
            }
         }                                                     // Füße
         else if ((FileHeader.GetVersion() >= 220) && (pCD->m_ppCabinets != NULL))
         {
            CCabinet *pC = NULL;
            int i, nCabs=1;                                    // erstmal einen suchen
            for (i=0; i<nCabs; i++)                            // 
            {
               pC = (CCabinet*)m_Cabinet.m_Cabinets.GetFirst();
               while (pC)
               {
                  if ((pC->nCountTL != 0) && (((int)pCD->m_ppCabinets[i]) == pC->nCountAllChassis))
                  {                                            // stimmt die Nummer,
                     pCD->m_ppCabinets[i] = pC;                // so stimmt auch die
                     nCabs = pC->nCountTL;                     // Anzahl
                     break;                                    // weitermachen !
                  }
                  pC = (CCabinet*)m_Cabinet.m_Cabinets.GetNext();
               }
            }
         }
         if (m_nLSType & DIPOLE_FLAT)                          // Flächenstrahler
         {
//            pCD->SetNoOfCoupledChassis(1, &m_ChassisData);
            bCoupled = true;
         }
         else if ((m_nLSType & DIPOLE_FLAT_H) &&               // Flächenstrahler Hybrid
                  (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))
         {
//            pCD->SetNoOfCoupledChassis(1, &m_ChassisData);
            bCoupled = true;
         }
      }
   }

   if (FileHeader.GetVersion()<=210)
   {
      m_Cabinet.m_dwFlags |= CHECK_ALL_WALLS;
   }
   if (FileHeader.GetVersion()==100)
   {
      m_Cabinet.m_dwFlags |= CALC_2D_CHASSISPOS;
      m_Cabinet.InitCabPoints();
      m_ChassisData.ProcessWithObjects(CBoxPropertySheet::DeterminChassisPositions, 0, (LPARAM)this);
   }

   CheckCabinetDesign();
   if (m_dVolume > 0.0)                                        // Gehäuse vorhanden ?
   {
      CloseFile();                                             // Datei kann (muß) geschlossen werden
   }

   Close3DView();                                              // Boxenansicht schließen

   if (bCoupled || (FileHeader.GetVersion() == 100))
   {                                                           // bei der alten Version 1.00 und bei Dipolen müssen 
      CSortFunctions::SetSortDirection(false);                 // die Chassis neu sortiert werden
      m_ChassisData.Sort(CSortFunctions::SortChassisType);
   }

   m_nFlagChanged |= UPDATE_CHASSIS_LIST;                      // die Chassisliste auf der Chassisseite muß neu initialisiert werden
   m_nFlagChanged |= UPDATE_CABINET_LIST;                      // die Gehäuseliste auf der Gehäuseseite muß neu initialisiert werden
   if (m_nFlagOk & CARABOX_BDD_DELETED)                        // Wenn ein vorhandener BDD-Datensatz gelöscht wurde
   {                                   
      m_nFlagOk &= ~CARABOX_BDD_DELETED;
      SetChanged(0);                                           // muß der Datensatz auf changed gesetzt werden
   }

   /*if (FileHeader.GetVersion() < 210)*/                      // bei Versionen kleiner als 2.1 mußten die
   {
      m_nFlagOk |= CARABOX_IMPED_EDITED;                       // Impedanzen (Flag = 1) und die
      m_nFlagOk &= ~CARABOX_INIT_STD_FRQ;                      // Übergangsfrequenzen (Flag = 0) immer eingegeben werden !
   } // diese werden ab 2.10 mit Standardwerten vorbelegt

   if (m_nLSType & (DIPOLE_FLAT|DIPOLE_FLAT_H))                // Flächenstrahler
   {
      m_nFlagOk |= CARABOX_DIPOL_RE_INIT;                      // müssen wegen der gekoppelten Chassis neu initialisiert werden
   }
   if( FileHeader.IsValid( nCRC_Load ) == 0 )
   {
      throw (DWORD)FILEERR_CRC;
   }
   return true;
}

bool CBoxPropertySheet::UpdateZeroSPL()
{
   int nPageID = 0;                                            // Seiten prüfen
   if      (!(m_nFlagOk & CARABOX_BASE_DATA))    nPageID = IDD_BOX_BASE_DATA;
   else if (!(m_nFlagOk & CARABOX_CROSS_OVER))   nPageID = IDD_BOX_CROSS_OVER;
   else if (!(m_nFlagOk & CARABOX_CHASSIS_NO))   nPageID = IDD_BOX_CHASSIS;
   else if (!(m_nFlagOk & CARABOX_CHASSIS_DATA)) nPageID = IDD_BOX_CHASSIS;
   else if (!(m_nFlagOk & CARABOX_CABINET))      nPageID = IDD_BOX_CABINET;
   else if (!(m_nFlagOk & CARABOX_CHASSIS_POS))  nPageID = IDD_BOX_CABINET;
   else if ( (m_nFlagChanged & CARABOX_ALL_DATA) ||            // Änderungen prüfen
            !(m_nFlagOk & CARABOX_SPL_ZERO)      ||            // Zerodegree nicht vorhanden
             (m_nFlagOk & CARABOX_DIPOL_RE_INIT))              // Reinitialisierung von Dipolen
   {
      m_nFlagOk &= ~(CARABOX_SPL_ZERO|CARABOX_BDD_DATA|CARABOX_DIPOL_RE_INIT);
      m_nFlagChanged |= CARABOX_ADOPT_EFFIC;
      SetAdoptDistance();
      if (CalcBoxChassisData() && CalcTransferFunc() && CalcZeroSPL())
      {
         m_nFlagChanged &= ~CARABOX_ALL_DATA;
         SetChanged(0);
         m_nFlagOk |= CARABOX_SPL_ZERO;
      }
   }
   if (nPageID)
   {  
      m_nFlagOk &= ~(CARABOX_SPL_ZERO|CARABOX_BDD_DATA);
      if (m_nFlagOk & CARABOX_PASS_CHECK) 
         return true;
      ::SendMessage(m_hWnd, PSM_SETCURSELID, 0, nPageID);
      return CheckCurrentPage();
   }
   return true;
}

int CBoxPropertySheet::CalcBoxChassisData()
{
   int      nReturn = 1;
   ChassisData *pCD;

   try
   {
      for (pCD=(ChassisData*)m_ChassisData.GetFirst(); pCD!=NULL; pCD=(ChassisData*)m_ChassisData.GetNext())
      {
         double   dEffDiameter;
         if (pCD->IsDummy()) break;                         // Die DummyChassis werden hier nicht direkt bestimmt
         if (pCD->IsRect())
            dEffDiameter = sqrt(4.0*pCD->dEffBreite*pCD->dEffHoehe/M_PI);
         else
            dEffDiameter = pCD->dEffDiameter;

         switch(pCD->nChassisTyp)
         {
            case CARABOX_CHASSISTYP_BR:                        // Baßreflex
            case CARABOX_CHASSISTYP_TL:                        // Transmissionline
            pCD->dInduReal    = 0.0;
            pCD->dInduImag    = 0.0;
            pCD->dInduExpReal = 1.0;
            pCD->dInduExpImag = 1.0;

            pCD->dQFact          = 10.0;                       // angenommen
            pCD->dResFreqFrei    = 0.0;
            pCD->dResFreqMounted = m_BasicData.dImpedMidFreqBR;
            GetDirectivityName(dEffDiameter, pCD);
            break;

            case CARABOX_CHASSISTYP_PM:                        // Passivmembran
            pCD->dInduReal    = 0.0;
            pCD->dInduImag    = 0.0;
            pCD->dInduExpReal = 1.0;
            pCD->dInduExpImag = 1.0;

            pCD->dQFact          = 4.0;                        // angenommen
            pCD->dResFreqFrei    = m_BasicData.dImpedLowFreqBR * 0.8; // angenommen
            pCD->dResFreqMounted = m_BasicData.dImpedMidFreqBR;
            GetDirectivityName(dEffDiameter, pCD);
            break;

            case CARABOX_CHASSISTYP_TT1:
            case CARABOX_CHASSISTYP_TT2:
            if ((m_nLSType & DIPOLE_FLAT) ||                   // Dipol Flächenstrahler
                (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))// oder echte Dipole
            {
               pCD->dInduReal    = 0.0;
               pCD->dInduImag    = 0.0;
               pCD->dInduExpReal = 1.0;
               pCD->dInduExpImag = 1.0;
            }
            else if (dEffDiameter < 150)                       // Annahme: 25mm Spule (alle mH-Werte sind auf 4 Ohm zu beziehen)
            {
               pCD->dInduReal    = 0.16;
               pCD->dInduImag    = 0.32;
               pCD->dInduExpReal = 0.8;
               pCD->dInduExpImag = 0.6;
            }
            else if (dEffDiameter < 250)                       // Annahme: 37mm Spule
            {
               pCD->dInduReal    = 0.21;
               pCD->dInduImag    = 0.52;
               pCD->dInduExpReal = 0.8;
               pCD->dInduExpImag = 0.6;
            }
            else                                               // Annahme: 50mm Spule
            {
               pCD->dInduReal    = 0.3;
               pCD->dInduImag    = 0.7;
               pCD->dInduExpReal = 0.8;
               pCD->dInduExpImag = 0.6;
            }

            if ((m_nLSType & DIPOLE_FLAT) ||                   // Dipol Flächenstrahler
                (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))// oder echte Dipole
            {
               pCD->dResFreqFrei = 0.0;
               pCD->dResFreqMounted = m_BasicData.dMainResFreq;
               pCD->dQFact = 2.0;
            }
            else if (m_BasicData.nBoxTyp == CARABOX_BOXTYP_CLOS)
            {
               pCD->dResFreqFrei = 0.0;
               pCD->dResFreqMounted = m_BasicData.dMainResFreq;
               if     (m_BasicData.nTypeTTResp == CARABOX_TTRESPONSETYPMINU) pCD->dQFact = 0.5;
               else if(m_BasicData.nTypeTTResp == CARABOX_TTRESPONSETYPNORM) pCD->dQFact = 0.9;
               else                                                          pCD->dQFact = 1.8;
            }
            else
            {
               pCD->dResFreqFrei = m_BasicData.dImpedLowFreqBR*m_BasicData.dImpedHighFreqBR/m_BasicData.dImpedMidFreqBR;
               pCD->dResFreqMounted = sqrt(m_BasicData.dImpedLowFreqBR  * m_BasicData.dImpedLowFreqBR   +
                                           m_BasicData.dImpedHighFreqBR * m_BasicData.dImpedHighFreqBR  -
                                           m_BasicData.dImpedMidFreqBR  * m_BasicData.dImpedMidFreqBR);
               pCD->dQFact = 0.7;                              // Annahme (gilt für Chassis in closed BR-Gehäuse)
            }

            GetDirectivityName(dEffDiameter, pCD);
            break;

            case CARABOX_CHASSISTYP_MT1:
            case CARABOX_CHASSISTYP_MT2:
            case CARABOX_CHASSISTYP_MT3:

            if ((m_nLSType & DIPOLE_FLAT) ||                   // Flächenstrahler
                pCD->IsSphere() ||                             // oder Kugelstrahler
                (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))// oder echte Dipole
            {
               pCD->dInduReal    = 0.0;
               pCD->dInduImag    = 0.0;
               pCD->dInduExpReal = 1.0;
               pCD->dInduExpImag = 1.0;
            }                                                  // wenig überwickelte Spulen
            else if (dEffDiameter < 150)                       // Annahme: 25mm Spule (alle mH-Werte sind auf 4 Ohm zu beziehen)
            {
               pCD->dInduReal = 0.1;
               pCD->dInduImag = 0.2;
               pCD->dInduExpReal = 0.8;
               pCD->dInduExpImag = 0.6;
            }
            else if (dEffDiameter < 250)                       // Annahme: 37mm Spule
            {
               pCD->dInduReal = 0.13;
               pCD->dInduImag = 0.35;
               pCD->dInduExpReal = 0.8;
               pCD->dInduExpImag = 0.6;
            }
            else                                               // Annahme: 50mm Spule
            {
               pCD->dInduReal = 0.2;
               pCD->dInduImag = 0.5;
               pCD->dInduExpReal = 0.8;
               pCD->dInduExpImag = 0.6;
            }

            pCD->dResFreqFrei = 0.0;
            if      (pCD->nChassisTyp == CARABOX_CHASSISTYP_MT1) pCD->dResFreqMounted = 0.5*m_BasicData.sMT1.dHPFreq;
            else if (pCD->nChassisTyp == CARABOX_CHASSISTYP_MT2) pCD->dResFreqMounted = 0.5*m_BasicData.sMT2.dHPFreq;
            else                                                 pCD->dResFreqMounted = 0.5*m_BasicData.sMT3.dHPFreq;
            if (pCD->dResFreqMounted < 100.0) pCD->dResFreqMounted = 100.0;
            pCD->dQFact = 1.0;                                 // Annahme

            GetDirectivityName(dEffDiameter, pCD);
            break;

            case CARABOX_CHASSISTYP_HT:
                                                               
            if ((m_nLSType & DIPOLE_FLAT) ||                   // Flächenstrahler
                pCD->IsSphere() ||                             // oder Kugelstrahler
                (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))// oder echte Dipole
            {
               pCD->dInduReal    = 0.0;
               pCD->dInduImag    = 0.0;
               pCD->dInduExpReal = 1.0;
               pCD->dInduExpImag = 1.0;
            }                                                  // unterwickelte Spulen
            else if( dEffDiameter < 37 )                       // Annahme: 16/19/25mm Spule (alle mH-Werte sind auf 4 Ohm zu beziehen)
            {
               pCD->dInduReal = 0.016;
               pCD->dInduImag = 0.025;
               pCD->dInduExpReal = 1.4;
               pCD->dInduExpImag = 0.8;
            }
            else                                               // Annahme: 37mm (und größer) Spule
            {
               pCD->dInduReal = 0.04;
               pCD->dInduImag = 0.07;
               pCD->dInduExpReal = 1.2;
               pCD->dInduExpImag = 0.7;
            }

            pCD->dResFreqFrei = 0.0;
            pCD->dResFreqMounted = 0.5*m_BasicData.sHT.dHPFreq;

            if( pCD->dResFreqMounted < 800.0 ) pCD->dResFreqMounted = 800.0;

            pCD->dQFact = 1.2;                                 // Annahme

            GetDirectivityName(dEffDiameter, pCD);
            break;
         }
      }
      CalcReferencePoints();                                   // Referenzpunkt berechnen
   }
   catch(DWORD nError)
   {
      char text[128];
      wsprintf(text, "CalcBoxChassisData Error : %0x", nError);
      GetActivePage()->MessageBox((long)text, IDS_PROPSHCAPTION, MB_OK|MB_ICONERROR|MBU_TEXTSTRING);
//      REPORT("CalcBoxChassisData Error : %0x", nError);
      nReturn = 0;
   }
   return (nReturn);
}

/********************************************************************/
/*                                                                  */
/* Berechnet die Übertragungsfunktion aller Chassisbeschleunigungen */
/*       für Systeme 2. Ordnung (Closed Box TT oder MT, HT)         */
/*           bzw. für TT in Vented Boxen und für BR/TL              */
/*    unter Berücksichtigung der Induktivität und Filterfunktion    */
/*  (Annahme: alle TT1, TT2 und BR/TL arbeiten in EINE (!!) Kammer) */
/*                                                                  */
/********************************************************************/
int CBoxPropertySheet::CalcTransferFunc()
{
   int   nReturn = 1;
   int   j;
   Complex  cLaplBR, cLaplTT, cLaplMT1, cLaplMT2, cLaplMT3, cLaplHT, cLaplace;
   Complex  cDummy1, cDummy2, cDummy3, cDummy4;
   Complex  cAmpljPM_PM(0.0,0.0), cAmpljPM_TT1(0.0,0.0), cAmpljPM_TT2(0.0,0.0);
   double   dRdc = 4.0;                          // 4 Ohm ist die Basis für die Induktivitätsberechnung
   double   dummyF_Frei, dummyF_CB, dummyF_BR, dummyF_Frei_PM;
   double   dQFactTTCB = 1.0, dQFactBR = 1.0;
   double   dFactBR = 0.0;                         // Faktor in BR-Transferfunktion (1+Compl_Vol/Compliance)
   double   dFactPM = 0.0;                         // Faktor in PM-Transferfunktion (1-fPMFrei²/fPMCB²)
   double   dFactTT_BR = 0.0;                      // Faktor in TT-Transferfunktion für BR-Boxen (1-fFrei²/fCB²)
   double   dTemp, dHPFreq, dTPFreq;               // sie bei 0 Hz nicht mehr gleich sind, sondern PM < TT.
   int      jPM = -1;
   int      nHPType = 0, nTPType = 0;
   ChassisData *pCD;
   FilterBox *pSphereFilter = NULL;
   FilterBox *pFB = NULL;

   if (m_nLSType & ACTIVE_LS)
   {
      pFB = GetOneWayFilter();                     // Filtertyp sichern
      dHPFreq = pFB->dHPFreq;
      dTPFreq = pFB->dTPFreq;
      nHPType = pFB->nHPType;                    
      nTPType = pFB->nTPType;
      if (m_nFlagChanged & CARABOX_FILTER_BDD)
      {
         pFB->nHPType = -1;                        // Ohne Filter rechnen
         pFB->nTPType = -1;
      }
      else if (m_nFlagChanged & CARABOX_FILTER_SPL)
      {                                            // Selektierte Filterfrequenz verwenden
         pFB->dHPFreq = m_BasicData.fLowerHPFrequ[m_BasicData.nSelLowerHPFrequ];
         pFB->dTPFreq = m_BasicData.fUpperTPFrequ[m_BasicData.nSelUpperTPFrequ];
      }
      else if (m_nFlagChanged & CARABOX_ADOPT_EFFIC)
      {                                            // maximalen Frequenzbereich nehmen
         pFB->dHPFreq = m_BasicData.fLowerHPFrequ[0];
         if (m_BasicData.nNoOfUpperTPFrequ)
            pFB->dTPFreq = m_BasicData.fUpperTPFrequ[m_BasicData.nNoOfUpperTPFrequ-1];
         else pFB->dTPFreq = 0.0;
      }
   }
   try
   {
      if (m_Cabinet.nCountTT1 > 0) CalcFilterBox(&m_BasicData.sTT1, &gm_dFreq[0]);
      if (m_Cabinet.nCountTT2 > 0) CalcFilterBox(&m_BasicData.sTT2, &gm_dFreq[0]);
      if (m_Cabinet.nCountMT1 > 0) CalcFilterBox(&m_BasicData.sMT1, &gm_dFreq[0]);
      if (m_Cabinet.nCountMT2 > 0) CalcFilterBox(&m_BasicData.sMT2, &gm_dFreq[0]);
      if (m_Cabinet.nCountMT3 > 0) CalcFilterBox(&m_BasicData.sMT3, &gm_dFreq[0]);
      if (m_Cabinet.nCountHT  > 0) CalcFilterBox(&m_BasicData.sHT , &gm_dFreq[0]);

      dummyF_Frei    = 0.0;
      dummyF_CB      = 0.0;
      dummyF_BR      = 0.0;
      dummyF_Frei_PM = 0.0;

      for (pCD = (ChassisData*)m_ChassisData.GetFirst(); pCD!=NULL; pCD = (ChassisData*)m_ChassisData.GetNext())
      {
         if (pCD->IsDummy())           break;
         if (!pCD->CalcTransferFunc()) continue;
         switch (pCD->nChassisTyp)
         {
            case CARABOX_CHASSISTYP_BR:                           // Baßreflex
            case CARABOX_CHASSISTYP_TL:                           // Transmissionline
            case CARABOX_CHASSISTYP_PM:                           // Passivmembran
            cLaplBR.SetValues(0.0, 1.0/pCD->dResFreqMounted);
            dQFactBR       = pCD->dQFact;
            dummyF_BR      = pCD->dResFreqMounted;
            dummyF_Frei_PM = pCD->dResFreqFrei;
            break;

            case CARABOX_CHASSISTYP_TT1:
            case CARABOX_CHASSISTYP_TT2:
            cLaplTT.SetValues(0.0, 1.0/pCD->dResFreqMounted);
            dQFactTTCB  = pCD->dQFact;
            dummyF_Frei = pCD->dResFreqFrei;
            dummyF_CB   = pCD->dResFreqMounted;
            break;

            case CARABOX_CHASSISTYP_MT1: cLaplMT1.SetValues(0.0, 1.0/pCD->dResFreqMounted); break;
            case CARABOX_CHASSISTYP_MT2: cLaplMT2.SetValues(0.0, 1.0/pCD->dResFreqMounted); break;
            case CARABOX_CHASSISTYP_MT3: cLaplMT3.SetValues(0.0, 1.0/pCD->dResFreqMounted); break;
            case CARABOX_CHASSISTYP_HT:  cLaplHT.SetValues( 0.0, 1.0/pCD->dResFreqMounted); break;
         }
      }

      if (dummyF_Frei_PM > 0.001)
      {
         for(j=1; j<CALCNOOFFREQ; j++)
         {
            if (dummyF_Frei_PM < gm_dFreq[j]) break;
         }
         if (fabs(gm_dFreq[j] - dummyF_Frei_PM) > fabs(dummyF_Frei_PM - gm_dFreq[j-1])) j--;
         jPM = j;
      }

      if ((dummyF_Frei != 0.0) && (dummyF_CB != 0.0))
      {
         dFactBR    = dummyF_CB*dummyF_CB/(dummyF_CB*dummyF_CB - dummyF_Frei*dummyF_Frei);
         dFactPM    = 1.0 - dummyF_Frei_PM*dummyF_Frei_PM/(dummyF_BR*dummyF_BR);
         dFactTT_BR = 1.0 - dummyF_Frei*dummyF_Frei/(dummyF_CB*dummyF_CB);
      }

      for (pCD=(ChassisData*)m_ChassisData.GetFirst(); pCD!=NULL; pCD=(ChassisData*)m_ChassisData.GetNext())
      {
         if (pCD->IsDummy())           break;
         if (!pCD->CalcTransferFunc()) continue;
         if (!pCD->CalcFilter())       continue;
         if (pCD->IsSphere())
         {
            if (pSphereFilter==NULL) pSphereFilter = new FilterBox;
            ZeroMemory(pSphereFilter, sizeof(FilterBox));
            pSphereFilter->dTPFreq  = CSCHALL*1000.0/(pCD->dEffDiameter*3.0); // Lambda / 3
            pSphereFilter->nTPOrder = 1;     // 1.Ordnung
            pSphereFilter->nTPType  = 0;     // Butterworth
            CalcFilterBox(pSphereFilter, &gm_dFreq[0]);
         }
         for(j=0; j<CALCNOOFFREQ; j++)
         {
            if(j <= FRQ_INDEX_20480_HZ) pCD->cAmplAccel[j].SetValues(1.0, 0.0);
            else                        pCD->cAmplAccel[j].SetValues(0.0, 0.0);

            switch (pCD->nChassisTyp)
            {                                
               case CARABOX_CHASSISTYP_BR:                     // Baßreflex
               case CARABOX_CHASSISTYP_PM:                     // Passivmembran
               case CARABOX_CHASSISTYP_TL:                     // Transmissionline
               cLaplace = cLaplTT*gm_dFreq[j];                 // ohne Filter, ohne Induktivität 
               cDummy1  = 1.0 + cLaplace/dQFactTTCB + cLaplace*cLaplace;
               cLaplace = cLaplBR*gm_dFreq[j];
               cDummy2  = 1.0 + cLaplace/dQFactBR + cLaplace*cLaplace;

               pCD->cAmplAccel[j] *= cLaplace*cLaplace/(dFactPM - dFactBR*cDummy1*cDummy2);
               // zur Normalisierung der Beschleunigung (bei 0Hz gleich mit TT)
               pCD->cAmplAccel[j] *= fabs(1.0 - dFactBR)*dummyF_BR*dummyF_BR/(dummyF_Frei*dummyF_Frei);
               // BR-/TL 'fühlen' das TT-Filter
               if (j==jPM) cAmpljPM_PM = pCD->cAmplAccel[j];
               if (m_Cabinet.nCountTT2 > 0) cDummy3 = m_BasicData.sTT2.cFilterBox[j];
               else                         cDummy3 = m_BasicData.sTT1.cFilterBox[j];
               break;

               case CARABOX_CHASSISTYP_TT1:
               if ((m_nLSType & DIPOLE_FLAT) ||                // Dipol Flächenstrahler
                   (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))// oder echte Dipole
               {
                  double dCorr = CSCHALL*2000.0/pCD->dEffHoehe;// Lambda / 2 der Schallwandhöhe
                  dCorr = pow(gm_dFreq[j]/dCorr+1.0, 0.25);    // corrfaktor = (f / fcorr + 1) ^ 1/4
                  cLaplace = cLaplTT*gm_dFreq[j];
                  cDummy1  = 1.0 + cLaplace/pCD->dQFact + cLaplace*cLaplace;
                  pCD->cAmplAccel[j] *= cLaplace*cLaplace/cDummy1*dCorr;
               }
               else if (m_BasicData.nBoxTyp == CARABOX_BOXTYP_CLOS)
               {
                  cLaplace = cLaplTT*gm_dFreq[j];
                  cDummy1  = 1.0 + cLaplace/pCD->dQFact + cLaplace*cLaplace;
                  pCD->cAmplAccel[j] *= cLaplace*cLaplace/cDummy1;
               }
               else                                            // TT in BR/TL/PM-Boxen
               {
                  cLaplace = cLaplBR*gm_dFreq[j];
                  cDummy1  = 1.0 + cLaplace/dQFactBR + cLaplace*cLaplace;
                  cLaplace = cLaplTT*gm_dFreq[j];
                  cDummy2  = 1.0 + cLaplace/dQFactTTCB + cLaplace*cLaplace;
                  pCD->cAmplAccel[j] *= cLaplace*cLaplace/(cDummy2 - dFactPM*dFactTT_BR/cDummy1);
                  if (j==jPM) cAmpljPM_TT1 = pCD->cAmplAccel[j];
               }
               cDummy3 = m_BasicData.sTT1.cFilterBox[j];       // Filter-Netzwerk
               break;

               case CARABOX_CHASSISTYP_TT2:
               if( m_BasicData.nBoxTyp == CARABOX_BOXTYP_CLOS )
               {
                  cLaplace = cLaplTT*gm_dFreq[j];
                  cDummy1 = 1.0 + cLaplace/pCD->dQFact + cLaplace*cLaplace;
                  pCD->cAmplAccel[j] *= cLaplace*cLaplace/cDummy1;
               }
               else                                            // TT in BR/TL/PM-Boxen
               {
                  cLaplace = cLaplBR*gm_dFreq[j];
                  cDummy1  = 1.0 + cLaplace/dQFactBR + cLaplace*cLaplace;
                  cLaplace = cLaplTT*gm_dFreq[j];
                  cDummy2  = 1.0 + cLaplace/dQFactTTCB + cLaplace*cLaplace;
                  pCD->cAmplAccel[j] *= cLaplace*cLaplace/(cDummy2 - dFactPM*dFactTT_BR/cDummy1);
                  if (j==jPM) cAmpljPM_TT2 = pCD->cAmplAccel[j];
               }
               cDummy3 = m_BasicData.sTT2.cFilterBox[j];       // Filter-Netzwerk
               break;

               case CARABOX_CHASSISTYP_MT1:
               cLaplace = cLaplMT1*gm_dFreq[j];
               cDummy1 = 1.0 + cLaplace/pCD->dQFact + cLaplace*cLaplace;
               pCD->cAmplAccel[j] *= cLaplace*cLaplace/cDummy1;
               cDummy3 = m_BasicData.sMT1.cFilterBox[j];       // Filter-Netzwerk
               break;

               case CARABOX_CHASSISTYP_MT2:
               cLaplace = cLaplMT2*gm_dFreq[j];
               cDummy1 = 1.0 + cLaplace/pCD->dQFact + cLaplace*cLaplace;
               if ((m_nLSType & DIPOLE_FLAT) ||                // Dipol Flächenstrahler
                   (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))// oder echte Dipole
               {
                  double dCorr = CSCHALL*2000.0/pCD->dEffHoehe;// Lambda / 2 der Schallwandhöhe
                  dCorr = pow(gm_dFreq[j]/dCorr+1.0, 0.25);     // corrfaktor = (f / fcorr + 1) ^ 1/4
                  pCD->cAmplAccel[j] *= cLaplace*cLaplace/cDummy1 * dCorr;
               }
               else
               {
                  pCD->cAmplAccel[j] *= cLaplace*cLaplace/cDummy1;
               }
               cDummy3 = m_BasicData.sMT2.cFilterBox[j];       // Filter-Netzwerk
               break;

               case CARABOX_CHASSISTYP_MT3:
               cLaplace = cLaplMT3*gm_dFreq[j];
               cDummy1 = 1.0 + cLaplace/pCD->dQFact + cLaplace*cLaplace;
               pCD->cAmplAccel[j] *= cLaplace*cLaplace/cDummy1;
               cDummy3 = m_BasicData.sMT3.cFilterBox[j];       // Filter-Netzwerk
               break;

               case CARABOX_CHASSISTYP_HT:
               cLaplace = cLaplHT*gm_dFreq[j];
               cDummy1 = 1.0 + cLaplace/pCD->dQFact + cLaplace*cLaplace;
               if ((m_nLSType & DIPOLE_FLAT) ||                // Dipol Flächenstrahler
                   (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))// oder echte Dipole
               {
                  double dCorr = CSCHALL*2000.0/pCD->dEffHoehe;// Lambda / 2 der Schallwandhöhe
                  dCorr = pow(gm_dFreq[j]/dCorr+1.0, 0.25);     // corrfaktor = (f / fcorr + 1) ^ 1/4
                  pCD->cAmplAccel[j] *= cLaplace*cLaplace/cDummy1 * dCorr;
               }
               else
               {
                  pCD->cAmplAccel[j] *= cLaplace*cLaplace/cDummy1;
               }
               cDummy3 = m_BasicData.sHT.cFilterBox[j];        // Filter-Netzwerk
               break;
            }

            if ((pCD->nChassisTyp != CARABOX_CHASSISTYP_BR) && // Kein Baßreflex
                (pCD->nChassisTyp != CARABOX_CHASSISTYP_TL) && // Keine Transmissionline
                (pCD->nChassisTyp != CARABOX_CHASSISTYP_PM) )  // keine Passivmembran
            {                                                  // Impedanz
               dTemp   = 0.002*M_PI*gm_dFreq[j]/dRdc;
               cDummy4.SetValues(1.0 + pow(dTemp*pCD->dInduReal, pCD->dInduExpReal),
                                       pow(dTemp*pCD->dInduImag, pCD->dInduExpImag));
               pCD->cAmplAccel[j] *= cDummy3/cDummy4;
            }
            else
               pCD->cAmplAccel[j] *= cDummy3;
            if (pCD->IsSphere())
            {
               pCD->cAmplAccel[j] /= pSphereFilter->cFilterBox[j];
            }
            if ((m_nLSType & DIPOLE_FLAT) ||                   // Dipol Flächenstrahler
                (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))// oder echte Dipole
            {
               int nC;
               ChassisData *pCDC;
               for (nC=1; nC<pCD->GetNoOfCoupled(); nC++)
               {
                  pCDC = pCD->GetCoupled(nC);
                  ASSERT(pCDC != NULL);
                  if (!pCDC->CalcTransferFunc())               // wenn er sie nicht selbst berechnet
                     pCDC->cAmplAccel[j] = pCD->cAmplAccel[j]; // kopieren
               }
            }
         }
      }
      if (jPM != -1)
      {
         double dFactPM;
         Complex cNull(0.0, 0.0);
         if (cAmpljPM_TT2 != cNull)
            dFactPM = Betrag(cAmpljPM_TT2/cAmpljPM_PM);
         else 
            dFactPM = Betrag(cAmpljPM_TT1/cAmpljPM_PM);

                  // Frequenzabhängigkeit der Transferfunktion
         for (pCD=(ChassisData*)m_ChassisData.GetFirst(); pCD!=NULL; pCD=(ChassisData*)m_ChassisData.GetNext())
         {
            if (!pCD->CalcTransferFunc()) continue;
            if (pCD->nChassisTyp == CARABOX_CHASSISTYP_PM)     // Passivmembran
            {
               for(j=0; j<CALCNOOFFREQ; j++)
                  pCD->cAmplAccel[j] *= dFactPM;
            }
         }
      }
   }
   catch (DWORD nError)
   {
      char text[128];
      wsprintf(text, "CalcTransferFunc Error : %0x", nError);
      GetActivePage()->MessageBox((long)text, IDS_PROPSHCAPTION, MB_OK|MB_ICONERROR|MBU_TEXTSTRING);
//      REPORT("CalcTransferFunc Error : %0x", nError);
      nReturn = 0;
   }
   if (pSphereFilter) delete pSphereFilter;

   if (pFB)
   {
      pFB->nHPType = nHPType;                         // Filtertyp rücksichern
      pFB->nTPType = nTPType;
   }
   return (nReturn);
}

/********************************************************************/
/*                                                                  */
/*    Berechnet die SP(L)-Übertragungsfunktion der gesamten Box     */
/*       für die auf 0° ausgerichtete Box im Abstand von 1m         */
/*    und bestimmt heirbei die notwendigen Anpassungsfaktoren.      */
/*                                                                  */
/* Annahme: alle Chassis einer Chassis-Gruppe (BR/TL, TT1/TT2, MT1  */
/*    MT2, MT3, HT) spielen gleich laut, auch wenn ihre Membran-    */
/*    flächen unterschiedlich sind (!!). Ihre Übertragungs-Grund-   */
/*    funktionen (Beschleunigung) sind gleich, auch ihre Filter     */
/*    (Ausnahme: TT1 <-> TT2) sind gleich, ihre Induktivitäten aber */
/*    evtl. unterschiedlich.                                        */
/*    Zu diesem Zweck werden die unterschiedlichen Lautstärken der  */
/*    zugehörigen Richtcharakteristikdaten (wenn unterschiedliche   */
/*    Flächen) durch einen Chassis-bezogenen Anpassungsfaktor inner-*/
/*    halb einer Chassis-Gruppe einander angepaßt (dCorrectAmpl),   */
/*    indem die Werte bei 5Hz als Bezugspunkt genommen werden.      */
/*    Die Anpassung der Gesamtlautstärke einer Chassis-Gruppe an die*/
/*    erforderliche Empfindlichkeit (dB/W/m) er folgt durch einen   */
/*    gruppenspezifischen Anpassungsfaktor (z.B. dCorrectAmplMT1).  */
/* Besonderheit: Eine Besonderheit stellt die Anpassung der Chassis-*/
/*    gruppe BR/TL dar. dCorrectAmpl der einzelnen BR/TL wird so    */
/*    eingestellt, daß nicht nur alle BR/TL untereinander gleich    */
/*    laut sind, sondern auch alle BR/TL zusammen bei 5 Hz so laut  */
/*    sind wie alle TT1/2 zusammen. Zur Anpassung an die Boxenemp-  */
/*    findlichkeit wird an die BR/TL als Gruppe der Anpassungsfaktor*/
/*    dCorrectAmplTT12 (!!) heranmultipliziert.                     */
/*                                                                  */
/*   Der übergebene Array pSPThetaPhiFreq bezieht sich auf die in   */
/*   CALBASTA definierten Winkel Theta (31: 90, 84 .. -90) und      */
/*   Phi (36: 0, 10, .. 350), Anzahl der Frequenzen = 118           */
/*                                                                  */
/********************************************************************/
int CBoxPropertySheet::CalcZeroSPL()
{
   int      nReturn = 1;
   int      k, nTheta1, nTheta2, nPhi1, nPhi2;
   int      index, subindex, index0BRTL = -1, index0TT12 = -1, indexCoupled = -1;
   int      nOffsetIndexChassis = 0;
   double   dThetaOrg, dPhiOrg;
   double   dSinTheta, dCosTheta, dSinPhi, dCosPhi;
   CVector  vDeltaZeroPos, vDZPn, vWallNorm, vDeltaZeroPosNormCH, vX1;
   double   dBetragDeltaZeroPos;
   double   dSUBetragSP_BRTL = 0, dSUBetragSP_TT12 = 0;
   double   dSUBetragSP_MT1  = 0, dSUBetragSP_MT2  = 0, dSUBetragSP_MT3 = 0, dSUBetragSP_HT = 0;
   double   dTemp, dThetaInt, dPhiInt;
   Complex  ***pSPThetaPhiFreq = NULL;
   Complex  cTemp, cPhi;
   
   ChassisData *pCD, *pCD0BRTL=NULL, *pCD0TT12 = NULL, *pCDsub = NULL, *pCDC = NULL;

   try
   {
      index = CARABOX_CABWALL_FRONT;
      if      (m_Cabinet.m_dwFlags & ZERO_DEG_REF_LEFT ) index = CARABOX_CABWALL_LEFT;
      else if (m_Cabinet.m_dwFlags & ZERO_DEG_REF_BACK ) index = CARABOX_CABWALL_BACK;
      else if (m_Cabinet.m_dwFlags & ZERO_DEG_REF_RIGHT) index = CARABOX_CABWALL_RIGHT;
      m_Cabinet.GetCabinetWall(index, NULL, &vWallNorm);
      vWallNorm.SetZ() = 0.0;
      vX1 = 0.001*m_Cabinet.vPosRef + Norm(vWallNorm) * m_dDistance;

      for (index=0; index<m_Cabinet.nCountAllChassis; )
      {
         pCD = (ChassisData*) m_ChassisData.GetAt(index);      // Hauptchassis
         if ((m_nLSType & DIPOLE_FLAT) ||                      // Dipol Flächenstrahler
             (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))
         {
            if (indexCoupled == -1) indexCoupled = pCD->GetFirstDummy();
            if (pCD == NULL) break;
            pCD  = pCD->GetCoupled(indexCoupled);              // aber die Gekoppelten
            pCDC = pCD;                                        // gekoppeltes Chassis merken
            if (pCD == NULL)                                   // kein weiteres gekoppeltes gefunden
            {
               index++;                                        // nächstes Hauptchassis nehmen
               indexCoupled = -1;                              // mit dem ersten gekoppelten wieder beginnen
               continue;
            }
         }

         vDeltaZeroPos = vX1 - 0.001*pCD->vPosChassis; // in m

         dBetragDeltaZeroPos = Betrag(vDeltaZeroPos);
         vDZPn   = vDeltaZeroPos/dBetragDeltaZeroPos;

         m_Cabinet.GetCabinetWall(pCD->nCabWall, NULL, &vWallNorm);
         dTemp     = atan2(Vy(vWallNorm), Vx(vWallNorm));
         SinCos(dTemp, dSinPhi, dCosPhi);
 
         dSinTheta = Vz(vWallNorm);
         dTemp     = asin(dSinTheta);
         dCosTheta = cos(dTemp);

         vDeltaZeroPosNormCH.SetX() =  Vx(vDZPn)*dCosTheta*dCosPhi + Vy(vDZPn)*dCosTheta*dSinPhi + Vz(vDZPn)*dSinTheta;
         vDeltaZeroPosNormCH.SetY() = -Vx(vDZPn)*dSinPhi           + Vy(vDZPn)*dCosPhi;
         vDeltaZeroPosNormCH.SetZ() = -Vx(vDZPn)*dSinTheta*dCosPhi - Vy(vDZPn)*dSinTheta*dSinPhi + Vz(vDZPn)*dCosTheta;

         dThetaOrg = asin(Vz(vDeltaZeroPosNormCH))*M180_D_PI;
         dPhiOrg   = atan2(Vy(vDeltaZeroPosNormCH), Vx(vDeltaZeroPosNormCH))*M180_D_PI;

         if(dPhiOrg   <   0.0) dPhiOrg  += 360.0;
         if(dPhiOrg   > 360.0) dPhiOrg   = 360.0;
         if(dThetaOrg < -90.0) dThetaOrg = -90.0;
         if(dThetaOrg >  90.0) dThetaOrg =  90.0;

         GetThetaPhiIndex(dThetaOrg, dPhiOrg, nTheta1, nTheta2, dThetaInt, nPhi1, nPhi2, dPhiInt);
         pSPThetaPhiFreq = ReadSP_Directiv(pCD->szDirectFileName);// Richtcharakteristikdaten einlesen
                                                               
         pCD->dCorrectAmpl = Betrag(pSPThetaPhiFreq[15][0][0]);// den Flächen-Anpassungsfaktor zunächst mit SP[0°][0°][5Hz] belegen
                                                               // zunächst Originalwert für 20m Abstand
                                                               // 0° Frequenzgang für Normmesspunkt (1m vor der Gehäusefront)
         dTemp = 2.0*M_PI*dBetragDeltaZeroPos/CSCHALL;
         dBetragDeltaZeroPos = 20.0 / dBetragDeltaZeroPos;
         if (pCD->nPolung == -1) dBetragDeltaZeroPos = -dBetragDeltaZeroPos;
         for( k = 0; k < CALCNOOFFREQ; k++ )
         {  // Formel !!
            SinCos(gm_dFreq[k]*dTemp, cPhi.imagteil, cPhi.realteil);// SinCos(gm_dFreq[k]*dTemp, dSinPhi, dCosPhi);
            cPhi.imagteil = -cPhi.imagteil;                    // -dSinPhi
            pCD->cSPZeroStandard[k] = GetSPLDirectiv(pSPThetaPhiFreq, nTheta1, nTheta2, dThetaInt, nPhi1, nPhi2, dPhiInt, k)*dBetragDeltaZeroPos*
                                      cPhi;                    // Complex(dCosPhi, -dSinPhi);
         }

         if (pCD->GetNoOfCoupled()) //m_nLSType & DIPOLE_FLAT)                          // Dipol Flächenstrahler
         {
            pCD = (ChassisData*) m_ChassisData.GetAt(index);   // Hauptchassis nehmen
            int nCoupled = pCD->GetNoOfCoupled();              // Anzahl der gekoppelten
            if (indexCoupled == pCD->GetFirstDummy())          // erstes gekoppeltes
            {
               for( k=0; k<CALCNOOFFREQ; k++)                  // Parameter übergeben
                  pCD->cSPZeroStandard[k] = pCDC->cSPZeroStandard[k];
               pCD->dCorrectAmpl = pCDC->dCorrectAmpl;
            }
            else                                               // weitere gekoppelte
            {
               for( k=0; k<CALCNOOFFREQ; k++)                  // Parameter addieren
                  pCD->cSPZeroStandard[k] += pCDC->cSPZeroStandard[k];
               pCD->dCorrectAmpl += pCDC->dCorrectAmpl;
            }
            if (indexCoupled++ != (nCoupled-1))                // letztes gekoppeltes
               continue;
         }

         switch (pCD->nChassisTyp)
         {
            case CARABOX_CHASSISTYP_BR:                        // Baßreflex
            case CARABOX_CHASSISTYP_TL:                        // Transmissionline
            case CARABOX_CHASSISTYP_PM:                        // Passivmembran
            if (index == nOffsetIndexChassis)
            {
               index0BRTL = index;                             // Merken den Index des 1. Chassis dieser Gruppe
               pCD0BRTL   = pCD;
               dSUBetragSP_BRTL = pCD->dCorrectAmpl*(m_Cabinet.nCountPM+m_Cabinet.nCountBR+m_Cabinet.nCountTL);
            }
            else                                               // Normierungsfaktor innerhalb der Gruppe, um gleiche Lautstärke zu erhalten
            {
               ASSERT(pCD0BRTL != NULL);
               pCD->dCorrectAmpl = pCD0BRTL->dCorrectAmpl/pCD->dCorrectAmpl;
            }
                                                               // das letzte Chassis vom Typ BR/TL/PM
            if (index == nOffsetIndexChassis + (m_Cabinet.nCountPM+m_Cabinet.nCountBR+m_Cabinet.nCountTL-1) )
            {
               ASSERT(pCD0BRTL != NULL);
               pCD0BRTL->dCorrectAmpl = 1.0;                   // 1. Chassis bestimmt die Lautstärke innerhalb der Gruppe
               nOffsetIndexChassis += (m_Cabinet.nCountPM+m_Cabinet.nCountBR+m_Cabinet.nCountTL);
            }
            break;

            case CARABOX_CHASSISTYP_TT1: case CARABOX_CHASSISTYP_TT2:
            if( index == nOffsetIndexChassis )                 // Voraussetzung: BR/TL/PM alle vor den TT1/TT2 im Chassisarray
            {
               index0TT12 = index;                             // Merken den Index des 1. Chassis dieser Gruppe
               pCD0TT12   = pCD;
               dSUBetragSP_TT12 = pCD->dCorrectAmpl*(m_Cabinet.nCountTT1+m_Cabinet.nCountTT2);
            }
            else                                               // Normierungsfaktor innerhalb der Gruppe, um gleiche Lautstärke zu erhalten
            {
               ASSERT(pCD0TT12!=NULL);
               pCD->dCorrectAmpl = pCD0TT12->dCorrectAmpl/pCD->dCorrectAmpl;
            }
                                                               // das letzte Chassis vom Typ TT1/2
            if (index == nOffsetIndexChassis + (m_Cabinet.nCountTT1+m_Cabinet.nCountTT2-1) )
            {
               ASSERT(pCD0TT12!=NULL);
               pCD0TT12->dCorrectAmpl = 1.0;                   // 1. Chassis bestimmt die Lautstärke innerhalb der Gruppe
               if (pCD0TT12->GetNoOfCoupled()) // m_nLSType & DIPOLE_FLAT)                    // Dipol Flächenstrahler
                  pCD0TT12->SetCorrAmplToSlave();
               nOffsetIndexChassis += (m_Cabinet.nCountTT1+m_Cabinet.nCountTT2);
               int nsubLast = index0BRTL+m_Cabinet.nCountPM+m_Cabinet.nCountBR+m_Cabinet.nCountTL;
               for (subindex=index0BRTL; subindex<nsubLast; subindex++)
               {                                               // Anpassen der BR/TL/PM-Lautstärken an die der TT
                  pCDsub = (ChassisData*) m_ChassisData.GetAt(subindex);
                  ASSERT(pCDsub!=NULL);
                  ASSERT(dSUBetragSP_TT12!=0.0);
                  ASSERT(dSUBetragSP_BRTL!=0.0);
                  if (pCDsub) pCDsub->dCorrectAmpl *= dSUBetragSP_TT12/dSUBetragSP_BRTL;
               }
            }
            break;

            case CARABOX_CHASSISTYP_MT1:
            if(index == nOffsetIndexChassis)
            {
               subindex = index;                    // Merken den Index des 1. Chassis dieser Gruppe
               pCDsub   = pCD;                      // das 1. Chassis bestimmt die 'Norm'-Lautstärke, alle Chassis gleich laut
               dSUBetragSP_MT1 = pCD->dCorrectAmpl*m_Cabinet.nCountMT1;
            }
            else                                    // Normierungsfaktor innerhalb der Gruppe, um gleiche Lautstärke zu erhalten
            {
               ASSERT((pCDsub!=NULL) && (pCDsub->nChassisTyp==CARABOX_CHASSISTYP_MT1));
               pCD->dCorrectAmpl  = pCDsub->dCorrectAmpl/pCD->dCorrectAmpl;
               ASSERT(dSUBetragSP_TT12!=0.0);
               ASSERT(dSUBetragSP_MT1!=0.0);
               pCD->dCorrectAmpl *= dSUBetragSP_TT12/dSUBetragSP_MT1; // alle Lautstärken an TT anpassen
            }
                                                      // das letzte Chassis vom Typ MT1
            if (index == nOffsetIndexChassis + (m_Cabinet.nCountMT1-1))
            {
               ASSERT((pCDsub!=NULL) && (pCDsub->nChassisTyp==CARABOX_CHASSISTYP_MT1));
               pCDsub->dCorrectAmpl  = 1.0;           // 1. Chassis bestimmt die Lautstärke innerhalb der Gruppe
               ASSERT(dSUBetragSP_TT12!=0.0);
               ASSERT(dSUBetragSP_MT1!=0.0);
               pCDsub->dCorrectAmpl *= dSUBetragSP_TT12/dSUBetragSP_MT1; // alle Lautstärken an TT anpassen
               if (pCDsub->GetNoOfCoupled()) // m_nLSType & DIPOLE_FLAT)           // Dipol Flächenstrahler
                  pCDsub->SetCorrAmplToSlave();

               nOffsetIndexChassis += m_Cabinet.nCountMT1;
            }
            break;

            case CARABOX_CHASSISTYP_MT2:
            if( index == nOffsetIndexChassis )
            {
               subindex = index;                    // Merken den Index des 1. Chassis dieser Gruppe
               pCDsub   = pCD;                      // das 1. Chassis bestimmt die 'Norm'-Lautstärke, alle Chassis gleich laut
               dSUBetragSP_MT2 = pCD->dCorrectAmpl*m_Cabinet.nCountMT2;
            }
            else                                    // Normierungsfaktor innerhalb der Gruppe, um gleiche Lautstärke zu erhalten
            {
               ASSERT((pCDsub!=NULL) && (pCDsub->nChassisTyp==CARABOX_CHASSISTYP_MT2));
               pCD->dCorrectAmpl  = pCDsub->dCorrectAmpl/pCD->dCorrectAmpl;
               ASSERT(dSUBetragSP_TT12!=0.0);
               ASSERT(dSUBetragSP_MT2 !=0.0);
               pCD->dCorrectAmpl *= dSUBetragSP_TT12/dSUBetragSP_MT2; // alle Lautstärken an TT anpassen
            }
                                                    // das letzte Chassis vom Typ MT2
            if( index == nOffsetIndexChassis + (m_Cabinet.nCountMT2-1) )
            {
               ASSERT((pCDsub!=NULL) && (pCDsub->nChassisTyp==CARABOX_CHASSISTYP_MT2));
               pCDsub->dCorrectAmpl  = 1.0; // 1. Chassis bestimmt die Lautstärke innerhalb der Gruppe
               ASSERT(dSUBetragSP_TT12!=0.0);
               ASSERT(dSUBetragSP_MT2 !=0.0);
               pCDsub->dCorrectAmpl *= dSUBetragSP_TT12/dSUBetragSP_MT2; // alle Lautstärken an TT anpassen
               if (pCDsub->GetNoOfCoupled()) // m_nLSType & DIPOLE_FLAT)           // Dipol Flächenstrahler
                  pCDsub->SetCorrAmplToSlave();

               nOffsetIndexChassis += m_Cabinet.nCountMT2;
            }
            break;

            case CARABOX_CHASSISTYP_MT3:
            if( index == nOffsetIndexChassis )
            {
               subindex = index;                    // Merken den Index des 1. Chassis dieser Gruppe
               pCDsub   = pCD;                                 // das 1. Chassis bestimmt die 'Norm'-Lautstärke, alle Chassis gleich laut
               dSUBetragSP_MT3 = pCD->dCorrectAmpl*m_Cabinet.nCountMT3;
            }
            else                                               // Normierungsfaktor innerhalb der Gruppe, um gleiche Lautstärke zu erhalten
            {
               ASSERT((pCDsub!=NULL) && (pCDsub->nChassisTyp==CARABOX_CHASSISTYP_MT3));
               pCD->dCorrectAmpl  = pCDsub->dCorrectAmpl/pCD->dCorrectAmpl;
               ASSERT(dSUBetragSP_TT12!=0.0);
               ASSERT(dSUBetragSP_MT3 !=0.0);
               pCD->dCorrectAmpl *= dSUBetragSP_TT12/dSUBetragSP_MT3;// alle Lautstärken an TT anpassen
            }
                                                               // das letzte Chassis vom Typ MT3
            if( index == nOffsetIndexChassis + (m_Cabinet.nCountMT3-1) )
            {
               ASSERT((pCDsub!=NULL) && (pCDsub->nChassisTyp==CARABOX_CHASSISTYP_MT3));
               pCDsub->dCorrectAmpl  = 1.0;                    // 1. Chassis bestimmt die Lautstärke innerhalb der Gruppe
               ASSERT(dSUBetragSP_TT12!=0.0);
               ASSERT(dSUBetragSP_MT3 !=0.0);
               pCDsub->dCorrectAmpl *= dSUBetragSP_TT12/dSUBetragSP_MT3; // alle Lautstärken an TT anpassen
               if (pCDsub->GetNoOfCoupled()) // m_nLSType & DIPOLE_FLAT)           // Dipol Flächenstrahler
                  pCDsub->SetCorrAmplToSlave();

               nOffsetIndexChassis += m_Cabinet.nCountMT3;
            }
            break;

            case CARABOX_CHASSISTYP_HT:
            if( index == nOffsetIndexChassis )
            {
               subindex = index;                               // Merken den Index des 1. Chassis dieser Gruppe
               pCDsub   = pCD;                                 // das 1. Chassis bestimmt die 'Norm'-Lautstärke, alle Chassis gleich laut
               dSUBetragSP_HT = pCD->dCorrectAmpl*m_Cabinet.nCountHT;
            }
            else                                               // Normierungsfaktor innerhalb der Gruppe, um gleiche Lautstärke zu erhalten
            {
               ASSERT((pCDsub!=NULL) && (pCDsub->nChassisTyp==CARABOX_CHASSISTYP_HT));
               pCD->dCorrectAmpl  = pCDsub->dCorrectAmpl/pCD->dCorrectAmpl;
               if (dSUBetragSP_TT12 != 0.0)
                  pCD->dCorrectAmpl *= dSUBetragSP_TT12/dSUBetragSP_HT; // alle Lautstärken an TT anpassen
            }
                                                               // das letzte Chassis vom Typ HT
            if( index == nOffsetIndexChassis + (m_Cabinet.nCountHT-1))
            {
               ASSERT((pCDsub!=NULL) && (pCDsub->nChassisTyp==CARABOX_CHASSISTYP_HT));
               pCDsub->dCorrectAmpl  = 1.0;                    // 1. Chassis bestimmt die Lautstärke innerhalb der Gruppe
               if (dSUBetragSP_TT12 != 0.0) pCDsub->dCorrectAmpl *= dSUBetragSP_TT12/dSUBetragSP_HT; // alle Lautstärken an TT anpassen
               if (pCDsub->GetNoOfCoupled()) // m_nLSType & DIPOLE_FLAT)           // Dipol Flächenstrahler
                  pCDsub->SetCorrAmplToSlave();
               nOffsetIndexChassis  += m_Cabinet.nCountHT;
            }
         }
         if ((m_nLSType & DIPOLE_FLAT) ||                      // Dipol Flächenstrahler
             (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))
         {
            pCD->SetCorrAmplToSlave();
         }
         else index++;
      }
      if (m_nFlagChanged & CARABOX_ADOPT_EFFIC)
      {
         if (!CalcInThread(CARABOX_ADOPT_EFFIC))
            throw (DWORD) 0;                                   // Abbruch ist kein echter Fehler
//         AdaptEffic();
      }
      else
         GetDeltaEffic(NULL, 0.0);
   }
   catch (DWORD nError)
   {
      if (nError)
      {
         char text[128];
         wsprintf(text, "CalcZeroSPL Error %0x", nError);
         GetActivePage()->MessageBox((long)text, IDS_PROPSHCAPTION, MB_OK|MB_ICONERROR|MBU_TEXTSTRING);
//      REPORT("CalcZeroSPL Error %0x", nError);
      }
      nReturn = 0;
   }

   return (nReturn);
}

/********************************************************************/
/*                                                                  */
/*          Paßt die Chassis-Lautstärken an die gewünschte          */
/*                    Boxen-Empfindlichkeit an                      */
/*                                                                  */
/********************************************************************/
void CBoxPropertySheet::AdaptEffic()
{
   const int nMaxTrials = 1000000;
   int      nUpdate = 0x000003ff;
   int      nCount  = 0;
   enum     ChassisIndex { nTT1=0, nMT1, nMT2, nMT3, nHT, nOver};
   union 
   {
      int          nk;
      ChassisIndex ci;
   } k, n;

   double   dTargetEffic, dDeltaEfficLast, dDeltaEfficNew, dSchwelle;
   double   dAddCorrect[5], dDeltaCorrect[5];
   double   dGradient = 1, dTime = 0;
   clock_t  time   = 0;
   int      nTimes = 0;
   int      nAddSubCorrect[5];
   int      nCountUnderflow[5] = {0,0,0,0,0};
   char     szText[128];
   char     szFormat[128];

   m_BasicData.dCorrectAmplTT12 = 1.0;                         // Anfangswerte initialisieren
   m_BasicData.dCorrectAmplMT1  = 1.0;
   m_BasicData.dCorrectAmplMT2  = 1.0;
   m_BasicData.dCorrectAmplMT3  = 1.0;
   m_BasicData.dCorrectAmplHT   = 1.0;
   dAddCorrect[nTT1]    = 0.1;
   dAddCorrect[nMT1]    = 0.1;
   dAddCorrect[nMT2]    = 0.1;
   dAddCorrect[nMT3]    = 0.1;
   dAddCorrect[nHT]     = 0.1;
   nAddSubCorrect[nTT1] = 1;                                   // 1 -> addieren, -1 -> subtrahieren
   nAddSubCorrect[nMT1] = 1;
   nAddSubCorrect[nMT2] = 1;
   nAddSubCorrect[nMT3] = 1;
   nAddSubCorrect[nHT]  = 1;

   dSchwelle = 0.001;                                          // Abbruchschwelle

   dTargetEffic = P0*pow(10.0, m_BasicData.dEffectivDBWm/20.0);// Zielwirkungsgrad ermitteln
   dTargetEffic /= m_dDistance;                                // auch Abhängig vom Abstand

   dDeltaEfficNew = GetDeltaEffic(dAddCorrect, dTargetEffic);  // Differenz bilden

   dDeltaCorrect[nTT1] = dDeltaEfficNew*0.1;                   // Anfangskorrekturwert ist abhängig von der Differenz
   dDeltaCorrect[nMT1] = dDeltaCorrect[nTT1];
   dDeltaCorrect[nMT2] = dDeltaCorrect[nTT1];
   dDeltaCorrect[nMT3] = dDeltaCorrect[nTT1];
   dDeltaCorrect[nHT]  = dDeltaCorrect[nTT1];

   if (m_Cabinet.nCountTT1 == 0)                               // Korrekturwerte auf die Abbruchschwelle setzen,
   {                                                           // wenn kein TT vorhanden
      ASSERT(m_Cabinet.nCountTT2 == 0);
      dDeltaCorrect[nTT1] = dSchwelle;
   }
   if (m_Cabinet.nCountMT1 == 0) dDeltaCorrect[nMT1] = dSchwelle;
   if (m_Cabinet.nCountMT2 == 0) dDeltaCorrect[nMT2] = dSchwelle;
   if (m_Cabinet.nCountMT3 == 0) dDeltaCorrect[nMT3] = dSchwelle;
   if (m_Cabinet.nCountHT  == 0) dDeltaCorrect[nHT]  = dSchwelle;

   k.ci = nTT1;

   if (m_hwndChildDlg)
   {
      double dTemp = (dDeltaCorrect[nTT1] + dDeltaCorrect[nMT1] + dDeltaCorrect[nMT2] + dDeltaCorrect[nMT3] + dDeltaCorrect[nHT]) * 0.2;
      dGradient = 20 / (dSchwelle - dTemp);
      ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_CNTR, PBM_SETRANGE32, 0, 100);
      ::LoadString(g_hInstance, IDS_OPTIMZE_RESULTS, szText, 128);
      ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_TEXT, WM_SETTEXT, 0, (LPARAM)szText);
      ::LoadString(g_hInstance, IDS_BGD_TIME, szFormat, 128);
      if (g_nUserMode & USERMODE_SURVEY)
      {
         nUpdate = 0x0000000f;
      }
   }

   do
   {
      if ((k.ci == nTT1) && (m_Cabinet.nCountTT1 == 0)) k.nk++;
      if ((k.ci == nMT1) && (m_Cabinet.nCountMT1 == 0)) k.nk++;
      if ((k.ci == nMT2) && (m_Cabinet.nCountMT2 == 0)) k.nk++;
      if ((k.ci == nMT3) && (m_Cabinet.nCountMT3 == 0)) k.nk++;
      if ((k.ci == nHT ) && (m_Cabinet.nCountHT  == 0)) k.ci=nTT1;

      if (nCount++ > nMaxTrials) break;                        // soviele Versuche sollten reichen
      if (m_hwndChildDlg)
      {
         if ((nCount & nUpdate) == nUpdate)
         {
            if (m_nCalcWhat & TERMINATE_THREAD_SAVE) break;
            else if (time == 0) time = clock();
            else
            {
               clock_t temp = clock();
               double  dSeconds = (double)(temp-time) / (double)CLOCKS_PER_SEC;
               time = temp;
               dTime += dSeconds;
               if (++nTimes > 1) dSeconds = dTime / (double)nTimes;
               int nSec = CEtsDiv::Round(dSeconds*((nMaxTrials-nCount) / nUpdate)),
                   nMin = nSec / 60;                           // Minuten
               if (nMin) nSec -= nMin*60;                      // Sekunden
               wsprintf(szText, szFormat, nMin, nSec);
               ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_TIME, WM_SETTEXT, 0, (LPARAM)szText);
            }
         }
      }

      dDeltaEfficLast   = dDeltaEfficNew;                      // alten Wert sichern,
                                                               // Korrekturwert ändern
      dAddCorrect[k.ci] = dAddCorrect[k.ci] + nAddSubCorrect[k.ci]*dDeltaCorrect[k.ci];
      dDeltaEfficNew    = GetDeltaEffic(dAddCorrect, dTargetEffic);// neuen berechnen

      if ((dDeltaEfficNew > dDeltaEfficLast) ||                // Effektivität Neu > Alt oder
          (dAddCorrect[k.ci] < -0.99))                         // ein Chassis wird umgepolt
      {
         nAddSubCorrect[k.ci] *= -1;                           // die Änderungsrichtung umdrehen
         if( dDeltaCorrect[k.ci] > dSchwelle) dDeltaCorrect[k.ci] *= 0.6;
         dAddCorrect[k.ci] = dAddCorrect[k.ci] + nAddSubCorrect[k.ci]*dDeltaCorrect[k.ci];
         if (dAddCorrect[k.ci] < -0.99)                        // Chassis darf nie umgepolt werden
         {
            dAddCorrect[k.ci] = -0.99;                         // warum wird es so leise ?
            if (++nCountUnderflow[k.ci])                       // 1 Chance hat er, dann
            {
               for (n.ci=nTT1; n.ci<nOver; n.nk++)             // anderen Zweig mit Pegel suchen
               {
                  if ((n.nk != k.nk) && (dDeltaCorrect[n.ci] != dSchwelle))
                  {
                     dAddCorrect[k.ci] = dAddCorrect[n.ci]*0.2;// und neuen Startwert setzen
                     dDeltaCorrect[k.ci] *= 2;                 // Korrekturwert erhöhen
                     break;
                  }
               }
               nCountUnderflow[k.ci] = 0;                      // zurück auf 0
            }
         }
         k.nk++;
         if(k.ci == nOver)   k.ci = nTT1;
         if (m_hwndChildDlg)
         {
            double dTemp = (dDeltaCorrect[nTT1] + dDeltaCorrect[nMT1] + dDeltaCorrect[nMT2] + dDeltaCorrect[nMT3] + dDeltaCorrect[nHT]) * dGradient;
            dTemp += 100.0;
            ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_CNTR, PBM_SETPOS, (int)dTemp, 0);
            if (g_nUserMode & USERMODE_SURVEY)
            {
               Sleep(100);
               sprintf(szText, "%.2f, %.2f, %.2f, %.2f, %.2f", dAddCorrect[nTT1], dAddCorrect[nMT1], dAddCorrect[nMT2], dAddCorrect[nMT3], dAddCorrect[nHT]);
               ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_TEXT, WM_SETTEXT, 0, (LPARAM)szText);
            }
         }
      }

   }while( (dDeltaCorrect[nTT1] > dSchwelle)||                    // Tieftonzweig
           (dDeltaCorrect[nMT1] > dSchwelle)||                    // Mitteltonzweig 1
           (dDeltaCorrect[nMT2] > dSchwelle)||                    // Mitteltonzweig 2
           (dDeltaCorrect[nMT3] > dSchwelle)||                    // Mitteltonzweig 3
           (dDeltaCorrect[nHT]  > dSchwelle));                    // Hochtonzweig 

   m_BasicData.dCorrectAmplTT12 = 1.0 + dAddCorrect[nTT1];
   m_BasicData.dCorrectAmplMT1  = 1.0 + dAddCorrect[nMT1];
   m_BasicData.dCorrectAmplMT2  = 1.0 + dAddCorrect[nMT2];
   m_BasicData.dCorrectAmplMT3  = 1.0 + dAddCorrect[nMT3];
   m_BasicData.dCorrectAmplHT   = 1.0 + dAddCorrect[nHT];

   GetDeltaEffic(NULL, 0.0);
}

double CBoxPropertySheet::GetCorrAmpl(int nChassisTyp, double *dAddCorrect)
{
   double dDummy[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
   if (dAddCorrect == NULL) dAddCorrect = dDummy;
   switch (nChassisTyp)
   {
      case CARABOX_CHASSISTYP_BR:                           // Baßreflex
      case CARABOX_CHASSISTYP_TL:                           // Transmissionline
      case CARABOX_CHASSISTYP_PM:                           // Passivmembran
      case CARABOX_CHASSISTYP_TT1:
      case CARABOX_CHASSISTYP_TT2: return m_BasicData.dCorrectAmplTT12 + dAddCorrect[0]; break;
      case CARABOX_CHASSISTYP_MT1: return m_BasicData.dCorrectAmplMT1  + dAddCorrect[1]; break;
      case CARABOX_CHASSISTYP_MT2: return m_BasicData.dCorrectAmplMT2  + dAddCorrect[2]; break;
      case CARABOX_CHASSISTYP_MT3: return m_BasicData.dCorrectAmplMT3  + dAddCorrect[3]; break;
      case CARABOX_CHASSISTYP_HT:  return m_BasicData.dCorrectAmplHT   + dAddCorrect[4]; break;
   }
   return 1.0;
}

double CBoxPropertySheet::GetDeltaEffic(double *pdAddCorrect, double dTargetEffic)
{
   double       dCorrectGroup, dDeltaEfficNew;
   ChassisData *pCD;
   int          j, nStart = FRQ_INDEX_CA_100_HZ, nEnd=FRQ_INDEX_CA_16000_HZ;

   if (dTargetEffic == 0.0) nStart = 0, nEnd = CALCNOOFFREQ-1;
   else
   {
      if (m_BasicData.nXoverTyp == CARABOX_CO_1W)              // 1 Wege in einem begrenzten Bereich optimieren
      {
         for(j=0; j<CALCNOOFFREQ; j++)                         // von unten die untere GF suchen
         {
            if (gm_dFreq[j] > m_BasicData.dLowLimitFreq)
            {
               nStart = j;
               break;
            }
         }
         for(j=CALCNOOFFREQ-1; j>=0; j--)                      // von oben die obere GF suchen
         {
            if (gm_dFreq[j] < m_BasicData.dHighLimitFreq)
            {
               nEnd = j;
               break;
            }
         }
      }
      else 
      {
         double dTemp = m_BasicData.sTT1.dTPFreq;
         if ((m_BasicData.nXoverTyp == CARABOX_CO_2_3W) || (m_BasicData.nXoverTyp == CARABOX_CO_3_4W))
         {
            dTemp = m_BasicData.sTT2.dTPFreq;
         }
         if (dTemp < gm_dFreq[FRQ_INDEX_CA_100_HZ])
         {
            for(j=FRQ_INDEX_CA_100_HZ-1; j>=0; j--)              // von oben die untere GF suchen
            {
               if (gm_dFreq[j] < dTemp)
               {
                  nStart = j;
                  break;
               }
            }
         }
      }
   }

   for(j=0; j<CALCNOOFFREQ; j++)
      m_BasicData.cSPTotalZeroStandard[j].SetValues(0.0, 0.0);

   for (pCD=(ChassisData*)m_ChassisData.GetFirst(); pCD!=NULL; pCD=(ChassisData*)m_ChassisData.GetNext())
   {
      if (!pCD->AdoptChassisEffic()) continue;
      dCorrectGroup = GetCorrAmpl(pCD->nChassisTyp, pdAddCorrect);
      for(j=nStart; j<=nEnd; j++)
      {
         m_BasicData.cSPTotalZeroStandard[j] += dCorrectGroup * pCD->dCorrectAmpl * pCD->cAmplAccel[j] * pCD->cSPZeroStandard[j];
      }
   }

   if (dTargetEffic == 0.0) nEnd = 0;
   dDeltaEfficNew = 0.0;
   for(j=nStart; j<nEnd; j++)
   {
      dDeltaEfficNew += fabs(dTargetEffic - Betrag(m_BasicData.cSPTotalZeroStandard[j]));
   }
   return dDeltaEfficNew;
}

/********************************************************************/
/*                                                                  */
/*    Berechnung der Schalldruckdaten für die BDD-Schnittstelle     */
/*    Array[1,3,5,10m][90,84..-90°][0,10..350°][118Freq]            */
/*                                                                  */
/********************************************************************/
int CBoxPropertySheet::CalcCaraBDD()
{
   int      i, j, k, m, nPhi, nTheta1, nTheta2, nPhi1, nPhi2, nWall;
   int      index, nReturn = 1, indexCoupled = -1, nProgressTemp = 0, nAvgSecSum = 0;
   CVector  vDeltaZeroPos, vDZPn, vDeltaZeroPosNormCH;
   double   dBetragDeltaZeroPos;
   double   dSinTheta, dCosTheta, dSinPhi, dCosPhi, dAvgSecSum = 0;
   double   dThetaInt, dPhiInt, dTemp1, dTemp2 = 0, dTemp3, dThetaOrg, dPhiOrg;
   double  *pdSinTheta = NULL,
           *pdCosTheta = NULL,
           *pdSinPhi   = NULL,
           *pdCosPhi   = NULL;
   double   dCorrectAmplGroup;
   double   dFakt1Volt;
   double   dStart, dStep;
   Complex  cWertTheta1Phi1, cWertTheta1Phi2, cWertTheta2Phi1, cWertTheta2Phi2;
   Complex  cWertThetaPhi;
   Complex  ***pSPThetaPhiFreq = NULL;
   ChassisData *pCD = NULL;
   CVector  vWallNorm;
   Complex  cTemp;
   int      nNoOfChassisToCalc = 0;
   clock_t  tEstimate1 = 0, tEstimate2;

   try
   {
      Allocate3D_SPL();                                        // SPL-Array[Entf=4][Theta=31][Phi=36][Freq=118]
                                                               // zum Abspeichern der CARA-BDD-Rohdaten (nicht komprimiert)
      for(i=0; i<SPL_NO_OF_DISTANCE; i++)                      // alles mit NUll vorbelegen bei wiederholter Berechnung
      {
         for(j=0; j<SPL_NO_OF_THETA; j++)
         {
            if((j == 0) || (j == SPL_NO_OF_THETA-1)) nPhi = 1;// für Theta=90°,-90° nur Phi=0°
            else                                     nPhi = SPL_NO_OF_PHI;
            for(k=0; k<nPhi; k++)
            {
               for(m=0; m<CALCNOOFFREQ; m++)
                  m_ppppcSP_3D[i][j][k][m].SetValues(0.0, 0.0);
            }
         }
      }

      pdSinTheta = new double[SPL_NO_OF_THETA];
      pdCosTheta = new double[SPL_NO_OF_THETA];
      pdSinPhi   = new double[SPL_NO_OF_PHI];
      pdCosPhi   = new double[SPL_NO_OF_PHI];

      dStart = 0.5 * M_PI;                                     // 90° in rad
      dStep  = -6.0 * M_PI_D_180;                              // -6° in rad
      for(i=0; i<SPL_NO_OF_THETA; i++)
      {
         SinCos(dStart, pdSinTheta[i], pdCosTheta[i]);
         dStart += dStep;
      }

      nWall = 0;
      if      (m_Cabinet.m_dwFlags & ZERO_DEG_REF_LEFT ) nWall = CARABOX_CABWALL_LEFT;
      else if (m_Cabinet.m_dwFlags & ZERO_DEG_REF_BACK ) nWall = CARABOX_CABWALL_BACK;
      else if (m_Cabinet.m_dwFlags & ZERO_DEG_REF_RIGHT) nWall = CARABOX_CABWALL_RIGHT;
      if (nWall)
      {
         m_Cabinet.GetCabinetWall(nWall, NULL, &vWallNorm);
         dTemp2 = atan2(Vy(vWallNorm), Vx(vWallNorm));
      }

      if (nWall) dStart = dTemp2;                              //  x° in rad
      else       dStart = 0.0;                                 //  0° in rad
      dStep  = 10.0 * M_PI_D_180;                              // 10° in rad
      for(i=0; i<SPL_NO_OF_PHI; i++)
      {
         SinCos(dStart, pdSinPhi[i], pdCosPhi[i]);
         dStart += dStep;
      }

      dFakt1Volt = 1.0/sqrt(1.0*m_BasicData.nNennImped);
      
      if (m_hwndChildDlg)
      {
         ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_CNTR, PBM_SETRANGE32, 0, SPL_NO_OF_DISTANCE*SPL_NO_OF_THETA);
         for (index=0; index<m_Cabinet.nCountAllChassis; index++)
         {
            pCD = (ChassisData*) m_ChassisData.GetAt(index);
            if (pCD->GetNoOfCoupled()) // m_nLSType & DIPOLE_FLAT)                       // Dipol Flächenstrahler
            {
               nNoOfChassisToCalc += pCD->GetNoOfCoupled()-pCD->GetFirstDummy();
            }
            else nNoOfChassisToCalc++;
         }
      }

      for (index=0; index<m_Cabinet.nCountAllChassis;)
      {
         pCD = (ChassisData*) m_ChassisData.GetAt(index);
         if ((m_nLSType & DIPOLE_FLAT) ||                      // Dipol Flächenstrahler
             (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE))
         {
            if (pCD == NULL) break;
            if (indexCoupled == -1)
            {
               if (m_hwndChildDlg)                             // Statusanzeige für die Berechnung
               {
                  char szText[128];
                  ::LoadString(g_hInstance, IDS_BGD, szText, 128);
                  strncat(szText, pCD->szDescrpt, 128);        // Beschreibung
                  ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_TEXT, WM_SETTEXT, 0, (LPARAM)szText);
                  nProgressTemp = pCD->GetNoOfCoupled()-pCD->GetFirstDummy();// Statusbereich
                  ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_CNTR, PBM_SETRANGE32, 0, SPL_NO_OF_DISTANCE*SPL_NO_OF_THETA*nProgressTemp);
                  nProgressTemp = 0;
               }
               indexCoupled = pCD->GetFirstDummy();
            }
            else
            {
               nProgressTemp += SPL_NO_OF_DISTANCE*SPL_NO_OF_THETA;
            }
            pCD  = pCD->GetCoupled(indexCoupled);              // aber die Gekoppelten
            if (pCD == NULL)                                   // kein weiteres gekoppeltes gefunden
            {
               index++;                                        // nächstes Hauptchassis nehmen
               indexCoupled = -1;                              // mit dem ersten gekoppelten wieder beginnen
               continue;
            }
         }
         else                                                  // andere Lautsprecher
         {
            if (m_hwndChildDlg)                                // Statusanzeige für die Berechnung
            {
               char szText[128];
               ::LoadString(g_hInstance, IDS_BGD, szText, 128);// Beschreibung
               strncat(szText, pCD->szDescrpt, 128);
               ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_TEXT, WM_SETTEXT, 0, (LPARAM)szText);
            }
         }

         if (m_hwndChildDlg)                                   // Statusanzeige für die Berechnungszeit
         {
            tEstimate2 = clock();
            if (tEstimate1)
            {
               double dSeconds;                                // Rechenzeit für ein Chassis
               dSeconds   = (double)(tEstimate2 - tEstimate1) / CLOCKS_PER_SEC;
               dAvgSecSum+= dSeconds;
               if (++nAvgSecSum > 1)
               {
                  dSeconds = dAvgSecSum / (double)nAvgSecSum; 
               }
               dSeconds  *= nNoOfChassisToCalc;                // Rechenzeit für alle restlichen
               int nSec   = CEtsDiv::Round(dSeconds);
               if (nSec > 0)
               {
                  char   szText[128];
                  char   szFormat[128];
                  int nMin   = nSec / 60;                      // Minuten
                  if (nMin) nSec -= nMin*60;                   // Sekunden
                  ::LoadString(g_hInstance, IDS_BGD_TIME, szFormat, 128);
                  wsprintf(szText, szFormat, nMin, nSec);
                  ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_TIME, WM_SETTEXT, 0, (LPARAM)szText);
               }
            }
            tEstimate1 = tEstimate2;
            nNoOfChassisToCalc--;
         }

         switch (pCD->nChassisTyp)
         {
            case CARABOX_CHASSISTYP_BR:                        // Baßreflex
            case CARABOX_CHASSISTYP_PM:                        // Passivmembran
            case CARABOX_CHASSISTYP_TL:                        // Transmissionline
            case CARABOX_CHASSISTYP_TT1:                       // TT1 & TT2
            case CARABOX_CHASSISTYP_TT2: dCorrectAmplGroup = m_BasicData.dCorrectAmplTT12; break;
            case CARABOX_CHASSISTYP_MT1: dCorrectAmplGroup = m_BasicData.dCorrectAmplMT1;  break;
            case CARABOX_CHASSISTYP_MT2: dCorrectAmplGroup = m_BasicData.dCorrectAmplMT2;  break;
            case CARABOX_CHASSISTYP_MT3: dCorrectAmplGroup = m_BasicData.dCorrectAmplMT3;  break;
            case CARABOX_CHASSISTYP_HT:  dCorrectAmplGroup = m_BasicData.dCorrectAmplHT;   break;
         }


         m_Cabinet.GetCabinetWall(pCD->nCabWall, NULL, &vWallNorm);// Flächennormalenvektor der Wand
         dTemp1    = atan2(Vy(vWallNorm), Vx(vWallNorm));
         SinCos(dTemp1, dSinPhi, dCosPhi);                     // Phi für Deltazeroposition
 
         dSinTheta = Vz(vWallNorm);                            // Theta für Deltazeroposition
         dTemp1    = asin(dSinTheta);
         dCosTheta = cos(dTemp1);

         pSPThetaPhiFreq = ReadSP_Directiv(pCD->szDirectFileName);// Richtcharakteristikdaten einlesen

         for(m=0; m<SPL_NO_OF_DISTANCE; m++)
         {
            for(i=0; i<SPL_NO_OF_THETA; i++)
            {
               if (m_hwndChildDlg)
               {
                  if (m_nCalcWhat & TERMINATE_THREAD_SAVE) throw (DWORD) TERMINATE_THREAD_SAVE;
                  else ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_CNTR, PBM_SETPOS, nProgressTemp+m*SPL_NO_OF_THETA+i, 0);
               }
               if((i == 0) || (i == SPL_NO_OF_THETA-1)) nPhi = 1;// für Theta=90°,-90° nur Phi=0°
               else                                     nPhi = SPL_NO_OF_PHI;
               for(j=0; j<nPhi; j++)
               {
                  CFloatPrecision fp;
                  CVector vDir(pdCosPhi[j]*pdCosTheta[i], pdSinPhi[j]*pdCosTheta[i], pdSinTheta[i]);
                  vDeltaZeroPos = 0.001 * m_Cabinet.vPosRef +
                                  vDir  * gm_dDist[m] -
                                  0.001 * pCD->vPosChassis; // in m

                  dBetragDeltaZeroPos = Betrag(vDeltaZeroPos);
                  vDZPn   = vDeltaZeroPos/dBetragDeltaZeroPos;

                  vDeltaZeroPosNormCH.SetX() =  Vx(vDZPn)*dCosTheta*dCosPhi + Vy(vDZPn)*dCosTheta*dSinPhi + Vz(vDZPn)*dSinTheta;
                  vDeltaZeroPosNormCH.SetY() = -Vx(vDZPn)*dSinPhi           + Vy(vDZPn)*dCosPhi;
                  vDeltaZeroPosNormCH.SetZ() = -Vx(vDZPn)*dSinTheta*dCosPhi - Vy(vDZPn)*dSinTheta*dSinPhi + Vz(vDZPn)*dCosTheta;

                  dThetaOrg = asin(Vz(vDeltaZeroPosNormCH))*M180_D_PI;
                  dPhiOrg   = atan2(Vy(vDeltaZeroPosNormCH), Vx(vDeltaZeroPosNormCH))*M180_D_PI;

                  if(dPhiOrg   <   0.0) dPhiOrg  += 360.0;
                  if(dPhiOrg   > 360.0) dPhiOrg   = 360.0;
                  if(dThetaOrg < -90.0) dThetaOrg = -90.0;
                  if(dThetaOrg >  90.0) dThetaOrg =  90.0;

                  GetThetaPhiIndex(dThetaOrg, dPhiOrg, nTheta1, nTheta2, dThetaInt, nPhi1, nPhi2, dPhiInt);

                  dTemp1 =  2.0*M_PI*gm_dDist[m]/CSCHALL;
                  dTemp2 = 20.0*dCorrectAmplGroup*pCD->dCorrectAmpl*dFakt1Volt/dBetragDeltaZeroPos;
                  dTemp3 =  2.0*M_PI*dBetragDeltaZeroPos/CSCHALL;
                  Complex cCos3Sin3;                           // double dSin3, dCos3;
                  Complex cCos1Sin1;                           // double dSin1, dCos1;
                  if (pCD->nPolung == -1) dTemp2 = -dTemp2;
                  for( k = 0; k < CALCNOOFFREQ; k++ )
                  {
                     SinCos(dTemp3*gm_dFreq[k], cCos3Sin3.imagteil, cCos3Sin3.realteil);// SinCos(dTemp3*gm_dFreq[k], dSin3, dCos3);
                     cCos3Sin3.imagteil = -cCos3Sin3.imagteil; // -dSin3
                     SinCos(gm_dFreq[k]*dTemp1, cCos1Sin1.imagteil, cCos1Sin1.realteil);// SinCos(gm_dFreq[k]*dTemp1, dSin1, dCos1);
                     m_ppppcSP_3D[m][i][j][k] += dTemp2 * pCD->cAmplAccel[k] *
                                                 GetSPLDirectiv(pSPThetaPhiFreq, nTheta1, nTheta2, dThetaInt, nPhi1, nPhi2, dPhiInt, k) *
                                                 cCos3Sin3 *   // Complex(dCos3, -dSin3) * 
                                                 cCos1Sin1;    // Complex(dCos1, dSin1);
//                     m_ppppcSP_3D[m][i][j][k] += cTest;
                  }
               }
            }
         }

         if (pCD->GetNoOfCoupled()) // m_nLSType & DIPOLE_FLAT)                          // Dipol Flächenstrahler
         {
            indexCoupled++;
         }
         else
         {
            index++;
         }
      }
   }
   catch (DWORD nError)
   {
      char text[128];
      if (nError != TERMINATE_THREAD_SAVE)
      {
         wsprintf(text, "CalcCaraBDD Error : %0x", nError);
         GetActivePage()->MessageBox((long)text, IDS_PROPSHCAPTION, MB_OK|MB_ICONERROR|MBU_TEXTSTRING);
      }
      nReturn = 0;
   }

   if (pdSinTheta)    delete[] pdSinTheta;
   if (pdCosTheta)    delete[] pdCosTheta;
   if (pdSinPhi)      delete[] pdSinPhi;
   if (pdCosPhi)      delete[] pdCosPhi;

   return nReturn;
}

/********************************************************************/
/*                                                                  */
/*       Erstellung des CARA-BDD-Files (LS-Datensatz für CARA)      */
/* nFlag = 1 -> kompletter BDD-File,                                */
/*       = 2 -> TMP-File für CaraBoxView                            */
/*                                                                  */
/********************************************************************/
void CBoxPropertySheet::CreateCaraBDD(int nFlag)
{
   int         i;
   int         fRotate,                          // = 0 -> alle Drehwinkel zugelassen (Regalboxen), = 3 -> Standboxen
               j, k, l, nPhi, nWall;
   long        nStartIndex;
   char        szDescription[32];
   char        szTradeMark[64];
   CVector     vWallNorm;

   int               filebegin, fileend;
   CFileHeader       FileHeader1("BSD", "2001", 100);
   CFileHeader       FileHeader2("BGD", "2001", 100);
   CFileHeader       FileHeader3("BMD", "2001", 100);
   // für die Schnittstelle Geometriedaten
   int         nDummy[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   int         nFiles = 0;
   int         nCountBoxPt;
   int         nCountTotalPt;
   int         nCountBoxPoly = 0;
   int         nCountChassisPoly = 0;
   int         nCountSphere = 0;
   int         *pnCountBoxPolyPt = NULL;                       // Anzahl der PolyPunkte pro Gehäusepolygon
   int         *pnCountChassisPolyPt = NULL;                   // Anzahl der Polypunkte pro Chassispolygon
   PointFloat  *psBox_CALE = NULL,
               *psChassis_CALE = NULL,
               *psSphereLocations = NULL;
   float       *pfSphereRadius = NULL;
                                                               // fortlaufend gegen den Uhrzeigersinn bei Draufsicht numeriert:
   int         **ppnIndexBoxPolyPt = NULL;                     // Array[anzBoxPoly][anzPtproBoxPoly]
   int         **ppnIndexChassisPolyPt = NULL;                 // Array[anzChassisPoly][anzPtproChassisPoly]
   PointFloat  fptPosZeroRefCALE, fptPosUserRefCALE;

   try
   {
      CalcReferencePoints();
      wsprintf( szDescription, m_BoxText.szDescrpt );          // Kurzbeschreibung für BDD-File
      wsprintf( szTradeMark, "?%s", m_BoxText.szTradeMark );   // Markenzeichen mit vorangestelltem '?' für selbst erstellte Boxen

//      if( m_Cabinet.dHeight < 650 )
         fRotate = 0;                                          // Regalbox: alle Drehwinkel zugelassen
//      else
//         fRotate = 3;                                        // Standbox
      
      if(nFlag == 1)                                           // SPL-Daten
      {
         if (m_hwndChildDlg)
         {
            char szText[64];
            ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_CNTR, PBM_SETRANGE32, 0, SPL_NO_OF_DISTANCE*SPL_NO_OF_THETA);
            ::LoadString(g_hInstance, IDS_BSD, szText, 64);
            ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_TEXT, WM_SETTEXT, 0, (LPARAM)szText);
         }

      }

      FileHeader1.CalcChecksum(NULL, 0);                       // Prüfsumme zurücksetzen
      FileHeader1.SetComment(szTradeMark);
      filebegin = SetFilePointer(m_hFile,0,NULL,FILE_CURRENT); // Filepointer auf Anfang merken
      m_pFH = NULL;
      WriteFileFH(&FileHeader1, sizeof(CFileHeader));
      m_pFH = &FileHeader1;

      if(nFlag == 1)
      {
         nStartIndex  = 0;                                     // Länge der Filterdaten
         m_nByteCount = 0;
         if (m_nLSType & ACTIVE_LS)                            // Aktive Lautsprecher
         {
            FilterBox *pFB = GetOneWayFilter();
            long  nOrder   = pFB->nHPOrder;                    // Hochpassfilter
            long  nType    = pFB->nHPType;
            float fQFactor = (float)pFB->dHPQFaktor;
            long  nCount;
            nStartIndex = 0;                                   // Länge der Daten
            if (pFB->nHPOrder == 0) nStartIndex += sizeof(long);
            else                    nStartIndex += sizeof(long)*3+sizeof(float)*(m_BasicData.nNoOfLowerHPFrequ+2);
            if (pFB->nTPOrder == 0) nStartIndex += sizeof(long);
            else                    nStartIndex += sizeof(long)*3+sizeof(float)*(m_BasicData.nNoOfUpperTPFrequ+2);

            WriteFileFH(&nStartIndex, sizeof(long));           // länge speichern

            WriteFileFH(&nOrder, sizeof(long));                // Ordnung speichern
            if (nOrder != 0)
            {
               WriteFileFH(&nType, sizeof(long));
               WriteFileFH(&fQFactor, sizeof(float));
               if (m_BasicData.nNoOfLowerHPFrequ == 2)         // variables Filter
               {
                  nCount = 1;
                  WriteFileFH(&nCount, sizeof(long));          // Frequenzbereich UGF, OGF
                  WriteFileFH(m_BasicData.fLowerHPFrequ, sizeof(float)*2);// 2 GF
               }
               else                                            // diskrete Frequenzen
               {
                  WriteFileFH(&m_BasicData.nNoOfLowerHPFrequ, sizeof(long));
                  WriteFileFH(m_BasicData.fLowerHPFrequ, sizeof(float)*m_BasicData.nNoOfLowerHPFrequ);
               }
               WriteFileFH(&m_BasicData.fLowerHPFrequ[m_BasicData.nSelLowerHPFrequ], sizeof(float));
            }

            nOrder   = pFB->nTPOrder;
            nType    = pFB->nTPType;
            fQFactor = (float)pFB->dTPQFaktor;
            WriteFileFH(&nOrder, sizeof(long));                // Ordnung speichern
            if (nOrder)
            {
               WriteFileFH(&nType, sizeof(long));
               WriteFileFH(&fQFactor, sizeof(float));
               if (m_BasicData.nNoOfUpperTPFrequ == 2)         // variables Filter
               {
                  nCount = 1;
                  WriteFileFH(&nCount, sizeof(long));          // Frequenzbereich UGF, OGF
                  WriteFileFH(m_BasicData.fUpperTPFrequ, sizeof(float)*2);// 2GF
               }
               else                                            // diskrete Frequenzen
               {
                  WriteFileFH(&m_BasicData.nNoOfUpperTPFrequ, sizeof(long));
                  WriteFileFH(m_BasicData.fUpperTPFrequ, sizeof(float)*m_BasicData.nNoOfUpperTPFrequ);
               }
               WriteFileFH(&m_BasicData.fUpperTPFrequ[m_BasicData.nSelUpperTPFrequ], sizeof(float));
            }
         }
         else WriteFileFH(&nStartIndex, sizeof(long));

         short       lang16_wert, lang16_wertNeu;
         short       SumLast_kurz8_wert;
         signed char kurz8_wert = 0;
         double      dMagn;

         for(i=0; i<SPL_NO_OF_DISTANCE; i++)
         {
            for(j=0; j<SPL_NO_OF_THETA; j++)
            {
               if (m_hwndChildDlg)
               {
                  if (m_nCalcWhat & TERMINATE_THREAD_SAVE) throw (DWORD) TERMINATE_THREAD_SAVE;
                  else ::SendDlgItemMessage(m_hwndChildDlg, IDC_PRGR_CNTR, PBM_SETPOS, i*SPL_NO_OF_THETA+j, 0);
               }
               if((j == 0) || (j == SPL_NO_OF_THETA-1)) nPhi = 1;// für Theta=90°,-90° nur Phi=0°
               else                                     nPhi = SPL_NO_OF_PHI;
               for(k=0; k<nPhi; k++)
               {                                               // Amplitudenwerte gepackt
                  dMagn = 20.0*log10(Betrag(m_ppppcSP_3D[i][j][k][0])/P0);
                  if (dMagn < -320.0)   dMagn = -320.0;
                  if (dMagn >  320.0)   dMagn =  320.0;
                  if     (dMagn > 0.0) lang16_wert = (short) (10*dMagn+0.5);
                  else if(dMagn < 0.0) lang16_wert = (short) (10*dMagn-0.5);
                  else                 lang16_wert = 0;

                  WriteFileBuffered(&lang16_wert, sizeof(short));

                  SumLast_kurz8_wert = 0;
                  for(l=1; l<CALCNOOFFREQ; l++)
                  {
                     dMagn = 20*log10(Betrag(m_ppppcSP_3D[i][j][k][l])/P0);
                     if( dMagn < -320.0 )   dMagn = -320.0;
                     if( dMagn >  320.0 )   dMagn =  320.0;
                     if( dMagn > 0.0 )      lang16_wertNeu = (short) (10*dMagn+0.5);
                     else if( dMagn < 0.0 ) lang16_wertNeu = (short) (10*dMagn-0.5);
                     else                   lang16_wertNeu = 0;

                     if(abs(lang16_wertNeu - (lang16_wert+SumLast_kurz8_wert)) > 127)
                     {
                        kurz8_wert         = -128;
                        lang16_wert        = lang16_wertNeu;
                        SumLast_kurz8_wert = 0;
                     }
                     else
                     {
                        kurz8_wert = (signed char) (lang16_wertNeu - (lang16_wert+SumLast_kurz8_wert));
                        SumLast_kurz8_wert = lang16_wertNeu - lang16_wert;
                     }
                  
                     WriteFileBuffered(&kurz8_wert, sizeof(signed char));

                     if( kurz8_wert == -128 )
                     {
                        WriteFileBuffered(&lang16_wert, sizeof(short));
                     }
                  }
                                                               // Phasenwerte gepackt
                  dMagn = Phase(m_ppppcSP_3D[i][j][k][0]);
                  if( dMagn > 0.0 )        lang16_wert = (short) (2*dMagn+0.5);
                  else if( dMagn < 0.0 )   lang16_wert = (short) (2*dMagn-0.5);
                  else                             lang16_wert = 0;

                  WriteFileBuffered(&lang16_wert, sizeof(short));

                  SumLast_kurz8_wert = 0;            
                  for( l = 1; l < CALCNOOFFREQ; l++ )
                  {
                     dMagn = Phase(m_ppppcSP_3D[i][j][k][l]);
                     if( dMagn > 0.0 )        lang16_wertNeu = (short) (2*dMagn+0.5);
                     else if( dMagn < 0.0 )   lang16_wertNeu = (short) (2*dMagn-0.5);
                     else                             lang16_wertNeu = 0;

                     if( abs(lang16_wertNeu - (lang16_wert+SumLast_kurz8_wert)) > 127 )
                     {
                        kurz8_wert         = -128;
                        lang16_wert        = lang16_wertNeu;
                        SumLast_kurz8_wert = 0;
                     }
                     else
                     {
                        kurz8_wert = (signed char) (lang16_wertNeu - (lang16_wert+SumLast_kurz8_wert));
                        SumLast_kurz8_wert = lang16_wertNeu - lang16_wert;
                     }

                     WriteFileBuffered(&kurz8_wert, sizeof(signed char));

                     if( kurz8_wert == -128 )
                     {
                        WriteFileBuffered(&lang16_wert, sizeof(short));
                     }
                  }
               }
            }
         }
         WriteFileBuffered(NULL, 0);
         // Ende der BSD-Daten
         fileend = SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);// Filepointer auf 'Hier' merken
         SetFilePointer(m_hFile, filebegin, NULL, FILE_BEGIN); // FilePointer auf Filebeginn setzen
         m_pFH = NULL;
         WriteFileFH(&FileHeader1, sizeof(CFileHeader));       // neue Prüfsumme schreiben
         SetFilePointer(m_hFile, fileend, NULL, FILE_BEGIN);   // FilePointer auf aktuelle Endpos setzen
      }

      nWall = 0;
      if      (m_Cabinet.m_dwFlags & ZERO_DEG_REF_LEFT ) nWall = CARABOX_CABWALL_LEFT;
      else if (m_Cabinet.m_dwFlags & ZERO_DEG_REF_BACK ) nWall = CARABOX_CABWALL_BACK;
      else if (m_Cabinet.m_dwFlags & ZERO_DEG_REF_RIGHT) nWall = CARABOX_CABWALL_RIGHT;
      if (nWall)
      {
         m_Cabinet.GetCabinetWall(nWall, NULL, &vWallNorm);
         SetMatrixPtr((Array44*)&m_Cabinet.m_pdTran, false);
         LoadIdentity();
         Rotate_z(-atan2(Vy(vWallNorm), Vx(vWallNorm)));
         SetMatrixPtr(NULL, false);
      }

      nCountBoxPt   = 0;
      nCountBoxPoly = 0;
      psBox_CALE = m_Cabinet.GetCabinetPolyPoints(nCountBoxPt);
      m_Cabinet.GetCabinetPolygons(nCountBoxPoly, &ppnIndexBoxPolyPt, &pnCountBoxPolyPt);

                           //NCOUNTTOTALPT, NCOUNTCHASSISPOLY, NCOUNTSPHERES, NCOUNTSHAPED
      long lParamCount[4] = {nCountBoxPt  , 0                , 0            , 0};
      m_ChassisData.ProcessWithObjects(CBoxPropertySheet::CountChassisPolyPoints, (WPARAM)this, (LPARAM)lParamCount);
      nCountTotalPt     = lParamCount[NCOUNTTOTALPT];
      nCountChassisPoly = lParamCount[NCOUNTCHASSISPOLY];
      nCountSphere      = lParamCount[NCOUNTSPHERES];

      if (nCountChassisPoly)
      {
         pnCountChassisPolyPt  = new int[nCountChassisPoly];
         psChassis_CALE        = new PointFloat[nCountTotalPt-nCountBoxPt];
         ppnIndexChassisPolyPt = new int*[nCountChassisPoly];
      }
      if (nCountSphere > 0)
      {
         psSphereLocations  = new PointFloat[nCountSphere];
         pfSphereRadius     = new float[nCountSphere];
      }
      long lParam[9]=  {(long)pnCountChassisPolyPt,            // PNCOUNTCHASSISPOLYPT
                        (long)psChassis_CALE,                  // PSCHASSIS_CALE
                        (long)ppnIndexChassisPolyPt,           // PPNINDEXCHASSISPOLYPT
                        0,                                     // NSTARTINDEX
                        0,                                     // IPOLY
                        nCountBoxPt,                           // NBOXPT
                        (long)psSphereLocations,               // PSSPHEREPOINTS
                        (long)pfSphereRadius,                  // PSSPHERERADIUS
                        0};                                    // NSPHEREINDEX

      m_ChassisData.ProcessWithObjects(CBoxPropertySheet::GetChassisPolygons, (WPARAM)this, (LPARAM)lParam);
      
      fptPosZeroRefCALE = m_Cabinet.VectorToBDDpoint(m_Cabinet.vPosRef);// ZeroDegreeRefPoint im BCS-System
      fptPosUserRefCALE = m_Cabinet.VectorToBDDpoint(m_Cabinet.vUserRef);// UserRefPoint im BCS-System

      FileHeader2.CalcChecksum(NULL, 0);                       // Prüfsumme zurücksetzen
      FileHeader2.SetComment(szTradeMark);
      filebegin = SetFilePointer(m_hFile,0,NULL,FILE_CURRENT); // Filepointer auf Anfang merken
      m_pFH = NULL;
      WriteFileFH(&FileHeader2, sizeof(CFileHeader));
      m_pFH = &FileHeader2;

      int nLSType = LOWORD(m_nLSType);                         // im Highword stehen für BDD-File ungültige Daten !
      WriteFileFH(&nLSType, sizeof(int));
      WriteFileFH(&fRotate, sizeof(int));
      WriteFileFH(&szDescription[0], 32*sizeof(char));
      WriteFileFH(&nDummy[0], 16*sizeof(int));
      WriteFileFH(&nFiles, sizeof(int));

      WriteFileFH(&nCountBoxPt, sizeof(int));
      WriteFileFH(&nCountTotalPt, sizeof(int));
      WriteFileFH(&nCountBoxPoly, sizeof(int));
      WriteFileFH(&nCountChassisPoly, sizeof(int));

      WriteFileFH(&nCountSphere, sizeof(int));                 
                                                               // alle Eckpunkte des Gehäuses und der Chassis
      WriteFileFH(&psBox_CALE[0], nCountBoxPt*sizeof(PointFloat));// Gehäuseeckpunkte
#ifdef _DEBUG
      for (int np=0; np<nCountBoxPt; np++)
         REPORT(" p(%d): (%f, %f, %f)", np+1, psBox_CALE[np].x, psBox_CALE[np].y, psBox_CALE[np].z);
#endif                                                             // Chassiseckpunkte
      if ((nCountTotalPt-nCountBoxPt) > 0)
         WriteFileFH(&psChassis_CALE[0], (nCountTotalPt-nCountBoxPt)*sizeof(PointFloat));

      for(i=0; i<nCountBoxPoly; i++)                           // alle Polygone des Gehäuses
      {
         WriteFileFH(&pnCountBoxPolyPt[i], sizeof(int));
         WriteFileFH(&ppnIndexBoxPolyPt[i][0], pnCountBoxPolyPt[i]*sizeof(int));
#ifdef _DEBUG
         REPORT("Box Poly(%d):%d", i+1, pnCountBoxPolyPt[i]);
         for (int np=0; np<pnCountBoxPolyPt[i]; np++)
            REPORT(" p(%d):%d", np+1, ppnIndexBoxPolyPt[i][np]);
#endif
      }
                                                               // Polys der Chassis
      for(i=0; i<nCountChassisPoly; i++)                       // alle Polygone des(r) Chassis
      {
         WriteFileFH(&pnCountChassisPolyPt[i], sizeof(int));
         WriteFileFH(&ppnIndexChassisPolyPt[i][0], pnCountChassisPolyPt[i]*sizeof(int));
#ifdef _DEBUG
         REPORT("Chassis Poly(%d):%d", i+1, pnCountChassisPolyPt[i]);
         for (int np=0; np<pnCountChassisPolyPt[i]; np++)
            REPORT(" p(%d):%d", np+1, ppnIndexChassisPolyPt[i][np]);
#endif
      }

      for(i=0; i<nCountSphere; i++)                            // alle Mittelpunkte der Kugeln
      {
         WriteFileFH(&psSphereLocations[i], sizeof(PointFloat));
         WriteFileFH(&pfSphereRadius[i], sizeof(float));
      }

      WriteFileFH(&fptPosZeroRefCALE.x, sizeof(float));
      WriteFileFH(&fptPosZeroRefCALE.y, sizeof(float));
      WriteFileFH(&fptPosZeroRefCALE.z, sizeof(float));

      WriteFileFH(&fptPosUserRefCALE.x, sizeof(float));
      WriteFileFH(&fptPosUserRefCALE.y, sizeof(float));
      WriteFileFH(&fptPosUserRefCALE.z, sizeof(float));

                                                               // Ende der BGD-Daten
      fileend = SetFilePointer(m_hFile,0,NULL,FILE_CURRENT);   // Filepointer auf 'Hier' merken
      SetFilePointer(m_hFile, filebegin, NULL, FILE_BEGIN );   // FilePointer auf Filebeginn setzen
      m_pFH = NULL;                                 
      WriteFileFH(&FileHeader2, sizeof(CFileHeader));          // neue Prüfsumme schreiben
      SetFilePointer(m_hFile, fileend, NULL, FILE_BEGIN );     // FilePointer auf aktuelle Endpos setzen

      if (lParamCount[NCOUNTSHAPED] > 0)                       // Chassis mit Membranform
      {
         FileHeader3.CalcChecksum(NULL, 0);                    // Prüfsumme zurücksetzen
         FileHeader3.SetComment("Material Data");
         WriteFileFH(&FileHeader3, sizeof(CFileHeader));
         m_pFH = &FileHeader3;                                 
         BYTE bPolyFlag = 0;
         for(i=0; i<nCountBoxPoly; i++)                        // alle Polygone des Gehäuses
         {
            WriteFileFH(&bPolyFlag, sizeof(BYTE));             // Gehäuse
         }
         m_ChassisData.ProcessWithObjects(CBoxPropertySheet::WriteChassisForm, (WPARAM)this, 0);
         WriteFileFH(&bPolyFlag, sizeof(BYTE));                // Grundmaterial
         long nMaterials = 0;
         WriteFileFH(&nMaterials, sizeof(long));               // Anzahl der Materialien
      }
   }
   catch (DWORD nError)
   {
      if (nError == TERMINATE_THREAD_SAVE)
      {
         m_nFlagOk &= ~CARABOX_BDD_DATA;
      }
      else
      {
         char text[128];
         wsprintf(text, "CreateCaraBDD Error :%d", nError);
         GetActivePage()->MessageBox((long)text, IDS_PROPSHCAPTION, MB_OK|MB_ICONERROR|MBU_TEXTSTRING);
      }
   }

   if(pnCountBoxPolyPt)     delete[] pnCountBoxPolyPt;         // alle dynamischen Datenfelder löschen
   if(pnCountChassisPolyPt) delete[] pnCountChassisPolyPt;
   if(ppnIndexBoxPolyPt)
   {
      for (i=0; i<nCountBoxPoly; i++)
         if(ppnIndexBoxPolyPt[i])
            delete[] ppnIndexBoxPolyPt[i];
      delete[] ppnIndexBoxPolyPt;
   }

   if (ppnIndexChassisPolyPt)
   {
      for (i=0; i<nCountChassisPoly; i++)
         if (ppnIndexChassisPolyPt[i])
            delete[] ppnIndexChassisPolyPt[i];
      delete[] ppnIndexChassisPolyPt;
   }
   if(psBox_CALE)        delete[] psBox_CALE;
   if(psChassis_CALE)    delete[] psChassis_CALE;
   if(psSphereLocations) delete[] psSphereLocations;
   if(pfSphereRadius)    delete[] pfSphereRadius;
}

int __cdecl CBoxPropertySheet::SortDirectivData(const void *p1, const void *p2)
{
   return strcmp(((SDirectivData*)p1)->pszDirectivFileName, ((SDirectivData*)p2)->pszDirectivFileName);
}

void CBoxPropertySheet::WriteFileBuffered(void *p, DWORD dwSize)
{
   if ((p != NULL) && (m_nBufferPointer + dwSize < FILE_BUFFERSIZE))
   {
      memcpy(&m_Buffer[m_nBufferPointer], p, dwSize);
      m_nBufferPointer += dwSize;
   }
   else
   {
      if (m_nBufferPointer)
      {
         WriteFileFH(m_Buffer, m_nBufferPointer);
         m_nBufferPointer = 0;
      }
      if (p != NULL)
      {
         WriteFileFH(p, dwSize);
      }
   }
   m_nByteCount += dwSize;
   if (m_nByteCount >= 139900)
   {
      dwSize = 0;
   }
}

void CBoxPropertySheet::CheckCabinetDesign(CCabinet*pC)
{
   if (pC == NULL) pC = &m_Cabinet;
   m_dVolume = pC->InitCabPoints()*0.000001;
   if (m_dVolume > 0)
   {
      m_ChassisData.ProcessWithObjects(CBoxPropertySheet::CheckChassisPos, (WPARAM)this, (LPARAM)pC);
      m_nFlagOk |= CARABOX_CABINET;
   }
   m_Cabinet.m_dwFlags &= ~CHECK_ALL_WALLS;
}

int CBoxPropertySheet::CheckChassisPos(void *p, WPARAM wP, LPARAM lP)
{
   BEGIN("CheckChassisPos");
   CBoxPropertySheet     *pThis  = (CBoxPropertySheet*) wP;
   ChassisData           *pCD    = (ChassisData*) p;
   CCabinet              *pCbase = &pThis->m_Cabinet;           // Basisgehäuse
   CCabinet              *pCcurr = (CCabinet*)lP;
   bool                   bInvalidate = false;

   if (pCD->IsHidden()) return 0;
   if (pCD->m_ppCabinets != NULL)
   {
      if ((pCD->m_ppCabinets[0]->nCountHT) ||
          (pCD->m_ppCabinets[0]->nCountTL) && (pCD->GetCabinet() == pThis->m_nCurrentCab))
      {
         pCbase->m_dwFlags |= CALC_3D_CHASSISPOS;
         pCcurr->m_dwFlags |= (1 << pCD->GetWall());
         bInvalidate = true;
      }
   }

   if (pCcurr->CheckWall(pCD->GetWall()) &&                    // soll diese Wand geprüft werden ?
       pCcurr->CanContainChassis() &&                          // darf dieses Segment Cahssis enthalten ?
       pCcurr->IsWallEnabled(pCD->GetWall()))                  // Ist diese Wand aktiv
   {
      CVector2D vrcWall[NO_OF_RECT_POINTS];
      if (pCbase->GetCabinetWall(pCD->nCabWall, (CVector2D*)vrcWall))
      {
         CCabinet::IsOnWall((CVector2D*)vrcWall, pCD);
      }
   }
   if (bInvalidate && pCD->m_ppCabinets)
   {
      if (pCbase->m_dwFlags & CALC_3D_CHASSISPOS)
      {
         pCD->dwFlagsCH |= CHASSIS_3D_POS_INVALID;
         pCbase->CalcChassisPos3D(pCD);
      }
      int i, nCabs = 0;
      if      (pCD->m_ppCabinets[0]->nCountHT)
      {
         nCabs = pCD->m_ppCabinets[0]->nCountHT;
         pThis->SendMsgTo3DDlg(WM_COMMAND, MAKELONG(IDC_BOX_VIEW, LIST_CHASSIS), (LPARAM)pThis->m_hWnd);
      }
      else if (pCD->m_ppCabinets[0]->nCountTL) nCabs = pCD->m_ppCabinets[0]->nCountTL;
      for (i=0; i<nCabs; i++)
      {
         pThis->SendMsgTo3DDlg(WM_COMMAND, MAKELONG(IDC_BOX_VIEW, LIST_CABINET+pCD->m_ppCabinets[i]->nCountAllChassis), (LPARAM)pThis->m_hWnd);
      }
   }
   return 1;
}

int CBoxPropertySheet::DeterminChassisPositions(void *p, WPARAM /*wParam*/, LPARAM lParam)
{
   CBoxPropertySheet *pThis = (CBoxPropertySheet*)lParam;
   CCabinet          *pC    = &pThis->m_Cabinet;
   ChassisData       *pCD   = (ChassisData*) p;

   if (pC->m_dwFlags & CALC_2D_CHASSISPOS)
   {
      pCD->dwFlagsCH |= CHASSIS_2D_POS_INVALID;
      pC->CalcChassisPos2D(pCD);
   }

   if (pC->m_dwFlags & CALC_3D_CHASSISPOS)
   {
      pCD->dwFlagsCH |= CHASSIS_3D_POS_INVALID;
      pC->CalcChassisPos3D(pCD);
   }
   return 1;
}

void CBoxPropertySheet::CloseFile()
{
   if (m_hFile != INVALID_HANDLE_VALUE)
   {
      CloseHandle(m_hFile);
      m_hFile = INVALID_HANDLE_VALUE;
   }
   m_pFH   = NULL;
}

int CBoxPropertySheet::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //REPORT("%x", uMsg);
   switch(uMsg)
   {
      case WM_SYSCOMMAND:
      switch (wParam)
      {
         case SC_CLOSE:
            if (!RequestSaveData()) return false;
            break;
         case IDS_STACK_CHILD_WNDS:
            StackProcessWindows();
            break;
         case IDS_STACK_CHILDV_WNDS:
            StackProcessWindows(1);
            break;
         case IDS_STACK_CHILDH_WNDS:
            StackProcessWindows(2);
            break;
         case IDS_CLOSE_CHILD_WNDS:
            m_ProcessHandles.Destroy();
            break;
      }
      if((wParam&0xFFF0)==SC_KEYMENU)
      {
         HWND hwndFocus = GetFocus();
         if (hwndFocus)
         {
            long nID = ::GetWindowLong(hwndFocus, GWL_ID);
            bool bSigned = false;
            switch(nID)
            {
               case IDC_CD_EDT_WIDTH1:     case IDC_CD_EDT_WIDTH2:
               case IDC_CD_EDT_DEPTH1:     case IDC_CD_EDT_DEPTH2:
               case IDC_CD_EDT_POS_M:
                  bSigned = true;
               case IDC_CD_EDT_HEIGHT:     case IDC_CD_EDT_WIDTH:  case IDC_CD_EDT_DEPTH:
               case IDC_CD_EDT_POS_HEIGHT:
               case IDC_CH_EDT_DIAMETER:   case IDC_CH_EDT_WIDTH:  case IDC_CH_EDT_HEIGHT:
               {
                  ETSDIV_NUTOTXT ntx = {0, ETSDIV_NM_STD|ETSDIV_NM_NO_END_NULLS|ETSDIV_UC_CTRL_INIT|ETSDIV_UC_CTRL_REINIT, -3, 0, 0};
                  if (bSigned) ntx.nmode |= ETSDIV_UC_CTRL_SIGNED;
                  GetActivePage()->m_nFlags |= NOKILLFOCUSNOTIFICATION;
                  bool bReturn = CEtsDiv::UnitConvertDlg(&ntx, hwndFocus);
                  GetActivePage()->m_nFlags &= ~NOKILLFOCUSNOTIFICATION;
                  if (bReturn)
                  {
                     ::SendMessage(hwndFocus, EM_SETMODIFY, 1, 0);
                     ::SendMessage(GetActivePage()->GetWindowHandle(), WM_COMMAND, MAKELONG(nID, EN_KILLFOCUS), (LPARAM)hwndFocus);
                  }
               }break;
               default: break;
            }
         }
         return TRUE;
      }break;
      case WM_KEYDOWN:
         wParam = wParam;
         break;
      case WM_COMMAND:
      switch (LOWORD(wParam))
      {
         case IDCANCEL:
            if (!RequestSaveData()) return false;
            break;
         case IDHELP:
         if (HIWORD(wParam) == BN_CLICKED)
         {
            CEtsPropertyPage*pP = GetActivePage();
            if (pP)
            {                // hWndFrom, idFrom        , code,            lParam
               PSHNOTIFY pshn = {{m_hWnd, LOWORD(wParam), HIWORD(wParam)}, 0};
               SendMessage(pP->GetWindowHandle(), WM_NOTIFY,0, (LPARAM)&pshn);
            }
         } break;
      } break;
      case WM_INITMENUPOPUP:
      if (HIWORD(lParam))
      {
         HMENU hMenu = (HMENU) wParam;
         UINT nEnable  = (m_ProcessHandles.GetCounter() == 0) ? MF_GRAYED:MF_ENABLED;
         UINT nEnableX = (m_ProcessHandles.GetCounter() <  2) ? MF_GRAYED:MF_ENABLED;
         ::EnableMenuItem(hMenu, IDS_STACK_CHILD_WNDS , nEnable);
         ::EnableMenuItem(hMenu, IDS_STACK_CHILDV_WNDS, nEnableX);
         ::EnableMenuItem(hMenu, IDS_STACK_CHILDH_WNDS, nEnableX);
         ::EnableMenuItem(hMenu, IDS_CLOSE_CHILD_WNDS , nEnable);
      } break;
      case WM_OPENGL_STATE:
      if (wParam == 1)
      {
         Sleep(1000);
         if (m_hwndChildDlg)
         {
            OFNOTIFY ofnNotify;
            ofnNotify.hdr.hwndFrom = m_hWnd;
            ofnNotify.hdr.idFrom   = 0;
            ofnNotify.hdr.code     = CDN_SELCHANGE;
            ofnNotify.lpOFN        = NULL;
            ofnNotify.pszFile      = NULL;
            ::SendMessage(m_hwndChildDlg, WM_NOTIFY, 0, (LPARAM) &ofnNotify);
         }
      }  break;
      case WM_ETSVIEW_CLOSED:
      {
         PROCESS_INFORMATION pi;
         pi.dwProcessId = lParam;
         int nElem = m_ProcessHandles.FindElement(FindProcessID, &pi);
         if (nElem != -1)
         {
            m_ProcessHandles.Delete(m_ProcessHandles.GetAt(nElem));
         }
      } break;
      case WM_WINDOWPOSCHANGED:
      {
         WINDOWPOS *pWp = (WINDOWPOS*)lParam;
         if (pWp->flags & SWP_HIDEWINDOW)
         {
            HICON hIcon =(HICON) SendMessage(m_hWnd, WM_SETICON, ICON_BIG, NULL);
            if (hIcon) ::DestroyIcon(hIcon);
         }
      }
   }
   return CEtsPropertySheet::OnMessage(uMsg, wParam, lParam);
}

bool CBoxPropertySheet::SaveBDDFile(char *pszFilePath)
{
   bool bReturn = false;
   try
   {
      ASSERT(m_hFile == INVALID_HANDLE_VALUE);
      CloseFile();
      m_hFile = CreateFile(pszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
      if (m_hFile == INVALID_HANDLE_VALUE) throw (DWORD) ERR_INVALID_HANDLE;
      CreateCaraBDD(1);
      SetEndOfFile(m_hFile);
      bReturn = true;
   }
   catch (DWORD nError)
   {
      char text[128];
      wsprintf(text, "SaveBDDFile Error %0x", nError);
      GetActivePage()->MessageBox((long)text, IDS_PROPSHCAPTION, MB_OK|MB_ICONERROR|MBU_TEXTSTRING);
   }
   CloseFile();
   return bReturn;
}

bool CBoxPropertySheet::OnWizardFinish(bool bRequest)
{
   if (CheckCurrentPage() && UpdateZeroSPL())
   {
      char szPathDir[MAX_PATH];
      GetCARADirectory(MAX_PATH, szPathDir);
      strcat(szPathDir,"\\LS_BOX");
      bool bOk = true;
      if ((m_BoxText.szFilename[0] == 0) || (m_nFlagOk & CARABOX_SAVE_AS))
      {
         CBoxFileDlg pfd(g_hInstance, 0, m_hWnd);
         pfd.SetInitialDir(szPathDir);
         pfd.SetFilter(IDS_BOX_FILTER);
         pfd.ModifyFlags(OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR, OFN_SHOWHELP);
         pfd.SetDefaultExt("BOX");
         if (m_BoxText.szFilename[0]!=0) pfd.SetFile(m_BoxText.szFilename);
         else                            pfd.SetFile(m_BoxText.szDescrpt);
         if (pfd.GetSaveFileName())
         {
            strncpy(m_BoxText.szFilename, pfd.GetFileTitle(), 256);
            int i, nLen = strlen(m_BoxText.szFilename);
            for (i=nLen-1; i>=0; i--)
            {
               if (m_BoxText.szFilename[i] == '.')
               {
                  m_BoxText.szFilename[i] = 0;
                  break;
               }
            }
         }
         else bOk = false;
         m_hwndChildDlg = NULL;
      }

      if (!(m_nFlagChanged & (CARABOX_ALL_DATA|CARABOX_SAVE_DATA)) && !(m_nFlagOk & CARABOX_SAVE_AS))
         bOk = false;

      if (bOk && (m_BoxText.szFilename[0] != 0))
      {
         char szFilePath[MAX_PATH*2];
         if (m_nFlagOk & CARABOX_SPL_ZERO)
         {
            if (m_nLSType & ACTIVE_LS)                         // bei aktiven Filtern darf die Filterfunktion
            {                                                  // nicht in den berechneten Daten enthalten sein
               m_nFlagChanged |= CARABOX_FILTER_BDD;
               CalcTransferFunc();                             // Filterfunktion ohne Anpassungen neu berechnen
               m_nFlagChanged &= ~CARABOX_FILTER_BDD;
               m_nFlagOk &= ~CARABOX_BDD_DATA;                 // BDD-Daten neu berechnen
            }
            if (!(m_nFlagOk&CARABOX_BDD_DATA))
            {
               CalcInThread(CARABOX_BDD_DATA);
            }
            if (m_nFlagOk & CARABOX_BDD_DATA)
            {
               CalcInThread(CARABOX_SAVE_DATA);
            }
         }
         BOOL bBDDFileDeleted = false;
         if (!(m_nFlagOk & CARABOX_BDD_DATA))
         {
            wsprintf(szFilePath, "%s\\%s.BDD", szPathDir, m_BoxText.szFilename);
            bBDDFileDeleted = ::DeleteFile(szFilePath);
            if (bBDDFileDeleted)
            {
               char szText[256];
               char szFormat[64];
               ::LoadString(g_hInstance, IDS_BDD_DELETED, szFormat, 64);
               wsprintf(szText, szFormat, m_BoxText.szFilename);
               GetActivePage()->MessageBox((long)szText, IDS_PROPSHCAPTION, MB_ICONINFORMATION|MB_OK|MBU_TEXTSTRING);
               m_nFlagOk |= CARABOX_BDD_DELETED;
            }
         }

         if (m_nLSType & ACTIVE_LS)                            // Aktiver Filter :
         {
            m_nFlagOk &= ~(CARABOX_BDD_DATA|CARABOX_SPL_ZERO); // Schalldruckdaten neu berechnen
         }

         wsprintf(szFilePath, "%s\\%s.BOX", szPathDir, m_BoxText.szFilename);
         SaveBoxData(szFilePath);
      }
   }
   if (bRequest)
   {
      CEtsPropertyPage *pDlg = GetActivePage();
      return (pDlg->MessageBox(IDS_CLOSE_BOXEDITOR, IDS_PROPSHCAPTION, MB_ICONQUESTION|MB_YESNO)==IDYES) ? true : false;
   }
   return false;
}

bool CBoxPropertySheet::SaveBoxData(char *pszPathName)
{
   bool bReturn = false;
   try
   {
      ASSERT(m_hFile == INVALID_HANDLE_VALUE);
      CloseFile();
      m_hFile = CreateFile(pszPathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
      if (m_hFile == INVALID_HANDLE_VALUE) throw (DWORD) ERR_INVALID_HANDLE;
      WriteBoxFile();
      m_nFlagChanged &= ~CARABOX_SAVE_DATA;
      bReturn = true;
   }
   catch (DWORD nError)
   {
      if (nError == ERR_INVALID_HANDLE)
      {
         CEtsPropertyPage *pDlg = GetActivePage();
         pDlg->MessageBox(IDS_ERROR_FILE_CREATION, IDS_PROPSHCAPTION, MB_OK|MB_ICONEXCLAMATION);
      }
      else
      {
         char text[128];
         wsprintf(text, "Save BOX file Error %0x", nError);
         GetActivePage()->MessageBox((long)text, IDS_PROPSHCAPTION, MB_OK|MB_ICONERROR|MBU_TEXTSTRING);
      }
      REPORT("SaveBoxData Error %0x", nError);
   }
   CloseFile();
   return bReturn;
}

void CBoxPropertySheet::OnBtnClckSaveAs()
{
   m_nFlagOk |= (CARABOX_SAVE_AS|CARABOX_PASS_CHECK);
   OnWizardFinish(false);
   m_nFlagOk &= ~(CARABOX_SAVE_AS|CARABOX_PASS_CHECK);
}

void CBoxPropertySheet::OnBtnClckDownLoad()
{
   ::ShellExecute(GetHWND(), "open", "http://www.ehlers.de/index.php", NULL, NULL, SW_SHOW);
/*
   HINTERNET hIS = NULL,
             hIC = NULL,
             hIF = NULL;
   char      szPathDir[MAX_PATH];
   try
   {
      hIS = InternetOpen("FTPGET", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
      if (hIS == NULL) throw GetLastError();
      hIC = InternetConnect(hIS, "http://www.ehlers.de/", INTERNET_DEFAULT_FTP_PORT, NULL, NULL, INTERNET_SERVICE_FTP, 0, 0);
      if (hIC == NULL) throw GetLastError();
      GetCARADirectory(MAX_PATH, szPathDir);
      strcat(szPathDir,"\\LS_BOX\\DOEDEL.txt");
      BOOL bResult = FtpGetFile(hIC, "doedel.txt", szPathDir, false, FILE_ATTRIBUTE_ARCHIVE, INTERNET_FLAG_TRANSFER_BINARY , 0);
   }
   catch (DWORD dwError)
   {
      char text[128];
      char szError[MAX_PATH] = "";
      DWORD dwChars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN), szError, MAX_PATH,  NULL);
      if (dwChars == 0)
      {
         szError[0] = 0;
//         dwChars = MAX_PATH;
//         InternetGetLastResponseInfo(&dwError, szError, &dwChars);
      }

      wsprintf(text, "Download Error %0x\n%s", dwError, szError);
      GetActivePage()->MessageBox((long)text, IDS_PROPSHCAPTION, MB_OK|MB_ICONERROR|MBU_TEXTSTRING);
   }
   if (hIF) InternetCloseHandle(hIF);
   if (hIC) InternetCloseHandle(hIC);
   if (hIS) InternetCloseHandle(hIS);
*/
}

void CBoxPropertySheet::OnBtnClckUpLoad()
{

}

bool CBoxPropertySheet::CheckCurrentPage()
{
   HWND hWndFocus = ::GetFocus();
   ASSERT(::IsWindow(hWndFocus));
   //              hWndFrom, nIdFrom                  , code,         lParam
   PSHNOTIFY psn = {{m_hWnd, ::GetDlgCtrlID(hWndFocus), PSN_WIZNEXT}, 0};
   if (m_nFlagOk & CARABOX_PASS_CHECK) psn.hdr.code = PSN_WIZBACK;
   HWND hWndCurrent = PropSheet_GetCurrentPageHwnd(m_hWnd);
   if (::IsWindow(hWndCurrent))
      ::SendMessage(hWndCurrent, WM_NOTIFY, 0, (LPARAM)&psn);

   if (m_nFlagOk & CARABOX_PASS_CHECK) return true;

   return (psn.lParam!=0) ? true : false;
}

bool CBoxPropertySheet::RequestSaveData()
{
   bool bOk = CheckAllPages();
   if ((m_nFlagOk != 0) && (m_nFlagChanged & CARABOX_SAVE_DATA))
   {
      CEtsPropertyPage *pDlg = GetActivePage();
      if (bOk && ((m_nFlagOk & CARABOX_ALL_DATA) == CARABOX_ALL_DATA))
      {
         int nIDQuestion = IDS_SAVE_CONSISTENT_DATA;
         if (!(m_nFlagOk & CARABOX_BDD_DATA) && (m_nFlagChanged & CARABOX_BDD_DATA))
         {
            nIDQuestion = IDS_CREATE_BDD_DATA;
         }
         if (pDlg->MessageBox(nIDQuestion, IDS_PROPSHCAPTION, MB_ICONQUESTION|MB_YESNO) == IDYES)
         {
            OnWizardFinish(false);
            return ((m_nFlagOk & CARABOX_BDD_DATA) && !(m_nFlagChanged & CARABOX_SAVE_DATA)) ? true : false;
         }
         return true;
      }
      else
      {
         m_nFlagOk &= ~(CARABOX_SPL_ZERO|CARABOX_BDD_DATA);
         switch (pDlg->MessageBox(IDS_NONCONSISTENT_DATA, IDS_PROPSHCAPTION, MB_ICONQUESTION|MB_YESNOCANCEL))
         {
            case IDYES:
               m_nFlagOk |= CARABOX_PASS_CHECK;
               OnWizardFinish(false);
               m_nFlagOk &= ~CARABOX_PASS_CHECK;
               break;
            case IDNO: break;
            case IDCANCEL:
            return false;
         }
         return true;
      }
   }
   return true;
}

void CBoxPropertySheet::StackProcessWindows(int nHow)
{
   WINDOWPLACEMENT wp;
   wp.length = sizeof(WINDOWPLACEMENT);
   ::GetWindowPlacement(m_hWnd, &wp);
   wp.showCmd |= MAKELONG(SW_RESTORE, nHow);
   wp.flags = (UINT)m_hWnd;
   if (nHow==0)      // hintereinander
   {
      wp.rcNormalPosition.right  = wp.rcNormalPosition.right  - wp.rcNormalPosition.left;
      wp.rcNormalPosition.bottom = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
      m_ProcessHandles.ProcessWithObjects(CBoxPropertySheet::EnumProcesses, 0, (LPARAM)&wp);
   }
   else
   {
      wp.rcNormalPosition = GetActivePage()->AdaptMonitorPoints((POINT*)&wp.rcNormalPosition, 0);
      int nCount = m_ProcessHandles.GetCounter();
      if (nHow == 1) // vertikal
      {
         wp.length = (wp.rcNormalPosition.right - wp.rcNormalPosition.left) / nCount;
         wp.rcNormalPosition.left = 0;
         m_ProcessHandles.ProcessWithObjects(CBoxPropertySheet::EnumProcesses, 0, (LPARAM)&wp);
      }
      else           // horizontal
      {
         wp.length = (wp.rcNormalPosition.bottom - wp.rcNormalPosition.top) / nCount;
         wp.rcNormalPosition.top = 0;
         m_ProcessHandles.ProcessWithObjects(CBoxPropertySheet::EnumProcesses, 0, (LPARAM)&wp);
      }
   }
}

int CBoxPropertySheet::EnumProcesses(void *ptr, WPARAM, LPARAM lParam)
{
   PROCESS_INFORMATION *pP = (PROCESS_INFORMATION*)ptr;
   if (pP->hProcess != NULL)
   {
      DWORD   exitcode;
      if(GetExitCodeProcess(pP->hProcess, &exitcode))
      {
         if(exitcode == STILL_ACTIVE)
         {
            EnumThreadWindows(pP->dwThreadId, CBoxPropertySheet::EnumWindows, lParam);
         }
         else
         {
            CloseHandle(pP->hProcess);
            pP->hProcess = NULL;
            if (pP->hThread)
            {
               CloseHandle(pP->hThread);
               pP->hThread = NULL;
            }
            return true;
         }
      }
   }

   return true;
}

BOOL CALLBACK CBoxPropertySheet::EnumWindows(HWND hwnd, LPARAM lParam)
{
   if (::IsWindow(hwnd))
   {
      HWND hwndParent = ::GetParent(hwnd);
//      if (!::IsRectEmpty(&wp.rcNormalPosition) && lParam)
      if ((hwndParent == NULL) && lParam)
      {
#ifdef _DEBUG
         char szText[256];
         ::GetWindowText(hwnd, szText, 256);
#endif
         WINDOWPLACEMENT wp;
         WINDOWPLACEMENT *pwp = (WINDOWPLACEMENT*)lParam;
         wp.length = sizeof(WINDOWPLACEMENT);
         ::GetWindowPlacement(hwnd, &wp);
         wp.showCmd = MAKELONG(LOWORD(pwp->showCmd), 0);
         ::SetForegroundWindow(hwnd);                          // in den Vordergrund stellen
         if      (HIWORD(pwp->showCmd) == 1)                   // vertikal (nebeneinander)
         {
            pwp->rcNormalPosition.right = pwp->rcNormalPosition.left+pwp->length;
            wp.rcNormalPosition = pwp->rcNormalPosition;
            ::SetWindowPos(hwnd, HWND_TOP, wp.rcNormalPosition.left, wp.rcNormalPosition.top,
                         wp.rcNormalPosition.right-wp.rcNormalPosition.left,// width
                         wp.rcNormalPosition.bottom-wp.rcNormalPosition.top,// height
                         SWP_SHOWWINDOW|SWP_ASYNCWINDOWPOS);
            pwp->rcNormalPosition.left  = pwp->rcNormalPosition.right;
         }
         else if (HIWORD(pwp->showCmd) == 2)                   // horizontal (untereinander)
         {
            pwp->rcNormalPosition.bottom = pwp->rcNormalPosition.top+pwp->length;
            wp.rcNormalPosition = pwp->rcNormalPosition;
            ::SetWindowPos(hwnd, HWND_TOP, wp.rcNormalPosition.left, wp.rcNormalPosition.top,
                         wp.rcNormalPosition.right-wp.rcNormalPosition.left,// width
                         wp.rcNormalPosition.bottom-wp.rcNormalPosition.top,// height
                         SWP_SHOWWINDOW|SWP_ASYNCWINDOWPOS);
            pwp->rcNormalPosition.top    = pwp->rcNormalPosition.bottom;
         }
         else                                                  // überlappend
         {
            POINT ptOffset = {pwp->rcNormalPosition.left-wp.rcNormalPosition.left,pwp->rcNormalPosition.top-wp.rcNormalPosition.top};
            ::OffsetRect(&wp.rcNormalPosition, ptOffset.x+20, ptOffset.y+20);
            ::SetWindowPos(hwnd, HWND_TOP, wp.rcNormalPosition.left, wp.rcNormalPosition.top,
                         pwp->rcNormalPosition.right,          // width
                         pwp->rcNormalPosition.bottom,         // height
                         SWP_SHOWWINDOW|SWP_ASYNCWINDOWPOS);
            pwp->rcNormalPosition.top  = wp.rcNormalPosition.top;
            pwp->rcNormalPosition.left = wp.rcNormalPosition.left;
         }
         return false;                                         // nur das Hauptwindow setzen
      }
   }
   return (hwnd != NULL) ? true : false;
}

int CBoxPropertySheet::SetPaintCtrlWnd(HWND hwndDlg, int nCtrl, int nToCtrl)
{
   HWND hwndGrp    = ::GetDlgItem(hwndDlg, nCtrl);
   HWND hwndToCtrl = ::GetDlgItem(hwndDlg, nToCtrl);
   ::SetWindowLong(hwndGrp, GWL_USERDATA, (LONG)hwndToCtrl);
   long lOldProc = ::SetWindowLong(hwndGrp, GWL_WNDPROC, (LONG)PaintCtrlSubClassProc);
   if (gm_lOldGrpCtrlSCProc == 0)
   {
      ASSERT(gm_lOldGrpCtrlCount == 0);
      gm_lOldGrpCtrlSCProc = lOldProc;
   }
   else
   {
      ASSERT(lOldProc == gm_lOldGrpCtrlSCProc);
   }

   return gm_lOldGrpCtrlCount++;
}

LRESULT CALLBACK CBoxPropertySheet::PaintCtrlSubClassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   LRESULT lR;
   switch (uMsg)
   {
      case WM_PAINT:
         lR = CallWindowProc((WNDPROC)gm_lOldGrpCtrlSCProc, hwnd, uMsg, wParam, lParam);
         ::InvalidateRect((HWND) ::GetWindowLong(hwnd, GWL_USERDATA), NULL, true);
         ::UpdateWindow((HWND) ::GetWindowLong(hwnd, GWL_USERDATA));
         return lR;
      case WM_DESTROY:
         ::SetWindowLong(hwnd, GWL_WNDPROC, gm_lOldGrpCtrlSCProc);
         gm_lOldGrpCtrlCount--;
         break;
   }
   lR = CallWindowProc((WNDPROC)gm_lOldGrpCtrlSCProc, hwnd, uMsg, wParam, lParam);
   if (gm_lOldGrpCtrlCount <= 0)
   {
      gm_lOldGrpCtrlSCProc = NULL;
      gm_lOldGrpCtrlCount  = 0;
   }
   return lR;
}

int CBoxPropertySheet::FindProcessID(const void *p1, const void *p2)
{
   if      (((PROCESS_INFORMATION*)p1)->dwProcessId == ((PROCESS_INFORMATION*)p2)->dwProcessId) return  0;
   else if (((PROCESS_INFORMATION*)p1)->dwProcessId >  ((PROCESS_INFORMATION*)p2)->dwProcessId) return  1;
   else                                                                                         return -1;
}

bool CBoxPropertySheet::LoadBoxData(const char *pszFileName)
{
   bool bReturn = false;
   try
   {
      ASSERT(m_hFile == INVALID_HANDLE_VALUE);
      CloseFile();
      m_hFile = CreateFile(pszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);
      if (m_hFile == INVALID_HANDLE_VALUE) throw (DWORD) ERR_INVALID_HANDLE;
      bReturn = ReadBoxFile(pszFileName);
   }
   catch(DWORD nError)
   {
      UINT uType = MB_OK|MB_ICONERROR;
      char text[64];
      switch (nError)
      {
         case FILEERR_READ: nError = IDS_ERROR_FILEREAD; break;
         case FILEERR_TYPE: nError = IDS_ERROR_FILETYPE; break;
         case FILEERR_CRC:  nError = IDS_ERROR_FILECRC;  break;
         default :
            wsprintf(text, "LoadBoxData() Exception Error: %0x", nError);
            nError = (DWORD)text;
            uType |= MBU_TEXTSTRING;
            break;
      }
      GetActivePage()->MessageBox(nError, IDS_PROPSHCAPTION, uType);
      InitData();
      m_ChassisData.Destroy();
//         REPORT("OnBtnClckOpen Error %0x", nError);
   }
   CloseFile();
   m_nFlagOk &= ~CARABOX_BDD_DATA; // Flag löschen, da noch nicht berechnet

   return bReturn;
}

void CBoxPropertySheet::SetWizardButtons(bool bBack, bool bNext)
{
   bool bFinish = ((m_nFlagOk&CARABOX_ALL_DATA) == CARABOX_ALL_DATA) ? true : false;
   HWND hwndBack   = GetWndBackBtn();
   HWND hwndNext   = GetWndNextBtn();
   HWND hwndFinish = GetWndFinishBtn();
//   HWND hwndDef    = NULL;
   EnableWindow(hwndBack, bBack);
   EnableWindow(hwndNext, bNext);
   EnableWindow(hwndFinish, bFinish);
/*
   if      (bNext)   hwndDef = hwndNext;
   else if (bBack)   hwndDef = hwndBack;
//   if (hwndDef == NULL) hwndDef = ::GetDlgItem(m_hWnd, IDCANCEL);
   ::SendMessage(m_hWnd, DM_SETDEFID, ::GetDlgCtrlID(hwndDef), 0);
*/
}

int CBoxPropertySheet::GetFileVersion()
{
   if (m_pFH) return m_pFH->GetVersion();
   return 0;
}

FilterBox* CBoxPropertySheet::GetOneWayFilter()
{
   switch (LOWORD(m_nLSType))
   {
      case ADD_ON1: return &m_BasicData.sTT1; break;            // Subwoofer, obere Grenzfrequenz
      case ADD_ON3: return &m_BasicData.sHT;  break;            // Hochtöner, untere Grenzfrequenz
//    case ADD_ON2:                           break;            // Mitteltöner, untere oder obere Grenzfrequenz
      default:      return &m_BasicData.sTT1; break;            // Stand Alone : Breitband = TT1
   }
}

void CBoxPropertySheet::CalcReferencePoints()
{
   ChassisData *pCD;
   double    dTemp,
             dMaxDipol = 1000,
             dMin      = m_Cabinet.GetBottomZPos(),
             dMax      = Vz(m_Cabinet.GetCabPoint(CAB_PT_LEFT_TOP_FRONT)),
             dBase     = dMin,
             dHeightHT = 0;
   CCabinet *pCmax = &m_Cabinet, *pCmin = &m_Cabinet;
   CVector2D vrcWall[NO_OF_RECT_POINTS];
   CVector   vNorm, vZ(0.0, 0.0, 1.0);
   int ncount = 0,
       nWall  = CARABOX_CABWALL_FRONT;

   if (m_Cabinet.m_Cabinets.GetCounter())                      // sind weitere Gehäusesegmente vorhanden
   {                         
      CCabinet*pC = (CCabinet*)m_Cabinet.m_Cabinets.GetFirst();
      while (pC)
      {  
         dTemp =  Vz(pC->GetCabPoint(CAB_PT_LEFT_BOTTOM_FRONT, true));
         if (dTemp < dMin)                                     // kann die Maximalhöhe größe
         {
            dMin  = dTemp;
            pCmin = pC;
         }
         dTemp =  Vz(pC->GetCabPoint(CAB_PT_LEFT_TOP_FRONT, true));
         if (dTemp > dMax)                                     // und der Referenzpunkt niedriger sein
         {
            dMax  = dTemp;
            pCmax = pC;
         }
         ASSERT(fabs(Vy(pC->vUserRef)) < 1e-13);               // Boden ist nicht schräg bei Theta = 0
         pC = (CCabinet*)m_Cabinet.m_Cabinets.GetNext();
      }
   }
   if (dBase != dMin)
   {
      dMax      += (dBase - dMin);                                // dMax ist noch vom Basisgehäuse aus gerechnet
      dMaxDipol -= (dBase - dMin);
   }

   if (m_nLSType & DIPOLE_FLAT)                                // Flächenstrahler
   {
      ncount = 1;
      for (pCD=(ChassisData*)m_ChassisData.GetFirst(); pCD!=NULL; pCD=(ChassisData*)m_ChassisData.GetNext())
      {
         if ((m_BasicData.nXoverTyp == CARABOX_CO_1W )||      // 1-Weg Breitband Dipol => TT1
             (pCD->nChassisTyp==CARABOX_CHASSISTYP_HT))       // Mehrweg Dipol immer den Hochtöner nehmen
         {                                                    // das höchste Chassis, das unter 1.0 m ist
            for (int i=pCD->GetFirstDummy(); i<pCD->GetNoOfCoupled(); i++)
            {
               dTemp = Vz(pCD->GetCoupled(i)->vPosChassis)-dBase;// Basisgehäuse als Bezugspunkt
               if ((dTemp > dHeightHT) && (dTemp < dMaxDipol))
               {
                  dHeightHT = dTemp;
               }
            }
            break;
         }
      }
   }
   else if (m_nLSType & DIPOLE_FLAT_H)                         // Flächenstrahler Hybrid
   {
      ChassisData* pCDfound = NULL;
      pCD=(ChassisData*)m_ChassisData.GetFirst();
      while (pCD!=NULL)
      {
         if (pCD->IsDummy()) break;                            // beim ersten Dummy stop
         if (pCD->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE)    // Dipole
         {
            if (pCDfound)                                      // schon einer gefunden
            {                                                  // den höchsten Chassistyp nehmen
               if (pCD->nChassisTyp > pCDfound->nChassisTyp) pCDfound = pCD;
            }
            else if (pCD->nChassisTyp >= CARABOX_CHASSISTYP_MT1)// ab MT1 nehmen
            {
               pCDfound = pCD;
            }
         }
         pCD=(ChassisData*)m_ChassisData.GetNext();
      }
      if (pCDfound)
      {
         for (int i=pCDfound->GetFirstDummy(); i<pCDfound->GetNoOfCoupled(); i++)
         {
            dTemp = Vz(pCDfound->GetCoupled(i)->vPosChassis)-dBase;// Basisgehäuse als Bezugspunkt
            if ((dTemp > dHeightHT) && (dTemp < dMaxDipol))
            {
               dHeightHT = dTemp;
               ncount    = 1;
            }
         }
      }
   }
   if (ncount == 0)                                            // nicht Dipol-Flächenstrahler
   {
      for (pCD=(ChassisData*)m_ChassisData.GetFirst(); pCD!=NULL; pCD=(ChassisData*)m_ChassisData.GetNext())
      {
         if (pCD->IsDummy()) break;
         m_Cabinet.CalcChassisPos3D(pCD);
         if (pCD->nChassisTyp == CARABOX_CHASSISTYP_HT)        // Hochtöner
         {
            dHeightHT += (Vz(pCD->vPosChassis) - dBase);
            ncount++;
         }
      }
   }

   if      (m_Cabinet.m_dwFlags & ZERO_DEG_REF_LEFT ) nWall = CARABOX_CABWALL_LEFT;
   else if (m_Cabinet.m_dwFlags & ZERO_DEG_REF_BACK ) nWall = CARABOX_CABWALL_BACK;
   else if (m_Cabinet.m_dwFlags & ZERO_DEG_REF_RIGHT) nWall = CARABOX_CABWALL_RIGHT;

   if      (ncount >  1) dHeightHT /= (double)ncount;
   else if (ncount == 0) dHeightHT = m_Cabinet.dHeight * 0.5;

   if (dHeightHT > dMax) dHeightHT = dMax;                     // nicht höher als die Box

   m_Cabinet.GetCabinetWall(nWall, NULL, &vNorm);              // Vorderwand schräg ?
   dHeightHT /= Sinus(vNorm, vZ);                              // auf die 2D-Wand projizieren

   m_Cabinet.GetCabinetWall(nWall, vrcWall);
   CalcInverseTran();

   double dCenterLine = CCabinet::GetXCenterLine(vrcWall);
   m_Cabinet.vPosRef.SetX()  = dCenterLine;                    // 2D-Koordinaten (x, y)
   m_Cabinet.vPosRef.SetY()  = dHeightHT;
   m_Cabinet.vPosRef.SetZ()  = 0.0;
   ReTransformVector(m_Cabinet.vPosRef);                       // 3D Koordinaten (x, y, z)

   if (dMin == dBase)
   {
      m_Cabinet.vUserRef.SetX() = dCenterLine;                 // 2D-Koordinaten (x, y)
      m_Cabinet.vUserRef.SetY() = 0;
      m_Cabinet.vUserRef.SetZ() = 0;
      ReTransformVector(m_Cabinet.vUserRef);                   // 3D Koordinaten (x, y, z)
      if (g_nUserMode & USERMODE_USER_REF)
         SendMsgTo3DDlg(WM_COMMAND, MAKELONG(IDC_BOX_VIEW, LIST_USER_REF_PT), (LPARAM)m_hWnd);
   }
   else
   {
      ZeroMemory(&m_Cabinet.vUserRef, sizeof(m_Cabinet.vUserRef));
   }
}

void CBoxPropertySheet::SetChanged(int nChanged)
{
   m_nFlagChanged |= (nChanged|CARABOX_SAVE_DATA);
}

unsigned int CBoxPropertySheet::CalcThread(void *p)
{
   CBoxPropertySheet *pThis = (CBoxPropertySheet*) p;

   if (pThis->m_nCalcWhat == CARABOX_BDD_DATA)
   {
      if (pThis->CalcCaraBDD())
         pThis->m_nFlagOk |= CARABOX_BDD_DATA;
   }
   else if (pThis->m_nCalcWhat == CARABOX_SAVE_DATA)
   {
      char szPathDir[MAX_PATH];
      char szFilePath[MAX_PATH];
      pThis->GetCARADirectory(MAX_PATH, szPathDir);
      wsprintf(szFilePath, "%s\\LS_BOX\\%s.BDD", szPathDir, pThis->m_BoxText.szFilename);
      if (!pThis->SaveBDDFile(szFilePath))
      {
         pThis->m_nFlagOk &= ~CARABOX_BDD_DATA;
      }
   }
   else if (pThis->m_nCalcWhat == CARABOX_ADOPT_EFFIC)
   {
      pThis->AdaptEffic();
   }
   pThis->CloseCalcThreadHandle();
   ::PostMessage(pThis->m_hwndChildDlg, WM_COMMAND, IDOK, 0);
   return 0;
}

bool CBoxPropertySheet::CalcInThread(int nWhat)
{
   unsigned int nThreadID;
   bool bReturn = true;
   ASSERT(m_hCalcThread == INVALID_HANDLE_VALUE);
   m_nCalcWhat   = nWhat;
   m_hCalcThread = (HANDLE) _beginthreadex(NULL, 0, CBoxPropertySheet::CalcThread, (void*)this, CREATE_SUSPENDED, &nThreadID);
   CAutoDisable ad(m_p3DViewDlg->GetWindowHandle());
   CProgressDlg dlg(g_hInstance, IDD_PROGRESS_VIEW, m_hWnd);
   dlg.SetPropertySheet(this);
   dlg.DoModal();
   if (m_nCalcWhat & TERMINATE_THREAD_SAVE)
      bReturn = false;
   m_nCalcWhat = 0;
   return bReturn;
}

void CBoxPropertySheet::CloseCalcThreadHandle()
{
   if (m_hCalcThread != INVALID_HANDLE_VALUE)
   {
      ::CloseHandle(m_hCalcThread);
      m_hCalcThread = INVALID_HANDLE_VALUE;
   }
}

void CBoxPropertySheet::TerminateThreadSave()
{
   m_nCalcWhat |= TERMINATE_THREAD_SAVE;
}

void CBoxPropertySheet::SetAdoptDistance()
{
   if (m_nLSType & (DIPOLE_FLAT|DIPOLE_FLAT_H))
   {
      m_dDistance = 10.0;
   }
   else
   {
      m_dDistance = 1.0;
   }
}

void CBoxPropertySheet::SetVersionText(HWND hwnd, long nID, long nVersion)
{
   if (nVersion)
   {
      char szText[32];
      if      (nVersion == 100) nVersion = 110;
      else if (nVersion == 110) nVersion = 200;
      wsprintf(szText, "%d.%02d", nVersion/100, nVersion%100);
      ::SetDlgItemText(hwnd, nID, szText);
   }
   else ::SetDlgItemText(hwnd, nID, "");
}

int CBoxPropertySheet::GetChassisTypeString(ChassisData *pCD, char *pszText, int nLen)
{
   int nIDStr = IDS_PM + pCD->nChassisTyp;
   if ((m_BasicData.nXoverTyp == CARABOX_CO_1W) && 
       (LOWORD(m_nLSType) == STAND_ALONE) &&
       (pCD->nChassisTyp == CARABOX_CHASSISTYP_TT1)) nIDStr = IDS_BB;
   return ::LoadString(g_hInstance, nIDStr, pszText, nLen);
}

CCabinet* CBoxPropertySheet::GetCurrentCab()
{
   CCabinet*pC = m_Cabinet.GetCabinet(m_nCurrentCab);
   return pC;
}

CCabinet* CBoxPropertySheet::GetCabinet(int nCab)
{
   CCabinet*pC = m_Cabinet.GetCabinet(nCab);
   return pC;
}


LRESULT CBoxPropertySheet::SendMsgTo3DDlg(UINT nCmd, WPARAM wParam, LPARAM lParam)
{
   HWND hwnd = m_p3DViewDlg->GetWindowHandle();
   if (hwnd)
   {
      return ::SendMessage(hwnd, nCmd, wParam, lParam);
   }
   return 0;
}

void CBoxPropertySheet::InitSphereFoot(ChassisData *pCD)
{
   ASSERT(pCD != NULL);
   CVector vNorm;
   if (pCD->GetWall() == CARABOX_CABWALL_TOP)
   {
      if (pCD->m_ppCabinets == NULL)
      {
         CCabinet *pCnew   = new CCabinet;
         pCD->m_ppCabinets = (CCabinet**) Alloc(sizeof(CCabinet*), NULL);
         pCD->m_ppCabinets[0] = pCnew;
         pCnew->SetBaseCabinet(&m_Cabinet);          // Basisgehäuse setzen
         pCnew->dHeight  = 40;                       // Höhe ist 40 mm
         pCnew->dDepth   = pCD->dEffDiameter * 0.25; // Tiefe und Breite
         pCnew->dWidth   = pCnew->dDepth;  // 25 % des Kugedurchmessers
         pCnew->nCountHT = 1;                        // Gehäuse besitzt eine Kugel (HT)
         pCnew->nCountPM = pCD->nCabWall;            // Gehäusewand des Chassis
         pCnew->InitCabPoints();                     // Punkte initialisieren
         pCnew->m_dwFlags |=  CANNOT_CONTAIN_CHASSIS;
         pCnew->m_dwFlags &= ~CAN_CONTAIN_CHASSIS;
//         pCnew->EnableWall(CARABOX_CABWALL_TOP   , false);
         pCnew->EnableWall(CARABOX_CABWALL_BOTTOM, false);
         pCD->dwFlagsCH |= CHASSIS_3D_POS_INVALID;
         m_Cabinet.GetCabinet(pCD->GetCabinet())->InitCabPoints();
         m_Cabinet.CalcChassisPos3D(pCD);
      }
      ::LoadString(g_hInstance, IDS_HOLDER_OF, pCD->m_ppCabinets[0]->m_szName, CAB_NAME_LEN);
      strncat(pCD->m_ppCabinets[0]->m_szName, pCD->szDescrpt, CAB_NAME_LEN);
   }
}

bool CBoxPropertySheet::IsSectionChangeable()
{
   return ((m_nCurrentCab != 0) && (m_nCurrentCab != m_nNoMovements)) ? true : false;
}
