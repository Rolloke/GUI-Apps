/********************************************************************/
/*                                                                  */
/*           DLL CARABOX für das CARA-Programmodul CARACAD          */
/*                                                                  */
/*        Boxen-Editor mit Erstellen der BDD-Boxen-Datensätze       */
/*                   (max. 32 Chassis incl. BR,TL)                  */
/*                                                                  */
/*     programmed by Dr.U.F.Thomanek and Rolf Kary-Ehlers           */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/

#define  STRICT
#include <WINDOWS.H>
#include <COMMCTRL.H>
#include <eh.H>

#include "RESOURCE.H"
#include "BOXGLOBAL.H"

#include "BoxPropertySheet.h"
#include "BoxDescription.h"
#include "BoxBaseData.h"
#include "BoxCrossOver.h"
#include "BoxChassisData.h"
#include "BoxCabinetDesign.h"
#include "CEtsDiv.h"
#include "Debug.h"
#include "CustErr.h"
#include "EtsRegistry.h"
#include "ETS3DGLRegKeys.h"
#include "EtsAppWndMsg.h"
#include "Ets3DGL.h"

#include "locale.h"

bool     g_bDoReport = 0;
#define  ETS_DEBUGNAME CARABOX_REGKEY
#define  ETSDEBUG_INCLUDE 
#define  ETSDEBUG_CREATEDATA ETS_DEBUGNAME, &g_bDoReport
#include "CEtsDebug.h"

int         g_nErrorCode;                 // Fehlercode, mit GetLastErrorCode() abfragbar
HINSTANCE   g_hInstance;                  // globale Speicherung des Instanz-Handles f. LoadString
HANDLE      g_hMyHeap;                    // Handle auf privaten Heap
int         g_nAllocCounter;
COLORREF    g_rgbPickColor = RGB(255, 0, 0);
char        g_szBoxPath[_MAX_PATH] = "";

int         g_nUserMode = 0;              // Registryparameter
   // 0x00000001 : 3D-Einstellungen; Sichtbare Wände ändern
   // 0x00000002 : 3D-Einstellungen; Viewvolume einstellbar
   // 0x00000004 : 3D-Einstellungen; Gehäusesegmente um Theta drehbar
   // 0x00000008 : FilterFrequenzen; Übergangsfrequenz im FilterDialog einzeln einstellbar

static const char *ErrText[] =
{
   "Error in Module CARABOX.DLL",   // 0
   "Destruction without Heap !",    // 1
   "HeapFree()-Error",              // 2
   "Heap Destruction failure !",    // 3
   "Memory leaks detected !",       // 4
   "Undeleted Memory Object"        // 5
};

CCaraBoxView   g_CaraBoxView;
CCaraSdb       g_CaraSdb;
CEts3DGL       g_Ets3DGL;

#define REG_USERMODE      "UserMode"

/********************************************************************/
/*                                                                  */
/*  Beschreibung der Schnittstellenfunktionen                       */
/*                                                                  */
/*  - GetLastErrorCode():                                           */
/*       Funktion: gibt den Fehlercode zurück, wenn eine der anderen*/
/*                 Schnittstellenfunktionen mit 'false' antwortet.  */
/*       Parameter: keine                                           */
/*                                                                  */
/*  - BoxEdit():                                                    */
/*       Funktion: mittels einfacher Daten wird ein Boxenmodell     */
/*                 erstellt (Closed, BR oder TL) und der BDD-Daten- */
/*                 satz für CARA abgespeichert (Dir: ..\MyBox\..)   */
/*       Parameter: keine                                           */
/*                                                                  */
/********************************************************************/

// Funktionsprototypen
void   DoPropSheet(HWND);

extern "C"
{
   void Destructor( void *);
   int  GetLastErrorCode();                      // Funktion für Rückgabe des letzten Fehlercodes
   bool BoxEdit( HWND );                         // erstellt/ändert ein neues/bestehendes Boxenmodell
   void SetBoxParams(const char*, DWORD);
}

/********************************************************************/
/*                       DLL Main Funktion                          */
/********************************************************************/
bool   WINAPI DllMain(HINSTANCE hInstance,DWORD fReason, LPVOID /*pvReseved*/)
{
   switch (fReason)
   {
      case DLL_PROCESS_ATTACH:                   // Einbinden der DLL in den Adressraum des Prozesses
      {
         DisableThreadLibraryCalls( hInstance );
         g_bDoReport = 0;

         g_hInstance  = hInstance;               // Instance-Handle global speichern
/*
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
*/
      } break;

      case DLL_PROCESS_DETACH:                   // Die DLL wird freigegeben
         if (g_hMyHeap)   HeapDestroy(g_hMyHeap);
         g_hMyHeap = NULL;
#ifdef _DEBUG_HEAP
         HeapWatch();
#else
         if (g_nAllocCounter != 0)
         {
            MessageBox( (HWND) NULL, ErrText[4], ErrText[0], MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
         }
#endif
         break;
   }

   return true;
}

void Se_TranslatorFnc(unsigned int e, _EXCEPTION_POINTERS* /*p*/)
{
   switch (e)
   {
      case EXCEPTION_ACCESS_VIOLATION:      e = ERR_ACCESS_VIOLATION;      break;
      case EXCEPTION_DATATYPE_MISALIGNMENT: e = ERR_DATATYPE_MISALIGNMENT; break;  
      case EXCEPTION_PRIV_INSTRUCTION:      e = ERR_PRIV_INSTRUCTION;      break;  
      case EXCEPTION_IN_PAGE_ERROR:         e = ERR_IN_PAGE_ERROR;         break;  
      case EXCEPTION_STACK_OVERFLOW:        e = ERR_STACK_OVERFLOW;        break;  
      case EXCEPTION_INVALID_DISPOSITION:   e = ERR_INVALID_DISPOSITION;   break;  
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: e = ERR_ARRAY_BOUNDS_EXCEEDED; break;
      case EXCEPTION_FLT_DENORMAL_OPERAND:  e = MATHERR_DENORMAL_OPERAND;  break;
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:    e = MATHERR_SING;              break;
      case EXCEPTION_FLT_INEXACT_RESULT:    e = MATHERR_INEXACT_RESULT;    break;
      case EXCEPTION_FLT_INVALID_OPERATION: e = MATHERR_INVALID_OPERATION; break;
      case EXCEPTION_FLT_OVERFLOW:          e = MATHERR_OVERFLOW;          break;
      case EXCEPTION_FLT_STACK_CHECK:       e = MATHERR_STACK_CHECK;       break;
      case EXCEPTION_FLT_UNDERFLOW:         e = MATHERR_UNDERFLOW;         break;
      case EXCEPTION_INT_DIVIDE_BY_ZERO:    e = ERR_INT_DIVIDE_BY_ZERO;    break;
      case EXCEPTION_INT_OVERFLOW:          e = ERR_INT_OVERFLOW;          break;
      case EXCEPTION_INVALID_HANDLE:        e = ERR_INVALID_HANDLE;        break;
      default : e = 0;
         break;
   }
   if (e)
   {
      REPORT("Exception : 0x%x",e);
      throw (DWORD) e;
   }
}

/********************************************************************/
/*                                                                  */
/*   Funktion zur Erstellung des Property-Sheets zur Dateneingabe   */
/*                                                                  */
/********************************************************************/
void   DoPropSheet(HWND hwndParent)
{
   void *pOld = _set_se_translator(Se_TranslatorFnc);

   if (g_hMyHeap == NULL)
   {
      g_hMyHeap    = HeapCreate(0, 1048576, 0);             // priv. Heap anlegen
      g_nAllocCounter = 0;
      if (g_hMyHeap == NULL) return;
   }
   HKEY hKey = NULL;
   SetGlobalRegParam(CARABOX_REGKEY, NULL, NULL, false);
   if (OpenRegistry(g_hInstance, KEY_READ, &hKey, CARAWALK_GL_SETTINGS, &g_bDoReport))
   {
      g_nUserMode = GetRegDWord(hKey, REG_USERMODE, 0);
   }
   if (hKey) EtsRegCloseKey(hKey);

   CBoxPropertySheet     bps;
   CBoxDescriptionPage   BoDePa;
   CBoxBaseDataPage      BoBaDaPa;
   CBoxCrossOverPage     BoCrOvPa;
   CBoxChassisDataPage   BoChDaPa;
   CBoxCabinetDesignPage BoCaDePa;
   char szLocale[_MAX_PATH];

   ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, szLocale, _MAX_PATH);
   setlocale(LC_ALL, szLocale);

   if (::IsWindow(hwndParent)) 
   {
      LRESULT lResult = ::SendMessage(hwndParent, WM_GETPICKCOLOR, 0, 0);
      if (lResult != 0) g_rgbPickColor = lResult;
   }

   bps.SetNoOfPages(5);
   bps.SetCaption(IDS_PROPSHCAPTION);

   bps.SetPageTitle(0, MAKEINTRESOURCE(IDS_BOX_DESCRIPTION));
   bps.SetPageTemplate(0, (LPSTR) MAKEINTRESOURCE(IDD_BOX_DESCRIPTION));
   bps.SetPropertyPage(0, &BoDePa);
   bps.SetPageHelp(0, true);

   bps.SetPageTitle(1, MAKEINTRESOURCE(IDS_BOX_BASE_DATA));
   bps.SetPageTemplate(1, (LPSTR) MAKEINTRESOURCE(IDD_BOX_BASE_DATA));
   bps.SetPropertyPage(1, &BoBaDaPa);
   bps.SetPageHelp(1, true);

   bps.SetPageTitle(2, MAKEINTRESOURCE(IDS_BOX_CROSS_OVER));
   bps.SetPageTemplate(2, (LPSTR) MAKEINTRESOURCE(IDD_BOX_CROSS_OVER));
   bps.SetPropertyPage(2, &BoCrOvPa);
   bps.SetPageHelp(2, true);

   bps.SetPageTitle(3, MAKEINTRESOURCE(IDS_BOX_CHASSIS_DATA));
   bps.SetPageTemplate(3, (LPSTR) MAKEINTRESOURCE(IDD_BOX_CHASSIS));
   bps.SetPropertyPage(3, &BoChDaPa);
   bps.SetPageHelp(3, true);

   bps.SetPageTitle(4, MAKEINTRESOURCE(IDS_BOX_CABINET));
   bps.SetPageTemplate(4, (LPSTR) MAKEINTRESOURCE(IDD_BOX_CABINET));
   bps.SetPropertyPage(4, &BoCaDePa);
   bps.SetPageHelp(4, true);

   bps.SetInstanceHandle(g_hInstance);
   bps.SetStartPage(0);
   bps.SetApplyButton(true);
   bps.SetHelp(true);
   bps.SetWizard(true);
   bps.SetPageIcon((UINT)-1, MAKEINTRESOURCE(IDI_ICON));
   bps.SetPageIcon(1, MAKEINTRESOURCE(IDI_ICON));
//   bps.SetWizard(false);
//   bps.SetHeaderBmp(MAKEINTRESOURCE(IDB_BITMAP1));
//   bps.SetWatermarkBmp(MAKEINTRESOURCE(IDB_BITMAP1));
   bps.SetFinishBtn(true);
   bps.SetWizardContextHelp(true);
   bps.DoModal(hwndParent);

   _set_se_translator((_se_translator_function)pOld);
}

void Destructor( void * /*parameter*/)                // DLL-Destructor
{
   g_CaraBoxView.Destructor();
   g_CaraSdb.Destructor();
   g_Ets3DGL.Destructor();
   CEtsDialog::Destructor();
   ETSDEBUGEND
}

int   GetLastErrorCode()                         // Funktion für Rückgabe des letzten Fehlercodes
{
   return (g_nErrorCode);
}

/********************************************************************/
/*                                                                  */
/*              Schnittstellenfunktion und Hauptprogramm            */
/*                                                                  */
/********************************************************************/
bool   BoxEdit( HWND hWnd )
{
   DoPropSheet(hWnd);
   return (g_nErrorCode == 0) ? true :false;
}

void SetBoxParams(const char*pszBoxPath, DWORD dwColor)
{
   if (pszBoxPath) strncpy(g_szBoxPath, pszBoxPath, _MAX_PATH);
   g_rgbPickColor = dwColor;
}

bool IsBetween(double dMin, double dVal, double dMax)
{
   return ((dMin <= dVal) && (dVal < dMax)) ? true : false;
}


#ifdef _DEBUG_HEAP
struct DebugList
{
   struct DebugList *pNext;
   struct DebugList *pLast;
   void             *pMem;
   char              comment[1];
};

DebugList *g_pDebugList = NULL;

bool DebugInsert(DebugList *p)
{
   if (p == NULL) return false;
   p->pNext = g_pDebugList;
   if (p->pNext) p->pNext->pLast = p;
   g_pDebugList = p;
   g_pDebugList->pLast = NULL;
   return true;
}

DebugList * DebugFind(void *p, void*pNew)
{
   if (p            == NULL) return NULL;
   if (g_pDebugList == NULL) return NULL;

   DebugList *q = g_pDebugList;
   while ((q != NULL))
   {
      if (q->pMem == p)   // gefunden
      {
         if (pNew != NULL) q->pMem = pNew;
         return q;
      }
      q = q->pNext;
   }
   return NULL;
}

bool HeapWatch()
{
   if ((g_hMyHeap == 0) && (g_nAllocCounter > 0))
   {
      MessageBox((HWND) NULL, ErrText[4], ErrText[0], MB_OK|MB_ICONHAND|MB_SYSTEMMODAL);
   }
   if (g_hMyHeap)
   {
      if (g_pDebugList)
      {
         TRACE("%s\n", ErrText[4]);
         DebugList *p = g_pDebugList;
         while (p != NULL)
         {
            TRACE("%s %s \n", ErrText[5], p->comment);
            p = p->pNext;
         }
      }
      return true;
   }
   return false;
}

void* __cdecl Alloc(size_t s, void* pOld, char *pFile, int nLine)
{
   void   *pointer = NULL;
   if (pOld) pointer = HeapReAlloc(g_hMyHeap, HEAP_ZERO_MEMORY, pOld, s);
   else      pointer = HeapAlloc(  g_hMyHeap, HEAP_ZERO_MEMORY, s);
   if(pointer == NULL)
   {
      REPORT("Error : HeapAlloc");
      throw (DWORD) CARABOX_ALLOCATION_ERROR;
   }
   else if (pOld == NULL)  g_nAllocCounter++;

   if (pOld) DebugFind(pOld, pointer); // Replace changed pointer
   else
   {
      char text[256];
      wsprintf(text,"%s(%d) :", pFile, nLine);
      int nLen = sizeof(DebugList) + strlen(text)+1;
      DebugList* pDB = (DebugList*) HeapAlloc(  g_hMyHeap, 0, nLen);
      if (DebugInsert(pDB))
      {
         strcpy(pDB->comment, text);
         pDB->pMem = pointer;
      }
   }

   return (pointer);
}

void   __cdecl Free(void *p)
{
   DebugList* pL = DebugFind(p, NULL);
   if (pL)
   {
      if (pL->pLast != NULL) pL->pLast->pNext = pL->pNext;
      else
         g_pDebugList     = pL->pNext;
      if (pL->pNext != NULL) pL->pNext->pLast = pL->pLast;
      HeapFree(g_hMyHeap, 0, pL);

      if(HeapFree(g_hMyHeap, 0, p) != NULL )   g_nAllocCounter--;
      else
      {
         REPORT("Error : HeapFree");
         throw (DWORD) CARABOX_DESTRUCTION_ERROR;
      }
   }
   else REPORT("Other-Pointer");

   if(HeapFree(g_hMyHeap, 0, p) != NULL )   g_nAllocCounter--;
   else
   {
      REPORT("Error : HeapFree");
      throw (DWORD) CARABOX_DESTRUCTION_ERROR;
   }
}

void * __cdecl operator new(unsigned int s, char *pFile, int nLine)
{
   return Alloc(s, NULL, pFile, nLine);
}

#else

void* __cdecl Alloc(size_t s, void* pOld)
{
   void   *pointer = NULL;
   if (pOld) pointer = HeapReAlloc(g_hMyHeap, HEAP_ZERO_MEMORY, pOld, s);
   else      pointer = HeapAlloc(  g_hMyHeap, HEAP_ZERO_MEMORY, s);
   if(pointer == NULL)
   {
      REPORT("Error : HeapAlloc");
      throw (DWORD) CARABOX_ALLOCATION_ERROR;
   }
   else if (pOld == NULL)  g_nAllocCounter++;

   return (pointer);
}

void   __cdecl Free(void *p)
{
   if(HeapFree(g_hMyHeap, 0, p) != NULL )   g_nAllocCounter--;
   else
   {
      REPORT("Error : HeapFree");
      throw (DWORD) CARABOX_DESTRUCTION_ERROR;
   }
}

void * __cdecl operator new(unsigned int s)
{
   return Alloc(s, NULL);
}

#endif

void __cdecl operator delete(void *p)
{
   Free(p);
}

