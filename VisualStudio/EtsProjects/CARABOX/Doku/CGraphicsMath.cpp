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
 
#include "CWindow.h"
#include "CGraphicsMath.h"
#include <math.h>


#define MK_PI 3.14159265358978                   // leiden nicht in Math.h definiert
#define EPS 1e-5                                 // für Tests mit DPOINT etc.

/*******************************************************************************

                        globale Variablen dieser Klasse

*******************************************************************************/

                                                 // Pixelgröße und Fangbereich
int  CGraphicsMath::gm_nPixel;
int  CGraphicsMath::gm_nPickwidth;
bool CGraphicsMath::gm_bDoClipping = true;       // Flag für DrawFullRegion ob geclipt werden soll (Win-Bug bei MetaFile)

                                                 // Winkel-Cache (Verhinderung von zuvielen Sin,Cos Funktionsaufrufen)
double CGraphicsMath::gm_dSin   = 0;
double CGraphicsMath::gm_dCos   = 1;
double CGraphicsMath::gm_dAngle = 0;


/*******************************************************************************
                              Rounding(double)

static public Function: runde double nach int besser und schneller als floor

*******************************************************************************/

int _fastcall CGraphicsMath::Rounding(double value)
{
   if(value== 0.0) return 0;
   else
   if(value > 0.0) return (((int)(value+value))+1)>>1;
   else            return -((((int)(-2.0*value))+1)>>1);
}


/*******************************************************************************
                              Rounding(FPOINT),(DPOINT)

static public Function: runde FPOINT nach POINT
                        runde DPOINT nach POINT

*******************************************************************************/

POINT _stdcall CGraphicsMath::Rounding(FPOINT& p)
{
   POINT e = {Rounding((double)p.x),Rounding((double)p.y)};

   return e;
}

POINT _stdcall CGraphicsMath::Rounding(DPOINT& p)
{
   POINT e = {Rounding( p.x ),Rounding( p.y )};

   return e;
}

/*******************************************************************************
                  GetAngle(POINT,POINT) auch FPOINT und DPOINT

static public Function: berechne den Winkel vom Vektor p2-p1

*******************************************************************************/

double _stdcall CGraphicsMath::GetAngle(POINT& from,POINT& to)
{
   return atan2(to.y-from.y,to.x-from.x);
}

double _stdcall CGraphicsMath::GetAngle(FPOINT& from,FPOINT& to)
{
   return atan2(to.y-from.y,to.x-from.x);
}

double _stdcall CGraphicsMath::GetAngle(DPOINT& from,DPOINT& to)
{
   return atan2(to.y-from.y,to.x-from.x);
}


/*******************************************************************************
                          RadToTenthDeg(double)

static public Function: erzeugen aus Winkel in RAD einen in DEG als int * 10
        
*******************************************************************************/

int _stdcall CGraphicsMath::RadToTenthDeg(double dAngle)
{
   return Rounding(1800*dAngle/MK_PI);
}


/*******************************************************************************
                   LineSectionLength(POINT,POINT) auch DPOINT

static public Function: berechne die länge einer Strecke

*******************************************************************************/

double _stdcall CGraphicsMath::LineSectionLength(const POINT& from,const POINT& to)
{
   int x,y;

   x = to.x - from.x;
   y = to.y - from.y;

   return _hypot(x,y);
}

double _stdcall CGraphicsMath::LineSectionLength(const DPOINT& from,const DPOINT& to)
{
   double x , y;

   x = to.x - from.x;
   y = to.y - from.y;

   return _hypot(x,y);
}

/*******************************************************************************
              LineSectionPointDistance(POINT&,POINT&,POINT&,int)

static public Function: berechne den abstand eines Punkte von einer Strecke
                        ist das Quadrat des Abstanden kleiner gleich max*max
                        gebe diesen Wert zurück, andernfals -1

*******************************************************************************/

int _stdcall CGraphicsMath::LineSectionPointDistance(const POINT& from,const POINT& to,const POINT& p,int max)
{
   POINT   a,b,c;
   int     scale,bqb,cqb;
   RECT    rc;
   int     ret = -1;                             // Returnwert defaultmäßig "Punkt nicht im Bereich um die Linie"

   rc.left   = from.x;                           // bilde normalisiertes Rechteck um die Linie
   rc.top    = from.y;
   rc.right  = to.x;
   rc.bottom = to.y;

   if(rc.left>rc.right ) swap(rc.left,rc.right );
   if(rc.top >rc.bottom) swap(rc.top ,rc.bottom);

   if(p.x < rc.left   - max) ;                   // Punkt im normalisierten Rechteck um die Linie ?
   else
   if(p.x > rc.right  + max) ;
   else
   if(p.y < rc.top    - max) ;
   else
   if(p.y > rc.bottom + max) ;
   else                                          // Punkt ist im normalisierten Rechteck um die Linie
   {
      a.x = p.x - to.x;                          // Bilde zwei Vektoren (mit to.x,to.x als Ursprung)
      a.y = p.y - to.y;

      b.x = from.x - to.x;
      b.y = from.y - to.y;
                                                 // Projeziere a auf b = c
      scale = a.x*b.x + a.y*b.y;
      bqb   = b.x*b.x + b.y*b.y;

      c.x = MulDiv(b.x,scale,bqb);
      c.y = MulDiv(b.y,scale,bqb);

      c.x -= a.x;                                // bestimme den Abstandsvektor zur Linie
      c.y -= a.y;

      cqb = c.x*c.x + c.y*c.y;                   // berechne den Betrag von d (aber im Quadrat)
                                                 // teste ob der betrag im bereich von max*max liegt
      if (cqb <= max*max) ret = cqb;             // Ja, Entfernungsquadrat zurückgeben
   }

   return ret;
}


/*******************************************************************************
                           RotatePoint(POINT  ,double)
                                      (FPOINT ,double)
                                      (DPOINT ,double)
                                      (int,int,double)

static public Function: drehe den Vektor POINT um double in RAD
                        oder die anderen Typen

*******************************************************************************/

POINT _stdcall CGraphicsMath::RotatePoint(POINT p,double dAngle)
{
   if(dAngle!=gm_dAngle)
   {
      gm_dSin   = sin(dAngle);
      gm_dCos   = cos(dAngle);
      gm_dAngle = dAngle;
   }

   double dx = p.x;
   double dy = p.y;
   double dnew_x;

   dnew_x = dx*gm_dCos-dy*gm_dSin;
   dy     = dx*gm_dSin+dy*gm_dCos;
   dx     = dnew_x;

   POINT r={Rounding(dx),Rounding(dy)};

   return r;
}

FPOINT _stdcall CGraphicsMath::RotatePoint(FPOINT p,double dAngle) // neu 15.3.2000
{
   if(dAngle!=gm_dAngle)
   {
      gm_dSin   = sin(dAngle);
      gm_dCos   = cos(dAngle);
      gm_dAngle = dAngle;
   }

   double dx = p.x;
   double dy = p.y;
   double dnew_x;

   dnew_x = dx*gm_dCos-dy*gm_dSin;
   dy     = dx*gm_dSin+dy*gm_dCos;
   dx     = dnew_x;

   FPOINT r = {(float)dx,(float)dy};

   return r;
}

DPOINT _stdcall CGraphicsMath::RotatePoint(DPOINT p,double dAngle) // neu 16.3.2000
{
   if(dAngle!=gm_dAngle)
   {
      gm_dSin   = sin(dAngle);
      gm_dCos   = cos(dAngle);
      gm_dAngle = dAngle;
   }

   double dx = p.x;
   double dy = p.y;
   double dnew_x;

   dnew_x = dx*gm_dCos-dy*gm_dSin;
   dy     = dx*gm_dSin+dy*gm_dCos;
   dx     = dnew_x;

   DPOINT r = { dx , dy };

   return r;
}

POINT _stdcall CGraphicsMath::RotatePoint(int x,int y,double dAngle)
{
   if(dAngle!=gm_dAngle)
   {
      gm_dSin   = sin(dAngle);
      gm_dCos   = cos(dAngle);
      gm_dAngle = dAngle;
   }

   double dx = x;
   double dy = y;
   double dnew_x;

   dnew_x = dx*gm_dCos-dy*gm_dSin;
   dy     = dx*gm_dSin+dy*gm_dCos;
   dx     = dnew_x;

   POINT r={Rounding(dx),Rounding(dy)};

   return r;
}


/*******************************************************************************
           LineSectionCutPoint(POINT&,POINT&,POINT&,POINT&,POINT&)

static public Function: berechne den Schnittpunkt zweier Strecken
                        diese Funktion ist nur für Strecken geeignet die genau
                        einen Schnittpunkt haben                       

*******************************************************************************/

int _stdcall CGraphicsMath::LineSectionCutPoint(POINT& f1,POINT& t1,POINT& f2,POINT& t2,POINT& CutPoint)
{
   POINT  r1;
   POINT  r2;                                    // Richtungsvektoren der Geranden
   POINT  d; 
   int    det;
   int    deth;
   double n;

   r1.x = t1.x - f1.x;
   r1.y = t1.y - f1.y;

   r2.x = t2.x - f2.x;
   r2.y = t2.y - f2.y;

   det = r1.x*r2.y - r1.y*r2.x;

   if(det==0)
   {
      d.x  = r2.x-r1.x;
      d.y  = r2.x-r1.y;
      deth = d.x*r2.y-d.y*r2.x;

      if(deth==0) return 0;                      // die Strecken liegen aufeinander
      else        return 1;                      // die Strecken sind parallel

   }

   d.x = f2.x-f1.x;
   d.y = f2.y-f1.y;

   deth = d.x*r2.y-d.y*r2.x;

   if(deth==0)
   {
      CutPoint.x = f1.x;
      CutPoint.y = f1.y;
      return 5;                                  // die Stecken hängen aneinander an f1.x
   }
   else
   if(deth==det)
   {
      CutPoint.x = t1.x;
      CutPoint.y = t1.y;
      return 6;                                  // die Stecken hängen aneinander an t1.x
   }
   else        
   {
      n = ((double)deth) / det;

      CutPoint.x = Rounding(n*r1.x + f1.x);
      CutPoint.y = Rounding(n*r1.y + f1.y);
   
      if ((n > 0.0)&&(n < 1.0)) return 2;        // der Schnittpunkt ist in der Strecke (f1,t1)
      if (n < 0.0)              return 3;        // der Schnittpunkt liegt außerhalb des Startpunktes (f1)
      else                      return 4;        // der Schnittpunkt liegt außerhalb des Endpunktes   (t1)
   }
}


/********************************************************************/
/*                                                                  */
/*        Funktion zur Prüfung, ob die übergebenen Strecken         */
/*                A und B sich gegenseitig schneiden                */
/*                                                                  */
/*   Rückgabewerte:                                                 */
/*   0 -> kein Schnittpunkt                                         */
/*   1 -> Strecken hängen aneinander, Berührung am Anfang/Ende      */
/*   2 -> ein Streckenendpunkt berührt innerhalb der anderen Strecke*/
/*   3 -> 'echter' Schnittpunkt                                     */
/*   4 -> unendlich viele Schnittpunkte                             */
/*                                                                  */
/********************************************************************/

int _stdcall CGraphicsMath::CheckLineSectionCut( POINT& A1, POINT& A2, POINT& B1, POINT& B2 )
{
   int      nDetLambda, nDetNenner;
   int      ret;
   POINT    A21, B12, B1A1;

   A21.x = A2.x - A1.x;                          // Richtungsvektoren für Strecken A und B
   A21.y = A2.y - A1.y;                          // A1 und B1 sind die Anfangspunkte der Strecken
   B12.x = B1.x - B2.x;
   B12.y = B1.y - B2.y;

   B1A1.x = B1.x - A1.x;                         // Differenzvektor der Streckenanfangspunkte
   B1A1.y = B1.y - A1.y;
                                                 // Punktekoordinaten sind < 10000 !! (kein Überlauf)
   nDetNenner = A21.x*B12.y - A21.y*B12.x;       // Nenner-Determinante der Cramer-Regel
   nDetLambda = B1A1.x*B12.y - B1A1.y*B12.x;     // Zähler-Determinante für Lambda der Strecke A
   
   if( nDetNenner != 0 )                         // Strecken A und B sind NICHT parallel
   {
      bool   blambda   =true ,bmy   =true;       // true entspricht lamdba,my genau 0 oder 1
      bool   blambda021=false,bmy021=false;      // true entspricht lambda,my zwischen 0 und 1

      int nDetMy = A21.x*B1A1.y - A21.y*B1A1.x;  // Zähler-Determinante für My der Strecke B

                                                 // Lambda, My sind die Geradengleichungs-Parameter
                                                 // dlambda wird nicht genau 0 oder 1
      if((nDetLambda!=0)&&(nDetLambda!=nDetNenner))
      {
         double dLambda = ((double) nDetLambda)/((double) nDetNenner);
         blambda        = false;
         blambda021     = (dLambda>0.0)&&(dLambda<1.0);
      }

      if((nDetMy!=0)&&(nDetMy!=nDetNenner))
      {
         double dMy = ((double) nDetMy)/((double) nDetNenner);
         bmy        = false;
         bmy021     = (dMy>0.0)&&(dMy<1.0);
      }

      if(blambda&bmy)
         ret = 1;                                // die Strecken A und B hängen aneinander
      else if(blambda&bmy021)
         ret = 2;                                // ein Streckenende(lambda) berührt innerhalb der anderen Strecke
      else if(bmy&blambda021)
         ret = 2;                                // ein Streckenende(my) berührt innerhalb der anderen Strecke
      else if(bmy021&blambda021)
         ret = 3;                                // Schnittpunkt zwischen beiden Strecken
      else
         ret = 0;                                // kein Schnittpunkt
   }
   else                                          // Strecken A und B sind parallel
   {
      if( nDetLambda == 0 )                      // die Strecken A und B liegen auf der gleichen Geraden
      {
         int  axy1,axy2,bxy1,bxy2;
         
         if(abs(A21.x) > abs(A21.y))             // die Strecke A zeigt eher in x-Richtung
         {
            if(A1.x < A2.x) axy1=A1.x,axy2=A2.x;
            else            axy1=A2.x,axy2=A1.x;

            if(B1.x < B2.x) bxy1=B1.x,bxy2=B2.x;
            else            bxy1=B2.x,bxy2=B1.x;
         }
         else                                    // die Strecke A zeigt eher in y-Richtung oder hat 45°
         {
            if(A1.y < A2.y) axy1=A1.y,axy2=A2.y;
            else            axy1=A2.y,axy2=A1.y;

            if(B1.y < B2.y) bxy1=B1.y,bxy2=B2.y;
            else            bxy1=B2.y,bxy2=B1.y;
         }

         if((axy2 <bxy1)||(axy1 >bxy2)) ret = 0; // beide Strecken berühren sich nicht
         else
         if((axy2==bxy1)||(axy1==bxy2)) ret = 1; // beide Strecken hängen aneinander
         else                           ret = 4; // die Strecken A und B überlappen sich -> unendlich viele Schnittpunkt
      }
      else ret = 0;                              // die parallelen Strecken schneiden sich im Unendlichen
   }

   return (ret);
}


/*******************************************************************************
                       CheckLineCutsRect(POINT,POINT,RECT)

static public Function: Geht Linie durch das Rechteck: 1=JA,0=NEIN

*******************************************************************************/

int _stdcall  CGraphicsMath::CheckLineCutsRect(POINT& p1,POINT& p2,RECT& rc)
{
   POINT from1 = {rc.left  ,rc.top};
   POINT to1   = {rc.right ,rc.top};
   if(CheckLineSectionCut(p1,p2,from1,to1)) return 1;
   POINT from2 = {rc.left  ,rc.bottom};
   POINT to2   = {rc.right ,rc.bottom};
   if(CheckLineSectionCut(p1,p2,from2,to2)) return 1;
   POINT from3 = {rc.left  ,rc.top};
   POINT to3   = {rc.left  ,rc.bottom};
   if(CheckLineSectionCut(p1,p2,from3,to3)) return 1;
   POINT from4 = {rc.right ,rc.top};
   POINT to4   = {rc.right ,rc.bottom};
   if(CheckLineSectionCut(p1,p2,from4,to4)) return 1;

   return 0;
}


/*******************************************************************************
                     CheckPolygonCutsRect(POINT *,int,RECT&)

static public Function: Durchdringt Polygon das Recteck: 1=JA,0=NEIN

*******************************************************************************/

int _stdcall CGraphicsMath::CheckPolygonCutsRect(POINT * pPoints,int number,RECT& rc)
{
   int i;

   for (i=0;i<number-1;i++)
   {
      if(CheckLineCutsRect(pPoints[i],pPoints[i+1],rc)) return 1;
   }
   return 0;
}


/*******************************************************************************
                      LineSectionAngle(POINT&,POINT&,POINT&)

static public Function: berechne Winkel zwischen zwei zusammenhängenden
                        Strecken t1->b1->t2

*******************************************************************************/

double _stdcall CGraphicsMath::LineSectionAngle(POINT& b1,POINT& t1,POINT& t2)
{
   POINT r1;
   POINT r2;

   r1.x = t1.x - b1.x;
   r1.y = t1.y - b1.y;

   if((r1.x==0)&&(r1.y==0)) return 0;

   r2.x = t2.x - b1.x;
   r2.y = t2.y - b1.y;

   if((r2.x==0)&&(r2.y==0)) return 0;

   double zaehler = r1.x*r2.x + r1.y*r2.y;
   double nenner  = _hypot(r1.x,r1.y) * _hypot(r2.x,r2.y);

   return acos(zaehler / nenner);
}


/*******************************************************************************
                           DrawLine(HDC,POINT,POINT)

static public Function: zeichne die Strecke f->t (ich bin tippfaul) außerdem
                        spart es etwas Speicher

*******************************************************************************/

void _stdcall CGraphicsMath::DrawLine(HDC hdc,POINT& f,POINT& t)
{
   MoveToEx(hdc,f.x,f.y,NULL);
   LineTo  (hdc,t.x,t.y);
}


/*******************************************************************************
                        DrawFullRegion(HDC,RGNDATA*,int)

static public Function: zeichne die Region und vergrößere die Teilrechtecke
                        um ext (damit eine Region auch die untere und rechte
                        Kannte enthält)

*******************************************************************************/

void _fastcall CGraphicsMath::DrawFullRegion(HDC hdc,const RGNDATA * pRegion,int ext)
{
   if(pRegion==NULL) return;                     // Region überhaupt vorhanden ?

                                                 // WIN-Bugs :
                                                 // 1. wenn hdc = MetaDC funktioniert GetClipBox nicht richtig
                                                 // 2. wenn hdc von BeginPaint dann erzeugt GetObjectType(hdc) eine Access violation
                                                 //    deswegen kann der hdc nicht immer auf MetaDC getestet werden wesswegen das Flag
                                                 //    gm_bDoClipping eingeführt wurde welches immer auf false gesetzt wird wenn mit
                                                 //    einem MetaDC gearbeitet wird und danach sofort wieder auf true zu setzen ist.
   RECT rcClip;

   if(gm_bDoClipping)                            // soll der Clippingtest durchgeführt werden ?
   {
      GetClipBox(hdc,&rcClip);                   // normalisiertes Rechteck, das den Clippingbereich umschließt
                                                 // Ist die Region überhaupt sichtbar ?
      RECT rc = pRegion->rdh.rcBound;            // besorge das umschließende Rechteck der Region

      if (rc.left > rcClip.right ) return;       // Region nicht sichtbar !
      if (rc.top  > rcClip.bottom) return;       // Region nicht sichtbar !
      rc.right  += ext;                          // vergrößere das Rechteck um ext
      if (rc.right < rcClip.left) return;        // Region nicht sichtbar !
      rc.bottom += ext;                          // vergrößere das Rechteck um ext
      if (rc.bottom < rcClip.top) return;        // Region nicht sichtbar !
   }
   
   register int   i;
   register int   nCount;
   register RECT* pRect;

   nCount = pRegion->rdh.nCount;                 // besorge die Anzahl der Teilrechtecke der Region
   pRect  = (RECT *) pRegion->Buffer;            // besorge Zeiger auf die Teilrechteckdaten der Region

   if(gm_bDoClipping)                            // soll der Clippingtest durchgeführt werden ?
      for(i=0;i<nCount;i++)                      // Schleife über alle Teilrechtecke
      {
         RECT rc = * pRect++;                    // besorge Koordinaten des Teilrechtecks

         if (rc.left > rcClip.right ) continue;  // Teilrechteck nicht sichtbar
         if (rc.top  > rcClip.bottom) continue;  // Teilrechteck nicht sichtbar
         rc.right  += ext;                       // vergrößere das Rechteck um ext
         if (rc.right < rcClip.left) continue;   // Teilrechteck nicht sichtbar
         rc.bottom += ext;                       // vergrößere das Rechteck um ext
         if (rc.bottom < rcClip.top) continue;   // Teilrechteck nicht sichtbar
                                                 // zeiche das Teilrechteck
         PatBlt(hdc,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,PATCOPY);
      }
   else                                          // zeichnen ohne Clippingtest
      for(i=0;i<nCount;i++)                      // Schleife über alle Teilrechtecke
      {
         RECT rc = * pRect++;                    // besorge Koordinaten des Teilrechtecks
         rc.right  += ext;                       // vergrößere das Rechteck um ext
         rc.bottom += ext;                       // vergrößere das Rechteck um ext
                                                 // zeiche das Teilrechteck
         PatBlt(hdc,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,PATCOPY);
      }
}


/*******************************************************************************
                     PointInFullRegion(RGNDATA*,FPOINT,float)

static public Function: teste ob Punkte FPOINT in Region um float vergrößert
                        liegt

*******************************************************************************/

bool _fastcall CGraphicsMath::PointInFullRegion(const RGNDATA* pRegion,FPOINT& p,float delta)
{
   if(pRegion==NULL) return false;               // Region überhaupt vorhanden ?
   
   register int   i;
   register int   nCount;
   register RECT* pRect;

   RECT rc = pRegion->rdh.rcBound;               // umschreibendes Rechteck besorgen

   rc.left--;                                    // Rechteck einen Pixel vergrößern (wegen Win-Bug in CreatePolygonRgn)
   rc.right++;                                   // soviele Bug-Fixes in dieser Datei (ist das Mircosoft ?)
   rc.top--;
   rc.bottom++;

   float lx = p.x + delta;                       // erzeuge aus dem Testpunkt zwei, welche jeweils um delta verschoben sind
   float gx = p.x - delta;                       // guter Trick, um das delta nicht andauernt mitzuschleppen (sorgt für speed)
   float ly = p.y + delta;
   float gy = p.y - delta;
                                                 // 1. liegt der Punkt überhaupt im umschreibenden Rechteck der Region ?

   if(lx < rc.left)   return false;
   if(ly < rc.top)    return false;
   if(gx > rc.right)  return false;
   if(gy > rc.bottom) return false;                                                 


   nCount = pRegion->rdh.nCount;                 // besorge die Anzahl der Teilrechtecke der Region
   pRect  = (RECT *) pRegion->Buffer;            // besorge Zeiger auf die Teilrechteckdaten der Region

   for(i=0;i<nCount;i++)                         // Schleife über alle Teilrechtecke
   {
      RECT * rc = pRect++;                       // besorge Pointer auf Koordinaten des Teilrechtecks
      
      if (lx < rc->left)   continue;
      if (ly < rc->top)    continue;
      if (gx > rc->right)  continue;
      if (gy > rc->bottom) continue;

      return true;                               // Punkt ist in diesem Rechteck vorhanden
   }
   return false;                                 // Punkt ist nicht in der Region enthalten
}

/*******************************************************************************
                                  InflateRegion

static public Function: vergrößere eine Region um inflate Pixel (Weltkoordinaten)

*******************************************************************************/

void _stdcall CGraphicsMath::InflateRegion(RGNDATA * pRegion,int inflate)
{
   if(pRegion==NULL) return;                     // Region überhaupt vorhanden ?
   
   register int   i;
   register int   nCount;
   register RECT* pRect;

   pRegion->rdh.rcBound.left  -=inflate;         // umschreibendes Rechteck vergrößern
   pRegion->rdh.rcBound.top   -=inflate;
   pRegion->rdh.rcBound.right +=inflate;
   pRegion->rdh.rcBound.bottom+=inflate;

   nCount = pRegion->rdh.nCount;                 // besorge die Anzahl der Teilrechtecke der Region
   pRect  = (RECT *) pRegion->Buffer;            // besorge Zeiger auf die Teilrechteckdaten der Region

   for(i=0;i<nCount;i++)                         // Schleife über alle Teilrechtecke
   {
      pRect->left  -= inflate;
      pRect->top   -= inflate;
      pRect->right += inflate;
      pRect->bottom+= inflate;

      pRect++;                                   // besorge Koordinaten des Teilrechtecks
   }
}

/*******************************************************************************
                              RegionToPointer(HRGN)

static public Function: erzeuge aus Windows Region Handle eine lokal gespeicherte
                        Region.
                        1. jetzt kann ich soviele Regionen anlegen wie ich möchte
                        2. obige Funktionen arbeiten nur mit solchen Regionen
                        3. so werde ich auch noch die meißten Win-Bugs los 

                        (get rid of the buggy stuff) 

*******************************************************************************/

RGNDATA * _stdcall CGraphicsMath::RegionToPointer(HRGN hThis)
{
   DWORD     needed;
   RGNDATA * puffer;
   DWORD     error;

   needed = GetRegionData(hThis,1,NULL);

   puffer = (RGNDATA*) new BYTE[needed];

   if(puffer==NULL) return NULL;

   error  = GetRegionData(hThis,needed,puffer);

   if(error==0)
   {
      delete puffer;
      return NULL;
   }

   if(puffer->rdh.iType!=RDH_RECTANGLES) 
   {
      delete puffer;
      return NULL;
   }

   return puffer;
}

/*******************************************************************************
                              Region1Contains2

static public Function: teste on RGN2 vollständig in RGN1 enthalten ist oder
                        sich teilweise durchdringen

                        0 = Fehler
                        1 = überhaupt nicht in RGN1
                        2 = nicht vollständig in RGN1
                        3 = vollständig in RGN1

*******************************************************************************/

int _stdcall CGraphicsMath::Region1Contains2(HRGN rgn1,HRGN rgn2)
{
   HRGN testrgn1=NULL;                           // für Windows-Bugfix (Bounding Rectangle nicht ok bei CreatePolygonRgn);
   HRGN testrgn2=NULL;                           // für Windows-Bugfix (Bounding Rectangle nicht ok bei CreatePolygonRgn);
   HRGN bigrect =NULL;                           // helper für neue Region
   HRGN ergebnis=NULL;                           // die zu testene Durchdringungsregion
   int  erg;
   int  error;

   try
   {
      if(!(bigrect =CreateRectRgn(0,0,10001,10001))) throw 1;      
      if(!(testrgn1=CreateRectRgn(0,0,1,1)))         throw 2;
      if(!(testrgn2=CreateRectRgn(0,0,1,1)))         throw 3;
      if(!(ergebnis=CreateRectRgn(0,0,1,1)))         throw 4;

      error = CombineRgn(testrgn1,bigrect,rgn1,RGN_AND);
      if((error==ERROR)||(error==NULLREGION)) throw 5;
      error = CombineRgn(testrgn2,bigrect,rgn2,RGN_AND);
      if((error==ERROR)||(error==NULLREGION)) throw 6;
      error = CombineRgn(ergebnis,testrgn1,testrgn2,RGN_AND);
      if (error==ERROR) throw 7;

      if (error==NULLREGION) erg = 1;
      else
      {
         error = EqualRgn(ergebnis,testrgn2);    // beachte EqualRgn liefert nicht ERROR !!!!!
         if(error==0) erg = 2;
         else         erg = 3;
      }
   }
   catch(int)                                    // alle Fehler hier abfangen 
   {
      erg = 0;
   }
                                                 // alle temporären Regionen löschen soweit vorhanden
   if(bigrect)  DeleteObject(bigrect);
   if(testrgn1) DeleteObject(testrgn1);
   if(testrgn2) DeleteObject(testrgn2);
   if(ergebnis) DeleteObject(ergebnis);
   
   return erg;                                    // Ergebnis übergeben
}


/*******************************************************************************
                            NormalizeRect(RECT,RECT)

static public Function: eine Rechteck normalisieren

*******************************************************************************/

void _fastcall CGraphicsMath::NormalizeRect(RECT& dest,RECT& source)
{
   dest = source;

   if(source.left > source.right)
   {
      dest.right = source.left;
      dest.left  = source.right;
   }

   if(source.top > source.bottom)
   {
      dest.bottom=source.top;
      dest.top   =source.bottom;
   }
}


/* // diese Funktion wird nicht mehr benötigt !!!!!!! (neu 16.03.2000)

bool CGraphicsMath::PointsInLineSection(POINT* TestPoints,int nCounter,POINT f,POINT t)
{
   static const double lambdamax = 1.0 + 1e-8;

   POINT  r;
   int    i;

   r.x = t.x - f.x;                               // den Richtungsvektor der Strecke berechnen
   r.y = t.y - f.y;

                                                  // es gibt vier Fälle r.x == 0 r.y!=0 und r.y==0,r.x!=0 und r.x,r.y!=0 und r.x,r.y==0

   if((r.x==0)&&(r.y!=0))                         // da r.x==0 müssen auf jeden Fall die Koordinaten f.x==t.x== TestPoints[].x sein
   {
      if(f.y > t.y) swap(f.y,t.y);                // f.y soll kleiner als t.y sein , dann müssen alle Y-Koordinaten von TestPoint[].y
                                                  // genau zwischen f.y >= ? <= t.y liegen

      for(i=0;i<nCounter;i++)                     // Schleife über alle zu testenden Punkte
      {
         if(TestPoints[i].x != f.x) return false; // eine X-Koordinate ist ungleich, also nicht alle Punkte auf der Strecke
         if(TestPoints[i].y < f.y)  return false;
         if(TestPoints[i].y > t.y)  return false;
      }
      return true;                                // alle Punkte liegen in der Strecke !
   }
   else if((r.x!=0)&&(r.y==0))                    // da r.y==0 müssen auf jeden Fall die Koordinaten f.y=t.y==TestPoints[].y sein
   {
      if(f.x > t.x) swap(f.x,t.x);                // f.x soll kleiner als t.x sein, dann müssen alle x-Koordinaten von TestPoints[].x
                                                  // genau zwischen f.x >= ? <= t.x liegen
      for(i=0;i<nCounter;i++)                     // Schleife über alle zu testenden Punkte
      {
         if(TestPoints[i].y != f.y) return false; // eine Y-Koordinate ist ungleich, also nicht alle Punkte auf der Strecke
         if(TestPoints[i].x < f.x)  return false;
         if(TestPoints[i].x > t.x)  return false;
      }
      return true;
   }
   else if((r.x!=0)&&(r.y!=0))
   {
     double r_rx = 1.0 / r.x;
     double r_ry = 1.0 / r.y;
      
     for(i=0;i<nCounter;i++)
     {
        int v = TestPoints[i].x-f.x;

        if(v!=0)
        {
           double lambdax = v * r_rx;
           if((lambdax < 0.0)||(lambdax > lambdamax)) return false;
           double newy = lambdax*r.y+f.y;
           if(fabs(newy - TestPoints[i].y)>1e-8) return false;
        }
        else
        if(TestPoints[i].y!=f.y) return false;
     }

     return true;
   }
   else return false; // keine Strecke, da r.x und r.y == 0
}
*/



#include <limits.h>

bool _stdcall CGraphicsMath::PtInPolygon(POINT * pptPoly,int nNumber,POINT ptTest,RECT * prcBound)
{
   RECT   rc;
   int    i;
   POINT *ppt;
   POINT  pt2;
   int    nCounter;

   if(prcBound!=NULL) rc = *prcBound;            // das umschreibende Rechteck ist schon vorhanden
   else                                          // das umschreibende Rechteck wird berechnet
   {
      rc.left  = rc.top    =  INT_MAX;
      rc.right = rc.bottom =  INT_MIN;

      for(i = 0,ppt = pptPoly;i<nNumber;i++,ppt++)
      {
         if(ppt->x < rc.left  ) rc.left   = ppt->x;
         if(ppt->x > rc.right ) rc.right  = ppt->x;
         if(ppt->y < rc.top   ) rc.top    = ppt->y;
         if(ppt->y > rc.bottom) rc.bottom = ppt->y;
      }
   }

   if(!PtInRect(&rc,ptTest)) return false;       // Punkt im umschreibenden Rechteck ?

   pt2      = ptTest;                            // bilde zweiten Punkt für Testlinie
   pt2.x    = rc.right + 50;
   nCounter = 0;                                 // Linieschnittzähler initialisieren

   for(i = 0,ppt = pptPoly;i<nNumber-1;i++,ppt++)// Schleife über alle Polygonlinien
   {                                             // und Schnitttest, erfolgreichen Schnitt zählen
      if(Intersect(ptTest,pt2,*ppt,*(ppt+1))) nCounter++;
   }
                                                 // und mit letzter Linie
   if(Intersect(ptTest,pt2,*ppt,*pptPoly)) nCounter++;

   return (nCounter&1)?true:false;
}

// spezial ohne Fehlertests für z.B. C3DView

bool _stdcall CGraphicsMath::DoublePtInPolygon(POINT * pptPoly,int nNumber,POINT& ptTest1,POINT& ptTest2,RECT * prc)
{
   int     i;                                    // für Schleife
   POINT * ppt;                                  // Zeiger auf Punkte
   POINT   ende;                                 // Punkt auserhalb des Polygons für Testlinie
   int     nCounter1;                            // Schnittzähler für Testlinie1
   int     nCounter2;                            // Schnittzähler für Testlinie2
   POINT   from;                                 // Punkte für die aktuelle Polygonlinie
   POINT   to;

   if(!PtInRect(prc,ptTest1)) return false;      // Punkt1 im umschreibenden Rechteck ?
   if(!PtInRect(prc,ptTest2)) return false;      // Punkt2 im umschreibenden Rechteck ?

   ende.y     = ptTest1.y;                       // bilde zweiten Punkt für Testlinie
   ende.x     = prc->right + 50;
   nCounter1  = 0;                               // Linieschnittzähler initialisieren
   nCounter2  = 0;
   
   ppt  =   pptPoly;                             // Pointer auf ersten Polygonpunkt übertragen
   from = * ppt;                                 // besorge den ersten Testpunkt

   for(i = 0;i < nNumber-1;i++)                  // Schleife über alle Polygonlinien
   {            
      to   = *(++ppt);                           // auf nächsten Polygonpunkt gehen und besorgen
                                                 
      if(Intersect(ptTest1,ende,from,to))        // und Schnitttest mit 1. Testlinie, erfolgreichen Schnitt zählen
         nCounter1++;
      if(Intersect(ptTest2,ende,from,to))        // und Schnitttest mit 2. Testlinie, erfolgreichen Schnitt zählen
         nCounter2++;

      from = to;                                 // Punkt wird nun aktueller from Punkt
   }

   to   = *pptPoly;
                                                 // und Schnitttest mit letzter Linie
   if(Intersect(ptTest1,ende,from,to)) nCounter1++;
   if(Intersect(ptTest2,ende,from,to)) nCounter2++;

   return (((nCounter1 & 1)?true:false)&&((nCounter2 & 1)?true:false));
}

// optimized Code for inline expansion

inline bool CGraphicsMath::Intersect(POINT& p1,POINT& p2,POINT& p3,POINT& p4)
{
   register int  dx;
   register int  dy;
   register bool a;

   dx = p2.x - p1.x;
   dy = p2.y - p1.y;

   a =  (dx*(p3.y - p1.y) > dy*(p3.x - p1.x))
       ^(dx*(p4.y - p1.y) > dy*(p4.x - p1.x));

   dx = p4.x - p3.x;
   dy = p4.y - p3.y;

   return a&&( (dx*(p1.y - p3.y) > dy*(p1.x - p3.x))
              ^(dx*(p2.y - p3.y) > dy*(p2.x - p3.x)));
}

BITMAPINFO * CGraphicsMath::GetBitmap8X8(HINSTANCE hInst,int nID)
{
#define B_SIZE_CX 8                              // Makros für die Bitmapgröße !
#define B_SIZE_CY 8
                                                 // Bitmap aus Resource Laden
   HBITMAP hBitmap = (HBITMAP) LoadImage(hInst,MAKEINTRESOURCE(nID),IMAGE_BITMAP,B_SIZE_CX,B_SIZE_CY,LR_DEFAULTCOLOR);

   if(hBitmap)                                   // eine Bitmap geladen ?
   {
      BITMAPINFOHEADER Bih;
      BITMAPINFO *     pBi;
      HDC              hdc = GetDC(NULL);        // ScreenDC besorgen
                                                 // BITMAPINFOHEADER mit Daten füllen
      Bih.biSize         = sizeof(BITMAPINFOHEADER);
      Bih.biWidth        = B_SIZE_CX;            // Größe der Bitmap eintragen
      Bih.biHeight       = B_SIZE_CY;
      Bih.biPlanes       = 1;                    // eine Bitplane
      Bih.biBitCount     = 32;                   // 32-Bit Colors
      Bih.biCompression  = BI_RGB;               // als RGB Farbewerte ablegen (->keine Palette)
      Bih.biSizeImage    = B_SIZE_CX*B_SIZE_CY*4;// Größe des Speichers für die Bits nur für 2er Potenzen gültig !
      Bih.biXPelsPerMeter= B_SIZE_CX;            // als dummies
      Bih.biYPelsPerMeter= B_SIZE_CY;
      Bih.biClrUsed      = 0;                    // alle Farben Wichtig
      Bih.biClrImportant = 0;
                                                 // Speicher für pBi besorgen
      pBi = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFOHEADER) + Bih.biSizeImage];

      if(pBi)                                    // Speicher erhalten ?
      {
         *((BITMAPINFOHEADER*)pBi) = Bih;        // Kopiere den Inhalt vom BITMAPINFOHEADER
                                                 // Kopiere alle Bits der Bitmap mit Umrechnung ins gewünschte Farbformat
         if(GetDIBits(hdc,hBitmap,0,B_SIZE_CY,&(pBi->bmiColors[0]),pBi,DIB_RGB_COLORS)==NULL)
         {
            delete pBi;                          // Fehler, dann Speicher freigeben
            pBi = NULL;                          // Pointer auf NULL
         }      
      }

      ReleaseDC(NULL,hdc);                       // ScreenDC freigeben
      DeleteObject(hBitmap);                     // Bitmap löschen
      return pBi;                                // Ergebnis oder Nullpointer zurück
   }
   return NULL;                                  // Fehler, Nullpointer zurück

#undef B_SIZE_CX
#undef B_SIZE_CY
}


/*******************************************************************************
                                SnapPointToLine

static public Function: Berechne Schnittpunkt von test und der Linie l1-l2
                        als Fangoption (das Lot von test auf die Linie l1-l2)

                        -1 = kein Schnittpunkt
                     != -1 = Entfernung des Schnittpunkte zu test

*******************************************************************************/

int _stdcall CGraphicsMath::SnapPointToLine(POINT l1,POINT l2,POINT test,DPOINT& snapped)
{
   POINT   a,b;
   DPOINT  c;

   int     scale,bqb;

   a.x = test.x - l2.x;                          // Bilde zwei Vektoren (mit l2.x,l2.y als Ursprung)
   a.y = test.y - l2.y;
   b.x = l1.x   - l2.x;
   b.y = l1.y   - l2.y;
                                                 // Projeziere a auf b = c
   scale = a.x*b.x + a.y*b.y;
   bqb   = b.x*b.x + b.y*b.y;

   if(bqb)                                       // Betrag des Vektors b vorhanden ?
   {
      double mul = ((double)scale) / ((double)bqb);

      if((mul >=0.0)&&(mul <= 1.0))              // wird die Projektion in b liegen ?
      {
         c.x = b.x*mul;
         c.y = b.y*mul;

         snapped.x = c.x + l2.x;                 // der Senkrechte Schnittpunkt auf der Linie l1,l2
         snapped.y = c.y + l2.y;
                                                 // Berechne den Abstand test zu snapped
         return Rounding(_hypot(test.x-snapped.x,test.y-snapped.y));
      }
   }
   return -1;
}


/********************************************************************************/
/*                                                                              */
/*        Funktion zur Prüfung, ob die übergebenen Strecken                     */
/*                A und B sich gegenseitig schneiden                            */
/*                                                                              */
/*   Rückgabewerte:                                                             */
/*   0 -> kein Schnittpunkt                                                     */
/*   1 -> Strecken hängen aneinander, Berührung am Anfang/Ende                  */
/*   2 -> ein Streckenendpunkt berührt innerhalb der zweiten Strecke            */
/*   3 -> ein Streckenendpunkt berührt innerhalb der ersten Strecke             */
/*   4 -> 'echter' Schnittpunkt                                                 */
/*   5 -> unendlich viele Schnittpunkte                                         */
/*                                                                              */
/********************************************************************************/


int _stdcall CGraphicsMath::CheckLineSectionCut( DPOINT& A1, DPOINT& A2, DPOINT& B1, DPOINT& B2 )
{
   double   dDetLambda, dDetNenner;
   int      ret;
   DPOINT   A21, B12, B1A1;

   A21.x = A2.x - A1.x;                          // Richtungsvektoren für Strecken A und B
   A21.y = A2.y - A1.y;                          // A1 und B1 sind die Anfangspunkte der Strecken
   B12.x = B1.x - B2.x;
   B12.y = B1.y - B2.y;

   B1A1.x = B1.x - A1.x;                         // Differenzvektor der Streckenanfangspunkte
   B1A1.y = B1.y - A1.y;
                                                 // Punktekoordinaten sind < 10000 !! (kein Überlauf)
   dDetNenner = A21.x*B12.y - A21.y*B12.x;       // Nenner-Determinante der Cramer-Regel
   dDetLambda = B1A1.x*B12.y - B1A1.y*B12.x;     // Zähler-Determinante für Lambda der Strecke A
   
   if( fabs(dDetNenner) > EPS )                  // Strecken A und B sind NICHT parallel
   {
      bool   blambda   =true ,bmy   =true;       // true entspricht lamdba,my genau 0 oder 1
      bool   blambda021=false,bmy021=false;      // true entspricht lambda,my zwischen 0 und 1

      double dDetMy = A21.x*B1A1.y-A21.y*B1A1.x; // Zähler-Determinante für My der Strecke B

                                                 // Lambda, My sind die Geradengleichungs-Parameter
                                                 // dlambda wird nicht genau 0 oder 1
      if((fabs(dDetLambda) > EPS)&&(fabs(dDetLambda-dDetNenner)>EPS))
      {
         double dLambda = dDetLambda / dDetNenner;
         blambda        = false;
         blambda021     = (dLambda>0.0)&&(dLambda<1.0);
      }

      if((fabs(dDetMy)>EPS)&&(fabs(dDetMy-dDetNenner)>EPS))
      {
         double dMy = dDetMy / dDetNenner;
         bmy        = false;
         bmy021     = (dMy>0.0)&&(dMy<1.0);
      }

      if(blambda&bmy)                            // die Strecken A und B hängen aneinander
         ret = 1;
      else if(blambda&bmy021)
         ret = 2;                                // ein Streckenende(lambda) berührt innerhalb der anderen Strecke
      else if(bmy&blambda021)
         ret = 3;                                // ein Streckenende(my) berührt innerhalb der anderen Strecke
      else if(bmy021&blambda021)
         ret = 4;                                // Schnittpunkt zwischen beiden Strecken
      else
         ret = 0;                                // kein Schnittpunkt
   }
   else                                          // Strecken A und B sind parallel
   {
      if(fabs(dDetLambda) < EPS)                 // die Strecken A und B liegen auf der gleichen Geraden
      {
         double axy1,axy2,bxy1,bxy2;
         
         if(fabs(A21.x) > fabs(A21.y))           // die Strecke A zeigt eher in x-Richtung
         {
            if(A1.x < A2.x) axy1=A1.x,axy2=A2.x;
            else            axy1=A2.x,axy2=A1.x;

            if(B1.x < B2.x) bxy1=B1.x,bxy2=B2.x;
            else            bxy1=B2.x,bxy2=B1.x;
         }
         else                                    // die Strecke A zeigt eher in y-Richtung oder hat 45°
         {
            if(A1.y < A2.y) axy1=A1.y,axy2=A2.y;
            else            axy1=A2.y,axy2=A1.y;

            if(B1.y < B2.y) bxy1=B1.y,bxy2=B2.y;
            else            bxy1=B2.y,bxy2=B1.y;
         }

         if((axy2 <bxy1)||(axy1 >bxy2)) ret = 0; // beide Strecken berühren sich nicht
         else
         if((fabs(axy2-bxy1)<EPS)||
            (fabs(axy1-bxy2)<EPS))  ret = 1;     // beide Strecken hängen aneinander
         else                       ret = 5;     // die Strecken A und B überlappen sich -> unendlich viele Schnittpunkt
      }
      else ret = 0;                              // die parallelen Strecken schneiden sich im Unendlichen
   }

   return (ret);
}


/********************************************************************************/
/*                                                                              */
/*        Funktion zur Prüfung, ob Punkt innerhalb eines Polygon liegt          */
/*                                                                              */
/*   Rückgabewerte:                                                             */
/*   0 -> Punkt innerhalb                                                       */
/*   1 -> Punkt auf einer Kannte                                                */
/*   2 -> Punkt außerhalb oder Fehler                                           */
/*                                                                              */
/********************************************************************************/

int _stdcall CGraphicsMath::PtInPolygonWithEdge(DPOINT * pptPoly,int nNumber,DPOINT ptTest)
{
   int      i,ret;
   DPOINT * ppt = pptPoly;
   DPOINT * ppN;
   DPOINT   pt2;
   int      nCounter;

   pt2.y    = 11000;                             // bilde zweiten Punkt für Testlinie
   pt2.x    = 11000;                             // diese Punkt liegt außerhalb unseres Wertebereiches

   if(nNumber < 3) return 2;                     // super Fehler (also außerhalb da kein Testpolygon)

   nCounter = 0;                                 // Linieschnittzähler initialisieren

   for(i = 0; i < nNumber; i++)                  // Schleife über alle Polygonlinien
   {   
      if(i < nNumber-1) ppN = ppt + 1;           // nachfolgenden Polygonpunkt besorgen
      else              ppN = pptPoly;           // und auch noch mit letzter Linie testen !
                                                 // und Schnitttest, erfolgreichen Schnitt zählen
      for(;;)                                    // der Trick mit den Endlosschleife
      {
         ret = CheckLineSectionCut(ptTest,pt2,*ppt,*ppN);

         switch(ret)
         {
            case 0:  break;                      // kein Schnitt !
            case 3:  pt2.y += 20;                // uneindeutiger Schnitt, also Testgerade mutieren
                     TRACE("Nicht eindeutig !\n");
                     continue;                   // bitte noch ein Versuch
            case 4:  nCounter++;                 // ein echter Schnittpunkt
                     break;
            default: return 1;                   // Punkt liegt auf eine Kannte (1,2,5)
         }
         break;                                  // Im Normalfall diese Schleife sofort abbrechen
      }

      ppt = ppN;                                 // Aktueller = Nachfolgender
   }

   if(nCounter&1) return 0;                      // Punkt liegt innerhalb
   else return 2;                                // Punkt liegt außerhalb
}


/**************************************+****************************************
                  Trapezoid(DPOINT&,DPOINT&,DPOINT&,DPOINT&)

static public Function: Sonderfunktion für Schnittpunkt für Trapez berechnen

*******************************************************************************/

bool _stdcall CGraphicsMath::Trapezoid(DPOINT& w,DPOINT& f,DPOINT& t,DPOINT& c)
{
   DPOINT r1;
   DPOINT r2;                                    // Richtungsvektoren der Geranden
   DPOINT d; 
   double det;
   double deth;
   double n;

   r1.x = t.x - f.x;
   r1.y = t.y - f.y;

   r2.x = c.x - t.x;
   r2.y = c.y - t.y;

   det = r1.x*r2.y - r1.y*r2.x;

   if(fabs(det) < EPS)
   {
      TRACE("fehler\n");
      return false;                              // die Strecken liegen aufeinander oder sind parallel
   }

   d.x = c.x-w.x;
   d.y = c.y-w.y;

   deth = d.x*r2.y-d.y*r2.x;

   n = deth / det;

   d.x = n*r1.x + w.x;                          // Berechne den Schnittpunkt
   d.y = n*r1.y + w.y;

   r2.x = d.x - t.x;                            // Berechne nocheinmal die det 
   r2.y = d.y - t.y;

   det = r1.x*r2.y - r1.y*r2.x;

   if(fabs(det) < EPS)
   {
      TRACE("Abfangen\n");
      return false;                             // die Strecken liegen aufeinander oder sind parallel
   }

   c = d;                                       // Schnittpunkt zurückgeben

   return true;                                 // Schnittpunkt ist berechnet
}

/********************************************************************/
/*                                                                  */
/* Funktion zur Prüfung, ob die beiden nacheinander und aneinander  */
/* hängenden Polygonstrecken A1-A2 und A2-A3 zueinander konvex sind.*/
/* Konvex heißt hier im Sinne einer in Draufsicht rechts drehenden  */
/* Schraube: hierzu wird das Vektorprodukt (A3-A2)X(A2-A1) gebildet,*/
/*           es muß eine positive z-Koordinate haben!! Das Verfahren*/
/*           funktioniert also nicht für senkrechte oder auf dem    */
/*           Kopf stehende Wandpolygone !!                          */
/*                                                                  */
/*   Rückgabe: -1 -> konkave Ecke in A2                             */
/*             +1 -> konvexe Ecke in A2                             */
/*              0 -> beide Polygonstrecken liegen auf einer Geraden */
/*                                                                  */
/********************************************************************/

                                                 // aus ETSTESS kopiert hier eingefügt (Bedeutung ist hier invertiert)

int _stdcall CGraphicsMath::CheckKonvex( DPOINT& A1, DPOINT& A2, DPOINT& A3 )
{
   DPOINT    A21, A32;
   double    nZ_Komp;


   A21.x = A2.x - A1.x;                          // 1. Strecke des übergebenen Polygonzweigs
   A21.y = A2.y - A1.y;
   A32.x = A3.x - A2.x;                          // 2. Strecke des übergebenen Polygonzweigs
   A32.y = A3.y - A2.y;

   nZ_Komp = A32.x*A21.y - A32.y*A21.x;          // z-Komponente des Vektors (A3-A2)X(A2-A1)

   if( fabs(nZ_Komp) > 1e-8)                     // nicht 0
      nZ_Komp /= fabs(nZ_Komp);

   return Rounding(nZ_Komp);
}


/**************************************+****************************************
                  ClipLine(POINT&,POINT&,RECT *)

static public Function: eine Linie p1,p2 am RECT * clippen

*******************************************************************************/

#define CLIP_LEFT   0x08
#define CLIP_TOP    0x04
#define CLIP_RIGHT  0x02
#define CLIP_BOTTOM 0x01

bool _stdcall CGraphicsMath::ClipLine(POINT & p1,POINT & p2,RECT * rc)
{
   int dx = (p2.x - p1.x)<<4;
   int dy = (p2.y - p1.y)<<4;

   for(int clip=0;clip < 3;clip++)
   {
      int ltrb1 = 0;
      int ltrb2 = 0;

      if(p1.x < rc->left  ) ltrb1 |= CLIP_LEFT;
      if(p1.x > rc->right ) ltrb1 |= CLIP_RIGHT;
      if(p1.y < rc->top   ) ltrb1 |= CLIP_TOP;
      if(p1.y > rc->bottom) ltrb1 |= CLIP_BOTTOM;

      if(p2.x < rc->left  ) ltrb2 |= CLIP_LEFT;
      if(p2.x > rc->right ) ltrb2 |= CLIP_RIGHT;
      if(p2.y < rc->top   ) ltrb2 |= CLIP_TOP;
      if(p2.y > rc->bottom) ltrb2 |= CLIP_BOTTOM;

      if((ltrb1|ltrb2)==0) return true;
      if((ltrb1&ltrb2)!=0) return false;
                                                 // Punkt p1 clippen
      if(ltrb1&CLIP_LEFT)
      {
         p1.y = p1.y + MulDiv(rc->left-p1.x,dy,dx);
         p1.x = rc->left;
      }
      else
      if(ltrb1&CLIP_TOP)
      {
         p1.x = p1.x + MulDiv(rc->top-p1.y,dx,dy);
         p1.y = rc->top;
      }
      else
      if(ltrb1&CLIP_RIGHT)
      {
         p1.y = p1.y + MulDiv(rc->right-p1.x,dy,dx);
         p1.x = rc->right;
      }
      else
      if(ltrb1&CLIP_BOTTOM)
      {
         p1.x = p1.x + MulDiv(rc->bottom-p1.y,dx,dy);
         p1.y = rc->bottom;
      }
                                                 // Punkt p2 clippen
      if(ltrb2&CLIP_LEFT)
      {
         p2.y = p2.y + MulDiv(rc->left-p2.x,dy,dx);
         p2.x = rc->left;
      }
      else
      if(ltrb2&CLIP_TOP)
      {
         p2.x = p2.x + MulDiv(rc->top-p2.y,dx,dy);
         p2.y = rc->top;
      }
      else
      if(ltrb2&CLIP_RIGHT)
      {
         p2.y = p2.y + MulDiv(rc->right-p2.x,dy,dx);
         p2.x = rc->right;
      }
      else
      if(ltrb2&CLIP_BOTTOM)
      {
         p2.x = p2.x + MulDiv(rc->bottom-p2.y,dx,dy);
         p2.y = rc->bottom;
      }
   }

   return false;
}
