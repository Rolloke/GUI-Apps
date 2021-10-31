/********************************************************************/
/*                                                                  */
/*                     Library Cara3DTr                             */
/*                                                                  */
/*        enthält Funktionen und globale Variablen für 3D           */
/*        Transformationen mit Matrizen                             */ 
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
#define  STRICT
#include <Windows.h>

#include "Cara3DTr.h"
#include "CVector.h"

Array44  g_Tran;
Array44  g_InvTran;
Array44 *g_pTran    = &g_Tran;
Array44 *g_pInvTran = &g_InvTran;

void LoadIdentity()
{
   ZeroMemory(g_pTran   , sizeof(double) * 16);
   ZeroMemory(g_pInvTran, sizeof(double) * 16);
   g_pTran->a.m11 = 1.0;
   g_pTran->a.m22 = 1.0;
   g_pTran->a.m33 = 1.0;
   g_pTran->a.m44 = 1.0;
}

/************************************************************************
* Name   : CreateTrMatrix                                               *
* Zweck  : Multiplizieren einer 3D-Transformationsmatrix	               *
* Aufruf : CreateTrMatrix(vFix, vScale, phix, phiy, phiz, vTran);       *
* Parameter    :                                                        *
* vFix     (E) : Rotationspunkt                              (CVector&) *
* vScale   (E) : Scalierungsvector                           (CVector&) *
* phix	  (E) : Drehwinkel um die x-Achse [rad]             (double)   *
* phiy	  (E) : Drehwinkel um die y-Achse [rad]             (double)   *
* phiz	  (E) : Drehwinkel um die z-Achse [rad]             (double)   *
* vTran    (E) : Verschiebungsvektor                         (CVector&) *
************************************************************************/
void CreateTrMatrix(CVector& vFix, CVector& vScale, double phix, double phiy, double phiz, CVector& vTran)
{
   Scale(vScale);
   Translate(-vFix);
   Rotate_x(phix);
   Rotate_y(phiy);
   Rotate_z(phiz);
   Translate(vTran+vFix);
}

void GetMatrix(Array44 &m)
{
   m = *g_pTran;
}

void SetMatrix(Array44 &m)
{
   *g_pTran = m;
}

bool SetMatrixPtr(Array44 *pm, bool bCalc)
{
   if (pm == NULL) g_pTran = &g_Tran;
   else            g_pTran = pm;
   if (bCalc) return CalcInverseTran();
   return true;
}

bool SetReMatrixPtr(Array44 *pm, bool bCalc)
{
   if (pm == NULL) g_pInvTran = &g_InvTran;
   else            g_pInvTran = pm;
   if (bCalc) return CalcInverseTran();
   return true;
}

/************************************************************************
* Name   : MultMatrix	                                                *
* Zweck  : Multiplizieren einer 4*4 Matrix                              *
* Aufruf : MultMatrix(m1)                                               *
* Parameter    :                                                        *
* m1	   (E) : Eingangsmatrix 1                          (double[4][4]) *
************************************************************************/
void MultMatrix(Array44 m1)
{
   Array44 m2 = *g_pTran;
   int i,j,k;
   for (i=0; i<4; i++)        // Zeile 
   {
      for (j=0; j<4; j++)     // Spalte
      {
	       k=0;
	       g_pTran->m[j][i] = m1.m[j][k] * m2.m[k][i];
	       for (k++; k<4; k++)
	       {
	          g_pTran->m[j][i] += m1.m[j][k] * m2.m[k][i];
	       }
      }
   }
}

/************************************************************************
* Name   : Rotate_x                                                     *
* Zweck  : Multiplizieren einer Rotationsmatrix um die x-Achse          *
* Aufruf : Rotate_x(phix, ma)                                           *
* Parameter    :                                                        *
* phix	   (E) : Drehwinkel um die x-Achse [rad]              (double) *
************************************************************************/
void Rotate_x(double phix)
{
   Array44 m;
   ZeroMemory(&m, sizeof(double) * 16);
   double sinphi = sin(phix);
   double cosphi = cos(phix);
   m.a.m11 = 1.0,
   m.a.m22 = cosphi, m.a.m23 =-sinphi;
   m.a.m32 = sinphi, m.a.m33 = cosphi;
   m.a.m44 = 1.0;
   MultMatrix(m);
}

/************************************************************************
* Name   : Rotate_y                                                     *
* Zweck  : Multiplizieren einer Rotationsmatrix um die y-Achse          *
* Aufruf : Rotate_y(phiy, ma)                                           *
* Parameter    :                                                        *
* phiy	   (E) : Drehwinkel um die y-Achse [rad]              (double) *
************************************************************************/
void Rotate_y(double phiy)
{
   Array44 m;
   ZeroMemory(&m, sizeof(double) * 16);
   double sinphi = sin(-phiy);
   double cosphi = cos(-phiy);
   m.a.m11 = cosphi, m.a.m13 =-sinphi;
   m.a.m22 = 1.0;
   m.a.m31 = sinphi, m.a.m33 = cosphi;
   m.a.m44 = 1.0;
   MultMatrix(m);
}

/************************************************************************
* Name   : Rotate_z                                                     *
* Zweck  : Multiplizieren einer Rotationsmatrix um die z-Achse          *
* Aufruf : Rotate_z(phiy, ma)                                           *
* Parameter    :                                                        *
* phiz	   (E) : Drehwinkel um die z-Achse [rad]              (double) *
************************************************************************/
void Rotate_z(double phiz)
{
   Array44 m;
   ZeroMemory(&m, sizeof(double) * 16);
   double sinphi = sin(phiz);
   double cosphi = cos(phiz);
   m.a.m11 = cosphi, m.a.m12 =-sinphi;
   m.a.m21 = sinphi, m.a.m22 = cosphi;
   m.a.m33 = 1.0;
   m.a.m44 = 1.0;
   MultMatrix(m);
}

/************************************************************************
* Name   : Translate                                                    *
* Zweck  : Multiplizieren einer Translationsmatrix                      *
* Aufruf : Translate(tr, ma)                                            *
* Parameter  :                                                          *
* tr	   (E): Verschiebungsvektor                              (double) *
************************************************************************/
void Translate(CVector& vTran)
{
   Array44 m;
   ZeroMemory(&m, sizeof(double) * 16);
   m.a.m14 = Vx(vTran);
   m.a.m24 = Vy(vTran);
   m.a.m34 = Vz(vTran);
   m.a.m11 = m.a.m22 = m.a.m33 = m.a.m44 = 1.0;
   MultMatrix(m);
}

/************************************************************************
* Name   : Scale                                                        *                      
* Zweck  : Multiplizieren einer Scalierungsmatrix                       *
* Aufruf : Scale(sx,sy,sz,ma)                                           *
* Parameter    :                                                        *
* vScale   (E) : Scalierungsvector                           (CVector&) *
************************************************************************/
void Scale(CVector& vScale)
{
   Array44 m;
   ZeroMemory(&m, sizeof(double) * 16);
   m.a.m11 = Vx(vScale);
   m.a.m22 = Vy(vScale);
   m.a.m33 = Vz(vScale);
   m.a.m44 = 1.0;
   MultMatrix(m);
}

/************************************************************************
* Name   : Shear                                                        *
* Zweck  : Multiplizieren einer Schermatrix                             *
* Aufruf : Scale(sx,sy,sz,ma)                                           *
* Parameter    :                                                        *
* vScale   (E) : Shervector                                  (CVector&) *
************************************************************************/
void Shear(CVector& vShear)
{
   Array44 m;
   ZeroMemory(&m, sizeof(double) * 16);
   m.a.m11 = m.a.m22 = m.a.m33 = m.a.m44 = 1.0;
   m.a.m12 = Vx(vShear);
   m.a.m13 = Vx(vShear);
   m.a.m21 = Vy(vShear);
   m.a.m23 = Vy(vShear);
   m.a.m31 = Vz(vShear);
   m.a.m32 = Vz(vShear);
   MultMatrix(m);
}

/************************************************************************
* Name   : TransformVector                                              *
* Zweck  : Transformieren eines Punktes                                 *
* Aufruf : TransformVector(pi, tran);                                   *
* Parameter   :                                                         *
* pi	   (E) : zu transformierender Vector                    (CVector) *
************************************************************************/
void TransformVector(CVector& vTr)
{
   CVector vPo;
   double *dpi,*dpo,norm;
   dpi = (double*) &vTr;
   dpo = (double*) &vPo;

   dpo[0] = g_pTran->a.m11*dpi[0]+g_pTran->a.m12*dpi[1]+g_pTran->a.m13*dpi[2]+g_pTran->a.m14;
   dpo[1] = g_pTran->a.m21*dpi[0]+g_pTran->a.m22*dpi[1]+g_pTran->a.m23*dpi[2]+g_pTran->a.m24;
   dpo[2] = g_pTran->a.m31*dpi[0]+g_pTran->a.m32*dpi[1]+g_pTran->a.m33*dpi[2]+g_pTran->a.m34;

   norm   = g_pTran->a.m41*dpi[0]+g_pTran->a.m42*dpi[1]+g_pTran->a.m43*dpi[2]+g_pTran->a.m44;
   if (norm!=1.0)
   {
      vPo /= norm;
   }
   vTr = vPo;
}

/************************************************************************
* Name   : ReTransformVector                                            *
* Zweck  : rück transformieren eines Punktes                            *
* Aufruf : ReTransformVector(pi, tran);                                 *
* Parameter   :                                                         *
* pi	   (E) : zu transformierender Vector                    (CVector) *
************************************************************************/
void ReTransformVector(CVector& vTr)
{
   CVector vPo;
   double *dpi,*dpo;
   dpi = (double*) &vTr;
   dpo = (double*) &vPo;
   dpi[0] -= g_pTran->a.m14;
   dpi[1] -= g_pTran->a.m24;
   dpi[2] -= g_pTran->a.m34;

   dpo[0] = dpi[0] * g_pInvTran->a.m11 + dpi[1] * g_pInvTran->a.m12 + dpi[2] * g_pInvTran->a.m13;
   dpo[1] = dpi[0] * g_pInvTran->a.m21 + dpi[1] * g_pInvTran->a.m22 + dpi[2] * g_pInvTran->a.m23;
   dpo[2] = dpi[0] * g_pInvTran->a.m31 + dpi[1] * g_pInvTran->a.m32 + dpi[2] * g_pInvTran->a.m33;
   
   vTr = vPo;
}

bool CalcInverseTran()
{
   double dDet;
   dDet = g_pTran->a.m41 + g_pTran->a.m42 + g_pTran->a.m43 + g_pTran->a.m44 - 1.0;

   Array44 A;
   if (fabs(dDet) > 1e-16) // Ausgleichsrechnung
   {
      int i, j, k, m = 3, n = 4;
      for (i=0; i<m; i++)
      {
         for (j=i; j<m; j++)
         {
            g_pInvTran->m[i][j] = g_pTran->m[0][i]*g_pTran->m[0][j];
         }
      }
      for (i=0; i<m; i++)
      {
         for (j=i; j<m; j++)
         {
            for (k=1; k<n; k++)
               g_pInvTran->m[i][j] += g_pTran->m[k][i]*g_pTran->m[k][j];
            g_pInvTran->m[j][i] = g_pInvTran->m[i][j];
         }
      }
      CopyMemory(&A.m[0], g_pInvTran->m[0], sizeof(double)*3);
      CopyMemory(&A.m[1], g_pInvTran->m[1], sizeof(double)*3);
      CopyMemory(&A.m[2], g_pInvTran->m[2], sizeof(double)*3);
   }
   else
   {
      CopyMemory(&A.m[0], g_pTran->m[0], sizeof(double)*3);
      CopyMemory(&A.m[1], g_pTran->m[1], sizeof(double)*3);
      CopyMemory(&A.m[2], g_pTran->m[2], sizeof(double)*3);
   }

   dDet = A.a.m11*A.a.m22*A.a.m33 +
          A.a.m12*A.a.m23*A.a.m31 +
          A.a.m13*A.a.m21*A.a.m32 -
          A.a.m13*A.a.m22*A.a.m31 -
          A.a.m11*A.a.m23*A.a.m32 -
          A.a.m12*A.a.m21*A.a.m33;

   if (fabs(dDet) > 1e-16)
   {
      dDet = 1.0 / dDet;
      g_pInvTran->a.m11 = (A.a.m22 * A.a.m33 - A.a.m23 * A.a.m32) * dDet;
      g_pInvTran->a.m12 = (A.a.m13 * A.a.m32 - A.a.m12 * A.a.m33) * dDet;
      g_pInvTran->a.m13 = (A.a.m12 * A.a.m23 - A.a.m13 * A.a.m22) * dDet;

      g_pInvTran->a.m21 = (A.a.m23 * A.a.m31 - A.a.m21 * A.a.m33) * dDet;
      g_pInvTran->a.m22 = (A.a.m11 * A.a.m33 - A.a.m13 * A.a.m31) * dDet;
      g_pInvTran->a.m23 = (A.a.m13 * A.a.m21 - A.a.m11 * A.a.m23) * dDet;

      g_pInvTran->a.m31 = (A.a.m21 * A.a.m32 - A.a.m22 * A.a.m31) * dDet;
      g_pInvTran->a.m32 = (A.a.m12 * A.a.m31 - A.a.m11 * A.a.m32) * dDet;
      g_pInvTran->a.m33 = (A.a.m11 * A.a.m22 - A.a.m21 * A.a.m12) * dDet;

      return true;
   }
   return false;
}
