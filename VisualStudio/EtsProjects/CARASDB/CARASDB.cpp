/********************************************************************/
/*                                                                  */
/*                        DLL CARASDB                               */
/*                                                                  */
/*     Lautsprecherauswahldatenbank/Dialog                          */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/

#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

#include <COMMCTRL.H>
#include <MATH.H>
#include <eh.H>
#include <locale.h>

#include "CCaraSdbDlg.h"
#include "CEtsLSMb.h"
#include "resource.h"
#include "CustErr.h"
#include "ETS3DGLRegKeys.h"
#include "CCaraMat.h"

bool     g_bDoReport = 0;
#define  ETS_DEBUGNAME CARASDB_REGKEY
#define  ETSDEBUG_INCLUDE 
#define  ETSDEBUG_CREATEDATA ETS_DEBUGNAME, &g_bDoReport
#include "CEtsDebug.h"

HINSTANCE      g_hInstance;      // globaler Instance-Handle
CEtsLsmb       g_CLsmb;
CCaraWalk      g_CaraWalk;
CCaraMat       g_CaraMat;
HANDLE         g_hMyHeap;        // Handle auf privaten Heap
int            g_nAllocCounter;

extern "C"
{
   void  Destructor(void *);
   int   DoModal(CARASDB_PARAMS &);
	void  UpdateDataBase(char*);
}

int DoModal(CARASDB_PARAMS &csdb)
{  
   void *pOld = _set_se_translator(Se_TranslatorFnc);
   int nReturn = 0;
   char szLocale[_MAX_PATH];
   CCaraSdbDlg    SdbDlg;

   ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, szLocale, _MAX_PATH);
   setlocale(LC_ALL, szLocale);

   INITCOMMONCONTROLSEX InitCtrls = {sizeof(INITCOMMONCONTROLSEX), 0};
   InitCtrls.dwICC = ICC_WIN95_CLASSES;
   InitCommonControlsEx(&InitCtrls);

   SdbDlg.Init(g_hInstance, IDD_SELECT_BOX, csdb.hwndParent);
   SdbDlg.m_pCaraSdbP = &csdb;
   nReturn = SdbDlg.DoModal();
   if ((nReturn!=IDOK) && (SdbDlg.m_nError))
      nReturn = SdbDlg.m_nError;

   _set_se_translator((_se_translator_function)pOld);
   return nReturn;
}

void  UpdateDataBase(char*pszName)
{
   CCaraSdbDlg SdbDlg;
   SdbDlg.UpdateDataBase(pszName);
}

void Destructor(void *p)
{
   g_CLsmb.Destructor();
   g_CaraWalk.Destructor();
   g_CaraMat.Destructor();
   CEtsDialog::Destructor();
   ETSDEBUGEND;
}

/********************************************************************/
/*                       DLL Main Funktion                          */
/********************************************************************/
BOOL WINAPI DllMain(HINSTANCE hInstance,DWORD fReason,LPVOID pvReseved)
{
   switch (fReason)
   {
      case DLL_PROCESS_ATTACH:                  // Einbinden der DLL in den Adressraum des Prozesses
      {
         DisableThreadLibraryCalls(hInstance);
         g_hInstance = hInstance;               // Instance-Handle global speichern

         HKEY hKey;
         char szRegKey[_MAX_PATH];
         wsprintf(szRegKey, "%s\\%s\\%s", SOFTWARE_REGKEY, ETSKIEL_REGKEY, ETS_DEBUGNAME);
         if (RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
         {
            DWORD dwSize = sizeof(DWORD), dwValue, dwType;
            if (RegQueryValueEx(hKey, REPORT_KEY, NULL, &dwType, (BYTE*) &dwValue, &dwSize)==ERROR_SUCCESS)
            {
               if ((dwType== REG_DWORD)&&(dwValue!=0))
               {
                  g_bDoReport = true;
                  wsprintf(szRegKey, "%s.DBG", ETS_DEBUGNAME);
                  DeleteFile(szRegKey);
               }
            }
            RegCloseKey(hKey);
         }
      } break;

      case DLL_PROCESS_DETACH:                  // Die DLL wird freigegeben
         if (g_hMyHeap)   HeapDestroy(g_hMyHeap);
         if (g_nAllocCounter != 0)
         {
            MessageBox( (HWND) NULL, "Memory leaks detected !",
                        "Fatal Error: HEAP", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
         }
         break;
   }

   return true;
}

void Se_TranslatorFnc(unsigned int e, _EXCEPTION_POINTERS*p)
{
   switch (e)
   {
      case EXCEPTION_ACCESS_VIOLATION:      REPORT("ACCESS_VIOLATION");      break;
      case EXCEPTION_DATATYPE_MISALIGNMENT: REPORT("DATATYPE_MISALIGNMENT"); break;  
      case EXCEPTION_PRIV_INSTRUCTION:      REPORT("PRIV_INSTRUCTION");      break;  
      case EXCEPTION_IN_PAGE_ERROR:         REPORT("IN_PAGE_ERROR");         break;  
      case EXCEPTION_STACK_OVERFLOW:        REPORT("STACK_OVERFLOW");        break;  
      case EXCEPTION_INVALID_DISPOSITION:   REPORT("INVALID_DISPOSITION");   break;  
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: REPORT("ARRAY_BOUNDS_EXCEEDED"); break;
      case EXCEPTION_FLT_DENORMAL_OPERAND:  REPORT("FLT_DENORMAL_OPERAND");  break;
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:    REPORT("FLT_DIVIDE_BY_ZERO");    break;
      case EXCEPTION_FLT_INEXACT_RESULT:    REPORT("FLT_INEXACT_RESULT");    break;
      case EXCEPTION_FLT_INVALID_OPERATION: REPORT("FLT_INVALID_OPERATION"); break;
      case EXCEPTION_FLT_OVERFLOW:          REPORT("FLT_OVERFLOW");          break;
      case EXCEPTION_FLT_STACK_CHECK:       REPORT("FLT_STACK_CHECK");       break;
      case EXCEPTION_FLT_UNDERFLOW:         REPORT("FLT_UNDERFLOW");         break;
      case EXCEPTION_INT_DIVIDE_BY_ZERO:    REPORT("INT_DIVIDE_BY_ZERO");    break;
      case EXCEPTION_INT_OVERFLOW:          REPORT("INT_OVERFLOW");          break;
      case EXCEPTION_INVALID_HANDLE:        REPORT("INVALID_HANDLE");        break;
      default : e = 0;
         break;
   }
   if (e) throw (DWORD) e;
}

void * __cdecl operator new(unsigned int s)
{
   void   *pointer;

   if (g_hMyHeap==NULL)
   {
      g_hMyHeap = HeapCreate(0, 1048576, 0); // priv. Heap anlegen
      g_nAllocCounter = 0;
      if (g_hMyHeap == NULL) throw (DWORD) CARABOX_ALLOCATION_ERROR;
   }

   if( (pointer = HeapAlloc(g_hMyHeap, HEAP_ZERO_MEMORY, s)) == NULL )
   {
      REPORT("Error : HeapAlloc");
      throw (DWORD) CARABOX_ALLOCATION_ERROR;
   }
   else   g_nAllocCounter++;

   return (pointer);
}

void __cdecl operator delete( void *s)
{
   if(HeapFree(g_hMyHeap, 0, s) != NULL )   g_nAllocCounter--;
   else
   {
      REPORT("Error : HeapFree");
      throw (DWORD) CARABOX_DESTRUCTION_ERROR;
   }
}
