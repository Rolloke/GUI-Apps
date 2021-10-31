/********************************************************************/
/*                                                                  */
/*  Header-Datei für die DLL CARAINFO: nur für das Aufrufprogramm   */
/*(Definition der relevanten Makros und der Aufruf-Klasse CCaraInfo)*/
/*                                                                  */
/* Die Schnittstellen-Funktionen von CARAINFO werden vom aufrufenden*/
/* Programm wie statische Klassen-Funktionen aufgerufen:            */
/*     CCaraInfo::publicFunction(..);                               */
/*                                                                  */
/* Das aufrufende Programm deklariert an einer geeigneten Stelle    */
/* eine Instanz der Klasse CCaraInfo als lokale (globale) Variable. */
/* Die Art und der Ort der Instanzdeklaration richtet sich nach dem */
/* Nutzungsbereich der CARAINFO.DLL. Wenn die Instanz gelöscht wird,*/
/* wird über den Destructor der Klasse CCaraInfo die DLL freigegeben*/
/* Das aufrufende Programm 'includes' die CCaraInfo.H und bindet die*/
/* Dateien CCaraInfo.LIB und CCaraInfo.H in ihr Projekt mit ein.    */
/*                                                                  */
/*                                                                  */
/*     programmed by Dr.U.F.Thomanek                                */
/*                                                                  */
/*     Version 1.1          30.09.99                                */
/*                                                                  */
/*     Edited by O. Wesnigk                                         */
/*                                                                  */
/********************************************************************/


#ifndef _CCARAINFOH_
#define _CCARAINFOH_


#define  CARAINFO_HEAPERROR            1         // Fehler beim Anlegen/Zugriff des bzw. auf den Heap
#define  CARAINFO_PROGRAMERROR         2         // Programmfehler
#define  CARAINFO_FILEREADERROR        3         // Fehler beim Lesen der betreffenden Resourcendatei
#define  CARAINFO_DLLERROR             0xFFFF    // DLL-Datei konnte nicht geladen werden


class CCaraInfo
{
   public:
      ~CCaraInfo();

             void  Destructor();                 // neu 27.08.99 der DLL - Destructor

      static int   GetLastError();               // Wenn Return-Wert der 'Haupt'-Schnittstellenfunktionen
                                                 // = false, dann wird hiermit der ErrorCode abgefragt.
      
      static bool  CallInfoResource(HWND,char *);// Handle des Aufruf-Fensters, Prog.Modulname

      static DWORD GetPackageID();               // CARA-ID besorgen (Sicherheitscode)
};


#endif      // _CCARAINFOH_

