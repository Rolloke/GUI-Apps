/********************************************************************/
/* Funktionen der Klasse CBoxChassisDataPage                        */
/********************************************************************/

#include "BoxChassisData.h"
#include "BoxPropertySheet.h"
#include "resource.h"
#include "AfxRes.h"
#include "Debug.h"
#include "CSortFunctions.h"
#include "BOXGLOBAL.H"
#include "COpenGLView.h"

#ifdef _DEBUG_HEAP
 #ifdef Alloc
  #undef Alloc
 #endif
 #define Alloc(p1, p2) Alloc(p1, p2, __FILE__, __LINE__)
 #define new DLL_DEBUG_NEW
#endif

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define CCE_CTRL_ID      0
#define CCE_CHASSIS_TYPE 1
#define CCE_BOX_CO_TYPE  2

const long CBoxChassisDataPage::gm_nChassisCountEdt[CARABOX_CHASSISTYPES][3] = 
{
   {IDC_CH_EDT_BR_TUBES  , CARABOX_CHASSISTYP_BR , 1<<CARABOX_BOXTYP_BR},
   {IDC_CH_EDT_T_LINES   , CARABOX_CHASSISTYP_TL , 1<<CARABOX_BOXTYP_TL},
   {IDC_CH_EDT_P_MEMBRANE, CARABOX_CHASSISTYP_PM , 1<<CARABOX_BOXTYP_PM},
   {IDC_CH_EDT_TT1       , CARABOX_CHASSISTYP_TT1, 1<<CARABOX_CO_2W|1<<CARABOX_CO_2_3W|1<<CARABOX_CO_3W|1<<CARABOX_CO_3_4W|1<<CARABOX_CO_4W|1<<CARABOX_CO_5W|0x00010000<<ADD_ON1|0x00010000<<ADD_ON2|0x00010000<<STAND_ALONE},
   {IDC_CH_EDT_TT2       , CARABOX_CHASSISTYP_TT2,                  1<<CARABOX_CO_2_3W|                 1<<CARABOX_CO_3_4W},
   {IDC_CH_EDT_MT1       , CARABOX_CHASSISTYP_MT1,                                     1<<CARABOX_CO_3W|1<<CARABOX_CO_3_4W|1<<CARABOX_CO_4W|1<<CARABOX_CO_5W},
   {IDC_CH_EDT_MT2       , CARABOX_CHASSISTYP_MT2,                                                                         1<<CARABOX_CO_4W|1<<CARABOX_CO_5W},
   {IDC_CH_EDT_MT3       , CARABOX_CHASSISTYP_MT3,                                                                                          1<<CARABOX_CO_5W},
   {IDC_CH_EDT_HT        , CARABOX_CHASSISTYP_HT , 1<<CARABOX_CO_2W|1<<CARABOX_CO_2_3W|1<<CARABOX_CO_3W|1<<CARABOX_CO_3_4W|1<<CARABOX_CO_4W|1<<CARABOX_CO_5W|0x00010000<<ADD_ON3}
};


CBoxChassisDataPage::CBoxChassisDataPage()
{
   m_bListChanged = true;
   m_pCurrent = NULL;
}

bool CBoxChassisDataPage::OnInitDialog(HWND hWnd)
{
   if (CEtsPropertyPage::OnInitDialog(hWnd))
   {
      ::EnumChildWindows(hWnd, SetHelpIDs, (LPARAM)hWnd);
      SetNumeric(IDC_CH_EDT_DIAMETER, true, true);
      SetNumeric(IDC_CH_EDT_WIDTH, true, true);
      SetNumeric(IDC_CH_EDT_HEIGHT, true, true);

      return true;
   }
   return false;
}

BOOL CALLBACK CBoxChassisDataPage::SetHelpIDs(HWND hwnd, LPARAM /*lParam*/)
{
   switch (::GetWindowLong(hwnd, GWL_ID))
   {
      case IDC_CH_GRP_MEMBRANE_PARAM:
      case IDC_CH_EDT_DIAMETER: case IDC_CH_EDT_HEIGHT: case IDC_CH_EDT_WIDTH:
      case IDC_CH_TXT_HEIGHT: case IDC_CH_TXT_WIDTH:
         ::SetWindowContextHelpId(hwnd, IDC_CH_TXT_DIAMETER);
         break;
      case IDC_CH_R_M_FORM1: case IDC_CH_R_M_FORM2: case IDC_CH_R_M_FORM3:
      case IDC_CH_R_M_FORM4: case IDC_CH_R_M_FORM5:
      case IDC_CH_TXT_HT: case IDC_CH_EDT_HT: case IDC_CH_TXT_MT3: case IDC_CH_TXT_MT2: case IDC_CH_TXT_MT1: case IDC_CH_TXT_MT:
      case IDC_CH_EDT_MT3: case IDC_CH_EDT_MT2: case IDC_CH_EDT_MT1: case IDC_CH_TXT_TT2: case IDC_CH_TXT_TT1: case IDC_CH_TXT_TT:
      case IDC_CH_EDT_TT2: case IDC_CH_EDT_TT1: case IDC_CH_TXT_T_LINES: case IDC_CH_EDT_T_LINES: case IDC_CH_TXT_P_MEMBRANES:
      case IDC_CH_EDT_P_MEMBRANE: case IDC_CH_TXT_BR_TUBES: case IDC_CH_EDT_BR_TUBES: case IDC_CH_GRP_TYPE_COUNT2:
      case IDC_CH_GRP_TYPE_COUNT1: case IDC_CH_GRP_TYPE_COUNT:
         ::SetWindowContextHelpId(hwnd, IDC_CH_GRP_TYPE_COUNT);
         break;
      case IDC_CH_EDT_DESCRIPTION: ::SetWindowContextHelpId(hwnd, IDC_CH_TXT_DESCRIPTION);break;
      case IDC_CH_BTN_CALC:        ::SetWindowContextHelpId(hwnd, IDC_CD_BTN_CALC);break;
   }
   return 1;
}

int CBoxChassisDataPage::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
#ifdef _DEBUG
   CEtsList  *plstCD = &pParent->m_ChassisData;
#endif
   bool     bChanged = false;
   switch (nID)
   {
      case IDC_CH_BTN_CALC:
         pParent->OnBtnClckSPLView();
         break;
      case IDC_CH_EDT_BR_TUBES: case IDC_CH_EDT_T_LINES: case IDC_CH_EDT_P_MEMBRANE:
      case IDC_CH_EDT_TT1:      case IDC_CH_EDT_TT2: 
      case IDC_CH_EDT_MT1:      case IDC_CH_EDT_MT2:     case IDC_CH_EDT_MT3:
      case IDC_CH_EDT_HT:
      if (nNotifyCode == EN_KILLFOCUS) OnKillFocusChassisCount(nID, hwndControl);
      break;
      case IDC_CH_LST_LS:
      if (nNotifyCode == LBN_SETFOCUS) OnSetFocusChassisList(nID, hwndControl);
      else if (nNotifyCode == LBN_SELCHANGE) OnSelChangeChassisList(nID, hwndControl);
      break;
      default:break;
   }

   if (m_pCurrent)                                             // Chassisparameter
   {
      ASSERT(plstCD->InList(m_pCurrent));
      switch (nID)
      {
         case IDC_CH_EDT_DESCRIPTION:
         if (nNotifyCode == EN_KILLFOCUS)                      // Chassisbeschreibung
         {
            GetDlgItemText(nID, m_pCurrent->szDescrpt, &bChanged, BOXTEXT_LEN-1);
         } break;
         case IDC_CH_EDT_DIAMETER:                             // Chassisdurchmesser
         if (((nNotifyCode == EN_KILLFOCUS) && !(m_nFlags & NOKILLFOCUSNOTIFICATION)) ||
              (nNotifyCode == EN_CHANGE))
         {
            if (GetDlgItemDouble(nID, m_pCurrent->dEffDiameter, 0, &bChanged) && bChanged)
            {
               CheckMinMaxDouble(nID, 0.0, 10000.0, 2, m_pCurrent->dEffDiameter);
               CheckCurrentChassisPosition();
            }
         } break;
         case IDC_CH_EDT_WIDTH:                 // Chassisbreite
         if (((nNotifyCode == EN_KILLFOCUS) && !(m_nFlags & NOKILLFOCUSNOTIFICATION)) ||
              (nNotifyCode == EN_CHANGE))
         {
            if (GetDlgItemDouble(nID, m_pCurrent->dEffBreite, 0, &bChanged) && bChanged)
            {
               double dMax = 10000.0;
               if (((pParent->m_nLSType & DIPOLE_FLAT) || (m_pCurrent->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE)) && 
                   (m_pCurrent->dEffHoehe > 0) && (nNotifyCode != EN_CHANGE)) dMax = m_pCurrent->dEffHoehe;
               CheckMinMaxDouble(nID, 0.0, dMax, 2, m_pCurrent->dEffBreite);
               CheckCurrentChassisPosition();
            }
         } break;
         case IDC_CH_EDT_HEIGHT:                // Chassishöhe
         if (((nNotifyCode == EN_KILLFOCUS) && !(m_nFlags & NOKILLFOCUSNOTIFICATION)) ||
              (nNotifyCode == EN_CHANGE))
         {
            if (GetDlgItemDouble(nID, m_pCurrent->dEffHoehe, 0, &bChanged) && bChanged)
            {
               double dMin = 0.0;
               if (((pParent->m_nLSType & DIPOLE_FLAT) || (m_pCurrent->GetMembrTyp() == CARABOX_MEMBRTYP_DIPOLE)) &&
                   (m_pCurrent->dEffBreite > 0) && (nNotifyCode != EN_CHANGE)) dMin = m_pCurrent->dEffBreite; 
               CheckMinMaxDouble(nID, dMin, 10000.0, 2, m_pCurrent->dEffHoehe);
               CheckCurrentChassisPosition();
            }
         } break;
         case IDC_CH_CHK_POLARITY:
         if (nNotifyCode == BN_CLICKED)
         {
            m_pCurrent->nPolung = ((SendDlgItemMessage(nID, BM_GETSTATE, 0, 0) & BST_CHECKED) ? -1 : 1);
            bChanged = true;
         } break;
         case IDC_CH_R_M_FORM0: case IDC_CH_R_M_FORM1: case IDC_CH_R_M_FORM2: case IDC_CH_R_M_FORM3: case IDC_CH_R_M_FORM4: case IDC_CH_R_M_FORM5:
         if (nNotifyCode == BN_CLICKED)
         {
            int nOld = m_pCurrent->GetMembrTyp();
            int nIndex = GetDlgRadioBtn(IDC_CH_R_M_FORM0);
            SetMembraneTypeFromIndex(m_pCurrent, nIndex);
            if (nOld != m_pCurrent->GetMembrTyp()) bChanged = true;
            else if (pParent->m_nLSType & DIPOLE_FLAT_H)
            {
               nIndex = -1;
               CheckDlgRadioBtn(IDC_CH_R_M_FORM0, nIndex);
               SetMembraneTypeFromIndex(m_pCurrent, nIndex);
               CheckChassisType();
            }
            CheckChassisForm();
         } break;
         default: break;
      }
   }
   if (bChanged)
   {
      CheckWizardButtons();
      pParent->SetChanged(CARABOX_CHASSIS_DATA);
      EnableWindow(GetDlgItem(IDC_CH_BTN_CALC), IsWindowEnabled(pParent->GetWndFinishBtn()));
   }
   return CEtsPropertyPage::OnCommand(nID , nNotifyCode, hwndControl);
}

int CBoxChassisDataPage::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch (nMsg)
   {
      case WM_DRAWITEM: return OnDrawItem((DRAWITEMSTRUCT *)lParam);
   }
   return CEtsPropertyPage::OnMessage(nMsg, wParam, lParam);
}

int  CBoxChassisDataPage::OnHelp(PSHNOTIFY*pN)
{
   ::WinHelp(m_hWnd, "CARACAD.hlp", HELP_CONTEXT, 1994);
   return CEtsPropertyPage::OnHelp(pN);
}

int  CBoxChassisDataPage::OnReset(PSHNOTIFY* pN)
{
   SendDlgItemMessage(IDC_CH_LST_LS, LB_RESETCONTENT, 0, 0);
   m_bListChanged = true;
   m_pCurrent = NULL;
   return CEtsPropertyPage::OnReset(pN);
}

int  CBoxChassisDataPage::OnSetActive(PSHNOTIFY* pN, bool bAccept)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet  *pC = pParent->GetCabinet(BASE_CABINET);
   BasicData *pB = &pParent->m_BasicData;
   CEtsList  *plstCD = &pParent->m_ChassisData;

   if (pParent->m_nFlagChanged & UPDATE_CHASSIS_LIST)
   {
      CSortFunctions::SetSortDirection(false);
      plstCD->Sort(CSortFunctions::SortChassisType);
      int nCount = 0;
      plstCD->ProcessWithObjects(ChassisData::CountValidListChassis, (WPARAM)&nCount, 0);
      SendDlgItemMessage(IDC_CH_LST_LS, LB_SETCOUNT, nCount, 0);
      
      pParent->m_nFlagChanged &= ~UPDATE_CHASSIS_LIST;
   }

   CheckCrossOverType();

   if (pParent->m_nFlagChanged & CARABOX_CHASSIS_LIST)
   {
      UpdateChassisList();
//      pParent->m_nFlagChanged &= ~CARABOX_CHASSIS_LIST
   }
   if (!plstCD->InList(m_pCurrent)) m_pCurrent = NULL;

   int  i, *pChassis = &pC->nCountBR;
   for (i=0; i<CARABOX_CHASSISTYPES; i++)
   {
      SetDlgItemInt(gm_nChassisCountEdt[i][CCE_CTRL_ID], pChassis[i], false);
   }

   InitChassisData();

   int nTT, nBB;
   if ((pB->nXoverTyp == CARABOX_CO_1W) && (LOWORD(pParent->m_nLSType) == STAND_ALONE))
   {
      nBB = SW_SHOW;
      nTT = SW_HIDE;
   }
   else
   {
      nBB = SW_HIDE;
      nTT = SW_SHOW;
   }

   ::ShowWindow(GetDlgItem(IDC_CH_TXT_TT1), nTT);
   ::ShowWindow(GetDlgItem(IDC_CH_TXT_TT), nTT);
   ::ShowWindow(GetDlgItem(IDC_CH_TXT_BB1), nBB);
   ::ShowWindow(GetDlgItem(IDC_CH_TXT_BB), nBB);

   CheckWizardButtons();
   EnableWindow(GetDlgItem(IDC_CH_BTN_CALC), IsWindowEnabled(pParent->GetWndFinishBtn()));

   return CEtsPropertyPage::OnSetActive(pN, bAccept);
}

int  CBoxChassisDataPage::OnWizardBack(PSHNOTIFY* pN, bool bAccept)
{
   m_nFlags |= NOERRORWINDOW;
   OnWizardNext(pN, bAccept);
   m_nFlags &= ~NOERRORWINDOW;
   int nResult = 1;
   if (::GetFocus() == ::GetDlgItem(pN->hdr.hwndFrom, pN->hdr.idFrom)) nResult = 0;
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, nResult);
   return 1;
}

int  CBoxChassisDataPage::OnWizardFinish(PSHNOTIFY*pN, bool bFinish)
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

int  CBoxChassisDataPage::OnWizardNext(PSHNOTIFY*pN, bool bAccept)
{
   REPORT("CBoxChassisDataPage::OnWizardNext");
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;

   if (pN) SetPageButtonFocus(pN);

   if (m_bListChanged)
   {
      UpdateChassisList();
   }

   CCabinet  *pC     = pParent->GetCabinet(BASE_CABINET);
   BasicData *pB     = &pParent->m_BasicData;
   CEtsList  *plstCD = &pParent->m_ChassisData;
   bAccept = false;
   
   if (pC->nCountAllChassis == 0) CreateErrorWnd(IDC_CH_GRP_TYPE_COUNT, IDS_ERROR_WRONGINPUT, true);
   else 
   {
      int  i, nCount = 0, *pChassis = &pC->nCountBR;
      for (i=0; i<3; i++)
      {
         nCount += pChassis[i];
      }
      if ((nCount == 0) && (pB->nBoxTyp != CARABOX_BOXTYP_CLOS) && !(pParent->m_nLSType & DIPOLE_FLAT))
      {
         CreateErrorWnd(IDC_CH_GRP_TYPE_COUNT, IDS_ERROR_WRONGINPUT, true);
         pParent->m_nFlagOk &= ~CARABOX_CHASSIS_NO;
      }
      else
      {
         pParent->m_nFlagOk |= CARABOX_CHASSIS_NO;
         int nCOType  = 1 << pB->nXoverTyp;
         if (pB->nXoverTyp == CARABOX_CO_1W)
         {
            nCOType |= 0x00010000 << LOWORD(pParent->m_nLSType);
         }
         for (; i<CARABOX_CHASSISTYPES; i++)
         {
            if (((gm_nChassisCountEdt[i][CCE_BOX_CO_TYPE] & nCOType) != 0) && (pChassis[i] == 0))
            {
               CreateErrorWnd(gm_nChassisCountEdt[i][CCE_CTRL_ID], IDS_ERROR_WRONGINPUT, true);
               break;
            }
         }

         if (i == CARABOX_CHASSISTYPES)
         {
            nCount = 0;
            plstCD->ProcessWithObjects(CBoxChassisDataPage::CheckChassisData, (WPARAM)&nCount, (LPARAM)this);
            if (nCount != -1) bAccept = true;
         }
      }

      if (bAccept) pParent->m_nFlagOk |=  CARABOX_CHASSIS_DATA;
      else         pParent->m_nFlagOk &= ~CARABOX_CHASSIS_DATA;
   }

   
   if (pN)
   {
      if (!HasPageButtonFocus(pN)) bAccept = false;
      pN->lParam = bAccept;
      return CEtsPropertyPage::OnWizardNext(pN, bAccept);
   }
   return 0;
}

int CBoxChassisDataPage::OnKillFocusChassisCount(WORD nID, HWND /*hwndControl*/)
{
   CCabinet *pC   = &((CBoxPropertySheet*)m_pParent)->m_Cabinet;
   int  i, nMax  = CARABOX_ANZMAXCHASSIS - pC->nCountAllChassis,
        nOld;
   int *pChassis = &pC->nCountBR;

   for (i=0; i<CARABOX_CHASSISTYPES; i++)
      if (gm_nChassisCountEdt[i][CCE_CTRL_ID] == nID) break;
   
   if (i==CARABOX_CHASSISTYPES) return false;

   nOld = pChassis[i];
   if (GetDlgItemInt(nID, pChassis[i], false) && CheckMinMaxInt(nID, 0, nMax, false, pChassis[i]))
   {
      if (nOld != pChassis[i])
      {
         m_bListChanged = true;
         ((CBoxPropertySheet*)m_pParent)->SetChanged(CARABOX_CHASSIS_DATA);
      }
      CountAllChassis();
   }
   if (m_bListChanged)
   {
      UpdateChassisList();
   }
   return true;
}

int CBoxChassisDataPage::OnSetFocusChassisList(WORD /*nID*/, HWND /*hwndControl*/)
{
   if (m_bListChanged)
   {
      UpdateChassisList();
   }
   return 1;
}

int CBoxChassisDataPage::OnSelChangeChassisList(WORD /*nID*/, HWND hwndControl)
{
   int nCurSel = SendMessage(hwndControl, LB_GETCURSEL, 0, 0);
   if (nCurSel != LB_ERR)
   {
      CEtsList *plstCD = &((CBoxPropertySheet*)m_pParent)->m_ChassisData;
      m_pCurrent = (ChassisData*)plstCD->GetAt(nCurSel);
      ASSERT(m_pCurrent != NULL);
      InitChassisData();
   }
   else m_pCurrent = NULL;
   return 1;
}

void CBoxChassisDataPage::CheckCrossOverType()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData *pB = &pParent->m_BasicData;
   CCabinet  *pC = pParent->GetCabinet(BASE_CABINET);
   bool bEnable;
   int  i, *pChassis = &pC->nCountBR;
   int  nBoxType     = 1 << pB->nBoxTyp,
        nCOType      = 1 << pB->nXoverTyp;

   if (pB->nXoverTyp == CARABOX_CO_1W)
   {
      nCOType |= 0x00010000 << LOWORD(pParent->m_nLSType);
   }
    
   for (i=0; i<CARABOX_CHASSISTYPES; i++)
   {
      if (i<3)
         bEnable = ((gm_nChassisCountEdt[i][CCE_BOX_CO_TYPE] & nBoxType) != 0) ? true : false;
      else
         bEnable = ((gm_nChassisCountEdt[i][CCE_BOX_CO_TYPE] & nCOType) != 0) ? true : false;
      EnableWindow(GetDlgItem(gm_nChassisCountEdt[i][CCE_CTRL_ID]), bEnable);
      if (bEnable)                     // Chassis oder Passives Element ist vorhanden
      {
         if (pChassis[i]==0)           // Keines da ?
         {
            pChassis[i] = 1;           // es soll mindestens eins da sein
            pParent->SetChanged(CARABOX_CHASSIS_LIST);
         }
      }
      else                             // Chassis oder Passives Element ist nicht vorhanden
      {
         if (pChassis[i])              // Nanu, eines da ?
         {
            pChassis[i] = 0;           // darf es aber nicht, löschen
            pParent->SetChanged(CARABOX_CHASSIS_LIST);
         }
      }
   }
}

void CBoxChassisDataPage::CountAllChassis()
{
   ((CBoxPropertySheet*)m_pParent)->m_Cabinet.CountAllChassis();
}

void CBoxChassisDataPage::InitChassisData()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CEtsList *plstCD = &pParent->m_ChassisData;
   if (!plstCD->InList(m_pCurrent))
      m_pCurrent= NULL;

   if (m_pCurrent == NULL)
   {
      SetDlgItemText(IDC_CH_EDT_DESCRIPTION, "");
      CheckDlgRadioBtn(IDC_CH_R_M_FORM0, -1);
      CheckDlgButton(m_hWnd, IDC_CH_CHK_POLARITY, BST_UNCHECKED);
      SetDlgItemText(IDC_CH_EDT_DIAMETER,    "");
      SetDlgItemText(IDC_CH_EDT_WIDTH,       "");
      SetDlgItemText(IDC_CH_EDT_HEIGHT,      "");
      CheckChassisForm();
      CheckChassisType();
   }
   else
   {
      int nForm;
      HWND hwndItem;
      CheckChassisType();
      SetDlgItemText(IDC_CH_EDT_DESCRIPTION, m_pCurrent->szDescrpt, BOXTEXT_LEN-1);
      CheckDlgButton(m_hWnd, IDC_CH_CHK_POLARITY, (m_pCurrent->nPolung < 0) ? BST_CHECKED : BST_UNCHECKED);
      switch (m_pCurrent->GetMembrTyp())
      {
         case CARABOX_MEMBRTYP_CON   : nForm = 0; break;
         case CARABOX_MEMBRTYP_DOME  : nForm = 1; break;
         case CARABOX_MEMBRTYP_FLAT1 : nForm = 2; break;
         case CARABOX_MEMBRTYP_FLAT2 : nForm = 3; break;
         case CARABOX_MEMBRTYP_SPHERE: nForm = 4; break;
         case CARABOX_MEMBRTYP_DIPOLE: nForm = 5; break;
         default:
            if (pParent->m_nLSType & DIPOLE_FLAT_H) nForm = -1;
            else if (m_pCurrent->nChassisTyp == CARABOX_CHASSISTYP_HT)
            {
               nForm = 1;
            }
            else
            {
               for (nForm=0; ; nForm++)
               {
                  hwndItem = GetDlgItem(IDC_CH_R_M_FORM0+nForm);
                  if (hwndItem==NULL) 
                  {
                     nForm=-1;
                     break;
                  }
                  if (IsWindowEnabled(hwndItem)) break;
               }
            }
            SetMembraneTypeFromIndex(m_pCurrent, nForm);
            break;
      }
      CheckChassisForm();
      CheckDlgRadioBtn(IDC_CH_R_M_FORM0, nForm);
      SetDlgItemDouble(IDC_CH_EDT_DIAMETER, m_pCurrent->dEffDiameter, 1);
      SetDlgItemDouble(IDC_CH_EDT_WIDTH, m_pCurrent->dEffBreite, 1);
      SetDlgItemDouble(IDC_CH_EDT_HEIGHT, m_pCurrent->dEffHoehe, 1);
   }
}

void CBoxChassisDataPage::CheckChassisForm()
{
   bool bWidth = false, bHeight = false, bCircle = false;
   if (m_pCurrent)
   {
      if (m_pCurrent->GetMembrTyp() != 0)
      {
         if (m_pCurrent->IsRect())
         {
            bWidth = bHeight = true;
         }
         else
         {
            bCircle = true;
         }
      }
   }
   EnableWindow(GetDlgItem(IDC_CH_EDT_DIAMETER), bCircle);
   EnableWindow(GetDlgItem(IDC_CH_EDT_WIDTH)   , bWidth);
   EnableWindow(GetDlgItem(IDC_CH_EDT_HEIGHT)  , bHeight);
}

void CBoxChassisDataPage::CheckChassisType()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BasicData * pB =  &pParent->m_BasicData;
   bool bCone     = false,
        bDome     = false,
        bFlatCirc = false,
        bFlatRect = false,
        bDipole   = false,
        bSphere   = false,
        bPole     = false;
   if (m_pCurrent)
   {
      if (pParent->m_nLSType & DIPOLE_FLAT)
      {
         if (m_pCurrent->nChassisTyp != CARABOX_CHASSISTYP_TT1) bPole = true;
      }
      else
      {
         switch (m_pCurrent->nChassisTyp)
         {
            case CARABOX_CHASSISTYP_TT1:
               if ((LOWORD(pParent->m_nLSType) == STAND_ALONE) && (pB->nBoxTyp == CARABOX_BOXTYP_CLOS))
                  bPole = true;  // Tieftöner geschlossener Standardboxen dürfen umgepolt werden
            case CARABOX_CHASSISTYP_TT2:
            case CARABOX_CHASSISTYP_PM:
               bCone = true;
            case CARABOX_CHASSISTYP_BR: case CARABOX_CHASSISTYP_TL:
               bFlatCirc = bFlatRect = true;
               break;
            case CARABOX_CHASSISTYP_MT1: case CARABOX_CHASSISTYP_MT2: case CARABOX_CHASSISTYP_MT3:
            case CARABOX_CHASSISTYP_HT:
            {
               bool bNoDipole = true;
               int  nSpheres = 0;
               if (pParent->m_nLSType & DIPOLE_FLAT_H)
               {
                  int  nDipoles = 0;
                  int  nNoDipoles = 0;
                  pParent->m_ChassisData.ProcessWithObjects(ChassisData::CountDipoles, (WPARAM)&nDipoles  , MAKELONG(m_pCurrent->nChassisTyp, 1));
                  pParent->m_ChassisData.ProcessWithObjects(ChassisData::CountDipoles, (WPARAM)&nNoDipoles, MAKELONG(m_pCurrent->nChassisTyp, 0));
                  if (nNoDipoles < 1) bDipole = true;
                  if (nDipoles   > 0) bNoDipole = false;
                  switch (m_pCurrent->nChassisTyp)
                  {
                     case CARABOX_CHASSISTYP_MT1:
                        if (pParent->m_Cabinet.nCountMT1 == 1) bNoDipole = bDipole = true;
                        break;
                     case CARABOX_CHASSISTYP_MT2:
                        if (pParent->m_Cabinet.nCountMT2 == 1) bNoDipole = bDipole = true;
                        break;
                     case CARABOX_CHASSISTYP_MT3:
                        if (pParent->m_Cabinet.nCountMT3 == 1) bNoDipole = bDipole = true;
                        break;
                     case CARABOX_CHASSISTYP_HT :
                        if (pParent->m_Cabinet.nCountHT  == 1) bNoDipole = bDipole = true;
                        break;
                  }
               }
               bCone = bDome = bFlatCirc = bFlatRect = bNoDipole;
               bPole = true;
               pParent->m_ChassisData.ProcessWithObjects(ChassisData::CountSpheres, (WPARAM)&nSpheres, m_pCurrent->nChassisTyp);
               if (nSpheres == 0) bSphere = bNoDipole;
               if (m_pCurrent->IsSphere()) bSphere = bNoDipole;
            } break;
            default: break;
         }
      }
      EnableWindow(GetDlgItem(IDC_CH_EDT_DESCRIPTION), true);
   }
   else EnableWindow(GetDlgItem(IDC_CH_EDT_DESCRIPTION), false);
   
   EnableWindow(GetDlgItem(IDC_CH_CHK_POLARITY), bPole);

   EnableWindow(GetDlgItem(IDC_CH_R_M_FORM0), bCone);
   EnableWindow(GetDlgItem(IDC_CH_R_M_FORM1), bDome);
   EnableWindow(GetDlgItem(IDC_CH_R_M_FORM2), bFlatCirc);
   EnableWindow(GetDlgItem(IDC_CH_R_M_FORM3), bFlatRect);
   EnableWindow(GetDlgItem(IDC_CH_R_M_FORM4), bSphere);
   EnableWindow(GetDlgItem(IDC_CH_R_M_FORM5), bDipole);
}

int CBoxChassisDataPage::CountChassisTypes(void *p, WPARAM /*wP*/, LPARAM lP)
{
   ChassisData *pCD = (ChassisData*) p;
   int i, *pCTypes  = (int*) lP;
   if (pCD->IsHidden() || pCD->IsDummy()) return 1;

   for (i=0; i<CARABOX_CHASSISTYPES; i++)
   {
      if (gm_nChassisCountEdt[i][CCE_CHASSIS_TYPE] == pCD->nChassisTyp) break;
   }

   if (i < CARABOX_CHASSISTYPES) pCTypes[i]++;

   return 1;
}

int CBoxChassisDataPage::CheckChassisData(void *p, WPARAM wP, LPARAM lP)
{
   ASSERT(lP != NULL);
   CBoxChassisDataPage *pThis = (CBoxChassisDataPage*) lP;
   ChassisData *pCD = (ChassisData*) p;
   if (pCD->IsHidden() || pCD->IsDummy())  return 1;

   int nError  = pCD->CheckChassisParam();

   if (nError)
   {
      if (!(pThis->m_nFlags & NOERRORWINDOW))
      {
         pThis->CreateErrorWnd(nError, IDS_ERROR_WRONGINPUT, true);
         pThis->m_pCurrent = pCD;
         pThis->InitChassisData();
         pThis->SendDlgItemMessage(IDC_CH_LST_LS, LB_SETCURSEL, ((int*)wP)[0], 0);
      }
      ((int*)wP)[0] = -1;
      return 0;
   }
   ((int*)wP)[0]++;
   return 1;
}

void CBoxChassisDataPage::UpdateChassisList()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet *pC     = pParent->GetCabinet(BASE_CABINET);
   CEtsList *plstCD = &pParent->m_ChassisData;
   ChassisData *pCD;
   int  i, j, nCount;
   bool bListChanged = false;
   int *pnChassis = &pC->nCountBR;
   int  pnChassisAct[CARABOX_CHASSISTYPES];

//   long plParam[3] = {(long)GetDlgItem(IDC_CH_LST_LS), 0, 0};
   
   ZeroMemory(pnChassisAct, sizeof(int)*CARABOX_CHASSISTYPES);
   plstCD->ProcessWithObjects(CBoxChassisDataPage::CountChassisTypes, 0, (LPARAM)pnChassisAct);

   m_bListChanged = false;

   for (i=0; i<CARABOX_CHASSISTYPES; i++)
   {
      nCount = pnChassis[i] - pnChassisAct[i];
      if (nCount > 0) // zu wenig Chasis diesen Typs
      {
         bListChanged = true;
         for (j=0; j<nCount; j++)
         {
            pCD = new ChassisData;
            if (pCD)
            {
               pCD->nChassisTyp = gm_nChassisCountEdt[i][CCE_CHASSIS_TYPE];
               pParent->GetChassisTypeString(pCD, pCD->szDescrpt, 32);
               char szText[32];
               wsprintf(szText, "(%d)", pnChassisAct[i]+j+1);
               strcat(pCD->szDescrpt, szText);
               plstCD->ADDHead(pCD);
            }
            if (pParent->m_nLSType & DIPOLE_FLAT)
            {
               pCD->SetMembrTyp(CARABOX_MEMBRTYP_FLAT2);
               pCD->SetNoOfCoupledChassis(1, plstCD);
            }
         }
      }
      else if (nCount < 0)
      {
         bListChanged = true;
         nCount = -nCount;
         for (j=0; j<nCount; j++)
         {
            pCD = NULL;
            plstCD->ProcessWithObjects(ChassisData::FindChassisOfType, gm_nChassisCountEdt[i][CCE_CHASSIS_TYPE], (LPARAM)&pCD);
            if (pCD)
            {
               int i, nCoupled = pCD->GetNoOfCoupled();        // auch die gekoppelten löschen
               for (i=0; i<nCoupled; i++)
               {
                  plstCD->Delete(pCD->GetCoupled(i));
               }
               plstCD->Delete(pCD);
            }
         }
      }
   }
      
   CSortFunctions::SetSortDirection(false);
   plstCD->Sort(CSortFunctions::SortChassisType);

   if (bListChanged)
   {
      nCount = 0;
      plstCD->ProcessWithObjects(ChassisData::CountValidListChassis, (WPARAM)&nCount, 0);
      SendDlgItemMessage(IDC_CH_LST_LS, LB_SETCOUNT, nCount, 0);
      pParent->m_nFlagOk &= ~(CARABOX_CHASSIS_DATA|CARABOX_CHASSIS_POS);

      if (nCount > 0)
      {
         m_pCurrent = (ChassisData*) plstCD->GetFirst();
         InitChassisData();
         SendDlgItemMessage(IDC_CH_LST_LS, LB_SETCURSEL, 0, 0);
      }
   }

   if (!plstCD->InList(m_pCurrent))
   {
      m_pCurrent = NULL;
      InitChassisData();
   }
}

LRESULT CBoxChassisDataPage::OnDrawItem(DRAWITEMSTRUCT *pDI)
{
   if (pDI->CtlType == ODT_LISTBOX)
   {
      CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
      CEtsList    *plstCD  = &pParent->m_ChassisData;
      ChassisData * pCD    = (ChassisData*)plstCD->GetAt(pDI->itemID);
      if (pCD)
      {
         char szText[32]   = "";
         char szIndex[16]  = "";
         int  nLen         = 4;
         long lLastSelItem = ::GetWindowLong(pDI->hwndItem, GWL_USERDATA);
         RECT rcItem = pDI->rcItem;
         ::InflateRect(&rcItem, -1, -1);
         nLen = pParent->GetChassisTypeString(pCD, szText, 32);
         wsprintf(szIndex, "%2d:", pDI->itemID+1);

         ::SaveDC(pDI->hDC);
         if (pDI->itemState & ODS_SELECTED)
         {
            int nBkColor = COLOR_BTNSHADOW;
            if (pDI->itemState & ODS_FOCUS) nBkColor = COLOR_HIGHLIGHT;
            ::SetBkColor(pDI->hDC, ::GetSysColor(nBkColor));
            ::SetTextColor(pDI->hDC, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
            ::FillRect(pDI->hDC, &rcItem, ::GetSysColorBrush(nBkColor));
            ::SetWindowLong(pDI->hwndItem, GWL_USERDATA, pDI->itemID);
         }
         else
         {
            int nColorB = COLOR_INFOBK;
            int nColorT = COLOR_INFOTEXT;
            if (pCD->CheckChassisParam() == 0)
            {
               nColorB = COLOR_WINDOW;
               nColorT = COLOR_WINDOWTEXT;
            }
            if (pDI->itemState & ODS_FOCUS)
            {
               swap(nColorB, nColorT);
            }
            ::SetBkColor(pDI->hDC, ::GetSysColor(nColorB));
            ::SetTextColor(pDI->hDC, ::GetSysColor(nColorT));
            ::FillRect(pDI->hDC, &rcItem, ::GetSysColorBrush(nColorB));
         }

         if (pDI->itemAction & ODA_DRAWENTIRE)
         {
            POINT pt;
            ::MoveToEx(pDI->hDC, pDI->rcItem.left, pDI->rcItem.bottom-1, &pt);
            ::LineTo(pDI->hDC  ,  pDI->rcItem.right, pDI->rcItem.bottom-1);
            ::SetWindowLong(pDI->hwndItem, GWL_USERDATA, -1);
         }
         if (szIndex[0] != 0)
         {
            RECT rcIndex = rcItem;
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

void CBoxChassisDataPage::SetMembraneTypeFromIndex(ChassisData *pCD, int nIndex)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   if (pCD)
   {
      int nTyp, nOld = pCD->GetMembrTyp();

      switch (nIndex)
      {
         case 0:  pCD->SetMembrTyp(CARABOX_MEMBRTYP_CON);   break;
         case 1:  pCD->SetMembrTyp(CARABOX_MEMBRTYP_DOME);  break;
         case 2:  pCD->SetMembrTyp(CARABOX_MEMBRTYP_FLAT1); break;
         case 3:  pCD->SetMembrTyp(CARABOX_MEMBRTYP_FLAT2); break;
         case 4:  pCD->SetMembrTyp(CARABOX_MEMBRTYP_SPHERE);break;
         case 5:  pCD->SetMembrTyp(CARABOX_MEMBRTYP_DIPOLE);break;
         default: pCD->SetMembrTyp(0);                      break;
      }
      nTyp = pCD->GetMembrTyp();
      if (nOld != nTyp)
      {
         if (nTyp == CARABOX_MEMBRTYP_DIPOLE)                  // soll es ein Dipol werden ?
         {
            pCD->SetNoOfCoupledChassis(1, &pParent->m_ChassisData);// Rückseite erzeugen
         }
         else if (nOld == CARABOX_MEMBRTYP_DIPOLE)             // war es ein Dipol  ?
         {
            pCD->SetNoOfCoupledChassis(0, &pParent->m_ChassisData);// gekoppelte löschen
         }
         if (nOld == CARABOX_MEMBRTYP_SPHERE)                  // war es eine Kugel ?
         {
            if (pCD->m_ppCabinets)                             // mit Halterung
            {
               CCabinet* pCbase = pParent->GetCabinet(BASE_CABINET);
               pCbase->m_Cabinets.Delete(pCD->m_ppCabinets[0]);// Halterung entfernen
               Free(pCD->m_ppCabinets);
               pCD->m_ppCabinets = NULL;
               pParent->m_nFlagChanged |= UPDATE_CABINET_LIST;
            }
         }
         CheckCurrentChassisPosition();
      }
   }
}

void CBoxChassisDataPage::CheckCurrentChassisPosition()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   ASSERT(pParent->m_ChassisData.InList(m_pCurrent));
//   int nCabWall = m_pCurrent->nCabWall;
   m_pCurrent->AdaptCoupledSize();
   if (pParent->m_dVolume < 0.001) return;
   CCabinet*pC = pParent->GetCabinet(m_pCurrent->GetCabinet());
   if (m_pCurrent->GetWall())
   {
      pC->m_dwFlags |= 1 << m_pCurrent->GetWall();
      CBoxPropertySheet::CheckChassisPos(m_pCurrent, (WPARAM)pParent, (LPARAM)pC);
      pC->m_dwFlags &= ~CHECK_ALL_WALLS;
   }
   pParent->SendMsgTo3DDlg(WM_COMMAND, MAKELONG(IDC_BOX_VIEW, LIST_CHASSIS), (LPARAM)m_hWnd);
}

void CBoxChassisDataPage::CheckWizardButtons()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   m_nFlags |= NOERRORWINDOW;
   OnWizardNext(NULL, false);
   m_nFlags &= ~NOERRORWINDOW;
   pParent->SetWizardButtons(true, (pParent->m_nFlagOk & CARABOX_CHASSIS_DATA)? true : false);
}
