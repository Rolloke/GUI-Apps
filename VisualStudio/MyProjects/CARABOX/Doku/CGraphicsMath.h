#ifndef _CGRAPHICSMATH_
#define _CGRAPHICSMATH_

/*******************************************************************************

                                  CGraphicsMath
                                  -------------

      statische Klasse als Funktionssammlung für Mathematische Funktionen
      die sich mit Graphik etc. beschäftigen.
      Viele andere Klassen wie z.B. CLabel werden zusätzlich von dieser
      Klasse abgeleitet, damit sie diese Funktionen verwenden können

                  (C) 1998-2001 ELAC Technische Software GmbH

                         VERSION 1.3 Stand 12.07.2001



                                                   programmed by Oliver Wesnigk
*******************************************************************************/

#ifndef _FPOINT_
#define _FPOINT_

struct FPOINT
{
   float x;
   float y;
};

#endif

#ifndef _DPOINT_
#define _DPOINT_

struct DPOINT                                    // neu 16.03.2000 für z.B. Podeste
{
   double x;
   double y;
};

#endif

class CGraphicsMath
{
protected:                                       // diese Funktionen sollen nur abgeleitete Klassen verwenden

   static int      _fastcall Rounding(double);   // Rundung von double nach int
   static POINT    _stdcall  Rounding(FPOINT&);  // Rundung von FPOINT nach POINT
   static POINT    _stdcall  Rounding(DPOINT&);  // Rundung von DPOINT nach POINT (neu 16.03.2000)
   static double   _stdcall  GetAngle(int,int);  // Winkel in RAD berechnen vom Vektor x,y
                                                 // Winkel in RAD berechnen vom Vektor p1-p2
   static double   _stdcall  GetAngle(POINT&,POINT&);
   static double   _stdcall  GetAngle(FPOINT&,FPOINT&); // neu 15.3.2000
   static double   _stdcall  GetAngle(DPOINT&,DPOINT&);

                                                 // Winkel RAD nach DEG als integer * 10 (0.1°)
   static int      _stdcall  RadToTenthDeg(double);
                                                 // Länge einer Strecke berechnen
   static double   _stdcall  LineSectionLength(const POINT& ,const POINT&);
   static double   _stdcall  LineSectionLength(const DPOINT&,const DPOINT&);

                                                 // Abstand eines Punkte zu einer Strecke bestimmen
   static  int     _stdcall  LineSectionPointDistance(const POINT&,const POINT&,const POINT&,int);
                                                 // einen Vektor drehen (POINT,RAD)
   static POINT    _stdcall  RotatePoint(POINT,double);
   static FPOINT   _stdcall  RotatePoint(FPOINT,double); // neu 15.03.2000
   static DPOINT   _stdcall  RotatePoint(DPOINT,double); // neu 16.03.2000

                                                 // einen Vektor drehen (x,y,RAD)
   static POINT    _stdcall  RotatePoint(int,int,double);
                                                 // Schnittpunkt von zwei Strecken bestimmen
   static int      _stdcall  LineSectionCutPoint(POINT&,POINT&,POINT&,POINT&,POINT&);
                                                 // zwei Strecken auf Schnitt prüfen
   static int      _stdcall  CheckLineSectionCut(POINT&,POINT&,POINT&,POINT&);
                                                 // teste ob Strecke das Rechteck schneidet
   static int      _stdcall  CheckLineCutsRect(POINT&,POINT&,RECT&);
                                                 // teste ob Polygon das Rechteck durchdringt
   static int      _stdcall  CheckPolygonCutsRect(POINT *,int,RECT&);
                                                 // Winkel zwischen zwei Strecken bestimmen in RAD
   static double   _stdcall  LineSectionAngle(POINT&,POINT&,POINT&);
                                                 // Strecke zeichnen (POINT,POINT)
   static void     _stdcall  DrawLine(HDC,POINT&,POINT&);
                                                 // eine Region zeichnen (vergrößert um eine int)
   static void     _fastcall DrawFullRegion(HDC,const RGNDATA *,int);
                                                 // teste ob Punkt in Region liegt welche um eine float vergrößert wird
   static bool     _fastcall PointInFullRegion(const RGNDATA*,FPOINT&,float);
                                                 // eine Region (HRGN) in eine lokalgespeicherte Region (RGNDATA*) verwandeln
   static RGNDATA* _stdcall  RegionToPointer(HRGN);
                                                 // ein Rechteck normalisieren
   static void     _fastcall NormalizeRect(RECT&,RECT&);
                                                 // eine Region um inflate Pixel vergrößern
   static void     _stdcall  InflateRegion(RGNDATA * pRegion,int inflate);
                                                 // teste ob rgn2 vollständig in rgn1 ist (dann true)
   static int      _stdcall  Region1Contains2(HRGN,HRGN);

   static bool     _stdcall  PtInPolygon(POINT*,int,POINT,RECT * prcBound=NULL);
   static bool     _stdcall  DoublePtInPolygon(POINT *,int,POINT&,POINT&,RECT *);

                                                 // DibSection aus Resource laden (hBitmap nach Dib)
   static BITMAPINFO * _stdcall GetBitmap8X8(HINSTANCE hInst,int nID);

   static int      _stdcall  SnapPointToLine(POINT l1,POINT l2,POINT test,DPOINT& snaped);

   static int      _stdcall  CheckLineSectionCut( DPOINT& A1, DPOINT& A2, DPOINT& B1, DPOINT& B2 );
   static int      _stdcall  PtInPolygonWithEdge(DPOINT*,int,DPOINT);

   static bool     _stdcall  Trapezoid(DPOINT& w,DPOINT& f,DPOINT& t,DPOINT& c);

   static int      _stdcall  CheckKonvex( DPOINT& A1, DPOINT& A2, DPOINT& A3 );

   static bool     _stdcall  ClipLine(POINT & p1,POINT & p2,RECT * rc);


private:
   static bool      inline   Intersect (POINT&,POINT&,POINT&,POINT&);

// Members

public:                                          // die Folgenden Daten werden von mehreren Modulen geändert 
                                                 // (CScrollView,CMagnifierView,CRectDialog,CFullView etc.)

   static int    gm_nPixel;                      // 1 Pixel in logischen Koordinaten wird von ScrollView eingestellt durch PrepareDC
   static int    gm_nPickwidth;                  // der Fangbereich in logischen Koordinaten
   static bool   gm_bDoClipping;                 // Flag für DrawFullRegion ob Rechtecke geclipt werden sollen (WIN-Bug bei Metafile)

private:

   static double gm_dAngle;                      // Cache für RotateDouble (um zuviele Sin,Cos Berechnungen zu vermeiden)
   static double gm_dSin;
   static double gm_dCos;
};
#endif
