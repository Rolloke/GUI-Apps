/********************************************************************/
/*                                                                  */
/* CARADiv.CPP für CARADiv.DLL                                      */
/* für CARA2DVIEW-und CARA3DVIEW Programmodule                      */
/*                                                                  */
/* Berechnung der Aufteilung von linearen, logarithmischen und      */
/* frequenzbezogenen Gitternetzen. Ausgabe von Zahlen in Zeichen-   */
/* ketten in wissenschaftlicher und auch Standard Darstellung.      */
/*                                                                  */
/*     programmed by Rolf Ehlers                                    */
/*                                                                  */
/*     Version 1.0            03.09.1999                            */
/*                                                                  */
/********************************************************************/

#define  STRICT

#include <WINDOWS.H>
#include <cmath>
#include <float.h>
#include "CustErr.h"
#include "resource.h"
#include <locale.h>

#define COMPILE_MULTIMON_STUBS
#include <multimon.h>

#ifndef ASSERT
   #define ASSERT
#endif
#pragma warning (disable:4996)

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
   void            *pFktParam;      // zusätzliche Parameter für die Ausgabefunktion
};

struct ETSDIV_CONVERT_DLG
{
   ETSDIV_NUTOTXT *pNtx;
   HWND            hwndEdit;
   HANDLE          hBoldFont;
   double          dFactor;
   int             nRound;
   int             nRoundYard;
   int             nRoundFeet;
   int             nRoundInch;
};

#define ETSDIV_LINEAR      1
#define ETSDIV_LOGARITHMIC 2
#define ETSDIV_FREQUENCY   3
#define ETSDIV_POLAR       4

#define ETSDIV_NM_STD            1              // Standard Zahlendarstellung 1.234
#define ETSDIV_NM_SCI            2              // Exponentdarstellung 1.234e-5
#define ETSDIV_NM_ENG            3              // Exponentdarstellung 1.234e3 mit durch 3 teilbarem Exponenten

#define ETSDIV_NM_NO_END_NULLS   0x0001         // Keine Nullen am Ende 
#define ETSDIV_NM_GROUPING       0x0002         // Guppierung der Zahlen vor dem Komma
#define ETSDIV_NM_LOCALE         0x0004         // Lokale einstellungen verwenden

#define ETSDIV_NM_AUTO_STD       0x0100         // Automatisch auf STD Modus umstellen
#define ETSDIV_NM_AUTO_SCI       0x0200         // Automatisch auf SCI Modus umstellen
#define ETSDIV_NM_AUTO_ENG       0x0400         // Automatisch auf ENG Modus umstellen

#define ETSDIV_UC_CTRL_INIT      0x0010         // Das EditCtrl initialisiert die Dialogfelder
#define ETSDIV_UC_CTRL_REINIT    0x0020         // Das EditCtrl wird wieder initialisiert
#define ETSDIV_UC_CTRL_SIGNED    0x0040         // Das EditCtrl wird wieder initialisiert

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

#define WM_DATA_EXCHANGE  (WM_APP + 1)

#define LOG10_2        0.30102999566398
#define LOG10_3        0.47712125471966
#define INVLOG10_2     3.32192809488736
#define M_PI           3.1415926535898
#define M_1_D_PI       0.3183098861837
#define M_180_D_PI    57.2957795130823
#define M_PI_D_180     0.0174532925199
#define M_200_D_PI    63.6619772367581
#define M_PI_D_200     0.0157079632679

#define UC_TEXTLEN    32

HINSTANCE           g_hInstance = NULL;                        // globaler Instance-Handle
static const double g_epsilon   = 1e-10;                       // globale Variable für die Genauigkeit der TestFunktionen
double             *g_pdSinus   = NULL;                        // Feld mit Sinuswerten
double             *g_pdCosinus = NULL;                        // Feld mit Cosinuswerten
int                 g_nValues   = 0;                           // Anzahl der Sinus und Cosinuswerte
HANDLE              g_hMyHeap   = NULL;                        // Handle auf privaten Heap
int                 g_nAllocCounter = 0;                       // Zähler für Heapinstanzen
double              g_dPI = M_PI;                              // Feld mit Cosinuswerten
bool                g_bInitLocale = false;

// Prototypen
extern "C"
{
   void   Destructor(void*);
   bool   CalculateLinDivision(ETSDIV_DIV *, double&, double&);
   bool   CalculateLogDivision(ETSDIV_DIV *, double&, double&);
   bool   CalculateFrqDivision(ETSDIV_DIV *, double&, double&);
   bool   NumberToText(ETSDIV_NUTOTXT*, char*);
   ULONG  Teiler(ULONG, int nt=0);
   int    TestFrqValue(double);
   int    TestLogValue(double);
   int    Round(double);
   int    DrawScale(ETSDIV_SCALE*);
   bool   RoundMinMaxValues(ETSDIV_DIV*pDiv);
   int    GetSineCosineOfAngle(int, DWORD, double, double&, double&);
   bool   NumberRepDlg(ETSDIV_NUTOTXT*, HWND);
   bool   UnitConvertDlg(ETSDIV_NUTOTXT*, HWND);
   double TextToNumber(char *);
}

void * __cdecl operator new(unsigned int);

void __cdecl operator delete( void *);

void DeleteSineAndCosineValues();
int  CalculateSineAndCosineValues(int, DWORD, double);
BOOL CALLBACK NumRepDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK UnitConvertDlgProc(HWND, UINT, WPARAM, LPARAM);
void InitLocale();

// Funktionen
void  Destructor(void*)
{
   DeleteSineAndCosineValues();
}


/****************************************************************************************
* Name   : CalculateLinDivision                                                         *
* Zweck  : Berechnet eine lineare Aufteilung eines Wertebereiches                       *
* Aufruf : CalculateLinDivision(pdiv, dStep, dStart);                                   *
* pdiv     (EA): Struktur mit Aufteilungsparametern                         (ETSDIV_DIV)*
* dStep     (A): normierte Schrittweite in Weltkoordinaten                    (double&) *
* dStart    (A): Anfangspunkt fuer die Darstellung in Weltkoordinaten         (double&) *
* Funktonswert: true bei Erfolg, sonst false                                            *
****************************************************************************************/
bool CalculateLinDivision(ETSDIV_DIV *pdiv, double& dStep, double& dStart)
{
   static const double range[] = {0.875, 1.5, 2.25, 3.25, 4.5, 6.25, 8.75, 15.0},// Schwellwerte
                       value[] = {     1.0, 2.0,  2.5,  4.0, 5.0,  7.5, 10.0   };// Schrittweite in Weltkoordinaten
   bool   bNegative = false;
   int    steps, i, exp;
   long   ldiff;
   double mant=0, dDiff, dNorm;
   UINT nOldFp = _controlfp(_PC_64, _MCW_PC);

   ldiff = pdiv->l2 - pdiv->l1;                                // Mögliche Anzahl der Schritte ermitteln
   if (ldiff <= 0) {SetLastError(ETSDIV_ERROR_DISTANCE); return false;}// Strecke darf nicht negativ oder 0 sein
   if (pdiv->stepwidth > 0) steps = Round((double)ldiff / (double)pdiv->stepwidth);
   else                     steps = 0;
   if (steps==0)   {SetLastError(ETSDIV_ERROR_STEPS); return false;}

   dDiff = pdiv->f2 - pdiv->f1;                                // Schrittweite in Weltkoordinaten ermitteln
   if (dDiff == 0.0) {SetLastError(ETSDIV_ERROR_RANGE); return false;}
   if (dDiff <  0.0) {dDiff = -dDiff; bNegative = true;};
   dNorm = dDiff / steps;

   exp = (int) log10(dNorm);                                   // Schrittweite normieren auf den Bereich 1.0,..,10.0
   dNorm = dNorm * pow(10.0, (double)-exp);
   if (dNorm < 1.0)
   {
      dNorm*=10;
      exp--;
   }
   pdiv->dZero = pow(10.0, (double)exp-8);
   for (i=0; i<7; i++)                                         // Schrittweite auf die Werte 10 * 10^exp normieren
   {
      if ((dNorm > range[i])&&(dNorm <= range[i+1]))            
      {
         mant = value[i];
         break;
      }
   }

   dStep = mant * pow((double)10, (double)exp);                                // echte Schrittweite berechnen
   if (dStep == 0.0) {SetLastError(ETSDIV_ERROR_STEPWIDTH); return false;}
   pdiv->division = Round(dDiff / dStep);                      // Anzahl der Schritte ermitteln
   if (bNegative)
   {
      dStart = ceil(pdiv->f1 / dStep);                         // Anfangspunkt der Gittereinteilung ermitteln
      if (dStart <= 0.0) dStart-=1.0;
      dStart *= dStep;
      dStep = -dStep;
   }
   else
   {
      dStart = floor(pdiv->f1 / dStep);                        // Anfangspunkt der Gittereinteilung ermitteln
      if (dStart >= 0.0) dStart+=1.0;
      dStart *= dStep;
   }
   if (pdiv->division == 0) {SetLastError(ETSDIV_ERROR_NUMSTEPS); return false;}
   pdiv->width  = ldiff / steps;                                // Schrittweite in logischen Koordinaten ermitteln
   pdiv->divfnc = ETSDIV_LINEAR;

   _controlfp(nOldFp, _MCW_PC);
   return true;
}

/****************************************************************************************
* Name   : CalculateFrquDivision                                                        *
* Zweck  : Berechnet eine logarithmische Aufteilung eines Wertebereiches                *
* Aufruf : CalculateFrquDivision(pdiv, dStep, dStart);                                  *
* Parameter:                                                                            *
* pdiv     (EA): Struktur mit Aufteilungsparametern                         (ETSDIV_DIV)*
* dStep     (A): 10´er Logarithmus der normierten Schrittweite in             (double&) *
*                Weltkoordinaten                                                        *
* dStart    (A): Anfangspunkt fuer die Darstellung 10´er Logarithmus von      (double&) *
*                Weltkoordinaten                                                        *
* Funktonswert: true bei Erfolg, sonst false                                            *
****************************************************************************************/
bool CalculateFrqDivision(ETSDIV_DIV *pdiv, double& dStep, double& dStart)
{
   bool   bNegative = false;
   long   temp, ldiff;
   double fteiler, ftemp, dDiff;
   UINT   nOldFp = _controlfp(_PC_64, _MCW_PC);

   ldiff = pdiv->l2 - pdiv->l1;                                // Mögliche Anzahl der Schritte ermitteln
   if (ldiff <= 0) {SetLastError(ETSDIV_ERROR_DISTANCE); return false;}// Strecke darf nicht negativ oder 0 sein
   if (pdiv->stepwidth < 0) temp = 0;
   else                     temp = ldiff / pdiv->stepwidth;
   if (temp == 0)  {SetLastError(ETSDIV_ERROR_STEPS); return false;}// Anzahl = 0: keine Aufteilung

   dDiff   =  (pdiv->f2 - pdiv->f1);
   if (dDiff == 0) {SetLastError(ETSDIV_ERROR_RANGE); return false;}
   if (dDiff <  0) {dDiff = -dDiff; bNegative = true;}
   fteiler = temp * LOG10_2 / dDiff;                           // Frequenzteilung ermitteln
   ASSERT(fteiler > 0.0);
   if (1.0 < fteiler)                                          // Oktaven und feinere Teilungen
   {                                                           
      if (fteiler > 2.0e9)                                     // Integer Wertebereich überschritten
      {
         dStep = floor(fteiler / 3.0);                         // Terzteilung in float berechnen
         dStep = LOG10_2 / (dStep*3.0);
      }
      else                                                     // Oktaven für (div = 1,2,4,8,16,...), Terzen für (div = 3,9,12,15,...)
      {
         pdiv->division = Teiler(Round(fteiler));              // günstigste Schrittweite ermitteln: f(div) = f0 * pow(2, 1/div);
         dStep  = LOG10_2 / (double)pdiv->division;            // Schrittweite Logarithmieren dStep = log(2)/div;
      }
   }
   else                                                        // gröber als Oktaven
   {
      ftemp = 0.2 + 1.0/fteiler;                               // Invertieren
      pdiv->division = Round(ftemp);                           // runden
      dStep  = LOG10_2 * (double)pdiv->division;               // Schrittweite Logarithmieren dStep = log(2)*div
   }

   temp        = (int) (dDiff / dStep);
   if (temp<=0)  {SetLastError(ETSDIV_ERROR_STEPWIDTH); return false;}
   pdiv->width = ldiff / temp;                                 // Schrittweite in logischen Koordinaten ermitteln

   if (bNegative)
   {
      temp    = (int) (pdiv->f1 * INVLOG10_2);                 // Startwert orientiert sich an Oktavteilung
      dStart  = 1.0 + LOG10_2 * (double)temp;                  // diese beginnt bei 10 Hz
      fteiler = dStart - pdiv->f1;                             // bei kleinerer Teilung
      dDiff   = floor(fteiler / dStep);                        // Startwert um die Anzahl der kleineren Schritte
      dStart  = dStart - dStep * dDiff;                        // vorverlegen
      dStep   = -dStep;
   }
   else
   {
      temp    = (int) (pdiv->f1 * INVLOG10_2);                 // Startwert orientiert sich an Oktavteilung
      dStart  = 1.0 + LOG10_2 * (double)temp;                  // diese beginnt bei 10 Hz
      fteiler = dStart - pdiv->f1;                             // bei kleinerer Teilung
      dDiff   = floor(fteiler / dStep);                        // Startwert um die Anzahl der kleineren Schritte
      dStart  = dStart - dStep * dDiff;                        // vorverlegen
   }
   pdiv->divfnc = ETSDIV_FREQUENCY;
   _controlfp(nOldFp, _MCW_PC);
   return true;
}

/***************************************************************************************
* Name   : CalculateLogDivision                                                        *
* Zweck  : Berechnet eine logarithmische Aufteilung eines Wertebereiches                *
* Aufruf : CalculateLogDivision(pdiv, dStep, dStart);                                  *
* Parameter:                                                                           *
* pdiv     (EA): Struktur mit Aufteilungsparametern                          (ETSDIV_DIV)*
* Ausgabeparameter:                                                                    *
* dStep     (A): normierte Schrittweite in Weltkoordinaten                    (double&)*
* dStart    (A): Anfangspunkt fuer die Darstellung in Weltkoordinaten         (double&)*
* Funktonswert: true bei Erfolg, sonst false                                           *
***************************************************************************************/
bool CalculateLogDivision(ETSDIV_DIV *pdiv, double& dStep, double& dStart)
{ 
   bool   bNegative = false;
   long   nsteps, nstart, ndiff;
   double dDiff;
   UINT   nOldFp = _controlfp(_PC_64, _MCW_PC);

   ndiff  = pdiv->l2 - pdiv->l1;
   if (ndiff <= 0) {SetLastError(ETSDIV_ERROR_DISTANCE); return false;}// Strecke darf nicht negativ oder 0 sein
   if (pdiv->stepwidth > 0) nsteps = ndiff / pdiv->stepwidth; // Anzahl der Schritte über den Darstellungsbereich berechnen
   else                     nsteps = 0;
   if (nsteps==0)  {SetLastError(ETSDIV_ERROR_STEPS); return false;}// Anzahl = 0 : keine Aufteilung

   dDiff       = pdiv->f2 - pdiv->f1;                          // Anzahl der Dekaden bestimmen
   if (dDiff <  0) {dDiff = -dDiff; bNegative = true;}
   ndiff       = (long)((double)ndiff / dDiff);                // Schrittweite pro Dekade
   pdiv->width = MulDiv(ndiff, 3, 10);                         // kleinste Schrittweite ermitteln von z.B.: log(2) - log(1) ca. 0.3
   
   nstart = (int) pdiv->f1;                                    // ganzzahligen Exponenten bestimmen
   dStart = pow((double)10, (double)nstart);                           // Startpunkt beginnt bei einer 10'er Potenz
   dDiff  = pow((double)10, (double)pdiv->f1);                         // echten Startpunkt bestimmen
   dDiff  = dStart - dDiff;                                    // Vorlauf berechnen
   dStep  = dStart * 0.1;                                      // Schrittweite eine niedrigere 10'er Potenz einstellen

   if (dDiff > dStep)                                          // sind Schritte davor
   {
      nsteps = (int) (dDiff / dStep);                          // neuen Startwert berechnen
      if (nsteps)
      {
         if (!bNegative) nsteps++;
         dStart = dStart - dStep * (double)nsteps;
      }
   }
   else                                                        // sonst hat die Schrittweite dieselbe 10'er Potenz
   {
      dStep *= 10.0;
      nsteps = (int) (dDiff / dStep);                          // neuen Startwert berechnen
      if (nsteps)
      {
         if (bNegative) nsteps--;
         dStart = dStart - dStep * (double)nsteps;
      }
   }
   if (bNegative) dStep  = -dStep;

   pdiv->divfnc = ETSDIV_LOGARITHMIC;
   _controlfp(nOldFp, _MCW_PC);
   return true;
}

/*
   nstart = (int) pdiv->f1;                                    // ganzzahligen Exponenten bestimmen
   dStart = pow((double)10, (double)nstart);                   // Startpunkt beginnt bei einer 10'er Potenz
   dDiff  = pow((double)10, (double)pdiv->f1);                 // echten Startpunkt bestimmen
   dDiff  = dStart - dDiff;                                    // Vorlauf berechnen

   dStep  = dStart * 0.1;                                      // Schrittweite eine niedrigere 10'er Potenz einstellen
   if (dDiff > dStep)                                          // sind Schritte davor
   {
      nsteps = (int) (dDiff / dStep);                          // neuen Startwert berechnen
      dStart = dStart - dStep * (double)nsteps;
   }
   else                                                        // sonst hat die Schrittweite dieselbe 10'er Potenz
   {
      dStep *= 10.0;
   }
*/

/************************************************************************
* Name  : NumberToText                                                  *
* Zweck : Wandelt eine Floatzahl in einen String                        *
* Aufruf: NumberToText(lpntx);                                          *
* Parameter :                                                           *
* pntx   (E): Zeiger auf die Struktur ETSDIV_NUTOTXT  (ETSDIV_NUTOTXT*) *
* pszText(A): Ausgabetext                                   (char*)     *
* Funktionswert : true, false                               (bool)      *
************************************************************************/
bool NumberToText(ETSDIV_NUTOTXT *pntx, char *pszText)
{
   int          i, j=0, k=0, dec=0, sign=0, point, ngroup0 = 0, ngroup1 = 0;
   int          nround = pntx->nround;
   WORD         nmode  = LOWORD(pntx->nmode);                  // werden lokal angelegt, damit sie in der Struktur nicht verändert werden
   WORD         nExt   = HIWORD(pntx->nmode);
   bool         bNull  = true;
   char *       str=NULL;
   char         szLocale[8], szDecimal[4], szThousend[4];
   div_t        d3;

   if (!g_bInitLocale) InitLocale();
   if (nExt & ETSDIV_NM_LOCALE)
   {
      if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IDIGITS, szLocale, 4)!=0)
         nround = atoi(szLocale);
   }

   if (nExt & ETSDIV_NM_GROUPING)
   {
      if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING , szLocale, 8)!=0)
      {
         ngroup0 = szLocale[0] - '0';
         ngroup1 = szLocale[2] - '0';
         if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, szThousend, 4)==0)
            szDecimal[0] = '.';
      }
   }

   if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDecimal, 4)==0)
      szDecimal[0] = ',';

   if (pntx->nmaxl < 3) {SetLastError(ETSDIV_ERROR_TEXTLEN); return false;}

   if ((nmode!=ETSDIV_NM_STD) && (nExt & ETSDIV_NM_AUTO_STD))
   {
      double dVal = fabs(pntx->fvalue);
      if (nmode==ETSDIV_NM_ENG)
      {
         if ((dVal > 1.0) && (dVal < 1000.0)) nmode = ETSDIV_NM_STD;
      }
      else if (nmode==ETSDIV_NM_SCI)
      {
         if ((dVal > 1.0) && (dVal < 10.0)) nmode = ETSDIV_NM_STD;
      }
   }

   if ((nmode==ETSDIV_NM_STD) && (nExt & (ETSDIV_NM_AUTO_SCI|ETSDIV_NM_AUTO_ENG)))
   {
      double dVal = pow((double)10, (double)-nround);
      if (fabs(pntx->fvalue) < dVal)
      {
         if (nExt&ETSDIV_NM_AUTO_SCI) nmode = ETSDIV_NM_SCI;   // Exponentielle Darstellung einschalten
         else                         nmode = ETSDIV_NM_ENG;
      }
   }
   
   if (nmode==ETSDIV_NM_STD)
   {
      str = fcvt(pntx->fvalue, nround, &dec, &sign);
   }
   
   if (nmode!=ETSDIV_NM_STD)                                   // exponentielle Darstellung
   {
      str = ecvt(pntx->fvalue, nround + 1, &dec, &sign);
   }

   if (sign) pszText[k++]= '-';                                // Minuszeichen einfuegen

   if (nmode==ETSDIV_NM_STD && str)                            // Standard Anzeige ohne Exponent
   {                                                           // nround = Anzahl der Stellen hinter dem Komma
      bool bComma = false;
      if (dec>0)                                               // Zahlen größer als |1|
      {
         point = dec;
         if (sign) point++;
         for (; ((k < pntx->nmaxl) && (str[j] != 0)); k++)
         {
            if (k==point) pszText[k]=szDecimal[0], bComma=true;// Dezimalpunkt setzen
            else                                               // Zahlen vor bzw. hinter dem Dezimalpunkt
            {
               if (((ngroup0!=0) || (ngroup1!=0)) && (j!=0))
               {
                  int nPrecoma = point - k;
                  if (nPrecoma > 0)
                  {
                     if (ngroup1 != 0)
                     {
                        nPrecoma -= ngroup0;
                        if (nPrecoma == 0)
                        {
                           pszText[k++] = szThousend[0];
                           point++;
                           ngroup0 = ngroup1 = 0;
                        }
                        else if ((nPrecoma % ngroup1) == 0)
                        {
                           pszText[k++] = szThousend[0];
                           point++;
                        }
                     }
                     else if ((ngroup0 != 0) && ((nPrecoma % ngroup0) == 0))
                     {
                        pszText[k++] = szThousend[0];
                        point++;
                     }
                  }
               }
               if (str[j] != '0') bNull = false;
               pszText[k]=str[j++];
            }
         }
      }
      else                                                     // Zahlen kleiner |1|
      {
         bComma=true;
         pszText[k++] = '0', pszText[k++] = szDecimal[0];
         for (; ((k < pntx->nmaxl) && (str[j] != 0)); k++)
         {
            if (dec)   pszText[k] = '0', dec++;
            else
            {
               if (str[j] != '0') bNull = false;
               pszText[k] = str[j++];
            }
         }
      }
      if ((nExt & ETSDIV_NM_NO_END_NULLS) && bComma)
      {
         while ((k>1) && pszText[k-1] == '0')
            k--;
         pszText[k] = 0;
      }
      if (pszText[k-1]==szDecimal[0]) pszText[k-1] = 0;
      else                   pszText[k  ] = 0;
   }
   else if (nmode==ETSDIV_NM_SCI)                               // immer eine Zahl vor dem Komma z.B.: 1.0e-4, 1e5
   {                                                            // nround = Anzahl Stellen hinter dem Komma
      bNull = (fabs(pntx->fvalue) < pntx->dZero) ? true : false;
      pszText[k++] = str[j++];
      if (nround) pszText[k++] = szDecimal[0];
      for (i=0; i<nround; i++)
      {
         pszText[k++]=str[j++];
      }
      if (nExt & ETSDIV_NM_NO_END_NULLS)
      {
         while ((k>1) && ((pszText[k-1] == '0')||(pszText[k-1] == szDecimal[0])))
            k--;
      }
      pszText[k++] = 'e';
      itoa(dec-1, &pszText[k], 10);
   }
   else if (nmode==ETSDIV_NM_ENG)                               // eine, zwei oder drei Zahlen vor dem Komma, Exponent in 3´er Schritten
   {                                                            // nround = Anzahl Stellen hinter dem Komma
      int nPoint = 1;
      bNull = (fabs(pntx->fvalue) < pntx->dZero) ? true : false;
      d3 = div(dec, 3);
      if      (!d3.rem)    d3.rem = 3         , d3.quot--;
      else if (d3.rem < 0) d3.rem = 3 + d3.rem, d3.quot--;
      if (d3.rem != 1) str = ecvt(pntx->fvalue, nround + d3.rem, &dec, &sign);
      for (i=0; i<d3.rem; i++)
      {
         pszText[k++] = str[j++];
      }
      if (nround) nPoint = k, pszText[k++] = szDecimal[0];
      for (i=0; i<nround; i++)
      {
         pszText[k++] = str[j++];
      }
      if (nExt & ETSDIV_NM_NO_END_NULLS)
      {
         while ((k>nPoint) && ((pszText[k-1] == '0')||(pszText[k-1] == szDecimal[0])))
            k--;
         if ((d3.quot == 0) && !bNull)
         {
            pszText[k] = 0;
            return true;
         }
      }
      pszText[k++] = 'e';
      itoa(d3.quot*3, &pszText[k], 10);
   }
   else {SetLastError(ETSDIV_ERROR_MODE); return false;}
   if (bNull)
   {
      strcpy(pszText, "0");
   }
   return true;
}

/****************************************************************************************
* Name   : Teiler                                                                       *
* Zweck  : liefert aus einer positiven Zahlenmenge die am nächsten liegenden Potenzen   *
*          von 2 oder vielfache von 3 fuer die Aufteilung in Terz, bzw. Oktav-Schritte  *
* Aufruf : Teiler(lt);                                                                  *
* Parameter :                                                                           *
* lt    (E) : Eingabewert                                                       (ULONG) *
* nt    (E) : gewünschte Teilung 0 : egal, 2 : Oktav, 3 : Terzen                (int)   *
* Funktionswert : s.o.                                                          (ULONG) *
****************************************************************************************/
ULONG Teiler(ULONG lt, int nt)
{
   div_t        v3;
   ULONG        test;
   
   if (lt < 5)                  return lt;
   
   if ((nt==2)||(nt==0)&&((lt%2)==0))
   {
      for (test = 0x08; test <= lt; test<<=1)
      {
         if (test == lt)        return lt;
         if ((test - lt) < 2)   return test;
      }
   }
   v3 = div(lt, 3);
   if (!v3.rem)                   return lt;
   else if ((v3.rem==2)||(nt==3)) return (v3.quot + 1) * 3;
   else                           return  v3.quot      * 3;
}


/***************************************************************************************
* Name   : TestFrqValue                                                                *
* Zweck  : Liefert die die Teilungsabstufung, aus der dieser Frequenzwert stammt.      *
* Aufruf : TestValue(value);                                                           *
* Parameter :                                                                          *
* value  (E):  Frequenzwert                                                  (double)  *
* Funktionswert : Teilungsabstufung.                                            (int)  *
* Für Oktav-Teilung liefert sie 2, für Terzteilung 3.                                  *
***************************************************************************************/
int TestFrqValue(double fvalue)
{
   double test;
      
   fvalue -= 1.0;                                              // Veschiebung der Oktavwerte um die Anfangsfrequenz 10 Hz

   test  = fvalue * INVLOG10_2;                                // auf Oktaven prüfen
   test -= Round(test);
   if (fabs(test) < g_epsilon) return 2;

   test = 3.0 * fvalue * INVLOG10_2;                           // auf Terteilung prüfen
   test -= Round(test);
   if (fabs(test) < g_epsilon) return 3;

   test = 9.0 * fvalue * INVLOG10_2;                           // auf 3-tel Terteilung prüfen
   test -= Round(test);
   if (fabs(test) < g_epsilon) return 9;
   
   return 0;                                                   // sonst nichts
}

/***************************************************************************************
* Name   : TestLogValue                                                                *
* Zweck  : Testet den logarithmischen Wert auf Übereinstimmung mit der Schrittweite    *
*          1, 2, 5                                                                     *
* Aufruf : TestLogValue(value);                                                        *
* Parameter :                                                                          *
* value (E) : Testwert                                                       (double)  *
* Funktionswert : Übereinstimmung mit der 1, 2, 5 Teilung.                      (int)  *
* (0, 1, 2, 5), 0 : Bei keiner Übereinstimmung                                         *
***************************************************************************************/
int TestLogValue(double fvalue)
{
   double ftest;
   fvalue = fabs(fmod(fvalue, 1.0));

   if (fvalue < g_epsilon)  return 1;
   ftest = fabs(fvalue - 1.0);
   if (ftest  < g_epsilon)  return 1;

   ftest = fabs(fvalue - LOG10_2);
   if (ftest  < g_epsilon)  return 2;

   ftest = fabs(fvalue -(1-LOG10_2));
   if (ftest  < g_epsilon)  return 5;
   return 0;
}

int Round(double dval)
{
   int temp = (int) dval;
   dval -= temp;
   if (dval >=  0.5) temp++;
   if (dval <= -0.5) temp--;
   return temp;
   //return (int)(dval+0.5);
}

int DrawScale(ETSDIV_SCALE*pScale)
{
   UINT nOldFp         = _controlfp(_PC_64, _MCW_PC);
   bool bTextAllocated = false;
   int  n              = 0;
   try
   {
      if ((pScale== NULL) || (pScale->pFkt == NULL))  throw (DWORD)ETSDIV_ERROR_POINTER;

      if (pScale->dwFlags & ETSDIV_FORMAT_TEXT)
      {
         if (pScale->ntx.nmaxl <= 3)                  throw (DWORD)ETSDIV_ERROR_TEXTLEN;
         if (pScale->pszText == NULL)
         {
            pScale->pszText = new char[pScale->ntx.nmaxl];     // Allocieren des Textes
            if (pScale->pszText) bTextAllocated  = true;
            else                                      throw (DWORD)ETSDIV_ERROR_POINTER;
         }
         if (pScale->ntx.nmode == 0) pScale->ntx.nmode = ETSDIV_NM_STD;
      }

      if (pScale->dwFlags & ETSDIV_LOG_WORLD_RANGE)
      {
         if (pScale->div.divfnc == ETSDIV_POLAR)      throw (DWORD)ETSDIV_ERROR_RANGE;
         if (pScale->div.divfnc == ETSDIV_LINEAR)              // braucht lineare Koordinaten
         {
            pScale->div.f1 = pow((double)10, (double)pScale->div.f1);
            pScale->div.f2 = pow((double)10, (double)pScale->div.f2);
            pScale->dwFlags &= ~ETSDIV_LOG_WORLD_RANGE;
         }
      }
      else
      {
         if (pScale->div.divfnc != ETSDIV_LINEAR)              // braucht logarithmische Koordinaten
         {
            pScale->div.f1 = log10(pScale->div.f1);
            pScale->div.f2 = log10(pScale->div.f2);
            pScale->dwFlags |= ETSDIV_LOG_WORLD_RANGE;
         }
      }

      switch(pScale->div.divfnc)
      {
/*******************************************************************************************************/
         case ETSDIV_LINEAR:                                   // lineare Aufteilung
         {
            double dStep, dStart;

            if (!CalculateLinDivision(&pScale->div, dStep, dStart)) throw (DWORD)0;
            bool bNegative = (dStep < 0.0);

            if (pScale->dwFlags & ETSDIV_CALC_ZEROVALUE)                         // automatische Berechnung des Nullwertes
            {
               pScale->ntx.dZero = pScale->div.dZero;
            }

            if (pScale->dwFlags & ETSDIV_CALC_GRADIENT)        // Automatische Berechnung der Geradengleichung
            {                                                  // für Positionsberechnung
               pScale->dGradient = (double)(pScale->div.l2 - pScale->div.l1) / (pScale->div.f2 - pScale->div.f1);
               pScale->dOrigin   = (double) pScale->div.l2 - pScale->div.f2*pScale->dGradient;
            }

            for (pScale->ntx.fvalue = dStart; ;pScale->ntx.fvalue+=dStep, n++)
            {
               if (bNegative)                                  // Abbruchbedingung der Schleife
               {  if (pScale->ntx.fvalue <= pScale->div.f2) break;}
               else
               {  if (pScale->ntx.fvalue >= pScale->div.f2) break;}

               if (pScale->dwFlags & (ETSDIV_CALC_POS))        // Berechnung der Position
               {
                  double dPos = (int)(pScale->dGradient * pScale->ntx.fvalue + pScale->dOrigin);
                  if (pScale->dwFlags & ETSDIV_CALC_X_POS) pScale->dPosX = dPos;
                  if (pScale->dwFlags & ETSDIV_CALC_Y_POS) pScale->dPosY = dPos;
               }

               if (pScale->dwFlags & ETSDIV_FORMAT_TEXT)       // Formatierung des Textes
               {
                  if (!NumberToText(&pScale->ntx, pScale->pszText)) throw (DWORD)0;
               }

               if (!pScale->pFkt(pScale)) break;               // Aufruf der Ausgabefunktion
            }
         } break;
/*******************************************************************************************************/
         case ETSDIV_LOGARITHMIC:                              // logartihmische Aufteilung
         if (fabs(pScale->div.f2 - pScale->div.f1) > 0.8)      // wenn der Bereich geeignet ist
         {
            double dStep, dStart, dEnd,                        // lineare Werte
                   dLogValue;                                  // logarithmischer Wert

            if (!CalculateLogDivision(&pScale->div, dStep, dStart)) throw (DWORD)0;
            bool bNegative = (dStep < 0.0);

            dEnd = pow((double)10, (double)pScale->div.f2);                    // Abbruchbedingung berechnen

            if (pScale->dwFlags & ETSDIV_CALC_GRADIENT)        // Automatische Berechnung der Geradengleichung
            {                                                  // für Positionsberechnung
               pScale->dGradient = (double)(pScale->div.l2 - pScale->div.l1) / (pScale->div.f2 - pScale->div.f1);
               pScale->dOrigin   = (double) pScale->div.l2 - pScale->div.f2*pScale->dGradient;
            }

            for (pScale->ntx.fvalue = dStart; ;pScale->ntx.fvalue+=dStep, n++)
            {
               if (bNegative)                                  // Abbruchbedingung
               {  if (pScale->ntx.fvalue < dEnd) break;}
               else
               {  if (pScale->ntx.fvalue > dEnd) break;}

               dLogValue = log10(pScale->ntx.fvalue);          // logarithmischen Wert berechnen
               int nTest = TestLogValue(dLogValue);            // Art des Wertes bestimmen
               pScale->dwFlags &= ~ETSDIV_CLEAR_DEKADE;
               if      (nTest == 1)                            // Dekaden, Schrittweite erhöhen
               {
                  if (bNegative) dStep *=  0.1;
                  else
                  {
                     if (dStep < pScale->ntx.fvalue)
                        dStep *= 10.0;
                  }
                  pScale->dwFlags |= ETSDIV_DEKADE_VALUE;
               }
               else if (nTest == 5) pScale->dwFlags |= ETSDIV_2DEKADE_VALUE; // Dekaden * 2
               else if (nTest == 2) pScale->dwFlags |= ETSDIV_5DEKADE_VALUE; // Dekaden * 5

               if (pScale->dwFlags & ETSDIV_CALC_POS)          // Berechnung der Position
               {
                  double dPos = (int)(pScale->dGradient * dLogValue + pScale->dOrigin);
                  if (pScale->dwFlags & ETSDIV_CALC_X_POS) pScale->dPosX = dPos;
                  if (pScale->dwFlags & ETSDIV_CALC_Y_POS) pScale->dPosY = dPos;
               }

               if (pScale->dwFlags & ETSDIV_FORMAT_TEXT)       // Formatierung des Textes
               {
                  if (!NumberToText(&pScale->ntx, pScale->pszText)) throw (DWORD)0;
               }

               if (!pScale->pFkt(pScale)) break;               // Aufruf der Ausgabefunktion
            }
            break;
         }
         else if ((pScale->dwFlags & ETSDIV_AUTO_FREQUENCY) == 0)// Automatisch auf Frequenzteilung umschalten ?
         {
            break;
         }
/*******************************************************************************************************/
         case ETSDIV_FREQUENCY:                                // Frequenzaufteilung
         {
            double dStep, dStart, dValue;                      // logarithmische Werte

            if (!CalculateFrqDivision(&pScale->div, dStep, dStart)) throw (DWORD)0;
            bool bNegative = (dStep < 0.0);

            if (pScale->dwFlags & ETSDIV_CALC_GRADIENT)        // Automatische Berechnung der Geradengleichung
            {                                                  // für Positionsberechnung
               pScale->dGradient = (double)(pScale->div.l2 - pScale->div.l1) / (pScale->div.f2 - pScale->div.f1);
               pScale->dOrigin   = (double) pScale->div.l2 - pScale->div.f2*pScale->dGradient;
            }

            for (dValue = dStart; ;dValue+=dStep, n++)
            {
               pScale->ntx.fvalue = pow((double)10, dValue);           // linearen Wert berechnen
                                                 
               if (bNegative)                                  // Abbruchbedingung der Schleife
               {  if (dValue <= pScale->div.f2) break;}
               else
               {  if (dValue >= pScale->div.f2) break;}

               if (pScale->dwFlags & ETSDIV_CALC_POS)          // Berechnung der Position
               {
                  double dPos = (int)(pScale->dGradient * dValue + pScale->dOrigin);
                  if (pScale->dwFlags & ETSDIV_CALC_X_POS) pScale->dPosX = dPos;
                  if (pScale->dwFlags & ETSDIV_CALC_Y_POS) pScale->dPosY = dPos;
               }

               if (pScale->dwFlags & ETSDIV_CALC_OCT_AND_THI)  // Berechnung der Oktav- und Terz-flags
               {
                  int nTest = TestFrqValue(dValue);            // Art des Wertes bestimmen
                  pScale->dwFlags &= ~ETSDIV_CLEAR_OKT_THI;
                  if (nTest == 2) pScale->dwFlags |= ETSDIV_OCTAVE_VALUE;
                  if (nTest == 3) pScale->dwFlags |= ETSDIV_THIRD_VALUE;
               }

               if (pScale->dwFlags & ETSDIV_FORMAT_TEXT)       // Formatierung des Textes
               {
                  if (!NumberToText(&pScale->ntx, pScale->pszText)) throw (DWORD)0;
               }

               if (!pScale->pFkt(pScale)) break;               // Aufruf der Ausgabefunktion
            }
         } break;
/*******************************************************************************************************/
         case ETSDIV_POLAR:                                    // Kreisaufteilung
         {

         } break;
         default: throw (DWORD)ETSDIV_ERROR_DIV_FKT;
      }
   }
   catch(DWORD dwError)
   {
      if (dwError != 0)
         SetLastError(dwError);
      n = 0;
   }

   if (bTextAllocated)                                         // Text wieder freigeben
   {
      delete[] pScale->pszText;
      pScale->pszText = NULL;
   }

   _controlfp(nOldFp, _MCW_PC);                                // Floating Point Precision
   return n;
}

bool RoundMinMaxValues(ETSDIV_DIV*pDiv)
{
   UINT   nOldFp = _controlfp(_PC_64, _MCW_PC);
   bool bOk = false;
   try
   {
      double dStart, dStep, dEnd;
      if (pDiv == NULL)         throw (DWORD) ETSDIV_ERROR_POINTER;
      if (pDiv->f1 > pDiv->f2)  throw (DWORD) ETSDIV_ERROR_RANGE;
      if (pDiv->divfnc == 0)    throw (DWORD) ETSDIV_ERROR_DIV_FKT;
      switch(pDiv->divfnc)
      {
/*******************************************************************************************************/
         case ETSDIV_LINEAR:                                   // lineare Aufteilung
         {                                                     // Schrittweite bestimmen
            pDiv->l1        = 0;
            pDiv->l2        = pDiv->stepwidth;
            pDiv->stepwidth = 1;
            if (!CalculateLinDivision(pDiv, dStep, dStart)) throw (DWORD)0;
            if (dStep < 1e-14)
            {
               dStep = 0.01;
               throw (DWORD)0;
            }

            for ( ;dStart > pDiv->f1; dStart -= dStep);        // Min-Wert bestimmen
            pDiv->f1 = dStart;

            dStart = floor((pDiv->f2 - pDiv->f1) / dStep);     // Anzahl der Schritte zum Max-Wert
            dEnd = pDiv->f1 + dStep * dStart;                  // zum Endwert springen

            for ( ;dEnd < pDiv->f2; dEnd += dStep);            // Max-Wert bestimmen
            pDiv->f2 = dEnd;
            bOk  = true;
         } break;
/*******************************************************************************************************/
         case ETSDIV_LOGARITHMIC:                              // logartihmische Aufteilung
         {
            if ((pDiv->f1 <= 0) || (pDiv->f2 <= 0)) throw (DWORD) ETSDIV_ERROR_RANGE;
            pDiv->f1 = log10(pDiv->f1);
            pDiv->f2 = log10(pDiv->f2);
            pDiv->l1        = 0;
            pDiv->l2        = 10;
            pDiv->stepwidth = 1;
            if (!CalculateLogDivision(pDiv, dStep, dStart)) throw (DWORD)0;
            dEnd = dStart;
            dStart = pDiv->f1;
                     pDiv->f1 = pDiv->f2;
                                pDiv->f2 = dStart;
            if (!CalculateLogDivision(pDiv, dStep, dStart)) throw (DWORD)0;
            pDiv->f1 = dEnd;
            pDiv->f2 = dStart;
            bOk  = true;
         } break;
/*******************************************************************************************************/
         case ETSDIV_FREQUENCY:                                // Frequenzaufteilung
         {
            int nTest,
                nOctave =  2,
                nThird  = -1,
                n3Third = -1;
            if ((pDiv->f1 <= 0) || (pDiv->f2 <= 0)) throw (DWORD) ETSDIV_ERROR_RANGE;
            pDiv->f1 = log10(pDiv->f1);
            pDiv->f2 = log10(pDiv->f2);
            pDiv->l1        =  0;
            pDiv->l2        = 2+(int)((pDiv->f2 - pDiv->f1) * (double)pDiv->stepwidth / LOG10_2);
            if (pDiv->stepwidth == 3) nThird = 3;
            if (pDiv->stepwidth == 9) nThird = 3, n3Third = 9;
            pDiv->stepwidth =  1;
            if (!CalculateFrqDivision(pDiv, dStep, dStart)) throw (DWORD)0;
            dEnd = dStart;
            for ( ; ; dStart -= dStep)
            {
               if (dStart < pDiv->f1)
               {
                  nTest = TestFrqValue(dStart);
                  if (nTest == nOctave) break;
                  if (nTest == nThird ) break;
                  if (nTest == n3Third) break;
               }
            }
            pDiv->l1 = nTest;
            pDiv->f1 = pow((double)10, dStart);

            for ( ; ; dEnd += dStep)
            {
               if (dEnd > pDiv->f2)
               {
                  nTest = TestFrqValue(dEnd);
                  if (nTest == nOctave) break;
                  if (nTest == nThird ) break;
                  if (nTest == n3Third) break;
               }
            }
            pDiv->l2 = nTest;
            pDiv->f2 = pow((double)10, dEnd);
            bOk = true;
         } break;
         default: throw (DWORD) ETSDIV_ERROR_DIV_FKT;
      }
   }
   catch (DWORD dwError)
   {
      if (dwError != 0)
         SetLastError(dwError);
   }
   _controlfp(nOldFp, _MCW_PC);
   return bOk;
}

int CalculateSineAndCosineValues(int nIndex, DWORD dwFlags, double dAngleIn)
{
   double  dStep, dAngle = 0;
   if (dwFlags & ETSDIV_POLAR_FROM_VALUE)                   // berechnen in gegebener Schrittweite
   {
      if (dAngleIn > 0) 
      {
         g_nValues = (int)(2*M_PI / dAngleIn);              // Anzahl der Werte berechnen
      }
      if ((g_nValues > 0) && (g_nValues < 3601))            // minimal 0.1° Schritte
      {
         dStep = dAngleIn;                                  // Schrittweite aus dAngle
      }
      else                                                  // Fehler Default Werte nehmen
      {
         nIndex = 0;                                        // Fehler :
         dwFlags |= ETSDIV_POLAR_FROM_INDEX;
      }
   }
   else if (dwFlags & ETSDIV_POLAR_FROM_INDEX)              // Anzahl der Werte als Index
   {
      if ((nIndex > 0) && (nIndex < 3601))
      {
         g_nValues = nIndex;
      }
      else g_nValues = 361;                                 // 1° Schritte
      dStep = 2*M_PI/(double)(g_nValues);
   }
   else
   {
      g_nValues = 361;                                      // 1° Schritte
      dStep = 2*M_PI/(double)(g_nValues);
   }

   g_pdSinus   = new double[g_nValues];                     // Felder anlegen
   g_pdCosinus = new double[g_nValues];
   if ((!g_pdSinus) || (!g_pdCosinus))                      // Fehler
   {
      DeleteSineAndCosineValues();                          // Werte löschen
      SetLastError(ETSDIV_ERROR_NO_ARRAY);
      return -1;
   }

   __asm                                                    // Berechnung in einer Assembler Schleife
   {
   	PUSHA                                                 ; Register retten
      FLD   dAngle                                          ; S:|dAngle|
      MOV   eax, dword ptr g_pdCosinus                      ; Adresse fuer Cosinus
      MOV   ebx, dword ptr g_pdSinus                        ; Adresse fuer Sinus
      MOV   ecx, g_nValues                                  ; Anzahl der Durchläufe
schleife:
         FLD ST                                             ; S:|dAngle|dAngle|
         FSINCOS                                            ; S:|cos|sin|dAngle|
         FSTP   qword ptr [eax]                             ; S:|sin|dAngle|
         FSTP   qword ptr [ebx]                             ; S:|dAngle|
         ADD    eax, 8                                      ; Pointer erhöhen
         ADD    ebx, 8                                      ; Pointer erhöhen
         FADD   dStep                                       ; S:|dAngle+dStep|
         LOOP   schleife                                    ; Schleife
      FSTSW  ax                                             ; BugFix für einige 487 Copros
      POPA                                                  ; Register wiederherstellen
   }
   return g_nValues;
}

void DeleteSineAndCosineValues()
{
   if (g_pdSinus)   delete[] g_pdSinus;
   if (g_pdCosinus) delete[] g_pdCosinus;
   g_pdSinus   = NULL;
   g_pdCosinus = NULL;
   g_nValues   = 0;
}

int GetSineCosineOfAngle(int nIndex, DWORD dwFlags, double dAngleIn, double &dSineOut, double &dCosineOut)
{
   if (dwFlags & ETSDIV_POLAR_FROM_INDEX)                      // gespeicherten Wert zurückgeben
   {
      if ((nIndex >= 0) && (nIndex < g_nValues))               // Index Bereich prüfen
      {
         dSineOut   = g_pdSinus[nIndex];
         dCosineOut = g_pdCosinus[nIndex];
         return nIndex;
      }
      else if (g_nValues == 0)
      {
         if (CalculateSineAndCosineValues(0, 0, 0) != -1)
         {
            return GetSineCosineOfAngle(nIndex, dwFlags, dAngleIn, dSineOut, dCosineOut);
         }
         SetLastError(ETSDIV_ERROR_RANGE);
         return -1;
      }
      else
      {
         SetLastError(ETSDIV_ERROR_RANGE);
         return -1;
      }
   }

   if (dwFlags & ETSDIV_POLAR_GRAD)                            // Winkelwert in Grad
   {
      dAngleIn *= M_PI_D_180;
   }
   else if (dwFlags & ETSDIV_POLAR_GON)                        // Winkelwert in Neugrad
   {
      dAngleIn *= M_PI_D_200;
   }

   if (dwFlags & ETSDIV_CALC_POLAR_VALUE)                      // nur einen Winkel berechnen
   {
      double dSin, dCos;
      __asm
      {
   	   PUSHA                                                 ; Register retten
         FLD      dAngleIn                                     ; Winkel laden
         FSINCOS                                               ; cos und sinus berechnen
         FSTSW  ax                                             ; BugFix für einige 487 Copros
         FSTP     dCos                                         ; cosinuswert ausgeben
         FSTP     dSin                                         ; sinuswert ausgeben
         POPA                                                  ; Register wiederherstellen
      }
      dSineOut   = dSin;
      dCosineOut = dCos;
      return 1;
   }

   if (dwFlags & ETSDIV_CALC_POLAR_VALUES)                     // sollen die Arrays neu berechnet werden
   {
      DeleteSineAndCosineValues();
      CalculateSineAndCosineValues(nIndex, dwFlags, dAngleIn);
   }
   else                                                        // Sinus und Cosinus Wert ermitteln
   {
      if (!g_nValues) CalculateSineAndCosineValues(0, 0, 0);   // Arrays noch nicht initialisiert ?
   }


   if (dwFlags & ETSDIV_POLAR_FROM_VALUE)                      // Winkelwert aus dem Array holen
   {
      nIndex = Round(dAngleIn * 0.5 * M_1_D_PI * (double)g_nValues);// Index ermitteln
      if (nIndex >= g_nValues) nIndex %= g_nValues;
      if ((nIndex >= 0) && (nIndex < g_nValues))               // Index Bereich prüfen
      {
         dSineOut   = g_pdSinus[nIndex];
         dCosineOut = g_pdCosinus[nIndex];
         return nIndex;
      }
      else
      {
         SetLastError(ETSDIV_ERROR_RANGE);
         return -1;
      }
   }

   return g_nValues;
}

bool UnitConvertDlg(ETSDIV_NUTOTXT *pNtx, HWND hwndParent)
{
   ETSDIV_CONVERT_DLG ucd;
   ucd.pNtx      = pNtx;
   ucd.hwndEdit  = hwndParent;
   ucd.hBoldFont = NULL;
   ucd.dFactor   = 1.0;

   if (DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_LENGTH_UNIT_CONVERT_DLG),
             hwndParent, (DLGPROC) UnitConvertDlgProc, (LPARAM) &ucd)==IDOK)
   {
      return true;
   }
   return false;
}

BOOL CALLBACK UnitConvertDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//   static const LOGFONT lfa={-12,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEVICE_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,FF_MODERN,"Arial"};
   static const char *szUnit[] = 
   {
      "mm", // 0 -> 1e-3
      "cm", // 1 -> 1e-2
      "dm", // 2 -> 1e-1
      "km"  // 3 -> 1e+3
   };
   switch(uMsg)
   {
      case WM_INITDIALOG:
      if (lParam)
      {
         BOOL bReturn = false;
         int  width;
         int  height;
         int  maxwidth;
         int  maxheight;
         int  minx;
         int  miny;
         const char *pszUnit = NULL;
         POINT ptPos;
         ETSDIV_CONVERT_DLG* pucd = (ETSDIV_CONVERT_DLG*)lParam;

         ::SetWindowLong(hwndDlg, DWL_USER, (LONG)lParam);

         pucd->pNtx->nmaxl  = UC_TEXTLEN;
         pucd->hBoldFont = (HFONT)::SendDlgItemMessage(hwndDlg, IDC_UNIT_T_MM, WM_GETFONT, 0, 0);
         LOGFONT lf;
         ::GetObject(pucd->hBoldFont, sizeof(LOGFONT), &lf);
         lf.lfWeight = FW_BOLD;
         pucd->hBoldFont = ::CreateFontIndirect(&lf);
         if(pucd->hBoldFont)
         {
            ::SendDlgItemMessage(hwndDlg, IDC_UNIT_T_MM, WM_SETFONT,(WPARAM) pucd->hBoldFont, 0);
         }
         switch(pucd->pNtx->nround)
         {
            case -3:             // mm
               pszUnit          = szUnit[0];
               pucd->dFactor    = 0.001;
               pucd->nRound     = 1;
               pucd->nRoundYard = 5;
               pucd->nRoundFeet = 5;
               pucd->nRoundInch = 3;
               break;
            case -2:             // cm
               pszUnit          = szUnit[1];
               pucd->dFactor    = 0.01;
               pucd->nRound     = 2;
               pucd->nRoundYard = 4;
               pucd->nRoundFeet = 4;
               pucd->nRoundInch = 2;
               break;
            case -1:             // dm
               pszUnit          = szUnit[2];
               pucd->dFactor    =    0.1;
               pucd->nRound     = 3;
               pucd->nRoundYard = 3;
               pucd->nRoundFeet = 3;
               pucd->nRoundInch = 3;
               break;
            case  3:             // km
               pszUnit          = szUnit[3];
               pucd->dFactor    = 1000.0;
               pucd->nRound     = 2;
               pucd->nRoundYard = 2;
               pucd->nRoundFeet = 2;
               pucd->nRoundInch = 1;
               break;
            default:             // m
               pucd->nRound     = 3;
               pucd->nRoundYard = 3;
               pucd->nRoundFeet = 3;
               pucd->nRoundInch = 2;
         }
         if (pszUnit)
         {
            ::SetWindowText(GetDlgItem(hwndDlg, IDC_UNIT_T_MM), pszUnit);
         }

         if (HIWORD(pucd->pNtx->nmode) & ETSDIV_UC_CTRL_INIT)
         {
            char szText[UC_TEXTLEN];
            ::GetWindowText(pucd->hwndEdit, szText, UC_TEXTLEN);
            pucd->pNtx->fvalue = TextToNumber(szText);
            long lStyle = ::GetWindowLong(pucd->hwndEdit, GWL_STYLE);
            if (lStyle&ES_READONLY)
            {
               lStyle = ::GetWindowLong(GetDlgItem(hwndDlg, IDC_UNIT_E_MM), GWL_STYLE)|ES_READONLY;
               ::SetWindowLong(GetDlgItem(hwndDlg, IDC_UNIT_E_MM), GWL_STYLE, lStyle);
               ::SetWindowLong(GetDlgItem(hwndDlg, IDC_UNIT_E_INCH), GWL_STYLE, lStyle);
               ::SetWindowLong(GetDlgItem(hwndDlg, IDC_UNIT_E_FEET), GWL_STYLE, lStyle);
               ::SetWindowLong(GetDlgItem(hwndDlg, IDC_UNIT_E_YARD), GWL_STYLE, lStyle);
            }
         }

         ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 1, (LPARAM)&bReturn);

         {                                             // get Workarea mit alter API !
            RECT rcWork;                               // gilt nur für den ersten Monitor
            SystemParametersInfo(SPI_GETWORKAREA,0,&rcWork,0);

            maxwidth  = rcWork.right;
            maxheight = rcWork.bottom;
            minx      = rcWork.left;
            miny      = rcWork.top;
         }

         {                                             // Dialogboxgrösse besorgen
            RECT rc;
            ::GetWindowRect(pucd->hwndEdit, &rc);
            ptPos.x = rc.left;
            ptPos.y = rc.top;
            GetWindowRect(hwndDlg, &rc);
            width  = rc.right - rc.left;
            height = rc.bottom - rc.top;
         }

         {                                             // Monitorinformationen finden
            HMONITOR    hMonitor = MonitorFromPoint(ptPos, MONITOR_DEFAULTTONEAREST);

            if(hMonitor)                               // der Unterschied hir zu ETSHELP ist, das hier die Workarea verwendet werden muß
            {                                          // da das Dialogfeld nicht TOPMOST ist !
               MONITORINFO sMonInfo;

               sMonInfo.cbSize = sizeof(MONITORINFO);

               if(GetMonitorInfo(hMonitor,&sMonInfo))
               {
                  maxwidth  = sMonInfo.rcWork.right;
                  maxheight = sMonInfo.rcWork.bottom;
                  minx      = sMonInfo.rcWork.left;
                  miny      = sMonInfo.rcWork.top;
               }
            }
         }
                                                       // left,top clipping
         if(ptPos.x <= minx) ptPos.x = minx + 1;
         if(ptPos.y <= miny) ptPos.y = miny + 1; 
                                                       // right, bottom clipping
         if(ptPos.x+width >= maxwidth)
         {
            ptPos.x = maxwidth - width -1;
         }
         if(ptPos.y+height >= maxheight)
         {
            ptPos.y = maxheight - height - 1;
         }

         ::MoveWindow(hwndDlg, ptPos.x, ptPos.y, width, height, false);

         ::SendDlgItemMessage(hwndDlg, IDC_UNIT_E_MM  ,EM_LIMITTEXT,(WPARAM) 5,(LPARAM) 0);
         ::SendDlgItemMessage(hwndDlg, IDC_UNIT_E_INCH,EM_LIMITTEXT,(WPARAM) 7,(LPARAM) 0);
         ::SendDlgItemMessage(hwndDlg, IDC_UNIT_E_FEET,EM_LIMITTEXT,(WPARAM) 6,(LPARAM) 0);
         ::SendDlgItemMessage(hwndDlg, IDC_UNIT_E_YARD,EM_LIMITTEXT,(WPARAM) 6,(LPARAM) 0);

         ::SetFocus(::GetDlgItem(hwndDlg, IDC_UNIT_E_INCH));
//         ::SendDlgItemMessage(hwndDlg, IDC_UNIT_E_INCH,EM_SETSEL,0,-1); // alles im Editcontroll selectieren
         return bReturn;
      }
      break;
      case WM_DATA_EXCHANGE:
      {
         BOOL *pbReturn = (BOOL*)lParam;
         BOOL bSet      = wParam;
         char szText[UC_TEXTLEN];
         ETSDIV_CONVERT_DLG* pucd = (ETSDIV_CONVERT_DLG*) ::GetWindowLong(hwndDlg, DWL_USER);
         ETSDIV_NUTOTXT  Ntx  = *pucd->pNtx;
         if (bSet)
         {
            Ntx.nround = pucd->nRound;
            ::NumberToText(&Ntx, szText);
            ::SetDlgItemText(hwndDlg, IDC_UNIT_E_MM, szText);
            ::SendDlgItemMessage(hwndDlg, IDC_UNIT_E_MM, EM_SETMODIFY, 0, 0);

            Ntx.nround = pucd->nRoundInch;
            Ntx.fvalue = pucd->pNtx->fvalue * pucd->dFactor / 0.0254;  // Inch
            ::NumberToText(&Ntx, szText);
            ::SetDlgItemText(hwndDlg, IDC_UNIT_E_INCH, szText);
            ::SendDlgItemMessage(hwndDlg, IDC_UNIT_E_INCH, EM_SETMODIFY, 0, 0);

            Ntx.nround = pucd->nRoundFeet;
            Ntx.fvalue = pucd->pNtx->fvalue * pucd->dFactor / 0.3048; // Foot
            ::NumberToText(&Ntx, szText);
            ::SetDlgItemText(hwndDlg, IDC_UNIT_E_FEET, szText);
            ::SendDlgItemMessage(hwndDlg, IDC_UNIT_E_FEET, EM_SETMODIFY, 0, 0);

            Ntx.nround = pucd->nRoundYard;
            Ntx.fvalue /= 3.0;                                          // Yard = Foot / 3.0
            ::NumberToText(&Ntx, szText);
            ::SetDlgItemText(hwndDlg, IDC_UNIT_E_YARD, szText);
            ::SendDlgItemMessage(hwndDlg, IDC_UNIT_E_YARD, EM_SETMODIFY, 0, 0);
            *pbReturn = true;
         }
         else
         {
            if (::SendDlgItemMessage(hwndDlg, *pbReturn, EM_GETMODIFY, 0, 0))
            {
               ::GetDlgItemText(hwndDlg, *pbReturn, szText, UC_TEXTLEN);
               pucd->pNtx->fvalue = TextToNumber(szText);
               if (!(HIWORD(pucd->pNtx->nmode) & ETSDIV_UC_CTRL_SIGNED))
               {
                  if(pucd->pNtx->fvalue < 0) pucd->pNtx->fvalue = 0;
               }
            }
            else *pbReturn = 0;
            switch(*pbReturn)
            {
               case IDC_UNIT_E_INCH:
                    pucd->pNtx->fvalue *= 0.0254 / pucd->dFactor;
                    break;
               case IDC_UNIT_E_FEET:
                    pucd->pNtx->fvalue *= 0.3048 / pucd->dFactor;
                    break;
               case IDC_UNIT_E_YARD:
                    pucd->pNtx->fvalue *= 3 * 0.3048 / pucd->dFactor;
                    break;
               case IDC_UNIT_E_MM:
                    break;
               default:
                  *pbReturn = 0;
                  break;
            }
         }
         return 0;
      }
      case WM_COMMAND:
      {
         if (HIWORD(wParam) == EN_KILLFOCUS)
         {
            BOOL bReturn = LOWORD(wParam);
            ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 0, (LPARAM)&bReturn);
            if (bReturn)
            {
               bReturn = 0;
               ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 1, (LPARAM)&bReturn);
            }
            return true;
         }
         else
         {
            if ((LOWORD(wParam) == IDCANCEL)||(LOWORD(wParam) == IDOK))
            {
               ETSDIV_CONVERT_DLG* pucd = (ETSDIV_CONVERT_DLG*) ::GetWindowLong(hwndDlg, DWL_USER);
               if (pucd->hBoldFont) ::DeleteObject(pucd->hBoldFont);
               if ((LOWORD(wParam) == IDOK) && (HIWORD(pucd->pNtx->nmode) & ETSDIV_UC_CTRL_REINIT))
               {
                  BOOL bReturn = GetWindowLong(GetFocus(), GWL_ID);
                  ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 0, (LPARAM)&bReturn);
                  char szText[UC_TEXTLEN];
                  pucd->pNtx->nround = 4 + pucd->pNtx->nround;
                  ::NumberToText(pucd->pNtx, szText);
                  ::SetWindowText(pucd->hwndEdit, szText);
               }
               EndDialog(hwndDlg, LOWORD(wParam));
               return 1;
            }
         }
      }break;
      default:
      {

      } break;
   }
   return 0;
}

bool NumberRepDlg(ETSDIV_NUTOTXT*pntx, HWND hwndParent)
{
   ETSDIV_NUTOTXT NTX = *pntx;
   if (DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_NUMBER_REPRESENTATION_DLG),
             hwndParent, (DLGPROC) NumRepDlgProc, (LPARAM) &NTX)==IDOK)
    {
       pntx->nmode  = NTX.nmode;
       pntx->nround = NTX.nround;
       pntx->dZero  = NTX.dZero;
       return true;
    }
    return false;
}

BOOL CALLBACK NumRepDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch(uMsg)
   {
      case WM_INITDIALOG:
      if (lParam)
      {
         BOOL bReturn = false;
         ::SetWindowLong(hwndDlg, DWL_USER, (LONG)lParam);
         ETSDIV_NUTOTXT* pNtx = (ETSDIV_NUTOTXT*)lParam;
         pNtx->fvalue = 123456789012345;
         pNtx->nmaxl  = _MAX_PATH;
         ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 1, (LPARAM)&bReturn);
         return bReturn;
      }
      break;
      case WM_DATA_EXCHANGE:
      {
         BOOL *pbReturn = (BOOL*)lParam;
         BOOL bSet    = wParam;
         char szText[_MAX_PATH];
         ETSDIV_NUTOTXT* pNtx = (ETSDIV_NUTOTXT*)::GetWindowLong(hwndDlg, DWL_USER);
         ETSDIV_NUTOTXT  Ntx = *pNtx;
         Ntx.nround = 15;
         Ntx.nmode  = ETSDIV_NM_STD|(IDC_NR_NO_END_NULLS<<16);
         if (bSet)
         {
            ::SetDlgItemInt(hwndDlg, IDC_NR_EDT_NO_OF_COMMA, pNtx->nround, false);
            ::CheckDlgButton(hwndDlg, IDC_NR_LOCALE_SETTINGS, (HIWORD(pNtx->nmode) & ETSDIV_NM_LOCALE      ) ? BST_CHECKED : BST_UNCHECKED);
            ::CheckDlgButton(hwndDlg, IDC_NR_NO_END_NULLS   , (HIWORD(pNtx->nmode) & ETSDIV_NM_NO_END_NULLS) ? BST_CHECKED : BST_UNCHECKED);
            ::CheckDlgButton(hwndDlg, IDC_NR_GROUPING       , (HIWORD(pNtx->nmode) & ETSDIV_NM_GROUPING    ) ? BST_CHECKED : BST_UNCHECKED);
            ::NumberToText(&Ntx, szText);
            ::SetDlgItemText(hwndDlg, IDC_NR_EDT_INPUT, szText);
            ::SendDlgItemMessage(hwndDlg, IDC_NR_EDT_INPUT, EM_SETMODIFY, 0, 0);
            ::NumberToText(pNtx, szText);
            ::SetDlgItemText(hwndDlg, IDC_NR_EDT_OUTPUT, szText);
            ::CheckDlgButton(hwndDlg, IDC_NR_R_MODE0, (LOWORD(pNtx->nmode) == ETSDIV_NM_STD) ? BST_CHECKED : BST_UNCHECKED);
            ::CheckDlgButton(hwndDlg, IDC_NR_R_MODE1, (LOWORD(pNtx->nmode) == ETSDIV_NM_SCI) ? BST_CHECKED : BST_UNCHECKED);
            ::CheckDlgButton(hwndDlg, IDC_NR_R_MODE2, (LOWORD(pNtx->nmode) == ETSDIV_NM_ENG) ? BST_CHECKED : BST_UNCHECKED);
            ::EnableWindow(GetDlgItem(hwndDlg, IDC_NR_GROUPING)       , (LOWORD(pNtx->nmode) == ETSDIV_NM_STD)   ? true : false);
            ::EnableWindow(GetDlgItem(hwndDlg, IDC_NR_EDT_NO_OF_COMMA), (HIWORD(pNtx->nmode) & ETSDIV_NM_LOCALE) ? false : true);
            *pbReturn = true;
         }
         else
         {
            pNtx->nround = ::GetDlgItemInt(hwndDlg, IDC_NR_EDT_NO_OF_COMMA, pbReturn, false);
            if (pNtx->nround <  0) pNtx->nround = 0;
            if (pNtx->nround > 15) pNtx->nround = 15;
            int nMode = 0;
            if (::IsDlgButtonChecked(hwndDlg, IDC_NR_LOCALE_SETTINGS) == BST_CHECKED) nMode |= ETSDIV_NM_LOCALE;
            if (::IsDlgButtonChecked(hwndDlg, IDC_NR_NO_END_NULLS   ) == BST_CHECKED) nMode |= ETSDIV_NM_NO_END_NULLS;
            if (::IsDlgButtonChecked(hwndDlg, IDC_NR_GROUPING       ) == BST_CHECKED) nMode |= ETSDIV_NM_GROUPING;
            nMode <<= 16;
            if (::IsDlgButtonChecked(hwndDlg, IDC_NR_R_MODE0)) nMode |= ETSDIV_NM_STD;
            if (::IsDlgButtonChecked(hwndDlg, IDC_NR_R_MODE1)) nMode |= ETSDIV_NM_SCI;
            if (::IsDlgButtonChecked(hwndDlg, IDC_NR_R_MODE2)) nMode |= ETSDIV_NM_ENG;
            pNtx->nmode = nMode;
            if (*pbReturn)
            {
               if (::SendDlgItemMessage(hwndDlg, IDC_NR_EDT_INPUT, EM_GETMODIFY, 0, 0))
               {
                  if (::GetDlgItemText(hwndDlg, IDC_NR_EDT_INPUT, szText, _MAX_PATH) > 0)
                  {
                     pNtx->fvalue = strtod(szText, NULL);// &szText[_MAX_PATH-1]);
                  }
                  else *pbReturn = false;
               }
            }
         }
         return 0;
      }
      case WM_COMMAND:
      {
         switch(LOWORD(wParam))
         {
            case IDC_NR_EDT_NO_OF_COMMA:
            case IDC_NR_EDT_INPUT:
               if (HIWORD(wParam) == EN_KILLFOCUS)
               {
                  BOOL bReturn = false;
                  ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 0, (LPARAM)&bReturn);
                  if (bReturn)
                  {
                     ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 1, (LPARAM)&bReturn);
                  }
               }break;
            case IDC_NR_LOCALE_SETTINGS: case IDC_NR_NO_END_NULLS: case IDC_NR_GROUPING:
            case IDC_NR_R_MODE0:         case IDC_NR_R_MODE1:      case IDC_NR_R_MODE2:
               if (HIWORD(wParam) == BN_CLICKED)
               {
                  BOOL bReturn = false;
                  ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 0, (LPARAM)&bReturn);
                  if (bReturn)
                  {
                     ::SendMessage(hwndDlg, WM_DATA_EXCHANGE, 1, (LPARAM)&bReturn);
                  }
               }break;
            case IDCANCEL:
            {
               EndDialog(hwndDlg, LOWORD(wParam));
               return 1;
            }
            case IDOK:
            {
               HWND hwndFocus = ::GetFocus();
               if ((hwndFocus == ::GetDlgItem(hwndDlg, IDC_NR_EDT_INPUT)) ||
                   (hwndFocus == ::GetDlgItem(hwndDlg, IDC_NR_EDT_NO_OF_COMMA)))
               {
                  SetFocus(::GetDlgItem(hwndDlg, IDOK));
                  break;
               }
               EndDialog(hwndDlg, LOWORD(wParam));
               return 1;
            }
            default: break;
         }
      }
      default:
      {

      } break;
   }
   return 0;
}

double TextToNumber(char *psz)
{
   char   szDecimal[4];
   GetLocaleInfo(LOCALE_USER_DEFAULT,LOCALE_SDECIMAL, szDecimal, 4);

   if(szDecimal[0] != 46)
   {
      for(int i=0;i<lstrlen(psz);i++)
      {
         if(psz[i] == szDecimal[0]) {psz[i] = 46;break;}
      }
   }

   return atof(psz);
}

void InitLocale()
{
   char szLocale[256];
   ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, szLocale, 32);
//   char * str = setlocale(LC_ALL, szLocale);
   g_bInitLocale = true;
}

/********************************************************************/
/*                       DLL Main Funktion                          */
/********************************************************************/
bool   WINAPI DllMain(HINSTANCE hInstance,DWORD fReason,LPVOID /* pvReseved */)
{
   switch (fReason)
   {
      case DLL_PROCESS_ATTACH:                   // Einbinden der DLL in den Adressraum des Prozesses
      {
         DisableThreadLibraryCalls(hInstance);
         g_hInstance = hInstance;                // Instance-Handle global speichern
      }  break;

      case DLL_PROCESS_DETACH:                   // Die DLL wird freigegeben
      {
         int dec, sign;
         char * str = fcvt(0.5, 1, &dec, &sign);
         free(str);
//         str = setlocale(LC_ALL, NULL);
         if (g_hMyHeap)   HeapDestroy(g_hMyHeap);
         if (g_nAllocCounter != 0)
         {
            MessageBox( (HWND) NULL, "Memory leaks detected !",
                        "Fatal Error: HEAP", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
         }
         break;
      }
   }

   return true;
}

void * __cdecl operator new(unsigned int s)
{
   if (!g_hMyHeap)
   {
      g_hMyHeap = HeapCreate(0, 1048576, 0);  // priv. Heap anlegen
      g_nAllocCounter = 0;
      if (g_hMyHeap == NULL) return NULL;
   }
   void   *pointer = HeapAlloc(g_hMyHeap, HEAP_ZERO_MEMORY, s);
   if(pointer != NULL)
   {
      g_nAllocCounter++;
   }

   return pointer;
}

void __cdecl operator delete( void *s)
{
   if(HeapFree(g_hMyHeap, 0, s) != NULL ) 
   {
      g_nAllocCounter--;
   }
}
