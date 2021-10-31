// CTestList.cpp : Definiert den Einsprungpunkt für die Anwendung.
//

#include "stdafx.h"
#include "resource.h"
#include "CEtsList.h"                            // Headerdatei für CEtsList einbinden
#include "Curve.h"                               // Headerdatei für Curve einbinden
#include "Plot.h"                                // Headerdatei für Plot einbinden
#include "FileHeader.h"                          // Headerdatei für FileHeader einbinden
#include "CVector.h"                          // Headerdatei für FileHeader einbinden

#define MAX_LOADSTRING 100
#define WM_EDIT_READY  32772

// Globale Variablen:
HINSTANCE hInst;					// aktuelle Instanz
TCHAR szTitle[MAX_LOADSTRING];						// Text der Titelzeile
TCHAR szWindowClass[MAX_LOADSTRING];				// Text der Titelzeile
char szPath[MAX_PATH*2];

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
	LoadString(hInstance, IDC_CTESTLIST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Initialisierung der Anwendung durchführen:
	if( !InitInstance( hInstance, nCmdShow ) ) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CTESTLIST);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CTESTLIST);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_CTESTLIST;
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

void TestCurve(HWND hwnd)
{
   int            nAnz = 1;
   CPlot          plot;
   CFileHeader    FileHeader( "EDT", "2000", 110 );

   plot.SetSize(nAnz);
   CCurve *pCurve =plot.GetCurve(0);
   pCurve->SetFormat(CF_SINGLE_SHORT);
   pCurve->SetSize(118);
   pCurve->SetXOrigin(5);
   pCurve->SetXStep(1.0/9.0);
   pCurve->SetDescription("curve");
   pCurve->SetWriteProtect(false);
   pCurve->ChangeFormat(CF_SHORT);

   plot.SetHeading("Überschrift");
   plot.SetXDivision(CCURVE_FREQUENCY);
   plot.SetYDivision(CCURVE_LINEAR);
   plot.SetXUnit("Hz");
   plot.SetYUnit("dB");
   plot.SetXNumMode(NM_STD);
   plot.SetYNumMode(NM_SCI);
/*
   DWORD  dwID;
   HANDLE hProcess;
   if (Call2DView(NULL, nAnz, &plot, true, true, hwnd, WM_EDIT_READY,
      TARGET_FUNCTION_CHANGEABLE, szPath, &dwID, &hProcess))
   {

   }
*/
}

void TestList(HWND hwnd)
{
   CEtsList list;
   int y, x , nlen;
   char * pText;
   WIN32_FIND_DATA   w32fd;
   HANDLE hFindFile = FindFirstFile("C:\\W98\\System\\*.exe", &w32fd);


   if (hFindFile != INVALID_HANDLE_VALUE)
   {
      do
      {
         nlen = strlen(w32fd.cFileName);
         if (nlen)
         {
            pText = new char[nlen+1];
            strcpy(pText, w32fd.cFileName);
            pText[nlen] = 0;
            list.ADDHead(pText);
         }
      }
      while (FindNextFile(hFindFile, &w32fd));
      FindClose(hFindFile);
   }
   HDC hdc = GetDC(hwnd);
   RECT rc;
   GetClientRect(hwnd, &rc);
   y =  5;
   x = 10;


   pText = new char[32];
   strcpy(pText, "Hallöle");
   
   for (pText=(char*)list.GetFirst(); pText != NULL; pText=(char*) list.GetNext())
   {
      TextOut(hdc, x, y+=15, pText, strlen(pText));
      if (y > rc.bottom)
      {
         y  = 5;
         x += 150;
      }
   }

//   int n = list.FindElement((SORTFUNCTION)strcmp, "Text Nr.: 22");
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

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
				case IDM_EXIT:
				   DestroyWindow( hWnd );
				   break;
            case IDM_TESTLIST:
            {
               TestList(hWnd);
            } break;
            case IDM_TESTCURVE:
            {
               TestCurve(hWnd);
            }
				default:
				   return DefWindowProc( hWnd, message, wParam, lParam );
			}
			break;
         case WM_EDIT_READY:
            {
               int  nAnz;
               bool bCRCOK;
               CPlot *pPlots = ReadPlots(szPath, nAnz, bCRCOK);
               if (pPlots)
               {
                  delete[] pPlots;
               }
            } break;
		case WM_PAINT:
			hdc = BeginPaint (hWnd, &ps);
			// ZU ERLEDIGEN: Hier beliebigen Code zum Zeichnen hinzufügen...
			RECT rt;
			GetClientRect( hWnd, &rt );
			DrawText( hdc, szHello, strlen(szHello), &rt, DT_CENTER );
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
