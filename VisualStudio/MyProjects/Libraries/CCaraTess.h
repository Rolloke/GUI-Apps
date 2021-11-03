/********************************************************************/
/*                                                                  */
/*  Header-Datei für die DLL CARATESS: nur für das Aufrufprogramm   */
/*(Definition der relevanten Makros und der Aufruf-Klasse CCaraTess)*/
/*                                                                  */
/* Die Schnittstellen-Funktionen von CARATESS werden vom aufrufenden*/
/* Programm wie statische Klassen-Funktionen aufgerufen:            */
/*     CCaraTess::publicFunction(..);                               */
/*                                                                  */
/* Das aufrufende Programm deklariert an einer geeigneten Stelle    */
/* eine Instanz der Klasse CCaraTess als lokale (globale) Variable. */
/* Die Art und der Ort der Instanzdeklaration richtet sich nach dem */
/* Nutzungsbereich der CARATESS.DLL. Wenn die Instanz gelöscht wird,*/
/* wird über den Destructor der Klasse CCaraTess die DLL freigegeben*/
/* Das aufrufende Programm 'includes' die CCaraTess.H und bindet die*/
/* Dateien CCaraTess.LIB und CCaraTess.H in ihr Projekt mit ein.    */
/*                                                                  */
/*                                                                  */
/*     programmed by Dr.U.F.Thomanek                                */
/*                                                                  */
/*     Version 1.1          27.08.99                                */
/*                                                                  */
/*     Edited by O. Wesnigk                                         */
/*                                                                  */
/********************************************************************/


#ifndef _CCARATESSH_
#define _CCARATESSH_


#define  CARATESS_HEAPERROR           1          // Fehlercode für Anlegen Datenfelder im Heap
#define  CARATESS_PROGRAMERROR        2          // Programmfehler, AnzKonkavEcken falsch gezählt
#define  CARATESS_DIVISIONSERROR      3          // Fehler beim Teilen von Polygonen
#define  CARATESS_DLLERROR            0xFFFF     // DLL-Datei konnte nicht geladen werden


struct CARATESS_PTSPOLY                          // Struktur für Ketten-Liste der Teilpolygone
{
   CARATESS_PTSPOLY   *pNextPolygon;             // Pointer auf nächstes Element der Liste, NULL=Listenende
   int                nPoints;                   // Anzahl der Eckpunkte des konvexen Teilpolygons
   int                nIndices[1];               // nIndices ist Pointer auf Liste mit den Nummern der
                                                 // Eckpunkte für das Teilpolygon (dynamisch angelegt !!)
};

struct CARATESS_PTS                              // Struktur: Übergabeparameter für PolygonTessellation(..)
{
   HANDLE             hHeap;                     // Handle auf privaten HEAP (wird übergeben), innerhalb
                                                 // dessen mit HeapAlloc, HeapFree dynamischer Speicher
                                                 // reserviert und freigegeben werden kann
   int                nPoints;                   // Anzahl der Punkte des Originalpolygons (nicht konvex)
   POINT              *pPoints;                  // Pointer auf Original-Punkteliste des Original-Polygons
   CARATESS_PTSPOLY   *pPolygon;                 // Pointer auf erstes Element der geketteten Liste von
                                                 // PTS_POLY-Strukturen mit den konvexen Teil-Polgondaten
};



class CCaraTess
{
   public:
      ~CCaraTess();
       
             void Destructor();                  // neu 27.08.99 der DLL - Destructor

      static int  GetLastError();                // Funktion für Rückgabe des letzten Fehlercodes

      static bool PolygonTessellation( CARATESS_PTS& ); // Aufruf zur Polygonzerteilung
};


#endif      // _CCARATESSH_

