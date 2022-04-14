// Plot.h: Schnittstelle für die Klasse CPlot.
// 31.07.98 Tho
//////////////////////////////////////////////////////////////////////

#ifndef __PLOT_H
#define __PLOT_H

#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

struct RANGE
{
  double x1, x2, y1, y2;                                      // Darzustellender Bereich in Weltkoordinaten
};

class CFileHeader;
class CCurve;

#define TARGET_FUNCTION_FIXED      9998
#define TARGET_FUNCTION_CHANGEABLE 9999

class CPlot  
{
public:
   CPlot();
   ~CPlot();

   // Zugriffsfunktionen für die enthaltenen Kurven
   bool     SetSize(int);
   int      GetNumCurves() const {return m_nNumCurves;};
   CCurve  *GetCurve(int n);

   // Weltkoordinatenbereich
   void     DeterminRange();
   void     SetDefaultRange(RANGE r) {m_DefaultRange = r;};
   void     SetDefaultRange(double x1, double x2, double y1, double y2);
   RANGE    GetDefaultRange()        {return m_DefaultRange;};
   void     SetXDivision(int nd);
   void     SetYDivision(int nd);
   // Parameter
   void     SetXNumMode(int nm);
   void     SetYNumMode(int nm);
   void     SetHeading(const char *s);
   void     SetXUnit(const char*);
   void     SetYUnit(const char*);
	const char* GetHeading() {return (const char*) m_pszHeading;};

   // CARAVIEW Dateifunktionen
   void     GetChecksum(CFileHeader *);
   bool     Read(HANDLE);
   void     Write(HANDLE);

private:
   CCurve  *m_pCurves;
   int      m_nNumCurves;
   RANGE    m_DefaultRange;
   char    *m_pszHeading;
};

// CPlot-Funktionsbeschreibungen
/******************************************************************************
* Name       : SetSize                                                        *
* Definition : bool SetSize(int n);                                           *
* Zweck      : Setzen der Anzahl der Kurven. Der Speicherplatz für die Kurven *
*              wird hier alloziert. Bei erneutem Setzen der Größe werden evtl.*
*              vorhandene Kurven gelöscht.                                    *
* Aufruf     : SetSize(n);                                                    *
* Parameter  :                                                                *
* n      (E) : neue Anzahl der Kurven                                (int)    *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
/******************************************************************************
* Name       : Write                                                          *
* Definition : void Write(HANDLE hfile);                                      *
* Zweck      : Schreiben der Daten von CPlot in eine Datei                    *
* Aufruf     : Write(hfile)                                                   *
* Parameter  :                                                                *
* hfile  (E) : gültiger Datei-Handle                                 (HANDLE) *
* Funktionswert keiner                                                        *
******************************************************************************/
/******************************************************************************
* Name       : Read                                                           *
* Definition : void Read(HANDLE hfile);                                       *
* Zweck      : Lesen von Plotdaten aus einer Datei und allozieren von         *
*              Speicherplatz.                                                 *
* Aufruf     : Read(hfile)                                                    *
* Parameter  :                                                                *
* hfile  (E) : gültiger Datei-Handle                                 (HANDLE) *
* Funktionswert keiner                                                        *
******************************************************************************/
/******************************************************************************
* Name       : GetChecksum                                                    *
* Definition : void GetChecksum(CFileHeader *pFH);                            *
* Zweck      : Berechnen der CRC-Checksumme aus allen Daten des CPlot-Objekts *
* Aufruf     : GetChecksum(pFH);                                              *
* Parameter  :                                                                *
* pFH    (E) : Zeiger auf CFileHeader-Objekt. Dieses Objekt wird als Dateikopf*
*              am Anfang der Datei gespeichert.                (CFileHeader*) *
* Funktionswert keiner                                                        *
******************************************************************************/
/******************************************************************************
* Name       : SetHeading                                                     *
* Definition : void SetHeading(const char *s);                                *
* Zweck      : Setzt die Überschrift für den Plot.                            *
* Aufruf     : SetHeading(s)                                                  *
* Parameter  :                                                                *
* s      (E) : Zeiger auf Überschrift                          (const char*)  *
* Funktionswert keiner                                                        *
******************************************************************************/
/******************************************************************************
* Name       : SetX- (Division, NumMode, Unit)                                *
               SetY- (Division, NumMode, Unit)                                *
* Definition : void SetXDivision(int nDiv);                                   *
*              void SetXNumMode(int nMode);                                   *
*              void SetXUnit(const char *szUnit);                             *
*              void SetYDivision(int nDiv);                                   *
*              void SetYNumMode(int nMode);                                   *
*              void SetYUnit(const char *szUnit);                             *
* Zweck      : Setzen der Parameter (Division, NumMode, Unit) für alle        *
*              enthaltenen Kurven jeweils für die X- bzw Y-Achse.             *
* Aufruf     : Funktions spezifisch !                                         *
* Parameter  (Funktions spezifisch !):                                        *
* nDiv   (E) : Aufteilung (CCURVE_POLAR, CCURVE_LINEAR,                       *
*                          CCURVE_LOGARITHMIC, CCURVE_FREQUENCY)     (int)    *
* nMode  (E) : Zahlendarstellung (NM_STD, NM_SCI, NM_ENG)            (int)    *
* szUnit (E) : Einheit                                         (const char*)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
/******************************************************************************
* Name       : DeterminRange                                                  *
* Definition : void DeterminRange();                                          *
* Zweck      : Ermitteln der Min bzw. Max-Werte für alle enthaltenen Kurven   *
* Aufruf     : DeterminRange();                                               *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
// Zusätzliche Funktionen
/***************************************************************************************
* Name       : Call2DView                                                              *
* Definition : bool Call2DView(const char*,int,CPlot*,bool,bool,HWND, char*,           *
                               DWORD*,HANDLE*);                                        *
* Zweck      : Aufrufen des Programms ETS2DVIEW mit Übergabe der Plot-Datei            *
* Aufruf     : Call2DView(pszProgrPath, nPlots, pPlots, bDeterminRange, bEdit, hWnd,   *
*                         pszFileName, pdwProcessID, phProcess);                       *
* Parameter  :                                                                         *
* pszProgrPath   (E): Pfad des Programms ETS2DView [optional]           (const char*)  *
* nPlots         (E): Anzahl der Plots die angezeigt bzw.editiert werden      (int)    *
* pPlots         (E): Array mit den übergebenen Plots                         (CPlot*) *
* bDeterminRange (E): Automatisches Ermitteln der Plotbereiche (true, false)  (bool)   *
* bEdit          (E): Editieren der Plots möglich (true, false)               (bool)   *
*                     Wird hier true übergeben, so werden die folgenden Parameter zum  *
*                     benötigt.                                                        *
* hWnd           (E): HANDLE des Hauptfensters des aufrufenden Programms.     (HWND)   *
*                     Dieser wird benötigt, damit ETS2DView eine Nachricht zurück-     *
*                     schicken kann, wenn das Editieren beendet wird.                  *
* nMsgID         (E): ID der Nachricht die zurückgeschickt wird.              (int)    *
* nParam         (E): Zusätzlicher Parameter, der an ETS2DView übergeben wird.(int)    *
*                     TARGET_FUNCTION_FIXED:      die Funktionen können nicht geändert *
                                                  werden.                              *
*                     TARGET_FUNCTION_CHANGEABLE: die Funktionen können geändert       *
*                                                 werden.                              *
* pszFileName    (A): Pfad und Name der Übergabedatei [optional].              (char*) *
*                     In diser stehen die editierten Daten, wenn die Nachricht (nMsgID)*
*                     zurückgesendet wird. Die Zeichenkette muß MAX_PATH*2 lang sein.  *
* pdwProcessID   (A): ID des erzeugten Prozesses von ETS2DView [optional].    (DWORD*) *
* phProcess      (A): HANDLE des erzeugten Prozesses von ETS2DView [optional].(HANDLE*)*
* Funktionswert : Aufruf erfolgt (true, false)                                (bool)   *
***************************************************************************************/
bool Call2DView(const char *, int, CPlot *, bool, bool, HWND, int, int, char *, PROCESS_INFORMATION*);

/******************************************************************************
* Name       : ReadPlots                                                      *
* Definition : CPlot *ReadPlots(const char *, int &, bool &);                 *
* Zweck      : Lesen und allozieren den Plots mit allen Kurven                *
* Aufruf     : ReadPlots(pszFileName, nPlots, bCRC_OK);                       *
* Parameter  :                                                                *
* pszFileName (E): Name und Pfad der zu lesenden Datei        (const char*)   *
* nPlots      (A): Anzahl der gelesenen Plots                       (int&)    *
* bCRC_OK     (A): Checksumme Ok                                    (bool&)   *
* Funktionswert: Array mit allozierten Plots                        (CPlots*) *
* Hinweis:       Die Plots müssen mit delete[] freigegeben werden !!          *
******************************************************************************/
CPlot *ReadPlots(const char *, int &, bool &);


#endif // !defined __PLOT_H
