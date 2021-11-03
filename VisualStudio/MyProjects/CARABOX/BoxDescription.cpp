/********************************************************************/
/* Funktionen der Klasse CBoxDescriptionPage                           */
/********************************************************************/

#include "BoxDescription.h"
#include "BoxPropertySheet.h"
#include "resource.h"
#include "AfxRes.h"
#include "Debug.h"
#include "CEtsFileDlg.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

extern char      g_szBoxPath[_MAX_PATH];


CBoxDescriptionPage::CBoxDescriptionPage()
{
}

bool CBoxDescriptionPage::OnInitDialog(HWND hWnd)
{
   if (CEtsPropertyPage::OnInitDialog(hWnd))
   {
      ::EnumChildWindows(hWnd, SetHelpIDs, (LPARAM)hWnd);
      HMENU hMenu = ::GetSystemMenu(::GetParent(hWnd), false);
      if (hMenu)
      {
         char text[64];
         ::InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION|MF_SEPARATOR, 0, NULL);
         ::LoadString(m_hInstance, IDS_STACK_CHILD_WNDS, text, 64);
         ::InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION, IDS_STACK_CHILD_WNDS, text);
         ::LoadString(m_hInstance, IDS_STACK_CHILDV_WNDS, text, 64);
         ::InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION, IDS_STACK_CHILDV_WNDS, text);
         ::LoadString(m_hInstance, IDS_STACK_CHILDH_WNDS, text, 64);
         ::InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION, IDS_STACK_CHILDH_WNDS, text);
         ::LoadString(m_hInstance, IDS_CLOSE_CHILD_WNDS, text, 64);
         ::InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION, IDS_CLOSE_CHILD_WNDS, text);
      }
      HWND hWndFileName = ::GetDlgItem(hWnd, IDC_BD_EDT_DESCRIPTION);
      if (hWndFileName)
      {
         SetWindowLong(hWndFileName, GWL_USERDATA, SetWindowLong(hWndFileName, GWL_WNDPROC, (long)CEtsFileDlg::EditFileNameProc));
      }

      return true;
   }
   return false;
}

BOOL CALLBACK CBoxDescriptionPage::SetHelpIDs(HWND hwnd, LPARAM /*lParam*/)
{
   switch (::GetWindowLong(hwnd, GWL_ID))
   {
      case IDC_BD_EDT_DESCRIPTION: case IDC_BD_EDT_FILENAME: case IDC_BD_EDT_MANUFACTURER:
         ::SetWindowContextHelpId(hwnd, IDC_BD_TXT_FILENAME);
         break;
      case IDC_BD_TXT_COMMENT:      ::SetWindowContextHelpId(hwnd, IDC_BD_EDT_COMMENT); break;
      case IDC_BD_EDT_FILE_VERSION: ::SetWindowContextHelpId(hwnd, IDC_BD_TXT_FILE_VERSION); break;
      case IDC_BD_TXT_BOXTYPE:
         ::SetWindowContextHelpId(hwnd, IDC_BD_COMBO_BOXTYPE);
         break;
      case IDC_BD_TXT_DESCRIPTION:   case IDC_BD_TXT_MANUFACTURER: case IDC_BD_GRP_DESCRIPTION:
         ::SetWindowContextHelpId(hwnd, IDC_BD_TXT_FILENAME);
         break;
   }
   return 1;
}

int CBoxDescriptionPage::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BoxText * pBT = &pParent->m_BoxText;
   bool bChanged = false;
   if (nNotifyCode == BN_CLICKED)
   {
      switch (nID)
      {
         case IDC_BD_BTN_NEW:
            pParent->OnBtnClckNew();
            InitDlgCtrls();
            bChanged = true;
            break;
         case IDC_BD_BTN_SAVE_AS:
            pParent->OnBtnClckSaveAs();
            InitDlgCtrls();
            return 1;
         case IDC_BD_BTN_OPEN:
            pParent->OnBtnClckOpen();
            InitDlgCtrls();
            bChanged = true;
            break;
         case IDC_BD_BTN_UPLOAD:
            pParent->OnBtnClckUpLoad();
            InitDlgCtrls();
            bChanged = true;
            break;
         case IDC_BD_BTN_DOWNLOAD:
            pParent->OnBtnClckDownLoad();
            return 1;
      }
   }
   else if (nNotifyCode == EN_KILLFOCUS)
   {
      switch (nID)
      {
         case IDC_BD_EDT_COMMENT:      GetDlgItemText(nID, pBT->szComment, &bChanged, 255);             break;
//         case IDC_BD_EDT_DESCRIPTION:  GetDlgItemText(nID, pBT->szDescrpt, &bChanged, BOXTEXT_LEN-1);   break;
         case IDC_BD_EDT_MANUFACTURER: GetDlgItemText(nID, pBT->szTradeMark, &bChanged, BOXTEXT_LEN-1); break;
      }
   }
   else if ((nNotifyCode == EN_CHANGE) && (nID == IDC_BD_EDT_DESCRIPTION))
   {
      GetDlgItemText(nID, pBT->szDescrpt, &bChanged, BOXTEXT_LEN-1);
   }
   else if (nNotifyCode == CBN_SELCHANGE)
   {
      switch (nID)
      {
         case IDC_BD_COMBO_BOXTYPE:
         {
            int nType = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
            int nLSType;

            switch (nType)
            {
               case   0: nLSType = STAND_ALONE;             break;// Standard LS-Typ
               case   1: nLSType = STAND_ALONE|DIPOLE_FLAT; break;// Dipolflachmembran
               case   2: nLSType = STAND_ALONE|DIPOLE_FLAT_H;break;// Dipolflachmembran+(hybrid)
               case   3: nLSType = ADD_ON1;                 break;// Subwoofer
               case   4: nLSType = ADD_ON3;                 break;// Zusatz-Hochtöner
               default : nLSType = 0;                       break;
            }
            bChanged = false;
            if (pParent->m_nLSType != nLSType)
            {
               if ((pParent->m_nFlagOk & CARABOX_ALL_DATA)||(pParent->m_nFlagChanged & CARABOX_BASE_DATA))
               {
                  if (MessageBox(IDS_CHANGE_LSTYPE, IDS_PROPSHCAPTION, MB_YESNO|MB_ICONEXCLAMATION) == IDYES)
                  {
                     pParent->OnChangeLSType();
                     bChanged = true;
                  }
                  else
                  {
                     InitDlgCtrls();
                  }
               }
               else  bChanged = true;
            }

            if (bChanged)
            {
               pParent->m_nLSType = nLSType;
               switch (nType)
               {
                  case   0:                                       // Standard
                     pParent->m_BasicData.nXoverTyp = CARABOX_CO_2W;// Vorgabe 2-Weg
                     pParent->m_BasicData.nBoxTyp   = CARABOX_BOXTYP_BR;// Vorgabe BR
                     break;
                  case   1:                                       // Dipolflachmembran
                     pParent->m_BasicData.nXoverTyp = CARABOX_CO_1W;// Nur 1, 2, 3-Wege
                     pParent->m_BasicData.nBoxTyp   = CARABOX_BOXTYP_CLOS;// nur geschlossen
                     break;
                  case   2:                                       // Dipolflachmembran (Hybrid)
                     pParent->m_BasicData.nXoverTyp = CARABOX_CO_2W;// Nur 1, 2, 3-Wege
                     pParent->m_BasicData.nBoxTyp   = CARABOX_BOXTYP_BR;// Vorgabe BR
                     break;
                  case   3:                                       // Subwoofer
                     pParent->m_BasicData.nXoverTyp = CARABOX_CO_1W;// Nur 1-Weg
                     pParent->m_BasicData.nBoxTyp   = CARABOX_BOXTYP_BR;// Vorgabe BR
                     break;
                  case   4:                                       // Zusatz-Hochtöner
                     pParent->m_BasicData.nXoverTyp = CARABOX_CO_1W;// Nur 1-Weg
                     pParent->m_BasicData.nBoxTyp   = CARABOX_BOXTYP_CLOS;
                     break;
               }
               pParent->m_BasicData.dLowLimitFreq  = CBoxPropertySheet::gm_dDefaultTransmisionRanges[nType][0];
               pParent->m_BasicData.dHighLimitFreq = CBoxPropertySheet::gm_dDefaultTransmisionRanges[nType][1];
            }
         } break;
         default: break;
      }
   }
   if (bChanged)
   {
      m_nFlags |= NOERRORWINDOW;
      OnWizardNext(NULL, false);
      m_nFlags &= ~NOERRORWINDOW;
      pParent->SetWizardButtons(false, (pParent->m_nFlagOk & CARABOX_DESCRIPTION)? true : false);
   }
   return CEtsPropertyPage::OnCommand(nID , nNotifyCode, hwndControl);
}

int  CBoxDescriptionPage::OnHelp(PSHNOTIFY*pN)
{
   ::WinHelp(m_hWnd, "CARACAD.hlp", HELP_CONTEXT, 1991);
   return CEtsPropertyPage::OnHelp(pN);
}

int  CBoxDescriptionPage::OnReset(PSHNOTIFY* pN)
{
   return CEtsPropertyPage::OnReset(pN);
}

int  CBoxDescriptionPage::OnSetActive(PSHNOTIFY* pN, bool bAccept)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   if (g_szBoxPath[0])
   {
      pParent->LoadBoxData(g_szBoxPath);
      g_szBoxPath[0] = 0;
   }
   InitDlgCtrls();
   pParent->SetWizardButtons(false, (pParent->m_nFlagOk & CARABOX_DESCRIPTION)? true : false);
   return CEtsPropertyPage::OnSetActive(pN, bAccept);
}

int  CBoxDescriptionPage::OnWizardBack(PSHNOTIFY* /*pN*/, bool /*bAccept*/)
{
//   m_nFlags |= NOERRORWINDOW;
//   OnWizardNext(pN, bAccept);
//   m_nFlags &= ~NOERRORWINDOW;
//   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, 0);
   return 1; 
}

int  CBoxDescriptionPage::OnWizardFinish(PSHNOTIFY*pN, bool bFinish)
{
   bFinish = false;
   OnWizardNext(pN, true);
   if (::GetWindowLong(m_hWnd, DWL_MSGRESULT) == 0)
   {
      CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
      bFinish = pParent->OnWizardFinish();
   }
   InitDlgCtrls();
   return CEtsPropertyPage::OnWizardFinish(pN, bFinish);
}

int  CBoxDescriptionPage::OnWizardNext(PSHNOTIFY*pN, bool bAccept)
{
   REPORT("CBoxDescriptionPage::OnWizardNext");
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   BoxText           *pBT     = &pParent->m_BoxText;
   bAccept = false;

   if      (pBT->szDescrpt[0]   == 0) CreateErrorWnd(IDC_BD_EDT_DESCRIPTION , IDS_ERROR_WRONGINPUT, true);
   else if (pBT->szTradeMark[0] == 0) CreateErrorWnd(IDC_BD_EDT_MANUFACTURER, IDS_ERROR_WRONGINPUT, true);
   else if (pParent->m_nLSType  ==-1) CreateErrorWnd(IDC_BD_COMBO_BOXTYPE   , IDS_ERROR_WRONGINPUT, true);
   else bAccept= true;
   if (bAccept) pParent->m_nFlagOk |=  CARABOX_DESCRIPTION;
   else         pParent->m_nFlagOk &= ~CARABOX_DESCRIPTION;

   if (pN)
   {
      pN->lParam = bAccept;
      return CEtsPropertyPage::OnWizardNext(pN, bAccept);
   }
   return 0;
}

void CBoxDescriptionPage::InitDlgCtrls()
{
   CBoxPropertySheet*pParent = (CBoxPropertySheet*)m_pParent;
   BoxText * pBT = &pParent->m_BoxText;
   SetDlgItemText(IDC_BD_EDT_DESCRIPTION , pBT->szDescrpt  , BOXTEXT_LEN-1);
   SetDlgItemText(IDC_BD_EDT_MANUFACTURER, pBT->szTradeMark, BOXTEXT_LEN-1);
   SetDlgItemText(IDC_BD_EDT_COMMENT     , pBT->szComment  , 255);
   if (pBT->szFilename[0] != 0)
   {
      int nLen = strlen(pBT->szFilename);
      strcat(pBT->szFilename, ".BOX");
      SetDlgItemText(IDC_BD_EDT_FILENAME, pBT->szFilename);
      pBT->szFilename[nLen] = 0;
   }
   else SetDlgItemText(IDC_BD_EDT_FILENAME, "");

   int nIndex, nLSType = ((CBoxPropertySheet*)m_pParent)->m_nLSType;
   switch (LOWORD(nLSType))
   {
      case STAND_ALONE: nIndex =  0; break;
      case ADD_ON1:     nIndex =  3; break;
      case ADD_ON3:     nIndex =  4; break;
      default :         nIndex = -1; break;
   }
   if (nLSType & DIPOLE_FLAT)
   {
      ASSERT(nIndex == 0);
      nIndex = 1;
   }
   if (nLSType & DIPOLE_FLAT_H)
   {
      ASSERT(nIndex == 0);
      nIndex = 2;
   }
   SendDlgItemMessage(IDC_BD_COMBO_BOXTYPE, CB_SETCURSEL, nIndex, 0);
   pParent->SetVersionText(m_hWnd, IDC_BD_EDT_FILE_VERSION, pParent->m_nFileVersion);
}

