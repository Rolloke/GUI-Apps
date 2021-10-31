/**************************************+****************************************

                                  Beispiel.cpp
                                  ------------

            Beispiele zur Verwendung der Druckerkomponente ETSPRTD.DLL
                      unter Verwendung der Klasse CEtsPrtD

                     (C) 1999 ELAC Technische Software GmbH

                          VERSION 1.0 Stand 20.10.1999


                                                    programmed by Oliver Wesnigk
*******************************************************************************/

#include "CEtsPrtD.h"                                                // Header der Komponente
                                                                     // zum Project noch die Datei CEtsPrtD.Lib hinzufügen
                                                                     // die DLL wird durch eine Klasse gekapselt eingebunden
                                                                     // damit sie nicht sofort geladen werden muß, sondern
                                                                     // erst dann wenn sie benötigt wird (load on call)
void WinMain(....)
{
   CEtsPrtD cetsprtd;                                                // Druckkomponente anlegen (kann auch globale angelegt werden)
                                                                     // nach dem Anlegen einer Instance in WinMain oder auch als
                                                                     // globale Variable kann die Klasse als statische Komponete
                                                                     // verwendet werden sie übernimmt auch die Speicherung des
                                                                     // aktuellen Druckers und seine Einstellungen
                                                                     // für einen Process sind diese Einstellungen auch in anderen
                                                                     // Komponenten (DLL´s) vorhanden !


// ..... hier der Anwendungscode einfügen .....



    CEtsPrtD.Destructor();                                           // die Druckkomponente freigeben
}



                                          KURTZÜBERSICHT ALLE VERWENDUNGSARTEN

SCHNITTSTELLENFUNKTION: DoDataExchange

BEMERKUNG: überall wo das Flag ETSPRTD_LOWMETRICS verwendet wird kann an dessen Stelle auch ETSPRTD_DEVICEUNITS
           verwendet werden, um in Device-Einheiten Daten zu erhalten oder zu setzen.

dwFlags                   Funktion                                                                  Beispielnummer
------------------------+-------------------------------------------------------------------------+---------------
0                       | teste ob ein Drucker installiert ist                                    | 1
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_QUIET           | teste ob ein Drucker installiert ist ohne eine Meldung                  | 13
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_PRTOFFSET|      | besorge den echten Druckeroffset in LOWMETRIC                           | 2
ETSPRTD_LOWMETRICS      |                                                                         |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_GETALLDATA      | besorge Druckereinstellungen und aktuellen Drucker (Internesformat)     | 3
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_SETALLDATA      | setze Druckereinstellungen und aktuellen Drucker (Internesformat)       | 4
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_SETALLDATA|     | setze Druckereinstellung und aktuellen Drucker                          | 5
ETSPRTD_LOWMETRICS      | mit ETSPRTD_SPECIALDATA, ETSPRTD_SPECIALDATA::rcMargin in LOWMETRIC     |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS      | Druckerdaten besorgen in LOWMETRIC                                      | 6
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS|     | Druckerdaten besorgen mit hDC und Druckjob starten mit Abfrage          | 7
ETSPRTD_RETURNDC        |                                                                         |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS|     | Druckerdaten besorgen mit hDC und Druckjob starten ohne Abfrage         | 8
ETSPRTD_RETURNDC|       |                                                                         |
ETSPRTD_QUIET           |                                                                         |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS|     | Druckerdaten besorgen mit hDC ohne Druckjobstart und ohne Abfrage       | 9
ETSPRTD_RETURNDC|       |                                                                         |
ETSPRTD_QUIET|          |                                                                         |
ETSPRTD_4MFCPREVIEW     |                                                                         |
------------------------+-------------------------------------------------------------------------+---------------

SCHNITTSTELLENFUNKTION: DoDialog

BEMERKUNG: überall wo das Flag ETSPRTD_LOWMETRICS verwendet wird kann an dessen Stelle auch ETSPRTD_DEVICEUNITS
           verwendet werden, um in Device-Einheiten Daten zu erhalten oder zu setzen.
           überall wo das Flag ETSPRTD_ANISO verwendet wird kann an dessen Stelle auch ETSPRTD_ISO
           verwendet werden, um die isometrische Vorschaudarstellung zu erzwingen.

dwFlags                   Funktion                                                                  Beispielnummer
------------------------+-------------------------------------------------------------------------+---------------
0                       | Druckersetup mit Vorschau ohne Inhalt ohne Datenrückgabe                |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS      | Druckersetup mit Vorschau ohne Inhalt mit Datenrückgabe                 |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_ANISO           | Druckersetup mit Vorschau mit Inhalt ohne Datenrückgabe                 |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS|     | Druckersetup mit Vorschau mit Inhalt mit Datenrückgabe                  | 10
ETSPRTD_ANISO           |                                                                         |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS|     | Druckersetup mit Vorschau mit Inhalt mit Datenrückgabe wobei            |
ETSPRTD_ANISO|          | das EnhMetaFile im Thread gerendert wird                                |
ETSPRTD_CALLBACK        |                                                                         |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_NOPREVIEW       | Druckersetup ohne Vorschau ohne Datenrückgabe                           |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS|     | Druckersetup ohne Vorschau mit Datenrückgabe                            |
ETSPRTD_NOPREVIEW       |                                                                         |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS|     | Druckendialog mit Vorschau ohne Inhalt                                  |
ETSPRTD_RETURNDC        |                                                                         |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS|     | Druckendialog ohne Vorschau                                             | 11
ETSPRTD_RETURNDC|       |                                                                         |
ETSPRTD_NOPREVIEW       |                                                                         |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS|     | Druckendialog mit Vorschau mit Inhalt                                   |
ETSPRTD_RETURNDC|       |                                                                         |
ETSPRTD_ANISO           |                                                                         |
------------------------+-------------------------------------------------------------------------+---------------
ETSPRTD_LOWMETRICS|     | Druckendialog mit Vorschau mit Inhalt wobei das EnhMetaFile im          | 12
ETSPRTD_RETURNDC|       | Thread gerendert wird                                                   |
ETSPRTD_ANISO           |                                                                         |
ETSPRTD_CALLBACK        |                                                                         |
------------------------+-------------------------------------------------------------------------+---------------

BEMERKUNG: Rückgabewerte für die Schnittstellenfunktionen (DoDataExchange,DoDialog,EndPrinting)

           0 = OK
           1 = Abbruch (durch den Benutzer ausgelößt)
          -1 = Fehler, der Fehlercode kann mit GetLastError erhalten werden,
               die DLL-Melde den Fehlercode dem Benutzer wenn er kritisch ist
               der verwender kann diesen Code mit GetLastError erhalten
           2 = Drucker wurde nicht gefunden nur durch ETSPRTD_SETALLDATA aber
               alle Ränder und Papiereinstellungen konnten übernommen werden
           3 = Drucker, Ränder, Papier und Orientation  mußten geändert werden
               auch nur durch ETSPRTD_SETALLDATA






// 1. Teste ob ein Drucker installiert ist

bool TestPrinterExists()
{
   ETSPRTD_PARAMETER params;                                         // dies ist die Struktur mit den Übergabeparametern zu der DLL

   ZeroMemory(&params,sizeof(params));                               // bitte immer zuerst mit 0 füllen
   if(CEtsPrtD::DoDataExchange(&params)==0) return true;             // ein Drucker ist vorhanden

   return false;                                                     // es ist kein Drucker vorhanden
}

// 2. besorge den echten Druckeroffset in LOWMETRIC

bool GetPrinterOffset(POINT& OffsetLowMetric)
{
   ETSPRTD_PARAMETER params;
   POINT             offset;

   ZeroMemory(&params,sizeof(params));

   params.dwFlags      = ETSPRTD_PRTOFFSET|ETSPRTD_LOWMETRICS;
   params.dwSize       = sizeof(POINT);
   params.pPrinterData = &offset;

   if(CEtsPrtD::DoDataExchange(&params)==0)
   {
      OffsetLowMetric = offset;                  // in offset legt die Funktion den Rückgabewert ab
      return true;                               // OK, Offset wurde in OffsetLowMetric eingetragen
   }
   return false;                                 // Fehler
}

// 3. besorge Druckereinstellungen und aktuellen Drucker (Internesformat)

void GetActualPrinterAndData()
{
   ETSPRTD_PARAMETER params;

   ZeroMemory(&params,sizeof(params));
   params.dwFlags  = ETSPRTD_GETALLDATA;

   if(CEtsPrtD::DoDataExchange(&params)==0)                          // da params.pPrinterData == NULL gibt dieser Aufruf
   {                                                                 // in params.dwSize die benötigte Puffergröße zurück
      char * puffer = new char[params.dwSize];                       // Puffer anlegen

      if(puffer)                                                     // konnte der puffer angelegt werden ?
      {
         params.pPrinterData = puffer;                               // Zeiger eintragen

         if(CEtsPrtD::DoDataExchange(&params))                       // Daten in den Puffer übertragen
         {
                                                                     // OK, die Daten sind nach puffer übertragen
            // ..... hier die Daten in ein
            //       File oder der Registry speichern .....
         }

         delete params.pPrinterData;
      }
   }
}

// 4. setze Druckereinstellungen und aktuellen Drucker (Internesformat)

void SetActualPrinterAndData(DWORD size,char * puffer)
{
   ETSPRTD_PARAMETER params;
   DWORD             size;
   char *            puffer;


   // .... hier lese die länge der Daten (size)
   //      lege Puffer (puffer) an
   //      lese den Puffer (puffer) ein .....

   ZeroMemory(&params,sizeof(params));
   params.dwFlags      = ETSPRTD_SETALLDATA;
   params.dwSize       = size;
   params.pPrinterData = puffer;

   switch(CEtsPrtD::DoDataExchange(&params))
   {
      case 0:                                    // alles OK, Drucker gefunden Einstellungen übernommen
      case 2:                                    // Drucker nicht gefunden, Standarddrucker konnte aber
                                                 // Ränder,Papier und Orientierung übernehmen
           break;
      case 3:                                    // Drucker nicht gefunden, Standarddrucker konnte
                                                 // die Einstellungen nicht vollständig übernehmen
                                                 // somit ist eine Neuformatierung des Dokumentes notwendig

           // ..... hier Druckerdaten besorgen und
           //       Dokument neu formatieren .....

           break;
      default:                                   // Fehler
   }

   // ..... hier den Puffer (puffer)
   //       wieder freigeben .....

}


// 5. setze Druckereinstellung und aktuellen Drucker

void SetKnownActualPrinterAndData()
{
   ETSPRTD_PARAMETER   params;
   ETSPRTD_SPECIALDATA special;

   // ..... hier die Daten (special) füllen z.B.

   lstrcpy(special.szPrinterName,"Drucker");     // der gewünschte Drucker
   special.rcMargin.left   = 100;                // alle Ränder auf 1cm setzen (LOWMETRIC)
   special.rcMargin.top    = 100;
   special.rcMargin.right  = 100;
   special.rcMargin.bottom = 100;
   special.dmOrientation   = DMORIENT_LANDSCAPE; // Orientierung auf Querformat
   special.dmPaperSize     = DMPAPER_LETTER      //  Letter, 8 1/2 by 11 inches

   // .....

   ZeroMemory(&params,sizeof(params));
   params.dwFlags      = ETSPRTD_SETALLDATA|ETSPRTD_LOWMETRICS;
   params.dwSize       = sizeof(special);
   params.pPrinterData = &special;

   switch(CEtsPrtD::DoDataExchange(&params))
   {
      case 0:                                    // alles OK, Drucker gefunden Einstellungen übernommen
      case 2:                                    // Drucker nicht gefunden, Standarddrucker konnte aber
                                                 // Ränder,Papier und Orientierung übernehmen
           break;
      case 3:                                    // Drucker nicht gefunden, Standarddrucker konnte
                                                 // die Einstellungen nicht vollständig übernehmen
                                                 // somit ist eine Neuformatierung des Dokumentes notwendig

           // ..... hier Druckerdaten besorgen und
           //       Dokument neu formatieren .....

           break;
      default:                                   // Fehler
   }
}

// 6. Druckerdaten besorgen in LOWMETRIC

void GetDataForPrinting()
{
   ETSPRTD_PARAMETER params;
   ETSPRTD_PRINTER   data;

   ZeroMemory(&params,sizeof(params));
   params.dwFlags      = ETSPRTD_LOWMETRICS;
   params.dwSize       = sizeof(data);
   params.pPrinterData = &data;

   if(CEtsPrtD::DoDataExchange(&params)==0)
   {
      ? = data.ptOffset;                         // der zum Drucken zu verwendende Offset (berücksichtig die Ränder)
      ? = data.rcMargin;                         // die vom Benutzer eingestellten Ränder
      ? = data.siDrawArea;                       // die verbliebene Größe des zu bedruckenden Bereiches
      ? = data.siPageSize;                       // die Gesamtgröße des eingestellten Papieres
   }
}



// 7. Druckerdaten besorgen mit hDC und Druckjob starten mit Abfrage

void PrintingYesNo(HWND hwndMain,const char * szDocName)
{
   ETSPRTD_PARAMETER params;
   ETSPRTD_PRINTER   data;

   ZeroMemory(&params,sizeof(params));

   params.dwFlags       = ETSPRTD_RETURNDC|ETSPRTD_LOWMETRIC;
   params.hParent       = hwndMain;              // Fensterhandle vom Hauptfenster (wichtig !)
   params.dwSize        = sizeof(ETSPRTD_PRINTER);
   params.pPrinterData  = &data;                 // Zeiger auf die Druckdaten (ETSPRTD_PRINTER)
   params.szApplication = "CARACAD";             // Namen der Anwendung
   params.szDocument    = szDocName;             // Filenamen des Dokumentes

   if(CEtsPrtD::DoDataExchange(&params)==0)      // Druckjob starten ? (Messagebox) und eventuell Ausgabedatei eingeben
   {

      // .... hier drucken mit den Daten
      //      welche in data enthalten sind .....

      CEtsPrtD::EndPrinting(&params);            // Druckjob beenden
   }
}

// 8. Druckerdaten besorgen mit hDC und Druckjob starten ohne Abfrage
//    Besonderheit: drucken in einem Thread

void SuddenPrinting(HWND hwndMain,const char * szDocName)
{
   ETSPRTD_PARAMETER * params;

                                                 // params dynamisch anlegen !
   params                = new ETSPRTD_PARAMETER;

   ZeroMemory(params,sizeof(params));

   params->pPrinterData  = new ETSPRTD_PRINTER;

   params->dwFlags       = ETSPRTD_RETURNDC|ETSPRTD_LOWMETRICS|ETSPRTD_QUIET;
   params->hParent       = hwndMain;             // Wichtig, obwohl keine Abfrage aber kann Ausgabedatei benötigen
   params->dwSize        = sizeof(ETSPRTD_PRINTER);
   params->szApplication = "CARACAD";
   params->szDocument    = szDocName;

   if(CEtsPrtD::DoDataExchange(&params)==0)      // Ok, Druckjob ist gestarted
   {
      DWORD dwThreadId;
                                                 // Druckthread erzeugen und params übergeben
                                                 // der Druckthread ist für die Freigabe von params verantwortlich
      CreateThread(NULL,NULL,PrintThread,params,NULL,&dwThreadId);
   }
}

LRESULT WINAPI PrintThread(LPVOID * parameter)
{
   ETSPRTD_PARAMETER * params = (ETSPRTD_PARAMETER*) parameter;

   // ..... hier drucken mit den Daten
   //       welche in params->pPrinterData stehen .....

   CEtsPrtD::EndPrinting(&params);               // Druckjob beenden

   delete params->pPrinterData;
   delete params;

   return 0;
}

// 9. Druckerdaten besorgen mit hDC ohne Druckjobstart und ohne Abfrage

void InitPreView(HWND hwndMain)
{
   ETSPRTD_PARAMETER params;
   ETSPRTD_PRINTER   data;

   ZeroMemory(&params,sizeof(params));

   params.dwFlags      = ETSPRTD_RETURNDC|ETSPRTD_LOWMETRICS|ETSPRTD_QUIET|ETSPRTD_4MFCPREVIEW;
   params.dwSize       = sizeof(ETSPRTD_PRINTER);
   params.pPrinterData = &data;
                                                 // Bemerkung der Fensterhandle, Anwendungsname und Dokumentname wird nicht benötigt
                                                 // kann aber natürlich initialisiert sein !
   if(CEtsPrtD::DoDataExchange(&params))
   {
      // .... hier zum Beispiel
      //      MFC-Preview bearbeiten etc. .....

      // entweder
         DeleteDC(data.hDC);                     // den hDC löschen
      // oder
         CEtsPrtD::EndPrinting(&params);         // den hDC löschen, Achtung params.dwFlags muß ETSPRTD_4MFCPREVIEW enthalten !
   }
}

// 10. Druckersetup mit Vorschau mit Inhalt mit Datenrückgabe

void PrinterSetup(HWND hwndMain,ENHMETAFILE hPreViewMetaFile)
{
   ETSPRTD_PARAMETER params;
   ETSPRTD_PRINTER   data;

   ZeroMemory(&params,sizeof(params));
   params.dwFlags      = ETSPRTD_LOWMETRICS|ETSPRTD_ANISO;
   params.hParent      = hwndMain;
   params.hPreView     = hPreViewMetaFile;       // die aufrufende Anwendung ist besitzer des MetaFiles
   params.dwSize       = sizeof(ETSPRTD_PRINTER);
   params.pPrinterData = &data;

   if(CEtsPrtD::DoDialog(&params)==0)            // kein Fehler und OK-Button
   {
      // .... hier mit folgenden Daten
      //      data.siPageSize
      //      data.rcMargin
      //      data.siDrawArea
      //      data.ptOffset
      //      neu formatieren .....
   }
}

// 11. Druckendialog ohne Vorschau

void Printing(HWND hwndMain,const char * szDocName)
{
   ETSPRTD_PARAMETER params;
   ETSPRTD_PRINTER   data;

   ZeroMemory(&params,sizeof(params));
   params.dwFlags       = ETSPRTD_LOWMETRICS|ETSPRTD_RETURNDC|ETSPRTD_NOPREVIEW;
   params.hParent       = hwndMain;
   params.dwSize        = sizeof(ETSPRTD_PRINTER);
   params.pPrinterData  = &data;
   params.szApplication = "CARA2DVIEW";          // Namen der Anwendung
   params.szDocument    = szDocName;             // Filenamen des Dokumentes

   if(CEtsPrtD::DoDialog(&params)==0)            // OK, Druckjob ist gestarted
   {
      // ..... hier drucken mit den Daten
      //       welche in data stehen .....

      CEtsPrtD::EndPrinting(&params);            // Druckjob beenden
   }
}

// 12. Druckendialog mit Vorschau mit Inhalt wobei das EnhMetaFile in einem Thread gerendert wird
//     und der Druckjob in einem Thread abläuft

void ComplexPrinting(HWND hwndMain,const char * szDocName)
{
   ETSPRTD_PARAMETER * params;
   ETSPRTD_PRINTER   * data;

   params                = new ETSPRTD_PARAMETER;

   ZeroMemory(params,sizeof(params));

   params->pPrinterData  = new ETSPRTD_PRINTER;

   params->dwFlags       = ETSPRTD_LOWMETRICS|ETSPRTD_RETURNDC|ETSPRTD_ANISO|ETSPRTD_CALLBACK;
   params->hParent       = hwndMain;
   params->hPreView      = RenderMetaFile;        // Zeiger auf Funktion zum Rendern des MetaFiles
   params->dwSize        = sizeof(ETSPRTD_PRINTER);
   params->szApplication = "CARA2DVIEW";          // Namen der Anwendung
   params->szDocument    = szDocName;             // Filenamen des Dokumentes

   if(CEtsPrtD::DoDataExchange(&params)==0)      // Ok, Druckjob ist gestarted
   {
      DWORD dwThreadId;
                                                 // Druckthread erzeugen und params übergeben
                                                 // der Druckthread ist für die Freigabe von params verantwortlich
                                                 // Funktion PrintThread siehe oben
      CreateThread(NULL,NULL,PrintThread,params,NULL,&dwThreadId);
   }
}

HENHMETAFILE RenderMetaFile()                    // Achtung _cdelc nicht _pascal (Funktion wird von ETSPRTD.DLL aufgerufen)
{
   // ..... hier das
   //       MetaFile erzeugen .....

   return hMetaFile;                             // MetaFileHandle zurückgeben (Achtung ETSPRTD.DLL löscht diese MetaFile)
}

// 13. Teste ob ein Drucker installiert ist ohne Meldung

bool TestPrinterExists()
{
   ETSPRTD_PARAMETER params;                                         // dies ist die Struktur mit den Übergabeparametern zu der DLL

   ZeroMemory(&params,sizeof(params));                               // bitte immer zuerst mit 0 füllen
   params.dwFlags = ETSPRTD_QUIET;                                   // damit keine Meldung ausgegeben wird !
   if(CEtsPrtD::DoDataExchange(&params)==0) return true;             // ein Drucker ist vorhanden

   return false;                                                     // es ist kein Drucker vorhanden
}