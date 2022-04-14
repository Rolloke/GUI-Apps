/********************************************************************/
/*                                                                  */
/*                     Library EtsRegistry                          */
/*                                                                  */
/*     Registryfunktionen mit automatischer Umschaltung auf         */
/*     INI-Dateien, wenn die Zugriffsrechte fehlen.                 */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/

#include "debug.h"
#include "EtsRegistry.h"
#include "ETS3DGLRegKeys.h"

#define MODULE_REGKEY   "CARA"   
#define SOFTWARE_REGKEY "Software"
#define ETSKIEL_REGKEY  "ETSKIEL"
#define REPORT_KEY      "Report"

static char *g_pszModuleRegKey   = MODULE_REGKEY;
static char *g_pszEtsKielRegKey  = ETSKIEL_REGKEY;
static char *g_pszSoftwareRegKey = SOFTWARE_REGKEY;
static char *g_pszReportRegKey   = REPORT_KEY;
static char  g_szRegistryAndVersionKey[MAX_PATH] = "";
static bool  g_bIniFile                          = false;
static bool  g_bWintNT                           = false;

void SetGlobalRegParam(char *pszModule, char *pszEtsKiel, char *pszSoftware, bool bIni)
{
   if (pszModule)   g_pszModuleRegKey   = pszModule;
   if (pszEtsKiel)  g_pszEtsKielRegKey  = pszEtsKiel;
   if (pszSoftware) g_pszSoftwareRegKey = pszSoftware;
   g_bIniFile = bIni;
   if (g_bIniFile)
   {
      strcpy(g_szRegistryAndVersionKey, g_pszModuleRegKey);
   }
   else
   {
      g_szRegistryAndVersionKey[0] = 0;
   }
}

bool OpenRegistry(HINSTANCE hInstance, REGSAM regsam, PHKEY phKey, char *pszSubKey, bool *pbDebug)
{
   ASSERT(*phKey == 0);
   char szRegKey[MAX_PATH];
   if (g_bIniFile)
   {
      *phKey = (HKEY)pszSubKey;
      return true;
   }
   if (g_szRegistryAndVersionKey[0] == 0)
   {
      OSVERSIONINFO osv = {sizeof(OSVERSIONINFO) , 0, 0, 0, 0, ""};
      ::GetVersionEx(&osv);
      if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
      {
         g_bWintNT = true;
      }

      GetModuleFileName(hInstance, szRegKey, MAX_PATH);
      int bytes = GetFileVersionInfoSize(szRegKey, NULL);
      if(bytes)
      {
         void * buffer = new char[bytes];
         GetFileVersionInfo(szRegKey, NULL, bytes, buffer);
         VS_FIXEDFILEINFO * Version;
         unsigned int     length;
         if(VerQueryValue(buffer,"\\",(void **) &Version, &length))
         {
            wsprintf(szRegKey, "%s\\%s\\%s", g_pszSoftwareRegKey, g_pszEtsKielRegKey, g_pszModuleRegKey);
            int nVersion = HIWORD(Version->dwFileVersionMS);
            int nSubVers = LOWORD(Version->dwFileVersionMS);
            if ((nSubVers > 0) && (nSubVers < 10)) nSubVers *= 10;
            wsprintf(g_szRegistryAndVersionKey, "%s\\V%d.%02d", szRegKey, nVersion, nSubVers);
            if (RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_READ, phKey)==ERROR_SUCCESS)
            {
               if (pbDebug) *pbDebug = (GetRegDWord(*phKey, g_pszReportRegKey, REPORT_KEY_DEFAULT) != 0) ? true : false;
               RegCloseKey(*phKey);
            }
            else
            {                                                  // noch kein Registryeintrag vorhanden
               DWORD dwDisposition = 0;
               if (RegCreateKeyEx(HKEY_CURRENT_USER, szRegKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, phKey, &dwDisposition)==ERROR_SUCCESS)
               {                                               // Registrykey erstellen
                  SetRegDWord(*phKey, g_pszReportRegKey, REPORT_KEY_DEFAULT);
                  EtsRegCloseKey(*phKey);
               }
               else
               {
                  ::GetWindowsDirectory(g_szRegistryAndVersionKey     , MAX_PATH);
                  strncat(g_szRegistryAndVersionKey, "\\"             , MAX_PATH);
                  strncat(g_szRegistryAndVersionKey, g_pszModuleRegKey, MAX_PATH);
                  strncat(g_szRegistryAndVersionKey, ".ini"           , MAX_PATH);
                  g_bIniFile = true;
                  *phKey = (HKEY)pszSubKey;
                  delete[] buffer;
                  return true;
               }
            }
         }
         delete[] buffer;
      }
      else strcpy(g_szRegistryAndVersionKey, g_pszModuleRegKey);
   }

   strcpy(szRegKey, g_szRegistryAndVersionKey);
   if (pszSubKey)
   {
      strcat(szRegKey, "\\");
      strcat(szRegKey, pszSubKey);
   }

   if (RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, regsam, phKey)==ERROR_SUCCESS)
      return true;

   if (regsam & KEY_WRITE)
   {
      DWORD dwDisposition = 0;
      if (RegCreateKeyEx(HKEY_CURRENT_USER, szRegKey, 0, NULL, REG_OPTION_NON_VOLATILE, regsam, NULL, phKey, &dwDisposition)==ERROR_SUCCESS)
         return true;
   }
   return false;
}

bool EtsRegCloseKey(HKEY &hKey)
{
   if (g_bIniFile)
   {
      hKey = NULL;
      return true;
   }
   bool bReturn = false;
   ASSERT(hKey != NULL);
   bReturn = (::RegCloseKey(hKey) == ERROR_SUCCESS) ? true : false;
   hKey = NULL;
   return bReturn;
}

DWORD GetRegDWord(HKEY hKey, char *pszKey, DWORD dwDef)
{
   if (g_bIniFile)
   {
      return ::GetPrivateProfileInt((LPCTSTR)hKey, pszKey, dwDef, ::g_szRegistryAndVersionKey);
   }
   else
   {
      DWORD dwSize = sizeof(DWORD), dwType, dwValue;
      if ((RegQueryValueEx(hKey, pszKey, NULL, &dwType, (BYTE*) &dwValue, &dwSize)==ERROR_SUCCESS) &&
          (dwType == REG_DWORD))
         return dwValue;
   }
   return dwDef;
}

bool SetRegDWord(HKEY hKey, char *pszKey, DWORD dwValue)
{
   if (g_bIniFile)
   {
      char text[32];
      wsprintf(text, "%d", dwValue);
      return (WritePrivateProfileString((LPCTSTR)hKey, pszKey, text, g_szRegistryAndVersionKey) != 0) ? true:false;
   }
   return (RegSetValueEx(hKey, pszKey, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD))==ERROR_SUCCESS) ? true : false;
}

bool GetRegBinary(HKEY hKey, char *pszKey, void *pParam, DWORD &dwSize, bool bText)
{
   if (g_bIniFile)
   {
      dwSize = GetPrivateProfileStruct((LPCTSTR)hKey, pszKey,  (LPTSTR) pParam, dwSize, ::g_szRegistryAndVersionKey);
      if (dwSize != NULL)
      {
         if (bText)
         {
            ((char*)pParam)[dwSize] = 0;
         }
         return true;
      }
   }
   DWORD dwType;
   if ((RegQueryValueEx(hKey, pszKey, NULL, &dwType, (BYTE*) pParam, &dwSize)==ERROR_SUCCESS) && (dwSize != 0))
   {
      if (bText)
      {
         if (dwType == REG_SZ)
         {
            ((char*)pParam)[dwSize] = 0;
            return true;
         }
      }
      else
      {
         if (dwType == REG_BINARY) return true;
      }
   }
   return false;
}

bool SetRegBinary(HKEY hKey, char *pszKey, BYTE* pbValue, DWORD dwSize, bool bText)
{
   if (bText && (pbValue != 0) && (dwSize==0))
   {
      dwSize = strlen((char*)pbValue);
   }
   if (g_bWintNT && bText) dwSize++;
   if (g_bIniFile)
   {
      WritePrivateProfileStruct((LPCTSTR)hKey, pszKey, (void*)pbValue, dwSize, ::g_szRegistryAndVersionKey);
      return true;
   }
   return (RegSetValueEx(hKey, pszKey, 0, bText? REG_SZ : REG_BINARY, pbValue, dwSize)==ERROR_SUCCESS) ? true : false;
}

