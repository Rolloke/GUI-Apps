#if !defined(AFX_TRANSFORMATION_H__BB1FF723_E172_11D0_9DB9_B051418EA04A__INCLUDED_)
#define AFX_TRANSFORMATION_H__BB1FF723_E172_11D0_9DB9_B051418EA04A__INCLUDED_
// Transformation.h : Header-Datei
//

 #include <afx.h>
 #include <cmath>

 #define TRM2D          3                                      // defines
 #define TR_POSTCON     1                                      // Modes zum setzen der Transformationsmatrix
 #define TR_PRECON      2
 #define TR_REPLACE     3
 #define TR_EPS      1e-13                                     // Minimaler Weltkoordinatenbereich
 #define TR_E           2.71828182846
 #define TR_PI          3.14159265358979323846
 #define TR_RAD(x)      (x * TR_PI * 0.00555555555555555555)   // Winkelumrechnung in [rad]

 typedef struct DPOINT
 {
    double x, y;                                               // Punkt in Weltkoordinaten
 } *PDPOINT, NEAR *NPDPOINT, FAR *LPDPOINT;


 typedef struct RANGE
 {
    double x1, x2, y1, y2;                                     // Darzustellender Bereich in Weltkoordinaten
 } *PRANGE, NEAR *NPRANGE, FAR *LPRANGE;

/////////////////////////////////////////////////////////////////////////////
// Fenster CTransformation

class AFX_EXT_CLASS CTransformation
{
// Konstruktion
public:
   CTransformation();
   virtual ~CTransformation();
                                                             
public:
   virtual void _fastcall TransformPoint(LPDPOINT, CPoint *);            // Punkt Transformation
   virtual void _fastcall TransformPoint(CPoint *, LPDPOINT);
   virtual bool  SetRange(LPRANGE);                            // und Range
   virtual RANGE GetRange() {return m_Range;};
   // Funktionen zum Setzen einer linearen Gerätetransformation
   void  SetView(CRect *);                                   
   CRect GetView()  {return m_View; };
   bool  SetRange(double x1, double x2, double y1, double y2);
   bool  SetDevicetran();                                      
   bool  SetDevicetran(CRect *, double, double, double, double);
   void  Cr_trans_matr(double, double, double, double, double, double, double);
   // Clipping Funktionen
   void  SetClip(bool c) {m_bClip = c;};
   bool  GetClip() {return m_bClip;};
   bool  SetClippRange(LPRANGE r);
   bool  SetClippRange(double x1, double x2, double y1, double y2);
   RANGE GetClippRange() {return m_ClippRange;};
   int _fastcall ClippLine(LPDPOINT, LPDPOINT);
   // 2D-Zeichenfunktionen 
   bool _fastcall Moveto(   CDC*, double, double);
   bool _fastcall Lineto(   CDC*, double, double);
   bool _fastcall Moveto(   CDC*, DPOINT *c);
   bool _fastcall Lineto(   CDC*, DPOINT *c);
   bool  Polyline( CDC*, int, LPDPOINT c);

   bool _fastcall Line(     CDC*, double, double, double, double);
   bool  Rectangle(CDC*, double, double, double, double);
   bool  Polyline( CDC*, int, double *, double *);
   bool  Polyline( CDC*, int, float *, float *);
   // Funktionen zum Setzen einer beliebigen Transformation
   void         SetTran(double m1[TRM2D][TRM2D], int how);
   static void  Translate(double, double, double ma[TRM2D][TRM2D]);
   static void  Scale(    double, double, double ma[TRM2D][TRM2D]);
   static void  Shear(    double, double, double ma[TRM2D][TRM2D]);
   static void  Rotate(   double, double ma[TRM2D][TRM2D]);
   static void  Compose_matr(double m1[TRM2D][TRM2D], double m2[TRM2D][TRM2D], double ma[TRM2D][TRM2D]);
   static RANGE MakeRange(DPOINT,  DPOINT);
	static void XFORMToMatrix(XFORM&, double m[TRM2D][TRM2D]);
	static void MatrixToXFORM(double m[TRM2D][TRM2D], XFORM&);

protected: 
   void  makeInverseMatrix();                      // erzeugen der Inversen Matrix nur für die lineare Gerätetransformation

protected:
   RANGE    m_Range;                               // Bereich in Weltkoordinaten
   RANGE    m_ClippRange;                          // Klippingbereich in Weltkoordinaten
   CRect    m_View;                                // Darstellungsbereich in logische Koordinaten
   bool     m_bChanged;                            // Indikator für View oder Range geändert
   bool     m_bClip;                               // Indikator ob geklippt wird
   DPOINT   m_OldPoint;                            // Alter Punkt, wird von MoveTo und LineTo gesetzt
   double   m_flTran[TRM2D][TRM2D];                // Transformationsmatrix
   double   m_flInvt[2][2];                        // Inverse dazu (*) !! siehe makeInverseMatrix !!
private:
   bool     m_bLinear;
};

/////////////////////////////////////////////////////////////////////////////
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_TRANSFORMATION_H__BB1FF723_E172_11D0_9DB9_B051418EA04A__INCLUDED_
