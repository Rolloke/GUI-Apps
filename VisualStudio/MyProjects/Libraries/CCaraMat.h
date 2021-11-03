/********************************************************************/
/*                                                                  */
/*  Header-Datei für die DLL CARAMAT: nur für das Aufrufprogramm    */
/* (Definition der relevanten Makros und der Aufruf-Klasse CCaraMat)*/
/*                                                                  */
/* Die Schnittstellen-Funktionen von CARAMAT werden vom aufrufenden */
/* Programm wie statische Klassen-Funktionen aufgerufen:            */
/*     CCaraMat::publicFunction(..);                                */
/*                                                                  */
/* Das aufrufende Programm deklariert an einer geeigneten Stelle    */
/* eine Instanz der Klasse CCaraMat als lokale (globale) Variable.  */
/* Die Art und der Ort der Instanzdeklaration richtet sich nach dem */
/* Nutzungsbereich der CARAMAT.DLL. Wenn die Instanz gelöscht wird, */
/* wird über den Destructor der Klasse CCaraMat die DLL freigegeben.*/
/* Das aufrufende Programm 'includes' die CCaraMat.H und bindet die */
/* Dateien CCaraMat.LIB und CCaraMat.H in ihr Projekt mit ein.      */
/*                                                                  */
/*                                                                  */
/*     programmed by Dr.U.F.Thomanek                                */
/*                                                                  */
/*     Version 1.1          30.09.99                                */
/*                                                                  */
/*     Edited by O. Wesnigk 31.08.99                                */
/*                                                                  */
/********************************************************************/


#ifndef _CCARAMATH_
#define _CCARAMATH_


#define  CARAMAT_HEAPERROR             1         // Fehlercode für Anlegen Datenfelder im Heap
#define  CARAMAT_PROGRAMERROR          2         // Programmfehler
#define  CARAMAT_CARAHELPERROR         3         // Fehler beim Laden des Help- oder Error-Strings
#define  CARAMAT_FILEREADERROR         4         // Fehler beim File-Lesen
#define  CARAMAT_FILESAVEERROR         5         // Fehler beim File-Schreiben
#define  CARAMAT_DLLERROR              0xFFFF    // DLL-Datei konnte nicht geladen werden
#define  CARAMAT_MATINDEXERROR         7         // falscher Materialindex (z.B. > Anzahl Materialien)
#define  CARAMAT_LOADBMERROR           8         // Fehler beim Laden einer Bitmap
#define  CARAMAT_USERHEAPERROR         9         // Fehler für Anlegen Datenfelder im Userheap (GetBitmapInfo)

#define  CARAMAT_MATDESCRLENGTH		  96   		 // Länge des szString für Materialbeschreibung
#define  CARAMAT_MATNAMELENGTH    	  32 			 // Länge des szString für Materialname
#define  CARAMAT_GROUPNAMELENGTH 	  32 		    // Länge des szString für den Namen der Materialgruppe
#define  CARAMAT_MAXNOOFFREQ     	 118 	   	 // maximale Anzahl der Frequenzstützpunkte (5 - 40960 Hz)

#define  CARAMAT_BITMAPTYPE8X8         8         // Typ der BitmapInfo-Daten (8x8 Bitmap)
#define  CARAMAT_BITMAPTYPE32X32      32         // Typ der BitmapInfo-Daten (32x32 Bitmap)


struct CARAMAT_MATDSCRS                          // Struktur für Materialbeschreibung für CARACAD
{
   int    nMatIndex;                             // Index/Nummer des Absorbermaterials
   int    nGroupIndex;                           // Index/Nummer der Materialgruppe, zu der Mat gehört
   char   pszMatName[CARAMAT_MATNAMELENGTH];     // Text für Materialname
   char   pszDescription[CARAMAT_MATDESCRLENGTH];// Text für Materialbeschreibung
   char   pszGroupName[CARAMAT_GROUPNAMELENGTH]; // Text für Materialgruppenname
};

struct CARAMAT_MATREFLS                          // Struktur für Materialdaten für CARACALC
{
   int      nMatIndex;                           // Index/Nummer des Absorbermaterials
   int      nNoOfFreq;                           // Anzahl der benutzten Frequenzstützpunkte
   double   dReflGradLowFreq;                    // Reflektionsgrad (Schallintensität !!) bei tiefen Freq.
   double   dFrequency[CARAMAT_MAXNOOFFREQ];     // Feld mit nNoOfFreq Frequenzwerten zwischen 5-40960 Hz
   double   dReflGradFreq[CARAMAT_MAXNOOFFREQ];  // Feld mit nNoOfFreq Reflektionsgraden zu den Frequenzen
};



class CCaraMat
{
   public:
      ~CCaraMat();

             void Destructor();                  // neu 27.08.99 by O. Wesnigk

      static int  GetLastError();                // Wenn Return-Wert der 'Haupt'-Schnittstellenfunktionen
                                                 // = false, dann wird hiermit der ErrorCode abgefragt.
      
      static bool EditMaterial( HWND );          // HWND: Handle des aufrufenden Fensters
      
      static bool SelectMaterial( HWND, int *, char * );
                                                 // HWND: Handle des aufrufenden Fensters
                                                 // int *: Pointer auf nMatIndex, Aufruf-/Rückgabeparameter
                                                 // char *: Pointer auf szString (Überschrift), Aufrufpar.

      static bool GetBitmapInfo( int, int, HANDLE, BITMAPINFO ** );
                                                 // int: nMatIndex, Index-Nummer des gewünschten Materials
                                                 // int: nType, TypMakro für gewünschte Bitmap (8x8, 32x32)
                                                 // HANDLE: hMyHeap, Handle auf privaten Heap des Aufrufers
                                                 // BITMAPINFO **: ppBitmapInfo, Pointer auf Pointer auf
                                                 //                BITMAPINFO-Struktur, die in hMyHeap von
                                                 //                CARAMAT angelegt und beschrieben wird.

      static bool GetMatDscrDaten( int, CARAMAT_MATDSCRS * );
                                                 // int: nMatIndex, gewünschter MaterialIndex
                                                 // MatDscrDaten *: Pointer auf Struktur für Texte,
                                                 //                 Rückgabeparameter

      static bool GetMatReflDaten( int, CARAMAT_MATREFLS * );
                                                 // int: nMatIndex, gewünschter MaterialIndex
                                                 // MatReflDaten *: Pointer auf Struktur für Refl-Grade,
                                                 //                 Rückgabeparameter
};


#endif      // _CARAMATH_

