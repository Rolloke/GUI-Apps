/********************************************************************/
/*                                                                  */
/*                        DLL CARAWALK                              */
/*                                                                  */
/*     Darstellung von Räumen und Lautsprechern in 3D               */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// CARAWALK.cpp : Definiert den Einsprungpunkt für die DLL-Anwendung.
//

#include "stdafx.h"

#include "Ets3dGL.h"
#include "CVector.h"
#include "CaraWalkDll.h"
#include "CCaraMat.h"
#include "resource.h"
#include "ETS3DGLRegKeys.h"
#include "EtsRegistry.h"
#include "locale.h"

#define INCLUDE_ETS_HELP
#include "CEtsDlg.h"

bool     g_bDoReport = REPORT_KEY_DEFAULT;
#define  ETS_DEBUGNAME "CARAWALK"
#define  ETSDEBUG_INCLUDE 
#define  ETSDEBUG_CREATEDATA ETS_DEBUGNAME, &g_bDoReport
#include "CEtsDebug.h"

// Definitionen für DoDataExchange
#define CARAWALK_GETHINSTANCE          1
#define CARAWALK_GETTOOLBAR_ID         2
#define CARAWALK_GETMENU_ID            3
#define CARAWALK_GETACCELERATOR_ID     4
#define CARAWALK_SETROOMDATA           5
#define CARAWALK_SETANGLE              6 
#define CARAWALK_SETBOXDATA            7 
#define CARAWALK_GETCOMMAND_IDS        8
#define CARAWALK_SET3DMODE             9

#define BOX_VIEW     "Boxview"
#define SCREEN_SAVER "ScreenSaver"

typedef CVector Vector;

HINSTANCE        g_hInstance = NULL;
HANDLE           g_hHeap     = NULL;
CCaraMat         g_CaraMat;
CEts3DGL         g_Ets3DGL;

class CCaraWalk
{
public:
   CCaraWalk() {m_pCaraWalk = NULL;};
   CCaraWalkDll *m_pCaraWalk;
};

extern "C" 
{
   void Destructor(    CCaraWalk*);
   HWND Create(        CCaraWalk*, LPCTSTR, DWORD, RECT *, HWND);
   bool DoDataExchange(CCaraWalk*, DWORD, int, void*);
   BOOL MoveWindowDll( CCaraWalk*, int, int, int, int, bool);
   void Delete(        CCaraWalkDll*);
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD fReason, LPVOID /* lpReserved */)
{
   switch (fReason)
   {
      case DLL_PROCESS_ATTACH:                   // Einbinden der DLL in den Adressraum des Prozesses
      {
         g_hInstance = hInstance;                // Instance-Handle global speichern
         DisableThreadLibraryCalls(hInstance);
         SetGlobalRegParam(ETS_DEBUGNAME, NULL, NULL, false);
      } break;
      case DLL_PROCESS_DETACH:
      {
         CCaraWalkDll::HeapDestroy();
      } break;
   }
   return TRUE;
}

void InitHeap()
{
   g_hHeap = HeapCreate(0, 1048576, 0);    // Heap mit Serialisierung anlegen !
//	if(g_hHeap==NULL) return FALSE;         // Diese DLL wurde nicht erfolgreich angelegt
   HKEY hKey = NULL;
   if (OpenRegistry(g_hInstance, KEY_READ, &hKey, NULL))
   {
      if (GetRegDWord(hKey, REPORT_KEY,0))
      {
         g_bDoReport = true;
         DeleteFile("CaraWalk.dbg");
      }
      EtsRegCloseKey(hKey);
   }
}
HWND Create(CCaraWalk* pC, LPCTSTR pszWndText, DWORD dwStyle, RECT *prcWnd, HWND hwndParent)
{
   if (g_hHeap == NULL) InitHeap();
   HWND hwnd = NULL;
   try
   {
      INITCOMMONCONTROLSEX InitCtrls = {sizeof(INITCOMMONCONTROLSEX), 0};
      InitCtrls.dwICC = ICC_WIN95_CLASSES;
      InitCommonControlsEx(&InitCtrls);

      char szLocale[_MAX_PATH];
      ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, szLocale, _MAX_PATH);
      setlocale(LC_ALL, szLocale);

      if (NULL == pC->m_pCaraWalk)
      {
         pC->m_pCaraWalk = new CCaraWalkDll;
      }
      if ((pszWndText != NULL) && strcmp(pszWndText, BOX_VIEW) == 0)
      {
         pC->m_pCaraWalk->m_nViewMode = BOXVIEW;
         pC->m_pCaraWalk->SetModes(ETS3D_PR_LOOK_AT, ETS3D_KEYBOARD_INPUT);
      }
      if ((pszWndText != NULL) && strcmp(pszWndText, SCREEN_SAVER) == 0)
      {
         pC->m_pCaraWalk->m_nViewMode = SCREENSAVER;
         pC->m_pCaraWalk->SetModes(0, ETS3D_PR_LOOK_AT);
//         pC->m_pCaraWalk->SetModes(ETS3D_PR_LOOK_AT, 0);
      }
      else
      {
         pC->m_pCaraWalk->SetModes(ETS3D_PR_LOOK_AT|ETS3D_KEYBOARD_INPUT, 0);
      }

      pC->m_pCaraWalk->InitParentMsgWnd(hwndParent);
      hwnd = pC->m_pCaraWalk->Create(pszWndText, dwStyle, prcWnd, hwndParent);
      SIZE szView ={prcWnd->right  - prcWnd->left, prcWnd->bottom - prcWnd->top};
      pC->m_pCaraWalk->SetSize(szView);
   }
   catch (DWORD dwError)
   {
      REPORT("Create Error %08x", dwError);
   }
   return hwnd;
}  

bool DoDataExchange(CCaraWalk*pC, DWORD dwType, int nSize, void* pParam)
{
   if (g_hHeap == NULL) InitHeap();
   switch (dwType)
   {
      case CARAWALK_GETHINSTANCE:
      if ((pParam!=NULL) && (nSize == sizeof(HINSTANCE)))
      {
         *((HINSTANCE*)pParam) = g_hInstance;
         return true;
      } break;
      case CARAWALK_GETTOOLBAR_ID:
      if ((pParam!=NULL) && (nSize == sizeof(int)))
      {
         *((int*)pParam) = IDR_ROOM_WALKING;
         return true;
      } break;
      case CARAWALK_GETMENU_ID:
      if ((pParam!=NULL) && (nSize == sizeof(int)))
      {
         *((int*)pParam)       = IDR_CARAWALK_MENU;
         return true;
      } break;
      case CARAWALK_GETACCELERATOR_ID:
      if ((pParam!=NULL) && (nSize == sizeof(int))) 
      {
         *((int*)pParam)       = IDR_ACCELERATOR;
         return true;
      } break;
      case CARAWALK_SETROOMDATA:
      if (pC->m_pCaraWalk)
      {
	      return pC->m_pCaraWalk->OnSetRoomData(pParam);
      } break;
      case CARAWALK_SETBOXDATA:
      if (pC->m_pCaraWalk)
      {
         return pC->m_pCaraWalk->OnSetBoxData((WPARAM) nSize, (char*) pParam);
      } break;
      case CARAWALK_SETANGLE:
      if (pC->m_pCaraWalk)
      {
      	pC->m_pCaraWalk->OnSetAngle(nSize, (CARAWALK_BoxPos*) pParam);
      } break;
      case CARAWALK_GETCOMMAND_IDS:
      {
         int pnCmdIDs[]=
         {
            IDM_WALK_FORWARD,
            IDM_WALK_BACKWARD,
            IDM_WALK_TURNRIGHT,
            IDM_WALK_TURNLEFT,
            IDM_WALK_LOOKUP,
            IDM_WALK_LOOKDOWN,
            IDM_WALK_GEAR_LEFT,
            IDM_WALK_GEAR_RIGHT,
            IDM_WALK_BACKWARD_CTRL,
            IDM_WALK_TURNLEFT_CTRL,
            IDM_WALK_LOOKDOWN_CTRL,
            IDM_WALK_LOOKUP_CTRL,
            IDM_WALK_TURNRIGHT_CTRL,
            IDM_WALK_FORWARD_CTRL,
            IDM_WALK_GEAR_LEFT_CTRL,
            IDM_WALK_GEAR_RIGHT_CTRL,
            IDM_VIEW_TEXTURE,
            IDM_VIEW_LIGHT,
            IDM_VIEW_BOXES,
            IDM_VIEW_DETAIL,
            IDM_VIEW_DETAIL_VERYLOW,
            IDM_VIEW_DETAIL_LOW,
            IDM_VIEW_DETAIL_MEAN,
            IDM_VIEW_DETAIL_HIGH,
            IDM_VIEW_DETAIL_VERYHIGH,
            IDM_OPTIONS_LIGHT,
            IDM_OPTIONS_BOX,
            IDM_OPTIONS_GHOSTMODE,
            IDM_OPTIONS_FLYMODE
         };
         if (nSize == sizeof(int))
         {
            *((int*)pParam) = sizeof(pnCmdIDs);
            return true;
         }
         if ((pParam) && (nSize == sizeof(pnCmdIDs)))
         {
            memcpy(pParam, pnCmdIDs, sizeof(pnCmdIDs));
            return true;
         }

      } break;
      case CARAWALK_SET3DMODE:
      if ((pParam!=NULL) && (nSize == (sizeof(DWORD)*2)))
      {
         try
         {
            if (NULL == pC->m_pCaraWalk)
            {
               pC->m_pCaraWalk = new CCaraWalkDll;
            }
            DWORD *pdwMode = (DWORD*)pParam;
            pC->m_pCaraWalk->SetModes(pdwMode[0], pdwMode[1]);
         }
         catch(DWORD dwError)
         {
            REPORT("Error %08x", dwError);
         }
      } break;
   }
   return false;
}

BOOL MoveWindowDll(CCaraWalk*pC, int nLeft, int nTop, int nWidth, int nHeight, bool bRedraw)
{
   if (pC->m_pCaraWalk)
   {
      return pC->m_pCaraWalk->MoveWindow(nLeft, nTop, nWidth, nHeight, bRedraw);
   }
   return false;
}

void Delete(CCaraWalkDll *pCCaraWalkDll)
{
   if (pCCaraWalkDll) delete pCCaraWalkDll;
}

void Destructor(CCaraWalk*pC)
{
   if ((pC != NULL) && (pC->m_pCaraWalk))
   {
      delete pC->m_pCaraWalk;
      pC->m_pCaraWalk = NULL;
   }
   g_CaraMat.Destructor();
   g_Ets3DGL.Destructor();
   CEtsDialog::Destructor();
   ETSDEBUGEND;
}

