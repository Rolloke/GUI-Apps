// Cara3D1.cpp: Implementierung der Klasse CEts3DGLDll.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ETS3DGL.h"
#include "DibSection.h"
#include "CVector.h"
#include "CustErr.h"
#include "ETS3DGLRegKeys.h"
#include "EtsAppWndMsg.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#define GWL_PFN_RENDERFUNC 0

typedef void (*RENDERFUNC)(CEts3DGLDll*);

extern bool g_bDoReport;


// ThreadFunktionen
/******************************************************************************
* Name       : OpenGLThread                                                   *  
* Zweck      : Initialisierung und Destruktion des OpenGL Rendercontextes.    *
*              Verarbeitung der Steuerungsnachrichten zum Rendern der         *
*              3D-Szene.                                                      *    
* Aufruf     : Durch die Funktion CreateThread()                              *
* Parameter  :                                                                *
* pParameter (E): Zeiger auf ein Objekt der Klasse CEts3DGLDll       (void*)  *
* Funktionswert : Abbruchbedingungen des Threads                              *
******************************************************************************/
unsigned __stdcall CEts3DGLDll::OpenGLThread(void *pParameter)
{
   BEGIN("OpenGLThread");
   MSG msg;
   ZeroMemory(&msg, sizeof(MSG));
   if (pParameter)
   {
      CEts3DGLDll *p3DGL = (CEts3DGLDll*) ((long*)pParameter)[0];// Klassenzeiger
      RECT         rc;
      HWND         hwndParent = (HWND) ((long*)pParameter)[1];   // Parentwindowhandle
      long         lUserData = 0;
      ASSERT(::IsWindow(hwndParent));

      if (!gm_ETS3D_Class)
      {
         WNDCLASSEX	 wcl;
         ZeroMemory(&wcl, sizeof(WNDCLASSEX));
         // Fensterklasse definieren
         wcl.cbSize        = sizeof(WNDCLASSEX);               // Größe von WNDCLASSEX
         wcl.hInstance     = gm_hInstance;                     // Handle zu dieser Instance
         wcl.lpszClassName = CETS3DGL_CLASSNAME;               // Name der Fensterklasse (Programmfenster)
         wcl.lpfnWndProc   = CEts3DGLDll::ThreadWindowFunc;    // Fensterfunktion
         wcl.style         = CS_OWNDC|CS_BYTEALIGNCLIENT;      // Fensterstil
         wcl.hCursor       = LoadCursor(NULL, IDC_ARROW);      // Cursorstil
         wcl.hbrBackground	= NULL;                             // weißer Hintergrund
         wcl.cbWndExtra    = 12;
         // Fensterklasse registrieren
         gm_ETS3D_Class = ::RegisterClassEx(&wcl);
      }

      ::GetClientRect(hwndParent, &rc);
      DWORD dwStyle = WS_CHILD;
      if (!(p3DGL->m_nModes & ETS3D_DC_DIB))
         dwStyle |= WS_VISIBLE;
      if ((p3DGL->m_nModes & ETS3D_DONT_WAIT))                 // nicht auf die Erstellung warten,
      {
         ::SetEvent(p3DGL->m_hEvent);                          // Nicht mehr Warten, die Daten hab ich
         ::Sleep(20);                                          // Dem anderen Thread ein bischen Zeit geben
         ::ResetEvent(p3DGL->m_hEvent);                        
      }
      RECT rcClient;
      if (gm_bBugFixBufferSize)                                // BugFix für Rage 128 GL und Windows 2000 ICD
      {                                                        // Rendercontext muß bei voller Bildschirmgröße erstellt werden
         rcClient  = rc;
         rc.right  = ::GetSystemMetrics(SM_CXSCREEN)-1;
         rc.bottom = ::GetSystemMetrics(SM_CYSCREEN)-1;
      }

      HWND hWndThread = CreateWindowEx(
         WS_EX_TOOLWINDOW,
         (LPCTSTR)gm_ETS3D_Class,                              // Name der Fensterklasse (Programmfenster)
         "3D-Window",                                          // Name des Fensters
         dwStyle,                                              // Fensterstil
         rc.left, rc.top,                                      // y-Koordinate, x-Koordinate
         rc.right, rc.bottom,                                  // Fensterbreite, Fensterhöhe
         hwndParent,                                           // übergeordnetes Fenster für das Threadwindow
         NULL,                                                 // kein Menu
         gm_hInstance,                                         // Handle auf diese Programminstance
         p3DGL);                                               // zusätzliche Argumente

      if (p3DGL->m_hWndThread == hWndThread)
      {
         p3DGL->m_nThreadID = ::GetCurrentThreadId();

         PostMessage(p3DGL->GetParent(), WM_COMMAND, MAKELONG(ETS3D_CD_INITIALIZED, 1), (LPARAM)hWndThread);

         if (p3DGL->m_nRenderTimer) p3DGL->KillTimer(p3DGL->m_nRenderTimer);
         if (!(p3DGL->m_nModes & ETS3D_NO_TIMER))              // Timer initialisieren
         {
            p3DGL->m_nRenderTimer = ::SetTimer(hwndParent, CONTROL_TIMER_EVENT, p3DGL->m_nTime, NULL);
         }

         if (gm_bBugFixBufferSize)                             // Die Fenstergröße wiederherstellen
         {
            PostMessage(hWndThread, WM_THREAD_SIZE, 0, 0);
         }

         while (GetMessage(&msg, NULL, NULL, NULL))
         {
            if (msg.message == WM_THREAD_SIZE)                 // Nachricht Größe ändern
            {
               MSG lmsg;
               _asm CLD;                                       // alle noch bestehenden Nachrichten löschen
               while(::PeekMessage(&lmsg, (HWND)-1, WM_THREAD_SIZE, WM_THREAD_SIZE, PM_REMOVE))
               {
                  msg.wParam = lmsg.wParam;                    // und die Parameter übernehmen
                  msg.lParam = lmsg.lParam;
               }
            }
                                                               // Windowfunktion direkt aufrufen
            ThreadWindowFunc(hWndThread, msg.message, msg.wParam, msg.lParam);
         };

         lUserData = ::GetWindowLong(hWndThread, GWL_USERDATA);
         if (lUserData)
         {
            p3DGL = (CEts3DGLDll*) lUserData;
         }
         if (p3DGL)
         {
            p3DGL->DestroyGLLists();                           // Open GL-Listen freigeben
            p3DGL->DestroyCallLists();                         // Löschen der SteuerungsListen
         }
      }
      else
      {
         ASSERT(false);
         PostMessage(p3DGL->GetParent(), WM_COMMAND, MAKELONG(ETS3D_CD_INITIALIZED, 2), (LPARAM)p3DGL->m_hWndThread);
      }
      if(p3DGL)
      {
         p3DGL->DestroyGL();                                   // Freigabe aller OpenGL-Parameter

         if(p3DGL->DestroyAtDllTermination())
         {                                                     // Noch ein Fenster für den Thread vorhanden
            if(!(p3DGL->m_nStates & NO_WM_DESTROY) && IsWindow(p3DGL->m_hWndThread))
            {
               DestroyWindow(p3DGL->m_hWndThread);
            }
         }
         p3DGL->m_nStates &= ~NO_WM_DESTROY;
         p3DGL->m_hWndThread = NULL;                           // Parameter für die Messages zurücksetzen
         p3DGL->m_nThreadID  = 0;
      }
#if USE_DIRECT_INPUT
      DirectXTerminate();
#endif
   }
   return 0;
}

LRESULT CALLBACK CEts3DGLDll::ThreadWindowFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   CEts3DGLDll *p3DGL = (CEts3DGLDll *)GetWindowLong(hwnd, GWL_USERDATA);
   if (p3DGL)
   {
      switch (message)
      {
         case WM_PAINT:                                        // Zeichnen des Fensters
         {
//            REPORT("WM_PAINT");
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
               p3DGL->OnPaintTH(ps);
            EndPaint(hwnd, &ps);
         }  break;
         case WM_ERASEBKGND:
//            REPORT("WM_ERASEBKGND");
            return 1;
         case WM_THREAD_SIZE:                                  // Nachricht Größe ändern
         { 
//            REPORT("WM_THREAD_SIZE");
            RECT rc;
            HWND hwndParent = p3DGL->GetParent();
            ::GetClientRect(hwndParent, &rc);
            ::MoveWindow(hwnd, 0, 0, rc.right, rc.bottom, false);
         } break;
         case WM_SIZE:
         {
//            REPORT("WM_SIZE");
            CEts3DGLDll *p3DGL = (CEts3DGLDll *)GetWindowLong(hwnd, GWL_USERDATA);
            p3DGL->OnSizeTH(wParam, LOWORD(lParam), HIWORD(lParam));
         }break;
         case WM_HIDE_THREAD_WND:                              // Nachricht Window verstecken
         p3DGL->m_nStates |= RENDERING_SUSPENDED;
         if (::IsWindowVisible(hwnd))
         {
//            REPORT("WM_HIDE_THREAD_WND");
            ShowWindow(hwnd, SW_HIDE);
            SendMessage(p3DGL->GetParent(), WM_THREAD_SIZE, 0, 0);
         } break;
         case WM_SHOW_THREAD_WND:                              // Nachricht Window Anzeigne
         p3DGL->m_nStates &= ~RENDERING_SUSPENDED;
         if (!::IsWindowVisible(hwnd))
         {
//            REPORT("WM_SHOW_THREAD_WND");
            if (!(p3DGL->m_nModes & ETS3D_DC_DIB))
            {
               ShowWindow(hwnd, SW_SHOW);
            }
            p3DGL->m_nStates &= ~RENDERING_NOW;                // Renderflag löschen
         } break;
//         case WM_SET_VIEWPORT:      p3DGL->OnSetViewPort();      break;// Setzen des Viewports
         case WM_SET_VIEWVOLUME:    p3DGL->OnSetViewVolume();    break;// Setzen des Viewing Volumes
         case WM_VALIDATE_COMMANDS: p3DGL->OnValidateCommands(); break;// Bearbeitung ungültiger OpenGL-State Variablen
         case WM_VALIDATE_LISTS:    p3DGL->OnValidateLists();    break;// Bearbeitung ungültiger OpenGL-Listen
         case WM_RENDER_SCENE:      p3DGL->OnRender();           break;// Rendern
         case WM_TRANSFORMPOINTS:                              // Punktkoordinaten Transformieren
            p3DGL->OnTransformPoints(wParam, (CVector*)lParam);
            break;
         case WM_COPY_FRAME_BUFFER:                            // Kopieren des Framebufferinhaltes für Bilder
            p3DGL->OnCopyFrameBuffer(wParam, lParam);
            break;
         case WM_SETFOCUS:
            if (hwnd != (HWND)wParam)
               SetFocus((HWND)wParam);
            break;
		   case WM_DESTROY:
         {
            PostQuitMessage(0);
         } break;
         case WM_ATTACH_THREAD_STATES:
         {                                                     // Klassenzeiger der neuen Instanz setzen
            long lValue = ::SetWindowLong(hwnd, GWL_USERDATA, wParam);
            ASSERT(lValue == (long)gm_pFirstObjectInstance);   // Der alte Klassenzeiger muß auf die erste Instanz zeigen
            p3DGL = (CEts3DGLDll *) wParam;
            p3DGL->AttachThreadStates();
         } break;
         case WM_DETACH_THREAD_STATES:
         {
            MSG msg;
            while(::PeekMessage(&msg, (HWND)-1, WM_RENDER_SCENE, WM_COPY_FRAME_BUFFER, PM_REMOVE));
            p3DGL->DetachThreadStates();                       // Klassenzeiger der ersten Instanz setzen
            long lValue = ::SetWindowLong(hwnd, GWL_USERDATA, (long)gm_pFirstObjectInstance);
            ASSERT(lValue == (long)p3DGL);                     // alter Klassenzeiger gültig ?
            SetEvent(p3DGL->m_hEvent);                         // erst jetzt freigeben
         } break;
         case WM_HELP:
            return DefWindowProc(hwnd, message, wParam, lParam);
         case WM_CONTEXTMENU:
         {
            HWND hwndParent = p3DGL->GetParent();
            ASSERT(::IsWindow(hwndParent));
            return ::PostMessage(hwndParent, message, wParam, (LPARAM)hwndParent);
         }
         default:
         {
//            REPORT("OnMessage %x", message);
            if (p3DGL->OnMessage(message, wParam, lParam))
               return 0;
            return DefWindowProc(hwnd, message, wParam, lParam);
         }
      }
   }
   else
   {
      switch (message)
      {
         case WM_CREATE:
         {
            RENDERFUNC pfnRenderScene = NULL;
            CREATESTRUCT *pcs = (CREATESTRUCT*)lParam;
            CEts3DGLDll *p3DGL = (CEts3DGLDll *) pcs->lpCreateParams;
            try
            {
               ::SetWindowLong(hwnd, GWL_USERDATA, (long)p3DGL);
               p3DGL->m_hWndThread = hwnd;
               p3DGL->GetTopLevelParent();
               if (!p3DGL->InitGL()) throw (int)1;
               if      (p3DGL->m_nModes & ETS3D_DC_DIB_1   )         // Renderfunktion auswählen
                  pfnRenderScene = CEts3DGLDll::RenderSingleDib,    REPORT("RenderSingleDib");
               else if (p3DGL->m_nModes & ETS3D_DC_DIB_2   )
                  pfnRenderScene = CEts3DGLDll::RenderDoubleDib,    REPORT("RenderDoubleDib");
               else if (p3DGL->m_nModes & ETS3D_DC_DIRECT_1)
                  pfnRenderScene = CEts3DGLDll::RenderSingleDirect, REPORT("RenderSingleDirect");
               else if (p3DGL->m_nModes & ETS3D_DC_DIRECT_2)
                  pfnRenderScene = CEts3DGLDll::RenderDoubleDirect, REPORT("RenderDoubleDirect");
               else                                                  // Falsche Parameter : Fehler
               {
                  p3DGL->m_nStates |= OPEN_GL_INIT_FAILED,          REPORT("DC-Renderparameter nicht gesetzt");
                   throw (int)2;
               }
               ::SetLastError(0);
               ::SetWindowLong(hwnd, GWL_PFN_RENDERFUNC, (long)pfnRenderScene);
               if (::GetLastError())  throw (int)3;
               SetEvent(p3DGL->m_hEvent);
               p3DGL->InitGLLists();                                 // Initialisierung der Listen
               p3DGL->m_nStates &= ~DO_NOT_VALIDATE;
            }
            catch (int nError)
            {
               SetEvent(p3DGL->m_hEvent);
               REPORT("Error Create GL-Window %d", nError);
               return -1;
            }
         }  return 0;
		   case WM_DESTROY:
         {
			   PostQuitMessage(0);
         } break;
         case WM_ERASEBKGND:
            REPORT("WM_ERASEBKGND");
            return 1;
         default:
            return DefWindowProc(hwnd, message, wParam, lParam);
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
LRESULT CEts3DGLDll::OnPaintTH(PAINTSTRUCT &ps)
{
   BEGIN("CEts3DGLDll::OnPaintTH");

   return 0;
}

// Fenstergröße, Zeichnen
/******************************************************************************
* Name       : OnSizeTH                                                       *
* Zweck      : Speichern der neuen Fenstergröße und setzen der Parameter      *
* Aufruf     : Durch die WindowMessage WM_SIZE.                               *
* Parameter  :                                                                *
* Funktionswert : 0 wenn durchgeführt                             (LRESULT)   *
******************************************************************************/
LRESULT CEts3DGLDll::OnSizeTH(UINT nType, int cx, int cy)
{
   BEGIN("OnSizeTH");
   REPORT("Size (%d, %d)", cx, cy);

   if (cx < 2) cx = 2;  // Bugfix für Matrox G400 ICD: ClientArea nie kleiner als 2
   if (cy < 2) cy = 2;

   m_szView.cx = cx;
   m_szView.cy = cy;

   if (m_nModes & (ETS3D_PR_ORTHOGONAL|ETS3D_PR_PERSPECTIVE))
      m_nStates |= INVALID_VOLUME;

   glViewport(0, 0, m_szView.cx, m_szView.cy);

   return 0;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Renderfunktion für eine DibSection
void CEts3DGLDll::RenderSingleDib(CEts3DGLDll *p3DGL)
{
   CAutoCriticalSection(&p3DGL->m_csDisplay);
   p3DGL->OnRenderSzene();                                     // Rendern
   glFinish();
   p3DGL->OnDrawToRenderDC(p3DGL->m_hDC, p3DGL->m_pDib->GetHeight());
   InvalidateRect(p3DGL->GetParent(), NULL, false);           // View auffrischen
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Renderfunktion für zwei DibSections
void CEts3DGLDll::RenderDoubleDib(CEts3DGLDll *p3DGL)
{
   p3DGL->OnRenderSzene();                                     // Rendern kann im Hintergrund durchgeführt werden
   glFinish();
   p3DGL->OnDrawToRenderDC(p3DGL->m_hDC, p3DGL->m_pDib->GetHeight());
   if (p3DGL->m_pDib)
   {
      CAutoCriticalSection(&p3DGL->m_csDisplay);
      p3DGL->m_pDib->SwapBitmaps();                            // vertauschen der Bitmaps
      InvalidateRect(p3DGL->GetParent(), NULL, false);         // View auffrischen
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Renderfunktion für direktes Rendern
void CEts3DGLDll::RenderSingleDirect(CEts3DGLDll *p3DGL)
{
   p3DGL->OnRenderSzene();                                  // Rendern
   glFinish();
   p3DGL->OnDrawToRenderDC(p3DGL->m_hDC, p3DGL->m_szView.cy);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Renderfunktion für direktes Rendern im double Buffer Mode
void CEts3DGLDll::RenderDoubleDirect(CEts3DGLDll *p3DGL)
{
   p3DGL->OnRenderSzene();                                     // Rendern kann im Hintergrund durchgeführt werden
   glFinish();
   SwapBuffers(p3DGL->m_hDC);                                  // OpenGL-Buffer vertauschen
   p3DGL->OnDrawToRenderDC(p3DGL->m_hDC, p3DGL->m_szView.cy);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// OpenGL-Initialisierung
bool CEts3DGLDll::InitGL()
{
   BEGIN("InitGL");
   try
   {
      int nPixelFormat = 0;
      bool bReportFirst =false;
      m_nStates |= DO_NOT_VALIDATE;
      if (m_nModes & ETS3D_DC_DIRECT)                          // Rendern in einen DeviceContext
      {
         REPORT("DirectDC");
         if (m_hDC == NULL)                                    // wenn kein anderer DC bei Initialisierung der Klasse
         {
            m_nStates &= ~DELETE_DC;
            m_hDC = ::GetDC(m_hWndThread);                     // übergeben wurde, DC vom Windowhandle holen.
         }
      }                                                        // dieses Fenster sollte den Klassenstil CS_OWNDC haben !!
      else if (m_nModes & ETS3D_DC_DIB)                        // Rendern in eine Dib-Section
      {
         REPORT("DibSection");
         SIZE szSize;
         if (m_nModes & ETS3D_SIZE_FIXED)                      // fest eingestellte Anfangsgröße
         {
            szSize = m_szView;                                 // die übergebene Größe übernehmen
         }
         else
         {
            szSize.cx = ::GetSystemMetrics(SM_CXSCREEN)+4;     // Bildschirmgröße ermitteln
            szSize.cy = ::GetSystemMetrics(SM_CYSCREEN)+4;
         }

         if (m_pDib == NULL) m_pDib = new CDibSection;         // Dibsection allocieren
         if (m_pDib)
         {
            HDC  hdc  = ::GetDC(NULL);                         // Haupt Gerätekontext holen
            if (hdc)
            {
               m_hDC      = ::CreateCompatibleDC(hdc);         // Memory DC erzeugen
               m_nStates |= DELETE_DC;
               ::ReleaseDC(NULL, hdc);                         // Gerätkontext freigeben
            }
            unsigned int nBits = GetDeviceCaps(m_hDC, BITSPIXEL);// Anzahl der Farbenbits ermitteln
            REPORT("BitsPerPixel: %d", nBits);
            if (m_nModes & ETS3D_CM_RGB)                       // RGB-Farbmodus einstellen
            {
               REPORT("RGB-Modus");
               if (m_Bits.nColor > nBits) m_Bits.nColor = nBits;
               m_pDib->SetColorUsage(DIB_RGB_COLORS, m_Bits.nColor);
            }
            else                                               // Colorindex Modus einstellen
            {
               REPORT("Colorindex-Modus");
               m_Bits.nColor = (m_nModes & ETS3D_CM_4096) ? 12 : 8;
               if (m_Bits.nColor > nBits) m_Bits.nColor = nBits;
               m_pDib->SetColorUsage(DIB_PAL_COLORS, m_Bits.nColor);
            }

            if (m_hDC)
            {
               if (m_pDib->Create(m_hDC, szSize.cx, szSize.cy))// DIB-Section erstellen
               {
                  if (m_nModes & ETS3D_DC_DIB_2)               // double Buffer
                  {
                     REPORT("DoubleBuffer");
                     m_pDib->DuplicateBitmap();                // zweite bitmap erstellen
                  }
               }
               else                                            // Fehler
               {
                  REPORT("DibSection konnte nicht erstellt werden");
                  ::DeleteDC(m_hDC);                           // Memory DC löschen
                  m_hDC = NULL;
               }
            }
         }
      }

      if (m_hDC == NULL) throw (int)1;                         // ist ein Gerätekontext vorhanden

      if (!g_bDoReport)                                        // ErstInfo erzeugen, nur wenn DebugReport ausgeschaltet
      {
         HANDLE hFile = CreateFile(ETS3DGL_DEBUG_FILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
         if (hFile == INVALID_HANDLE_VALUE) bReportFirst = true;// ist die Debug Datei nicht vorhanden, ErstInfo erzeugen
         else
         {
            char szRead[] = GLSETUP_INFO_PROPERTIES;           // ist die Debug Datei vorhanden
            int nLen  = strlen(szRead);
            DWORD dwRead;
            ::ReadFile(hFile, szRead, nLen, &dwRead, NULL);    // Erste Zeile lesen
            ::CloseHandle(hFile);
            if (strncmp(GLSETUP_INFO_PROPERTIES, szRead, nLen) != 0)// ist es nicht die ErstInfo
            {
               ::DeleteFile(ETS3DGL_DEBUG_FILE);               // DebugDatei löschen
               bReportFirst = true;                            // ErstInfo erzeugen
            }
         }
         if (bReportFirst)                        
         {
            g_bDoReport  = true;                               // DebugReport einschalten
            REPORT("%s\r\nAvailable Pixelformats", GLSETUP_INFO_PROPERTIES);
         }
      }

      nPixelFormat = SetDCPixelFormat(m_hDC, m_nModes, true);  // Pixelformat setzen

      if (nPixelFormat == 0)                                   // Pixelformat konnte nicht gestzt werden ?
      {                                                        // Dibsection format benutzen
         if ((m_pDib != NULL) && ((m_nModes & ETS3D_DIBSECTION_DONTCARE) != 0))
         {
            DestroyGL();                                       // alte Parameter löschen
            if (m_nModes & ETS3D_DC_DIB_1) m_nModes |= ETS3D_DC_DIRECT_1;
            if (m_nModes & ETS3D_DC_DIB_2) m_nModes |= ETS3D_DC_DIRECT_2;
            m_nModes &= ~ETS3D_DC_DIB;
            return InitGL();
         }
         throw (int)2;
      }

      m_hGL_RC = wglCreateContext(m_hDC);                      // Render-Kontext erzeugen
      if (m_hGL_RC == NULL)                  throw (int)3;

      if (!wglMakeCurrent(m_hDC, m_hGL_RC))  throw (int)4;     // Den aktuellen Kontext setzen

      if (gm_bBugFixBufferSize)                                // BugFix für Rage 128 GL und Windows 2000 ICD
      {
         SIZE szSize;

         szSize.cx = ::GetSystemMetrics(SM_CXSCREEN);          // Bildschirmgröße ermitteln
         szSize.cy = ::GetSystemMetrics(SM_CYSCREEN);

         glViewport(0,0,szSize.cx,szSize.cy);                  // neu von Olli !
         glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
         glFinish();
      }

      if (g_bDoReport)
      {
         REPORT("\r\n%s\r\n", GLSETUP_INFO_ABILITIES);
         if (m_nStates & BMP_FORMAT_AVAILABLE)            REPORT("Bitmap Format available");
         if (m_nStates & HARDWARE_ACCELERATION_AVAILABLE) REPORT("Hardware acceleration available");
         const GLubyte *pText = glGetString(GL_VENDOR);
         if (pText) REPORT("Company   : %s", pText);
         pText = glGetString(GL_RENDERER);
         if (pText) REPORT("Renderer  : %s", pText);
         pText = glGetString(GL_VERSION);
         if (pText) REPORT("Version   : %s", pText);
         pText = glGetString(GL_EXTENSIONS);

         if (pText)
         {
			   int l = lstrlen((char*)pText);
			   char * pOut = (char*) calloc(l + 2,1);

            REPORT("Extensions:");
			   if(pOut)
			   {
			      strcpy(pOut,(char*) pText);
               char *ptr =(char*)pOut;
               for (int i=0; pOut[i] !=0;i++)
               {
                  if (pOut[i] == ' ')
                  {
                     ((char*)pOut)[i] = 0;
                     REPORT("%s", ptr);
                     ptr = (char*)&pOut[i+1];
                  }
               }
               free(pOut);
            }
         }
         if (bReportFirst) g_bDoReport = false;
      }
   }
   catch(int error)
   {
      DWORD dwLast = GetLastError();
      switch(error)
      {
         case  1: SetLastError(ETS3DERR_SET_HDC);    break;
         case  2: SetLastError(ETS3DERR_SET_PIXFMT); break;
         case  3: SetLastError(ETS3DERR_CREATE_RC);  break;
         case  4: SetLastError(ETS3DERR_MKCURR_RC);  break;
         case  5: SetLastError(ETS3DERR_NOTIMER);    break;
      }
      REPORT("InitGL()-Error: %d, %x", error, dwLast);
      return false;
   }

   m_nStates |= OPEN_GL_INITIALIZED;
   if (m_nModes & ETS3D_DONT_WAIT)                             // nicht warten auf initialisierung
      PostOpenGLState(1, 0);                                   // aber Nachricht senden, wenn OpenGL Initialisiert

   return true;
}

/******************************************************************************
* Name       : SetDCPixelFormat                                               *
* Zweck      : Setzen bzw. Abfragen des Pixelformates für den Device Context  *
*              abhängig vom Modus für OpenGL.                                 *
* Aufruf     : SetDCPixelFormat(hDC, nModes, bSet);                           *
* Parameter  :                                                                *
* hDC     (E): Device Context                                         (HDC)   *
* nModes  (E): Flags für die Einstellungen                            (DWORD) *
* bSet    (E): Setzen des Pixelformates (true, false);                (bool)  *
* Funktionswert : Index des Pixelformates, bei Fehler 0.              (int)   *
******************************************************************************/
int CEts3DGLDll::SetDCPixelFormat(HDC hDC, DWORD nModes, bool bSet)
{
   if (!hDC) return 0;
   BEGIN("SetDCPixelFormat");
   PIXELFORMATDESCRIPTOR pfd =
   {
      sizeof (PIXELFORMATDESCRIPTOR),                          // Size of this structure
      1,                                                       // Version number
      PFD_DRAW_TO_WINDOW |                                     // Flags
      PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI,
      PFD_TYPE_RGBA,                                           // RGBA pixel values
      m_Bits.nColor,                                           // colorbits
      0, 0, 0, 0, 0, 0,                                        // color r,g,b, /shift r,g,b
      m_Bits.nAlpha, 0,                                        // alpha buffer, alpha shift (not supported)
      m_Bits.nAccum,                                           // accumulation buffer
      0, 0, 0, 0,                                              // accumulation r,g,b,a
      m_Bits.nDepth,                                           // depth buffer
      m_Bits.nStencil,                                         // stencil buffer
      0,                                                       // No auxiliary buffers
      PFD_MAIN_PLANE,                                          // Layer type
      0,                                                       // Reserved (must be 0)
      0, 0, 0                                                  // No layer masks
   };
   int          nColors, nPixelFormat = PIXELFORMAT_DEFAULT;
   bool         bNoDisplay = false;
   LPLOGPALETTE pPalette;
   BYTE         byRedMask, byGreenMask, byBlueMask;

   if (nModes & ETS3D_DC_DIRECT_2)                             // double Buffer-Mode setzen
   {
      pfd.dwFlags &= ~PFD_SUPPORT_GDI;
      pfd.dwFlags |=  PFD_DOUBLEBUFFER;
   }

   if ((nModes & ETS3D_DC_DIB) && (m_pDib!=NULL))              // Rendern in eine DibSection
   {
      const BITMAPINFO * pBmI = m_pDib->GetBitmapInfo();
      if (pBmI)
      {
         pfd.cColorBits = (char) pBmI->bmiHeader.biBitCount;   // Anzahl der Bits pro Pixel ermitteln
         pfd.dwFlags &= ~PFD_DRAW_TO_WINDOW;                   // Window Flag löschen
         pfd.dwFlags |=  PFD_DRAW_TO_BITMAP;                   // Bitmap Flag setzen
      }
   }
   else                                                        // Direkt Rendern
   {
      if (pfd.cColorBits == 0)
         pfd.cColorBits = GetDeviceCaps(hDC, BITSPIXEL);       // Anzahl der Farbenbits ermitteln
   }

   if (nModes & (ETS3D_CM_256|ETS3D_CM_4096))                  // Color-Index-Mode
   {
      pfd.iPixelType = PFD_TYPE_COLORINDEX;
      pfd.cColorBits = (nModes & ETS3D_CM_256) ? 8 : 12;
      pfd.dwFlags   |= PFD_NEED_PALETTE;
   }

   if (m_Bits.nGeneric)  pfd.dwFlags |= PFD_GENERIC_FORMAT;    // keine Hardwarebeschleunigung
   if (m_Bits.nGenAccel) pfd.dwFlags |= PFD_GENERIC_ACCELERATED;// Softwarebeschleunigung
   if (m_nStates & ETS3D_DOUBLEBUFFER_DONTCARE) pfd.dwFlags |= PFD_DOUBLEBUFFER_DONTCARE;
   if (m_nStates & ETS3D_DIBSECTION_DONTCARE)   pfd.dwFlags |= PFD_DRAW_TO_BITMAP_DONTCARE;

   try
   {
      if (bSet)
      {
         HKEY hKey;
         char szRegKey[_MAX_PATH];
         wsprintf(szRegKey, "%s\\%s\\%s", SOFTWARE_REGKEY, ETSKIEL_REGKEY, CETS3DGL_REGKEY);
         if (RegOpenKeyEx(HKEY_CURRENT_USER, szRegKey, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
         {
            DWORD dwSize = sizeof(DWORD), dwValue, dwType;
            if (RegQueryValueEx(hKey, REGKEY_PIXELFORMAT, NULL, &dwType, (BYTE*) &dwValue, &dwSize)==ERROR_SUCCESS)
            {
               if (dwType== REG_DWORD)
               {
                  nPixelFormat = dwValue;
               }
            }
            RegCloseKey(hKey);
         }
      }

      PIXELFORMATDESCRIPTOR pfdOld = pfd;                      // pfmtDescriptor sichern
      if (nPixelFormat != 0)                                   // Pixelformat aus der Registry versuchen 
      {
         REPORT("PixelFormat from Registry : %d", nPixelFormat);
         if (!::SetPixelFormat(hDC, nPixelFormat, &pfd))
         {
            pfd = pfdOld;
            nPixelFormat= 0;
         }
      }

      if (nPixelFormat == 0)                                   // Pixelformat nicht gesetzt ?
      {
         nPixelFormat = FindPixelFormat(hDC, pfd);             // Pixelformat der Funktion FindPixelFormat versuchen
         if (nPixelFormat != 0)
         {
            if (!bSet) throw (int)5;
            if (!::SetPixelFormat(hDC, nPixelFormat, &pfd))
            {
               pfd = pfdOld;                                   // pfmtDescriptor zurücksetzen
               nPixelFormat= 0;
            }
         }
      }
      
      if (nPixelFormat == 0)                                   // Pixelformat nicht gesetzt ?
      {
         nPixelFormat = ::ChoosePixelFormat(hDC, &pfd);        // Pixelformat der Funktion ChoosePixelFormat versuchen
         REPORT("PixelFormat from ChoosePixelFormat : %d", nPixelFormat);
         if (nPixelFormat != 0)
         {
            if (!::SetPixelFormat(hDC, nPixelFormat, &pfd)) nPixelFormat= 0;
         }
      }

      if (nPixelFormat == 0) throw (int)6;
      REPORT("Choosen Pixelformat: %d", nPixelFormat);

      m_Bits.nColor      = pfd.cColorBits;                     // Speichern der Bits
      m_Bits.nDepth      = pfd.cDepthBits;
      m_Bits.nStencil    = pfd.cStencilBits;
      m_Bits.nAccum      = pfd.cAccumBits;
      m_Bits.nAlpha      = pfd.cAlphaBits;
      m_Bits.nGeneric    = ((pfd.dwFlags & PFD_GENERIC_FORMAT) !=0);
      m_Bits.nGenAccel   = ((pfd.dwFlags & PFD_GENERIC_ACCELERATED) !=0);
      m_Bits.nSuppGDI    = ((pfd.dwFlags & PFD_SUPPORT_GDI) !=0);
      m_Bits.nSuppGL     = ((pfd.dwFlags & PFD_SUPPORT_OPENGL) !=0);
      m_Bits.nNeedPal    = ((pfd.dwFlags & PFD_NEED_PALETTE) !=0);
      m_Bits.nNeedSysPal = ((pfd.dwFlags & PFD_NEED_SYSTEM_PALETTE) !=0);
      m_Bits.nStereo     = ((pfd.dwFlags & PFD_STEREO) !=0);
      m_Bits.nSwapLayBuf = ((pfd.dwFlags & PFD_SWAP_LAYER_BUFFERS) !=0);
      m_Bits.nSwapCopy   = ((pfd.dwFlags & PFD_SWAP_COPY) !=0);
      m_Bits.nSwapExch   = ((pfd.dwFlags & PFD_SWAP_EXCHANGE) !=0);

      if ((pfd.dwFlags & PFD_NEED_PALETTE) !=0)                // Wenn eine Palette benötigt wird und gesetzt werden soll
      {
         nColors = 1 << pfd.cColorBits;
         if (nColors > 16384) nColors = 16384;                 // Palettegröße auf 16384 Farbeinträge begrenzen
         REPORT("Pallette with %d Colors", nColors);

         pPalette = (LOGPALETTE*) new char[sizeof (LOGPALETTE) + (nColors * sizeof (PALETTEENTRY))];
            
         pPalette->palVersion    = 0x300;
         pPalette->palNumEntries = (WORD) nColors;

         byRedMask   = (BYTE) ((1 << pfd.cRedBits)   - 1);     // Berechnen der Palettenfarben
         byGreenMask = (BYTE) ((1 << pfd.cGreenBits) - 1);
         byBlueMask  = (BYTE) ((1 << pfd.cBlueBits)  - 1);

         if (pfd.cRedShift   == 0) pfd.cRedShift   = 0, byRedMask   = 7;
         if (pfd.cGreenShift == 0) pfd.cGreenShift = 3, byGreenMask = 7;
         if (pfd.cBlueShift  == 0) pfd.cBlueShift  = 6, byBlueMask  = 3;

         PALETTEENTRY* pColors = &pPalette->palPalEntry[0];
         for (int i=0; i<nColors; i++)
         {
            pColors[i].peRed   = CDibSection::GetColorForPalette(i,pfd.cRedShift  ,byRedMask);
            pColors[i].peGreen = CDibSection::GetColorForPalette(i,pfd.cGreenShift,byGreenMask);
            pColors[i].peBlue  = CDibSection::GetColorForPalette(i,pfd.cBlueShift ,byBlueMask);
            pColors[i].peFlags = 0;
         }

         if ((pfd.cColorBits==8) && (pfd.cRedBits  ==3) && (pfd.cRedShift  ==0)&&
                                    (pfd.cGreenBits==3) && (pfd.cGreenShift==3)&&
                                    (pfd.cBlueBits ==2) && (pfd.cBlueShift ==6))
         {
            CDibSection::DefaultOverwrite(pColors);
         }

         m_hPalette = ::CreatePalette(pPalette);
         delete[] ((char*)pPalette);

         if (m_hPalette != NULL)
         {
            ::SelectPalette(hDC, m_hPalette, FALSE);
            ::RealizePalette(hDC);
         }
         else throw (int)7;
      }
   }
   catch(int error)
   {
      if (error == 5)
      {
         return nPixelFormat;                     // 5 : nur Ermitteln des Pixelformates
      }
      DWORD nLastErr = ::GetLastError();
      switch (error)
      {
         case 1: REPORT("ChoosePixelFormat Error : %d", nLastErr); break;
         case 2: REPORT("SetPixelFormat Error : %d", nLastErr); break;
         case 3: REPORT("DescribePixelFormat Error : %d", nLastErr); break;
         case 4: REPORT("CreatePalette Error : %d", nLastErr); break;
      }
      return 0;
   }
   return nPixelFormat;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// OpenGL-Destruktion
void CEts3DGLDll::DestroyGL()
{
   BEGIN("DestroyGL");

   if (m_hGL_RC)
   {
      if(!wglMakeCurrent(NULL, NULL))                          // Entfernen des Rendercontextes
      {
		   REPORT("Error Release Context %d",GetLastError());
      }

      if (wglDeleteContext(m_hGL_RC)) m_hGL_RC = NULL;         // Löschen des Rendercontextes
      else REPORT("Error Delete Context %x, CODE %x", m_hGL_RC, GetLastError());
   }
   if ((m_nModes & ETS3D_DC_DIRECT) && (m_hDC != NULL))        // direktes Rendern
   {
      BOOL bOk = false;
      HWND hwnd = NULL;
      if (m_nStates & DELETE_DC)
      {
         bOk = DeleteDC(m_hDC);
      }
      else
      {
         hwnd = WindowFromDC(m_hDC);
         if (hwnd) bOk = ReleaseDC(hwnd, m_hDC);                  // Gibt es ein Fenster zum DC, DC freigeben
      }
      if (!bOk) REPORT("Error DeleteDC %x, %x", hwnd, m_hDC);
      if (m_hPalette)
      {
         bOk = DeleteObject(m_hPalette);
         if (!bOk) REPORT("Error DeleteObject %x", m_hPalette);
         m_hPalette = NULL;
      }
   }
   m_hDC = NULL;
   if (m_pDib)                                                 // Rendern in eine DibSection
   {  // CriticalSection
      delete m_pDib;                                           // löschen
      m_pDib= NULL;
   }
   m_nStates &= ~OPEN_GL_INITIALIZED;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Löschen der Listen
void CEts3DGLDll::DestroyCallLists()
{
   BEGIN("DestroyCallLists");
   RenderList *pList = m_pRenderList; 
   while (pList)                                               // OpenGL-Listen-Liste freigeben
   {
      RenderList *p = pList;
      pList = pList->pNext;
      glDeleteLists(p->nListID, p->nRange);                    // OpenGLListe freigeben
      delete p;
   }
   m_pRenderList = NULL;
   pList = m_pCommandList;
   while (pList)                                               // OpenGL-Command-Liste freigeben
   {
      RenderList *p = pList;
      pList = pList->pNext;
      glDeleteLists(p->nListID, p->nRange);                    // OpenGLListe freigeben
      delete p;
   }
   m_pCommandList = NULL;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Viewport einstellen
void CEts3DGLDll::OnSetViewPort()
{
   BEGIN("OnSetViewPort");
   CAutoCriticalSection acs(m_pcsExt);
   glViewport(0, 0, m_szView.cx, m_szView.cy);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Viewvolume einstellen
void CEts3DGLDll::OnSetViewVolume()
{
   BEGIN("OnSetViewVolume");
   glMatrixMode(GL_PROJECTION);                                // Projektionsmatrixstapel initialisieren
   glLoadIdentity();                                           // Einheitsmatrix laden
   CAutoCriticalSection acs(m_pcsExt);
   if      (m_nModes & ETS3D_PR_PERSPECTIVE)                   // Perspektivische Projektion
   {
      REPORT("ETS3D_PR_PERSPECTIVE");
      gluPerspective(m_dAlpha,                                 // Öffnungswinkel
         ((double)m_szView.cx)/((double)m_szView.cy),          // Seitenverhältnis
                     m_Volume.Near, m_Volume.Far);             // vordere und hintere Grenze
   }
   else if (m_nModes & ETS3D_PR_ORTHOGONAL)                    // Orthogonale Projektion
   {
      REPORT("ETS3D_PR_ORTHOGONAL");
      float  Aspect = (float)(((double)m_szView.cx)/((double)m_szView.cy));
      float  left, top, right, bottom;
      if (Aspect >= 1.0)                                       // Seitenverhältnis ermitteln
      {
         left   = m_Volume.Left  * Aspect;
         right  = m_Volume.Right * Aspect;
         top    = m_Volume.Top;
         bottom = m_Volume.Bottom;
      }
      else 
      {
         if (Aspect < 0.05f) Aspect = 0.05f;
         left   = m_Volume.Left;
         right  = m_Volume.Right;
         top    = m_Volume.Top    / Aspect;
         bottom = m_Volume.Bottom / Aspect;
      }
                                                               // Sichtvolumen
      glOrtho(left, right, bottom, top, m_Volume.Near, m_Volume.Far);
   }
   else if (m_nModes & ETS3D_PR_FRUSTUM)                       // Einstellung des Sichvolumens (perspektivische Projektion)
   {
      REPORT("ETS3D_PR_FRUSTUM");
      glFrustum(m_Volume.Left  , m_Volume.Right,
                m_Volume.Bottom, m_Volume.Top  ,
                m_Volume.Near  , m_Volume.Far);                // Sichtvolumen
   }
   if (m_nModes & ETS3D_PR_LOOK_AT)                            // LookAt-Perspektive
   {
      REPORT("ETS3D_PR_LOOK_AT");
      double sinphi   = sin(RAD10(m_LookAt.nPhi));
      double cosphi   = cos(RAD10(m_LookAt.nPhi));
      double sintheta = sin(RAD10(m_LookAt.nTheta));
      double costheta = cos(RAD10(m_LookAt.nTheta));
      double sinxi    = sin(RAD10(m_LookAt.nXi));
      double cosxi    = cos(RAD10(m_LookAt.nXi));

      double dCenterx = m_LookAt.dEyex + sinphi;
      double dCentery = m_LookAt.dEyey + sintheta;
      double dCenterz = m_LookAt.dEyez + cosphi * costheta;

      double dUpx     = -sintheta * sinphi * cosxi - cosphi * sinxi;
      double dUpy     =  cosxi * costheta;
      double dUpz     = -sintheta * cosphi * cosxi + sinphi * sinxi;

      gluLookAt(m_LookAt.dEyex, m_LookAt.dEyey, m_LookAt.dEyez,
                dCenterx      , dCentery      , dCenterz   ,
                dUpx          , dUpy          , dUpz);
   }
   glMatrixMode(GL_MODELVIEW);                                 // Modelansicht einstellen
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Rendern der Listen
void CEts3DGLDll::OnRender()
{
   if (m_nStates & RENDERING_SUSPENDED) return;
   RENDERFUNC pfnRenderScene = (RENDERFUNC)::GetWindowLong(m_hWndThread, GWL_PFN_RENDERFUNC);// Renderfunktion
   ClearBuffer();
   if (pfnRenderScene) pfnRenderScene(this);                   // Rendern der Darstellungselemente
   m_nStates &= ~RENDERING_NOW;                                // Renderflag löschen
}

void CEts3DGLDll::ClearBuffer()
{
   if (m_nModes & ETS3D_CM_RGB)                                // im RGB-Modus
      glClearColor(INV_256(GetRValue(m_cBkGnd)), INV_256(GetGValue(m_cBkGnd)), INV_256(GetBValue(m_cBkGnd)), 0.0);
   else                                                        // im Color-Index-Modus
      glClearIndex((float)::GetNearestPaletteIndex(m_hPalette, m_cBkGnd));
   glClear(m_nClearBits);                                      // AusgabeBufferBits löschen
}
/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGLDll::OnRenderSzeneDll()
{
   BEGIN("CEts3DGLDll::OnRenderSzene");
   if (m_pRenderList) RenderSceneR(m_pRenderList, this);

   MsgBoxGlError();
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Rekursives Aufrufen der Listen
void CEts3DGLDll::RenderSceneR(RenderList *pList, void*pParam)
{
   if (pList->bRender && glIsList(pList->nListID))
   {
      REPORT("CallList(%d)", pList->nListID);
      glCallList(pList->nListID);
      if (pList->pfnDirect) pList->pfnDirect((CEts3DGLDll*)pParam, pList->nListID, pList->nRange);
   }
   if (pList->pNext) RenderSceneR(pList->pNext, pParam);
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Validieren der Listen
void CEts3DGLDll::OnValidateLists()
{
   BEGIN("OnValidateLists");
   if (m_pRenderList)
   {
      CAutoCriticalSection acs(m_pcsExt);
      ValidateR(m_pRenderList, this);
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Validieren der OpenGL-Variablen
void CEts3DGLDll::OnValidateCommands()
{
   BEGIN("OnValidateCommands");
   if (m_pCommandList)
   {
      CAutoCriticalSection acs(m_pcsExt);
      ValidateR(m_pCommandList, this);
   }
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
// Rekursives Aufrufen der Funktionen zur Listenerstellung
void CEts3DGLDll::ValidateR(RenderList *pList, void *pParam)
{
   ASSERT(pList->pfnRender != NULL);
   if (pList->bInvalid && pList->bRender)
   {
      REPORT("Validating (%d)", pList->nListID);
      pList->pfnRender((CEts3DGLDll*)pParam, pList->nListID, pList->nRange);
      pList->bInvalid = false;

      MsgBoxGlError();
   }
   if (pList->pNext) ValidateR(pList->pNext, pParam);
}

/******************************************************************************
* Name       : OnTransformPoints                                              *
* Zweck      : Transformation bzw. Rücktransformation von                     *
*              Bildschirmkoordinaten in 3D Koordinaten.                       *
* Aufruf     : Durch die ThreadMessage WM_TRANSFORMPOINTS                     *
* Parameter  :                                                                *
* wParam  (E): LOWORD : Transformationsrichtung (0, 1),            (WPARAM)   *
*              HIWORD : Anzahl der Punkte                                     *
* pV     (EA): Zeiger auf Feld mit den Punktkoordinaten            (CVector*) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CEts3DGLDll::OnTransformPoints(WPARAM wParam, CVector *pV)
{
   BEGIN("OnTransformPoints");
   bool     bProject    = (wParam&0x80000000) ? true : false;
   int      i, nCount   = (wParam&0x7fffffff), viewport[4];
   double   modelMatrix[16], projMatrix[16], *pdIn, *pdOut;
   CVector  vErg;
   pdOut =(double*)&vErg;

   glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
   glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
   glGetIntegerv(GL_VIEWPORT, viewport);

   if (bProject)                                               // Projektion
   {
      for (i=0; i<nCount; i++)
      {
         pdIn = (double*) &pV[i];
         gluProject (pdIn[0], pdIn[1], pdIn[2],
                     modelMatrix, projMatrix, viewport,
                     &pdOut[0], &pdOut[1], &pdOut[2]);
         pV[i] = vErg;
      }
   }
   else                                                        // Rückprojektion
   {
      for (i=0; i<nCount; i++)
      {
         pdIn = (double*) &pV[i];
         gluUnProject(pdIn[0], pdIn[1], pdIn[2],
                     modelMatrix, projMatrix, viewport,
                     &pdOut[0], &pdOut[1], &pdOut[2]);
         pV[i] = vErg;
      }
   }
   m_nStates &= ~TRANSFORMING_NOW;
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGLDll::OnCopyFrameBuffer(WPARAM wParam, LPARAM lParam)
{
   BEGIN("OnCopyFrameBuffer");
   bool bDeleteDib   = false;
   bool bDeleteDC    = true;
   static int pnBuffer[] = 
   {
      GL_FRONT,
      GL_BACK, 
      GL_FRONT_LEFT, 
      GL_BACK_LEFT, 
      GL_FRONT_RIGHT, 
      GL_BACK_RIGHT,
      GL_LEFT,
      GL_RIGHT,
      GL_FRONT_AND_BACK,
      GL_AUX0,
      GL_AUX1,
      GL_AUX2,
      GL_AUX3
   };
   CDibSection *pDib = NULL;
   HDC          hDC  = NULL;
   try
   {
      HANDLE hOldBmp = NULL;
      if (((wParam&COPY_TO_MASK) == COPY_TO_DIB) || ((wParam&COPY_TO_MASK) == RENDER_TO_DIP))
      {
         pDib = (CDibSection*) lParam; 
      }
      else pDib = new CDibSection, bDeleteDib = true;

      if (!m_hDC)                                              throw (int)1;
      if (!pDib)                                               throw (int)2;

      int    nWidth  = m_szView.cx,
             nHeight = m_szView.cy;
      GLenum nFormat = GL_BGRA_EXT;
      GLenum nType   = GL_UNSIGNED_BYTE;
      hDC = CreateCompatibleDC(m_hDC);                  // Kompatiblen DC anlegen
      if (!hDC)                                                throw (int)3;

      nWidth = (nWidth >> 2) << 2;                             // Breite muß durch 4 teilbar sein

      if (m_nModes & ETS3D_CM_RGB)
      {
         pDib->SetColorUsage(DIB_RGB_COLORS, 32);              // 32 Bit zum Kopieren des Framebuffers
      }
      else
      {
         pDib->SetColorUsage(DIB_PAL_COLORS, GetDeviceCaps(m_hDC, BITSPIXEL));
         nFormat = GL_COLOR_INDEX;
      }

      pDib->Destroy();                                         // Dibsection freigeben
      if (wParam & DELETE_DIBSECTION)
      {
         bDeleteDib = true;
         throw (int)0;
      }
      if (wParam & DESTROY_DIBSECTION)                      throw (int)0;
      if (pDib->Create(hDC, nWidth, nHeight))
      {                                                        // DeleteDC wird von CDibSection
         bDeleteDC = false;                                    // übernommen
      }
      else                                                  throw (int)5;

      void *p = pDib->GetBits();
      if (!p)                                               throw (int)7;
      if (wParam == RENDER_TO_DIP)
      {
         if (!(m_nModes & ETS3D_DC_DIB)) throw (int) 15;
         PIXELFORMATDESCRIPTOR pfd;
         ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
         pfd.nSize = sizeof (PIXELFORMATDESCRIPTOR);
         ::SetPixelFormat(hDC, ::GetPixelFormat(m_hDC), &pfd);
         if (!wglMakeCurrent(hDC, m_hGL_RC))  throw (int)16;     // Den aktuellen Kontext setzen
      }
      if (m_Bits.nSwapExch || (m_nStates & CLEAR_INVALID))
      {
         REPORT("Rendering Szene again");
         if (m_nStates & CLEAR_INVALID)
         {
            m_nStates |=  RENDERING_NOW;                       // Renderflag setzen
            OnSetViewPort();
            OnSetViewVolume();
            OnValidateCommands();
            OnValidateLists();
            m_nStates &= ~CLEAR_INVALID;
         }
         OnRender();
         glFinish();
      }
      if (wParam == RENDER_TO_DIP)
      {
         OnDrawToRenderDC(hDC, nHeight);
         if (!wglMakeCurrent(m_hDC, m_hGL_RC))  throw (int)17; // Den aktuellen Kontext setzen
         throw (int)0;
      }

      GLint nBuffer;                                           // Daten kopieren
      glGetIntegerv(GL_READ_BUFFER, &nBuffer);
      glReadBuffer(pnBuffer[gm_nBugFixBuffer]);
      glReadPixels(0, 0, nWidth, nHeight, nFormat, nType, p);
      OnDrawToRenderDC(hDC, nHeight);
      glReadBuffer(nBuffer);
      pDib->Convert32to24bit();

      switch (wParam&COPY_TO_MASK)
      {
         case COPY_TO_DIB: break;
         case COPY_TO_HGLOBAL:                                 // erzeugen eines globalen Memoryhandles
         {
            HGLOBAL* pHGL = (HGLOBAL*)lParam;
            *pHGL = pDib->GetDIBData();
            if (*pHGL == NULL)                                 throw (int)8;
         } break;
         case COPY_TO_FILE:                                    // Kopieren der Daten in eine Datei
         {
            char * pszFileName = (char*) lParam;
            HANDLE hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE)                 throw (int)9;
            bool bSave = pDib->SaveInFile(hFile);
            CloseHandle(hFile);
            if (!bSave)                                        throw (int)10;
         } break;
         case COPY_TO_HDC:                                     // Ausgabe auf einen DC
         {
            ETS3DGL_Draw *pDraw = (ETS3DGL_Draw*) lParam;
            if (!pDib->StretchBlt(pDraw->hDC, pDraw->rcFrame.left, pDraw->rcFrame.top, pDraw->rcFrame.right-pDraw->rcFrame.left, pDraw->rcFrame.bottom-pDraw->rcFrame.top, SRCCOPY)) 
               throw (int)11;
         } break;
         case COPY_TO_ENHMETA:
         {
            ETS3DGL_Draw *pDraw = (ETS3DGL_Draw*) lParam;
            pDraw->hDC = CreateEnhMetaFile(NULL, NULL, NULL, "CEts3DGLDll\0Framebuffer\0");
            if (pDraw->hDC == NULL)                            throw (int)12;
            ::SetStretchBltMode(pDraw->hDC, HALFTONE);
            ::SetBrushOrgEx(pDraw->hDC, 0, 0, NULL);
            pDraw->rcFrame.left   = 0;
            pDraw->rcFrame.top    = 0;
            pDraw->rcFrame.right  = nWidth;
            pDraw->rcFrame.bottom = nHeight;
            if (!pDib->StretchBlt(pDraw->hDC, pDraw->rcFrame.left, pDraw->rcFrame.top, nWidth, nHeight, SRCCOPY)) throw 13;
            pDraw->hMeta = ::CloseEnhMetaFile(pDraw->hDC);
            pDraw->hDC = NULL;
         } break;
         case COPY_TO_PRINTER:
         {
            ETS3DGL_Draw *pDraw = (ETS3DGL_Draw*) lParam;
            if (pDraw->hDC == NULL) throw 12;
            if (!pDib->StretchDIBits(pDraw->hDC, pDraw->rcFrame)) throw (int)13;
         } break;
         default: throw (int)14;
      }
   }
   catch(int nError)
   {
      REPORT("Error :%d, %x", nError, ::GetLastError());
   }
   if (bDeleteDC &&  (hDC != NULL))
   {
      ::DeleteDC(hDC);
   }
   if (bDeleteDib && (pDib != NULL))
   {
      delete pDib;
   }
   m_nStates &= ~COPY_FRAME_BMP_NOW;
}

/******************************************************************************
* Name       : DetachThreadStates                                             *
* Definition : void DetachThreadStates();                                     *
* Zweck      : Freigabe der OpenGL-Listen
* Aufruf     : 
* Parameter  : 
******************************************************************************/
void CEts3DGLDll::DetachThreadStates()
{
   if (m_pDib)                                              // Bitmap inhalt löschen
   {
      CAutoCriticalSection(&this->m_csDisplay);             // Kritische Sektion !
      HBRUSH hBr = ::CreateSolidBrush(m_cBkGnd);            // Hintergrundpinsel erzeugen
      ::SaveDC(m_pDib->GetDC());
      ::SelectObject(m_pDib->GetDC(), hBr);                 // Selectieren
      if (m_pDib->GetSecondBitmapHandle() != NULL)          // Doublebuffer :
         m_pDib->SwapBitmaps();                             // Hintergrundbitmap in Vordergrund und löschen
      ::PatBlt(m_pDib->GetDC(), 0, 0, m_pDib->GetWidth(), m_pDib->GetHeight(), PATCOPY);
      if (m_pDib->GetSecondBitmapHandle() != NULL)          // wieder in Hintergrund
         m_pDib->SwapBitmaps();
      ::RestoreDC(m_pDib->GetDC(), -1);
      ::DeleteObject(hBr);                                  // Brush löschen
   }
   DestroyCallLists();                                      // Listen löschen
   DestroyGLLists();
}
/******************************************************************************
* Name       : AttachThreadStates                                             *
* Definition : void CEts3DGLDll::AttachThreadStates()                         *
* Zweck      : Initialisieren der OpenGL Listen. Durch die Windowmessage wird *
*              vor dieser Funktion der neue Klassenzeiger gesetzt.            *
* Aufruf     : durch WM_ATTACH_THREAD_STATES.                                 *
* Parameter  : keine                                                          *
******************************************************************************/
void CEts3DGLDll::AttachThreadStates()
{
   InitGLLists();
   m_nStates &= ~DO_NOT_VALIDATE;
   m_nStates |= OPEN_GL_INITIALIZED;
   SetEvent(m_hEvent);                  // Synchronisierungs Event setzen
}

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
#ifdef ETSDEBUG_INCLUDE
void ReportFormat(int nPixFmt, int nValue, PIXELFORMATDESCRIPTOR &Pfd)
{
// REPORT("|No|wnd|bmp|ci|db|gdi|gl|p|sp|ga|gf|st|sl|sc|se|col|dpt|stn|lt|equaly|");
   REPORT("|%2d|%3d|%3d|%2d|%2d|%3d|%2d|%d|%2d|%2d|%2d|%2d|%2d|%2d|%2d|%3d|%3d|%3d|%2d|%6x|%s|",
      nPixFmt,
      (Pfd.dwFlags&PFD_DRAW_TO_WINDOW)       ? 1 : 0,
      (Pfd.dwFlags&PFD_DRAW_TO_BITMAP)       ? 1 : 0,
      (Pfd.iPixelType==PFD_TYPE_COLORINDEX)  ? 1 : 0,
      (Pfd.dwFlags&PFD_DOUBLEBUFFER)         ? 1 : 0,
      (Pfd.dwFlags&PFD_SUPPORT_GDI)          ? 1 : 0,
      (Pfd.dwFlags&PFD_SUPPORT_OPENGL)       ? 1 : 0,
      (Pfd.dwFlags&PFD_NEED_PALETTE)         ? 1 : 0,
      (Pfd.dwFlags&PFD_NEED_SYSTEM_PALETTE)  ? 1 : 0,
      (Pfd.dwFlags&PFD_GENERIC_ACCELERATED)  ? 1 : 0,
      (Pfd.dwFlags&PFD_GENERIC_FORMAT)       ? 1 : 0,
      (Pfd.dwFlags&PFD_STEREO)               ? 1 : 0,
      (Pfd.dwFlags&PFD_SWAP_LAYER_BUFFERS)   ? 1 : 0,
      (Pfd.dwFlags&PFD_SWAP_COPY)            ? 1 : 0,
      (Pfd.dwFlags&PFD_SWAP_EXCHANGE)        ? 1 : 0,
      Pfd.cColorBits,
      Pfd.cDepthBits,
      Pfd.cStencilBits,
      Pfd.iLayerType,
      nValue,
      nValue>=0xE0000000 ? "x":" ");
}
#endif

/******************************************************************************
* Name       : 
* Definition : 
* Zweck      : 
* Aufruf     : 
* Parameter  : 
******************************************************************************/
int CEts3DGLDll::FindPixelFormat(HDC hdc, PIXELFORMATDESCRIPTOR &Pfd)
{
//   BEGIN("FindPixelFormat");
   UINT  nMaxPixFmt, nPixFmt = 1, nBestPixFmt = 0, nValue, nMaxValue = 0xE0000000, nSize = sizeof (PIXELFORMATDESCRIPTOR);
   m_nStates &= ~(BMP_FORMAT_AVAILABLE|HARDWARE_ACCELERATION_AVAILABLE);
   PIXELFORMATDESCRIPTOR PfdTest;
   ZeroMemory(&PfdTest, nSize);
   PfdTest.nSize = nSize;
   nMaxPixFmt = DescribePixelFormat(hdc, nPixFmt, nSize, &PfdTest);
   if (nMaxPixFmt == 0) return 0;

#ifdef ETSDEBUG_INCLUDE
   REPORT("No  : Pixelformat number");
   REPORT("wnd : Draw to Window");
   REPORT("bmp : Draw to Bitmap");
   REPORT("ci  : Color Index");
   REPORT("db  : Double Buffer");
   REPORT("gdi : Support GDI");
   REPORT("gl  : Support OpenGL");
   REPORT("p   : Need Palette");
   REPORT("sp  : Need Systempalette");
   REPORT("ga  : Generic Accelerated");
   REPORT("gf  : Generic Format");
   REPORT("st  : Stereo");
   REPORT("sl  : Swap Layer Buffers");
   REPORT("sc  : Swap Copy");
   REPORT("se  : Swap Exchange");
   REPORT("col : Colorbits");
   REPORT("dpt : DepthBits");
   REPORT("stn : StencilBits");
   REPORT("lt  : LayerType\r\n");
   REPORT("|No|wnd|bmp|ci|db|gdi|gl|p|sp|ga|gf|st|sl|sc|se|col|dpt|stn|lt|equaly|");
#endif
   
   while(nPixFmt < nMaxPixFmt)
   {
      nValue = 0;                                                                            // Wertung
      if ((Pfd.dwFlags&PFD_DRAW_TO_WINDOW)      == (PfdTest.dwFlags&PFD_DRAW_TO_WINDOW))      nValue |= 0x80000000;
      if (PfdTest.dwFlags&PFD_DRAW_TO_BITMAP) m_nStates |= BMP_FORMAT_AVAILABLE;
      if ((Pfd.dwFlags&PFD_DRAW_TO_BITMAP)      == (PfdTest.dwFlags&PFD_DRAW_TO_BITMAP))      nValue |= 0x80000000;
      if (Pfd.iPixelType == PfdTest.iPixelType)                                               nValue |= 0x40000000;
      if (Pfd.dwFlags&PFD_DOUBLEBUFFER_DONTCARE)                                              nValue |= 0x20000000;
      if ((Pfd.dwFlags&PFD_DOUBLEBUFFER)        == (PfdTest.dwFlags&PFD_DOUBLEBUFFER))        nValue |= 0x20000000;
      if ((Pfd.dwFlags&PFD_SUPPORT_OPENGL)      == (PfdTest.dwFlags&PFD_SUPPORT_OPENGL))      nValue |= 0x10000000;
      if (PfdTest.cColorBits > 0)   // Farbtiefe > 0
      {
         if (Pfd.cColorBits == PfdTest.cColorBits)                                            nValue |= 0x0FF00000;
         else                                                                                 nValue |= (PfdTest.cColorBits & 0x000000FF) << 20;
      }
      if ((Pfd.dwFlags&PFD_GENERIC_FORMAT)      == (PfdTest.dwFlags&PFD_GENERIC_FORMAT))      nValue |= 0x00080000;
      if (!(PfdTest.dwFlags&PFD_GENERIC_FORMAT))m_nStates |= HARDWARE_ACCELERATION_AVAILABLE;
      if ((Pfd.dwFlags&PFD_GENERIC_ACCELERATED) == (PfdTest.dwFlags&PFD_GENERIC_ACCELERATED)) nValue |= 0x00040000;
      if ((Pfd.dwFlags&PFD_SUPPORT_GDI)         == (PfdTest.dwFlags&PFD_SUPPORT_GDI))         nValue |= 0x00020000;
      if ((Pfd.dwFlags&PFD_NEED_PALETTE)        == (PfdTest.dwFlags&PFD_NEED_PALETTE))        nValue |= 0x00010000;
      if (Pfd.dwFlags&PFD_DEPTH_DONTCARE)                                                     nValue |= 0x0000FF00;
      if (PfdTest.cDepthBits > 0)   // Depthbuffer vorhanden
      {
         if (Pfd.cDepthBits == PfdTest.cDepthBits)                                            nValue |= 0x0000FF00;
         else                                                                                 nValue |= (PfdTest.cDepthBits & 0x000000FF) << 8;
      }
      if (Pfd.cStencilBits                      == PfdTest.cStencilBits)                      nValue |= 0x00000080;
      if (Pfd.cStencilBits                      <  PfdTest.cStencilBits)                      nValue |= 0x00000040;
      if ((Pfd.dwFlags&PFD_NEED_SYSTEM_PALETTE) == (PfdTest.dwFlags&PFD_NEED_SYSTEM_PALETTE)) nValue |= 0x00000020;
      if (Pfd.dwFlags&PFD_STEREO_DONTCARE)                                                    nValue |= 0x00000010;
      if ((Pfd.dwFlags&PFD_STEREO)              == (PfdTest.dwFlags&PFD_STEREO))              nValue |= 0x00000010;
      if ((Pfd.dwFlags&PFD_SWAP_LAYER_BUFFERS)  == (PfdTest.dwFlags&PFD_SWAP_LAYER_BUFFERS))  nValue |= 0x00000008;
      if ((Pfd.dwFlags&PFD_SWAP_COPY)           == (PfdTest.dwFlags&PFD_SWAP_COPY))           nValue |= 0x00000004;
      if ((Pfd.dwFlags&PFD_SWAP_EXCHANGE)       == (PfdTest.dwFlags&PFD_SWAP_EXCHANGE))       nValue |= 0x00000002;
      if (nValue > nMaxValue)
      {
         nMaxValue   = nValue;
         nBestPixFmt = nPixFmt;
      }
#ifdef ETSDEBUG_INCLUDE
      ReportFormat(nPixFmt, nValue, PfdTest);     
#endif
      nPixFmt++;
      ZeroMemory(&PfdTest, nSize);
      PfdTest.nSize = nSize;
      DescribePixelFormat (hdc, nPixFmt, nSize, &PfdTest);
   }
#ifdef ETSDEBUG_INCLUDE
   REPORT("Needed PF = 0, Best fit = %d", nBestPixFmt);
   ReportFormat(0, 0, Pfd);
#endif
   ZeroMemory(&PfdTest, nSize);
   PfdTest.nSize = nSize;
   DescribePixelFormat (hdc, nBestPixFmt, nSize, &Pfd);
#ifdef ETSDEBUG_INCLUDE
   ReportFormat(nBestPixFmt, 0, Pfd);
   REPORT("AccumBits %d : (%d,%d,%d,%d)", Pfd.cAccumBits, Pfd.cAccumRedBits, Pfd.cAccumGreenBits, Pfd.cAccumBlueBits, Pfd.cAccumAlphaBits);
   REPORT("ColorBits %d : (%d,%d,%d,%d)", Pfd.cColorBits, Pfd.cRedBits, Pfd.cGreenBits, Pfd.cBlueBits, Pfd.cAlphaBits);
   REPORT("ColorShift   : (%d,%d,%d,%d)", Pfd.cRedShift, Pfd.cGreenShift, Pfd.cBlueShift, Pfd.cAlphaShift);
#endif

   return nBestPixFmt;
}

#if USE_DIRECT_INPUT
void CEts3DGLDll::DirectXInit(HWND hwnd)
{
   HRESULT hr;
   try
   {
      hr = DirectInputCreate(gm_hInstance, DIRECTINPUT_VERSION, &gm_pdi, NULL);
      if (FAILED(hr)) throw (int) 1;

      hr = gm_pdi->CreateDevice(GUID_SysKeyboard, &gm_pKeyboard, NULL);
      if (FAILED(hr)) throw (int) 2;

      hr = gm_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
      if (FAILED(hr)) throw (int) 3;

      hr = gm_pKeyboard->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE|DISCL_FOREGROUND);
      if (FAILED(hr)) throw (int) 4;
   }
   catch (int nError)
   {
      REPORT("DirectX-Creation Failed : %d", nError);
      DirectXError(hr);
      DirectXTerminate();
   }
}

void CEts3DGLDll::DirectXTerminate()
{
   if (gm_pKeyboard) 
   {
      gm_pKeyboard->Unacquire();
      gm_pKeyboard->Release();
      gm_pKeyboard = NULL;
   }
   if (gm_pdi) 
   {
      gm_pdi->Release();
      gm_pdi = NULL;
   }
}

void CEts3DGLDll::DirectXInput(HWND hwnd)
{
   if (gm_pKeyboard) 
   {
      BYTE diks[256];             /* DirectInput keyboard state buffer */
      HRESULT hr;
      again:;
      hr = gm_pKeyboard->GetDeviceState(sizeof(diks), &diks);
      if (hr == DIERR_INPUTLOST)
      {
         hr = gm_pKeyboard->Acquire();
         if (SUCCEEDED(hr)) 
         {
             goto again;
         }
      }

      if (SUCCEEDED(hr))
      {
         int i;
         LPARAM lParam = 0;
         if (diks[0x01d] & 0x80) lParam |= MK_CONTROL, diks[0x01d] &= ~0x80;// Ctrl
         if (diks[0x02a] & 0x80) lParam |= MK_SHIFT  , diks[0x02a] &= ~0x80;// Shift
         if (diks[0x0b8] & 0x80) lParam |= MK_LBUTTON, diks[0x0b8] &= ~0x80;// Alt Gr
         if (diks[0x09d] & 0x80) lParam |= MK_RBUTTON, diks[0x09d] &= ~0x80;// Ctrl right
         if (diks[0x036] & 0x80) lParam |= MK_MBUTTON, diks[0x036] &= ~0x80;// Shift right
         for (i=0; i<256; i++) 
         {
            if (diks[i] & 0x80)
            {
               ThreadWindowFunc(hwnd, WM_CHAR, (WPARAM)i, lParam);
            }
         }
      }
      else
      {
         DirectXError(hr);
      }
   }
}

void CEts3DGLDll::DirectXError(HRESULT hr)
{
   REPORT("Error %x : Serverity(%d), Facility(%d), Code(%d)", hr, SCODE_SEVERITY(hr), SCODE_FACILITY(hr), SCODE_CODE(hr));
} 
#endif

