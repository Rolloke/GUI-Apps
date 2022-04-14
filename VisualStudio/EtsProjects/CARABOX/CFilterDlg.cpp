/********************************************************************/
/* Funktionen der Klasse CFilterDlg                                */
/********************************************************************/
#include "CFilterDlg.h"
#include "resource.h"
#include "BoxPropertySheet.h"
#include <COMMCTRL.H>

extern int g_nUserMode;              // Registryparameter

CFilterDlg::CFilterDlg()
{
   Constructor();
}

CFilterDlg::CFilterDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

void CFilterDlg::Constructor()
{
   ZeroMemory(&m_Filter, sizeof(FilterBox));
   m_bHP = m_bTP = false;
   m_nTPMinOrder = m_nTPMaxOrder = m_nHPMinOrder = m_nHPMaxOrder = 0;
   m_dMinFrq = m_dMaxFrq = 0.0;
   m_bFrqChanged = false;
}

bool CFilterDlg::OnInitDialog(HWND hWnd) 
{
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      SetNumeric(IDC_FILTER_EDT_HP_QF, true, true);
      SetNumeric(IDC_FILTER_EDT_TP_QF, true, true);
      m_bHP = (m_Filter.dHPFreq != 0.0) ? true : false;
      m_bTP = (m_Filter.dTPFreq != 0.0) ? true : false;
      EnableGroupItems(GetDlgItem(IDC_FILTER_GRP_HP), m_bHP, 0);
      EnableGroupItems(GetDlgItem(IDC_FILTER_GRP_TP), m_bTP, 0);
      if (m_bHP)
      {
         SetDlgItemDouble(IDC_FILTER_EDT_HP_LOW_FRQ, m_Filter.dHPFreq, 1);
         CheckFilterType(IDC_FILTER_SPIN_HP_ORDER, IDC_FILTER_EDT_HP_ORDER, IDC_FILTER_EDT_HP_QF, m_Filter.nHPType, m_Filter.dHPQFaktor, m_Filter.nHPOrder, m_nHPMinOrder, m_nHPMaxOrder);
         SetDlgItemInt(   IDC_FILTER_EDT_HP_ORDER  , m_Filter.nHPOrder, false);
         SendDlgItemMessage(IDC_FILTER_COMBO_HP_CHARACTERISTIC, CB_SETCURSEL, m_Filter.nHPType, 0);
         EnableWindow(GetDlgItem(IDC_FILTER_EDT_HP_LOW_FRQ), (g_nUserMode & USERMODE_FILTER_FRQ) ? true : false);
      }
      if (m_bTP)
      {
         SetDlgItemDouble(IDC_FILTER_EDT_TP_UPP_FRQ, m_Filter.dTPFreq, 1);
         CheckFilterType(IDC_FILTER_SPIN_TP_ORDER, IDC_FILTER_EDT_TP_ORDER, IDC_FILTER_EDT_TP_QF, m_Filter.nTPType, m_Filter.dTPQFaktor, m_Filter.nTPOrder, m_nTPMinOrder, m_nTPMaxOrder);
         SetDlgItemInt(   IDC_FILTER_EDT_TP_ORDER  , m_Filter.nTPOrder, false);
         SendDlgItemMessage(IDC_FILTER_COMBO_TP_CHARACTERISTIC, CB_SETCURSEL, m_Filter.nTPType, 0);
         EnableWindow(GetDlgItem(IDC_FILTER_EDT_TP_UPP_FRQ), (g_nUserMode & USERMODE_FILTER_FRQ) ? true : false);
      }
      CBoxPropertySheet::CalcFilterBox(&m_Filter, CBoxPropertySheet::gm_dFreq);
      ::EnumChildWindows(hWnd, SetHelpIDs, (LPARAM)hWnd);
      return true;
   }
   return false;
}

BOOL CALLBACK CFilterDlg::SetHelpIDs(HWND hwnd, LPARAM /*lParam*/)
{
   switch (::GetWindowLong(hwnd, GWL_ID))
   {
      case IDC_FILTER_COMBO_HP_CHARACTERISTIC: 
      case IDC_FILTER_COMBO_TP_CHARACTERISTIC: case IDC_FILTER_TXT_TP_CHARACTERISTIC:
         ::SetWindowContextHelpId(hwnd, IDC_FILTER_TXT_HP_CHARACTERISTIC);
         break;
      case IDC_FILTER_EDT_HP_ORDER: case IDC_FILTER_SPIN_HP_ORDER:
      case IDC_FILTER_EDT_TP_ORDER: case IDC_FILTER_SPIN_TP_ORDER: case IDC_FILTER_TXT_TP_ORDER:
         ::SetWindowContextHelpId(hwnd, IDC_FILTER_TXT_HP_ORDER);
         break;
      case IDC_FILTER_EDT_HP_QF:
      case IDC_FILTER_EDT_TP_QF: case IDC_FILTER_TXT_TP_QF:
         ::SetWindowContextHelpId(hwnd, IDC_FILTER_TXT_HP_QF);
         break;
      case IDC_FILTER_EDT_HP_LOW_FRQ: case IDC_FILTER_TXT_HP_LOW_FRQ_UNIT:
         ::SetWindowContextHelpId(hwnd, IDC_FILTER_TXT_HP_LOW_FRQ);
         break;
      case IDC_FILTER_TXT_TP_UPP_FRQ: case IDC_FILTER_TXT_TP_UPP_FRQ_UNIT:
         ::SetWindowContextHelpId(hwnd, IDC_FILTER_EDT_TP_UPP_FRQ);
         break;
      case IDOK:     ::SetWindowContextHelpId(hwnd, IDS_BTN_OK); break;
      case IDCANCEL: ::SetWindowContextHelpId(hwnd, IDS_BTN_CANCEL); break;
   }
   return 1;
}

void CFilterDlg::OnEndDialog(int nResult)
{
   CEtsDialog::OnEndDialog(nResult);                              // Funktion der Basisklasse aufrufen
}

int CFilterDlg::OnOk(WORD)
{
   int nID = IDCANCEL;
   if      ((m_Filter.nHPType == 1) && (m_Filter.dHPQFaktor == 0.0)) CreateErrorWnd(IDC_FILTER_EDT_HP_QF, IDS_ERROR_WRONGINPUT, true);
   else if ((m_Filter.nTPType == 1) && (m_Filter.dTPQFaktor == 0.0)) CreateErrorWnd(IDC_FILTER_EDT_TP_QF, IDS_ERROR_WRONGINPUT, true);
   else if (m_bHP && (m_Filter.dHPFreq == 0.0)) CreateErrorWnd(IDC_FILTER_EDT_HP_LOW_FRQ, IDS_ERROR_WRONGINPUT, true);
   else if (m_bTP && (m_Filter.dTPFreq == 0.0)) CreateErrorWnd(IDC_FILTER_EDT_TP_UPP_FRQ, IDS_ERROR_WRONGINPUT, true);
   else nID = IDOK;
   return nID;
}

int CFilterDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   bool bChanged = false;
   switch(nID)
   {
// Hochpass
      case IDC_FILTER_EDT_HP_LOW_FRQ:
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetDlgItemDouble(nID, m_Filter.dHPFreq, 0, &bChanged) && bChanged)
         {
            CheckMinMaxDouble(nID, m_dMinFrq, m_dMaxFrq, 2, m_Filter.dHPFreq);
            m_bFrqChanged = true;
         }
      } break;
      case IDC_FILTER_EDT_HP_QF:
      if (nNotifyCode == EN_CHANGE)
      {
         if (GetDlgItemDouble(nID, m_Filter.dHPQFaktor, 0, &bChanged) && bChanged)
         {
            if (m_Filter.dHPQFaktor <= 0.001) m_Filter.dHPQFaktor = 0.001;
            CheckMinMaxDouble(nID, 0.001, 100.0, 4, m_Filter.dHPQFaktor);
         }
      } break;
      case IDC_FILTER_EDT_HP_ORDER:
      if (nNotifyCode == EN_CHANGE)
      {
         if (GetDlgItemInt(nID, m_Filter.nHPOrder, false, -1, &bChanged) && bChanged)
            CheckMinMaxInt(nID, m_nHPMinOrder, m_nHPMaxOrder, false, m_Filter.nHPOrder);
      } break;
      case IDC_FILTER_COMBO_HP_CHARACTERISTIC:
      if (nNotifyCode == CBN_SELCHANGE)
      {
         int nType = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
         if (nType != m_Filter.nHPType)
         {
            m_Filter.nHPType = nType;
            CheckFilterType(IDC_FILTER_SPIN_HP_ORDER, IDC_FILTER_EDT_HP_ORDER, IDC_FILTER_EDT_HP_QF, m_Filter.nHPType, m_Filter.dHPQFaktor, m_Filter.nHPOrder, m_nHPMinOrder, m_nHPMaxOrder);
            bChanged = true;
         }
      } break;

// Tiefpass
      case IDC_FILTER_EDT_TP_UPP_FRQ:
      if (nNotifyCode == EN_KILLFOCUS)
      {
         if (GetDlgItemDouble(nID, m_Filter.dTPFreq, 0, &bChanged) && bChanged)
         {
            CheckMinMaxDouble(nID, m_dMinFrq, m_dMaxFrq, 2, m_Filter.dTPFreq);
            m_bFrqChanged = true;
         }
      } break;
      case IDC_FILTER_EDT_TP_QF:
      if (nNotifyCode == EN_CHANGE)
      {
         if (GetDlgItemDouble(nID, m_Filter.dTPQFaktor, 0, &bChanged) && bChanged)
         {
            if (m_Filter.dTPQFaktor <= 0.001) m_Filter.dTPQFaktor = 0.001;
            CheckMinMaxDouble(nID, 0.001, 100.0, 4, m_Filter.dTPQFaktor);
         }
      } break;
      case IDC_FILTER_EDT_TP_ORDER:
      if (nNotifyCode == EN_CHANGE)
      {
         if (GetDlgItemInt(nID, m_Filter.nTPOrder, false, -1, &bChanged) && bChanged)
            CheckMinMaxInt(nID, m_nTPMinOrder, m_nTPMaxOrder, false, m_Filter.nTPOrder);
      } break;
      case IDC_FILTER_COMBO_TP_CHARACTERISTIC:
      if (nNotifyCode == CBN_SELCHANGE)
      {
         int nType = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
         if (nType != m_Filter.nTPType)
         {
            m_Filter.nTPType = SendDlgItemMessage(nID, CB_GETCURSEL, 0, 0);
            CheckFilterType(IDC_FILTER_SPIN_TP_ORDER, IDC_FILTER_EDT_TP_ORDER, IDC_FILTER_EDT_TP_QF, m_Filter.nTPType, m_Filter.dTPQFaktor, m_Filter.nTPOrder, m_nTPMinOrder, m_nTPMaxOrder);
            bChanged = true;
         }
      } break;
      default:
      return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
   }
   if (bChanged)
   {
      CBoxPropertySheet::CalcFilterBox(&m_Filter, CBoxPropertySheet::gm_dFreq);
      ::InvalidateRect(GetDlgItem(IDC_FILTER_WND_FREQUENCY_RESP), NULL, true);
   }
   return 0;
}

int CFilterDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch (nMsg)
   {
      case WM_DRAWITEM   : return OnDrawItem((DRAWITEMSTRUCT *)lParam);
   }
   return CEtsDialog::OnMessage(nMsg, wParam, lParam);
}

/******************************************************************************
* Definition :
* Zweck      :
* Aufruf     :
* Parameter (EA):
* Funktionswert :
******************************************************************************/
void CFilterDlg::CheckFilterType(int nIDSpin, int nIDOrder, int nIDQF, int nType, double& dQF, int& nOrder, int& nMin, int& nMax)
{
   switch (nType)
   {
      case 0: nMin = 1; nMax = 4; break;  // 0->Butterworth,
      case 1: nMin = 2; nMax = 3; break;  // 1->'entdämpft' (d.h. Q variabel, nur für 2./3. Ordnung)
      case 2: nMin = 1; nMax = 4; break;  // 2->Bessel
      case 3: nMin = 1; nMax = 4; break;  // 3->Tschebyscheff mit 0.5 dB Welligkeit
      case 4: nMin = 4; nMax = 4; break;  // 4->Linkwitz-Riley (nur f. 4. Ordnung)
   }
   SendDlgItemMessage(nIDSpin, UDM_SETRANGE, 0, MAKELPARAM(nMax, nMin));
   CheckMinMaxInt(nIDOrder, nMin, nMax, false, nOrder, false);
   if (nType == 1)
   {
      EnableWindow(GetDlgItem(nIDQF), true);
      if (dQF < 1e-13) dQF = 0.707;
      SetDlgItemDouble(nIDQF, dQF, 4);
   }
   else
   {
      EnableWindow(GetDlgItem(nIDQF), false);
      SetDlgItemText(nIDQF, "");
   }
   
   EnableWindow(GetDlgItem(nIDOrder), ((nMax-nMin)==0) ? false: true);
}

/******************************************************************************
* Definition :
* Zweck      :
* Aufruf     :
* Parameter (EA):
* Funktionswert :
******************************************************************************/
LRESULT CFilterDlg::OnDrawItem(DRAWITEMSTRUCT *pDI)
{
   if ((pDI->CtlType == ODT_STATIC) && (pDI->CtlID == IDC_FILTER_WND_FREQUENCY_RESP))
   {
      char szText[64] = "88888";
      double dmY, dbY, dmX, dbX, dVal;
      double dMinVal   = -80.0,
             dMaxVal   =  20.0,
             dMinPhase = -185.0,
             dMaxPhase =  185.0;
      double *pdX = CBoxPropertySheet::gm_dFreq;
      int    nWidth  = pDI->rcItem.right  - pDI->rcItem.left;
      int    nHeight = pDI->rcItem.top - pDI->rcItem.bottom;
      int    i, nTextW, nTextH;
      POINT  ptLine;
      RECT   rcText = {0,0,0,0};
      COLORREF ColMagn  = RGB(255,0,0);
      COLORREF ColPhase = RGB(0,0,255);
      HPEN hMagnPen  = ::CreatePen(PS_SOLID, 1, ColMagn);
      HPEN hPhasePen = ::CreatePen(PS_SOLID, 1, ColPhase);
      HPEN hGridPen  = ::CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_GRAYTEXT));
                                                               
      dmX = (double)nWidth / (log10(pdX[CALCNOOFFREQ-1])-log10(pdX[0]));// Transformation X-Achse
      dbX = (double)pDI->rcItem.left - dmX * log10(pdX[0]);
      dmY = (double)nHeight / (dMaxVal - dMinVal);             // Transformation Amplituden-Achse
      dbY = (double)pDI->rcItem.bottom - dmY * dMinVal;

      ::SaveDC(pDI->hDC);
      ::SetBkColor(pDI->hDC, ::GetSysColor(COLOR_WINDOW));
      ::SelectObject(pDI->hDC, hGridPen);                      // für die Gitterlinien
      ::SetTextColor(pDI->hDC, ::GetSysColor(COLOR_GRAYTEXT)); // für den Gittertext

      ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_LEFT|DT_TOP|DT_CALCRECT);
      nTextW = rcText.right  - rcText.left;                    // Textgröße ermitteln
      nTextH = rcText.bottom - rcText.top;                     // Hintergrund löschen
      ::FillRect(pDI->hDC, &pDI->rcItem, ::GetSysColorBrush(COLOR_WINDOW));

      rcText.bottom = pDI->rcItem.bottom;                      // Frequenzachsenbeschriftung
      rcText.top    = rcText.bottom - nTextH;
      rcText.left   = pDI->rcItem.right - nTextW;
      rcText.right  = pDI->rcItem.right;
      strcpy(szText, "[Hz]");
      ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_RIGHT|DT_BOTTOM|DT_SINGLELINE);
      for (i=9; i<CALCNOOFFREQ-1; i+=18)                       // Oktaven bei CALE-Frequenzen
      {
         ptLine.x = (int)(dmX * log10(pdX[i]) + dbX);
         ptLine.y = pDI->rcItem.top;
         ::MoveToEx(pDI->hDC, ptLine.x, ptLine.y, NULL);
         ptLine.y = rcText.top;
         ::LineTo(pDI->hDC, ptLine.x, ptLine.y);
         rcText.left   = ptLine.x - (nTextW>>1);
         rcText.right  = ptLine.x + (nTextW>>1);
         wsprintf(szText, "%d", (int)pdX[i]);
         ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_CENTER|DT_BOTTOM|DT_SINGLELINE);
      }

      rcText.left  = pDI->rcItem.left;                      // Amplitudenachsenbeschriftung
      rcText.right = rcText.left + nTextW;
      rcText.top    = pDI->rcItem.top;
      rcText.bottom = rcText.top + nTextH;
      strcpy(szText, "[dB]");
      ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_LEFT|DT_BOTTOM|DT_SINGLELINE);
      rcText.right = rcText.left + (nTextW>>1);
      for (i=(int)dMinVal+20; i<(int)dMaxVal; i+=20)        // 20 dB Schritte
      {
         ptLine.x = rcText.right;
         ptLine.y = (int)(dmY * i + dbY);
         ::MoveToEx(pDI->hDC, ptLine.x, ptLine.y, NULL);
         ptLine.x = pDI->rcItem.right;
         ::LineTo(pDI->hDC, ptLine.x, ptLine.y);
         rcText.bottom = ptLine.y + (nTextH>>1);
         rcText.top    = rcText.bottom - nTextH;
         wsprintf(szText, "%d", i);
         ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_LEFT|DT_BOTTOM|DT_SINGLELINE);
      }

      ::SelectObject(pDI->hDC, hMagnPen);                   // Ausgabe der Amplitude
      for (i=0; i<CALCNOOFFREQ; i++)
      {
         dVal = 20.0*log10(Betrag(m_Filter.cFilterBox[i]));
         ptLine.x = (int)(dmX * log10(pdX[i]) + dbX);
         ptLine.y = (int)(dmY * dVal + dbY);
         if (i==0) ::MoveToEx(pDI->hDC, ptLine.x, ptLine.y, NULL);
         else      ::LineTo(pDI->hDC, ptLine.x, ptLine.y);
      }

      dmY = (double)nHeight / (dMaxPhase - dMinPhase);      // Transformation für die Phase
      dbY = (double)pDI->rcItem.bottom - dmY * dMinPhase;

      rcText.right = pDI->rcItem.right;                     // Phasenachsenbeschriftung
      rcText.left  = pDI->rcItem.right-nTextW;
      ::SetTextColor(pDI->hDC, ColPhase);                   // für den Gittertext
      for (i=-90; i<=90; i+=90)                             // 90° Schritte
      {
         ptLine.x = rcText.right;
         ptLine.y = (int)(dmY * i + dbY);
         rcText.bottom = ptLine.y + (nTextH>>1);
         rcText.top    = rcText.bottom - nTextH;
         wsprintf(szText, "%d", i);
         ::DrawText(pDI->hDC, szText, strlen(szText), &rcText, DT_RIGHT|DT_BOTTOM|DT_SINGLELINE);
      }

      ::SelectObject(pDI->hDC, hPhasePen);                  // Ausgabe der Phase
      for (i=0; i<CALCNOOFFREQ; i++)
      {
         dVal =     Phase(m_Filter.cFilterBox[i]);
         ptLine.x = (int)(dmX * log10(pdX[i]) + dbX);
         ptLine.y = (int)(dmY * dVal + dbY);
         if (i==0) ::MoveToEx(pDI->hDC, ptLine.x, ptLine.y, NULL);
         else      ::LineTo(pDI->hDC, ptLine.x, ptLine.y);
      }

      ::RestoreDC(pDI->hDC, -1);
      ::DeleteObject(hMagnPen);
      ::DeleteObject(hPhasePen);
      ::DeleteObject(hGridPen);
      return 1;
   }
   return 0;
}
