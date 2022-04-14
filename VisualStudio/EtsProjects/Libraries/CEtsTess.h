/********************************************************************/
/*                                                                  */
/*  Header-Datei für die DLL ETSTESS: nur für das Aufrufprogramm    */
/*(Definition der relevanten Makros und der Aufruf-Klasse CEtsTess) */
/*                                                                  */
/* Die Schnittstellen-Funktionen von ETSTESS werden vom aufrufenden */
/* Programm wie statische Klassen-Funktionen aufgerufen:            */
/*     CEtsTess::publicFunction(..);                                */
/*                                                                  */
/* Das aufrufende Programm deklariert an einer geeigneten Stelle    */
/* eine Instanz der Klasse CEtsTess als lokale (globale) Variable.  */
/* Die Art und der Ort der Instanzdeklaration richtet sich nach dem */
/* Nutzungsbereich der ETSTESS.DLL. Wenn die Instanz gelöscht wird, */
/* wird über den Destructor der Klasse CEtsTess die DLL freigegeben */
/* Das aufrufende Programm 'includes' die CEtsTess.H und bindet die */
/* Dateien CEtsTess.LIB und CEtsTess.H in ihr Projekt mit ein.      */
/*                                                                  */
/*                                                                  */
/*     programmed by Dr.U.F.Thomanek                                */
/*                                                                  */
/*     Version 1.0          06.09.99                                */
/*                                                                  */
/*     Edited by O. Wesnigk                                         */
/*                                                                  */
/********************************************************************/


#ifndef _CETSTESSH_
#define _CETSTESSH_


#define  ETSTESS_HEAPERROR           1           // Fehlercode für Anlegen Datenfelder im Heap
#define  ETSTESS_PROGRAMERROR        2           // Programmfehler, AnzKonkavEcken falsch gezählt
#define  ETSTESS_DIVISIONSERROR      3           // Fehler beim Teilen von Polygonen
#define  ETSTESS_DLLERROR            0xFFFF      // DLL-Datei konnte nicht geladen werden


struct ETSTESS_PTSPOLY                           // Struktur für Ketten-Liste der Teilpolygone
{
   ETSTESS_PTSPOLY   *pNextPolygon;              // Pointer auf nächstes Element der Liste, NULL=Listenende
   int                nPoints;                   // Anzahl der Eckpunkte des konvexen Teilpolygons
   int                nIndices[1];               // nIndices ist Pointer auf Liste mit den Nummern der
                                                 // Eckpunkte für das Teilpolygon (dynamisch angelegt !!)
};

struct ETSTESS_PTS                               // Struktur: Übergabeparameter für PolygonTessellation(..)
{
   HANDLE             hHeap;                     // Handle auf privaten HEAP (wird übergeben), innerhalb
                                                 // dessen mit HeapAlloc, HeapFree dynamischer Speicher
                                                 // reserviert und freigegeben werden kann
   int                nPoints;                   // Anzahl der Punkte des Originalpolygons (nicht konvex)
   POINT              *pPoints;                  // Pointer auf Original-Punkteliste des Original-Polygons
   ETSTESS_PTSPOLY   *pPolygon;                  // Pointer auf erstes Element der geketteten Liste von
                                                 // PTS_POLY-Strukturen mit den konvexen Teil-Polgondaten
};



class CEtsTess
{
   public:
      ~CEtsTess();
       
             void Destructor();                  // neu 27.08.99 der DLL - Destructor

      static int  GetLastError();                // Funktion für Rückgabe des letzten Fehlercodes

      static bool PolygonTessellation( ETSTESS_PTS& ); // Aufruf zur Polygonzerteilung
};


#endif      // _CETSTESSH_

