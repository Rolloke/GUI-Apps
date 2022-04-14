#if !defined(__CALCMPLX_H)
#define __CALCMPLX_H

#ifndef __cplusplus
#error Must use C++ for the type complex.
#endif

#include<math.h>

class Complex {

   public:
								      // Prototypen der Elementfunktion der Klasse

		 Complex(double _re_value,double _im_value=0);        // Konstruktoren
		 Complex();
								      // WertrÅckgabefunktionen von komplexen Zahlen
		 friend double  Real(Complex&);                       // Funktion, die den Realteil zurÅckliefert
		 friend double  Imag(Complex&);                       // Funktion, die den ImaginÑrteil zurÅckliefert
		 friend Complex Konj(Complex&);			      // Funktion, die die konjugiert komplexe Zahl liefert
		 friend double  Betrag(Complex&);                     // Funktion, die den Betrag einer komplexen Zahl zurÅckliefert
		 friend double  Asqr(Complex&);                       // Funktion, die das Quadrat des Betrages einer komplexen Zahle liefert
		 friend double  Phase(Complex&);		      // Funktion, die die Phase einer komplexen Zahl zurÅckliefert in Degree
		 friend Complex Sqrt(Complex&);                       // Wurtzenfunktion liefert Hauptwert

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
		 Complex	operator+();
		 Complex	operator-();

   // private: // auskomentiert 16.10.96 fÅr Directzugriffe
		 double realteil,imagteil;                            // Datenelemente der Klasse (Realteil und ImaginÑrteil)
	      };

								      // Definition der Elementfunktionen

inline Complex::Complex(double _re_value,double _im_value)
{
   realteil = _re_value;
   imagteil = _im_value;
}      
inline Complex::Complex()
{
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
   return hypot(wert.realteil,wert.imagteil);
}

inline double Asqr(Complex &wert)
{
   return wert.realteil*wert.realteil+wert.imagteil*wert.imagteil;
}

#endif // (__CALCMPLX_H)

