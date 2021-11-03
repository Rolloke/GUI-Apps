// EvaluateMeasurements.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AC_COM.h"
#include "BasicParamDlg.h"
#include "EvaluateMeasurements.h"
#include "Measurement.h"
#include "AC_COMDlg.h"
#include "Curve.h" 
#include "MathCurve.h"
#include "Plot.h"
#include "FileHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEvaluateMeasurements 

#define SHOW_ALL_VALUES                  0  // Anzeige der Messwerte
#define SHOW_DB_VALUES_FROM_DATA_LOGGER  1
#define SHOW_VALUES_SPEAKER_TEST         2
#define SHOW_SELF_TEST_AUTO_EQ_STEREO    3
#define SHOW_SELF_TEST_AUTO_EQ_SURROUND  4
#define SHOW_AUTO_BALANCE_FUNCTION       5
#define SHOW_AUTO_SOUND                  6


#define UPDATE_LIST 2

int CEvaluateMeasurements::m_nColumnWidth[NO_OF_MEASUREMENT_COL] = {30, 120, 280, 60, 60, 60};

long CEvaluateMeasurements::gm_lOldEditCtrlWndProc = 0;
char CEvaluateMeasurements::gm_cDecimal[4] = ".";
char CEvaluateMeasurements::gm_cList[4]    = ",";

CEvaluateMeasurements::CEvaluateMeasurements(CWnd* pParent /*=NULL*/)
	: CDialog(CEvaluateMeasurements::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEvaluateMeasurements)
	m_nMeaningOfTestVal = -1;
	m_bShowCenter = TRUE;
	m_bShowLeft = TRUE;
	m_bShowRight = TRUE;
	m_bShowAverage = FALSE;
	//}}AFX_DATA_INIT
   m_pParent  = NULL;
   m_nItem    = -1;
   m_nSubItem = -1;
}


void CEvaluateMeasurements::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEvaluateMeasurements)
	DDX_Control(pDX, IDC_EM_COMBO_MEANING_OF_TV, m_cShowTestVal);
	DDX_Control(pDX, IDC_EM_MEASUREMENT_LIST, m_cMeasurements);
	DDX_Check(pDX, IDC_EM_CK_SHOW_CENTER, m_bShowCenter);
	DDX_Check(pDX, IDC_EM_CK_SHOW_LEFT, m_bShowLeft);
	DDX_Check(pDX, IDC_EM_CK_SHOW_RIGHT, m_bShowRight);
	DDX_Check(pDX, IDC_EM_CK_SHOW_AVERAGE, m_bShowAverage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEvaluateMeasurements, CDialog)
	//{{AFX_MSG_MAP(CEvaluateMeasurements)
   ON_MESSAGE(WM_HELP, OnHelp)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_EM_MEASUREMENT_LIST, OnGetdispinfoMeasurementList)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_EM_MEASUREMENT_LIST, OnRclickMeasurementList)
	ON_CBN_SELCHANGE(IDC_EM_COMBO_MEANING_OF_TV, OnSelchangeAcComboMeaningOfTv)
	ON_NOTIFY(NM_CLICK, IDC_EM_MEASUREMENT_LIST, OnClickEmMeasurementList)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_EM_MEASUREMENT_LIST, OnBeginlabeleditEmMeasurementList)
	ON_BN_CLICKED(IDC_EM_BTN_SHOW_CURVES, OnEmBtnShowCurves)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_EM_MEASUREMENT_LIST, OnEndlabeleditEmMeasurementList)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_EM_MEASUREMENT_LIST, OnCustomDraw)
	ON_WM_TIMER()
	ON_NOTIFY(HDN_ITEMCLICK, IDC_EM_MEASUREMENT_LIST, OnItemclickEmMeasurementList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_EM_MEASUREMENT_LIST, OnColumnclickEmMeasurementList)
	ON_BN_CLICKED(IDC_EM_BTN_DELETE_MEASUREMENT, OnEmBtnDeleteMeasurement)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDHELP, CDialog::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEvaluateMeasurements 

BOOL CEvaluateMeasurements::OnInitDialog() 
{
   if (!m_pParent) return false;
	CDialog::OnInitDialog();
   GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, gm_cDecimal, 4);
   GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST   , gm_cList   , 4);

   CString string;
   m_cMeasurements.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

   m_ImageList.Create(IDB_LIST_IMAGES, 13, 3, ILC_COLOR16);
   CImageList *pIL = m_cMeasurements.SetImageList(&m_ImageList, LVSIL_SMALL);
   if (pIL)
   {
      pIL->DeleteImageList();
   }

   string.LoadString(IDS_MEASUREMENT_NO);
   m_cMeasurements.InsertColumn(0, string, LVCFMT_LEFT, m_nColumnWidth[0]);
   string.LoadString(IDS_MEASUREMENT_TIME);
   m_cMeasurements.InsertColumn(1, string, LVCFMT_LEFT, m_nColumnWidth[1]);
   string.LoadString(IDS_MEASUREMENT);
   m_cMeasurements.InsertColumn(2, string, LVCFMT_LEFT, m_nColumnWidth[2]);
   string.LoadString(IDS_CALC_AVERAGE);
   m_cMeasurements.InsertColumn(3, string, LVCFMT_LEFT, m_nColumnWidth[3]);
   string.LoadString(IDS_OLD_TRACK9);
   m_cMeasurements.InsertColumn(4, string, LVCFMT_LEFT, m_nColumnWidth[4]);
   string.LoadString(IDS_NEW_TRACK10);
   m_cMeasurements.InsertColumn(5, string, LVCFMT_LEFT, m_nColumnWidth[5]);

   CDataExchange dx(this, false);
   DDX_CBIndex(&dx, IDC_EM_COMBO_MEANING_OF_TV, m_nMeaningOfTestVal);
   m_nMeaningOfTestVal = -1;
   OnSelchangeAcComboMeaningOfTv();

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CEvaluateMeasurements::SetMeaningOfTestVal(CAudioControlData *pACData)
{
   switch (pACData->cMeaningOfTestValues)
   {
      case NO_VALUES:                  m_nMeaningOfTestVal = SHOW_ALL_VALUES;
         break;
      case DB_VALUES_FROM_DATA_LOGGER: m_nMeaningOfTestVal = SHOW_DB_VALUES_FROM_DATA_LOGGER;
         break;
      case VALUES_LEFT_SPEAKER:        
      case VALUES_CENTER_SPEAKER:      
      case VALUES_RIGHT_SPEAKER:       m_nMeaningOfTestVal = SHOW_VALUES_SPEAKER_TEST;
         break;
      case SELF_TEST_AUTO_EQ:
         if (pACData->bLoudspeakermodeSurround)
                                       m_nMeaningOfTestVal = SHOW_SELF_TEST_AUTO_EQ_SURROUND;
         else                          m_nMeaningOfTestVal = SHOW_SELF_TEST_AUTO_EQ_STEREO;
         break;
      case AUTO_BALANCE_FUNCTION:      m_nMeaningOfTestVal = SHOW_AUTO_BALANCE_FUNCTION;
        break;
      case AUTO_SOUND:                 m_nMeaningOfTestVal = SHOW_AUTO_SOUND;
         break;
   }
}

void CEvaluateMeasurements::OnGetdispinfoMeasurementList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   CMeasurement *pM = NULL;
   POSITION pos;
   pM = GetMeasurement(pDispInfo->item.iItem, pos);
   if (!pM) return;
   if(pDispInfo->item.mask & LVIF_TEXT)
   {
      switch (pDispInfo->item.iSubItem)
      {
         case 0: wsprintf(pDispInfo->item.pszText, "%d", pDispInfo->item.iItem+1); break;
         case 1:
         {
            CString str = pM->m_Time.Format(IDS_MEASURE_TIME_FMT);
            strcpy(pDispInfo->item.pszText, str);
         } break;
         case 2: m_pParent->m_cMeaningOfTestVal.GetLBText(pM->m_ACData.cMeaningOfTestValues, pDispInfo->item.pszText); break;
         case 3:
         if (pM->m_ACData.cMeaningOfTestValues == DB_VALUES_FROM_DATA_LOGGER)
         {
            CString str;
            str.Format("%.2f%c %.2f", pM->m_dT1, gm_cList[0], pM->m_dT2);
            strcpy(pDispInfo->item.pszText, str);
         } break;
         case 4:
         if (pM->m_ACData.cMeaningOfTestValues == DB_VALUES_FROM_DATA_LOGGER)
         {
            CString str;
            str.Format("%.0f%c %.0f", pM->m_dF1, gm_cList[0], pM->m_dF2);
            strcpy(pDispInfo->item.pszText, str);
         } break;
      }
   }
   if (pDispInfo->item.mask & LVIF_IMAGE)
   {
      if (m_nMeaningOfTestVal == SHOW_ALL_VALUES)
         pDispInfo->item.iImage = 2;
      else
         pDispInfo->item.iImage = pM->m_ACData.bFree3 ? 1 : 0;
   }
	
	*pResult = 0;
}

void CEvaluateMeasurements::OnDestroy() 
{
   for (int i=0; i<NO_OF_MEASUREMENT_COL; i++)
   {
      m_nColumnWidth[i] = m_cMeasurements.GetColumnWidth(i);
   }
   m_ImageList.DeleteImageList();
	CDialog::OnDestroy();
}

void CEvaluateMeasurements::OnRclickMeasurementList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   UINT Flags = 0;
   POINT ptCursor, ptScreen;
   CMenu Menu, * pContext = NULL;
   LV_ITEM lvItem = { LVIF_STATE|LVIF_PARAM|LVIF_IMAGE, -1, 0, 0, LVIS_SELECTED, NULL, 0, 0, NULL};

   GetCursorPos(&ptCursor);
   ptScreen = ptCursor;
   m_cMeasurements.ScreenToClient(&ptCursor);
   ptCursor.x = 2;
   Menu.LoadMenu(IDR_MEASUREMENT_CONTEXT);
   pContext = Menu.GetSubMenu(0);
   if (pContext)
   {
      CMeasurement *pM = NULL;
      POSITION pos     = NULL;

      lvItem.iItem = m_cMeasurements.HitTest(ptCursor, &Flags);
      if ((lvItem.iItem != -1) && (Flags & LVHT_ONITEMLABEL))
      {
         m_cMeasurements.GetItem(&lvItem);
         if ((lvItem.state & LVIS_SELECTED)==0)
         {
            lvItem.iItem = -1;
         }
      }
      if (lvItem.iItem != -1)
      {
         pM = GetMeasurement(lvItem.iItem, pos);
         if (pM)
         {
            CString str,strFormat,
                    strTime = pM->m_Time.Format(IDS_MEASURE_TIME_FMT);
            int nPos = pContext->GetMenuItemCount()-1;
            strFormat.LoadString(ID_DELETE_MEASUREMENT);
            str.Format(strFormat, strTime);
            pContext->InsertMenu(nPos++, MF_BYPOSITION|MF_STRING, ID_DELETE_MEASUREMENT, str);
            pContext->InsertMenu(nPos, MF_BYPOSITION|MF_SEPARATOR);
         }
      }
      pContext->EnableMenuItem(ID_DISABLE_CHECKMARKS, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
      pContext->EnableMenuItem(ID_ENABLE_CHECKMARKS, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
      switch (pContext->TrackPopupMenu(TPM_RETURNCMD, ptScreen.x, ptScreen.y, this))
      {
         case ID_DELETE_MEASUREMENT:
         {
            if (pos && pM)
            {
               delete pM;
               m_pParent->m_Measurements.RemoveAt(pos);
               SetMeasurementCount();
               m_cMeasurements.InvalidateRect(NULL);
            }
         } break;
         default: 
          break;
      }
   }
   Menu.DestroyMenu();
	
	*pResult = 0;
}

bool CEvaluateMeasurements::IsMeasurementValid(CMeasurement *pM)
{
   switch (m_nMeaningOfTestVal)
   {
      case SHOW_ALL_VALUES: return true;
      case SHOW_DB_VALUES_FROM_DATA_LOGGER:
         if (pM->m_ACData.cMeaningOfTestValues == DB_VALUES_FROM_DATA_LOGGER) return true;
         break;
      case SHOW_VALUES_SPEAKER_TEST:
      if ((pM->m_ACData.cMeaningOfTestValues == VALUES_LEFT_SPEAKER  ) ||
          (pM->m_ACData.cMeaningOfTestValues == VALUES_CENTER_SPEAKER) ||
          (pM->m_ACData.cMeaningOfTestValues == VALUES_RIGHT_SPEAKER ) )return true;;
         break;
      case SHOW_SELF_TEST_AUTO_EQ_STEREO:
         if ((pM->m_ACData.cMeaningOfTestValues == SELF_TEST_AUTO_EQ) && 
             (pM->m_ACData.bLoudspeakermodeSurround == 0)) return true;
         break;
      case SHOW_SELF_TEST_AUTO_EQ_SURROUND:
         if ((pM->m_ACData.cMeaningOfTestValues == SELF_TEST_AUTO_EQ) && 
             (pM->m_ACData.bLoudspeakermodeSurround == 1)) return true;
         break;
      case SHOW_AUTO_BALANCE_FUNCTION:
         if (pM->m_ACData.cMeaningOfTestValues == AUTO_BALANCE_FUNCTION) return true;
         break;
      case SHOW_AUTO_SOUND:
         if (pM->m_ACData.cMeaningOfTestValues == AUTO_SOUND) return true;
         break;
   }
   return false;
}

CMeasurement* CEvaluateMeasurements::GetMeasurement(int nIndex, POSITION &pos)
{
   CMeasurement *pM;
   POSITION posOld;
   pos = m_pParent->m_Measurements.GetHeadPosition();
   int n = -1;
   while (pos)
   {
      posOld = pos;
      pM = m_pParent->m_Measurements.GetNext(pos);
      if (IsMeasurementValid(pM)) n++;
      if (n == nIndex)
      {
         pos = posOld;
         return pM;
      }
   }
   return NULL;
}

void CEvaluateMeasurements::SetMeasurementCount()
{
   CMeasurement *pM;
   POSITION pos = m_pParent->m_Measurements.GetHeadPosition();
   int n = 0;
   while (pos)
   {
      pM = m_pParent->m_Measurements.GetNext(pos);
      if (IsMeasurementValid(pM)) n++;
   }
   m_cMeasurements.DeleteAllItems();
   m_cMeasurements.SetItemCount(n);
}

void CEvaluateMeasurements::OnSelchangeAcComboMeaningOfTv() 
{
   int nOldVal = m_nMeaningOfTestVal;
   CDataExchange dx(this, true);
	DDX_CBIndex(&dx, IDC_EM_COMBO_MEANING_OF_TV, m_nMeaningOfTestVal);
	if (nOldVal != m_nMeaningOfTestVal)
   {
      bool bEnable       = false;
      bool bSurround     = false;
      SetMeasurementCount();
      m_cMeasurements.InvalidateRect(NULL);
      GetDlgItem(IDC_EM_BTN_DELETE_MEASUREMENT)->EnableWindow(false);

      switch (m_nMeaningOfTestVal)
      {
         case SHOW_SELF_TEST_AUTO_EQ_SURROUND:
            bSurround = true;
         case SHOW_SELF_TEST_AUTO_EQ_STEREO:
            bEnable = true;
            break;
         default: break;
      }
      CString str1, str2, str3, str, slash = _T("/");
      LVCOLUMN lvc;
      lvc.mask = LVCF_TEXT;
      switch (m_nMeaningOfTestVal)
      {
         case SHOW_DB_VALUES_FROM_DATA_LOGGER:
            str1.LoadString(IDS_DATALOGGER_T1_T2);
            str2.LoadString(IDS_DATALOGGER_F1_F2);
            str3.LoadString(IDS_DATALOGGER_F_LIN);
            break;
         case SHOW_ALL_VALUES:
            str1.LoadString(IDS_CALC_AVERAGE);
            str.LoadString(IDS_DATALOGGER_T1_T2);
            str1 = str1 + slash + str;
            str2.LoadString(IDS_OLD_TRACK9);
            str.LoadString(IDS_DATALOGGER_F1_F2);
            str2 = str2 + slash + str;
            str3.LoadString(IDS_NEW_TRACK10);
            str.LoadString(IDS_DATALOGGER_F_LIN);
            str3 = str3 + slash + str;
            break;
         case SHOW_SELF_TEST_AUTO_EQ_SURROUND:
         case SHOW_SELF_TEST_AUTO_EQ_STEREO:
            str2.LoadString(IDS_OLD_TRACK9);
            str3.LoadString(IDS_NEW_TRACK10);
         case SHOW_VALUES_SPEAKER_TEST:
            str1.LoadString(IDS_CALC_AVERAGE);
            break;
      }
      lvc.pszText    = (char*)LPCTSTR(str1);
      lvc.cchTextMax = str1.GetLength();
      m_cMeasurements.SetColumn(3, &lvc);
      lvc.pszText    = (char*)LPCTSTR(str2);
      lvc.cchTextMax = str2.GetLength();
      m_cMeasurements.SetColumn(4, &lvc);
      lvc.pszText    = (char*)LPCTSTR(str3);
      lvc.cchTextMax = str3.GetLength();
      m_cMeasurements.SetColumn(5, &lvc);

      GetDlgItem(IDC_EM_CK_SHOW_CENTER)->EnableWindow(bSurround);
      GetDlgItem(IDC_EM_CK_SHOW_LEFT)->EnableWindow(bEnable);
      GetDlgItem(IDC_EM_CK_SHOW_RIGHT)->EnableWindow(bEnable);
      GetDlgItem(IDC_EM_CK_SHOW_AVERAGE)->EnableWindow(bEnable);
   }
}

void CEvaluateMeasurements::OnClickEmMeasurementList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LVHITTESTINFO lvHTI;

   GetCursorPos(&lvHTI.pt);
   m_cMeasurements.ScreenToClient(&lvHTI.pt);

   m_cMeasurements.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHTI);
   if (lvHTI.iItem != -1)
   {
      POSITION pos     = NULL;
      CMeasurement *pM = GetMeasurement(lvHTI.iItem, pos);
      if (pM)
      {
         GetDlgItem(IDC_EM_BTN_DELETE_MEASUREMENT)->EnableWindow(true);
         if (lvHTI.flags == LVHT_ONITEMICON)
         {
            bool bChanged = false;
            if (lvHTI.iSubItem == 0)
            {
               pM->m_ACData.bFree3 = !pM->m_ACData.bFree3, bChanged = true;
            }
            else
            {
               switch (m_nMeaningOfTestVal)
               {
                  case SHOW_DB_VALUES_FROM_DATA_LOGGER:
                  {
                     if(lvHTI.iSubItem == 5)
                     {
                        pM->m_ACData.bFree4 = !pM->m_ACData.bFree4, bChanged = true;
                     }
                  } break;
                  case SHOW_VALUES_SPEAKER_TEST:
                  {
                     if(lvHTI.iSubItem == 3)
                     {
                        pM->m_ACData.bFree2 = !pM->m_ACData.bFree2, bChanged = true;
                     }
                  } break;
                  case SHOW_SELF_TEST_AUTO_EQ_STEREO:
                  case SHOW_SELF_TEST_AUTO_EQ_SURROUND:
                  {
                     switch(lvHTI.iSubItem)
                     {
                        case 3:
                           pM->m_ACData.bFree2 = !pM->m_ACData.bFree2, bChanged = true;
                           break;
                        case 4:
                           if (!(CAC_COMApp::gm_dwMessageBoxFlags&MBF_WARN_TRACK9) || AfxMessageBox(IDS_TRACK_9_OF_THE_TESTCD, MB_ICONINFORMATION|MB_OKCANCEL|MB_HELP, IDC_BS_CK_TRACK9OLD)==IDOK)
                           {
                              pM->m_ACData.bFree1 = !pM->m_ACData.bFree1, bChanged = true;
                              if (pM->m_ACData.bFree1) pM->m_ACData.bFree4 = false;
                           } break;
                        case 5:
                           if (!(CAC_COMApp::gm_dwMessageBoxFlags&MBF_WARN_TRACK10) || AfxMessageBox(IDS_TRACK_10_OF_THE_TESTCD, MB_ICONINFORMATION|MB_OKCANCEL|MB_HELP, IDC_BS_CK_TRACK10NEW)==IDOK)
                           {
                              pM->m_ACData.bFree4 = !pM->m_ACData.bFree4, bChanged = true;
                              if (pM->m_ACData.bFree4) pM->m_ACData.bFree1 = false;
                           } break;
                     }
                  } break;
               }
            }
            if (bChanged)
            {
               m_cMeasurements.Update(lvHTI.iItem);
               m_pParent->SetDataChanged();
            }
         }
      }
   }
   else if (m_nItem != -1)
   {
      m_cMeasurements.SetItemState(m_nItem, 0, LVIS_FOCUSED);
      PostMessage(WM_TIMER, UPDATE_LIST, 0);
   }
   m_nItem = lvHTI.iItem;

   if (m_nItem == -1)
   {
      GetDlgItem(IDC_EM_BTN_DELETE_MEASUREMENT)->EnableWindow(false);
   }

	*pResult = 0;
}

void CEvaluateMeasurements::OnEmBtnShowCurves() 
{
   CPlot *pPlots    = NULL;
   CCurve*pCurve    = NULL;
   int   nPlots     = 0;
   bool  bAutoRange = false;
   int   nDiv;

   switch (m_pParent->m_BasicParam.m_nDivision)
   {
      case 0: nDiv = CCURVE_LINEAR;      break;
      case 1: nDiv = CCURVE_LOGARITHMIC; break;
      case 2: nDiv = CCURVE_FREQUENCY;   break;
   }

   if (!UpdateData(true)) return;
   switch (m_nMeaningOfTestVal)
   {
      case SHOW_ALL_VALUES:
      case SHOW_AUTO_BALANCE_FUNCTION:
      case SHOW_AUTO_SOUND:
      {                                                        // eine Einzelmessung wird angezeigt
         m_pParent->ShowCurve(m_cMeasurements.GetSelectionMark());
      } return;
      case SHOW_DB_VALUES_FROM_DATA_LOGGER:                    // Messungen aneinanderhängen
      {
         CMeasurement *pM;
         int nFrequencyLin = 0;
         int nFrequencyLog = 0;
         int nLogger       = 0;
         int  pnCurves[1];
         POSITION pos  = m_pParent->m_Measurements.GetHeadPosition();
         while (pos)
         {
            pM = m_pParent->m_Measurements.GetNext(pos);
            if (IsMeasurementValid(pM))
            {
               if (pM->m_ACData.bFree3)                        // soll die SelfTestkurve angezeigt werden
               {
                  if (pM->m_ACData.bFree1)
                  {
                     if (pM->m_ACData.bFree4) nFrequencyLin++;
                     else                     nFrequencyLog++;
                  }
                  else nLogger++;
               }
            }
         }
         try
         {
            nPlots = 0;
            int nFLin = -1;
            int nFLog = -1;
            int nL    = -1;
            int nP    = 1;
            if (nFrequencyLin) nFLin = nPlots++;
            if (nFrequencyLog) nFLog = nPlots++;
            if (nLogger)       nL    = nPlots++;
            CString strHeadline;
            if (nPlots)
            {
               pPlots = new CPlot[nPlots];
               if (!pPlots)                     throw (int) ERROR_SETCURVE;
               if (nFrequencyLin)
               {
                  if (!pPlots[nFLin].SetSize(nFrequencyLin)) throw (int) ERROR_SETCURVE;
                  pPlots[nFLin].SetXDivision(CCURVE_LINEAR);
                  strHeadline.LoadString(IDS_FREQUENCY_RESPONSE);
                  pPlots[nFLin].SetHeading(strHeadline);
               }
               if (nFrequencyLog)
               {
                  if (!pPlots[nFLog].SetSize(nFrequencyLog)) throw (int) ERROR_SETCURVE;
                  pPlots[nFLog].SetXDivision(nDiv);
                  strHeadline.LoadString(IDS_FREQUENCY_RESPONSE);
                  pPlots[nFLog].SetHeading(strHeadline);
               }
               if (nLogger)
               {
                  if (!pPlots[nL].SetSize(nLogger)) throw (int) ERROR_SETCURVE;
                  pPlots[nL].SetXDivision(CCURVE_LINEAR);
                  m_cShowTestVal.GetLBText(m_nMeaningOfTestVal, strHeadline);
                  pPlots[nL].SetHeading(strHeadline);
               }
               nFrequencyLin = 0;
               nFrequencyLog = 0;
               nLogger       = 0;
               CPlot *pP = NULL;
               pos = m_pParent->m_Measurements.GetHeadPosition();
               while (pos)
               {
                  pM = m_pParent->m_Measurements.GetNext(pos);
                  if (IsMeasurementValid(pM))
                  {                                                  // soll die SelfTestkurve angezeigt werden
                     if (pM->m_ACData.bFree3)                        // soll die SelfTestkurve angezeigt werden
                     {
                        if (pM->m_ACData.bFree1)
                        {
                           if (pM->m_ACData.bFree4) pP = &pPlots[nFLin], pnCurves[0] = nFrequencyLin++;
                           else                     pP = &pPlots[nFLog], pnCurves[0] = nFrequencyLog++;
                        }
                        else                        pP = &pPlots[nL   ], pnCurves[0] = nLogger++;
                        int nReturn = pM->m_ACData.GetPlots(&pP, nP, bAutoRange, pnCurves, pnCurves[0]+1);
                        if (nReturn != 0) throw nReturn;
                     }
                  }
               }
            }
         }
         catch (int nError)
         {
            if ((nError == ERROR_SETCURVE) || (nError == ERROR_SETVALUE))
            {
               if (pPlots) delete[] pPlots;
               pPlots = NULL;
            }
         }
      }break;
      case SHOW_VALUES_SPEAKER_TEST:                           // SpeakerTest
      {
         CMeasurement *pM;
         bool *pbCalcAverage = NULL;
         bool *pbShowCurve = NULL;
         int nCurves   = 0;
         int nAVG      = 0;
         int nShown    = 0;
         POSITION pos  = m_pParent->m_Measurements.GetHeadPosition();
         while (pos)
         {
            pM = m_pParent->m_Measurements.GetNext(pos);
            if (IsMeasurementValid(pM))
            {                                                  // soll die SelfTestkurve angezeigt werden
               nCurves++;                                      // Datenzähler erhöhen
               if (pM->m_ACData.bFree2) nAVG++;
               if (pM->m_ACData.bFree3) nShown++;
            }
         }
         if ((nCurves == 0)||((nShown == 0) && (nAVG < 2)))
         {
            // Fehler
         }
         else
         {
            try
            {
               bool bShowAverage = (nAVG > 1) ? true : false;
               int  pnCurves[1];
               if (bShowAverage)
               {
                  pbCalcAverage = new bool[nCurves];
                  nCurves++;
               }
               pbShowCurve = new bool[nCurves];
               if (bShowAverage) pbShowCurve[nCurves-1] = true;
               nPlots = 2;
               pPlots = new CPlot[nPlots];
               if (!pPlots)                     throw (int) ERROR_SETCURVE;
               if (!pPlots[0].SetSize(nCurves)) throw (int) ERROR_SETCURVE;
               if (!pPlots[1].SetSize(nCurves)) throw (int) ERROR_SETCURVE;
               pPlots[0].SetXDivision(nDiv);
               pPlots[1].SetXDivision(nDiv);

               nCurves = 0;
               nAVG    = 0;
               pos = m_pParent->m_Measurements.GetHeadPosition();
               while (pos)
               {
                  pM = m_pParent->m_Measurements.GetNext(pos);
                  if (IsMeasurementValid(pM))
                  {                                                  // soll die SelfTestkurve angezeigt werden
                     if (bShowAverage)
                     {
                        pbCalcAverage[nCurves] = pM->m_ACData.bFree2;
                     }
                     pbShowCurve[nCurves] = pM->m_ACData.bFree3;
                     pnCurves[0] = nCurves;
                     nCurves++;
                     int nReturn = pM->m_ACData.GetPlots(&pPlots, nPlots, bAutoRange, pnCurves, nCurves);
                     if (nReturn != 0) throw nReturn;
                  }
               }
               if (bShowAverage)
               {
                  int i, j, nValues, nAVG0, nAVG1;
                  double dSum0, dSum1, dVal;
                  CString strAVG;
                  CCurve *pAvgCurve1, *pAvgCurve2;
                  nCurves = pPlots[0].GetNumCurves()-1;
                  pCurve = pPlots[0].GetCurve(0);
                  if (!pCurve) throw (int) ERROR_SETCURVE;
                  nValues = pCurve->GetNumValues();
                  pAvgCurve1 = pPlots[0].GetCurve(nCurves);
                  if (!pAvgCurve1)  throw (int) ERROR_SETCURVE;
                  pAvgCurve2 = pPlots[1].GetCurve(nCurves);
                  if (!pAvgCurve2)  throw (int) ERROR_SETCURVE;
                  strAVG.LoadString(IDS_AC_AVERAGE);
                  CAudioControlData::SetCurveParams(pAvgCurve1, strAVG, pCurve->GetXUnit(), pCurve->GetYUnit(), nValues, true, pCurve->GetX_Value(0), pCurve->GetX_Value(nValues-1));
                  pAvgCurve1->SetWidth(8);
                  pAvgCurve1->SetColor(RGB(0,0,0));
                  pCurve = pPlots[1].GetCurve(0);
                  if (!pCurve) throw (int) ERROR_SETCURVE;
                  CAudioControlData::SetCurveParams(pAvgCurve2, strAVG, pCurve->GetXUnit(), pCurve->GetYUnit(), nValues, true, pCurve->GetX_Value(0), pCurve->GetX_Value(nValues-1));
                  pAvgCurve2->SetWidth(8);
                  pAvgCurve2->SetColor(RGB(0,0,0));
                  for (j=0; j<nValues; j++)
                  {
                     dSum0 = 0;
                     dSum1 = 0;
                     nAVG0 = 0;
                     nAVG1 = 0;
                     for (i=0; i<nCurves; i++)
                     {
                        dVal = pPlots[0].GetCurve(i)->GetY_Value(j);
                        if (dVal != 0.0)
                        {
                           nAVG0++;
                           dSum0 += dVal;
                        }
                        dVal = pPlots[1].GetCurve(i)->GetY_Value(j);
                        if (dVal != 0.0)
                        {
                           nAVG1++;
                           dSum1 += dVal;
                        }
                     }
                     if (nAVG0 > 1) dSum0 /= (double) nAVG0;
                     pAvgCurve1->SetY_Value(j) = dSum0;
                     if (nAVG1 > 1)dSum1 /= (double) nAVG1;
                     if (nAVG1 == 0)
                     {
                        pAvgCurve2->SetY_Value(j) = CAudioControlData::gm_dNAN;
                        pAvgCurve2->SetInterupted(true);
                     }
                     else pAvgCurve2->SetY_Value(j) = dSum1;
                  }
               }
               if (bShowAverage) nShown++, nCurves++;
               if (nShown)
               {
                  if (nShown != nCurves)
                  {
                     int i, j;
                     CCurve *pc1, *pc2;
                     CPlot *pPlot2 = new CPlot[nPlots];
                     if (!pPlot2[0].SetSize(nShown)) throw (int) ERROR_SETCURVE;
                     if (!pPlot2[1].SetSize(nShown)) throw (int) ERROR_SETCURVE;
                     pPlot2[0].SetHeading(pPlots[0].GetHeading());
                     pPlot2[1].SetHeading(pPlots[1].GetHeading());
                     for (i=0, j=0; i<nShown; i++,j++)
                     {
                        for (; j<nCurves; j++) if (pbShowCurve[j]) break;
                        if (j<nCurves)
                        {
                           pc1 = pPlots[0].GetCurve(j);
                           pc2 = pPlot2[0].GetCurve(i);
                           pc2->RobAllParameter(*pc1);
                           pc1 = pPlots[1].GetCurve(j);
                           pc2 = pPlot2[1].GetCurve(i);
                           pc2->RobAllParameter(*pc1);
                        }
                     }
                     delete[] pPlots;
                     pPlots = pPlot2;
                  }
                  RANGE r = m_pParent->m_BasicParam.m_Range2;
                  pPlots[1].SetDefaultRange(r);
                  r.y1 = m_pParent->m_BasicParam.m_Range1.y1, r.y2 = m_pParent->m_BasicParam.m_Range1.y2;
                  pPlots[0].SetDefaultRange(r);
                  CString strHeadline;
                  m_cShowTestVal.GetLBText(m_nMeaningOfTestVal, strHeadline);
                  if (!pPlots[0].GetHeading()) pPlots[0].SetHeading(strHeadline);
                  if (!pPlots[1].GetHeading()) pPlots[1].SetHeading(strHeadline);
               }
            }
            catch (int nError)
            {
               if ((nError == ERROR_SETCURVE) || (nError == ERROR_SETVALUE))
               {
                  if (pPlots) delete[] pPlots;
                  pPlots = NULL;
               }
            }
            if (pbCalcAverage) delete[] pbCalcAverage;
            if (pbShowCurve)   delete[] pbShowCurve;
         }
      }break;
      case SHOW_SELF_TEST_AUTO_EQ_STEREO:                      // SelfTest Stereo
      case SHOW_SELF_TEST_AUTO_EQ_SURROUND:                    // SelfTest Surround
      {
         CMeasurement *pM;
         int nCurves   = 0;
         int nACData   = 0;
         int nStereo   = m_bShowLeft + m_bShowRight;
         int nSurround = nStereo + m_bShowCenter;
         POSITION pos  = m_pParent->m_Measurements.GetHeadPosition();
         while (pos)
         {
            pM = m_pParent->m_Measurements.GetNext(pos);
            if (pM->m_ACData.bFree3 && IsMeasurementValid(pM))
            {                                                  // soll die SelfTestkurve angezeigt werden
               nACData++;                                      // Datenzähler erhöhen
               if (pM->m_ACData.bLoudspeakermodeSurround) nCurves += nSurround;// bei Surround max. 3 Kurven
               else                                       nCurves += nStereo;  // bei Stereo max. 2 Kurven
            }
         }
         if ((nACData == 0) || (nCurves == 0))
         {
            // Fehler
         }
         else
         {
            try
            {
               bool bShowAverage = (m_bShowAverage && (nACData > 1)) ? true : false;
               int   pnCurves[3];
               if (bShowAverage) nCurves++;
               nPlots = 1;
               pPlots = new CPlot[nPlots];
               if (!pPlots)                     throw (int) ERROR_SETCURVE;
               if (!pPlots[0].SetSize(nCurves)) throw (int) ERROR_SETCURVE;
               pPlots[0].SetXDivision(nDiv);
               nCurves = 0;
               nACData = 0;
               pos = m_pParent->m_Measurements.GetHeadPosition();
               while (pos)
               {
                  pM = m_pParent->m_Measurements.GetNext(pos);
                  if (pM->m_ACData.bFree3 && IsMeasurementValid(pM))
                  {                                                  // soll die SelfTestkurve angezeigt werden
                     if (m_bShowLeft  ) pnCurves[0] = nCurves;
                     else pnCurves[0] = -1;
                     if (m_bShowRight )
                     {
                        if (pnCurves[0]!=-1) pnCurves[1] = pnCurves[0]+1;
                        else                 pnCurves[1] = nCurves;
                     }
                     else pnCurves[1] = -1;
                     if (m_bShowCenter)
                     {
                        if      (pnCurves[1]!=-1) pnCurves[2] = pnCurves[1]+1;
                        else if (pnCurves[0]!=-1) pnCurves[2] = pnCurves[0]+1;
                        else                      pnCurves[2] = nCurves;
                     }
                     else pnCurves[2] = -1;

                     nACData++;
                     if (pM->m_ACData.bLoudspeakermodeSurround) nCurves += nSurround;// bei Surround max. 3 Kurven
                     else                                       nCurves += nStereo;  // bei Stereo max. 2 Kurven
                     int nReturn = pM->m_ACData.GetPlots(&pPlots, nPlots, bAutoRange, pnCurves, nACData);
                     if (nReturn != 0) throw nReturn;
                  }
               }
               if (bShowAverage)
               {
                  int i, j, nValues;
                  double dSum, dX;
                  bool bSingle;
                  CString strAVG;
                  CCurve *pAvgCurve;
                  nCurves = pPlots->GetNumCurves()-1;
                  pCurve = pPlots[0].GetCurve(0);
                  if (!pCurve) throw (int) ERROR_SETCURVE;
                  bSingle = (pCurve->GetFormat()<3) ? false : true;
                  pAvgCurve = pPlots[0].GetCurve(nCurves);
                  if (!pAvgCurve)  throw (int) ERROR_SETCURVE;
                  nValues = pCurve->GetNumValues();
                  strAVG.LoadString(IDS_AC_AVERAGE);
                  CAudioControlData::SetCurveParams(pAvgCurve, strAVG, pCurve->GetXUnit(), pCurve->GetYUnit(), nValues, true, pCurve->GetX_Value(0), pCurve->GetX_Value(nValues-1), bSingle);
                  pAvgCurve->SetWidth(8);
                  pAvgCurve->SetColor(RGB(0,0,0));
                  for (j=0; j<nValues; j++)
                  {
                     dSum = 0;
                     dX  = pCurve->GetX_Value(j);
                     for (i=0; i<nCurves; i++)
                     {
                        dSum += ((CMathCurve*)pPlots->GetCurve(i))->Get_Y_Value(dX);
                     }
                     dSum /= (double) nCurves;
                     pAvgCurve->SetY_Value(j) = dSum;
                     if (!bSingle) pAvgCurve->SetX_Value(j, dX);
                  }
               }
               pPlots[0].SetDefaultRange(m_pParent->m_BasicParam.m_Range1);
               CString strHeadline;
               m_cShowTestVal.GetLBText(m_nMeaningOfTestVal, strHeadline);
               pPlots[0].SetHeading(strHeadline);
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
         }
      }break;
   }
   if (pPlots != NULL)
   {
      if (pPlots[0].GetNumCurves() > 0)
      {
         m_pParent->ShowPlot(nPlots, pPlots, bAutoRange);
      }
      delete[] pPlots;
   }
   else if (CAC_COMApp::gm_dwMessageBoxFlags&MBF_WARN_NO_CURVESEL)
   {
      AfxMessageBox(IDS_ERROR_NO_CURVE, MB_OK|MB_ICONINFORMATION);
   }

}

void CEvaluateMeasurements::OnBeginlabeleditEmMeasurementList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 1;
   if (m_nMeaningOfTestVal == SHOW_DB_VALUES_FROM_DATA_LOGGER)
   {
      LVHITTESTINFO lvHTI;
      ::GetCursorPos(&lvHTI.pt);
      m_cMeasurements.ScreenToClient(&lvHTI.pt);
      m_cMeasurements.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHTI);
      if ((lvHTI.iItem != -1) && (lvHTI.flags & LVHT_ONITEM) && ((lvHTI.iSubItem == 3) || (lvHTI.iSubItem == 4)))
      {
         CEdit* pEdit = m_cMeasurements.GetEditControl();
         if (pEdit)
         {
            if (::GetWindowLong(m_cMeasurements.m_hWnd, GWL_USERDATA)==0)
            {
               m_nItem    = lvHTI.iItem;
               m_nSubItem = lvHTI.iSubItem;
               gm_lOldEditCtrlWndProc = ::SetWindowLong(pEdit->m_hWnd, GWL_WNDPROC, (long)EditCtrlWndProc);
               ::SetWindowLong(m_cMeasurements.m_hWnd, GWL_USERDATA, (long)this);
               CString str = m_cMeasurements.GetItemText(m_nItem, m_nSubItem);
               pEdit->SetWindowText(str);
            	*pResult = 0;
            }
         }
      }
   }
}

LRESULT CALLBACK CEvaluateMeasurements::EditCtrlWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (gm_lOldEditCtrlWndProc == 0) return 0;

   if (uMsg == WM_WINDOWPOSCHANGING)
   {
      WINDOWPOS *pwp = (WINDOWPOS*)lParam;
      HWND hwndP = ::GetParent(hwnd);
      CRect rc;
      CEvaluateMeasurements *pDlg = (CEvaluateMeasurements*) ::GetWindowLong(hwndP, GWL_USERDATA);
      if (pDlg)
      {
         ListView_GetSubItemRect(hwndP, pDlg->m_nItem, pDlg->m_nSubItem, LVIR_BOUNDS, (RECT*)&rc);
         pwp->x = rc.left;
         pwp->y = rc.top;
         pwp->cx= rc.Width();
         pwp->cy= rc.Height();
      }
   }  
   return CallWindowProc((WNDPROC)gm_lOldEditCtrlWndProc, hwnd, uMsg, wParam, lParam);
}

void CEvaluateMeasurements::OnEndlabeleditEmMeasurementList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   
	*pResult = 0;
   CEdit* pEdit = m_cMeasurements.GetEditControl();
   if (pEdit)
   {
      ::SetWindowLong(pEdit->m_hWnd, GWL_WNDPROC, gm_lOldEditCtrlWndProc);
      gm_lOldEditCtrlWndProc = 0;
      if ((pDispInfo->item.mask & LVIF_TEXT) &&
          (pDispInfo->item.pszText != NULL) &&
          (m_nMeaningOfTestVal == SHOW_DB_VALUES_FROM_DATA_LOGGER) && 
          ((m_nSubItem == 3) || (m_nSubItem == 4)))
      {
         CMeasurement *pM = NULL;
         POSITION pos;
         pM = GetMeasurement(pDispInfo->item.iItem, pos);
         if (pM)
         {
            double d1, d2;
            char *pComma = strstr(pDispInfo->item.pszText, gm_cList);
            if (pComma)
            {
               pComma[0] = ' ';
               pComma++;
               for (;(*pComma == ' ') && (*pComma != 0); pComma++);
               d2 = atof(pComma);
               d1 = atof(pDispInfo->item.pszText);
   	         if (((d2 == 0.0) && (*pComma != '0')) ||
                   ((d1 == 0.0) && (pDispInfo->item.pszText[0] != '0')))
                  m_nSubItem = 0;
               if (d1 > d2) CBasicParamDlg::Swap(d1, d2);
            }
            else  m_nSubItem = 0;

            if (m_nSubItem == 3)
            {
               double dMax = pM->m_ACData.GetPeakHoldTime() * 0.1875;
               if (d1 < 0   ) d1 = 0;
               if (d2 > dMax) d2 = dMax;

               pM->m_dT1 = d1;
               pM->m_dT2 = d2;
               m_pParent->SetDataChanged();
            }
            else if (m_nSubItem == 4)
            {
               pM->m_dF1 = d1;
               pM->m_dF2 = d2;
               m_pParent->SetDataChanged();
            }

            if ((pM->m_dF1 != 0.0) && (pM->m_dF2 != 0.0) && (pM->m_dT2 != 0.0))
               pM->m_ACData.bFree1 = true;
            else
               pM->m_ACData.bFree1 = false;

            m_cMeasurements.Update(pDispInfo->item.iItem);
         }
      }
   }
   m_nSubItem = 0;
   ::SetWindowLong(m_cMeasurements.m_hWnd, GWL_USERDATA, (long)0);
}

LRESULT CEvaluateMeasurements::OnHelp(WPARAM wParam, LPARAM lParam)
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

void CEvaluateMeasurements::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NMLVCUSTOMDRAW	*pNmCustDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	*pResult = 0;
   switch(pNmCustDraw->nmcd.dwDrawStage) 
   {
      case CDDS_PREPAINT: 
         *pResult = CDRF_NOTIFYITEMDRAW;
         break;
      case CDDS_ITEMPREPAINT:
      {
         CMeasurement *pM = NULL;
         POSITION pos;
         pM = GetMeasurement(pNmCustDraw->nmcd.dwItemSpec, pos);
         if (!pM) break;
         *pResult = CDRF_NOTIFYSUBITEMDRAW;
         switch (pM->m_ACData.cMeaningOfTestValues)
         {
            case DB_VALUES_FROM_DATA_LOGGER:
            if (pM->m_ACData.bFree1)
            { 
               pNmCustDraw->clrText   = GetSysColor(COLOR_INFOTEXT);
               pNmCustDraw->clrTextBk = GetSysColor(COLOR_INFOBK);
               *pResult |= CDRF_NEWFONT;
            }break;
            case VALUES_LEFT_SPEAKER:
            case VALUES_RIGHT_SPEAKER:
            case VALUES_CENTER_SPEAKER:
               break;
         }
      }break;
      case CDDS_SUBITEM|CDDS_ITEMPREPAINT:
      {
         CMeasurement *pM = NULL;
         POSITION pos;
         pM = GetMeasurement(pNmCustDraw->nmcd.dwItemSpec, pos);
         if (pM)
         {
            switch (pM->m_ACData.cMeaningOfTestValues)
            {
               case DB_VALUES_FROM_DATA_LOGGER:
                  if(pNmCustDraw->iSubItem == 5)
                  {
                     if (m_nMeaningOfTestVal == SHOW_ALL_VALUES)
                        *pResult = DrawSubitemImage(pNmCustDraw, 2);
                     else if (pM->m_ACData.bFree1)
                        *pResult = DrawSubitemImage(pNmCustDraw, pM->m_ACData.bFree4);
                     else
                        *pResult = DrawSubitemImage(pNmCustDraw, 2);
                  }
                  break;
               case VALUES_LEFT_SPEAKER:
               case VALUES_RIGHT_SPEAKER:
               case VALUES_CENTER_SPEAKER:
                  if(pNmCustDraw->iSubItem == 3)
                  {
                     if (m_nMeaningOfTestVal == SHOW_ALL_VALUES)
                        *pResult = DrawSubitemImage(pNmCustDraw, 2);
                     else
                        *pResult = DrawSubitemImage(pNmCustDraw, pM->m_ACData.bFree2);
                  }
                  break;
               case SELF_TEST_AUTO_EQ:
               {
                  switch(pNmCustDraw->iSubItem)
                  {
//                     case 3: *pResult = DrawSubitemImage(pNmCustDraw, pM->m_ACData.bFree2); break;
                     case 4:
                        if (m_nMeaningOfTestVal == SHOW_ALL_VALUES)
                           *pResult = DrawSubitemImage(pNmCustDraw, 2);
                        else
                           *pResult = DrawSubitemImage(pNmCustDraw, pM->m_ACData.bFree1);
                        break;
                     case 5:
                        if (m_nMeaningOfTestVal == SHOW_ALL_VALUES)
                           *pResult = DrawSubitemImage(pNmCustDraw, 2);
                        else
                           *pResult = DrawSubitemImage(pNmCustDraw, pM->m_ACData.bFree4);
                        break;
                  }
               } break;
            }
         }
      }break;
   }
}

LRESULT CEvaluateMeasurements::DrawSubitemImage(NMLVCUSTOMDRAW *pNmCustDraw, int nImage)
{
   CRect rc;
   m_cMeasurements.GetSubItemRect(pNmCustDraw->nmcd.dwItemSpec, pNmCustDraw->iSubItem, LVIR_BOUNDS, rc);
   rc.left += 2;
   int nIndex = (pNmCustDraw->nmcd.uItemState & CDIS_FOCUS) ? COLOR_HIGHLIGHT : COLOR_WINDOW;
   ::FillRect(pNmCustDraw->nmcd.hdc, &rc, GetSysColorBrush(nIndex));
   m_ImageList.Draw(CDC::FromHandle(pNmCustDraw->nmcd.hdc), nImage, rc.TopLeft(), ILD_NORMAL);
   return CDRF_SKIPDEFAULT;
}

void CEvaluateMeasurements::OnTimer(UINT nIDEvent) 
{
   if	(nIDEvent == UPDATE_LIST) m_cMeasurements.InvalidateRect(NULL);
	CDialog::OnTimer(nIDEvent);
}

void CEvaluateMeasurements::OnItemclickEmMeasurementList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	
	*pResult = 0;
}

void CEvaluateMeasurements::OnColumnclickEmMeasurementList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   CMeasurement *pM = NULL;
   POSITION pos;
   pM = GetMeasurement(0, pos);
   if (pM)
   {
      bool bSet = 0;;
      int iItem = -1;
      if(pNMListView->iSubItem == 0) iItem = pNMListView->iSubItem;
      else
      {
         switch (m_nMeaningOfTestVal)
         {
            case SHOW_DB_VALUES_FROM_DATA_LOGGER:
               if(pNMListView->iSubItem == 5) iItem = pNMListView->iSubItem;
               break;
            case SHOW_SELF_TEST_AUTO_EQ_SURROUND: case SHOW_SELF_TEST_AUTO_EQ_STEREO:
               if(pNMListView->iSubItem == 4) iItem = pNMListView->iSubItem;
               if(pNMListView->iSubItem == 5) iItem = pNMListView->iSubItem;
               break;
            case SHOW_VALUES_SPEAKER_TEST:
               if(pNMListView->iSubItem == 3) iItem = pNMListView->iSubItem;
               break;
            default: break;
         }
      }
      if (iItem != -1)
      {
         int i, n = m_cMeasurements.GetItemCount();
         POINT ptScreen;
         CMenu Menu, * pContext = NULL;
         GetCursorPos(&ptScreen);
         Menu.LoadMenu(IDR_MEASUREMENT_CONTEXT);
         pContext = Menu.GetSubMenu(0);
         if (pContext)
         {
            pContext->EnableMenuItem(ID_DELETE_MEASUREMENT, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
            switch (pContext->TrackPopupMenu(TPM_RETURNCMD, ptScreen.x, ptScreen.y, this))
            {
               case ID_ENABLE_CHECKMARKS:  bSet = true; break;
               case ID_DISABLE_CHECKMARKS: bSet = false; break;
               default: n = 0;break;
            }
            if ((n!=0) && ((iItem == 4)||(iItem == 5)))
            {
               int nID = (iItem == 4) ? IDS_TRACK_9_OF_THE_TESTCD : IDS_TRACK_10_OF_THE_TESTCD;
               if (AfxMessageBox(nID, MB_ICONINFORMATION|MB_OKCANCEL|MB_HELP, IDC_BS_CK_TRACK10NEW)!=IDOK)
                  n = 0;
            }
         }
         for (i=0; i<n; i++)
         {
            if (i!=0) pM = GetMeasurement(i, pos);
            if (!pM) break;
            if (iItem == 0)
            {
               pM->m_ACData.bFree3 = bSet;
            }
            else
            {
               switch (pM->m_ACData.cMeaningOfTestValues)
               {
                  case DB_VALUES_FROM_DATA_LOGGER:
                     if(iItem == 5) pM->m_ACData.bFree4 = bSet;
                     break;
                  case VALUES_LEFT_SPEAKER: case VALUES_RIGHT_SPEAKER: case VALUES_CENTER_SPEAKER:
                     if(iItem == 3) pM->m_ACData.bFree2 = bSet;
                     break;
                  case SELF_TEST_AUTO_EQ:
                  {
                     if (bSet)
                     {
                        if(iItem == 4) pM->m_ACData.bFree1 = true, pM->m_ACData.bFree4 = false;
                        if(iItem == 5) pM->m_ACData.bFree4 = true, pM->m_ACData.bFree1 = false;
                     }
                     else
                     {
                        if(iItem == 4) pM->m_ACData.bFree1 = false;
                        if(iItem == 5) pM->m_ACData.bFree4 = false;
                     }
                  } break;
               }
            }
         }
         if (n) m_cMeasurements.InvalidateRect(NULL);
      }
   }
	
	*pResult = 0;
}

void CEvaluateMeasurements::OnEmBtnDeleteMeasurement() 
{
   CMeasurement *pM = NULL;
   POSITION pos     = NULL;
   pM = GetMeasurement(m_nItem, pos);
   if (pos && pM)
   {
      CString str,strFormat,
              strTime = pM->m_Time.Format(IDS_MEASURE_TIME_FMT);
      strFormat.LoadString(ID_DELETE_MEASUREMENT);
      str.Format(strFormat, strTime);
      if (AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION) == IDYES)
      {
         delete pM;
         m_pParent->m_Measurements.RemoveAt(pos);
         SetMeasurementCount();
         m_cMeasurements.InvalidateRect(NULL);
         m_nItem = -1;
         GetDlgItem(IDC_EM_BTN_DELETE_MEASUREMENT)->EnableWindow(false);
      }
   }
}
