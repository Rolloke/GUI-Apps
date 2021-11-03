/********************************************************************/
/* Funktionen der Klasse CBoxBaseDataPage                           */
/********************************************************************/
#include <stdlib.h>

#include "BoxBaseData.h"
#include "resource.h"
#include "BoxPropertySheet.h"
#include "AfxRes.h"
#include "Debug.h"
#include "CSortFunctions.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define MIN_ADD_ON_HT    1000.0
#define MAX_ADD_ON_SUBW  1000.0

CBoxBaseDataPage::CBoxBaseDataPage()
{
}

bool CBoxBaseDataPage::OnInitDialog(HWND hWnd)
{
   if (CEtsPropertyPage::OnInitDialog(hWnd))
   {
      AutoInitBtnBmp(60, 30);
      ::EnumChildWindows(hWnd, SetHelpIDs, (LPARAM)hWnd);
      SetNumeric(IDC_BB_EDT_SENSITIVITY, true, true);
      SetNumeric(IDC_BB_EDT_RANGEFROM, true, true);
      SetNumeric(IDC_BB_EDT_RANGETO, true, true);
      SetNumeric(IDC_BB_EDT_CLOSEDMAX, true, true);
      SetNumeric(IDC_BB_EDT_BR_TLMAX1, true, true);
      SetNumeric(IDC_BB_EDT_BR_TLMAX2, true, true);
      SetNumeric(IDC_BB_EDT_BR_TLMIN, true, true);
      return true;
   }
   return false;
}

BOOL CALLBACK CBoxBaseDataPage::SetHelpIDs(HWND hwnd, LPARAM /*lParam*/)
{
   switch (::GetWindowLong(hwnd, GWL_ID))
   {
      case IDC_BB_R_TYPE0: case IDC_BB_R_TYPE1: case IDC_BB_R_TYPE2: case IDC_BB_R_TYPE3:
         ::SetWindowContextHelpId(hwnd, IDC_BB_TXT_TYPE);
         break;
      case IDC_BB_TXT_IMPEDANCE:   case IDC_BB_EDT_IMPEDANCE:   case IDC_BB_TXT_IMPEDANCEUNIT:
      case IDC_BB_TXT_SENSITIVITY: case IDC_BB_EDT_SENSITIVITY: case IDC_BB_TXT_SENSITIVITYUNIT:
      case IDC_BB_TXT_RANGE:       case IDC_BB_EDT_RANGEFROM:   case IDC_BB_TXT_RANGE1: 
      case IDC_BB_EDT_RANGETO:     case IDC_BB_TXT_RANGEUNIT:
         ::SetWindowContextHelpId(hwnd, IDC_BB_TXT_BASE);
         break;
      case IDC_BB_TXT_CLOSED:   case IDC_BB_TXT_CLOSEDMAX:   case IDC_BB_EDT_CLOSEDMAX: case IDC_BB_TXT_CLOSEDUNIT:
      case IDC_BB_TXT_BR_TL: case IDC_BB_TXT_BR_TLMAX1: case IDC_BB_EDT_BR_TLMAX1: case IDC_BB_TXT_BR_TLUNIT2:
      case IDC_BB_TXT_BR_TLMAX2: case IDC_BB_EDT_BR_TLMAX2: case IDC_BB_TXT_BR_TLUNIT: case IDC_BB_TXT_BR_TLMIN: case IDC_BB_EDT_BR_TLMIN:
         ::SetWindowContextHelpId(hwnd, IDC_BB_TXT_IMP_BASS);
         break;
      case IDC_BB_R_FR0:   case IDC_BB_R_FR1:   case IDC_BB_R_FR2:
      case IDC_BB_TXT_FR0: case IDC_BB_TXT_FR1: case IDC_BB_TXT_FR2:
         ::SetWindowContextHelpId(hwnd, IDC_BB_TXT_FR);
         break;
   }
   return 1;
}

int CBoxBaseDataPage::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData         *pB      = &pParent->m_BasicData;
   bool bChanged = false;
   switch (nID)
   {
      case IDC_BB_R_TYPE0: case IDC_BB_R_TYPE1: case IDC_BB_R_TYPE2: case IDC_BB_R_TYPE3:
      if (nNotifyCode == BN_CLICKED)
      {
         int nOld = pB->nBoxTyp;
         switch (GetDlgRadioBtn(IDC_BB_R_TYPE0))
         {
            case 0: pB->nBoxTyp = CARABOX_BOXTYP_BR;   break;  // Typ = Vented Box
            case 1: pB->nBoxTyp = CARABOX_BOXTYP_CLOS; break;  // Typ = Closed Box
            case 2: pB->nBoxTyp = CARABOX_BOXTYP_TL;   break;  // Typ = TL-Box
            case 3: pB->nBoxTyp = CARABOX_BOXTYP_PM;   break;  // Typ = PM-Box
            default:pB->nBoxTyp = 0;                   break;  // undefiniert
         }
         if (pB->nBoxTyp != nOld)
         {
            bChanged = true;
            pParent->SetChanged(CARABOX_CHASSIS_LIST);
            pParent->m_nFlagOk      &= ~(CARABOX_CHASSIS_DATA|CARABOX_CHASSIS_POS);
            CheckBoxType();
            InitDlgCtrls();
            SetBassImpedance();
         }
      } break;
      case IDC_BB_CK_CALC_IMPEDANCE_VALUES:
      if (nNotifyCode == BN_CLICKED)
      {
         bool bCheck;
         if (pParent->m_nFlagOk & CARABOX_IMPED_EDITED)        // gesetzt
         {
            pParent->m_nFlagOk &= ~CARABOX_IMPED_EDITED;       // löschen
            bCheck = true;
            bChanged = true;
            SetBassImpedance();
         }
         else
         {
            pParent->m_nFlagOk |= CARABOX_IMPED_EDITED;
            bCheck = false;
         }
         ::CheckDlgButton(m_hWnd, nID, bCheck);
      } break;
      case IDC_BB_EDT_IMPEDANCE:                               // Impedanz
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetDlgItemInt(nID, pB->nNennImped, false, 0, &bChanged) && bChanged)
            CheckMinMaxInt(nID, 1, 100, false, pB->nNennImped);
      } break;
      case IDC_BB_EDT_SENSITIVITY:                              // Empfindlichkeit
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetDlgItemDouble(nID, pB->dEffectivDBWm, 0, &bChanged) && bChanged)
         {
            CheckMinMaxDouble(nID, 60.0, 110.0, 2, pB->dEffectivDBWm);
         }
         else if (bChanged) pB->dEffectivDBWm = 0;
      } break;
      case IDC_BB_EDT_RANGEFROM:                               // Übertragungsbereich von
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetDlgItemDouble(nID, pB->dLowLimitFreq, 0, &bChanged) && bChanged)
         {
            double dMin = 5;
            if (LOWORD(pParent->m_nLSType) == ADD_ON3) dMin = MIN_ADD_ON_HT;
            CheckMinMaxDouble(nID, dMin, 100000.0, 2, pB->dLowLimitFreq);
            if ((pB->dHighLimitFreq != 0.0) && (pB->dLowLimitFreq > pB->dHighLimitFreq))
            {
               swap(pB->dLowLimitFreq, pB->dHighLimitFreq);
               SetDlgItemDouble(IDC_BB_EDT_RANGEFROM, pB->dLowLimitFreq, 1);
               SetDlgItemDouble(IDC_BB_EDT_RANGETO  , pB->dHighLimitFreq, 1);
            }
            pB->CheckFilterArrayRange();
            SetBassImpedance();
         }
         else if (bChanged) pB->dLowLimitFreq = 0;
      } break;
      case IDC_BB_EDT_RANGETO:                                 // Übertragungsbereich bis
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetDlgItemDouble(nID, pB->dHighLimitFreq, 0, &bChanged) && bChanged)
         {
            double dMax = 100000.0;
            if (LOWORD(pParent->m_nLSType) == ADD_ON1) dMax = MAX_ADD_ON_SUBW;
            CheckMinMaxDouble(nID, 5.0, dMax, 2, pB->dHighLimitFreq);
            if ((pB->dLowLimitFreq != 0.0) && (pB->dLowLimitFreq > pB->dHighLimitFreq))
            {
               swap(pB->dLowLimitFreq, pB->dHighLimitFreq);
               SetDlgItemDouble(IDC_BB_EDT_RANGEFROM, pB->dLowLimitFreq, 1);
               SetDlgItemDouble(IDC_BB_EDT_RANGETO  , pB->dHighLimitFreq, 1);
               SetBassImpedance();
            }
            pB->CheckFilterArrayRange();
         }
         else if (bChanged) pB->dHighLimitFreq = 0;
      } break;
      case IDC_BB_EDT_CLOSEDMAX:                               // Geschlossene Box Maximum
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetDlgItemDouble(nID, pB->dMainResFreq, 0, &bChanged) && bChanged)
         {
            CheckMinMaxDouble(nID, 0.01, 1000.0, 3, pB->dMainResFreq);
             pParent->m_nFlagOk |= CARABOX_IMPED_EDITED;
            ::CheckDlgButton(m_hWnd, IDC_BB_CK_CALC_IMPEDANCE_VALUES, false);
        }
         else if (bChanged) pB->dMainResFreq = 0;
      } break;
      case IDC_BB_EDT_BR_TLMAX1:                               // BR/TL 1. Maximum
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetDlgItemDouble(nID, pB->dImpedLowFreqBR, 0, &bChanged) && bChanged)
         {
            CheckMinMaxDouble(nID, 0.01, 1000.0, 3, pB->dImpedLowFreqBR);
            pParent->m_nFlagOk |= CARABOX_IMPED_EDITED;
            ::CheckDlgButton(m_hWnd, IDC_BB_CK_CALC_IMPEDANCE_VALUES, false);
            CheckBRTLMinMax(nID);
         }
         else if (bChanged) pB->dImpedLowFreqBR = 0;
      } break;
      case IDC_BB_EDT_BR_TLMIN:                                // BR/TL Minimum
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetDlgItemDouble(nID, pB->dImpedMidFreqBR, 0, &bChanged) && bChanged)
         {
            CheckMinMaxDouble(nID, 0.01, 1000.0, 3, pB->dImpedMidFreqBR);
            pParent->m_nFlagOk |= CARABOX_IMPED_EDITED;
            ::CheckDlgButton(m_hWnd, IDC_BB_CK_CALC_IMPEDANCE_VALUES, false);
            CheckBRTLMinMax(nID);
         }
         else if (bChanged) pB->dImpedMidFreqBR = 0;
      } break;
      case IDC_BB_EDT_BR_TLMAX2:                               // BR/TL 2. Maximum
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetDlgItemDouble(nID, pB->dImpedHighFreqBR, 0, &bChanged) && bChanged)
         {
            CheckMinMaxDouble(nID, 0.01, 1000.0, 3, pB->dImpedHighFreqBR);
            pParent->m_nFlagOk |= CARABOX_IMPED_EDITED;
            ::CheckDlgButton(m_hWnd, IDC_BB_CK_CALC_IMPEDANCE_VALUES, false);
            CheckBRTLMinMax(nID);
         }
         else if (bChanged) pB->dImpedHighFreqBR = 0;
      } break;
      case IDC_BB_R_FR0:case IDC_BB_R_FR1:case IDC_BB_R_FR2:
      if ((nNotifyCode == BN_CLICKED) && (pB->nBoxTyp == CARABOX_BOXTYP_CLOS))
      {
         int nOld = pB->nTypeTTResp;
         switch (GetDlgRadioBtn(IDC_BB_R_FR0))
         {
            case 0: pB->nTypeTTResp = CARABOX_TTRESPONSETYPMINU; break;// TT-Frequenzgangtyp für tiefe Frequenzen: überdämpft (Q=0.5)
            case 1: pB->nTypeTTResp = CARABOX_TTRESPONSETYPNORM; break;// TT-Frequenzgangtyp für tiefe Frequenzen: normal (Q=0.9..1)
            case 2: pB->nTypeTTResp = CARABOX_TTRESPONSETYPPLUS; break;// TT-Frequenzgangtyp für tiefe Frequenzen: unterdämpft (Q=2)
            default: break;
         }
         if (nOld != pB->nTypeTTResp)
         {
            bChanged = true;
            SetBassImpedance();
         }
      } break;
      default: break;
   }
   if (bChanged)
   {
      CheckWizardButtons();
      pParent->SetChanged(CARABOX_BASE_DATA);
   }
   return CEtsPropertyPage::OnCommand(nID , nNotifyCode, hwndControl);
}
 
int  CBoxBaseDataPage::OnHelp(PSHNOTIFY*pN)
{
   ::WinHelp(m_hWnd, "CARACAD.hlp", HELP_CONTEXT, 1992);
   return CEtsPropertyPage::OnHelp(pN);
}

int  CBoxBaseDataPage::OnQueryCancel(PSHNOTIFY*pN, bool bCancel)
{
   DeleteAutoBtnBmp();
   return CEtsPropertyPage::OnQueryCancel(pN, bCancel);
}

int  CBoxBaseDataPage::OnReset(PSHNOTIFY* pN)
{  
   return CEtsPropertyPage::OnReset(pN);
}

int  CBoxBaseDataPage::OnSetActive(PSHNOTIFY* pN, bool bAccept)
{
//   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CheckBoxType();
   InitDlgCtrls();
   SetBassImpedance();
   CheckWizardButtons();
   return CEtsPropertyPage::OnSetActive(pN, bAccept);
}

int  CBoxBaseDataPage::OnWizardBack(PSHNOTIFY* pN, bool bAccept)
{
   m_nFlags |= NOERRORWINDOW;
   OnWizardNext(pN, bAccept);
   m_nFlags &= ~NOERRORWINDOW;
   int nResult = 1;
   if (::GetFocus() == ::GetDlgItem(pN->hdr.hwndFrom, pN->hdr.idFrom)) nResult = 0;
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, nResult);
   return 1;
}

int  CBoxBaseDataPage::OnWizardFinish(PSHNOTIFY*pN, bool bFinish)
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

int  CBoxBaseDataPage::OnWizardNext(PSHNOTIFY*pN, bool bAccept)
{
   REPORT("CBoxBaseDataPage::OnWizardNext");
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData         *pB      = &pParent->m_BasicData;
   bAccept = false;

   if (pN) SetPageButtonFocus(pN);

   if      (pB->nNennImped     ==   0) CreateErrorWnd(IDC_BB_EDT_IMPEDANCE, IDS_ERROR_WRONGINPUT, true);
   else if (pB->dEffectivDBWm  == 0.0) CreateErrorWnd(IDC_BB_EDT_SENSITIVITY, IDS_ERROR_WRONGINPUT, true);
   else if (pB->dLowLimitFreq  == 0.0) CreateErrorWnd(IDC_BB_EDT_RANGEFROM, IDS_ERROR_WRONGINPUT, true);
   else if (pB->dHighLimitFreq == 0.0) CreateErrorWnd(IDC_BB_EDT_RANGETO, IDS_ERROR_WRONGINPUT, true);
   else if ((pB->dHighLimitFreq-pB->dLowLimitFreq)<1.0)
   { 
      pB->dHighLimitFreq = pB->dLowLimitFreq+1;
      SetDlgItemDouble(IDC_BB_EDT_RANGETO, pB->dHighLimitFreq, 2);
      CreateErrorWnd(IDC_BB_EDT_RANGETO, IDS_EQUAL_VALUES, true);
   }
   else if ((pParent->m_nLSType & DIPOLE_FLAT) || (LOWORD(pParent->m_nLSType) == ADD_ON3))
   {
      pB->dMainResFreq = pB->dLowLimitFreq * 1.2;
      bAccept = true;
   }
   else if (pB->nBoxTyp     == 0) CreateErrorWnd(IDC_BB_R_TYPE0, IDS_ERROR_WRONGINPUT, true);
   else if (pB->nTypeTTResp == 0) CreateErrorWnd(IDC_BB_R_FR0, IDS_ERROR_WRONGINPUT, true);
   else bAccept= true;
   
   if (bAccept)
   {
      bAccept = false;
      switch (pB->nBoxTyp)
      {
         case CARABOX_BOXTYP_CLOS:
            if      (pB->dMainResFreq     == 0.0) CreateErrorWnd(IDC_BB_EDT_CLOSEDMAX, IDS_ERROR_WRONGINPUT, true);
            else bAccept= true;
            break;
         case CARABOX_BOXTYP_BR: case CARABOX_BOXTYP_TL: case CARABOX_BOXTYP_PM:
            if      (pB->dImpedLowFreqBR  == 0.0) CreateErrorWnd(IDC_BB_EDT_BR_TLMAX1, IDS_ERROR_WRONGINPUT, true);
            else if (pB->dImpedMidFreqBR  == 0.0) CreateErrorWnd(IDC_BB_EDT_BR_TLMIN , IDS_ERROR_WRONGINPUT, true);
            else if (pB->dImpedHighFreqBR == 0.0) CreateErrorWnd(IDC_BB_EDT_BR_TLMAX2, IDS_ERROR_WRONGINPUT, true);
            else bAccept= true;
            break;
         case 0:  // 
            if (pParent->m_nLSType & DIPOLE_FLAT) bAccept = true;
            else ASSERT(false);
            break;
      }
   }

   if (bAccept) pParent->m_nFlagOk |=  CARABOX_BASE_DATA;
   else         pParent->m_nFlagOk &= ~CARABOX_BASE_DATA;
   if (pN)
   {
      if (!HasPageButtonFocus(pN)) bAccept = false;
      pN->lParam = bAccept;
      return CEtsPropertyPage::OnWizardNext(pN, bAccept);
   }
   return 0;
}
 
void CBoxBaseDataPage::CheckBoxType()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData         *pB      = &pParent->m_BasicData;
   bool bClosed = false, bBrTl = false, bBoxType = true, bTtReponseType = false;
   if ((pParent->m_nLSType & DIPOLE_FLAT) || (LOWORD(pParent->m_nLSType) == ADD_ON3))
   {
      bBoxType = false;
   }
   else
   {
      switch (pB->nBoxTyp)
      {
         case CARABOX_BOXTYP_CLOS:                                // Typ = Closed Box
            bClosed = true;
            bTtReponseType = true;
            break;
         case CARABOX_BOXTYP_BR:                                  // Typ = Vented Box
         case CARABOX_BOXTYP_TL:                                  // Typ = TL-Box
         case CARABOX_BOXTYP_PM:                                  // Typ = TL-Box
            bBrTl = true;
            break;
         default: break;                                          // undefiniert
      }
   }

   EnableWindow(GetDlgItem(IDC_BB_R_TYPE0), bBoxType);
   EnableWindow(GetDlgItem(IDC_BB_R_TYPE2), bBoxType);
   EnableWindow(GetDlgItem(IDC_BB_R_TYPE1), bBoxType);
   EnableWindow(GetDlgItem(IDC_BB_R_TYPE3), bBoxType);

   EnableWindow(GetDlgItem(IDC_BB_EDT_CLOSEDMAX), bClosed);
   EnableWindow(GetDlgItem(IDC_BB_EDT_BR_TLMAX1), bBrTl);
   EnableWindow(GetDlgItem(IDC_BB_EDT_BR_TLMAX2), bBrTl);
   EnableWindow(GetDlgItem(IDC_BB_EDT_BR_TLMIN) , bBrTl);
   EnableWindow(GetDlgItem(IDC_BB_CK_CALC_IMPEDANCE_VALUES), bClosed||bBrTl);

   EnableWindow(GetDlgItem(IDC_BB_R_FR0), bTtReponseType);
   EnableWindow(GetDlgItem(IDC_BB_R_FR1), bTtReponseType);
   EnableWindow(GetDlgItem(IDC_BB_R_FR2), bTtReponseType);
}

void CBoxBaseDataPage::CheckBRTLMinMax(int nID)
{
   BasicData * pB = &((CBoxPropertySheet*)m_pParent)->m_BasicData;
   if ((pB->dImpedLowFreqBR != 0) && (pB->dImpedMidFreqBR != 0) && (pB->dImpedHighFreqBR != 0))
   {
      CSortFunctions::SetSortDirection(true);
      qsort(&pB->dImpedLowFreqBR, 3, sizeof(double), CSortFunctions::SortDouble);
      if (CSortFunctions::gm_bEqual)
      {
         CEtsDialog::CreateErrorWnd(nID, IDS_EQUAL_VALUES);
         switch (nID)
         {
            case IDC_BB_EDT_BR_TLMAX1: pB->dImpedLowFreqBR  = 0; break;
            case IDC_BB_EDT_BR_TLMIN:  pB->dImpedMidFreqBR  = 0; break;
            case IDC_BB_EDT_BR_TLMAX2: pB->dImpedHighFreqBR = 0; break;
         }
         SetDlgItemText(nID, "");
         CSortFunctions::gm_bChanged = false;
      }
      if (CSortFunctions::gm_bChanged)
      {
         SetDlgItemDouble(IDC_BB_EDT_BR_TLMAX1, pB->dImpedLowFreqBR , 2);
         SetDlgItemDouble(IDC_BB_EDT_BR_TLMIN , pB->dImpedMidFreqBR , 2);
         SetDlgItemDouble(IDC_BB_EDT_BR_TLMAX2, pB->dImpedHighFreqBR, 2);
      }
   }
}


void CBoxBaseDataPage::InitDlgCtrls()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData         *pB      = &pParent->m_BasicData;
   int nBtn;

   SetDlgItemInt(IDC_BB_EDT_IMPEDANCE, pB->nNennImped, false);

   if ((LOWORD(pParent->m_nLSType) == ADD_ON3) && (pB->dLowLimitFreq  < MIN_ADD_ON_HT  ))
   {
      pB->dLowLimitFreq  = MIN_ADD_ON_HT;
      pParent->SetChanged(CARABOX_BASE_DATA);
   }
   if ((LOWORD(pParent->m_nLSType) == ADD_ON1) && ((pB->dHighLimitFreq > MAX_ADD_ON_SUBW)||(pB->dHighLimitFreq == 0.0)))
   {
      pB->dHighLimitFreq = MAX_ADD_ON_SUBW;
      pParent->SetChanged(CARABOX_BASE_DATA);
   }

   SetDlgItemDouble(IDC_BB_EDT_SENSITIVITY, pB->dEffectivDBWm   , 1);
   SetDlgItemDouble(IDC_BB_EDT_RANGEFROM  , pB->dLowLimitFreq   , 2);
   SetDlgItemDouble(IDC_BB_EDT_RANGETO    , pB->dHighLimitFreq  , 1);

   if ((pParent->m_nLSType & DIPOLE_FLAT) || (LOWORD(pParent->m_nLSType) == ADD_ON3))
   {                                                           // Dipol oder Zusatz Hochtöner
      SetDlgItemText(IDC_BB_EDT_CLOSEDMAX, "");
      SetDlgItemText(IDC_BB_EDT_BR_TLMAX1, "");
      SetDlgItemText(IDC_BB_EDT_BR_TLMAX2, "");
      SetDlgItemText(IDC_BB_EDT_BR_TLMIN , "");
      nBtn = -1;
      CheckDlgRadioBtn(IDC_BB_R_TYPE0, nBtn);
      CheckDlgRadioBtn(IDC_BB_R_FR0, nBtn);
      ::CheckDlgButton(m_hWnd, IDC_BB_CK_CALC_IMPEDANCE_VALUES, false);
   }
   else
   {
      switch (pB->nBoxTyp)
      {
         case CARABOX_BOXTYP_BR:   nBtn = 0; break;
         case CARABOX_BOXTYP_CLOS: nBtn = 1; break;
         case CARABOX_BOXTYP_TL:   nBtn = 2; break;
         case CARABOX_BOXTYP_PM:   nBtn = 3; break;
        default:                   nBtn = -1;break;
      }
      CheckDlgRadioBtn(IDC_BB_R_TYPE0, nBtn);

      if (pB->nBoxTyp == CARABOX_BOXTYP_CLOS)
      {
         SetDlgItemDouble(IDC_BB_EDT_CLOSEDMAX  , pB->dMainResFreq , 2);
         SetDlgItemText(IDC_BB_EDT_BR_TLMAX1, "");
         SetDlgItemText(IDC_BB_EDT_BR_TLMAX2, "");
         SetDlgItemText(IDC_BB_EDT_BR_TLMIN , "");
         switch (pB->nTypeTTResp)
         {
            case CARABOX_TTRESPONSETYPMINU: nBtn = 0; break;// TT-Frequenzgangtyp für tiefe Frequenzen: überdämpft (Q=0.5)
            case CARABOX_TTRESPONSETYPNORM: nBtn = 1; break;// TT-Frequenzgangtyp für tiefe Frequenzen: normal (Q=0.9..1)
            case CARABOX_TTRESPONSETYPPLUS: nBtn = 2; break;// TT-Frequenzgangtyp für tiefe Frequenzen: unterdämpft (Q=2)
            default:                        nBtn = -1;break;  // undefiniert
         }
      }
      else
      {
         SetDlgItemText(IDC_BB_EDT_CLOSEDMAX, "");
         SetDlgItemDouble(IDC_BB_EDT_BR_TLMAX1, pB->dImpedLowFreqBR , 2);
         SetDlgItemDouble(IDC_BB_EDT_BR_TLMAX2, pB->dImpedHighFreqBR, 2);
         SetDlgItemDouble(IDC_BB_EDT_BR_TLMIN , pB->dImpedMidFreqBR , 2);
         nBtn = -1;
      }
      CheckDlgRadioBtn(IDC_BB_R_FR0, nBtn);
      ::CheckDlgButton(m_hWnd, IDC_BB_CK_CALC_IMPEDANCE_VALUES, (pParent->m_nFlagOk & CARABOX_IMPED_EDITED) ? false : true);
   }
}


void CBoxBaseDataPage::SetBassImpedance()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData         *pB      = &pParent->m_BasicData;
   if ((LOWORD(pParent->m_nLSType) == ADD_ON3) || (pParent->m_nLSType & DIPOLE_FLAT))
   {
      // nix
   }
   else if (!(pParent->m_nFlagOk & CARABOX_IMPED_EDITED))
   {
      if (pB->nBoxTyp == CARABOX_BOXTYP_CLOS)
      {
         double dFactor = 1.4;
         switch (pB->nTypeTTResp)
         {
            case CARABOX_TTRESPONSETYPMINU: dFactor = 0.7; break;
            case CARABOX_TTRESPONSETYPPLUS: dFactor = 1.8; break;
            default:break;
         }
         pB->dMainResFreq = pB->dLowLimitFreq * dFactor;
         SetDlgItemDouble(IDC_BB_EDT_CLOSEDMAX, pB->dMainResFreq, 2);
      }
      else if ((pB->nBoxTyp == CARABOX_BOXTYP_BR) ||
               (pB->nBoxTyp == CARABOX_BOXTYP_TL) ||
               (pB->nBoxTyp == CARABOX_BOXTYP_PM))
      {
         pB->dImpedMidFreqBR  = pB->dLowLimitFreq   * 1.18;
         pB->dImpedLowFreqBR  = pB->dImpedMidFreqBR / 1.75;
         pB->dImpedHighFreqBR = pB->dImpedMidFreqBR * 1.75;
         SetDlgItemDouble(IDC_BB_EDT_BR_TLMIN   , pB->dImpedMidFreqBR , 2);
         SetDlgItemDouble(IDC_BB_EDT_BR_TLMAX1  , pB->dImpedLowFreqBR , 2);
         SetDlgItemDouble(IDC_BB_EDT_BR_TLMAX2  , pB->dImpedHighFreqBR, 2);
      }
   }
}

void CBoxBaseDataPage::CheckWizardButtons()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   m_nFlags |= NOERRORWINDOW;
   OnWizardNext(NULL, false);
   m_nFlags &= ~NOERRORWINDOW;
   pParent->SetWizardButtons(true, (pParent->m_nFlagOk & CARABOX_BASE_DATA)? true : false);
}
