/********************************************************************/
/*                                                                  */
/*   Header-Datei für die DLL CARABOX: nur für das Aufrufprogramm   */
/*(Definition der relevanten Makros und der Aufruf-Klasse CCaraInfo)*/
/*                                                                  */
/* Die Schnittstellen-Funktionen von CARABOX werden vom aufrufenden */
/* Programm wie statische Klassen-Funktionen aufgerufen:            */
/*     CCaraBox::publicFunction(..);                                */
/*                                                                  */
/* Das aufrufende Programm deklariert an einer geeigneten Stelle    */
/* eine Instanz der Klasse CCaraBox als lokale (globale) Variable.  */
/* Die Art und der Ort der Instanzdeklaration richtet sich nach dem */
/* Nutzungsbereich der CARABOX.DLL. Wenn die Instanz gelöscht wird, */
/* wird über den Destructor der Klasse CCaraBox die DLL freigegeben */
/* Das aufrufende Programm 'includes' die CCaraBox.H und bindet die */
/* Dateien CCaraBox.LIB und CCaraBox.H in ihr Projekt mit ein.      */
/*                                                                  */
/*                                                                  */
/*     programmed by Dr.U.F.Thomanek                                */
/*                                                                  */
/*     Version 1.0          01.09.99                                */
/*                                                                  */
/********************************************************************/


#ifndef _CCARABOXH_
#define _CCARABOXH_


#define  CARABOX_HEAPERROR             1         // Fehler beim Anlegen/Zugriff des bzw. auf den Heap
#define  CARABOX_PROGRAMERROR          2         // Programmfehler
#define  CARABOX_DLLERROR              0xFFFF    // DLL-Datei konnte nicht geladen werden


class CCaraBox
{
   public:
      ~CCaraBox();

             void Destructor();

      static int  GetLastError();                // Wenn Return-Wert der 'Haupt'-Schnittstellenfunktionen
                                                 // = false, dann wird hiermit der ErrorCode abgefragt.
      
      static bool BoxEdit( HWND ); 		      	 // Aufruf Boxeneditor
};


#endif      // _CCARABOXH_

