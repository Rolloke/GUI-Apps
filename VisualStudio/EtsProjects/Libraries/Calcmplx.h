// Klasse Complex: 2.3.99



#if !defined(__CALCMPLX_H)
#define __CALCMPLX_H

#ifndef __cplusplus
#error Must use C++ for the type complex.
#endif


#include <math.h>
#ifndef M_PI
   #define M_PI                3.1415926535898
#endif

#pragma warning( push)
#pragma warning( disable : 4239)

class Complex {

   public:
                              // Prototypen der Elementfunktion der Klasse

       Complex(double _re_value,double _im_value=0);        // Konstruktoren
       Complex();
       void SetValues(double dR, double dI) {realteil = dR, imagteil = dI;}; // Funktion, die die Phase einer komplexen Zahl zurückliefert in RAD
                                          // Wertrückgabefunktionen von komplexen Zahlen
       friend double  Real(Complex&);     // Funktion, die den Realteil zurckliefert
       friend double  Imag(Complex&);     // Funktion, die den Imagin„rteil zurckliefert
       friend Complex Konj(Complex&);     // Funktion, die die konjugiert komplexe Zahl liefert
       friend double  Betrag(Complex&);   // Funktion, die den Betrag einer komplexen Zahl zurckliefert
       friend double  Asqr(Complex&);     // Funktion, die das Quadrat des Betrages einer komplexen Zahle liefert
       friend double  Phase(Complex&);    // Funktion, die die Phase einer komplexen Zahl zurückliefert in Degree
       friend double  PhaseRAD(Complex&); // Funktion, die die Phase einer komplexen Zahl zurückliefert in RAD
       friend Complex ComplexAP(double, double);// Funktion, die Betrag und Phase (RAD) in eine komplexe Zahl wandelt
       friend Complex Sqrt(Complex&);     // Wurzelfunktion liefert Hauptwert
       friend Complex Pow(Complex&,double);// Exponentialfunktion
       friend Complex Pow(Complex&, Complex&);// Exponentialfunktion
       friend Complex Sin(Complex&);      // Liefert den Sinus
       friend Complex Cos(Complex&);      // Liefert den Cosinus
       friend Complex Tan(Complex&);      // Liefert den Tangens
       friend Complex Sinh(Complex&);     // Liefert den Sinus Hyperbolicus
       friend Complex Cosh(Complex&);     // Liefert den Cosinus Hyperbolicus
       friend Complex Tanh(Complex&);     // Liefert den Tangens Hyperbolicus
       friend Complex Logn(Complex&);     // Liefert den natürlichen Logarithmus
       friend Complex Logb(double, Complex&);// Liefert den Logarithmus zur Basis b
       friend Complex Logb(Complex&, Complex&);// Liefert den Logarithmus zur Komplexen Basis b
       friend Complex Asinh(Complex&);    // Liefert den Arcus Sinus Hyperbolicus
       friend Complex Acosh(Complex&);    // Liefert den Arcus Cosinus Hyperbolicus
       friend Complex Atanh(Complex&);    // Liefert den Arcus Tangens Hyperbolicus

       friend Complex operator+(Complex&, Complex&);
       friend Complex operator+(double, Complex&);
       friend Complex operator+(Complex&, double);
       friend Complex operator-(Complex&, Complex&);
       friend Complex operator-(double, Complex&);
       friend Complex operator-(Complex&, double);
       friend Complex operator*(Complex&, Complex&);
       friend Complex operator*(Complex&, double);
       friend Complex operator*(double, Complex&);
       friend Complex operator/(Complex&, Complex&);
       friend Complex operator/(Complex&, double);
       friend Complex operator/(double, Complex&);
       friend int     operator==(Complex&, Complex&);
       friend int     operator!=(Complex&, Complex&);

       Complex&       operator+=(Complex&);
       Complex&       operator+=(double);
       Complex&       operator-=(Complex&);
       Complex&       operator-=(double);
       Complex&       operator*=(Complex&);
       Complex&       operator*=(double);
       Complex&       operator/=(Complex&);
       Complex&       operator/=(double);
       Complex operator+();
       Complex operator-();

   // private: // auskomentiert 16.10.96 für Directzugriffe
       double realteil,imagteil;         // Datenelemente der Klasse (Realteil und Imaginärteil)
         };

                              // Definition der Elementfunktionen

inline Complex::Complex(double _re_value,double _im_value)
{
   realteil = _re_value;
   imagteil = _im_value;
}      
inline Complex::Complex()
{
    realteil = 0.0;
    imagteil = 0.0;
}

inline Complex Complex::operator+()
{
   return *this;
}

inline Complex Complex::operator-()
{
   return Complex(-realteil, -imagteil);
}

inline Complex& Complex::operator+=(Complex& wert)
{
   realteil += wert.realteil;
   imagteil += wert.imagteil;
   return *this;
}

inline Complex& Complex::operator+=(double wert)
{
   realteil += wert;
   return *this;
}

inline Complex& Complex::operator-=(Complex& wert)
{
   realteil -= wert.realteil;
   imagteil -= wert.imagteil;
   return *this;
}

inline Complex& Complex::operator-=(double wert)
{
   realteil -= wert;
   return *this;
}

inline Complex& Complex::operator*=(Complex& wert)
{  double zwischen;

   zwischen = realteil*wert.realteil - imagteil*wert.imagteil;
   imagteil = realteil*wert.imagteil + wert.realteil*imagteil;
   realteil = zwischen;
   return *this;
}

inline Complex& Complex::operator*=(double wert)
{
   realteil *= wert;
   imagteil *= wert;
   return *this;
}

inline Complex& Complex::operator/=(Complex& wert)
{  double teil,zwischen;

   teil = wert.realteil*wert.realteil+wert.imagteil*wert.imagteil;
   zwischen = (realteil*wert.realteil+imagteil*wert.imagteil)/teil;
   imagteil = (wert.realteil*imagteil-realteil*wert.imagteil)/teil;
   realteil = zwischen;
   return *this;
}

inline Complex& Complex::operator/=(double wert)
{
   realteil /= wert;
   imagteil /= wert;
   return *this;
}

inline Complex operator+(Complex& _z1, Complex& _z2)
{
   return Complex(_z1.realteil + _z2.realteil, _z1.imagteil + _z2.imagteil);
}

inline Complex operator+(double real, Complex& _z2)
{
   return Complex(real + _z2.realteil, _z2.imagteil);
}

inline Complex operator+(Complex& _z1, double real)
{
   return Complex(_z1.realteil + real, _z1.imagteil);
}

inline Complex operator-(Complex& _z1, Complex& _z2)
{
   return Complex(_z1.realteil - _z2.realteil, _z1.imagteil - _z2.imagteil);
}

inline Complex operator-(double real, Complex& _z2)
{
   return Complex(real - _z2.realteil, -_z2.imagteil);
}

inline Complex operator-(Complex& _z1, double real)
{
   return Complex(_z1.realteil - real, _z1.imagteil);
}

inline Complex operator*(Complex& _z1, double real)
{
   return Complex(_z1.realteil*real, _z1.imagteil*real);
}

inline Complex operator*(double real, Complex& _z2)
{
   return Complex(_z2.realteil*real, _z2.imagteil*real);
}

inline Complex operator*(Complex& _z1, Complex& _z2)
{
   return Complex(_z1.realteil*_z2.realteil - _z1.imagteil*_z2.imagteil,
        _z1.realteil*_z2.imagteil + _z2.realteil*_z1.imagteil);
}



inline Complex operator/(Complex& _z1, Complex& _z2)
{  double teil;

   teil = _z2.realteil*_z2.realteil+_z2.imagteil*_z2.imagteil;
   return Complex( (_z1.realteil*_z2.realteil + _z1.imagteil*_z2.imagteil)/teil,
         (_z2.realteil*_z1.imagteil - _z1.realteil*_z2.imagteil)/teil);
}

inline Complex operator/(Complex& _z1, double real)
{
   return Complex(_z1.realteil/real, _z1.imagteil/real);
}

inline Complex operator/(double real, Complex& _z2)
{  double wert;

   wert = _z2.realteil*_z2.realteil+_z2.imagteil*_z2.imagteil;
   return Complex( ( real*_z2.realteil)/wert,
         (-real*_z2.imagteil)/wert);
}

inline int operator==(Complex& _z1, Complex& _z2)
{
   return _z1.realteil == _z2.realteil && _z1.imagteil == _z2.imagteil;
}

inline int operator!=(Complex& _z1, Complex& _z2)
{
   return _z1.realteil != _z2.realteil || _z1.imagteil != _z2.imagteil;
}

// Friend Funktionen der Klasse Complex


inline double Real(Complex& wert)
{
   return wert.realteil;
}

inline double Imag(Complex& wert)
{
   return wert.imagteil;
}

inline Complex Konj(Complex& wert)
{
   return Complex(wert.realteil,-wert.imagteil);
}

inline double Betrag(Complex& wert)
{
   return _hypot(wert.realteil,wert.imagteil);
}

inline double Asqr(Complex &wert)
{
   return wert.realteil*wert.realteil+wert.imagteil*wert.imagteil;
}

inline double Phase(Complex& wert)
{
   if (wert.realteil == 0.0)
   {
      if (wert.imagteil > 0.0)     return(90.0);
      else
	    if (wert.imagteil < 0.0)    return(-90.0);
	    else                        return(0.0);
   }
   else if (wert.imagteil == 0.0)
   {
	    if (wert.realteil > 0.0)    return(0.0);
	    else                        return(180.0);
   }
   else return(atan2(wert.imagteil,wert.realteil)*180/M_PI);
}

inline double PhaseRAD(Complex& wert)
{
   if (wert.realteil == 0.0)
   {
      if (wert.imagteil > 0.0) return M_PI*0.5;
      else
      if (wert.imagteil < 0.0) return -M_PI*0.5;
      else                     return 0;
   }
   else if (wert.imagteil == 0.0)
   {
      if (wert.realteil > 0.0) return  0;
      else                     return  M_PI;
   }
   else return  atan2(wert.imagteil,wert.realteil);
}

inline Complex Sqrt(Complex& wert) //implementiert nach Numerical Recipes in C, S.711/12
{
    Complex c(0.0,0.0);
    double x,y,w,r;

    if ((wert.realteil == 0.0)&&(wert.imagteil == 0.0))
        return c;

    else
    {
        x = fabs(wert.realteil);
        y = fabs(wert.imagteil);
        if (x>=y)
        {
            r = y/x;
            w = sqrt(x)*sqrt(0.5*(1.0+sqrt(1.0+r*r)));
        }
        else
        {
            r = x/y;
            w = sqrt(y)*sqrt(0.5*(r+sqrt(1.0+r*r)));
        }
        if (wert.realteil>=0.0)
        {
            c.realteil = w;
            c.imagteil = wert.imagteil/(2.0*w);
        }
        else
        {
            c.imagteil = (wert.imagteil >= 0.0) ? w : -w;
            c.realteil = wert.imagteil/(2.0*c.imagteil);
        }
        return c;
    }
}

inline Complex ComplexAP(double dAmplitude, double dPhase)
{
   Complex c;
   c.realteil = dAmplitude * cos(dPhase);
   c.imagteil = dAmplitude * sin(dPhase);
   return c;
}

inline Complex Pow(Complex& wert, double dPot)
{
   Complex c;
   double dBetrag = pow(Betrag(wert), dPot);
   double dPhase  = PhaseRAD(wert) * dPot;
   return ComplexAP(dBetrag, dPhase);
}

inline Complex Pow(Complex& wert1, Complex &wert2)
{
   Complex c;
   double dA1    = Betrag(wert1);
   double dA2    = Betrag(wert2);
   double dPhi1  = PhaseRAD(wert1);
   double dPhi2  = PhaseRAD(wert2);
   double dLnA1  = log(dA1);
   double dSinP2 = sin(dPhi2);
   double dCosP2 = cos(dPhi2);
   double djx    = dA2 * (dLnA1*dSinP2 + dPhi1*dCosP2);
   c.realteil = cos(djx);
   c.imagteil = sin(djx);
   c *= exp(dA2*(dLnA1*dCosP2 - dPhi1*dSinP2));
   return c;
}

inline Complex Logn(Complex& wert)
{
   double dBetrag = Betrag(wert);
   double dPhase  = PhaseRAD(wert);
   return Complex(log(dBetrag), dPhase);
}

inline Complex Logb(double b, Complex& wert)
{
   return Logn(wert) / log(b);
}

inline Complex Logb(Complex& b, Complex& wert)
{
   return Logn(wert) / Logn(b);
}

inline Complex Asinh(Complex& wert)
{
   return Logn(wert + Sqrt(wert * wert + Complex(1, 0)));
}

inline Complex Acosh(Complex& wert)
{
   return Logn(wert + Sqrt(wert * wert - Complex(1, 0)));
}

inline Complex Atanh(Complex& wert)
{
   return 0.5 * Logn((Complex(1, 0) + wert) / (Complex(1, 0) - wert));
}

inline Complex Sin(Complex& wert)
{
   Complex c;
   c.realteil = sin(wert.realteil) * cosh(wert.imagteil);
   c.imagteil = cos(wert.realteil) * sinh(wert.imagteil);
   if (wert.imagteil <= 0) c.imagteil = -c.imagteil;
   return c;
}

inline Complex Cos(Complex& wert)
{
   Complex c;
   c.realteil = cos(wert.realteil) * cosh(wert.imagteil);
   c.imagteil = sin(wert.realteil) * sinh(wert.imagteil);
   if (wert.imagteil > 0) c.imagteil = -c.imagteil;
   return c;
}

inline Complex Tan(Complex& wert)
{
   Complex c;
   wert *= 2;
   c.realteil = sin(wert.realteil);
   c.imagteil = sinh(wert.imagteil);
   if (wert.imagteil < 0) c.imagteil = -c.imagteil;
   c /= (cos(wert.realteil) + cosh(wert.imagteil));
   return c;
}

inline Complex Sinh(Complex& wert)
{
   Complex c;
   c.realteil = sinh(wert.realteil) * cos(wert.imagteil);
   c.imagteil = cosh(wert.realteil) * sin(wert.imagteil);
   if (wert.imagteil <= 0) c.imagteil = -c.imagteil;
   return c;
}

inline Complex Cosh(Complex& wert)
{
   Complex c;
   c.realteil = cosh(wert.realteil) * cos(wert.imagteil);
   c.imagteil = sinh(wert.realteil) * sin(wert.imagteil);
   if (wert.imagteil > 0) c.imagteil = -c.imagteil;
   return c;
}

inline Complex Tanh(Complex& wert)
{
   Complex c;
   wert *= 2;
   c.realteil = sinh(wert.realteil);
   c.imagteil = sin(wert.imagteil);
   if (wert.imagteil < 0) c.imagteil = -c.imagteil;
   c /= (cosh(wert.realteil) + cos(wert.imagteil));
   return c;
}

#pragma warning( pop)

#endif // (__CALCMPLX_H)


