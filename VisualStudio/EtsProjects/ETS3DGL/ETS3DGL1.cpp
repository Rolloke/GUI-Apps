// Cara3D1.cpp: Implementierung der Klasse CEts3DGLDll.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ETS3DGL.h"
#include "DibSection.h"
#include "resource.h"
#include "CustErr.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

extern HINSTANCE g_hInstance;
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
bool CEts3DGLDll::gm_bBugFixMoveWindow = BUGFIX_MOVEWINDOW_DEFAULT;
bool CEts3DGLDll::gm_bBugFixBufferSize = BUGFIX_BUFFERSIZE_DEFAULT;
bool CEts3DGLDll::gm_bNoGraphic        = false;
int  CEts3DGLDll::gm_nBugFixBuffer     = BUGFIX_BUFFER_DEFAULT;
ATOM CEts3DGLDll::gm_ETS3D_Class       = 0;
ATOM CEts3DGLDll::gm_ETS3DParentClass  = 0;
HINSTANCE CEts3DGLDll::gm_hInstance    = NULL;
bool CEts3DGLDll::gm_bInitRegistryValues = true;

#ifdef _DEBUG
   LPDIRECTINPUT       CEts3DGLDll::gm_pdi       = NULL;
   LPDIRECTINPUTDEVICE CEts3DGLDll::gm_pKeyboard = NULL;
#endif

LRESULT CALLBACK CEts3DGLDll::WindowFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   // Dise ist die aktuelle !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   PropertyWindowFunc *pnWF = (PropertyWindowFunc*) GetProp(hwnd, OLD_WINDOW_PROC);
   if (pnWF)
   {
      switch (message)
      {
         case WM_PAINT:
         {
            REPORT("WM_PAINT");
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            ASSERT(pnWF->p3DGL != NULL);
            pnWF->p3DGL->ClassWindowFunc(hwnd, message, (WPARAM)hdc, (LPARAM)&ps);
            EndPaint(hwnd, &ps);
         } return 0;
         case WM_ERASEBKGND:
         {
            REPORT("WM_ERASEBKGND");
            ASSERT(pnWF->p3DGL != NULL);
            return pnWF->p3DGL->ClassWindowFunc(hwnd, message, wParam, lParam);
         } break;
         case WM_THREAD_SIZE:
         REPORT("WM_THREAD_SIZE %d", wParam);
         switch (wParam)
         {
            case 0:
               InvalidateRect(hwnd, NULL, true);
               UpdateWindow(hwnd);
               break;
         } return 0;
         case WM_REMOVE_PROPERTY:
         if (wParam)
         {
            CEts3DGLDll *p3DGL = (CEts3DGLDll*) wParam;
            if (!p3DGL->DestroyAtDllTermination())
               p3DGL->OnDestroy();
            ::RemoveProp(hwnd, OLD_WINDOW_PROC);
            if (pnWF->pfnWindowFunc) ::SetWindowLong(hwnd, GWL_WNDPROC, pnWF->pfnWindowFunc);
            else                     ::DestroyWindow(hwnd);
            free(pnWF);
            return 1;
         }
         return 0;
         case WM_DESTROY:
            ASSERT(pnWF->p3DGL != NULL);
            pnWF->p3DGL->ClassWindowFunc(hwnd, message, wParam, lParam);
            ::RemoveProp(hwnd, OLD_WINDOW_PROC);
            if (pnWF->pfnWindowFunc)
            {
               CallWindowProc((WNDPROC)pnWF->pfnWindowFunc, hwnd, message, wParam, lParam);
               ::SetWindowLong(hwnd, GWL_WNDPROC, pnWF->pfnWindowFunc);
            }
            free(pnWF);
            return 0;
            break;
         case WM_SETFOCUS:
            InvalidateRect(hwnd, NULL, true);
            return 0;
         case WM_CHANGE_3DMODES:                                  // ChangeModes
         if (lParam)
         {
            CEts3DGLDll *p3DGL = (CEts3DGLDll*) lParam;
            p3DGL->ChangeModes(wParam);
         } break;
         case WM_ENTERMENULOOP:
            pnWF->p3DGL->m_nStates |= IS_INSIDE_MENU_LOOP;
            break;
         case WM_EXITMENULOOP:
            pnWF->p3DGL->m_nStates &= ~IS_INSIDE_MENU_LOOP;
            break;
         default:
         {
            LRESULT lResult = 0;
            ASSERT(pnWF->p3DGL != NULL);
            lResult = pnWF->p3DGL->ClassWindowFunc(hwnd, message, wParam, lParam);
            if (lResult) return 0;
         } break;
      }
      
      if (pnWF->pfnWindowFunc)
      {
         return CallWindowProc((WNDPROC)pnWF->pfnWindowFunc, hwnd, message, wParam, lParam);
      }
      else
         return ::DefWindowProc(hwnd, message, wParam, lParam);
   }
   else
   {
      if (message == WM_CREATE)
      {
         CREATESTRUCT*pCS = (CREATESTRUCT*) lParam;
         PropertyWindowFunc *pnWF = (PropertyWindowFunc*) GetProp(hwnd, OLD_WINDOW_PROC);
         if (!pnWF)
         {
            pnWF = (PropertyWindowFunc*) malloc(sizeof(PropertyWindowFunc));
            if (!pnWF) return -1;
            pnWF->pfnWindowFunc = 0;
            pnWF->p3DGL = (CEts3DGLDll*) pCS->lpCreateParams;
            ::SetProp(hwnd, OLD_WINDOW_PROC, (HANDLE)pnWF);
         }
         return 0;
      }
      else return ::DefWindowProc(hwnd, message, wParam, lParam);
   }
   return 0;
}

LRESULT CEts3DGLDll::ClassWindowFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
      case WM_ERASEBKGND:
      {
         if (IsInitialized())
         {
            if (::IsWindowVisible(m_hWndThread) || (m_nModes & ETS3D_DC_DIB))
               return 1;
         }
         HWND hwnd = GetParent();
         if (hwnd)
         {
            RECT rc;
            ::GetClientRect(hwnd, &rc);
            HBRUSH hBr = CreateSolidBrush(m_cBkGnd);
            if (hBr)
            {
               int nResult = ::FillRect((HDC) wParam, &rc, hBr);
               DeleteObject(hBr);
               return nResult;
            }
         }
      } break;
      case WM_PAINT:
      {
         OnPaint(*((PAINTSTRUCT*)lParam));
      } break;
      case WM_MOVE:
      if (m_nThreadID)
      {
         PostThreadMessage(WM_THREAD_SIZE, 0, 0);
         return 0;
      } break;
      case WM_SIZE:
      if (m_nThreadID)
      {
         if ((wParam == SIZE_MAXIMIZED)||(wParam == SIZE_RESTORED))
         {
            PostThreadMessage(WM_THREAD_SIZE, 0, 0);
            if (wParam == SIZE_MAXIMIZED)
            {
               PostThreadMessage(WM_SHOW_THREAD_WND, 0, 0);
            }
         }
         return 0;
      } break;
      case WM_TIMER:
      {
         BOOL bValidate = 
            ((wParam==(WPARAM)m_nRenderTimer) &&               // ist es der richtige Event
            !IsRendering()   &&                                // wird gerade gerendert ?
            !IsSuspended()   &&                                // läuft der Thread nicht ?
            !DoNotValidate());                                 // nicht validieren ?
         OnTimer((long)wParam, bValidate);                     // Aufruf der virtuellen Funktion der Klasse
         if (bValidate)                                        // Aufruf der Dll-Funktion
            OnTimerDll((long)wParam, (TIMERPROC*) lParam);
      } break;
      case WM_ENTERSIZEMOVE:
      if (m_nThreadID)
      {
         ASSERT(::GetCurrentThreadId() != m_nThreadID);
         PostThreadMessage(WM_HIDE_THREAD_WND, 0, 0);
      } break;
      case WM_EXITSIZEMOVE:
      if (m_nThreadID)
      {
         ASSERT(::GetCurrentThreadId() != m_nThreadID);
         if (gm_bBugFixMoveWindow)                             // Bugfix für Matrox G400 ICD: MoveWindow + Größe ändern, damit der Buffer neu angelegt wird
         {
            SIZE szView = m_szView;
            ::SendMessage(m_hWndThread, WM_SIZE, SIZE_RESTORED, MAKELPARAM(szView.cx-1, szView.cy-1));
            ::SendMessage(m_hWndThread, WM_SIZE, SIZE_RESTORED, MAKELPARAM(szView.cx  , szView.cy  ));
         }
         PostThreadMessage(WM_SHOW_THREAD_WND, 0, 0);
      } break;
      case WM_COMMAND:
      {
#ifdef _MULTI_3DWINDOWS
         LONG lMEInfo = ::GetMessageExtraInfo();
         if ((lMEInfo!= NULL) && (lMEInfo != (LONG)this)) break;
#endif
/*
         if (LOWORD(wParam) == ETS3D_CD_INITIALIZED)
         {
            DWORD idAttach   = ::GetCurrentThreadId();
            DWORD idAttachTo = m_nThreadID;
            AttachThreadInput(idAttach, idAttachTo, true);
         }
*/
         long lResult = OnCommandDll(LOWORD(wParam), HIWORD(wParam), (HWND) lParam);
         if (lResult != 0) lResult = OnCommand(LOWORD(wParam), HIWORD(wParam), (HWND) lParam);
         if (lResult == 0)
            return 1;
      } break;
      case WM_COPY_FRAME_BUFFER:                               // OnCopyFrameBuffer
      {
#ifdef _MULTI_3DWINDOWS
         if (::GetMessageExtraInfo() != (LONG)this) break;
#endif
         int nCount = 0;
         while (m_nStates & COPY_FRAME_BMP_NOW)
         {
            Sleep(10);
            if (nCount++ > 1000) return 0;
         }

         if (::GetCurrentThreadId() == m_nThreadID)
         {
            OnCopyFrameBuffer(wParam, lParam);
         }
         else if (m_nThreadID)
         {
//            SendMessage(m_hWndThread, WM_RENDER_SCENE, 0, 0);
            SendMessage(m_hWndThread, message, wParam, lParam);
            while (m_nStates & COPY_FRAME_BMP_NOW)
            {
               Sleep(10);
               if (nCount++ > 1000) return 0;
            }
         }
      } break;
      case WM_DESTROY:
         return OnDestroy();
      default:
      {
//         REPORT("OnMessage(%x)", message);
         if (OnMessage(message, wParam, lParam))
            return 1;
      } break;
   }
   return 0;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
LRESULT CEts3DGLDll::OnDestroy()
{
   BEGIN("OnDestroy");

   if (DestroyAtDllTermination())
   {
      DetachThread();
   }
   else
   {
      KillRenderThread();
   }

   if (m_nStates & CS_DIB_INITIALIZED)
   {
      m_nStates &= ~CS_DIB_INITIALIZED;
      DeleteCriticalSection(&m_csDisplay);
   }

   if ((m_pcsExt != NULL) && ((m_nStates & CS_EXT_INITIALIZED) !=0))
   {
      if (m_nStates & CRITICAL_SECTION_ATTACHED)
      {
         REPORT("Attached external CriticalSection !");
      }
      else
      {
         m_nStates &= ~CS_EXT_INITIALIZED;
         DeleteCriticalSection(m_pcsExt);
         delete m_pcsExt;
         m_pcsExt = NULL;
      }
   }

   return 0;
}

/******************************************************************************
* Name       : OnPaint                                                        *
* Zweck      : Ausgabe des Bildschirminhaltes. Ist OpenGL initialisiert und   *
*              der Modus ETS3D_DC_DIB, so wird der Inhalt der DibSection aus- *
*              gegeben. Ist der Modus ETS3D_DC_DIRECT, so wird das Fenster nur*
*              Invalidiert bzw. wenn der Ausgabethread angehalten wurde wird  *
*              die Hintergrundfarbe ausgegeben.                               *
* Aufruf     : Durch die WindowMessage WM_PAINT.                              *
* Parameter  : keine                                                          *
* Funktionswert : 0 wenn durchgeführt                             (LRESULT)   *
******************************************************************************/
LRESULT CEts3DGLDll::OnPaint(PAINTSTRUCT &ps)
{
   BEGIN("CEts3DGLDll::OnPaint");
   if (!IsInitialized()) return 0;

   if ((m_nModes & ETS3D_DC_DIB) && (m_pDib != NULL))
   {
      CAutoCriticalSection(&this->m_csDisplay);
      HPALETTE hPal    = m_pDib->GetPaletteHandle();
      HPALETTE hOldPal = NULL;
      if (hPal)
      {
         REPORT("SelectPalette");
         hOldPal = SelectPalette(ps.hdc, hPal, false);
         RealizePalette(ps.hdc);
      }

      RECT rcView;
      ::GetClientRect(GetParent(), &rcView);
      m_pDib->SetDIBitsToDevice(ps.hdc, rcView, (m_nModes & ETS3D_DC_DIB_2) ? true : false);

      if (hOldPal)
      {
         SelectPalette(ps.hdc, hOldPal, false);
         RealizePalette(ps.hdc);
      }
   }
   else if (m_nModes & ETS3D_DC_DIRECT)
   {
      m_nStates |= INVALID_VIEW;
   }
   return 0;
}

// Timer und Rendersteuerung
/******************************************************************************
* Name       : OnTimerDll                                                     *
* Zweck      : Steuerung des OpenGL-Threads, Aufrufen der Validierungs- und   *
*              Renderfunktionen.                                              *
* Aufruf     : Durch die Window-Message WM_TIMER.                             *
* Parameter  :                                                                *
* Funktionswert : 0 wenn durchgeführt                             (LRESULT)   *
******************************************************************************/
LRESULT CEts3DGLDll::OnTimerDll(long nTimerID, TIMERPROC *pfnTimer)
{
   BYTE bKeys[256];
   if ((m_nModes & ETS3D_KEYBOARD_INPUT) && !(m_nStates & IS_INSIDE_MENU_LOOP) &&
       (::GetFocus() == (HWND)::GetWindowLong(m_hWndThread, GWL_PARENTWINDOW)) && 
       ::GetKeyboardState(bKeys))
   {
      int i;
      LPARAM lParam = 0;
      if (bKeys[17] & 0x80) lParam |= MK_CONTROL, bKeys[17] &= ~0x80;// Ctrl
      if (bKeys[16] & 0x80) lParam |= MK_SHIFT  , bKeys[16] &= ~0x80;// Shift
      if (bKeys[18] & 0x80) lParam |= MK_LBUTTON, bKeys[18] &= ~0x80;// Alt
      if (bKeys[32] & 0x80) lParam |= MK_RBUTTON;                    // Space
      for (i=0; i<256; i++) 
      {
         if (bKeys[i] & 0x80)
         {
            OnMessage(WM_CHAR, (WPARAM)i, lParam);
         }
      }
   }
   if (m_nStates & CLEAR_INVALID)                              // ist ein Invalid Flag gesetzt ?
   {
//    BEGIN("OnTimer");                                        // Dann die TimerFunktion anzeigen
      ASSERT(::GetCurrentThreadId() != m_nThreadID);
      m_nStates |=  RENDERING_NOW;                             // Renderflag setzen
      if (m_nStates & INVALID_SIZE)    {PostThreadMessage(WM_SET_VIEWPORT     , 0, 0); /*REPORT("Invalid Size");    */}
      if (m_nStates & INVALID_VOLUME)  {PostThreadMessage(WM_SET_VIEWVOLUME   , 0, 0); /*REPORT("Invalid Volume");  */}
      if (m_nStates & INVALID_COMMANDS){PostThreadMessage(WM_VALIDATE_COMMANDS, 0, 0); /*REPORT("Invalid Commands");*/}
      if (m_nStates & INVALID_LISTS)   {PostThreadMessage(WM_VALIDATE_LISTS   , 0, 0); /*REPORT("Invalid Lists");   */}
      m_nStates &= ~CLEAR_INVALID;
      PostThreadMessage(WM_RENDER_SCENE, 0, 0);
   }

   if (m_nStates & TIME_CHANGED)
   {
      if (m_nRenderTimer) KillTimer(m_nRenderTimer);          // Timer beenden
      m_nRenderTimer = ::SetTimer(GetParent(), CONTROL_TIMER_EVENT, m_nTime, NULL);
      m_nStates &= ~TIME_CHANGED;
   }
   return 0;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Erstellung der verketteten Listen für die OpenGL-Listen
bool CEts3DGLDll::CreateRenderListDll(int nID, LISTFUNCTION pfn, int nRange, bool bCommand)
{
   BEGIN("CreateRenderList");
   RenderList List;
   ZeroMemory(&List, sizeof(RenderList));
   try
   {
      if (bCommand)
      {
         m_pCommandList = CreateRenderListR(m_pCommandList, nID, pfn, nRange);
         m_nStates |= INVALID_COMMANDS;
      }
      else
      {
         m_pRenderList = CreateRenderListR(m_pRenderList, nID, pfn, nRange);
         m_nStates |= INVALID_LISTS;
      }
   }
   catch(int nError)
   {
      REPORT("found equal ID´s: %d", nError);
      return false;
   }
   return true;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Rekursive Funktion zur Erstellung der Listen
RenderList* CEts3DGLDll::CreateRenderListR(RenderList *pList, int nID, LISTFUNCTION pfn, int nRange)
{
   if (pList == NULL)                                          // Anfang der Liste erzeugen
   {
      if (nRange == 0) throw (int)nID;
      pList = new RenderList;
      pList->bRender   = true;
      pList->bInvalid  = true;
      pList->nListID   = nID;
      pList->nRange    = nRange;
      pList->pfnRender = pfn;
      pList->pfnDirect = NULL;
      pList->pNext     = NULL;
   }
   else if (nID == pList->nListID)                             // Liste muß identifizierbar sein
   {
      if (nRange == 0)
      {
         pList->pfnDirect = pfn;
      }
      else throw (int)nID;
   }
   else
   {
      pList->pNext = CreateRenderListR(pList->pNext, nID, pfn, nRange);
   }
   return pList;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Setzen der Zustände der Listen
bool CEts3DGLDll::SetListDll(int nID, UINT nParam)
{
   RenderList List;
   ZeroMemory(&List, sizeof(RenderList));
   bool bReturn = false;
   if (nParam & 0x0000000f & INVALID_LIST)
   {
      if (m_pRenderList) bReturn = SetListR(m_pRenderList, nID, nParam);
      if (bReturn) m_nStates |= INVALID_LISTS;
   }
   else if (nParam & 0x0000000f & INVALID_COMMAND)
   {
      if (m_pCommandList) bReturn = SetListR(m_pCommandList, nID, nParam);
      if (bReturn) m_nStates |= INVALID_COMMANDS;
   }
   return bReturn;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Rekursive Funktion zur Suche der Liste mit der gegebenen ID
bool CEts3DGLDll::SetListR(RenderList *pList, int nID, UINT nParam)
{
   if (pList->nListID == nID)
   {
      switch (nParam&0x00f0)
      {
         case INVALID_COMMAND    &0x00f0:
         case INVALID_LIST       &0x00f0: pList->bInvalid = true;  break;
         case RENDER_LIST        &0x00f0:
         case RENDER_COMMAND     &0x00f0: pList->bRender  = true;  break;
         case DONT_RENDER_LIST   &0x00f0:    
         case DONT_RENDER_COMMAND&0x00f0: pList->bRender  = false; break;
      }
      return true;
   }
   if (pList->pNext) return SetListR(pList->pNext, nID, nParam);
   return false;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Beenden des Timers und löschen sämtlicher noch anstehender Timernachrichten
BOOL CEts3DGLDll::KillTimer(UINT& nTimer)
{
//   BEGIN("KillTimer");
   BOOL bKill = false;
   HWND hwndParent = GetParent();
   if (hwndParent)
   {
	   MSG msg;
      bKill = ::KillTimer(hwndParent, nTimer);
      nTimer = 0;
      _asm CLD;
      ::PeekMessage(&msg, hwndParent, WM_TIMER, WM_TIMER, PM_REMOVE|PM_NOYIELD);
   }
   return bKill;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Transformieren der Koordinaten durch Aufruf der Threadfunktion und Warten auf Beendigung dieser
void CEts3DGLDll::TransformPointsDll(CVector *pV, int nPoints, bool bDir)
{
   if (::GetCurrentThreadId() == m_nThreadID)
   {
      OnTransformPoints(nPoints|((bDir)?0x80000000 : 0), pV);
   }
   else
   {
      ::SendMessage(m_hWndThread, WM_TRANSFORMPOINTS, nPoints|((bDir)?0x80000000 : 0), (LPARAM)pV);
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Starten des Renderthreads und Warten auf die Initialisierung
bool CEts3DGLDll::StartRenderThread(HWND hwndParent)
{
   BEGIN("StartRenderThread");
   try
   {
      m_hEvent  = ::CreateEvent(NULL, true, false, NULL);
      if (!m_hEvent)                                  throw (int)1;
      unsigned int nThreadId;
      long plThreadParms[2] = {(long)this, (long)hwndParent};  // Diese Parameter können hier lokal angelegt werden,
                                                               // da der Hauptthread auf die Initialisierung der
                                                               // Threadparameter mit Hilfe eines Events wartet !!!
      m_hThread = (HANDLE) _beginthreadex(NULL, 0, OpenGLThread, plThreadParms, 0, &nThreadId);
      if (m_hThread == 0)                             throw (int)2;

      DWORD dwResult = WaitForThreadEvent();
      if (dwResult == WAIT_TIMEOUT)                   throw (int)3;
      if ((m_nStates & OPEN_GL_INITIALIZED) == 0)     throw (int)4;
      PostOpenGLState(1, 0);                                   // OpenGL Initialisiert
   }
   catch (int nError)
   {
      switch (nError)
      {
         case 1: REPORT("No Syncronise Event Created");       SetLastError(ETS3DERR_NOSYNCEVEVT); break;
         case 2: REPORT("No Thread Created");                 SetLastError(ETS3DERR_NOTHREAD);    break;
         case 3: REPORT("Timeout Error initialising OpenGL"); SetLastError(ETS3DERR_TIMEOUT);     break;
         case 4: REPORT("Error initialising OpenGL");         break;
      }
      KillRenderThread();
      return false;
   }

   return true;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGLDll::KillRenderThread()
{
   BEGIN("KillRenderThread");

   if (m_nRenderTimer) KillTimer(m_nRenderTimer);// Timer beenden

   if ((m_hThread != NULL) && (m_nThreadID != 0))// Thread noch vorhanden ?
   {
      DWORD dwExitCode;
      BOOL  bPosted  = false;                    // noch kein WM_QUIT an den Thread übergeben !
      int   nTimeout = 0;
      
      while(GetExitCodeThread(m_hThread, &dwExitCode) && (dwExitCode == STILL_ACTIVE))
      {
         nTimeout++;

         if(!bPosted)                            // noch keine WM_QUIT Nachricht gesendet
         {                                       // Dann versuche diese Nachricht zu senden
            if (m_hWndThread && ::IsWindow(m_hWndThread))
            {
               bPosted = PostThreadMessage(WM_DESTROY, 0, 0);
               if (bPosted) m_nStates |= NO_WM_DESTROY;
            }
            else 
               bPosted = PostThreadMessage(WM_QUIT, 0, 0);
            if (bPosted)
            {
               REPORT("WM_QUIT or WM_DESTROY message posted");
            }
            else
            {                                    // THREAD_ID nicht OK oder keine message queue vorhanden
               if(GetLastError()==ERROR_INVALID_THREAD_ID)
               { 
                  if(nTimeout > 10)              
                  {                              // Thread auf diese Art einfach abwürgen !
                     TerminateThread(m_hThread,0xffffffff);
                     REPORT("Do hard thread termination");
                  }
               }
            }
         }

         Sleep(20);                              // Jetzt diesen Thread etwas lahmlegen
      }
      if(nTimeout) REPORT("Wait counter: %d",nTimeout);
   }

   if (m_hEvent ) CloseHandle(m_hEvent);
   if (m_hThread) CloseHandle(m_hThread);

   m_hThread     = NULL;
   m_hEvent      = NULL;
   m_nThreadID   = 0;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
DWORD CEts3DGLDll::WaitForThreadEvent()
{
   DWORD dwResult;
   static const char szHeading[] = "Thread Event Error !";
   static const char szTimeout[] = "Timeout waiting for thread event.\nWait annother 20 seconds ?";
   static const char szFailed[]  = "Wait failed.";
   do
   {
      dwResult = ::WaitForSingleObject(m_hEvent, 20000);
      if (dwResult == WAIT_TIMEOUT)
      {
         if (::MessageBox(NULL, szTimeout, szHeading, MB_YESNO|MB_TASKMODAL|MB_ICONSTOP)==IDNO)
            break;
      }
      else if (dwResult == WAIT_FAILED)
      {
         ::MessageBox(NULL, szFailed, szHeading, MB_YESNO|MB_TASKMODAL|MB_ICONSTOP);
         break;
      }
      else if (dwResult == WAIT_OBJECT_0)
      {
         break;
      }
   } while (dwResult);
   return dwResult;
}

/******************************************************************************
* Name       : OnCommandDll                                                   *
* Zweck      : Auswerten der Commands für die automatische Ablaufsteuerung bei*
*              der Ausführung des Renderthreads im Hintergrund.               *
* Aufruf     : Durch die Window-Message WM_COMMAND.                           *
* Parameter  :                                                                *
* wID      (E): Command ID.                                          (WORD)   *
* wNotify  (E): Notify-Code.                                         (WORD)   *
* hwndFrom (E): Fenster, das diese Nachricht gesendet hat.           (HWND)   *
* Funktionswert : 1, wenn die Nachricht an die virtuelle Funktion    (LRESULT)*
*                 OnCommand der Klasse CEts3DGLDll weitergegeben werden soll,    *
*                 sonst 0.                                                    *
******************************************************************************/
LRESULT CEts3DGLDll::OnCommandDll(WORD wID, WORD wNotify, HWND hwndFrom)
{
   UINT nColorM = 0;
   UINT nRender = 0;
   switch(wID)
   {
      case ETS3D_CD_INITIALIZED:                               // initialisiert
      if (wNotify)
      {
         REPORT("OpenGL-Initialized");
      }
      else
      {
         REPORT("OpenGL-Init failed");
         if (m_nModes & ETS3D_AUTO_DELETE)                     // automatisch löschen ?
            PostMessage(m_hWndThread, WM_CLOSE, 0, 0);
      } return 1;    // Nachricht weiterleiten
   }
   return 1;         // Nachricht weiterleiten
}

/******************************************************************************
* Name       : ChangeModes                                                    *
* Zweck      : Ändern der Einstellungen für den Rendercontext                 *
* Aufruf     : Durch die WM_USER-Message WM_CHANGE_3DMODES.                   *
* Parameter  :                                                                *
* nNewMode (E): Neue Einstellungs-Flags                               (DWORD) *
* Funktionswert :keiner                                                       *
******************************************************************************/
void CEts3DGLDll::ChangeModes(DWORD nNewMode)
{
   bool bChange = true;

   if      ((nNewMode & ETS3D_DC_DIRECT) && (m_nModes & ETS3D_DC_DIRECT)) bChange = false;
   else if ((nNewMode & ETS3D_DC_DIRECT) && (m_nModes & ETS3D_DC_DIB))
   {
      int npf    = GetPixelFormat(m_hDC);
      int nNewpf = SetDCPixelFormat(m_hDC, nNewMode, false);
      if ((npf != 0) && (npf != nNewpf)) bChange = false;
   }

   if (bChange)
   {
      bool bDontWait = (m_nModes & ETS3D_DONT_WAIT) ? true :false;
      m_nModes &= ~ETS3D_DONT_WAIT;
      HWND hwndParent = GetParent();
      KillRenderThread();
      m_nModes  = nNewMode;
      m_nStates = INVALID_VOLUME|INVALID_SIZE|INVALID_BKGND;
      StartRenderThread(hwndParent);
      if (bDontWait) m_nModes |= ETS3D_DONT_WAIT;
   }
   else ErrorMessage("Could not change Pixelformat !");
}


/*
#ifdef _MULTI_3DWINDOWS
LRESULT CALLBACK CEts3DGLDll::WindowFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   PropertyWindowFunc *pnWF = (PropertyWindowFunc*) GetProp(hwnd, OLD_WINDOW_PROC);
   int  i;

   if (pnWF)
   {
      switch (message)
      {
         case WM_PAINT:
         {
            REPORT("WM_PAINT");
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            for (i=0; i<pnWF->nCount; i++)
            {
               ASSERT(pnWF->p3DGL[i] != NULL);
               pnWF->p3DGL[i]->ClassWindowFunc(hwnd, message, (WPARAM)hdc, (LPARAM)&ps);
            }
            EndPaint(hwnd, &ps);
         } return 0;
         case WM_ERASEBKGND:
         {
            REPORT("WM_ERASEBKGND");
            LRESULT lResult = 0;
            for (i=0; i<pnWF->nCount; i++)
            {
               ASSERT(pnWF->p3DGL[i] != NULL);
               lResult |= pnWF->p3DGL[i]->ClassWindowFunc(hwnd, message, wParam, lParam);
            }
            return lResult;
         } break;
         case WM_THREAD_SIZE:
         REPORT("WM_THREAD_SIZE %d", wParam);
         switch (wParam)
         {
            case 0:
            InvalidateRect(hwnd, NULL, true);
            UpdateWindow(hwnd);
            break;
         } return 0;
         case WM_REMOVE_PROPERTY:
         if (wParam)
         {
            CEts3DGLDll *p3DGL = (CEts3DGLDll*) wParam;
            p3DGL->OnDestroy();
            if(--pnWF->nCount == 0)
            {
               ::RemoveProp(hwnd, OLD_WINDOW_PROC);
               if (pnWF->pfnWindowFunc)
                  ::SetWindowLong(hwnd, GWL_WNDPROC, pnWF->pfnWindowFunc);
               free(pnWF);
               return 1;
            }
            else
            {
               for (i=0; i<pnWF->nCount; i++)
               {
                  ASSERT(pnWF->p3DGL[i] != NULL);
                  if (pnWF->p3DGL[i] == p3DGL)
                  {
                     pnWF->p3DGL[i] = pnWF->p3DGL[pnWF->nCount];
                  }
               }
            }
         }
         return 0;
         case WM_DESTROY:
            for (i=0; i<pnWF->nCount; i++)
            {
               pnWF->p3DGL[i]->ClassWindowFunc(hwnd, message, wParam, lParam);
            }
            if (pnWF->pfnWindowFunc == 0)
               PostQuitMessage(0);
            break;
         case WM_SETFOCUS:
            InvalidateRect(hwnd, NULL, true);
            return 0;
         case WM_CHANGE_3DMODES:                                  // ChangeModes
         if (lParam)
         {
            CEts3DGLDll *p3DGL = (CEts3DGLDll*) lParam;
            p3DGL->ChangeModes(wParam);
         } break;
         default:
         {
            LRESULT lResult = 0;
            for (i=0; i<pnWF->nCount; i++)
            {
               lResult |= pnWF->p3DGL[i]->ClassWindowFunc(hwnd, message, wParam, lParam);
            }
            if (lResult) return 0;
         } break;
      }
      
      if (pnWF->pfnWindowFunc)
         return CallWindowProc((WNDPROC)pnWF->pfnWindowFunc, hwnd, message, wParam, lParam);
      else
         return ::DefWindowProc(hwnd, message, wParam, lParam);
   }
   else
   {
      if (message == WM_CREATE)
      {
         CREATESTRUCT*pCS = (CREATESTRUCT*) lParam;
         PropertyWindowFunc *pnWF = (PropertyWindowFunc*) GetProp(hwnd, OLD_WINDOW_PROC);
         if (!pnWF)
         {
            pnWF = (PropertyWindowFunc*) malloc(sizeof(PropertyWindowFunc));
            if (!pnWF) return -1;
            pnWF->pfnWindowFunc = 0;
            pnWF->nCount        = 1;
            pnWF->p3DGL[0]      = (CEts3DGLDll*) pCS->lpCreateParams;
            ::SetProp(hwnd, OLD_WINDOW_PROC, (HANDLE)pnWF);
         }
         return 0;
      }
  		if (message == WM_DESTROY)
      {
         ASSERT(false);
      }
      else return ::DefWindowProc(hwnd, message, wParam, lParam);
   }
   return 0;
}
*/