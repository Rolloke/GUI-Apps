#ifndef _C3DTRV_H_
#define _C3DTRV_H_

class CVector;

struct ARRAY44
{
   double m11, m21, m31, m41,
          m12, m22, m32, m42,
          m13, m23, m33, m43,
          m14, m24, m34, m44;
};

union Array44
{
   double  m[4][4];
   double  v[16];
   ARRAY44 a;
};

/************************************************************************
* Name   : Rotate_x                                                     *
* Zweck  : Setzt die Einheitsmatrix als aktuelle Matrix                 *
* Aufruf : Rotate_x(phix, ma)                                           *
* Parameter : keine                                                     *
************************************************************************/
void LoadIdentity();

/************************************************************************
* Name   : Rotate_x                                                     *
* Zweck  : Multiplizieren einer Rotationsmatrix um die x-Achse          *
* Aufruf : Rotate_x(phix, ma)                                           *
* Parameter    :                                                        *
* phix	   (E) : Drehwinkel um die x-Achse [rad]              (double) *
************************************************************************/
void Rotate_x(double);

/************************************************************************
* Name   : Rotate_y                                                     *
* Zweck  : Multiplizieren einer Rotationsmatrix um die y-Achse          *
* Aufruf : Rotate_y(phiy, ma)                                           *
* Parameter    :                                                        *
* phiy	   (E) : Drehwinkel um die y-Achse [rad]              (double) *
************************************************************************/
void Rotate_y(double);

/************************************************************************
* Name   : Rotate_z                                                     *
* Zweck  : Multiplizieren einer Rotationsmatrix um die z-Achse          *
* Aufruf : Rotate_z(phiy, ma)                                           *
* Parameter    :                                                        *
* phiz	   (E) : Drehwinkel um die z-Achse [rad]              (double) *
************************************************************************/
void Rotate_z(double);

/************************************************************************
* Name   : Translate                                                    *
* Zweck  : Multiplizieren einer Translationsmatrix                      *
* Aufruf : Translate(tr, ma)                                            *
* Parameter  :                                                          *
* tr	   (E): Verschiebungsvektor                            (CVector&) *
************************************************************************/
void Translate(CVector&);

/************************************************************************
* Name   : Scale                                                        *
* Zweck  : Multiplizieren einer Scalierungsmatrix                       *
* Aufruf : Scale(sx,sy,sz,ma)                                           *
* Parameter    :                                                        *
* vScale   (E) : Scalierungsvector                           (CVector&) *
************************************************************************/
void Scale(CVector&);

/************************************************************************
* Name   : Shear                                                        *                      
* Zweck  : Multiplizieren einer Schermatrix                             *
* Aufruf : Scale(sx,sy,sz,ma)                                           *
* Parameter    :                                                        *
* vScale   (E) : Shearvector                                 (CVector&) *
************************************************************************/
void Shear(CVector&);

/************************************************************************
* Name   : MultMatrix	                                                *
* Zweck  : Multiplizieren einer 4*4 Matrix mit der aktuellen Matrix     *
* Aufruf : MultMatrix(m1)                                               *
* Parameter    :                                                        *
* m1	   (E) : Eingangsmatrix 1                                (Array44)*
************************************************************************/
void MultMatrix(Array44);

/************************************************************************
* Name   : GetMatrix  	                                                *
* Zweck  : Liefert die aktuelle Transformationsmatrix                   *
* Aufruf : GetMatrix(m1)                                                *
* Parameter    :                                                        *
* m1	   (A) : aktuelle Matrix                                 (Array44)*
************************************************************************/
void GetMatrix(Array44&);

/************************************************************************
* Name   : SetMatrix  	                                                *
* Zweck  : Setzt die aktuelle Transformationsmatrix                     *
* Aufruf : SetMatrix(m1)                                                *
* Parameter    :                                                        *
* m1	   (E) : neue Matrix                                    (Array44)*
************************************************************************/
void SetMatrix(Array44&);
bool SetMatrixPtr(Array44*, bool);
bool SetReMatrixPtr(Array44*, bool);

/************************************************************************
* Name   : CreateTrMatrix                                               *
* Zweck  : Multiplizieren einer 3D-Transformationsmatrix, die um einen  *
*          Fixpunkt scaliert, rotiert und verschiebt.                   *
* Aufruf : CreateTrMatrix(vFix, vScale, phix, phiy, phiz, vTran);       *
* Parameter    :                                                        *
* vFix     (E) : Rotationspunkt                              (CVector&) *
* vScale   (E) : Scalierungsvector                           (CVector&) *
* phix	    (E) : Drehwinkel um die x-Achse [rad]             (double) *
* phiy	    (E) : Drehwinkel um die y-Achse [rad]             (double) *
* phiz	    (E) : Drehwinkel um die z-Achse [rad]             (double) *
* vTran    (E) : Verschiebungsvektor                         (CVector&) *
************************************************************************/
void CreateTrMatrix(CVector&, CVector&, double, double, double, CVector&);

/************************************************************************
* Name   : TransformVector                                              *
* Zweck  : Transformiert einen Vektor mit der aktuellen                 *
*          Transformationsmatrix                                        *
* Aufruf : ReTransformVector(v);                                        *
* Parameter  :                                                          *
* tr	  (EA): zu transformierender Vektor                    (CVector&) *
************************************************************************/
void TransformVector(CVector&);
/************************************************************************
* Name   : ReTransformVector                                            *
* Zweck  : ReTransformiert einen Vektor mit der inversen der aktuellen  *
*          Transformationsmatrix                                        *
* Aufruf : ReTransformVector(v);                                        *
* Parameter  :                                                          *
* tr	  (EA): zu transformierender Vektor                    (CVector&) *
************************************************************************/
void ReTransformVector(CVector&);

/************************************************************************
* Name   : Determinante                                                 *
* Zweck  : Berechnet die Determinante der Transformationsmatrix         *
* Aufruf : Determinante();                                              *
* Parameter  :                                                          *
* Rückgabe : Wert der Determinante                       (double)       *
************************************************************************/
bool CalcInverseTran();

#endif