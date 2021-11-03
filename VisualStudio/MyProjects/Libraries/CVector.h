#ifndef __CALCVECT_H
  #define __CALCVECT_H

#ifndef EPSILON
   #define EPSILON 1.0e-20
#endif

#include <math.h>

#pragma warning(disable: 4239)
#pragma warning(disable: 4710)

class CVector {
   friend class CVector2D;

   public:
                             // Prototypen der Elementfunktion der Klasse
                             // Konstruktoren
       CVector(double _x_value,double _y_value, double _z_value)
          {xcomp = _x_value; ycomp = _y_value; zcomp = _z_value;};
       CVector() {xcomp=ycomp=zcomp=0.0;};
                             // WertrÅckgabefunktionen von Vektoren
       friend double  Vx(CVector& wert){return wert.xcomp;};   // X-teil des Vektors
       friend double  Vy(CVector& wert){return wert.ycomp;};   // Y-teil des Vektors
       friend double  Vz(CVector& wert){return wert.zcomp;};   // Z-teil des Vektors
       double&        SetX()           {return xcomp;};
       double&        SetY()           {return ycomp;};
       double&        SetZ()           {return zcomp;};
       friend double  Betrag(CVector&);                        // Betrag des Vektors
       friend double  Quadbetrag(CVector&);                    // Quadrat des Betrags des Vektors
       friend double  Sinus(CVector&,CVector&);                // Sinus des Winkels zwischen zwei Vektoren   (*)
       friend double  Cosinus(CVector&,CVector&);              // Cosinus des Winkels zwischen zwei Vektoren (*)
       friend double  Tangens(CVector&,CVector&);              // Tangens des Winkels zwischen zwei Vektoren (*)
       friend CVector  Product(CVector&, CVector&);            // Vektorprodukt aus zwei Vektoren
       friend CVector  Norm(CVector&);                         // Normierung des Vektors
       friend double  Spat(CVector&, CVector&, CVector&);      // Spatprodukt aus drei Vektoren

       friend CVector operator+(CVector& _z1, CVector& _z2)
          { return CVector(_z1.xcomp+_z2.xcomp, _z1.ycomp+_z2.ycomp, _z1.zcomp+_z2.zcomp);};
       friend CVector operator-(CVector& _z1, CVector& _z2)
          { return CVector(_z1.xcomp-_z2.xcomp, _z1.ycomp-_z2.ycomp, _z1.zcomp-_z2.zcomp);};
       friend double operator*(CVector& _z1, CVector& _z2)   // Skalarprodukt
          { return (_z1.xcomp*_z2.xcomp+_z1.ycomp*_z2.ycomp+_z1.zcomp*_z2.zcomp);}
       friend CVector operator*(CVector& _z1, double zahl)
          { return CVector(_z1.xcomp*zahl, _z1.ycomp*zahl, _z1.zcomp*zahl);};
       friend CVector operator*(double zahl, CVector& _z1)
          { return CVector(_z1.xcomp*zahl, _z1.ycomp*zahl, _z1.zcomp*zahl);};
       friend CVector operator/(CVector& _z1, double zahl)
          { return CVector(_z1.xcomp/zahl, _z1.ycomp/zahl, _z1.zcomp/zahl);};

       friend int    operator==(CVector& _z1, CVector& _z2)
          { return (Quadbetrag(_z1 - _z2) < EPSILON);};

       friend int    operator!=(CVector& _z1, CVector& _z2)
          { return (Quadbetrag(_z1 - _z2) > EPSILON);};

       CVector&       operator+=(CVector& wert)
          {xcomp+=wert.xcomp; ycomp+=wert.ycomp; zcomp+=wert.zcomp; return *this;};
       CVector&       operator-=(CVector& wert)
          {xcomp-=wert.xcomp; ycomp-=wert.ycomp; zcomp-=wert.zcomp; return *this;};
       CVector&       operator*=(double wert)
          {xcomp*=wert; ycomp*=wert; zcomp*=wert; return *this;};
       CVector&       operator/=(double wert)
          {xcomp/=wert; ycomp/=wert; zcomp/=wert; return *this;};
       CVector  operator+() {return *this;};
       CVector  operator-() {return CVector(-xcomp, -ycomp, -zcomp);};

   private:
       double xcomp,ycomp,zcomp;                               // Datenelemente der Klasse (X, Y, Z-Komponente)
};

/*  (*)  Anmerkung :
   Die Funktion Cosinus hat den Wertebereich -1 <= Phi <= 1 f¸r Winkel von 0..180∞.
   Die Funktion Sinus hat den Wertebereich 0 <= Phi <= 1 f¸r Winkel von 0..180∞.
   Der ermittelte Winkel des Sinus ist immer ein spitzer Winkel, f¸r 0..90∞ gilt:
   Phi = 180∞ - Phi !!
*/

// 2D-Vector
class CVector2D {
   public:
                             // Prototypen der Elementfunktion der Klasse
                             // Konstruktoren
       CVector2D(double _x_value,double _y_value)
       {xcomp = _x_value; ycomp = _y_value;};
       CVector2D() {xcomp=ycomp=0.0;};
                             // WertrÅckgabefunktionen von Vektoren
       friend double  Vx(CVector2D& wert){return wert.xcomp;};   // X-teil des Vektors
       friend double  Vy(CVector2D& wert){return wert.ycomp;};   // Y-teil des Vektors
       double&        SetX()           {return xcomp;};
       double&        SetY()           {return ycomp;};
       friend double  Betrag(CVector2D&);                        // Betrag des Vektors
       friend double  Quadbetrag(CVector2D&);                    // Quadrat des Betrags des Vektors
       friend double  Cosinus(CVector2D&,CVector2D&);            // Cosinus des Winkels zwischen zwei Vektoren (*)
       friend CVector2D  Ortho(CVector2D&, CVector2D&);          // Vektorprodukt aus zwei Vektoren
       friend CVector2D  Norm(CVector2D&);                       // Normierung des Vektors
       friend double  Spat(CVector2D&, CVector2D&, CVector2D&);  // Spatprodukt aus drei Vektoren

//       friend CVector2D operator=(CVector& _z1)
//         {return CVector2D(_z1.xcomp, _z1.ycomp);};
       friend CVector2D operator+(CVector2D& _z1, CVector2D& _z2)
          { return CVector2D(_z1.xcomp+_z2.xcomp, _z1.ycomp+_z2.ycomp);};
       friend CVector2D operator-(CVector2D& _z1, CVector2D& _z2)
          { return CVector2D(_z1.xcomp-_z2.xcomp, _z1.ycomp-_z2.ycomp);};
       friend double operator*(CVector2D& _z1, CVector2D& _z2)   // Skalarprodukt
          { return (_z1.xcomp*_z2.xcomp + _z1.ycomp*_z2.ycomp);}
       friend CVector2D operator*(CVector2D& _z1, double zahl)
          { return CVector2D(_z1.xcomp*zahl, _z1.ycomp*zahl);};
       friend CVector2D operator*(double zahl, CVector2D& _z1)
          { return CVector2D(_z1.xcomp*zahl, _z1.ycomp*zahl);};
       friend CVector2D operator/(CVector2D& _z1, double zahl)
          { return CVector2D(_z1.xcomp/zahl, _z1.ycomp/zahl);};

       friend int    operator==(CVector2D& _z1, CVector2D& _z2)
          { return (Quadbetrag(_z1 - _z2) < EPSILON);};

       friend int    operator!=(CVector2D& _z1, CVector2D& _z2)
          { return (Quadbetrag(_z1 - _z2) > EPSILON);};

       CVector2D&       operator+=(CVector2D& wert)
          {xcomp+=wert.xcomp; ycomp+=wert.ycomp; return *this;};
       CVector2D&       operator-=(CVector2D& wert)
          {xcomp-=wert.xcomp; ycomp-=wert.ycomp; return *this;};
       CVector2D&       operator*=(double wert)
          {xcomp*=wert; ycomp*=wert; return *this;};
       CVector2D&       operator/=(double wert)
          {xcomp/=wert; ycomp/=wert; return *this;};
       CVector2D  operator+() {return *this;};
       CVector2D  operator-() {return CVector2D(-xcomp, -ycomp);};

   private:
       double xcomp,ycomp;                               // Datenelemente der Klasse (X, Y-Komponente)
};


// 3D-Vector Funktionen
inline double Quadbetrag(CVector& wert)
{
   return ( wert.xcomp*wert.xcomp + wert.ycomp*wert.ycomp + wert.zcomp*wert.zcomp );
}

inline double Betrag(CVector& wert)
{
   return  sqrt(Quadbetrag(wert));
}

inline double Cosinus(CVector& _z1,CVector& _z2)
{
   return ((_z1*_z2) / (Betrag(_z1) * Betrag(_z2)));
}

inline CVector Product(CVector& _z1, CVector& _z2)
{
   return CVector(_z1.ycomp*_z2.zcomp - _z1.zcomp*_z2.ycomp,
                  _z1.zcomp*_z2.xcomp - _z1.xcomp*_z2.zcomp,
                  _z1.xcomp*_z2.ycomp - _z1.ycomp*_z2.xcomp);
}

inline double Spat(CVector& _z1, CVector& _z2, CVector& _z3)
{
   CVector p = Product(_z2, _z3);
   return _z1 * p;
}

inline double Sinus(CVector& _z1,CVector& _z2)
{
   CVector p = Product(_z1, _z2);
   return   (Betrag(p) / (Betrag(_z1) * Betrag(_z2)));
}

inline double Tangens(CVector& _z1,CVector& _z2)
{
   CVector p = Product(_z1, _z2);
   double dBetrag = Betrag(p);
   if (dBetrag < EPSILON) dBetrag = EPSILON;
   return (_z1*_z2) / dBetrag;
}

inline CVector Norm(CVector& _z1)
{
   double db = Betrag(_z1);
   if (db > EPSILON) return _z1 / db;
   else              return CVector(0.0, 0.0, 0.0);
}

// Funktionen 2D
inline double Quadbetrag(CVector2D& wert)
{
   return ( wert.xcomp*wert.xcomp + wert.ycomp*wert.ycomp);
}

inline double Betrag(CVector2D& wert)
{
   return  sqrt(Quadbetrag(wert));
}

inline double Cosinus(CVector2D& _z1,CVector2D& _z2)
{
   return (_z1*_z2) / (Betrag(_z1) * Betrag(_z2));
}

inline CVector2D Ortho(CVector2D& _z1, CVector2D& _z2)
{
   return CVector2D(_z1.ycomp-_z2.ycomp, _z2.xcomp-_z1.xcomp);
}

inline CVector2D Norm(CVector2D& _z1)
{
   double db = Betrag(_z1);
   if (db > EPSILON) return _z1 / db;
   else              return CVector2D(0.0, 0.0);
}

#endif  // (__CALCVECT_H)
