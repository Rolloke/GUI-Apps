#ifndef _CETSPRTD_
#define _CETSPRTD_

/**************************************+****************************************

                                   CEtsPrtD.h
                                   ----------

            Declaration der Schnittstellenklasse für die ETSPRTD.DLL

                     (C) 1999 ELAC Technische Software GmbH

                          VERSION 1.0 Stand 26.08.1999


                                                   programmed by Oliver Wesnigk
*******************************************************************************/


/**************************************+***********************************************
                                   Strukturen

**************************************************************************************/

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
   HENHMETAFILE hPreView;                        // Handle auf Metafile für Seitenvorschau in der Dialogbox oder Callbackfunktionspointer
   const char * szApplication;                   // Name der Anwendung  max 128
   const char * szDocument;                      // Name des Dokumentes max 256
                                                 // Input / Output
   DWORD        dwSize;                          // größe des Puffers für die Angeforderten Daten !
   void *       pPrinterData;                    // Puffer für die Angeforderten Daten !
};

struct ETSPRTD_SPECIALDATA                       // Struktur um einen Drucker zu setzen
{
   char  szPrinterName[32];                      // Name des Druckers
   RECT  rcMargin;                               // die Ränder für den Drucker (LOWMETRIC oder DEVICEUNITS)
   short dmOrientation;                          // die Seitenorientierung
   short dmPaperSize;                            // die Papierart
};

/**************************************+***********************************************
                          die Flags für die Schnittstelle

**************************************************************************************/

#define ETSPRTD_LOWMETRICS  0x00000001           // Ich möchte ETSPRTD_PRINTER Daten bekommen in LOWMETRIC-Einheiten
#define ETSPRTD_DEVICEUNITS 0x00000002           // Ich möchte ETSPRTD_PRINTER Daten bekommen in DEVICEUNITS
#define ETSPRTD_ISO         0x00000004           // die Vorschau soll Isotropisch dargestellt werden
#define ETSPRTD_ANISO       0x00000008           // die Vorschau soll Anisotropisch dargestellt werden
#define ETSPRTD_RETURNDC    0x00000010           // in den ETSPRTD_PRINTER bitte auch einen DeviceContext 
                                                 // zum Drucken übergeben und wenn nicht ETSPRTF_4MFCPREVIEW definiert Druckjob starten
#define ETSPRTD_GETALLDATA  0x00000020           // Ich möchte alle Druckereinstellungen erhalten
#define ETSPRTD_SETALLDATA  0x00000040           // Ich möchte alle Druckereinstellungen setzen
#define ETSPRTD_QUIET       0x00000080           // keine Meldung beim Starten eines Druckjobs über DoDataExchange, 
                                                 // bei PrintToFile wird Dialog für Ausgabedatei angezeigt
#define ETSPRTD_NOPREVIEW   0x00000100           // Dialogbox ohne Vorschau aufbauen
#define ETSPRTD_CALLBACK    0x00000200           // Metafile über Callbackfunktion erstellen
#define ETSPRTD_PRTOFFSET   0x00000400           // nur echten Printeroffset besorgen
#define ETSPRTD_4MFCPREVIEW 0x00000800           // kein Startdoc etc. ausführen (nur bei DoDataExchange,EndPrinting)
#define ETSPRTD_INVALID     0xfffff000           // alle nicht gültigen Flags ! (zur Fehlerabfrage)


/**************************************+***********************************************
                                    CEtsPrtD

**************************************************************************************/

class CEtsPrtD
{
public:
   CEtsPrtD(){};
   ~CEtsPrtD();

          void Destructor    (void);             // der Destructor darf nicht static sein
   static int  DoDataExchange(ETSPRTD_PARAMETER *);
   static int  EndPrinting   (ETSPRTD_PARAMETER *);
   static int  DoDialog      (ETSPRTD_PARAMETER *);
};

#endif                                           // ifndef