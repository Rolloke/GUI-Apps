/********************************************************************/
/*                                                                  */
/*  Header-Datei für die DLL CARAURIS: nur für das Aufrufprogramm   */
/*(Definition der relevanten Makros und der Aufruf-Klasse CCaraUris)*/
/*                                                                  */
/* Die Schnittstellen-Funktionen von CARAURIS werden vom aufrufenden*/
/* Programm wie statische Klassen-Funktionen aufgerufen:            */
/*     CCaraUris::publicFunction(..);                               */
/*                                                                  */
/* Das aufrufende Programm deklariert an einer geeigneten Stelle    */
/* eine Instanz der Klasse CCaraUris als lokale (globale) Variable. */
/* Die Art und der Ort der Instanzdeklaration richtet sich nach dem */
/* Nutzungsbereich der CARAURIS.DLL. Wenn die Instanz gelöscht wird,*/
/* wird über den Destructor der Klasse CCaraUris die DLL freigegeben*/
/* Das aufrufende Programm 'includes' die CCaraUris.H und bindet die*/
/* Dateien CCaraUris.LIB und CCaraUris.H in ihr Projekt mit ein.    */
/*                                                                  */
/*                                                                  */
/*     programmed by Dr.U.F.Thomanek                                */
/*                                                                  */
/*     Version 1.1             27.08.99                             */
/*                                                                  */
/*     Edited by O. Wesnigk                                         */
/*                                                                  */
/********************************************************************/


#ifndef _CCARAURISH_
#define _CCARAURISH_


#define  CARAURIS_HEAPERROR           1          // Fehlercode für Anlegen Datenfelder im Heap
#define  CARAURIS_PROGRAMERROR        2          // Programmfehler, Umriß nicht erstellbar
#define  CARAURIS_PARAMERROR          3          // Übergebene Parameter z.B. Winkel Psi falsch (s.unten)
#define  CARAURIS_DLLERROR            0xFFFF     // DLL-Datei konnte nicht geladen werden


#include "CALCVECT.H"                            // Vektorheader einbinden



struct CARAURIS_POLY                             // Struktur für die Daten z.B. eines Boxenpolygons
{
   int                nPoints;                   // Anzahl der Eckpunkte des Boxenpolygons
   int                *pnIndices;                // pnIndices ist Pointer auf Liste mit den Nummern der
                                                 // Eckpunkte für das betr. Polygon (dynamisch angelegt !!)
};

struct CARAURIS_UMRIPARAMS                       // Übergabegabestruktur für die Umrißerstellung
{
   HANDLE             hHeap;                     // Handle auf privaten HEAP (wird übergeben), innerhalb
                                                 // dessen mit HeapAlloc, HeapFree dynamischer Speicher
                                                 // reserviert und freigegeben werden kann
   int                nPoints;                   // Anzahl aller Eckpunkte der Boxenpolygone
   Vector             *pvPoints;                 // Pointer auf Punkteliste aller Boxenpolygoneckpunkte
   int                nNoOfPolys;                // Anzahl der Boxenpolygone
   CARAURIS_POLY      *pPolygon;                 // Pointer auf die Liste von POLYBOX-Strukturen
   int                nFlagProj;                 // Flag für die Projektionsrichtung in CALE-Koordinaten:
                                                 // 1 = Proj entlang der x-Achse auf die yz-Ebene (x=0)
                                                 // 2 = Proj entlang der y-Achse auf die zx-Ebene (y=0)
                                                 // 3 = Proj entlang der z-Achse auf die xy-Ebene (z=0)
   int                nPhi;                      // Ausrichtung der Box im CALE-System: Winkel Phi in °
   int                nTheta;                    // Ausrichtung der Box im CALE-System: Winkel Theta in °
   int                nPsi;                      // Ausrichtung der Box im CALE-System: Winkel Psi in °
                                                 // (für Psi gelten nur die Werte 0°, 90°, 180° u. 270° !!)
};

struct CARAURIS_POLYUMRISS                       // Struktur für Daten des Umrißpolygons (Rückgabeparameter)
{
   int                nPoints;                   // Anzahl der Eckpunkte des Umrißpolygons (Wert: 0)
   Vector             *pvPoints;                 // Pointer auf Eckpunkt-Arrays des Umrisses (Wert: NULL)
};




class CCaraUris
{
   public:
      ~CCaraUris();

             void Destructor();                  // neu 27.08.99 der DLL - Destructor

      static int  GetLastError();                // Funktion für Rückgabe des letzten Fehlercodes

                                                 // Aufruf zur Erstellung des Umrißpolygons
      static bool BoxenUmriss( CARAURIS_UMRIPARAMS&, CARAURIS_POLYUMRISS& );
};


#endif      // _CCARAURISH_

