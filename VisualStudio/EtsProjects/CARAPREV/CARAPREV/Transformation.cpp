// Transformation.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Transformation.h"
#include "Curve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#include "curve.h"
/////////////////////////////////////////////////////////////////////////////
// CTransformation

CTransformation::CTransformation()
{
//   TRACE("CTransformation::Constructor()\n");
   m_View         = CRect(0,0,0,0);
   m_bChanged     = false;
   m_bLinear      = true;
   m_bClip        = false;
   Scale(1.0, 1.0, m_flTran);
   m_flInvt[0][0] = 1.0, m_flInvt[0][1] = 0.0;
   m_flInvt[1][0] = 1.0, m_flInvt[1][1] = 0.0;
   RANGE range    = {0.0, 1.0 ,0.0 , 1.0};
   m_Range        = range;
   m_ClippRange   = range;
   memset(&m_OldPoint  , 0, sizeof(DPOINT));
}

CTransformation::~CTransformation()
{
}

/***************************************************************************************
* Name   : SetView                                                                     *
* Zweck  : Setzt den View in logischen Koordinaten für die Transformation und das      *
*          Klippen.                                                                    *
* Aufruf : SetView(r);                                                                 *
* Parameter :                                                                          *
* r     (E) : Clipprechteck                                                   (CRect*) *
****************************************************************************************/
void CTransformation::SetView(CRect *r)
{
   m_View     = *r;
   m_bChanged = true;
   m_bLinear  = true;
}

/***************************************************************************************
* Name   : SetRange                                                                    *
* Zweck  : Setzt den Bereich in Weltkoordinaten für die Transformation                 *
* Aufruf : SetRange(r);                                                                *
* Parameter :                                                                          *
* r     (E) : Weltkoordinatenbereich                                         (LPRANGE) *
* Funktionswert: (true, false)                                               (bool)    *
****************************************************************************************/
bool CTransformation::SetRange(LPRANGE r)
{
   ASSERT((r->x2 > r->x1) && (r->y2 > r->y1));
   if (((r->x2 - r->x1) < TR_EPS)||((r->y2 - r->y1) < TR_EPS)) return false;
   m_Range      = *r;
   m_bChanged   = true;
   m_bLinear    = true;
   return true;
}

/***************************************************************************************
* Name   : SetRange                                                                    *
* Zweck  : Setzt den Bereich in Weltkoordinaten für die Transformation                 *
* Aufruf : SetView(x1, x2, y1, y2);                                                    *
* Parameter :                                                                          *
* x1, x2, y1, y2 (E): Weltkoordinatenbereich                                 (double)  *
* Funktionswert: (true, false)                                               (bool)    *
****************************************************************************************/
bool CTransformation::SetRange(double x1, double x2, double y1, double y2)
{
   RANGE range;
   range.x1 = x1;
   range.x2 = x2;
   range.y1 = y1;
   range.y2 = y2;
   return SetRange(&range);
}

/***************************************************************************************
* Name   : SetClippRange                                                               *
* Zweck  : Setzt den Klippingbereich in Weltkoordinaten                                *
* Aufruf : SetClippRange(r);                                                           *
* Parameter :                                                                          *
* r     (E) : Weltkoordinatenbereich                                         (LPRANGE) *
* Funktionswert: (true, false)                                               (bool)    *
****************************************************************************************/
bool CTransformation::SetClippRange(LPRANGE r)
{
   if ((r->x2 < r->x1) || (r->y2 < r->y1)) return false;
   if ((fabs(r->x2 - r->x1) < TR_EPS)||(fabs(r->y2 - r->y1) < TR_EPS)) return false;
   m_ClippRange = *r;
   return true;
}

/***************************************************************************************
* Name   : SetClippRange                                                               *
* Zweck  : Setzt den Klippingbereich in Weltkoordinaten                                *
* Aufruf : SetClippRange(x1, x2, y1, y2);                                              *
* Parameter :                                                                          *
* x1, x2, y1, y2 (E): Weltkoordinatenbereich                                 (double)  *
* Funktionswert: (true, false)                                               (bool)    *
***************************************************************************************/
bool CTransformation::SetClippRange(double x1, double x2, double y1, double y2)
{
   RANGE range;
   range.x1 = x1;
   range.x2 = x2;
   range.y1 = y1;
   range.y2 = y2;
   return SetClippRange(&range);
}


// 2D-Funktionen
/************************************************************************
* Name   : Translate                                                    *
* Zweck  : Erzeugen einer Translationsmatrix                            *
* Aufruf : Translate(dx,dy,ma)                                          *
* Parameter    :                                                        *
* dx       (E) : Verschiebungin x-Richtung                      (double)*
* dy       (E) : Verschiebungin y-Richtung                      (double)*
* ma       (A) : Ausgangsmatrix                   (double[TRM2D][TRM2D])*
************************************************************************/
void CTransformation::Translate(double dx, double dy, double ma[TRM2D][TRM2D])
{
   int i=0,j=0;
   ma[i][j++]= 1.0, ma[i][j++]= 0.0, ma[i++][j]=  dx;
   j=0;
   ma[i][j++]= 0.0, ma[i][j++]= 1.0, ma[i++][j]=  dy;
   j=0;
   ma[i][j++]= 0.0, ma[i][j++]= 0.0, ma[i++][j]= 1.0;
}

/************************************************************************
* Name   : Scale                                                        *
* Zweck  : Erzeugen einer Scalierungsmatrix                             *
* Aufruf : Scale(sx,sy,ma)                                                        *
* Parameter    :                                                        *
* sx       (E) : Scalierungsfaktor in x-Richtung                (double)*
* sy       (E) : Scalierungsfaktor in y-Richtung                (double)*
* ma       (A) : Ausgangsmatrix                   (double[TRM2D][TRM2D])*
************************************************************************/
void CTransformation::Scale(double sx, double sy, double ma[TRM2D][TRM2D])
{
   int i=0,j=0;
   ma[i][j++]=  sx, ma[i][j++]= 0.0, ma[i++][j]= 0.0;
   j=0;
   ma[i][j++]= 0.0, ma[i][j++]=  sy, ma[i++][j]= 0.0;
   j=0;
   ma[i][j++]= 0.0, ma[i][j++]= 0.0, ma[i++][j]= 1.0;
}

/************************************************************************
* Name   : Shear                                                        *
* Zweck  : Erzeugen einer Scheermatrix                                  *
* Aufruf : Shear(tx,ty,ma)                                              *
* Parameter    :                                                        *
* tx       (E) : Scheerfaktor in x-Richtung                     (double)*
* ty       (E) : Scheerfaktor in y-Richtung                     (double)*
* ma       (A) : Ausgangsmatrix                   (double[TRM2D][TRM2D])*
************************************************************************/
void CTransformation::Shear(double tx, double ty, double ma[TRM2D][TRM2D])
{
   int i=0,j=0;
   ma[i][j++]= 1.0, ma[i][j++]=  tx, ma[i++][j]= 0.0;
   j=0;
   ma[i][j++]=  ty, ma[i][j++]= 1.0, ma[i++][j]= 0.0;
   j=0;
   ma[i][j++]= 0.0, ma[i][j++]= 0.0, ma[i++][j]= 1.0;
}

/************************************************************************
* Name   : Rotate                                                       *
* Zweck  : Erzeugen einer Rotationsmatrix                               *
* Aufruf : Rotate(phi, ma)                                              *
* Parameter    :                                                        *
* phi      (E) : Drehwinkel in [rad].                          (double) *
* ma       (A) : Ausgangsmatrix                   (double[TRM2D][TRM2D])*
************************************************************************/
void CTransformation::Rotate(double phi, double ma[TRM2D][TRM2D])
{
   CFloatPrecision _fp;
   double sinphi = sin(phi);
   double cosphi = cos(phi);
   int i=0,j=0;
   ma[i][j++]=cosphi, ma[i][j++]=-sinphi, ma[i++][j]= 0.0;
   j=0;
   ma[i][j++]=sinphi, ma[i][j++]= cosphi, ma[i++][j]= 0.0;
   j=0;
   ma[i][j++]=   0.0, ma[i][j++]=    0.0, ma[i++][j]= 1.0;
}

RANGE CTransformation::MakeRange(DPOINT p1, DPOINT p2)
{
   RANGE r;
   if (p1.x < p2.x) {r.x1 = p1.x; r.x2 = p2.x; /* TRACE("x:Ok, ");     */}
   else             {r.x1 = p2.x; r.x2 = p1.x; /* TRACE("x:Swaped, "); */}
   if (p1.y < p2.y) {r.y1 = p1.y; r.y2 = p2.y; /* TRACE("y:Ok\n");     */}
   else             {r.y1 = p2.y; r.y2 = p1.y; /* TRACE("y:Swaped\n"); */}
   return r;
}

/************************************************************************
* Name   : SetTran                                                      *
* Zweck  : Setzen der Transformation der Member Variablen m_flTran      *
* Aufruf : SetTran(me, how)                                             *
* Parameter :                                                           *
* me    (E) : Eingangsmatrix                      (double[TRM2D][TRM2D])*
* how   (E) : TR_PRECON  : m_flTran = me x m_flTran            (int)    *
*             TR_POSTCON : m_flTran = m_flTran x me                     *
*             TR_REPLACE : m_flTran = me                                *
************************************************************************/
void CTransformation::SetTran(double me[TRM2D][TRM2D], int how)
{
   double ma[TRM2D][TRM2D];
   switch(how)
   {
      case TR_PRECON  :
         Compose_matr(me, m_flTran, ma);
         memcpy(m_flTran, ma, 9 * sizeof(double));
         break;
        case TR_POSTCON :
         Compose_matr(m_flTran, me, ma);
         memcpy(m_flTran, ma, 9 * sizeof(double));
         break;
        case TR_REPLACE :
         memcpy(m_flTran, me, 9 * sizeof(double));
         break;
   }
   if ((m_flTran[2][0]==0.0) && (m_flTran[2][1]==0.0) && (m_flTran[2][2]==1.0)) m_bLinear  = true;
   else  m_bLinear  = false;
   m_bChanged = false;
}

/************************************************************************
* Name   : Cr_trans_matr                                                *
* Zweck  : Berechnen der 2D-Transformationsmatrix. Das Ergebnis wird    *
*          Member Varablen m_flTran zugewiesen.                         *
* Aufruf : Cr_trans_matr(fx, fy, sclx, scly, phix, phiy, dx, dy);       *
* Parameter :                                                           *
* fx    (E) : Rotationspunkt x-Komponente                       (double)*
* fy    (E) : Rotationspunkt y-Komponente                       (double)*
* sclx  (E) : Scalierungsfaktor in x-Richtung                   (double)*
* scly  (E) : Scalierungsfaktor in y-Richtung                   (double)*
* phi   (E) : Drehwinkel um die x-Achse [rad]                   (double)*
* dx    (E) : Verschiebung in x-Richtung                        (double)*
* dy    (E) : Verschiebung in y-Richtung                        (double)*
************************************************************************/
void CTransformation::Cr_trans_matr(double fx, double fy, double sclx, double scly, double phi, double dx, double dy)
{
   double m1[TRM2D][TRM2D],m2[TRM2D][TRM2D],m3[TRM2D][TRM2D],m4[TRM2D][TRM2D];

   Scale(sclx,scly,m1);
   Translate(-fx, -fy, m2);
   Compose_matr(m1,m2,m3);
   Rotate(phi, m1);
   Compose_matr(m1,m3,m4);
   Translate(fx+dx,fy+dy,m2);
   Compose_matr(m2,m4,m_flTran);
   if ((m_flTran[2][0]==0.0) && (m_flTran[2][1]==0.0) && (m_flTran[2][2]==1.0)) m_bLinear  = true;
   else  m_bLinear  = false;
   m_bChanged = false;
}

/******************************************************************************
* Name   : SetDevicetran                                                      *
* Zweck  : Setzen einer Geraetetransformation von Weltkoordinaten in Pixel-   *
*          koordinaten. Das Ergebnis wird der Member Varablen m_flTran        *
*          zugewiesen. Zusätzlich wird die inverse Matrix berechnet.          *
* Aufruf : Set_Devicetran(r, x1, x2, y1, y2);                                 *
* Parameter    :                                                              *
* r        (E) : Darstellungsbereich in Logischen Koordinaten     (CRect*)    *
* x1, x2   (E) : untere und obere Grenze horizontal               (double)    *
* y1, y2   (E) : untere und obere Grenze vertikal                 (double)    *
* Funktionswert : true, false                                     (bool)      *
******************************************************************************/
bool CTransformation::SetDevicetran(CRect *r, double x1, double x2, double y1, double y2)
{
   RANGE ra = {x1, x2, y1, y2};

   if (!SetRange(&ra)) return false;
   SetView(r);
   return SetDevicetran();
}

bool CTransformation::SetDevicetran()
{
   if (!m_bChanged) return true;

   double sx, sy;
   double m1[TRM2D][TRM2D];
   double m2[TRM2D][TRM2D];
                                                               // Bereich zum Nullpunkt schieben
   Translate(-m_Range.x1, -m_Range.y1, m_flTran);

   sx = (m_Range.x2   - m_Range.x1);                           // Skalierungsfaktor berechnen
   if (sx == 0.0)               return false;
   sx = (m_View.right - m_View.left) / sx;

   sy = (m_Range.y2   - m_Range.y1);
   if (sy==0.0)                 return false;
   sy = (m_View.bottom - m_View.top) / sy;

   Scale(sx, sy, m1);
   Compose_matr(m1, m_flTran, m2);

   Translate(m_View.left, m_View.top, m1);                     // View zum Nullpunkt Schieben
   Compose_matr(m1, m2, m_flTran);

   m_bLinear  = true;
   makeInverseMatrix();                                        // Inverse der Matrix berechnen

   m_bChanged = false;                                         // Änderungen angepaßt
   return true;
}

/*********************************************************************************
* Name   : makeInverseMatrix                                                     *
* Zweck  : Berechnung der Inversen Transformationsmatrix zur Rücktransformation  *
*          der Gerätekoordinaten in Weltkoordinaten. Diese gilt nur für die      *
*          lineare Gerätetransformation, die durch SetDevicetran erstellt wird   *
*          diese Funktion wird nur von SetDevicetran aufgerufen. Die Inverse     *
*          Transformationsmatrix hat die  Dimension 2 * 2.                       *
* Aufruf : makeInverseMatrix();                                                  *
* Parameter    : keine                                                           *
*********************************************************************************/
void CTransformation::makeInverseMatrix()
{
   CFloatPrecision _fp;
   if (m_flTran[0][0]!=0.0)
   {
      m_flInvt[0][0] =  1.0            / m_flTran[0][0];
      m_flInvt[0][1] = -m_flTran[0][2] * m_flInvt[0][0];
   }

   if (m_flTran[1][1]!=0.0)
   {
      m_flInvt[1][0] =  1.0            / m_flTran[1][1];
      m_flInvt[1][1] = -m_flTran[1][2] * m_flInvt[1][0];
   }
}

/******************************************************************************
* Name   : Moveto                                                             *
* Zweck  : Setzen des aktuellen Cursors in Weltkoordinaten                    *
* Aufruf : Moveto(x, y);                                                      *
* Parameter    :                                                              *
* x, y     (E) : X-Y-Koordinate des Punktes                        (double)   *
* Funktionswert : true, false                                      (bool)     *
******************************************************************************/
bool CTransformation::Moveto(CDC* pDC, double x, double y)
{
   DPOINT p ={x, y};
   return Moveto(pDC, &p);
}

bool CTransformation::Moveto(CDC* pDC, DPOINT *pp)
{
   CPoint p;

   if ((!pDC)||(m_bChanged)) return false;                     // Wenn kein Device Context übergeben wurde
                                                                                                                                // oder keine Transformation gesetzt wurde
   m_OldPoint = *pp;
   TransformPoint(&m_OldPoint, &p);
   pDC->MoveTo(p);
   return true;
}

/******************************************************************************
* Name   : Lineto                                                             *
* Zweck  : Zeichnen einer Linie vom aktuellen Cursor zum Punkt (x, y) auf dem *
*          Gerätekontext des Objekts und der Transformation in der Matrix     *
*          (m_flTran).                                                        *
* Aufruf : Lineto(x, y);                                                      *
* Parameter    :                                                              *
* x, y     (E) : X-Y-Koordinate des Punktes                        (double)   *
* Funktionswert : true, false                                      (bool)     *
******************************************************************************/
bool CTransformation::Lineto(CDC* pDC, double x, double y)
{
   DPOINT p =  {x, y};
   return Lineto(pDC, &p);
}

bool CTransformation::Lineto(CDC* pDC, DPOINT* pp)
{
   if ((!pDC)||(m_bChanged)) return false;                     // Wenn kein Device Context übergeben wurde
                                                               // oder keine Transformation gesetzt wurde
   CPoint   p;                                                 // kann auch nichts ausgegeben werden
   DPOINT   newpoint  = *pp;                                   // newpoint belegen
   DPOINT   temppoint = newpoint;
   int      c         = 1;                                     // für NoClip auf "Linie innerhalb des Bereichs" setzen
   
   if (m_bClip) c = ClippLine(&m_OldPoint, &newpoint);         // Clippen, wenn erwünscht
   if (c & 0x08)
      return true;
   if (c & 0x01)                                               // Ist die Linie Innerhalb des Bereichs ?
   {
      if (c&0x02)                                              // wurde p1 geklippt ?
      {
         TransformPoint(&m_OldPoint, &p);                      // p1 neu ausrechnen und setzen
         pDC->MoveTo(p);
      }
      TransformPoint(&newpoint, &p);                           // p2 ausrechnen und setzen
      pDC->LineTo(p);
   }
   m_OldPoint = temppoint;                                     // neuen Punkt in m_OldPoint speichern
   return true;
}

/******************************************************************************
* Name   : Line                                                               *
* Zweck  : Zeichnen einer Linie von Punkt 1 zu Punkt 2 auf dem Gerätekontext  *
*          des Objekts (m_DC) und der Transformation in der Matrix (m_flTran).*
* Aufruf : Line(hdc, tran, x1, y1, x2, y2);                                   *
* Parameter    :                                                              *
* hdc      (E) : Handle des Ausgabegeraetes                      (HDC)        *
* tran     (E) : Transformationsmatrix                           (double[][]) *
* x1, y1   (E) : X-Y-Koordinate des Punktes 1                    (double)     *
* x2, y2   (E) : X-Y-Koordinate des Punktes 2                    (double)     *
* Funktionswert : true , false                                   (bool)       *
******************************************************************************/
bool CTransformation::Line(CDC* pDC, double x1, double y1, double x2, double y2)
{
   if (!Moveto(pDC, x1, y1)) return false;
   return Lineto(pDC, x2, y2);
}

/*********************************************************************************
* Name   : Rectangle                                                             *
* Zweck  : Zeichnen eines Rechtecks - definiert durch left, top, right, bottom - *
*          auf dem Gerätekontext des Objekts (m_DC) und der Transformation in    *
*          der Matrix (m_flTran).                                                *
* Aufruf : Rectangle(left, top, right, bottom)                                   *
* Parameter         :                                                            *
* left, top     (E) : linke, obere Ecke des Rechtecks                   (double) *
* right, bottom (E) : rechte, untere Ecke des Rechtecks                 (double) *
* Funktionswert : true, false                                           (bool)   *
*********************************************************************************/
bool CTransformation::Rectangle(CDC* pDC, double left, double top, double right, double bottom)
{
   bool rValue = true;
   rValue &= Moveto(pDC, left,  top);
   rValue &= Lineto(pDC, right, top);
   rValue &= Lineto(pDC, right, bottom);
   rValue &= Lineto(pDC, left,  bottom);
   rValue &= Lineto(pDC, left,  top);
   return rValue;
}

/*********************************************************************************
* Name   : Polyline                                                              *
* Zweck  : Zeichnen einer Polyline - definiert durch die Punkte in den Arrays    *
*          (px, py) mit der Anzahl numpoints - auf dem Gerätekontext des Objekts *
*              (m_DC) und der Transformation in der Matrix (m_flTran).           *
* Aufruf : Polyline(CDC* pDC, int numpoints, double *px, double *py)             *
* Parameter    :                                                                 *
* numpoints(E) : Anzahl der Punkte                                 (int)         *     
* px, py   (E) : Array der X-Y-Koordinaten der Punkte der Polyline (double*)     *
* Funktionswert : true, false                                      (bool)        *
********************************************************************************/
bool CTransformation::Polyline(CDC* pDC, int numpoints, double *px, double *py)
{
   int i;
   Moveto(pDC, px[0], py[0]);

   for (i=1; i<numpoints; i++)
   {
      if (!Lineto(pDC, px[i], py[i]))
      {
         TRACE("Error Lineto");
         return false;
      }
   }
   return true;
}
// das gleiche fuer Arrays des Typs DPOINT
bool CTransformation::Polyline(CDC* pDC, int numpoints, LPDPOINT c)
{
   int i;
   Moveto(pDC, &c[0]);

   for (i=1; i<numpoints; i++)
   {
      Lineto(pDC, &c[i]);
   }
   return true;
}
// das gleiche fuer float Arrays px, py
bool CTransformation::Polyline(CDC* pDC, int numpoints, float *px, float *py)
{
   int i;
   if (!Moveto(pDC, (double)px[0], (double)py[0])) return false;

   for (i=1; i<numpoints; i++)
   {
      if (!Lineto(pDC, (double)px[i], (double)py[i]))
      {
         TRACE("Error Lineto");
         return false;
      }
   }
   return true;
}

/******************************************************************************************
* Name   :  ClipLine                                                                      *
* Zweck  :  Clippt eine Linie in einem durch m_ClippRange vorgegebenen Clipprechteck im   *
*           Weltkoordinatenbereich.                                                       *
* Aufruf : ClipLine(p1, p2);                                                              *
* Parameter :                                                                             *
* p1, p2 (E): Anfangs und Endpunkte der Linie                                 (LPDPOINT)  *
* Funktonswert:                                                                           *
* Bit 0 : 1 = Linie innerhalb,  0 = Linie außerhalb des Cliprechtecks         (int)       *
* Bit 1 : 1 = p1 geclipt, 0 = p1 nicht geclipt                                            *
* Bit 2 : 1 = p2 geclipt, 0 = p2 nicht geclipt                                            *
* Bit 4 : 1 = Fehler beim clippen                                                         *
******************************************************************************************/
#define TR_LEFT    0x08
#define TR_TOP     0x04
#define TR_RIGHT   0x02
#define TR_BOTTOM  0x01
int CTransformation::ClippLine(LPDPOINT pp1, LPDPOINT pp2)
{
   int    ltrb1, ltrb2, count=0;
   double dx, dy;
   int    c = 1;
   do
   {
      ltrb1 = ltrb2 = 0;
      if      (pp1->x < m_ClippRange.x1) ltrb1 |= TR_LEFT;
      else if (pp1->x > m_ClippRange.x2) ltrb1 |= TR_RIGHT;
      if      (pp1->y < m_ClippRange.y1) ltrb1 |= TR_BOTTOM;
      else if (pp1->y > m_ClippRange.y2) ltrb1 |= TR_TOP;

      if      (pp2->x < m_ClippRange.x1) ltrb2 |= TR_LEFT;
      else if (pp2->x > m_ClippRange.x2) ltrb2 |= TR_RIGHT;
      if      (pp2->y < m_ClippRange.y1) ltrb2 |= TR_BOTTOM;
      else if (pp2->y > m_ClippRange.y2) ltrb2 |= TR_TOP;

      if ((ltrb1&ltrb2)!=0) return 0;
      if ((ltrb1|ltrb2)==0) return c;

      if (!count)
      {
         dx = pp2->x - pp1->x;
         dy = pp2->y - pp1->y;
      }

      CFloatPrecision _fp;

      if      (ltrb1&TR_LEFT)
      {
         pp1->y = pp1->y + (m_ClippRange.x1 - pp1->x) * dy / dx;
         pp1->x = m_ClippRange.x1;
         if (ltrb2 & (TR_LEFT|TR_RIGHT)) ltrb2 &= (TR_LEFT|TR_RIGHT);
         c |= 0x02;
      }
      else if (ltrb1&TR_TOP)
      {
         pp1->x = pp1->x + (m_ClippRange.y2 - pp1->y) * dx / dy;
         pp1->y = m_ClippRange.y2;
         if (ltrb2 & (TR_TOP|TR_BOTTOM)) ltrb2 &= (TR_TOP|TR_BOTTOM);
         c |= 0x02;
      }
      else if (ltrb1&TR_RIGHT)
      {
         pp1->y = pp1->y + (m_ClippRange.x2 - pp1->x) * dy / dx;
         pp1->x = m_ClippRange.x2;
         if (ltrb2 & (TR_LEFT|TR_RIGHT)) ltrb2 &= (TR_LEFT|TR_RIGHT);
         c |= 0x02;
      }
      else if (ltrb1&TR_BOTTOM)
      {
         pp1->x = pp1->x + (m_ClippRange.y1 - pp1->y) * dx / dy;
         pp1->y = m_ClippRange.y1;
         if (ltrb2 & (TR_TOP|TR_BOTTOM)) ltrb2 &= (TR_TOP|TR_BOTTOM);
         c |= 0x02;
      }

      if      (ltrb2&TR_LEFT)
      {
         pp2->y = pp2->y + (m_ClippRange.x1 - pp2->x) * dy / dx;
         pp2->x = m_ClippRange.x1;
         c |= 0x04;
      }
      else if (ltrb2&TR_TOP)
      {
         pp2->x = pp2->x + (m_ClippRange.y2 - pp2->y) * dx / dy;
         pp2->y = m_ClippRange.y2;
         c |= 0x04;
      }
      else if (ltrb2&TR_RIGHT)
      {
         pp2->y = pp2->y + (m_ClippRange.x2 - pp2->x) * dy / dx;
         pp2->x = m_ClippRange.x2;
         c |= 0x04;
      }
      else if (ltrb2&TR_BOTTOM)
      {
         pp2->x = pp2->x + (m_ClippRange.y1 - pp2->y) * dx / dy;
         pp2->y = m_ClippRange.y1;
         c |= 0x04;
      }
   }
   while (count++ < 4);

//   ASSERT(false);
   TRACE("!!! Fehler beim Clippen !!!\n");
   TRACE("Bereich: (%f, %f), (%f, %f)\n",  m_ClippRange.x1, m_ClippRange.x2, m_ClippRange.y1, m_ClippRange.y2);
   TRACE("p1: (%f, %f),  p2: (%f, %f)\n", pp1->x, pp1->y, pp2->x, pp2->y);
   return 0x08;
}
#undef TR_LEFT
#undef TR_TOP
#undef TR_RIGHT
#undef TR_BOTTOM

/************************************************************************
* Name   : TransformPoint                                               *
* Zweck  : Transformieren eines Punktes vom :                           *
*          (1) Weltkoordinatensystem in das Gerätekoordinatensystem     *
*          (2) Gerätekoordinatensystem in das Weltkoordinatensystem     *
* Aufruf : TransformPoint(pi, po);                                      *
* Parameter (1):                                                        *
* pi       (E) : Eingangspunkt x,y                           (LPDPOINT) *
* po       (A) : Ausgangspunkt x,y                           (CPoint*)  *
* Parameter (2): !! gilt nur für eine lineare Transformation !!         *
*                (*) siehe makeInverseMatrix()                          *
* pi       (E) : Eingangspunkt x,y                           (CPoint*)  *
* po       (A) : Ausgangspunkt x,y                           (LPDPOINT) *
************************************************************************/
void CTransformation::TransformPoint(LPDPOINT pi, CPoint *po)
{
   double xo, yo;
   xo   = m_flTran[0][0] * pi->x + m_flTran[0][1] * pi->y + m_flTran[0][2];
   yo   = m_flTran[1][0] * pi->x + m_flTran[1][1] * pi->y + m_flTran[1][2];

   if (!m_bLinear)
   {
      double norm = m_flTran[2][0] * pi->x + m_flTran[2][1] * pi->y + m_flTran[2][2];
      if (norm!=1.0) // Normierung
      {
         xo /= norm;
         yo /= norm;
      }
   }

   xo -= (po->x = (long) xo);
   if (xo >= 0.5) po->x++;
   if (xo <=-0.5) po->x--;

   yo -= (po->y = (long)yo);
   if (yo >= 0.5) po->y++;
   if (yo <=-0.5) po->y--;
}

void CTransformation::TransformPoint(CPoint *pi, LPDPOINT po)
{
   if (m_bLinear)
   {
      po->x = m_flInvt[0][0] * pi->x + m_flInvt[0][1];
      po->y = m_flInvt[1][0] * pi->y + m_flInvt[1][1];
   }
   else
   {
      // evtl. Gleichung lösen Release 2.0
   }
}

/************************************************************************
* Name   : Compose_matr                                                 *
* Zweck  : Multiplikation von 3*3 Matrizen, Regel : ma = m1 x m2        *
* Aufruf : Compose_matr(m1,m2,ma)                                       *
* Parameter :                                                           *
* m1    (E) : Eingangsmatrix 1                    (double[TRM2D][TRM2D])*
* m1    (E) : Eingangsmatrix 2                    (double[TRM2D][TRM2D])*
* ma    (E) : Ausgangsmatrix                      (double[TRM2D][TRM2D])*
************************************************************************/
void CTransformation::Compose_matr(double m1[TRM2D][TRM2D], double m2[TRM2D][TRM2D], double ma[TRM2D][TRM2D])
{
   int i,j,k;
   CFloatPrecision _fp;
   for (i=0; i<TRM2D; i++)
   {
      for (j=0; j<TRM2D; j++)
      {
         k=0;
         ma[i][j]=m1[i][k]*m2[k][j];
         for (k=1; k<TRM2D; k++)
         {
            ma[i][j]+=m1[i][k]*m2[k][j];
         }
      }
   }
}

void CTransformation::MatrixToXFORM(double m[TRM2D][TRM2D], XFORM &xf)
{
   xf.eM11 = (float)m[0][0];
   xf.eM21 = (float)m[0][1];
   xf.eDx  = (float)m[0][2];

   xf.eM12 = (float)m[1][0];
   xf.eM22 = (float)m[1][1];
   xf.eDy  = (float)m[1][2];
}

void CTransformation::XFORMToMatrix(XFORM &xf, double m[TRM2D][TRM2D])
{
   m[0][0] = xf.eM11;
   m[0][1] = xf.eM21;
   m[0][2] = xf.eDx;

   m[1][0] = xf.eM12;
   m[1][1] = xf.eM22;
   m[1][2] = xf.eDx;

   m[2][0] = 0;
   m[2][1] = 0;
   m[2][2] = 1;
}
