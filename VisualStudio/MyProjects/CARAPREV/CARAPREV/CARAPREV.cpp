/********************************************************************/
/*                                                                  */
/*                       DLL ETSPreview                             */
/*                                                                  */
/*     Druckervorscheu und gemeinsame Programmodule für die         */
/*     Programme ETS2DV, ETS3DV, Factura, WaveGen                   */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// CARAPREV.cpp : Legt die Initialisierungsroutinen für die DLL fest.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "CARAPREV.h"
#include "ETS3DGLRegKeys.h"

static AFX_EXTENSION_MODULE CARAPREVDLL = { NULL, NULL };

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TCHAR g_szAppName[16];

CGetAppName::CGetAppName()
{
#ifndef _DEBUG
   HANDLE hMod = GetModuleHandle(NULL);
   TCHAR szName[_MAX_PATH];
   DWORD nLen = GetModuleFileName((HINSTANCE)hMod, szName, _MAX_PATH);
   if (nLen)
   {
	  int i;
      for (i=nLen; i>=0; i--)
      {
         if      (szName[i] == '.' ) szName[i] = 0;
         else if (szName[i] == '\\') break;
      }
      strncpy(g_szAppName, &szName[i+1], 8);
   }
   else
#endif
   {
      strcpy(g_szAppName, "ETSPREV");
   }
}

CGetAppName g_Name;

bool     g_bDoReport = REPORT_KEY_DEFAULT;

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#define  ETSDEBUG_CREATEDATA g_szAppName, &g_bDoReport
#endif
#include "CEtsDebug.h"

CGetAppName::~CGetAppName()
{
   //ETSDEBUGEND;
}

HINSTANCE g_hCaraPreviewInstance = NULL;
int       g_nFileVersion         = 0;
void     *g_AfxCommDlgProc       = NULL;
COLORREF  g_CustomColors[16] = {0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,
                                0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,
                                0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,
                                0x00FFFFFF,0x00FFFFFF,0x00FFFFFF,0x00FFFFFF};

// static Functions
UINT CALLBACK CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   if (uiMsg == WM_INITDIALOG)
   {
      CHOOSECOLOR *pcc = (CHOOSECOLOR*) lParam;
      if (hdlg && pcc && pcc->lCustData)
      {
         TCHAR *text = (TCHAR*) pcc->lCustData;
         ::SetWindowText(hdlg, text);
         return 0;
      }
   }
   if (g_AfxCommDlgProc)
      return (((COMMDLGPROC) g_AfxCommDlgProc)(hdlg,uiMsg,wParam,lParam));
   else
      return 0;
}

UINT CALLBACK CFHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   if (uiMsg == WM_INITDIALOG)
   {
      CHOOSEFONT *pcf = (CHOOSEFONT*) lParam;
      if (hdlg && pcf && pcf->lCustData)
      {
         TCHAR *text = (TCHAR*) pcf->lCustData;
         ::SetWindowText(hdlg, text);
         return 0;
      }
   }
   if (g_AfxCommDlgProc)
      return (((COMMDLGPROC) g_AfxCommDlgProc)(hdlg,uiMsg,wParam,lParam));
   else
      return 0;
}

extern void AFXAPI AfxTextFloatFormat(CDataExchange* pDX, int nIDC,
	void* pData, double value, int nSizeGcvt);

BOOL DDX_TextVar(CDataExchange *pDX, int nID, int nAcc, double &dVar, bool bThrow)
{
	BOOL bOK = FALSE;       // assume failure
	TRY
	{
      if (pDX->m_bSaveAndValidate)
      {
         static const int nTextSize = 64;
         TCHAR text[nTextSize];
         ::GetDlgItemText(pDX->m_pDlgWnd->GetSafeHwnd(), nID, text, nTextSize);
         int i, c;

         TCHAR szDecimal[4];
         if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDecimal, 4)==0)
            szDecimal[0] = ',';

         for (i=0; ((c=text[i])!=0) && (i<nTextSize); i++)
         {
            if (c == (int)'.') {text[i] = szDecimal[0]; continue;}// Punkt ist immer erlaubt
            if (!isdigit(c) &&                                 // Nummern
                (c != (int)szDecimal[0]) &&                    // Dezimalzeichen
                (c != (int)'-') && (c != (int)'+') &&          // Vorzeichen
                (c != (int)'e') &&                             // Exponent
                (c != (int)' '))                               // Leerzeichen
            {
               pDX->m_bSaveAndValidate = false;
               ::AfxTextFloatFormat(pDX, nID, &dVar, dVar, nAcc);
               pDX->m_bSaveAndValidate = true;
               pDX->Fail();
            }
         }
         dVar = atof(text);
      }
      else if (!pDX->m_bSaveAndValidate && _isnan(dVar))
      {
         CString str("--");
         DDX_Text(pDX, nID, str);
      }
      else
      {
         if (nAcc == FLT_DIG) nAcc++;
         ::AfxTextFloatFormat(pDX, nID, &dVar, dVar, nAcc);
      }
		bOK = TRUE;         // it worked
	}
	CATCH(CUserException, e)
	{
		// validation failed - user already alerted, fall through
      if (bThrow) pDX->Fail();
		ASSERT(!bOK);
		// Note: DELETE_EXCEPTION_(e) not required
	}
	AND_CATCH_ALL(e)
	{
		// validation failed due to OOM or other resource failure
      if (bThrow) pDX->Fail();
		e->ReportError(MB_ICONEXCLAMATION, AFX_IDP_INTERNAL_FAILURE);
		ASSERT(!bOK);
//		DELETE_EXCEPTION(e);
	}
	END_CATCH_ALL

	return bOK;
}

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Entfernen Sie dies, wenn Sie lpReserved verwenden
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("ETSPREV.DLL Initializing!\n");
		
		// One-Time-Initialisierung der Erweiterungs-DLL
		if (!AfxInitExtensionModule(CARAPREVDLL, hInstance))
			return 0;

      g_hCaraPreviewInstance = hInstance;
		// Diese DLL in Ressourcenkette einfügen
		// HINWEIS: Wird diese Erweiterungs-DLL implizit durch eine reguläre
		//  MFC-DLL (wie z.B. ein ActiveX-Steuerelement) anstelle einer
		//  MFC-Anwendung eingebunden, dann möchten Sie möglicherweise diese
		//  Zeile aus DllMain entfernen und eine eigene Funktion einfügen,
		//  die von dieser Erweiterungs-DLL exportiert wird. Die reguläre DLL,
		//  die diese Erweiterungs-DLL verwendet, sollte dann explizit die 
		//  Initialisierungsfunktion der Erweiterungs-DLL aufrufen. Anderenfalls 
		//  wird das CDynLinkLibrary-Objekt nicht mit der Recourcenkette der 
		//  regulären DLL verbunden, was zu ernsthaften Problemen
		//  führen kann.

		new CDynLinkLibrary(CARAPREVDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("ETSPREV.DLL Terminating!\n");
		// Bibliothek vor dem Aufruf der Destruktoren schließen
		AfxTermExtensionModule(CARAPREVDLL);
	}
	return 1;   // OK
}
