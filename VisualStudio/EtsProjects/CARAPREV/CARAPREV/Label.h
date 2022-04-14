// Label.h: Schnittstelle für die Klasse CLabel.
//
//////////////////////////////////////////////////////////////////////

#if !defined (AFX_Label_H__5897BC23_E97C_11D0_9DB9_B051418EA04A__INCLUDED_)
#define AFX_Label_H__5897BC23_E97C_11D0_9DB9_B051418EA04A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

#define NO_PATH 4

#define STOP_PROCESSING      0x01      // Abbruch Bearbeitung der Label
#define DELETE_LABEL         0x02      // entfernen und löschen des Labels aus der Liste
#define REMOVE_LABEL         0x04      // entfernen des Labels aus der Liste
#define INSERT_LABEL_BEFORE  0x08      // einfügen des Labels vor der aktuellen Position
#define INSERT_LABEL_AFTER   0x10      // einfügen des Labels hinter der aktuellen Position
#define DONT_PROCESS_CONTENT 0x20      // Den Inhalt des Labelcontainers nicht bearbeiten

#define SCALE_ARBITRARY       0        // Skalierungsart eines Labels : beliebig
#define SCALE_NONPROPORTIONAL 1        // nicht Spiegelbar, Seitenverhältnis beliebig
#define SCALE_PROPORTIONAL    2        // Seitenverhältnis konstant

#define DEV_COORD           0x01
#define INFLATE_RECT        0x02

#define ROTATION_POINT        -2
#define ROTATING_POINT        -3

//#define CHECK_LABEL_CONSTRUCTION
//#define CHECK_LABEL_DESTRUCTION
//#define CHECK_LABEL_SERIALIZATION

typedef CTypedPtrList < CObList, CPen* >   CPenList;
typedef CTypedPtrList < CObList, CBrush* > CBrushList;
typedef CTypedPtrList < CObList, CFont* >  CFontList;

class CLabel;
class CFileHeader;

struct ProcessLabel                    // struktur für die Bearbeitung der Label durch die Funktion
{                                      // ProcessWithLabels()
   CLabel *pl;                         // übergebenes Labelobjekt
   int     nPos;                       // Zählvariable, bzw. Zustandsspeicher
   void   *pParam1;                    // zusätzliche Parameter 1
   void   *pParam2;                    // zusätzliche Parameter 2
};

struct BSState                         // Bitstruktur für die Zustände eines Labels
{             
   unsigned char changed         :1;   // Labelpunkte verändert
   unsigned char shapedrawn      :1;   // Umriß ist gezeichnet
   unsigned char picked          :1;   // Label ist gepickt
   unsigned char pointpicked     :1;   // Labelpunkt ist gepickt
   unsigned char doublepicked    :1;   // Label ist durch Doubleclic gepickt  (*1)
   unsigned char fixed           :1;   // Labelpunkte sind unveränderbar
   unsigned char visible         :1;   // Label ist sichtbar
   unsigned char drawpickpoints  :1;   // Labelpunkte sollen gezeichnet werden(*1)
   unsigned char drawframe       :1;   // Rahmen soll gezeichnet werden       (*2)
   unsigned char drawbackgnd     :1;   // Hintergrund soll gezeichnet werden  (*2)
   unsigned char scaling         :2;   // Skalierungsart des Labels           (*2)
   unsigned char scaleable       :1;   // der Label kann skaliert werden      (*3)
   unsigned char deletecontent   :1;   // enthaltene Label werden mit gelöscht(*2)
   unsigned char delayedscale    :1;   // der Label muß neu scaliert werden   (*4)
   unsigned char textchanged     :1;   // der TextLabel wurde geändert        (*4)
   unsigned char cliplabel       :1;   // der Label wird geclippt
   unsigned char hidden          :1;   // der Label wird versteckt            (*5)
   unsigned char no_undo         :1;   // kein Undo ausführen
   unsigned char multitextlines  :1;   // Mehrere Textzeilen                  (*2)
   unsigned char calctextrect    :1;   // Textrechteck berechten              (*2)
   unsigned char previewmode     :1;   // Plot ist im Preview                 (*6)
   unsigned char rotationmode    :1;   // rotation
   unsigned char text_extended   :1;   // Text wurde für Zeichnen & so erweitert
   unsigned char dummy3          :8;
};

// (*1) kann unterschiedliche Bedeutungen für die von CLabel abgeleiteten Klassen haben
// (*2) hat nicht für alle von CLabel abgeleiteten Klassen eine Bedeutung
// (*3) Skalierbare Label haben einen Punkt mehr als nicht skalierbare. Deshalb wird
//      diese Eigenschaft ausschließlich im Konstruktor eingestellt. Die Skalierbarkeit
//      wird für einige abgleitete Label-Klassen benötigt und wurde in der Basisklasse 
//      implementiert, um denselben Code für die ableiteten Klassen wieder zu verwenden.
// (*4) für TextLabel: nach Änderungen muß neu berechnet werden.
// (*5) Label, die außerhalb des Viewbereichs sind, werden geclipped oder versteckt. Sie werden
//      wieder sichtbar, wenn sie sich innerhalb des Viewbereiches befinden.
// (*6) im Preview kann in einem Plot keine Kurve gepickt werden.

template <class T> void swap(T& x, T& y)
{
   T Temp;
   Temp = x;
          x = y;
              y = Temp;
}

class AFX_EXT_CLASS CLabel : public CObject
{
   DECLARE_SERIAL(CLabel)
public:
      CLabel();
      // virtuelle Funktionen
      virtual        ~CLabel();
      virtual bool   Draw(CDC*, int bAtr = 1);
      virtual bool   DrawShape(CDC*,bool);
      virtual int    IsOnLabel(CDC*, CPoint *, CRect*);
      virtual CRect  GetRect(CDC *pDC=NULL, UINT nFlags=0);
      virtual bool   GetRgn(int, CDC *, CRgn *, UINT nFlags=0);
      virtual void   SetPoint(CPoint);
      virtual void   MoveLabel(CPoint);
      virtual void   ScaleLabel(CSize, CSize);
      virtual void   Serialize(CArchive &);

      void           GetChecksum(CFileHeader*);
	   static  CPen*  SelectPen(CDC*, CPen*);
	   static  CBrush*SelectBrush(CDC*, CBrush*);
      static  void   SetFileVersion(int n) {gm_nFileVersion = n;};
	   static  bool   IsNegativeY(CDC*);
      void           SetNoUndoCmd(bool b) {m_State.no_undo = b;};
      bool           DoUndoCmd() {return !m_State.no_undo;};
      // Funktionen zum setzen und erfragen der Punkte eines Labels
      void           SetPoint(CPoint, int);
      int            GetCount() {return m_nCount;};
      CPoint         GetPoint(int);
      CPoint         GetPoint();
      bool           SetMatchPoint(CPoint *);
      bool           SetMatchPoint(int);
      int            GetMatchPoint() {return m_nAct;};
      static  void   SetPickPointSize(CSize p) {gm_PickPointSize=p;};
      static  CSize  GetPickPointSize()        {return gm_PickPointSize;};
      // Funktionen zum setzen und erfragen der Sichtbarkeit eines Labels
      void           SetVisible(bool b)      {m_State.visible = b; if (!b) ResetPickStates();};
      bool           IsVisible()             {return m_State.visible;};
      void           SetHidden(bool b)       {m_State.hidden = b;};
      bool           IsHidden()              {return m_State.hidden;};
      // Funktionen zum setzen und erfragen der Editierbarkeit eines Labels
      void           SetFixed(bool b)        {m_State.fixed = b;};
      bool           IsFixed()               {return m_State.fixed;};
      // Funktionen zum setzen und erfragen der Aktualität eines Labels
      void           SetChanged(bool b)      {m_State.changed = b;};
      bool           IsChanged()             {return m_State.changed;};
      void           SetShapeDrawn(bool b)   {m_State.shapedrawn = b;};
      bool           IsShapeDrawn()          {return m_State.shapedrawn;};
      // Funktionen zum setzen und erfragen der Darstellung eines Labels
      void           SetDrawFrame(bool b)    {m_State.drawframe = b;};
      bool           DoDrawFrame()           {return m_State.drawframe;};
      void           SetDrawBkGnd(bool b)    {m_State.drawbackgnd = b;};
      bool           DoDrawBkGnd()           {return m_State.drawbackgnd;};
      COLORREF       GetBkColor()            {return m_BkColor;};
      void           SetBkColor(COLORREF c)  {m_BkColor = c;};
      bool           IsMultiLineText() {return m_State.multitextlines;};
      void           SetPreviewMode(bool b)  {m_State.previewmode = b;}
      void           SetStates(BSState s)    {m_State = s;};
      BSState        GetStates()             {return m_State;};
      // Darstellung des Pickzustandes
      static  void   SetShapePen(COLORREF color = 0xFFFFFFFF);
      static COLORREF GetShapePenColor()       {return gm_ShapePenColor;}
      // Funktionen zur Scalierung eines Labels
      void           SetScaling(int ns)      {if (m_State.scaleable) m_State.scaling = ns;};
      int            GetScaling()            {return m_State.scaling;};
      bool           SetScale(int b, bool bSetSize = false);
      bool           SetScaleSize(CSize szScale); 
      bool           CheckScale()            {if (m_State.scaleable && (m_State.scaling == SCALE_PROPORTIONAL)) return CheckScale(m_pP[0], m_pP[1]); return false;};
      bool           CheckScale(CPoint&, CPoint&);
      // Funktionen zum setzen und erfragen der Pickzustände eines Labels
      void           SetPicked(bool b)       {m_State.picked = b;};
      bool           IsPicked()              {return m_State.picked;};
      void           SetPointPicked(bool b)  {m_State.pointpicked = b;};
      void           SetDrawPickPoints(bool b){m_State.drawpickpoints = b;};
      bool           DoDrawPickPoints()      {return m_State.drawpickpoints;};
      bool           IsPointPicked()         {return m_State.pointpicked;};
      void           SetDoublePicked(bool b) {m_State.doublepicked = b;};
      bool           IsDoublePicked()        {return m_State.doublepicked;};
      void           ResetPickStates();

      // Funktionen zum Picken eines Labels
      int            HitCurve(CPoint *, CRect*pr=NULL);
      static int     HitLine(CPoint *, CPoint *, CPoint *, CRect*pr=NULL);
      static bool    ClipLine(CPoint *, CPoint *, CRect *);
	   static int     HitPolygon(CPoint*, int, CPoint*, CRect*);
      static CPoint  PixelToLogical(CDC*, int, int);
	   static int     MaxPickDistance();
	   static int     IsInside() {return MaxPickDistance()-1;};
      static int     IsOutside() {return -1;};
	   static bool    IsBetween(int, int, int);  // (n, nLo, nUp) true => n >= nLo && n <=nUp
      static CPoint  GetNearestPoint(CRect*, CPoint*);
      static int     DecreaseLimited(int, int, int);
      static int     IncreaseLimited(int, int, int);
      // globale Listenfunktionen für Pen-, Brush-, Font-Recourcen
      static void    LoadGDIObjects(CArchive &);
      static void    SaveGDIObjects(CArchive &);
      static void    DeleteLOGArray();
      static CFont  *FindFont(int);
      static CPen   *FindPen(int);
      static CBrush *FindBrush(int);
      static CFont  *FindFont(LOGFONT*);
      static CPen   *FindPen(LOGPEN*);
      static CBrush *FindBrush(LOGBRUSH*);
      static int     FindFont(CFont*);
      static int     FindPen(CPen*);
      static int     FindBrush(CBrush*);
      static void    DeleteGraficObjects(bool bAll=false);
      static void    SetSaveGlobal(bool bSave) {gm_bSaveGlobal = bSave;};
      // Zeichnen eines Rechtecks mit richtiger Größe
      static void    Rectangle(CDC *, LPRECT, bool);
      static void    Rectangle(CDC *, int, int, int, int, bool);
      // Arbeiten mit Archiven / Dateien
	   static void    ArchiveRead(CArchive &, void*, UINT, bool bForce = false);
	   static void    ArchiveWrite(CArchive &, const void*, UINT);
	   static char   *ReadVariableString(CArchive &, bool bForce = false);
	   static void    WriteVariableString(CArchive&, char*);
      static COLORREF GetInvertedColor(COLORREF);

protected:                      // Vererbbare Variablen
      CPoint  *m_pP;            // Zeiger auf die Punkte eines Labels
      int      m_nCount;        // Anzahl der Punktes eines Labels
      int      m_nAct;          // Index des aktuellen Matchpoints
      BSState  m_State;         // Zustandsspeicher des Labels
      COLORREF m_BkColor;       // Hintergrundfarbe

      static void CALLBACK LineDDA_Proc(int, int, LPARAM);
      struct sLineDDA
      {
         CDC * pDC;
         int      nCount;
         int      nSize;
         char    *pcArray;
         COLORREF color;
      };
   	static void ChangeGlobalObject(CGdiObject *, CGdiObject *);
      // Deklaration einer static-Variablen im Header, Scope-Deklaration im CPP-File vor dem Konstruktor
      static  CPen        *gm_pShapePen;        // Stift zum markieren eines Labels
      static  CSize        gm_PickPointSize;    // Größe der Pickpunkte
      static  CPenList     gm_PenList;          // globale Liste mit den erzeugten Zeichenstiften
      static  CBrushList   gm_BrushList;        // globale Liste mit den erzeugten Zeichenpinseln
      static  CFontList    gm_FontList;         // globale Liste mit den erzeugten Zeichenschriften
      static  LOGPEN      *gm_pPenArray;        // globales Array zum Laden von Stiften aus einer Datei
      static  int          gm_nPens;            // Anzahl der Pens
      static  LOGBRUSH    *gm_pBrushArray;      // globales Array zum Laden von Pinseln aus einer Datei
      static  int          gm_nBrushs;          // Anzahl der Pinsel
      static  LOGFONT     *gm_pFontArray;       // globales Array zum Laden von Schriften aus einer Datei
      static  int          gm_nFonts;           // Anzahl der Schriften
      static  bool         gm_bSaveGlobal;      // Flag für das Speichern von Eigenschaften in globalen Variablen
      static  CPtrList     gm_GlobalGdiObjects; // Diese werden zur initialisierung der Objekte im Konstruktor verwendet
      static  COLORREF     gm_ShapePenColor;    // Farbe des globalen Markierungsstiftes
      static  int          gm_nFileVersion;     // Version der Datei für das Laden eines Labels
public:
      static  bool         gm_bColorMatching;   // Farbanpassung an den Drucker ein/aus
      static  bool         gm_bFillPath;        // FillPath ein/aus

      // Funktionen und Variablen zum allozieren und freigeben von Speicher in der Release-Version
#ifdef _DEBUG
   public:
   	virtual void AssertValid() const;
#else
   public:
      static void* __cdecl operator new(size_t nSize);
      static void  __cdecl operator delete(void* p);
      static bool HeapWatch();
      static void DestroyHeap();
   protected:
      static  HANDLE       gm_hHeap;
      static  unsigned     gm_nInstances;
#endif
};


// Definition einer TypedPtrList
typedef CTypedPtrList < CObList, CLabel* > CLabelList;

#endif // !defined(AFX_Label_H__5897BC23_E97C_11D0_9DB9_B051418EA04A__INCLUDED_)
