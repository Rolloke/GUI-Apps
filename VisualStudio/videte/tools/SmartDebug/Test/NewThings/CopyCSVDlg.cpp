#include "stdafx.h"
#include "AddIn.h"
#include "Connect.h"

#include "copycsvdlg.h"
#include "resource.h"
#include <math.h>
#include <stdio.h>
#include <algorithm>

#define lengthof(X) (sizeof(X)/sizeof(X[0]))

#define SAFE_DELETE(X) if (X) { delete X; X = NULL;}
extern CAddInModule _AtlModule;
extern CComPtr<EnvDTE::_DTE> g_pDTE;

#define THIS_POINTER_CDIALOG     L"CCopyCSVDlg"

#define NOKILLFOCUSNOTIFICATION 0x00000001
#define NOERRORWINDOW           0x00000002
#define REPORT_ID               0x00000004
#define IS_IN_MENU_LOOP         0x00000008
#define DLG_DOCK_ABLE           0x00000010
#define MODAL_W_CONTEXT_HELP    0x00000020
#define MODAL_W_MIN_BTN         0x00000040
#define MODAL_W_MAX_BTN         0x00000080

struct MessageFilter
{
   int    code;
   WPARAM wParam;
   LPARAM lParam;
   BOOL   bResult;
};

#define ETSDIV_POLAR       0
#define ETSDIV_LINEAR      1
#define ETSDIV_LOGARITHMIC 2
#define ETSDIV_FREQUENCY   3

struct ETSDIV_DIV                   // Struktur für die Aufteilung von Strecken
{
   ETSDIV_DIV(long L1, long L2, long sw, double F1, double F2)
   {
      l1 = L1;
      l2 = L2;
      stepwidth = sw;
      f1        = F1;
      f2        = F2;
      divfnc = 0;
      division = 0;
      dZero = 0;
   }
   long        l1, l2,              // Darstellungsbereich in logischen Koordinaten
               width,               // ermittelte Schrittweite in logischen Koordinaten (Platz für Linienbeschriftung)
               stepwidth,           // eingestellte Schrittweite in logischen Koordinaten
               division,            // Teilung der Frequenzschritte f(div) = f0 * pow(2, 1/div);
               divfnc;              // Funktion die zur Ermittlung der Teilung verwendet wurde
   double      f1, f2;              // Darzustellender Bereich in Weltkoordinaten
   double      dZero;               // Ausgabe von 0, wenn die Werte kleiner sind als der Wert
};


WNDPROC CCopyCSVDlg::gm_oldDrawGraphProc        = NULL;
HHOOK   CCopyCSVDlg::gm_hDll_Hook               = NULL;
int     CCopyCSVDlg::gm_nWindowThreadProcessID  = 0;
HWND    CCopyCSVDlg::gm_hWndTopLevelParent      = NULL;
int     CCopyCSVDlg::gm_nCallCount              = 0;
int     CCopyCSVDlg::gm_nModal                  = 1;

CCopyCSVDlg::CCopyCSVDlg(void)
{
   m_pSelected = NULL;
   m_ptCurve   = NULL;
   m_nPoints   = 0;
   m_nZoom     = 0;
   m_nZooming  = 0;
   m_hWnd      = NULL;
   m_hWndParent= NULL;
   m_nFlags    = 0;
   ZeroMemory(&m_rcZoom, sizeof(RECT));
   ZeroMemory(&m_rcZooming, sizeof(RECT));
   ZeroMemory(&m_szRB_Border, sizeof(SIZE));
   m_dFaktor = 1.0;
   m_dOffset = 0.0;
   m_dStart  = 0.0;
   m_dStep   = 1.0;
   m_nSteps  = 1;
   m_nStartX = 0;
   m_nStepX  = 1;
   m_nStepsX = 1;
   m_bModal  = true; 
   gm_nCallCount = 0;
}

CCopyCSVDlg::~CCopyCSVDlg(void)
{
   if (m_pSelected)
   {
      SysFreeString(m_pSelected);
   }
   delete m_ptCurve;
}

int CCopyCSVDlg::DoModal(HWND hWndParent)
{
   if (_AtlModule.GetResourceInstance() != NULL)
   {
      m_bModal = true;
      m_hWndParent = hWndParent;
      return (int)DialogBoxParam(_AtlModule.GetResourceInstance(),
         //MAKEINTRESOURCE(IDD_WATCH_ARRAY),
         (LPCTSTR)(WORD)(IDD_WATCH_ARRAY),
         hWndParent,
         (DLGPROC) DialogProc, (LPARAM) this);
   }
   return 0;
}

BOOL CALLBACK CCopyCSVDlg::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)
   {
      case WM_INITDIALOG:
      if (lParam)
      {
         CCopyCSVDlg * pDlg = NULL;
         ::SetWindowLong(hwndDlg, DWL_USER, (LONG)lParam);
         pDlg = (CCopyCSVDlg*)lParam;
         if (pDlg && /*pDlg->m_bModal &&*/ !IsWindowVisible(hwndDlg))
         {
            ShowWindow(hwndDlg, SW_SHOW);
         }
         if (pDlg!=NULL)
         {
            if (pDlg->OnInitDialog(hwndDlg)) return 1;
            else
            {
               pDlg->OnEndDialog(-1);
            }
         }
         return 0;
      }
      break;
      case WM_NCDESTROY:
      {
         CCopyCSVDlg* pDlg = (CCopyCSVDlg*)(INT_PTR)::GetWindowLong(hwndDlg, DWL_USER);
         delete pDlg;
      } break;

      case WM_COMMAND:
      {
         CCopyCSVDlg* pDlg = (CCopyCSVDlg*)(INT_PTR)::GetWindowLong(hwndDlg, DWL_USER);
         if (pDlg)
         {
            //if (CCopyCSVDlg::gm_nIDAPPLY == LOWORD(wParam))
            //{
            //   pDlg->OnApply();
            //   return 1;
            //}
            switch(LOWORD(wParam))
            {
               case IDCANCEL:
               {
                  pDlg->OnEndDialog(IDCANCEL);
                  return 1;
               }
               case IDOK:
               {
                  if (pDlg->OnOk(HIWORD(wParam)) == IDOK)
                  {
                     pDlg->OnEndDialog(IDOK);
                  }
                  return 1;
               }
               default:
               //if ((pDlg->m_hWndForeground!=NULL) && 
               //    !::IsWindow(pDlg->m_hWndForeground))
               //{
               //   pDlg->m_nFlags &= ~NOKILLFOCUSNOTIFICATION;
               //   pDlg->m_hWndForeground           = NULL;
               //}
               //if (pDlg->m_nFlags & NOKILLFOCUSNOTIFICATION)
               //{
               //   switch (HIWORD(wParam))
               //   {
               //      case EN_KILLFOCUS:
               //      case BN_KILLFOCUS:
               //      case LBN_KILLFOCUS:
               //      case CBN_KILLFOCUS:
               //         TRACE("Invalid Killfocus\n");
               //         return 1;
               //      default: break;
               //   }
               //}
               return pDlg->OnCommand(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
            }
         }
      }
      default:
      {
         CCopyCSVDlg* pDlg = (CCopyCSVDlg*)(INT_PTR)::GetWindowLong(hwndDlg, DWL_USER);
         if (pDlg) return pDlg->OnMessage(uMsg, wParam, lParam);
      } break;
   }
   return 0;
}

bool CCopyCSVDlg::OnInitDialog(HWND hWnd)
{
   m_hWnd = hWnd;

   SetDlgItemInt(m_hWnd, IDC_EDT_VAL_FROM, 0, FALSE);
   SetDlgItemInt(m_hWnd, IDC_EDT_VAL_TO, 1, FALSE);
   EnableWindow(GetDlgItem(m_hWnd, IDC_BTN_SET_VALUE), FALSE);
   EnableWindow(GetDlgItem(m_hWnd, IDC_BTN_GRAPH), FALSE);

   if (m_pSelected)
   {
      SetDlgItemText(m_hWnd, IDC_EDT_EXPRESSION, m_pSelected);
   }
   gm_oldDrawGraphProc =(WNDPROC)SetWindowLongPtr(GetDlgItem(m_hWnd, IDC_WND_GRAPH), GWL_WNDPROC, (LONG)DrawGraphProc);
   ::SetWindowLongPtr(GetDlgItem(m_hWnd, IDC_WND_GRAPH), GWL_USERDATA, (LONG)this);

   RECT rcDlg, rcList;
   GetWindowRect(GetDlgItem(m_hWnd, IDC_LIST_VALUES), &rcList);
   GetWindowRect(m_hWnd, &rcDlg);
   ScreenToClient((LPPOINT)&rcDlg, 2);
   ScreenToClient((LPPOINT)&rcList, 2);
   m_szRB_Border.cx = rcDlg.right - rcList.right;
   m_szRB_Border.cy = rcDlg.bottom - rcList.bottom;
   if (!m_bModal)                                              // ein nichtmodaler Dialog
   {
      //WCHAR  sExpr[256];

      HWND hP = ::GetParent(m_hWnd);
      if (hP == NULL)
      {
         ::SetParent(m_hWnd, m_hWndParent);
         //swprintf(sExpr, L"SetParent(%x, %x)", m_hWnd, m_hWndParent);
         //SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_ADDSTRING, 0, (LPARAM)sExpr);
      }
      if (gm_hWndTopLevelParent == NULL)
      {
         gm_hWndTopLevelParent = GetTopLevelParent(m_hWnd); // Top Level Parent !!
         //swprintf(sExpr, L"%x = GetTopLevelParent()", gm_hWndTopLevelParent);
         //SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_ADDSTRING, 0, (LPARAM)sExpr);
      }
      if (gm_nWindowThreadProcessID == 0)
      {
         GetWindowThreadID(m_hWnd);
         //swprintf(sExpr, L"%x = GetWindowThreadID()", gm_nWindowThreadProcessID);
         //SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_ADDSTRING, 0, (LPARAM)sExpr);
      }
      if (gm_nWindowThreadProcessID != 0 && gm_hDll_Hook == NULL)
      {
         DWORD dwID = ::GetCurrentThreadId();
         gm_hDll_Hook = SetWindowsHookEx(WH_MSGFILTER, (HOOKPROC)MsgFilterProc, NULL, gm_nWindowThreadProcessID);
         //swprintf(sExpr, L"%x = SetWindowsHookEx(..), %x", gm_hDll_Hook, dwID);
         //SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_ADDSTRING, 0, (LPARAM)sExpr);
      }
   }

   if (gm_hDll_Hook) SetProp(hWnd, THIS_POINTER_CDIALOG, gm_hDll_Hook);

   return true;
}

HWND CCopyCSVDlg::GetTopLevelParent(HWND hwndChild)
{
   HWND hwndParent;
   while(hwndChild)
   {
      hwndParent = ::GetParent(hwndChild);
      if (hwndParent == NULL) return hwndChild;
      hwndChild = hwndParent;
   };
   return NULL;
}

void CCopyCSVDlg::OnEndDialog(int nResult)
{
   if (m_hWnd)
   {
      if (::GetProp(m_hWnd, THIS_POINTER_CDIALOG))
      {
         ::RemoveProp(m_hWnd, THIS_POINTER_CDIALOG);
      }
      if (m_bModal)
      {
         ::SetWindowLong(m_hWnd, DWL_USER, NULL);
         EndDialog(m_hWnd, nResult);
      }
      else
      {
         DestroyWindow(m_hWnd);
      }
      m_hWnd = NULL;
   }
}

struct sx
{
   double d1, d2, d3;
};

int CCopyCSVDlg::OnOk(WORD nNotify)
{
#ifdef _TEST_APP
   int i, n;
   WCHAR  sExpr[256];
   WCHAR*pstr;
   double dValue;
   n = rand();
   i=0;
   SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_RESETCONTENT, 0, 0);
   m_nPoints = 1000;
   for (i=0; i<m_nPoints; i++)
   {
      dValue = sin((i+n)*0.01);
      swprintf(sExpr, L"%.15f", dValue);
      SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_ADDSTRING, 0, (LPARAM)sExpr);
   }
   EnableWindow(GetDlgItem(m_hWnd, IDC_BTN_GRAPH), TRUE);
   OnCommand(IDC_BTN_GRAPH, BN_CLICKED, m_hWnd);
#else
   CComPtr<EnvDTE::Debugger> pDebugger;
   CComPtr<EnvDTE::Expression> pExpression, pEx;
   CComPtr<EnvDTE::Expressions> pExpressions;
   BSTR bsValue, bsType;
   WCHAR  sExpr[256];
   HRESULT hr = g_pDTE->get_Debugger(&pDebugger);
   if (SUCCEEDED(hr))
   {
      WCHAR  sExprEx[256];
      VARIANT_BOOL bHex;
      pDebugger->get_HexDisplayMode(&bHex);
      GetDlgItemText(m_hWnd, IDC_EDT_EXPRESSION, sExpr, 255);
      WCHAR *pExp = wcsstr(sExpr, L"%d");
      if (pExp)
      {
         wsprintf(sExprEx, sExpr, 0);
      }
      else
      {
         wcscpy_s(sExprEx, lengthof(sExprEx), sExpr);
      }
      m_Selected.vt = VT_EMPTY;
      hr = pDebugger->GetExpression(sExprEx, VARIANT_TRUE, 0, &pExpression);
      if (SUCCEEDED(hr))
      {
         BOOL bTrans;
         int i, n, nFrom, nTo, nSucceeded=0;
         pExpression->get_Value(&bsValue);
         SetDlgItemText(m_hWnd, IDC_EDT_VALUE, bsValue);
         pExpression->get_Type(&bsType);
         SetDlgItemText(m_hWnd, IDC_EDT_TYPE, bsType);
         SysFreeString(bsValue);
         SysFreeString(bsType);
         hr = pExpression->get_DataMembers(&pExpressions);
         if (SUCCEEDED(hr))
         {
             long fCount =0;
             hr = pExpressions->get_Count(&fCount);
             if (SUCCEEDED(hr) && fCount > 1)
             {
                m_Selected.vt = VT_I4;
                pEx = 0;
                SetDlgItemInt(m_hWnd, IDC_EDT_VAL_TO, fCount, FALSE);
                m_nPoints = fCount;
                SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_RESETCONTENT, 0, (LPARAM)bsValue);
                for (m_Selected.lVal=1; m_Selected.lVal<=fCount; m_Selected.lVal++)
                {
                   hr = pExpressions->Item(m_Selected, &pEx);
                   if (SUCCEEDED(hr))
                   {
                       pEx->get_Value(&bsValue);
                       SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_ADDSTRING, 0, (LPARAM)bsValue);
                       SysFreeString(bsValue);
                   }
                   else
                   { 
                       FormatMessage("Error pExpressions->Item: %x\r\n", hr);
                       break;
                   }
                }
                m_Selected.llVal = 1;
                ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BTN_GRAPH),SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_GETCOUNT, 0, 0) > 0? TRUE:FALSE);
                return 0;
             }
             else
             { 
                FormatMessage("Error pExpressions->get_Count: %x\r\n", hr);
             }
         }
         else
         {
            FormatMessage("Error pExpression->get_DataMembers: %x\r\n", hr);
         }

         nFrom = GetDlgItemInt(m_hWnd, IDC_EDT_VAL_FROM, &bTrans, FALSE);
         nTo   = GetDlgItemInt(m_hWnd, IDC_EDT_VAL_TO, &bTrans, FALSE);
         if (nFrom > nTo)
         {
            std::swap(nFrom, nTo);
            SetDlgItemInt(m_hWnd, IDC_EDT_VAL_FROM, nFrom, FALSE);
            SetDlgItemInt(m_hWnd, IDC_EDT_VAL_TO, nTo, FALSE);
         }
         n = nTo - nFrom;
         m_nPoints = n;
         if (n > 1)
         {
            SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_RESETCONTENT, 0, (LPARAM)bsValue);
            BSTR *bsValues = (BSTR*) new BYTE[sizeof(BSTR)*m_nPoints];
            n = 0;
            for (i=nFrom; i<nTo; i++)
            {
               if (pExp)
               {
                  wsprintf(sExprEx, sExpr, i);
               }
               else
               {
                  wsprintf(sExprEx, L"(%s)[%d]", sExpr, i);
               }
               pExpression = NULL;
               hr = pDebugger->GetExpression(sExprEx, VARIANT_TRUE, 0, &pExpression);
               if (SUCCEEDED(hr))
               {
                  hr = pExpression->get_Value(&bsValues[n++]);
                  if (SUCCEEDED(hr)) nSucceeded++;
               }
               if ((i&0x0000000f) == 0)
               {
                  SetDlgItemInt(m_hWnd, IDC_EDT_VALUE, n, FALSE);
               }
            }
            for (i=0; i<m_nPoints; i++)
            {
               SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_ADDSTRING, 0, (LPARAM)bsValues[i]);
               SysFreeString(bsValues[i]);
            }
            delete bsValues;
            if (nSucceeded)
            {
               ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BTN_GRAPH), TRUE);
               OnCommand(IDC_BTN_GRAPH, BN_CLICKED, m_hWnd);
            }
            else
            {
               ::CheckDlgButton(m_hWnd, IDC_BTN_GRAPH, BST_UNCHECKED);
               ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BTN_GRAPH), FALSE);
            }
         }
      }
   }
   if (FAILED(hr))
   {
      wsprintf(sExpr, L"Error code: %d, %x", hr, hr);
      SetDlgItemText(m_hWnd, IDC_EDT_VALUE, sExpr);
   }
   //bool bChanged  = true;
   //HWND hwndApply = GetDlgItem(CCopyCSVDlg::gm_nIDAPPLY);
   //if (hwndApply)                                           // ist ein Apply-Button vorhanden
   //{
   //   bChanged = (IsWindowEnabled(hwndApply)!=0) ? true : false;
   //   EnableWindow(hwndApply, false);                       // Apply-Button Disablen
   //}
   //if (bChanged)                                            // wenn sich etwas geändert hat und
   //{
   //   HWND hwndParent = m_hWndParent;
   //   if (!IsWindow(hwndParent)) hwndParent = GetParent(m_hWnd);
   //   if (IsWindow(hwndParent))                             // ein Elternfenster vorhanden ist :
   //   {                                                     // Posten der Nachricht WM_COMMAND, mit der ID des Dialogfeldes, nNotify, Fensterhandle des Dialogfeldes
   //      WPARAM wParam = MAKELONG(m_nID, nNotify);
   //      LPARAM lParam = (LPARAM)m_hWnd;
   //      PostMessage(hwndParent, WM_COMMAND, wParam, lParam);
   //   }
   //}
#endif
   return 0;
}

void CCopyCSVDlg::FormatMessage(const char *szFmt, ...)
{
    char sOut[1024];
    wchar_t sOutW[1024];
    va_list args;
    va_start(args, szFmt);
    int i, nBuf;
    nBuf = vsprintf_s(sOut, lengthof(sOut), szFmt, args);
    va_end(args);
    for (i=0; i<=nBuf; i++)
    {
        sOutW[i] = sOut[i];
    }
    SendDlgItemMessage(m_hWnd, IDC_EDT_DEBUG_OUT, EM_SETSEL, -1, -1);
    SendDlgItemMessage(m_hWnd, IDC_EDT_DEBUG_OUT, EM_REPLACESEL, FALSE, (LPARAM)sOutW);
}

int CCopyCSVDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   if (nNotifyCode == BN_CLICKED)
   {
      switch(nID)
      {      
      case IDC_CK_DBUG_OUT:
      {
          int nThis  = SW_HIDE;
          int nOther = SW_SHOW;
          int nIdOther = IDC_LIST_VALUES;
          if (IsDlgButtonChecked(m_hWnd, nID))
          {
              nThis  = SW_SHOW;
              nOther = SW_HIDE;
          }
          if (IsDlgButtonChecked(m_hWnd, IDC_BTN_GRAPH)) nIdOther = IDC_WND_GRAPH;
          ShowWindow(GetDlgItem(m_hWnd, IDC_EDT_DEBUG_OUT), nThis);
          ShowWindow(GetDlgItem(m_hWnd, nIdOther), nOther);
      }break;
      case IDC_BTN_GRAPH:
      {
         if (IsDlgButtonChecked(m_hWnd, nID))
         {
            int i, iMax=-1, iMin=-1;
            double *pdValues = new double[m_nPoints];
            double dMax = -1.0e100, dMin = 1.0e100, dTemp;
            wchar_t szValue[256], *pend=0;
#ifndef _DEBUG
            int j, n;
            char    szValueA[256];
#endif
            SAFE_DELETE(m_ptCurve);
            m_ptCurve = new POINT[m_nPoints];
            m_nZoom     = 0;
            m_nZooming  = 0;
            ZeroMemory(&m_rcZoom, sizeof(RECT));
            ZeroMemory(&m_rcZooming, sizeof(RECT));
            for (i=0; i<m_nPoints; i++)
            {
#ifndef _DEBUG
               n = (int)SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_GETTEXT, i, (LPARAM)szValue);
               for (j=0; j<=n; j++)
               {
                  szValueA[j] = (char)szValue[j];
               }
               dTemp = atof(szValueA);
               //FormatMessage("Value: %s, %f\r\n", szValueA, dTemp);
#else
               SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_GETTEXT, i, (LPARAM)szValue);
               dTemp = wcstod(szValue, &pend);
#endif
               pdValues[i] = dTemp;
               if (dTemp > dMax)
               {
                  dMax = dTemp;
                  iMax = i;
               }
               if (dTemp < dMin)
               {
                  dMin = dTemp;
                  iMin = i;
               }
            }

            m_dFaktor = 10000.0 / (dMax - dMin);
            dTemp  = (dMin + dMax) * 0.5;
            dTemp *= m_dFaktor;
            for (i=0; i<m_nPoints; i++)
            {
               m_ptCurve[i].x = i;
               m_ptCurve[i].y = (int)(pdValues[i]*m_dFaktor - dTemp);
            }
            m_dOffset = dTemp;
            delete pdValues;

            RECT rc;
            GetClientRect(m_hWnd, &rc);
            ETSDIV_DIV div(rc.top, rc.bottom, 50, dMin, dMax);
            CalculateLinDivision(&div, m_dStep, m_dStart);
            m_nSteps = Round((div.f2 - div.f1) / m_dStep);
            m_nSteps++;

            ETSDIV_DIV div1(rc.left, rc.right, 100, 0.0, (double)m_nPoints);
            CalculateLinDivision(&div1, dMin, dMax);
            m_nStepX  = Round(dMin);
            m_nStartX = Round(dMax);
            m_nStepsX = m_nPoints / m_nStepX;

            ShowWindow(GetDlgItem(m_hWnd, IDC_LIST_VALUES), SW_HIDE);
            ShowWindow(GetDlgItem(m_hWnd, IDC_WND_GRAPH  ), SW_SHOW);
            InvalidateRect(GetDlgItem(m_hWnd, IDC_WND_GRAPH  ), NULL, 1);
            EnableWindow(GetDlgItem(m_hWnd, IDC_BTN_SET_VALUE), FALSE);
         }
         else
         {
            ShowWindow(GetDlgItem(m_hWnd, IDC_WND_GRAPH  ), SW_HIDE);
            ShowWindow(GetDlgItem(m_hWnd, IDC_LIST_VALUES), SW_SHOW);
         }
      } break;
      case IDC_BTN_COPY_VALUES:
      {
            int i, nSize = 0;
            for (i=0; i<m_nPoints; i++)
            {
                nSize += (int)SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_GETTEXTLEN, i, 0);
                nSize += 3;
            }
            if (nSize > 0 && OpenClipboard(m_hWnd))
            {
                EmptyClipboard();
                nSize++;
                HLOCAL hLMem = GlobalAlloc(GHND, nSize*sizeof(wchar_t));
                if (hLMem)
                {
                    wchar_t szValue[128];
                    wchar_t *psz, *pszValues = (wchar_t*)GlobalLock(hLMem);
                    if (pszValues)
                    {
                        pszValues[0] = 0;
                        for (i=0; i<m_nPoints; i++)
                        {
                            SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_GETTEXT, i, (LPARAM)szValue);
                            psz = wcschr(szValue, L'.');
                            if (psz) psz[0] = L',';
                            wcscat_s(pszValues, nSize, szValue);
                            wcscat_s(pszValues, nSize, L"\r\n");
                        }
                        GlobalUnlock(hLMem);
                    }
                    if (::SetClipboardData(CF_UNICODETEXT, hLMem) == NULL)
                    {
                        LocalFree(hLMem);
                        hLMem = NULL;
                    }
                }
                CloseClipboard();
            }
      }break;
      case IDC_BTN_SET_VALUE:
      {
         int nSel = (int)SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_GETCURSEL, 0, 0);
         if (nSel != LB_ERR)
         {
            CComPtr<EnvDTE::Debugger> pDebugger;
            CComPtr<EnvDTE::Expression> pExpression;
            CComPtr<EnvDTE::Expressions> pExpressions;
            WCHAR  sExpr[256];
            HRESULT hr = g_pDTE->get_Debugger(&pDebugger);
            if (SUCCEEDED(hr))
            {
               GetDlgItemText(m_hWnd, IDC_EDT_EXPRESSION, sExpr, 255);
               WCHAR  sExprEx[256];
               pExpression = NULL;
               if (m_Selected.vt != VT_EMPTY)
               {
                   hr = pDebugger->GetExpression(sExpr, VARIANT_TRUE, 0, &pExpression);
                   if (SUCCEEDED(hr))
                   {
                       hr = pExpression->get_DataMembers(&pExpressions);
                       if (SUCCEEDED(hr))
                       {
                           hr = pExpressions->Item(m_Selected, &pExpression);
                       }
                   }
               }
               else
               {
                   if (wcsstr(sExpr, L"%d"))
                   {
                      wsprintf(sExprEx, sExpr, nSel);
                   }
                   else
                   {
                      wsprintf(sExprEx, L"(%s)[%d]", sExpr, nSel);
                   }
                   hr = pDebugger->GetExpression(sExprEx, VARIANT_TRUE, 0, &pExpression);
               }
               if (SUCCEEDED(hr) && pExpression)
               {
                  GetDlgItemText(m_hWnd, IDC_EDT_VALUE, sExpr, 255);
                  hr = pExpression->put_Value(sExpr);
                  if (SUCCEEDED(hr))
                  {
                     EnableWindow(GetDlgItem(m_hWnd, IDC_BTN_SET_VALUE), FALSE);
                     SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_INSERTSTRING, nSel, (LPARAM)sExpr);
                     SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_DELETESTRING, nSel+1, (LPARAM)sExpr);
                  }
               }
            }
         }
      }break;
      }
   } 
   else if (nNotifyCode == EN_CHANGE)
   {
      if (nID == IDC_EDT_VALUE)
      {
         EnableWindow(GetDlgItem(m_hWnd, IDC_BTN_SET_VALUE), TRUE);
      }
   }
   else if (nNotifyCode == LBN_SELCHANGE)
   {
      if (nID == IDC_LIST_VALUES)
      {
         int nSel = (int)SendMessage(hwndControl, LB_GETCURSEL, 0, 0);
         if (nSel != LB_ERR)
         {
            wchar_t szValue[256];
            SendMessage(hwndControl, LB_GETTEXT, nSel, (LPARAM)szValue);
            SetDlgItemText(m_hWnd, IDC_EDT_VALUE, szValue);
            EnableWindow(GetDlgItem(m_hWnd, IDC_BTN_SET_VALUE), FALSE);
            if (m_Selected.vt != VT_EMPTY)
            {
                m_Selected.lVal = nSel+1;
            }
         }
      }
   }

   return true;
}

BOOL CCopyCSVDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   switch (nMsg)
   {
      case WM_SIZE: return OnSize((UINT)wParam, LOWORD(lParam), HIWORD(lParam));
   }
   return 0;
}

BOOL  CCopyCSVDlg::OnSize(UINT nType, short cx, short cy)
{
   RECT rcDlg, rcList;
   BOOL bUpdate = TRUE;//!gm_nModal;
   GetWindowRect(GetDlgItem(m_hWnd, IDC_LIST_VALUES), &rcList);
   GetWindowRect(m_hWnd, &rcDlg);
   ScreenToClient((LPPOINT)&rcDlg, 2);
   ScreenToClient((LPPOINT)&rcList, 2);
   rcList.right  = rcDlg.right  - m_szRB_Border.cx;
   rcList.bottom = rcDlg.bottom - m_szRB_Border.cy;
   MoveWindow(GetDlgItem(m_hWnd, IDC_LIST_VALUES), &rcList, bUpdate);
   MoveWindow(GetDlgItem(m_hWnd, IDC_WND_GRAPH), &rcList, bUpdate);
   MoveWindow(GetDlgItem(m_hWnd, IDC_EDT_DEBUG_OUT), &rcList, bUpdate);

   GetWindowRect(GetDlgItem(m_hWnd, IDC_EDT_EXPRESSION), &rcList);
   ScreenToClient((LPPOINT)&rcList, 2);
   rcList.right  = rcDlg.right  - m_szRB_Border.cx;
   MoveWindow(GetDlgItem(m_hWnd, IDC_EDT_EXPRESSION), &rcList, bUpdate);

   GetWindowRect(GetDlgItem(m_hWnd, IDC_EDT_VALUE), &rcList);
   ScreenToClient((LPPOINT)&rcList, 2);
   rcList.right  = rcDlg.right  - m_szRB_Border.cx;
   MoveWindow(GetDlgItem(m_hWnd, IDC_EDT_VALUE), &rcList, bUpdate);

   GetWindowRect(GetDlgItem(m_hWnd, IDC_EDT_TYPE), &rcList);
   ScreenToClient((LPPOINT)&rcList, 2);
   rcList.right  = rcDlg.right  - m_szRB_Border.cx;
   MoveWindow(GetDlgItem(m_hWnd, IDC_EDT_TYPE), &rcList, bUpdate);

   GetWindowRect(GetDlgItem(m_hWnd, IDC_EDT_VAL_TO), &rcList);
   ScreenToClient((LPPOINT)&rcList, 2);
   rcList.right  = rcDlg.right  - m_szRB_Border.cx;
   MoveWindow(GetDlgItem(m_hWnd, IDC_EDT_VAL_TO), &rcList, bUpdate);

   GetWindowRect(GetDlgItem(m_hWnd, IDCANCEL), &rcList);
   ScreenToClient((LPPOINT)&rcList, 2);
   int nHeight = rcList.bottom - rcList.top;
   rcList.bottom = rcDlg.bottom - m_szRB_Border.cx;
   rcList.top = rcList.bottom -nHeight;
   MoveWindow(GetDlgItem(m_hWnd, IDCANCEL), &rcList, bUpdate);

   GetWindowRect(GetDlgItem(m_hWnd, IDC_BTN_GRAPH), &rcList);
   ScreenToClient((LPPOINT)&rcList, 2);
   nHeight = rcList.bottom - rcList.top;
   rcList.bottom = rcDlg.bottom - m_szRB_Border.cx;
   rcList.top = rcList.bottom -nHeight;
   MoveWindow(GetDlgItem(m_hWnd, IDC_BTN_GRAPH), &rcList, bUpdate);

   GetWindowRect(GetDlgItem(m_hWnd, IDC_BTN_COPY_VALUES), &rcList);
   ScreenToClient((LPPOINT)&rcList, 2);
   nHeight = rcList.bottom - rcList.top;
   rcList.bottom = rcDlg.bottom - m_szRB_Border.cx;
   rcList.top = rcList.bottom -nHeight;
   MoveWindow(GetDlgItem(m_hWnd, IDC_BTN_COPY_VALUES), &rcList, bUpdate);

   GetWindowRect(GetDlgItem(m_hWnd, IDOK), &rcList);
   ScreenToClient((LPPOINT)&rcList, 2);
   nHeight = rcList.bottom - rcList.top;
   rcList.bottom = rcDlg.bottom - m_szRB_Border.cx;
   rcList.top = rcList.bottom -nHeight;
   MoveWindow(GetDlgItem(m_hWnd, IDOK), &rcList, bUpdate);
   if (IsDlgButtonChecked(m_hWnd, IDC_BTN_GRAPH))
   {
      InvalidateRect(GetDlgItem(m_hWnd,IDC_WND_GRAPH), NULL, TRUE);
   }
   if (!bUpdate)
   {
    InvalidateRect(m_hWnd, NULL, TRUE);
   }

   return 1;
}

void CCopyCSVDlg::PrepareGraphDC(HWND hWnd, HDC hdc)
{
   RECT rc;
   GetClientRect(hWnd, &rc);
   int nHeight = rc.bottom - rc.top;
   HRGN rgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
   SelectObject(hdc, rgn);
   SetMapMode(hdc, MM_ANISOTROPIC);
   if (m_nZoom)
   {
      int nHeight = (m_rcZoom.bottom-m_rcZoom.top) / 2;
      SetWindowOrgEx(hdc, m_rcZoom.left, m_rcZoom.top+nHeight, NULL);
      SetWindowExtEx(hdc, m_rcZoom.right-m_rcZoom.left, nHeight, NULL);
   }
   else
   {
      SetWindowOrgEx(hdc, 0, 0, NULL);
      SetWindowExtEx(hdc, m_nPoints, 5500, NULL);
   }
   SetViewportExtEx(hdc, rc.right - rc.left, -nHeight / 2, NULL);
   SetViewportOrgEx(hdc, 0, nHeight / 2, NULL);
}

void CCopyCSVDlg::OnDrawGraph(HWND hWnd, HDC hdc)
{
   if (m_ptCurve)
   {
      HPEN hPen = CreatePen(PS_SOLID, 0, RGB(255,0,0));
      SaveDC(hdc);
      PrepareGraphDC(hWnd, hdc);
      if (!m_nZoom)
      {
         int i, nPos;
         double dVal;
         wchar_t szTxt[64];
         TEXTMETRIC tm;
         RECT rc;
         GetClientRect(m_hWnd, &rc);
         DPtoLP(hdc, (POINT*)&rc, 2);
         GetTextMetrics(hdc, &tm);
         SetBkMode(hdc, TRANSPARENT);
         SetTextColor(hdc, RGB(255,255,255));
         SelectObject(hdc, GetStockObject(WHITE_PEN));
         dVal = m_dStart;
         for (i=0; i<m_nSteps; i++)
         {
            nPos = Round(dVal*m_dFaktor - m_dOffset);
            swprintf(szTxt, lengthof(szTxt), L"%g", dVal);
            TextOut(hdc, 0, nPos+tm.tmHeight, szTxt, (int)wcslen(szTxt));
            MoveToEx(hdc, 0, nPos, NULL);
            LineTo(hdc, m_nPoints, nPos);
            dVal += m_dStep;
         }
         nPos = m_nStartX;
         for (i=0; i<m_nStepsX; i++)
         {
            swprintf_s(szTxt, lengthof(szTxt), L"%d", nPos);
            TextOut(hdc, nPos, rc.top, szTxt, (int)wcslen(szTxt));
            MoveToEx(hdc, nPos, rc.bottom, NULL);
            LineTo(hdc, nPos, rc.top);
            nPos += m_nStepX;
         }
      }

      SelectObject(hdc, hPen);
      Polyline(hdc, m_ptCurve, m_nPoints);
      RestoreDC(hdc, -1);
      DeleteObject(hPen);
   }
}

void CCopyCSVDlg::OnLButtonDownGraph(HWND hWnd, UINT nFlags, POINT pt)
{
   HDC hdc = GetDC(hWnd);
   wchar_t szValues[128];
   PrepareGraphDC(hWnd, hdc);
   DPtoLP(hdc, &pt, 1);

   _itow_s(pt.x, szValues, lengthof(szValues), 10);
   wcscat_s(szValues, lengthof(szValues), L": ");

   if (pt.x >=0 && pt.x < m_nPoints)
   {
      wchar_t szValue[128];
      SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_GETTEXT, pt.x, (LPARAM)szValue);
      wcscat_s(szValues, lengthof(szValues), szValue);
      SendDlgItemMessage(m_hWnd, IDC_LIST_VALUES, LB_SETCURSEL, pt.x, 0);
   }
   SetDlgItemText(m_hWnd, IDC_EDT_VALUE, szValues);

   ReleaseDC(hWnd, hdc);
}

void CCopyCSVDlg::OnRButtonDownGraph(HWND hWnd, UINT nFlags, POINT pt)
{
   m_rcZooming.left = pt.x;
   m_rcZooming.top  = pt.y;
}

void CCopyCSVDlg::OnMouseMoveGraph(HWND hWnd, UINT nFlags, POINT pt)
{
   if (nFlags & MK_RBUTTON)
   {
      HDC hdc = GetDC(hWnd);
      SaveDC(hdc);
      PrepareGraphDC(hWnd, hdc);
      SelectObject(hdc, GetStockObject(WHITE_PEN));
      SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
      if (m_nZooming)
      {
         SetROP2(hdc, R2_XORPEN);
         Rectangle(hdc, m_rcZooming.left, m_rcZooming.top, m_rcZooming.right, m_rcZooming.bottom);
         DPtoLP(hdc, &pt, 1);
         m_rcZooming.right  = pt.x;
         m_rcZooming.bottom = pt.y;
         Rectangle(hdc, m_rcZooming.left, m_rcZooming.top, m_rcZooming.right, m_rcZooming.bottom);
      }
      else
      {
         m_rcZooming.right  = pt.x;
         m_rcZooming.bottom = pt.y;
         DPtoLP(hdc, (POINT*)&m_rcZooming, 2);
         Rectangle(hdc, m_rcZooming.left, m_rcZooming.top, m_rcZooming.right, m_rcZooming.bottom);
         m_nZooming = 1;
      }

      RestoreDC(hdc, -1);
      ReleaseDC(hWnd, hdc);
   }
}

void CCopyCSVDlg::OnRButtonUpGraph(HWND hWnd, UINT nFlags, POINT pt)
{
   m_rcZoom = m_rcZooming;
   
   int nTemp;
   if (m_rcZoom.right < m_rcZoom.left)
   {
      nTemp = m_rcZoom.right;
      m_rcZoom.right = m_rcZoom.left;
      m_rcZoom.left = nTemp;
   }
   if (m_rcZoom.bottom < m_rcZoom.top)
   {
      nTemp = m_rcZoom.bottom;
      m_rcZoom.bottom = m_rcZoom.top;
      m_rcZoom.top = nTemp;
   }

   //RECT rc;
   //GetClientRect(m_hWnd, &rc);
   //ETSDIV_DIV div(rc.top, rc.bottom, 50, dMin, dMax);
   //CalculateLinDivision(&div, m_dStep, m_dStart);
   //m_nSteps = Round((div.f2 - div.f1) / m_dStep);
   //m_nSteps++;

   //ETSDIV_DIV div1(rc.left, rc.right, 100, 0.0, (double)m_nPoints);
   //CalculateLinDivision(&div1, dMin, dMax);
   //m_nStepX  = Round(dMin);
   //m_nStartX = Round(dMax);
   //m_nStepsX = m_nPoints / m_nStepX;

   m_nZoom    = 1;
   m_nZooming = 0;
   InvalidateRect(hWnd, NULL, TRUE);
}

void CCopyCSVDlg::ScreenToClient(LPPOINT lpPoint, int nPoints)
{
   for (int i=0; i<nPoints; i++)
   {
      ::ScreenToClient(m_hWnd, &lpPoint[i]);
   }
}

BOOL CCopyCSVDlg::MoveWindow(HWND hWnd, RECT*prc, BOOL bRepaint)
{
   return ::MoveWindow(hWnd, prc->left, prc->top, prc->right-prc->left, prc->bottom-prc->top, bRepaint);
}

BOOL CALLBACK CCopyCSVDlg::MsgFilterEnum(HWND hwnd, LPARAM lParam)
{
   TCHAR szName[32];
   ::GetClassName(hwnd, szName, 32);
   if (wcscmp(szName, L"#32770") == 0)
   {
      MessageFilter *pmsf = (MessageFilter*)lParam;
      MSG *pmsg = (MSG*)pmsf->lParam;
      if (::GetProp(hwnd, THIS_POINTER_CDIALOG) == gm_hDll_Hook)
      {
         CCopyCSVDlg* pDlg = (CCopyCSVDlg*)::GetWindowLong(hwnd, DWL_USER);
         if (pDlg)
         {
            if ((pDlg->m_nFlags & IS_IN_MENU_LOOP) && 
                ((pmsg->message >= WM_MOUSEFIRST) && (pmsg->message <= WM_MOUSELAST))||
                ((pmsg->message >= WM_KEYFIRST  ) && (pmsg->message <= WM_KEYLAST)))
            {
               return (BOOL)::CallNextHookEx(gm_hDll_Hook, pmsf->code, pmsf->wParam, pmsf->lParam);
            }
         }
      }
      pmsg->time = 0;
      pmsf->bResult = ::IsDialogMessage(hwnd, pmsg);
      if (pmsf->bResult)   // Message für diesen Dialog ? 
      {
#ifdef _DEBUG
         ::GetWindowText(hwnd, szName, 32);
         //TRACE("%x, %s\n", hwnd, szName);
#endif // _DEBUG
         return 0;
      }
   }
   return 1;
}

/******************************************************************************
* Name       : MsgFilterProc                                                  *
* Zweck      : Filtern der Nachrichten für eine nicht modale Dialogbox aus der*
*              Nachrichtenschleife. Wenn die Nachricht von der Dialogbox      *
*              verarbeitet wird, so muß sie nicht weiter verarbeitet werden.  *
* Aufruf     : als HOOK Funktion in der Nachrichtenschleife, in der sie       *
*              installiert wurde.                                             *
* Parameter  :                                                                *
* code    (E): Code der Nachrichtverarbeitung                          (int)  *
* wParam (E) : erster Parameter der Nachricht                        (WPARAM) *
* lParam (E) : zweiter Parameter der Nachricht                       (LPARAM) *
* Funktionswert : ungleich 0, wenn die Nachricht nicht weiter        (LRESULT)*
*                 verarbeitet werden soll:                                    *
******************************************************************************/
LRESULT CALLBACK CCopyCSVDlg::MsgFilterProc(int code, WPARAM wParam,  LPARAM lParam)
{
   if (code >= 0) // wenn code größer gleich 0 ist dürfen die Nachrichten untersucht werden
   {
      if (lParam)
      {
         if (gm_hWndTopLevelParent)
         {
            MessageFilter msf ={code, wParam, lParam, 0};
            MSG *pmsg = (MSG*)lParam;
            if (pmsg->time != 0)
            {
               ::EnumChildWindows(gm_hWndTopLevelParent, MsgFilterEnum, (LPARAM)&msf);
               gm_nCallCount++;
            }
            if (msf.bResult)
            {
               MSG *pmsg = (MSG*)lParam;
               MSG msg;
               _asm CLD;
               PeekMessage(&msg, pmsg->hwnd, pmsg->message, pmsg->message, PM_REMOVE|PM_NOYIELD);
               return msf.bResult;
            }
         }
      }
   }
   return CallNextHookEx(gm_hDll_Hook, code, wParam, lParam);
}

HWND CCopyCSVDlg::Create(HWND hWndParent)
{
   m_hWndParent = hWndParent;
   if (m_hWnd && IsWindow(m_hWnd))
   {
      if (IsIconic(m_hWnd))
      {
         ChangeModalStyles(false);
         ShowWindow(m_hWnd, SW_RESTORE);
      }
      else
      {
         ChangeModalStyles(true);
         ShowWindow(m_hWnd, SW_MINIMIZE);
      }
      return m_hWnd;
   }
   else
   {
      if (_AtlModule.GetResourceInstance() != NULL)
      {
         m_bModal = false;
         return CreateDialogParam(_AtlModule.GetResourceInstance(),
             //MAKEINTRESOURCE(IDD_WATCH_ARRAY),
             (LPCTSTR)(WORD)(IDD_WATCH_ARRAY),
             m_hWndParent,
             (DLGPROC) DialogProc, (LPARAM) this);
      }
      return NULL;
   }
}

void CCopyCSVDlg::ChangeModalStyles(bool bMin)
{
   long lStyle;
   HMENU hMenu = NULL;
   if (m_hWnd) hMenu = ::GetSystemMenu(m_hWnd, false);
   if (hMenu)
   {
      if (bMin)
      {
         if ((m_nFlags & MODAL_W_CONTEXT_HELP) && (m_nFlags & (MODAL_W_MIN_BTN|MODAL_W_MAX_BTN)))
         {
            lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
            if (m_nFlags & MODAL_W_MIN_BTN) lStyle |= WS_MINIMIZEBOX;
            if (m_nFlags & MODAL_W_MAX_BTN) lStyle |= WS_MAXIMIZEBOX;
            else EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);

            EnableMenuItem(hMenu, SC_RESTORE, MF_BYCOMMAND | MF_ENABLED);
            EnableMenuItem(hMenu, SC_MINIMIZE, MF_BYCOMMAND | MF_GRAYED);

            ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

            lStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
            lStyle &= ~WS_EX_CONTEXTHELP;
            ::SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyle);
         }
      }
      else
      {
         if ((m_nFlags & MODAL_W_CONTEXT_HELP) && (m_nFlags & (MODAL_W_MIN_BTN|MODAL_W_MAX_BTN)))
         {
            lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
            if (m_nFlags & MODAL_W_MIN_BTN) lStyle &= ~WS_MINIMIZEBOX;
            if (m_nFlags & MODAL_W_MAX_BTN) lStyle &= ~WS_MAXIMIZEBOX;
            else EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED);

            EnableMenuItem(hMenu, SC_RESTORE, MF_BYCOMMAND | MF_GRAYED);
            EnableMenuItem(hMenu, SC_MINIMIZE, MF_BYCOMMAND | MF_ENABLED);

            ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

            lStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
            lStyle |= WS_EX_CONTEXTHELP;
            ::SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyle);
         }
      }
   }
}

LRESULT CALLBACK CCopyCSVDlg::DrawGraphProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   CCopyCSVDlg *pThis = (CCopyCSVDlg*)GetWindowLongPtr(hWnd, GWL_USERDATA);
   switch (Msg)
   {
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         BeginPaint(hWnd, &ps);
         pThis->OnDrawGraph(hWnd, ps.hdc);
         EndPaint(hWnd, &ps);
      }  return 0;
      case WM_ERASEBKGND:
      {
         RECT rc;
         GetClientRect(hWnd, &rc);
         PatBlt((HDC)wParam, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, BLACKNESS);
      }return 1;
      case WM_LBUTTONDOWN:
      {
         POINT pt = {LOWORD(lParam), HIWORD(lParam)};
         pThis->OnLButtonDownGraph(hWnd, (UINT)wParam, pt);
      } break;
      case WM_RBUTTONDOWN:
      {
         POINT pt = {LOWORD(lParam), HIWORD(lParam)};
         pThis->OnRButtonDownGraph(hWnd, (UINT)wParam, pt);
      } break;
      case WM_RBUTTONUP:
      {
         POINT pt = {LOWORD(lParam), HIWORD(lParam)};
         pThis->OnRButtonUpGraph(hWnd, (UINT)wParam, pt);
      } break;
      case WM_MOUSEMOVE:
      {
         POINT pt = {LOWORD(lParam), HIWORD(lParam)};
         pThis->OnMouseMoveGraph(hWnd, (UINT)wParam, pt);
      } break;
   }
   return CallWindowProc(gm_oldDrawGraphProc, hWnd, Msg, wParam, lParam);
}

int CCopyCSVDlg::Round(double dval)
{
   return (int)(dval+0.5);
}

bool CCopyCSVDlg::CalculateLinDivision(ETSDIV_DIV *pdiv, double& dStep, double& dStart)
{
   static const double range[] = {0.875, 1.5, 2.25, 3.25, 4.5, 6.25, 8.75, 15.0},// Schwellwerte
                       value[] = {     1.0, 2.0,  2.5,  4.0, 5.0,  7.5, 10.0   };// Schrittweite in Weltkoordinaten
   bool   bNegative = false;
   int    steps, i, exp;
   long   ldiff;
   double mant, dDiff, dNorm;

   ldiff = pdiv->l2 - pdiv->l1;                                // Mögliche Anzahl der Schritte ermitteln
   if (ldiff <= 0) {return false;}// Strecke darf nicht negativ oder 0 sein
   if (pdiv->stepwidth > 0) steps = Round((double)ldiff / (double)pdiv->stepwidth);
   else                     steps = 0;
   if (steps==0)   {return false;}

   dDiff = pdiv->f2 - pdiv->f1;                                // Schrittweite in Weltkoordinaten ermitteln
   if (dDiff == 0.0) {return false;}
   if (dDiff <  0.0) {dDiff = -dDiff; bNegative = true;};
   dNorm = dDiff / steps;

   exp = (int) log10(dNorm);                                   // Schrittweite normieren auf den Bereich 1.0,..,10.0
   dNorm = dNorm * pow(10.0, -exp);
   if (dNorm < 1.0)
   {
      dNorm*=10;
      exp--;
   }
   pdiv->dZero = pow(10.0, exp-8);
   for (i=0; i<7; i++)                                         // Schrittweite auf die Werte 10 * 10^exp normieren
   {
      if ((dNorm > range[i])&&(dNorm <= range[i+1]))            
      {
         mant = value[i];
         break;
      }
   }

   dStep = mant * pow(10.0, (double)exp);                      // echte Schrittweite berechnen
   pdiv->division = Round(dDiff / dStep);                      // Anzahl der Schritte ermitteln
   if (dStep == 0.0) { return false;}
   if (bNegative)
   {
      dStart = ceil(pdiv->f1 / dStep);                         // Anfangspunkt der Gittereinteilung ermitteln
      if (dStart <= 0.0) dStart-=1.0;
      dStart *= dStep;
      dStep = -dStep;
   }
   else
   {
      dStart = floor(pdiv->f1 / dStep);                        // Anfangspunkt der Gittereinteilung ermitteln
      if (dStart >= 0.0) dStart+=1.0;
      dStart *= dStep;
   }
   if (pdiv->division == 0) { return false;}
   pdiv->width  = ldiff / steps;                                // Schrittweite in logischen Koordinaten ermitteln
   pdiv->divfnc = ETSDIV_LINEAR;

   return true;
}
