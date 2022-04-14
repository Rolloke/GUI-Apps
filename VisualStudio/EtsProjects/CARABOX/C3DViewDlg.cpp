/********************************************************************/
/* Funktionen der Klasse C3DViewDlg                                */
/********************************************************************/
#include "C3DViewDlg.h"
#include "resource.h"
#include <COMMCTRL.H>
#include "BoxPropertySheet.h"
#include "CEtsPropertyPage.h"
#include "Debug.h"
#include "CEtsDiv.h"
#include "Cara3DTr.h"
#include "CScaleDlg.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define INVALID_VALUE 0xffffffff

#define WM_TAB_CTRL   (WM_APP+1)

extern int g_nUserMode;              // Registryparameter

C3DViewDlg::C3DViewDlg()
{
   Constructor();
}

C3DViewDlg::~C3DViewDlg()
{
//   m_GLView.Destructor();          // nur, wenn dies die letzte Instanz ist
}

C3DViewDlg::C3DViewDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

void C3DViewDlg::Constructor()
{
   m_pPropertySheet = NULL;
   m_nBmpWnd[0]     = 0;
   m_nBmpWnd[1]     = 0;
   m_nBmpWnd[2]     = 0;
   m_dFactor[0]     = 0.1;
   m_dFactor[1]     = 0.1;
   m_dFactor[2]     = 0.1;
   m_nTxtWnd[0]     = 0;
   m_nTxtWnd[1]     = 0;
   m_nTxtWnd[2]     = 0;
   m_nCheckBtn[0]   = 0;
   m_nCheckBtn[1]   = 0;
   m_nCheckBtn[2]   = 0;
   m_nAccuracy1     = 2;
   m_nAccuracy2     = 2;
   m_hwndCabinetPage = NULL;
   m_bSetCursor     = true;
}

bool C3DViewDlg::OnInitDialog(HWND hWnd) 
{
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      if (m_pPropertySheet == NULL)
      {
         ASSERT(false);
         return false;
      }

      m_GLView.SetPropertySheet(m_pPropertySheet);
      HWND hwndBoxView = GetDlgItem(IDC_BOX_VIEW);
      if (hwndBoxView)
      {
         RECT rcView;
         GetClientRect(hwndBoxView, &rcView);
         m_GLView.Create("3D Boxview", 0, &rcView, hwndBoxView);
      }

      SetNumeric(IDC_3D_EDT_TRAN_X, true, true);
      SetNumeric(IDC_3D_EDT_TRAN_Y, true, true);
      SetNumeric(IDC_3D_EDT_TRAN_Z, true, true);

      SendDlgItemMessage(IDC_3D_EDT_TRAN_X, EM_LIMITTEXT, 31, 0);
      SendDlgItemMessage(IDC_3D_EDT_TRAN_Y, EM_LIMITTEXT, 31, 0);
      SendDlgItemMessage(IDC_3D_EDT_TRAN_Z, EM_LIMITTEXT, 31, 0);
      SendDlgItemMessage(IDC_3D_EDT_CAB_NAME,EM_LIMITTEXT, CAB_NAME_LEN, 0);

      ::SetWindowContextHelpId(GetDlgItem(IDC_3D_EDT_TRAN_Y), IDC_3D_EDT_TRAN_X);
      ::SetWindowContextHelpId(GetDlgItem(IDC_3D_EDT_TRAN_Z), IDC_3D_EDT_TRAN_X);

      ::SetWindowContextHelpId(GetDlgItem(IDC_3D_CK_FRONT_WALL) , IDC_3D_CK_TOP_WALL);
      ::SetWindowContextHelpId(GetDlgItem(IDC_3D_CK_BACK_WALL)  , IDC_3D_CK_TOP_WALL);
      ::SetWindowContextHelpId(GetDlgItem(IDC_3D_CK_LEFT_WALL)  , IDC_3D_CK_TOP_WALL);
      ::SetWindowContextHelpId(GetDlgItem(IDC_3D_CK_RIGHT_WALL) , IDC_3D_CK_TOP_WALL);
      ::SetWindowContextHelpId(GetDlgItem(IDC_3D_CK_BOTTOM_WALL), IDC_3D_CK_TOP_WALL);

      if (g_nUserMode & USERMODE_WALLS)
      {
         ::ShowWindow(GetDlgItem(IDC_3D_GRP_VISIBLE_WALLS), SW_SHOW);
         for (int i=0; i<NO_OF_WALLS; i++)
            ::ShowWindow(GetDlgItem(IDC_3D_CK_TOP_WALL+i), SW_SHOW);
      }
      else
      {
         ::ShowWindow(GetDlgItem(IDC_3D_GRP_VISIBLE_WALLS), SW_HIDE);
         ::ShowWindow(GetDlgItem(IDC_3D_TXT_STATUS), SW_SHOW);
      }

      HWND hwnd;
      long lResult;
      int  i;
      for (i=IDC_3D_BTN_TRAN_X; i<=IDC_3D_BTN_TRAN_Z; i++)
      {
         hwnd = GetDlgItem(i);
         lResult = ::SetWindowLong(hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)ButtonSubClass));
         ASSERT(lResult==0);
      }

      for (i=IDC_3D_R_TRANS0; i<=IDC_3D_R_TRANS6; i++)
      {
         hwnd = GetDlgItem(i);
         lResult = ::SetWindowLong(hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)ButtonSubClass));
         ASSERT(lResult==0);
      }

      for (i=IDC_3D_EDT_TRAN_X; i<=IDC_3D_EDT_TRAN_Z; i++)
      {
         hwnd = GetDlgItem(i);
         lResult = ::SetWindowLong(hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)EditSubClass));
         ASSERT(lResult==0);
      }

      hwnd = GetDlgItem(IDC_3D_EDT_CAB_NAME);
      lResult = ::SetWindowLong(hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)EditSubClass));
      ASSERT(lResult==0);

      hwnd = GetDlgItem(IDC_3D_BTN_UPDATE);
      lResult = ::SetWindowLong(hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)ButtonSubClass));
      ASSERT(lResult==0);
      hwnd = GetDlgItem(IDC_3D_BTN_CREATE_CABINET);
      lResult = ::SetWindowLong(hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)ButtonSubClass));
      ASSERT(lResult==0);
      hwnd = GetDlgItem(IDC_3D_CK_CAN_CONTAIN_CHASSIS);
      lResult = ::SetWindowLong(hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)ButtonSubClass));
      ASSERT(lResult==0);
      hwnd = GetDlgItem(IDC_3D_LST_CAB);  // Achtung ButtonClass!
      lResult = ::SetWindowLong(hwnd, GWL_USERDATA, ::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)ButtonSubClass));
      ASSERT(lResult==0);
      
      CheckDlgRadioBtn(IDC_3D_R_TRANS0, m_GLView.m_nTransMode);
      UpdateTransMode(true);
      OnCabLstContentChanged(::GetDlgItem(m_hwndCabinetPage, IDC_CD_LST_CHASSIS_CAB));
      OnChangeCabinet(IDC_CD_LST_CHASSIS_CAB, 0, m_hWnd);
      int nRange[2] = {IDM_VIEW_DETAIL_VERYLOW, IDM_VIEW_DETAIL_VERYHIGH};
      SetMenuItem(IDM_VIEW_DETAIL_VERYLOW+m_GLView.m_nResolution-1, MFT_RADIOCHECK, (UINT) &nRange[0]);
   }
   return true;
}

void C3DViewDlg::OnEndDialog(int nResult)
{
   CEtsDialog::OnEndDialog(nResult);
}

int C3DViewDlg::OnOk(WORD)
{
   return IDOK;
}

int C3DViewDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   switch(nID)
   {
/**** View Parameter *****************************************************************************/
      case IDM_VIEW_X_SCALE: OnClickXScale(); break;
      case IDM_VIEW_Y_SCALE: OnClickYScale(); break;
      case IDM_VIEW_Z_SCALE: OnClickZScale(); break;
      case IDM_VIEW_PHI_SCALE: OnClickPhiScale(); break;
      case IDM_VIEW_DETAIL_VERYLOW: case IDM_VIEW_DETAIL_LOW: case IDM_VIEW_DETAIL_MEAN: case IDM_VIEW_DETAIL_HIGH: case IDM_VIEW_DETAIL_VERYHIGH:
      {
         int nOldRes = m_GLView.m_nResolution;
         m_GLView.m_nResolution = nID - IDM_VIEW_DETAIL_VERYLOW + 1;
         if (nOldRes != m_GLView.m_nResolution)
         {
            int nRange[2] = {IDM_VIEW_DETAIL_VERYLOW, IDM_VIEW_DETAIL_VERYHIGH};
            SetMenuItem(IDM_VIEW_DETAIL_VERYLOW+m_GLView.m_nResolution-1, MFT_RADIOCHECK, (UINT) &nRange[0]);
            m_GLView.InvalidateAllCabinets();
            m_GLView.InvalidateList(LIST_CHASSIS);
         }
      }break;
      case IDM_VIEW_BLEND1: case IDM_VIEW_BLEND2: case IDM_VIEW_BLEND3: case IDM_VIEW_BLEND4:
      case IDM_VIEW_BLEND5: case IDM_VIEW_BLEND6: case IDM_VIEW_BLEND7:
      {
         int nRange[2] = {IDM_VIEW_BLEND1, IDM_VIEW_BLEND7};
         SetMenuItem(nID, MFT_RADIOCHECK, (UINT)&nRange); 
         m_GLView.m_nAlphaBlend = nID - IDM_VIEW_BLEND1 + 1;
         m_GLView.m_nAlphaBlend |= ALPHA_BLENDING;
         m_GLView.m_bChanged[0] = true;
         UpdateCtrlStates();
         m_GLView.InvalidateTransMode(true, TRANS_SETTINGS);
      }break;
      case IDM_VIEW_PARALLEL: case IDM_VIEW_PERSPECTIVE: OnClickPerspective(); break;
      case IDM_VIEW_3D_COPY:
      {
         HGLOBAL hGl = NULL;
         HWND hwnd = m_GLView.GetHWND();
         ::SendMessage(hwnd, WM_COPY_FRAME_BUFFER, COPY_TO_HGLOBAL, (LPARAM)&hGl);
         if (hGl && OpenClipboard(hwnd) && EmptyClipboard())
         {
            if (SetClipboardData(CF_DIB, hGl) == NULL)
               GlobalFree(hGl);
            CloseClipboard();
         }
      }break;
      case IDM_VIEW_BACK_SIDE:
         m_GLView.m_bShowBackSide = !m_GLView.m_bShowBackSide;
         UpdateCtrlStates();
         break;
/**** Set Parameter ******************************************************************************/
      case IDM_SET_X_SCALE: OnDblClickXScale(); break;
      case IDM_SET_Y_SCALE: OnDblClickYScale(); break;
      case IDM_SET_Z_SCALE: OnDblClickZScale(); break;
      case IDM_SET_PHI_SCALE: OnDblClickPhiScale(); break;
      case IDM_SET_LIGHT_COLOR: OnClickLightColor(); break;
      case IDM_SET_AMBIENT_COLOR: OnClickAmbientColor(); break;
      case IDM_SET_SPECULAR_COLOR: OnClickSpecularColor(); break;
      case IDC_3D_PIC_SET_ALPHA_BLEND: OnClickAlphaBlend(); break;
      case IDM_SET_STANDARD_LIGHTPARAM:
      {
         double dMaxDim = m_pPropertySheet->m_Cabinet.GetMaxDimension();
         m_GLView.m_dPhiLight        = 20.0;
         m_GLView.m_dThetaLight      = 30.0;
         m_GLView.m_dDistanceLight   = dMaxDim*0.01;

         m_GLView.m_dSpotCutExponent = 20;
         m_GLView.m_dSpotCutOff      = 60;

         m_GLView.m_nAlphaBlend      =  2;
         m_GLView.m_dAlphaBlend      = 50;

         m_GLView.m_dLightIntens[0] = 70;
         m_GLView.m_dLightIntens[1] = 70;
         m_GLView.m_dLightIntens[2] = 70;
         m_GLView.m_AmbientColor[0] = 0.2f;
         m_GLView.m_AmbientColor[1] = 0.2f;
         m_GLView.m_AmbientColor[2] = 0.2f;

         m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
         if (TRANS_TURNLIGHT == m_GLView.m_nTransMode) UpdateTransMode();
         else m_GLView.InvalidateTransMode(true, TRANS_TURNLIGHT);
         
         m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
         if (TRANS_LIGHT_SPOT == m_GLView.m_nTransMode) UpdateTransMode();
         else m_GLView.InvalidateTransMode(true, TRANS_LIGHT_SPOT);

         m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
         if (TRANS_LIGHT_INTENS == m_GLView.m_nTransMode) UpdateTransMode();
         else m_GLView.InvalidateTransMode(true, TRANS_LIGHT_INTENS);
      }break;
      case IDC_3D_PIC_SET_BKCOLOR:
      {
         COLORREF cBkColor = m_GLView.GetBkColor();
         char *pszHeading = LoadString(IDS_BKCOLOR);
         if (ChooseColorT(pszHeading, cBkColor, m_hWnd, m_hInstance) == IDOK)
         {
            m_GLView.SetBkColor(cBkColor);
         }
         free((void*)pszHeading);
      } break;
      case IDC_3D_PIC_SET_BOX_COLOR:
      {
         COLORREF cColor = m_GLView.m_cBox;
         char *pszHeading = LoadString(IDS_BOX_COLOR);
         if (ChooseColorT(pszHeading, cColor, m_hWnd, m_hInstance) == IDOK)
         {
            m_GLView.m_cBox = cColor;
            m_GLView.InvalidateAllCabinets();
         }
         free((void*)pszHeading);
      } break;
      case IDC_3D_PIC_SET_CHASSIS_COLOR:
      {
         COLORREF cColor = m_GLView.m_cChassis;
         char *pszHeading = LoadString(IDS_CHASSIS_COLOR);
         if (ChooseColorT(pszHeading, cColor, m_hWnd, m_hInstance) == IDOK)
         {
            m_GLView.m_cChassis = cColor;
            m_GLView.InvalidateList(LIST_CHASSIS);
         }
         free((void*)pszHeading);
      } break;
      case IDC_3D_PIC_SET_SCALE_COLOR:
      {
         COLORREF cColor = m_GLView.m_cScale;
         char *pszHeading = LoadString(IDS_SELECTION_COLOR);
         if (ChooseColorT(pszHeading, cColor, m_hWnd, m_hInstance) == IDOK)
         {
            m_GLView.m_cScale = cColor;
            m_GLView.InvalidateList(LIST_SCALE);
         }
         free((void*)pszHeading);
      } break;
      case IDM_SET_SPECULAR_MAT:
      {
         char *pszHeading = LoadString(IDS_SPEC_MAT_COLOR);
         if (ChooseColorT(pszHeading, m_GLView.m_cSpecular, m_hWnd, m_hInstance) == IDOK)
         {
            m_GLView.InvalidateCommand(PRE_CMD);
         }
         free((void*)pszHeading);
      } break;
      case IDM_SET_INVERSE_DRAW_DIR:
         if (m_GLView.m_nAlphaBlend & ALPHA_DRAW_DIR) m_GLView.m_nAlphaBlend &= ~ALPHA_DRAW_DIR;
         else                                         m_GLView.m_nAlphaBlend |=  ALPHA_DRAW_DIR;
         m_GLView.InvalidateAllCabinets();
         break;
/**** Dlg Ctrl Messages **************************************************************************/
      case IDC_CD_LST_CHASSIS_CAB:                             // Ändern des Selektierten Gehäuses
      {                                                        // diese Message kann auch von anderen Dialogen kommen
         OnChangeCabinet(nID ,nNotifyCode, hwndControl);       // 
      } break;
      case IDC_BOX_VIEW:                                       // Command für den 3D View,
      {
         m_GLView.InvalidateList(nNotifyCode);                 // um eine Liste neu zu Rendern
         nNotifyCode = 0;
      }break;
      case IDC_3D_LST_CAB:                                     // Command an die Gehäuseliste
      if (nNotifyCode == CBN_SELCHANGE)                        // Item gewählt
      {
         int nItem = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
         if (nItem != m_pPropertySheet->m_nCurrentCab)         // geändert ?
         {
            if (m_hwndCabinetPage)
            {                                                  // Auswahl an die Cabinet PropertySheet übermitteln
               ::SendDlgItemMessage(m_hwndCabinetPage, IDC_CD_LST_CHASSIS_CAB, CB_SETCURSEL, nItem, 0);
               ::SendMessage(m_hwndCabinetPage, WM_COMMAND, MAKELONG(IDC_CD_LST_CHASSIS_CAB, CBN_SELCHANGE), (LPARAM)m_hWnd); 
            }
         }
      }
      else if (nNotifyCode == CBN_CONTENT_CHANGED)             // User Notify Code !!!
      {                                                        // hwndControl ist die Master-Liste in der Cabinet PropertySheet
         OnCabLstContentChanged(hwndControl);
      }break;
      case IDC_GRP_CABINET_PARAM: InitCabinetData(); break;
/**** Dlg Ctrl Messages geordnet nach Notify Code ************************************************/
      default:
      if ((nNotifyCode == BN_LIST_CHANGED) && (nID ==IDC_3D_BTN_UPDATE))
      {
         InitCabinetLists();
      }
      else if (nNotifyCode == EN_KILLFOCUS)
      {
         if ((nID == IDC_3D_EDT_CAB_NAME) && 
             !(::GetWindowLong(GetDlgItem(nID), GWL_STYLE) & ES_READONLY))
         {                                                     // Name des Gehäuses geändert
            bool bChanged = false;
            CCabinet *pC = m_pPropertySheet->GetCurrentCab();
            GetDlgItemText(nID, pC->m_szName, &bChanged, CAB_NAME_LEN);
            if (bChanged)
            {
               if (m_hwndCabinetPage)                          // Master-Liste updaten
               {
                  ::SendDlgItemMessage(m_hwndCabinetPage, IDC_CD_LST_CHASSIS_CAB, CB_INSERTSTRING, m_pPropertySheet->m_nCurrentCab, (LPARAM)pC->m_szName);
                  ::SendDlgItemMessage(m_hwndCabinetPage, IDC_CD_LST_CHASSIS_CAB, CB_DELETESTRING, m_pPropertySheet->m_nCurrentCab+1, NULL);
                  ::SendDlgItemMessage(m_hwndCabinetPage, IDC_CD_LST_CHASSIS_CAB, CB_SETCURSEL   , m_pPropertySheet->m_nCurrentCab, NULL);
                                                               // Slave-Liste updaten
                  ::SendMessage(m_hWnd, WM_COMMAND, MAKELONG(IDC_3D_LST_CAB, CBN_CONTENT_CHANGED), (LPARAM)::GetDlgItem(m_hwndCabinetPage, IDC_CD_LST_CHASSIS_CAB));
               }
            }
         }
         else if ((nID == IDC_3D_EDT_TRAN_X)||(nID == IDC_3D_EDT_TRAN_Y)||(nID == IDC_3D_EDT_TRAN_Z))
         {
            bool bChanged = false;
            double *pdVar = GetTransModeVar();
            if (pdVar)
            {
               if (GetDlgItemDouble(nID, pdVar[nID-IDC_3D_EDT_TRAN_X], pdVar[nID-IDC_3D_EDT_TRAN_X], &bChanged))
               {
                  if (bChanged)
                  {
                     m_GLView.m_bChanged[nID-IDC_3D_EDT_TRAN_X] = true;
//                     m_GLView.InvalidateTransMode();
                     UpdateTransMode(false, -2);
                     NotifyCabPageSegmentMoved();
                  }
               }
            }
         }
      }
      else if (nNotifyCode == BN_CLICKED)
      {
         switch (nID)
         {
            case IDC_3D_R_TRANS0: case IDC_3D_R_TRANS1: case IDC_3D_R_TRANS2: case IDC_3D_R_TRANS3:
            case IDC_3D_R_TRANS4: case IDC_3D_R_TRANS5: case IDC_3D_R_TRANS6: case IDC_3D_R_TRANS7:
            case IDC_3D_R_TRANS8:
            {
               int nOld = m_GLView.m_nTransMode;
               m_GLView.m_nTransMode = GetDlgRadioBtn(IDC_3D_R_TRANS0);
               if (nOld != m_GLView.m_nTransMode)
               {
                  UpdateTransMode(true);
               }
            } break;
            case IDC_3D_BTN_UPDATE:     UpdateVolume(); break;
            case IDC_3D_BTN_TRAN_X:     OnClickXbutton(hwndControl);break;
            case IDC_3D_BTN_TRAN_Y:     OnClickYbutton(hwndControl);break;
            case IDC_3D_BTN_TRAN_Z:     OnClickZbutton(hwndControl);break;
            case IDC_3D_CK_TOP_WALL:    OnCheckWall(CARABOX_CABWALL_TOP);    break;
            case IDC_3D_CK_FRONT_WALL:  OnCheckWall(CARABOX_CABWALL_FRONT);  break;
            case IDC_3D_CK_BACK_WALL:   OnCheckWall(CARABOX_CABWALL_BACK);   break;
            case IDC_3D_CK_LEFT_WALL:   OnCheckWall(CARABOX_CABWALL_LEFT);   break;
            case IDC_3D_CK_RIGHT_WALL:  OnCheckWall(CARABOX_CABWALL_RIGHT);  break;
            case IDC_3D_CK_BOTTOM_WALL: OnCheckWall(CARABOX_CABWALL_BOTTOM); break;
            case IDC_3D_BTN_CREATE_CABINET: OnNewCabinet(); break;
            case IDC_3D_BTN_DELETE_CABINET: OnDeleteCabinet(); break;
            case IDC_3D_CK_CAN_CONTAIN_CHASSIS: OnCheckCanContainChassis(); break;
         }
      }
      else if (nNotifyCode == BN_DBLCLK)
      {
         switch (nID)
         {
            case IDC_3D_BTN_TRAN_X: OnDblClickXbutton(hwndControl);break;
            case IDC_3D_BTN_TRAN_Y: OnDblClickYbutton(hwndControl);break;
            case IDC_3D_BTN_TRAN_Z: OnDblClickZbutton(hwndControl);break;
         }
      }

      break;
   }

   return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
}

int C3DViewDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch (nMsg)
   {
      case WM_MOUSEMOVE:
      {
         POINTS pt = MAKEPOINTS(lParam);
         RECT   rcClient;

         if ((wParam & (MK_LBUTTON|MK_RBUTTON)) == 0)
            return 0;

         if (m_bSetCursor)
         {
            m_bSetCursor = false;
            return 0;
         }

         HWND hwnd = GetDlgItem(IDC_BOX_VIEW);
         GetClientRect(hwnd, &rcClient);
         POINT  ptCenter = {(rcClient.left+rcClient.right ) >> 1,
                            (rcClient.top +rcClient.bottom) >> 1}; 

         REPORT("curpos: %d, %d", pt.x, pt.y);
         pt.x = (short)((long)pt.x - ptCenter.x);// - pt.x;
         pt.y = (short)((long)pt.y - ptCenter.y);// - pt.y;

         REPORT("pos   : %d, %d", pt.x, pt.y);
         REPORT("center: %d, %d", ptCenter.x, ptCenter.y);

         m_bSetCursor = true;
         ClientToScreen(hwnd, &ptCenter);
         SetCursorPos(ptCenter.x, ptCenter.y);


         if (wParam & MK_LBUTTON)
         {
            double *pdVar = GetTransModeVar();
            if (pdVar)
            {
               switch(m_GLView.m_nTransMode)
               {
                  case TRANS_MOVE:
                  if (m_pPropertySheet->IsSectionChangeable() && !m_GLView.IsScaleActive())
                  {
                     m_GLView.TurnBox(pt.x, pt.y);
                  }
                  else
                  {
                     if (m_GLView.m_nScaleModeZ & SHOW_SCALE)
                     {
                        pdVar[2] += m_dFactor[2]*(pt.y);
                        m_GLView.m_bChanged[2] = (m_dFactor[2] != 0) ? true : false;
                     }
                     else
                     {
                        pdVar[0] += m_dFactor[0]*(pt.x);
                        pdVar[1] += m_dFactor[1]*(pt.y);
                        m_GLView.m_bChanged[0] = (m_dFactor[0] != 0) ? true : false;
                        m_GLView.m_bChanged[1] = (m_dFactor[1] != 0) ? true : false;
                     }
                  } break;
                  case TRANS_ZOOM:
                     pdVar[0] += m_dFactor[0]*(pt.y);
                     m_GLView.m_bChanged[0] = (m_dFactor[0] != 0) ? true : false;
                     break;
                  case TRANS_TURN:
                  if (m_pPropertySheet->IsSectionChangeable() && !m_GLView.IsScaleActive(false))
                  {
                     m_GLView.TurnBox(pt.x, pt.y);
                  }
                  else
                  {
                     pdVar[0] += m_dFactor[0]*(pt.x);
                     pdVar[1] += m_dFactor[1]*(pt.y);
                     m_GLView.m_bChanged[0] = (m_dFactor[0] != 0) ? true : false;
                     m_GLView.m_bChanged[1] = (m_dFactor[1] != 0) ? true : false;
                  } break;
                  case TRANS_TURNLIGHT:
                     pdVar[0] += m_dFactor[0]*(pt.x);
                     pdVar[1] += m_dFactor[1]*(pt.y);
                     m_GLView.m_bChanged[0] = (m_dFactor[0] != 0) ? true : false;
                     m_GLView.m_bChanged[1] = (m_dFactor[1] != 0) ? true : false;
                     break;
                  case TRANS_LIGHT_INTENS:
                     pdVar[0] += m_dFactor[0]*(pt.y);
                     pdVar[1] += m_dFactor[0]*(pt.y);
                     pdVar[2] += m_dFactor[0]*(pt.y);
                     m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = (m_dFactor[0] != 0) ? true : false;
                     break;
                  case TRANS_LIGHT_SPOT:
                     pdVar[0] += m_dFactor[0]*(pt.y);
                     m_GLView.m_bChanged[0] = (m_dFactor[0] != 0) ? true : false;
                     break;
                  case TRANS_SETTINGS:
                     pdVar[0] += m_dFactor[0]*(pt.y);
                     m_GLView.m_bChanged[0] = (m_dFactor[0] != 0) ? true : false;
                     break;
               }
               if (m_GLView.m_bChanged[0] || m_GLView.m_bChanged[1] || m_GLView.m_bChanged[2])
                  UpdateTransMode();
            }
         }
         if (wParam & MK_RBUTTON)
         {
            double *pdVar = GetTransModeVar();
            if (pdVar)
            {
               switch(m_GLView.m_nTransMode)
               {
                  case TRANS_MOVE:
                  if (m_pPropertySheet->IsSectionChangeable() && !m_GLView.IsScaleActive())
                  {
                     m_GLView.MoveBox(pt.x, pt.y, 0);
                  }
                  else
                  {
                     pdVar[0] += m_dFactor[0]*(pt.x);
                     pdVar[2] += m_dFactor[2]*(pt.y);
                     m_GLView.m_bChanged[0] = (m_dFactor[0] != 0) ? true : false;
                     m_GLView.m_bChanged[2] = (m_dFactor[2] != 0) ? true : false;
                  } break;
                  case TRANS_TURN:                                // Spezialfall beim Drehen
                  if (!m_pPropertySheet->IsSectionChangeable())   // des Haupgehäuses
                  {                                               // Bewegung durch rechte Maustaste zulassen
                     m_GLView.MoveBox(pt.x, pt.y, 0);
                  }
                  else if (!m_GLView.IsScaleActive(false))
                  {
                     m_GLView.MoveBox(pt.x, pt.y, 0);
                  }
                  break;
                  case TRANS_ZOOM:
                     pdVar[1] += m_dFactor[1]*(pt.y);
                     m_GLView.m_bChanged[1] = (m_dFactor[1] != 0) ? true : false;
                     break;
                  case TRANS_TURNLIGHT:
                     pdVar[2] += m_dFactor[2]*(pt.y);
                     m_GLView.m_bChanged[2] = (m_dFactor[2] != 0) ? true : false;
                     break;
                  case TRANS_LIGHT_SPOT:
                     pdVar[1] += m_dFactor[1]*(pt.y);
                     m_GLView.m_bChanged[1] = (m_dFactor[1] != 0) ? true : false;
                     break;
                  case TRANS_SETTINGS:
                     pdVar[2] += m_dFactor[2]*(pt.y);
                     m_GLView.m_bChanged[2] = (m_dFactor[2] != 0) ? true : false;
                     break;
               }
               if (m_GLView.m_bChanged[0] || m_GLView.m_bChanged[1] || m_GLView.m_bChanged[2])
                  UpdateTransMode();
            }
         }
      }break;
      case WM_NOTIFY:
      {
         NMHDR *pnmHdr = (NMHDR*)lParam;
         if (UDN_DELTAPOS == pnmHdr->code)
         {
            NMUPDOWN *pnmUD = (NMUPDOWN*)lParam;
            HWND hwndBuddy  = GetNextDlgTabItem(m_hWnd, pnmHdr->hwndFrom, 1);
            if (hwndBuddy)
            {
               char szText[32];
               int nLen  = ::SendMessage(hwndBuddy, WM_GETTEXT, 31, (LPARAM)szText);
               if (nLen > 0)
               {
                  short  dwSel[2];
                  *((DWORD*)dwSel)   = ::SendMessage(hwndBuddy, EM_GETSEL, 0, 0);
                  int    i, nIdBuddy = ::GetDlgCtrlID(hwndBuddy);// ID ermitteln
                  int    nSel        = dwSel[1]-dwSel[0];      // Länge der Selection ermitteln
                  double dTemp       = atof(szText);           // Wert ermitteln

                  for (i=0; i<nLen; i++)                       // Dezimaltrennzeichen suchen
                     if ((szText[i] == '.')||(szText[i] == ',')) break;

                  if (nSel==0)
                  {
                     int nAcc = m_nAccuracy1;                  // Keine Selection
                     if (nIdBuddy == IDC_3D_EDT_TRAN_Z)nAcc = m_nAccuracy2;
                     dwSel[1] = (short)(i+nAcc+1);             // Selection anhand der benötigten Genauigkeit setzen
                     dwSel[0] = (short)(dwSel[1]-1);
                     nSel  = 1;
                  }

                  int nExp = i-dwSel[1];                       // Schrittweite ermitteln
                  if (nExp < 0) nExp++;                        // bei Kommazahlen Exponet verringern
                  double dIncrease = pow(10, nExp);            // Schrittweite berechnen
                  if (pnmUD->iDelta > 0) dTemp -= dIncrease;   // Subtrahieren / Addieren
                  else                   dTemp += dIncrease;
                  double *pdVar = GetTransModeVar();           // Variable
                  pdVar[nIdBuddy-IDC_3D_EDT_TRAN_X] = dTemp;   // setzen
                  int nOldmode = gm_nmode;
                  gm_nmode &= ~ETSDIV_NM_NO_END_NULLS;         // alle Stellen anzeigen
                  m_GLView.m_bChanged[nIdBuddy-IDC_3D_EDT_TRAN_X] = true;
                  UpdateTransMode();                           // Updaten
                  gm_nmode = nOldmode;

                  if (szText[0] == '-')                        // Vorzeichenänderung
                  {
                     if (dTemp > 0)                            // nach Plus
                     {
                        dwSel[0]--;                            // Selection verringern 
                        dwSel[1]--;
                     }
                  }
                  else
                  {
                     if (dTemp < 0)                            // nach Minus
                     {
                        dwSel[0]++;                            // Selection erhöhen
                        dwSel[1]++;
                     }
                  }
                  if (nExp >= 0)                               // vor dem Komma
                  {
                     nLen = ::SendMessage(hwndBuddy, WM_GETTEXT, 31, (LPARAM)szText);
                     nSel = i;                                 // Position des Decimalpunktes merken
                     for (i=0; i<nLen; i++)                    // Dezimaltrennzeichen suchen
                        if ((szText[i] == '.')||(szText[i] == ',')) break;
                     if (nSel != i)                            // Dezimaltrennzeichen verschoben
                     {
                        dwSel[0] += ((short)(i-nSel));         // Selection anpassen
                        dwSel[1] += ((short)(i-nSel));
                     }
                  }
                  if (::GetFocus() != hwndBuddy) ::SetFocus(hwndBuddy);
                  ::SendMessage(hwndBuddy, EM_SETSEL, dwSel[0], dwSel[1]);

                  NotifyCabPageSegmentMoved();
               }
            }
         }
      }break;
      case WM_LBUTTONDOWN: CatchCursorPos(MAKEPOINTS(lParam)); break;
      case WM_LBUTTONUP:
      {
         ::ShowCursor(true);
         ::ClipCursor(NULL);
         ::ReleaseCapture();
         NotifyCabPageSegmentMoved();
      } break;
      case WM_RBUTTONDOWN: CatchCursorPos(MAKEPOINTS(lParam)); break;
      case WM_RBUTTONUP:
      {
         ::ShowCursor(true);
         ::ClipCursor(NULL);
         ::ReleaseCapture();
         NotifyCabPageSegmentMoved();
      } break;
      case WM_MENUSELECT: SetStatusText(wParam, NULL);break;
      case WM_EXITMENULOOP: SetStatusText(0, "");break;
      case WM_TAB_CTRL:
      {
         HWND hWnd = ::GetNextDlgTabItem(m_hWnd, (HWND)lParam, 0);
         if (hWnd) ::SetFocus(hWnd);
      } break;
      default:
      if((wParam&0xFFF0)==SC_KEYMENU)
      {
         HWND hwndFocus = GetFocus();
         if (hwndFocus)
         {
            long nID = ::GetWindowLong(hwndFocus, GWL_ID);
            if ((nID >= IDC_3D_EDT_TRAN_X) && (nID <= IDC_3D_EDT_TRAN_Z))
            {
               if (m_GLView.m_nTransMode == TRANS_MOVE)
               {
                  ETSDIV_NUTOTXT ntx = {0, ETSDIV_NM_STD|ETSDIV_NM_NO_END_NULLS|ETSDIV_UC_CTRL_INIT|ETSDIV_UC_CTRL_REINIT, 0, 0, 0};
                  ntx.nmode |= ETSDIV_UC_CTRL_SIGNED;
                  m_nFlags |= NOKILLFOCUSNOTIFICATION;
                  bool bReturn = CEtsDiv::UnitConvertDlg(&ntx, hwndFocus);
                  m_nFlags &= ~NOKILLFOCUSNOTIFICATION;
                  if (bReturn)
                  {
                     ::SendMessage(hwndFocus, EM_SETMODIFY, 1, 0);
                     ::SendMessage(m_hWnd, WM_COMMAND, MAKELONG(nID, EN_KILLFOCUS), (LPARAM)hwndFocus);
                  }
               }
            }
         }
         return TRUE;
      }break;
   }
   return CEtsDialog::OnMessage(nMsg, wParam, lParam);
}

/******************************************************************************
* Definition : void CatchCursorPos();
* Zweck      : Fangen des Cursor im 3D-View, Cursoranzeige abschalten
* Parameter (E): pt: Aktuelle Cursorposition
******************************************************************************/
void C3DViewDlg::CatchCursorPos(POINTS pt)
{
   HWND hwnd = m_GLView.GetThreadHWND();
   RECT   rcClient;
   ::GetClientRect(hwnd, &rcClient);
   POINT ptC = {pt.x, pt.y};
   ::MapWindowPoints(m_hWnd, hwnd, &ptC, 1);
   if (::PtInRect(&rcClient, ptC))
   {
      POINT  ptConv = {(rcClient.left+rcClient.right ) >> 1,
                       (rcClient.top +rcClient.bottom) >> 1}; 
      ::ClientToScreen(hwnd, &ptConv);

      m_bSetCursor = true;
      ::SetCursorPos(ptConv.x, ptConv.y);
      rcClient.left   = ptConv.x - 40;
      rcClient.right  = ptConv.x + 40;
      rcClient.top    = ptConv.y - 40;
      rcClient.bottom = ptConv.y + 40;
      ::ClipCursor(&rcClient);
      ::ShowCursor(false);
      ::SetCapture(m_GLView.GetThreadHWND());
   }
}

/******************************************************************************
* Definition :void UpdateTransMode(bool);
* Zweck      : Updaten der Controls in der Dialogbox
* Parameter (E): bUpdate: Ändern der Buttons und Editfelder          (bool)
******************************************************************************/
void C3DViewDlg::UpdateTransMode(bool bUpdate, int nMode)
{
   bool bMessage = (nMode == -2) ? true : false;
   if (bUpdate)
   {
      int i;
      char szUnit[64];
      for (i=0; i<3; i++)
      {
         if (m_nBmpWnd[i]) ::ShowWindow(GetDlgItem(IDC_3D_BTN_TRAN_X+i), SW_HIDE);
         if (m_nTxtWnd[i]) ::ShowWindow(GetDlgItem(IDC_3D_TXT_TRAN_X+i), SW_HIDE);
         else              ::EnableWindow(GetDlgItem(IDC_3D_EDT_TRAN_X+i), true);
         if (m_nCheckBtn[i])
         {
            CheckDlgButton(m_hWnd, IDC_3D_BTN_TRAN_X+i,MF_UNCHECKED);
            ModifyDlgItemStyle(IDC_3D_BTN_TRAN_X+i, BS_ICON, m_nCheckBtn[i]);
         }
         m_dFactor[i]   = 0;
         m_nBmpWnd[i]   = 0;
         m_nTxtWnd[i]   = 0;
         m_nCheckBtn[i] = 0;
      }
      SetStatusText(IDC_3D_R_TRANS0+m_GLView.m_nTransMode, NULL);
      switch (m_GLView.m_nTransMode)
      {
         case TRANS_MOVE:
            m_nBmpWnd[0]   = IDC_3D_PIC_TRAN_X;
            m_nBmpWnd[1]   = IDC_3D_PIC_TRAN_Y;
            m_nBmpWnd[2]   = IDC_3D_PIC_TRAN_Z;
            if (m_pPropertySheet->IsSectionChangeable())
            {
               m_GLView.m_nAlphaBlend &= ~ALPHA_DRAW_DIR;
               m_nCheckBtn[0] = BS_CHECKBOX|BS_PUSHLIKE;
               m_nCheckBtn[1] = BS_CHECKBOX|BS_PUSHLIKE;
               m_nCheckBtn[2] = BS_CHECKBOX|BS_PUSHLIKE;
               if (!m_GLView.IsScaleActive())
               {
                  m_GLView.m_nScaleModeY |= SHOW_SCALE;
               }
               m_GLView.OnChangeCabinet();
               m_GLView.InvalidateList(LIST_SCALE);
            }
            ::LoadString(m_hInstance, IDS_UNIT_MOVE, szUnit, 64);
            SetDlgItemText(IDC_3D_TXT_UNITS, szUnit);
#ifdef UNIT_METER
            m_nAccuracy1   = 3;
            m_nAccuracy2   = 3;
#else
            m_nAccuracy1   = 0;
            m_nAccuracy2   = 0;
#endif
            break;
         case TRANS_TURN:
            m_nBmpWnd[0] = IDC_3D_PIC_TURN_PHI;
            m_dFactor[0] = 0.5;
            m_dFactor[2] = 0.0;
            ::LoadString(m_hInstance, IDS_UNIT_TURN, szUnit, 64);
            SetDlgItemText(IDC_3D_TXT_UNITS, szUnit);
            if (!m_pPropertySheet->IsSectionChangeable())
            {
               m_nBmpWnd[1] = IDC_3D_PIC_TURN_THETA;
               m_dFactor[1] = 0.5;
               m_nAccuracy1  = 1;
               m_nAccuracy2  = 1;
            }
            else
            {
               m_GLView.m_nAlphaBlend &= ~ALPHA_DRAW_DIR;
               m_nCheckBtn[0] = BS_CHECKBOX|BS_PUSHLIKE;
               if (g_nUserMode & USERMODE_THETA)
               {
                  m_nCheckBtn[1] = BS_CHECKBOX|BS_PUSHLIKE;
                  m_nBmpWnd[1] = IDC_3D_PIC_TURN_THETA;
                  m_dFactor[1] = 0.5;
               }
               else
               {
                  m_dFactor[1] = 0.0;
               }
               if (!(m_GLView.m_nScaleModePhi   & SHOW_SCALE) && 
                   !(m_GLView.m_nScaleModeTheta & SHOW_SCALE))
               {
                  m_GLView.m_nScaleModePhi |= SHOW_SCALE;
               }
               m_GLView.InvalidateList(LIST_SCALE);

               m_nAccuracy1  = 2;
               m_nAccuracy2  = 2;
            }
            break;
         case TRANS_ZOOM:
            m_nBmpWnd[0] = IDC_3D_PIC_ZOOM;
            m_dFactor[0] = -0.01;
            if (g_nUserMode & USERMODE_VOLUME)
            {
               m_nBmpWnd[1] = IDC_3D_PIC_ZOOM_DUMMY1;
               m_nBmpWnd[2] = IDC_3D_PIC_ZOOM_DUMMY2;
               m_dFactor[1] = 0.01;
               m_dFactor[2] = 0.01;
            }
            m_nAccuracy1 = 2;
            m_nAccuracy2 = 2;
            ::LoadString(m_hInstance, IDS_UNIT_ZOOM, szUnit, 64);
            SetDlgItemText(IDC_3D_TXT_UNITS, szUnit);
            break;
         case TRANS_TURNLIGHT:
            m_nBmpWnd[0] = IDC_3D_PIC_LITE_PHI;
            m_nBmpWnd[1] = IDC_3D_PIC_LITE_THETA;
            m_nBmpWnd[2] = IDC_3D_PIC_LITE_DIST;
            m_dFactor[0] =  0.1;
            m_dFactor[1] = -0.1;
            m_dFactor[2] = -0.1;
            m_nAccuracy1 = 1;
            m_nAccuracy2 = 1;
            ::LoadString(m_hInstance, IDS_UNIT_LIGHT_DIR, szUnit, 64);
            SetDlgItemText(IDC_3D_TXT_UNITS, szUnit);
            break;
         case TRANS_LIGHT_INTENS:
            m_nBmpWnd[0] = IDC_3D_PIC_LIGHT_RED;
            m_nBmpWnd[1] = IDC_3D_PIC_LIGHT_GREEN;
            m_nBmpWnd[2] = IDC_3D_PIC_LIGHT_BLUE;
            m_dFactor[0] = -0.1;
            m_nAccuracy1 = 1;
            m_nAccuracy2 = 1;
            ::LoadString(m_hInstance, IDS_UNIT_LIGHT_INTENS, szUnit, 64);
            SetDlgItemText(IDC_3D_TXT_UNITS, szUnit);
            break;
         case TRANS_LIGHT_SPOT:
            m_nBmpWnd[0] = IDC_3D_PIC_LIGHT_EXP;
            m_dFactor[0] = -0.1;
            m_nBmpWnd[1] = IDC_3D_PIC_LIGHT_CUT_OFF;
            m_dFactor[1] = -0.1;
            m_nAccuracy1 = 1;
            m_nBmpWnd[2] = IDC_3D_PIC_LIGHT_ON_OFF;
            m_nTxtWnd[2] = IDS_LIGHTING;
            m_nCheckBtn[2] = BS_CHECKBOX|BS_PUSHLIKE;
            ::LoadString(m_hInstance, IDS_UNIT_LIGHT_SOURCE, szUnit, 64);
            SetDlgItemText(IDC_3D_TXT_UNITS, szUnit);
            break;
         case TRANS_SETTINGS:
            m_nBmpWnd[0] = IDC_3D_PIC_SET_ALPHA_BLEND;
            m_dFactor[0] = -0.1;
            m_nCheckBtn[0] = BS_CHECKBOX|BS_PUSHLIKE;
            m_nAccuracy1 = 1;
            m_nBmpWnd[1] = IDC_3D_PIC_SET_PROJECTION;
            m_nTxtWnd[1] = IDS_ORTHOGONAL + (m_GLView.m_bPerspective?1:0);
            m_nCheckBtn[1] = BS_CHECKBOX|BS_PUSHLIKE;
            m_nBmpWnd[2] = IDC_3D_PIC_SET_SHININES;
            m_dFactor[2] = -0.1;
            m_nAccuracy2 = 1;
            ::LoadString(m_hInstance, IDS_UNIT_REPRESENTATION, szUnit, 64);
            SetDlgItemText(IDC_3D_TXT_UNITS, szUnit);
            break;
      }
      for (i=0; i<3; i++)
      {
         if (m_nBmpWnd[i])
         {
            ShowWindow(GetDlgItem(IDC_3D_BTN_TRAN_X+i), SW_SHOW);
            SendDlgItemMessage(IDC_3D_BTN_TRAN_X+i, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)::LoadImage(m_hInstance, MAKEINTRESOURCE(m_nBmpWnd[i]), IMAGE_ICON, 16,16, LR_DEFAULTCOLOR));
            ::SetWindowContextHelpId(GetDlgItem(IDC_3D_BTN_TRAN_X+i), m_nBmpWnd[i]);
            if (m_nCheckBtn[i])
            {
               ModifyDlgItemStyle(IDC_3D_BTN_TRAN_X+i, m_nCheckBtn[i], 0);
               if (m_nCheckBtn[i] & BS_BITMAP) ModifyDlgItemStyle(IDC_3D_BTN_TRAN_X+i, 0, BS_ICON);
            }
            int nShow = SW_SHOW;
            if (m_nTxtWnd[i])
            {
               char szText[64];
               ::LoadString(m_hInstance, m_nTxtWnd[i], szText, 64);
               SendDlgItemMessage(IDC_3D_TXT_TRAN_X+i, WM_SETTEXT, 0, (LPARAM)szText);
               ShowWindow(GetDlgItem(IDC_3D_TXT_TRAN_X+i), SW_SHOW);
               nShow = SW_HIDE;
            }
            ShowWindow(GetDlgItem(IDC_3D_EDT_TRAN_X+i) , nShow);
            ShowWindow(GetDlgItem(IDC_3D_SPIN_TRAN_X+i), nShow);
         }
         else
         {
            ShowWindow(GetDlgItem(IDC_3D_BTN_TRAN_X+i) , SW_HIDE);
            ShowWindow(GetDlgItem(IDC_3D_EDT_TRAN_X+i) , SW_HIDE);
            ShowWindow(GetDlgItem(IDC_3D_SPIN_TRAN_X+i), SW_HIDE);
         }
      }
      UpdateCtrlStates();
   }

   double *pdVar = GetTransModeVar();
   switch (m_GLView.m_nTransMode)
   {
      case TRANS_ZOOM:
         if (m_GLView.m_bChanged[0]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_X, 0.1, 10, m_nAccuracy1, pdVar[0], bMessage);
         if (m_GLView.m_bChanged[1]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_Y, 0.2, 20, m_nAccuracy1, pdVar[1], bMessage);
         break;
      case TRANS_TURN:
         if (m_GLView.m_bChanged[1]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_Y, -90.0, 90.0, m_nAccuracy1, pdVar[1], bMessage);
         break;
      case TRANS_TURNLIGHT:
         if (m_GLView.m_bChanged[1]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_Y, -90.0, 90.0, m_nAccuracy1, pdVar[1], bMessage);
         if (m_GLView.m_bChanged[2]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_Z, 0, 100, m_nAccuracy2, pdVar[2], bMessage);
         break;
      case TRANS_LIGHT_INTENS:
         if (m_GLView.m_bChanged[0]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_X, 0, 100, m_nAccuracy1, pdVar[0], bMessage);
         if (m_GLView.m_bChanged[1]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_Y, 0, 100, m_nAccuracy1, pdVar[1], bMessage);
         if (m_GLView.m_bChanged[2]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_Z, 0, 100, m_nAccuracy1, pdVar[2], bMessage);
         break;
      case TRANS_SETTINGS:
         if (m_GLView.m_bChanged[0]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_X, 1.0, 100.0, m_nAccuracy1, pdVar[0], bMessage);
         if (m_GLView.m_bChanged[2]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_Z, 0.0, 100.0, m_nAccuracy2, pdVar[2], bMessage);
         break;
      case TRANS_LIGHT_SPOT:
         if (m_GLView.m_bChanged[0]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_X, 0.0, 100.0, m_nAccuracy1, pdVar[0], bMessage);
         if (m_GLView.m_bChanged[1]) CheckMinMaxDouble(IDC_3D_EDT_TRAN_Y, 5.0, 180.0, m_nAccuracy1, pdVar[1], bMessage);
         break;
   }

   if (pdVar)
   {
      if (bUpdate) m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
      else         m_GLView.InvalidateTransMode(false, nMode);
      if (m_GLView.m_bChanged[0] && (m_nTxtWnd[0] == NULL))
         SetDlgItemDouble(IDC_3D_EDT_TRAN_X, pdVar[0], m_nAccuracy1);
      if (m_GLView.m_bChanged[1] && (m_nTxtWnd[1] == NULL))
         SetDlgItemDouble(IDC_3D_EDT_TRAN_Y, pdVar[1], m_nAccuracy1);
      if (m_GLView.m_bChanged[2] && (m_nTxtWnd[2] == NULL))
         SetDlgItemDouble(IDC_3D_EDT_TRAN_Z, pdVar[2], m_nAccuracy2);
      m_GLView.Reset();
   }
   else m_GLView.InvalidateTransMode(true, nMode);
}

/********************************************************************************
* Definition :void UpdateCtrlStates();
* Zweck      : Updaten der Zustände der Controlls und des Menüs in der Dialogbox
********************************************************************************/
void C3DViewDlg::UpdateCtrlStates()
{
   UINT nEnableXYZ      = MF_DISABLED|MF_GRAYED;
   UINT nEnablePhi      = MF_DISABLED|MF_GRAYED;
   UINT nEnableScaleX   = MF_DISABLED|MF_GRAYED;
   UINT nEnableScaleY   = MF_DISABLED|MF_GRAYED;
   UINT nEnableScaleZ   = MF_DISABLED|MF_GRAYED;
   UINT nEnableScalePhi = MF_DISABLED|MF_GRAYED;
   UINT nEnableBackSide = MF_DISABLED|MF_GRAYED;
   switch (m_GLView.m_nTransMode)
   {
      case TRANS_MOVE:
      {
#ifdef UNIT_METER
         m_dFactor[0]   =  0.001;
         m_dFactor[1]   = -0.001;
         m_dFactor[2]   =  0.001;
#else
         m_dFactor[0]   =  1.0;
         m_dFactor[1]   = -1.0;
         m_dFactor[2]   =  1.0;
#endif
         if (m_pPropertySheet->IsSectionChangeable())
         {
            nEnableXYZ = MF_ENABLED;
            if (m_GLView.m_nScaleModeX & SHOW_SCALE)
            {
               m_dFactor[1] = 0;
               m_dFactor[2] = 0;
               m_GLView.m_dPhi   =  0;
               if (m_GLView.m_bShowBackSide)
               {
                  m_GLView.m_dTheta = -90;
                  m_dFactor[0] = -m_dFactor[0];
               }
               else
               {
                  m_GLView.m_dTheta =  90;
               }
               nEnableScaleX   = MF_ENABLED;
               nEnableBackSide = MF_ENABLED;
            }
            else if (m_GLView.m_nScaleModeY & SHOW_SCALE)
            {
               m_dFactor[0] = 0;
               m_dFactor[2] = 0;
               if (m_GLView.m_bShowBackSide)
               {
                  m_GLView.m_dPhi = 180;
               }
               else 
               {
                  m_GLView.m_dPhi =  0;
               }
               m_GLView.m_dTheta = 0;
               nEnableScaleY   = MF_ENABLED;
               nEnableBackSide = MF_ENABLED;
            }
            else if (m_GLView.m_nScaleModeZ & SHOW_SCALE)
            {
               m_dFactor[0] = 0;
               m_dFactor[1] = 0;
               m_GLView.m_dPhi   =   0;
               if (m_GLView.m_bShowBackSide)
               {
                  m_GLView.m_dTheta = -90;
                  m_dFactor[2] = -m_dFactor[2];
               }
               else 
               {
                  m_GLView.m_dTheta =  90;
               }
               nEnableScaleZ   = MF_ENABLED;
               nEnableBackSide = MF_ENABLED;
            }
            else
            {
               m_GLView.m_dPhi   =  30;
               m_GLView.m_dTheta =  15;
               m_dFactor[0] = 0;
               m_dFactor[1] = 0;
               m_dFactor[2] = 0;
               nEnableScaleX = nEnableScaleY = nEnableScaleZ = MF_ENABLED;
            }
            m_GLView.InvalidateList(LIST_MATRIX);
         }
         CheckDlgButton(m_hWnd, IDC_3D_BTN_TRAN_X, m_GLView.m_nScaleModeX & SHOW_SCALE ? MF_CHECKED : MF_UNCHECKED);
         CheckDlgButton(m_hWnd, IDC_3D_BTN_TRAN_Y, m_GLView.m_nScaleModeY & SHOW_SCALE ? MF_CHECKED : MF_UNCHECKED);
         CheckDlgButton(m_hWnd, IDC_3D_BTN_TRAN_Z, m_GLView.m_nScaleModeZ & SHOW_SCALE ? MF_CHECKED : MF_UNCHECKED);
         ::EnableWindow(GetDlgItem(IDC_3D_EDT_TRAN_X), (m_dFactor[0] != 0) ? true : false);
         ::EnableWindow(GetDlgItem(IDC_3D_EDT_TRAN_Y), (m_dFactor[1] != 0) ? true : false);
         ::EnableWindow(GetDlgItem(IDC_3D_EDT_TRAN_Z), (m_dFactor[2] != 0) ? true : false);
      } break;
      case TRANS_TURN:
      {
         if (m_pPropertySheet->IsSectionChangeable())
         {
            nEnablePhi      = nEnableScalePhi = MF_ENABLED;
            nEnableBackSide = MF_ENABLED;
            if (m_GLView.m_nScaleModePhi & SHOW_SCALE)
            {
               m_dFactor[0] = -0.5;
               if (g_nUserMode & USERMODE_THETA)
               {
                  m_dFactor[1] = 0.5;
               }
               m_GLView.m_dPhi   =   0;
               if (m_GLView.m_bShowBackSide)
               {
                  m_GLView.m_dTheta = -90;
                  m_dFactor[0] = -m_dFactor[0];
               }
               else
               {
                  m_GLView.m_dTheta =  90;
               }
            }
            else
            {
               m_dFactor[0] = 0;
               m_dFactor[1] = 0;
               m_dFactor[2] = 0;
               m_GLView.m_dPhi   =  30;
               m_GLView.m_dTheta =  15;
            }
            ::EnableWindow(GetDlgItem(IDC_3D_EDT_TRAN_X), (m_dFactor[0] != 0) ? true : false);
            ::EnableWindow(GetDlgItem(IDC_3D_EDT_TRAN_Y), (m_dFactor[1] != 0) ? true : false);
            m_GLView.InvalidateList(LIST_MATRIX);
            CheckDlgButton(m_hWnd, IDC_3D_BTN_TRAN_X, m_GLView.m_nScaleModePhi & SHOW_SCALE ? MF_CHECKED : MF_UNCHECKED);
            if (g_nUserMode & USERMODE_THETA)
            {
               CheckDlgButton(m_hWnd, IDC_3D_BTN_TRAN_Y, ((m_pPropertySheet->m_nCurrentCab != 0) && (m_GLView.m_nScaleModeTheta & SHOW_SCALE)) ? MF_CHECKED : MF_UNCHECKED);
            }
         }
         else
         {
            ::EnableWindow(GetDlgItem(IDC_3D_EDT_TRAN_X), true);
            ::EnableWindow(GetDlgItem(IDC_3D_EDT_TRAN_Y), true);
         }
      } break;
      case TRANS_SETTINGS:
         CheckDlgButton(m_hWnd, IDC_3D_BTN_TRAN_X, (m_GLView.m_nAlphaBlend&ALPHA_BLENDING) ? MF_CHECKED : MF_UNCHECKED);
         CheckDlgButton(m_hWnd, IDC_3D_BTN_TRAN_Y, m_GLView.m_bPerspective ? MF_UNCHECKED : MF_CHECKED);
         if (m_GLView.m_bChanged[1])
         {
            char szText[64];
            ::LoadString(m_hInstance, IDS_ORTHOGONAL + (m_GLView.m_bPerspective?1:0), szText, 64);
            SendDlgItemMessage(IDC_3D_TXT_TRAN_Y, WM_SETTEXT, 0, (LPARAM)szText);
         }
         break;
      case TRANS_LIGHT_SPOT:
         CheckDlgButton(m_hWnd, IDC_3D_BTN_TRAN_Z, m_GLView.m_bLighting ? MF_CHECKED : MF_UNCHECKED);
         break;
   }   

   SetMenuItem(IDM_VIEW_X_SCALE, m_GLView.m_nScaleModeX & SHOW_SCALE ? MF_CHECKED : MF_UNCHECKED, nEnableXYZ);
   SetMenuItem(IDM_VIEW_Y_SCALE, m_GLView.m_nScaleModeY & SHOW_SCALE ? MF_CHECKED : MF_UNCHECKED, nEnableXYZ);
   SetMenuItem(IDM_VIEW_Z_SCALE, m_GLView.m_nScaleModeZ & SHOW_SCALE ? MF_CHECKED : MF_UNCHECKED, nEnableXYZ);
   SetMenuItem(IDM_VIEW_PHI_SCALE, m_GLView.m_nScaleModePhi & SHOW_SCALE ? MF_CHECKED : MF_UNCHECKED, nEnablePhi);
/*
   if (g_nUserMode & USERMODE_THETA)
   {
      CheckDlgButton(m_hWnd, IDC_3D_BTN_TRAN_Y, (m_GLView.m_nScaleModeTheta & SHOW_SCALE) ? MF_CHECKED : MF_UNCHECKED);
   }
*/
   SetMenuItem(IDM_SET_X_SCALE, INVALID_VALUE  , nEnableScaleX);
   SetMenuItem(IDM_SET_Y_SCALE, INVALID_VALUE  , nEnableScaleY);
   SetMenuItem(IDM_SET_Z_SCALE, INVALID_VALUE  , nEnableScaleZ);
   SetMenuItem(IDM_SET_PHI_SCALE, INVALID_VALUE, nEnableScalePhi);

   SetMenuItem(IDC_3D_PIC_SET_ALPHA_BLEND, (m_GLView.m_nAlphaBlend&ALPHA_BLENDING) ? MF_CHECKED : MF_UNCHECKED, INVALID_VALUE);
   SetMenuItem(IDM_VIEW_BACK_SIDE, (m_GLView.m_bShowBackSide) ? MF_CHECKED : MF_UNCHECKED, nEnableBackSide);
   int nRange[2] = {IDM_VIEW_PARALLEL, IDM_VIEW_PERSPECTIVE}; 
   SetMenuItem(m_GLView.m_bPerspective ? IDM_VIEW_PERSPECTIVE:IDM_VIEW_PARALLEL, MFT_RADIOCHECK, (UINT)&nRange);
}

/******************************************************************************
* Definition : double * GetTransModeVar();
* Zweck      : liefert die Variablen, die von den aktiven Controls geändert
*              werden. Dise sind abhängig von dem eingestellten Modus und dem
*              gewählten Gehäusegment.
* Funktionswert : Feld mit double-Variablen maximal double[3]        (double*)
******************************************************************************/
double * C3DViewDlg::GetTransModeVar()
{
   switch (m_GLView.m_nTransMode)
   {
      case TRANS_MOVE:
      {
         if (m_pPropertySheet->IsSectionChangeable())
            return (double*)&m_GLView.m_dTranCabX;
         else
            return (double*)&m_GLView.m_vTranslation;
      }
      case TRANS_TURN:
      {
         if (m_pPropertySheet->IsSectionChangeable())
            return (double*)&m_GLView.m_dPhiCab;
         else
            return (double*)&m_GLView.m_dPhi;
      }
      case TRANS_ZOOM:         return (double*)&m_GLView.m_dZoom;
      case TRANS_TURNLIGHT:    return (double*)&m_GLView.m_dPhiLight;
      case TRANS_LIGHT_INTENS: return (double*)&m_GLView.m_dLightIntens[0];
      case TRANS_LIGHT_SPOT:   return (double*)&m_GLView.m_dSpotCutExponent;
      case TRANS_SETTINGS:     return (double*)&m_GLView.m_dAlphaBlend;
   }
   return NULL;
}

/******************************************************************************
* Definition : void OnNewCabinet();
* Zweck      : erzeugt ein neues Gehäusesegment
******************************************************************************/
void C3DViewDlg::OnNewCabinet()
{
   if (m_hwndCabinetPage)                                      // Cabinet Page
   {
      char szFormat[CAB_NAME_LEN];
      int nCount;
      CCabinet *pCbase  = m_pPropertySheet->GetCabinet(BASE_CABINET);
      CCabinet *pNewCab = new CCabinet;
      nCount = pNewCab->SetBaseCabinet(pCbase);
      CCabinet* pC = m_pPropertySheet->GetCurrentCab();        // Vorlage
      pNewCab->dHeight = pC->dHeight;                          // kopieren
      pNewCab->dWidth  = pC->dWidth;
      pNewCab->dWidth1 = pC->dWidth1;
      pNewCab->dWidth2 = pC->dWidth2;
      pNewCab->dDepth  = pC->dDepth;
      pNewCab->dDepth1 = pC->dDepth1;
      pNewCab->dDepth2 = pC->dDepth2;
      if (pC->m_dwFlags & WIDTH_1_2_ON_BACK) pNewCab->m_dwFlags |= WIDTH_1_2_ON_BACK;

      pNewCab->InitCabPoints();                                // Punkte initialisieren
      pNewCab->vPosRef = CVector(0, pNewCab->dWidth, pC->dHeight);
      if (!pC->IsBaseCabinet()) pNewCab->vPosRef += pC->vPosRef;
      pNewCab->SetTransformation();

      ::LoadString(m_hInstance, IDS_SUB_CABINET, szFormat, CAB_NAME_LEN);
      wsprintf(pNewCab->m_szName, szFormat, nCount);
      ::SendMessage(m_hwndCabinetPage, WM_COMMAND, MAKELONG(IDC_3D_BTN_CREATE_CABINET, BN_CLICKED), (LPARAM)m_hWnd);
      InitCabinetLists();
   }
}

/******************************************************************************
* Definition : void OnChangeCabinet(WORD, WORD, HWND);
* Zweck      : Nachricht, dass das Gehäusesegment geändert wurde
* Aufruf     :
* Parameter (E): nID : IDC_CD_LST_CHASSIS_CAB                        (WORD)
*           (E): nNotifyCode: Code der Nachricht                     (WORD)
*   (0,...,nNoOfCabinets)    : dieses Gehäusesegment updaten
*   NOTIFY_KILLFOCUS_CAB_SIZE: Maße des aktuellen Gehäuses geändert
*           (E): hwndFrom: Fenster, dass die Nachricht gesendet hat  (HWDN)
* Funktionswert :
******************************************************************************/
void C3DViewDlg::OnChangeCabinet(WORD /*nID*/, WORD nNotifyCode, HWND /*hwndFrom*/)
{
   if (nNotifyCode == NOTIFY_KILLFOCUS_CAB_SIZE)
   {
      if ((m_GLView.m_nScaleModeX     & SCALE_SIDES) || 
          (m_GLView.m_nScaleModeY     & SCALE_SIDES) ||
          (m_GLView.m_nScaleModeZ     & SCALE_SIDES) ||
          (m_GLView.m_nScaleModePhi   & SCALE_SIDES) ||
          (m_GLView.m_nScaleModeTheta & SCALE_SIDES))
      {
         m_GLView.OnChangeCabinet();
         m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
         UpdateTransMode();
         m_GLView.InvalidateList(LIST_SCALE);
      }
      m_GLView.InvalidateList(LIST_CABINET+m_pPropertySheet->m_nCurrentCab);
   }
   else
   {
      InitCabinetData();
      m_GLView.m_nScale = 0;                                      // Bezugsquader meistens Basisgehäuse
      m_GLView.OnChangeCabinet();                                 // Parameter im OpenGLView updaten
//      m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
      UpdateTransMode(true);                                      // Controls updaten
      m_GLView.InvalidateList(LIST_CABINET+m_pPropertySheet->m_nCurrentCab);
      m_GLView.InvalidateList(LIST_CABINET+nNotifyCode);          // Listen Updaten
      m_GLView.InvalidateList(LIST_SCALE);                        // Listen Updaten
   }
}

/******************************************************************************
* Definition :void ;InitCabinetData()
* Zweck      : Initialisiert die Controls, wenn das Gehäusegment geändert wird
******************************************************************************/
void C3DViewDlg::InitCabinetData()
{
   CCabinet *pC   = m_pPropertySheet->GetCurrentCab();
   int       nCab = m_pPropertySheet->m_nCurrentCab;
   bool bNameReadOnly            = false;
   bool bEnableCanContainChassis = true;
   bool bDeleteCabinet           = true;

   if (pC->IsBaseCabinet())                                    // Das Basisgehäuse
   {
      bEnableCanContainChassis = false;                        // Es darf immer Chassis enthalten
      bNameReadOnly  = true;                                   // Der Name wird nicht geändert
      bDeleteCabinet = false;                                  // das Basisgehäuse darf nicht gelöscht werden
   }
   else
   {
      if (pC->m_dwFlags & CANNOT_CONTAIN_CHASSIS)              // darf der Quader keine Chassis enthalten
      {
         bEnableCanContainChassis = false;                     // darf das Flag CAN_CONTAIN_CHASSIS nicht geändert werden
         ASSERT(pC->CanContainChassis() == false);             // das Flag "darf nicht" ist dominant !
      }
      if (pC->nCountHT)                                        // enthält eine Kugel
      {
         bEnableCanContainChassis = false;                     // darf das Flag CAN_CONTAIN_CHASSIS nicht geändert werden
//         bNameReadOnly = true;                                 // Bei Kugeln wird der Text nicht geändert
         bDeleteCabinet = false;                               // Der Kugelabstandhalter darf nicht gelöscht werden
         m_pPropertySheet->m_nNoMovements = pC->nCountAllChassis;// Position wird von der Kugel bestimmt.
      }
      if (pC->nCountTL)
      {
         bEnableCanContainChassis = false;                     // darf das Flag CAN_CONTAIN_CHASSIS nicht geändert werden
//         bNameReadOnly = true;                               // Bei Füßen wird der Text nicht geändert
         bDeleteCabinet = false;                               // Der Fuß darf nicht gelöscht werden
      }
   }

   for (int i=0; i<NO_OF_WALLS; i++)
   {
      int nCount = 0;
      m_pPropertySheet->m_ChassisData.ProcessWithObjects(ChassisData::CountChassisOnWall, (WPARAM)&nCount, MAKELONG(i+1, nCab));
      ::EnableWindow(GetDlgItem(IDC_3D_CK_TOP_WALL+i), (nCount == 0) ? true : false);
      if (nCount)
      {
         bEnableCanContainChassis = false;                     // darf das Flag CAN_CONTAIN_CHASSIS nicht geändert werden
         bDeleteCabinet = false;
         ASSERT(pC->CanContainChassis() == true);              // es muß Chassis enthalten dürfen
      }
      if (g_nUserMode & USERMODE_WALLS)
      {
         ::CheckDlgButton(m_hWnd, IDC_3D_CK_TOP_WALL+i, pC->IsWallEnabled(CARABOX_CABWALL_TOP+i) ? MF_CHECKED:MF_UNCHECKED);
      }
   }

   ::EnableWindow(GetDlgItem(IDC_3D_BTN_DELETE_CABINET), bDeleteCabinet);
   ::EnableWindow(GetDlgItem(IDC_3D_CK_CAN_CONTAIN_CHASSIS), bEnableCanContainChassis);
   ::CheckDlgButton(m_hWnd, IDC_3D_CK_CAN_CONTAIN_CHASSIS, pC->CanContainChassis() ? MF_CHECKED:MF_UNCHECKED);
   SendDlgItemMessage(IDC_3D_LST_CAB, CB_SETCURSEL, m_pPropertySheet->m_nCurrentCab, 0);
   if (pC->IsBaseCabinet())
   {
      char szText[CAB_NAME_LEN*2];
      ::LoadString(m_hInstance, IDS_TOTAL_CABINET, szText, CAB_NAME_LEN);
      strncat(szText, pC->m_szName, CAB_NAME_LEN*2);
      SetDlgItemText(IDC_3D_EDT_CAB_NAME, szText);
   }
   else
   {
      SetDlgItemText(IDC_3D_EDT_CAB_NAME, pC->m_szName);
   }
   SendDlgItemMessage(IDC_3D_EDT_CAB_NAME, EM_SETREADONLY, bNameReadOnly, 0);
}

/******************************************************************************
* Definition : void OnClickXbutton(HWND);
* Zweck      : Auswertung OnClickBtn()
******************************************************************************/
void C3DViewDlg::OnClickXbutton(HWND hwnd)
{
   SetStatusText(GetWindowContextHelpId(hwnd), NULL);
   switch (m_GLView.m_nTransMode)
   {
      case TRANS_MOVE:
         OnClickXScale();
         return;
      case TRANS_TURNLIGHT:
      {
         double *pV = GetTransModeVar();
         if (fmod(pV[0], 15.0) != 0.0) pV[0]  = 0.0;
         else                          pV[0] += 15.0;
         m_GLView.m_bChanged[0] = true;
      }break;
      case TRANS_TURN:
      if (m_pPropertySheet->m_nCurrentCab)
      {
         OnClickPhiScale();
         return;
      }
      else
      {
         double *pV = GetTransModeVar();
         if (fmod(pV[0], 15.0) != 0.0) pV[0]  = 0.0;
         else                          pV[0] += 15.0;
         m_GLView.m_bChanged[0] = true;
      }break;
      case TRANS_SETTINGS: OnClickAlphaBlend(); break;
      case TRANS_LIGHT_INTENS: OnClickLightColor(); break;
      case TRANS_ZOOM:
      case TRANS_LIGHT_SPOT:
         break;
   }
   if (m_GLView.m_bChanged[0])
   {
      UpdateTransMode();
   }
}

/******************************************************************************
* Definition : void OnClickYbutton(HWND);
* Zweck      : Auswertung OnClickBtn()
******************************************************************************/
void C3DViewDlg::OnClickYbutton(HWND hwnd)
{
   SetStatusText(GetWindowContextHelpId(hwnd), NULL);
   switch (m_GLView.m_nTransMode)
   {
      case TRANS_MOVE:
         OnClickYScale();
         return;
      case TRANS_TURN:
      if (m_pPropertySheet->m_nCurrentCab == 0)
      {
         double *pV = GetTransModeVar();
         if (fmod(pV[1], 15) != 0.0) pV[1]  = 0;
         else                        pV[1] += 15.0;
         if (pV[1] > 90.0) pV[1] = -90.0;
         m_GLView.m_bChanged[1] = true;
      }break;
      case TRANS_TURNLIGHT:
      {
         double *pV = GetTransModeVar();
         if (fmod(pV[1], 15) != 0.0) pV[1]  = 0;
         else                        pV[1] += 15.0;
         if (pV[1] > 90.0) pV[1] = -90.0;
         m_GLView.m_bChanged[1] = true;
      }break;
      case TRANS_ZOOM:
      {
//         m_GLView.m_dVolumeXY = 
      }break;
      case TRANS_SETTINGS: OnClickPerspective(); break;
      case TRANS_LIGHT_INTENS: OnClickAmbientColor(); break;
      case TRANS_LIGHT_SPOT:
      {
         double *pV = GetTransModeVar();
         if (fmod(pV[1], 15) != 0.0) pV[1]  = 15;
         else                        pV[1] += 15.0;
         if (pV[1] == 195) pV[1] = 360;
         else if (pV[1] >= 360) pV[1] = 15;
         m_GLView.m_bChanged[1] = true;
      }break;
   }
   if (m_GLView.m_bChanged[1])
   {
      UpdateTransMode();
   }
}

/******************************************************************************
* Definition : void OnClickZbutton(HWND);
* Zweck      : Auswertung OnClickBtn()
******************************************************************************/
void C3DViewDlg::OnClickZbutton(HWND hwnd)
{
   SetStatusText(GetWindowContextHelpId(hwnd), NULL);
   switch (m_GLView.m_nTransMode)
   {
      case TRANS_MOVE:
         OnClickZScale();
         return;
      case TRANS_LIGHT_SPOT:
         m_GLView.m_bLighting = !m_GLView.m_bLighting;
         m_GLView.m_bChanged[2] = true;
         UpdateCtrlStates();
         break;
      case TRANS_TURN:
      {
      }break;
      case TRANS_LIGHT_INTENS: OnClickSpecularColor(); break;
      case TRANS_ZOOM:
      case TRANS_TURNLIGHT:
         break;
   }
   if (m_GLView.m_bChanged[2])
   {
      UpdateTransMode();
   }
}

/******************************************************************************
* Definition : void ModifyDlgItemStyle(int, DWORD, DWORD);
* Zweck      : Ändern eine Controlstyles eines Dialogelementes
* Parameter (E): nID: ID des Dialog Ctrls                              (int)
*           (E): dwAdd   : Flag, das gesetzt werden soll               (DWORD)
*           (E): dwRemove: Flag, das entfernt werden soll              (DWORD)
******************************************************************************/
void C3DViewDlg::ModifyDlgItemStyle(int nID, DWORD dwAdd, DWORD dwRemove)
{
   HWND hwnd = GetDlgItem(nID);
   if (hwnd)
   {
      DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
      dwStyle &= ~dwRemove;
      dwStyle |= dwAdd;
      ::SetWindowLong(hwnd, GWL_STYLE, dwStyle);
   }
}

/******************************************************************************
* Definition : static LRESULT CALLBACK ButtonSubClass(HWND, UINT, WPARAM, LPARAM)
* Zweck      : Messagefilter für Buttons. Ändert die Nachricht WM_RBUTTONUP
*              in WM_COMMAND mit BN_DBLCLCK-Notification.
* Aufruf     : als MsgProc
* Parameter (E): siehe CallWindowProc(..)
* Funktionswert : dito
******************************************************************************/
LRESULT CALLBACK C3DViewDlg::ButtonSubClass(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (uMsg == WM_RBUTTONUP)
   {
      ::SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(hwnd), BN_DBLCLK), (LPARAM)hwnd);
      return 0;
   }
   else if ((uMsg == WM_KEYDOWN) && (wParam == VK_TAB))
   {
      ::SendMessage(GetParent(hwnd), WM_TAB_CTRL, (WPARAM)GetDlgCtrlID(hwnd), (LPARAM)hwnd);
   }

   return CallWindowProc((WNDPROC)GetWindowLong(hwnd, GWL_USERDATA), hwnd, uMsg, wParam, lParam);
}

/*********************************************************************************
* Definition : static LRESULT CALLBACK ButtonSubClass(HWND, UINT, WPARAM, LPARAM)
* Zweck      : Messagefilter für EditCtrls. Filter aus der Nachricht WM_KEYDOWN
*              die Entertaste und sendet die EN_KILLFOCUS Notification zur Werte-
*              übernahme.
* Aufruf     : als MsgProc
* Parameter (E): siehe CallWindowProc(..)
* Funktionswert : dito
******************************************************************************/
LRESULT CALLBACK C3DViewDlg::EditSubClass(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (uMsg == WM_KEYDOWN)
   {
      if (wParam == VK_RETURN)
      {
         ::SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(hwnd), EN_KILLFOCUS), (LPARAM)hwnd);
         return 0;
      }
      else if (wParam == VK_TAB)
      {
         ::SendMessage(GetParent(hwnd), WM_TAB_CTRL, (WPARAM)GetDlgCtrlID(hwnd), (LPARAM)hwnd);
         return 0;
      }
   }
   return CallWindowProc((WNDPROC)GetWindowLong(hwnd, GWL_USERDATA), hwnd, uMsg, wParam, lParam);
}

/******************************************************************************
* Definition : void OnDblClickXbutton(HWND);
* Zweck      : Auswertung OnDblClickBtn()
******************************************************************************/
void C3DViewDlg::OnDblClickXbutton(HWND /*hwnd*/)
{
   switch (m_GLView.m_nTransMode)
   {
      case TRANS_MOVE: OnDblClickXScale(); break;
      case TRANS_TURN: OnDblClickPhiScale(); break;
      case TRANS_SETTINGS:
      if (g_nUserMode & USERMODE_ALPHA_CONTEXT)
      {
         HMENU hSub = NULL, hMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_BLENDFUNC));
         if (hMenu) hSub = ::GetSubMenu(hMenu, 0);
         if (hSub)
         {
            POINT pt;
            ::CheckMenuRadioItem(hSub, 0, 6, LOWORD(m_GLView.m_nAlphaBlend)-1, MF_BYPOSITION);
            ::CheckMenuItem(hSub, IDM_SET_INVERSE_DRAW_DIR, MF_BYCOMMAND|((m_GLView.m_nAlphaBlend & ALPHA_DRAW_DIR)?MF_CHECKED:MF_UNCHECKED));
            ::GetCursorPos(&pt);
            ::TrackPopupMenu(hSub, 0, pt.x, pt.y, 0, m_hWnd, NULL);
         }
      }break;
   }
}

/******************************************************************************
* Definition : void OnDblClickYbutton(HWND);
* Zweck      : Auswertung OnDblClickBtn()
******************************************************************************/
void C3DViewDlg::OnDblClickYbutton(HWND /*hwnd*/)
{
   switch (m_GLView.m_nTransMode)
   {
      case TRANS_MOVE: OnDblClickYScale(); break;
   }
}

/******************************************************************************
* Definition : void OnDblClickZbutton(HWND);
* Zweck      : Auswertung OnDblClickBtn()
******************************************************************************/
void C3DViewDlg::OnDblClickZbutton(HWND /*hwnd*/)
{
   switch (m_GLView.m_nTransMode)
   {
      case TRANS_MOVE: OnDblClickZScale(); break;
   }
}

/******************************************************************************
* Definition : void InitCabinetLists();
* Zweck      : Initialsiert die OpenGLlisten für die Gehäusesegmente
******************************************************************************/
void C3DViewDlg::InitCabinetLists()
{
   int nOldCount = SendDlgItemMessage(IDC_3D_LST_CAB, CB_GETCOUNT, 0, 0);
   int i, nCount = m_pPropertySheet->m_Cabinet.m_Cabinets.GetCounter();
   for (i=1; i<=nCount; i++)
   {
      if (!m_GLView.CreateCabinetList(i))                      // Neue Liste erzeugen
      {                                                        // Schon vorhanden ?
         m_GLView.PerformList(LIST_CABINET+i, true);
         m_GLView.InvalidateList(LIST_CABINET+i);              // Updaten !
      }
   }
   for (; i<=nOldCount; i++)
   {
      m_GLView.PerformList(LIST_CABINET+i, false);
      m_GLView.InvalidateList(LIST_CABINET+i);                 // Updaten !
   }
   UpdateVolume();
}

/******************************************************************************
* Definition : void UpdateVolume();
* Zweck      : Berechnet das Darstellungsvolumen für die aktuelle Objektgröße
*              und Invalidiert alle Darstellungslisten.
******************************************************************************/
void C3DViewDlg::UpdateVolume()
{
   double dMaxDim       = m_pPropertySheet->m_Cabinet.GetMaxDimension();
   double dDistLight    = dMaxDim*0.01;
   m_GLView.m_dVolumeXY = dMaxDim * 0.0009;
   m_GLView.m_dVolumeZ  = m_GLView.m_dVolumeXY * 20;
   m_GLView.m_dZoom     = 1.2;

   if ((m_GLView.m_dDistanceLight > 0.0) && (dDistLight > m_GLView.m_dDistanceLight))
   {
      m_GLView.m_dDistanceLight = dDistLight;
      m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
      if (TRANS_TURNLIGHT == m_GLView.m_nTransMode) UpdateTransMode();
      else m_GLView.InvalidateTransMode(true, TRANS_TURNLIGHT);
   }


   m_GLView.m_vTranslation.SetX() = 0;
   m_GLView.m_vTranslation.SetY() = 0;
#ifdef UNIT_METER
   m_GLView.m_vTranslation.SetZ() = -(m_GLView.m_dVolumeXY * 2.0);
#else
   m_GLView.m_vTranslation.SetZ() = -(m_GLView.m_dVolumeXY * 2000.0);
#endif

   if ((m_GLView.m_nTransMode == TRANS_MOVE) && (m_pPropertySheet->m_nCurrentCab == 0))
   {
      m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
      UpdateTransMode();
   }
   else m_GLView.InvalidateList(LIST_MATRIX);

   m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
   if (m_GLView.m_nTransMode == TRANS_ZOOM) UpdateTransMode();
   else m_GLView.InvalidateTransMode(true, TRANS_ZOOM);

   m_GLView.SetProjection();
   m_GLView.InvalidateAllLists();
}

/******************************************************************************
* Definition : void OnCabLstContentChanged(HWND );
* Zweck      : Erneuert den Inhalt der Slaveliste für die Gehäusesegmente
* Parameter (E): hwndControl : Windowhadle der Masterliste           (HWND)
******************************************************************************/
void C3DViewDlg::OnCabLstContentChanged(HWND hwndControl)
{
   SendDlgItemMessage(IDC_3D_LST_CAB, CB_RESETCONTENT, 0, 0);  // Inhalt der Slave-Liste löschen
   int i, nCount = ::SendMessage(hwndControl, CB_GETCOUNT, 0, 0);
   char szText[CAB_NAME_LEN];
   for (i=0; i<nCount; i++)                                    // Inhalt der Master-Liste
   {                                                           // in die Slave Liste übertragen
      ::SendMessage(hwndControl, CB_GETLBTEXT, i, (LPARAM)szText);
      SendDlgItemMessage(IDC_3D_LST_CAB, CB_ADDSTRING, 0, (LPARAM)szText);
   }
   SendDlgItemMessage(IDC_3D_LST_CAB, CB_SETCURSEL, m_pPropertySheet->m_nCurrentCab, 0);
}

/******************************************************************************
* Definition : void OnDeleteCabinet();
* Zweck      : Löscht ein Gehäusesegment und passt die Segmentnummern für die
*              übrigen Segmente und Chassis auf den Segmenten neu an.
******************************************************************************/
void C3DViewDlg::OnDeleteCabinet()
{
   char szFormat[64], szText[128];
   ::LoadString(m_hInstance, IDS_DELETE_CAB, szFormat, 64);
   CCabinet*pCdel = m_pPropertySheet->GetCurrentCab();
   wsprintf(szText, szFormat, pCdel->m_szName);
   if ((pCdel != NULL) && !pCdel->IsBaseCabinet() &&
       (MessageBox((long)szText, IDS_PROPSHCAPTION, MB_YESNO|MB_ICONQUESTION|MBU_TEXTSTRING)==IDYES))
   {
      int nNo = 1;
      CCabinet*pCbase = m_pPropertySheet->GetCabinet(BASE_CABINET);// alle Chassis von dem aktuellen Segment entfernen
      m_pPropertySheet->m_ChassisData.ProcessWithObjects(ChassisData::ChangeCabinetTo, pCdel->nCountAllChassis, TRUE);
      pCbase->m_Cabinets.Remove(pCdel);                        // aktuelles Segment entfernen
      CCabinet*pC = (CCabinet*) pCbase->m_Cabinets.GetFirst();
      while (pC)                                               // Liste durchsuchen
      {
         if (pC->nCountAllChassis != nNo)                      // ist die Segmentnummer falsch ?
         {                                                     // Chassisgehäusesegmentnummer anpassen
            m_pPropertySheet->m_ChassisData.ProcessWithObjects(ChassisData::ChangeCabinetTo, MAKELONG(pC->nCountAllChassis, nNo), FALSE);
            pC->nCountAllChassis = nNo;                        // Segmentnummer setzen
         }
         pC = (CCabinet*) pCbase->m_Cabinets.GetNext();
         nNo++;                                                // Segmente sind durchnummeriert
      }
      m_GLView.PerformList(LIST_CABINET+nNo, false);           // letzte GL-Liste disablen
      // CabinetPage benachrichtigen, daß die Gehäusesegmentliste ungültig ist
      ::SendMessage(m_hwndCabinetPage, WM_COMMAND, MAKELONG(IDC_CD_LST_CHASSIS_CAB, CBN_CONTENT_CHANGED), (LPARAM)m_hWnd);
      SendDlgItemMessage(IDC_3D_LST_CAB, CB_SETCURSEL, 0, 0);  // Basisgehäuse auswählen
      ::SendMessage(m_hWnd, WM_COMMAND, MAKELONG(IDC_3D_LST_CAB, CBN_SELCHANGE), (LPARAM)m_hWnd);
      UpdateVolume();                                          // Alles updaten
      delete pCdel;
   }
}

/******************************************************************************
* Definition : void OnCheckWall(int);
* Zweck      : Enablen bzw. disablen von Wänden in einem Segment.
* Parameter (E): nWall: Wandnummer (1,..,6)                          (int)
******************************************************************************/
void C3DViewDlg::OnCheckWall(int nWall)
{
   CCabinet*pC = m_pPropertySheet->GetCurrentCab();
   if (pC->IsWallEnabled(nWall))
   {
      pC->EnableWall(nWall, false);
      m_GLView.InvalidateList(LIST_CABINET+m_pPropertySheet->m_nCurrentCab);
   }
   else
   {
      pC->EnableWall(nWall, true);
      m_GLView.InvalidateList(LIST_CABINET+m_pPropertySheet->m_nCurrentCab);
   }
}

/******************************************************************************
* Definition : void OnCheckCanContainChassis();
* Zweck      : Legt fest, ob das Gehäuse Chassis enthalten darf
******************************************************************************/
void C3DViewDlg::OnCheckCanContainChassis()
{
   CCabinet*pC = m_pPropertySheet->GetCurrentCab();
   if (pC->m_dwFlags & CAN_CONTAIN_CHASSIS)
   {
      pC->m_dwFlags &= ~CAN_CONTAIN_CHASSIS;
   }
   else
   {
      pC->m_dwFlags |= CAN_CONTAIN_CHASSIS;
   }
}

/******************************************************************************
* Definition : void NotifyCabPageSegmentMoved();
* Zweck      : Benachrichtigt die Cabinetpage, das ein Segment bewegt wurde.
******************************************************************************/
void C3DViewDlg::NotifyCabPageSegmentMoved()
{
   if (m_pPropertySheet->m_nCurrentCab != 0)
   {
      int nCode = 0;
      switch (m_GLView.m_nTransMode)
      {
         case TRANS_MOVE:
         case TRANS_TURN:
            nCode = VIEW2DN_SEGMENT_MOVED;
            break;
      }
      ::SendMessage(m_hwndCabinetPage, WM_COMMAND, MAKELONG(IDC_CD_POSITION_VIEW, nCode), (LPARAM)m_hWnd);
   }
}

/******************************************************************************
* Definition : bool SetMenuItem(UINT, UINT, UINT, HMENU);
* Zweck      : Updaten des Menüs in der Dialogbox
* Parameter (E): nItem: ID des Menüpunktes                           (UINT)
*           (E!) nItem: ID des radiochecked Menüpunktes              (UINT)
*           (E): nCheck: Checken des Menüpunktes                     (UINT)
*                (MF_CHECKED, MF_UNCHECKED)
*                (MFT_RADIOCHECK): Sonderfall; Achtung andere Parameter !!!
*                (INVALID_VALUE): nicht (un)checken
*           (E): nEnable: Enablen des Menüpunktes                    (UINT)
*                (MF_ENABLE, MF_GRAYED, MF_DISABLED)
*                (INVALID_VALUE): nicht enablen/disablen
*           (E!) nEnable: (int*)pnRange;                            (int[2])
*                pnRage[0]: Untergrenze
*                pnRage[1]: Obergrenze
*           (E): [hMenu]: Menühandle zur rekursiven Bearbeitung      (HMENU)
*                aller Untermenüs
* Funktionswert : Menupunkt ID enthalten                             (bool)
******************************************************************************/
bool C3DViewDlg::SetMenuItem(UINT nItem, UINT nCheck, UINT nEnable, HMENU hMenu)
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

void C3DViewDlg::OnClickXScale()
{
   if (m_GLView.m_nScaleModeX & SHOW_SCALE)
   {
      m_GLView.m_nScaleModeX &= ~SHOW_SCALE;
   }
   else if (m_pPropertySheet->m_nCurrentCab)
   {
      m_GLView.SetScalesOff();
      m_GLView.m_nScaleModeX |= SHOW_SCALE;
   }
   UpdateCtrlStates();
   m_GLView.InvalidateList(LIST_SCALE);
   ::SetFocus(GetDlgItem(IDC_3D_EDT_TRAN_X));
}

void C3DViewDlg::OnClickYScale()
{
   if (m_GLView.m_nScaleModeY & SHOW_SCALE)
   {
      m_GLView.m_nScaleModeY &= ~SHOW_SCALE;
   }
   else if (m_pPropertySheet->m_nCurrentCab)
   {
      m_GLView.SetScalesOff();
      m_GLView.m_nScaleModeY |=  SHOW_SCALE;
   }
   UpdateCtrlStates();
   m_GLView.InvalidateList(LIST_SCALE);
   ::SetFocus(GetDlgItem(IDC_3D_EDT_TRAN_Y));
}

void C3DViewDlg::OnClickZScale()
{
   if (m_GLView.m_nScaleModeZ & SHOW_SCALE)
   {
      m_GLView.m_nScaleModeZ &= ~SHOW_SCALE;
   }
   else if (m_pPropertySheet->m_nCurrentCab)
   {
      m_GLView.SetScalesOff();
      m_GLView.m_nScaleModeZ |=  SHOW_SCALE;
   }
   UpdateCtrlStates();
   m_GLView.InvalidateList(LIST_SCALE);
   ::SetFocus(GetDlgItem(IDC_3D_EDT_TRAN_Z));
}

void C3DViewDlg::OnClickPhiScale()
{
   if (m_GLView.m_nScaleModePhi & SHOW_SCALE)
   {
      m_GLView.m_nScaleModePhi &= ~SHOW_SCALE;
   }
   else if (m_pPropertySheet->m_nCurrentCab)
   {
      m_GLView.SetScalesOff(false);
      m_GLView.m_nScaleModePhi |= SHOW_SCALE;
      m_GLView.m_dPhi   = 0;
      m_GLView.m_dTheta =  90;
      m_GLView.InvalidateList(LIST_MATRIX);
   }
   UpdateCtrlStates();
   m_GLView.InvalidateList(LIST_SCALE);
   ::SetFocus(GetDlgItem(IDC_3D_EDT_TRAN_X));
}

void C3DViewDlg::OnDblClickXScale()
{
   if (m_pPropertySheet->m_nCurrentCab)
   {
      CAutoDisable ad(m_pPropertySheet->GetHWND());
      CScaleDlg dlg(m_hInstance, IDD_SCALE, m_hWnd);
      dlg.m_nWhat      = SCALE_WIDTH;
      dlg.m_nScaleMode = m_GLView.m_nScaleModeX;
      dlg.m_nScale     = m_GLView.m_nScale;
      if ( (dlg.DoModal() == IDOK) && 
          ((m_GLView.m_nScaleModeX != dlg.m_nScaleMode) ||
           (m_GLView.m_nScale      != dlg.m_nScale)))
      {
         if (m_GLView.m_nScale != dlg.m_nScale)
            m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
         else
            m_GLView.m_bChanged[0] = true;
         SetStatusText(0, dlg.m_szText);
         m_GLView.m_nScaleModeX = dlg.m_nScaleMode;
         m_GLView.m_nScale      = dlg.m_nScale;
         m_GLView.InvalidateList(LIST_SCALE);
         m_GLView.OnChangeCabinet();
         UpdateCtrlStates();
         UpdateTransMode();
      }
   }
}

void C3DViewDlg::OnDblClickYScale()
{
   if (m_pPropertySheet->m_nCurrentCab)
   {
      CAutoDisable ad(m_pPropertySheet->GetHWND());
      CScaleDlg dlg(m_hInstance, IDD_SCALE, m_hWnd);
      dlg.m_nWhat = SCALE_HEIGHT;
      dlg.m_nScaleMode = m_GLView.m_nScaleModeY;
      dlg.m_nScale     = m_GLView.m_nScale;
      if ( (dlg.DoModal() == IDOK) && 
          ((m_GLView.m_nScaleModeY != dlg.m_nScaleMode) ||
           (m_GLView.m_nScale      != dlg.m_nScale)))
      {
         if (m_GLView.m_nScale != dlg.m_nScale)
            m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
         else
            m_GLView.m_bChanged[1] = true;
         SetStatusText(0, dlg.m_szText);
         m_GLView.m_nScaleModeY = dlg.m_nScaleMode;
         m_GLView.m_nScale      = dlg.m_nScale;
         m_GLView.InvalidateList(LIST_SCALE);
         m_GLView.OnChangeCabinet();
         UpdateCtrlStates();
         UpdateTransMode();
      }
   }
}

void C3DViewDlg::OnDblClickZScale()
{
   if (m_pPropertySheet->m_nCurrentCab)
   {
      CAutoDisable ad(m_pPropertySheet->GetHWND());
      CScaleDlg dlg(m_hInstance, IDD_SCALE, m_hWnd);
      dlg.m_nWhat = SCALE_DEPTH;
      dlg.m_nScaleMode = m_GLView.m_nScaleModeZ;
      dlg.m_nScale     = m_GLView.m_nScale;
      if ( (dlg.DoModal() == IDOK) &&
          ((m_GLView.m_nScaleModeZ != dlg.m_nScaleMode) ||
           (m_GLView.m_nScale      != dlg.m_nScale)))
      {
         if (m_GLView.m_nScale != dlg.m_nScale)
            m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
         else
            m_GLView.m_bChanged[2] = true;
         SetStatusText(0, dlg.m_szText);
         m_GLView.m_nScaleModeZ = dlg.m_nScaleMode;
         m_GLView.m_nScale      = dlg.m_nScale;
         m_GLView.InvalidateList(LIST_SCALE);
         m_GLView.OnChangeCabinet();
         UpdateCtrlStates();
         UpdateTransMode();
      }
   }
}

void C3DViewDlg::OnDblClickPhiScale()
{
   if (m_pPropertySheet->m_nCurrentCab)
   {
      CAutoDisable ad(m_pPropertySheet->GetHWND());
      CScaleDlg dlg(m_hInstance, IDD_SCALE, m_hWnd);
      dlg.m_nWhat      = SCALE_ANGLE_PHI;
      dlg.m_nScaleMode = m_GLView.m_nScaleModePhi;
      dlg.m_nScale     = m_GLView.m_nScale;
      if ( (dlg.DoModal() == IDOK) && 
          ((m_GLView.m_nScaleModePhi != dlg.m_nScaleMode) ||
           (m_GLView.m_nScale        != dlg.m_nScale)))
      {
         if (m_GLView.m_nScale != dlg.m_nScale)
            m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
         else
            m_GLView.m_bChanged[0] = true;
         SetStatusText(0, dlg.m_szText);
         m_GLView.m_nScaleModePhi = dlg.m_nScaleMode;
         m_GLView.m_nScale        = dlg.m_nScale;
         m_GLView.InvalidateList(LIST_SCALE);
         m_GLView.OnChangeCabinet();
         UpdateCtrlStates();
         UpdateTransMode();
      }
   }
}

void C3DViewDlg::OnClickLightColor()
{
   COLORREF cColor = CGlLight::FloatsToColor(m_GLView.m_Light.m_pfAmbientColor);
   char *pszHeading = LoadString(IDS_LIGHT_COLOR);
   if (ChooseColorT(pszHeading, cColor, m_hWnd, m_hInstance) == IDOK)
   {
      m_GLView.m_Light.SetAmbientColor(cColor);
      m_GLView.m_dLightIntens[0] = m_GLView.m_Light.m_pfAmbientColor[0]*100;
      m_GLView.m_dLightIntens[1] = m_GLView.m_Light.m_pfAmbientColor[1]*100;
      m_GLView.m_dLightIntens[2] = m_GLView.m_Light.m_pfAmbientColor[2]*100;
      m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
      if (m_GLView.m_nTransMode == TRANS_LIGHT_INTENS) UpdateTransMode();
      else m_GLView.InvalidateTransMode(true, TRANS_LIGHT_INTENS);
   }
   free((void*)pszHeading);
}

void C3DViewDlg::OnClickAmbientColor()
{
   float fColor[3] = {(float)m_GLView.m_AmbientColor[0], (float)m_GLView.m_AmbientColor[1], (float)m_GLView.m_AmbientColor[2]};
   COLORREF cColor = CGlLight::FloatsToColor(fColor);
   char *pszHeading = LoadString(IDS_LIGHT_COLOR_D);
   if (ChooseColorT(pszHeading, cColor, m_hWnd, m_hInstance) == IDOK)
   {
      CGlLight::ColorToFloat(cColor, fColor);
      m_GLView.m_AmbientColor[0] = fColor[0];
      m_GLView.m_AmbientColor[1] = fColor[1];
      m_GLView.m_AmbientColor[2] = fColor[2];
      m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
      if (m_GLView.m_nTransMode == TRANS_LIGHT_INTENS) UpdateTransMode();
      else m_GLView.InvalidateTransMode(true, TRANS_LIGHT_INTENS);
   }
   free((void*)pszHeading);
}

void C3DViewDlg::OnClickSpecularColor()
{
   COLORREF cColor = CGlLight::FloatsToColor(m_GLView.m_Light.m_pfSpecularColor);
   char *pszHeading = LoadString(IDS_LIGHT_COLOR_S);
   if (ChooseColorT(pszHeading, cColor, m_hWnd, m_hInstance) == IDOK)
   {
      m_GLView.m_Light.SetSpecularColor(cColor);
      m_GLView.m_bChanged[0] = m_GLView.m_bChanged[1] = m_GLView.m_bChanged[2] = true;
      if (m_GLView.m_nTransMode == TRANS_LIGHT_INTENS) UpdateTransMode();
      else m_GLView.InvalidateTransMode(true, TRANS_LIGHT_INTENS);
   }
   free((void*)pszHeading);
}

void C3DViewDlg::OnClickAlphaBlend()
{
   if (m_GLView.m_nAlphaBlend&ALPHA_BLENDING) m_GLView.m_nAlphaBlend &= ~ALPHA_BLENDING;
   else                                       m_GLView.m_nAlphaBlend |=  ALPHA_BLENDING;
   m_GLView.m_bChanged[0] = true;
   UpdateCtrlStates();
   m_GLView.InvalidateTransMode(true, TRANS_SETTINGS);
}

void C3DViewDlg::OnClickPerspective()
{
   m_GLView.m_bPerspective = !m_GLView.m_bPerspective;
   m_GLView.m_bChanged[1] = true;
   UpdateCtrlStates();
   m_GLView.InvalidateTransMode(true, TRANS_SETTINGS);
}

void C3DViewDlg::SetStatusText(int nID, char *pszText)
{
   HWND hwnd = GetDlgItem(IDC_3D_TXT_STATUS);
   if (hwnd && ::IsWindowVisible(hwnd))
   {
      bool bAlloc = false;
      if ((nID != 0) && (nID != ::GetWindowLong(hwnd, GWL_USERDATA)))
      {
         pszText = LoadString(nID);
         bAlloc = true;
      }
      if (pszText)
      {
         if (bAlloc)
         {
            int i, nLen = strlen(pszText);
            for (i=0; i<nLen; i++)
            {
               if (pszText[i] == '\n') pszText[i] = ' ';
               if (pszText[i] == '\t') pszText[i] = ' ';
            }
         }
         ::SetWindowText(hwnd, pszText);
         if (bAlloc) free(pszText);
      }
      else if (nID == 0) ::SetWindowText(hwnd, "");
      ::SetWindowLong(hwnd, GWL_USERDATA, nID);
   }
}

