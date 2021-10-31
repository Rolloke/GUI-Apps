/**************************************+****************************************

                                 PreViewWnd.cpp
                                 --------------

                   das Vorschaufenster für den Druckerdialog

                  (C) 1999-2000 ELAC Technische Software GmbH

                         VERSION 1.1 Stand 20.09.2000


                                                   programmed by Oliver Wesnigk
*******************************************************************************/

#include "stdafx.h"

#define  ETSDEBUG_INCLUDE
#include <CEtsDebug.h>
#include <CEtsHelp.h>
#include <process.h>

struct DRAWPARAMETER
{
   bool         bValid;
   RECT         rcPage;
   RECT         rcMargin;
   POINT        ptOffset;
   SIZE         siArea;
   HBITMAP      hBitmap;
   bool         bMono;
};

/**************************************+****************************************

                               globale Variablen

*******************************************************************************/

const char       g_szPreViewClassName[]= "ETSPRTD_PreView";
volatile HWND    g_hwndPreView         = NULL;   // der Fensterhandle des PreViewFensters
int              g_nSizeX;                       // die Größe des PreViewFensters
int              g_nSizeY;
PREVIEWPARAMETER g_PreViewParam        = {false,{0,0},{0,0,0,0},false}; 
DRAWPARAMETER    g_DrawParam           = {false,{0,0,0,0},{0,0,0,0},{0,0},{0,0},NULL};
volatile HENHMETAFILE g_hMetaFile      = NULL;   // Handle des MetaFiles
bool             g_bType               = false;  // Flag, für die Art der Darstellung
bool             g_bOneMetaFile        = false;  // Flag, das MetaFile über Callback erzeugt wurde
void *           g_pCallback;                    // die Callbackfunktion für das Metafile rendern
HANDLE           g_hThreadEvent;                 // Event für Thread - Syncronisation

CRITICAL_SECTION g_csDataSyncronize;
CRITICAL_SECTION g_csMetaSyncronize;

extern HINSTANCE g_hInstance;                    // der Instancehandle dieser DLL

ATOM             g_aClass = NULL;                // ATOM der Fensterklasse

/**************************************+****************************************

                             Funktionsdeklarationen

*******************************************************************************/

LRESULT CALLBACK PreViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void             CalcNewDrawParameter();
void             OnPaint(HWND hwnd);
void _cdecl      CreateDrawBitmap(LPVOID);
void _cdecl      CreateMetaFile(LPVOID);

/**************************************+****************************************
                                 InitPreViewWnd

public Function: Registriere die Fensterklasse für das Vorschaufenster

return         : true == ok , false == Fehler

*******************************************************************************/

bool InitPreViewWnd()
{
   WNDCLASSEX  wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = PreViewWndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = g_hInstance;
   wc.hIcon         = NULL;
   wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
   wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = g_szPreViewClassName;
   wc.hIconSm       = NULL;
   wc.cbSize        = sizeof(WNDCLASSEX);

   g_aClass = RegisterClassEx(&wc);
   if(!g_aClass) return false;

   g_hThreadEvent = CreateEvent(NULL,            // Standard Sicherheit
                                FALSE,           // Auto-Reset-Event
                                TRUE,            // ist signalisiert
                                NULL);           // keinen Namen

   if(g_hThreadEvent==NULL) return false;

   InitializeCriticalSection(&g_csDataSyncronize);
   InitializeCriticalSection(&g_csMetaSyncronize);

   return true;
}


/**************************************+****************************************
                                ReleasePreViewWnd

public Function: die Vorschaufensterklasse unregistrieren

*******************************************************************************/

void ReleasePreViewWnd()
{
    if(g_hThreadEvent) CloseHandle(g_hThreadEvent);
    if(g_aClass)       UnregisterClass((const char*)g_aClass,g_hInstance);

    DeleteCriticalSection(&g_csDataSyncronize);
    DeleteCriticalSection(&g_csMetaSyncronize);
}


/**************************************+****************************************
                                CreatePreViewWnd

public Function: erzeuge die eine mögliche Instance des Vorschaufensters !

return         : HWND hPreViewWnd oder NULL == Fehler

*******************************************************************************/

HWND CreatePreViewWnd(HWND wndParent,int x,int y,int cx,int cy)
{
   if(g_hwndPreView!=NULL) return NULL;          // es ist nur eine Instance dieses Fensters möglich !
                                                 // Fenster erzeugen !
   g_hwndPreView = CreateWindow(g_szPreViewClassName,
                                "",
                                WS_VISIBLE|WS_CHILD,
                                x,
                                y,
                                cx,
                                cy,
                                wndParent,
                                NULL,
                                g_hInstance,
                                NULL);

   if((g_bOneMetaFile)&&(g_hwndPreView!=NULL)&&(g_pCallback!=NULL))
   {                                             // neuen MetaFile-Render-Thread erzeugen
      _beginthread(CreateMetaFile,NULL,g_pCallback);
   }

   return g_hwndPreView;                         // Fensterhandle zurückgeben
}


/**************************************+****************************************
                           SetPreViewWndPageParameter

public Function: Übergebe die benötigten Papiermaße und Ränder für die
                 Vorschaudarstellung

*******************************************************************************/

void SetPreViewWndPageParameter(const PREVIEWPARAMETER * pP)
{
   bool update;

   if((g_hwndPreView)&&(IsWindow(g_hwndPreView)))
   {                                             // ein Update nur durchführen wenn sich Daten geändert haben !
      update = (g_PreViewParam.bValid          != pP->bValid);
      update|= (g_PreViewParam.siPageSize.cx   != pP->siPageSize.cx);
      update|= (g_PreViewParam.siPageSize.cy   != pP->siPageSize.cy);
      update|= (g_PreViewParam.rcMargin.left   != pP->rcMargin.left);
      update|= (g_PreViewParam.rcMargin.top    != pP->rcMargin.top);
      update|= (g_PreViewParam.rcMargin.right  != pP->rcMargin.right);
      update|= (g_PreViewParam.rcMargin.bottom != pP->rcMargin.bottom);
      update|= (g_PreViewParam.bMono           != pP->bMono);

      if(update)                                 // Daten haben sich geändert, übernehmen und neu Zeichnen etc.
      {
         g_PreViewParam = *pP;
                                                 // zur Verhinderung von divide by zero error
         if(pP->siPageSize.cx==0) g_PreViewParam.bValid = false;
         if(pP->siPageSize.cy==0) g_PreViewParam.bValid = false;

         CalcNewDrawParameter();
      }
   }
}


/**************************************+****************************************
                               SetPreViewMetaFile

public Function: Übergebe das Metafile für die Darstellung und die Art wie
                 die Vorschau dargestellt werden soll

*******************************************************************************/

void SetPreViewMetaFile(HENHMETAFILE hMetaFile,bool bType)
{
   EnterCriticalSection(&g_csMetaSyncronize);
   g_hMetaFile    = hMetaFile;                   // Metafile für die Vorschau
   g_bType        = bType;                       // true == ISO , false == ANISO
   g_bOneMetaFile = false;                       // der Besitzer des Metafiles ist der Aufrufer der DLL
   g_pCallback    = NULL;
   LeaveCriticalSection(&g_csMetaSyncronize);
}

/**************************************+****************************************
                               SetPreViewCallback

public Function: Übergebe den Pointer auf die Callbackfunktion zur Erzeugung
                 des MetaFiles für die Vorschau

*******************************************************************************/

void SetPreViewCallback(void * pFkt,bool bType)
{
   EnterCriticalSection(&g_csMetaSyncronize);
   g_hMetaFile    = NULL;
   g_bType        = bType;
   g_bOneMetaFile = true;                        // Metafile löschen nach Beendigung
   g_pCallback    = pFkt;
   LeaveCriticalSection(&g_csMetaSyncronize);
}




/**************************************+****************************************
                                 PreViewWndProc

private Function: die Fensternachrichtenbearbeitungsfunktion

*******************************************************************************/

LRESULT CALLBACK PreViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case WM_PAINT:
           OnPaint(hwnd);
           break;

      case WM_SIZE:           
           g_nSizeX = LOWORD(lParam);
           g_nSizeY = HIWORD(lParam);
           CalcNewDrawParameter();           
           break;

      case WM_APP+1:                             // MetaFile für Vorschau vom Thread fertig gerendert
           CalcNewDrawParameter();
           break;

      case WM_APP:                               // Bitmap für Vorschau vom Thread fertig gerendert
           if(g_DrawParam.hBitmap) 
              DeleteObject(g_DrawParam.hBitmap);
           g_DrawParam.hBitmap = (HBITMAP) lParam;
           InvalidateRect(hwnd,NULL,true);
           break;

      case WM_HELP:                              // die Hilfe für die Vorschau ausgeben
           {
              char       szText[1024];
              LPHELPINFO hi = (LPHELPINFO) lParam;
              POINT      p  = hi->MousePos;
              RECT       rc = g_DrawParam.rcPage;

              rc.right +=rc.left;
              rc.bottom+=rc.top;

              ScreenToClient(hwnd,&p);

              if(hi->iContextType==HELPINFO_WINDOW)
                 if(PtInRect(&rc,p))
                    if(LoadString(g_hInstance,IDHS_PREVIEWTEXT,szText,1024))
                       CEtsHelp::CreateContextWnd(szText,hi->MousePos.x,hi->MousePos.y);
           }
           break;


      case WM_NCDESTROY:                         // sollte eigentlich die letzte Nachricht sein
           {                                     // aber der Render-Thread könnte ja noch eine Posten
              BEGIN("WM_NCDESTROY");
              MSG msg;

              EnterCriticalSection(&g_csDataSyncronize);
              g_hwndPreView = NULL;              // das PreViewFenster ist ab jetzt nicht mehr vorhanden !
              LeaveCriticalSection(&g_csDataSyncronize);

              g_DrawParam.bValid    = false;     // die g_DrawParam sind nicht mehr gültig !
              g_PreViewParam.bValid = false;     // die g_PreViewParam sind nicht mehr gültig !

              if(g_DrawParam.hBitmap)            // noch eine Vorschaubitmap vorhanden,
              {                                  // dann diese löschen
                 DeleteObject(g_DrawParam.hBitmap);
                 g_DrawParam.hBitmap = NULL;
                 REPORT("bitmap deleted");
              }
                                                 // eine eventuell vorhandene WM_APP & WM_APP + 1 -Nachricht aus der Warteschlange besorgen
                                                 // wenn WM_APP ,die enthaltene Bitmap noch freigegeben

              _asm CLD;                          // Bugfix für PeekMessage
              while(PeekMessage( &msg, hwnd, WM_APP, WM_APP + 1, PM_REMOVE | PM_NOYIELD ))              
              {                                 
                 if(msg.message==WM_APP)
                 {
                    if(msg.lParam)               // die in der Nachricht enthaltene Bitmap löschen
                    {
                       DeleteObject((HBITMAP)msg.lParam);
                       REPORT("outstanding bitmap deleted");
                    }
                 }
                 else
                 {
                    REPORT("WM_APP + 1 message deleted");
                 }
              }
                                                 // g_hMetaFile wird auch im Thread verwendet
              EnterCriticalSection(&g_csMetaSyncronize);      
              if(g_bOneMetaFile)                 // muß das Metafile gelöscht werden ?
              {
                 if(g_hMetaFile)
                 {
                    DeleteEnhMetaFile(g_hMetaFile);
                    g_hMetaFile    = NULL;
                    g_bOneMetaFile = false;
                 }
                 REPORT("metafile deleted");
              }
              else g_hMetaFile = NULL;           // den Zugriff auf diese Metadatei verhindern !

              LeaveCriticalSection(&g_csMetaSyncronize);
           }
                                                 // ! kein break ! damit Nachricht auch an Windows weitergeleitet wird !
      default: return DefWindowProc (hwnd, msg, wParam, lParam);
   }
   return 0;                                     // Nachricht wurde bearbeitet
}

/**************************************+****************************************
                                    OnPaint

private Function: die Seitenvorschau einzeichnen

*******************************************************************************/

void OnPaint(HWND hwnd)
{
   PAINTSTRUCT ps;
   HBRUSH      hBrush;

   BeginPaint(hwnd,&ps);

   if(!g_DrawParam.bValid)
   {
      EndPaint(hwnd,&ps);
      return;
   }
   
   hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));

   if(hBrush)
   {
      SaveDC(ps.hdc);                            // den DC sichern 
                                                 // setze den Schwarzen Stift für die Papierumrahmung
      SelectObject(ps.hdc,GetStockObject(BLACK_PEN));
      SelectObject(ps.hdc,hBrush);               // Pinsel für den Schatten setzen
                                                 // setze den Zeichenstartpunkt
      SetViewportOrgEx(ps.hdc,g_DrawParam.rcPage.left ,g_DrawParam.rcPage.top,NULL);
                                                 // zeichne den Schatten
      PatBlt(ps.hdc,8,8,g_DrawParam.rcPage.right,g_DrawParam.rcPage.bottom,PATCOPY);
                                                 // zeichne die weiße Seite
      PatBlt(ps.hdc,1,1,g_DrawParam.rcPage.right -1,g_DrawParam.rcPage.bottom-1,WHITENESS);
                                                 // zeichne die Papierumrahmung (kein Rectangle wegen ATI-Bug)
      MoveToEx(ps.hdc,0                       ,0,NULL);
      LineTo  (ps.hdc,g_DrawParam.rcPage.right,0);
      LineTo  (ps.hdc,g_DrawParam.rcPage.right,g_DrawParam.rcPage.bottom);
      LineTo  (ps.hdc,0                       ,g_DrawParam.rcPage.bottom);
      LineTo  (ps.hdc,0                       ,0);

      if(g_DrawParam.hBitmap)                    // soll ein Metafile ausgebegen werden
      {
         HDC memdc = CreateCompatibleDC(ps.hdc);
         if(memdc)
         {
            SelectObject(memdc,g_DrawParam.hBitmap);

            BitBlt(ps.hdc,g_DrawParam.ptOffset.x,
                          g_DrawParam.ptOffset.y,
                          g_DrawParam.siArea.cx,
                          g_DrawParam.siArea.cy,
                          memdc,
                          0,
                          0,
                          SRCCOPY);
            DeleteDC(memdc);
         }
      }
                                                 // zeichne die Ränder ein
      DrawFocusRect(ps.hdc,&g_DrawParam.rcMargin); 

      RestoreDC(ps.hdc, -1);
      DeleteObject(hBrush);
   }

   EndPaint(hwnd,&ps);
}


/**************************************+****************************************
                              CalcNewDrawParameter

private Function: berechne alle Skalierungsdaten für die Seitenvorschau neu

*******************************************************************************/

void CalcNewDrawParameter()
{
   SIZE  page;
   POINT offset;
   int   factor;
   int   div;
   RECT  rcM;
   RECT  rcA;

   if(g_PreViewParam.bValid)                     // nur berechnen wenn Daten gültig !
   {                                             // Abzug von g_nSizeY : die 2  Pixel sind für die Umrahmung der Seitendarstellung
                                                 //                      die 10 Pixel sind für die Schattendarstellung
      factor = g_nSizeY-2-10;                    // skaliere so, daß Xmax und Ymax in die Vorschau-Y-Richtung hineinpassen !
      div    = g_PreViewParam.siPageSize.cy;     // zuerst Ymax als Teiler
      if(g_PreViewParam.siPageSize.cx > div)     // ist Xmax größer, dann Xmax als Teiler
         div = g_PreViewParam.siPageSize.cx;     // der Faktor g_nSizeY-2-10 ist immer kleiner als in X-Richtung !
                                                 // berechne die Seitenparameter
      page.cx  = 2 + MulDiv(g_PreViewParam.siPageSize.cx,factor,div);
      page.cy  = 2 + MulDiv(g_PreViewParam.siPageSize.cy,factor,div);
      offset.x = (g_nSizeX - 10 - page.cx) >>1;
      offset.y = (g_nSizeY - 10 - page.cy) >>1;
                                                 // berechne die Rändergrößen (bei Left,Top + 1 wegen Randabstand)
      rcM.left   = MulDiv(g_PreViewParam.rcMargin.left  ,factor,div) + 1;
      rcM.top    = MulDiv(g_PreViewParam.rcMargin.top   ,factor,div) + 1;
      rcM.right  = page.cx - MulDiv(g_PreViewParam.rcMargin.right ,factor,div);
      rcM.bottom = page.cy - MulDiv(g_PreViewParam.rcMargin.bottom,factor,div);

                                                 // berechne den Zeichenbereich für das Metafile
      rcA        = rcM;

      if(g_bType)                                // bitte Isometrisch
      {
         int width  = rcA.right -rcA.left;
         int height = rcA.bottom-rcA.top;

         if(width < height)                      // nach X-Skalieren
         {
            int offset  = (height-width)>>1;
            rcA.top    += offset;
            rcA.bottom  = rcA.top + width;
         }
         else                                    // nach Y-Skalieren
         {
            int offset  = (width-height)>>1;
            rcA.left   += offset;
            rcA.right   = rcA.left + height;
         }                          
      }
                                                 // übertrage die Daten in die Zeichenstruktur
      g_DrawParam.bValid          = true; 
      g_DrawParam.rcPage.left     = offset.x;
      g_DrawParam.rcPage.top      = offset.y; 
      g_DrawParam.rcPage.right    = page.cx; 
      g_DrawParam.rcPage.bottom   = page.cy;
      g_DrawParam.rcMargin        = rcM;
      g_DrawParam.ptOffset.x      = rcA.left;
      g_DrawParam.ptOffset.y      = rcA.top;

      EnterCriticalSection(&g_csDataSyncronize); // g_DrawParam werden vom Thread verwendet !                                                
         g_DrawParam.siArea.cx       = rcA.right - rcA.left;
         g_DrawParam.siArea.cy       = rcA.bottom- rcA.top;
         g_DrawParam.bMono           = g_PreViewParam.bMono;
      LeaveCriticalSection(&g_csDataSyncronize);

      EnterCriticalSection(&g_csMetaSyncronize);
      if(g_hMetaFile)                            // gibt es ein Vorschaubild ?
      {
         LeaveCriticalSection(&g_csMetaSyncronize);
                                                 // warten bis ein vorhergehender Bitmap-Render-Thread beendet ist
         WaitForSingleObject(g_hThreadEvent,INFINITE);
                                                 // neuen Render-Thread erzeugen (es soll nur 1. Render-Thread vorhanden sein)
         if(_beginthread(CreateDrawBitmap,NULL,NULL)==-1)
         {
            SetEvent(g_hThreadEvent);            // Thread konnte nicht erzeugt werden, Event signalisieren (wichtig !)
         }
      }
      else
      { 
         LeaveCriticalSection(&g_csMetaSyncronize);

         if(IsWindow(g_hwndPreView))             // Es gibt keinen Inhalt in der Vorschau !
            InvalidateRect(g_hwndPreView,NULL,true);
      }
   }
}


/**************************************+****************************************
                                CreateDrawBitmap

private Function: Render-Thread, erzeugt die Bitmap für die Vorschau im
                  Hintergrund

*******************************************************************************/

void _cdecl CreateDrawBitmap(LPVOID)
{
   BEGIN("CreateDrawBitmap");

   EnterCriticalSection(&g_csDataSyncronize);
      int          width  = g_DrawParam.siArea.cx;
      int          height = g_DrawParam.siArea.cy;
      bool         bMono  = g_DrawParam.bMono;
   LeaveCriticalSection(&g_csDataSyncronize);

   bool    sendbitmap = false;                   // neu 19.11.1999 für Win2000 Bitmap wurde erstellt
   HDC     hdc        = GetDC(NULL);             // Desktop Wnd !
   HBITMAP hBit;                                 // die erstellte Bitmap
   HBITMAP hBitMono;                             // die Bitmap in Graustufen

   if(hdc)
   {
      HDC memdc = CreateCompatibleDC(hdc);

      if(memdc)
      {
         SaveDC(memdc);

         hBit = CreateCompatibleBitmap(hdc,width,height);

         if(hBit)                                // zuerst immer in Farbe rendern (wegen Win95/98)
         {                                       // Rendern und Farbanpassung dauert auf Win9X sehr lange !
            RECT    rc   = {0,0,width,height};

            SelectObject(memdc,hBit);
            PatBlt(memdc,0,0,width,height,WHITENESS);

            EnterCriticalSection(&g_csMetaSyncronize);
            if(g_hMetaFile)
               PlayEnhMetaFile(memdc,g_hMetaFile,&rc);
            LeaveCriticalSection(&g_csMetaSyncronize);

			   sendbitmap = true;                   // Ok, Bitmap ist erstellt worden
         }

         if(bMono & sendbitmap)                  // neu 6.3.2000 Farben nach Graustufen wandeln
         {
            struct BiDa                          // Trick, ist doch nicht schlecht oder ?
            {
               BITMAPINFO b;
               RGBQUAD    c[256];
            };

            void       * pBits;
            BiDa         bInfo;                  // Speicher für BITMAPINFO (BITMAPINFOHEADER + COLORTABLE)

            bInfo.b.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
            bInfo.b.bmiHeader.biWidth         = width;
            bInfo.b.bmiHeader.biHeight        = height;
            bInfo.b.bmiHeader.biPlanes        = 1;
            bInfo.b.bmiHeader.biBitCount      = 8;
            bInfo.b.bmiHeader.biCompression   = BI_RGB;
            bInfo.b.bmiHeader.biSizeImage     = 0;
            bInfo.b.bmiHeader.biXPelsPerMeter = 100;
            bInfo.b.bmiHeader.biYPelsPerMeter = 100;
            bInfo.b.bmiHeader.biClrUsed       = 256;
            bInfo.b.bmiHeader.biClrImportant  = 256;

            ZeroMemory(bInfo.b.bmiColors,256*sizeof(RGBQUAD));

            for (int i=1;i<256;i++)
            {
               bInfo.b.bmiColors[i].rgbBlue    = i;
               bInfo.b.bmiColors[i].rgbGreen   = i;
               bInfo.b.bmiColors[i].rgbRed     = i;
            } 
                                                 // erzeuge ein 256 Farb Bitmap mit 256 Graustufen
            hBitMono = CreateDIBSection(memdc,(BITMAPINFO*) &bInfo,DIB_RGB_COLORS,&pBits,NULL,0);

            if(hBitMono)                         // Bitmap erzeugt ?
            {
               HDC memdc2 = CreateCompatibleDC(hdc);

               if(memdc2)
               {
                  SaveDC(memdc2);
                  //SetStretchBltMode(memdc2,HALFTONE);
                  //SetBrushOrgEx(memdc2,0,0,NULL);

                  SelectObject(memdc2,hBitMono);
                  StretchBlt(memdc2,0,0,width,height,memdc,0,0,width,height,SRCCOPY);
                  
                  RestoreDC(memdc2,-1);
                  DeleteDC(memdc2);
                  DeleteObject(hBit);
                  hBit = hBitMono;               // Bitmap Mono zurückgeben !
               }                  
            }
         }

         RestoreDC(memdc,-1);
         DeleteDC(memdc);
      }
      ReleaseDC(NULL,hdc);
   }

   if(sendbitmap)                                // kein Fehler also neue Bitmap senden !
   {
      EnterCriticalSection(&g_csDataSyncronize);
         if(IsWindow(g_hwndPreView))
         {
            PostMessage(g_hwndPreView,WM_APP,0,(LPARAM) hBit);
            REPORT("new Bitmap posted !");
         }
         else
         {
            DeleteObject(hBit);
            REPORT("new Bitmap destroyed !");
         }
      LeaveCriticalSection(&g_csDataSyncronize);
   }

   SetEvent(g_hThreadEvent);                     // Ich bin Fertig !

   _endthread();
}

/**************************************+****************************************
                                 CreateMetaFile

private Function: Render-Thread, erzeugt das Metafile für die Vorschau durch
                  eine Callback-Funktion im Hintergrund

*******************************************************************************/

void _cdecl CreateMetaFile(LPVOID pParam)
{
   BEGIN("CreateMetaFile");

   HENHMETAFILE (*pFkt)() = (HENHMETAFILE (*)()) pParam;
   HENHMETAFILE hMeta;

   hMeta = pFkt();                         // Metafile durch Funktion erzeugen lassen

   EnterCriticalSection(&g_csDataSyncronize);
      if(IsWindow(g_hwndPreView))
      {
         EnterCriticalSection(&g_csMetaSyncronize);
            g_hMetaFile = hMeta;
         LeaveCriticalSection(&g_csMetaSyncronize);
         PostMessage(g_hwndPreView,WM_APP + 1,0,0);
         REPORT("new metafile stored !");
      }
      else
      {
         DeleteEnhMetaFile(hMeta);
         REPORT("new metafile deleted !");
      }
   LeaveCriticalSection(&g_csDataSyncronize);

   _endthread();
}
