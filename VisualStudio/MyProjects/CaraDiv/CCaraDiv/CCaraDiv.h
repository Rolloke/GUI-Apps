/*******************************************************************************
                                 CEts3DGL
  Zweck:
   Berechnung von linearer und logarithmischer Streckenaufteilung in 
   Abhängigkeit vom Darstellungsbereich und der Schrittweite in logischen 
   Koordinaten und dem aufzuteilenden Bereich in Weltkoordinaten.
  Header:            Bibliotheken:        DLL´s:
   ----              EtsBind.lib          ETSBIND.dll
   ----              ----                 ETSDIV.dll
  Benutzung:
   Die ETSBIND.dll ist eine statisch gebundene Dll. Die ETSDIV.dll ist eine
   "load on call-Dll". D.h., sie wird erst geladen, wenn sie das erste mal be-
   nötigt wird.
  Copyright:
   (C) 1999 ELAC Technische Software GmbH
  VERSION 
   1.10 Stand 08.12.1999
                                                   programmed by Rolf Ehlers
*******************************************************************************/


#ifndef _CETSDIV_H_
#define _CETSDIV_H_

#include <windows.h>


struct ETSDIV_SCALE;
typedef bool (*ETSDIV_SCALE_FKT) (ETSDIV_SCALE*);

struct ETSDIV_NUTOTXT               // Struktur für Textformatierung
{
   double      fvalue;              // zu wandelende float Zahl
   UINT        nmode;               // Rundungsmodus NM_STD, NM_SCI, NM_ENG. Die Modi koennen mit | kombiniert werden.
   int         nround;              // Stellen hinter dem Komma, auf die gerundet wird
   int         nmaxl;               // maximale Zeichenkettenlänge
   double      dZero;               // Ausgabe von 0, wenn die Werte kleiner sind als dieser Wert
};

struct ETSDIV_DIV                   // Struktur für die Aufteilung von Strecken
{
   long        l1, l2,              // Darstellungsbereich in logischen Koordinaten
               width,               // ermittelte Schrittweite in logischen Koordinaten (Platz für Linienbeschriftung)
               stepwidth,           // eingestellte Schrittweite in logischen Koordinaten
               division,            // Teilung der Frequenzschritte f(div) = f0 * pow(2, 1/div);
               divfnc;              // Funktion die zur Ermittlung der Teilung verwendet wurde
   double      f1, f2;              // Darzustellender Bereich in Weltkoordinaten
   double      dZero;               // Ausgabe von 0, wenn die Werte kleiner sind als der Wert
};

struct ETSDIV_SCALE                 // Struktur für das Zeichnen einer Skala
{
   ETSDIV_DIV       div;            // Struktur für die Aufteilung von Strecken
   ETSDIV_NUTOTXT   ntx;            // Struktur für Textformatierung
   char            *pszText;        // Zeiger auf Ausgabetext
   ETSDIV_SCALE_FKT pFkt;           // Zeiger auf Ausgabefunktion
   DWORD            dwFlags;        // Darstellungsflags
   double           dPosX,          // Ausgabeposition X
                    dPosY,          // Ausgabeposition Y
                    dGradient,      // Parameter für die Geradengleichung zur Berechnung
                    dOrigin;        // der Position
   void            *pParam;         // zusätzliche Parameter
};

#define ETSDIV_POLAR       0
#define ETSDIV_LINEAR      1
#define ETSDIV_LOGARITHMIC 2
#define ETSDIV_FREQUENCY   3

#define ETSDIV_NM_STD            0x00000001     // Standard Zahlendarstellung 1.234
#define ETSDIV_NM_SCI            0x00000002     // Exponentdarstellung 1.234e-5
#define ETSDIV_NM_ENG            0x00000003     // Exponentdarstellung 1.234e3 mit durch 3 teilbarem Exponenten

#define ETSDIV_NM_NO_END_NULLS   0x00010000     // Keine Nullen am Ende 
#define ETSDIV_NM_GROUPING       0x00020000     // Guppierung der Zahlen vor dem Komma
#define ETSDIV_NM_LOCALE         0x00040000     // Lokale einstellungen verwenden

#define ETSDIV_NM_AUTO_STD       0x01000000     // Automatisch auf STD Modus umstellen
#define ETSDIV_NM_AUTO_SCI       0x02000000     // Automatisch auf SCI Modus umstellen
#define ETSDIV_NM_AUTO_ENG       0x04000000     // Automatisch auf ENG Modus umstellen

#define ETSDIV_UC_CTRL_INIT      0x00100000     // Das EditCtrl initialisiert die Dialogfelder
#define ETSDIV_UC_CTRL_REINIT    0x00200000     // Das EditCtrl wird wieder initialisiert
#define ETSDIV_UC_CTRL_SIGNED    0x00400000     // Auch negative Zahlen

#define ETSDIV_CALC_GRADIENT     0x00000001     // berechne Geradengleichung
#define ETSDIV_CALC_X_POS        0x00000002     // berechne X-Position
#define ETSDIV_CALC_Y_POS        0x00000004     // berechne Y-Position
#define ETSDIV_CALC_POS          0x00000006     // dito X,Y
#define ETSDIV_CALC_POLAR_VALUES 0x00000008     // Sinus und Cosinuswerte neu berechnen mit Anzahl in nIndex
#define ETSDIV_CALC_ZEROVALUE    0x00000010     // berechne Null-Wert
#define ETSDIV_CALC_OCT_AND_THI  0x00000020     // berechne Oktaven und Terzflags
#define ETSDIV_CALC_POLAR_VALUE  0x00000040     // Sinus und Cosinuswert berechnen

#define ETSDIV_FORMAT_TEXT       0x00000100     // Formatierte Zahlenausgabe
#define ETSDIV_AUTO_FREQUENCY    0x00000200     // Automatische Umschaltung auf Frequenzteilung

#define ETSDIV_LOG_WORLD_RANGE   0x00001000     // Weltkoordinatenbereich logarithmisch
#define ETSDIV_POLAR_GRAD        0x00002000     // Winkelbereich in Grad 0 - 360°
#define ETSDIV_POLAR_RAD         0x00004000     // Winkelbereich in Rad  0 - 2*pi
#define ETSDIV_POLAR_GON         0x00008000     // Winkelbereich in Gon  0 - 400°

#define ETSDIV_DEKADE_VALUE      0x00010000     // Wert entspricht einer Dekade
#define ETSDIV_5DEKADE_VALUE     0x00020000     // Wert entspricht dem 5-fachen einer Dekade
#define ETSDIV_2DEKADE_VALUE     0x00040000     // Wert entspricht dem 2-fachen einer Dekade
#define ETSDIV_CLEAR_DEKADE      0x000f0000     // Löschflag

#define ETSDIV_OCTAVE_VALUE      0x00100000     // Wert entspricht einer Oktave
#define ETSDIV_THIRD_VALUE       0x00200000     // Wert entspricht einer Terz
#define ETSDIV_CLEAR_OKT_THI     0x00f00000     // Löschflag
#define ETSDIV_POLAR_FROM_INDEX  0x01000000     // Sinus und Cosinus-Werte aus einem Index
#define ETSDIV_POLAR_FROM_VALUE  0x02000000     // Sinus und Cosinus-Werte aus einem Winkelwert

#define LOG10_2        0.301029995664
#define LOG10_3        0.47712125472
#define INVLOG10_2     3.32192809489

class CEtsDiv
{
   public:
      CEtsDiv();
      void  Destructor();
      static bool  CalculateLinDivision(ETSDIV_DIV *, double&, double&);
      static bool  CalculateLogDivision(ETSDIV_DIV *, double&, double&);
      static bool  CalculateFrqDivision(ETSDIV_DIV *, double&, double&);
      static bool  NumberToText(ETSDIV_NUTOTXT*, char*);
      static ULONG Teiler(ULONG, int nt=0);
      static int   TestFrqValue(double);
      static int   TestLogValue(double);
      static int   Round(double);
      static int   DrawScale(ETSDIV_SCALE*);
      static bool  RoundMinMaxValues(ETSDIV_DIV*);
      static int   GetSineCosineOfAngle(int, DWORD, double, double&, double&);
      static bool  NumberRepresentationDlg(ETSDIV_NUTOTXT*, HWND);
      static bool  UnitConvertDlg(ETSDIV_NUTOTXT*, HWND);
      static double TextToNumber(char*);
};

/****************************************************************************************
* Name   : CalculateLinDivision                                                         *
* Zweck  : Berechnet eine lineare Aufteilung eines Wertebereiches                       *
* Aufruf : CalculateLinDivision(pdiv, fstep, fstart);                                   *
* pdiv     (EA): Struktur mit Aufteilungsparametern                        (ETSDIV_DIV) *
* fstep     (A): normierte Schrittweite in Weltkoordinaten                    (double&) *
* fstart    (A): Anfangspunkt fuer die Darstellung in Weltkoordinaten         (double&) *
* Funktonswert: true bei Erfolg, sonst false                                            *
****************************************************************************************/

/****************************************************************************************
* Name   : CalculateFrquDivision                                                        *
* Zweck  : Berechnet eine logarithmische Aufteilung eines Wertebereiches                *
* Aufruf : CalculateFrquDivision(pdiv, fstep, fstart);                                  *
* Parameter:                                                                            *
* pdiv     (EA): Struktur mit Aufteilungsparametern                        (ETSDIV_DIV) *
* fstep     (A): 10´er Logarithmus der normierten Schrittweite in             (double&) *
*                Weltkoordinaten                                                        *
* fstart    (A): Anfangspunkt fuer die Darstellung 10´er Logarithmus von      (double&) *
*                Weltkoordinaten                                                        *
* Funktonswert: true bei Erfolg, sonst false                                            *
****************************************************************************************/

/****************************************************************************************
* Name   : CalculateLogDivision                                                         *
* Zweck  : Berechnet eine logarithmische Aufteilung eines Wertebereiches                *
* Aufruf : CalculateLogDivision(pdiv, fstep, fstart);                                   *
* Parameter:                                                                            *
* pdiv     (EA): Struktur mit Aufteilungsparametern                        (ETSDIV_DIV) *
* Ausgabeparameter:                                                                     *
* fstep     (A): normierte Schrittweite in Weltkoordinaten                    (double&) *
* fstart    (A): Anfangspunkt fuer die Darstellung in Weltkoordinaten         (double&) *
* Funktonswert: true bei Erfolg, sonst false                                            *
****************************************************************************************/

/****************************************************************************************
* Name  : NumberToText                                                                  *
* Zweck : Wandelt eine Floatzahl in einen String                                        *
* Aufruf: NumberToText(lpntx, pszText);                                                 *
* Parameter :                                                                           *
* pntx   (E): Zeiger auf die Struktur ETSDIV_NUTOTXT                  (ETSDIV_NUTOTXT*) *
* pszText(A): Ausgabetext                                                       (char*) *
* Funktionswert : true, false                                                    (bool) *
****************************************************************************************/

/****************************************************************************************
* Name   : Teiler                                                                       *
* Zweck  : liefert aus einer positiven Zahlenmenge die am nächsten liegenden Potenzen   *
*          von 2 oder vielfache von 3 fuer die Aufteilung in Terz, bzw. Oktav-Schritte  *
* Aufruf : Teiler(lt, nt);                                                              *
* Parameter :                                                                           *
* lt    (E) : Eingabewert                                                       (ULONG) *
* nt    (E) : gewünschte Teilung 0 : egal, 2 : Oktav, 3 : Terzen                (int)   *
* Funktionswert : s.o.                                                          (ULONG) *
****************************************************************************************/

/****************************************************************************************
* Name   : TestFrqValue                                                                 *
* Zweck  : Liefert die die Teilungsabstufung, aus der dieser Frequenzwert stammt.       *
* Aufruf : TestValue(value);                                                            *
* Parameter :                                                                           *
* value  (E):  Frequenzwert                                                    (double) *
* Funktionswert : Teilungsabstufung.                                              (int) *
* Für Oktav-Teilung liefert sie 2, für Terzteilung 3.                                   *
****************************************************************************************/

/****************************************************************************************
* Name   : TestLogValue                                                                 *
* Zweck  : Testet den logarithmischen Wert auf Übereinstimmung mit der Schrittweite     *
*          1, 2, 5                                                                      *
* Aufruf : TestLogValue(value);                                                         *
* Parameter :                                                                           *
* value (E) : Testwert                                                         (double) *
* Funktionswert : Übereinstimmung mit der 1, 2, 5 Teilung.                        (int) *
* (0, 1, 2, 5), 0 : Bei keiner Übereinstimmung                                          *
****************************************************************************************/

#endif      // _CCARAINFOH_

