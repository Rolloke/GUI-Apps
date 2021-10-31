// PlotLabel.h: Schnittstelle für die Klasse CPlotLabel.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLOTLabel_H__FF559990_470A_11D1_9DB9_B051418EA04A__INCLUDED_)
#define AFX_PLOTLabel_H__FF559990_470A_11D1_9DB9_B051418EA04A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <winbase.h>
#include "Transformation.h"
#include "RectLabel.h"
#include "LabelContainer.h"
#include "PreviewFileHeader.h"
#include "LineLabel.h"
#include "TextLabel.h"
#include "Curve.h"
#include "CEtsDiv.h"

#define NUM_CURVE_COLORS 16
#define GRID_LINE_MASK   0x0fff                                // nur die unteren 12 Bit werden genutzt
#define HIDE_GRID_LINE   0x8000
#define HIDE_GRID_TEXT   0x4000

#define DONT_DELETE_INSERTED_CURVE 0x0001
#define FIT_INSERTED_CURVE_ID      0x0002
#define REPLACE_CURVE_TEXT         0x0004

#define NO_LOGARITHMIC_CURVES 0x01
#define NO_POLAR_CURVES       0x02

#define PAN_ZOOM_RIGHT 0
#define PAN_ZOOM_LEFT  1
#define PAN_ZOOM_UP    2
#define PAN_ZOOM_DOWN  3

class CFileHeader;
class CCurveLabel;

struct BSPlotDefaults
{
   unsigned char xDivision    :2;                              // Teilung der x-Achse (CCURVE_LINEAR, CCURVE_LOGARITHMIC, CCURVE_FREQUENCY)
   unsigned char yDivision    :2;                              // Teilung der y-Achse
   unsigned char xNumMode     :2;                              // Darstellung der x-Achsenzahlen (NM_STD, NM_SCI, NM_ENG)
   unsigned char yNumMode     :2;                              // Darstellung der y-Achsenzahlen
};

struct SStandardPlotSettings
{
   LOGFONT           sm_HeadingLogFont;         // Font für die Überschrift
   COLORREF          sm_HeadingColor;           // Überschriftfarbe
   LOGFONT           sm_GridTextLogFont;        // Font für die Gittertexte
   COLORREF          sm_GridTextColor;          // Gittertextfarbe
   COLORREF          sm_GridLineColor;          // Gitterlinienfarbe
   LOGFONT           sm_CurveLogFont;           // Font für die Kurventexte
   COLORREF          sm_CurveColors[NUM_CURVE_COLORS];// Farbreihenfolge
   LOGPEN            sm_PlotFrame;              // Rahmen des Plots
   LOGBRUSH          sm_PlotBrush;              // Hintergrund des Plots
   COLORREF          sm_BkColor;                // Hintergrundfarbe des Plots
   char              sm_cNumMode_x;             // Darstellung der x-Achsenzahlen (NM_STD, NM_SCI, NM_ENG)
   char              sm_cNumMode_y;             // Darstellung der y-Achsenzahlen (NM_STD, NM_SCI, NM_ENG)
   char              sm_cRound_x;               // Nachkommastellen x
   char              sm_cRound_y;               // Nachkommastellen y
   WORD              sm_wGridStep_x;            // mittlerer Linienabstand für die x-Achse
   WORD              sm_wGridStep_y;            // mittlerer Linienabstand für die y-Achse
   long              sm_nDrawframe;             // Kurvenrahmen zeichnen
};


class CListDlg;

class AFX_EXT_CLASS CPlotLabel : public CRectLabel, public CTransformation
{
   friend class CCurveLabel;
   DECLARE_SERIAL(CPlotLabel)
public:
   CPlotLabel();
   CPlotLabel(CPoint *pl);
   virtual ~CPlotLabel(); 
   virtual void   Serialize(CArchive &);
   virtual bool   Draw(CDC*, int bAtr = 1);
   virtual bool   DrawShape(CDC*, bool);
#ifdef _DEBUG
   	virtual void AssertValid() const;
#endif
   virtual void _fastcall TransformPoint(LPDPOINT, CPoint *);
   virtual void _fastcall TransformPoint(CPoint *, LPDPOINT);

   void           GetChecksum(CFileHeader*);
   // Funktionen für Weltkoordinatenbereich
   void           SetDefaults();
   void           SetDefaultRange(RANGE* pRange = NULL);
   bool           SetRange(LPRANGE);
   bool           SetRangeInLogical(CDC *, CRect);
   RANGE          GetRange();
   void           AddRange(RANGE*);
   void           DestroyRangeList();
   int            GetNoOfRanges();
   bool           CheckDefault(){return ((m_RangeList.GetCount() > 1) ? true: false);};
   bool           CheckZoom()   {return ((m_RangePos != m_RangeList.GetTailPosition()) ? true : false);};
   bool           CheckReZoom() {return ((m_RangePos != m_RangeList.GetHeadPosition()) ? true : false);};
   bool           Zoom();
   bool           ReZoom();
   bool           SelectZoom();
   bool           UpdateZoom();
   bool           ZoomPolarPlot(CDC*, CPoint, int);
   bool           PanZoom(int nZoom);
   void           SetMarker(int iM, double dPos);
   BOOL           GetMarker(int iM, double&);
   // Transformations- und Weltkoordinatenfunktionen
   bool           Polyline(CDC*, int, int, int, CCurveLabel*);
   void           CartesianToPolar(DPOINT*);
   void           PolarToCartesian(DPOINT*);
//   bool           DrawCurve(CDC* pDC, int numpoints, LPDPOINT c);
   int            CatchGridValue(CDC*, CPoint *p, LPDPOINT);
   // Funktionen zum ermitteln der Gitteraufteilung und Textpositionen
   void           UpdateChanges(CDC*);
   RANGE          DeterminRange();
   bool           DeterminDefaultRange();
   int            DeterminCurveTextPositions(CDC*);
   void           DeterminViewRect(CDC*);
   bool           CalculateViewDivision(CDC *);
   void           Set_Y_DivText(ETSDIV_NUTOTXT *, CRect *, CPoint *, CDC *);
   void           Set_X_DivText(ETSDIV_NUTOTXT *, CRect *, CPoint *, CDC *, int&);
   // Zugriff auf Kurven
   CLabelContainer *GetCurveList()     {return &m_Curves;};
   bool           SetReferenceCurve(CCurveLabel *);
   CCurveLabel   *GetReferenceCurve()    {return m_pRefCurve;};
   void           SetReferenceOn(bool b);
   bool           IsReferenceOn()            {return m_bReferenceOn;};
   bool           InsertCurve(CCurveLabel *, bool bTest=false, long nPos=0x0000ffff);
   bool           RemoveCurve(CCurveLabel *, int &);
   double         GetStepValue(int , int, double&);
   static int     CheckCurveDivision(CLabel*, void *);
   static int     CheckInsertCurves(CLabel*, void*pParam);
   // Darstellungsparameter Plot und Überschrift
   void           SetHeading(const TCHAR *psz){m_PlotHeading.SetText(psz);};
   const TCHAR*    GetHeading()         {return m_PlotHeading.GetText();};
   void           SetHeadingColor(COLORREF);
   COLORREF       GetHeadingColor()    {return m_PlotHeading.GetTextColor();};
   void           SetHeadingLogFont(LOGFONT);
   LOGFONT        GetHeadingLogFont()  {return m_PlotHeading.GetLogFont();};
	bool           IsOnHeading(CDC*, CPoint*);
   void           SetHeadingPicked(bool b) {m_bHeadingPicked = b;};
   bool           IsHeadingPicked()        {return m_bHeadingPicked;};
   // Darstellungsparameter für Gittereinteilung
   void           SetXDivision(int nd);
   int            GetXDivision()       {return m_Parameter.xDivision;};
   void           SetYDivision(int nd);
   int            GetYDivision()       {return m_Parameter.yDivision;};
   void           SetXGridStep(WORD wS){m_wGridStep_x = wS;};
   void           SetYGridStep(WORD wS){m_wGridStep_y = wS;};
   WORD           GetXGridStep()       {return m_wGridStep_x;};
   WORD           GetYGridStep()       {return m_wGridStep_y;};
   void           SetGridLineColor(COLORREF);
   COLORREF       GetGridLineColor()   {return m_GridLineColor;};
   // Darstellungsparamter für Gitterbeschriftung
   void           SetGridTextColor(COLORREF);
   COLORREF       GetGridTextColor()   {return m_Unit_x.GetTextColor();};
   void           SetGridTextLogFont(LOGFONT);
   LOGFONT        GetGridTextLogFont() {return m_Unit_x.GetLogFont();};
   void           SetXNumMode(int nm)  {m_Parameter.xNumMode = nm;};
   int            GetXNumMode()        {return m_Parameter.xNumMode;};
   void           SetYNumMode(int nm)  {m_Parameter.yNumMode = nm;};
   int            GetYNumMode()        {return m_Parameter.yNumMode;};
   void           SetXNumModeEx(int);
   int            GetXNumModeEx();
   void           SetYNumModeEx(int);
   int            GetYNumModeEx();
   void           SetXNoEndNulls(int nm){m_Parameter.xNoEndNulls = nm;};
   int            NoXEndNulls()         {return m_Parameter.xNoEndNulls;};
   void           SetYNoEndNulls(int nm){m_Parameter.yNoEndNulls = nm;};
   int            NoYEndNulls()         {return m_Parameter.yNoEndNulls;};
   void           SetPlotID(int n){m_Parameter.targetfunc= n;};
   int            GetPlotID()     {return m_Parameter.targetfunc;};
   void           SetXRound(char cR)   {m_cRound_x = (m_cRound_x&0xc0)|(cR&0x3f);};
   void           SetYRound(char cR)   {m_cRound_y = (m_cRound_y&0xc0)|(cR&0x3f);};
   char           GetXRound()          {return m_cRound_x&0x3f;};
   char           GetYRound()          {return m_cRound_y&0x3f;};
   bool           GroupXNumbers()      {return (m_cRound_x&0x80)?true:false;};
   bool           GroupYNumbers()      {return (m_cRound_y&0x80)?true:false;};
   bool           LocaleXNumbers()     {return (m_cRound_x&0x40)?true:false;};
   bool           LocaleYNumbers()     {return (m_cRound_y&0x40)?true:false;};
   void           SetGroupXNumbers(bool);
   void           SetGroupYNumbers(bool);
   void           SetLocaleXNumbers(bool);
   void           SetLocaleYNumbers(bool);
   void           SetXUnit(TCHAR *psz);
   const TCHAR*    GetXUnit()           {return m_Unit_x.GetText();};
   void           SetYUnit(TCHAR *psz);
   const TCHAR*    GetYUnit()           {return m_Unit_y.GetText();};
   // Darstellungsparameter für Kurventext
   void           SetCurveLogFont(LOGFONT);
   LOGFONT        GetCurveLogFont()    {return m_CurveLogFont;};
   // Parameter für Kurven
   void           SetEditable(bool b)  {m_Parameter.editable = b;};
   bool           IsEditable()         {return m_Parameter.editable;};
   // Schnittstelle zu CARACALC
   void           Write(HANDLE);
   void           Read(HANDLE);
   // Helfer Funktionen
   void           SetOffset(double * pOff) {m_pdOffset = pOff;};
   void           CheckCurveColors(bool bTest=false);
   void           SaveStandards();
   void           SetStandards();
   void           SetNewDefaults();
   bool           CheckDeleteCurrentRange();
   void           DeleteCurrentRange();
   LRESULT        GetHelpCmd(CDC*, CPoint);
   
private:
	static int     SelectZoom(CListDlg*, int);
   void           InitPlot();

   // Plotbereich
   CTextLabel        m_PlotHeading;    // Plotüberschrift
   LOGFONT           m_CurveLogFont;   // Font für die Kurventexte
   bool              m_bHeadingPicked; // Punkte auf Referenzkurve beziehen
   // Viewbereich Linien
   CLabelContainer   m_Gridlines_x;    // Liste der Gitterlinien
   CLabelContainer   m_Gridlines_y;    // Liste der Gitterlinien
   WORD              m_wGridStep_x;    // mittlere Gitterschrittweite in logischen Koordinaten x
   WORD              m_wGridStep_y;    // mittlere Gitterschrittweite in logischen Koordinaten y
   COLORREF          m_GridLineColor;  // Gitterlinienfarbe
   // Viewbereich Texte
   CLabelContainer   m_Gridtext_x;     // Liste der Gitterbeschriftungstexte
   CLabelContainer   m_Gridtext_y;     // Liste der Gitterbeschriftungstexte
   CTextLabel        m_Unit_x;         // Einheit für die X-Achse
   CTextLabel        m_Unit_y;         // Einheit für die Y-Achse
   char              m_cRound_x;       // Anzahl der Nachkommastellen x
   char              m_cRound_y;       // Anzahl der Nachkommastellen y
   BSParameter       m_Parameter;      // Darstellungsparameter
   BSPlotDefaults    m_Defaults;       // Defaultparameter
   // View-Weltkoordinaten
   CPtrList          m_RangeList;      // Liste mit den Zoom-Bereichen
   POSITION          m_RangePos;       // aktueller Bereichsindex
   RANGE             m_DefaultRange;   // DefaultBereich
   double           *m_pdMarker;
   // View-Kurven
   CLabelContainer   m_Curves;         // Liste der enthaltenen Kurven
   bool              m_bReferenceOn;   // Punkte auf Referenzkurve beziehen
   CCurveLabel      *m_pRefCurve;      // Zeiger auf Referenzkurve
   double           *m_pdOffset;
   
public:
   static SStandardPlotSettings gm_StdPltSets;
};

#endif // !defined(AFX_PLOTLabel_H__FF559990_470A_11D1_9DB9_B051418EA04A__INCLUDED_)
