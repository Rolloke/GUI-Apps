/********************************************************************/
/* Funktionen der Klasse CBoxCrossOverPage                           */
/********************************************************************/

#include "BoxCrossOver.h"
#include "BoxPropertySheet.h"
#include "resource.h"
#include "AfxRes.h"
#include "Debug.h"
#include "CSortFunctions.h"
#include "CFilterDlg.h"
#include "CEtsDiv.h"
#include "C3DViewDlg.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define LIST_ITEMDATA_EDT_CHANGED 1

const double CBoxCrossOverPage::gm_d2WayFrq     =  2300.0;
const double CBoxCrossOverPage::gm_d2_3WayFrq   =  1900.0;
const double CBoxCrossOverPage::gm_d3WayFrq[2]  = { 450.0, 2300.0 };
const double CBoxCrossOverPage::gm_d3_4WayFrq   =   380.0;
const double CBoxCrossOverPage::gm_d4WayFrq[3]  = { 350.0,  800.0, 2900.0 };
const double CBoxCrossOverPage::gm_d5WayFrq[4]  = { 350.0,  800.0, 2000.0, 4000.0 };

const int CBoxCrossOverPage::gm_nEditFloat[] = 
{
   IDC_XO_EDT_2W_TF,
   IDC_XO_EDT_2_3W_TFTT1, IDC_XO_EDT_2_3W_TFTT2,
   IDC_XO_EDT_3W_TF1, IDC_XO_EDT_3W_TF2, 
   IDC_XO_EDT_3_4W_TF1TT1, IDC_XO_EDT_3_4W_TF2, IDC_XO_EDT_3_4W_TFTT2,
   IDC_XO_EDT_4W_TF1, IDC_XO_EDT_4W_TF2, IDC_XO_EDT_4W_TF3,
   IDC_XO_EDT_5W_TF1, IDC_XO_EDT_5W_TF2, IDC_XO_EDT_5W_TF3, IDC_XO_EDT_5W_TF4
};
const int CBoxCrossOverPage::gm_nGrp[]   = {IDC_XO_GRP_1_W,IDC_XO_GRP_2_W,IDC_XO_GRP_2_3_W,IDC_XO_GRP_3_W,IDC_XO_GRP_3_4_W,IDC_XO_GRP_4_W,IDC_XO_GRP_5_W};
const int CBoxCrossOverPage::gm_nRadio[] = {IDC_XO_R_1_W  ,IDC_XO_R_2_W  ,IDC_XO_R_2_3_W  ,IDC_XO_R_3_W  ,IDC_XO_R_3_4_W  ,IDC_XO_R_4_W  ,IDC_XO_R_5_W};

CBoxCrossOverPage::CBoxCrossOverPage()
{
   ZeroMemory(m_pdFrequencies, sizeof(double)*CUT_OFF_FREQUENCIES);
   m_bSymbolicBtn = true;
}

bool CBoxCrossOverPage::OnInitDialog(HWND hWnd)
{
   if (CEtsPropertyPage::OnInitDialog(hWnd))
   {
      AutoInitBtnBmp(16, 16);
      int i, nCount = sizeof(gm_nRadio) / sizeof(int);
      for (i=0; i<nCount; i++)
      {
         CBoxPropertySheet::SetPaintCtrlWnd(m_hWnd, gm_nGrp[i], gm_nRadio[i]);
      }

      nCount = sizeof(gm_nEditFloat) / sizeof(int);
      for (i=0; i<nCount; i++)
      {
         SetNumeric(gm_nEditFloat[i], true, true);
      }

      for (i=0; i<NO_OF_FILTER_FRQ; i++)
      {
         SendDlgItemMessage(IDC_XO_LST_1W_UG, CB_ADDSTRING, 0, i);
         SendDlgItemMessage(IDC_XO_LST_1W_OG, CB_ADDSTRING, 0, i);
      }
      
      ::EnumChildWindows(hWnd, SetHelpIDs, (LPARAM)this);
      ::CheckDlgButton(m_hWnd, IDC_XO_CK_SYMBOLIC_BTN, m_bSymbolicBtn);
      ::PostMessage(m_hWnd, WM_COMMAND, IDC_XO_CK_SYMBOLIC_BTN, 0);
      return true;
   }
   return false;
}

BOOL CALLBACK CBoxCrossOverPage::SetHelpIDs(HWND hwnd, LPARAM lParam)
{
   CBoxCrossOverPage* pThis = (CBoxCrossOverPage*)lParam;
   int nID = ::GetWindowLong(hwnd, GWL_ID);
   DWORD dwStyle;
   switch (nID)
   {
      case IDC_XO_BTN_CALC: ::SetWindowContextHelpId(hwnd, IDC_CD_BTN_CALC); break;
      case IDC_XO_EDT_5W_TF1: case IDC_XO_EDT_5W_TF2: case IDC_XO_EDT_5W_TF3: case IDC_XO_EDT_5W_TF4:
         ::SetWindowContextHelpId(hwnd, IDC_XO_R_5_W);
         break;
      case IDC_XO_BTN_1W_UG:   case IDC_XO_BTN_1W_BP:    case IDC_XO_BTN_1W_OG:
      case IDC_XO_BTN_5W_TT:
      case IDC_XO_BTN_4W_TT:
      case IDC_XO_BTN_3_4W_TT1:
      case IDC_XO_BTN_3_4W_TT2:
      case IDC_XO_BTN_3W_TT:
      case IDC_XO_BTN_2_3W_TT1:
      case IDC_XO_BTN_2_3W_TT2:
      case IDC_XO_BTN_2W_TT:
         if (pThis->SendDlgItemMessage(nID, BM_GETIMAGE, (WPARAM)IMAGE_ICON, NULL)==NULL)
         {
            pThis->SendDlgItemMessage(nID, BM_SETIMAGE, (WPARAM)IMAGE_ICON, pThis->SendDlgItemMessage(IDC_XO_BTN_1W_UG, BM_GETIMAGE, (WPARAM)IMAGE_ICON, NULL));
         }
      case IDC_XO_BTN_5W_MT1:
      case IDC_XO_BTN_5W_MT2:
      case IDC_XO_BTN_5W_MT3:
      case IDC_XO_BTN_4W_MT1:
      case IDC_XO_BTN_4W_MT2:
      case IDC_XO_BTN_3_4W_MT:
      case IDC_XO_BTN_3W_MT:
         if (pThis->SendDlgItemMessage(nID, BM_GETIMAGE, (WPARAM)IMAGE_ICON, NULL)==NULL)
         {
            pThis->SendDlgItemMessage(nID, BM_SETIMAGE, (WPARAM)IMAGE_ICON, pThis->SendDlgItemMessage(IDC_XO_BTN_1W_BP, BM_GETIMAGE, (WPARAM)IMAGE_ICON, NULL));
         }
      case IDC_XO_BTN_5W_HT:
      case IDC_XO_BTN_4W_HT:
      case IDC_XO_BTN_3_4W_HT:
      case IDC_XO_BTN_3W_HT:
      case IDC_XO_BTN_2_3W_HT:
      case IDC_XO_BTN_2W_HT:
         if (pThis->SendDlgItemMessage(nID, BM_GETIMAGE, (WPARAM)IMAGE_ICON, NULL)==NULL)
         {
            pThis->SendDlgItemMessage(nID, BM_SETIMAGE, (WPARAM)IMAGE_ICON, pThis->SendDlgItemMessage(IDC_XO_BTN_1W_OG, BM_GETIMAGE, (WPARAM)IMAGE_ICON, NULL));
         }
         ::SetWindowContextHelpId(hwnd, IDC_XO_BTN_2W_HT);
         dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
         if (pThis->m_bSymbolicBtn) dwStyle |=  BS_ICON;
         else                       dwStyle &= ~BS_ICON;
         ::SetWindowLong(hwnd, GWL_STYLE, dwStyle);
         break;
      case IDC_XO_EDT_4W_TF1: case IDC_XO_EDT_4W_TF2: case IDC_XO_EDT_4W_TF3:
         ::SetWindowContextHelpId(hwnd, IDC_XO_R_4_W);
         break;
      case IDC_XO_EDT_3_4W_TF1TT1: case IDC_XO_EDT_3_4W_TFTT2: case IDC_XO_EDT_3_4W_TF2:
         ::SetWindowContextHelpId(hwnd, IDC_XO_R_3_4_W);
         break;
      case IDC_XO_EDT_3W_TF1: case IDC_XO_EDT_3W_TF2:
         ::SetWindowContextHelpId(hwnd, IDC_XO_R_3_W);
         break;
      case IDC_XO_EDT_2_3W_TFTT1: case IDC_XO_EDT_2_3W_TFTT2:
         ::SetWindowContextHelpId(hwnd, IDC_XO_R_2_3_W);
         break;
      case IDC_XO_EDT_2W_TF: ::SetWindowContextHelpId(hwnd, IDC_XO_R_2_W); break;
      case IDC_XO_LST_1W_UG: case IDC_XO_LST_1W_OG: case IDC_XO_TXT_1W_UG: case IDC_XO_TXT_1W_OG:
         ::SetWindowContextHelpId(hwnd, IDC_XO_R_1_W);
         break;
      case IDC_XO_GRP_2_W: case IDC_XO_GRP_2_3_W: case IDC_XO_GRP_3_W: case IDC_XO_GRP_3_4_W: case IDC_XO_GRP_4_W: case IDC_XO_GRP_5_W:
         ::SetWindowContextHelpId(hwnd, IDC_XO_GRP_2_W);
         break;
   }
   return 1;
}

int CBoxCrossOverPage::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData         *pB      = &pParent->m_BasicData;
   int nIndex = 0;
   bool bChanged = false;
   switch (nID)
   {
      case IDC_XO_BTN_CALC:
         pParent->OnBtnClckSPLView();
      break;
      case IDC_XO_BTN_RESET_FILTER:
      if (MessageBox(IDS_REQU_RESETFILTER, IDS_PROPSHCAPTION, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2)==IDYES)
      {
         pB->ResetFilterData();
         pParent->m_nFlagOk &= ~(CARABOX_FILTER_EDITED|CARABOX_FILTER2_EDITED|CARABOX_EXT_FRQ_RNG);
         InitFilter();
         EnableWindow(GetDlgItem(IDC_XO_BTN_RESET_FILTER), false);
      } break;
      case IDC_XO_CK_SYMBOLIC_BTN:
      {
         m_bSymbolicBtn = !m_bSymbolicBtn;
         ::EnumChildWindows(m_hWnd, SetHelpIDs, (LPARAM)this);
         ::CheckDlgButton(m_hWnd, nID, m_bSymbolicBtn);
         ::InvalidateRect(m_hWnd, NULL, false);
      } break;
      case IDC_XO_CK_INIT_STD_VALUES:
      if (nNotifyCode == BN_CLICKED)
      {
         bool bCheck;
         if (pParent->m_nFlagOk & CARABOX_INIT_STD_FRQ)        // gesetzt
         {
            pParent->m_nFlagOk &= ~CARABOX_INIT_STD_FRQ;       // löschen
            bCheck = false;
         }
         else
         {
            int i, nCount = sizeof(gm_nEditFloat) / sizeof(int);
            for (i=0; i<nCount; i++)
            {
               SetDlgItemText(gm_nEditFloat[i], "");
            }
            pParent->m_nFlagOk |= CARABOX_INIT_STD_FRQ;
            InitStandardFrequencies();
            SetControlValues();
            bChanged = true;
            bCheck = true;
         }
         ::CheckDlgButton(m_hWnd, nID, bCheck);
      }break;
      case IDC_XO_R_1_W: case IDC_XO_R_2_W:case IDC_XO_R_2_3_W:case IDC_XO_R_3_W: case IDC_XO_R_3_4_W: case IDC_XO_R_4_W: case IDC_XO_R_5_W:
      if (nNotifyCode == BN_CLICKED)
      {
         int nOld = pB->nXoverTyp;
         switch(GetDlgRadioBtn(IDC_XO_R_1_W))
         {
            case 0: pB->nXoverTyp = CARABOX_CO_1W;    break; // NW-Typ7: normale 1-Wege-Box, 1-2 Grenzfrequenzen
            case 1: pB->nXoverTyp = CARABOX_CO_2W;    break; // NW-Typ1: normale 2-Wege-Box, 1 Überg.Freq
            case 2: pB->nXoverTyp = CARABOX_CO_2_3W;  break; // NW-Typ2: 2(3)-Wege-Box, 1 Überg.Freq + 2.TT früher abgekoppelt
            case 3: pB->nXoverTyp = CARABOX_CO_3W;    break; // NW-Typ3: normale 3-Wege-Box, 2 Überg.Freq
            case 4: pB->nXoverTyp = CARABOX_CO_3_4W;  break; // NW-Typ4: 3(4)-Wege-Box, 2 Überg.Freq + 2.TT früher abgekoppelt
            case 5: pB->nXoverTyp = CARABOX_CO_4W;    break; // NW-Typ5: normale 4-Wege-Box, 3 Überg.Freq
            case 6: pB->nXoverTyp = CARABOX_CO_5W;   break; // NW-Typ6: normale 5-Wege-Box, 4 Überg.Freq
         }
         if (nOld != pB->nXoverTyp)
         {
            if (pParent->m_nFlagOk & CARABOX_INIT_STD_FRQ)
            {
               int i, nSize = 0, nIdStart = 0;
               switch(nOld)
               {
                  case CARABOX_CO_5W:   nSize = 4; nIdStart = IDC_XO_EDT_5W_TF1;      break;
                  case CARABOX_CO_4W:   nSize = 3; nIdStart = IDC_XO_EDT_4W_TF1;      break;
                  case CARABOX_CO_3_4W: nSize = 3; nIdStart = IDC_XO_EDT_3_4W_TF1TT1; break;
                  case CARABOX_CO_3W:   nSize = 2; nIdStart = IDC_XO_EDT_3W_TF1;      break;
                  case CARABOX_CO_2_3W: nSize = 2; nIdStart = IDC_XO_EDT_2_3W_TFTT1;  break;
                  case CARABOX_CO_2W:   nSize = 1; nIdStart = IDC_XO_EDT_2W_TF;       break;
               }
               for (i=0; i<nSize; i++)
                  SetDlgItemText(nIdStart+i, "");

               InitStandardFrequencies();
               SetControlValues();
            }
            else
            {
               GetControlFrequencies();
               InitFilter();
               CheckFilterButtons();
            }

            CheckCrossOverType();
            ::InvalidateRect(m_hWnd, NULL, false);
            ::UpdateWindow(m_hWnd);

            pParent->SetChanged(CARABOX_CROSS_OVER|CARABOX_CHASSIS_NO);
            pParent->m_nFlagOk &= ~(CARABOX_CROSS_OVER|CARABOX_CHASSIS_NO);
            bChanged = true;
         }
      } break;
      case IDC_XO_EDT_2_3W_TFTT2:            // 2-3W Tieftöner 2
      case IDC_XO_EDT_3_4W_TFTT2:            // 3-4W Tieftöner 2
      if (nNotifyCode == EN_KILLFOCUS)
      {
         double dFmax = pB->dHighLimitFreq;
         if (pB->sTT1.dTPFreq != 0.0) dFmax = pB->sTT1.dTPFreq;
         if (GetDlgItemDouble(nID, pB->sTT2.dTPFreq, 0, &bChanged) && bChanged)
         {
            CheckMinMaxDouble(nID, pB->dLowLimitFreq, dFmax, 2, pB->sTT2.dTPFreq);;
            pParent->m_nFlagOk &= ~CARABOX_INIT_STD_FRQ;       // löschen
            ::CheckDlgButton(m_hWnd, IDC_XO_CK_INIT_STD_VALUES, false);
            InitTT2Filter();
         }
         else if (bChanged)
         {
            pB->sTT2.dTPFreq = 0;
            InitTT2Filter();
         }
         CheckFilterButtons();
      } break;
      case IDC_XO_EDT_2W_TF:                 //   2W-Trennfrequenz
      case IDC_XO_EDT_2_3W_TFTT1:            // 2-3W-Trennfrequenz
      if (nNotifyCode == EN_KILLFOCUS)
      {
         double dFmin = pB->dLowLimitFreq;
         if ((nID == IDC_XO_EDT_2_3W_TFTT1) && (pB->sTT2.dTPFreq != 0.0)) dFmin = pB->sTT2.dTPFreq;
         if (GetDlgItemDouble(nID, m_pdFrequencies[0], 0, &bChanged) && bChanged)
         {
            CheckMinMaxDouble(nID, dFmin, pB->dHighLimitFreq, 2, m_pdFrequencies[0]);
            pParent->m_nFlagOk &= ~CARABOX_INIT_STD_FRQ;       // löschen
            ::CheckDlgButton(m_hWnd, IDC_XO_CK_INIT_STD_VALUES, false);
            InitFilter(1);
            if (nID == IDC_XO_EDT_2_3W_TFTT1) InitTT2Filter();
         }
         else if (bChanged)
         {
            m_pdFrequencies[0] = 0;
            InitFilter(1);
         }
         CheckFilterButtons();
      } break;
      case IDC_XO_EDT_3W_TF2:                //   3W Trenfrequenz 2
      case IDC_XO_EDT_3_4W_TF2:    nIndex++; // 3-4W Trenfrequenz 2
      case IDC_XO_EDT_3W_TF1:                //   3W Trenfrequenz 1
      case IDC_XO_EDT_3_4W_TF1TT1:           // 3-4W Trenfrequenz 1
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetCutOffFrequencies(nID, nIndex, 2))
         {
            InitFilter(1<<nIndex);
            if (nID == IDC_XO_EDT_3_4W_TF1TT1) InitTT2Filter();
            bChanged = true;
         }
         CheckFilterButtons();
      } break;
      case IDC_XO_EDT_4W_TF3: nIndex++;      //   4W Trenfrequenz 3
      case IDC_XO_EDT_4W_TF2: nIndex++;      //   4W Trenfrequenz 2
      case IDC_XO_EDT_4W_TF1:                //   4W Trenfrequenz 1
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetCutOffFrequencies(nID, nIndex, 3))
         {
            InitFilter(1<<nIndex);
            bChanged = true;
         }
         CheckFilterButtons();
      } break;
      case IDC_XO_EDT_5W_TF4: nIndex++;      //   5W Trenfrequenz 4
      case IDC_XO_EDT_5W_TF3: nIndex++;      //   5W Trenfrequenz 3
      case IDC_XO_EDT_5W_TF2: nIndex++;      //   5W Trenfrequenz 2
      case IDC_XO_EDT_5W_TF1:                //   5W Trenfrequenz 1
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetCutOffFrequencies(nID, nIndex, 4))
         {
            InitFilter(1<<nIndex);
            bChanged = true;
         }
         CheckFilterButtons();
      } break;
      case IDC_XO_BTN_1W_UG: case IDC_XO_BTN_1W_OG: case IDC_XO_BTN_1W_BP:
      {
         double *pdF  = GetPtr1WGFrequ(nID);
         double dTemp = 0;
         FilterBox *pFB = pParent->GetOneWayFilter();
         if (pdF) dTemp = *pdF, *pdF = 0;
         CallFilterDialog(*pFB);
         if (pdF) *pdF = dTemp;
      } break;
      case IDC_XO_BTN_2W_TT:    case IDC_XO_BTN_2_3W_TT1: case IDC_XO_BTN_3W_TT:
      case IDC_XO_BTN_3_4W_TT1: case IDC_XO_BTN_4W_TT:    case IDC_XO_BTN_5W_TT:
      {
         CallFilterDialog(pB->sTT1);
      } break;
      case IDC_XO_BTN_2_3W_TT2:case IDC_XO_BTN_3_4W_TT2:
      {
         if (CallFilterDialog(pB->sTT2))
            pParent->m_nFlagOk |= CARABOX_FILTER2_EDITED;
      } break;
      case IDC_XO_BTN_2W_HT:   case IDC_XO_BTN_2_3W_HT: case IDC_XO_BTN_3W_HT:
      case IDC_XO_BTN_3_4W_HT: case IDC_XO_BTN_4W_HT:   case IDC_XO_BTN_5W_HT:
      {
         CallFilterDialog(pB->sHT);
      } break;
      case IDC_XO_BTN_3W_MT: case IDC_XO_BTN_3_4W_MT: case IDC_XO_BTN_4W_MT1: case IDC_XO_BTN_5W_MT1:
      {
         CallFilterDialog(pB->sMT1);
      } break;
      case IDC_XO_BTN_4W_MT2: case IDC_XO_BTN_5W_MT2:
      {
         CallFilterDialog(pB->sMT2);
      } break;
      case IDC_XO_BTN_5W_MT3:
      {
         CallFilterDialog(pB->sMT3);
      } break;
      case IDC_XO_LST_1W_UG: case IDC_XO_LST_1W_OG:
      {
         float *pFrequArray = NULL;
         bool   bUG         = true;
         long  *pnSel       = NULL;
         long  *pnCount     = NULL;
         if (nID == IDC_XO_LST_1W_UG)
         {
            pFrequArray = pB->fLowerHPFrequ;
            pnSel       = &pB->nSelLowerHPFrequ;
            pnCount     = &pB->nNoOfLowerHPFrequ;
         }
         else
         {
            bUG         = false;
            pFrequArray = pB->fUpperTPFrequ;
            pnSel       = &pB->nSelUpperTPFrequ;
            pnCount     = &pB->nNoOfUpperTPFrequ;
         }
         ASSERT((*pnSel >= 0) && (*pnSel < NO_OF_FILTER_FRQ));
         switch (nNotifyCode)
         { 
            case CBN_DROPDOWN:                                 // wenn ein neuer Item gewählt wird
            case CBN_KILLFOCUS:                                // wenn das Editfenster den Focus verliert
            case CBN_SELCHANGE:                                // Wenn ein Item selektiert wird
            if (SendDlgItemMessage(nID, CB_GETITEMDATA, LIST_ITEMDATA_EDT_CHANGED, 0)) // Editierfenster geändert ?
            {
               if (*pnSel != CB_ERR)                           // Index Ok ?
               {
                  double dValue;
                  float  fValue = 0.0f;                        // Bei Dropdown keine Fehlermeldung, da Absturz
                  if (nNotifyCode == CBN_DROPDOWN) m_nFlags |= NOERRORWINDOW;
 
                  bool bOk = GetDlgItemDouble(nID, dValue, 0);// Wert Holen
                  if (bOk && (dValue != 0))                    // Gültigen Wert prüfen
                  {                                            // Wertebereich
                     bOk = CheckMinMaxDouble(nID, pB->dLowLimitFreq, pB->dHighLimitFreq, 1, dValue);
                     fValue = (float)dValue;
                  }
                  
                  pFrequArray[*pnSel] = fValue;                // Einfügen
                  pB->SortFilterArrays(bUG);                   // Sortieren

                  if (CSortFunctions::gm_bChanged && CSortFunctions::gm_bEqual)
                  {                                            // gleiche Werte sind quatsch
                     if (bOk) CreateErrorWnd(nID, IDS_EQUAL_VALUES, true);
                     for (int i=0; i<NO_OF_FILTER_FRQ; i++)
                     {
                        if (pFrequArray[i] == (float) dValue)  // suchen
                        {
                           pFrequArray[i] = 0;                 // entfernen
                           break;
                        }
                     }
                     pB->SortFilterArrays(bUG);                // wieder sortieren
                  }
                  pB->CountFilterFrequencies();                // gültige Werte zählen
                  FilterBox *pFB = pParent->GetOneWayFilter();
                  if (*pnCount == 0)                           // keine Werte
                  {
                     if (bUG) pFB->nHPOrder = 0;               // Filter aus
                     else     pFB->nTPOrder = 0;
                  }
                  else                                         // mehr als 1 Wert
                  {
                     if (bUG)                                  // Filter initialisieren
                     {
                        if (pFB->nHPOrder==0)
                        {
                           pFB->nHPOrder = 3;
                           pFB->nHPType  = 3;
                        }
                     }
                     else
                     {
                        if (pFB->nTPOrder==0)
                        {
                           pFB->nTPOrder = 3;
                           pFB->nTPType  = 3;
                        }
                     }
                  }

                  if ((pB->nNoOfLowerHPFrequ > 1) || (pB->nNoOfUpperTPFrequ > 1))
                     pParent->m_nLSType |= ACTIVE_LS;          // Aktive Lautsprecher mit mehreren Frequenzen
                  else
                     pParent->m_nLSType &= ~ACTIVE_LS;

                                                               // Index nach dem Editieren einstellen
                  if (bUG) *pnSel = 0;                         // Hochpass immer auf den ersten
                  else if (*pnCount > 0)                       // Tiefpass immer auf den letzten
                  {
                     *pnSel = *pnCount-1;
                  }
                  else *pnSel = 0;
                  SendDlgItemMessage(nID, CB_SETCURSEL, *pnSel, 0);
                  *GetPtr1WGFrequ(nID) = pFrequArray[*pnSel];
                  SendDlgItemMessage(nID, CB_SETITEMDATA, LIST_ITEMDATA_EDT_CHANGED, 0);
                  m_nFlags &= ~NOERRORWINDOW;
                  nNotifyCode = CBN_SELCHANGE;
               }
               bChanged = true;
            }
            if (nNotifyCode == CBN_SELCHANGE)
            {
               int nSel = SendDlgItemMessage(nID, CB_GETCURSEL, 0,0);
               if (nSel != CB_ERR) *pnSel = nSel;
               SetDlgItemDouble(nID, pFrequArray[*pnSel], 1); // Editfenstertext setzen
               if (pFrequArray[*pnSel] > 0.0)
                  *GetPtr1WGFrequ(nID) = pFrequArray[*pnSel];

               if (*pnSel < *pnCount) SendDlgItemMessage(nID, CB_SETITEMDATA, LIST_ITEMDATA_EDT_CHANGED, 0);
               else                   SendDlgItemMessage(nID, CB_SETITEMDATA, LIST_ITEMDATA_EDT_CHANGED, 1);
               
               if (nID == IDC_XO_LST_1W_UG) SetDlgItemInt(IDC_XO_TXT_1W_UG, pB->nSelLowerHPFrequ+1, false);
               else                         SetDlgItemInt(IDC_XO_TXT_1W_OG, pB->nSelUpperTPFrequ+1, false);
            }break;
            case CBN_EDITCHANGE:                               // Änderung merken
               SendDlgItemMessage(nID, CB_SETITEMDATA, LIST_ITEMDATA_EDT_CHANGED, 1);
               break;
         }
      }
      default: break;
   }
   if (bChanged)
   {
      CheckWizardButtons();
      EnableWindow(GetDlgItem(IDC_XO_BTN_CALC), IsWindowEnabled(pParent->GetWndFinishBtn()));
      pParent->SetChanged(CARABOX_CROSS_OVER);
   }
   return CEtsPropertyPage::OnCommand(nID , nNotifyCode, hwndControl);
}

bool CBoxCrossOverPage::GetCutOffFrequencies(int nID, int nIndex, int nCount)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData         *pB      = &pParent->m_BasicData;
   int i;
   ASSERT(nCount <= CUT_OFF_FREQUENCIES);
   ASSERT((nIndex >= 0) && (nIndex < nCount));

   double dFmin = pB->dLowLimitFreq;
   if ((nID == IDC_XO_EDT_3_4W_TF1TT1) && (pB->sTT2.dTPFreq != 0.0)) dFmin = pB->sTT2.dTPFreq;
   bool bChanged = false;
   if (GetDlgItemDouble(nID, m_pdFrequencies[nIndex], 0, &bChanged) && bChanged)
   { 
      if (!CheckMinMaxDouble(nID, dFmin, pB->dHighLimitFreq, 2, m_pdFrequencies[nIndex]))
         bChanged = true;

      if (!bChanged) return true;
      pParent->m_nFlagOk &= ~CARABOX_INIT_STD_FRQ;       // löschen
      ::CheckDlgButton(m_hWnd, IDC_XO_CK_INIT_STD_VALUES, false);
      for (i=0; i<nCount; i++)
      {
         if (m_pdFrequencies[i] == 0.0) return false;
      }
      pParent->SetChanged(CARABOX_CROSS_OVER);
      pParent->m_nFlagOk &= ~CARABOX_CROSS_OVER;
      CSortFunctions::SetSortDirection(true);
      qsort(m_pdFrequencies, nCount, sizeof(double), CSortFunctions::SortDouble);
      if (CSortFunctions::gm_bEqual)
      {
         CEtsDialog::CreateErrorWnd(nID, IDS_EQUAL_VALUES);
         return false;
      }
      else if (CSortFunctions::gm_bChanged)
      {
         nID -= nIndex;
         for (i=0; i<nCount; i++)
         {
            SetDlgItemDouble(nID + i, m_pdFrequencies[i], 1);
         }
      }
      return true;
   }
   else if (bChanged)
   {
      m_pdFrequencies[nIndex] = 0;
      return true;
   }
   return false;
}

int CBoxCrossOverPage::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch (nMsg)
   {
      case WM_DRAWITEM   : return OnDrawItem((DRAWITEMSTRUCT *)lParam);
      case WM_MEASUREITEM: return OnMeasureItem((MEASUREITEMSTRUCT*)lParam);
   }
   return CEtsPropertyPage::OnMessage(nMsg, wParam, lParam);
}

int  CBoxCrossOverPage::OnHelp(PSHNOTIFY*pN)
{
   ::WinHelp(m_hWnd, "CARACAD.hlp", HELP_CONTEXT, 1993);
   return CEtsPropertyPage::OnHelp(pN);
}

int  CBoxCrossOverPage::OnReset(PSHNOTIFY* pN)
{
   ZeroMemory(m_pdFrequencies, sizeof(double)*CUT_OFF_FREQUENCIES);
   int i, nCount = sizeof(gm_nEditFloat) / sizeof(int);
   for (i=0; i<nCount; i++)
   {
      SetDlgItemText(gm_nEditFloat[i], "");
   }
   return CEtsPropertyPage::OnReset(pN);
}

int  CBoxCrossOverPage::OnQueryCancel(PSHNOTIFY*pN, bool bCancel)
{
   DeleteAutoBtnBmp();
   return CEtsPropertyPage::OnQueryCancel(pN, bCancel);
}

void CBoxCrossOverPage::InitStandardFrequencies()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData *pB     = &pParent->m_BasicData;
   double *pdArray  = NULL;
   int     i, nSize = 0;
   switch(pB->nXoverTyp)
   {
      case CARABOX_CO_5W:
         pdArray = (double*)gm_d5WayFrq;
         nSize   = sizeof(gm_d5WayFrq) / sizeof(double);
         break;
      case CARABOX_CO_4W:
         pdArray = (double*)gm_d4WayFrq;
         nSize   = sizeof(gm_d4WayFrq) / sizeof(double);
         break;
      case CARABOX_CO_3_4W:
         pB->sTT2.dTPFreq = gm_d3_4WayFrq;
      case CARABOX_CO_3W:
         pdArray = (double*)gm_d3WayFrq;
         nSize   = sizeof(gm_d3WayFrq) / sizeof(double);
         break;
      case CARABOX_CO_2_3W:
         pB->sTT2.dTPFreq = gm_d2_3WayFrq;
      case CARABOX_CO_2W:
         pdArray = (double*)&gm_d2WayFrq;
         nSize   = 1;
         break;
      case CARABOX_CO_1W: break;
      default: ASSERT(false);
   }

   for (i=0; i<nSize; i++)
   {
      m_pdFrequencies[i] = pdArray[i];
   }
}

void CBoxCrossOverPage::GetControlFrequencies()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData *pB     = &pParent->m_BasicData;
   int     i, nSize = 0, nIdStart=0;
   switch(pB->nXoverTyp)
   {
      case CARABOX_CO_5W:
         nSize    = 4;
         nIdStart = IDC_XO_EDT_5W_TF1;
         break;
      case CARABOX_CO_4W:
         nSize    = 3;
         nIdStart = IDC_XO_EDT_4W_TF1;
         break;
      case CARABOX_CO_3_4W:
         if (!GetDlgItemDouble(IDC_XO_EDT_3_4W_TFTT2, pB->sTT2.dTPFreq, 0)) pB->sTT2.dTPFreq = 0;
         nSize    = 2;
         nIdStart = IDC_XO_EDT_3_4W_TF1TT1;
         InitTT2Filter();
         break;
      case CARABOX_CO_3W:
         nSize    = 2;
         nIdStart = IDC_XO_EDT_3W_TF1;
         break;
      case CARABOX_CO_2_3W:
         if (!GetDlgItemDouble(IDC_XO_EDT_2_3W_TFTT2, pB->sTT2.dTPFreq, 0)) pB->sTT2.dTPFreq = 0;
         nSize    = 1;
         nIdStart = IDC_XO_EDT_2_3W_TFTT1;
         InitTT2Filter();
         break;
      case CARABOX_CO_2W:
         nSize    = 1;
         nIdStart = IDC_XO_EDT_2W_TF;
         break;
      case CARABOX_CO_1W: break;
      default: ASSERT(false);
   }

   for (i=0; i<nSize; i++)
   {
      if (!GetDlgItemDouble(i+nIdStart, m_pdFrequencies[i], 0)) m_pdFrequencies[i] = 0;
   }
   InitFilter();
}

int  CBoxCrossOverPage::OnSetActive(PSHNOTIFY* pN, bool bAccept)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData *pB     = &pParent->m_BasicData;
   int nCheck = -1;
   bool b_R_1_W, b_R_2_W, b_R_3_W, b_R_2_3_W, b_R_3_4_W, b_R_4_W, b_R_5_W;
   if (!(pParent->m_nFlagOk & CARABOX_CROSS_OVER) && (pParent->m_nFlagOk & CARABOX_INIT_STD_FRQ))
   {
      InitStandardFrequencies();
   }
   else if (pParent->m_nFlagOk & CARABOX_CROSS_OVER)
   {
      switch(pB->nXoverTyp)
      {
         case CARABOX_CO_5W:
            m_pdFrequencies[3] = pB->sMT3.dTPFreq;
         case CARABOX_CO_4W:
            m_pdFrequencies[2] = pB->sMT2.dTPFreq;
         case CARABOX_CO_3W:
         case CARABOX_CO_3_4W: 
            m_pdFrequencies[1] = pB->sMT1.dTPFreq;
         case CARABOX_CO_2W:
         case CARABOX_CO_2_3W:
            m_pdFrequencies[0] = pB->sTT1.dTPFreq;
            break;
         case CARABOX_CO_1W: break;
         default: ASSERT(false);
      }
   }

   switch(pB->nXoverTyp)
   {
      case CARABOX_CO_1W:   nCheck = 0; break;
      case CARABOX_CO_2W:   nCheck = 1; break;
      case CARABOX_CO_2_3W: nCheck = 2; break;
      case CARABOX_CO_3W:   nCheck = 3; break;
      case CARABOX_CO_3_4W: nCheck = 4; break;
      case CARABOX_CO_4W:   nCheck = 5; break;
      case CARABOX_CO_5W:   nCheck = 6; break;
   }

   b_R_1_W = b_R_2_W = b_R_3_W = b_R_2_3_W = b_R_3_4_W = b_R_4_W = b_R_5_W = false;

   if (pParent->m_nLSType & DIPOLE_FLAT)                       // reiner Dipol
   {
      b_R_1_W = b_R_2_W = b_R_3_W = true;
   }
   else if (LOWORD(pParent->m_nLSType) != STAND_ALONE)         // Einwege (sub, add on tweeter)
   {
      b_R_1_W = true;
   }
   else if (pParent->m_nLSType & DIPOLE_FLAT_H)                // Hybrid Dipol
   {
      b_R_2_W = b_R_3_W = b_R_4_W = true;
   }
   else
   {
      b_R_1_W = b_R_2_W = b_R_3_W = b_R_2_3_W = b_R_3_4_W = b_R_4_W = b_R_5_W = true;
   }

   EnableWindow(GetDlgItem(IDC_XO_R_1_W)  , b_R_1_W);
   EnableWindow(GetDlgItem(IDC_XO_R_2_W)  , b_R_2_W);
   EnableWindow(GetDlgItem(IDC_XO_R_3_W)  , b_R_3_W);
   EnableWindow(GetDlgItem(IDC_XO_R_2_3_W), b_R_2_3_W);
   EnableWindow(GetDlgItem(IDC_XO_R_3_4_W), b_R_3_4_W);
   EnableWindow(GetDlgItem(IDC_XO_R_4_W)  , b_R_4_W);
   EnableWindow(GetDlgItem(IDC_XO_R_5_W)  , b_R_5_W);

   ::CheckDlgButton(m_hWnd, IDC_XO_CK_INIT_STD_VALUES, (pParent->m_nFlagOk & CARABOX_INIT_STD_FRQ)? true : false);

   EnableWindow(GetDlgItem(IDC_XO_BTN_RESET_FILTER), (pParent->m_nFlagOk & (CARABOX_FILTER_EDITED|CARABOX_FILTER2_EDITED))? true : false);

   CheckCrossOverType();

   CheckDlgRadioBtn(IDC_XO_R_1_W, nCheck);

   SetControlValues();

   CheckWizardButtons();
   EnableWindow(GetDlgItem(IDC_XO_BTN_CALC), IsWindowEnabled(pParent->GetWndFinishBtn()));

   return CEtsPropertyPage::OnSetActive(pN, bAccept);
}

int  CBoxCrossOverPage::OnWizardBack(PSHNOTIFY* pN, bool bAccept)
{
   m_nFlags |= NOERRORWINDOW;
   OnWizardNext(pN, bAccept);
   m_nFlags &= ~NOERRORWINDOW;
   int nResult = 1;
   if (::GetFocus() == ::GetDlgItem(pN->hdr.hwndFrom, pN->hdr.idFrom)) nResult = 0;
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, nResult);
   return 1;
}

int  CBoxCrossOverPage::OnWizardFinish(PSHNOTIFY*pN, bool bFinish)
{
   bFinish = false;
   OnWizardNext(pN, true);
   if (pN->lParam)
   {
      CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
      bFinish = pParent->OnWizardFinish();
   }
   return CEtsPropertyPage::OnWizardFinish(pN, bFinish);
}

int  CBoxCrossOverPage::OnWizardNext(PSHNOTIFY*pN, bool bAccept)
{
   REPORT("CBoxCrossOverPage::OnWizardNext");
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData * pB = &pParent->m_BasicData;
   bAccept = false;

   if (pN) SetPageButtonFocus(pN);
   
   if (pB->nXoverTyp == 0) CreateErrorWnd(IDC_XO_R_2_W, IDS_ERROR_WRONGINPUT, true);
   else 
   {
      int nID = 0, i, nCount=-1;
      switch (pB->nXoverTyp)
      {
         case CARABOX_CO_2W:
/*
            if (pB->sTT1.dTPFreq==0.0) CreateErrorWnd(IDC_XO_EDT_2W_TF, IDS_ERROR_WRONGINPUT, true);
            else bAccept = true;
*/
            if (CheckFrequency(IDC_XO_EDT_2W_TF, pB->sTT1.dTPFreq)) bAccept = true;
            break;
         case CARABOX_CO_2_3W:
/*
            if      (pB->sTT2.dTPFreq==0.0) CreateErrorWnd(IDC_XO_EDT_2_3W_TFTT2, IDS_ERROR_WRONGINPUT, true);
            else if (pB->sTT1.dTPFreq==0.0) CreateErrorWnd(IDC_XO_EDT_2_3W_TFTT1, IDS_ERROR_WRONGINPUT, true);
            else bAccept = true;
*/
            if (CheckFrequency(IDC_XO_EDT_2_3W_TFTT1, pB->sTT1.dTPFreq) && 
                CheckFrequency(IDC_XO_EDT_2_3W_TFTT2, pB->sTT2.dTPFreq)) bAccept = true;
            break;
         case CARABOX_CO_3W:   nID = IDC_XO_EDT_3W_TF1;      nCount = 2; break;
         case CARABOX_CO_3_4W: 
/*
            if (pB->sTT2.dTPFreq == 0.0) CreateErrorWnd(IDC_XO_EDT_3_4W_TFTT2, IDS_ERROR_WRONGINPUT, true);
            else               {nID = IDC_XO_EDT_3_4W_TF1TT1; nCount = 2;}
*/
            if (CheckFrequency(IDC_XO_EDT_3_4W_TFTT2, pB->sTT2.dTPFreq))
               {nID = IDC_XO_EDT_3_4W_TF1TT1; nCount = 2;}
            break;
         case CARABOX_CO_4W:   nID = IDC_XO_EDT_4W_TF1;      nCount = 3; break;
         case CARABOX_CO_5W:   nID = IDC_XO_EDT_5W_TF1;      nCount = 4; break;
         case CARABOX_CO_1W:    // 1-2 Grenzfrequenzen
         {
            nCount = -2;
            switch (LOWORD(pParent->m_nLSType))
            {
               case ADD_ON1:                                // Subwoofer
               {
                  if (CheckFrequency(IDC_XO_LST_1W_OG, pB->sTT1.dTPFreq)) nCount = 0;
               } break;
               case ADD_ON3:                                // Hochtöner
               {
                  if (CheckFrequency(IDC_XO_LST_1W_UG, pB->sHT.dHPFreq)) nCount = 0;
               } break;
/*
               case ADD_ON2:  break;                        // Mitteltöner
               {
                 if (CheckFrequency(IDC_XO_LST_1W_OG, pB->sTT1.dTPFreq) &&
                     CheckFrequency(IDC_XO_LST_1W_UG, pB->sTT1.dHPFreq)) nCount = 0;
               } break;
*/
               case STAND_ALONE:                            // STAND_ALONE
                  nCount = 0;                               //  Breitband = TT
                  break;
               default: break;
            }
         }break;
         default: nCount = -2;
      }
      for (i=0; i<nCount; i++)
      {
         if (!CheckFrequency(nID+i, m_pdFrequencies[i])) break;
      }
//      if      (nCount == -1) bAccept = true; 
      if (i == nCount)  bAccept = true;

      if (bAccept) pParent->m_nFlagOk |=  CARABOX_CROSS_OVER;
      else         pParent->m_nFlagOk &= ~CARABOX_CROSS_OVER;
   }

   if (pN)
   {
      if (!HasPageButtonFocus(pN)) bAccept = false;
      pN->lParam = bAccept;
      return CEtsPropertyPage::OnWizardNext(pN, bAccept);
   }
   return 0;
}
bool CBoxCrossOverPage::CheckFrequency(int nID, double dF)
{
   if (dF == 0.0)
   {
      CreateErrorWnd(nID, IDS_ERROR_WRONGINPUT, true);
      return false;
   }
   else if (dF < ((CBoxPropertySheet*)m_pParent)->m_BasicData.dLowLimitFreq)
   {
      CreateErrorWnd(nID, IDS_ERROR_LOW_FREQUENCY, true);
      return false;
   }
   else if (dF > ((CBoxPropertySheet*)m_pParent)->m_BasicData.dHighLimitFreq)
   {
      CreateErrorWnd(nID, IDS_ERROR_HIGH_FREQUENCY, true);
      return false;
   }
   return true;
}

void CBoxCrossOverPage::CheckCrossOverType()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData *pB     = &pParent->m_BasicData;
   long       nFlags = BS_AUTORADIOBUTTON;
   
   bool bOneWay = (pB->nXoverTyp == CARABOX_CO_1W) ? true : false;
   if (bOneWay)
   {
      if (LOWORD(pParent->m_nLSType) == STAND_ALONE) bOneWay = false;
   }

   EnableGroupItems(GetDlgItem(IDC_XO_GRP_1_W)  , bOneWay, nFlags);
   EnableGroupItems(GetDlgItem(IDC_XO_GRP_2_W)  , (pB->nXoverTyp == CARABOX_CO_2W  ) ? true : false, nFlags);
   EnableGroupItems(GetDlgItem(IDC_XO_GRP_2_3_W), (pB->nXoverTyp == CARABOX_CO_2_3W) ? true : false, nFlags);
   EnableGroupItems(GetDlgItem(IDC_XO_GRP_3_W)  , (pB->nXoverTyp == CARABOX_CO_3W  ) ? true : false, nFlags);
   EnableGroupItems(GetDlgItem(IDC_XO_GRP_3_4_W), (pB->nXoverTyp == CARABOX_CO_3_4W) ? true : false, nFlags);
   EnableGroupItems(GetDlgItem(IDC_XO_GRP_4_W)  , (pB->nXoverTyp == CARABOX_CO_4W  ) ? true : false, nFlags);
   EnableGroupItems(GetDlgItem(IDC_XO_GRP_5_W)  , (pB->nXoverTyp == CARABOX_CO_5W  ) ? true : false, nFlags);
}

void CBoxCrossOverPage::InitTT2Filter()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   if (pParent->m_nFlagOk & CARABOX_FILTER2_EDITED) return;
   BasicData *pB     = &pParent->m_BasicData;

   if((pB->sTT2.dTPFreq != 0.0) && (pB->sTT1.dTPFreq != 0.0))
   {
      if ((pB->sTT2.dTPFreq/pB->sTT1.dTPFreq) > 0.65 )
      {
         pB->sTT2.nTPOrder = 3;  // TP-Filter 3. Ordnung angenommen
         pB->sTT2.nTPType  = 3;  // Tschebyscheff 0.5 dB Welligkeit angenommen
      }
      else
      {
         pB->sTT2.nTPOrder = 2;         // TP-Filter 2. Ordnung angenommen
         pB->sTT2.nTPType  = 0;         // Butterworth angenommen
      }
   }
}

bool CBoxCrossOverPage::CallFilterDialog(FilterBox &Filter)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData         *pB      = &pParent->m_BasicData;
   CAutoDisable ad(pParent->m_p3DViewDlg->GetWindowHandle());
   CFilterDlg dlg(m_hInstance, IDD_FILTER_PROPERTIES, m_hWnd);
   dlg.m_dMinFrq = pB->dLowLimitFreq;
   dlg.m_dMaxFrq = pB->dHighLimitFreq;
   CopyMemory(&dlg.m_Filter, &Filter, sizeof(FilterBox)-sizeof(Filter.cFilterBox));
   if (dlg.DoModal() == IDOK)
   {
      CopyMemory(&Filter, &dlg.m_Filter, sizeof(FilterBox)-sizeof(Filter.cFilterBox));
      pParent->SetChanged(CARABOX_CROSS_OVER);
      pParent->m_nFlagOk &= ~CARABOX_CROSS_OVER;
      pParent->m_nFlagOk |= CARABOX_FILTER_EDITED;
      if (dlg.m_bFrqChanged) pParent->m_nFlagOk |= CARABOX_EXT_FRQ_RNG;
      EnableWindow(GetDlgItem(IDC_XO_BTN_RESET_FILTER), true);
      return true;
   }
   return false;
}

void CBoxCrossOverPage::CheckFilterButtons()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData         *pB      = &pParent->m_BasicData;
   switch(pB->nXoverTyp)
   {
      case CARABOX_CO_1W:     // NW-Typ7: normale 1-Wege-Box, 1-2 Grenzfrequenzen
      {
         FilterBox *pFB = NULL;
         switch (LOWORD(pParent->m_nLSType))
         {
            case ADD_ON1: pFB = &pB->sTT1; break;              // Subwoofer, obere Grenzfrequenz
            case ADD_ON3: pFB = &pB->sHT;  break;              // Hochtöner, untere Grenzfrequenz
      //    case ADD_ON2:                  break;              // Mitteltöner, untere oder obere Grenzfrequenz
            default:      pFB = &pB->sTT1; break;              // Stand Alone : Breitband = TT1
         }
         EnableWindow(GetDlgItem(IDC_XO_BTN_1W_OG),  (pFB->dHPFreq!=0.0) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_1W_BP), ((pFB->dTPFreq!=0.0)&&(pFB->dHPFreq!=0.0)) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_1W_UG) , (pFB->dTPFreq!=0.0) ? true : false);
      } break;
      case CARABOX_CO_2W:     // NW-Typ1: normale 2-Wege-Box, 1 Überg.Freq
         EnableWindow(GetDlgItem(IDC_XO_BTN_2W_TT)   , (pB->sTT1.dTPFreq!=0.0) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_2W_HT)   , (pB->sHT.dHPFreq !=0.0) ? true : false);
         break;
      case CARABOX_CO_2_3W:   // NW-Typ2: 2(3)-Wege-Box, 1 Überg.Freq + 2.TT früher abgekoppelt
         EnableWindow(GetDlgItem(IDC_XO_BTN_2_3W_TT1), (pB->sTT1.dTPFreq!=0.0) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_2_3W_TT2), (pB->sTT2.dTPFreq!=0.0) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_2_3W_HT) , (pB->sHT.dHPFreq !=0.0) ? true : false);
         break;
      case CARABOX_CO_3W:     // NW-Typ3: normale 3-Wege-Box, 2 Überg.Freq
         EnableWindow(GetDlgItem(IDC_XO_BTN_3W_TT)   , (pB->sTT1.dTPFreq !=0.0) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_3W_MT)   , ((pB->sMT1.dTPFreq!=0.0)&&(pB->sMT1.dHPFreq!=0.0)) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_3W_HT)   , (pB->sHT.dHPFreq  !=0.0) ? true : false);
         break;
      case CARABOX_CO_3_4W:  // NW-Typ4: 3(4)-Wege-Box, 2 Überg.Freq + 2.TT früher abgekoppelt
         EnableWindow(GetDlgItem(IDC_XO_BTN_3_4W_TT1), (pB->sTT1.dTPFreq !=0.0) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_3_4W_TT2), (pB->sTT2.dTPFreq !=0.0) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_3_4W_MT) , ((pB->sMT1.dTPFreq!=0.0)&&(pB->sMT1.dHPFreq!=0.0)) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_3_4W_HT) , (pB->sHT.dHPFreq  !=0.0) ? true : false);
         break;
      case CARABOX_CO_4W:     // NW-Typ5: normale 4-Wege-Box, 3 Überg.Freq
         EnableWindow(GetDlgItem(IDC_XO_BTN_4W_TT)   , (pB->sTT1.dTPFreq !=0.0) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_4W_MT1)  , ((pB->sMT1.dTPFreq!=0.0)&&(pB->sMT1.dHPFreq!=0.0)) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_4W_MT2)  , ((pB->sMT2.dTPFreq!=0.0)&&(pB->sMT2.dHPFreq!=0.0)) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_4W_HT)   , (pB->sHT.dHPFreq  !=0.0) ? true : false);
         break;
      case CARABOX_CO_5W:   // NW-Typ6: normale 5-Wege-Box, 4 Überg.Freq
         EnableWindow(GetDlgItem(IDC_XO_BTN_5W_TT)   , (pB->sTT1.dTPFreq !=0.0) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_5W_MT1)  , ((pB->sMT1.dTPFreq!=0.0)&&(pB->sMT1.dHPFreq!=0.0)) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_5W_MT2)  , ((pB->sMT2.dTPFreq!=0.0)&&(pB->sMT2.dHPFreq!=0.0)) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_5W_MT3)  , ((pB->sMT3.dTPFreq!=0.0)&&(pB->sMT3.dHPFreq!=0.0)) ? true : false);
         EnableWindow(GetDlgItem(IDC_XO_BTN_5W_HT)   , (pB->sHT.dHPFreq  !=0.0) ? true : false);
         break;
   }
}

void CBoxCrossOverPage::InitFilter(DWORD dwInit)
{
   BasicData         *pB      = &((CBoxPropertySheet*)m_pParent)->m_BasicData;
   switch(pB->nXoverTyp)
   {
      case CARABOX_CO_1W:     // NW-Typ7: normale 1-Wege-Box, 1-2 Grenzfrequenzen
         pB->sTT1.dHPFreq = pB->sTT1.dTPFreq = 0.0;            // löschen der aller Filter
         pB->sMT1.dHPFreq = pB->sMT1.dTPFreq = 0.0;
         pB->sMT2.dHPFreq = pB->sMT2.dTPFreq = 0.0;
         pB->sMT3.dHPFreq = pB->sMT3.dTPFreq = 0.0;
         pB->sHT.dHPFreq  = pB->sHT.dTPFreq  = 0.0;
         break;
      case CARABOX_CO_2W:     // NW-Typ1: normale 2-Wege-Box, 1 Überg.Freq
      case CARABOX_CO_2_3W:   // NW-Typ2: 2(3)-Wege-Box, 1 Überg.Freq + 2.TT früher abgekoppelt
         if (dwInit&0x01) pB->sHT.dHPFreq  = pB->sTT1.dTPFreq = m_pdFrequencies[0];

         pB->sTT1.dHPFreq = 0.0;                               // löschen der anderen Filter
         pB->sMT1.dHPFreq = pB->sMT1.dTPFreq = 0.0;
         pB->sMT2.dHPFreq = pB->sMT2.dTPFreq = 0.0;
         pB->sMT3.dHPFreq = pB->sMT3.dTPFreq = 0.0;
         pB->sHT.dTPFreq  = 0.0;
         break;
      case CARABOX_CO_3W:     // NW-Typ3: normale 3-Wege-Box, 2 Überg.Freq
      case CARABOX_CO_3_4W:   // NW-Typ4: 3(4)-Wege-Box, 2 Überg.Freq + 2.TT früher abgekoppelt
         if (dwInit&0x01) pB->sTT1.dTPFreq = pB->sMT1.dHPFreq = m_pdFrequencies[0];
         if (dwInit&0x02) pB->sMT1.dTPFreq = pB->sHT.dHPFreq  = m_pdFrequencies[1];

         pB->sTT1.dHPFreq = 0.0;                               // löschen der anderen Filter
         pB->sMT2.dHPFreq = pB->sMT2.dTPFreq = 0.0;
         pB->sMT3.dHPFreq = pB->sMT3.dTPFreq = 0.0;
         pB->sHT.dTPFreq  = 0.0;
         break;
      case CARABOX_CO_4W:     // NW-Typ5: normale 4-Wege-Box, 3 Überg.Freq
         if (dwInit&0x01) pB->sTT1.dTPFreq = pB->sMT1.dHPFreq = m_pdFrequencies[0];
         if (dwInit&0x02) pB->sMT1.dTPFreq = pB->sMT2.dHPFreq = m_pdFrequencies[1];
         if (dwInit&0x04) pB->sMT2.dTPFreq = pB->sHT.dHPFreq  = m_pdFrequencies[2];
         pB->sTT1.dHPFreq = 0.0;                               // löschen der anderen Filter
         pB->sMT3.dHPFreq = pB->sMT3.dTPFreq = 0.0;
         pB->sHT.dTPFreq  = 0.0;
         break;
      case CARABOX_CO_5W:     // NW-Typ6: normale 5-Wege-Box, 4 Überg.Freq
         if (dwInit&0x01) pB->sTT1.dTPFreq = pB->sMT1.dHPFreq = m_pdFrequencies[0];
         if (dwInit&0x02) pB->sMT1.dTPFreq = pB->sMT2.dHPFreq = m_pdFrequencies[1];
         if (dwInit&0x04) pB->sMT2.dTPFreq = pB->sMT3.dHPFreq = m_pdFrequencies[2];
         if (dwInit&0x08) pB->sMT3.dTPFreq = pB->sHT.dHPFreq  = m_pdFrequencies[3];
         pB->sTT1.dHPFreq = 0.0;                               // löschen der anderen Filter
         pB->sHT.dTPFreq  = 0.0;
         break;
   }
}

LRESULT CBoxCrossOverPage::OnDrawItem(DRAWITEMSTRUCT *pDI)
{
   if (pDI->CtlType == ODT_COMBOBOX)
   {
      BasicData         *pB      = &((CBoxPropertySheet*)m_pParent)->m_BasicData;
      float *pFrequArray = NULL;
      if      (pDI->CtlID == IDC_XO_LST_1W_UG) pFrequArray = pB->fLowerHPFrequ;
      else if (pDI->CtlID == IDC_XO_LST_1W_OG) pFrequArray = pB->fUpperTPFrequ;
      if (pFrequArray)
      {
         char szText[32]  = "----";
         char szIndex[16] = "";
         int  nLen         = 4;
         long lLastSelItem = ::GetWindowLong(pDI->hwndItem, GWL_USERDATA);
         RECT rcItem = pDI->rcItem;
         ::InflateRect(&rcItem, -1, -1);
         if (pFrequArray[pDI->itemID] > MIN_ACC_FLOAT)
         {
            ETSDIV_NUTOTXT ntx = {(double)pFrequArray[pDI->itemID], ETSDIV_NM_STD, 1, 32, 0};
            CEtsDiv::NumberToText(&ntx, szText);
            wsprintf(szIndex, "%2d:", pDI->itemID+1);
            nLen = strlen(szText);
         }
         ::SaveDC(pDI->hDC);
         if (pDI->itemAction & ODA_SELECT)
         {
            ::SetBkColor(pDI->hDC, ::GetSysColor(COLOR_HIGHLIGHT));
            ::SetTextColor(pDI->hDC, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
            ::FillRect(pDI->hDC, &rcItem, ::GetSysColorBrush(COLOR_HIGHLIGHT));
            ::SetWindowLong(pDI->hwndItem, GWL_USERDATA, pDI->itemID);
         }
         else
         {
            ::SetBkColor(pDI->hDC, ::GetSysColor(COLOR_WINDOW));
            ::SetTextColor(pDI->hDC, ::GetSysColor(COLOR_WINDOWTEXT));
            ::FillRect(pDI->hDC, &rcItem, ::GetSysColorBrush(COLOR_WINDOW));
         }
         if (pDI->itemAction & ODA_DRAWENTIRE)
         {
            POINT pt;
            ::MoveToEx(pDI->hDC, pDI->rcItem.left, pDI->rcItem.bottom, &pt);
            ::LineTo(pDI->hDC  ,  pDI->rcItem.right, pDI->rcItem.bottom);
            ::SetWindowLong(pDI->hwndItem, GWL_USERDATA, -1);
         }
         if (szIndex[0] != 0)
         {
            RECT rcIndex = rcItem;
//            int nWidth = MulDiv(rcItem.right-rcItem.left, 25, 100);
//            rcIndex.right -= nWidth;
//            rcItem.left   += nWidth;
            ::DrawText(pDI->hDC, szIndex, strlen(szIndex), &rcIndex, DT_LEFT|DT_TOP|DT_SINGLELINE);
            ::DrawText(pDI->hDC, szText, nLen, &rcItem, DT_RIGHT|DT_TOP|DT_SINGLELINE);
         }
         else ::DrawText(pDI->hDC, szText, nLen, &rcItem, DT_RIGHT|DT_TOP|DT_SINGLELINE);

         ::RestoreDC(pDI->hDC, -1);

         if ((pDI->itemAction & ODA_SELECT) && (lLastSelItem != -1))
         {
            pDI->itemAction &= ~ODA_SELECT;
            pDI->itemID      = lLastSelItem;
            OnDrawItem(pDI);
            ::SetWindowLong(pDI->hwndItem, GWL_USERDATA, -1);
         }
         return true;
      }
   }
   return 0;
}

LRESULT CBoxCrossOverPage::OnMeasureItem(MEASUREITEMSTRUCT * /*pMI*/)
{
   return 0;
}

double * CBoxCrossOverPage::GetPtr1WGFrequ(int nID)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
//   BasicData         *pB      = &pParent->m_BasicData;
   FilterBox *pFB = pParent->GetOneWayFilter();
   switch (nID)
   {
      case IDC_XO_LST_1W_UG: case IDC_XO_BTN_1W_OG: return &pFB->dHPFreq;
      case IDC_XO_LST_1W_OG: case IDC_XO_BTN_1W_UG: return &pFB->dTPFreq;
      case IDC_XO_BTN_1W_BP: break;
      default:
      ASSERT(false);
   }
   return NULL;
}

void CBoxCrossOverPage::CheckWizardButtons()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   m_nFlags |= NOERRORWINDOW;
   OnWizardNext(NULL, false);
   m_nFlags &= ~NOERRORWINDOW;
   pParent->SetWizardButtons(true, (pParent->m_nFlagOk & CARABOX_CROSS_OVER)? true : false);
}

void CBoxCrossOverPage::SetControlValues()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData *pB     = &pParent->m_BasicData;
   int        nID=0, i, nCount;

   switch (pB->nXoverTyp)
   {
      case CARABOX_CO_2W:   nID = IDC_XO_EDT_2W_TF;       nCount = 1; break;
      case CARABOX_CO_2_3W:
         if (pB->sTT2.dTPFreq != 0.0) SetDlgItemDouble(IDC_XO_EDT_2_3W_TFTT2, pB->sTT2.dTPFreq, 1);
         else                         SetDlgItemText(  IDC_XO_EDT_2_3W_TFTT2, "");
//         OnCommand(IDC_XO_EDT_2_3W_TFTT2, EN_KILLFOCUS, GetDlgItem(IDC_XO_EDT_2_3W_TFTT2));
         nID = IDC_XO_EDT_2_3W_TFTT1;  nCount = 1; 
         break;
      case CARABOX_CO_3W:   nID = IDC_XO_EDT_3W_TF1;      nCount = 2; break;
      case CARABOX_CO_3_4W: 
         if (pB->sTT2.dTPFreq != 0.0) SetDlgItemDouble(IDC_XO_EDT_3_4W_TFTT2, pB->sTT2.dTPFreq, 1);
         else                         SetDlgItemText(  IDC_XO_EDT_3_4W_TFTT2, "");
//         OnCommand(IDC_XO_EDT_3_4W_TFTT2, EN_KILLFOCUS, GetDlgItem(IDC_XO_EDT_3_4W_TFTT2));
         nID = IDC_XO_EDT_3_4W_TF1TT1; nCount = 2; 
         break;
      case CARABOX_CO_4W:   nID = IDC_XO_EDT_4W_TF1;      nCount = 3; break;
      case CARABOX_CO_5W:   nID = IDC_XO_EDT_5W_TF1;      nCount = 4; break;
      case CARABOX_CO_1W:
      {
         double dUpper = 0, dLower = 0;
         InitFilter();                                         // löscht alle Filter
         if ((LOWORD(pParent->m_nLSType) == STAND_ALONE) && !(pParent->m_nLSType&DIPOLE_FLAT))
         {
            pB->sTT1.dTPFreq     = pB->dHighLimitFreq;         // Obere GF belegen
            dUpper               = pB->dHighLimitFreq;
            pB->fUpperTPFrequ[0] = (float) pB->dHighLimitFreq;
            ::EnableWindow(GetDlgItem(IDC_XO_BTN_1W_OG), true);
         }
         else if (LOWORD(pParent->m_nLSType) == ADD_ON1)       // Subwoofer
         {                                                     // Obere GF belegen
            if (pB->nNoOfUpperTPFrequ == 0) pB->fUpperTPFrequ[0] = (float)pB->dHighLimitFreq;
               
            dUpper = pB->sTT1.dTPFreq = pB->fUpperTPFrequ[pB->nSelUpperTPFrequ];
            dLower = pB->sTT1.dHPFreq = pB->fLowerHPFrequ[pB->nSelLowerHPFrequ];
         }
         else if (LOWORD(pParent->m_nLSType) == ADD_ON3)       // Zusatz Hochtöner
         {                                                     // Untere GF belegen
            if (pB->nNoOfLowerHPFrequ == 0) pB->fLowerHPFrequ[0] = (float)pB->dLowLimitFreq;

            dLower = pB->sHT.dHPFreq = pB->fLowerHPFrequ[pB->nSelLowerHPFrequ];
            ::EnableWindow(GetDlgItem(IDC_XO_LST_1W_OG), false);
            ::EnableWindow(GetDlgItem(IDC_XO_BTN_1W_OG), false);
            ::EnableWindow(GetDlgItem(IDC_XO_BTN_1W_BP), false);
         }
         
         if (dLower != 0.0)
         {
            SetDlgItemInt(IDC_XO_TXT_1W_UG, pB->nSelLowerHPFrequ+1, false);
            SetDlgItemDouble(IDC_XO_LST_1W_UG, dLower, 1);
         }
         else
         {
            SetDlgItemText(IDC_XO_LST_1W_UG, "");
            SetDlgItemText(IDC_XO_TXT_1W_UG, "");
         }
         SendDlgItemMessage(IDC_XO_LST_1W_UG, CB_SETITEMDATA, LIST_ITEMDATA_EDT_CHANGED, 0);

         if (dUpper != 0.0)
         {
            SetDlgItemInt(IDC_XO_TXT_1W_OG, pB->nSelUpperTPFrequ+1, false);
            SetDlgItemDouble(IDC_XO_LST_1W_OG, dUpper, 1);
         }
         else
         {
            SetDlgItemText(IDC_XO_LST_1W_OG, "");
            SetDlgItemText(IDC_XO_TXT_1W_OG, "");
         }
         SendDlgItemMessage(IDC_XO_LST_1W_OG, CB_SETITEMDATA, LIST_ITEMDATA_EDT_CHANGED, 0);
         nCount = 0;
      } break;
      default:             nCount = 0; break;
   }
   
   ASSERT(nCount <= CUT_OFF_FREQUENCIES);
   if (nCount>CUT_OFF_FREQUENCIES) nCount = CUT_OFF_FREQUENCIES;
   for (i=0; i<nCount; i++)
   {
      if (m_pdFrequencies[i] != 0.0)
      {
         SetDlgItemDouble(nID+i, m_pdFrequencies[i], 1);
      }
      else SetDlgItemText(nID+i, "");
   }

   for (; i<CUT_OFF_FREQUENCIES; i++)
   {
      m_pdFrequencies[i] = 0;
   }

   if (pB->nXoverTyp != CARABOX_CO_1W)
   {
      if (!(pParent->m_nFlagOk & CARABOX_EXT_FRQ_RNG))
         InitFilter();                                            // Initialisieren der Filter
      CheckFilterButtons();
   }
}
