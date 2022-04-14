// CARASCS.cpp : Definiert den Einsprungpunkt für die Anwendung.
//

#include "stdafx.h"
#include "resource.h"
#include "CCaraWalk.h"
#include "CCaraInfo.h"
#include "CCaraSdb.h"
#include "debug.h"
#include "EtsAppWndMsg.h"
#include "EtsRegistry.h"
#include <commctrl.h>
#include <time.h>

#define COMPILE_MULTIMON_STUBS
#include "MultiMon.h"

// #include <stdio.h>

#define MAX_LOADSTRING 100
#define MAX_BOXES 10
#define STEP_TIMER 100

#define INVALID_VALUE            0xffffffff

#define SCREENSAVER    1
#define IN_DLG_VIEW    2
#define AS_SETUP_VIEW  3
#define SAVE_SETTINGS  4

#define ANIMATION "Animation"
#define START_POS "Start"
#define STEP_PARM "Step"
#define ANI_TIME  "Time"
#define MATERIAL  "Material"

#define BOXES        "Boxes"
#define COUNT_BOXES  "Count"
#define BOX_NO       "Box%d"


#define WM_DATA_EXCHANGE  (WM_APP + 1)

struct AnimationDlg
{
   int nWhat;
   int nBox;
   int nStepTime;
   int nBoxes;
   CARAWALK_BoxPos bp;
   bool bBoxChanged;
   HWND hwndParent;
};

// Globale Variablen:
HINSTANCE  g_hInstance = NULL;					                  // aktuelle Instanz
TCHAR      g_szTitle[MAX_LOADSTRING] = "";							// Text der Titelzeile
TCHAR      g_szWindowClass[MAX_LOADSTRING] = "";					// Text der Titelzeile
int        g_nMode      =   0;                                 // Modus
CCaraWalk  g_BoxView;                                          // 3D-View
int        g_nBoxes     =   0;                                 // Anzahl der Boxen
int        g_nStepTime  =  50;                                 // ms
bool       g_bAnimation = false;                               // Animation an / aus

HWND       g_hwndGLView = NULL;                                // 3D-Fenster
HWND       g_hwndStatus = NULL;                                // Statusbar

unsigned short g_nMaterial  = 0;                               // Materialindexzähler

CARAWALK_BoxPos g_BoxPos[MAX_BOXES];
CARAWALK_BoxPos g_BoxPosStep[MAX_BOXES];
CARAWALK_BoxPos g_BoxPosStart[MAX_BOXES];
char            g_szBoxName[MAX_BOXES][64];



// Vorausdeklarationen von Funktionen, die in diesem Code-Modul enthalten sind:
ATOM				MyRegisterClass( HINSTANCE hInstance );
BOOL				InitInstance( HINSTANCE, int, LPSTR);
LRESULT CALLBACK	WndProc( HWND, UINT, WPARAM, LPARAM );
bool           SetMenuItem(UINT, UINT, UINT, HMENU);
BOOL CALLBACK  AnimationDlgProc(HWND, UINT, WPARAM, LPARAM);
double         GetRandomNumber(double, double);
void           RandomInitAnimation(int );
void           StopAnimation(HWND);
void           StartAnimation(HWND);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
 	// ZU ERLEDIGEN: Fügen Sie hier den Code ein.
	MSG msg;
	HACCEL hAccelTable;
   CCaraSdb  sdb;
	// Globale Zeichenfolgen initialisieren
	LoadString(hInstance, IDS_APP_TITLE, g_szTitle      , MAX_LOADSTRING);
	LoadString(hInstance, IDC_CARASCS  , g_szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
   SetGlobalRegParam(g_szWindowClass, NULL, NULL, false);

	// Initialisierung der Anwendung durchführen:
	if( !InitInstance( hInstance, nCmdShow, lpCmdLine) ) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CARASCS);

	// Hauptnachrichtenschleife:
	while(GetMessage(&msg, NULL, 0, 0)) 
	{
		if( !TranslateAccelerator (msg.hwnd, hAccelTable, &msg) ) 
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

   g_BoxView.Destructor();
   sdb.Destructor();

   hPrevInstance = NULL; // wegen Compilerwarnung !
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

	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = (WNDPROC)WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = LoadIcon(hInstance, (LPCTSTR)IDI_CARASCS);
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName  = (LPCSTR)IDC_CARASCS;
	wcex.lpszClassName = g_szWindowClass;
	wcex.hIconSm       = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData)
{
   MONITORINFOEX mi;
   mi.cbSize = sizeof(MONITORINFOEX);
   RECT *pRect = (RECT*)dwData;
   GetMonitorInfo(hMonitor, &mi);
   if (pRect)
   {
      RECT rcDest;
      ::UnionRect(&rcDest, pRect, &mi.rcMonitor);
      *pRect = rcDest;
   }
   return true;
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
BOOL InitInstance( HINSTANCE hInstance, int  nCmdShow, LPSTR lpCmdLine)
{
   HWND hWnd;
   HWND hwndParent = NULL;
   RECT rcWnd = {CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT};
   g_hInstance = hInstance; // Instanzzugriffsnummer in unserer globalen Variable speichern
   DWORD dwStyle = 0;

/*
   // zum Testen !!
            FILE *fp = fopen("c:\\sreensave.txt", "at");
            if (fp)
            {
               fprintf(fp, "%s\n", lpCmdLine);
               fclose(fp);
            }
*/
   if (lpCmdLine)                                              // Kommandozeilenparameter untersuchen
   {
      if ((lpCmdLine[0] == '/')&&((lpCmdLine[1] == 'S')||(lpCmdLine[1] == 's')))
      {                                                        // Screensaver starten
         nCmdShow = SW_SHOW;
         dwStyle  = WS_VISIBLE|WS_POPUP;
         g_nMode  = SCREENSAVER;
         ::SetRectEmpty(&rcWnd);
         EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)MonitorEnumProc, (LPARAM) &rcWnd);
      }
      else if ((lpCmdLine[0] == '/')&&((lpCmdLine[1] == 'p')||(lpCmdLine[1] == 'P')))
      {                                                        // Vorschau im Einstellungsdialog View
         nCmdShow    = SW_SHOW;
         dwStyle     = WS_CHILD|WS_VISIBLE;
         hwndParent  = (HWND)atoi(&lpCmdLine[2]);
         ASSERT(::IsWindow(hwndParent)!=0);
         ::GetClientRect(hwndParent, &rcWnd);
         g_nMode  = IN_DLG_VIEW;
      }                                                        
      else if ((lpCmdLine[0] == '/')&&((lpCmdLine[1] == 'c')||(lpCmdLine[1] == 'C'))&&(lpCmdLine[1] == ':'))
      {                                                        // Konfigurieren im Einstellungsdialog
         nCmdShow = SW_SHOW;
         dwStyle  = WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CAPTION;
         hwndParent  = (HWND)atoi(&lpCmdLine[3]);
         ASSERT(::IsWindow(hwndParent)!=0);
         g_nMode  = AS_SETUP_VIEW;
      }
      else                                                     // Konfigurieren nur so
      {
         nCmdShow = SW_SHOW;
         dwStyle  = WS_VISIBLE|WS_OVERLAPPEDWINDOW;
         g_nMode  = AS_SETUP_VIEW;
      }
   }

   hWnd = CreateWindow(g_szWindowClass, g_szTitle, dwStyle, rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, hwndParent, NULL, hInstance, NULL);
   
   if(!hWnd)
   {
      return FALSE;
   }

   if (nCmdShow == SW_MAXIMIZE)
   {
      ::SetMenu(hWnd, NULL);
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static const char szUninstallKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
	switch( message ) 
	{
		case WM_PAINT:                                           // Zeichnen
      {
	      PAINTSTRUCT ps;
	      HDC hdc;
			hdc = BeginPaint (hWnd, &ps);
			EndPaint( hWnd, &ps);
      }	break;

      case WM_SIZE:                                            // Fenstergröße
      if (g_hwndStatus && g_hwndGLView)
      {
         int cx = LOWORD(lParam);
         int cy = HIWORD(lParam);
         RECT rc;
         ::GetClientRect(g_hwndStatus, &rc);
         cy -= rc.bottom;
         ::MoveWindow(g_hwndGLView, 0, 0, cx, cy, false);
         ::MoveWindow(g_hwndStatus, 0, cy, cx, rc.bottom, false);
      } break;

      case WM_TIMER:                                           // Animation Timer
      if (wParam == STEP_TIMER)
      {
         double dMaxDimension = 20;                            // ViewVolume Ausdehnung
         double dMinZ = 0.4 / dMaxDimension;

         int i;
         for (i=0; i< g_nBoxes; i++)                           // Alle Boxen
         {
            dMaxDimension = 20;
            g_BoxPos[i].nPhi     += g_BoxPosStep[i].nPhi;      // Animieren
            g_BoxPos[i].nTheta   += g_BoxPosStep[i].nTheta;
            g_BoxPos[i].nPsi     += g_BoxPosStep[i].nPsi;
            g_BoxPos[i].pdPos[0] += g_BoxPosStep[i].pdPos[0];
            g_BoxPos[i].pdPos[1] += g_BoxPosStep[i].pdPos[1];
            g_BoxPos[i].pdPos[2] += g_BoxPosStep[i].pdPos[2];
                                                               // Die Position sollte im sichtbaren Bereich bleiben
            if (g_BoxPos[i].pdPos[2] > dMaxDimension) g_BoxPos[i].pdPos[2] = dMinZ;
            if (g_BoxPos[i].pdPos[2] < dMinZ        ) g_BoxPos[i].pdPos[2] = dMaxDimension;
            dMaxDimension = 0.5 + g_BoxPos[i].pdPos[2] * 0.8;
            if (g_BoxPos[i].pdPos[0] >  dMaxDimension) g_BoxPos[i].pdPos[0] = -dMaxDimension;
            if (g_BoxPos[i].pdPos[0] < -dMaxDimension) g_BoxPos[i].pdPos[0] =  dMaxDimension;
            if (g_BoxPos[i].pdPos[1] >  dMaxDimension) g_BoxPos[i].pdPos[1] = -dMaxDimension;
            if (g_BoxPos[i].pdPos[1] < -dMaxDimension) g_BoxPos[i].pdPos[1] =  dMaxDimension;
                                                               // Position übergeben
            g_BoxPos[i].pdPos[2] = -g_BoxPos[i].pdPos[2];
            g_BoxView.DoDataExchange(CARAWALK_SETANGLE, i, &g_BoxPos[i]);
            g_BoxPos[i].pdPos[2] = -g_BoxPos[i].pdPos[2];
         }
      }break;

      case WM_SETSTATUSBAR_TEXT:                               // Statuszeilentext von CCARAWALK
      if (g_hwndStatus)
      {
         ::SetWindowText(g_hwndStatus, (char*)lParam);
      }break;

      case WM_3DCHECK:                                         // Menukontrolle
      {
         HMENU hMenu = ::GetMenu(hWnd);
         if (hMenu)
         {
            UINT nCheck  = INVALID_VALUE;
            UINT nEnable = INVALID_VALUE;
            int  pnRange[2];
            switch (lParam)
            {
               case UNCHECKED:   nCheck  = MF_UNCHECKED;          break;
               case CHECKED:     nCheck  = MF_CHECKED;            break;
               case DISABLE_ITEM:nEnable = MF_DISABLED|MF_GRAYED; break;
               case ENABLE_ITEM: nEnable = MF_ENABLED;            break;
               case RADIOCHECKED:
               {
                  nCheck  = MFT_RADIOCHECK;
                  nEnable = (UINT) pnRange;
                  pnRange[0] = IDM_VIEW_DETAIL_VERYLOW;
                  pnRange[1] = IDM_VIEW_DETAIL_VERYHIGH;
               }break;
            }
            SetMenuItem(wParam, nCheck, nEnable, hMenu);
         }
      }break;

      case WM_MENUSELECT: case WM_EXITMENULOOP:                // Statusbar updaten
      if (g_hwndStatus)
      {
         UINT uItem;
         UINT uFlags = (UINT) HIWORD(wParam);                  // menu flags 
         char szText[MAX_PATH];
         if (message == WM_EXITMENULOOP)             uItem = IDC_CARA_STANDARD;
         if ((uFlags != 0xffff) && (lParam != NULL)) uItem = (UINT) LOWORD(wParam);
         else                                        uItem = IDC_CARA_STANDARD;
         if (::LoadString(g_hInstance, uItem, szText, MAX_PATH))
         {
            ::SetWindowText(g_hwndStatus, szText);
         }
      }break;

      case WM_ENTERSIZEMOVE: case WM_EXITSIZEMOVE:
      if (g_hwndStatus && g_hwndGLView)
      {
         if (g_bAnimation)
         {
            if (message == WM_ENTERSIZEMOVE)
            {
               StopAnimation(hWnd);
               g_bAnimation = true;
            }
            else StartAnimation(hWnd);
         }
         SendMessage(g_hwndGLView, message, wParam, lParam);
      } break;

      case WM_COMMAND:                                         // OnCommand
      {
      	int wmId, wmEvent;
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Menüauswahlen analysieren:
			switch(wmId) 
			{
				case IDM_ABOUT:                                    // About CARASCS
            {
               CCaraInfo CaraInfo;     // eine lokale Instanz der DLL-Klasse
               char exename[32];
               strcpy(exename, g_szTitle);
               strcat(exename, ".scr");
               CCaraInfo::CallInfoResource(hWnd, exename);
               CaraInfo.Destructor();
            } break;

            case IDM_VIEW_ANIMATION:
            {
               if (g_bAnimation) StopAnimation(hWnd);
               else              StartAnimation(hWnd);
            } break;

            case IDM_OPTIONS_TEXTUREINDEX:                     // Texturbitmaps
            {
               HMENU hMenu = ::GetMenu(hWnd);
               if (g_nMaterial == 0xffff)
               {
                  g_nMaterial = 0;
                  if (hMenu) SetMenuItem(wParam, MF_CHECKED, INVALID_VALUE, hMenu);
               }
               else
               {
                  g_nMaterial = 0xffff;
                  if (hMenu) SetMenuItem(wParam, MF_UNCHECKED, INVALID_VALUE, hMenu);
               }
            } break;

            case IDM_OPTIONS_ANIMATION:                        // Animationsdialog
            {
               AnimationDlg AD;
               AD.nBox  = 0;
               AD.nWhat = 0;
               AD.nStepTime = g_nStepTime;
               AD.bBoxChanged = false;
               AD.hwndParent  = hWnd;
               AD.nBoxes      = g_nBoxes;
               if (::DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_ANIMATION_DLG), hWnd, (DLGPROC) AnimationDlgProc, (LPARAM) &AD) == IDOK)
               {
                  for (int i=0; i< g_nBoxes; i++)
                  {
                     g_BoxPos[i] = g_BoxPosStart[i];
                  }
                  g_nStepTime = AD.nStepTime;
                  g_nBoxes    = AD.nBoxes;

                  StopAnimation(hWnd);
                  StartAnimation(hWnd);

                  if (AD.bBoxChanged)
                  {
                     g_BoxView.DoDataExchange(CARAWALK_SETBOXDATA, MAKELONG(0, 0xffff), NULL);
                     for (i=0; i<g_nBoxes; i++)
                     {
                        if (g_nMaterial != 0xffff) g_nMaterial = (unsigned short)(i+1);
                        g_BoxView.DoDataExchange(CARAWALK_SETBOXDATA, MAKELONG(0, g_nMaterial), g_szBoxName[i]);
                     }
                  }
               }
            }break;

            case IDM_OPTIONS_BOX:
            if (g_hwndStatus && g_hwndGLView)
            {
               bool bAnimation = g_bAnimation;
               StopAnimation(hWnd);
               ::SendMessage(g_hwndGLView, message, wParam, lParam);
               if (bAnimation) StartAnimation(hWnd);
            }break;

            case IDM_OPTIONS_LIGHT:
            case IDM_VIEW_TEX_MAG_LINEAR:case IDM_VIEW_TEX_MIN_LINEAR:case IDM_VIEW_LIGHT:case IDM_VIEW_TEXTURE:
            case IDM_VIEW_DETAIL_VERYLOW:case IDM_VIEW_DETAIL_LOW:case IDM_VIEW_DETAIL_MEAN:case IDM_VIEW_DETAIL_HIGH:case IDM_VIEW_DETAIL_VERYHIGH:
            if (g_hwndStatus && g_hwndGLView)                  // Weiterleiten der Befehle, wenn nötig
            {
               ::SendMessage(g_hwndGLView, message, wParam, lParam);
            }break;

				case IDM_EXIT:                                     // Programmende
				   DestroyWindow(hWnd);
				   break;

            case IDM_EXIT_AND_SAVE:                            // Programmende mit speichern
               g_nMode = SAVE_SETTINGS;
				   DestroyWindow(hWnd);
				   break;

				default:                                           // alle anderen Commands
               return DefWindowProc(hWnd, message, wParam, lParam);
			}
      }break;

      case WM_KEYDOWN: // case WM_MOUSEMOVE:
      if (g_nMode == SCREENSAVER)
      {
         DestroyWindow(hWnd);
      }break;

      case WM_CREATE:                                          // OnCreate
      {
         int   i;
         BOOL  dummy;
         HKEY  hKey  = NULL;
         HMENU hMenu = NULL;
         RECT  rcClient;
         if (g_nMode == SCREENSAVER)                           // Screensave braucht kein Menü
         {
            ::SetMenu(hWnd, NULL);
         }
         else
         {
            hMenu = ::GetMenu(hWnd);                           // Menu des Fensters
         }

         if ((::RegOpenKeyEx(HKEY_LOCAL_MACHINE, szUninstallKey, 0, KEY_QUERY_VALUE|KEY_READ, &hKey) == ERROR_SUCCESS) &&
             (hKey != NULL))                                   // Cara Verzeichnis suchen
         {
            FILETIME ft;
            int i;
            char  szSubKey[MAX_PATH];
            DWORD dwType, dwLength;
            bool bFound = false;
            for (i=0, dwLength = MAX_PATH; ::RegEnumKeyEx(hKey, i, szSubKey, &dwLength, NULL, NULL, 0, &ft) == ERROR_SUCCESS; i++, dwLength = MAX_PATH)
            {
               if (strncmp(szSubKey, "CARA", 4) == 0)
               {
                  char szKey[MAX_PATH];
                  int nVersion = atoi(&szSubKey[4]);
                  if (nVersion < 200) continue;
                  HKEY hSubKey = 0;
                  strncpy(szKey, szUninstallKey, MAX_PATH);
                  strncat(szKey, szSubKey, MAX_PATH);
                  if ((::RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, KEY_QUERY_VALUE|KEY_READ, &hSubKey) == ERROR_SUCCESS) &&
                      (hSubKey != NULL))
                  {
                     dwLength = MAX_PATH;
                     if ((::RegQueryValueEx(hSubKey, "UninstallString", NULL, &dwType, (BYTE*)&szKey, &dwLength) == ERROR_SUCCESS) && 
                         (dwType == REG_SZ))
                     {
                        char *pstr1 = strstr(&szKey[3], ":\\");
                        char *pstr2 = strstr(&szKey[3], "CARA.");
                        if (pstr1 && pstr2)
                        {
                           pstr2[0] = 0;
                           ::SetCurrentDirectory(&pstr1[-1]);
                           bFound = true;
                        }
                     }
                     ::RegCloseKey(hSubKey);
                     if (bFound) break;
                  }
               }
            }
            ::RegCloseKey(hKey);
            if (!bFound)
            {
               ::GetSystemDirectory(szSubKey, MAX_PATH);
               ::SetCurrentDirectory(szSubKey);
            }

         }
         if (g_nMode == AS_SETUP_VIEW)                         // Bei Setup mit Statuswindow
         { 
            char szText[64];
            ::LoadString(g_hInstance, IDC_CARA_STANDARD, szText, 64);
            g_hwndStatus = CreateStatusWindow(WS_CHILD|SBARS_SIZEGRIP|WS_VISIBLE, szText, hWnd, 100);
         }

         hKey = NULL;                                          // Registryparameter lesen
         if (OpenRegistry(g_hInstance, KEY_READ, &hKey, ANIMATION))
         {                                                     // Animation
            DWORD dwSize = sizeof(CARAWALK_BoxPos)*MAX_BOXES;
            GetRegBinary(hKey, START_POS, g_BoxPosStart, dwSize);
            dwSize =sizeof(CARAWALK_BoxPos)*MAX_BOXES;
            GetRegBinary(hKey, STEP_PARM, g_BoxPosStep, dwSize);
            g_nStepTime = GetRegDWord(hKey, ANI_TIME, g_nStepTime);
            g_nMaterial = (unsigned short)GetRegDWord(hKey, MATERIAL, 0xffff);
         }
         if (hKey) EtsRegCloseKey(hKey);

         if (OpenRegistry(g_hInstance, KEY_READ, &hKey, BOXES))
         {                                                     // Lautsprecher
            DWORD dwSize;
            char szBox[32];
            g_nBoxes = GetRegDWord(hKey, COUNT_BOXES, 0);
            for (i=0; i< g_nBoxes; i++)
            {
               dwSize = 64;
               wsprintf(szBox, BOX_NO, i+1);
               GetRegBinary(hKey, szBox, g_szBoxName[i], dwSize, true);
            }
         }
         if (hKey) EtsRegCloseKey(hKey);

         if (hMenu)                                            // Menuparameter initialisieren
         {
            SetMenuItem(wParam, (g_nMaterial == 0xffff) ? MF_UNCHECKED: MF_CHECKED, INVALID_VALUE, hMenu);
         }

         unsigned t = (unsigned) time(NULL);
         srand(t);                                             // Zufallsgenerator initialisieren
         if (g_nBoxes==0)                                      // Keine Boxen ?
         {
            g_nBoxes = 1;                                      // Na denn erstmal eine
            strcpy(g_szBoxName[0], "Omni_400");                // Do schau her, die "Omni"
            RandomInitAnimation(0);
         }

         for (i=0; i<g_nBoxes; i++)                            // Startpositionen initialisieren
         {
            if (g_BoxPosStart[i].pdPos[2] < 0.5) g_BoxPosStart[i].pdPos[2] = 1;
            g_BoxPos[i] = g_BoxPosStart[i];
         }

         GetClientRect(hWnd, &rcClient);                       // Client ermitteln
         UINT nFlags = 0;
         if (g_hwndStatus)
         {
            RECT rc;
            ::GetClientRect(g_hwndStatus, &rc);
            nFlags  = WS_CHILD|WS_VISIBLE;
            rcClient.bottom -= rc.bottom;
         }
         g_hwndGLView = g_BoxView.Create(SCREEN_SAVER, nFlags, &rcClient, hWnd);// erzeugen
         if (g_hwndGLView)                                     // Ok ?
         {
            for (i=0; i<g_nBoxes; i++)                         // initialisieren
            {
               if (g_nMaterial != 0xffff) g_nMaterial = (unsigned short)(i+1);
               g_BoxView.DoDataExchange(CARAWALK_SETBOXDATA, MAKELONG(0, g_nMaterial), g_szBoxName[i]);
            }
            if (g_nStepTime <= 0) g_nStepTime = 20;            // 0 ? das geht doch nicht !
            StartAnimation(hWnd);

            if (g_nMode == SCREENSAVER)                        // Windoof Bescheid sagen !
               ::SystemParametersInfo(SPI_SCREENSAVERRUNNING, TRUE, &dummy, 0);
         }
      }break;

		case WM_DESTROY:                                         // Programmende
      {
         BOOL dummy;
         if (g_hwndStatus && g_hwndGLView)
         {
            ::PostMessage(g_hwndGLView, WM_CLOSE, 0, 0);
         }
			PostQuitMessage(0);
         if      (g_nMode == SCREENSAVER) ::SystemParametersInfo(SPI_SCREENSAVERRUNNING,FALSE,&dummy,0);
         else if (g_nMode == SAVE_SETTINGS)
         {
            HKEY hkey = NULL;
            if (OpenRegistry(g_hInstance, KEY_WRITE, &hkey, ANIMATION))
            {
               DWORD dwSize = sizeof(CARAWALK_BoxPos)*MAX_BOXES;
               SetRegBinary(hkey, START_POS, (BYTE*)g_BoxPosStart, dwSize);
               dwSize = sizeof(CARAWALK_BoxPos)*MAX_BOXES;
               SetRegBinary(hkey, STEP_PARM, (BYTE*)g_BoxPosStep, dwSize);
               SetRegDWord(hkey, ANI_TIME, g_nStepTime);
               SetRegDWord(hkey, MATERIAL, g_nMaterial);
               EtsRegCloseKey(hkey);
            }
            if (OpenRegistry(g_hInstance, KEY_WRITE, &hkey, BOXES))
            {
               char szBox[32];
               SetRegDWord(hkey, COUNT_BOXES, g_nBoxes);
               for (int i=0; i<g_nBoxes; i++)
               {
                  wsprintf(szBox, BOX_NO, i+1);
                  SetRegBinary(hkey, szBox, (BYTE*)g_szBoxName[i], 0, true);
               }
               EtsRegCloseKey(hkey);
            }
         }
      } break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
   }
   return 0;
}

bool SetMenuItem(UINT nItem, UINT nCheck, UINT nEnable, HMENU hMenu)
{
   if (hMenu)
   {
      HMENU hSubMenu;
      bool  bOk = false;
      int   i, nCount = ::GetMenuItemCount(hMenu);
      for (i=0; i<nCount; i++)
      {
         hSubMenu = ::GetSubMenu(hMenu, i);
         if (hSubMenu)
         {
            bOk = SetMenuItem(nItem, nCheck, nEnable, hSubMenu);
            if (bOk) return true;
         }
      }
      if (nCheck != INVALID_VALUE)
      {
         if (nCheck == MFT_RADIOCHECK)
         {
            int *pRange = (int*) nEnable;
            if (CheckMenuRadioItem(hMenu, pRange[0], pRange[1], nItem, MF_BYCOMMAND)) return true;
            return false;
         }
         else if (CheckMenuItem(hMenu, nItem,  nCheck|MF_BYCOMMAND) != INVALID_VALUE) bOk = true;
      }
      if (nEnable != INVALID_VALUE)
      {
         if (EnableMenuItem(hMenu, nItem,  nEnable|MF_BYCOMMAND) != INVALID_VALUE) bOk = true;
      }
      return bOk;
   }
   return false;
}

BOOL CALLBACK AnimationDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)
   {
      case WM_NOTIFY:                                          // Notifications
      if (wParam == IDC_AN_BOX_LIST)                           // für die Liste
      {
         NMHDR* pnmh = (NMHDR*)lParam;
         if (pnmh->code == LVN_GETDISPINFO)                    // Anzeigeinfo
         {
            NMLVDISPINFOA *pdi = (NMLVDISPINFO*)lParam;
            if ((pdi->item.iSubItem == 0) && (pdi->item.mask & LVIF_TEXT))
            {
               strncpy(pdi->item.pszText, g_szBoxName[pdi->item.iItem], pdi->item.cchTextMax);
            }
         }
         else if (pnmh->code == NM_CLICK)                      // Mausklick
         {
            HWND hwndList = ::GetDlgItem(hwndDlg, IDC_AN_BOX_LIST);
            LVHITTESTINFO lvHTI;
            GetCursorPos(&lvHTI.pt);
            ::ScreenToClient(hwndList, &lvHTI.pt);
            ::SendMessage(hwndList, LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHTI);
            if (lvHTI.iItem != -1)                             // Item gefunden
            {
               AnimationDlg *pAD = (AnimationDlg*) ::GetWindowLong(hwndDlg, DWL_USER);
               if (pAD->nBox != lvHTI.iItem)                   // Item gewechselt
               {
                  BOOL bReturn = 0;                            // Einstellungen speichern
                  if (pAD->nWhat == 0) g_BoxPosStart[pAD->nBox] = pAD->bp;
                  else                 g_BoxPosStep[pAD->nBox]  = pAD->bp;
                  pAD->nBox = lvHTI.iItem;                     // neuer Item
                                                               // Buttonstates updaten
                  ::EnableWindow(::GetDlgItem(hwndDlg, IDC_AN_BTN_GETBOX), (pAD->nBox <= pAD->nBoxes) ? true : false);
                  ::EnableWindow(::GetDlgItem(hwndDlg, IDC_AN_BTN_DELBOX), (pAD->nBox == pAD->nBoxes-1) ? true : false);
                                                               // Parameter updaten
                  ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 2, (LPARAM)&bReturn);
               }
            }
         }
      }break;
      case WM_INITDIALOG:                                      // OnInitDialog
      {
         BOOL bReturn = 1;
         if (lParam == NULL) return -1;                        // Parameter vorhanden ?
         ::SetWindowLong(hwndDlg, DWL_USER, lParam);
         AnimationDlg *pAD = (AnimationDlg*) lParam;

         HWND hwndList = ::GetDlgItem(hwndDlg, IDC_AN_BOX_LIST);
         if (hwndList)                                         // Liste Initialisieren
         {
            long lResult = ::SendMessage(hwndList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT, LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
            LVCOLUMNA lvc;
            RECT rc;
            ::GetClientRect(hwndList, &rc);
            ZeroMemory(&lvc, sizeof(lvc));
            lvc.mask       = LVCF_SUBITEM|LVCF_WIDTH;
            lvc.cx         = rc.right;
            lvc.iSubItem   = 0;
            lResult = ::SendMessage(hwndList, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);
            lResult = ::SendMessage(hwndList, LVM_SETITEMCOUNT, MAX_BOXES, 0);
         }
         LVITEMA lvi;
         lvi.mask      = LVIF_STATE;
         lvi.state     = LVIS_SELECTED;
         lvi.stateMask = LVIS_SELECTED;
         ::SendDlgItemMessage(hwndDlg, IDC_AN_BOX_LIST, LVM_SETSELECTIONMARK, pAD->nBox, 0);
         ::SendDlgItemMessage(hwndDlg, IDC_AN_BOX_LIST, LVM_SETITEMSTATE, pAD->nBox, (LPARAM)&lvi);
                                                               // Radiobuttons
         ::SendDlgItemMessage(hwndDlg, (pAD->nWhat == 0) ? IDC_AN_R_WHAT : IDC_AN_R_WHAT1, BM_SETCHECK, BST_CHECKED, 0);

         bReturn = 0;                                          // LS Parameter
         ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 2, (LPARAM)&bReturn);
         ::SetDlgItemInt(hwndDlg, IDC_AN_EDT_REP_TIME, pAD->nStepTime, false);// Animationtime
                                                               // Buttonstates
         ::EnableWindow(::GetDlgItem(hwndDlg, IDC_AN_BTN_GETBOX), (pAD->nBox <= pAD->nBoxes) ? true : false);
         ::EnableWindow(::GetDlgItem(hwndDlg, IDC_AN_BTN_DELBOX), (pAD->nBox == pAD->nBoxes-1) ? true : false);
         return bReturn;
      }
      break;
      case WM_DATA_EXCHANGE:                                   // Datenaustausch für die LS Parameter
      {
         AnimationDlg *pAD = (AnimationDlg*) ::GetWindowLong(hwndDlg, DWL_USER);
         if (pAD == NULL) return 0;
         BOOL *pbReturn = (BOOL*)lParam;
         BOOL bSet      = wParam;
         if ((pAD->nBox >= 0) && (pAD->nBox < MAX_BOXES))
         {
            CARAWALK_BoxPos *pB;
            if (pAD->nWhat == 0) pB = &g_BoxPosStart[pAD->nBox];
            else                 pB = &g_BoxPosStep[pAD->nBox];
            if (bSet)                                          // Dialog Initialisieren
            {
               if (bSet == 2) pAD->bp = *pB;
               ::SetDlgItemInt(hwndDlg, IDC_AN_EDT_TURN_X, -pAD->bp.nTheta, true);
               ::SetDlgItemInt(hwndDlg, IDC_AN_EDT_TURN_Y, pAD->bp.nPhi  , true);
               ::SetDlgItemInt(hwndDlg, IDC_AN_EDT_TURN_Z, pAD->bp.nPsi  , true);

               ::SetDlgItemInt(hwndDlg, IDC_AN_EDT_MOVE_X, -(int)(pAD->bp.pdPos[0]*1000), true);
               ::SetDlgItemInt(hwndDlg, IDC_AN_EDT_MOVE_Y, -(int)(pAD->bp.pdPos[1]*1000), true);
               ::SetDlgItemInt(hwndDlg, IDC_AN_EDT_MOVE_Z,  (int)(pAD->bp.pdPos[2]*1000), true);
               *pbReturn = 1;
            }
            else                                               // Daten holen
            {
               bool bChanged  = false;                         // Min / Max Parameter für
               int nMaxAngle  =  100;                          // Animation
               int nMinAngle  = -100;
               double dMaxPos =  1;
               double dMinPos = -1;
               if (pAD->nWhat == 0)                            // Anfangsposition
               {
                  nMaxAngle = 360;
                  nMinAngle =   0;
                  dMaxPos   =  19;
                  dMinPos   = -19;
               }
               pAD->bp.nTheta = -(int)::GetDlgItemInt(hwndDlg, IDC_AN_EDT_TURN_X, pbReturn, true);
               if (pAD->bp.nTheta > nMaxAngle) pAD->bp.nTheta = nMaxAngle, bChanged = true;
               if (pAD->bp.nTheta < nMinAngle) pAD->bp.nTheta = nMinAngle, bChanged = true;
               pAD->bp.nPhi   = ::GetDlgItemInt(hwndDlg, IDC_AN_EDT_TURN_Y, pbReturn, true);
               if (pAD->bp.nPhi > nMaxAngle) pAD->bp.nPhi = nMaxAngle, bChanged = true;
               if (pAD->bp.nPhi < nMinAngle) pAD->bp.nPhi = nMinAngle, bChanged = true;
               pAD->bp.nPsi   = ::GetDlgItemInt(hwndDlg, IDC_AN_EDT_TURN_Z, pbReturn, true);
               if (pAD->bp.nPsi > nMaxAngle) pAD->bp.nPsi = nMaxAngle, bChanged = true;
               if (pAD->bp.nPsi < nMinAngle) pAD->bp.nPsi = nMinAngle, bChanged = true;
      
               pAD->bp.pdPos[0] = 0.001 * -(int)::GetDlgItemInt(hwndDlg, IDC_AN_EDT_MOVE_X, pbReturn, true);
               if (pAD->bp.pdPos[0] > dMaxPos) pAD->bp.pdPos[0] = dMaxPos, bChanged = true;
               if (pAD->bp.pdPos[0] < dMinPos) pAD->bp.pdPos[0] = dMinPos, bChanged = true;
               pAD->bp.pdPos[1] = 0.001 * -(int)::GetDlgItemInt(hwndDlg, IDC_AN_EDT_MOVE_Y, pbReturn, true);
               if (pAD->bp.pdPos[1] > dMaxPos) pAD->bp.pdPos[1] = dMaxPos, bChanged = true;
               if (pAD->bp.pdPos[1] < dMinPos) pAD->bp.pdPos[1] = dMinPos, bChanged = true;
               pAD->bp.pdPos[2] = 0.001 *  (int)::GetDlgItemInt(hwndDlg, IDC_AN_EDT_MOVE_Z, pbReturn, true);
               if (pAD->bp.pdPos[2] > dMaxPos) pAD->bp.pdPos[2] = dMaxPos, bChanged = true;
               if (pAD->nWhat == 0) dMinPos = 0.5;
               if (pAD->bp.pdPos[2] < dMinPos) pAD->bp.pdPos[2] = dMinPos, bChanged = true;

               if (bChanged)::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 1, lParam);
            }
         }
      }break;
      case WM_COMMAND:                                         // OnCommand
      {
         AnimationDlg *pAD = (AnimationDlg*) ::GetWindowLong(hwndDlg, DWL_USER);
         if (pAD == NULL) return 0;
         if ((LOWORD(wParam) == IDCANCEL)||(LOWORD(wParam) == IDOK))// Ok / Cancel
         {
            if (LOWORD(wParam) == IDOK)                        // Ok
            {                                                  // Speichern
               if (pAD->nWhat == 0) g_BoxPosStart[pAD->nBox] = pAD->bp;
               else                 g_BoxPosStep[ pAD->nBox] = pAD->bp;
            }
            EndDialog(hwndDlg, LOWORD(wParam));
            return 1;
         }
         else if (HIWORD(wParam) == EN_KILLFOCUS)              // OnKillFocus
         {
            BOOL bReturn = 0;
            if (LOWORD(wParam) == IDC_AN_EDT_REP_TIME)
            {                                                  // Steptime
               pAD->nStepTime = ::GetDlgItemInt(hwndDlg, IDC_AN_EDT_REP_TIME, &bReturn, false);
            }
            else                                               // alle anderen
            {
               ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 0, (LPARAM)&bReturn);
            }
         }
         else if (HIWORD(wParam) == BN_CLICKED)                // OnButtonClicked
         {
            switch (LOWORD(wParam))
            {
               case IDC_AN_R_WHAT: case IDC_AN_R_WHAT1:        // Radio What
               {
                  int nWhat = (::SendDlgItemMessage(hwndDlg, IDC_AN_R_WHAT, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 0 : 1;
                  if (pAD->nWhat != nWhat)
                  {
                     if (pAD->nWhat == 0) g_BoxPosStart[pAD->nBox] = pAD->bp;
                     else                 g_BoxPosStep[ pAD->nBox] = pAD->bp;
                     pAD->nWhat = nWhat;
                     ::ShowWindow(::GetDlgItem(hwndDlg, IDC_AN_TXT_MOVE ), nWhat ? SW_SHOW : SW_HIDE);
                     ::ShowWindow(::GetDlgItem(hwndDlg, IDC_AN_TXT_TURN ), nWhat ? SW_SHOW : SW_HIDE);
                     ::ShowWindow(::GetDlgItem(hwndDlg, IDC_AN_TXT_POS  ), nWhat ? SW_HIDE : SW_SHOW);
                     ::ShowWindow(::GetDlgItem(hwndDlg, IDC_AN_TXT_ANGLE), nWhat ? SW_HIDE : SW_SHOW);
                     BOOL bReturn = 0;
                     ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 2, (LPARAM)&bReturn);
                  }
               }break;
               case IDC_AN_BTN_GETBOX:                         // Lautsprecher Auswählen
               {
                  bool  bIndexChanged = false;
                  BOOL bReturn = 0;

                  CARASDB_PARAMS sdbParam;                     // CARASdb Dialog
                  ZeroMemory(&sdbParam, sizeof(sdbParam));
                  sdbParam.hwndParent = hwndDlg;
                  sdbParam.Initial.bitAllUnits = true;         // alle Lautsprecher

                  for ( ; pAD->nBox>0; pAD->nBox--)            // Daten müssen zusammenhängen
                  {
                     if (g_szBoxName[pAD->nBox-1][0] == 0)
                        bIndexChanged = true;
                     else break;
                  }
                  if (bIndexChanged)                           // Auswahl geändert
                  {                                            // Liste updaten
                     ::SendDlgItemMessage(hwndDlg, IDC_AN_BOX_LIST, LVM_SETSELECTIONMARK, pAD->nBox, 0);
                     ::SendDlgItemMessage(hwndDlg, IDC_AN_BOX_LIST, LVM_SETHOTITEM, pAD->nBox, 0);
                     ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 2, (LPARAM)&bReturn);
                  }
                  strcpy(sdbParam.szFileName[0], g_szBoxName[pAD->nBox]);
                  if (CCaraSdb::DoModal(sdbParam))             // Sdb Dialog aufrufen
                  {
                     strcpy(g_szBoxName[pAD->nBox], sdbParam.szFileName[0]);
                     InvalidateRect(GetDlgItem(hwndDlg, IDC_AN_BOX_LIST), NULL, false);
                     pAD->bBoxChanged = true;                  // Boxen geändert
                     int nOldCount = pAD->nBoxes;
                     for (pAD->nBoxes=0; pAD->nBoxes<MAX_BOXES; pAD->nBoxes++)
                     {
                        if (g_szBoxName[pAD->nBoxes][0] == 0)  // Aktuelle Anzahl der Boxen ermitteln
                           break;
                     }
                     if (nOldCount < pAD->nBoxes)              // Neue Box
                     {
                        RandomInitAnimation(pAD->nBox);        // wird initialisiert
                        ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 2, (LPARAM)&bReturn);
                     }
                  }
               }break;
               case IDC_AN_BTN_DELBOX:                         // Lautsprecher Löschen
               if (pAD->nBoxes>0)
               {
                  pAD->nBoxes--;
                  g_szBoxName[pAD->nBoxes][0] = 0;
                  pAD->bBoxChanged = true;
                  InvalidateRect(GetDlgItem(hwndDlg, IDC_AN_BOX_LIST), NULL, false);
               }break;
               case IDC_AN_CK_STOP:                            // Animation anhalten
               {
                  if (::SendDlgItemMessage(hwndDlg, IDC_AN_CK_STOP, BM_GETCHECK, 0, 0) == BST_CHECKED)
                  {
                     StopAnimation(pAD->hwndParent);
                  }
                  else
                  {
                     StartAnimation(pAD->hwndParent);
                  }
               }break;
            }
         }
      }break;
      default:
      {

      } break;
   }
   return 0;
}

double GetRandomNumber(double dMin, double dMax)
{
   double dRandom  = (double)rand();
   double dMaxRand = (double)RAND_MAX;
   double dX = (dMax - dMin) / dMaxRand;
   return dMin + dX * dRandom;
}

void RandomInitAnimation(int nBox)
{
   if ((nBox>=0) && (nBox<MAX_BOXES))
   {
      g_BoxPosStart[nBox].nPhi     = (long)GetRandomNumber(0, 360);
      g_BoxPosStart[nBox].nPsi     = (long)GetRandomNumber(0, 360);
      g_BoxPosStart[nBox].nTheta   = (long)GetRandomNumber(0, 360);
      g_BoxPosStart[nBox].pdPos[0] = GetRandomNumber(-10, 10);
      g_BoxPosStart[nBox].pdPos[1] = GetRandomNumber(-10, 10);
      g_BoxPosStart[nBox].pdPos[2] = GetRandomNumber(0.5, 19);

      g_BoxPosStep[nBox].nPhi     = (long)GetRandomNumber(-8, 8);
      g_BoxPosStep[nBox].nPsi     = (long)GetRandomNumber(-8, 8);
      g_BoxPosStep[nBox].nTheta   = (long)GetRandomNumber(-8, 8);
      g_BoxPosStep[nBox].pdPos[0] = GetRandomNumber(-.05, .05);
      g_BoxPosStep[nBox].pdPos[1] = GetRandomNumber(-.05, .05);
      g_BoxPosStep[nBox].pdPos[2] = GetRandomNumber(-.05, .05);
   }
}

void StopAnimation(HWND hWnd)
{
   ::KillTimer(hWnd, STEP_TIMER);
   g_bAnimation = false;
   HMENU hMenu = ::GetMenu(hWnd);
   if (hMenu) SetMenuItem(IDM_VIEW_ANIMATION, g_bAnimation ? MF_CHECKED:MF_UNCHECKED, INVALID_VALUE, hMenu);
}

void StartAnimation(HWND hWnd)
{
   UINT nTimer = 0;
   if (g_nStepTime) 
   {
      nTimer = ::SetTimer(hWnd, STEP_TIMER, g_nStepTime, NULL);
   }
   g_bAnimation = (nTimer == STEP_TIMER) ? true : false;
   HMENU hMenu = ::GetMenu(hWnd);
   if (hMenu) SetMenuItem(IDM_VIEW_ANIMATION, g_bAnimation ? MF_CHECKED:MF_UNCHECKED, INVALID_VALUE, hMenu);
}
