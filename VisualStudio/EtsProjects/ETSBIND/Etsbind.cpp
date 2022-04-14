#define  WIN32_LEAN_AND_MEAN		                // Selten benutzte Teile der Windows-Header nicht einbinden
#define  STRICT                                  // Wichtig für typesave API declarations
#include <windows.h>
#define   ETSBIND_EXPORTS                        // Wichtig, damit die Funktionen exportiert werden
#include "ETSBIND.h"
#include "ETS3DGLRegKeys.h"
#ifdef _DEBUG
   #include <stdio.h>
#endif

const char g_szHeadLine[]     = "Critical runtime error !";
const char *g_szError[]        = {"%s file not found\n%s%s",
                                  "%s can´t bind all functions\n%s%s",
                                  "%s illegal destructor call ignored\n%s%s",
                                  "%s not removed\n%s%s",
                                  "%s destructor not called\n%s%s",
                                  "%s destructor not found\n%s%s"};

#define PATH_LENGTH 512
#define _MAX_PATH   256

char g_szAltPath[PATH_LENGTH];   // ein alternativer Suchpath für die DLL´s
char g_szFN[_MAX_PATH] = "";     // Aufrufendes Modul
bool g_bDebug          = false;

void GetRegValues();


BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
   switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
      {
         DisableThreadLibraryCalls(hModule);   // kein Aufruf von DLLMain bei Threaderzeugung
         GetRegValues();
      }
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

#ifdef _DEBUG
void DebugPrint(char *lpszFormat, ...)
{   
   va_list args;
   va_start(args, lpszFormat);
   FILE *fp = fopen("test.txt", "a+t");
   if (fp == NULL) return;
   TCHAR szBuffer[1024];

   vsprintf(szBuffer, lpszFormat, args);

   fprintf(fp, "%s\n", szBuffer);

   va_end(args);
   fclose(fp);
}
#endif

void GetRegValues()
{
   HKEY hKey;
#ifdef _DEBUG
   DebugPrint("Test");
#endif
   if(RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\ETSKIEL\\ETSBIND",0,KEY_READ,&hKey)==ERROR_SUCCESS)
   {
      DWORD  dwSize = sizeof(DWORD), dwType, dwValue = 0;
      char  *path   = REGKEY_DLLPATH;
#ifdef _DEBUG
      DebugPrint("RegOpened");
#endif

      if(RegQueryValueEx(hKey, REGKEY_USELOCALDLL,NULL, &dwType,(BYTE*)&dwValue, &dwSize)==ERROR_SUCCESS)
      {
         if ((dwType==REG_DWORD) && (dwValue != 0))            // Type muß DWORD sein
         {
#ifdef _DEBUG
            DebugPrint("UseLocal");
#endif
            GetCurrentDirectory(PATH_LENGTH, g_szAltPath);
            RegCloseKey(hKey);
            return;
         }
      }
#ifdef _DEBUG
      DebugPrint("Past Local");
#endif
      if(RegQueryValueEx(hKey, REGKEY_USEDEBUGDLL,NULL, &dwType,(BYTE*)&dwValue, &dwSize)==ERROR_SUCCESS)
      {
         if ((dwType==REG_DWORD) && (dwValue != 0))            // Type muß DWORD sein
         {
#ifdef _DEBUG
            DebugPrint("UseDebug");
#endif
            path = REGKEY_DLLPATH_DEBUG;
            g_bDebug = true;
         }
      }
#ifdef _DEBUG
      DebugPrint("Past Debug");
#endif
      if (!g_bDebug)
      {
         HANDLE hMod = GetModuleHandle(NULL);
         GetModuleFileName((HINSTANCE)hMod, g_szFN, _MAX_PATH);
         int   bytes = GetFileVersionInfoSize(g_szFN,NULL);
#ifdef _DEBUG
         DebugPrint("Module %s", g_szFN);
#endif
         if(bytes)
         {
            void         *buffer = new char[bytes];
            unsigned int  length = bytes;
            if (buffer)
            {
               GetFileVersionInfo(g_szFN, NULL, bytes, buffer);
               VS_FIXEDFILEINFO * Version;
               if(VerQueryValue(buffer,"\\",(void **) &Version, &length))
               {
                  if (Version->dwFileFlags & VS_FF_DEBUG)
                  {
#ifdef _DEBUG
                     DebugPrint("Debug Prog");
#endif
                     path = REGKEY_DLLPATH_DEBUG;
                     g_bDebug = true;
                  }
               }
            }
            if (!g_bDebug)
            {
               struct VerX
               {
                  BYTE lang, sublang;
                  WORD w;
               };
               length = sizeof(VerX);
               VerX *v;
               if (buffer)
               {
                  if (VerQueryValue(buffer,"\\VarFileInfo\\Translation", (void**) &v, &length))
                  {
                     if      (v->lang == LANG_GERMAN)
                     {
#ifdef _DEBUG
                        DebugPrint("German Release");
#endif
                        path = REGKEY_DLLPATH_RELEASE;
                     }
                     else if (v->lang == LANG_ENGLISH)
                     {
#ifdef _DEBUG
                        DebugPrint("English Release");
#endif
                        path = REGKEY_DLLPATH_RELEASE_E;
                     }
                  }
               }
            }
            if (buffer) delete[] buffer;
         }
      }
      dwSize = PATH_LENGTH;
      if(RegQueryValueEx(hKey,path,NULL,&dwType,(BYTE*)g_szAltPath,&dwSize)==ERROR_SUCCESS)
      {
         if(dwType==REG_SZ)                      // Type muß REG_SZ sein
         {
#ifdef _DEBUG
            DebugPrint("Path: %s", g_szAltPath);
#endif
            RegCloseKey(hKey);                   // war nicht vorhanden 27.3.2000 verbessert
            return;                              // OK, alternativen Path besorgt
         }
      }
      RegCloseKey(hKey);                         // war nicht vorhanden 27.3.2000 verbessert
   }

   g_szAltPath[0] = 0;                           //
}


void ETSErrorMessage(const char * szDllName,int nNumber)
{
   char szMessage[1024];
                                                 // Achtung keine Parameterüberprüfung !
   wsprintf(szMessage, g_szError[nNumber],szDllName, "Error occurred in Module :\n",  g_szFN);

   MessageBox(NULL,szMessage,g_szHeadLine,MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
}


// interne Funktion DLL wieder Freigeben, Datenfelder löschen

void FreeAndClear(const char * szDllName, HMODULE& hModule,void** ppFct,int nCount)
{
   if(hModule)
   {
      FreeLibrary(hModule);                      // dann DLL freigeben
   }

   hModule = NULL;                               // und Handle zurücksetzen
                                                 // und alle Funktionspointer löschen
   for (int clear = 0;clear <nCount;clear++)
   {
      ppFct[clear] = NULL;
   }
}



// exportierte Funktionen

ETSBIND_API bool pascal ETSBind(const char * szDllName, HMODULE& hModule,void** ppFct,int nCount,bool bLoadError)
{
   hModule = NULL;                               // DLL noch nicht geladen !

   if(!g_bDebug && GetFileAttributes(szDllName) != 0xffffffff)// im aktuellen Dir diese DLL vorhanden ?
   {                                             // abfrage nötig da das System auch woanders suchen kann !
      hModule = LoadLibrary(szDllName);          // lade die Komponenten-Dll im aktuellen Path, da hier vorhanden !
   }

   if((hModule == NULL)&&                        // DLL noch nicht geladen und
      (g_szAltPath[0] != 0))                     // einen alternativer Suchpath  vorhanden ?
   {
      char szPath[512];

      wsprintf(szPath,"%s%s",g_szAltPath,szDllName);

      hModule = LoadLibrary(szPath);             // versuche die Komponenten-Dll zu laden im g_zsAltPath
   }

   if(hModule == NULL)                           // DLL immer noch nicht geladen ?
   {
      hModule = LoadLibrary(szDllName);          // versuche die Komponenten-Dll zu laden mit alle Suchmethoden
   }

   if(hModule)                                   // konnte DLL geladen werden ?
   {
                                                 // überprüfe ob der Destructor nicht vorhanden ist
      if(GetProcAddress(hModule,MAKEINTRESOURCE(1))==NULL)
      {
         ETSErrorMessage(szDllName,5);           // Error: destructor not found
                                                 // DLL Freigeben , Datenfelder löschen
         FreeAndClear(szDllName,hModule,ppFct,nCount);
         ExitProcess(0x0001);                    // Programme beenden , Fehlercode 1
      }
      else                                       // OK, Destructor vorhanden alle weiteren Funktionen binden
      {
         for (int nLoad=0;nLoad < nCount;nLoad++)
         {                                       // alle Funktionen beginnen bei Ordinal 2 aufsteigend ohne Lücken
            ppFct[nLoad] = GetProcAddress(hModule,MAKEINTRESOURCE(nLoad + 2));
            if(ppFct[nLoad] == NULL) break;      // Funktion konnte nicht gebunden werden
         }

         if(nLoad != nCount)                     // Fehler beim Binden der Funktionen ?
         {
            ETSErrorMessage(szDllName,1);        // Error: can´t bind all functions
                                                 // DLL Freigeben , Datenfelder löschen
            FreeAndClear(szDllName,hModule,ppFct,nCount);
            ExitProcess(2 + nLoad);              // Programme beenden , Fehlercode Orddinal das nich gefunden wurde
         }
         else                                    // OK, die DLL wurde eingebunden
         {
            SetLastError(0x0000);                // kein Fehler !
            return false;                        // Rückgabewert wenn kein Fehler
         }
      }
   }
   else                                          // die DLL konnte nicht geladen werden
   {
      if(bLoadError)                             // soll ein Ladenfehler ausgewerted werden ?
      {
         ETSErrorMessage(szDllName,0);           // Error: file not found
         ExitProcess(0xffff);                    // Programme beenden !
      }
      else
      {
         SetLastError(0xffff);                   // setze Fehlercode DLL konnte nicht gefunden werden
      }
   }
   return true;                                  // Funktion mit Fehler beenden
}

ETSBIND_API void pascal ETSUnBind(const char * szDllName, HMODULE& hModule,void** ppFct,int nCount,const int * pnInstances,void * pParam)
{
   if(pnInstances != NULL)                       // eine dynamische DLL-Schnittstellenklasse ?
   {
      if(*pnInstances != 1)                      // nur die letzte Instanz darf ETSUnBind aufrufen
      {
         ETSErrorMessage(szDllName,2);           // Error: illegal destructor call ignored
         return;                                 // Funktion sofort beenden !
      }
   }

   if(hModule != NULL)                           // ist die DLL noch eingebunden ?
   {
      void (*pDestructor)(void*);                // Pointer für den DLL-Destructor
                                                 // und Pointer besorgen
      pDestructor = (void(*)(void*)) GetProcAddress(hModule,MAKEINTRESOURCE(1));

                                                 // rufe den DLL Destructor auf (immer die Funktion mit Ordinal 1)
      if(pDestructor) pDestructor(pParam);       // wenn vorhanden aufrufen
      else ETSErrorMessage(szDllName,4);         // Error: destructor not called
                                                 // DLL Freigeben , Datenfelder löschen
      FreeAndClear(szDllName,hModule,ppFct,nCount);
   }
}

ETSBIND_API void pascal ETSTestUnBind(const char * szDllName, HMODULE& hModule,const int * pnInstances)
{
   bool bDoTest = true;                          // Flag, soll der Test erfolgen

   if(pnInstances != NULL)                       // eine dynamische DLL-Schnittstellenklasse ?
   {
      if(*pnInstances > 0) bDoTest = false;      // noch weitere Instanzen vorhanden, kein Test !
   }

   if(bDoTest)                                   // soll der Test erfolgen ?
   {
      if(hModule != NULL)                        // DLL noch eingebunden, dann Fehlermeldung !
      {
         ETSErrorMessage(szDllName,3);           // Error: not removed
      }
   }
}
 
ETSBIND_API bool pascal ETSFoundInDefault(const char * szDllName)
{
   if(!g_bDebug && GetFileAttributes(szDllName) != 0xffffffff)// im aktuellen Dir diese DLL vorhanden ?
   {
      return true;
   }

   if(g_szAltPath[0] != 0)                       // einen alternativer Suchpath  vorhanden ?
   {
      char szPath[512];

      wsprintf(szPath,"%s%s",g_szAltPath,szDllName);

      if(GetFileAttributes(szPath) != 0xffffffff)// im alternativem Dir diese DLL vorhanden ?
      {
         return true;
      }
   }

   return false;                                 // DLL oder "Datei" nicht gefunden !
}

ETSBIND_API bool pascal ETSGetCommonPath(char * szCommon,int length)
{
   int l = 0;

   while(g_szAltPath[l]!=0) l++;

   if((l==0)||(l >= length)) return false;       // kein alternativer Path vorhanden oder Puffer zu klein !

   wsprintf(szCommon,"%s",g_szAltPath);          // Path kopieren !

   return true;
}

