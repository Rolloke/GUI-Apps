/********************************************************************/
/*                                                                  */
/*  Header-Datei für die DLL CARACONF: nur für das Aufrufprogramm   */
/*(Definition der relevanten Makros und der Aufruf-Klasse CCaraConf)*/
/*                                                                  */
/* Die Schnittstellen-Funktionen von CARACONF werden vom aufrufenden*/
/* Programm wie statische Klassen-Funktionen aufgerufen:            */
/*     CCaraConf::publicFunction(..);                               */
/*                                                                  */
/* Das aufrufende Programm deklariert an einer geeigneten Stelle    */
/* eine Instanz der Klasse CCaraConf als lokale (globale) Variable. */
/* Die Art und der Ort der Instanzdeklaration richtet sich nach dem */
/* Nutzungsbereich der CARACONF.DLL. Wenn die Instanz gelöscht wird,*/
/* wird über den Destructor der Klasse CCaraConf die DLL freigegeben*/
/* Das aufrufende Programm 'includes' die CCaraConf.H und bindet die*/
/* Dateien CCaraConf.LIB und CCaraConf.H in ihr Projekt mit ein.    */
/*                                                                  */
/*                                                                  */
/*     programmed by Dr.U.F.Thomanek                                */
/*                                                                  */
/*     Version 2.1          09.05.2001                              */
/*                                                                  */
/*     Edited by O. Wesnigk                                         */
/*                                                                  */
/********************************************************************/


#ifndef _CCARACONFH_
#define _CCARACONFH_


#define  CARACONF_HEAPERROR         1            // Fehlercode für Anlegen Datenfelder im Heap
#define  CARACONF_PROGRAMERROR      2            // Programmfehler, AnzKonkavEcken falsch gezählt
#define  CARACONF_DRAWERROR         3            // Fehler beim Zeichnen der BoxenConfiguration
#define  CARACONF_HELPERROR         4            // Fehler beim Laden des HelpStrings
#define  CARACONF_CONFERROR         5            // Fehler Konfiguration nicht vorhanden
#define  CARACONF_DLLERROR          0xFFFF       // DLL-Datei konnte nicht geladen werden


                                                 // Lautsprecher-Konfigurationen
#define STEREO                    1              // rechte und linke HiFi-Box, evtl. als Sat-Sub-Anlage
#define QUADRO                    2              // vier 'Main'-Speaker als Quadro-Anlage

#define SURROUND1_1               3              // Surround-Prologic: 2 Main,1 Center,2 Effekt
#define SURROUND1_2               4              // Surround-Prologic: 2 Main,2 Center,2 Effekt
#define SURROUND2_1               5              // wie SURROUND1_1 mit 2 weiteren Front-Effekt-Speaker (Yamaha)
#define SURROUND2_2               6              // wie SURROUND1_2 mit 2 weiteren Front-Effekt-Speaker (Yamaha)
#define SURROUND3_1               7              // wie SURROUND1_1 aber Ortung wie bei QUADRO (alle LS !), digital 5.1
#define SURROUND3_2               8              // wie SURROUND1_2 aber Ortung wie bei QUADRO (alle LS !), digital 5.1

#define PA_PLUSORTUNG             9              // freie Anordnung zur Beschallung (PA) mit Ortung
#define PA_OHNEORTUNG            10              // freie Anordnung zur Beschallung (PA) ohne Ortung

#define SURRDIGITAL6_1           11              // Surround Digital 6.1, mit rear Center, front u. rear Center je 1x
#define SURRDIGITAL6_2           12              // Surround Digital 6.1, mit rear Center (1x), front Center (2x)
#define SURRDIGITAL6_3           13              // Surround Digital 6.1, mit rear Center (2x), front Center (1x)
#define SURRDIGITAL6_4           14              // Surround Digital 6.1, mit rear Center, front u. rear Center je 2x

#define SURRDIGITAL7_1           15              // Surround Digital 7.1, mit 2 side Effect Spkr, front Center (1x)
#define SURRDIGITAL7_2           16              // Surround Digital 7.1, mit 2 side Effect Spkr, front Center (2x)

#define SURRDIGITAL8_1           17              // Surround Digital 8.1, mit 2 side Effect Spkr 
                                                 //                       und rear Center, front u. rear Center je 1x
#define SURRDIGITAL8_2           18              // Surround Digital 8.1, mit 2 side Effect Spkr
                                                 //                       und rear Center (1x), front Center (2x)
#define SURRDIGITAL8_3           19              // Surround Digital 8.1, mit 2 side Effect Spkr
                                                 //                       und rear Center (2x), front Center (1x)
#define SURRDIGITAL8_4           20              // Surround Digital 8.1, mit 2 side Effect Spkr
                                                 //                       und rear Center, front u. rear Center je 2x


                                                 // Standard-Boxentypen in CARA
#define STAND_ALONE               1              // eigenständige, alleine breitbandig spielfähige Box
#define MASTER                    2              // Subwoofer einer (N+1)-Satelliten-Subwoofer-Anlage
#define PSEUDO_MASTER             3              // 'virtueller' Teil eines oben definierten Subwoofers
#define SLAVE                     4              // Satellit einer (N+1)-Satelliten-Subwoofer-Anlage
#define ADD_ON1                   5              // eigenständiger Subwoofer (ADD_ONs sind bandbegrenzte LS)
#define ADD_ON2                   6              // eigenständiger Zusatz-Lautsprecher (z.B. Mid-Range)
#define ADD_ON3                   7              // eigenständiger Zusatz-Lautsprecher (z.B. Hochton-Modul)


                                                 // Bitstruktur für CARACAD CARASDB etc.
struct SourceConfiguration
{
   unsigned int bitMain1Front   : 1;             // Hauptlautsprecher
   unsigned int bitMain2Front   : 1;
   unsigned int bitMain3Rear    : 1;             // Quadrolautsprecher für hinten
   unsigned int bitMain4Rear    : 1;
   unsigned int bitCenterFront  : 1;             // Centerlautsprecher für vorne
   unsigned int bitDoubleFront  : 1;             // ist der Center vorne ein Doppelcenter 
   unsigned int bitCenterRear   : 1;             // Centerlautsprecher für hinten (neu 8.5.2001)
   unsigned int bitDoubleRear   : 1;             // ist der Center hinten ein Doppelcenter (neu 8.5.2001)
   unsigned int bitEffect1Front : 1;             // Effektlautsprecher für vorne oder für die Seiten (neu 8.5.2001)
   unsigned int bitEffect2Front : 1;
   unsigned int bitSideEffect   : 1;             // Flag, ob Effect(1/2)Front = Effektlautsprecher für 7.1 und 8.1
   unsigned int bitEffect3Rear  : 1;             // Effektlautsprecher für hinten
   unsigned int bitEffect4Rear  : 1;
   unsigned int bitAddonUnit    : 1;             // Subwoofer etc.
   unsigned int bitAllUnits     : 1;             // für PA , es sind alle Lautsprecher erlaubt

   unsigned int bitDummies      : 17;            // für zukünftige Erweiterungen
};


/********************************************************************/
/*                                                                  */
/*  Beschreibung der Schnittstellenfunktionen                       */
/*                                                                  */
/*  - GetLastError():                                               */
/*       Funktion: gibt den Fehlercode zurück, wenn eine der anderen*/
/*                 Schnittstellenfunktionen mit 'false' antwortet.  */
/*       Parameter: keine                                           */
/*                                                                  */
/*  - DrawBoxConfiguration( int, HDC, Rect& ):                      */
/*       Funktion: zeichnet ein Piktogramm der Boxenkonfiguration   */
/*                 entsprechenden Typs in den DeviceKontext des     */
/*                 übergebenen Rechtecks; der Hintergrund wird mit  */
/*                 Weiß gefüllt, Boxen in Blau, Hörer in Grün und   */
/*                 die Ortungsbeziehungen in Rot.                   */
/*       Parameter: ConfigTyp, Handle auf DeviceKontext, Referenz   */
/*                 auf ein Rechteck vom Typ RECT                    */
/*                                                                  */
/*  - GetAllConfigTypeId( int * ):                                  */
/*       Funktion: Eintragen aller Konfigurations-Ids in das über-  */
/*                 gebene Integerarrayfeld und Rückgabe der Anzahl  */
/*                 aller Konfigurationstypen                        */
/*                 (wenn übergebener Pointer == NULL, dann wird nur */
/*                  die Anzahl zurückgeben)                         */
/*       Parameter: Pointer auf Indexfeld                           */
/*                                                                  */
/*  - GetConfigTypeName( int, char *, int ):                        */
/*       Funktion: Eintragen des ausgewählten Konfigurationsnamens  */
/*                 in das übergebene Textfeld                       */
/*       Parameter: Konfigurationstyp, Pointer auf Textfeld,        */
/*                 Länge des Textfeldes                             */
/*                                                                  */
/*  - GetContextHelp( int, char *, int ):                           */
/*       Funkton: schreibt in das übergebene Textfeld den zu einem  */
/*                Konfigurationstyp zugehörigen Hilfetext.          */
/*       Paramter: ConfigTyp, Pointer aif Textfeld, Länge des Text- */
/*                feldes (zu verwenden wie bei LoadString())        */
/*                                                                  */
/********************************************************************/

class CCaraConf
{
   public:
      ~CCaraConf();

             void Destructor();                  // neu 27.8.99 der DLL - Destructor

      static int  GetLastError();                // Funktion für Rückgabe des letzten Fehlercodes
                                                 // zeichnet ausgewählte Boxenkonfiguration
      static bool DrawBoxConfiguration( int, HDC, RECT& );
      static int  GetAllConfigTypeId( int * );   // Anzahl und ID-Nummern der Konfig-Typen zurückgeben
                                                 // Name des ausgewählten Konfig-Typs zurückgeben
      static bool GetConfigTypeName( int, char *, int );
                                                 // Rückgabe des Hilfetextes zum ausgewählten Konfig-Typ
      static bool GetContextHelp( int, char *, int );
                                                 // Rückgabe des BitFields für eine Konfiguration (neu 9.05.2001)
      static SourceConfiguration GetInitConfiguration(int);
};


#endif      // _CCARACONFH_

