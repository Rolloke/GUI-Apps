// Curve.h: Schnittstelle für die Klasse CCurve.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURVE_H__45DBF622_F7BC_11D0_9DB9_B051418EA04A__INCLUDED_)
#define AFX_CURVE_H__45DBF622_F7BC_11D0_9DB9_B051418EA04A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

struct BSParameter
{
   unsigned char xDivision    :2; // Teilung der x-Achse (CCURVE_POLAR, CCURVE_LINEAR, CCURVE_LOGARITHMIC, CCURVE_FREQUENCY)
   unsigned char yDivision    :2; // Teilung der y-Achse
   unsigned char xNumMode     :2; // Darstellung der x-Achsenzahlen (NM_STD, NM_SCI, NM_ENG)
   unsigned char yNumMode     :2; // Darstellung der y-Achsenzahlen
   unsigned char editable     :1; // Kurve editierbar
   unsigned char splinemode   :1; // Kurvenpunkte werden als quadratischer Spline editiert
   unsigned char sorted       :1; // Kurvenwerte sind nach X-Werten sortiert
   unsigned char xNoEndNulls  :1; // X keine 0-en am Ende
   unsigned char yNoEndNulls  :1; // Y keine 0-en am Ende
   unsigned char penchanged   :1; // der Zeichenstift wurde geändert
   unsigned char writeprotect :1; // Schreibgeschützte Kurve
   unsigned char interupted   :1; // Kurve ist unterbrochen
   unsigned char targetfunc   :4; // Zielfunktion Nr.: n
   unsigned char reference    :1; // Referenzkurve
   unsigned char sortable     :1; // Kurvenwerte sortierbar
   unsigned char format       :3; // Kurvenformat
   unsigned char angleformat  :2; // Winkelformat GRAD, GON, RAD
   unsigned char is_a_copy    :1; // Kurve ist eine Kopie
   unsigned char locus        :2; // Ortskurve
};

struct BSParameterEx
{
   DWORD scatter:1;
   DWORD scatterform:4;
   DWORD dummy:27;
};

/*
   Kurvenformate:
   Die Kurvenformate sind für verschiedene Anwendungsbereiche definiert worden.
   Das alte Kurvenformat ist standardmäßig eingestellt. Die neuen Kurvenformate
   müssen mit der Funktion "SetFormat(n)" eingestellt werden. Dies muß vor dem
   Aufrufen der Funktion "SetSize(n)" geschehen.
   Bei den Kurvenformaten mit den äquidistanten, aufsteigenden X-Werten, werden die 
   X-Werte aus einem Anfangswert und einer Schrittweite berechnet. Diese werden mit 
   den Funktionen "SetXOrigin(d)" und "SetXStep(d)" gesetzt.
   Die Berechnung der X-Werte orientiert sich weiterhin an der eingestellten Teilung
   der X-Achse :
   CCURVE_POLAR, CCURVE_LINEAR : dOrg + i * dStep       =>  Schrittweite linear
   CCURVE_LOGARITHMIC          : dOrg * 10^(i * dStep)  =>  Schrittweite an Dekaden orientiert
   CCURVE_FREQUENCY            : dOrg *  2^(i * dStep)  =>  Schrittweite an Oktaven orientiert
*/

struct SCurve
{
   double x, y;
};

struct SCurveFloat
{
   float x, y;
};

struct SCurveShort
{
   short x, y;
};

// Kurvenformate für beliebige X-Werte :
#define CF_DOUBLE           0    // altes Kurvenformat         : double Werte für die X und Y Achse
#define CF_FLOAT            1    // Kurvenformat float         : float Werte für die X und Y Achse
#define CF_SHORT            2    // Kurvenformat short         : short Werte für die X und Y Achse
// Kurvenformate für äquidistante, aufsteigende X-Werte :
#define CF_SINGLE_DOUBLE    3    // Kurvenformat single double : double Werte für die Y-Werte
#define CF_SINGLE_FLOAT     4    // Kurvenformat single float  : float Werte für die Y-Werte
#define CF_SINGLE_SHORT     5    // Kurvenformat single short  : short Werte für die Y-Werte
#define CF_ARBITRARY        6    // Kurvenformat für beliebige Arrays
#define CF_ARBITRARY_CONTR  7    // Kurvenformat für beliebige Arrays, die von der Kurve erzeugbar und löschbar sind

struct CF_GET_CURVE_PARAMS;       // Struktur für den Zugriff auf beliebige Arrays
typedef int (*GET_CRV_PARAM_FKT)(CF_GET_CURVE_PARAMS*);

#define CF_GET_SIZE         1
#define CF_GET_VALUE        2
#define CF_GET_XVALUE       3
#define CF_GET_YVALUE       4
#define CF_SET_VALUE        5
#define CF_SET_XVALUE       6
#define CF_SET_YVALUE       7
#define CF_GET_Y_VALUE      8
#define CF_SORT_XASC        9
#define CF_GET_X_MIN_STEP  10
#define CF_DELETE_VALUES   11
#define CF_SET_SIZE        12
#define CF_WRITE_FILE      13
#define CF_READ_FILE       14
#define CF_GET_FORMAT_EX   15
#define CF_COPY_CURVE      16
#define CF_COPY_VALUES     17

#define CF_CHANGE_TO_SINGLE -1   // Kurvenformat von double nach single ändern
#define CF_CHANGE_TO_DOUBLE -2   // Kurvenformat von single nach double ändern

#define CF_CHANGE_TO_DOUBLE_TYPE -3 // Kurvenformat double-Type ändern
#define CF_CHANGE_TO_FLOAT_TYPE  -4 // Kurvenformat float-Type ändern
#define CF_CHANGE_TO_SHORT_TYPE  -5 // Kurvenformat short-Type ändern

/*
   Achsenteilung
   Diese wird mit den Funktionen "SetXDivision(n)" und "SetYDivision(n)" gesetzt.
*/
#define CCURVE_POLAR        0    // Nur für die X-Achse eines Plots : Darstellung als Polardiagramm
#define CCURVE_LINEAR       1    // Lineare Achsenteilung
#define CCURVE_LOGARITHMIC  2    // Logarithmische Achsenteilung an Dekaden orientiert.
#define CCURVE_FREQUENCY    3    // Logarithmische Achsenteilung an Oktaven orientiert.

/*
   Winkelbereich:
   Bereiche für eine Vollkreis zur Berechnung Sinus und Cosinus-Werte für die 
   "X-Achse" in Polardarstellung
   Wird mit der Funktion "SetAngleFormat(n)" gesetzt.
*/
#define CCURVE_GRAD         0    // Neugrad X : (0,..,400)
#define CCURVE_GON          1    // Grad    X : (0,..,360)°
#define CCURVE_RAD          2    // Rad     x : (0,..,2*Pi)

/*
   Zahlendarstellung
   Für die Darstellung der Zahlen im Plot:
   Wird mit den Funktionen "SetXNumMode(n)" und "SetYNumMode(n)" gesetzt.
*/
#ifndef NM_STD
   #define NM_STD 1              // Standard Zahlendarstellung 1.0, 0.01 ohne Exponent
#endif
#ifndef NM_SCI
   #define NM_SCI 2              // Wissenschaftliche Zahlendarstellung mit Exponent 1.0e0, 1.0e-2, 
#endif
#ifndef NM_ENG
   #define NM_ENG 3              // Wissenschaftliche Zahlendarstellung mit Exponent 1.0e0, 10.0e-3, 
#endif                           // durch 3 teilbar.

class CFileHeader;

#ifdef CARA_MFC_HEADER
class AFX_EXT_CLASS CFloatPrecision
#else
class CFloatPrecision
#endif
{
public:
   CFloatPrecision();
   ~CFloatPrecision();
private:
   UINT m_nOldFp;
};

#ifdef CARA_MFC_HEADER
  class AFX_EXT_CLASS CCurve
#else
  class CCurve
#endif
{
public:
   CCurve();
   CCurve(CCurve *);
   virtual  ~CCurve();
   void     Destroy();
   // Datei -Funktionen
   void     GetChecksum(CFileHeader *);
   bool     Read(HANDLE);
   void     Write(HANDLE);
   // Zugriffsfunktionen auf Beschreibungstexte
   void     SetDescription(const TCHAR *s);
   TCHAR    *GetDescription() {return m_pszDescription;};
   void     SetXUnit(const TCHAR *);
   TCHAR    *GetXUnit() {return m_pszUnitx;};
   void     SetYUnit(const TCHAR *);
   TCHAR    *GetYUnit() {return m_pszUnity;};
   void     SetLocusUnit(const TCHAR *);
   TCHAR    *GetLocusUnit() {return m_pszUnitLocus;};
   // Zugriffsfunktionen auf Kurvenwerte
	bool     AttachArray(void*, int, int, bool bTestXSorted=false);
	void    *DetachArray();
	const void * GetArrayPointer();
   bool     SetSize(int);
	int      GetArraySize();
   int      GetNumValues() {return m_nNum;};
   double   GetX_Value(int);
   double&  SetX_Value(int);
   bool     SetX_Value(int, double);
   double   GetY_Value(int);
   double&  SetY_Value(int);
   bool     SetY_Value(int, double);
	SCurve   GetCurveValue(int);
	bool     SetCurveValue(int, SCurve&);
	bool     SetLocusValue(int, double);
	double   GetLocusValue(int);
	bool     IsValue(int);
   void     CopyValues(CCurve*);
   double   GetYMin();
   double   GetYMax();
   double   GetXMin();
   double   GetXMax();
   void     SetOffset(double dOf)   {m_dOffset = dOf;  };
   double   GetOffset()             {return m_dOffset; };
   void     SetXOrigin(double dOrg) {m_dOrg    = dOrg; };
   double   GetXOrigin()            {return m_dOrg;    };
   void     SetXStep(double dStep)  {m_dStep   = dStep;};
   double   GetXStep()              {return m_dStep;   };
   void     RobAllParameter(CCurve&);
   // Parameterfunktionen
   void     SetXDivision(int nd) {m_Parameter.xDivision = nd;};
   int      GetXDivision()       {return m_Parameter.xDivision;};
   void     SetYDivision(int nd) {m_Parameter.yDivision = nd;};
   int      GetYDivision()       {return m_Parameter.yDivision;};
   void     SetLocus(int);
   int      GetLocus()           {return m_Parameter.locus;};
   void     SetXNumMode(int nm)  {m_Parameter.xNumMode = nm;};
   int      GetXNumMode()        {return m_Parameter.xNumMode;};
   void     SetYNumMode(int nm)  {m_Parameter.yNumMode = nm;};
   int      GetYNumMode()        {return m_Parameter.yNumMode;};
   void     SetXNoEndNulls(int nm){m_Parameter.xNoEndNulls = nm;};
   int      NoXEndNulls()         {return m_Parameter.xNoEndNulls;};
   void     SetYNoEndNulls(int nm){m_Parameter.yNoEndNulls = nm;};
   int      NoYEndNulls()         {return m_Parameter.yNoEndNulls;};
   void     SetParameter(BSParameter p){m_Parameter =p;};
   BSParameter GetParameter()    {return m_Parameter;};
   bool     SetFormat(int);
   int      GetFormat()          {return m_Parameter.format;};
	bool     ChangeFormat(int nFormat);
   void     SetCrvParamFkt(GET_CRV_PARAM_FKT, void*);
   void     SetAngleFormat(int n){m_Parameter.angleformat = n;};
   int      GetAngleFormat()     {return m_Parameter.angleformat;};
   void     SetTargetFuncNo(int No){m_Parameter.targetfunc = No;};
   int      GetTargetFuncNo()    {return m_Parameter.targetfunc;};
   void     SetAsCopy(bool b)    {m_Parameter.is_a_copy = b;};
   bool     IsACopy()            {return m_Parameter.is_a_copy;};
   void     SetInterupted(bool b){m_Parameter.interupted = b;};
   bool     IsInterupted()       {return m_Parameter.interupted;};

   void     SetScatter(bool b)   {m_ParameterEx.scatter= b;};
   bool     IsScatter()          {return m_ParameterEx.scatter;};
   void     SetScatterForm(int n){m_ParameterEx.scatterform = n;};
   int     GetScatterForm()     {return m_ParameterEx.scatterform;};
   
   // Zustandsfunktionen
   void     SetEditable(bool b)  {if (!m_Parameter.writeprotect) m_Parameter.editable = b;};
   bool     IsEditable()         {return (m_Parameter.editable && !m_Parameter.reference);};
   void     SetWriteProtect(bool b){m_Parameter.writeprotect = b; m_Parameter.editable = !b;};
   bool     IsWriteProtect()     {return m_Parameter.writeprotect;};
   void     SetSorted(bool b)    {m_Parameter.sorted = b;};
   bool     IsSorted()           {return m_Parameter.sorted;};
   void     SetSplineMode(bool b){m_Parameter.splinemode = b;};
   bool     IsSplineMode()       {return m_Parameter.splinemode;};
   void     SetPenChanged(bool b){m_Parameter.penchanged = b;};
   bool     IsPenChanged()       {return m_Parameter.penchanged;};
   void     SetReference(bool b) {m_Parameter.reference = b;};
   bool     IsReference()        {return m_Parameter.reference;};
   void     SetSortable(bool b)  {m_Parameter.sortable = b;};
   bool     IsSortable()         {return m_Parameter.sortable;};
   void     TestXSorted();
	bool     AreXvaluesEqualySpaced();

   // Darstellungparameter
   void     SetWidth(LONG x)     {m_LogPen.lopnWidth.x = x; m_LogPen.lopnWidth.y = x; SetPenChanged(true);};
   void     SetColor(COLORREF c) {m_LogPen.lopnColor = c;      SetPenChanged(true);};
   LONG     GetWidth()           {return m_LogPen.lopnWidth.x;};
   COLORREF GetColor()           {return m_LogPen.lopnColor;};

   bool     IsValid();

public:
   static void*    alloc(size_t, void *pOld=NULL);
   static void     free(void*);
   static bool     HeapWatch();
   static void     DestroyHeap();
	static short    DoubleToShort(double);

protected:
   int            m_nNum;
   BSParameter    m_Parameter;
   BSParameterEx  m_ParameterEx;
   void          *m_pArray;
   LOGPEN         m_LogPen;
   TCHAR          *m_pszUnitx;
   TCHAR          *m_pszUnity;
   TCHAR          *m_pszUnitLocus;
   TCHAR          *m_pszDescription;
   double         m_dOffset;
   double         m_dOrg, m_dStep;
   GET_CRV_PARAM_FKT m_pGetCrvParamFkt;
   void           *m_pGetCrvParam;
private:
   void Init();
   static UINT    gm_nInstances;
   static HANDLE  gm_hHeap;
   static double  gm_dDummy;
};

struct CF_GET_CURVE_PARAMS       // Struktur für den Zugriff auf beliebige Arrays
{
   int     nWhat;
   int     nIndex;
   CCurve *pCurve;
   SCurve *pValue;
   void   *pParam;
};

/******************************************************************************
* Name       : CCurve                                                         *
* Definition : CCurve(CCurve *);                                              *
* Zweck      : Konstruktor mit Argument CCurve zur Initialisierung der        *
*              Parameter der Kurve. Die Kurvenwerte müssen mit der Funktion   *
*              "CopyValues(pCurve)" kopiert werden.                           *
* Aufruf     : CCurve(pCurve);                                                *
* Parameter  :                                                                *
* pCurve  (E): Kurve zur Initialisierung                            (CCurve*) *
* Funktionswert : keiner                                                      *
******************************************************************************/
/******************************************************************************
* Name       : alloc                                                          *
* Definition : void* alloc(size_t, void *);                                   *
* Zweck      : Allozieren von Speicherplatz für die Kurvenwerte.              *
* Aufruf     : alloc(nSize, pOld);                                            *
* Parameter  :                                                                *
* nSize   (E): Größe des zu allozierenden Speicherbereiches in Bytes. (size_t)*
* pOld    (E): Zeiger auf alten, gültigen Speicherbereich             (void*) *
*              zum Reallozieren.                                              *
* Funktionswert : Zeiger auf den allozierten Speicherbereich          (void*) *
******************************************************************************/
/******************************************************************************
* Name       : free                                                           *
* Definition : void free(void* );                                             *
* Zweck      : Freigeben des mit der Funktion "CCurve::alloc()" allozierten   *
*              Speicherbereiches.                                             *
* Aufruf     : free(p);                                                       *
* Parameter  :                                                                *
* p       (E): Zeiger auf gültigen Speicherbereich.                           *
* Funktionswert : keiner                                                      *
******************************************************************************/
/******************************************************************************
* Name       : HeapWatch                                                      *
* Definition : bool HeapWatch();                                              *
* Zweck      : Überprüfung des CCurve eigenen Heaps auf Speicherlecks durch   *
*              nicht freigegebene Objekte bei Programmende.                   *
* Aufruf     : HeapWatch();                                                   *
* Parameter  : keine                                                          *
* Funktionswert : (true, false)                                       (bool)  *
******************************************************************************/
/******************************************************************************
* Name       : GetCurveValue                                                  *
* Definition : SCurve GetCurveValue(int);                                     *
* Zweck      : Liefert den X-und Y-Kurvenwert an der Stelle (n).              *
* Aufruf     : GetCurveValue(n);                                              *
* Parameter  :                                                                *
* n       (E): Index des Kurenwertes.                                (int)    *
* Funktionswert : Struktur mit dem Kurvenwert                        (SCurve) *
******************************************************************************/
/******************************************************************************
* Name       : GetX_Value                                                     *
* Definition : double GetX_Value(int n);                                      *
* Zweck      : liefert den X-Wert der Kurve für den Index n.                  *
* Aufruf     : GetX_Value(n);                                                 *
* Parameter  :                                                                *
* n      (E) : Index für den Kurvenwert                              (int)    *
* Funktionswert : Kurvenwert                                         (double) *
******************************************************************************/
/******************************************************************************
* Name       : GetY_Value                                                     *
* Definition : double GetY_Value(int n);                                      *
* Zweck      : liefert den Y-Wert der Kurve für den Index n.                  *
* Aufruf     : GetY_Value(n);                                                 *
* Parameter  :                                                                *
* n      (E) : Index für den Kurvenwert                              (int)    *
* Funktionswert : Kurvenwert                                         (double) *
******************************************************************************/
/******************************************************************************
* Name       : SetX_Value                                                     *
* Definition : double SetX_Value(int n);                                      *
* Zweck      : Setzt den X-Wert der Kurve für den Index n.                    *
* Aufruf     : SetX_Value(n);                                                 *
* Parameter  :                                                                *
* n      (E) : Index für den Kurvenwert                              (int)    *
* Funktionswert : Referenz auf den Kurvenwert                        (double&)*
******************************************************************************/
/******************************************************************************
* Name       : SetX_Value                                                     *
* Definition : bool SetX_Value(int, double);                                  *
* Zweck      : Setzt den X-Wert der Kurve für den Index n.                    *
* Aufruf     : SetX_Value(n, dVal);                                           *
* Parameter  :                                                                *
* n      (E) : Index für den Kurvenwert                              (int)    *
* dVal   (E) : neuer X-Kurvenwert                                    (double) *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
/******************************************************************************
* Name       : SetY_Value                                                     *
* Definition : double GetY_Value(int n);                                      *
* Zweck      : setzt den Y-Wert der Kurve für den Index n.                    *
* Aufruf     : SetY_Value(n);                                                 *
* Parameter  :                                                                *
* n      (E) : Index für den Kurvenwert                              (int)    *
* Funktionswert : Kurvenwert                                         (double) *
******************************************************************************/
/******************************************************************************
* Name       : SetY_Value                                                     *
* Definition : bool SetY_Value(int, double);                                  *
* Zweck      : Setzt den Y-Wert der Kurve für den Index n.                    *
* Aufruf     : SetY_Value(n, dVal);                                           *
* Parameter  :                                                                *
* n      (E) : Index für den Kurvenwert                              (int)    *
* dVal   (E) : neuer Y-Kurvenwert                                    (double) *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
/******************************************************************************
* Name       : SetCurveValue                                                  *
* Definition : bool SetCurveValue(int, SCurve &);                             *
* Zweck      : Setzen des X- und Y-Wertes der Kurve an der Stelle (n)         *
* Aufruf     : bool SetCurveValue(n, c);
* Parameter  :                                                                *
* n       (E): Index des Kurenwertes.                                (int)    *
* c       (E): Struktur mit dem Kurvenwert                           (SCurve&)*
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
/******************************************************************************
* Name       : SetSize                                                        *
* Definition : bool SetSize(int n);                                           *
* Zweck      : Setzt die Größe des Arrays für die Kurvenwerte. Wird eine neues*
*              Array alloziert, so werden die Kurvenwerte auf 0 gesetzt. Wird *
*              die Arraygröße verändert, so bleiben die alten Werte erhalten  *
*              vom Arrayanfang her erhalten.                                  *
* Aufruf     : SetSize(n);                                                    *
* Parameter  :                                                                *
* n      (E) : neue Größe des Arrays für die Kurvenwerte             (int)    *
* Funktionswert (true, false)                                        (bool)   *
******************************************************************************/
/******************************************************************************
* Name       : Destroy                                                        *
* Definition : void Destroy();                                                *
* Zweck      : Freigabe des Speicherplatzes für die Kurvenwerte               *
* Aufruf     : Destroy();                                                     *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
/******************************************************************************
* Name       : Write                                                          *
* Definition : void Write(HANDLE);                                            *
* Zweck      : Schreiben der Daten der Kurve in eine Datei                    *
* Aufruf     : Write(hfile);                                                  *
* Parameter  :                                                                *
* hfile  (E) : gültiger Datei-Handle                                 (HANDLE) *
* Funktionswert keiner                                                        *
******************************************************************************/
/******************************************************************************
* Name       : Read                                                           *
* Definition : void Read(HANDLE);                                             *
* Zweck      : Lesen der Daten der Kurve aus einer Datei.                     *
* Aufruf     : Read(hfile);                                                   *
* Parameter  :                                                                *
* hfile  (E) : gültiger Datei-Handle                                 (HANDLE) *
* Funktionswert keiner                                                        *
******************************************************************************/
/******************************************************************************
* Name       : GetChecksum                                                    *
* Definition : void GetChecksum(CFileHeader *pFH);                            *
* Zweck      : Berechnen der CRC-Checksumme aus allen Daten des CCurve-Objekts*
* Aufruf     : GetChecksum(pFH);                                              *
* Parameter  :                                                                *
* pFH    (E) : Zeiger auf CFileHeader-Objekt. Dieses Objekt wird als Dateikopf*
*              am Anfang der Datei gespeichert.                (CFileHeader*) *
* Funktionswert keiner                                                        *
******************************************************************************/
/******************************************************************************
* Name       : IsValid                                                        *
* Definition : bool IsValid();                                                *
* Zweck      : ermittelt ob das Kurvenobjekt gültig ist                       *
* Aufruf     : IsValid();                                                     *
* Parameter  : keine                                                          *
* Funktionswert keiner                                                        *
******************************************************************************/
/******************************************************************************
* Name       : SetDescription                                                 *
* Definition : void SetDescription(TCHAR *s);                                  *
* Zweck      : Setzt den Beschreibungstext für die Kurve.                     *
* Aufruf     : SetDescription(s)                                              *
* Parameter  :                                                                *
* s      (E) : Zeiger auf Beschreibungstext                          (TCHAR*)  *
* Funktionswert keiner                                                        *
******************************************************************************/
/******************************************************************************
* Name       : SetXUnit                                                       *
* Definition : void SetXUnit(TCHAR *s);                                        *
* Zweck      : Setzt den Text für die Einheit der X-Achse                     *
* Aufruf     : SetXUnit(s);                                                   *
* Parameter  :                                                                *
* s      (E) : Zeiger auf Text                                       (TCHAR*)  *
* Funktionswert keiner                                                        *
******************************************************************************/
/******************************************************************************
* Name       : SetYUnit                                                       *
* Definition : void SetYUnit(TCHAR *s);                                        *
* Zweck      : Setzt den Text für die Einheit der Y-Achse                     *
* Aufruf     : SetYUnit(s);                                                   *
* Parameter  :                                                                *
* s      (E) : Zeiger auf Text                                       (TCHAR*)  *
* Funktionswert keiner                                                        *
******************************************************************************/
/******************************************************************************
* Name       : GetXMax                                                        *
* Definition : double GetXMax();                                              *
* Zweck      : liefert den Maximalen X-Wert                                   *
* Aufruf     : GetXMax();                                                     *
* Parameter  :                                                                *
* Funktionswert: Maximaler X-Wert                              (double)       *
******************************************************************************/
/******************************************************************************
* Name       : GetXMin                                                        *
* Definition : double GetXMin();                                              *
* Zweck      : liefert den minimalen X-Wert                                   *
* Aufruf     : GetXMin();                                                     *
* Parameter  :                                                                *
* Funktionswert: minimaler X-Wert                              (double)       *
******************************************************************************/
/******************************************************************************
* Name       : GetYMax                                                        *
* Definition : double GetYMax();                                              *
* Zweck      : liefert den Maximalen Y-Wert                                   *
* Aufruf     : GetYMax();                                                     *
* Parameter  :                                                                *
* Funktionswert: Maximaler Y-Wert                              (double)       *
******************************************************************************/
/******************************************************************************
* Name       : GetYMin                                                        *
* Definition : double GetYMin();                                              *
* Zweck      : liefert den minimalen Y-Wert                                   *
* Aufruf     : GetYMin();                                                     *
* Parameter  :                                                                *
* Funktionswert: minimaler Y-Wert                              (double)       *
******************************************************************************/
/******************************************************************************
* Name       : CopyValues                                                     *
* Definition : void CopyValues(CCurve *);                                     *
* Zweck      : Kopieren aller Kurvenwerte aus der übergebenen Kurve.          *
* Aufruf     : CopyValues(pc);                                                *
* Parameter  :                                                                *
* pc     (E) : Kurve mit den neuen Werten                        (CCurve*)    *
* Funktionswert : keiner                                                      *
******************************************************************************/
/******************************************************************************
* Name       : TestXSorted                                                    *
* Definition : void TestXSorted();                                            *
* Zweck      : Prüfen ob die Kurvenwerte schon sortiert sind.                 *
*              Die Funktion setzt die Kurveninternen Parameter.               *
* Aufruf     : TestXSorted();                                                 *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
/******************************************************************************
* Name       : AttachArray                                                    *
* Definition : bool AttachArray(void *, int, int, bool);                      *
* Zweck      : Übergeben eines short-, float- oder double- Feldes für die     *
*              Kurvenwerte.                                                   *
* Aufruf     : AttachArray(pArray, nCount, nFormat, bTestXSorted);            *
* Parameter  :                                                                *
* pArray  (E): Zeiger auf neues Array mit den Kurvenwerten.          (void*)  *
* nCount  (E): Anzahl der Kurvenwerte                                (int)    *
* nFormat (E): Format des Feldes                                     (int)    *
*    CF_DOUBLE        : (SCurve*)      X- und Y-Werte                         *
*    CF_FLOAT         : (SCurveFloat*)   "      "                             *
*    CF_SHORT         : (SCurveShort*)   "      "                             *
*    CF_SINGLE_DOUBLE : (double*)      nur Y-Werte                            *
*    CF_SINGLE_FLOAT  : (float*)         "      "                             *
*    CF_SINGLE_SHORT  : (short*)         "      "                             *
* bTestXSorted (E): Überprüfen, ob die X-Werte sortiert sind         (bool)   *
* Funktionswert : Feld übergeben (true, false)                       (bool)   *
******************************************************************************/
/******************************************************************************
* Name       : DetachArray                                                    *
* Definition : void * DetachArray();                                          *
* Zweck      : Entfernen eines übergebenen Feldes, so daß es von CCurve nicht *
*              mehr freigegeben wird.                                         *
* Aufruf     : DetachArray();                                                 *
* Parameter  : Keine                                                          *
* Funktionswert : Zeiger auf das Feld                                (void*)  *
******************************************************************************/
/******************************************************************************
* Name       : GetArrayPointer                                                *
* Definition : const void * GetArrayPointer();                                *
* Zweck      : Liefert den Zeiger auf das Feld mit den Kurvenwerten.          *
* Aufruf     : GetArrayPointer();                                             *
* Parameter  : Keine                                                          *
* Funktionswert : Zeiger auf das Feld                                (void*)  *
******************************************************************************/
/******************************************************************************
* Name       : SetFormat                                                      *
* Definition : bool SetFormat(int);                                           *
* Zweck      : Setzen des Formates für die Kurvewerte.                        *
* Hinweis !  : Das Format kann nur im leeren Zustand gesetzt werden. D.h.     *
*              wenn die Kurve keine Kurvenwerte enthält.                      *
* Aufruf     : SetFormat(nFormat)                                             *
* Parameter  :                                                                *
* nFormat (E): Format des Feldes                                     (int)    *
*    CF_DOUBLE        : (SCurve*)      X- und Y-Werte                         *
*    CF_FLOAT         : (SCurveFloat*)   "      "                             *
*    CF_SHORT         : (SCurveShort*)   "      "                             *
*    CF_SINGLE_DOUBLE : (double*)      nur Y-Werte                            *
*    CF_SINGLE_FLOAT  : (float*)         "      "                             *
*    CF_SINGLE_SHORT  : (short*)         "      "                             *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/

#endif // !defined(AFX_CURVE_H__45DBF622_F7BC_11D0_9DB9_B051418EA04A__INCLUDED_)
