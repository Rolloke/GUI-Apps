/********************************************************************/
/* Funktionen der Klasse CBoxCabinetDesignPage                      */
/********************************************************************/

#include "BoxCabinetDesign.h"
#include "BoxPropertySheet.h"
#include "resource.h"
#include "AfxRes.h"
#include "Debug.h"
#include "CSortFunctions.h"
#include "Cara3DTr.h"
#include "CEtsDiv.h"
#include "C3DViewDlg.h"
#include <stdio.h>
#include "BOXGLOBAL.H"

#ifdef _DEBUG_HEAP
 #ifdef Alloc
  #undef Alloc
 #endif
 #define Alloc(p1, p2) Alloc(p1, p2, __FILE__, __LINE__)
 #define new DLL_DEBUG_NEW
#endif

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define CCD_CTRL_ID      0
#define CCD_INDEX_TOTAL  1
#define CCD_INDEX_OTHER  2

#define PT_UN_PICKED   -100000

#define POSITION_VIEW "PositionView"

extern HINSTANCE g_hInstance;

const short CBoxCabinetDesignPage::gm_nCabinetSizes[CARABOX_CABSIZES][3] = 
{
   {IDC_CD_EDT_HEIGHT, 0, 0},    // 0
   {IDC_CD_EDT_WIDTH , 3, 4},    // 1
   {IDC_CD_EDT_DEPTH , 5, 6},    // 2
   {IDC_CD_EDT_WIDTH1, 1, 4},    // 3
   {IDC_CD_EDT_WIDTH2, 1, 3},    // 4
   {IDC_CD_EDT_DEPTH1, 2, 6},    // 5
   {IDC_CD_EDT_DEPTH2, 2, 5}     // 6
};

LONG CBoxCabinetDesignPage::gm_lOldPositionViewSubClassProc = NULL;

CBoxCabinetDesignPage::CBoxCabinetDesignPage()
{
   m_pCurrent        = NULL;
   m_bViewFocus      = false;
   m_nWall           = 0;
   m_hWndPosView     = NULL;
   m_rcView.left     = 0;
   m_rcView.right    = 1;
   m_rcView.top      = 1;
   m_rcView.bottom   = 0;
   m_pptSegments     = NULL;
   m_pvSegments      = NULL;
   m_nSizeOfSegmentArray = 0;
   m_ptOld.x = m_ptOld.y = PT_UN_PICKED;
}

CBoxCabinetDesignPage::~CBoxCabinetDesignPage()
{
   if (m_pptSegments) Free(m_pptSegments);
   if (m_pvSegments)  Free(m_pvSegments);
}

LRESULT CALLBACK CBoxCabinetDesignPage::PositionViewSubClassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   CBoxCabinetDesignPage *pCDP = (CBoxCabinetDesignPage*) ::GetProp(hwnd, POSITION_VIEW);
   if (pCDP)
   {
      switch (uMsg)
      {
         case WM_PAINT:
         {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (hdc)
            {
               ::SaveDC(hdc);
               HRGN hrgn  = ::CreateRectRgn(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
               HBRUSH hBrBkGnd;
               if (::GetFocus() == hwnd) hBrBkGnd= ::GetSysColorBrush(COLOR_INFOBK);
               else                      hBrBkGnd = (HBRUSH) ::GetStockObject(WHITE_BRUSH);
               ::SelectObject(hdc, hBrBkGnd);
               ::FillRect(hdc, &ps.rcPaint, hBrBkGnd);
               ::SelectClipRgn(hdc, hrgn);
               pCDP->OnPosViewPrepareDC(hdc);
               pCDP->OnPosViewDraw(hdc, &ps);
               ::RestoreDC(hdc, -1);
               if (hrgn) ::DeleteObject(hrgn);
            }
            EndPaint(hwnd, &ps);
         } return 0;
         case WM_ERASEBKGND:
            return 1;
         case WM_MOUSEMOVE:
            pCDP->OnPosViewMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));
            return 0;
         case WM_LBUTTONDOWN:
            pCDP->OnPosViewLBtnDown(wParam, LOWORD(lParam), HIWORD(lParam));
            return 0;
         case WM_LBUTTONUP:
            pCDP->OnPosViewLBtnUp(wParam, LOWORD(lParam), HIWORD(lParam));
            return 0;
         case WM_KEYDOWN:
            return pCDP->OnPosViewKeyDown(wParam, lParam);
         case WM_KILLFOCUS:
            if (pCDP->m_pCurrent != NULL)
            {
               pCDP->CheckCurrentChassisPosition();
               pCDP->InitChassisPos();
            }
         case WM_SETFOCUS:
            ::InvalidateRect(hwnd, NULL, true);
            return 0;
         case WM_GETDLGCODE:
         if (::GetFocus() == hwnd) return DLGC_WANTALLKEYS;
         else                      return 0;
         case WM_DESTROY:
         {
            ::RemoveProp(hwnd, POSITION_VIEW);
            ::SetWindowLong(hwnd, GWL_WNDPROC, gm_lOldPositionViewSubClassProc);
            gm_lOldPositionViewSubClassProc = NULL;
            return 0;
         }
         default: break;
      }
   }
   return CallWindowProc((WNDPROC)gm_lOldPositionViewSubClassProc, hwnd, uMsg, wParam, lParam);
}

bool CBoxCabinetDesignPage::OnInitDialog(HWND hWnd)
{
   if (CEtsPropertyPage::OnInitDialog(hWnd))
   {
      m_pptSegments         = (POINT*)Alloc(sizeof(POINT)*NO_OF_RECT_POINTS, NULL);
      m_nSizeOfSegmentArray = NO_OF_RECT_POINTS;
      m_hWndPosView = GetDlgItem(IDC_CD_POSITION_VIEW);
      gm_lOldPositionViewSubClassProc = ::SetWindowLong(m_hWndPosView, GWL_WNDPROC, (LONG)PositionViewSubClassProc);
      ::SetProp(m_hWndPosView, POSITION_VIEW, (HANDLE)this);
      ::EnumChildWindows(hWnd, SetHelpIDs, (LPARAM)hWnd);
      CBoxPropertySheet::SetPaintCtrlWnd(m_hWnd, IDC_CD_GRP_CABINET_PARAM, IDC_CD_GRP_WIDTH_CTRL);
      CBoxPropertySheet::SetPaintCtrlWnd(m_hWnd, IDC_CD_GRP_POSINCABINET1, IDC_CD_CK_ZERO_DEG_REF_WALL);

      SetNumeric(IDC_CD_EDT_HEIGHT, true, true);
      SetNumeric(IDC_CD_EDT_WIDTH , true, true);
      SetNumeric(IDC_CD_EDT_DEPTH , true, true);

      SetNumeric(IDC_CD_EDT_WIDTH1, true, true);
      SetNumeric(IDC_CD_EDT_WIDTH2, true, true);
      SetNumeric(IDC_CD_EDT_DEPTH1, true, true);
      SetNumeric(IDC_CD_EDT_DEPTH2, true, true);

      SetNumeric(IDC_CD_EDT_POS_HEIGHT, true, true);
      SetNumeric(IDC_CD_EDT_POS_M, true, true);
      return true;
   }
   return false;
}

BOOL CALLBACK CBoxCabinetDesignPage::SetHelpIDs(HWND hwnd, LPARAM /*lParam*/)
{
//   HWND hwndParent = (HWND)lParam;
   switch (::GetWindowLong(hwnd, GWL_ID))
   {
      case IDC_CD_EDT_DEPTH: case IDC_CD_EDT_WIDTH:
      case IDC_CD_TXT_HEIGHT: case IDC_CD_TXT_WIDTH: case IDC_CD_TXT_DEPTH:
         ::SetWindowContextHelpId(hwnd, IDC_CD_EDT_HEIGHT);
         break;
      case IDC_CD_GRP_CABINET_PARAM: ::SetWindowContextHelpId(hwnd, IDC_CD_GRP_WIDTH_CTRL); break;
      case IDC_CD_GRP_POSINCABINET1: ::SetWindowContextHelpId(hwnd, IDC_CD_POSITION_VIEW); break;
      case IDC_CD_EDT_POS_HEIGHT: case IDC_CD_LST_LS:  case IDC_CD_R_POS_M: case IDC_CD_EDT_POS_M:
//      case IDC_CD_POSITION_VIEW:
      case IDC_CD_TXT_DEPTH1:      ::SetWindowContextHelpId(hwnd, IDC_CD_EDT_DEPTH1);break;
      case IDC_CD_TXT_DEPTH2:      ::SetWindowContextHelpId(hwnd, IDC_CD_EDT_DEPTH2);break;
      case IDC_CD_TXT_WIDTH1:      ::SetWindowContextHelpId(hwnd, IDC_CD_EDT_WIDTH1);break;
      case IDC_CD_TXT_WIDTH2:      ::SetWindowContextHelpId(hwnd, IDC_CD_EDT_WIDTH2);break;
      case IDC_CD_R_POS_MH:        ::SetWindowContextHelpId(hwnd, IDC_CD_R_POS_M);break;
      case IDC_CD_TXT_POS_HEIGHT:  ::SetWindowContextHelpId(hwnd, IDC_CD_EDT_POS_HEIGHT);break;
      case IDC_CD_TXT_POS_M:       ::SetWindowContextHelpId(hwnd, IDC_CD_EDT_POS_M);break;
   }
   return 1;
}

int CBoxCabinetDesignPage::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet *pC     = pParent->GetCurrentCab();

   if (m_pCurrent)                                             // Chassisparameter
   {
      #ifdef _DEBUG
      CEtsList *plstCD = &pParent->m_ChassisData;
      ASSERT(plstCD->InList(m_pCurrent));
      #endif
   }

   bool bChanged    = false;
   switch (nID)
   {
      case IDC_CD_EDT_HEIGHT: case IDC_CD_EDT_WIDTH: case IDC_CD_EDT_DEPTH:
      case IDC_CD_EDT_WIDTH1: case IDC_CD_EDT_WIDTH2:
      case IDC_CD_EDT_DEPTH1: case IDC_CD_EDT_DEPTH2:
      if ((nNotifyCode == EN_KILLFOCUS) &&
          !(m_nFlags & NOKILLFOCUSNOTIFICATION) &&
          (OnKillFocusCabinetSizes(nID, hwndControl)==1) &&
          (m_pCurrent == NULL))
      {
         int nWall = 0;
         switch (nID)
         {
            case IDC_CD_EDT_HEIGHT: nWall = CARABOX_CABWALL_FRONT; break;
            case IDC_CD_EDT_WIDTH:  nWall = CARABOX_CABWALL_FRONT; break;
            case IDC_CD_EDT_DEPTH:  nWall = CARABOX_CABWALL_LEFT; break;
            case IDC_CD_EDT_WIDTH1: nWall = (pC->m_dwFlags&WIDTH_1_2_ON_BACK) ? CARABOX_CABWALL_TOP : CARABOX_CABWALL_FRONT; break;
            case IDC_CD_EDT_WIDTH2: nWall = (pC->m_dwFlags&WIDTH_1_2_ON_BACK) ? CARABOX_CABWALL_TOP : CARABOX_CABWALL_FRONT; break;
            case IDC_CD_EDT_DEPTH1: nWall = CARABOX_CABWALL_LEFT; break;
            case IDC_CD_EDT_DEPTH2: nWall = CARABOX_CABWALL_LEFT; break;
         }
         if (SetChassisLstSel(nWall))
         {
            SetCurrentWall();
            InitCabinetData();
         }
      }break;
      case IDC_CD_LST_LS:
         if (nNotifyCode == LBN_SELCHANGE) OnSelChangeChassisList(nID, hwndControl);
         break;
      case IDC_3D_BTN_CREATE_CABINET:
         if (nNotifyCode == BN_CLICKED) OnNewCabinet(hwndControl);
         break;
      case IDC_CD_POSITION_VIEW:
      if (nNotifyCode == VIEW2DN_SEGMENT_MOVED)
      {
         ChassisData *pCD = (ChassisData*)pParent->m_ChassisData.GetFirst();
         while (pCD)
         {
            if (pCD->IsPositionValid())                        // Chassis schon korrekt positioniert
            {
               if (pCD->dwFlagsCH & CHASSIS_POS_ON_2_WALLS)    // erstreckt es sich über mehrere Segmente
                  pCD->dwFlagsCH &= ~CHASSIS_POS_VALID;        // Position ungültig setzen. In OnWizardNext() neu prüfen
               if (pCD->GetCabinet() == pParent->m_nCurrentCab)// Befindet sich das Chassis auf dem aktuellen Segment
                  pCD->dwFlagsCH &= ~CHASSIS_3D_POS_INVALID;   // ist die 3D Position ungültig
            }
            pCD = (ChassisData*)pParent->m_ChassisData.GetNext();
         }
         if (pParent->m_Cabinet.GetNoOfWallSegments() > 1)     // sind mehrere Segmente im 2D-View
         {
            SetCurrentWall(m_nWall);                           // updaten
            ::InvalidateRect(m_hWndPosView, NULL, true);
         }
         pParent->SetChanged(CARABOX_CABINET);
      } break;
      case IDC_CD_R_SET_WIDTH1: case IDC_CD_R_SET_WIDTH2:
      if (nNotifyCode == BN_CLICKED)
      {
         if (GetDlgRadioBtn(IDC_CD_R_SET_WIDTH1) == 1)
         {
            if (!(pC->m_dwFlags&WIDTH_1_2_ON_BACK))
            {
               pC->m_dwFlags |=  WIDTH_1_2_ON_BACK;
               if (m_pCurrent == NULL)
               {
                  SetChassisLstSel(CARABOX_CABWALL_TOP);
               }
               bChanged = true;
            }
         }
         else
         {
            if (pC->m_dwFlags&WIDTH_1_2_ON_BACK)
            {
               pC->m_dwFlags &= ~WIDTH_1_2_ON_BACK;
               if (m_pCurrent == NULL)
               {
                  SetChassisLstSel(CARABOX_CABWALL_FRONT);
               }
               bChanged = true;
            }
         }
         if (bChanged)
         {
            SetCurrentWall();
            InitCabinetData();
            OnKillFocusCabinetSizes(nID, hwndControl);
         }
      } break;
      case IDC_CD_CK_ZERO_DEG_REF_WALL:
      if (nNotifyCode == BN_CLICKED)
      {
         CCabinet *pC = pParent->GetCabinet(BASE_CABINET);
         if (IsDlgButtonChecked(m_hWnd, nID) == MF_UNCHECKED)
         {
            ::CheckDlgButton(m_hWnd, nID, MF_CHECKED);
         }
         else
         {
            pC->m_dwFlags &= ~(ZERO_DEG_REF_LEFT|ZERO_DEG_REF_RIGHT|ZERO_DEG_REF_BACK);
            switch (LOWORD(m_nWall))
            {
               case CARABOX_CABWALL_FRONT: break;
               case CARABOX_CABWALL_LEFT:  pC->m_dwFlags |= ZERO_DEG_REF_LEFT;  break;
               case CARABOX_CABWALL_BACK:  pC->m_dwFlags |= ZERO_DEG_REF_BACK;  break;
               case CARABOX_CABWALL_RIGHT: pC->m_dwFlags |= ZERO_DEG_REF_RIGHT; break;
            }
            pParent->SetChanged(CARABOX_CHASSIS_POS);
            Invalidate3D(LIST_CHASSIS);
         }
      } break;
      case IDC_CD_BTN_GRAPH:
      if (nNotifyCode == BN_CLICKED)
      {
         if (pParent->m_p3DViewDlg->GetWindowHandle() == NULL)
         {
            pParent->m_p3DViewDlg->Init(g_hInstance, IDD_3D_VIEW, pParent->GetHWND());
            pParent->m_p3DViewDlg->m_hwndCabinetPage = m_hWnd;
            HWND hwnd = pParent->m_p3DViewDlg->Create();
            if (hwnd)
            {
               ::SendMessage(hwnd, WM_EXITSIZEMOVE, EXIT_SIZE_MOVE_SET|EXIT_SIZE_MOVE_DOCK|EXIT_SIZE_MOVE_FORCE|EXIT_SIZE_MOVE_RIGHT, (LPARAM)pParent->GetHWND());
            }
         }
      } break;
      case IDC_CD_BTN_CALC:
      if (nNotifyCode == BN_CLICKED)
      {
         pParent->OnBtnClckSPLView();
      } break;
      case IDC_CD_LST_CHASSIS_CAB:
      if (nNotifyCode == CBN_SELCHANGE)
      {
         bChanged = OnSelChangeCabinet(nID, hwndControl);
      }
      else if (nNotifyCode == CBN_CONTENT_CHANGED)
      {
         UpdateCabinetList();
      }break;
      case IDC_CD_LST_CHASSIS_POS:                             // Auswahl der Wand 
      if (nNotifyCode == CBN_SELCHANGE)
      {
         bChanged =  OnSelChangeWall(nID, hwndControl);
      } break;
      default: break;
   }

   if (m_pCurrent)                                             // Chassisparameter
   {
      switch (nID)
      {
         case IDC_CD_EDT_POS_HEIGHT:                           // Chassishöhe
         if ((nNotifyCode == EN_KILLFOCUS) &&
             !(m_nFlags & NOKILLFOCUSNOTIFICATION))
         {
            double dTemp = m_pCurrent->dHeightOnCabWall;
            if (GetDlgItemDouble(nID, dTemp, 0, &bChanged))
            {
               if (bChanged)
               {
                  m_pCurrent->SetHeightOnCabWall() = dTemp;
                  CheckCurrentChassisPosition();
                  bChanged = false; // Wizardbuttons nicht noch einmal testen
                  InitChassisPos();
                  Invalidate3D(LIST_CHASSIS);
                  ::InvalidateRect(m_hWndPosView, NULL, true);
               }
            }
         } break;
         case IDC_CD_EDT_POS_M:                                // Chassismittelpossition
         if ((nNotifyCode == EN_KILLFOCUS) &&
             !(m_nFlags & NOKILLFOCUSNOTIFICATION))
         {
            double dTemp = m_pCurrent->dDistFromCenter;
            if (GetDlgItemDouble(nID, dTemp, 0, &bChanged))
            {
               if (bChanged)
               {
                  m_pCurrent->SetDistFromCenter() = dTemp;
                  CheckCurrentChassisPosition();
                  bChanged = false; // Wizardbuttons nicht noch einmal testen
                  InitChassisPos();
                  Invalidate3D(LIST_CHASSIS);
                  ::InvalidateRect(m_hWndPosView, NULL, true);
               }
            }
         } break;
         case IDC_CD_R_POS_M:
         if (nNotifyCode == BN_CLICKED)
         {
            if (IsDlgButtonChecked(m_hWnd, nID) == MF_UNCHECKED)
            {
               ::CheckDlgButton(m_hWnd, nID, MF_CHECKED);
            }
            if (m_pCurrent->dDistFromCenter != 0.0)
            {
               m_pCurrent->SetDistFromCenter() = 0.0;
               CheckCurrentChassisPosition();
               InitChassisPos();
               Invalidate3D(LIST_CHASSIS);
               ::InvalidateRect(m_hWndPosView, NULL, true);
            }
         } break;
         case IDC_CD_R_POS_MH:
         if (nNotifyCode == BN_CLICKED)
         {
//            double dCenterHeight = (Vy(m_vrcWall[3]) - Vy(m_vrcWall[2]))*0.5;
            double dCenterHeight = pC->dHeight*0.5;
            if (IsDlgButtonChecked(m_hWnd, nID) == MF_UNCHECKED)
            {
               ::CheckDlgButton(m_hWnd, nID, MF_CHECKED);
            }
            if (m_pCurrent->dHeightOnCabWall != dCenterHeight)
            {
               m_pCurrent->SetHeightOnCabWall() = dCenterHeight;
               CheckCurrentChassisPosition();
               InitChassisPos();
               Invalidate3D(LIST_CHASSIS);
               ::InvalidateRect(m_hWndPosView, NULL, true);
            }
         } break;
         default:
            break;
      }

      if (bChanged)
      {
         CheckWizardButtons();
         EnableWindow(GetDlgItem(IDC_CD_BTN_CALC), IsWindowEnabled(pParent->GetWndFinishBtn()));
         pParent->SetChanged(CARABOX_CHASSIS_POS);
      }
   }
   return CEtsPropertyPage::OnCommand(nID , nNotifyCode, hwndControl);
}

int CBoxCabinetDesignPage::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch (nMsg)
   {
      case WM_DRAWITEM  : return OnDrawItem((DRAWITEMSTRUCT *)lParam);
      case WM_HELP:
      if (lParam)
      {
         HELPINFO *pHI = (HELPINFO*) lParam;
         if ((pHI->iContextType == HELPINFO_WINDOW) || (pHI->iContextType == HELPINFO_MENUITEM))
         {
            if ((pHI->iCtrlId == IDC_CD_PICTURE) || (pHI->iCtrlId == IDC_CD_PICTURE1))
            {
               if (((CBoxPropertySheet*)m_pParent)->GetCurrentCab()->m_dwFlags & WIDTH_1_2_ON_BACK)
                  pHI->iCtrlId = IDC_CD_PICTURE1;
               else
                  pHI->iCtrlId = IDC_CD_PICTURE;
            }
         }
      }
   }
   return CEtsPropertyPage::OnMessage(nMsg, wParam, lParam);
}

int  CBoxCabinetDesignPage::OnHelp(PSHNOTIFY*pN)
{
   ::WinHelp(m_hWnd, "CARACAD.hlp", HELP_CONTEXT, 1995);
   return CEtsPropertyPage::OnHelp(pN);
}

int  CBoxCabinetDesignPage::OnReset(PSHNOTIFY* pN)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   SendDlgItemMessage(IDC_CD_LST_LS, LB_RESETCONTENT, 0, 0);
   m_pCurrent = NULL;
   m_nWall    = 0;
   pParent->SendMsgTo3DDlg(WM_COMMAND, IDCANCEL, (LPARAM)m_hWnd);
   return CEtsPropertyPage::OnReset(pN);
}

int  CBoxCabinetDesignPage::OnSetActive(PSHNOTIFY* pN, bool bAccept)
{
   BEGIN("OnSetActive");
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet *pC     = pParent->GetCabinet(BASE_CABINET); // Initialisiert wird nur das Basisgehäuse
   CEtsList *plstCD = &pParent->m_ChassisData;
   int       nCount = 0;

   if (!pParent->m_ChassisData.InList(m_pCurrent)) m_pCurrent = NULL;

   bool bChanged = false;
   if (pParent->m_nLSType & (DIPOLE_FLAT|DIPOLE_FLAT_H))
   {
      if (pC->dDepth  == 0.0) pC->dDepth = 100, bChanged = true;
      if ((pC->dHeight == 0.0) || (pC->dWidth  == 0.0))
      {
         ChassisData *pCD = (ChassisData*) plstCD->GetFirst();
         while (pCD)
         {
            if (pCD->IsDummy()) break;
            if ((pParent->m_nLSType & DIPOLE_FLAT_H) && 
                (pCD->GetMembrTyp() != CARABOX_MEMBRTYP_DIPOLE))
            {
               pCD = (ChassisData*) plstCD->GetNext();
               continue;
            }
            if (pC->dHeight < pCD->dEffHoehe) pC->dHeight = pCD->dEffHoehe;
            if (pC->dWidth  == 0.0) pC->dWidth  = pCD->dEffBreite;
            else                    pC->dWidth += pCD->dEffBreite;
            pCD = (ChassisData*) plstCD->GetNext();
         }
         pC->dHeight = 10 * (int)(pC->dHeight * 0.11 + 0.51);
         pC->dWidth  = 10 * (int)(pC->dWidth  * 0.11 + 0.51);
         bChanged = true;
      }
   }
   else
   {
      if ((pC->dHeight == 0.0) || (pC->dWidth  == 0.0) || (pC->dDepth == 0.0))
      {
         ChassisData *pCD = (ChassisData*) plstCD->GetFirst();
         while (pCD)
         {
            if (pCD->IsDummy()) break;
            if (pCD->IsRound())
            {
               if (pC->dWidth < pCD->dEffDiameter) pC->dWidth = pCD->dEffDiameter;
               if (pC->dHeight == 0.0) pC->dHeight  = pCD->dEffDiameter;
               else                    pC->dHeight += pCD->dEffDiameter;
            }
            else
            {
               if (pC->dWidth < pCD->dEffBreite) pC->dWidth = pCD->dEffBreite;
               if (pC->dHeight == 0.0) pC->dHeight  = pCD->dEffHoehe;
               else                    pC->dHeight += pCD->dEffHoehe;
            }
            pCD = (ChassisData*) plstCD->GetNext();
         }
         pC->dHeight = 10 * (int)(pC->dHeight * 0.13 + 0.51);  // 130 %
         pC->dWidth  = 10 * (int)(pC->dWidth  * 0.12 + 0.51);  // 120 %
         pC->dDepth   = pC->dWidth;
         bChanged = true;
      }
   }
   if (bChanged)
   {
      pC->m_dwFlags |= CHECK_ALL_WALLS;
      pParent->CheckCabinetDesign();
   }
   
   if (pParent->m_nFlagChanged & UPDATE_CABINET_LIST) UpdateCabinetList();

   plstCD->ProcessWithObjects(ChassisData::CountValidListChassis, (WPARAM)&nCount, 0);
   SendDlgItemMessage(IDC_CD_LST_LS, LB_SETCOUNT, nCount+1, 0);
   if (nCount) EnableWindow(GetDlgItem(IDC_CD_LST_LS), true);

   int nOld = pParent->m_nCurrentCab;
   pParent->m_nCurrentCab = -1;
   InitChassisData();
   pParent->m_nCurrentCab = nOld;

   SetVolumeString();

   CheckWizardButtons();
   EnableWindow(GetDlgItem(IDC_CD_BTN_CALC), IsWindowEnabled(pParent->GetWndFinishBtn()));

   if (pParent->m_p3DViewDlg->GetWindowHandle())
   {
      ::ShowWindow(pParent->m_p3DViewDlg->GetWindowHandle(), SW_SHOW);
   }
   else
   {
      ::PostMessage(m_hWnd, WM_COMMAND, IDC_CD_BTN_GRAPH, (LPARAM)m_hWnd);
   }

   return CEtsPropertyPage::OnSetActive(pN, bAccept);
}

int CBoxCabinetDesignPage::OnKillActive(PSHNOTIFY *pN, bool bAccept)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   if (pParent->m_p3DViewDlg->GetWindowHandle())
   {
      ::ShowWindow(pParent->m_p3DViewDlg->GetWindowHandle(), SW_HIDE);
   }
   return CEtsPropertyPage::OnKillActive(pN, bAccept);
}

int  CBoxCabinetDesignPage::OnWizardBack(PSHNOTIFY* pN, bool bAccept)
{
   m_nFlags |= NOERRORWINDOW;
   OnWizardNext(pN, bAccept);
   m_nFlags &= ~NOERRORWINDOW;
   int nResult = 1;
   if (::GetFocus() == ::GetDlgItem(pN->hdr.hwndFrom, pN->hdr.idFrom)) nResult = 0;
   ::SetWindowLong(m_hWnd, DWL_MSGRESULT, nResult);
   return 1;
}

int  CBoxCabinetDesignPage::OnWizardFinish(PSHNOTIFY*pN, bool bFinish)
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

int  CBoxCabinetDesignPage::OnWizardNext(PSHNOTIFY*pN, bool bAccept)
{
   REPORT("CBoxCabinetDesignPage::OnWizardNext");
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet  *pC     = pParent->GetCabinet(BASE_CABINET);
   CEtsList  *plstCD = &pParent->m_ChassisData;
   int i, nCab = 0, nCount = 0;
   bAccept = false;

   if (pN) SetPageButtonFocus(pN);
   do
   {
      double  *pdCabSizes = &pC->dHeight;
      for (i=0; i<3; i++)
      {
         if (pdCabSizes[i] == 0.0)
         {
            SetCurrentCabinet(nCab);
            CreateErrorWnd(gm_nCabinetSizes[i][CCD_CTRL_ID], IDS_ERROR_WRONGINPUT, true);
            break;
         }
      }
      if (i == 3)                                           // Gehäuse ok
      {
         if (++nCab == 1) pC = (CCabinet*)pParent->m_Cabinet.m_Cabinets.GetFirst();
         else             pC = (CCabinet*)pParent->m_Cabinet.m_Cabinets.GetNext();
      }
      else break;
   } while (pC != NULL);

   if (i == 3)                                              // Gehäuse ok
   {
      pParent->m_nFlagOk |= CARABOX_CABINET;
      nCount = 0;
      plstCD->ProcessWithObjects(CBoxCabinetDesignPage::CheckChassisPositions, (WPARAM)&nCount, (LPARAM)this);
      if (nCount != -1)
      {
         pParent->m_nFlagOk |= CARABOX_CHASSIS_POS;
         bAccept = true;
      }
      else pParent->m_nFlagOk &= ~CARABOX_CHASSIS_POS;
   }
   else                                                     // Gehäuse nicht ok
   {
      pParent->m_nFlagOk &= ~CARABOX_CABINET;
   }

   if (pN)
   {
      if (!HasPageButtonFocus(pN)) bAccept = false;
      pN->lParam = bAccept;
      return CEtsPropertyPage::OnWizardNext(pN, bAccept);
   }
   return 0;
}

int CBoxCabinetDesignPage::OnSelChangeChassisList(WORD /*nID*/, HWND hwndControl)
{
   int nCurSel = ::SendMessage(hwndControl, LB_GETCURSEL, 0, 0);
   ChassisData *pOld = m_pCurrent;
   m_pCurrent = NULL;
   if (nCurSel != LB_ERR)
   {
      if (nCurSel > 0)
      {
         CEtsList *plstCD = &((CBoxPropertySheet*)m_pParent)->m_ChassisData;
         m_pCurrent = (ChassisData*)plstCD->GetAt(nCurSel-1);
         ASSERT(m_pCurrent != NULL);
      }
   }

   if (pOld != m_pCurrent)
   {
      InitChassisData();
   }

   return 1;
}

int CBoxCabinetDesignPage::OnKillFocusCabinetSizes(WORD nID, HWND /*hwndControl*/)
{
   const static double dMinGap = 1;
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet *pC     = pParent->GetCurrentCab();
   int      i;
   double  *pdCabSizes = &pC->dHeight, dOld;
   bool bChanged = false;

   if ((nID == IDC_CD_R_SET_WIDTH1)||(nID == IDC_CD_R_SET_WIDTH2)) bChanged = true;
   else
   {
      for (i=0; i<CARABOX_CABSIZES; i++)
      {
         if (gm_nCabinetSizes[i][CCD_CTRL_ID] == nID) break;
      }
      double dWidth1plusWidth2 = pC->dWidth1 + pC->dWidth2;
      if (i < 3)
      {
         double dMin = 0.0;
         double dMax = 10000.0;
         if ((pC->m_dwFlags & COMPLEX_CAB_DESIGN) && (pC->m_dwFlags & WIDTH_1_2_ON_BACK) && (dWidth1plusWidth2 > 0.0))
         {
            if      (i==1) dMin = dWidth1plusWidth2 * (pC->dDepth - (pC->dDepth1 + pC->dDepth2)) / pC->dDepth;
            else if (i==2) dMin = dWidth1plusWidth2 * (pC->dDepth1 + pC->dDepth2) / (-pC->dWidth + dWidth1plusWidth2);
         }
         else
         {
            if (i!=0) dMin = pdCabSizes[gm_nCabinetSizes[i][CCD_INDEX_TOTAL]]+pdCabSizes[gm_nCabinetSizes[i][CCD_INDEX_OTHER]];
         }
         if (dMin < 0.0) dMin = 0.0;
         if (GetDlgItemDouble(nID, pdCabSizes[i], 0, &bChanged))
         {
            if (bChanged) CheckMinMaxDouble(nID, dMin+dMinGap, dMax, 2, pdCabSizes[i]);
         }
         else if (bChanged) pdCabSizes[i] = 0;
      }
      else if (i < CARABOX_CABSIZES)
      {
         dOld = pdCabSizes[i];
         double dMaxSize;
         double dMinSize = -10000.0;
         dMaxSize = pdCabSizes[gm_nCabinetSizes[i][CCD_INDEX_TOTAL]] - pdCabSizes[gm_nCabinetSizes[i][CCD_INDEX_OTHER]];
         if ((pC->m_dwFlags & COMPLEX_CAB_DESIGN) && (pC->m_dwFlags & WIDTH_1_2_ON_BACK) && (dWidth1plusWidth2 > 0.0))
         {
            if ((i==3) || (i==4))
            {
               double dTemp = pC->dWidth / (1.0 - (pC->dDepth1 + pC->dDepth2) / pC->dDepth) - pdCabSizes[gm_nCabinetSizes[i][CCD_INDEX_OTHER]];
               if (dTemp < dMaxSize) dMaxSize = dTemp;
            }
            else
            {
               dMinSize = pC->dDepth * (1.0 - pC->dWidth / dWidth1plusWidth2) - pdCabSizes[gm_nCabinetSizes[i][CCD_INDEX_OTHER]];
            }
         }
         if (GetDlgItemDouble(nID, pdCabSizes[i], 0, &bChanged))
         {
             if (bChanged) CheckMinMaxDouble(nID, dMinSize+dMinGap, dMaxSize-dMinGap, 2, pdCabSizes[i]);
         }
         else if (bChanged) pdCabSizes[i] = 0.0;
      }
   }
   if (bChanged)
   {
      bool bEnabled = (pParent->m_nFlagOk & CARABOX_CABINET)? true : false;
      pParent->SetChanged(CARABOX_CABINET);
      pParent->m_nFlagOk &= ~CARABOX_CABINET;
      switch (nID)
      {
         case IDC_CD_EDT_HEIGHT: 
            pC->m_dwFlags |= (1<<CARABOX_CABWALL_BACK|1<<CARABOX_CABWALL_FRONT|1<<CARABOX_CABWALL_LEFT|1<<CARABOX_CABWALL_RIGHT);
            break;
         case IDC_CD_EDT_WIDTH:
            pC->m_dwFlags |= (1<<CARABOX_CABWALL_TOP|1<<CARABOX_CABWALL_BOTTOM|1<<CARABOX_CABWALL_FRONT|1<<CARABOX_CABWALL_BACK);
            break;
         case IDC_CD_EDT_DEPTH:
            pC->m_dwFlags |= (1<<CARABOX_CABWALL_TOP|1<<CARABOX_CABWALL_BOTTOM|1<<CARABOX_CABWALL_LEFT|1<<CARABOX_CABWALL_RIGHT);
            break;
         case IDC_CD_EDT_WIDTH1:
            if (pC->m_dwFlags & WIDTH_1_2_ON_BACK)
               pC->m_dwFlags |= (1<<CARABOX_CABWALL_TOP|1<<CARABOX_CABWALL_BOTTOM|1<<CARABOX_CABWALL_BACK|1<<CARABOX_CABWALL_LEFT);
            else
               pC->m_dwFlags |= (1<<CARABOX_CABWALL_TOP|1<<CARABOX_CABWALL_FRONT|1<<CARABOX_CABWALL_BACK|1<<CARABOX_CABWALL_LEFT);
            break;
         case IDC_CD_EDT_WIDTH2:
            if (pC->m_dwFlags & WIDTH_1_2_ON_BACK)
               pC->m_dwFlags |= (1<<CARABOX_CABWALL_TOP|1<<CARABOX_CABWALL_BOTTOM|1<<CARABOX_CABWALL_BACK|1<<CARABOX_CABWALL_RIGHT);
            else
               pC->m_dwFlags |= (1<<CARABOX_CABWALL_TOP|1<<CARABOX_CABWALL_FRONT|1<<CARABOX_CABWALL_BACK|1<<CARABOX_CABWALL_RIGHT);
            break;
         case IDC_CD_EDT_DEPTH1:
            pC->m_dwFlags |= (1<<CARABOX_CABWALL_TOP|1<<CARABOX_CABWALL_LEFT|1<<CARABOX_CABWALL_RIGHT|1<<CARABOX_CABWALL_FRONT);
            break;
         case IDC_CD_EDT_DEPTH2:
            pC->m_dwFlags |= (1<<CARABOX_CABWALL_TOP|1<<CARABOX_CABWALL_LEFT|1<<CARABOX_CABWALL_RIGHT|1<<CARABOX_CABWALL_BACK);
            break;
         case IDC_CD_R_SET_WIDTH1: case IDC_CD_R_SET_WIDTH2:
            if (pC->m_dwFlags & COMPLEX_CAB_DESIGN)
               pC->m_dwFlags |= (1<<CARABOX_CABWALL_TOP|1<<CARABOX_CABWALL_LEFT|1<<CARABOX_CABWALL_RIGHT|1<<CARABOX_CABWALL_BACK|1<<CARABOX_CABWALL_FRONT);
            else pC->m_dwFlags |= 1<<CARABOX_CABWALL_TOP;
            break;
      }

      pParent->CheckCabinetDesign(pC);

      pParent->SendMsgTo3DDlg(WM_COMMAND, (WPARAM)MAKELONG(IDC_CD_LST_CHASSIS_CAB, NOTIFY_KILLFOCUS_CAB_SIZE), (LPARAM)m_hWnd);
      Invalidate3D(LIST_CHASSIS);
      Invalidate3D(LIST_SCALE);
      Invalidate3D(LIST_CABINET+pParent->m_nCurrentCab);
      if (pParent->m_nCurrentCab != HIWORD(m_nWall))
      {
         m_nWall = MAKELONG(LOWORD(m_nWall), pParent->m_nCurrentCab); 
         m_pCurrent = NULL;
      }
      SetCurrentWall(m_nWall);

      if (!bEnabled && (pParent->m_nFlagOk & CARABOX_CABINET))
      { 
         EnableGroupItems(GetDlgItem(IDC_CD_GRP_POSINCABINET), true, 0);
      }
      SetVolumeString();
      if (m_pCurrent)
      {
         if (m_pCurrent->dwFlagsCH & (CHASSIS_CABWALL_FLAGS_CHANGED|CHASSIS_POS_CHANGED))
         {
            if (m_pCurrent->GetNoOfCoupled())
               CheckCurrentChassisPosition();
            m_pCurrent->dwFlagsCH &= ~(CHASSIS_CABWALL_FLAGS_CHANGED|CHASSIS_POS_CHANGED);
            if (m_pCurrent->nCabWall == 0) m_pCurrent = NULL;
         }
         InitChassisData();
      }
      else
      {
         InitChassisData(false);
      }
      return 1;
   }
   return 0;
}

void CBoxCabinetDesignPage::InitCabinetSizes()
{
   CCabinet *pC = ((CBoxPropertySheet*)m_pParent)->GetCurrentCab();
   int       i;   
   double    *pdCabSizes = &pC->dHeight;
   for (i=0; i<CARABOX_CABSIZES; i++)
   {
      if (pdCabSizes[i] != 0.0)
         SetDlgItemDouble(gm_nCabinetSizes[i][CCD_CTRL_ID], pdCabSizes[i], 1);
      else
         SetDlgItemText(gm_nCabinetSizes[i][CCD_CTRL_ID], "");
   }
}

void CBoxCabinetDesignPage::InitCabinetData()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet *pC = pParent->GetCurrentCab();
   if (pC->m_dwFlags & WIDTH_1_2_ON_BACK)
   {
      CheckDlgRadioBtn(IDC_CD_R_SET_WIDTH1, 1);
      ::ShowWindow(GetDlgItem(IDC_CD_PICTURE ), SW_HIDE);
      ::ShowWindow(GetDlgItem(IDC_CD_PICTURE1), SW_SHOW);
   }
   else
   {
      CheckDlgRadioBtn(IDC_CD_R_SET_WIDTH1, 0);
      ::ShowWindow(GetDlgItem(IDC_CD_PICTURE ), SW_SHOW);
      ::ShowWindow(GetDlgItem(IDC_CD_PICTURE1), SW_HIDE);
   }
}

void CBoxCabinetDesignPage::InitChassisData(bool bSetWall)
{
   BEGIN("InitChassisData");
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CEtsList          *plstCD  = &pParent->m_ChassisData;
   int                nPos    = plstCD->FindElement(NULL, m_pCurrent);

   if ((nPos == -1) && (m_pCurrent != NULL))
   {
      bSetWall = true;
      m_pCurrent = NULL;
   }

   ::SendMessage(GetDlgItem(IDC_CD_LST_LS), LB_SETCURSEL, nPos+1, 0);
 
   if (m_pCurrent == NULL)
   {
      if (pParent->m_nFlagOk & CARABOX_CABINET)
      {
         if (bSetWall)
         {
            if (!SetCurrentCabinet(HIWORD(m_nWall)))
            {
               SetCurrentWall();
               InvalidateRect(m_hWndPosView, NULL, true);
               InitChassisPosList();
            }
         }
         SetDlgItemText(IDC_CD_EDT_POS_HEIGHT, "");
         SetDlgItemText(IDC_CD_EDT_POS_M, "");
         ::CheckDlgButton(m_hWnd, IDC_CD_R_POS_M, false);
         ::EnableWindow(GetDlgItem(IDC_CD_EDT_POS_HEIGHT), false);
         ::EnableWindow(GetDlgItem(IDC_CD_EDT_POS_M)     , false);
         ::EnableWindow(GetDlgItem(IDC_CD_R_POS_M)       , false);
         ::EnableWindow(GetDlgItem(IDC_CD_R_POS_MH)      , false);
      }
      else
      {
         EnableGroupItems(GetDlgItem(IDC_CD_GRP_POSINCABINET), false, 0);
         InvalidateRect(m_hWndPosView, NULL, true);
      }
   }
   else
   {
      if (bSetWall)
      {
         if ((m_pCurrent->GetWall() == 0) || !SetCurrentCabinet(m_pCurrent->GetCabinet(), false))
         {
            SetCurrentWall();
            InvalidateRect(m_hWndPosView, NULL, true);
            InitChassisPosList();
         }
      }
      EnableGroupItems(GetDlgItem(IDC_CD_GRP_POSINCABINET), true, 0);
      InitChassisPos();
   }
}

void CBoxCabinetDesignPage::InitChassisPos()
{
   ASSERT(m_pCurrent != NULL);
   double dCenterHeight = (Vy(m_vrcWall[3]) - Vy(m_vrcWall[2]))*0.5;
   EnableWindow(GetDlgItem(IDC_CD_EDT_POS_HEIGHT), m_pCurrent->nCabWall != 0);

   ::CheckDlgButton(m_hWnd, IDC_CD_R_POS_M , (m_pCurrent->dDistFromCenter == 0.0          ) ? true : false);
   ::CheckDlgButton(m_hWnd, IDC_CD_R_POS_MH, (m_pCurrent->dHeightOnCabWall== dCenterHeight) ? true : false);

   SetDlgItemDouble(IDC_CD_EDT_POS_HEIGHT, m_pCurrent->dHeightOnCabWall, 1);
   SetDlgItemDouble(IDC_CD_EDT_POS_M     , m_pCurrent->dDistFromCenter, 1);
}

bool CBoxCabinetDesignPage::CheckCurrentChassisPosition(bool bInit)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet          *pCcurr  = pParent->GetCurrentCab();
   CCabinet          *pCbase  = pParent->GetCabinet(BASE_CABINET);
   bool bChanged = false;
   bool bOK      = false;
   ASSERT(pParent->m_ChassisData.InList(m_pCurrent));
   if (m_pCurrent->nCabWall != m_nWall) return true;

   if (pCcurr->CanContainChassis() && pCcurr->IsWallEnabled(m_pCurrent->GetWall()))
   {
      bool bValid = m_pCurrent->IsPositionValid();
      int  i, nSegments = pCbase->GetNoOfWallSegments();
      CVector2D vQ;
      bOK = CCabinet::IsOnWall((CVector2D*)m_vrcWall, m_pCurrent, &vQ, true);
      if (bInit && !bOK)
      {
         bOK = CCabinet::InitChassisPos((CVector2D*)m_vrcWall, m_pCurrent);
      }
      if (!bOK)
      {
         pCbase->CalcChassisPos3D(m_pCurrent);
         if (nSegments > 1)
         {
            nSegments--;
            for (i=0; i<nSegments; i++)
            {
               bOK = CCabinet::IsOnWall((CVector2D*)&m_pvSegments[i*NO_OF_RECT_POINTS], m_pCurrent, &vQ);
               if (bOK)
               {
                  m_pCurrent->nCabWall   = pCbase->GetWallSegmentArray()[(i+1)];
                  m_pCurrent->dwFlagsCH |= CHASSIS_2D_POS_INVALID;
                  if (pCbase->CalcChassisPos2D(m_pCurrent) && SetCurrentCabinet(m_pCurrent->GetCabinet(), false))
                  {
                     InitChassisPos();
                  }
                  break;
               }
            }
         }
         if (!bOK) bOK = IsChassisOnWallSegments(m_pCurrent);
      }
      if (bValid != bOK)
      {
         bChanged = true;
      }
   }
   else
   {
      bChanged = true;
   }

   if (bOK)
   {
      if (m_pCurrent->GetNoOfCoupled())                        // Gekoppeltes vorhanden (nur im Basisgehäuse)
      {
         pCbase->AdaptCoupledPosition(m_pCurrent);             // Position anpassen
         ChassisData *pCoupled = m_pCurrent->GetCoupled(0);    // gekoppeltes auf der Rückwand prüfen
         CVector2D vrcWall[NO_OF_RECT_POINTS];                 // Rückwand holen
         Array44 Tran;
         SetMatrixPtr(&Tran, false);                           // Globale Transformation nicht ändern
         pCbase->GetCabinetWall(pCoupled->nCabWall, (CVector2D*)vrcWall);
         bOK = CCabinet::IsOnWall((CVector2D*)vrcWall, pCoupled);
         SetMatrixPtr(NULL, false);                            // Globale Transformation wiederherstellen
         if (!bOK && (pCbase->GetNoOfWallSegments() > 1))      // nicht auf dieser Wand ?
         {
            SetCurrentWall(pCoupled->nCabWall);                // weitere Wandsegmente prüfen
            bOK = IsChassisOnWallSegments(m_pCurrent);
            SetCurrentWall(m_pCurrent->nCabWall);
         }
         if (!bOK)
         {
            m_pCurrent->dwFlagsCH &= ~(CHASSIS_POS_VALID|CHASSIS_POS_ON_2_WALLS);
         }
      }
      else if (m_pCurrent->IsSphere())
      {
         pCbase->CalcChassisPos3D(m_pCurrent);
         if (m_pCurrent->m_ppCabinets)
         {
            Invalidate3D(LIST_CABINET+m_pCurrent->m_ppCabinets[0]->nCountAllChassis);
         }
      }
   }
   else                                                        // paßt nicht auf die Wand
   {
      bChanged = true;
   }
   if (bChanged)
   {
      CheckWizardButtons();
      EnableWindow(GetDlgItem(IDC_CD_BTN_CALC), IsWindowEnabled(pParent->GetWndFinishBtn()));
      pParent->SetChanged(CARABOX_CHASSIS_POS);
   }
   return bChanged;
}

int CBoxCabinetDesignPage::CheckChassisPositions(void *p, WPARAM wParam, LPARAM lParam)
{
   CBoxCabinetDesignPage *pThis  = (CBoxCabinetDesignPage*)lParam;
   ChassisData           *pCH    = (ChassisData*)p;
//   CCabinet              *pC     = ((CBoxPropertySheet*)pThis->m_pParent)->GetCabinet(pCH->GetCabinet());
   CCabinet              *pCbase = ((CBoxPropertySheet*)pThis->m_pParent)->GetCabinet(BASE_CABINET);

   CVector2D rcWall[NO_OF_RECT_POINTS];

   if (pCH->IsHidden()) return 1;
   int nError = pCH->CheckPositionParam();

   while (nError)
   {
      if ((nError == IDC_CD_EDT_POS_HEIGHT) &&                 // Position ungültig
          (pCH->dwFlagsCH & CHASSIS_POS_ON_2_WALLS))           // möglicherweise auf > 2 Wänden
      {
         pThis->m_pCurrent = pCH;
         pThis->InitChassisData();
         pThis->SendDlgItemMessage(IDC_CD_LST_LS, LB_SETCURSEL, ((int*)wParam)[0]+1, 0);
         pThis->CheckCurrentChassisPosition();
         if (pCH->IsPositionValid())
         {
            nError = 0;
            continue;
         }
      }
      if (!(pThis->m_nFlags & NOERRORWINDOW))
      {
         pThis->CreateErrorWnd(nError, IDS_ERROR_WRONGINPUT, true);
         if (pThis->m_pCurrent != pCH)
         {
            pThis->m_pCurrent = pCH;
            pThis->InitChassisData();
            pThis->SendDlgItemMessage(IDC_CD_LST_LS, LB_SETCURSEL, ((int*)wParam)[0]+1, 0);
         }
      }
      ((int*)wParam)[0] = -1;
      return 0;
   }
   ((int*)wParam)[0]++;

   nError = 0;

   if (!pCbase->CalcChassisPos3D(pCH))     // 3D-Position
   {
      ((int*)wParam)[0] = -1;
   }

   return 1;
}

void CBoxCabinetDesignPage::SetCurrentWall(int nNewCabWall)
{
   int       i, j, k, nCabWall;
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet *pCbase = pParent->GetCabinet(BASE_CABINET);// Basisgehäuse !
   RECT rcClient;
   CVector vNorm;

   if (nNewCabWall)
   {
      nCabWall = nNewCabWall;
      m_nWall  = 0;
   }
   else if (m_pCurrent && m_pCurrent->nCabWall)
   {
      nCabWall = m_pCurrent->nCabWall;
   }
   else
   {
      int nWall = SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_GETCURSEL, 0, 0);
      if (nWall != CB_ERR)
      {
         nWall = SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_GETITEMDATA, nWall, 0);
      }
      if ((nWall <= 0) || (nWall > CARABOX_CABWALL_BOTTOM))
      {
         if (SetChassisLstSel(CARABOX_CABWALL_FRONT))
            nWall = CARABOX_CABWALL_FRONT;
         else if (SetChassisLstSel(CARABOX_CABWALL_BACK))
            nWall = CARABOX_CABWALL_BACK;
         else if (SetChassisLstSel(CARABOX_CABWALL_LEFT))
            nWall = CARABOX_CABWALL_LEFT;
         else if (SetChassisLstSel(CARABOX_CABWALL_RIGHT))
            nWall = CARABOX_CABWALL_RIGHT;
         else if (SetChassisLstSel(CARABOX_CABWALL_TOP))
            nWall = CARABOX_CABWALL_TOP;
         else if (SetChassisLstSel(CARABOX_CABWALL_BOTTOM))
            nWall = CARABOX_CABWALL_BOTTOM;
         else nWall = 0;
      }
      nCabWall  = MAKELONG(nWall, pParent->m_nCurrentCab);
   }

   ::GetClientRect(m_hWndPosView, &rcClient);

   if (m_nWall != nCabWall)
   {
      if (pCbase->GetCabinetWall(nCabWall, (CVector2D*)m_vrcWall))
      {
         m_nWall = nCabWall;
         pCbase->InitWallSegmentArray(m_nWall);

         m_pptSegments[0].x = CEtsDiv::Round(Vx(m_vrcWall[0]));
         m_pptSegments[0].y = CEtsDiv::Round(Vy(m_vrcWall[0]));
         m_rcView.left = m_rcView.right  = m_pptSegments[0].x;
         m_rcView.top  = m_rcView.bottom = m_pptSegments[0].y;
         for (i=1; i<NO_OF_RECT_POINTS; i++)
         {
            m_pptSegments[i].x = CEtsDiv::Round(Vx(m_vrcWall[i]));
            m_pptSegments[i].y = CEtsDiv::Round(Vy(m_vrcWall[i]));
            m_rcView.left   = min(m_rcView.left  , m_pptSegments[i].x);
            m_rcView.right  = max(m_rcView.right , m_pptSegments[i].x);
            m_rcView.top    = max(m_rcView.top   , m_pptSegments[i].y);
            m_rcView.bottom = min(m_rcView.bottom, m_pptSegments[i].y);
         }

         if (pCbase->GetNoOfWallSegments() > 1)                // sollen mehr Segmente angezeigt werden
         {
            CCabinet *pCcurr = pCbase->GetCabinet(HIWORD(m_nWall));
            const DWORD *pdwSegmentArray = pCbase->GetWallSegmentArray();
            CVector vDummy, pvWall[NO_OF_RECT_POINTS];
            Array44 NegativeTransformation;
            int nSegments = pCbase->GetNoOfWallSegments();
            int nSizeOfSegmentArray = nSegments * NO_OF_RECT_POINTS;
            if (nSizeOfSegmentArray > m_nSizeOfSegmentArray)
            {
               m_nSizeOfSegmentArray = nSizeOfSegmentArray;
               m_pptSegments  = (POINT*)    Alloc(sizeof(POINT    )*m_nSizeOfSegmentArray, m_pptSegments);
               m_pvSegments   = (CVector2D*)Alloc(sizeof(CVector2D)*m_nSizeOfSegmentArray-NO_OF_RECT_POINTS, m_pvSegments);
            }
            if (!pCcurr->IsBaseCabinet())
            {
               SetMatrixPtr(&NegativeTransformation, false);
               vDummy = -pCcurr->vPosRef;
               LoadIdentity();
               if (Vy(pCcurr->vUserRef) != 0.0) Rotate_y(-Vy(pCcurr->vUserRef)); // Theta
               if (Vz(pCcurr->vUserRef) != 0.0) Rotate_z(-Vz(pCcurr->vUserRef)); // Phi
               Translate(vDummy);
               SetMatrixPtr(NULL, false);
            }

            k = NO_OF_RECT_POINTS;
            for (j=1; j<nSegments; j++)
            {
               pCbase->GetCabinetWall(pdwSegmentArray[j], NULL, &vDummy, &pvWall[0]);
               for (i=0; i<NO_OF_RECT_POINTS; i++, k++)
               {
                  if (!pCcurr->IsBaseCabinet())
                  {
                     SetMatrixPtr(&NegativeTransformation, false);
                     TransformVector(pvWall[i]);
                     SetMatrixPtr(NULL, false);
                  }
                  TransformVector(pvWall[i]);
                  m_pvSegments[k-NO_OF_RECT_POINTS].SetX() = Vx(pvWall[i]);
                  m_pvSegments[k-NO_OF_RECT_POINTS].SetY() = Vy(pvWall[i]);
                  m_pptSegments[k].x = CEtsDiv::Round(Vx(pvWall[i]));
                  m_pptSegments[k].y = CEtsDiv::Round(Vy(pvWall[i]));
                  m_rcView.left   = min(m_rcView.left  , m_pptSegments[k].x);
                  m_rcView.right  = max(m_rcView.right , m_pptSegments[k].x);
                  m_rcView.top    = max(m_rcView.top   , m_pptSegments[k].y);
                  m_rcView.bottom = min(m_rcView.bottom, m_pptSegments[k].y);
               }
            }
         }

         int nDim21 = MulDiv(21,(m_rcView.right-m_rcView.left), (rcClient.right-rcClient.left));

         switch (LOWORD(m_nWall))
         {
            case CARABOX_CABWALL_BOTTOM:
               m_rcView.right  = max(m_rcView.right , (long)Vx(m_vrcWall[3])+nDim21);
               break;
            case CARABOX_CABWALL_TOP:  case CARABOX_CABWALL_FRONT: case CARABOX_CABWALL_LEFT:
               m_rcView.right  = max(m_rcView.right , (long)Vx(m_vrcWall[2])+nDim21);
               break;
            case CARABOX_CABWALL_BACK: case CARABOX_CABWALL_RIGHT:
               m_rcView.left   = min(m_rcView.left  , (long)Vx(m_vrcWall[1])-nDim21);
               break;
         }

         int  nCcx = rcClient.right  - rcClient.left,
              nCcy = rcClient.bottom - rcClient.top, 
              nVcx = m_rcView.right  - m_rcView.left,
              nVcy = m_rcView.top    - m_rcView.bottom,
              ndx=0, ndy=0;

         double dRX = (double)nVcx / (double)nCcx;
         double dRY = (double)nVcy / (double)nCcy;

         if (dRX > dRY) ndy =  (int)((1.0 - dRX / dRY) * 0.5 * nVcy);
         else           ndx = -(int)((1.0 - dRY / dRX) * 0.5 * nVcx);

         ::InflateRect(&m_rcView, ndx, ndy);
      }
      else
      {
         ZeroMemory(&m_vrcWall, sizeof(CVector2D)*NO_OF_RECT_POINTS);
         ::SetRectEmpty(&m_rcView);
         ::InflateRect(&m_rcView, 1, 1);
      }

      bool bEnable = true;
      UINT nCheck  = MF_UNCHECKED;
      if (HIWORD(m_nWall) != 0)
      {
         bEnable = false;
      }
      else if (pParent->m_nLSType & (DIPOLE_FLAT|DIPOLE_FLAT_H))  // Dipol Lautsprecher:
      {                                                           // Hauptschallwand nur vorne
         if (LOWORD(m_nWall) == CARABOX_CABWALL_FRONT)
            nCheck = MF_CHECKED;
         else
            bEnable = false;
      }
      else                                                        // andere
      {
         switch (LOWORD(m_nWall))
         {
            case CARABOX_CABWALL_FRONT:                           // vorne : Normalfall
               if ((pCbase->m_dwFlags & (ZERO_DEG_REF_LEFT|ZERO_DEG_REF_RIGHT|ZERO_DEG_REF_BACK)) == 0)
                  nCheck = MF_CHECKED;
               break;
            case CARABOX_CABWALL_LEFT:                            // andere : Zusatzflags
               if (pCbase->m_dwFlags & ZERO_DEG_REF_LEFT)
                  nCheck = MF_CHECKED;
               break;
            case CARABOX_CABWALL_BACK:
               if (pCbase->m_dwFlags & ZERO_DEG_REF_BACK)
                  nCheck = MF_CHECKED;
               break;
            case CARABOX_CABWALL_RIGHT:
               if (pCbase->m_dwFlags & ZERO_DEG_REF_RIGHT)
                  nCheck = MF_CHECKED;
               break;
            case CARABOX_CABWALL_TOP: case CARABOX_CABWALL_BOTTOM:
            default:
               bEnable = false;
               break;
         }
      }
      ::EnableWindow(GetDlgItem(IDC_CD_CK_ZERO_DEG_REF_WALL), bEnable);
      ::CheckDlgButton(m_hWnd, IDC_CD_CK_ZERO_DEG_REF_WALL, nCheck);
   }
}

void CBoxCabinetDesignPage::OnPosViewPrepareDC(HDC hdc)
{
   RECT rcClient;
   ::GetClientRect(m_hWndPosView, &rcClient);
   ::InflateRect(&rcClient, -5, -5);
   ::SetMapMode(hdc, MM_ANISOTROPIC);
   ::SetViewportExtEx(hdc, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, NULL);
   ::SetViewportOrgEx(hdc, rcClient.left, rcClient.top, NULL);
   ::SetWindowExtEx(hdc, m_rcView.right-m_rcView.left, m_rcView.bottom-m_rcView.top, NULL);
   ::SetWindowOrgEx(hdc, m_rcView.left, m_rcView.top, NULL);
}

void CBoxCabinetDesignPage::OnPosViewDraw(HDC hdc, PAINTSTRUCT* /*pPS*/)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   if (!(pParent->m_nFlagOk & CARABOX_CABINET)) return;

   CEtsList *plstCD = &pParent->m_ChassisData;
   double dCenterX  = CCabinet::GetXCenterLine((CVector2D*)m_vrcWall);
   int i, nPoint = 0, nDim2, nDim3, nDim4, nDim6, nDim12, nDim21;
   RECT rcClient;
   HPEN   hBluePen   = ::CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
   HPEN   hDotPen    = ::CreatePen(PS_DOT  , 0, RGB(0, 0, 0));
   HBRUSH hBlueBrush = ::CreateSolidBrush(RGB(0,0,255));
   ::GetClientRect(m_hWndPosView, &rcClient);
   ::SaveDC(hdc);
                                                               // Bezugspunkt und Pfeilkoordinaten
   nDim2  = MulDiv( 1,(m_rcView.right-m_rcView.left), (rcClient.right-rcClient.left));
   nDim3  = MulDiv( 3,(m_rcView.right-m_rcView.left), (rcClient.right-rcClient.left));
   nDim4  = MulDiv( 4,(m_rcView.right-m_rcView.left), (rcClient.right-rcClient.left));
   nDim6  = MulDiv( 6,(m_rcView.right-m_rcView.left), (rcClient.right-rcClient.left));
   nDim12 = MulDiv(12,(m_rcView.right-m_rcView.left), (rcClient.right-rcClient.left));
   nDim21 = MulDiv(21,(m_rcView.right-m_rcView.left), (rcClient.right-rcClient.left));

   nPoint = 2;                                                 // Meistens Punkt 2
   switch (LOWORD(m_nWall))
   {
      case CARABOX_CABWALL_BOTTOM:
         nPoint = 3;                                           // nur für Bottom Punkt 3
      case CARABOX_CABWALL_TOP:  case CARABOX_CABWALL_FRONT: case CARABOX_CABWALL_LEFT:
         nDim4 =-nDim4;                                        // Pfeilrichtung umdrehen
         nDim6 =-nDim6;
         nDim12=-nDim12;
         nDim21=-nDim21;
         break;
      case CARABOX_CABWALL_BACK: case CARABOX_CABWALL_RIGHT:
         nPoint = 1; break;
   }

   POINT ptBlue  = {CEtsDiv::Round(Vx(m_vrcWall[nPoint])), CEtsDiv::Round(Vy(m_vrcWall[nPoint]))};

   POINT ptTr[7] = {{ptBlue.x-nDim21,ptBlue.y+nDim2},          // 0, Pfeilpolygon
                    {ptBlue.x-nDim12,ptBlue.y+nDim2},          // 1
                    {ptBlue.x-nDim12,ptBlue.y+nDim4},          // 2
                    {ptBlue.x-nDim6 ,ptBlue.y},                // 3
                    {ptBlue.x-nDim12,ptBlue.y-nDim4},          // 4
                    {ptBlue.x-nDim12,ptBlue.y-nDim2},          // 5
                    {ptBlue.x-nDim21,ptBlue.y-nDim2}};         // 6

   ::MoveToEx(hdc, m_pptSegments[0].x, m_pptSegments[0].y, NULL);
   ::PolylineTo(hdc, &m_pptSegments[0], NO_OF_RECT_POINTS);
   ::LineTo(hdc, m_pptSegments[0].x, m_pptSegments[0].y);
   if (pParent->m_Cabinet.GetNoOfWallSegments() > 1)
   {
      int nSegments = pParent->m_Cabinet.GetNoOfWallSegments();
      for (i=1; i<nSegments; i++)
      {
         ::MoveToEx(hdc, m_pptSegments[i*NO_OF_RECT_POINTS].x, m_pptSegments[i*NO_OF_RECT_POINTS].y, NULL);
         ::PolylineTo(hdc, &m_pptSegments[i*NO_OF_RECT_POINTS], NO_OF_RECT_POINTS);
         ::LineTo(hdc, m_pptSegments[i*NO_OF_RECT_POINTS].x, m_pptSegments[i*NO_OF_RECT_POINTS].y);
      }
   }

   ::SelectObject(hdc, hDotPen);                               // Zeichnen der Mittellinie
   ::MoveToEx(hdc, CEtsDiv::Round(dCenterX), CEtsDiv::Round(Vy(m_vrcWall[0])), NULL);
   ::LineTo(  hdc, CEtsDiv::Round(dCenterX), CEtsDiv::Round(Vy(m_vrcWall[1])));
                                                               // Zeichnen der Chassis
   plstCD->ProcessWithObjects(CBoxCabinetDesignPage::DrawChassisPositions, (WPARAM)hdc, (LPARAM)this);
   if (m_pCurrent && (m_pCurrent->nCabWall == m_nWall))
   {
      m_pCurrent->Draw(hdc, true, (CVector2D*)m_vrcWall);
   }
   
   ::SelectObject(hdc, hBluePen);                              // Zeichnen des Bezugspunktes mit Pfeil
   ::SelectObject(hdc, hBlueBrush);
   ::Ellipse(hdc, ptBlue.x-nDim3,ptBlue.y-nDim3,ptBlue.x+nDim3,ptBlue.y+nDim3);
   ::Polygon(hdc, (CONST POINT*)&ptTr, 7);

   ::RestoreDC(hdc, -1);
   if (hBluePen)   ::DeleteObject(hBluePen);
   if (hBlueBrush) ::DeleteObject(hBlueBrush);
   if (hDotPen)    ::DeleteObject(hDotPen);
}

int CBoxCabinetDesignPage::DrawChassisPositions(void *p, WPARAM wParam, LPARAM lParam)
{
   CBoxCabinetDesignPage *pThis = (CBoxCabinetDesignPage*)lParam;
   ChassisData           *pCH   = (ChassisData*)p;
   if (pCH != pThis->m_pCurrent)
   {
      if (pCH->nCabWall == pThis->m_nWall)
      {
         pCH->Draw((HDC) wParam, false, (CVector2D*)pThis->m_vrcWall);
      }
      else
      {
         CBoxPropertySheet *pParent = (CBoxPropertySheet*)pThis->m_pParent;
         int nSeg = pParent->m_Cabinet.FindWallSegment(pCH->nCabWall);
         if (nSeg > 0)
         {
            pCH->Draw((HDC) wParam, false, (CVector2D*)&pThis->m_pvSegments[(nSeg-1)*NO_OF_RECT_POINTS]);
         }
      }
   }
   return 1;
}

LRESULT CBoxCabinetDesignPage::OnPosViewKeyDown(WPARAM dwKey, LPARAM /*lParam*/)
{
   int nElement = -2;
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   if (m_pCurrent)
   {
      int x, y;
      x = y = m_ptOld.x = m_ptOld.y = 0;
      switch (dwKey)
      {
         case VK_LEFT:  x -= 1; break;
         case VK_RIGHT: x += 1; break;
         case VK_DOWN:  y -= 1; break;
         case VK_UP:    y += 1; break;
         default:
            nElement = -1;
            break;
      }
      if (nElement == -2)
      {
         MoveActualChassis(x, y, false);
         if (CheckCurrentChassisPosition())
            ::InvalidateRect(m_hWndPosView, NULL, true);
      }
      m_ptOld.x = m_ptOld.y = PT_UN_PICKED;
   }
   else nElement = -1;

   if (nElement == -1)
   {
      switch (dwKey)
      {
         case VK_ADD:
            nElement = pParent->m_ChassisData.FindElement(NULL, m_pCurrent)+1;
            if (nElement >= pParent->m_ChassisData.GetCounter()) nElement = 0;
         break;
         case VK_SUBTRACT:
            nElement = pParent->m_ChassisData.FindElement(NULL, m_pCurrent)-1;
            if (nElement<0) nElement = pParent->m_ChassisData.GetCounter()-1;
         break;
         case VK_RETURN:
         {
            if (CheckCurrentChassisPosition())
               InitChassisPos();
         }break;
         case VK_F4:
         {
            char   szFormat[256];
            char   szText[256];
            double dWallThickness = 15.0;
            CCabinet *pC = pParent->GetCabinet(BASE_CABINET);
            double dVolume = pC->GetVolume(dWallThickness);
            RECT   rc;
            ::LoadString(m_hInstance, IDS_EXTRA_TEXT, szFormat, 256);
            sprintf(szText, szFormat,
                    dVolume*0.000001, dWallThickness,
                    pC->nCountAllChassis,
                    pC->nCountBR+pC->nCountPM+pC->nCountTL,
                    pC->nCountTT1+pC->nCountTT2, ((pC->nCountTT1+pC->nCountTT2) ? pParent->m_BasicData.dCorrectAmplTT12 : 0),
                    pC->nCountMT1, ((pC->nCountMT1) ? pParent->m_BasicData.dCorrectAmplMT1 : 0),
                    pC->nCountMT2, ((pC->nCountMT2) ? pParent->m_BasicData.dCorrectAmplMT2 : 0),
                    pC->nCountMT3, ((pC->nCountMT3) ? pParent->m_BasicData.dCorrectAmplMT3 : 0),
                    pC->nCountHT,  ((pC->nCountHT)  ? pParent->m_BasicData.dCorrectAmplHT  : 0));
            ::GetWindowRect(m_hWndPosView, &rc);
            CEtsHelp::CreateContextWnd(szText, rc.left, rc.top);
         } break;
         case VK_F5:
         {

         }break;
         case VK_TAB:
         {
            short nState = GetKeyState(VK_SHIFT);
            ::SendMessage(m_hWnd, WM_NEXTDLGCTL, ((nState == 0)||(nState == 1)) ? 0 : 1, 0);
         }
         default :nElement = -1; break;
      }
      if (nElement != -1)
      {
         m_pCurrent = (ChassisData*)pParent->m_ChassisData.GetAt(nElement);
         if (m_pCurrent)
         {
            if (m_pCurrent->IsDummy())
            {
               m_pCurrent = NULL;
            }
            InitChassisData();
         }
      }
   }
   return 0;
}

void CBoxCabinetDesignPage::OnPosViewMouseMove(DWORD dwKeys, int x, int y)
{
   if ((dwKeys & MK_LBUTTON) && (m_pCurrent != NULL) && (m_ptOld.x != PT_UN_PICKED) && (m_ptOld.y != PT_UN_PICKED))
   {
      MoveActualChassis(x, y);
   }
}

void CBoxCabinetDesignPage::MoveActualChassis(int x, int y, bool bTransform)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   POINT              pt      = {x, y};

   HDC                hdc     = ::GetDC(m_hWndPosView);
   OnPosViewPrepareDC(hdc);
   if (bTransform) ::DPtoLP(hdc, &pt, 1);
   POINT   ptOffset = {pt.x-m_ptOld.x, pt.y-m_ptOld.y};
   m_ptOld = pt;

   if (ptOffset.x || ptOffset.y)
   {
      RECT rc1, rc2, rc;
      rc1 = m_pCurrent->GetRect(hdc, true, (CVector2D*)m_vrcWall);
      m_pCurrent->SetDistFromCenter()  = CEtsDiv::Round(m_pCurrent->dDistFromCenter ) + ptOffset.x;
      m_pCurrent->SetHeightOnCabWall() = CEtsDiv::Round(m_pCurrent->dHeightOnCabWall) + ptOffset.y;
      rc2 = m_pCurrent->GetRect(hdc, true, (CVector2D*)m_vrcWall);
      ::UnionRect(&rc, &rc1, &rc2);
      ::InflateRect(&rc, 2, 2);
      ::InvalidateRect(m_hWndPosView, &rc, true);
      InitChassisPos();
      pParent->SetChanged(CARABOX_CHASSIS_POS);
      pParent->m_nFlagOk &= ~CARABOX_CHASSIS_POS;
   }
   ::ReleaseDC(m_hWndPosView, hdc);
}

void CBoxCabinetDesignPage::OnPosViewLBtnDown(DWORD /*dwKeys*/, int x, int y)
{
   ::SetFocus(m_hWndPosView);
   ::SetCapture(m_hWndPosView);
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   HDC hdc = ::GetDC(m_hWndPosView);
   POINT ptDev = {x, y};
   POINT pt    = ptDev;
   OnPosViewPrepareDC(hdc);
   ::DPtoLP(hdc, &pt, 1);
   RECT rc;
   if (m_pCurrent) rc = m_pCurrent->GetRect(hdc, true, (CVector2D*)m_vrcWall);
   if (m_pCurrent && ::PtInRect(&rc, ptDev))       // Chassis war schon gepickt
   {
      m_ptOld = pt;                                // bewegen in MouseMove
   }
   else
   {
      long lP[3] = {(long)&ptDev, (long)0, (long)hdc};
      ChassisData *pOld = m_pCurrent;
      m_pCurrent = NULL;
      pParent->m_ChassisData.ProcessWithObjects(CBoxCabinetDesignPage::HitChassisPosition, (WPARAM)lP, (LPARAM)this);
      if (pOld != m_pCurrent)
      {
         if (m_pCurrent == NULL)
         {
            lP[1] = -1;
         }
         ::SendMessage(GetDlgItem(IDC_CD_LST_LS), LB_SETCURSEL, lP[1]+1, 0);
         if ((m_pCurrent != NULL) && (HIWORD(m_nWall) != m_pCurrent->GetCabinet()))
         {  // Wenn gepickt wurde und das aktuelle Gehäusesegment geändert wird,
            m_ptOld.x = m_ptOld.y = PT_UN_PICKED;  // darf das Chassis nicht sofort bewegt werden
         }                                         // Das Chassis wird dann in OnLBtnUp() initialisiert
         else                                      // bleibt das aktuelle Gehäusesegment erhalten
         {
            InitChassisData();                     // Chassis sofort initialisieren
            m_ptOld = pt;                          // Bewegung sofort zulassen
         }
      }
   }
   ::ReleaseDC(m_hWndPosView, hdc);
}

int CBoxCabinetDesignPage::HitChassisPosition(void *p, WPARAM wParam, LPARAM lParam)
{
   CBoxCabinetDesignPage *pThis = (CBoxCabinetDesignPage*)lParam;
   ChassisData *pCH = (ChassisData*)p;
   if ((pCH != pThis->m_pCurrent) && !pCH->IsDummy())
   {
      RECT rc;
      POINT *ppt = (POINT*) ((long*)wParam)[0];
      if (pCH->nCabWall == pThis->m_nWall)
      {
         rc = pCH->GetRect((HDC)((long*)wParam)[2], true, (CVector2D*)pThis->m_vrcWall);
      }
      else
      {
         int nSeg = ((CBoxPropertySheet*)pThis->m_pParent)->m_Cabinet.FindWallSegment(pCH->nCabWall);
         if (nSeg > 0)
         {
            rc = pCH->GetRect((HDC)((long*)wParam)[2], true, (CVector2D*)&pThis->m_pvSegments[(nSeg-1)*NO_OF_RECT_POINTS]);
         }
      }
      if (::PtInRect(&rc, *ppt))
      {
         pThis->m_pCurrent = pCH;
         return 0;
      }
   }
   ((long*)wParam)[1]++;
   return 1;
}

void CBoxCabinetDesignPage::OnPosViewLBtnUp(DWORD /*dwKeys*/, int x, int y)
{
   if (::GetCapture() == m_hWndPosView)
      ::ReleaseCapture();

   if (m_pCurrent != NULL)
   {
      if ((m_ptOld.x != PT_UN_PICKED) && (m_ptOld.y != PT_UN_PICKED))
      {
         CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
         if (pParent->m_nFlagChanged & CARABOX_CHASSIS_POS)
         {
            if (CheckCurrentChassisPosition())
               InitChassisPos();
            m_ptOld.x = m_ptOld.y = PT_UN_PICKED;
            ::InvalidateRect(m_hWndPosView, NULL, true);

            Invalidate3D(LIST_CHASSIS);
         }
      }
      else
      {
         InitChassisData();
      }
   }
   else
   {
      CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
      int i, nSegments = pParent->m_Cabinet.GetNoOfWallSegments();
      if (nSegments > 1)
      {
         HDC hdc = ::GetDC(m_hWndPosView);
         POINT ptDev = {x, y};
         POINT pt    = ptDev;
         OnPosViewPrepareDC(hdc);
         ::DPtoLP(hdc, &pt, 1);
         HRGN hrgn = NULL;
         for (i=1; i<nSegments; i++)
         {
            hrgn = ::CreatePolygonRgn(&m_pptSegments[i*NO_OF_RECT_POINTS], NO_OF_RECT_POINTS, WINDING);
            if (hrgn)
            {
               if (::PtInRegion(hrgn, pt.x, pt.y))
               {
                  int nWall = HIWORD(pParent->m_Cabinet.GetWallSegmentArray()[i]);
                  if (nWall != HIWORD(m_nWall))
                  {
                     SetCurrentCabinet(nWall, true);
                     ::DeleteObject(hrgn);
                     break;
                  }
               }
               ::DeleteObject(hrgn);
            }
         }
         ::ReleaseDC(m_hWndPosView, hdc);
      }
   }
}

LRESULT CBoxCabinetDesignPage::OnDrawItem(DRAWITEMSTRUCT *pDI)
{
   if (pDI->CtlType == ODT_LISTBOX)
   {
      char szText[64]  = "";
      char szIndex[64] = "";
      int  nLen         = 4;
      RECT rcItem = pDI->rcItem;
      ::InflateRect(&rcItem, -1, -1);

      CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
      CEtsList *plstCD = &pParent->m_ChassisData;
      ChassisData * pCD = NULL;
      if (pDI->itemID == 0)
      {
         ::LoadString(m_hInstance, IDS_CABINET_VIEW, szText, 32);
         nLen = strlen(szText);
      }
      else if (pDI->itemID>0)
      {
         pCD = (ChassisData*)plstCD->GetAt(pDI->itemID-1);
         if (pCD)
         {
            nLen = pParent->GetChassisTypeString(pCD, szText, 64);
            wsprintf(szIndex, "%d: %s", pDI->itemID, pCD->szDescrpt);
         }
      }

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
         bool bOK = false;
         int nColorB = COLOR_INFOBK;
         int nColorT = COLOR_INFOTEXT;
         if (pCD)
         {
            if ((pCD->nCabWall > 0) && (pCD->IsPositionValid())) bOK = true;
         }

         if (bOK) nColorB = COLOR_WINDOW, nColorT = COLOR_WINDOWTEXT;
         if (pDI->itemState & ODS_FOCUS)
         {
            swap(nColorB, nColorT);
         }

         ::SetTextColor(pDI->hDC, ::GetSysColor(nColorT));
         ::SetBkColor(pDI->hDC, ::GetSysColor(nColorB));
         ::FillRect(pDI->hDC, &rcItem, ::GetSysColorBrush(nColorB));
      }

      {
         POINT pt;
         ::MoveToEx(pDI->hDC, pDI->rcItem.left, pDI->rcItem.bottom-1, &pt);
         ::LineTo(pDI->hDC  ,  pDI->rcItem.right, pDI->rcItem.bottom-1);
         ::SetWindowLong(pDI->hwndItem, GWL_USERDATA, -1);
      }

      if (szIndex[0] != 0)
      {
         RECT rcIndex = rcItem;
         rcIndex.right -= 30;
         ::DrawText(pDI->hDC, szIndex, strlen(szIndex), &rcIndex, DT_LEFT|DT_TOP|DT_SINGLELINE);
         ::DrawText(pDI->hDC, szText, nLen, &rcItem, DT_RIGHT|DT_TOP|DT_SINGLELINE);
      }
      else ::DrawText(pDI->hDC, szText, nLen, &rcItem, DT_LEFT|DT_TOP|DT_SINGLELINE);

      ::RestoreDC(pDI->hDC, -1);
      return true;
   }
   return 0;
}

void CBoxCabinetDesignPage::SetVolumeString()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   char text[64], format[64], number[32];
   ::LoadString(m_hInstance, IDC_CD_TXT_BOX_VOLUME, format, 64);
   ETSDIV_NUTOTXT ntx = {pParent->m_dVolume, ETSDIV_NM_STD|ETSDIV_NM_NO_END_NULLS, 1, 32, 0};
   CEtsDiv::NumberToText(&ntx, number);
   wsprintf(text, format, number);
   SetDlgItemText(IDC_CD_GRP_CABINET_PARAM, text);
}

void CBoxCabinetDesignPage::CheckWizardButtons()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   m_nFlags |= NOERRORWINDOW;
   OnWizardNext(NULL, false);
   m_nFlags &= ~NOERRORWINDOW;
   pParent->SetWizardButtons(true, false);
}

bool CBoxCabinetDesignPage::SetCurrentCabinet(int nCab, bool bResetCurrent)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   int nOld = pParent->m_nCurrentCab;
   if (nCab != pParent->m_nCurrentCab)
   {
      pParent->m_nCurrentCab = nCab;                           // neues setzen
      SendDlgItemMessage(IDC_CD_LST_CHASSIS_CAB, CB_SETCURSEL, nCab, 0);

      if (bResetCurrent && (m_pCurrent != NULL))               // Wenn aktuelles Chassis gelöscht werden soll
      {
         m_pCurrent = NULL;                                    // auf NULL setzen
         if (HIWORD(m_nWall) != nCab)
         {
            SetCurrentWall(MAKELONG(LOWORD(m_nWall), nCab));
            InitCabinetData();
            InitCabinetSizes();
            InitChassisPosList();
            ::InvalidateRect(m_hWndPosView, NULL, true);
         }
         InitChassisData(false);
      }
      else
      {
         InitCabinetData();
         InitCabinetSizes();
         InitChassisPosList();
         SetCurrentWall();
         if ((m_pCurrent != NULL) && (m_pCurrent->GetWall() == 0))
         {
            SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_SETCURSEL, 0, 0);
         }
         ::InvalidateRect(m_hWndPosView, NULL, true);
      }
                                                               // OnChangeCabinet im 3D-Dialog aufrufen
      pParent->SendMsgTo3DDlg(WM_COMMAND, MAKELONG(IDC_CD_LST_CHASSIS_CAB, nOld), (LPARAM)m_hWnd);
      return true;
   }
   return false;
}

void CBoxCabinetDesignPage::OnNewCabinet(HWND hwndControl)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet *pC = (CCabinet*)pParent->m_Cabinet.m_Cabinets.GetLast();

   if ((pC != NULL) && (SendDlgItemMessage(IDC_CD_LST_CHASSIS_CAB, CB_ADDSTRING, 0, (LPARAM)pC->m_szName) != CB_ERR))
   {                                                           // Slave-Liste updaten
      pParent->SendMsgTo3DDlg(WM_COMMAND, MAKELONG(IDC_3D_LST_CAB, CBN_CONTENT_CHANGED), (LPARAM)GetDlgItem(IDC_CD_LST_CHASSIS_CAB));
      if (SendDlgItemMessage(IDC_CD_LST_CHASSIS_CAB, CB_SETCURSEL, pParent->m_Cabinet.m_Cabinets.GetCounter(), 0) != CB_ERR)
      {                                                        // Master-Liste: Auswahl
         ::SendMessage(m_hWnd, WM_COMMAND, MAKELONG(IDC_CD_LST_CHASSIS_CAB, CBN_SELCHANGE), (LPARAM)hwndControl);
      }
   }
}

void CBoxCabinetDesignPage::Invalidate3D(int nWhat)
{
   ((CBoxPropertySheet*)m_pParent)->SendMsgTo3DDlg(WM_COMMAND, MAKELONG(IDC_BOX_VIEW, nWhat), (LPARAM)m_hWnd);
}

bool CBoxCabinetDesignPage::SetChassisLstSel(int nWall)
{
   int i, nCount = SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_GETCOUNT, 0, 0);
   for (i=0; i<nCount; i++)
   {
      if (SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_GETITEMDATA, i, 0) == nWall)
      {
         SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_SETCURSEL, i, 0);
         return true;
      }
   }
   SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_SETCURSEL, (WPARAM)-1, 0);
   return false;
}

bool CBoxCabinetDesignPage::OnSelChangeCabinet(WORD nID, HWND hwndControl)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   bool bChanged = false;
   int nCab = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
   CCabinet *pC = pParent->GetCabinet(nCab);
   if ((hwndControl != GetDlgItem(nID)) ||               // Message kommt von einer anderen DlgBox
       (m_pCurrent == NULL) ||                           // Kein Chassis ausgewählt
       !pC->CanContainChassis() ||                       // Quader kann keine Chassis enthalten
       ((m_pCurrent != NULL) && (m_pCurrent->GetWall() == 0)))// Chassis hat noch keine Wand
   {
      bool bResetCurrent = false;
//      if (hwndControl == pParent->m_p3DViewDlg->GetWindowHandle()) bResetCurrent = true;
      if (hwndControl != GetDlgItem(nID)) bResetCurrent = true;
      else if (!pC->CanContainChassis())  bResetCurrent = true;
      SetCurrentCabinet(nCab, bResetCurrent);
   }
   else if (m_pCurrent)
   {
      bChanged = false;
      if (nCab != m_pCurrent->GetCabinet())
      {
         if (MessageBox(IDS_CHANGE_CABINET_OF_CHASSIS,IDS_PROPSHCAPTION, MB_ICONQUESTION|MB_YESNO) == IDYES)
         {
            bChanged = true;
         }
      }
      if (bChanged)
      {
         m_pCurrent->SetCabinet(nCab);
         m_pCurrent->dwFlagsCH |= CHASSIS_3D_POS_INVALID;
         SetCurrentCabinet(nCab, false);
         CheckCurrentChassisPosition(true);
         InitChassisPos();
         AddCabinetsToChassis(m_pCurrent, m_pCurrent->GetWall());
         Invalidate3D(LIST_CHASSIS);
      }
      else
      {
         SetCurrentCabinet(nCab, true);
      }
   }
   return bChanged;
}

bool CBoxCabinetDesignPage::OnSelChangeWall(WORD nID, HWND /*hwndControl*/)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   bool bChanged = false;
   int nSel = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
   ASSERT(nSel != CB_ERR);

   if (m_pCurrent)
   {
      int nOldWall = m_pCurrent->GetWall();
      int nNewWall = SendDlgItemMessage(nID, CB_GETITEMDATA, nSel, 0);
      if (nOldWall != nNewWall)
      {
         bChanged = false;
         if (nOldWall == 0) bChanged = true;
         else if (nNewWall == 0) SendDlgItemMessage(nID, CB_SETCURSEL, nSel, 0);
         else if (MessageBox(IDS_CHANGE_WALL_OF_CHASSIS, IDS_PROPSHCAPTION, MB_ICONQUESTION|MB_YESNO) == IDYES)
         {
            bChanged = true;
         }
         if (bChanged)
         {
            m_pCurrent->nCabWall = MAKELONG(nNewWall, pParent->m_nCurrentCab);

            SetCurrentWall();
            m_pCurrent->dwFlagsCH |= CHASSIS_3D_POS_INVALID;
            CheckCurrentChassisPosition(true);
            bChanged = false; // Wizardbuttons nicht noch einmal testen
            InitChassisPos();
            AddCabinetsToChassis(m_pCurrent, nOldWall);
            ::InvalidateRect(m_hWndPosView, NULL, true);
            Invalidate3D(LIST_CHASSIS);
         }
         else
         {
            m_pCurrent = NULL;
            InitChassisData();
            SetCurrentWall();
            ::InvalidateRect(m_hWndPosView, NULL, true);
         }
      }
   }
   else
   {
      SetCurrentWall();
      ::InvalidateRect(m_hWndPosView, NULL, true);
   }
   return bChanged;
}

bool CBoxCabinetDesignPage::IsChassisOnWallSegments(ChassisData *pCD)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet          *pCbase  = pParent->GetCabinet(BASE_CABINET);
   int  i, nSegments = pCbase->GetNoOfWallSegments();
   bool bReturn = false;
   if (nSegments > 1)
   {
      HRGN hrgn = NULL;
      int *pnPoly = new int[nSegments];
      if (pnPoly)
      {
         for (i=0; i<nSegments; i++)
            pnPoly[i] = NO_OF_RECT_POINTS;
         hrgn = ::CreatePolyPolygonRgn(m_pptSegments, pnPoly, nSegments, WINDING);
         delete pnPoly;
      }
      if (hrgn)
      {
         if (pCD->IsRect())
         {
            RECT rc = pCD->GetRect(NULL, false, (CVector2D*)m_vrcWall);
            POINT pt;
            if (rc.top > rc.bottom)
            {
               i = rc.top;
               rc.top = rc.bottom;
               rc.bottom = i;
            }
            bReturn = true;
            pt.x = rc.left;
            for (pt.y=rc.top; pt.y<=rc.bottom; pt.y+=2)
            {
               if (!::PtInRegion(hrgn, pt.x, pt.y)) break;
            }
            if (pt.y<=rc.bottom) bReturn = false;
            else
            {
               pt.x = rc.right;
               for (pt.y=rc.top+1; pt.y<=rc.bottom; pt.y+=2)
               {
                  if (!::PtInRegion(hrgn, pt.x, pt.y)) break;
               }
               if (pt.y<=rc.bottom) bReturn = false;
               else
               {
                  pt.y = rc.top;
                  for (pt.x=rc.left+1; pt.x<=rc.right; pt.x+=2)
                  {
                     if (!::PtInRegion(hrgn, pt.x, pt.y)) break;
                  }
                  if (pt.x<=rc.right) bReturn = false;
                  else
                  {
                     pt.y = rc.bottom;
                     for (pt.x=rc.left; pt.x<=rc.right; pt.x+=2)
                     {
                        if (!::PtInRegion(hrgn, pt.x, pt.y)) break;
                     }
                     if (pt.x<=rc.right) bReturn = false;
                  }
               }
            }
         }
         else
         {
            double dCenterX = CCabinet::GetXCenterLine(m_vrcWall);
            double dXpos    = dCenterX + pCD->dDistFromCenter;
            double dYpos    = pCD->dHeightOnCabWall;
            double dRadius  = 0.5*pCD->dEffDiameter;
            double dDeltaBogen, dPhi;
            if (pCD->IsSphere()) dRadius = 5;   // 10 mm Durchmesser
            int nNoOfPoints = (int) ((pCD->dEffDiameter)*M_PI/2.5);
            POINT  pt;
            dDeltaBogen = 2.0*M_PI/nNoOfPoints;
            dPhi    = dDeltaBogen * 0.3;
            for(i=0; i<nNoOfPoints; i++)
            {
               pt.x = CEtsDiv::Round(dXpos + dRadius*cos(dPhi));
               pt.y = CEtsDiv::Round(dYpos + dRadius*sin(dPhi));
               if (!::PtInRegion(hrgn, pt.x, pt.y)) break;
               dPhi += dDeltaBogen;
            }
            if (i == nNoOfPoints) bReturn = true;
         }
         ::DeleteObject(hrgn);
      }
   }
   if (bReturn) pCD->dwFlagsCH |=  (CHASSIS_POS_VALID|CHASSIS_POS_ON_2_WALLS);
   else         pCD->dwFlagsCH &= ~CHASSIS_POS_VALID;
   return bReturn;
}

void CBoxCabinetDesignPage::UpdateCabinetList()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   CCabinet          *pCbase  = pParent->GetCabinet(BASE_CABINET);
   CCabinet *pSub = (CCabinet*) pCbase->m_Cabinets.GetFirst();
   int  nCount = SendDlgItemMessage(IDC_CD_LST_CHASSIS_CAB, CB_GETCOUNT, 0, 0);
   if (nCount) SendDlgItemMessage(IDC_CD_LST_CHASSIS_CAB, CB_RESETCONTENT, 0, 0); 
   SendDlgItemMessage(IDC_CD_LST_CHASSIS_CAB, CB_ADDSTRING, 0, (LPARAM)pCbase->m_szName);
   while (pSub)
   {
      SendDlgItemMessage(IDC_CD_LST_CHASSIS_CAB, CB_ADDSTRING, 0, (LPARAM)pSub->m_szName);
      pSub = (CCabinet*) pCbase->m_Cabinets.GetNext();
   }
   SendDlgItemMessage(IDC_CD_LST_CHASSIS_CAB, CB_SETCURSEL, pParent->m_nCurrentCab, 0);
                                                               // Slave-Liste updaten
   pParent->SendMsgTo3DDlg(WM_COMMAND, MAKELONG(IDC_3D_LST_CAB, CBN_CONTENT_CHANGED), (LPARAM)GetDlgItem(IDC_CD_LST_CHASSIS_CAB));
   pParent->m_nFlagChanged &= ~UPDATE_CABINET_LIST;
}

void CBoxCabinetDesignPage::InitChassisPosList()
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   int i, nWall;             // altes Gehäuse merken
   char szText[64];
   CCabinet *pC = pParent->GetCurrentCab();                 // aktuelles Gehäuse

   if (m_pCurrent)                                          // Chassis ausgewählt
   {
      i = 0;                                                // Wandliste Liste neu aufbauen
      SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_RESETCONTENT, 0, 0);
   
      ::LoadString(m_hInstance, IDS_NO_WALL, szText, 64);
      SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_ADDSTRING, i, (LPARAM)szText);
      if (m_pCurrent->GetWall() == 0)
      {
         if (pC->CanContainChassis())
         {
            SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_SETCURSEL, i++, 0);
         }
         else
         {
            ::LoadString(m_hInstance, IDS_CANNOT_CONTAIN_CHASSIS, szText, 64);
            SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_ADDSTRING, i, (LPARAM)szText);
            SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_SETCURSEL, i++, 0);
         }
      }
      if (pC->CanContainChassis())
      {
         CVector2D vrcWall[NO_OF_RECT_POINTS];
         for (nWall=CARABOX_CABWALL_TOP; nWall<=CARABOX_CABWALL_BOTTOM; nWall++)
         {
            if (m_pCurrent->IsSphere() && (nWall != CARABOX_CABWALL_TOP)) continue;
            if (pC->IsWallEnabled(nWall) && pC->GetCabinetWall(nWall, vrcWall) && pC->InitChassisPos(vrcWall, m_pCurrent, true))
//          if (pC->IsWallEnabled(nWall))
            {
               ::LoadString(m_hInstance, nWall+IDS_NO_WALL, szText, 64);
               SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_ADDSTRING, 0, (LPARAM)szText);
               if (nWall == m_pCurrent->GetWall())
                  SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_SETCURSEL, i, 0);
               SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_SETITEMDATA, i++, nWall);
            }
         }
      }
   }
   else
   {
      i = 0;                                                // Wandliste Liste neu aufbauen
      int nOldSel = SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_GETCURSEL, 0, 0);
      if (nOldSel != CB_ERR)
      {
         nOldSel = SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_GETITEMDATA, nOldSel, 0);
      }
      SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_RESETCONTENT, 0, 0);
      CVector2D vrcWall[NO_OF_RECT_POINTS];
      for (nWall=CARABOX_CABWALL_TOP; nWall<=CARABOX_CABWALL_BOTTOM; nWall++)
      {
         if (pC->IsWallEnabled(nWall))
         {
            ::LoadString(m_hInstance, nWall+IDS_NO_WALL, szText, 64);
            SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_ADDSTRING, 0, (LPARAM)szText);
            SendDlgItemMessage(IDC_CD_LST_CHASSIS_POS, CB_SETITEMDATA, i++, nWall);
         }
      }
      if (nOldSel != CB_ERR) SetChassisLstSel(nOldSel);
   }
}

bool CBoxCabinetDesignPage::AddCabinetsToChassis(ChassisData *pCD, int nOldWall)
{
   CBoxPropertySheet *pParent = (CBoxPropertySheet*)m_pParent;
   bool bReturn = false;

   if (pCD->IsSphere())                                        // eine Kugel
   {
      if (pCD->m_ppCabinets == NULL)                           // muß einen Fuß haben
      {
         pParent->InitSphereFoot(pCD);
         bReturn = true;
      }
   }
   else
   {
      CCabinet*pCBase = pParent->GetCabinet(BASE_CABINET);
      CCabinet*pCcd   = pParent->GetCabinet(pCD->GetCabinet());
      CCabinet*pCzmin = NULL;
      bool     bRemoveFeet = false;

      if ((pCD->GetWall() == CARABOX_CABWALL_BOTTOM) ||        // soll ein chassis auf die Bodenplatte gesetzt werden ?
          (nOldWall == CARABOX_CABWALL_BOTTOM))                // oder von der Bodenplatte entfernt werden ?
      {
         CCabinet*pC = (CCabinet*)pCBase->m_Cabinets.GetFirst();
         double   dTemp, dzmin, dBase = Vz(pCBase->GetCabPoint(CAB_PT_LEFT_BOTTOM_FRONT));
         dzmin = dBase;
         for ( ; pC!=NULL; pC=(CCabinet*)pCBase->m_Cabinets.GetNext())
         {
            if (pC->nCountTL) continue;                        // Abstandhalter unten nicht mitzählen
            if (pC->nCountHT) continue;                        // Abstandhalter Kugel nicht mitzählen
            dTemp = Vz(pC->GetCabPoint(CAB_PT_LEFT_BOTTOM_FRONT));
            if (dTemp < dBase)                                 // niedrigstes Gehäuse suchen
            {
               pCzmin = pC;
               dzmin  = dTemp;
            }
         }

         if ((pCzmin != NULL) && (pCzmin != pCcd))             // Gehäuse ist nicht das niedrigste
         {
            dzmin = dBase - dzmin;
            if (dzmin > 30)                                    // ist der Abstand zum Boden schon groß genug ?
            {
               bRemoveFeet = true;                             // so brauchen wir keine Füße mehr
            }
         }
      }

      if ((nOldWall == CARABOX_CABWALL_BOTTOM) && (pCD->GetWall() != CARABOX_CABWALL_BOTTOM))
         bRemoveFeet = true;                                   // wird das Chassis vom Boden entfernt ?

      if (bRemoveFeet)                                         // Füße entfernen ?
      {
         if (pCD->m_ppCabinets != NULL)                        // hat er welche ?
         {
            for (int i=0; i<NO_OF_RECT_POINTS; i++)
            {
               pCBase->m_Cabinets.Delete(pCD->m_ppCabinets[i]);
            }
            Free(pCD->m_ppCabinets);
            pCD->m_ppCabinets = NULL;
            bReturn = true;
         }
      }                                                        // Füße einfügen, wenn noch keine da sind
      else if ((pCD->m_ppCabinets == NULL) && (pCD->GetWall() == CARABOX_CABWALL_BOTTOM))
      {
         int       i;
         char      szFormat[CAB_NAME_LEN];
         ::LoadString(g_hInstance, IDS_FOOT_OF, szFormat, CAB_NAME_LEN);
         CCabinet *pCfoot = NULL;
         pCD->m_ppCabinets = (CCabinet**) Alloc(sizeof(CCabinet*)*NO_OF_RECT_POINTS, NULL);
         for (i=0; i<NO_OF_RECT_POINTS; i++)
         {
            pCfoot = new CCabinet;
            pCD->m_ppCabinets[i] = pCfoot;
            pCfoot->SetBaseCabinet(pCBase);              // Basisgehäuse setzen
            pCfoot->dHeight   = 50;                      // Höhe ist 50 mm
            pCfoot->dDepth    = 30;                      // Tiefe und Breite
            pCfoot->dWidth    = 30;                      // 30 mm
            pCfoot->nCountTL  = NO_OF_RECT_POINTS;       // Gehäuse ist Abstandhalter für unten
            pCfoot->nCountPM  = pCD->nCabWall;           // Gehäusewand des Chassis
            pCfoot->nCountPM |= ((i+1) << 8);            // Punktindex bzw.Position
            pCfoot->InitCabPoints();                     // Punkte initialisieren
            pCfoot->m_dwFlags |=  CANNOT_CONTAIN_CHASSIS;
            pCfoot->m_dwFlags &= ~CAN_CONTAIN_CHASSIS;
            wsprintf(pCfoot->m_szName, szFormat, i+1);
            strncat(pCfoot->m_szName, pCcd->m_szName, CAB_NAME_LEN);
         }
         pCD->dwFlagsCH |= CHASSIS_3D_POS_INVALID;
         pCBase->CalcChassisPos3D(pCD);
         bReturn = true;
      }
   }
   if (bReturn)
   {
      pParent->SendMsgTo3DDlg(WM_COMMAND, MAKELONG(IDC_3D_BTN_UPDATE, BN_LIST_CHANGED), (LPARAM)m_hWnd);
      UpdateCabinetList();
   }
   return bReturn;
}
