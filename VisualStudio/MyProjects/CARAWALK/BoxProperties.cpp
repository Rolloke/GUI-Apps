/********************************************************************/
/* Funktionen der Klasse CBoxPropertiesDlg                                  */
/********************************************************************/
#include "stdafx.h"
#include "BoxProperties.h"
#include "resource.h"
#include "CaraWalkDll.h"
#include "CEtsFileDlg.h"

#include "Ets3dGL.h"
#include "FileHeader.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define STRINGSIZE 256

extern HINSTANCE g_hInstance;
 
CBoxPropertiesDlg::CBoxPropertiesDlg()
{
   BEGIN("CBoxPropertiesDlg()");
   Constructor();
}

CBoxPropertiesDlg::CBoxPropertiesDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   BEGIN("CBoxPropertiesDlg(..)");
   Constructor();
}

void CBoxPropertiesDlg::Constructor()
{
   BEGIN("Constructor");
   m_nCurSel   = 0;
   m_nPolygon  = 0;
   m_nMaterial = -1;
   gm_nIDAPPLY = IDAPPLY;
   m_Boxes.SetDestroyMode(false);
   m_pSelBox   = NULL;
   m_pCaraWalk = NULL;
}

CBoxPropertiesDlg::~CBoxPropertiesDlg()
{
   BEGIN("~CBoxPropertiesDlg()");
   SetpBox(NULL);
}

bool CBoxPropertiesDlg::OnInitDialog(HWND hWnd) 
{
   BEGIN("OnInitDialog");
   CEtsDialog::OnInitDialog(hWnd);
   UpdateNewData();
   InitComboBox();
   m_pSelBox = (CBoxObj*) m_Boxes.GetAt(m_nCurSel);
   if (m_pSelBox)
   {
      SendDlgItemMessage(IDC_BPR_POLY_SPIN, UDM_SETRANGE32, 0, m_pSelBox->GetNoOfPolygons());
      SendDlgItemMessage(IDC_BPR_POLY_SPIN, UDM_SETPOS, 0, 0);
      InitMaterialParam(m_pSelBox->GetMaterial(0));
   }
   SetNumeric(IDC_BPR_SHININESS_EDT, true, true);
   SetNumeric(IDC_BPR_SCALE_X_EDT, true, true);
   SetNumeric(IDC_BPR_SCALE_Y_EDT, true, true);
   return true;
}


void CBoxPropertiesDlg::SetpBox(CBoxObj *pBdd)
{
   BEGIN("SetpBox");
   if (pBdd != NULL)
   {
      m_Boxes.ADDHead(pBdd);
   }
   else
   {
      m_Boxes.Destroy();
   }
}

void CBoxPropertiesDlg::UpdateNewData()
{
   BEGIN("UpdateNewData");
   if (m_Boxes.GetCounter()>0)
   {
      char text[STRINGSIZE];
      SendDlgItemMessage(IDC_BOXDLG_BOXCOMBO, CB_RESETCONTENT, 0, 0);

      CBoxObj *pBox;
      for (pBox=(CBoxObj*)m_Boxes.GetFirst(); pBox!=NULL; pBox=(CBoxObj*) m_Boxes.GetNext())
      {
         wsprintf(text,"%s", pBox->GetBoxName());
         SendDlgItemMessage(IDC_BOXDLG_BOXCOMBO, CB_ADDSTRING, 0, (LPARAM)text);
      }

      SendDlgItemMessage(IDC_BOXDLG_BOXCOMBO, CB_SETCURSEL, m_nCurSel, 0);

      CEtsDialog::OnOk(0);
   }
}

int CBoxPropertiesDlg::OnOk(WORD)
{
   BEGIN("OnOk");
   HWND hwndFocus = GetFocus();

   if ((hwndFocus == GetDlgItem(IDOK)) ||
       (hwndFocus == GetDlgItem(IDAPPLY)) ||
       (hwndFocus == GetDlgItem(IDC_BOXDLG_BOXCOMBO)))
   {
      return CEtsDialog::OnOk((WORD)m_nCurSel);
   }
   else
   {
      SetFocus(GetNextDlgTabItem(m_hWnd, hwndFocus, false));
   }
   return 0;
}


int CBoxPropertiesDlg::OnCancel()
{
   BEGIN("OnCancel");
   return IDCANCEL;
}

int CBoxPropertiesDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   switch(nID)
   {
/****************************************************************************************/
      case IDC_MATERIAL_COMBO:
      if (nNotifyCode == CBN_SELCHANGE)
      {
         int nCursel = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
//         int nCount  = SendDlgItemMessage(nID, CB_GETCOUNT, 0, 0);
         if (nCursel != CB_ERR)
         {
            CMaterial *pM;
            if (nCursel == 0) pM = NULL;                       // Kein Material
            if (nCursel == 1)                                  // Neues Material
            {
               if (!m_pCaraWalk) pM = NULL;
               else
               {
                  pM = m_pCaraWalk->GetMaterial(NULL, true);
                  if (pM)
                  {
                     char szText[64];
                     pM->nMatNo = m_nMaterial;
                     m_nMaterial--;
                     GetMaterialText(pM, szText);
                     SendDlgItemMessage(nID, CB_ADDSTRING, 0, (LPARAM)szText);
                  }
               }
            }
            else
            {
               if (!m_pCaraWalk) pM = NULL;
               else
               {
                  int nIndex = nCursel-2;
                  pM = m_pCaraWalk->GetMaterialFromIndex(nIndex);
               }
            }
            m_pSelBox->SetMaterial(m_nPolygon, pM);
            InitMaterialParam(pM);
         }
      } break;
      case IDC_BOXDLG_BOXCOMBO:                    // Combobox
      if (nNotifyCode == CBN_SELCHANGE)
      {
         int nCursel = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
         m_pSelBox   = (CBoxObj*)m_Boxes.GetAt(nCursel);
         if (m_pSelBox)
         {
            SendDlgItemMessage(IDC_BPR_POLY_SPIN, UDM_SETRANGE32, 0, m_pSelBox->GetNoOfPolygons());
            SendDlgItemMessage(IDC_BPR_POLY_SPIN, UDM_SETPOS, 0, 0);
            InitMaterialParam(m_pSelBox->GetMaterial(0));
         }
      } return 0;
      case IDC_BPR_POLY_EDT:
      if (nNotifyCode == EN_CHANGE)
      {
         bool bChanged = false;
         GetDlgItemInt(nID, m_nPolygon, false, -1, &bChanged);
         if (bChanged)
         {
            if (m_pSelBox)
            {
               CVector vfln = m_pSelBox->SelectPolygon(m_nPolygon);
               REPORT("%d, (%f, %f, %f)", m_nPolygon, Vx(vfln), Vy(vfln), Vz(vfln));               
/*
               CBoxPosition *pPos = m_pSelBox->GetpPosition(0);
               pPos->m_nPsi   = 0;
               pPos->m_nTheta = 90 -GRAD(atan2(-Vz(vfln), Vy(vfln)));
               pPos->m_nPhi   = 0;
*/
               InitMaterialParam(m_pSelBox->GetMaterial(m_nPolygon));
               WPARAM wParam = MAKELONG(m_nID, (WORD)0);
               LPARAM lParam = (LPARAM) m_hWnd;
               PostMessage(m_hWndParent, WM_COMMAND, wParam, lParam);
            }
         }

      }return 0;
   }
   if ((nNotifyCode == BN_CLICKED) && (m_pSelBox != NULL))
   {
      bool bFound = false;
      CMaterial *pM = m_pSelBox->GetMaterial(m_nPolygon);
      if (pM)
      {
         COLORREF color = 0;
         bFound = true;
         switch (nID)
         {
            case IDC_BTN_AMBIENT:
            color = CGlLight::FloatsToColor(pM->m_pfAmbient);
            if (CEtsDialog::ChooseColorT("ambient", color, m_hWnd, m_hInstance))
            {
               CGlLight::ColorToFloat(color, pM->m_pfAmbient);
               InvalidateRect(GetDlgItem(IDC_AMBIENT_COL), NULL, false);
            }break;
            case IDC_BTN_DIFFUSE:
            color = CGlLight::FloatsToColor(pM->m_pfDiffuse);
            if (CEtsDialog::ChooseColorT("diffuse", color, m_hWnd, m_hInstance))
            {
               CGlLight::ColorToFloat(color, pM->m_pfDiffuse);
               InvalidateRect(GetDlgItem(IDC_DIFFUSE_COL), NULL, false);
            }break;
            case IDC_BTN_SPECULAR:
            color = CGlLight::FloatsToColor(pM->m_pfSpecular);
            if (CEtsDialog::ChooseColorT("specular", color, m_hWnd, m_hInstance))
            {
               CGlLight::ColorToFloat(color, pM->m_pfSpecular);
               InvalidateRect(GetDlgItem(IDC_SPECULAR_COL), NULL, false);
            }break;
            case IDC_BTN_EMISSION:
            color = CGlLight::FloatsToColor(pM->m_pfEmision);
            if (CEtsDialog::ChooseColorT("emmission", color, m_hWnd, m_hInstance))
            {
               CGlLight::ColorToFloat(color, pM->m_pfEmision);
               InvalidateRect(GetDlgItem(IDC_EMISSION_COL), NULL, false);
            }break;
            case IDC_BTN_COLOR:
            color = CGlLight::FloatsToColor(pM->m_pfAverageImageColor);
            if (CEtsDialog::ChooseColorT("color", color, m_hWnd, m_hInstance))
            {
               CGlLight::ColorToFloat(color, pM->m_pfAverageImageColor);
               InvalidateRect(GetDlgItem(IDC_MATERIAL_COL), NULL, false);
            }break;
            case IDC_BTN_BITMAP:
            {
               CEtsFileDlg dlg(m_hInstance, 0, m_hWnd);
               dlg.SetFilter(IDS_BMP_FILTER);
               dlg.ModifyFlags(OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST, 0);
               const char *pszName = pM->GetBitmapFileName();
               if (pszName) dlg.SetInitialDir(pszName);
               if (dlg.GetOpenFileName())
               {
                  char*pszFN = (char*)dlg.GetFilePathName();
                  BITMAPINFO *pbmi = NULL;
                  CCaraWalkDll::GetBmpFile(pszFN, &pbmi);
                  if (pbmi)
                  {
                     if (pM->SetTexture(pbmi))
                     {
                        pM->SetBitmapFileName(pszFN);
                     }
                     CCaraWalkDll::Free(pbmi);
                  }
                  InitComboBox();
                  InitMaterialParam(pM);
               }
            }break;
            case IDC_CK_BMP_FILE:
            {
               pM->m_bSaveBmpAsFile = (SendDlgItemMessage(nID, BM_GETCHECK,0,0) == BST_CHECKED)? true : false;
            } break;
            case IDC_CK_TEX_GENCOORD:
            {
               pM->m_bGenerateCoordinates = (SendDlgItemMessage(nID, BM_GETCHECK,0,0) == BST_CHECKED)? true : false;
            } break;
            default: bFound = false;
         }
      }
      if (bFound) return 0;
      bFound = true;
      switch (nID)
      {
         case IDC_BPR_SAVE_BOX:
         {
            char szFilePath[_MAX_PATH];
            CFileHeader fh;
            m_pCaraWalk->SetBoxPath(szFilePath, (char*)m_pSelBox->GetFileName());
            HANDLE hFile = CreateFile(szFilePath, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
               DWORD dwRead;
               do
               {
                  ::ReadFile(hFile, &fh, sizeof(fh), &dwRead, NULL);
                  if (dwRead == sizeof(fh))
                  {
                     if (fh.IsType(BOX_MATERIAL_DATA))
                        break;
                     ::SetFilePointer(hFile, fh.GetLength(), NULL, FILE_CURRENT);
                  }
               }while (dwRead == sizeof(fh));

               if (!fh.IsType(BOX_MATERIAL_DATA))
               {
                  fh.SetType(BOX_MATERIAL_DATA);
                  fh.SetComment("");
                  fh.CalcChecksum();
                  ::WriteFile(hFile, &fh, sizeof(fh), &dwRead, NULL);
               }
               m_pSelBox->WriteMaterial(hFile, &fh);
               CloseHandle(hFile);
            }
         }break;
         case ID_CLOSE:
         {
            OnCancel();
            OnEndDialog(0);
         } break;
         case IDOK:    OnOk(0);   break;
         case IDAPPLY: OnApply(); break;
         default: bFound = false;
      }
      if (bFound) return 0;
   }
   if ((nNotifyCode == EN_KILLFOCUS) && (m_pSelBox != NULL))
   {
//      bool bFound   = true;
      bool bChanged = false;
      CMaterial *pM = m_pSelBox->GetMaterial(m_nPolygon);
      if (pM)
      {
         double dTemp;
         switch (nID)
         {
            case IDC_BPR_SCALE_X_EDT:
            if (GetDlgItemDouble(nID, dTemp, 0, &bChanged) && bChanged)
            {
               CheckMinMaxDouble(nID, 0.001, 1000.0, 2, dTemp);
               pM->m_fsScaleFactor = (float) dTemp;
            }break;
            case IDC_BPR_SCALE_Y_EDT:
            if (GetDlgItemDouble(nID, dTemp, 0, &bChanged) && bChanged)
            {
               CheckMinMaxDouble(nID, 0.001, 1000.0, 2, dTemp);
               pM->m_ftScaleFactor = (float) dTemp;
            }break;
            case IDC_BPR_SHININESS_EDT:
            if (GetDlgItemDouble(nID, dTemp, 0, &bChanged) && bChanged)
            {
               CheckMinMaxDouble(nID, 0.0, 128.0, 2, dTemp);
               pM->m_fShininess = (float) dTemp;
            }break;
//            default: bFound = false;
         }
      }
   }
   return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
}

int CBoxPropertiesDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
//   BEGIN("OnMessage");
   switch(nMsg)
   {
      case WM_DRAWITEM:
      if ((wParam >= IDC_AMBIENT_COL) && (wParam <= IDC_MATERIAL_COL))
      {
         DRAWITEMSTRUCT *pDI = (DRAWITEMSTRUCT*) lParam;
         COLORREF color = 0;
         CMaterial *pM = m_pSelBox->GetMaterial(m_nPolygon);
         if (pM)
         {
            HBRUSH hBr = NULL;
            switch (pDI->CtlID)
            {
               case IDC_AMBIENT_COL:  color = CGlLight::FloatsToColor(pM->m_pfAmbient); break;
               case IDC_DIFFUSE_COL:  color = CGlLight::FloatsToColor(pM->m_pfDiffuse); break;
               case IDC_SPECULAR_COL: color = CGlLight::FloatsToColor(pM->m_pfSpecular); break;
               case IDC_EMISSION_COL: color = CGlLight::FloatsToColor(pM->m_pfEmision); break;
               case IDC_MATERIAL_COL: color = CGlLight::FloatsToColor(pM->m_pfAverageImageColor); break;
               default: break;
            }
            hBr = ::CreateSolidBrush(color);
            ::FillRect(pDI->hDC, &pDI->rcItem, hBr);
            ::DeleteObject(hBr);
         }
         else ::FillRect(pDI->hDC, &pDI->rcItem, ::GetSysColorBrush(COLOR_MENU));
         return 0;
      }
      else if (wParam == IDC_BTN_BITMAP)
      {
         DRAWITEMSTRUCT *pDI = (DRAWITEMSTRUCT*) lParam;
         CMaterial *pM = m_pSelBox->GetMaterial(m_nPolygon);
         BYTE *pBits = NULL;
         if (pM) pBits = pM->GetTexImage();
         if (pBits)
         {
            BITMAPINFO bmi;
            bmi.bmiHeader.biBitCount      = 24;
            bmi.bmiHeader.biClrImportant  =  0;
            bmi.bmiHeader.biClrUsed       =  0;
            bmi.bmiHeader.biPlanes        =  1;
            bmi.bmiHeader.biSize          =  sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biXPelsPerMeter =  MulDiv(GetDeviceCaps(pDI->hDC, LOGPIXELSX), 10000, 254);
            bmi.bmiHeader.biYPelsPerMeter =  MulDiv(GetDeviceCaps(pDI->hDC, LOGPIXELSY), 10000, 254);
            bmi.bmiHeader.biCompression   = BI_RGB;
            bmi.bmiHeader.biHeight        = pM->m_nHeight;
            bmi.bmiHeader.biWidth         = pM->m_nWidth;
            bmi.bmiHeader.biSizeImage     = (bmi.bmiHeader.biBitCount>>3) * pM->m_nHeight * pM->m_nWidth;

            BYTE col;
            unsigned int i;
            for (i=0; i<bmi.bmiHeader.biSizeImage; i+=3)
            {
               col = pBits[i+0];
                     pBits[i+0] = pBits[i+2];
                                  pBits[i+2] = col;
            }
            ::StretchDIBits(pDI->hDC, 0, 0, pDI->rcItem.right, pDI->rcItem.bottom,
               0, 0, pM->m_nWidth, pM->m_nHeight, pBits, &bmi, DIB_RGB_COLORS, SRCCOPY);

            for (i=0; i<bmi.bmiHeader.biSizeImage; i+=3)
            {
               col = pBits[i+0];
                     pBits[i+0] = pBits[i+2];
                                  pBits[i+2] = col;
            }
            return 0;
         }
         ::SetTextAlign(pDI->hDC, TA_CENTER|TA_BASELINE);
         ::TextOut(pDI->hDC, pDI->rcItem.right>>1, pDI->rcItem.bottom>>1, "Textur", strlen("Textur"));
      }
      break;
      default:  return CEtsDialog::OnMessage(nMsg, wParam, lParam);
   }
   return 0;
}

void CBoxPropertiesDlg::InitMaterialParam(CMaterial *pM)
{
   bool bEnable = (pM != NULL) ? true : false;
   ::EnableWindow(GetDlgItem(IDC_BTN_AMBIENT), bEnable);
   ::EnableWindow(GetDlgItem(IDC_BTN_DIFFUSE), bEnable);
   ::EnableWindow(GetDlgItem(IDC_BTN_SPECULAR), bEnable);
   ::EnableWindow(GetDlgItem(IDC_BTN_EMISSION), bEnable);
   ::EnableWindow(GetDlgItem(IDC_BTN_COLOR), bEnable);
   ::EnableWindow(GetDlgItem(IDC_BTN_BITMAP), bEnable);
   ::EnableWindow(GetDlgItem(IDC_CK_BMP_FILE), bEnable);
   ::EnableWindow(GetDlgItem(IDC_CK_TEX_GENCOORD), bEnable);
   ::EnableWindow(GetDlgItem(IDC_BPR_SCALE_X_EDT), bEnable);
   ::EnableWindow(GetDlgItem(IDC_BPR_SCALE_Y_EDT), bEnable);

   int nSel = 0;
   if (bEnable)
   {
      char szText[64];
      GetMaterialText(pM, szText);
      nSel = SendDlgItemMessage(IDC_MATERIAL_COMBO, CB_FINDSTRING, 2, (LPARAM)szText);
      SetDlgItemDouble(IDC_BPR_SCALE_X_EDT, pM->m_fsScaleFactor, 2);
      SetDlgItemDouble(IDC_BPR_SCALE_Y_EDT, pM->m_ftScaleFactor, 2);
      SetDlgItemDouble(IDC_BPR_SHININESS_EDT, pM->m_fShininess, 2);
      SetDlgItemInt(IDC_BPR_SIZE_X_EDT, pM->m_nWidth , false);
      SetDlgItemInt(IDC_BPR_SIZE_Y_EDT, pM->m_nHeight, false);
      SendDlgItemMessage(IDC_CK_TEX_GENCOORD, BM_SETCHECK, pM->m_bGenerateCoordinates ? BST_CHECKED : BST_UNCHECKED,0);
      SendDlgItemMessage(IDC_CK_BMP_FILE    , BM_SETCHECK, pM->m_bSaveBmpAsFile       ? BST_CHECKED : BST_UNCHECKED,0);
   }
   SendDlgItemMessage(IDC_MATERIAL_COMBO, CB_SETCURSEL, nSel, 0);

   ::InvalidateRect(GetDlgItem(IDC_BTN_BITMAP), NULL, true);
   ::InvalidateRect(GetDlgItem(IDC_AMBIENT_COL), NULL, true);
   ::InvalidateRect(GetDlgItem(IDC_DIFFUSE_COL), NULL, true);
   ::InvalidateRect(GetDlgItem(IDC_SPECULAR_COL), NULL, true);
   ::InvalidateRect(GetDlgItem(IDC_EMISSION_COL), NULL, true);
   ::InvalidateRect(GetDlgItem(IDC_MATERIAL_COL), NULL, true);

   ::InvalidateRect(GetDlgItem(IDC_BTN_BITMAP), NULL, true);
}

void CBoxPropertiesDlg::InitComboBox()
{
   SendDlgItemMessage(IDC_MATERIAL_COMBO, CB_RESETCONTENT, 0, 0);
   SendDlgItemMessage(IDC_MATERIAL_COMBO, CB_ADDSTRING, 0, (LPARAM)"kein Material");
   SendDlgItemMessage(IDC_MATERIAL_COMBO, CB_ADDSTRING, 0, (LPARAM)"neues Material");
   if (m_pCaraWalk)
   {
      int nIndex = 0;
      CMaterial *pM;
      char szText[64];
      while ((pM = m_pCaraWalk->GetMaterialFromIndex(nIndex)) != NULL)
      {
         GetMaterialText(pM, szText);
         if (pM->nMatNo < m_nMaterial) m_nMaterial = pM->nMatNo - 1;
         SendDlgItemMessage(IDC_MATERIAL_COMBO, CB_ADDSTRING, 0, (LPARAM)szText);
         nIndex++;
      }
   }
}

void CBoxPropertiesDlg::GetMaterialText(CMaterial *pM, char *pszText)
{
   const char *pFN;
   pFN = pM->GetBitmapFileName();
   if (pFN)
   {
      int i, nLen = strlen(pFN);
      for (i=nLen-1; i>=0; i--)
         if (pFN[i] == '\\')
         {
            i++;
            break;
         }
      wsprintf(pszText, "%s, M %d", &pFN[i], pM->nMatNo);
   }
   else
   {
      wsprintf(pszText, "Material %d", pM->nMatNo);
   }
}

void CBoxPropertiesDlg::OnEndDialog(int n)
{
   if (m_pSelBox)
   {
      m_pSelBox->SelectPolygon(-1);
      WPARAM wParam = MAKELONG(m_nID, (WORD)0);
      LPARAM lParam = (LPARAM) m_hWnd;
      PostMessage(m_hWndParent, WM_COMMAND, wParam, lParam);
   }
   CEtsDialog::OnEndDialog(n);
}
