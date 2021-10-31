// Test.cpp : Definiert den Einsprungpunkt für die Anwendung.
//

#include "stdafx.h"
#include "resource.h"
#include "CEtsDiv.h"

#define MAX_LOADSTRING 100

// Globale Variablen:
HINSTANCE hInst;					// aktuelle Instanz
TCHAR szTitle[MAX_LOADSTRING];								// Text der Titelzeile
TCHAR szWindowClass[MAX_LOADSTRING];								// Text der Titelzeile

// Vorausdeklarationen von Funktionen, die in diesem Code-Modul enthalten sind:
ATOM				MyRegisterClass( HINSTANCE hInstance );
BOOL				InitInstance( HINSTANCE, int );
LRESULT CALLBACK	WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK	About( HWND, UINT, WPARAM, LPARAM );

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
 	// ZU ERLEDIGEN: Fügen Sie hier den Code ein.
	MSG msg;
	HACCEL hAccelTable;

	// Globale Zeichenfolgen initialisieren
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Initialisierung der Anwendung durchführen:
	if( !InitInstance( hInstance, nCmdShow ) ) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TEST);


	// Hauptnachrichtenschleife:
	while( GetMessage(&msg, NULL, 0, 0) ) 
	{
		if( !TranslateAccelerator (msg.hwnd, hAccelTable, &msg) ) 
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	return msg.wParam;
}



//
//  FUNKTION: MyRegisterClass()
//
//  AUFGABE: Registriert die Fensterklasse.
//
//  KOMMENTARE:
//
//    Diese Funktion und ihre Verwendung sind nur notwendig, wenn dieser Code
//    mit Win32-Systemen vor der 'RegisterClassEx'-Funktion kompatibel sein soll,
//    die zu Windows 95 hinzugefügt wurde. Es ist wichtig diese Funktion aufzurufen,
//    damit der Anwendung kleine Symbole mit den richtigen Proportionen zugewiesen
//    werden.
//
ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_TEST);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_TEST;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNKTION: InitInstance(HANDLE, int)
//
//   AUFGABE: Speichert die Instanzzugriffsnummer und erstellt das Hauptfenster
//
//   KOMMENTARE:
//
//        In dieser Funktion wird die Instanzzugriffsnummer in einer globalen Variable
//        gespeichert und das Hauptprogrammfenster erstellt und angezeigt.
//
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
   HWND hWnd;

   hInst = hInstance; // Instanzzugriffsnummer in unserer globalen Variable speichern

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if( !hWnd ) 
   {
      return FALSE;
   }

   ShowWindow( hWnd, nCmdShow );
   UpdateWindow( hWnd );

   return TRUE;
}


bool DrawFunction(ETSDIV_SCALE*pScale)
{
   TextOut((HDC)pScale->pParam, (int)pScale->dPosX, (int)pScale->dPosY-7, pScale->pszText, strlen(pScale->pszText));
   POINT point;
   MoveToEx((HDC)pScale->pParam, 0, (int)pScale->dPosY, &point);
   LineTo(  (HDC)pScale->pParam, (int)pScale->dPosX, (int)pScale->dPosY);
   return true;
}

//
//  FUNKTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  AUFGABE:  Verarbeitet Nachrichten für das Hauptfenster.
//
//  WM_COMMAND	- Anwendungsmenü verarbeiten
//  WM_PAINT	- Hauptfenster darstellen
//  WM_DESTROY	- Beendigungsnachricht ausgeben und zurückkehren
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
   RECT         rcClient;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
   static ETSDIV_NUTOTXT Ntx = {0.0, ETSDIV_NM_STD|ETSDIV_NM_GROUPING,2,16,0};

	switch( message ) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Menüauswahlen analysieren:
			switch( wmId ) 
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
            case ID_FILE_NTXDIALOG:
               CEtsDiv::NumberRepresentationDlg(&Ntx, hWnd);
               Ntx.fvalue = 1;
               Ntx.nmode  |= ETSDIV_NM_AUTO_SCI;
               CEtsDiv::UnitConvertDlg(&Ntx, hWnd);
               break;
				case IDM_EXIT:
				   DestroyWindow( hWnd );
				   break;
				default:
				   return DefWindowProc( hWnd, message, wParam, lParam );
			}
			break;
		case WM_PAINT:
         GetClientRect( hWnd, &rcClient );
			hdc = BeginPaint (hWnd, &ps);
			// ZU ERLEDIGEN: Hier beliebigen Code zum Zeichnen hinzufügen...
         ETSDIV_SCALE scale;
         char text[16];
         ZeroMemory(&scale, sizeof(scale));
         scale.div.l1 = rcClient.top+10;
         scale.div.l2 = rcClient.bottom-10;
         scale.div.f1 = -4000096.0;
         scale.div.f2 = -1000050;
         scale.div.stepwidth = 25;
         scale.div.divfnc = ETSDIV_LINEAR;
         scale.ntx = Ntx;
         scale.pszText    = text;
         scale.pParam     = hdc;
         scale.pFkt       = DrawFunction;
         scale.dPosX      = 20;
         scale.dwFlags    = ETSDIV_CALC_GRADIENT|ETSDIV_CALC_Y_POS|ETSDIV_FORMAT_TEXT|ETSDIV_CALC_ZEROVALUE;

         CEtsDiv::DrawScale(&scale);

			EndPaint( hWnd, &ps );
			break;
		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
   }
   return 0;
}

// Nachrichtenbehandlungsroutine für "Info"-Feld.
LRESULT CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
   return FALSE;
}
