/********************************************************************/
/*                                                                  */
/*  Testprogramm für DLL CARADIV für die CARA-Programmodule         */
/*                                                                  */
/*                 Abfrage der Versionsinformationen                */
/*                                                                  */
/*     programmed by Rolf Ehlers                                    */
/*                                                                  */
/*     Version 1.0            15.05.98                              */
/*                                                                  */
/********************************************************************/




#define  STRICT

#include <WINDOWS.H>
#include "C:\Daten\MyProjects\CaraDiv\CCaraDiv\CCaraDiv.h"



// Funktionsprototypen
LRESULT CALLBACK  WindowFunc( HWND, UINT, WPARAM, LPARAM );

HINSTANCE   g_hThisInst;


int WINAPI WinMain( HINSTANCE hThisInst,
				        HINSTANCE hPrevInst,
   				     LPSTR     lpszArgs,
				        int       nWinMode )
{
   MSG         msg;
   WNDCLASSEX	 wcl;
   HWND        hWnd;


   g_hThisInst = hThisInst;
   
   // Fensterklasse definieren
   wcl.hInstance	= hThisInst;		      // Handle zu dieser Instance
   wcl.lpszClassName	= "Rolf_Win";	             // Name der Fensterklasse (Programmfenster)
   wcl.lpfnWndProc	= WindowFunc;	   	      // Fensterfunktion
   wcl.style		= CS_VREDRAW|CS_HREDRAW;   // Fensterstil (Standardstil)
   wcl.cbSize		= sizeof(WNDCLASSEX);      // Größe von WNDCLASSEX
   wcl.hIcon		= NULL;                    // Großes Icon-Symbol
   wcl.hIconSm	= NULL;                    // Kleines Icon-Symbol
   wcl.hCursor	= LoadCursor(NULL, IDC_ARROW); // Cursorstil
   wcl.lpszMenuName	= NULL;     		      // Pointer auf Menu in .rc-Datei
   wcl.cbClsExtra		= 0;                // keine Extras
   wcl.cbWndExtra		= 0;	             // keine extra Informationen
   wcl.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH); // weißer Hintergrund

   // Fensterklasse registrieren
   if( !RegisterClassEx( &wcl ) )	return (0);

   // Haupt-Fenster anlegen
   hWnd = CreateWindowEx( 0, "Rolf_Win",        // Name der Fensterklasse (Programmfenster)
                              "",               // Titel des Fensters
                              WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, // Fensterstil normal
                              CW_USEDEFAULT,    // x-Koordinate
                              CW_USEDEFAULT,    // y-Koordinate
                              308,              // Fensterbreite
                              327,              // Fensterhöhe
                              HWND_DESKTOP,     // kein übergeordnetes Fenster
                              NULL,             // kein Menu
                              hThisInst,        // Handle auf diese Programminstance
                              NULL );           // keine zusätzlichen Argumente

   
   // Das Haupt-Fenster anzeigen
   ShowWindow( hWnd, nWinMode );
   UpdateWindow( hWnd );

   // Nachrichtenschleife
   while( GetMessage( &msg, NULL, 0, 0 ) )
   {
      TranslateMessage( &msg );                 // Tastatureingabe zulassen
      DispatchMessage( &msg );                  // Kontrolle an Windows übergeben
   }

   return (msg.wParam);
}

bool DrawFunction(ETSDIV_SCALE*pScale)
{
   TextOut((HDC)pScale->pParam, (int)pScale->dPosX, (int)pScale->dPosY-7, pScale->pszText, strlen(pScale->pszText));
   POINT point;
   MoveToEx((HDC)pScale->pParam, 0, (int)pScale->dPosY, &point);
   LineTo(  (HDC)pScale->pParam, (int)pScale->dPosX, (int)pScale->dPosY);
   return true;
}


/********************************************************************/
/*                                                                  */
/*            Call-Back-Funktion für das Haupt-Fenster              */
/*   Diese Fensterfunktion zur definierten Fensterklasse "ThoWin"   */
/*              erhält Nachrichten von Windows 95                   */
/*                                                                  */
/********************************************************************/
LRESULT CALLBACK WindowFunc( HWND   hwnd, UINT   message, WPARAM wParam, LPARAM lParam)
{
   PAINTSTRUCT  paintstruct;
   HDC          hdc;
   RECT         rcClient;
   int          nMatIndex = 3;
   CEtsDiv      caraDiv;
   
   switch( message )
   {
      case WM_DESTROY:				         		 // Programm beenden
         PostQuitMessage( 0 );
         break;

      case WM_CLOSE:
         DestroyWindow( hwnd );
         caraDiv.Destructor();
         break;

      case WM_LBUTTONDOWN:
         {
            double dAngle= 1.2, dSin, dCos;
            CEtsDiv::GetSineCosineOfAngle(10, ETSDIV_POLAR_FROM_INDEX, dAngle, dSin, dCos);
         }
         break;

      case WM_PAINT:
         GetClientRect( hwnd, &rcClient );
         hdc = BeginPaint( hwnd, &paintstruct );
         ETSDIV_SCALE scale;
         char text[16];
         ZeroMemory(&scale, sizeof(scale));
         scale.div.l1 = rcClient.top+10;
         scale.div.l2 = rcClient.bottom-10;
         scale.div.f1 = 5;
         scale.div.f2 = 40960;
         scale.div.stepwidth = 30;
         scale.div.divfnc = ETSDIV_FREQUENCY;
         scale.ntx.nmaxl  = 16;
         scale.ntx.nround = 1;
         scale.ntx.nmode  = ETSDIV_NM_STD;
         scale.pszText    = text;
         scale.pParam     = hdc;
         scale.pFkt       = DrawFunction;
         scale.dPosX      = 20;
         scale.dwFlags    = ETSDIV_CALC_GRADIENT|ETSDIV_CALC_Y_POS|ETSDIV_FORMAT_TEXT|ETSDIV_CALC_ZEROVALUE;

         CEtsDiv::DrawScale(&scale);

         EndPaint( hwnd, &paintstruct );
         break;

         default:	            							 // Alle anderen Nachrichten werden von Windows verarbeitet
	  return ( DefWindowProc( hwnd, message, wParam, lParam ) );
	}

	return (0);
}


