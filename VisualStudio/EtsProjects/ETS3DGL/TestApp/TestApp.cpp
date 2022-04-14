// TestApp.cpp : Definiert den Einsprungpunkt für die Anwendung.
//

#include "stdafx.h"
#include "resource.h"
#include "TestCara3D.h"
#include "CVector.h"

bool     g_bDoReport;
#define  ETS_DEBUGNAME "TestApp"
#define  ETSDEBUG_INCLUDE 
#define  ETSDEBUG_CREATEDATA ETS_DEBUGNAME, &g_bDoReport
#include "CEtsDebug.h"


// Vorausdeklarationen von Funktionen, die in diesem Code-Modul enthalten sind:
LRESULT CALLBACK	WindowProc( HWND, UINT, WPARAM, LPARAM);
ATOM				MyRegisterClass( HINSTANCE hInstance );
BOOL				InitInstance( HINSTANCE, int );
LRESULT CALLBACK	About( HWND, UINT, WPARAM, LPARAM );

HINSTANCE   g_hInstance;                     // aktuelle Instanz
#define MAX_LOADSTRING 100
// Globale Variablen:
TCHAR       szTitle[MAX_LOADSTRING];         // Text der Titelzeile
TCHAR       szWindowClass[MAX_LOADSTRING];   // Text der Titelzeile

//double  dFact  = 110;
//POINT   ptOff  = {-100, -200};
double  dFact  = 270;
POINT   ptOff  = {-920, -1070};
bool    bSlow  = false;

void DrawFanDC(int nCount, CVector *pV, void *pParam)
{
   int i=0;
   ::MoveToEx((HDC)pParam, (int)(dFact*Vx(pV[i]))+ptOff.x, (int)(dFact*Vz(pV[i]))+ptOff.y, NULL);
   ::Ellipse((HDC)pParam, (int)(dFact*Vx(pV[i]))+ptOff.x-5, (int)(dFact*Vz(pV[i]))+ptOff.y-5, (int)(dFact*Vx(pV[i]))+ptOff.x+5, (int)(dFact*Vz(pV[i]))+ptOff.y+5);
   for (i=1; i<nCount; i++)
   {
      ::LineTo((HDC)pParam, (int)(dFact*Vx(pV[i]))+ptOff.x, (int)(dFact*Vz(pV[i]))+ptOff.y);
      if (i>=2)
      {
         CVector pC = (pV[0]+pV[i]+pV[i-1])/3.0;
         char text[32];
         wsprintf(text,"%d", i-1);
         ::TextOut((HDC)pParam, (int)(dFact*Vx(pC))+ptOff.x, (int)(dFact*Vz(pC))+ptOff.y, text, strlen(text));
         ::LineTo((HDC)pParam, (int)(dFact*Vx(pV[0]))+ptOff.x, (int)(dFact*Vz(pV[0]))+ptOff.y);
         ::MoveToEx((HDC)pParam, (int)(dFact*Vx(pV[i]))+ptOff.x, (int)(dFact*Vz(pV[i]))+ptOff.y, NULL);
      }
      if (bSlow) Sleep(500);
   }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
 	// ZU ERLEDIGEN: Fügen Sie hier den Code ein.
	MSG msg;
	HACCEL hAccelTable;
   CEts3DGL c3DGL;

	// Globale Zeichenfolgen initialisieren
	LoadString(hInstance, IDS_APP_TITLE, szTitle      , MAX_LOADSTRING);
	LoadString(hInstance, IDC_TESTAPP  , szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

   // Initialisierung der Anwendung durchführen:
	if( !InitInstance( hInstance, nCmdShow ) ) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TESTAPP);

	// Hauptnachrichtenschleife:
	while( GetMessage(&msg, NULL, 0, 0) ) 
	{
		if( !TranslateAccelerator (msg.hwnd, hAccelTable, &msg) ) 
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
   ETSDEBUGEND;
   c3DGL.Destructor();
	return msg.wParam;
}

/***************************************************************************************
*  FUNKTION: MyRegisterClass()                                                         *
*  AUFGABE: Registriert die Fensterklasse.                                             *
*  KOMMENTARE:                                                                         *
*    Diese Funktion und ihre Verwendung sind nur notwendig, wenn dieser Code           *
*    mit Win32-Systemen vor der 'RegisterClassEx'-Funktion kompatibel sein soll,       *
*    die zu Windows 95 hinzugefügt wurde. Es ist wichtig diese Funktion aufzurufen,    *
*    damit der Anwendung kleine Symbole mit den richtigen Proportionen zugewiesen      *
*    werden.                                                                           *
***************************************************************************************/
ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;

	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style			 = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	 = (WNDPROC)WindowProc;
	wcex.cbClsExtra	 = 0;
	wcex.cbWndExtra	 = 0;
	wcex.hInstance		 = hInstance;
	wcex.hIcon			 = LoadIcon(hInstance, (LPCTSTR)IDI_TESTAPP);
	wcex.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	 = (LPCSTR)IDC_TESTAPP;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm		 = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

/***************************************************************************************
*   FUNKTION: InitInstance(HANDLE, int)                                                *
*   AUFGABE: Speichert die Instanzzugriffsnummer und erstellt das Hauptfenster         *
*   KOMMENTARE:                                                                        *
*        In dieser Funktion wird die Instanzzugriffsnummer in einer globalen Variable  *
*        gespeichert und das Hauptprogrammfenster erstellt und angezeigt.              *
***************************************************************************************/
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
   HWND hWnd;

   g_hInstance = hInstance; // Instanzzugriffsnummer in unserer globalen Variable speichern

   hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if( !hWnd ) 
   {
      return FALSE;
   }

   ShowWindow( hWnd, nCmdShow );
   UpdateWindow( hWnd );

   return TRUE;
}

/*********************************************************************
*  FUNKTION: WindowProc(HWND, unsigned, WORD, LONG)                  *
*  AUFGABE:  Verarbeitet Nachrichten für das Hauptfenster.           *
*  WM_COMMAND	- Anwendungsmenü verarbeiten                          *
*  WM_PAINT	- Hauptfenster darstellen                                *
*  WM_DESTROY	- Beendigungsnachricht ausgeben und zurückkehren      *
*********************************************************************/
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
   static CTestCara3D *pTC3D = NULL;
	switch( message ) 
	{
		case WM_COMMAND:
      {
//         REPORT("WM_COMMAND");
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam);
         switch (wmId)
         {
            case ID_EDIT_COPY_TO_META:
            if (pTC3D)
            {
               CVector pv;
               pTC3D->TransformPoints(&pv, 1, true);
               ETS3DGL_Draw draw;
               pTC3D->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_ENHMETA, (LPARAM)&draw);
               if (draw.hMeta)
               {
                  HDC hdc = GetDC(NULL);
                  if (hdc)
                  {
                     PlayEnhMetaFile(hdc, draw.hMeta, &draw.rcFrame);
                     ReleaseDC(NULL, hdc);
                  }
                  DeleteEnhMetaFile(draw.hMeta);
               }
            } break;
            case ID_EDIT_COPY_VIEW:
            if (pTC3D)
            {
               HGLOBAL hGl = NULL;
               pTC3D->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_HGLOBAL, (LPARAM)&hGl);
               if (hGl && OpenClipboard(hWnd) && EmptyClipboard())
               {
                  if (SetClipboardData(CF_DIB, hGl) == NULL)
                     GlobalFree(hGl);
                  CloseClipboard();
               }
            } break;
            case ID_EDIT_COPY_TO_BITMAP:
            if (pTC3D)
            {
               pTC3D->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_FILE, (LPARAM)"test.bmp");
            } break;
            case ID_EDIT_COPY_TO_DC:
            if (pTC3D)
            {
               ETS3DGL_Draw draw;
               draw.rcFrame.left   = 0;
               draw.rcFrame.top    = 0;
               draw.rcFrame.right  = 100;
               draw.rcFrame.bottom = 100;
               draw.hDC = GetDC(NULL);
               if (draw.hDC)
               {
                  pTC3D->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_HDC, (LPARAM)&draw);
                  ReleaseDC(NULL, draw.hDC);
               }
            } break;
				case IDM_EXIT:
            {
				   DestroyWindow( hWnd );
            } break;
            case ID_OPTIONS_KILLGRAFICS:
            {
               if (pTC3D)
               {
                  delete pTC3D;
               }
               pTC3D = NULL;
            } break;
            case ID_OPTIONS_STARTGRAFICS:
            {
               REPORT("Create");
			      RECT rc;
			      GetClientRect( hWnd, &rc);
               pTC3D = new CTestCara3D;
               if (pTC3D)
               {
                  char szFilePath[] = "bmp.bmp";
                  pTC3D->LoadBitmap(szFilePath);
                  pTC3D->SetModes(ETS3D_DC_DIRECT_2, ETS3D_DC_CLEAR);
                  pTC3D->SetModes(ETS3D_DESTROY_AT_DLL_TERMINATION, 0);
                  if (!pTC3D->Create("Hallo Welt", WS_VISIBLE|WS_CHILD, &rc, hWnd))
                  {
                     delete pTC3D;
                     pTC3D = NULL;
                     return -1;
                  }
               }
            } break;
            default:
            if (pTC3D)
            {
               pTC3D->OnCommand(wmId, wmEvent, (HWND)lParam);
            } break;
         }
      } break;
      case WM_MOUSEMOVE:
         break;
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         ::BeginPaint(hWnd, &ps);
         ::EndPaint(hWnd, &ps);
         ::SetFocus(hWnd);
      }   break;
      case WM_CREATE:
      {
         REPORT("Create");
			RECT rc;
			GetClientRect( hWnd, &rc);
         pTC3D = new CTestCara3D;
         if (pTC3D)
         {
            char szFilePath[] = "bmp.bmp";
            pTC3D->LoadBitmap(szFilePath);
            pTC3D->SetModes(ETS3D_DC_DIRECT_2, ETS3D_DC_CLEAR);
            pTC3D->SetModes(ETS3D_DESTROY_AT_DLL_TERMINATION, 0);
            if (!pTC3D->Create("Hallo Welt", WS_VISIBLE|WS_CHILD, &rc, hWnd))
            {
               delete pTC3D;
               pTC3D = NULL;
               return -1;
            }
         }
         return 0;
      }
      case WM_ENTERSIZEMOVE: case WM_EXITSIZEMOVE:
         if (pTC3D) pTC3D->SendMessage(message, wParam, lParam);
         break;
      case WM_SIZE:
         if (pTC3D) pTC3D->MoveWindow(0, 0, LOWORD(lParam), HIWORD(lParam), false);
         break;
		case WM_DESTROY:
      {
         REPORT("Destroy");
         if (pTC3D)
         {
            delete pTC3D;
         }
         pTC3D = NULL;
			PostQuitMessage( 0 );
      } break;
		default:
            return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


/*
      case WM_KEYDOWN:
         switch (wParam)
         {
            case 40:   ptOff.y+=10;  break;
            case 38:   ptOff.y-=10;  break;
            case 37:   ptOff.x-=10;  break;
            case 39:   ptOff.x+=10;  break;
            case 107:  dFact+=10;    break;
            case 109:  dFact-=10;    break;
            case 13:   bSlow = !bSlow; break;
         }
         ::InvalidateRect(hWnd, NULL, true);
         break;
            case ID_EDIT_COPY_TO_META:
            if (pTC3D)
            {
               ETS3DGL_Draw draw;
               pTC3D->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_ENHMETA, (LPARAM)&draw);
               if (draw.hMeta)
               {
                  HDC hdc = GetDC(NULL);
                  if (hdc)
                  {
                     PlayEnhMetaFile(hdc, draw.hMeta, &draw.rcFrame);
                     ReleaseDC(NULL, hdc);
                  }
                  DeleteEnhMetaFile(draw.hMeta);
               }
            } break;
            case ID_EDIT_COPY_VIEW:
            if (pTC3D)
            {
               HGLOBAL hGl = NULL;
               pTC3D->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_HGLOBAL, (LPARAM)&hGl);
               if (hGl && OpenClipboard(hWnd) && EmptyClipboard())
               {
                  if (SetClipboardData(CF_DIB, hGl) == NULL)
                     GlobalFree(hGl);
                  CloseClipboard();
               }
            } break;
            case ID_EDIT_COPY_TO_BITMAP:
            if (pTC3D)
            {
               pTC3D->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_FILE, (LPARAM)"test.bmp");
            } break;
            case ID_EDIT_COPY_TO_DC:
            if (pTC3D)
            {
               ETS3DGL_Draw draw;
               draw.rcFrame.left   = 0;
               draw.rcFrame.top    = 0;
               draw.rcFrame.right  = 100;
               draw.rcFrame.bottom = 100;
               draw.hDC = GetDC(NULL);
               if (draw.hDC)
               {
                  pTC3D->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_HDC, (LPARAM)&draw);
                  ReleaseDC(NULL, draw.hDC);
               }
            } break;
            case ID_EDIT_TESTTRIANGLE:
            if (pTC3D)
            {
               RECT rcClient;
               ::GetClientRect(hWnd, &rcClient);
//               CVector p1(rcClient.left, rcClient.top, 0), p2(rcClient.left, rcClient.bottom, 0), p3(rcClient.right, rcClient.top, 0);
               CVector p1(2.00, 2.41, 8.38), p2(6.00, 2.41, 2.78), p3(6.00, 2.41, 3.71);
               ETS3DGL_Fan Fan;
               Fan.dStep = 1.3475871426606;
               Fan.pFn   = DrawFanDC;
               HDC hdc = ::GetDC(hWnd);
               ::SetTextAlign(hdc, TA_BASELINE|TA_CENTER);
               Fan.pParam = hdc;
               pTC3D->DrawTriangleFan(&p1, &p2, &p3, &Fan);
               ::ReleaseDC(hWnd, hdc);
            }
            case ID_FILE_PRINT:
            if (pTC3D)
            {
               HDC hDC = ::CreateDC("WINSPOOL", "HP DeskJet 550C Printer", NULL, NULL);
               DOCINFO di;
               ZeroMemory(&di, sizeof(DOCINFO));
               di.cbSize      = sizeof(DOCINFO);
               di.lpszDocName = "Document";
               StartDoc(hDC, &di);
               StartPage(hDC);
               TextOut(hDC, 100,  100, "Obere Ecke", 10);
               TextOut(hDC,2100, 2100, "Untere Ecke", 11);
               ETS3DGL_Draw draw;
               draw.hDC = hDC;
               pTC3D->SendMessage(WM_COPY_FRAME_BUFFER, COPY_TO_PRINTER, (LPARAM)&draw);

               EndPage(hDC);
               EndDoc(hDC);
               if (hDC) DeleteDC(hDC);
            } break;
*/
