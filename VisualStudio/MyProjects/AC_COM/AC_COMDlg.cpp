// AC_COMDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "AC_COM.h"
#include "AC_COMDlg.h"
#include "CCaraInfo.h"
#include "Curve.h" 
#include "Plot.h"
#include "FileHeader.h"
#include "ETS3DGLRegKeys.h"
#include "AudioControlData.h"
#include "IRcodeDlg.h"
#include "MsgBoxBehaviour.h"
#include "CEtsCDPlayer.h"
#include "EtsBind.h"
#include "CEtsDlg.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// int g_nCol = COLOR_ACTIVECAPTION;
HBRUSH    g_hBrush = NULL;
COLORREF  g_cCol   = 0;
/////////////////////////////////////////////////////////////////////////////
// CAC_COMDlg Dialogfeld
#define PARAM_VALUES_SIZE           76

#define SEND_GETDATA               "@R"      // Kommunikations Strings
#define SEND_PUTDATA               "@C"

#define AC_FILETYPE  "ACD"

#define SAVED_CHANGED  0x01
#define SENT_CHANGED   0x02
#define CHANGED        SAVED_CHANGED|SENT_CHANGED

#define WM_FROM_2D_VIEW (WM_APP +1)

#define PORT_ERROR_NO          0
#define PORT_ERROR_CHECKSUM    1
#define PORT_ERROR_READ_WRITE  2
#define PORT_ERROR_ALPHASIGN   3
#define PORT_ERROR_OPEN        4
#define PORT_ERROR_ACKNOWLEDGE 5
#define PORT_ERROR_NO_DATA     6
#define PORT_ERROR_OK          7

#define DEFAULT_CD_PLACE  (EXIT_SIZE_MOVE_SET|EXIT_SIZE_MOVE_BOTTOM|EXIT_SIZE_MOVE_FORCE|EXIT_SIZE_MOVE_DOCK)
/*
RC5-Code gilt auch für folgende CD-Player
Arcam
Grundig
Linn
Matsui
Meridian
Micromega
Mission
Namim
Proton
Quad
Radmor
Revox/Studer
Sae
Sansui
Universum

Marantz (RC-10CD) Index = 10 ?
*/

#define FIRST_TAB_CTRL  1

int  CAC_COMDlg::gm_pnTabSystemConfig[AC_SYSTEMCONFIG_TABS][AC_SYSTEMCONFIG_CTRLS] =
{
   {IDS_LEVEL_DISPLAY     , IDC_AC_R_DIGITAL            , IDC_AC_R_ANALOG               , 0                      , IDC_AC_R_DIGITAL},
   {IDS_LOUDSPEAKER_CONFIG, IDC_AC_R_STEREO             , IDC_AC_R_SURROUND             , 0                      , IDC_AC_R_STEREO},
   {IDS_RT60_MEASURE      , IDC_AC_R_RT60_SINUS         , IDC_AC_R_RT60_RAUSCHEN        , 0                      , IDC_AC_R_RT60_SINUS},
   {IDS_LOUDSPEAKER       , IDC_AC_R_2WAY               , IDC_AC_R_3WAY                 , 0                      , IDC_AC_R_2WAY},
   {IDS_MEASUREMENT_FILTER, IDC_AC_R_FILTER_A           , IDC_AC_R_FILTER_C             , 0                      , IDC_AC_R_FILTER_A},
   {IDS_PEAKHOLD          , IDC_AC_TXT_PEAK_HOLD_TIME   , IDC_AC_COMBO_PEAK_HOLD_TIME   , 0                      , IDC_AC_COMBO_PEAK_HOLD_TIME},
   {IDS_CALIBRATION       , IDC_AC_TXT_CALIBRATION      , IDC_AC_EDT_CALIBRATION        , IDC_AC_SPIN_CALIBRATION, IDS_CALIBRATION_HELP},
};

int CAC_COMDlg::gm_pnTabTables[AC_TABLES_TABS][AC_TABLES_CTRLS] =
{
   {IDS_CD_ROOM      , IDC_AC_COMBO_CD_A           , IDC_AC_COMBO_CD_B             , IDC_AC_COMBO_CD_C            , IDS_CD_ROOM_HELP},
   {IDS_NETWORK      , IDC_AC_CK_NETWORK           , IDC_AC_BTN_CONF_NETWORK       , 0                            , IDC_AC_CK_NETWORK},
   {IDS_AUTOBALANCE  , IDC_AC_CK_AUTOBALANCE_DELTAT, IDC_AC_CK_AUTOBALANCE_DB_LEVEL, 0                            , IDC_AC_CK_AUTOBALANCE_DELTAT},
   {IDS_SOUND_COUNTER, IDC_AC_TXT_NO_OF_SOUNDS_AUTO, IDC_AC_EDT_NO_OF_SOUNDS_AUTO  , IDC_AC_SPIN_NO_OF_SOUNDS_AUTO, IDC_AC_EDT_NO_OF_SOUNDS_AUTO},
   {IDS_MEMO_COUNTER , IDC_AC_TXT_MEMO_COUNTER     , IDC_AC_EDT_MEMO_COUNTER       , IDC_AC_SPIN_MEMO_COUNTER     , IDC_AC_EDT_MEMO_COUNTER},
   {IDS_BAD_POINTS   , IDC_AC_TXT_BAD_POINTS       , IDC_AC_EDT_BAD_POINTS         , IDC_AC_SPIN_BAD_POINTS       , IDC_AC_SPIN_BAD_POINTS},
   {IDS_WEIGHTING    , IDC_AC_TXT_WEIGHTING        , IDC_AC_EDT_WEIGHTING          , IDC_AC_SPIN_WEIGHTING        , IDC_AC_SPIN_WEIGHTING},
};

HWND CAC_COMDlg::gm_hWndActualHit = NULL;

CAC_COMDlg::CAC_COMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAC_COMDlg::IDD, pParent)
{
   BEGIN("CAC_COMDlg");

   //{{AFX_DATA_INIT(CAC_COMDlg)
	m_bNetwork             = FALSE;
	m_bAutoBalanceDeltaT   = FALSE;
	m_bAutoBalancedBLevel  = FALSE;
	m_nLoudspeakerWays     = -1;
	m_nLevelMeter          = -1;
	m_nFilterType          = -1;
	m_nRT60                = -1;
	m_nLoudspeakerMode     = -1;
	m_strNewName           = _T("");
	m_bShowCurveImediately = FALSE;
	m_nAC_Settings         = 0;
	//}}AFX_DATA_INIT

   // Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_bDataOk            = false;
   m_bDataChanged       = false;
   m_bWaitErrorTime     = false;
   m_bUpdateOnGetData   = true;
   m_nLastStatusTextID  = 0;
   m_nMeasureNo         = -1;
   m_pACData            = new CAudioControlData;
   m_CommPort.SetTimeOutRead(m_BasicParam.m_nEventTime);

   m_nDisplayDivX       = 330;
   m_bHitDisplayDivX    = false;
   m_pCDPlayer          = NULL;
}

CAC_COMDlg::~CAC_COMDlg()
{
   BEGIN("~CAC_COMDlg");
   POSITION             pos;
   pos = m_CalledProcess.GetHeadPosition();
   while (pos)
   {
      DeleteProcessInfo((PROCESS_INFORMATION*) m_CalledProcess.GetNext(pos));
   }
   DeleteMeasurements();
   delete m_pACData;
}

void CAC_COMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAC_COMDlg)
	DDX_Control(pDX, IDC_AC_COMBO_MEANING_OF_TV, m_cMeaningOfTestVal);
	DDX_Control(pDX, IDC_AC_COMBO_PEAK_HOLD_TIME, m_cPeakHold);
	DDX_Control(pDX, IDC_AC_COMBO_CD_C, m_cCD_RoomC);
	DDX_Control(pDX, IDC_AC_COMBO_CD_B, m_cCD_RoomB);
	DDX_Control(pDX, IDC_AC_COMBO_CD_A, m_cCD_RoomA);
	DDX_Control(pDX, IDC_AC_COMBO_PORT, m_cPort);
	DDX_Check(pDX, IDC_AC_CK_NETWORK, m_bNetwork);
	DDX_Check(pDX, IDC_AC_CK_AUTOBALANCE_DELTAT, m_bAutoBalanceDeltaT);
	DDX_Check(pDX, IDC_AC_CK_AUTOBALANCE_DB_LEVEL, m_bAutoBalancedBLevel);
	DDX_Radio(pDX, IDC_AC_R_3WAY, m_nLoudspeakerWays);
	DDX_Radio(pDX, IDC_AC_R_ANALOG, m_nLevelMeter);
	DDX_Radio(pDX, IDC_AC_R_FILTER_A, m_nFilterType);
	DDX_Radio(pDX, IDC_AC_R_RT60_SINUS, m_nRT60);
	DDX_Radio(pDX, IDC_AC_R_STEREO, m_nLoudspeakerMode);
	DDX_Text(pDX, IDC_AC_EDT_CURVENAME, m_strNewName);
	DDX_Check(pDX, IDC_AC_CK_SHOW_CURVES, m_bShowCurveImediately);
	DDX_Radio(pDX, IDC_AC_R_BASIC_AC_SETTINGS, m_nAC_Settings);
	//}}AFX_DATA_MAP
   if (!pDX->m_bSaveAndValidate)
   {
      bool bShowCARA;
      GetDlgItem(IDC_AC_BTN_CONF_NETWORK)->EnableWindow(m_bNetwork);
      switch ((int)m_pACData->cMeaningOfTestValues)
      {
         case DB_VALUES_FROM_DATA_LOGGER:
         case SELF_TEST_AUTO_EQ:                               // Lautsprecherfrequenzgänge
         case AUTO_BALANCE_FUNCTION:                           // Keine Messkurven !
         case AUTO_SOUND:                                      // Messkurven ? (testen !!!)
         case VALUES_LEFT_SPEAKER:
         case VALUES_CENTER_SPEAKER:
         case VALUES_RIGHT_SPEAKER:
            bShowCARA = true;
            break;
         case NO_VALUES:
            bShowCARA = false;
            break;
      }
      GetDlgItem(IDC_AC_BTN_SHOW_CURVES)->EnableWindow(bShowCARA);
      SetMenuItem(ID_FILE_SAVE, INVALID_VALUE, (m_bDataChanged&SAVED_CHANGED) ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
   }
}

BEGIN_MESSAGE_MAP(CAC_COMDlg, CDialog)
	//{{AFX_MSG_MAP(CAC_COMDlg)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_AC_BTN_GETDATA, OnAcBtnGetdata)
	ON_CBN_SELCHANGE(IDC_AC_COMBO_CD_A, OnSelchangeAcComboCdA)
	ON_CBN_SELCHANGE(IDC_AC_COMBO_CD_B, OnSelchangeAcComboCdB)
	ON_CBN_SELCHANGE(IDC_AC_COMBO_CD_C, OnSelchangeAcComboCdC)
	ON_NOTIFY(UDN_DELTAPOS, IDC_AC_SPIN_CALIBRATION, OnDeltaposAcSpinCalibration)
	ON_NOTIFY(TCN_SELCHANGE, IDC_AC_TAB_TABLES, OnSelchangeAcTabTables)
	ON_NOTIFY(TCN_SELCHANGING, IDC_AC_TAB_TABLES, OnSelchangingAcTabTables)
	ON_NOTIFY(UDN_DELTAPOS, IDC_AC_SPIN_MEMO_COUNTER, OnDeltaposAcSpinMemoCounter)
	ON_NOTIFY(UDN_DELTAPOS, IDC_AC_SPIN_BAD_POINTS, OnDeltaposAcSpinBadPoints)
	ON_NOTIFY(UDN_DELTAPOS, IDC_AC_SPIN_NO_OF_SOUNDS_AUTO, OnDeltaposAcSpinNoOfSoundsAuto)
	ON_NOTIFY(UDN_DELTAPOS, IDC_AC_SPIN_WEIGHTING, OnDeltaposAcSpinWeighting)
	ON_BN_CLICKED(IDC_AC_BTN_SHOW_CURVES, OnAcBtnShowCurves)
	ON_NOTIFY(TCN_SELCHANGE, IDC_AC_TAB_SYSTEM_CONFIG, OnSelchangeAcTabSystemConfig)
	ON_NOTIFY(TCN_SELCHANGING, IDC_AC_TAB_SYSTEM_CONFIG, OnSelchangingAcTabSystemConfig)
	ON_CBN_SELCHANGE(IDC_AC_COMBO_PEAK_HOLD_TIME, OnSelchangeAcComboPeakHoldTime)
	ON_CBN_SELCHANGE(IDC_AC_COMBO_MEANING_OF_TV, OnSelchangeAcComboTestPoints)
	ON_WM_TIMER()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
	ON_COMMAND(ID_OPTIONS_PORT, OnOptionsPort)
	ON_BN_CLICKED(IDC_AC_BTN_PUTDATA, OnAcBtnPutdata)
	ON_BN_CLICKED(IDC_AC_CK_AUTOBALANCE_DB_LEVEL, OnChange)
	ON_EN_KILLFOCUS(IDC_AC_EDT_CURVENAME, OnKillfocusAcEdtCurveName)
	ON_BN_CLICKED(IDC_AC_CK_NETWORK, OnAcCkNetwork)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_AC_BTN_CONF_NETWORK, OnAcBtnConfNetwork)
	ON_BN_CLICKED(IDC_AC_BTN_START_MEASUREMENT, OnAcBtnStartMeasurement)
	ON_COMMAND(ID_CONTEXT_HELP, OnContextHelp)
	ON_COMMAND(ID_OPTIONS_BASIC_PARAMS, OnOptionsBasicParams)
	ON_BN_CLICKED(IDC_AC_BTN_START_MEASUREMENT_ONCE, OnAcBtnStartMeasurementOnce)
	ON_BN_CLICKED(IDC_AC_BTN_EVALUATE_MEASUREMENTS, OnAcBtnEvaluateMeasurements)
	ON_BN_CLICKED(IDC_AC_CK_SHOW_CURVES, OnAcCkShowCurves)
	ON_COMMAND(ID_EDIT_IR_CODES, OnEditIrCodes)
	ON_COMMAND(ID_FILE_INSERT, OnFileInsert)
	ON_BN_CLICKED(IDC_AC_R_BASIC_AC_SETTINGS, OnAcSettings)
	ON_COMMAND(ID_OPTIONS_FRQ_RSP_COR, OnOptionsFrqRspCor)
	ON_COMMAND(ID_OPTIONS_MSG_BOX_BEHAVIOUR, OnOptionsMsgBoxBehaviour)
	ON_COMMAND(ID_VIEW_CD_PLAYER, OnViewCdPlayer)
	ON_WM_SYSCOMMAND()
   ON_MESSAGE(WM_FROM_2D_VIEW, OnMsgFrom2DView)
	ON_BN_CLICKED(IDC_AC_CK_AUTOBALANCE_DELTAT, OnChange)
	ON_BN_CLICKED(IDC_AC_R_3WAY, OnChange)
	ON_BN_CLICKED(IDC_AC_R_2WAY, OnChange)
	ON_BN_CLICKED(IDC_AC_R_ANALOG, OnChange)
	ON_BN_CLICKED(IDC_AC_R_DIGITAL, OnChange)
	ON_BN_CLICKED(IDC_AC_R_FILTER_A, OnChange)
	ON_BN_CLICKED(IDC_AC_R_FILTER_C, OnChange)
	ON_BN_CLICKED(IDC_AC_R_RT60_RAUSCHEN, OnChange)
	ON_BN_CLICKED(IDC_AC_R_RT60_SINUS, OnChange)
	ON_BN_CLICKED(IDC_AC_R_STEREO, OnChange)
	ON_BN_CLICKED(IDC_AC_R_SURROUND, OnChange)
	ON_BN_CLICKED(IDC_AC_R_EXTENDED_AC_SETTINGS, OnAcSettings)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
   ON_MESSAGE(WM_MENUSELECT, OnMenuSelect)
   ON_MESSAGE(WM_DROPFILES, OnDropFiles)
	ON_COMMAND_RANGE(ID_VIEW_TABS, ID_VIEW_BUTTONS, OnViewTabs)
   ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
   ON_MESSAGE(WM_ENTERMENULOOP, OnEnterMenuLoop)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// private Behandlungsroutinen

bool CAC_COMDlg::SetMenuItem(UINT nItem, UINT nCheck, UINT nEnable, HMENU hMenu)
{
   if (hMenu == NULL) hMenu = ::GetMenu(m_hWnd);

   if (hMenu)
   {
      HMENU hSubMenu;
      bool  bOk = false;
      int   i, nCount = ::GetMenuItemCount(hMenu);
      for (i=0; i<nCount; i++)
      {
         hSubMenu = ::GetSubMenu(hMenu, i);
         if (hSubMenu)
         {
            bOk = SetMenuItem(nItem, nCheck, nEnable, hSubMenu);
            if (bOk) return true;
         }
      }
      if (nCheck != INVALID_VALUE)
      {
         if (nCheck == MFT_RADIOCHECK)
         {
            int *pRange = (int*) nEnable;
            if (CheckMenuRadioItem(hMenu, pRange[0], pRange[1], nItem, MF_BYCOMMAND)) return true;
            return false;
         }
         else if (CheckMenuItem(hMenu, nItem,  nCheck|MF_BYCOMMAND) != INVALID_VALUE) bOk = true;
      }
      if (nEnable != INVALID_VALUE)
      {
         if (EnableMenuItem(hMenu, nItem,  nEnable|MF_BYCOMMAND) != INVALID_VALUE) bOk = true;
      }
      return bOk;
   }
   return false;
}

void CAC_COMDlg::InitDlgCtrls()
{
   m_bNetwork            = m_pACData->bExpertLevel             ? 1 : 0; // Expertenebene ?
   m_nLoudspeakerWays    = m_pACData->bLoudspeakersystem2W     ? 1 : 0; // Lautsprechersystem 3 Wege = 0, 2 Wege = 1
   m_nLoudspeakerMode    = m_pACData->bLoudspeakermodeSurround ? 1 : 0; // Modus Stereo = 0, Surround = 1
   m_bAutoBalanceDeltaT  = m_pACData->bAutobalanceDeltaT       ? 1 : 0; // Autobalance Delta-T
   m_bAutoBalancedBLevel = m_pACData->bAutobalance_dB_Level    ? 1 : 0; // Autobalance dB-Level
   m_nLevelMeter         = m_pACData->bLevelMeterSettingDig    ? 1 : 0; // Pegelmesser Balken = 0, Digital = 1
   m_nRT60               = m_pACData->bRT60_Noise              ? 1 : 0; // RT60 Sinus = 0, Rauschen = 1
   m_nFilterType         = m_pACData->bFilterTypeC             ? 1 : 0; // Filter A = 0, C = 1

   m_cCD_RoomA.SetCurSel(m_pACData->cCdPlayerRoomA);
   m_cCD_RoomB.SetCurSel(m_pACData->cCdPlayerRoomB);
   m_cCD_RoomC.SetCurSel(m_pACData->cCdPlayerRoomC);
   m_cPeakHold.SetCurSel(m_pACData->nPeakHold);
   if (m_pACData->cMeaningOfTestValues == SELF_TEST_AUTO_EQ)
   {
      if (m_pACData->bFree1)
      {
         m_cMeaningOfTestVal.SetCurSel(SELF_TEST_AUTO_EQ_TRACK9OLD);
      }
      else if (m_pACData->bFree4)
      {
         m_cMeaningOfTestVal.SetCurSel(SELF_TEST_AUTO_EQ_TRACK10NEW);
      }
      else
      {
         m_cMeaningOfTestVal.SetCurSel(m_pACData->cMeaningOfTestValues);
      }
   }
   else
   {
      m_cMeaningOfTestVal.SetCurSel(m_pACData->cMeaningOfTestValues);
   }

   GetDlgItem(IDC_AC_BTN_SHOW_CURVES)->EnableWindow(((m_pACData->cMeaningOfTestValues > NO_VALUES) && (m_pACData->cMeaningOfTestValues <= AUTO_SOUND))?true : false);

   OnDeltaposAcSpinCalibration(NULL, NULL);
	OnDeltaposAcSpinMemoCounter(NULL, NULL);
	OnDeltaposAcSpinBadPoints(NULL, NULL);
	OnDeltaposAcSpinNoOfSoundsAuto(NULL, NULL);
	OnDeltaposAcSpinWeighting(NULL, NULL);
   m_strNewName.LoadString(IDS_MEASUREMENT_VALUES);
/*
   // aktuellen m_pACData->cCD_Track zum CD-Player schicken !!
*/
   GetDlgItem(IDC_AC_BTN_EVALUATE_MEASUREMENTS)->EnableWindow((m_Measurements.GetCount() > 0) ? true : false);

   UpdateData(false);
}

void CAC_COMDlg::DeleteProcessInfo(PROCESS_INFORMATION *pi)
{
   if (pi)
   {
      if (pi->hProcess)
      {
         DWORD   exitcode;
         if(GetExitCodeProcess(pi->hProcess, &exitcode))
         {
            if(exitcode == STILL_ACTIVE)
            {
               ::EnumThreadWindows(pi->dwThreadId, CloseThreadWindow, NULL);
            }
         }
         ::CloseHandle(pi->hProcess);
      }
      if (pi->hThread) ::CloseHandle(pi->hThread);
      delete pi;
   }
}

int CAC_COMDlg::SendDataToAC()
{
   int nReturn = 0;
   char szData[4];
   m_pACData->CalculateChecksum(m_pACData->cChecksum1, m_pACData->cChecksum2);
   try
   {
      if (!OpenPort())                       throw (int) PORT_ERROR_OPEN;
      if (!m_CommPort.WritePort(SEND_PUTDATA, strlen((const char*)SEND_PUTDATA)))
                                             throw (int) PORT_ERROR_READ_WRITE;
      Sleep(150);                                           // 150 ms warten (min 100ms, max 250 ms)
      if (!m_CommPort.WritePort(m_pACData, DATA_SIZE))throw (int) PORT_ERROR_READ_WRITE;
//      Sleep(150);                                           // 150 ms warten (min 100ms, max 250 ms)
      if (!m_CommPort.ReadPort(szData, 2))   throw (int) PORT_ERROR_READ_WRITE;
      if      (szData[0] != '@')             throw (int) PORT_ERROR_ALPHASIGN;
      if      (szData[1] == 'A')             throw (int) PORT_ERROR_NO;
      else if (szData[1] == 'N')             throw (int) PORT_ERROR_ACKNOWLEDGE;
      SetDataChanged(SENT_CHANGED);
   }
   catch (int nError)
   {
      if (nError)
      {
         ReportTransmissionError(nError);
      }
      nReturn = nError;
   }
   if (CAC_COMApp::gm_bClosePortImediate) m_CommPort.ClosePort();

   return nReturn;
}

int CAC_COMDlg::GetDataFromAC()
{
   int nReturn = 0;
   try
   {
      BYTE cCS1, cCS2;
      if (!OpenPort()) throw (int) PORT_ERROR_OPEN;
      if (!m_CommPort.WritePort(SEND_GETDATA, strlen((const char*)SEND_GETDATA))) throw (int) PORT_ERROR_READ_WRITE;
      Sleep(150);                                           // 150 ms warten (min 100ms, max 250 ms)
      if (!m_CommPort.ReadPort(m_pACData, DATA_SIZE)) throw (int) PORT_ERROR_READ_WRITE;// Daten lesen
      if (m_pACData->cAt         != '@') throw (int) PORT_ERROR_ALPHASIGN;// Übertragungszeichen und
      if (m_pACData->cCommResult != 'D') throw (int) PORT_ERROR_NO_DATA;// Daten vom AC prüfen

      m_pACData->CalculateChecksum(cCS1, cCS2);
      if ((cCS1 != m_pACData->cChecksum1) || (cCS2 != m_pACData->cChecksum2))
         throw (int) PORT_ERROR_CHECKSUM;

      SetData(true);
      if (m_bUpdateOnGetData)
      {
         SetDataChanged();
         SetDataChanged(SENT_CHANGED);
         m_pACData->InitMeasurementParams();
         InitDlgCtrls();                                       // Daten anzeigen
      }
   }
   catch (int nError)
   {
      if (nError != PORT_ERROR_NO)
      {
         if (!m_PMState.Polling)
            SetData(false);
      }
      nReturn = nError;
   }
   if (CAC_COMApp::gm_bClosePortImediate) m_CommPort.ClosePort();// Port wieder schließen
   return nReturn;
}

void CAC_COMDlg::SetStatusText(int nID)
{
   if (m_bWaitErrorTime) return;
   if (nID != m_nLastStatusTextID)
   {
      char text[MAX_PATH];
      if ((nID != 0) && ::LoadString(AfxGetInstanceHandle(), nID, text, MAX_PATH))
      {
         SetStatusText(text);
      }
      else
      {
//         TRACE("Missing Msg ID : %d\n", nID);
      }
      m_nLastStatusTextID = nID;
   }
}

void CAC_COMDlg::SetStatusText(const char *pstr)
{
   if (m_bWaitErrorTime) return;
   CWnd *pWnd = GetDlgItem(IDC_AC_STATUS);
   if (pWnd)
   {
//      TRACE("%s\n", pstr);
      pWnd->SetWindowText(pstr);
      pWnd->UpdateWindow();
      m_nLastStatusTextID = 0;
   }
}

void CAC_COMDlg::ShowTextLonger(DWORD dwMS)
{
   if (!m_bWaitErrorTime)
   {
      m_bWaitErrorTime = true;
      SetTimerSave(ID_WAIT_ERROR_TIMER, dwMS, NULL);
   }
}

void CAC_COMDlg::ReportTransmissionError(int nError)
{
   CString strCaption, strText;
   strCaption.LoadString(IDS_TRANSMISSION_ERROR);
   strText.LoadString(IDS_TRANSMISSION_ERROR+nError);
   strCaption += strText;
   SetStatusText(strCaption);
   ShowTextLonger(2000);
   m_nLastStatusTextID = 0;
}

void CAC_COMDlg::NotYetImplemented()
{
   CString str;
   str.LoadString(IDS_NOT_YET_IMPLEMENTED);
   SetStatusText(str);
   ShowTextLonger(2000);
}

BOOL CALLBACK CAC_COMDlg::EnumChildWndFunc(HWND hwnd, LPARAM lParam)
{
   long lValue = GetWindowLong(hwnd, GWL_USERDATA);
   if (lValue == 0)
   {
      ::SetWindowLong(hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (long)CAC_COMDlg::SubClassProc));
   }
   lValue = GetWindowLong(hwnd, GWL_ID);
   if (lValue)
   {
      DWORD nSetID;
      switch (lValue)                                             // Status IDs anpassen
      {
         case IDC_AC_CK_AUTOBALANCE_DB_LEVEL: nSetID = IDC_AC_CK_AUTOBALANCE_DELTAT; break;
         case IDC_AC_R_RT60_RAUSCHEN:         nSetID = IDC_AC_R_RT60_SINUS;          break;
         case IDC_AC_R_3WAY:                  nSetID = IDC_AC_R_2WAY;                break;
         case IDC_AC_R_SURROUND:              nSetID = IDC_AC_R_STEREO;              break;
         case IDC_AC_R_ANALOG:                nSetID = IDC_AC_R_DIGITAL;             break;
         case IDC_AC_R_FILTER_C:              nSetID = IDC_AC_R_FILTER_A;            break;
         case IDC_AC_SPIN_NO_OF_SOUNDS_AUTO:  nSetID = IDC_AC_EDT_NO_OF_SOUNDS_AUTO; break;
         case IDC_AC_SPIN_MEMO_COUNTER:       nSetID = IDC_AC_EDT_MEMO_COUNTER;      break;
         case IDC_AC_EDT_BAD_POINTS:          nSetID = IDC_AC_SPIN_BAD_POINTS;       break;
         case IDC_AC_EDT_WEIGHTING:           nSetID = IDC_AC_SPIN_WEIGHTING;        break;
         case IDC_AC_SPIN_CALIBRATION:        nSetID = IDC_AC_EDT_CALIBRATION;       break;
         case IDC_AC_BTN_START_MEASUREMENT_ONCE: nSetID = IDC_AC_BTN_START_MEASUREMENT;break;
         default :nSetID = lValue; break; 
      }
      ::SetWindowContextHelpId(hwnd, nSetID);
   }
   return 1;
}

BOOL CALLBACK CAC_COMDlg::CloseThreadWindow(HWND hwnd, LPARAM lParam)
{
   if (::IsWindow(hwnd))
   {
      DWORD dwResult = 0;
      ::SendMessageTimeout(hwnd, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 1000, &dwResult);
   }
   return (hwnd != NULL) ? true : false;
}


void CAC_COMDlg::SaveData()
{
	TRY
	{
      CFile file(m_strFileName, CFile::modeCreate|CFile::modeWrite);
      CArchive ar(&file, CArchive::store);
      CFileHeader fh(AC_FILETYPE, "2000", 200);
      int nCount = m_Measurements.GetCount();
      if (nCount == 0)
      {
         fh.SetVersion(100);
         fh.CalcChecksum(m_pACData, DATA_SIZE);
      }
      else
      {
//         fh.CalcChecksum(m_pACData, DATA_SIZE);
      }
      ar.Write(&fh, sizeof(CFileHeader));
      if (nCount == 0)
      {
         ar.Write(m_pACData, DATA_SIZE);
      }
      else
      {
         m_Measurements.Serialize(ar);
      }
      SetDataChanged(SAVED_CHANGED);
   }
	CATCH_ALL(e)
	{
      MSG msg = {AfxGetMainWnd()->m_hWnd, 0, (WPARAM)LPCTSTR(m_strFileName), 0, 0, {0, 0}};
		AfxGetThread()->ProcessWndProcException(e, &msg);
	}
	END_CATCH_ALL
}

void CAC_COMDlg::ReadData()
{
	TRY
	{
      CFile file(m_strFileName, CFile::modeRead);
      CArchive ar(&file, CArchive::load);
      CFileHeader fh;
      ar.Read(&fh, sizeof(CFileHeader));
      unsigned long lChecksum = fh.GetChecksum();
      fh.CalcChecksum();
      DeleteMeasurements();
      if (fh.IsType(AC_FILETYPE))
      {
         if (fh.GetVersion() == 100)
         {
            ar.Read(m_pACData, DATA_SIZE);
            fh.CalcChecksum(m_pACData, DATA_SIZE);
            if (fh.IsValid(lChecksum))
            {
               m_Measurements.AddTail(new CMeasurement(*m_pACData));
               SetData(true);
               SetDataChanged(CHANGED);
               InitDlgCtrls();
               m_EvalMeasurement.SetMeaningOfTestVal(m_pACData);
               SetStatusText(IDS_DATA_OK);
               ShowTextLonger(2000);
            }
            else
            {
               SetData(false);
               SetStatusText(IDS_DATA_NOT_OK);
               ShowTextLonger(2000);
            }
         }
         else
         {
            m_Measurements.Serialize(ar);
            SetDataChanged(CHANGED);
            if (!m_Measurements.IsEmpty())
            {
               CMeasurement *pM = m_Measurements.GetHead();
               if (pM)
               {
                  SetData(true);
                  SetStatusText(IDS_DATA_OK);
                  ShowTextLonger(2000);
                  *m_pACData = pM->m_ACData;
                  InitDlgCtrls();
                  m_EvalMeasurement.SetMeaningOfTestVal(m_pACData);
               }
            }
         }
      }
   }
	CATCH_ALL(e)
	{
      MSG msg = {AfxGetMainWnd()->m_hWnd, 0, (WPARAM)LPCTSTR(m_strFileName), 0, 0, {0, 0}};
		AfxGetThread()->ProcessWndProcException(e, &msg);
	}
	END_CATCH_ALL
}

void CAC_COMDlg::SaveChangedData()
{
   if (m_bDataOk && (m_bDataChanged&SAVED_CHANGED))
   {
      if (CAC_COMApp::gm_dwMessageBoxFlags & MBF_AUTO_SAVEDATA) // automatisch speichern
      {
         OnFileSave();
      }
      else if ((CAC_COMApp::gm_dwMessageBoxFlags & MBF_REQU_SAVEDATA) && // Fragen ob speichern
               (AfxMessageBox(IDS_SAVE_CHANGED_DATA, MB_YESNO|MB_ICONQUESTION)==IDYES))
      {
         OnFileSave();
      }
   }
}

void CAC_COMDlg::SetData(bool bOk)
{
   m_bDataOk = bOk;
   GetDlgItem(IDC_AC_BTN_PUTDATA)->EnableWindow(bOk);
}

void CAC_COMDlg::SetDataChanged(BYTE bReset)
{
   if (bReset)
   {
      m_bDataChanged &=  ~bReset;
   }
   else
   {
      m_bDataChanged = CHANGED;
   }
   SetMenuItem(ID_FILE_SAVE, INVALID_VALUE, (m_bDataChanged&SAVED_CHANGED) ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
}


/////////////////////////////////////////////////////////////////////////////
// CAC_COMDlg Nachrichten-Handler

BOOL CAC_COMDlg::OnInitDialog()
{
   BEGIN("OnInitDialog");
   DWORD i, dwNeeded, dwPorts;
   char  szItem[128];

   m_EvalMeasurement.m_pParent   = this;
   CAudioControlData::gm_pParent = this;

   CDialog::OnInitDialog();

   HMODULE hModule = ::GetModuleHandle("uxtheme.dll");
   if (hModule != NULL)
   {
      BOOL (*pf)() = (BOOL(*)())::GetProcAddress(hModule, "IsThemeActive");
      if (pf != NULL)
      {
         if (pf())
         {
            CAC_COMApp::gm_bWindowsXP = true;
         }
      }
   }

   CString str;
   CWinApp*pApp = AfxGetApp();

	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

   CMenu *pM = GetSystemMenu(FALSE);
   pM->EnableMenuItem(SC_MAXIMIZE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
   pM->EnableMenuItem(SC_SIZE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
   pM->EnableMenuItem(SC_RESTORE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
   
   dwPorts = pApp->GetProfileInt(REGKEY_DEFAULT, NO_PORT_ENUM, 0);
   if (LOWORD(dwPorts) != 0)
   {
      COMMCONFIG CC;
      ULONG ulSize = sizeof(COMMCONFIG);
      CC.dwSize    = ulSize;
      for(i=1; i<=LOWORD(dwPorts); i++)
      {
         wsprintf(szItem, "COM%d", i);
         if ((HIWORD(dwPorts) == 0) && !GetDefaultCommConfig(szItem, &CC, &ulSize))
            continue;
         REPORT("Port : %d, %s", i, szItem);
         m_cPort.AddString(szItem);
      }
   }
   else
   {
      pApp->WriteProfileInt(REGKEY_DEFAULT, NO_PORT_ENUM, 4);  // NoPortEnum auf 4 Ports setzen, falls
      ::EnumPorts(NULL, 1, NULL, 0, &dwNeeded, &dwPorts);      // er bei EnumPorts abstürzt
      REPORT("Port : %d, %d", dwNeeded, dwPorts);
      if(dwNeeded)
      {
         PORT_INFO_1 * pPortInfo;
         pPortInfo = (PORT_INFO_1 * ) new BYTE[dwNeeded];
         if(pPortInfo!=NULL)
         {
            _COMMCONFIG CC;
            ULONG ulSize = sizeof(_COMMCONFIG);
            CC.dwSize    = ulSize;
            ::EnumPorts(NULL, 1, (BYTE*)pPortInfo, dwNeeded, &dwNeeded, &dwPorts);
            REPORT("Ports : %d, %d", dwNeeded, dwPorts);
            for(i=0;i<dwPorts;i++)
            {
               REPORT("Port : %d, %s", i, pPortInfo[i].pName);
               if (strstr(pPortInfo[i].pName, "COM") != NULL)
               {
                  char * pstr = strstr(pPortInfo[i].pName, ":");
                  if (pstr) pstr[0] = 0;
                  if (!GetDefaultCommConfig(pPortInfo[i].pName, &CC, &ulSize))
                     continue;
                  m_cPort.AddString(pPortInfo[i].pName);
               }
            }
            delete pPortInfo;
         }
      }
      // NoPortEnum auf 0 setzen, da er bei EnumPorts nicht abstürzte (Witziger Bugfix gell)
      pApp->WriteProfileInt(REGKEY_DEFAULT, NO_PORT_ENUM, 0);
   }

   m_cPort.SetCurSel(0);
   str = pApp->GetProfileString(REGKEY_DEFAULT, COM_PORT, NULL);
   if (!str.IsEmpty())
   {
      int nSel = m_cPort.FindString(-1, str);
      if (nSel != CB_ERR) m_cPort.SetCurSel(nSel);
   }
   
   REPORT("CD-CTRL");
   dwPorts = m_cCD_RoomA.GetCount();
   for (i=0; i<dwPorts; i++)
   {
      m_cCD_RoomA.GetLBText(i, szItem);
      m_cCD_RoomB.AddString(szItem);
      m_cCD_RoomC.AddString(szItem);
   }

   REPORT("Tab-CTRL");
   dwPorts = pApp->GetProfileInt(REGKEY_DEFAULT, TAB_BUTTONS, 0);
   if (CAC_COMApp::gm_bWindowsXP)
   {
      CMenu *pMenu = GetMenu();
      if (pMenu)
      {
         pMenu = pMenu->GetSubMenu(2);
         if (pMenu)
         {
            pMenu->RemoveMenu(0, MF_BYPOSITION);
            pMenu->RemoveMenu(0, MF_BYPOSITION);
            pMenu->RemoveMenu(0, MF_BYPOSITION);
         }
      }
      dwPorts = 0;
   }
   else
   {
      int nRange[2] = {ID_VIEW_TABS, ID_VIEW_BUTTONS};
      SetMenuItem((dwPorts!=0) ? ID_VIEW_BUTTONS:ID_VIEW_TABS, MFT_RADIOCHECK, (UINT)&nRange[0]);
   }
   CWnd * pWnd = GetDlgItem(IDC_AC_TAB_TABLES);
   if (pWnd)
   {
      UINT nCount = 4;
      if (dwPorts) pWnd->ModifyStyle(0, TCS_BUTTONS);
      TCITEMA tcItem;
      tcItem.mask       = TCIF_TEXT|TCIF_PARAM;
      tcItem.pszText    = szItem;
      tcItem.cchTextMax = 128;
      if (CAC_COMApp::gm_bSpecialistMode) nCount = AC_TABLES_TABS;
      for (i=0; i<nCount; i++)
      {
         ::LoadString(AfxGetInstanceHandle(), gm_pnTabTables[i][0], tcItem.pszText, tcItem.cchTextMax);
         tcItem.lParam = i;
         pWnd->SendMessage(TCM_INSERTITEM, (WPARAM)i, (LPARAM) &tcItem);
      }
   }

   pWnd = GetDlgItem(IDC_AC_TAB_SYSTEM_CONFIG);
   if (pWnd)
   {
      if (dwPorts) pWnd->ModifyStyle(0, TCS_BUTTONS);
      TCITEMA tcItem;
      tcItem.mask       = TCIF_TEXT|TCIF_PARAM;
      tcItem.pszText    = szItem;
      tcItem.cchTextMax = 128;
      if (CAC_COMApp::gm_bWindowsXP)
      {
//         ::SetClassLong(pWnd->m_hWnd, GCL_HBRBACKGROUND, (long)::GetSysColorBrush(COLOR_BTNFACE));
//         g_hBrush = (HBRUSH)::GetClassLong(pWnd->m_hWnd, GCL_HBRBACKGROUND);

//         ::SetClassLong(pWnd->m_hWnd, GCL_HBRBACKGROUND, (long)::GetSysColorBrush(g_nCol));
      }
  
      for (i=0; i<AC_SYSTEMCONFIG_TABS; i++)
      {
         ::LoadString(AfxGetInstanceHandle(), gm_pnTabSystemConfig[i][0], tcItem.pszText, tcItem.cchTextMax);
         tcItem.lParam = i;
         pWnd->SendMessage(TCM_INSERTITEM, (WPARAM)i, (LPARAM) &tcItem);
      }
   }

   REPORT("Spin-CTRL");
   GetDlgItem(IDC_AC_SPIN_BAD_POINTS       )->SendMessage(UDM_SETRANGE32,          15,  0);
   GetDlgItem(IDC_AC_SPIN_CALIBRATION      )->SendMessage(UDM_SETRANGE32, (WPARAM)-50, 50);
   GetDlgItem(IDC_AC_SPIN_MEMO_COUNTER     )->SendMessage(UDM_SETRANGE32,           0, 15);
   GetDlgItem(IDC_AC_SPIN_NO_OF_SOUNDS_AUTO)->SendMessage(UDM_SETRANGE32,           0, 12);
   GetDlgItem(IDC_AC_SPIN_WEIGHTING        )->SendMessage(UDM_SETRANGE32,           0, 15);

   SetData(m_bDataOk);

   REPORT("EnumChildWndFunc");
   ::EnumChildWindows(m_hWnd, EnumChildWndFunc, (LPARAM) this);

   ::SetWindowLong(m_hWnd, GWL_USERDATA, (long) this);

   if (((CAC_COMApp*)pApp)->m_cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
   {
      REPORT("Open file");
      m_strFileName = ((CAC_COMApp*)pApp)->m_cmdInfo.m_strFileName;
      ReadData();
   }

   SetMenuItem(ID_OPTIONS_FRQ_RSP_COR, INVALID_VALUE, (CAC_COMApp::gm_bSpecialistMode) ? MF_ENABLED : MF_DISABLED|MF_GRAYED);

   dwPorts = pApp->GetProfileInt(REGKEY_DEFAULT, CD_DRIVE, DEFAULT_CD_PLACE);
   if (dwPorts & EXIT_SIZE_MOVE_SET) OnViewCdPlayer();

   return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CAC_COMDlg::OnDestroy()
{
   BEGIN("OnDestroy");
   if (m_bDataChanged & SENT_CHANGED)
   {
      if (CAC_COMApp::gm_dwMessageBoxFlags & MBF_AUTO_SENDDATA) // automatisch senden
      {
         OnAcBtnPutdata();
      }
      else if ((CAC_COMApp::gm_dwMessageBoxFlags & MBF_REQU_SENDDATA) && // Fragen ob senden
               (AfxMessageBox(IDS_SEND_DATA, MB_YESNO|MB_ICONQUESTION)==IDYES))
      {
         OnAcBtnPutdata();
      }
   }
   CString str;
   CWinApp *pApp = AfxGetApp();
   m_cPort.GetWindowText(str);
   pApp->WriteProfileString(REGKEY_DEFAULT, COM_PORT, str);

   SaveChangedData();
   POSITION pos = m_Timers.GetHeadPosition();
   while (pos)
   {
      KillTimerSave((UINT)m_Timers.GetAt(pos));
      pos = m_Timers.GetHeadPosition();
   }

   CDialog::WinHelp(0L, HELP_QUIT);

   if (CAC_COMApp::gm_dwMessageBoxFlags & MBF_WARN_DISCONECT)
      AfxMessageBox(IDS_CUT_AC_FROM_PC, MB_ICONINFORMATION|MB_OK, 200);
   DestroyCDPlayer(false);
	CDialog::OnDestroy();
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CAC_COMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CAC_COMDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAC_COMDlg::OnOK()
{
 	DestroyCDPlayer(false);
   CDialog::OnOK();
}
void CAC_COMDlg::OnCancel() 
{
 	DestroyCDPlayer(false);
	CDialog::OnCancel();
}

void CAC_COMDlg::OnAcBtnGetdata()
{
   CString str;
   int nRequest, nPort,
       nPorts   = m_cPort.GetCount(),
       nPortSel = m_cPort.GetCurSel();
   for (nPort=0, nRequest=0; nPortSel != CB_ERR; nRequest++)
   {
      str.Format(IDS_OPEN_PORT_TRIALS, nPort+1, nRequest+1);
      SetStatusText(str);
      int nReturn = GetDataFromAC();
      if      (nReturn == 0)
      {
         SetStatusText(IDS_DATA_SENT);
         ShowTextLonger(2000);
         bool bInsert = false;
         if (m_bUpdateOnGetData && (m_pACData->cMeaningOfTestValues != NO_VALUES))
         {
            if (CAC_COMApp::gm_dwMessageBoxFlags & MBF_AUTO_INSERTDATA) // automatisch einfügen
            {
               bInsert = true;
            }
            else if ((CAC_COMApp::gm_dwMessageBoxFlags & MBF_REQU_INSERTDATA) && // Fragen ob einfügen
                     (AfxMessageBox(IDS_INSERT_DATA_FROM_AC, MB_YESNO|MB_ICONQUESTION|MB_HELP, IDC_AC_BTN_EVALUATE_MEASUREMENTS) == IDYES))
            {
               bInsert = true;
            }
         }     
         if (bInsert)
         {
            m_Measurements.AddTail(new CMeasurement(*m_pACData));
            m_EvalMeasurement.SetMeaningOfTestVal(m_pACData);
            GetDlgItem(IDC_AC_BTN_EVALUATE_MEASUREMENTS)->EnableWindow((m_Measurements.GetCount() > 0) ? true : false);
         }
         break;
      }
      else if (nReturn == PORT_ERROR_OPEN)
      {
         nRequest = m_BasicParam.m_nPortTrials + 1;
         ReportTransmissionError(nReturn);
      }
      if (nRequest == m_BasicParam.m_nPortTrials)
      {
         ReportTransmissionError(nReturn);
         if (AfxMessageBox(IDS_AC_READY, MB_OKCANCEL|MB_ICONEXCLAMATION|MB_HELP, IDC_AC_COMBO_PORT) == IDCANCEL)
            break;
      }
      if (nRequest > m_BasicParam.m_nPortTrials)                     // mehrfach abfragen, dann anderen Port nehmem
      {
         ReportTransmissionError(nReturn);
         nPort++;
         if (nPort>=nPorts) break;
         if (CAC_COMApp::gm_dwMessageBoxFlags & MBF_REQU_CHANGEPORT) // Fragen ob weiterschalten
         {
            if (AfxMessageBox(IDS_CHANGE_PORT, MB_YESNO|MB_ICONQUESTION|MB_HELP, IDC_AC_COMBO_PORT) == IDNO)
               break;
         }
         else if (!(CAC_COMApp::gm_dwMessageBoxFlags & MBF_AUTO_CHANGEPORT)) // automatisch weiterschalten
            break;
         nRequest = -1;
         nPortSel++;
         if (nPortSel >= nPorts) nPortSel = 0;
         nPortSel = m_cPort.SetCurSel(nPortSel);
      }
   }
}

void CAC_COMDlg::OnAcBtnPutdata()
{
   if (m_bDataOk && UpdateData(true))
   {
      BYTE szData[PARAM_VALUES_SIZE];
      m_pACData->bExpertLevel             = m_bNetwork            ? 1 : 0;
      m_pACData->bLoudspeakersystem2W     = m_nLoudspeakerWays    ? 1 : 0;
      m_pACData->bLoudspeakermodeSurround = m_nLoudspeakerMode    ? 1 : 0;
      m_pACData->bAutobalanceDeltaT       = m_bAutoBalanceDeltaT  ? 1 : 0;
      m_pACData->bAutobalance_dB_Level    = m_bAutoBalancedBLevel ? 1 : 0;
      m_pACData->bLevelMeterSettingDig    = m_nLevelMeter         ? 1 : 0;
      m_pACData->bRT60_Noise              = m_nRT60               ? 1 : 0;
      m_pACData->bFilterTypeC             = m_nFilterType         ? 1 : 0;

      m_pACData->cMeaningOfTestValues     = m_cMeaningOfTestVal.GetCurSel();
      m_pACData->nPeakHold = m_cPeakHold.GetCurSel();
      // Der AC-Meßdatenbereich soll nicht überschrieben werden !

      m_pACData->SaveParam(szData, PARAM_VALUES_SIZE);       // AC-Parameterbereich sichern
      m_bUpdateOnGetData = false;
      OnAcBtnGetdata();                                           // AC-Daten holen
      m_bUpdateOnGetData = true;
      m_pACData->RestoreParam(szData, PARAM_VALUES_SIZE);    // AC-Parameterbereich zurückspeichern
      
      CString str;
      int nRequest, nPort,
          nPorts   = m_cPort.GetCount(),
          nPortSel = m_cPort.GetCurSel();
      for (nPort=0, nRequest=0; nPortSel != CB_ERR; nRequest++)
      {
         str.Format(IDS_OPEN_PORT_TRIALS, nPort+1, nRequest+1);
         SetStatusText(str);
         int nReturn = SendDataToAC();
         if (nReturn == 0)
         {
            SetStatusText(IDS_DATA_SENT);
            ShowTextLonger(2000);
            SetDataChanged(SENT_CHANGED);
            break;
         }
         else if (nReturn == PORT_ERROR_OPEN)
         {
            nRequest = m_BasicParam.m_nPortTrials + 1;
         }

         if (nRequest == m_BasicParam.m_nPortTrials)
         {
            if (AfxMessageBox(IDS_AC_READY, MB_OKCANCEL|MB_ICONEXCLAMATION|MB_HELP, IDC_AC_COMBO_PORT) == IDCANCEL)
               break;
         }
         if (nRequest > m_BasicParam.m_nPortTrials)                   // m_nPortTrials abfragen, dann anderen Port nehmem
         {
            nPort++;
            if (nPort>=nPorts) break;
            if (CAC_COMApp::gm_dwMessageBoxFlags & MBF_REQU_CHANGEPORT) // Fragen ob weiterschalten
            {
               if (AfxMessageBox(IDS_CHANGE_PORT, MB_YESNO|MB_ICONQUESTION|MB_HELP, IDC_AC_COMBO_PORT) == IDNO)
                  break;
            }
            else if (!(CAC_COMApp::gm_dwMessageBoxFlags & MBF_AUTO_CHANGEPORT)) // automatisch weiterschalten
               break;
            nRequest = -1;
            nPortSel++;
            if (nPortSel >= nPorts) nPortSel = 0;
            nPortSel = m_cPort.SetCurSel(nPortSel);
         }
      }
   }
}

void CAC_COMDlg::OnSelchangeAcComboCdA()
{
   if (m_bDataOk)
   {
      BYTE cSel = m_cCD_RoomA.GetCurSel();
      if (m_pACData->cCdPlayerRoomA!= cSel)
      {
         m_pACData->cCdPlayerRoomA = cSel;
         SetDataChanged();
      }
   }
}

void CAC_COMDlg::OnSelchangeAcComboCdB()
{
   if (m_bDataOk)
   {
      BYTE cSel = m_cCD_RoomB.GetCurSel();
      if (m_pACData->cCdPlayerRoomB != cSel)
      {
         m_pACData->cCdPlayerRoomB= cSel;
         SetDataChanged();
      }
   }
}

void CAC_COMDlg::OnSelchangeAcComboCdC()
{
   if (m_bDataOk)
   {
      BYTE cSel = m_cCD_RoomC.GetCurSel();
      if (m_pACData->cCdPlayerRoomC != cSel)
      {
         m_pACData->cCdPlayerRoomC = cSel;
         SetDataChanged();
      }
   }
}

void CAC_COMDlg::OnSelchangeAcComboTestPoints()
{
   if (m_bDataOk) SetDataChanged();
}

void CAC_COMDlg::OnSelchangeAcComboPeakHoldTime()
{
   if (m_bDataOk) SetDataChanged();
}

void CAC_COMDlg::OnDeltaposAcSpinCalibration(NMHDR* pNMHDR, LRESULT* pResult)
{
   int nCalibration;
   if (pNMHDR && pResult)
   {
	   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
      nCalibration = pNMUpDown->iPos;
      if (m_bDataOk) SetDataChanged();
	   *pResult = 0;
   }
   else
   {
      nCalibration = (m_pACData->cCalibrationValue < BYTE(100)) ? (int)m_pACData->cCalibrationValue : 100 + (-(int)m_pACData->cCalibrationValue);
      GetDlgItem(IDC_AC_SPIN_CALIBRATION)->SendMessage(UDM_SETPOS, 0, (LPARAM) MAKELONG((short) nCalibration, 0));
   }
   CString str;
   str.Format("%1.2f dB", 0.1*nCalibration);
   CDataExchange dx(this, false);
	DDX_Text(&dx, IDC_AC_EDT_CALIBRATION, str);
}

void CAC_COMDlg::OnDeltaposAcSpinMemoCounter(NMHDR* pNMHDR, LRESULT* pResult)
{
   int nOutput, nValue = m_pACData->nMemo;
   if (pNMHDR && pResult)
   {
	   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
      nValue = pNMUpDown->iPos;
      if (m_bDataOk) SetDataChanged();
   	*pResult = 0;
   }
   else
   {
      GetDlgItem(IDC_AC_SPIN_MEMO_COUNTER)->SendMessage(UDM_SETPOS, 0, (LPARAM) MAKELONG((short) nValue, 0));
   }
   nOutput = (nValue + 1) *4;
   CDataExchange dx(this, false);
	DDX_Text(&dx, IDC_AC_EDT_MEMO_COUNTER, nOutput);

   if (pNMHDR && pResult)
   {
      m_pACData->nMemo = nValue;
   }
}

void CAC_COMDlg::OnDeltaposAcSpinNoOfSoundsAuto(NMHDR* pNMHDR, LRESULT* pResult)
{
   int nOutput, nValue = m_pACData->nSound;
   if (pNMHDR && pResult)
   {
	   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
      nValue = pNMUpDown->iPos;
      if (m_bDataOk) SetDataChanged();
   	*pResult = 0;
   }
   else
   {
      GetDlgItem(IDC_AC_SPIN_NO_OF_SOUNDS_AUTO)->SendMessage(UDM_SETPOS, 0, (LPARAM) MAKELONG((short) nValue, 0));
   }

   nOutput = (nValue + 1) *4;
   CDataExchange dx(this, false);
	DDX_Text(&dx, IDC_AC_EDT_NO_OF_SOUNDS_AUTO, nOutput);

   if (pNMHDR && pResult)
   {
      m_pACData->nSound = nValue;
   }
}

void CAC_COMDlg::OnDeltaposAcSpinBadPoints(NMHDR* pNMHDR, LRESULT* pResult)
{
   int nOutput, nValue = m_pACData->nInvQuality;
   if (pNMHDR && pResult)
   {
	   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
      nValue = pNMUpDown->iPos;
      if (m_bDataOk) SetDataChanged();
   	*pResult = 0;
   }
   else
   {
      GetDlgItem(IDC_AC_SPIN_BAD_POINTS)->SendMessage(UDM_SETPOS, 0, (LPARAM) MAKELONG((short) nValue, 0));
   }
   nOutput = 16 - nValue;
   CDataExchange dx(this, false);
	DDX_Text(&dx, IDC_AC_EDT_BAD_POINTS, nOutput);

   if (pNMHDR && pResult)
   {
      m_pACData->nInvQuality = nValue;
   }
}

void CAC_COMDlg::OnAcCkNetwork() 
{
   CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_AC_CK_NETWORK, m_bNetwork);
   GetDlgItem(IDC_AC_BTN_CONF_NETWORK)->EnableWindow(m_bNetwork);
   OnChange();
}

void CAC_COMDlg::OnChange()
{
   if (m_bDataOk) SetDataChanged();
}

void CAC_COMDlg::OnDeltaposAcSpinWeighting(NMHDR* pNMHDR, LRESULT* pResult)
{
   int nOutput, nValue = m_pACData->nWeigting;
   if (pNMHDR && pResult)
   {
	   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
      nValue = pNMUpDown->iPos;
      if (m_bDataOk) SetDataChanged();
   	*pResult = 0;
   }
   else
   {
      GetDlgItem(IDC_AC_SPIN_WEIGHTING)->SendMessage(UDM_SETPOS, 0, (LPARAM) MAKELONG((short) nValue, 0));
   }

   nOutput = (nValue + 1) *4;
   CDataExchange dx(this, false);
	DDX_Text(&dx, IDC_AC_EDT_WEIGHTING, nOutput);

   if (pNMHDR && pResult)
   {
      m_pACData->nWeigting = nValue;
   }
}

void CAC_COMDlg::OnSelchangingAcTabTables(NMHDR* pNMHDR, LRESULT* pResult)
{
	int i, nCursel;
   if ((pNMHDR != NULL) && (pResult != NULL))
   {
      nCursel= ::SendMessage(pNMHDR->hwndFrom, TCM_GETCURSEL, 0, 0);
	   *pResult = 0;
   }
   else nCursel = (int) pResult;

   for (i=FIRST_TAB_CTRL; i<AC_TABLES_CTRLS-1; i++)
   {
      if (gm_pnTabTables[nCursel][i])
         GetDlgItem(gm_pnTabTables[nCursel][i])->ShowWindow(SW_HIDE);
   }
}

void CAC_COMDlg::OnSelchangeAcTabTables(NMHDR* pNMHDR, LRESULT* pResult)
{
	int i, nCursel;
   if ((pNMHDR != NULL) && (pResult != NULL))
   {
      nCursel= ::SendMessage(pNMHDR->hwndFrom, TCM_GETCURSEL, 0, 0);
	   *pResult = 0;
   }
   else nCursel = (int) pResult;
   for (i=FIRST_TAB_CTRL; i<AC_TABLES_CTRLS-1; i++)
   {
      if (gm_pnTabTables[nCursel][i])
         GetDlgItem(gm_pnTabTables[nCursel][i])->ShowWindow(SW_SHOW);
   }
}

void CAC_COMDlg::OnSelchangeAcTabSystemConfig(NMHDR* pNMHDR, LRESULT* pResult)
{
	int i, nCursel;
   if ((pNMHDR != NULL) && (pResult != NULL))
   {
      nCursel= ::SendMessage(pNMHDR->hwndFrom, TCM_GETCURSEL, 0, 0);
	   *pResult = 0;
   }
   else nCursel = (int) pResult;
   for (i=FIRST_TAB_CTRL; i<AC_SYSTEMCONFIG_CTRLS-1; i++)
   {
      if (gm_pnTabSystemConfig[nCursel][i])
         GetDlgItem(gm_pnTabSystemConfig[nCursel][i])->ShowWindow(SW_SHOW);
   }
}

void CAC_COMDlg::OnSelchangingAcTabSystemConfig(NMHDR* pNMHDR, LRESULT* pResult)
{
	int i, nCursel;
   if ((pNMHDR != NULL) && (pResult != NULL))
   {
      nCursel= ::SendMessage(pNMHDR->hwndFrom, TCM_GETCURSEL, 0, 0);
	   *pResult = 0;
   }
   else nCursel = (int) pResult;

   for (i=FIRST_TAB_CTRL; i<AC_SYSTEMCONFIG_CTRLS-1; i++)
   {
      if (gm_pnTabSystemConfig[nCursel][i])
         GetDlgItem(gm_pnTabSystemConfig[nCursel][i])->ShowWindow(SW_HIDE);
   }
}

LRESULT CAC_COMDlg::OnDropFiles(WPARAM wParam, LPARAM lParam)
{
   UINT nReturn = ::DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0);
   if (nReturn == 1)
   {
      nReturn = ::DragQueryFile((HDROP)wParam, 0, m_strFileName.GetBufferSetLength(MAX_PATH), MAX_PATH);
      m_strFileName.ReleaseBuffer();
      if (nReturn == 1)
      {
         SaveChangedData();
         ReadData();
      }
   }
   else if (nReturn > 1)
   {
      UINT i;
      CMeasurementList Old;
      while (!m_Measurements.IsEmpty())                        // Daten sichern
      {
         Old.AddTail(m_Measurements.RemoveHead());
      }
      
      for (i=0; i<nReturn; i++)
      {
         ::DragQueryFile((HDROP)wParam, i, m_strFileName.GetBufferSetLength(MAX_PATH), MAX_PATH);
         m_strFileName.ReleaseBuffer();
         ReadData();
         while (!m_Measurements.IsEmpty())                     // Daten sichern
         {
            Old.AddTail(m_Measurements.RemoveHead());
         }
      }
      while (!Old.IsEmpty())                                    // alte Daten zurückspeichern (davor)
      {
         m_Measurements.AddHead(Old.RemoveTail());
      }
   }
   ::DragFinish((HDROP)wParam);

   return 0;
}

LRESULT CAC_COMDlg::OnMenuSelect(WPARAM wParam, LPARAM lParam)
{
   UINT uItem  = (UINT) LOWORD(wParam); // menu item or submenu index 
   UINT uFlags = (UINT) HIWORD(wParam); // menu flags 
   HMENU hmenu = (HMENU) lParam;        // handle to menu clicked 
   if ((uFlags != 0xffff) && (hmenu != NULL))
   {
      if (uItem > 20) SetStatusText(uItem);
      else            SetStatusText("");
   }
   else
   {
      SetStatusText("");
   }
   return 1;
}

void CAC_COMDlg::OnOptionsFrqRspCor() 
{
   if (CAC_COMApp::gm_bSpecialistMode)
      CAudioControlData::EditFrequencyResponseCorr();
}

LRESULT CAC_COMDlg::OnMsgFrom2DView(WPARAM wParam, LPARAM lParam)
{
   BEGIN("OnMsgFrom2DView");
   DWORD                dwProcessId = (DWORD)lParam;
   PROCESS_INFORMATION *pi;
   POSITION             pos, ActualPos;

   if (CAC_COMApp::gm_nCARAVersion <= 200)   // in Versionen < 2.00 kann in lParam etwas anderes als die 
   {                                         // ProcessID zurückgeliefert werden.
      if ((m_CalledProcess.GetCount()==1) && // Ist nur eine Instanz geöffnet und
          (dwProcessId == 1))                // ProcessId == 1, d.h. : Daten geändert
      {
         pos = m_CalledProcess.GetHeadPosition();// wird dafür gesorgt,
         pi= (PROCESS_INFORMATION*) m_CalledProcess.GetAt(pos);// daß der Prozeß aus der Liste entfernt wird.
         if (pi != NULL) dwProcessId = pi->dwProcessId;
      }
   }
   else                                      // in neueren Versionen ist lParam immer die Prozeß ID
   {
      lParam = HIWORD(wParam);               // der Rückgabeparameter "geändert" steht dann im HIWORD(wParam)
   }

   if (LOWORD(wParam) == TARGET_FUNCTION_CHANGEABLE)
   {
      CAudioControlData::Return2DFrequencyResponseCorr(lParam);
   }

   pos = m_CalledProcess.GetHeadPosition();
   while (pos)
   {
      ActualPos = pos;
      pi= (PROCESS_INFORMATION*) m_CalledProcess.GetNext(pos);
      if ((pi != NULL) && (pi->dwProcessId == dwProcessId))
      {
         DeleteProcessInfo(pi);
         m_CalledProcess.RemoveAt(ActualPos);
         break;
      }
   }
   return 1;
}

void CAC_COMDlg::OnAcBtnShowCurves()
{
   CPlot   *pPlots = NULL;
   int      nPlots = 1;
   bool     bAutoRange = true;

   CAudioControlData *pACData = m_pACData;
   if (m_nMeasureNo != -1)
   {
      POSITION pos;
      CMeasurement *pM = m_EvalMeasurement.GetMeasurement(m_nMeasureNo, pos);
      if (pM) pACData = &pM->m_ACData;
   }
   try
   {
      if (!m_bDataOk) throw (int) NO_DATA;
      int nReturn = pACData->GetPlots(&pPlots, nPlots, bAutoRange, NULL, 0);
      if (nReturn != 0) throw nReturn;
   }
   catch (int nError)
   {
      switch (nError)
      {
         case NO_CORRECT_CURVE:
            AfxMessageBox(IDS_NO_CORRECT_CURVE, MB_OK|MB_ICONSTOP);
         case ERROR_SETCURVE:
         case ERROR_SETVALUE:
            if (pPlots) delete[] pPlots;
            pPlots = NULL;
            break;
         default: break;
      }
   }

   if (pPlots && (pPlots[0].GetNumCurves() > 0))
   {
      CString strHeadline;
      m_cMeaningOfTestVal.GetLBText(pACData->cMeaningOfTestValues, strHeadline);
      pPlots[0].SetHeading(strHeadline);
      ShowPlot(nPlots, pPlots, bAutoRange);
   }
   if (pPlots) delete[] pPlots;
}

void CAC_COMDlg::ShowPlot(int nPlots, CPlot *pPlots, bool bAutoRange)
{
   while (true)
   {
      PROCESS_INFORMATION *pi= new PROCESS_INFORMATION;
      ZeroMemory(pi, sizeof(PROCESS_INFORMATION));
      char szEditFileName[MAX_PATH*2] = "";
      if (Call2DView(CAC_COMApp::gm_strCARAPath, nPlots, pPlots, bAutoRange, CAC_COMApp::gm_bEditCurves, m_hWnd, WM_FROM_2D_VIEW,
                     CAC_COMApp::gm_bEditCurves ? TARGET_FUNCTION_CHANGEABLE:TARGET_FUNCTION_FIXED,
                     szEditFileName, pi))
      {
         if (CAC_COMApp::gm_bEditCurves && (szEditFileName[0] !=0))
            CAC_COMApp::gm_strEditFileName = _T(szEditFileName);
         CString str  = CAC_COMApp::gm_strCARAPath + _T("ETS2DV.exe"); 
         DWORD dwDummy;
         int   bytes = GetFileVersionInfoSize((char*)LPCTSTR(str), &dwDummy);
         if(bytes)
         {
            void * buffer = new char[bytes];
            GetFileVersionInfo((char*)LPCTSTR(str), 0, bytes, buffer);
            VS_FIXEDFILEINFO * Version;
            unsigned int     length;
            if(VerQueryValue(buffer,"\\",(void **) &Version, &length))
            {
               int nVersion = HIWORD(Version->dwFileVersionMS);
               int nSubVers = LOWORD(Version->dwFileVersionMS);
               if ((nSubVers > 0) && (nSubVers < 10)) nSubVers *= 10;
               CAC_COMApp::gm_nCARAVersion = 100*nVersion + nSubVers;
            }
            delete[] buffer;
         }
         m_CalledProcess.AddHead(pi);
         break;
      }
      else
      {
         DeleteProcessInfo(pi);
         CString str;
         str.LoadString(IDS_SELECT_VIEW_PROG);
         if (!((CAC_COMApp*)AfxGetApp())->GetFolderPath(CAC_COMApp::gm_strCARAPath, NULL, FOLDERPATH_CONCAT_SLASH, str))
            break;
      }
   }
}

void CAC_COMDlg::OnTimer(UINT nIDEvent) 
{
   switch (nIDEvent)
   {
      case ID_POLL_AC_DATA_MEASUREMENT: PollMeasurementEvent(nIDEvent); break;

      case ID_WAIT_ERROR_TIMER:                                // Update der Fehleranzeige in der Statusbar
      {  KillTimerSave(ID_WAIT_ERROR_TIMER);                   // nach Ablauf dieser Message wieder möglich
         m_bWaitErrorTime = false;
      }break;

      default:                                                 // Timermeldung für Statusbarupdate mißbraucht
      {                                                        // da diese nicht überlaufen kann
         SetStatusText(nIDEvent);
      } break;
   }

   CDialog::OnTimer(nIDEvent);
}

void CAC_COMDlg::PollMeasurementEvent(UINT nIDEvent)
{
   int nResult = GetDataFromAC();
   TRACE("Poll : %d : %d\n", nResult, m_pACData->cMeaningOfTestValues);
   m_PMState.PollingIsOk = (nResult == 0) ? 1 : 0;

   if (m_PMState.PollingIsOk && !m_PMState.PollingWasOk && (m_pACData->cMeaningOfTestValues != 0))
   {
      CString str;
      switch (m_pACData->cMeaningOfTestValues)
      {
         case DB_VALUES_FROM_DATA_LOGGER:
         {
            int nPHtime = m_pACData->GetPeakHoldTime();
            if (nPHtime)
            {
               KillTimerSave(nIDEvent);
               SetTimerSave(nIDEvent, nPHtime, NULL);
            }
         } break;
         case VALUES_LEFT_SPEAKER: case VALUES_CENTER_SPEAKER: case VALUES_RIGHT_SPEAKER:
         case SELF_TEST_AUTO_EQ:   case AUTO_BALANCE_FUNCTION: case AUTO_SOUND:
         {
            if (m_pCDPlayer) m_pCDPlayer->SendMessage(WM_COMMAND, IDC_CD_STOP, 0);
            if (m_Measurements.GetCount())
            {
               CMeasurement *pM = m_Measurements.GetTail();
               if (pM->m_ACData == *m_pACData) break;
            }
            m_Measurements.AddTail(new CMeasurement(*m_pACData));
            m_cMeaningOfTestVal.GetWindowText(str);
            m_PMState.Counter++;
            GetDlgItem(IDC_AC_BTN_EVALUATE_MEASUREMENTS)->EnableWindow((m_Measurements.GetCount() > 0) ? true : false);
            if (m_bShowCurveImediately)
               OnAcBtnShowCurves();
         }break;
      }
   }

   m_PMState.PollingWasOk = m_PMState.PollingIsOk;
   if ((m_PMState.Counter==1) && m_PMState.MeasureOnce)
   {
      StopMeasurement();
   }
}

UINT CAC_COMDlg::SetTimerSave(UINT nIDEvent, UINT nElapse, void (CALLBACK EXPORT *lpfnTimer)(HWND,UINT,UINT,DWORD))
{
   //if (nIDEvent
   UINT nResult = SetTimer(nIDEvent, nElapse, lpfnTimer);
   if (nResult)
   {
      m_Timers.AddTail((void*)nIDEvent);
   }
   return nResult;
}

void CAC_COMDlg::KillTimerSave(UINT nID)
{
   MSG msg;
   ZeroMemory(&msg, sizeof(MSG));
   KillTimer(nID);
   _asm CLD;
   PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE|PM_NOYIELD);

   POSITION pos = m_Timers.Find((void*)nID);
   if (pos)
   {
      m_Timers.RemoveAt(pos);
   }
}

void CAC_COMDlg::OnAppExit()
{
   OnOK();
}

void CAC_COMDlg::OnFileNew()
{
   SaveChangedData();
   m_strFileName.Empty();
   m_pACData->InitData();
   DeleteMeasurements();
   SetData(true);
   InitDlgCtrls();
}

void CAC_COMDlg::OnFileInsert() 
{
   if (((CAC_COMApp*)AfxGetApp())->DoFileDialog(false, true, IDR_MAINFRAME, m_strFileName))
   {
      CMeasurementList Old;
      while (!m_Measurements.IsEmpty())                        // Daten sichern
      {
         Old.AddTail(m_Measurements.RemoveHead());
      }
      if (::GetFileAttributes(m_strFileName) == FILE_ATTRIBUTE_DIRECTORY)
      {
         CString str = m_strFileName;
         const char *psz = LPCTSTR(str);
         int nLen = strlen(psz);
         const char *pszF = &psz[nLen+1];
         while (pszF[0] != 0)
         {
            CString str1 = _T(psz);
            CString str2 = _T("\\");
            CString str3 = _T(pszF);
            m_strFileName = str1 + str2 + str3;
            ReadData();
            while (!m_Measurements.IsEmpty())                     // Daten sichern
            {
               Old.AddTail(m_Measurements.RemoveHead());
            }
            nLen = strlen(pszF);
            pszF = &pszF[nLen+1];
         }
      }
      else
      {
         ReadData();
      }
      while (!Old.IsEmpty())                                    // alte Daten zurückspeichern (davor)
      {
         m_Measurements.AddHead(Old.RemoveTail());
      }
   }
}


void CAC_COMDlg::OnFileOpen()
{
   SaveChangedData();
   if (((CAC_COMApp*)AfxGetApp())->DoFileDialog(false, false, IDR_MAINFRAME, m_strFileName))
   {
      ReadData();
   }
}

void CAC_COMDlg::OnFileSave()
{
   if (m_strFileName.IsEmpty())
      OnFileSaveAs();
   else if (m_bDataOk && (m_bDataChanged&SAVED_CHANGED) && UpdateData(true))
   {
      SaveData();
   }
}

void CAC_COMDlg::OnFileSaveAs()
{
   bool bUntitled = false;
   if (m_strFileName.IsEmpty())
   {
      m_strFileName.LoadString(AFX_IDS_UNTITLED);
      bUntitled = true;
   }
   if (m_bDataOk && UpdateData(true))
   {
      if (((CAC_COMApp*)AfxGetApp())->DoFileDialog(true, false, IDR_MAINFRAME, m_strFileName))
         SaveData();
      else if (bUntitled)
         m_strFileName.Empty();
   }
}

void CAC_COMDlg::OnHelpFinder()
{
   WinHelp(0, HELP_FINDER);
}

LRESULT CAC_COMDlg::OnCommandHelp(WPARAM, LPARAM lParam)
{
	if (lParam == 0 && m_nIDHelp != 0)
		lParam = HID_BASE_CONTROL + m_nIDHelp;
	if (lParam != 0)
	{
		CWinApp* pApp = AfxGetApp();
		if (pApp != NULL)
			pApp->WinHelp(lParam);
		return TRUE;
	}
	return FALSE;
}

void CAC_COMDlg::WinHelp(DWORD dwData, UINT nCmd) 
{
   DWORD dwBase = dwData&0x000f0000;
   if (dwBase == HID_BASE_PROMPT)
   {
      dwData &= ~0x000f0000;
      dwData = dwData + HID_BASE_CONTROL;
   }
   else if (dwBase == HID_BASE_CONTROL)
   {
      RECT  rc;
      POINT pt;
      ::GetCursorPos(&pt);
      ::GetWindowRect(m_hWnd, &rc);
      if (::PtInRect(&rc, pt))
      {
         long lPtr[2] = {(long)&pt, 0};
         ::EnumChildWindows(m_hWnd, ChildWndFromPt, (LPARAM) lPtr);
         if (lPtr[1])
         {
            DWORD dwHelpID = ::GetWindowLong((HWND)lPtr[1], GWL_ID);
            if ((dwHelpID != 0x0000ffff) && (dwHelpID != 0x00000001))
               dwData = HID_BASE_CONTROL + dwHelpID;
            else
               dwData = HID_BASE_RESOURCE + m_nIDHelp;
         }
         else dwData = HID_BASE_RESOURCE + IDD_AC_COM_DIALOG;
      }
   }

	CDialog::WinHelp(dwData, nCmd);
}

BOOL CALLBACK CAC_COMDlg::ChildWndFromPt(HWND hwnd, LPARAM lParam)
{
   RECT  rc;
   long *plPtr = (long*)lParam;
   if ((::GetWindowContextHelpId(hwnd) != 0xffffffff) && ::IsWindowVisible(hwnd))
   {
      ::GetWindowRect(hwnd, &rc);
      if (::PtInRect(&rc, *((POINT*)plPtr[0])))
      {
         plPtr[1] = (long)hwnd;
         return 0;
      }
   }
   plPtr[1] = NULL;
   return 1;
}

void CAC_COMDlg::OnOptionsPort()
{
   CString strPortAdress;
   m_cPort.GetWindowText(strPortAdress);
   m_CommPort.ConfigDlg(m_hWnd, strPortAdress);
}

void CAC_COMDlg::OnKillfocusAcEdtCurveName() 
{
   CDataExchange dx(this, true);
	DDX_Text(&dx, IDC_AC_EDT_CURVENAME, m_strNewName);
}

void CAC_COMDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
   if (m_hWnd != gm_hWndActualHit)
   {
      TRACKMOUSEEVENT tme;
      tme.cbSize      = sizeof(TRACKMOUSEEVENT);
      tme.dwFlags     = TME_LEAVE;
      tme.hwndTrack   = m_hWnd;
      tme.dwHoverTime = HOVER_DEFAULT;
      _TrackMouseEvent(&tme);
      gm_hWndActualHit = m_hWnd;
      SetStatusText("");
   }
   CDialog::OnMouseMove(nFlags, point);
}

LRESULT CAC_COMDlg::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
   SetStatusText("");
   return 0;
}

BOOL CAC_COMDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CAC_COMDlg::OnOptionsBasicParams()      
{
   if (m_BasicParam.DoModal() == IDOK)
   {
      m_CommPort.SetTimeOutRead(m_BasicParam.m_nEventTime);
   }
}


void CAC_COMDlg::OnAcBtnStartMeasurementOnce() 
{
   m_PMState.MeasureOnce = true;	
   OnAcBtnStartMeasurement();
}

void CAC_COMDlg::OnAcBtnStartMeasurement() 
{
   if (m_PMState.Polling)
   {
      StopMeasurement();
   }
   else
   {
      BOOL bCKm1 = 0, bCKmn = 0;
      OnAcBtnGetdata();
      if (m_bDataOk)
      {
         SetTimerSave(ID_POLL_AC_DATA_MEASUREMENT, m_BasicParam.m_nPollMeasurementTime, NULL);
         m_PMState.Polling      = true;
         m_PMState.PollingWasOk = true;
         if (m_PMState.MeasureOnce) bCKm1 = 1;
         else                       bCKmn = 1;
         if (m_pACData->nPeakHold >= 5)
         {
            if (AfxMessageBox(IDS_SWITCH_OF_PEAKHOLD, MB_YESNO|MB_ICONQUESTION, IDC_AC_BTN_START_MEASUREMENT)==IDYES)
            {
               m_cPeakHold.SetCurSel(0);
               OnAcBtnPutdata();
            }
         }
      }
      CDataExchange dx(this, false);
	   DDX_Check(&dx, IDC_AC_BTN_START_MEASUREMENT_ONCE, bCKm1);
	   DDX_Check(&dx, IDC_AC_BTN_START_MEASUREMENT     , bCKmn);
   }
}

void CAC_COMDlg::StopMeasurement()
{
   BOOL bCKm = 0;
   KillTimerSave(ID_POLL_AC_DATA_MEASUREMENT);
   m_PMState.Reset();
   CDataExchange dx(this, false);
	DDX_Check(&dx, IDC_AC_BTN_START_MEASUREMENT_ONCE, bCKm);
	DDX_Check(&dx, IDC_AC_BTN_START_MEASUREMENT     , bCKm);
}

void CAC_COMDlg::OnAcBtnConfNetwork() 
{
   NotYetImplemented();
}

void CAC_COMDlg::OnContextHelp() 
{
   SendMessage(WM_SYSCOMMAND, SC_CONTEXTHELP);
}

void CAC_COMDlg::OnViewTabs(UINT nID) 
{
   int nButtons = 0;
   int nRange[2] = {ID_VIEW_TABS, ID_VIEW_BUTTONS};
   CWnd * pWnd1 = GetDlgItem(IDC_AC_TAB_TABLES);
   CWnd * pWnd2 = GetDlgItem(IDC_AC_TAB_SYSTEM_CONFIG);

   if (ID_VIEW_TABS == nID)
   {
      pWnd1->ModifyStyle(TCS_BUTTONS, 0);
      pWnd2->ModifyStyle(TCS_BUTTONS, 0);
      SetMenuItem(ID_VIEW_TABS, MFT_RADIOCHECK, (UINT) &nRange[0]);
   }
   else
   {
      pWnd1->ModifyStyle(0, TCS_BUTTONS);
      pWnd2->ModifyStyle(0, TCS_BUTTONS);
      nButtons = 1;
      SetMenuItem(ID_VIEW_BUTTONS, MFT_RADIOCHECK, (UINT) &nRange[0]);
   }
   AfxGetApp()->WriteProfileInt(REGKEY_DEFAULT, TAB_BUTTONS, nButtons);
}

bool CAC_COMDlg::OpenPort()
{
   CString strPortAdress;
   m_cPort.GetWindowText(strPortAdress);
   return m_CommPort.OpenPort(strPortAdress);
}

void CAC_COMDlg::CalcChecksum(CFileHeader &fh)
{
   POSITION pos = m_Measurements.GetHeadPosition();
   CMeasurement *pM;
   while (pos)
   {
      pM = m_Measurements.GetNext(pos);
      if (pM)
      {
         time_t time = pM->m_Time.GetTime();
         fh.CalcChecksum(&time, sizeof(time_t));
         fh.CalcChecksum(&pM->m_ACData, sizeof(CAudioControlData));
         fh.CalcChecksum(&pM->m_Position, sizeof(CVector));
      }
   }
}

void CAC_COMDlg::DeleteMeasurements()
{
   while (!m_Measurements.IsEmpty())
   {
      delete m_Measurements.RemoveHead();
   }
}

void CAC_COMDlg::OnAcBtnEvaluateMeasurements() 
{
   m_EvalMeasurement.DoModal();
}

void CAC_COMDlg::ShowCurve(int nMeasureNo)
{
   m_nMeasureNo = nMeasureNo;
   OnAcBtnShowCurves();
   m_nMeasureNo = -1;
}

void CAC_COMDlg::OnAcCkShowCurves() 
{
   CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_AC_CK_SHOW_CURVES, m_bShowCurveImediately);
}

void CAC_COMDlg::OnEditIrCodes() 
{
   CIRcodeDlg dlg;
   memcpy(dlg.m_cCD_CodeCommands , m_pACData->cCD_CodeCommands , CODE_COMMANDS);
   memcpy(dlg.m_cCD_CodeParameter, m_pACData->cCD_CodeParameter, CODE_PARAMETER);
   memcpy(dlg.m_cCD_CodeTrack    , m_pACData->cCD_CodeTrack    , CODE_TRACK);
   if (dlg.DoModal() == IDOK)
   {
      memcpy(m_pACData->cCD_CodeCommands , dlg.m_cCD_CodeCommands , CODE_COMMANDS);
      memcpy(m_pACData->cCD_CodeParameter, dlg.m_cCD_CodeParameter, CODE_PARAMETER);
      memcpy(m_pACData->cCD_CodeTrack    , dlg.m_cCD_CodeTrack    , CODE_TRACK);
   }
}


void CAC_COMDlg::OnAcSettings() 
{
   NMHDR   nmhdr;
   LRESULT lRes = 0;
   CDataExchange dx(this, true);
   DDX_Radio(&dx, IDC_AC_R_BASIC_AC_SETTINGS, m_nAC_Settings);
   if (m_nAC_Settings == 0)
   {
      nmhdr.hwndFrom = ::GetDlgItem(m_hWnd, IDC_AC_TAB_TABLES);
      OnSelchangingAcTabTables(&nmhdr, &lRes);
      ::ShowWindow(nmhdr.hwndFrom, SW_HIDE);
      nmhdr.hwndFrom = ::GetDlgItem(m_hWnd, IDC_AC_TAB_SYSTEM_CONFIG);
      OnSelchangeAcTabSystemConfig(&nmhdr,  &lRes);
      ::ShowWindow(nmhdr.hwndFrom, SW_SHOW);
   }
   else
   {
      nmhdr.hwndFrom = ::GetDlgItem(m_hWnd, IDC_AC_TAB_SYSTEM_CONFIG);
      OnSelchangingAcTabSystemConfig(&nmhdr,  &lRes);
      ::ShowWindow(nmhdr.hwndFrom, SW_HIDE);
      nmhdr.hwndFrom = ::GetDlgItem(m_hWnd, IDC_AC_TAB_TABLES);
      OnSelchangeAcTabTables(&nmhdr, &lRes);
      ::ShowWindow(nmhdr.hwndFrom, SW_SHOW);
   }
}


void CAC_COMDlg::OnOptionsMsgBoxBehaviour() 
{
   CMsgBoxBehaviour dlg;
   dlg.m_dwFlags = CAC_COMApp::gm_dwMessageBoxFlags;
   if (dlg.DoModal() == IDOK)
   {
      CAC_COMApp::gm_dwMessageBoxFlags = dlg.m_dwFlags;
   }
}

// CDPlayer
LRESULT CALLBACK CAC_COMDlg::SubClassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   long lValue  = ::GetWindowLong(hwnd, GWL_USERDATA);
   long lIDctrl = ::GetWindowLong(hwnd, GWL_ID);
   ASSERT(lValue != 0);
   switch (uMsg)
   {
      case WM_MOUSEMOVE:
      {
         DWORD dwHelpID  = ::GetWindowContextHelpId(hwnd);
         HWND hwndParent = ::GetParent(hwnd);                  // Help IDs der Ctrls zur Statusanzeige weiterleiten
         if ((lIDctrl == IDC_AC_TAB_SYSTEM_CONFIG)||(lIDctrl == IDC_AC_TAB_TABLES))
         {
            TCHITTESTINFO thi;
            POINTSTOPOINT(thi.pt, lParam);
            int nTab = ::SendMessage(hwnd, TCM_HITTEST, 0, (LPARAM)&thi);
            if (thi.flags & TCHT_ONITEMLABEL)
            {
               if (lIDctrl == IDC_AC_TAB_SYSTEM_CONFIG) dwHelpID = CAC_COMDlg::gm_pnTabSystemConfig[nTab][AC_SYSTEMCONFIG_CTRLS-1];
               else                                     dwHelpID = CAC_COMDlg::gm_pnTabTables[nTab][AC_TABLES_CTRLS-1];
            }
         }
         ::PostMessage(hwndParent, WM_TIMER, dwHelpID, 0);
         if (hwnd != gm_hWndActualHit)
         {
            TRACKMOUSEEVENT tme;
            tme.cbSize      = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags     = TME_LEAVE;
            tme.hwndTrack   = hwnd;
            tme.dwHoverTime = HOVER_DEFAULT;
            _TrackMouseEvent(&tme);
            gm_hWndActualHit = hwnd;
         }
      } break;
      case WM_MOUSELEAVE:
      {
         gm_hWndActualHit = ::GetParent(hwnd);
         TRACKMOUSEEVENT tme;
         tme.cbSize      = sizeof(TRACKMOUSEEVENT);
         tme.dwFlags     = TME_LEAVE;
         tme.hwndTrack   = gm_hWndActualHit;
         tme.dwHoverTime = HOVER_DEFAULT;
         _TrackMouseEvent(&tme);
      } break;
      case WM_LBUTTONDOWN:
      if (lIDctrl == IDC_AC_COMBO_MEANING_OF_TV) return 0;
      break;
      case WM_LBUTTONDBLCLK:
      case WM_KEYDOWN:
         if (lIDctrl == IDC_AC_COMBO_MEANING_OF_TV) return 0;
      break;
      case WM_DESTROY:
         ::SetWindowLong(hwnd, GWL_WNDPROC, lValue);
         break;
   }

   return ::CallWindowProc((WNDPROC)lValue, hwnd, uMsg, wParam, lParam);
}

void CAC_COMDlg::OnViewCdPlayer() 
{
   if (m_pCDPlayer == NULL)
   {
      m_pCDPlayer = new CEtsCDPlayer;
      if (m_pCDPlayer)
      {
         HWND hwnd = m_pCDPlayer->Create(m_hWnd);
         if (hwnd != NULL)
         {
            DWORD dwParam = AfxGetApp()->GetProfileInt(REGKEY_DEFAULT, CD_DRIVE, DEFAULT_CD_PLACE);
            dwParam |= EXIT_SIZE_MOVE_SET;
            dwParam &= ~EXIT_SIZE_MOVE_GET;
            ::PostMessage(hwnd, WM_EXITSIZEMOVE, dwParam, (LPARAM)m_hWnd);
            return;
         }
      }
   }
   else DestroyCDPlayer(true);
}

void CAC_COMDlg::DestroyCDPlayer(bool bCommand)
{
   if (m_pCDPlayer)
   {
      DWORD dwParam = 0xffffffff;
      m_pCDPlayer->SendMessage(WM_EXITSIZEMOVE, EXIT_SIZE_MOVE_GET, (LPARAM)&dwParam);
      if (dwParam == 0xffffffff) // kein CD-Player
      {
         dwParam = AfxGetApp()->GetProfileInt(REGKEY_DEFAULT, CD_DRIVE, DEFAULT_CD_PLACE);
         if (dwParam == 0xffffffff) dwParam = DEFAULT_CD_PLACE;
         else                       dwParam &= ~EXIT_SIZE_MOVE_SET;
      }
      else if (!bCommand)
      {
         dwParam |= EXIT_SIZE_MOVE_SET;
      }

      AfxGetApp()->WriteProfileInt(REGKEY_DEFAULT, CD_DRIVE, dwParam);

      m_pCDPlayer->SendMessage(WM_COMMAND, IDCANCEL, (LPARAM)m_hWnd);
      delete m_pCDPlayer;
      m_pCDPlayer = NULL;
   }
}

LRESULT CAC_COMDlg::OnEnterMenuLoop(WPARAM wParam, LPARAM lParam)
{
   HMENU hMenu  = (HMENU) wParam;
   UINT nItem   = ID_VIEW_CD_PLAYER;
   UINT nEnable = (ETSFoundInDefault("ETSCDPL.Dll") != 0) ? MF_ENABLED : MF_GRAYED|MF_DISABLED;
   UINT nCheck  = MF_UNCHECKED;
   if (m_pCDPlayer)
   {
       HWND hWnd = m_pCDPlayer->GetWindowHandle();
       if (hWnd != NULL) nCheck = MF_CHECKED;
       else DestroyCDPlayer(true);
   }
   SetMenuItem(nItem, nCheck, nEnable, hMenu);
   return 0;
}
/*
HBRUSH CAC_COMDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   switch (::GetWindowLong(pWnd->m_hWnd, GWL_ID))
   {
      case IDC_AC_R_DIGITAL:             case IDC_AC_R_ANALOG:
      case IDC_AC_R_STEREO:              case IDC_AC_R_SURROUND:
      case IDC_AC_R_RT60_SINUS:          case IDC_AC_R_RT60_RAUSCHEN:
      case IDC_AC_R_3WAY:                case IDC_AC_R_2WAY:
      case IDC_AC_R_FILTER_A:            case IDC_AC_R_FILTER_C:
      case IDC_AC_TXT_PEAK_HOLD_TIME:    case IDC_AC_TXT_CALIBRATION:
      case IDC_AC_CK_NETWORK:
      case IDC_AC_CK_AUTOBALANCE_DELTAT: case IDC_AC_CK_AUTOBALANCE_DB_LEVEL:
      case IDC_AC_TXT_NO_OF_SOUNDS_AUTO: case IDC_AC_TXT_MEMO_COUNTER:
      case IDC_AC_TXT_BAD_POINTS:
      case IDC_AC_TXT_WEIGHTING:
      {
         hbr = ::GetSysColorBrush(g_nCol);
         pDC->SetBkColor(::GetSysColor(g_nCol));
      } break;
   }	
	return hbr;
}
*/