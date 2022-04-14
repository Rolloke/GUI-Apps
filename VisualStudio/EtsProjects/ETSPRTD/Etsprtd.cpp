/**************************************+****************************************

                                   Etsprtd.cpp
                                   -----------

                 DLL-für Druckerdialogfeld und Druckjobbehandlung

                  (C) 1999 - 2000 ELAC Technische Software GmbH

                          VERSION 1.2 Stand 15.08.2000


                                                   programmed by Oliver Wesnigk
*******************************************************************************/

#include "stdafx.h"

#include "CEtsHelp.h"                            // für die Contexthilfe

/*******************************************************************************
                 diese MACROS definieren für DEBUG / REPORT Build
                         im RELEASE-Build auskommentieren !
*******************************************************************************/

bool    g_bGenerateReport  = false;              // als Default kein Reportfile erzeugen !
#define ETSDEBUG_INCLUDE                         // damit der Debugcode in dieses File integriert wird
                                                 // damit globale Instance initialisiert wird
#define ETSDEBUG_CREATEDATA "ETSPRTD",&g_bGenerateReport 
#include <CEtsDebug.h>

bool DllProcessAttach(HINSTANCE hInstance);
bool DllProcessDetach();
void GetRegValues();


BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
   switch (fdwReason)
   {
      case DLL_PROCESS_ATTACH:
           return DllProcessAttach(hinstDLL);
      case DLL_PROCESS_DETACH:
           DllProcessDetach();
           return FALSE;
           break;
   }

   return TRUE;
}


/*******************************************************************************
                           alle Fehlercodes der CARAPRTD.DLL

*******************************************************************************/

#define DLLE 0xE0000000                          // wird mit dem ERROR_CODE verknüpft für die Übergabe an SetLastError

#define ERROR_PROCEED          0                 //            Fehler bearbeitet (nur zur Weiterleitung)
#define ERROR_PINFO            1                 // 0xE0000001 "pInfo is invalid"
#define ERROR_ENUMNOPRINTER    2                 // 0xE0000002 "EnumPrinters(): no printers found"
#define ERROR_ENUMPRTFAILED    3                 // 0xE0000003 "EnumPrinters(): failed"
#define ERROR_DWNEEDED         4                 // 0xE0000004 "dwNeeded is invalid"
#define ERROR_PPRTINFO         5                 // 0xE0000005 "allocation of pPrinterInfo failed"
#define ERROR_ENUMPRTGET       6                 // 0xE0000006 "EnumPrinters(): get printers failed"
#define ERROR_HWND             7                 // 0xE0000007 "invalid window handle"
#define ERROR_PPINFO           8                 // 0xE0000008 "ppInfo is invalid"
#define ERROR_GETPROFILE       9                 // 0xE0000009 "GetProfileString(): failed"
#define ERROR_NOPRINTER       10                 // 0xE000000a "no printer found"
#define ERROR_COMMASEARCH     11                 // 0xE000000b "comma search failed"
#define ERROR_STRINGOVERFLOW  12                 // 0xE000000c "String overflow"
#define ERROR_OPENPRTFAILED   13                 // 0xE000000d "OpenPrinter(): failed"
#define ERROR_GETPRTNO        14                 // 0xE000000e "GetPrinter(): no PRINTER_INFO_2 exists"
#define ERROR_GETPRTFAILED    15                 // 0xE000000f "GetPrinter(): failed"
#define ERROR_ALLOCPPINFO     16                 // 0xE0000010 "allocation of ppInfo failed"
#define ERROR_GETPRTGET       17                 // 0xE0000011 "GetPrinter(): get PRINTER_INFO_2 failed"
#define ERROR_PDEVMODE        18                 // 0xE0000012 "pDevMode is invalid"
#define ERROR_REALLOCPPINFO   19                 // 0xE0000013 "reallocation of ppInfo failed"
#define ERROR_DOCPROPSIZE     20                 // 0xE0000014 "DocumentProperties(): get size failed"
#define ERROR_ALLOCDEVMODE    21                 // 0xE0000015 "allocation of pDevMode failed"
#define ERROR_DOCPROPDEVMODE  22                 // 0xE0000016 "DocumentProperties(): get DevMode failed"
#define ERROR_PPRTNAME        23                 // 0xE0000017 "pPrinterName is invalid"
#define ERROR_DOCPROPFAILED   24                 // 0xE0000018 "DocumentProperties(): failed"
#define ERROR_CREATEIC        25                 // 0xE0000019 "CreateIC or DC(): failed"
#define ERROR_DELETEDC        26                 // 0xE000001a "DeleteDC(): failed"
#define ERROR_PTSCALE         27                 // 0xE000001b "ptScale is invalid"
#define ERROR_DMORIENT        28                 // 0xE000001c "bitflag DM_ORIENTATION is missing" !! wird nicht mehr verwendet !!
#define ERROR_DMORINVALID     29                 // 0xE000001d "dmOrientation is invalid"
#define ERROR_HEAPHANDLE      30                 // 0xE000001e "Heap is invalid"
#define ERROR_DEVCAPFAILED    31                 // 0xE000001f "DeviceCapabilities(): failed"
#define ERROR_PPORTNAME       32                 // 0xE0000020 "pPortName is invalid"
#define ERROR_STARTDOC        33                 // 0xE0000021 "StartDoc(): failed"
#define ERROR_STARTPAGE       34                 // 0xE0000022 "StartPage(): failed"
#define ERROR_ENDPAGE         35                 // 0xE0000023 "EndPage(): failed"
#define ERROR_ENDDOC          36                 // 0xE0000024 "EndDoc(): failed"
#define ERROR_ALLOCPPAPERS    37                 // 0xE0000025 "allocation of pPapers failed"

// Fehler die der Anwender dieser DLL auslösen kann !

#define ERROR_REENTRANT       38                 // 0xE0000026 "this Component is not reentrant"
#define ERROR_PPARAMETER      39                 // 0xE0000027 "pointer to function parameters is invalid"
#define ERROR_DWFLAGS         40                 // 0xE0000028 "dwFlags in function parameters is invalid"
#define ERROR_BUFFER          41                 // 0xE0000029 "buffer size is invalid"
#define ERROR_PBUFFER         42                 // 0xE000002a "buffer pointer is invalid"
#define ERROR_METAFILE        43                 // 0xE000002b "metafilehandle is invalid"
#define ERROR_SETAD           44                 // 0xE000002c "data to set are invalid"
#define ERROR_PRINTERNOTFOUND 45                 // 0xE000002d "printer to set not found"
#define ERROR_CALLBACK        46                 // oxE000002e "callback pointer invalid"

/*******************************************************************************
                            alle Fehlertexte der ETSPRTD.DLL
                              (DEBUG oder REPORT Build !)
*******************************************************************************/

#ifdef ETSDEBUG_INCLUDE                          // nur DEBUG oder REPORT Buid
const char * g_szERRORS[47] =
  {"",                                           // ERROR_PROCEED
   "pInfo is invalid",                           // ERROR_PINFO
   "EnumPrinters(): no printers found",          // ERROR_ENUMNOPRINTER
   "EnumPrinters(): failed",                     // ERROR_ENUMPRTFAILED
   "dwNeeded is invalid",                        // ERROR_DWNEEDED
   "allocation of pPrinterInfo failed",          // ERROR_PPRTINFO
   "EnumPrinters(): get printers failed",        // ERROR_ENUMPRTGET
   "invalid window handle",                      // ERROR_HWND
   "ppInfo is invalid",                          // ERROR_PPINFO
   "GetProfileString(): failed",                 // ERROR_GETPROFILE
   "no printer found",                           // ERROR_NOPRINTER
   "comma search failed",                        // ERROR_COMMASEARCH
   "String overflow",                            // ERROR_STRINGOVERFLOW
   "OpenPrinter(): failed",                      // ERROR_OPENPRTFAILED
   "GetPrinter(): no PRINTER_INFO_2 exists",     // ERROR_GETPRTNO
   "GetPrinter(): failed",                       // ERROR_GETPRTFAILED
   "allocation of ppInfo failed",                // ERROR_ALLOCPPINFO
   "GetPrinter(): get PRINTER_INFO_2 failed",    // ERROR_GETPRTGET
   "pDevMode is invalid",                        // ERROR_PDEVMODE
   "reallocation of ppInfo failed",              // ERROR_REALLOCPPINFO
   "DocumentProperties(): get size failed",      // ERROR_DOCPROPSIZE
   "allocation of pDevMode failed",              // ERROR_ALLOCDEVMODE
   "DocumentProperties(): get DevMode failed",   // ERROR_DOCPROPDEVMODE
   "pPrinterName is invalid",                    // ERROR_PPRTNAME
   "DocumentProperties(): failed",               // ERROR_DOCPROPFAILED
   "CreateIC or DC(): failed",                   // ERROR_CREATEIC
   "DeleteDC(): failed",                         // ERROR_DELETEDC
   "ptScale is invalid",                         // ERROR_PTSCALE
   "bitflag DM_ORIENTATION is missing",          // ERROR_DMORIENT
   "dmOrientation is invalid",                   // ERROR_DMORINVALID
   "Heap is invalid",                            // ERROR_HEAPHANDLE
   "DeviceCapabilities(): failed",               // ERROR_DEVCAPFAILED
   "pPortName is invalid",                       // ERROR_PPORTNAME
   "StartDoc(): failed",                         // ERROR_STARTDOC
   "StartPage(): failed",                        // ERROR_STARTPAGE
   "EndPage(): failed",                          // ERROR_ENDPAGE
   "EndDoc(): failed",                           // ERROR_ENDDOC
   "allocation of pPapers failed",               // ERROR_ALLOCPPAPERS

// ab hier sind es alles Fehler, welche die Schnittstelle zu der DLL-Betreffen

   "this Component is not reentrant",            // ERROR_REENTRANT
   "pointer to function parameters is invalid",  // ERROR_PPARAMETER
   "dwFlags in function parameters is invalid",  // ERROR_DWFLAGS
   "buffer size is invalid",                     // ERROR_BUFFER
   "buffer pointer is invalid",                  // ERROR_PBUFFER
   "metafilehandle is invalid",                  // ERROR_METAFILE
   "data to set are invalid",                    // ERROR_SETAD
   "printer to set not found",                   // ERROR_PRINTERNOTFOUND
   "callback pointer invalid"                    // ERROR_CALLBACK
  };
#endif

/*******************************************************************************

                         Printerstatus nach Resource-String-ID

*******************************************************************************/

struct PS2RSID                                   // Struktur, um für den Statuswert die ResourceID zu finden
{
   DWORD Status;
   int   ID;
};

#define MAXPS2RSID 26                            // die Anzahl der Statuswerte

const PS2RSID g_Status2Id[MAXPS2RSID] = 
{{PRINTER_STATUS_BUSY             ,IDS_STATUS_BUSY},             // Busy              , Ausgelastet
 {PRINTER_STATUS_DOOR_OPEN        ,IDS_STATUS_DOOR_OPEN},        // Door open         , Abdeckung ist offen
 {PRINTER_STATUS_ERROR            ,IDS_STATUS_ERROR},            // Error             , Fehler
 {PRINTER_STATUS_INITIALIZING     ,IDS_STATUS_INITIALIZING},     // Initializing      , Initialisierend
 {PRINTER_STATUS_IO_ACTIVE        ,IDS_STATUS_IO_ACTIVE},        // I/O active        , E. / A. aktive
 {PRINTER_STATUS_MANUAL_FEED      ,IDS_STATUS_MANUAL_FEED},      // Manual feed       , Manuelle Papierzufuhr
 {PRINTER_STATUS_NO_TONER         ,IDS_STATUS_NO_TONER},         // No toner          , kein Toner
 {PRINTER_STATUS_NOT_AVAILABLE    ,IDS_STATUS_NOT_AVAILABLE},    // Not available     , Nichtverfügbar
 {PRINTER_STATUS_OFFLINE          ,IDS_STATUS_OFFLINE},          // Offline           , Offline
 {PRINTER_STATUS_OUT_OF_MEMORY    ,IDS_STATUS_OUT_OF_MEMORY},    // Out of memory     , Nicht genügend Arbeitsspeicher
 {PRINTER_STATUS_OUTPUT_BIN_FULL  ,IDS_STATUS_OUTPUT_BIN_FULL},  // Output bin full   , Ausgabefach ist voll
 {PRINTER_STATUS_PAGE_PUNT        ,IDS_STATUS_PAGE_PUNT},        // Page punt         , Seitenverwurf
 {PRINTER_STATUS_PAPER_JAM        ,IDS_STATUS_PAPER_JAM},        // Paper jam         , Papierstau
 {PRINTER_STATUS_PAPER_OUT        ,IDS_STATUS_PAPER_OUT},        // Paper out         , Papiermangel
 {PRINTER_STATUS_PAPER_PROBLEM    ,IDS_STATUS_PAPER_PROBLEM},    // Paper problem     , Papierproblem
 {PRINTER_STATUS_PAUSED           ,IDS_STATUS_PAUSED},           // Paused            , Angehalten
 {PRINTER_STATUS_PENDING_DELETION ,IDS_STATUS_PENDING_DELETION}, // Pending deletion  , Löschen anstehend
 {PRINTER_STATUS_POWER_SAVE       ,IDS_STATUS_POWER_SAVE},       // Power saved       , Energie Sparmodus
 {PRINTER_STATUS_PRINTING         ,IDS_STATUS_PRINTING},         // Printing          , Druckend
 {PRINTER_STATUS_PROCESSING       ,IDS_STATUS_PROCESSING},       // Processing        , Verarbeitend
 {PRINTER_STATUS_SERVER_UNKNOWN   ,IDS_STATUS_SERVER_UNKNOWN},   // Server unknown    , Unbekannter Server
 {PRINTER_STATUS_TONER_LOW        ,IDS_STATUS_TONER_LOW},        // Toner low         , Niedriger Tonerstand
 {PRINTER_STATUS_USER_INTERVENTION,IDS_STATUS_USER_INTERVENTION},// User intervention , Benutzereingriff nötig
 {PRINTER_STATUS_WAITING          ,IDS_STATUS_WAITING},          // Waiting           , Wartend
 {PRINTER_STATUS_WARMING_UP       ,IDS_STATUS_WARMING_UP},       // Warming up        , Anlaufend
 {0                               ,IDS_STATUS_READY}             // Ready             , Bereit
};

/*******************************************************************************
                                 Struktur Declarationen

*******************************************************************************/

struct ETSPRTD_PRINTER
{
   RECT      rcMargin;                           // die eingestellten Ränder
   POINT     ptOffset;                           // der zu verwendende Offset
   SIZE      siDrawArea;                         // an den Offset anschließende Ausdehnung des Druckbereiches
   SIZE      siPageSize;                         // die Gesamtgröße des eingestellten Papieres
   HDC       hDC;                                // Devicecontext zum Drucken
};

struct ETSPRTD_PARAMETER                         // Übergabedaten zur DLL
{                                                // Input
   DWORD        dwFlags;                         // Flags für Rückgabe und Darstellung
   HWND         hParent;                         // Fensterhandle wenn Dialogfeld Dargestellt werden soll
   HENHMETAFILE hPreView;                        // Handle auf Metafile für Seitenvorschau in Dialogbox
   const char * szApplication;                   // Name der Anwendung  max 128
   const char * szDocument;                      // Name des Dokumentes max 256
                                                 // Input / Output
   DWORD        dwSize;                          // größe des Puffers für die Angeforderten Daten !
   void *       pPrinterData;                    // Puffer für die Angeforderten Daten !
};

struct ETSPRTD_SPECIALDATA                       // Struktur um einen Drucker zu setzen (für SetSpecialPrinter)
{                                                // SetSpecialPrinter ist nur für die Kompatiblität von CARA zur Version 1.0 (Win95 / Nt 4.0)
   char  szPrinterName[32];                      // Achtung unter Win2k ist diese Beschränkung nicht gültig
   RECT  rcMargin;
   short dmOrientation;
   short dmPaperSize;
};

struct PRINTERPARAMETER
{
   int       nMaxCopies;                         // die maximal zuläßige Anzahl an Kopien
   bool      bPrintToFile;                       // Druckerausgabe in Datei

                                                 // dieser Datenblock wird von der Funktion "GetPageParameters" initialisiert

   POINT     ptOffset;                           // der Offset ab dem der bedruckbare Bereich auf der Seite beginnt [Deviceunits]
   SIZE      siPageSize;                         // die größe der Seite [Deviceunits]
   SIZE      siDrawArea;                         // die größe des bedruckbaren Bereiches auf der Seite [Deviceunits]
   double    dScaleX2Lom;                        // Skalierungsfaktor für X [Deviceunits] nach [Low Metric]
   double    dScaleY2Lom;                        // Skalierungsfaktor für Y [Deviceunits] nach [Low Metric]
   double    dScaleX2Dev;                        // Skalierungsfaktor für X [Low Metric] nach [Deviceunits]
   double    dScaleY2Dev;                        // Skalierungsfaktor für Y [Low Metric] nach [Deviceunits]
   RECT      rcMinMargin;                        // die minimal Ränder in [Low Metric]

   RECT      rcMargin;                           // die Eingestellten Ränder in [Low Metric]
};


/*******************************************************************************
                             die Flags für die Schnittstelle

*******************************************************************************/

#define ETSPRTD_LOWMETRICS  0x00000001           // Ich möchte ETS_PRTD_PRINTER Daten bekommen in LOWMETRIC-Einheiten
#define ETSPRTD_DEVICEUNITS 0x00000002           // Ich möchte ETS_PRTD_PRINTER Daten bekommen in DEVICEUNITS
#define ETSPRTD_ISO         0x00000004           // die Vorschau soll Isotropisch dargestellt werden
#define ETSPRTD_ANISO       0x00000008           // die Vorschau soll Anisotropisch dargestellt werden
#define ETSPRTD_RETURNDC    0x00000010           // in den ETS_PRTD_PRINTER bitte auch einen DeviceContext zum Drucken übergeben und Druckjob starten
#define ETSPRTD_GETALLDATA  0x00000020           // Ich möchte alle Druckereinstellungen erhalten
#define ETSPRTD_SETALLDATA  0x00000040           // Ich möchte alle Druckereinstellungen setzen
#define ETSPRTD_QUIET       0x00000080           // keine Meldung beim Starten eines Druckjobs über DoDataExchange, bei PrintToFile wird Dialog für Ausgabedatei angezeigt
                                                 // oder beim Testen ob ein Drucker installiert ist
#define ETSPRTD_NOPREVIEW   0x00000100           // Dialogbox ohne Vorschau aufbauen
#define ETSPRTD_CALLBACK    0x00000200           // Metafile über Callbackfunktion besorgen
#define ETSPRTD_PRTOFFSET   0x00000400           // nur Printeroffset besorgen
#define ETSPRTD_4MFCPREVIEW 0x00000800           // kein Startdoc etc. ausführen (nur bei DoDataExchange)
#define ETSPRTD_INVALID     0xfffff000           // alle nicht gültigen Flags ! (zur Fehlerabfrage)


/*******************************************************************************
                               die globalen Variablen

*******************************************************************************/

HINSTANCE        g_hInstance        = NULL;      // Instancehandle der DLL
HANDLE           g_hHeap            = NULL;      // Handle für den Heap dieser DLL
char             g_szStdPrinter[512];            // String mit dem aktuellen Standarddruckernamen (nur wegen NT nötig !)
HICON            g_hHochIcon        = NULL;      // Iconhandles für die Orientierungsdarstellung
HICON            g_hQuerIcon        = NULL;
HICON            g_hPrtd            = NULL;
DWORD            g_dwExtraSpace     = 2;         // Verkleinerung des Druckbereiches als Bug-Fix
char             g_szDecimal[2];                 // der Dezimalpunkt oder das Dezimalkomma (Sprachenabhängig)
bool             g_bInDllCode       = false;     // Flag, zur überprüfung ob diese DLL Reentrant aufgerufen wird
DWORD            g_tlsidLastError;               // ID für den Slot des Fehlercodes !

PRINTER_INFO_2 * g_pActualInfo      = NULL;      // Zeiger auf PRINTER_INFO_2 des aktuellen Druckers (Dialogfeld)
PRINTERPARAMETER g_ActualParameters;             // und die Ränder etc.

PRINTER_INFO_2 * g_pSavedInfo       = NULL;      // Zeiger auf PRINTER_INFO_2 des Eingestellten Druckers
PRINTERPARAMETER g_SavedParameters;              // und die Ränder etc.

bool             g_bPrinterDialogTyp;            // der Type der Dialogbox (Printersetup==false, Printing==true)

                                                 // Daten welche für das Initialisieren eines Druckjob benötigt werden
DOCINFO          g_DocInfo;                      // die DOCINFO-Struktur für den Printjob
char             g_szDocName[512];               // der Dokumentname + Anwendungsname für den Printjob
char             g_szOutputFile[512];            // enthält den Ausgabedateinamen wenn in eine Datei gedrucket werden soll

CEtsHelp         g_CEtsHelp;                     // für die ETSHELP.DLL

bool             g_bQuiet = false;               // Flag, keine Meldung, ob ein Drucker installiert werden soll, default == false -> Meldung eingeschaltet

/*******************************************************************************
                                  ein paar MAKROS

*******************************************************************************/

#define ALLOC(size)       HeapAlloc  (g_hHeap,0      ,(size))
#define REALLOC(mem,size) HeapReAlloc(g_hHeap,0,(mem),(size))
#define FREE(mem)         HeapFree   (g_hHeap,0,(mem))


/**************************************+****************************************
                              Funktionsprototypen

*******************************************************************************/

                                                 // für die Dialogbox Nachrichtenbearbeitung
LRESULT CALLBACK DlgMessageFunction(HWND,int,WPARAM,LPARAM);
   LRESULT OnInitDialog            (HWND);
   LRESULT OnCommand               (HWND,int,int);
      void    OnSelectPrinter      (HWND);
      void    OnChangePrinterSetup (HWND);
      void    OnOrientation        (HWND,int);
      void    OnChangeCopies       (HWND);
      void    OnChangeMargin       (HWND,int);
   LRESULT OnHelpMessage           (HWND,LPHELPINFO);
   LRESULT OnDestroy               (HWND);
                                                 // alle privaten Funktionen für die Druckerauswertung
void  InitPrinterComboBox     (HWND,PRINTER_INFO_2 **);
void  GetDefaultPrinter       (PRINTER_INFO_2 **);
void  GetPrinterParameter     (char * ,PRINTER_INFO_2 **);
DWORD GetPrinterDefaultDevMode(char * ,PDEVMODE&);
void  UpdatePrinterStatus     (HWND,PRINTER_INFO_2 *);
void  UpdatePageOrientation   (HWND,PRINTER_INFO_2 *);
void  SetPageOrientation      (HWND);
void  GetPageParameters       (PRINTER_INFO_2 *,PRINTERPARAMETER&);
void  ChangePrinterSetup      (HWND,PRINTER_INFO_2 *);
void  UpdateCopies            (HWND,PRINTER_INFO_2 *,PRINTERPARAMETER&);
void  SetCopies               (HWND,PRINTER_INFO_2 *);
bool  CheckAndUpdateMargins   (HWND,PRINTERPARAMETER&);
void  UpdatePreView           (PRINTERPARAMETER&,bool bMono);
bool  CheckAndSetPageOptions  (PRINTER_INFO_2 *,short,short);

                                                 // alle privaten Funktionen für die Schnittstellen zu der DLL
bool  FindInstalledPrinters(const char * szPrinterName);
int   GetPrinterOffset    (ETSPRTD_PARAMETER *);
int   GetAllData          (ETSPRTD_PARAMETER *);
int   SetAllData          (ETSPRTD_PARAMETER *);
int   SetSpecialPrinter   (ETSPRTD_PARAMETER *);
int   FillPrinterData     (ETSPRTD_PARAMETER *);
int   StartPrinting       (ETSPRTD_PARAMETER *);
int   GetPrinterOutputFile(ETSPRTD_PARAMETER *);

/**************************************+****************************************
                                DllProcessAttach

private Function: Initialisierung der DLL

*******************************************************************************/

bool DllProcessAttach(HINSTANCE hInstance)
{
   g_hInstance = hInstance;                      // Instancehandle der DLL

   DisableThreadLibraryCalls(hInstance);         // kein Aufruf von DLLMain bei Threaderzeugung

   g_tlsidLastError = TlsAlloc();                // Fehlercode bitte für jeden Thread lokal benutzen
   if(g_tlsidLastError==0xffffffff) return false;

   g_hHeap     = HeapCreate(0,65536,0);          // ohne Ausnahmen, werden Überall abgefangen
   if(g_hHeap==NULL)     return false;

                                                 // für die Heapidentifizierung den Namen als erstes Objekt ablegen
   char * szHeapName = (char*) ALLOC(64);        // immer 64 Bytes weil dann leichter zu finden !
   if(szHeapName) lstrcpy(szHeapName,"Local Heap from ETSPRTD.DLL");

   g_hHochIcon = (HICON) LoadImage(hInstance,MAKEINTRESOURCE(IDI_ICON3),IMAGE_ICON,32,32,LR_DEFAULTCOLOR);
   if(g_hHochIcon==NULL) return false;

   g_hQuerIcon = (HICON) LoadImage(hInstance,MAKEINTRESOURCE(IDI_ICON2),IMAGE_ICON,32,32,LR_DEFAULTCOLOR);
   if(g_hQuerIcon==NULL) return false;

   g_hPrtd     = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON1));

                                                 // besorge das Trennzeichen für "Nachkommastellen"
   GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SDECIMAL,g_szDecimal,2);

   InitPreViewWnd();                             // die Vorschaufensterklasse registrieren

   GetRegValues();

   return true;
}


/**************************************+****************************************
                                DllProcessDetach
                                  
private Function: Aufräumarbeiten

*******************************************************************************/

bool DllProcessDetach()
{
   TlsFree(g_tlsidLastError);                    // den Slot für die Fehlercodes wieder freigeben

   if(g_hHeap)     HeapDestroy(g_hHeap);         // den Heap mit den Druckerdaten löschen
   if(g_hHochIcon) DestroyIcon(g_hHochIcon);     // die die Icons für die Orientierung löschen
   if(g_hQuerIcon) DestroyIcon(g_hQuerIcon);
   if(g_hPrtd)     DestroyIcon(g_hPrtd);

   ReleasePreViewWnd();                          // die Vorschaufensterklasse unregistrieren

   return false;                                 // dieser Wert ist egal !
}

/**************************************+****************************************
                                  GetRegValues

private Function: einlesen der Registry-Werte für DLL-Optionen
*******************************************************************************/

void GetRegValues()
{
   HKEY hKey;

   if(RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\ETSKIEL\\ETSPRTD",0,KEY_READ,&hKey)==ERROR_SUCCESS)
   {
      DWORD dwSize = sizeof(DWORD), dwValue, dwType;

      dwSize = sizeof(DWORD);

      if(RegQueryValueEx(hKey,"Space",NULL,&dwType,(BYTE*)&dwValue,&dwSize)==ERROR_SUCCESS)
      {
         if(dwType==REG_DWORD)
         {
            if(dwValue < 100)                    // aber maximal nur bis 100
               g_dwExtraSpace = dwValue;         // übernehme neuen printable area bug fix
         }
      }

      dwSize = sizeof(DWORD);

      if(RegQueryValueEx(hKey,"Report",NULL,&dwType,(BYTE*)&dwValue,&dwSize)==ERROR_SUCCESS)
      {
         if(dwType==REG_DWORD)
         {
            if(dwValue!=0)
            {
               g_bGenerateReport = true;         // erzeuge ein Reportfile
               DeleteFile("ETSPRTD.DBG");        // lösche vorhergehendes File
            }
         }
      }

      RegCloseKey(hKey);
   }
}


/*******************************************************************************
                              Rounding(double)

private Function: runde double nach int besser und schneller als floor

*******************************************************************************/

int _fastcall Rounding(double value)
{
   if(value== 0.0) return 0;
   else
   if(value > 0.0) return (((int)(value+value))+1)>>1;
   else            return -((((int)(-2.0*value))+1)>>1);
}

/*******************************************************************************
                                PutLowMetrics

private Function: die Zahl nValue in ein MarginEditControl des Dialogfeldes
                  eintragen

*******************************************************************************/

void PutLowMetrics(HWND hDlg,int nId,int nValue)
{
   char szValue[128];
   int  mm  = nValue / 10;
   int  dec = nValue - 10 * mm;

   wsprintf(szValue,"%d%s%d",mm,g_szDecimal,dec);

   SetDlgItemText(hDlg,nId,szValue);
}

/*******************************************************************************
                                GetLowMetrics

private Function: eine Zahl rnValue aus einem MarginEditControl des Dialogfeldes
                  besorgen

*******************************************************************************/

void GetLowMetrics(HWND hDlg,int nId,int& rnValue)
{
   char   szValue[128];
   int    len;
   double dValue;

   GetDlgItemText(hDlg,nId,szValue,128);

   len = lstrlen(szValue);

   if(len)                                       // ein String vorhanden ?
   {
      if(g_szDecimal[0]!=46)                     // ist eine Länderanpassung nötig ?
      {
         for(int i=0;i<len;i++)                  // Ja, den Landspezifischen Dezimalpunkt durch ´.´ ersetzen (nötig für atof)
         {
            if(szValue[i]==g_szDecimal[0])
            {
               szValue[i]=46;
            }
         }
      }

      dValue  = atof(szValue);                   // Wert umwandeln
      rnValue = Rounding(10*dValue);             // und nach LOWMETRIC runden
   }
   else rnValue = 0;
}


/**************************************+****************************************
                              MessageBoxNoPrinter

private Function: dem Anwender mitteilen, daß kein Drucker installiert ist und
                  ihm ermöglichen (durch control-applet) einen zu installieren.

*******************************************************************************/

void MessageBoxNoPrinter()
{
   if(g_bQuiet) return;                          // keine Meldung ausgeben !

   char szText [256]="";
   char szTitle[256]="";

   LoadString(g_hInstance,IDS_MBNP_TEXT ,szText ,256);
   LoadString(g_hInstance,IDS_MBNP_TITLE,szTitle,256);

   if(MessageBox(NULL,szText,szTitle,MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL)==IDYES)
   {
      OSVERSIONINFO osvi;                        // test ob Ich auf NT oder Win95/98 laufe
      bool          bDoOldMethod = false;

      osvi.dwOSVersionInfoSize = sizeof(osvi);

      if(GetVersionEx(&osvi)&&                   // Operation System is not NT
         (osvi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS))
      {                                          // ist es Windoof Win95 (ABC) oder Win98 (SE)
         if(osvi.dwMajorVersion==4)
         {
            bDoOldMethod = true;
         }
      }

      if(bDoOldMethod)                           // alte Methode verwenden
      {
         STARTUPINFO         startupinfo;
         PROCESS_INFORMATION pinfo;

         ZeroMemory(&startupinfo,sizeof(startupinfo));
         ZeroMemory(&pinfo      ,sizeof(pinfo));

         startupinfo.cb          = sizeof(startupinfo);
         startupinfo.wShowWindow = SW_SHOW;

         if(CreateProcess(NULL,                  // super Trick, öffnet den Druckerordner (unter WIN95/98)
                                                 // unter NT 4.0 (getestet, OK) und 5.0 Beta 2 (getestet, OK)
                          "control.exe main.cpl @2",
                          NULL,                  // Win2000 getestet , funktioniert nicht !!!!!!!!!!!!
                          NULL,
                          false,
                          0,
                          NULL,
                          NULL,
                          &startupinfo,
                          &pinfo))
         {
            if(pinfo.hProcess) CloseHandle(pinfo.hProcess);
            if(pinfo.hThread)  CloseHandle(pinfo.hThread);
         }
      }
      else                                          // neue Methode verwenden
      {
   
         HRESULT      hres;                         // dieser Code funktioniert auf Win95B Win98 Win NT und Win2000
         LPITEMIDLIST pidl;                         // andere Versionen noch nicht getestet
         LPMALLOC     pIMalloc;

         CoInitialize(NULL);                        // Ich benötige OLE32

         hres = SHGetMalloc(&pIMalloc);             // Shell - Interface für Speicherfreigabe etc.

         if(SUCCEEDED(hres))                        // OK, dann weiter
         {                                          // besorge die ITEMIDLIST Struktur für das Verzeichnis (Printers / Drucker)
            hres = SHGetSpecialFolderLocation(NULL,CSIDL_PRINTERS,&pidl);

            if(SUCCEEDED(hres))                     // idl konnte besorgt werden
            {
               SHELLEXECUTEINFO sei;                // Ok, mit ShellExecute dieses Verzeichnis öffnen
                                                    // als Explorer - Fenster
               ZeroMemory(&sei,sizeof(sei));

               sei.cbSize   = sizeof(sei);
               sei.fMask    = SEE_MASK_IDLIST | SEE_MASK_INVOKEIDLIST;
               sei.lpVerb   = "open";
               sei.nShow    = SW_SHOW;
               sei.lpIDList = pidl;                 // ShellExecute soll über die idl zugreifen

               ShellExecuteEx(&sei);                // Explorer - Fenster öffnen

               pIMalloc->Free(pidl);                // den Pointer auf idl freigeben
            }
            pIMalloc->Release();                    // Interface freigeben
         }

         CoUninitialize();                          // OLE32 beenden
      }
   }
}


/*******************************************************************************
                               TreatException

private Function: eine Laufzeitfehler behandeln

*******************************************************************************/

void TreatException(int error)
{
   if (error==ERROR_PROCEED) return;             // die Ausnahme wurde schon behandelt !
                                                 // Fehlercode Threadlokal speichern
   TlsSetValue(g_tlsidLastError, (void*)(DLLE|error));

   #ifdef ETSDEBUG_INCLUDE                       // soll der Fehler auch ausgegeben werden (Report / Debug)
      char szError[256];                         // hier die Klartextfehlermeldung ausgeben
      wsprintf(szError,"Error:%s !",g_szERRORS[error]);
      REPORT(szError);
   #endif

   if(error==ERROR_NOPRINTER)
      MessageBoxNoPrinter();                     // Es ist kein Drucker installiert !
   else                                          // alle anderen Fehler
   {
      if(error==ERROR_PRINTERNOTFOUND)           // diese Meldung nicht ausgeben, ist anders zu behandeln
         return;

      char szError[256];

      wsprintf(szError,"Errorcode: %#X",DLLE|error);
      MessageBox(NULL,szError,"Critical runtime error in module ETSPRTD.DLL",MB_OK|MB_TASKMODAL|MB_ICONSTOP);
   }
}




/**************************************+****************************************
                               TestSavedPrinter

private Function: überprüfe ob es schon einen ausgewählten Drucker gibt,
                  sonst versuche den Standarddrucker zu besorgen
                  mit seinen Standardeinstellungen etc.

*******************************************************************************/

void TestSavedPrinter()
{
   BEGIN("TestSavedPrinter");

   try
   {
      if(!g_hHeap) throw ERROR_HEAPHANDLE;

      if(g_pSavedInfo==NULL)                     // noch kein vorhergehender Drucker vorhanden ?
      {
         GetDefaultPrinter(&g_pSavedInfo);       // besorge den Standarddrucker (bein Fehler nach catch)
                                                 // besorge die Parameter des Standarddruckers
         GetPageParameters(g_pSavedInfo,g_SavedParameters);
                                                 // die Grundeinstellungen der Ränder setzen
         g_SavedParameters.rcMargin.left   = 127;
         g_SavedParameters.rcMargin.top    = 127;
         g_SavedParameters.rcMargin.right  = 127;
         g_SavedParameters.rcMargin.bottom = 127;
                                                 // nicht in eine Datei ausgeben
         g_SavedParameters.bPrintToFile    = false;
         g_SavedParameters.nMaxCopies      = 1;  // nur 1 Kopie drucken
      }
   }
   catch(int error)                              // alle Ausnahmen abfangen
   {
      TreatException(error);                     // Ausnahme behandeln
      throw ERROR_PROCEED;
   }
}




/*******************************************************************************
                               DlgMessageFunction

callback Function: die Nachrichtenfunktion der Dialogbox
*******************************************************************************/

LRESULT CALLBACK DlgMessageFunction(HWND hDlg,int nMessage,WPARAM wParam,LPARAM lParam)
{
   switch(nMessage)
   {
      case WM_COMMAND:    return OnCommand(hDlg,LOWORD(wParam),HIWORD(wParam));
      case WM_INITDIALOG: return OnInitDialog(hDlg);
      case WM_DESTROY:    return OnDestroy(hDlg);
      case WM_HELP:       return OnHelpMessage(hDlg,(LPHELPINFO)lParam);
   }
   return (FALSE);
}


/*******************************************************************************
                                 OnInitDialog

private Function: Dialogbox initialisieren
*******************************************************************************/

LRESULT OnInitDialog(HWND hDlg)
{
   BEGIN("OnInitDialog");

   try
   {
      if(g_hPrtd) SendMessage(hDlg,WM_SETICON,ICON_SMALL,(LPARAM) g_hPrtd);

      char szTitle[256];

      if(g_bPrinterDialogTyp)
      {
          LoadString(g_hInstance,IDS_DPRINTING,szTitle,256);
          SetDlgItemText(hDlg,IDOK,szTitle);     // OK-Button zu Drucken ändern
      }
      else LoadString(g_hInstance,IDS_DSETUP   ,szTitle,256);

      SendMessage(hDlg,WM_SETTEXT,0,(LPARAM) szTitle);

      InitPrinterComboBox(GetDlgItem(hDlg,IDC_SELECTPRINTER),&g_pActualInfo);
      UpdatePrinterStatus(hDlg,g_pActualInfo);
      UpdatePageOrientation(hDlg,g_pActualInfo);
      SetCopies(hDlg,g_pActualInfo);
      UpdateCopies(hDlg,g_pActualInfo,g_ActualParameters);
      CheckAndUpdateMargins(hDlg,g_ActualParameters);

      if(g_ActualParameters.bPrintToFile)        // Ausdruck in Datei ?
      {                                          // dann die zugehörige Checkbox setzen
         SendDlgItemMessage(hDlg,IDC_FILE,BM_SETCHECK,1,0);
      }
                                                 // Vorschaufenster initialisieren wenn vorhanden
      HWND hPage = GetDlgItem(hDlg,IDC_PAGEPREVIEW);
      if(IsWindow(hPage))                        // nur wenn Vorschau auch erzeugt werden soll
      {
         RECT rc;
         GetClientRect(hPage,&rc);
         CreatePreViewWnd(hPage,0,0,rc.right,rc.bottom);

         bool bMono = true;

         if( g_pActualInfo->pDevMode->dmFields&DM_COLOR)
            if(g_pActualInfo->pDevMode->dmColor 
               == DMCOLOR_COLOR) bMono = false;

         UpdatePreView(g_ActualParameters,bMono);
      }
   }
   catch(int error)
   {
      TreatException(error);
      EndDialog(hDlg,-1);                        // Dialogfeld mit Fehler beenden
   }

   return (TRUE);
}


/*******************************************************************************
                                  OnCommand

private Function: Dialogbox Control-Commands bearbeiten
*******************************************************************************/

LRESULT OnCommand(HWND hDlg,int nCommand,int flags)
{
   switch(nCommand)
   {
      case IDC_SELECTPRINTER:
           if(flags==CBN_SELCHANGE)
           {
              OnSelectPrinter(hDlg);
           }
           break;

      case IDC_BUTTON_SETUP:
           if(flags==BN_CLICKED)
           {
              OnChangePrinterSetup(hDlg);
           }
           break;

      case IDC_PORTRAIT:
      case IDC_LANDSCAPE:
           if(flags==BN_CLICKED)
           {
              OnOrientation(hDlg,nCommand);
           }
           break;

      case IDC_COPIES:
           if(flags==EN_KILLFOCUS)
           {
              OnChangeCopies(hDlg);
           }
           break;
      case IDC_LEFT:
      case IDC_TOP:
      case IDC_RIGHT:
      case IDC_BOTTOM:
           if(flags==EN_KILLFOCUS)
           {
              OnChangeMargin(hDlg,nCommand);
           }
           break;

      case IDOK:
           if(flags==BN_CLICKED)
           {
              HWND from;

              if((from=GetFocus())!=NULL)        // DEFPUSHBUTTON - Umgehung
              {  
                 if(from!=GetDlgItem(hDlg,IDOK))
                 {
                    PostMessage(hDlg,WM_NEXTDLGCTL,0,0);
                    break;
                 }
              }

              if(g_pActualInfo)
              {
                 if(g_pSavedInfo) FREE(g_pSavedInfo);
                 g_pSavedInfo      = g_pActualInfo;
                 g_SavedParameters = g_ActualParameters;
                 g_pActualInfo     = NULL;

                 if(SendDlgItemMessage(hDlg,IDC_FILE,BM_GETCHECK,0,0)&BST_CHECKED)
                      g_SavedParameters.bPrintToFile = true;
                 else g_SavedParameters.bPrintToFile = false;
              }

              EndDialog(hDlg,IDOK);
           }
           break;

      case IDCANCEL:
           if(g_pActualInfo)
           {
              FREE(g_pActualInfo);
              g_pActualInfo= NULL;
           }
           EndDialog(hDlg,IDCANCEL);
           break;

      default: return (FALSE);
   }
   return (TRUE);
}


/*******************************************************************************
                                 OnSelectPrinter

private Function: neuen Drucker mit Kombinationsfeld auswählen
*******************************************************************************/

void OnSelectPrinter(HWND hDlg)
{
   int  nSel;
   char szName[256];

   BEGIN("OnSelectPrinter");

   try
   {                                             // besorge den Index des ausgewählten Druckers
      nSel = SendDlgItemMessage(hDlg,IDC_SELECTPRINTER,CB_GETCURSEL,0,0);
                                                 // besorge über den Index den Druckernamen
      SendDlgItemMessage(hDlg,IDC_SELECTPRINTER,CB_GETLBTEXT,nSel,(LPARAM) szName);
      if(g_pActualInfo==NULL) throw ERROR_PINFO; // gibt es einen Drucker ?
                                                 // überprüfe ob sich der Drucker wirklich geändert hat
      if(lstrcmp(szName,g_pActualInfo->pPrinterName)!=NULL)
      {
         REPORT("printer selection changed to: %s",szName);

         FREE(g_pActualInfo);                    // vorhergehende Druckerparameter löschen
                                                 // Druckerparameter neu besorgen !
         GetPrinterParameter(szName,&g_pActualInfo);
         UpdatePrinterStatus(hDlg,g_pActualInfo);
         SetPageOrientation(hDlg);
         UpdateCopies(hDlg,g_pActualInfo,g_ActualParameters);
         GetPageParameters(g_pActualInfo,g_ActualParameters);
         CheckAndUpdateMargins(hDlg,g_ActualParameters);
      }
   }
   catch(int error)
   {
      TreatException(error);
      EndDialog(hDlg,-1);
   }
}


/*******************************************************************************
                                 OnSelectPrinter

private Function: neuen Drucker mit Kombinationsfeld auswählen
*******************************************************************************/

void OnChangePrinterSetup(HWND hDlg)
{
   BEGIN("OnChangePrinterSetup");

   try
   {
      ChangePrinterSetup(hDlg,g_pActualInfo);
   }
   catch(int error)
   {
      TreatException(error);
      EndDialog(hDlg,-1);
   }
}


/*******************************************************************************
                                 OnOrientation

private Function: Ändere die Papierausrichtung
*******************************************************************************/

void OnOrientation(HWND hDlg,int command)
{
   BEGIN("OnOrientation");

                                                 // eine Sicherheitsabfrage (es kann Drucker geben welche kein LANDSCAPE unterstützen)
   if(g_pActualInfo->pDevMode->dmFields&DM_ORIENTATION)
   {
      try
      {
         if(command==IDC_PORTRAIT)
         {
            g_pActualInfo->pDevMode->dmOrientation=DMORIENT_PORTRAIT;
            SendDlgItemMessage(hDlg,IDC_ORIENTICON,STM_SETICON,(WPARAM) g_hHochIcon,NULL);
         }
         else
         {
            g_pActualInfo->pDevMode->dmOrientation=DMORIENT_LANDSCAPE;
            SendDlgItemMessage(hDlg,IDC_ORIENTICON,STM_SETICON,(WPARAM) g_hQuerIcon,NULL);
         }

         GetPageParameters(g_pActualInfo,g_ActualParameters);
         CheckAndUpdateMargins(hDlg,g_ActualParameters);
      }
      catch(int error)
      {
         TreatException(error);
         EndDialog(hDlg,-1);
      }
   }
}


/*******************************************************************************
                                 OnChangeCopies

private Function: eine neue Anzahl von Kopien übernehmen
*******************************************************************************/

void OnChangeCopies(HWND hDlg)
{
   BOOL  Translated;
   int   value;

   BEGIN("OnChangeCopies");

   try
   {
      if(g_pActualInfo==NULL) throw ERROR_PINFO;
      if(g_pActualInfo->pDevMode==NULL) throw ERROR_PDEVMODE;
      if(g_pActualInfo->pDevMode->dmFields&DM_COPIES)
      {
         value = GetDlgItemInt(hDlg,IDC_COPIES,&Translated,FALSE);
         if((value < 1)||(Translated==FALSE)) value = 1;
         else if(value > g_ActualParameters.nMaxCopies) value = g_ActualParameters.nMaxCopies;
         SetDlgItemInt(hDlg,IDC_COPIES,value,FALSE);
         g_pActualInfo->pDevMode->dmCopies = (short) value;
      }
   }
   catch(int error)
   {
      TreatException(error);
      EndDialog(hDlg,-1);
   }
}

/*******************************************************************************
                                  OnChangeMargin

private Function: eine Randeinstellung wurde geändert
*******************************************************************************/

void OnChangeMargin(HWND hDlg,int nId)
{
   BEGIN("OnChangeMargin");

   try
   {
      int nNew;
      
      GetLowMetrics(hDlg,nId,nNew);

      switch(nId)
      {
         case IDC_LEFT  : g_ActualParameters.rcMargin.left   = nNew;break;
         case IDC_TOP   : g_ActualParameters.rcMargin.top    = nNew;break;
         case IDC_RIGHT : g_ActualParameters.rcMargin.right  = nNew;break;
         case IDC_BOTTOM: g_ActualParameters.rcMargin.bottom = nNew;break;
      }

      CheckAndUpdateMargins(hDlg,g_ActualParameters);
   }
   catch(int error)
   {
      TreatException(error);
      EndDialog(hDlg,-1);
   }
}


/*************************************+*****************************************
                                OnHelpMessage

private Function: Kontexthilfe bearbeiten oder weiterleiten
*******************************************************************************/

LRESULT OnHelpMessage(HWND hDlg,LPHELPINFO hi)
{
   char  szText[1024];
   RECT  rc;
   DWORD id;

   if(hi->iContextType==HELPINFO_WINDOW)
   {
      if(hi->iCtrlId==IDOK)                      // der OK-Button ?
      {
         if(!g_bPrinterDialogTyp) id = IDHS_OK;  // Hilfetext für Setupdialog
         else id = IDHS_PRINTOK;                 // Hilfetext für Printerdialog
      }
      else id = hi->dwContextId;                 // besorge ID für Hilfetext

      if(LoadString(g_hInstance,id,szText,1024)) // laden des Hilfetextes
      {                                          // überprüfe ob Mausposition innerhalb des Controls liegt
                                                 // denn Kontexthilfe kann auch mit F1 ausgelößt werden
                                                 // und dann ist das Fenster irgendwo (diese wurde in Version 1.0 vergessen !)
         GetWindowRect(GetDlgItem(hDlg,hi->iCtrlId),&rc);
          
         if(PtInRect(&rc,hi->MousePos))
         {                                       // Hilfefenster an Mausposition erzeugen
            CEtsHelp::CreateContextWnd(szText,hi->MousePos.x,hi->MousePos.y);
         }
         else
         {                                       // Hilfefenster an Controlposition erzeugen
            CEtsHelp::CreateContextWnd(szText,rc.left + 3,rc.top + 3);
         }
      }
   }
   return (TRUE);
}



/*******************************************************************************
                                   OnDestroy

private Function: Aufräumarbeiten, wenn Dialogbox geschlossen wird
*******************************************************************************/

LRESULT OnDestroy(HWND hDlg)
{
   BEGIN("OnDestroy");
                                                 // das Icon aus dem Darstellungsconrtol entfernen
   SendDlgItemMessage(hDlg,IDC_ORIENTICON,STM_SETICON,(WPARAM) NULL,NULL);

   return (TRUE);
}


/*******************************************************************************
                              InitPrinterComboBox

private Function: Kombinationsfeld mit allen vorhandenen Druckern füllen
                  und den aktuell eingestellten Drucker vorbelegen, fals
                  dieser nicht mehr vorhanden ist den neuen Standarddrucker
                  besorgen !

return          : true = Ok , false = Fehler
*******************************************************************************/

void InitPrinterComboBox(HWND hControl,PRINTER_INFO_2 **ppInfo)
{
   DWORD            i;
   DWORD            nSel;
   DWORD            dwNeeded;
   DWORD            dwPrinters;
   DWORD            dwHave       = 4096;
   PRINTER_INFO_5 * pPrinterInfo = NULL;

   BEGIN("InitPrinterComboBox");

   try
   {
      pPrinterInfo = (PRINTER_INFO_5 *) ALLOC(dwHave);

      if(pPrinterInfo==NULL) throw ERROR_PPRTINFO;

      if(!EnumPrinters(PRINTER_ENUM_LOCAL|
                       PRINTER_ENUM_CONNECTIONS, // besorge die benötigte Puffergröße für alle vorhandenen Drucker
                       NULL,
                       5,
                       (LPBYTE) pPrinterInfo,
                       dwHave,
                       &dwNeeded,
                       &dwPrinters)) 
      {
         if(GetLastError()==ERROR_INSUFFICIENT_BUFFER)
         {
            if(dwNeeded==NULL) throw ERROR_DWNEEDED;   // Puffergröße Ok ?

            FREE(pPrinterInfo);

            pPrinterInfo = NULL;                 // nur zur Sicherheit
            
            pPrinterInfo = (PRINTER_INFO_5 *) ALLOC(dwNeeded);

            dwHave = dwNeeded;

            if(!EnumPrinters(PRINTER_ENUM_LOCAL|
                             PRINTER_ENUM_CONNECTIONS,
                             NULL,
                             5,
                             (LPBYTE) pPrinterInfo,
                             dwHave,
                             &dwNeeded,
                             &dwPrinters)) throw ERROR_ENUMPRTFAILED;
         }
         else throw ERROR_ENUMPRTFAILED;
      }

      if(dwPrinters == 0) throw ERROR_ENUMNOPRINTER;

      REPORT("find %d printers",dwPrinters);     // Anzahl der gefundenen Drucker ausgeben
                                                 // ist der Control-Fenster-Handle ok
      if(hControl==NULL)      throw ERROR_HWND;
      if(!IsWindow(hControl)) throw ERROR_HWND;

      for (i=0;i<dwPrinters;i++)                 // und alle Druckernamen an den Dialogcontrol übertragen
      {
         SendMessage(hControl,CB_ADDSTRING,0,(LPARAM) pPrinterInfo[i].pPrinterName);
      }

      nSel = 0;                                  // den ersten Eintrag wählen

      if(*ppInfo)                                // ein aktueller Druckername vorhanden
      {                                          // dann diesen Auswählen
         nSel = SendMessage(hControl,CB_FINDSTRINGEXACT,-1,(LPARAM) (*ppInfo)->pPrinterName);

         if(nSel==CB_ERR)                        // Drucker wurde nicht gefunden !
         {
            REPORT("printer %s not found !",(*ppInfo)->pPrinterName);

            GetDefaultPrinter(ppInfo);

            nSel = SendMessage(hControl,CB_FINDSTRINGEXACT,-1,(LPARAM) (*ppInfo)->pPrinterName);
         }
      }

      REPORT("select printer with index %d",nSel);

      SendMessage(hControl,CB_SETCURSEL,nSel,0); // den Eintrag (nSel) wählen
      FREE(pPrinterInfo);                        // Puffer freigeben
   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);
      if(pPrinterInfo) FREE(pPrinterInfo);       // Puffer freigeben wenn er vorhanden ist
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}


/*******************************************************************************
                                GetDefaultPrinter

private Function: besorge den Standarddrucker 

return          : *ppInfo wird mit der PRINTER_INFO_2 Struktur gefüllt oder                  
                  wird NULL wenn kein Drucker vorhanden ist !
*******************************************************************************/

void GetDefaultPrinter(PRINTER_INFO_2 **ppInfo)
{
   char szPrinter[1024];                         // Puffer für den Druckernamen
   int  i = 0;                                   // Wichtig !

   BEGIN("GetDefaultPrinter");

   try
   {
      if(ppInfo==NULL) throw ERROR_PPINFO;       // Übergabezeiger ist ungültig !

      *ppInfo = NULL;                            // Setze die Kennung, kein Drucker vorhanden

      if(!GetProfileString("windows","device","none",szPrinter,1024)) 
         throw ERROR_GETPROFILE;
      if(lstrcmp(szPrinter,"none")==NULL) 
         throw ERROR_NOPRINTER;

      while(szPrinter[i]!=44)                    // suche das Komma hinter dem Druckernamen
      {
         i++;
         if(i>=1024) throw ERROR_COMMASEARCH;    // Pufferüberlauf verhindern !
      }

      szPrinter[i] = 0;                          // String beim Komma beenden !

      REPORT("default printer : %s",szPrinter);  // Druckername in Skript schreiben

      if(lstrlen(szPrinter) > 511)               // ist der Druckername zu lang ?
         throw ERROR_STRINGOVERFLOW;

      lstrcpy(g_szStdPrinter,szPrinter);         // Namen in globaler Variable sichern
      GetPrinterParameter(szPrinter,ppInfo);     // Drucker Informationen besorgen !
   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}

/*******************************************************************************
                                GetPrinterParameter

private Function: besorge alle Druckereinstellungen

return          : *ppInfo wird mit der PRINTER_INFO_2 Struktur gefüllt oder
                  zu NULL wenn ein Fehler auftritt
*******************************************************************************/

                                                 // neue Funktion für GetPrinterParameter für Pointer Durchdringungs Test für BugFix (Canon BJC 4300)
bool PointersIntersect(void * p1,void * p2,int size)
{
   int dis = (int)((LPBYTE)p2-(LPBYTE)p1);

   if((dis >=0)&&(dis < size)) return true;
   else return false;
}


void GetPrinterParameter(char * szPrinterName,PRINTER_INFO_2 **ppInfo)
{
   HANDLE   hPrinter = NULL;
   DWORD    dwNeeded = 0;
   PDEVMODE pDevMode = NULL;

   BEGIN("GetPrinterParameter");

   try
   {
      if(ppInfo==NULL) throw ERROR_PPINFO;
                                                 // öffnet den Drucker, um Informationen zu besorgen
      if(!OpenPrinter(szPrinterName,&hPrinter,NULL)) 
         throw ERROR_OPENPRTFAILED;
                                                 // besorge die benötigte Puffergröße für PRINTER_INFO_2
      if(GetPrinter(hPrinter,2,NULL,0,&dwNeeded)) 
         throw ERROR_GETPRTNO;
      if(GetLastError()!=ERROR_INSUFFICIENT_BUFFER) 
         throw ERROR_GETPRTFAILED;
      if(dwNeeded==NULL) throw ERROR_DWNEEDED;
                                                 // Ok, besorge den Puffer für PRINTER_INFO_2
      *ppInfo = (PRINTER_INFO_2 *) ALLOC(dwNeeded);
      if(*ppInfo==NULL) throw ERROR_ALLOCPPINFO;

                                                 // Ok, fülle den Puffer mit den Informationen
      if(!GetPrinter(hPrinter,2,(LPBYTE) *ppInfo,dwNeeded,&dwNeeded)) 
         throw ERROR_GETPRTGET;

                                                 // Teste ob sich Stringpointer und DevMode-Daten Speicher teilen
                                                 // BugFix für z.B. Canon BJC 4300 (neu 11.1.2000)
     if((*ppInfo)->pDevMode)                     // neu für Version 1.1 von ETSPRTD.DLL
     {
        PDEVMODE pDev  = (*ppInfo)->pDevMode;
        bool     share = false;
        int      size  = pDev->dmSize + pDev->dmDriverExtra;

        if((*ppInfo)->pServerName)     share|= PointersIntersect(pDev,(*ppInfo)->pServerName    ,size);
        if((*ppInfo)->pPrinterName)    share|= PointersIntersect(pDev,(*ppInfo)->pPrinterName   ,size);
        if((*ppInfo)->pShareName)      share|= PointersIntersect(pDev,(*ppInfo)->pShareName     ,size);
        if((*ppInfo)->pPortName)       share|= PointersIntersect(pDev,(*ppInfo)->pPortName      ,size);
        if((*ppInfo)->pDriverName)     share|= PointersIntersect(pDev,(*ppInfo)->pDriverName    ,size);
        if((*ppInfo)->pComment)        share|= PointersIntersect(pDev,(*ppInfo)->pComment       ,size);
        if((*ppInfo)->pLocation)       share|= PointersIntersect(pDev,(*ppInfo)->pLocation      ,size);
                                                 // pDevMode darf sich durchdringen
        if((*ppInfo)->pSepFile)        share|= PointersIntersect(pDev,(*ppInfo)->pSepFile       ,size);
        if((*ppInfo)->pPrintProcessor) share|= PointersIntersect(pDev,(*ppInfo)->pPrintProcessor,size);
        if((*ppInfo)->pDatatype)       share|= PointersIntersect(pDev,(*ppInfo)->pDatatype      ,size);
        if((*ppInfo)->pParameters)     share|= PointersIntersect(pDev,(*ppInfo)->pParameters    ,size);
                                                 // pSecurityDescriptor sollte null sein bei 95/98 und bei NT Irgentwas ?
        if((*ppInfo)->pSecurityDescriptor)
        {
           share|= PointersIntersect(pDev,(*ppInfo)->pSecurityDescriptor,size);
        }

        if(share)                                // BugFix für Canon BJC 4300 o.ä. durchführen
        {
           REPORT("pointer sharing violation, correct this by clearing pDevMode");

           (*ppInfo)->pDevMode = NULL;
        }
      }

      if((*ppInfo)->pDevMode==NULL)              // ein Drucker der sich nicht an die Windowsspezification hält oder obiger BugFix
      {                                          // der hauptsächliche CARA 1.1 Bugfix für das Drucken !
         DWORD    dwSize   = NULL;
         int      oldadr   = abs((int)(*ppInfo));// die alte Addresse von *ppInfo zwischenspeichern
         int      newadr;
         int      dispatch;

         REPORT("found printer without pDevMode, try to correct this problem");
                                                 // besorge die Default DevMode Struktur für diesen Drucker
         dwSize = GetPrinterDefaultDevMode(szPrinterName,pDevMode);
         if(pDevMode==NULL) throw ERROR_PDEVMODE;// ist pDevMode gültig ?
                                                 // Ok, Puffer für PRINTER_INFO_2 vergrößern, so daß pDevMode angehängt werden kann
         *ppInfo = (PRINTER_INFO_2 *) REALLOC(*ppInfo,dwNeeded+dwSize);
                                                 // konnte der Puffer vergrößert werden ?
         if((*ppInfo)==NULL) throw ERROR_REALLOCPPINFO;

         newadr   = abs((int)(*ppInfo));         // dispatch ist für die Zeigerkorrektur nötig !
         dispatch = (newadr - oldadr);
                                                 // berechne den Zeiger der DevMode-Daten in PRINTER_INFO_2 und trage ihn ein
         (*ppInfo)->pDevMode=(PDEVMODE) (((char*)*ppInfo)+dwNeeded);
                                                 // kopiere die DevMode Daten an das Ende von PRINTER_INFO_2
         REPORT("copy pDevMode to PRINTER_INFO_2");
         CopyMemory((*ppInfo)->pDevMode,pDevMode,dwSize);
                                                 // durch die Reallocation kann die Struktur im Speicher verschoben worden sein,
                                                 // deswegen müssen alle Pointer neu berechnet werden
         REPORT("correct pointers in PRINTER_INFO_2");
         if((*ppInfo)->pServerName)     (*ppInfo)->pServerName     += dispatch;
         if((*ppInfo)->pPrinterName)    (*ppInfo)->pPrinterName    += dispatch;
         if((*ppInfo)->pShareName)      (*ppInfo)->pShareName      += dispatch;
         if((*ppInfo)->pPortName)       (*ppInfo)->pPortName       += dispatch;
         if((*ppInfo)->pDriverName)     (*ppInfo)->pDriverName     += dispatch;
         if((*ppInfo)->pComment)        (*ppInfo)->pComment        += dispatch;
         if((*ppInfo)->pLocation)       (*ppInfo)->pLocation       += dispatch;
                                                 // pDevMode ist neu
         if((*ppInfo)->pSepFile)        (*ppInfo)->pSepFile        += dispatch;
         if((*ppInfo)->pPrintProcessor) (*ppInfo)->pPrintProcessor += dispatch;
         if((*ppInfo)->pDatatype)       (*ppInfo)->pDatatype       += dispatch;
         if((*ppInfo)->pParameters)     (*ppInfo)->pParameters     += dispatch;
                                                 // pSecurityDescriptor sollte null sein bei 95/98 und bei NT Irgentwas ?
         if((*ppInfo)->pSecurityDescriptor)
         {
            char * value = (char*) (*ppInfo)->pSecurityDescriptor;
            value+= dispatch;
            (*ppInfo)->pSecurityDescriptor = (PSECURITY_DESCRIPTOR) value;
         }

         FREE(pDevMode);                         // und die DevMode Daten löschen, da kopiert                                   
      }

      if(hPrinter)                               // Drucker noch geöffnet ?
      {
         if(!ClosePrinter(hPrinter))
         {
            REPORT("ClosePrinter(): failed !");
         }
      }
   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);

      if((ppInfo)&&(*ppInfo!=NULL))              // Wenn noch Speicher allociert ist, diesen freigeben
      {
         FREE(*ppInfo);
         *ppInfo = NULL;                         // Kennung für, kein Drucker vorhanden
      }

      if(hPrinter)                               // Drucker noch geöffnet ?
      {
         if(!ClosePrinter(hPrinter))
         {
            REPORT("ClosePrinter(): failed !");
         }
      }

      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}


/*******************************************************************************
                             GetPrinterDefaultDevMode

private Function: die Default DevMode Struktur für den Drucker besorgen
 
return          : !=0 die größe der DevMode-Struktur und pDevMode ist gefüllt
                  ==0 Fehler und pDevMode ist NULL
*******************************************************************************/

DWORD GetPrinterDefaultDevMode(char * szPrinterName,PDEVMODE& pDevMode)
{
   DWORD    dwNeeded;

   BEGIN("GetPrinterDefaultDevMode");

   try
   {
      pDevMode = NULL;                           // pDevMode initialisieren
                                                 // benötigte Puffergröße besorgen
      dwNeeded = DocumentProperties(NULL,NULL,szPrinterName,NULL,NULL,0);
      if(dwNeeded==0) throw ERROR_DOCPROPSIZE;   // konnte Puffergröße besorgt werden ?
      pDevMode = (PDEVMODE) ALLOC(dwNeeded);     // Speicher für den Puffer besorgen
                                                 // konnte Speicher besorgt werden
      if(pDevMode==NULL) throw ERROR_ALLOCDEVMODE;
                                                 // Ok, pDevMode mit den Daten füllen
      if(DocumentProperties(NULL,NULL,szPrinterName,pDevMode,NULL,DM_OUT_BUFFER)!=IDOK) 
         throw ERROR_DOCPROPDEVMODE;

      return dwNeeded;                           // Größe von pDevMode zurückgeben !
   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);

      if(pDevMode)                               // gibt es noch eine DevMode-Struktur ?
      {
         FREE(pDevMode);
         pDevMode = NULL;
      }
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}

/*******************************************************************************
                              UpdatePrinterStatus

private Function: fülle alle Statusinformationen der Dialogbox mit den
                  Einstellungen des übergebenen Druckers (in PRINTER_INFO_2)
 
return          : true = Ok , false = Fehler
*******************************************************************************/

void UpdatePrinterStatus(HWND hDlg,PRINTER_INFO_2 * pInfo)
{
   char szStatus[256];
   char szResource[128];
   int  i;

   BEGIN("UpdatePrinterStatus");

   try
   {
      if(pInfo==NULL) throw ERROR_PINFO;         // Druckerinformationen vorhanden ?
                                                 // handelt es sich um den Standarddrucker ?
      if(lstrcmp(pInfo->pPrinterName,g_szStdPrinter)==NULL)
      {
         if(!LoadString(g_hInstance,IDS_DEFAULTPRINTER,szStatus,128))
         {
            szStatus[0]=0;
         }
      }
      else szStatus[0]=0;

                                                 // suche den passenden Resourcestring für den Druckerstatus
      for(i=0;i < MAXPS2RSID - 1;i++)
      {
         if(pInfo->Status == g_Status2Id[i].Status) break;
      }
                                                 // den Statustext aus der Resource laden und anhängen
      if(LoadString(g_hInstance,g_Status2Id[i].ID,szResource,128))
         lstrcat(szStatus,szResource);
                                                 // Dialogfeld Controls füllen
      SetDlgItemText(hDlg,IDC_STATUS ,szStatus);
      SetDlgItemText(hDlg,IDC_TYP    ,pInfo->pDriverName);
      SetDlgItemText(hDlg,IDC_ORT    ,pInfo->pPortName);
      SetDlgItemText(hDlg,IDC_COMMENT,pInfo->pComment);
   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}


/*******************************************************************************
                              UpdatePageOrientation

private Function: Stellt die aktuelle Seitenausrichtung in der Dialogbox ein
 
return          : true = Ok , false = Fehler
*******************************************************************************/

void UpdatePageOrientation(HWND hDlg,PRINTER_INFO_2 * pInfo)
{
   BEGIN("UpdatePageOrientation");
                                                 // besorge die Windowhandles der Controls für die Orientierung
   HWND     wndPortrait  = GetDlgItem(hDlg,IDC_PORTRAIT);
   HWND     wndLandscape = GetDlgItem(hDlg,IDC_LANDSCAPE);
   HWND     wndIcon      = GetDlgItem(hDlg,IDC_ORIENTICON);

   try
   {                                             // alle Übergabeparameter überprüfen
      if(pInfo==NULL)           throw ERROR_PINFO;
      if(pInfo->pDevMode==NULL) throw ERROR_PDEVMODE;
                                                 // unterstützt der Drucker Orientierungen
      if(pInfo->pDevMode->dmFields&DM_ORIENTATION)
      {
         if(!IsWindowEnabled(wndPortrait ))
         {
            REPORT("enable orientation radios");
            EnableWindow(wndPortrait ,TRUE);
            EnableWindow(wndLandscape,TRUE);
         }

         switch(pInfo->pDevMode->dmOrientation)
         {
            case DMORIENT_PORTRAIT:
                 SendMessage(wndPortrait  ,BM_SETCHECK ,1,0);
                 SendMessage(wndLandscape ,BM_SETCHECK ,0,0);
                 SendMessage(wndIcon      ,STM_SETICON,(WPARAM) g_hHochIcon,NULL);
                 break;

            case DMORIENT_LANDSCAPE:
                 SendMessage(wndPortrait  ,BM_SETCHECK ,0,0);
                 SendMessage(wndLandscape ,BM_SETCHECK ,1,0);
                 SendMessage(wndIcon      ,STM_SETICON,(WPARAM) g_hQuerIcon,NULL);
                 break;

            default: throw ERROR_DMORINVALID;
         }
      }
      else                                       // Drucker unterstützt keine Orientierungen
      {
         REPORT("disable orientation radios");
         SendMessage(wndPortrait  ,BM_SETCHECK ,1,0);
         SendMessage(wndLandscape ,BM_SETCHECK ,0,0);
         SendMessage(wndIcon      ,STM_SETICON ,(WPARAM) g_hHochIcon,NULL);
         EnableWindow(wndPortrait ,FALSE);
         EnableWindow(wndLandscape,FALSE);
      }
   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}

/**************************************+****************************************
                               SetPageOrientation

private Function: Übernimmt die Seitenausrichtung aus der Dialogbox und
                  trägt diese in der pDevMode - Daten ein.
                  wird nur von OnSelectPrinter aufgerufen
*******************************************************************************/

void SetPageOrientation(HWND hDlg)
{
   BEGIN("SetPageOrientation");

   HWND     wndPortrait  = GetDlgItem(hDlg,IDC_PORTRAIT);
   HWND     wndLandscape = GetDlgItem(hDlg,IDC_LANDSCAPE);
   HWND     wndIcon      = GetDlgItem(hDlg,IDC_ORIENTICON);

                                                // nur übernehmen wenn Drucker dieses auch unterstützt !
   if(g_pActualInfo->pDevMode->dmFields&DM_ORIENTATION)
   {
      if(!IsWindowEnabled(wndPortrait ))
      {
         REPORT("enable orientation radios");
         EnableWindow(wndPortrait ,TRUE);
         EnableWindow(wndLandscape,TRUE);
      }

      if(SendDlgItemMessage(hDlg,IDC_PORTRAIT,BM_GETCHECK,0,0)&BST_CHECKED)
      {
         g_pActualInfo->pDevMode->dmOrientation=DMORIENT_PORTRAIT;
      }
      else
      {
         g_pActualInfo->pDevMode->dmOrientation=DMORIENT_LANDSCAPE;
      }
   }
   else                                          // Drucker unterstützt keine Orientierungen
   {
      REPORT("disable orientation radios");
      SendMessage(wndPortrait  ,BM_SETCHECK ,1,0);
      SendMessage(wndLandscape ,BM_SETCHECK ,0,0);
      SendMessage(wndIcon      ,STM_SETICON ,(WPARAM) g_hHochIcon,NULL);
      EnableWindow(wndPortrait ,FALSE);
      EnableWindow(wndLandscape,FALSE);
   }
}


/*******************************************************************************
                                GetPageParameters

private static Function: besorge die Papiergröße,Ränder,Druckbereich etc.,
                         für den Drucken in PRINTER_INFO_2 *
                         das Ergebnis in PRINTERPARAMETER zurückgegeben
 
*******************************************************************************/

void GetPageParameters(PRINTER_INFO_2 * pInfo,PRINTERPARAMETER& rP)
{
   POINT  ptScale;
   RECT   rcMargin;
   HDC    hic;

   BEGIN("GetPageParameters");

   try
   {
      if(pInfo==NULL) throw ERROR_PINFO;         // teste alle benötigten Übergabeparameter

      if(pInfo->pPrinterName==NULL) throw ERROR_PPRTNAME;
      if(pInfo->pDevMode==NULL)     throw ERROR_PDEVMODE;
                                                 // eine Informationcontext anlegen, um die Druckerdaten zu besorgen
      hic = CreateIC("WINSPOOL",pInfo->pPrinterName,NULL,pInfo->pDevMode);
      if(hic==NULL)   throw ERROR_CREATEIC;      // Fehler beim Anlegen des Informationcontext
                                                 // alle Druckerdaten besorgen
      rP.ptOffset.x    = GetDeviceCaps(hic,PHYSICALOFFSETX);
      rP.ptOffset.y    = GetDeviceCaps(hic,PHYSICALOFFSETY);
      rP.siPageSize.cx = GetDeviceCaps(hic,PHYSICALWIDTH);
      rP.siPageSize.cy = GetDeviceCaps(hic,PHYSICALHEIGHT);
      rP.siDrawArea.cx = GetDeviceCaps(hic,HORZRES);
      rP.siDrawArea.cy = GetDeviceCaps(hic,VERTRES);
      ptScale.x        = GetDeviceCaps(hic,LOGPIXELSX);
      ptScale.y        = GetDeviceCaps(hic,LOGPIXELSY);
                                                 // der Informationscontext wird ab hier nicht mehr benötigt !
      if(!DeleteDC(hic)) throw ERROR_DELETEDC;   // Fehler beim Löschen des Informationscontext
      if(ptScale.x==0)   throw ERROR_PTSCALE;    // zwei Tests,um divide by zero zu unterbinden
      if(ptScale.y==0)   throw ERROR_PTSCALE;
                                                 // berechne die Skalierungsfaktoren [Deviceunits] <-> [Low Metrics]
      rP.dScaleX2Lom   = 254.0 / ptScale.x;
      rP.dScaleX2Dev   = ptScale.x / 254.0;
      rP.dScaleY2Lom   = 254.0 / ptScale.y;
      rP.dScaleY2Dev   = ptScale.y / 254.0;
                                                 // Verkleinerung des Druckbereiches (Mini-Bug-Fix)
      rP.ptOffset.x   +=g_dwExtraSpace;
      rP.ptOffset.y   +=g_dwExtraSpace;
      rP.siDrawArea.cx-=g_dwExtraSpace << 1;
      rP.siDrawArea.cy-=g_dwExtraSpace << 1;
                                                 // berechne die minimal zulässigen Ränder in [Deviceunits]
      rcMargin.left    = rP.ptOffset.x;
      rcMargin.right   = rP.siPageSize.cx - (rP.siDrawArea.cx + rP.ptOffset.x);
      rcMargin.top     = rP.ptOffset.y;
      rcMargin.bottom  = rP.siPageSize.cy - (rP.siDrawArea.cy + rP.ptOffset.y);
                                                 // transformiere die minimal zulässigen Ränder nach [Low Metrics]
      rP.rcMinMargin.left   = Rounding(rP.dScaleX2Lom * rcMargin.left);
      rP.rcMinMargin.top    = Rounding(rP.dScaleY2Lom * rcMargin.top);
      rP.rcMinMargin.right  = Rounding(rP.dScaleX2Lom * rcMargin.right);
      rP.rcMinMargin.bottom = Rounding(rP.dScaleY2Lom * rcMargin.bottom);
                                                 // Ausgeben aller Daten
      REPORT("Offset    x, y %d,%d",rP.ptOffset.x       ,rP.ptOffset.y);
      REPORT("PageSize cx,cy %d,%d",rP.siPageSize.cx    ,rP.siPageSize.cy);
      REPORT("DrawArea cx,cy %d,%d",rP.siDrawArea.cx    ,rP.siDrawArea.cy);
      REPORT("Scale     x, y %d,%d",ptScale.x           ,ptScale.y);
      REPORT("MinMargin l, t %d,%d",rP.rcMinMargin.left ,rP.rcMinMargin.top);
      REPORT("MinMargin r, b %d,%d",rP.rcMinMargin.right,rP.rcMinMargin.bottom);

   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}

/*******************************************************************************
                                   ChangeSetup

private Function: die Druckereinstellungen des aktuellen Druckers über die
                  Treiberdialogbox ändern etc.

*******************************************************************************/

void ChangePrinterSetup(HWND hDlg,PRINTER_INFO_2 * pInfo)
{
   HANDLE    hPrinter    = NULL;
   PDEVMODE  pDevModeOut = NULL;
   bool      update      = false;
   DWORD     size;
   char *    szName;

   BEGIN("ChangePrinterSetup");

   try
   {
      if(pInfo==NULL)  throw ERROR_PINFO;        // Übergabeparameter gültig ?
      szName = pInfo->pPrinterName;              // Zeiger auf den Druckernamen bilden
      if(szName==NULL) throw ERROR_PPRTNAME;     // Druckernamenzeiger gültig ?
      if(pInfo->pDevMode==NULL)                  // gibt es Druckereinstellungen in den Druckerdaten ? 
         throw ERROR_PDEVMODE;
                                                 // berechne die benötigte Puffergröße für die Druckereinstellungen
      size = pInfo->pDevMode->dmSize + pInfo->pDevMode->dmDriverExtra;
      if(size==0) throw ERROR_DWNEEDED;          // Puffergröße gültig ?
      pDevModeOut = (PDEVMODE) ALLOC(size);      // Speicher für den Puffer besorgen
      if(pDevModeOut==NULL)                      // konnte Speicher besorgt werden ?
         throw ERROR_ALLOCDEVMODE;
                                                 // Ok, dann den Drucker öffnen
      if(!OpenPrinter(szName,&hPrinter,NULL)) 
         throw ERROR_OPENPRTFAILED;

      switch(DocumentProperties(hDlg,            // Ok, dann die Druckersetup Dialogbox aufrufen
                                hPrinter,
                                szName,
                                pDevModeOut,
                                pInfo->pDevMode,
                                DM_IN_BUFFER|DM_OUT_BUFFER|DM_IN_PROMPT))
      {
         case IDOK:                              // Sollen neue Daten übernommen werden ?
              REPORT("DocumentProperties(): return ok, copy DevModeData");
              CopyMemory(pInfo->pDevMode,pDevModeOut,size);
              update = true;                     // Dialogbox muß upgedatet werden
              break;
      
         case IDCANCEL:                          // es sollen keine neuen Daten übernommen werden !
              REPORT("DocumentProperties(): return cancel");
              break;

         default: throw ERROR_DOCPROPFAILED;     // Fehler !
      }

      FREE(pDevModeOut);                         // Puffer freigeben !

      if(!ClosePrinter(hPrinter))                // Drucker-Handle schließen
         REPORT("ClosePrinter() failed !");

      if(update)
      {
         UpdatePageOrientation(hDlg,pInfo);
         GetPageParameters(g_pActualInfo,g_ActualParameters);
         SetCopies(hDlg,g_pActualInfo);
         CheckAndUpdateMargins(hDlg,g_ActualParameters);
      }
   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);
      if(pDevModeOut) FREE(pDevModeOut);         // wurde der Puffer schon erzeugt, dann diesen freigeben
      if(hPrinter)                               // wurde der Drucker schon geöffnet, dann diesen schließen
      {
         if(!ClosePrinter(hPrinter)) 
            REPORT("ClosePrinter() failed !");
      }
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}

/*******************************************************************************
                                 UpdateCopies

private Function: Überprüft, ob der Drucker Kopien unterstützt und übernimmt
                  voreigestellte Daten etc. und initialisiert die Controlls

*******************************************************************************/

void UpdateCopies(HWND hDlg,PRINTER_INFO_2 * pInfo,PRINTERPARAMETER& Params)
{
   DWORD max;
   BOOL  Translated;
   DWORD value;

   BEGIN("UpdateCopies");

   try
   {                                             // alle verwendeten Parameter überprüfen
      if(pInfo==NULL)              throw ERROR_PINFO;
      if(pInfo->pDriverName==NULL) throw ERROR_PPRTNAME;
      if(pInfo->pDevMode==NULL)    throw ERROR_PDEVMODE;
      if(pInfo->pPortName==NULL)   throw ERROR_PPORTNAME;      
                                                 // unterstütz dieser Drucker Kopien ?
                                                 // pPortName muß leider übergeben werden denn der Druckertreiber vom
                                                 // HP3100 stürzt ohne ihn ab !!!! (nur der !)
      max = DeviceCapabilities(pInfo->pPrinterName,
                               pInfo->pPortName,
                               DC_COPIES,
                               NULL,
                               NULL);

      if(max==-1) throw ERROR_DEVCAPFAILED;      // Funktionsaufruf erfolgreich ?
                                                 // max == 0,1: keine Kopien werden unterstützt
                                                 // sonst ist max die Maximalanzahl der Kopien
                                                 // Flag in DevMode nicht gesetzt -> unterstütz auch keine Kopien
      if(!(pInfo->pDevMode->dmFields&DM_COPIES)) max = 1;

      if((max==0)||(max==1))                     // dieser Drucker unterstützt keine Kopien !
      {
         SetDlgItemInt(hDlg,IDC_COPIES,1,FALSE);
         EnableWindow(GetDlgItem(hDlg,IDC_COPIES),FALSE);
         REPORT("printer can´t print multiple copies");
         Params.nMaxCopies = 1;
      }
      else                                       // dieser Drucker unterstützt Kopien !
      {
         if(max > 65535) max = 65535;            // mehr sind nicht möglich !

         value = GetDlgItemInt(hDlg,IDC_COPIES,&Translated,FALSE);

         if((value < 1)||(Translated==FALSE)) value = 1;
         else if(value > max)                 value = max;

         SetDlgItemInt(hDlg,IDC_COPIES,value,FALSE);
         EnableWindow(GetDlgItem(hDlg,IDC_COPIES),TRUE);
                                                 // aktuelle Kopien eintragen
         pInfo->pDevMode->dmCopies = (short) value; 

         REPORT("printer can print a maximum of %d copies",max);

         Params.nMaxCopies = max;
      }
   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}

/*******************************************************************************
                                 SetCopies

private Function: wenn der Drucker mehrere Kopien unterstützt, übertrage
                  den aktuellen Wert in das Dialogcontrol

*******************************************************************************/

void SetCopies(HWND hDlg,PRINTER_INFO_2 * pInfo)
{
   BEGIN("SetCopies");

   try
   {                                             // alle verwendeten Parameter überprüfen
      if(pInfo==NULL)              throw ERROR_PINFO;
      if(pInfo->pDevMode==NULL)    throw ERROR_PDEVMODE;

      if(pInfo->pDevMode->dmFields&DM_COPIES)    // unterstütz dieser Drucker Kopien ?
      {                                          // Ja, dann übernehme die aktuelle einstellung in die Dialogbox
         SetDlgItemInt(hDlg,IDC_COPIES,pInfo->pDevMode->dmCopies,FALSE);
         REPORT("copy dmCopies to dialogcontrol");
      }
   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}

/*******************************************************************************
                                CheckAndUpdateMargins

private Function: Überprüfe die Ränder auf Gültigkeit und trage sie erneut
                  in die Dialogfeldcontrols ein wenn hDlg != NULL

return: true   = eine Rändereinstellung mußte geändert werden
        false  = die Rändereinstellungen sind komplett gültig

*******************************************************************************/

bool CheckAndUpdateMargins(HWND hDlg,PRINTERPARAMETER& rP)
{
#define MINPRT 101                               // minimal zulässiger bedruckbarer Bereich

   int  w;
   bool bRetValue = false;                       // noch wurde keine Rändereinstellung korrigiert

   BEGIN("CheckAndUpdateMargins");

   try
   {                                             // überprüfe die Einstellung der minimalen Ränder
      if(rP.rcMargin.left   < rP.rcMinMargin.left  ) rP.rcMargin.left   = rP.rcMinMargin.left  ,bRetValue=true;
      if(rP.rcMargin.top    < rP.rcMinMargin.top   ) rP.rcMargin.top    = rP.rcMinMargin.top   ,bRetValue=true;
      if(rP.rcMargin.right  < rP.rcMinMargin.right ) rP.rcMargin.right  = rP.rcMinMargin.right ,bRetValue=true;
      if(rP.rcMargin.bottom < rP.rcMinMargin.bottom) rP.rcMargin.bottom = rP.rcMinMargin.bottom,bRetValue=true;

                                                 // überprüfe die Einstellung der maximalen Ränder
                                                 // der bedruckbare Bereich sollte 1cm nicht unterschreiten
      w = Rounding(rP.dScaleX2Lom * rP.siPageSize.cx)-rP.rcMargin.left-rP.rcMargin.right;

      if(w < MINPRT)                             // die Breite des bedruckbaren Bereiches unterschreitet 1cm
      {                                          // versuche das Problem zu korrigieren indem zuerst die größe Rändereinstellung
         bRetValue = true;                       // so neu eingestellt wird, daß 1cm bedruckbarer Bereich übrig bleibt

         if(rP.rcMargin.left > rP.rcMargin.right)// der linke Rand ist der größere
         {
            rP.rcMargin.left -= MINPRT - w;
                                                 // der linke Rand darf aber nicht die minimal zulässige Einstellung unterschreiten
            if(rP.rcMargin.left < rP.rcMinMargin.left)
            {                                    
               w = rP.rcMinMargin.left - rP.rcMargin.left;
               rP.rcMargin.left += w;            // es muß der linke Rand vergrößert werden,
               rP.rcMargin.right-= w;            // daraufhin den rechten Rand verkleinern !
               if(rP.rcMargin.right < rP.rcMinMargin.right)
               {                                 // aber auch der rechte Rand darf nicht die minimal zulässige Einstellung unterschreiten
                  rP.rcMargin.right = rP.rcMinMargin.right;
                                                 // so kann es durchaus passieren, das der beruckbare Bereich kleiner als 1cm wird !
                                                 // dies läßt sich aber nicht verhindern
               }
            }
         }
         else                                    // der rechte Rand ist der größere
         {
            rP.rcMargin.right-= MINPRT - w;
                                                 // der rechte Rand darf aber nicht die minimal zulässige Einstellung unterschreiten
            if(rP.rcMargin.right < rP.rcMinMargin.right)
            {
               w = rP.rcMinMargin.right - rP.rcMargin.right;
               rP.rcMargin.right += w;           // es muß der rechte Rand vergrößert werden,
               rP.rcMargin.left  -= w;           // daraufhin den linken Rand verkleinern !
               if(rP.rcMargin.left < rP.rcMinMargin.left)
               {                                 // aber auch der linke Rand darf nicht die minimal zulässige Einstellung unterschreiten
                  rP.rcMargin.left = rP.rcMinMargin.left;
                                                 // so kann es durchaus passieren, das der beruckbare Bereich kleiner als 1cm wird !
                                                 // dies läßt sich aber nicht verhindern
               }
            }
         }      
      }

                                                 // überprüfe die Einstellung der maximalen Ränder
                                                 // der bedruckbare Bereich sollte 1cm nicht unterschreiten
      w = Rounding(rP.dScaleY2Lom * rP.siPageSize.cy)-rP.rcMargin.top-rP.rcMargin.bottom;

      if(w < MINPRT)                             // die Höhe des bedruckbaren Bereiches unterschreitet 1cm
      {                                          // versuche das Problem zu korrigieren indem zuerst die größe Rändereinstellung
         bRetValue = true;                       // so neu eingestellt wird, daß 1cm bedruckbarer Bereich übrig bleibt
         if(rP.rcMargin.top > rP.rcMargin.bottom)// der obere Rand ist der größere
         {
            rP.rcMargin.top -= MINPRT - w;
                                                 // der obere Rand darf aber nicht die minimal zulässige Einstellung unterschreiten
            if(rP.rcMargin.top < rP.rcMinMargin.top)
            {                                    
               w = rP.rcMinMargin.top - rP.rcMargin.top;
               rP.rcMargin.top   += w;           // es muß der obere Rand vergrößert werden,
               rP.rcMargin.bottom-= w;           // daraufhin den unteren Rand verkleinern !
               if(rP.rcMargin.bottom < rP.rcMinMargin.bottom)
               {                                 // aber auch der untere Rand darf nicht die minimal zulässige Einstellung unterschreiten
                  rP.rcMargin.bottom = rP.rcMinMargin.bottom;
                                                 // so kann es durchaus passieren, das der beruckbare Bereich kleiner als 1cm wird !
                                                 // dies läßt sich aber nicht verhindern
               }
            }
         }
         else                                    // der untere Rand ist der größere
         {
            rP.rcMargin.bottom-= MINPRT - w;
                                                 // der untere Rand darf aber nicht die minimal zulässige Einstellung unterschreiten
            if(rP.rcMargin.bottom < rP.rcMinMargin.bottom)
            {
               w = rP.rcMinMargin.bottom - rP.rcMargin.bottom;
               rP.rcMargin.bottom += w;          // es muß der untere Rand vergrößert werden,
               rP.rcMargin.top    -= w;          // daraufhin den oberen Rand verkleinern !
               if(rP.rcMargin.top < rP.rcMinMargin.top)
               {                                 // aber auch der linke Rand darf nicht die minimal zulässige Einstellung unterschreiten
                  rP.rcMargin.top = rP.rcMinMargin.top;
                                                 // so kann es durchaus passieren, das der beruckbare Bereich kleiner als 1cm wird !
                                                 // dies läßt sich aber nicht verhindern
               }
            }
         }      
      }

      if(hDlg)                                   // diese Funktion wird auch von SetAllData und SetSpecialData verwenden !
      {
         PutLowMetrics(hDlg,IDC_LEFT  ,rP.rcMargin.left  );
         PutLowMetrics(hDlg,IDC_TOP   ,rP.rcMargin.top   );
         PutLowMetrics(hDlg,IDC_RIGHT ,rP.rcMargin.right );
         PutLowMetrics(hDlg,IDC_BOTTOM,rP.rcMargin.bottom);

         bool bMono = true;

         if( g_pActualInfo->pDevMode->dmFields&DM_COLOR)
            if(g_pActualInfo->pDevMode->dmColor 
               == DMCOLOR_COLOR) bMono = false;

         UpdatePreView(rP,bMono);
      }

      return bRetValue;
   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }

#undef MINPRT
}


/**************************************+****************************************
                                 UpdatePreView

private Function: den PreView mit neuen Daten zur Darstellung versorgen

*******************************************************************************/

void UpdatePreView(PRINTERPARAMETER& rP,bool bMono)
{
   PREVIEWPARAMETER Pre;

   Pre.bValid          = true;
   Pre.siPageSize.cx   = Rounding(rP.dScaleX2Lom * rP.siPageSize.cx);
   Pre.siPageSize.cy   = Rounding(rP.dScaleY2Lom * rP.siPageSize.cy);
   Pre.rcMargin.left   = rP.rcMargin.left;
   Pre.rcMargin.top    = rP.rcMargin.top;
   Pre.rcMargin.right  = rP.rcMargin.right;
   Pre.rcMargin.bottom = rP.rcMargin.bottom;

   Pre.bMono = bMono;

   SetPreViewWndPageParameter(&Pre);
}


/**************************************+****************************************
                             CheckAndSetPageOptions

private Function: überprüfe ob der Drucker die Seitengröße und die Ausrichtung
                  unterstütz und setze diese

return:  true  = wird so nicht unterstützt
         false = wird so unterstützt

*******************************************************************************/

bool CheckAndSetPageOptions(PRINTER_INFO_2 * pInfo,short dmPaperSize,short dmOrientation)
{
   bool   bRetValue = false;                     // noch wurde keine Papiereinstellung korrigiert
   DWORD  dwNeeded;
   WORD * pPapers;

   BEGIN("CheckAndSetPageOptions");

   try
   {                                             // alle verwendeten Parameter überprüfen
      if(pInfo==NULL)              throw ERROR_PINFO;
      if(pInfo->pDriverName==NULL) throw ERROR_PPRTNAME;
      if(pInfo->pDevMode==NULL)    throw ERROR_PDEVMODE;
      if(pInfo->pPortName==NULL)   throw ERROR_PPORTNAME;

                                                 // unterstützt dieser Drucker Orientierungen ?
      if(pInfo->pDevMode->dmFields&DM_ORIENTATION)
      {                                          // Achtung gehe davon aus wenn DM_ORIENTATION vorhanden LANDSCAPE erlaubt !
                                                 // übernehme die Orientierung wenn erlaubt
         pInfo->pDevMode->dmOrientation = dmOrientation;
      }
      else
      {
         REPORT("printer do not support orientations");
         bRetValue = true;                       // Drucker unterstützt keine Orientierungen !
      }

      if(dmPaperSize)                            // muß != 0 sein da kein Benutzerformat unterstützt wird !
      {
                                                 // überprüfe ob das Papierformat unterstützt wird
         dwNeeded = DeviceCapabilities(pInfo->pPrinterName,
                                       pInfo->pPortName,
                                       DC_PAPERS,
                                       NULL,
                                       NULL);

         if(dwNeeded==0)throw ERROR_DEVCAPFAILED;// gibt es Papiere ?
                                                 // besorge den Puffer für die unterstützten Papierformate
         pPapers = (WORD*) ALLOC(dwNeeded * sizeof(WORD));
         if(pPapers==NULL) throw ERROR_ALLOCPPAPERS;
                                                 // besorge alle unterstützten Papierformate
         DeviceCapabilities(pInfo->pPrinterName,
                            pInfo->pPortName,
                            DC_PAPERS,
                            (char*)pPapers,
                            NULL);
                                                 // suche das zu setzende Papierformat in dieser Liste
         for(DWORD i=0;i<dwNeeded;i++)
         {
            if(dmPaperSize==pPapers[i]) break;
         }

         FREE(pPapers);                          // Puffer für Papierformate wieder freigeben

         if(i!=dwNeeded)                         // wurde diese Papierformat gefunden ?
         {                                       // Ja, dann einstellen
            pInfo->pDevMode->dmFields   |= DM_PAPERSIZE;
            pInfo->pDevMode->dmPaperSize = dmPaperSize;
         }
         else
         {
            REPORT("paperformat not supported");
            bRetValue = true;
         }
      }
      else
      {
         REPORT("userpaperformat not supported");
         bRetValue = true;                       // Benutzerpapierformate werden hier nicht unterstützt
      }

      return bRetValue;
   }
   catch(int error)
   {
      TreatException(error);
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}






























/**************************************+****************************************
                                  FindPrinter

private Function: suche ob dieser Drucker auf dem Rechner installiert ist

return:    true == Ja  , dieser Drucker ist installiert
return    false == Nein, dieser Drucker ist nicht installiert

*******************************************************************************/

bool FindInstalledPrinters(const char * szPrinterName)
{
   DWORD            i;
   DWORD            dwNeeded;
   DWORD            dwPrinters;
   PRINTER_INFO_5 * pPrinterInfo = NULL;

   BEGIN("FindInstalledPrinters");

   try
   {
      if(EnumPrinters(PRINTER_ENUM_LOCAL,        // besorge die benötigte Puffergröße für alle vorhandenen Drucker
                      NULL,
                      5,
                      NULL,
                      0,
                      &dwNeeded,
                      &dwPrinters)) throw ERROR_ENUMNOPRINTER;
                                                 // Puffergröße richtig besorgt
      if(GetLastError()!=ERROR_INSUFFICIENT_BUFFER) 
         throw ERROR_ENUMPRTFAILED;

      if(dwNeeded==NULL) throw ERROR_DWNEEDED;   // Puffergröße Ok ?
                                                 // Puffer anlegen
      pPrinterInfo = (PRINTER_INFO_5 *) ALLOC(dwNeeded);
                                                 // konnte Puffer angelegt werden ?
      if(pPrinterInfo==NULL) throw ERROR_PPRTINFO;

      if(!EnumPrinters(PRINTER_ENUM_LOCAL,       // Ok, Puffer mit allen localen Druckern füllen
                       NULL,
                       5,
                       (BYTE*) pPrinterInfo,
                       dwNeeded,
                       &dwNeeded,
                       &dwPrinters)) throw ERROR_ENUMPRTGET;

      for (i=0;i<dwPrinters;i++)                 // und Drucker suchen
      {
         if(lstrcmp(pPrinterInfo[i].pPrinterName,szPrinterName)==NULL) break;
      }      

      FREE(pPrinterInfo);                        // Puffer freigeben

      if(i==dwPrinters) return false;            // Drucker wurde nicht gefunden
      else return true;                          // Ok, Drucker wurde gefunden
   }
   catch(int error)                              // hier werden alle Fehler abgefangen !
   {
      TreatException(error);
      if(pPrinterInfo) FREE(pPrinterInfo);       // Puffer freigeben wenn er vorhanden ist
      throw ERROR_PROCEED;                       // Ausnahme weiterleiten
   }
}




/**************************************+****************************************
                                GetPrinterOffset

private Function: 

*******************************************************************************/

int GetPrinterOffset(ETSPRTD_PARAMETER * pParams)
{
   BEGIN("GetPrinterOffset");

   try
   {
      if(pParams->dwSize!=sizeof(POINT)) 
         throw ERROR_BUFFER;
      if(IsBadWritePtr(pParams->pPrinterData,sizeof(POINT)))
         throw ERROR_PBUFFER;

      POINT p;

      if(pParams->dwFlags&ETSPRTD_LOWMETRICS)    // Druckeroffset in LOWMETRICS besorgen
      {
         p.x = Rounding(g_SavedParameters.dScaleX2Lom * g_SavedParameters.ptOffset.x);
         p.y = Rounding(g_SavedParameters.dScaleY2Lom * g_SavedParameters.ptOffset.y);
      }
      else
      {
         p = g_SavedParameters.ptOffset;
      }

      CopyMemory(pParams->pPrinterData,&p,sizeof(POINT));

      return 0;
   }
   catch(int error)
   {
      TreatException(error);
      throw ERROR_PROCEED;
   }
}



// der Aufbau der Daten welche diese Funktion zurückgibt

// DWORD qwSignum:            "PRTD" als Erkennung des richtigen Datenblockes
// DWORD dwVersion:           100 für 1.00
// der Druckernamen mit abschliessender 0
// die DevMode-Daten des Druckers
// die PRINTERPARAMETER


/**************************************+****************************************
                                  GetAllData

private Function: 

*******************************************************************************/

int GetAllData      (ETSPRTD_PARAMETER * pParams)
{
   BEGIN("GetAllData");

   try
   {
      if(g_pSavedInfo == NULL) throw ERROR_NOPRINTER;
                                                 // bestimme die größe des Druckernamens mit der Abschlußnull
      DWORD   dwNameSize = lstrlen(g_pSavedInfo->pPrinterName) + 1;
                                                 // bestimme die größe der DevMode-Daten
      DWORD   dwDevSize  = g_pSavedInfo->pDevMode->dmSize+g_pSavedInfo->pDevMode->dmDriverExtra;
                                                 // bestimme die größe alle Daten
      DWORD   dwSize     = 8 + dwNameSize + dwDevSize + sizeof(PRINTERPARAMETER);


      if(pParams->pPrinterData==NULL)            // soll nur die Größe des benötigten Puffers berechnet werden ?
      {                                          // darf ich hier schreiben ?
         if(IsBadWritePtr(pParams,sizeof(ETSPRTD_PARAMETER))) 
            throw ERROR_PPARAMETER;

         pParams->dwSize = dwSize;               // benötigte Puffergröße eintragen
      }
      else
      {           
         if(pParams->dwSize!=dwSize) 
            throw ERROR_BUFFER;
         if(IsBadWritePtr(pParams->pPrinterData,dwSize))
            throw ERROR_PBUFFER;
                                                 // alle Daten in den Puffer eintragen
         char * pPut = (char *) pParams->pPrinterData;

         *((DWORD*)pPut) = 0x44545250;           // Kennung eintragen
         pPut+=4;                                // ein DWORD weiter
         *((DWORD*)pPut) = 100;                  // Version eintragen
         pPut+=4;                                // ein DWORD weiter , Druckernamen eintragen
         lstrcpy(pPut,g_pSavedInfo->pPrinterName);
         pPut+=dwNameSize;                       // um Stringlänge + Abschlußnull weiter
                                                 // die DevMode-Daten eintragen
         CopyMemory(pPut,g_pSavedInfo->pDevMode,dwDevSize);
         pPut+=dwDevSize;                        // um größe der DevMode-Daten weiter
                                                 // zum Abschluß die PRINTERPARAMETER eintragen
         CopyMemory(pPut,&g_SavedParameters,sizeof(PRINTERPARAMETER));
      }
      return 0;
   }
   catch(int error)
   {
      TreatException(error);
      throw ERROR_PROCEED;
   }
}

/**************************************+****************************************
                                  SetAllData

private Function: 

*******************************************************************************/

int SetAllData      (ETSPRTD_PARAMETER * pParams)
{
   BEGIN("SetAllData");

   try
   {
      char             szPrinterName[512];       // (neu 6.3.2000 Win2k tut dies nicht !) Windows begrenzt diesen Namen
      PRINTER_INFO_2 * pNew;                     // Zeiger auf die Daten für den neuen Drucker
      PRINTERPARAMETER NewParams;                // die PRINTERPARAMETER für den neuen Drucker
      DWORD            dwCompareSize = 0;        // für Größenvergleich
      DEVMODE *        pDevMode;                 // Zeiger auf die DevMode-Daten in den Übergabeparametern
      DWORD            dwNameSize;               // größe des Druckernamens
      DWORD            dwDevSize;                // größe der DevMode-Daten

                                                 // vergleiche die größe mit den minimal möglichen
      if(pParams->dwSize < 8 + 2 + sizeof(DEVMODE))
         throw ERROR_BUFFER;

      if(IsBadReadPtr(pParams->pPrinterData,pParams->dwSize))
         throw ERROR_PBUFFER;
                                                 // Zeiger auf die Daten besorgen
      char * pPut = (char *) pParams->pPrinterData;
                                                 // ist die Kennung richtig ?
      if(*((DWORD*)pPut)!=0x44545250) throw ERROR_SETAD;
      pPut         += 4;                         // ein DWORD weiter
      dwCompareSize+= 4;

      if(*((DWORD*)pPut)!=100) throw ERROR_SETAD;// ist die Version richtig ?
      pPut         += 4;                         // ein DWORD weiter
      dwCompareSize+= 4;
                                                 // Druckername richtig ?
      if(IsBadStringPtr(pPut,512)) throw ERROR_STRINGOVERFLOW;
      if(lstrlen(pPut) >=512) throw ERROR_STRINGOVERFLOW;

      lstrcpy(szPrinterName,pPut);               // OK, Druckernamen kopieren
      dwNameSize    = lstrlen(szPrinterName) + 1;
      pPut         += dwNameSize;
      dwCompareSize+= dwNameSize;

      if(dwCompareSize > pParams->dwSize) throw ERROR_PBUFFER;

      pDevMode      = (PDEVMODE) pPut;           // Zeiger auf die DevMode-Daten bilden
      dwDevSize     = pDevMode->dmSize + pDevMode->dmDriverExtra;
      pPut         += dwDevSize;
      dwCompareSize+= dwDevSize;
      
      if(dwCompareSize + sizeof(PRINTERPARAMETER) > pParams->dwSize) throw ERROR_PBUFFER;
                                                 // die Printerparameter kopieren
      CopyMemory(&NewParams,pPut,sizeof(PRINTERPARAMETER));

      if(FindInstalledPrinters(szPrinterName))   // gibt es diesen Drucker
      {                                          // OK besorge die PRINTER_INFO_2 Struktur
         GetPrinterParameter(szPrinterName,&pNew);
      }
      else                                       // Drucker wurde nicht gefunden
      {                                          // Einstellungen mit SetSpecialPrinter setzen
         ETSPRTD_PARAMETER   params;
         ETSPRTD_SPECIALDATA data;

         ZeroMemory(&params,sizeof(ETSPRTD_PARAMETER));

         params.dwFlags      = ETSPRTD_SETALLDATA|ETSPRTD_LOWMETRICS;
         params.dwSize       = sizeof(ETSPRTD_SPECIALDATA);
         params.pPrinterData = &data;

                                                 // (neu 9.3.2000) Achtung der Druckername muß auf 31 Zeichen begrenzt werden !
         if(lstrlen(szPrinterName) > 31)
         {
            szPrinterName[31] = 0;               // den Rest abschneiden
         }
                                                 // Anmerkung , der Drucker wird sowieso nicht gefunden
         lstrcpy(data.szPrinterName,szPrinterName);
         data.rcMargin      = NewParams.rcMargin;
         data.dmOrientation = pDevMode->dmOrientation;
         data.dmPaperSize   = pDevMode->dmPaperSize;
                                                 // SetSpecialPrinter ist alte Kompatibilitätsfunktion
                                                 // um auch CARA 1.0 Dateien zu Laden (ETS2DV / ETS3DV)
                                                 // ansonsten sollte diese Funktion nicht benutzt werden
         int nRetValue = SetSpecialPrinter(&params);

         return nRetValue;
      }
                                                 // kopiere die DEVMODE-Daten in die PRINTER_INFO_2 Struktur
      CopyMemory(pNew->pDevMode,pDevMode,dwDevSize);
      g_SavedParameters = NewParams;             // kopiere die PRINTERPARAMETER

      if(g_pSavedInfo) FREE(g_pSavedInfo);       // den alten Drucker freigeben
      g_pSavedInfo = pNew;                       // und den neuen eintragen

      return 0;                                  // Drucker konnte erfolgreich gesetzt werden !
   }
   catch(int error)
   {
      TreatException(error);
      throw ERROR_PROCEED;
   }
}


/**************************************+****************************************
                                SetSpecialPrinter

private Function: neuen Drucker als aktuellen einstellen

return:  0 = alles Ok, Drucker gefunden und Daten eingestellt
         2 = Drucker wurde nicht gefunden durch STD-Printer ersetzt aber
             alle Ränder und Papiereinstellungen OK
         3 = Ränder und oder Papier und oder Orientation mußten geändert werden

*******************************************************************************/

int SetSpecialPrinter(ETSPRTD_PARAMETER * pParams)
{
   PRINTER_INFO_2 * pNew = NULL;                 // Zeiger auf die Daten für den neuen Drucker
   int nRetValue         = 0;                    // wurde der Drucker gefunden, oder Seitengröße ok

   BEGIN("SetSpecialPrinter");

   try
   {
      ETSPRTD_SPECIALDATA * pData;               // Zeiger auf die Daten zum setzen des Druckers
      PRINTERPARAMETER      NewParams;           // die PRINTERPARAMETER für den neuen Drucker

                                                 // vergleiche die größe des Struktur zum setzen
      if(pParams->dwSize !=sizeof(ETSPRTD_SPECIALDATA))
         throw ERROR_BUFFER;

      if(IsBadReadPtr(pParams->pPrinterData,pParams->dwSize))
         throw ERROR_PBUFFER;
                                                 // typecast damit es einfacher ist !
      pData = (ETSPRTD_SPECIALDATA*) pParams->pPrinterData;

                                                 // gibt es diesen Drucker
      if(FindInstalledPrinters(pData->szPrinterName))
      {                                          // OK besorge die PRINTER_INFO_2 Struktur
         GetPrinterParameter(pData->szPrinterName,&pNew);
      }
      else
      {
         nRetValue = 2;                          // dieser Drucker wurde nicht gefunden
         GetDefaultPrinter(&pNew);               // besorge die PRINTER_INFO_2 Struktur vom Standarddrucker
      }
                                                 // überprüfe ob Drucker das Pagierformat und Orientierung unterstützt
                                                 // und übernehme diese Werte
      if(CheckAndSetPageOptions(pNew,pData->dmPaperSize,pData->dmOrientation))
      {
         nRetValue = 3;
      }
                                                 // besorge die Seitenparameter des Druckers
      GetPageParameters(pNew,NewParams);
                                                 // wurden die Ränder in Deviceunits übergeben ?
      if(pParams->dwFlags&ETSPRTD_DEVICEUNITS)
      {                                          // dann die Ränder nach LOWMETRIC transformieren
         pData->rcMargin.left   = Rounding(NewParams.dScaleX2Lom * pData->rcMargin.left);
         pData->rcMargin.top    = Rounding(NewParams.dScaleY2Lom * pData->rcMargin.top);
         pData->rcMargin.right  = Rounding(NewParams.dScaleX2Lom * pData->rcMargin.right);
         pData->rcMargin.bottom = Rounding(NewParams.dScaleY2Lom * pData->rcMargin.bottom);
      }
                                                 // übernehme nun die Ränder und setze die anderen Daten
      NewParams.rcMargin     = pData->rcMargin;
      NewParams.bPrintToFile = false;
      NewParams.nMaxCopies   = 1;
                                                 // überprüfe die Rändereinstellungen
      if(CheckAndUpdateMargins(NULL,NewParams))
      {
         nRetValue = 3;                          // Rändereinstellungen mußten geändert werden
      }
                                                 // Ok, Drucker kann übernommen werden
      g_SavedParameters = NewParams;             // kopiere die PRINTERPARAMETER
      if(g_pSavedInfo) FREE(g_pSavedInfo);       // den alten Drucker freigeben
      g_pSavedInfo = pNew;                       // und den neuen eintragen
      pNew         = NULL;

      return nRetValue;                          // Drucker konnte erfolgreich gesetzt werden !
   }
   catch(int error)
   {
      if(pNew) FREE(pNew);                       // bei Fehler die PRINTER_INFO_2 löschen
      TreatException(error);
      throw ERROR_PROCEED;
   }
}


/**************************************+****************************************
                                FillPrinterData

private Function: 

*******************************************************************************/

int FillPrinterData(ETSPRTD_PARAMETER * pParams)
{
   ETSPRTD_PRINTER Data;

   BEGIN("FillPrinterData");

   try
   {                                             // die Übergabeparameter überprüfen
      if(pParams->dwSize!=sizeof(ETSPRTD_PRINTER)) 
         throw ERROR_BUFFER;
      if(IsBadWritePtr(pParams->pPrinterData,sizeof(ETSPRTD_PRINTER)))
         throw ERROR_PBUFFER;

      if(pParams->dwFlags&ETSPRTD_LOWMETRICS)    // alle Einheiten nach LOWMETRIC
      {
         double scalex = g_SavedParameters.dScaleX2Lom;
         double scaley = g_SavedParameters.dScaleY2Lom;

         Data.rcMargin.left   = g_SavedParameters.rcMargin.left;
         Data.rcMargin.top    = g_SavedParameters.rcMargin.top;
         Data.rcMargin.right  = g_SavedParameters.rcMargin.right;
         Data.rcMargin.bottom = g_SavedParameters.rcMargin.bottom;
         Data.ptOffset.x      = Data.rcMargin.left - Rounding(scalex * g_SavedParameters.ptOffset.x);
         Data.ptOffset.y      = Data.rcMargin.top  - Rounding(scaley * g_SavedParameters.ptOffset.y);
         Data.siPageSize.cx   = Rounding(scalex * g_SavedParameters.siPageSize.cx);
         Data.siPageSize.cy   = Rounding(scaley * g_SavedParameters.siPageSize.cy);
      }
      else                                       // alle Einheiten nach DEVICEUNITS
      {
         double scalex = g_SavedParameters.dScaleX2Dev;
         double scaley = g_SavedParameters.dScaleY2Dev;

         Data.rcMargin.left   = Rounding(scalex * g_SavedParameters.rcMargin.left);
         Data.rcMargin.top    = Rounding(scaley * g_SavedParameters.rcMargin.top);
         Data.rcMargin.right  = Rounding(scalex * g_SavedParameters.rcMargin.right);
         Data.rcMargin.bottom = Rounding(scaley * g_SavedParameters.rcMargin.bottom);
         Data.ptOffset.x      = Data.rcMargin.left - g_SavedParameters.ptOffset.x;
         Data.ptOffset.y      = Data.rcMargin.top  - g_SavedParameters.ptOffset.y;
         Data.siPageSize.cx   = g_SavedParameters.siPageSize.cx;
         Data.siPageSize.cy   = g_SavedParameters.siPageSize.cy;
      }

      Data.siDrawArea.cx   = Data.siPageSize.cx - Data.rcMargin.left - Data.rcMargin.right;
      Data.siDrawArea.cy   = Data.siPageSize.cy - Data.rcMargin.top  - Data.rcMargin.bottom;
      Data.hDC             = NULL;               // noch kein Druckerdevicehandle

      CopyMemory(pParams->pPrinterData,&Data,sizeof(ETSPRTD_PRINTER));
      
      return 0;
   }
   catch(int error)
   {
      TreatException(error);
      throw ERROR_PROCEED;
   }
}


/**************************************+****************************************
                                 StartPrinting

private Function: 

*******************************************************************************/

int StartPrinting(ETSPRTD_PARAMETER * pParams)
{
   char szApplication[128];
   char szDocument[256];
   HDC  hdc = NULL;

   BEGIN("StartPrinting");

   try
   {
      FillPrinterData(pParams);                  // für den Job nötige Daten eintragen
                                                 // überprüfe ob ein gültiger Fensterhandle übergeben wurde
      if((pParams->dwFlags&ETSPRTD_4MFCPREVIEW)==0)
      {
         if(pParams->hParent==NULL)      throw ERROR_HWND;
         if(!IsWindow(pParams->hParent)) throw ERROR_HWND;

                                                 // drucken initialisieren erwünscht (kein Flag ETSPRTD_4MFCPREVIEW)
            if((pParams->dwFlags&ETSPRTD_QUIET)==0)
            {
                                                 // Druckennachfrage erwünscht (kein Flag ETSPRTD_QUIET)
            LoadString(g_hInstance,IDS_MBQ_PRINTING,szDocument,256);
            if(MessageBox(pParams->hParent,
                          szDocument,
                          g_pSavedInfo->pPrinterName,
                          MB_ICONQUESTION|MB_APPLMODAL|MB_YESNO)!=IDYES)
            {
               return 1;                         // der Benutzer hat das Drucken abgebrochen
            }
         }
      }
                                                 // drucken initialisieren erwünscht (kein Flag ETSPRTD_4MFCPREVIEW)
      if((pParams->dwFlags&ETSPRTD_4MFCPREVIEW)==0)
      {
                                                 // Ist ein Anwendungsname übergeben worden ?
         if(!IsBadStringPtr(pParams->szApplication,1024))
         {                                       // Achtung wenn ein Eingabestring 1024 Char´s überschreitet 
                                                 // wird kein ERROR_STRINGOVERFLOW ausgelöst
            if(lstrlen(pParams->szApplication) >127) throw ERROR_STRINGOVERFLOW;
            lstrcpy(szApplication,pParams->szApplication);
         }
         else
         {
            lstrcpy(szApplication,"ETSPRTD");    // lade den sonst zu verwendenden Anwendungsname
         }
                                                 // Ist ein Dokumentname übergeben worden ?
         if(!IsBadStringPtr(pParams->szDocument,1024))
         {
            if(lstrlen(pParams->szDocument) > 255) throw ERROR_STRINGOVERFLOW;
            lstrcpy(szDocument,pParams->szDocument);
         }
         else 
         {
            if(!LoadString(g_hInstance,IDS_DEFAULTDOC,szDocument,255))
               lstrcpy(szDocument,"unknown");    // der Default Document Name
         }
                                                 // bilde den String für DOCINFO
         wsprintf(g_szDocName,"%s:%s",szApplication,szDocument);

                                                 // jetzt die DOCINFO-Struktur anlegen
         ZeroMemory(&g_DocInfo,sizeof(DOCINFO));
         g_DocInfo.cbSize = sizeof(DOCINFO);
         g_DocInfo.lpszDocName = g_szDocName;

         if(g_SavedParameters.bPrintToFile)      // soll in eine Datei Ausgedruckt werden ?
         {                                       // besorge den Ausgabedateinamen
            if(GetPrinterOutputFile(pParams))
            {
               return 1;                         // Abgebrochen !
            }
            g_DocInfo.lpszOutput= g_szOutputFile;// der Ausgabedateiname
         }
      }                                          // endif drucken initialisieren
                                                 // besorge den Drucker-DeviceContext
      hdc = CreateDC("WINSPOOL",
                     g_pSavedInfo->pPrinterName,
                     NULL,
                     g_pSavedInfo->pDevMode);

      if(hdc==NULL) throw ERROR_CREATEIC;

                                                 // drucken initialisieren erwünscht (kein Flag ETSPRTD_4MFCPREVIEW)
      if((pParams->dwFlags&ETSPRTD_4MFCPREVIEW)==0)
      {
         if(StartDoc(hdc,&g_DocInfo)<=0) throw ERROR_STARTDOC;
         if(StartPage(hdc)<=0) throw ERROR_STARTPAGE;
      }                                          // endif drucken initialisieren

                                                 // den DC für die druckende Anwendung übergeben
      ((ETSPRTD_PRINTER*)(pParams->pPrinterData))->hDC = hdc;

      return 0;
   }
   catch(int error)
   {
      TreatException(error);
      if(hdc) DeleteDC(hdc);
      throw ERROR_PROCEED;
   }
}


/**************************************+****************************************
                              GetPrinterOutputFile

public Function: das Standarddialogfeld für Dateinameneingabe öffnen, um den
                 Benutzer die Ausgabedatei für den Druckjob eingeben zu lassen.

return         : 0 == OK -> Dateinamen + Path in g_szOutputFile
                 1 == Abbruch oder Fehler

*******************************************************************************/

int GetPrinterOutputFile(ETSPRTD_PARAMETER * pParams)
{
   OPENFILENAME ofn;
   char *       szPtr;
   char         szFilter[256];
   char         szTitle[256];
   char         szDefault[256];
   bool         test = true;
                                                 // laden den Filter für das Dialogfeld,den Titel und den Standarddateinamen
   test = LoadString(g_hInstance,IDS_OFNFILTER ,szFilter ,256)?true:false;
   test&= LoadString(g_hInstance,IDS_OFNTITLE  , szTitle ,256)?true:false;
   test&= LoadString(g_hInstance,IDS_OFNDEFAULT,szDefault,256)?true:false; 

   if(!test) return 1;                           // die Strings konnten nicht geladen werden
                                                 // ersetzte alle Tab-Zeichen (\t) durch Nullen (\0)
                                                 // guter Trick, um Filter aus Resourcen zu laden.
   szPtr=szFilter; while((*szPtr)!=0) if((*szPtr++)==9) *(szPtr-1) = 0;
   lstrcpy(g_szOutputFile,szDefault);            // den Standarddateinamen kopieren

   ofn.lStructSize       = sizeof(OPENFILENAME);
   ofn.hwndOwner         = pParams->hParent;
   ofn.hInstance         = NULL;
   ofn.lpstrFilter       = szFilter;
   ofn.lpstrCustomFilter = 0;
   ofn.nFilterIndex      = 0;
   ofn.lpstrFile         = g_szOutputFile;
   ofn.nMaxFile          = 512;
   ofn.lpstrFileTitle    = 0;
   ofn.nMaxFileTitle     = 0;
   ofn.lpstrInitialDir   = NULL;
   ofn.lpstrTitle        = szTitle;
   ofn.Flags             = OFN_EXPLORER|OFN_HIDEREADONLY|OFN_NOCHANGEDIR|OFN_OVERWRITEPROMPT;
   ofn.nFileOffset       = 0;
   ofn.nFileExtension    = 0;
   ofn.lpstrDefExt       = "prn";
   ofn.lCustData         = 0;
   ofn.lpfnHook          = NULL;
   ofn.lpTemplateName    = NULL;

   if(GetSaveFileName(&ofn)) return 0;           // Dialog ausführen, Ok eine Ausgabedatei wurde ausgewählt
   return 1;                                     // Fehler oder Abbruch !
}


/**************************************+****************************************
********************************************************************************

             Ab hier folgen alle Schnittstellen zu dieser DLL
                      
********************************************************************************
*******************************************************************************/


/**************************************+****************************************
                                 DoDataExchange

public Function: Datenaustauschfunktion mit ETSPRTD.DLL

return         : -1 == Fehler, oder kein Drucker installiert
                  0 == alles ist OK
                  1 == Benutzerabbruch
                  2 == Drucker wurde nicht gefunden nur durch ETSPRTD_SETALLDATA
                       alle Ränder und Papiereinstellungen OK
                  3 == Ränder und oder Papier und oder Orientation 
                       mußten geändert werden nur durch ETSPRTD_SETALLDATA
                      
*******************************************************************************/

extern "C"                                       // Wichtig, Namenkonvention nach C-Standard für die DEF-Datei !
{
   int DoDataExchange(ETSPRTD_PARAMETER * pParams)
   {
      int ret;

      BEGIN("DoDataExchange");

      if(g_bInDllCode)                           // Flag, zur überprüfung ob diese DLL Reentrant aufgerufen wird
      {                                          // die wird mit absicht nicht über Exception handling gelöst !
         TreatException(ERROR_REENTRANT);
         SetLastError(DLLE|ERROR_REENTRANT);
         return -1;
      }      

      g_bInDllCode = true;
      TlsSetValue(g_tlsidLastError,0);           // noch kein Fehler !

      try
      {
                                                 // überprüfe den Pointer auf die Übergabeparameter dieser Funktion
         if(IsBadReadPtr(pParams,sizeof(ETSPRTD_PARAMETER))) 
            throw ERROR_PPARAMETER;

         if(pParams->dwFlags==ETSPRTD_QUIET)     // nur Druckerüberprüfung ?
         {
            g_bQuiet = true;                     // Bitte keine Meldung ausgeben !
         }

         TestSavedPrinter();                     // überprüfe den Eingestellten Drucker oder besorge den Standarddrucker

         switch(pParams->dwFlags)                // für die Auswertung der zulässigen Flag-Angaben
         {
            case ETSPRTD_PRTOFFSET|
                 ETSPRTD_LOWMETRICS:             // neu 14.4.99 nur den Druckeroffset zurückgeben (LOWMETRICS oder DEVICEUNITS)
            case ETSPRTD_PRTOFFSET|
                 ETSPRTD_DEVICEUNITS:
                 ret = GetPrinterOffset(pParams);
                 break;

            case ETSPRTD_GETALLDATA:             // alle Einstellungen des eingestellten Druckers besorgen !
                 ret = GetAllData(pParams);      // z.B. zum Speichern im Dokument oder Registry
                 break;

            case ETSPRTD_SETALLDATA:             // alle Einstellung des eingestellten Druckers setzen
                 ret = SetAllData(pParams);      // z.B. zum Laden aus einem Dokument oder der Registry
                 break;

            case ETSPRTD_SETALLDATA|             // neu 14.4.99 Drucker über die Struktur ETSPRTD_SETDATA einstellen
                 ETSPRTD_LOWMETRICS:
            case ETSPRTD_SETALLDATA|
                 ETSPRTD_DEVICEUNITS:
                 ret = SetSpecialPrinter(pParams);
                 break;

            case ETSPRTD_LOWMETRICS:             // besorge den Druckbereich für Druckervorschau in LOWMETRIC
            case ETSPRTD_DEVICEUNITS:            // besorge den Druckbereich für Druckervorschau in DEVICEUNITS
                 ret = FillPrinterData(pParams);
                 break;

            case ETSPRTD_LOWMETRICS|             // besorge Druckbereich(LOWMETRICS ),DC und initialisiere das Drucken mit Abfrage
                 ETSPRTD_RETURNDC:
            case ETSPRTD_DEVICEUNITS|            // besorge Druckbereich(DEVICEUNITS),DC und initialisiere das Drucken mit Abfrage
                 ETSPRTD_RETURNDC:
            case ETSPRTD_LOWMETRICS|             // besorge Druckbereich(LOWMETRICS ),DC und initialisiere das Drucken ohne Abfrage
                 ETSPRTD_RETURNDC|
                 ETSPRTD_QUIET:
            case ETSPRTD_DEVICEUNITS|            // besorge Druckbereich(DEVICEUNITS),DC und initialisiere das Drucken ohne Abrfrage
                 ETSPRTD_RETURNDC|
                 ETSPRTD_QUIET:
            case ETSPRTD_LOWMETRICS|             // besorge Druckbereich(LOWMETRICS ),DC ohne Initialisierung ohne Abfrage
                 ETSPRTD_RETURNDC|
                 ETSPRTD_4MFCPREVIEW|
                 ETSPRTD_QUIET:
            case ETSPRTD_DEVICEUNITS|            // besorge Druckbereich(DEVICEUNITS),DC ohne Initialisierung ohne Abfrage
                 ETSPRTD_RETURNDC|
                 ETSPRTD_4MFCPREVIEW|
                 ETSPRTD_QUIET:
                 ret = StartPrinting(pParams);
                 break;

            case 0:                              // nur Test ob ein Drucker auch installiert ist !
            case ETSPRTD_QUIET:                  // nur Test ob ein Drucker auch installiert ist ohne Meldung !
                 ret = 0;
                 break;

            default: throw ERROR_DWFLAGS;
         }
      }
      catch(int error)
      {
         TreatException(error);
         ret = -1;                               // Fehler oder kein Drucker gefunden !
      }

      g_bInDllCode = false;
      g_bQuiet     = false;                      // Bitte keine Meldung ausgeben immer zurücksetzen !
      SetLastError((DWORD)TlsGetValue(g_tlsidLastError));
      return ret;
   }
}                                                // END für extern "C"


/**************************************+****************************************
                                  EndPrinting

public Function: einen gestarteten Druckjob beenden

return         : -1 == Fehler
                  0 == alles OK
                
*******************************************************************************/

extern "C"
{                                                // diese DLL-Funktion ist die einzige die Reentrant ist !
   int EndPrinting(ETSPRTD_PARAMETER * pParams)
   {
      HDC hdc = NULL;

      BEGIN("EndPrinting");

      try
      {                                          // überprüfe die Pointer der Übergabeparameter dieser Funktion
         if(IsBadReadPtr(pParams,sizeof(ETSPRTD_PARAMETER))) 
            throw ERROR_PPARAMETER;
         if(pParams->dwSize!=sizeof(ETSPRTD_PRINTER)) 
            throw ERROR_BUFFER;
         if(IsBadReadPtr(pParams->pPrinterData,sizeof(ETSPRTD_PRINTER)))
            throw ERROR_PBUFFER;

         hdc = ((ETSPRTD_PRINTER*)pParams->pPrinterData)->hDC;
                                                 // Druckjob wurde initialisiert ?
         if((pParams->dwFlags&ETSPRTD_4MFCPREVIEW)==0)
         {                                       // Seite beenden       
            if(EndPage(hdc)<=0) throw ERROR_ENDPAGE;
                                                 // das Dokument beenden
            if(EndDoc(hdc)<=0) throw ERROR_ENDDOC;
         }

         if(!DeleteDC(hdc)) throw ERROR_DELETEDC;// den Devicecontext löschen
         return 0;
      }
      catch(int error)
      {
         TreatException(error);
         if(hdc) DeleteDC(hdc);                  // gibt es den DC noch ?, dann löschen
         SetLastError((DWORD)TlsGetValue(g_tlsidLastError));
         return -1;
      }
   }
}                                                // END für extern "C"

/*******************************************************************************
                                    DoDialog

public Function: Dialogbox anzeigen und auswerten

return         : -1 Fehler
                  0 alle OK
                  1 Benutzerabbruch

*******************************************************************************/
extern "C"
{
   int DoDialog(ETSPRTD_PARAMETER * pParams)
   {
      int   ret;
      DWORD dwFlags;
      DWORD size;
      bool  bGetData   = false;                  // sollen auch die Daten besorgt werden ?
      DWORD dwDialogID = IDD_PRINTERDIALOG;      // ID der Resource für das Dialogfeld

      BEGIN("DoDialog");

      if(g_bInDllCode)                           // Flag, zur überprüfung ob diese DLL Reentrant aufgerufen wird
      {                                          // die wird mit absicht nicht über Exception handling gelöst !
         TreatException(ERROR_REENTRANT);
         SetLastError(DLLE|ERROR_REENTRANT);     // Fehlercode der DLL setzen
         return -1;
      }      

      g_bInDllCode = true;
      TlsSetValue(g_tlsidLastError,0);           // noch kein Fehler !

      try
      {
         TestSavedPrinter();                     // überprüfe den Eingestellten Drucker oder besorge den Standarddrucker
                                                 // überprüfe den Pointer auf die Übergabeparameter dieser Funktion       
         if(IsBadReadPtr(pParams,sizeof(ETSPRTD_PARAMETER))) 
            throw ERROR_PPARAMETER;              // Ich muß hier auch schreiben können
         if(IsBadWritePtr(pParams,sizeof(ETSPRTD_PARAMETER))) 
            throw ERROR_PPARAMETER;

         dwFlags = pParams->dwFlags;

                                                 // alle ungültigen Flags nicht zulassen und mit Fehler antworten !
         if(dwFlags&(ETSPRTD_INVALID|ETSPRTD_GETALLDATA|ETSPRTD_SETALLDATA|ETSPRTD_QUIET|ETSPRTD_4MFCPREVIEW))
            throw ERROR_DWFLAGS;
                                                 // nur wenn ETS_PRTD_ISO oder ETS_PRTD_ANISO gesetzt wird hPreView ausgewertet
         if(dwFlags&ETSPRTD_ISO)
         {
            if(dwFlags&ETSPRTD_ANISO)            // beide Darstellungen sind unmöglich !
               throw ERROR_DWFLAGS;

            if(dwFlags&ETSPRTD_NOPREVIEW)        // es soll aber kein PreView erstellt werden
               throw ERROR_DWFLAGS;

            if(dwFlags&ETSPRTD_CALLBACK)         // soll das Metafile im Thread erzeugt werden ?
            {
                                                 // überprüfe ob ein gültiger Funktionszeiger
               if(pParams->hPreView==NULL)
                  throw ERROR_CALLBACK;
               if(IsBadCodePtr((FARPROC)pParams->hPreView))
                  throw ERROR_CALLBACK;

               SetPreViewCallback((void*)pParams->hPreView,true);
            }
            else
            {
                                                // überprüfe ob ein gültiges Metafile vorhanden ist
               if(pParams->hPreView==NULL) 
                  throw ERROR_METAFILE;
               if(GetEnhMetaFileBits(pParams->hPreView,0,NULL)==0)
                  throw ERROR_METAFILE;

               SetPreViewMetaFile(pParams->hPreView,true);
            }
         }
         else
         if(dwFlags&ETSPRTD_ANISO)
         {
            if(dwFlags&ETSPRTD_NOPREVIEW)        // es soll aber kein PreView erstellt werden
               throw ERROR_DWFLAGS;

            if(dwFlags&ETSPRTD_CALLBACK)         // soll das Metafile im Thread erzeugt werden ?
            {
                                                 // überprüfe ob ein gültiger Funktionszeiger
               if(pParams->hPreView==NULL)
                  throw ERROR_CALLBACK;
               if(IsBadCodePtr((FARPROC)pParams->hPreView))
                  throw ERROR_CALLBACK;

               SetPreViewCallback((void*)pParams->hPreView,false);
            }
            else
            {
                                                 // überprüfe ob ein gültiges Metafile vorhanden ist
               if(pParams->hPreView==NULL) 
                  throw ERROR_METAFILE;
               if(GetEnhMetaFileBits(pParams->hPreView,0,NULL)==0)
                  throw ERROR_METAFILE;

               SetPreViewMetaFile(pParams->hPreView,false);
            }
         }
         else
         {
            SetPreViewMetaFile(NULL,false);      // kein Vorschaumetafile vorhanden

            if(dwFlags&ETSPRTD_NOPREVIEW)        // das Dialogfeld ohne die Seitenvorschau
               dwDialogID = IDD_PRINTERDIALOGNO;
         }
                                                 // soll auch gleich gedruckt werden ?
         if(dwFlags&ETSPRTD_RETURNDC)
         {                                       // dann muß einer der beiden Flags (ETS_PRTD_LOWMETRICS oder ETS_PRTD_DEVICEUNITS) gesetzt sein !
              if(dwFlags&ETSPRTD_LOWMETRICS)
              {
                 if(dwFlags&ETSPRTD_DEVICEUNITS)
                    throw ERROR_DWFLAGS;         // beide Einheiten sind unmöglich !

                 g_bPrinterDialogTyp = true;     // Drucken - Dialogfeld
              }
              else
              if(dwFlags&ETSPRTD_DEVICEUNITS)
              {
                 g_bPrinterDialogTyp = true;     // Drucken - Dialogfeld
              }
              else throw ERROR_DWFLAGS;          // keine Einheitenangabe vorhanden !
         }
         else 
         {
            g_bPrinterDialogTyp = false;         // Drucker einrichten - Dialogfeld

            if(dwFlags&ETSPRTD_LOWMETRICS)
            {
                 if(dwFlags&ETSPRTD_DEVICEUNITS)
                    throw ERROR_DWFLAGS;         // beide Einheiten sind unmöglich !

                 bGetData = true;                // auch die Druckerdaten zurückgeben
            }
            else
            if(dwFlags&ETSPRTD_DEVICEUNITS)
            {
                 bGetData = true;                // auch die Druckerdaten zurückgeben
            }
         }

                                                 // überprüfe ob ein gültiger Fensterhandle übergeben wurde
         if(pParams->hParent==NULL)      throw ERROR_HWND;
         if(!IsWindow(pParams->hParent)) throw ERROR_HWND;

                                                 // den Eingestellten Drucker als aktuellen Drucker für das Dialogfeld auswählen
         GetPrinterParameter(g_pSavedInfo->pPrinterName,&g_pActualInfo);
                                                 // berechne die größe der DEVMODE-Daten (hier auch size immer OK)
         size = g_pSavedInfo->pDevMode->dmSize+g_pSavedInfo->pDevMode->dmDriverExtra;
                                                 // kopiere die DEVMODE-Daten !
         CopyMemory(g_pActualInfo->pDevMode,g_pSavedInfo->pDevMode,size);
         g_ActualParameters = g_SavedParameters; // kopiere die PRINTERPARAMETER

         switch(DialogBoxParam(g_hInstance,
                               MAKEINTRESOURCE(dwDialogID),
                               pParams->hParent,
                               (DLGPROC) DlgMessageFunction,
                               NULL))
         {
            case IDOK:
                 if(g_bPrinterDialogTyp)         // soll der Druckjob gestartet werden ?
                 {                               // Wichtig nicht nach Druckstart fragen
                    pParams->dwFlags|=ETSPRTD_QUIET;
                    ret = StartPrinting(pParams);
                 }
                 else                            // nur OK zurückgeben (Drucker Einrichten Dialog)
                 {
                    if(bGetData)
                       ret = FillPrinterData(pParams);
                    else
                       ret = 0;
                 }
                 break;
            case IDCANCEL:
                 ret = 1;                        // Benutzerabbruch zurückgeben
                 break;
            default: throw ERROR_PROCEED;        // ein Fehler ist in dem Dialogfeld aufgetretten
         }
      }
      catch(int error)
      {
         TreatException(error);
         ret = -1;                               // ein Fehler ist aufgetreten
      }

      SetPreViewMetaFile(NULL,false);            // Vorschaumetafile wieder ungültig !
      g_bInDllCode = false;
      SetLastError((DWORD)TlsGetValue(g_tlsidLastError));
      return ret;
   }
}


/*******************************************************************************
                                    Destructor

public Function: nötig für ausruf von Destructor eingebetteter Module

*******************************************************************************/

extern "C"
{
   void Destructor(void * pParam)
   {
      g_CEtsHelp.Destructor();
      ETSDEBUGEND;                               // ETSDEBUG.DLL Destructor aufrufen !
   }
}
