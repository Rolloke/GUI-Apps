/********************************************************************/
/* Funktionen der Klasse CMixerOptionDlg                            */
/********************************************************************/
#include "stdafx.h"
#include "MixerOptionDlg.h"
#include "MixerDlg.h"
#include "EtsRegistry.h"
#include "ETS3DGLRegKeys.h"
#include "resource.h"
#include "CEtsHelp.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"
 
CMixerOptionDlg::CMixerOptionDlg()
{
   Constructor();
}

CMixerOptionDlg::CMixerOptionDlg(HINSTANCE hInstance, int nID, HWND hwndParent) : CEtsDialog(hInstance, nID, hwndParent)
{
   Constructor();
}

CMixerOptionDlg::~CMixerOptionDlg()
{
} 

void CMixerOptionDlg::Constructor()
{
}

bool CMixerOptionDlg::OnInitDialog(HWND hWnd) 
{
   BEGIN("OnInitDialog");
   if (CEtsDialog::OnInitDialog(hWnd))
   {
      HKEY hKey = NULL;
      if (OpenRegistry(m_hInstance, KEY_READ, &hKey, MIXER))   // Welcher Mixer ?
      {
         FILETIME ft;
         int i;
         char  szSubKey[MAX_PATH] = MIXER;
         strcat(szSubKey, "\\");
         int nLen = strlen(szSubKey);
         DWORD dwLength;
         for (i=0, dwLength = MAX_PATH; ::RegEnumKeyEx(hKey, i, &szSubKey[nLen], &dwLength, NULL, NULL, 0, &ft) == ERROR_SUCCESS; i++, dwLength = MAX_PATH)
         {
            EnumMixerKeys(szSubKey);
         }         
      }
      EtsRegCloseKey(hKey);
      return true;
   }
   return false;
}

void CMixerOptionDlg::OnEndDialog(int nResult)
{
   CEtsDialog::OnEndDialog(nResult);                              // Funktion der Basisklasse aufrufen
}

int CMixerOptionDlg::OnOk(WORD nNotifyCode)
{
   if (nNotifyCode == 0)
   {
      return IDOK;
   }
   return 0;
}

int CMixerOptionDlg::OnCommand(WORD nID , WORD nNotifyCode, HWND hwndControl)
{
   if (nNotifyCode == BN_CLICKED)
   {
   }

   return CEtsDialog::OnCommand(nID , nNotifyCode, hwndControl);
}

int CMixerOptionDlg::OnMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
/*
   switch(nMsg)
   {
   }
*/
   return CEtsDialog::OnMessage(nMsg, wParam, lParam);
}



void CMixerOptionDlg::EnumMixerKeys(char *pszSubKey)
{
   HKEY hKey = NULL;
   FILETIME ft;
   int i;
   if (OpenRegistry(m_hInstance, KEY_READ, &hKey, pszSubKey))   // Welcher Mixer ?
   {
      strcat(pszSubKey, "\\");
      int nLen = strlen(pszSubKey);
      DWORD dwLength;
      for (i=0, dwLength = MAX_PATH; ::RegEnumKeyEx(hKey, i, &pszSubKey[nLen], &dwLength, NULL, NULL, 0, &ft) == ERROR_SUCCESS; i++, dwLength = MAX_PATH)
      {
         EnumMixerKeys(pszSubKey);
      }
   }
   EtsRegCloseKey(hKey);
}
