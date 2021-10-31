/********************************************************************/
/* Funktionen der Klasse CLightdlg                                  */
/********************************************************************/
#include "stdafx.h"
#include "LightDlg.h"
#include "resource.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define STRINGSIZE 256

CLightdlg::CLightdlg()
{
   BEGIN("CLightdlg()");
   Constructor();
}

CLightdlg::CLightdlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   BEGIN("CLightdlg(..)");
   Constructor();
}

void CLightdlg::Constructor()
{
   m_pLp          = NULL;
   m_nCurSel      = 1;
   m_nLightIntens = 50;
   gm_nIDAPPLY    = IDAPPLY;
}

bool CLightdlg::OnInitDialog(HWND hWnd) 
{
   BEGIN("OnInitDialog");
   CEtsDialog::OnInitDialog(hWnd);

//   char text[STRINGSIZE]="";

   SendDlgItemMessage(IDC_LIGHTDLG_LIGHTCOMBO, CB_SETCURSEL, m_nCurSel, 0);
//   SendDlgItemMessage(IDC_LIGHTDLG_LIGHTCOMBO, CB_GETLBTEXT, m_nCurSel, (LPARAM)text);
//   SetDlgItemText(IDC_LIGHTDLG_LIGHTCOMBO, text);

   SendDlgItemMessage(IDC_LIGHTDLG_DIMMER, TBM_SETRANGE, 0, MAKELONG(0, 100));
   for (int i=10; i<100; i+=10)
   {
      SendDlgItemMessage(IDC_LIGHTDLG_DIMMER, TBM_SETTIC, 0, i);
   }

   if (m_pLp)
   {
      if (m_nCurSel > 0) m_Lp = m_pLp[m_nCurSel-1];
      UpdateParams();
   }
   return true;
}

int CLightdlg::OnOk(WORD)
{
   HWND hwndFocus = GetFocus();

   if ((hwndFocus == GetDlgItem(IDOK)) ||
       (hwndFocus == GetDlgItem(IDAPPLY)) ||
       (hwndFocus == GetDlgItem(IDC_LIGHTDLG_LIGHTCOMBO)))
   {
      if (m_LightColor == 0) m_LightColor = RGB(255,255,255);
      BYTE red   = GetRValue(m_LightColor);
      BYTE green = GetGValue(m_LightColor);
      BYTE blue  = GetBValue(m_LightColor);
      float factor = 1.0f / 255.0f;
      m_Lp.m_pfAmbientColor[0] = (factor * (float)red  );
      m_Lp.m_pfAmbientColor[1] = (factor * (float)green);
      m_Lp.m_pfAmbientColor[2] = (factor * (float)blue );
      m_Lp.m_pfDiffuseColor[0] = (factor * (float)red  );
      m_Lp.m_pfDiffuseColor[1] = (factor * (float)green);
      m_Lp.m_pfDiffuseColor[2] = (factor * (float)blue );
      factor = (float)(0.01 * m_nLightIntens);
      for (int i=0; i<3; i++)
      {
         m_Lp.m_pfAmbientColor[i] *= factor;
         m_Lp.m_pfDiffuseColor[i] *= factor;
         if (m_nCurSel==0) m_pfAmbient[i] = m_Lp.m_pfAmbientColor[i];
      }
      if ((m_nCurSel==0) && IsWindow(m_hWndParent))
      {
         red   = (BYTE) (m_pfAmbient[0] * 255.0f);
         green = (BYTE) (m_pfAmbient[1] * 255.0f);
         blue  = (BYTE) (m_pfAmbient[2] * 255.0f);
         COLORREF color = RGB(red, green, blue);
         WPARAM wParam = MAKELONG(m_nID, (WORD)0xffff);
         LPARAM lParam = (LPARAM) color;
         PostMessage(m_hWndParent, WM_COMMAND, wParam, lParam);
         SetChanged(false);
         return IDOK;
      }
      else
      {
         m_pLp[m_nCurSel-1]  = m_Lp;
         return CEtsDialog::OnOk((WORD)(m_nCurSel-1));
      }
   }
   else
   {
      SetFocus(GetNextDlgTabItem(m_hWnd, hwndFocus, false));
   }
   return 0;
}

void CLightdlg::UpdateParams()
{
   BEGIN("UpdateParams");
   BYTE cMax, bFactor = 255, red, green, blue;
   float *pfAmbient = &m_Lp.m_pfAmbientColor[0];
   if (m_nCurSel==0)
   {
      pfAmbient = &m_pfAmbient[0];
      m_Lp.m_bEnabled = ((pfAmbient[0] != 0.0) && (pfAmbient[1] != 0.0) && (pfAmbient[2] != 0.0)) ? true : false;
   }

   red   = (BYTE)(pfAmbient[0] * bFactor),
   green = (BYTE)(pfAmbient[1] * bFactor),
   blue  = (BYTE)(pfAmbient[2] * bFactor);
      
   cMax = red;
   if (green > cMax) cMax = green;
   if (blue  > cMax) cMax = blue;
   if (cMax > 0) 
   {
      m_nLightIntens = MulDiv(cMax, 100, bFactor);
      red            = (unsigned char)MulDiv(red  , bFactor, cMax);
      green          = (unsigned char)MulDiv(green, bFactor, cMax);
      blue           = (unsigned char)MulDiv(blue , bFactor, cMax);
   }
   else
   {
      m_nLightIntens = 0;
   }

   EnableWindow(GetDlgItem(IDC_LIGHTDLG_CUT_OFF), m_nCurSel);
   EnableWindow(GetDlgItem(IDC_LIGHTDLG_EXPONENT), m_nCurSel);
   EnableWindow(GetDlgItem(IDC_LIGHTDLG_LIGHTON), m_nCurSel);
   EnableWindow(GetDlgItem(IDC_LIGHTDLG_SPOT), m_nCurSel);

   m_LightColor = RGB(red, green, blue);

   if (m_nCurSel>0)
   {
      SetDlgItemDouble(IDC_LIGHTDLG_CUT_OFF,  2.0     * (double)m_Lp.m_fSpotCutOff, 1);
      SetDlgItemDouble(IDC_LIGHTDLG_EXPONENT, 0.78125 * (double)m_Lp.m_fSpotExponent, 1);
      SendDlgItemMessage(IDC_LIGHTDLG_LIGHTON, BM_SETCHECK, (m_Lp.m_bEnabled) ? BST_CHECKED : BST_UNCHECKED, 0);
      SendDlgItemMessage(IDC_LIGHTDLG_SPOT, BM_SETCHECK, (m_Lp.m_pfPosition[3] == 0.0) ? BST_CHECKED : BST_UNCHECKED, 0);
      OnLightDlgSpot();
   }
   else
      SendDlgItemMessage(IDC_LIGHTDLG_LIGHTON, BM_SETCHECK, BST_CHECKED, 0);
   SendDlgItemMessage(IDC_LIGHTDLG_DIMMER, TBM_SETPOS, true, m_nLightIntens);
   SetChanged(false);
}

int CLightdlg::OnCancel()
{
   return IDCANCEL;
}

int CLightdlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   switch(nID)
   {
/****************************************************************************************/
      case IDC_LIGHTDLG_LIGHTCOMBO:                // ComboBox
      {
         if (nNotifyCode == CBN_SELCHANGE)
         {
            if (m_pLp)
            {
               if (IsChanged())
               {
                  if (MessageBox(IDS_VALUES_CHANGED, IDS_MSG_CARA_WALK, MB_YESNO|MB_ICONQUESTION) == IDYES)
                  {
                     OnOk(0);
                     SetChanged(false);
                  }
               }
               m_nCurSel = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
               if (m_nCurSel > 0) m_Lp = m_pLp[m_nCurSel-1];
               UpdateParams();
            }
         }
      } break;
/****************************************************************************************/
      case IDC_LIGHTDLG_CUT_OFF:                   // Öffnungswinkel
      if (nNotifyCode==EN_KILLFOCUS)
      {
         double dValue = 0;
         if(GetDlgItemDouble(nID, dValue))
         {
            if (dValue == 360.0) dValue  = 180.0;
            else
            {
               CheckMinMaxDouble(nID, 0.0, 180.0, 2, dValue);
               dValue *= 0.5;
            }
            if (m_Lp.m_fSpotCutOff != (float)dValue)
            {
               m_Lp.m_fSpotCutOff = (float) dValue;
               SetChanged(true);
            }
         }
      } break;
/****************************************************************************************/
      case IDC_LIGHTDLG_EXPONENT:                  // Bündelung
      if (nNotifyCode==EN_KILLFOCUS)
      {
         double dValue = 0;
         if(GetDlgItemDouble(nID, dValue))
         {
            CheckMinMaxDouble(nID, 0.0, 100.0, 2, dValue);
            dValue *= 1.28;
            if (m_Lp.m_fSpotExponent != (float)dValue)
            {
               m_Lp.m_fSpotExponent = (float) dValue;
               SetChanged(true);
            }
         }
      } break;
/****************************************************************************************/
      case IDC_LIGHTDLG_LIGHTON:                   // Licht an, aus
      {
         m_Lp.m_bEnabled = (SendDlgItemMessage(nID, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true : false;
         SetChanged(true);
      } break;
/****************************************************************************************/
      case IDC_LIGHTDLG_SPOT:  return OnLightDlgSpot();
      case IDC_LIGHTDLG_COLOR: return OnLightColor();
      case IDOK:    OnOk(0);   break;
      case IDAPPLY: OnApply(); break;
      default:
      return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
   }
   return 0;
}

int CLightdlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch(nMsg)
   {
      case WM_NOTIFY:
      {
//         NMHDR *nmHdr = (NMHDR*) lParam;
         switch(wParam)
         {
/////////////////////////////////////////////////////////////////////////////////////////
            case IDC_LIGHTDLG_DIMMER:              // Lichtintensität
            {
               int nLightIntens = SendDlgItemMessage(wParam, TBM_GETPOS, 0, 0);
               if (m_nLightIntens != nLightIntens)
               {
                  m_nLightIntens = nLightIntens;
                  SetChanged(true);
               }
            }break;
         }
      }break;
      default:
         return CEtsDialog::OnMessage(nMsg, wParam, lParam);
   }
   return 0;
}

int CLightdlg::OnLightColor()
{
   BEGIN("OnLightColor");
   char text[STRINGSIZE];
   ::LoadString(m_hInstance, IDS_LIGHTCOLORDLG_HEADING, text, STRINGSIZE);
   if (ChooseColorT(text, m_LightColor, m_hWnd, m_hInstance)) SetChanged(true);
   return 0;
}

int CLightdlg::OnLightDlgSpot()
{
   BEGIN("OnLightDlgSpot");
   if (SendDlgItemMessage(IDC_LIGHTDLG_SPOT, BM_GETCHECK, 0,0) == BST_CHECKED)
   {                                            // Lichtquelle für paralleles Licht
      m_Lp.m_pfPosition[3] = 0.0;
      EnableWindow(GetDlgItem(IDC_LIGHTDLG_CUT_OFF), false);
      EnableWindow(GetDlgItem(IDC_LIGHTDLG_EXPONENT), false);
   }
   else                                         // Lichtquelle als Strahler
   {
      m_Lp.m_pfPosition[3] = 1.0;
      EnableWindow(GetDlgItem(IDC_LIGHTDLG_CUT_OFF) , true);
      EnableWindow(GetDlgItem(IDC_LIGHTDLG_EXPONENT), true);
   }
   SetChanged(true);
   return 0;
}

#undef STRINGSIZE

void CLightdlg::SetAmbientColor(COLORREF color)
{
   float factor = 1.0f / 255.0f;
   m_pfAmbient[0] = GetRValue(color)*factor;
   m_pfAmbient[1] = GetGValue(color)*factor;
   m_pfAmbient[2] = GetBValue(color)*factor;
}
