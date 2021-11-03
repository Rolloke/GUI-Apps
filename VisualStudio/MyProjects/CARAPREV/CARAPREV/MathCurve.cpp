// MathCurve.cpp: Implementierung der Klasse CMathCurve.
//
//////////////////////////////////////////////////////////////////////

// für alle Programme, die nicht MFC einbinden, müssen die
// folgende Zeile auskommentieren.
#include "stdafx.h"

#ifdef CARA_MFC_HEADER
   #include "CEtsDiv.h"
#else
   #ifndef _WINDOWS_
      #define  STRICT
      #include <windows.h>
   #endif
   #include <commctrl.h>
   #include "Debug.h"
   #define LOG10_2 0.301029995664
#endif

#include "MathCurve.h"
#include "CETSMathClass.h"
#include <cmath>
#include <float.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define UNIT_STR_LEN 256

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////


/******************************************************************************
* Name       : SortXAsc                                                       *
* Definition : SortXAsc()                                                     *
* Zweck      : Sortieren der Kurvenwerte nach aufsteigenden X-Werten          *
* Aufruf     : SortXAsc();                                                    *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CMathCurve::SortXAsc()
{
   switch (m_Parameter.format)
   {
      case CF_DOUBLE: qsort((void*)m_pArray, m_nNum, sizeof(SCurve), SCurvexSortfunc);  break;
      case CF_FLOAT:  qsort((void*)m_pArray, m_nNum, sizeof(SCurve), SCurvexSortfuncF); break;
      case CF_SHORT:  qsort((void*)m_pArray, m_nNum, sizeof(SCurve), SCurvexSortfuncS); break;
      case CF_ARBITRARY:
      case CF_ARBITRARY_CONTR:
      if (m_pGetCrvParamFkt)
      {
         CF_GET_CURVE_PARAMS gcp = {CF_SORT_XASC, 0, this, NULL, m_pGetCrvParam};
         VERIFY(m_pGetCrvParamFkt(&gcp)!=0);
      } break;
      case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:                break;
      default: ASSERT(false); break;
   }
   SetSorted(true);
}


/******************************************************************************
* Name       : Derive_X                                                       *
* Definition : bool Derive_X(CMathCurve *pmc=NULL);                           *
* Zweck      : Ableiten der Kurve nach X                                      *
* Aufruf     : Derive_X([pmc])                                                *
* Parameter  :                                                                *
* [pmc]  (EA): Zeiger auf ein neues Kurvenobjekt. [optional]    (CMathCurve*) *
*              Wird hier ein gültiger Zeiger auf ein Kurvenobjekt übergeben,  *
*              so wird das Ergebnis der Berechnung in dem übergebenen Objekt  *
*              zurückgegeben. Anderenfalls wird dieses Objekt verändert.      *
* Funktionswert : (true, false)                                               *
******************************************************************************/
// Berechnung der Steigung für die Werte (i, i+1)
// und Zuweisung an neu berechneten X-Wert(i) mit dem Steigungswert Y(i)
bool CMathCurve::Derive_X(CMathCurve *pmc)
{
   int      i, iy=0;

   if (!m_pArray)             return false;
   if (pmc == NULL)           return false;
   if (!pmc->SetSize(m_nNum)) return false;

   if (GetLocus())
   {
      CFloatPrecision _fp;
      Complex dC;
      SCurve  sc1, sc2;
      double  dl;
      for (i=0; i<m_nNum; i++)
      {
         if (i==0)
         {
            dl  = GetLocusValue(i+1) - GetLocusValue(i);
            sc1 = GetCurveValue(i+1);
            sc2 = GetCurveValue(i);
         }
         else if (i==m_nNum-1)
         {
            dl = GetLocusValue(i) - GetLocusValue(i-1);
            sc1 = GetCurveValue(i);
            sc2 = GetCurveValue(i-1);
         }
         else
         {
            dl = GetLocusValue(i+1) - GetLocusValue(i-1);
            sc1 = GetCurveValue(i+1);
            sc2 = GetCurveValue(i-1);
         }
         dC = (*((Complex*)&sc1)-*((Complex*)&sc2)) / dl;
         pmc->SetCurveValue(i, *((SCurve*)&dC)); 
      }

      pmc->DivideUnit(m_pszUnitLocus);
      pmc->SetXUnit(pmc->GetYUnit());
      return true;
   }
   else if (IsSorted())
   {
      double   dx, dy;
      CFloatPrecision _fp;

      for (i=0; i<m_nNum; i++)
      {
         if (i==0)
         {
            dx = GetX_Value(i+1) - GetX_Value(i);
            dy = GetY_Value(i+1) - GetY_Value(i);
         }
         else if (i==m_nNum-1)
         {
            dx = GetX_Value(i) - GetX_Value(i-1);
            dy = GetY_Value(i) - GetY_Value(i-1);
         }
         else
         {
            dx = GetX_Value(i+1) - GetX_Value(i-1);
            dy = GetY_Value(i+1) - GetY_Value(i-1);
         }

         pmc->SetX_Value(i, GetX_Value(i));
         if (dx <= 0)                          return false;      // wenn die X-Werte nicht stetig steigen: Fehler
         pmc->SetY_Value(i, dy / dx);                             // differenzieren
      }

      pmc->DivideUnit(m_pszUnitx);
      return true;
   }
   return false;
}



/******************************************************************************
* Name       : Integrate_X                                                    *
* Definition : bool Integrate_X(double xstart, CMathCurve *pmc=NULL);         *
* Zweck      : Integrieren der Kurve zur X-Achse                              *
* Aufruf     : Integrate_X(ystart[, pmc]);                                    *
* Parameter  :                                                                *
* ystart (E) : Startwert für die Integration                    (double)      *
* [pmc]  (EA): Zeiger auf ein neues Kurvenobjekt. [optional]    (CMathCurve*) *
*              Wird hier ein gültiger Zeiger auf ein Kurvenobjekt übergeben,  *
*              so wird das Ergebnis der Berechnung in dem übergebenen Objekt  *
*              zurückgegeben. Anderenfalls wird dieses Objekt verändert.      *
* Funktionswert : (true, false)                                               *
******************************************************************************/
bool CMathCurve::Integrate_X(double ystart, CMathCurve *pmc)
{
   int      i, nPStep, nPCount=0, nPProg=0, num = m_nNum;

   if (!pmc->SetSize(num)) return false;

   nPStep = (num-1) >> 7;                                      // Fortschrittsanzeige num / 128
   if (!nPStep) nPStep++;

   if (GetLocus())
   {
      double  dx;
      Complex cy;
      SCurve  sc1, sc2;
      CFloatPrecision _fp;

      SetLocusUnit(pmc->GetLocusUnit());

      if (ystart == 0)
      {
         sc1 = GetCurveValue(0);
         sc2 = GetCurveValue(1);
         cy  = (*((Complex*)&sc2) + *((Complex*)&sc1)) * 0.5;
         dx  = GetLocusValue(1) - GetLocusValue(0);
         cy  = cy * dx;
      }
      pmc->SetCurveValue(0, *((SCurve*)&cy));                   // Startwert für die Integration

      // Der Punkt p, für den die Fläche berechnet wird liegt in der Mitte eines Intervalls x1, x2
      for (i=0; i<num; i++)
      {
         if (m_hProgressWnd)
         {
            if (!::IsWindowVisible(m_hProgressWnd))
               return false;
            if (++nPCount>=nPStep)
            {
               SendMessage(m_hProgressWnd, PBM_SETPOS, ++nPProg, 0L);
               nPCount=0;
            }
         }

         if (i>0)
         {
            sc1 = GetCurveValue(i-1);
            sc2 = GetCurveValue(i);
            cy  = (*((Complex*)&sc2) + *((Complex*)&sc1)) * 0.5;
            dx  = GetLocusValue(i) - GetLocusValue(i-1);
            sc1 = pmc->GetCurveValue(i-1);
            cy  = *((Complex*)&sc1) + (cy * dx);              // integrieren
            pmc->SetCurveValue(i, *((SCurve*)&cy));
         }
      }

      pmc->MultiplyUnit(m_pszUnitLocus);
      pmc->SetXUnit(pmc->GetYUnit());
   }
   else if (IsSorted())
   {
      double   dx, y;
      CFloatPrecision _fp;

      if (ystart == 0)
      {
         y  = (GetY_Value(1) + GetY_Value(0))*0.5;
         dx =  GetX_Value(1) - GetX_Value(0);
         ystart = y * dx;
      }
      pmc->SetY_Value(0, ystart);                                 // Startwert für die Integration

      // Der Punkt p, für den die Fläche berechnet wird liegt in der Mitte eines Intervalls x1, x2
      for (i=0; i<num; i++)
      {
         if (m_hProgressWnd)
         {
            if (!::IsWindowVisible(m_hProgressWnd))
               return false;
            if (++nPCount>=nPStep)
            {
               SendMessage(m_hProgressWnd, PBM_SETPOS, ++nPProg, 0L);
               nPCount=0;
            }
         }

         pmc->SetX_Value(i, GetX_Value(i));

         if (i>0)
         {
            y  = (GetY_Value(i) + GetY_Value(i-1))*0.5;
            dx =  GetX_Value(i) - GetX_Value(i-1);
            pmc->SetY_Value(i, pmc->GetY_Value(i-1) + y * dx);               // integrieren
         }
      }
      pmc->MultiplyUnit(m_pszUnitx);
   }
   return true;
}
/******************************************************************************
* Name       : FFT                                                    *
* Definition : bool FFT(bool bT2F, CMathCurve *pmc=NULL);                     *
* Zweck      : Berechnen einer FFT für die Kurvenwerte                        *
* Aufruf     : FFT(bT2F, pmc]);                                               *
* Parameter  :                                                                *
* bT2F (E) : (true: t -> F, false: F -> t)                                    *
* pmc  (EA): Zeiger auf ein neues Kurvenobjekt.                 (CMathCurve*) *
* Funktionswert : (true, false)                                               *
******************************************************************************/
bool CMathCurve::FFT(bool bT2F, CMathCurve *pmc/*=NULL*/)
{
   if (!m_pArray)             return false;
   if (pmc == NULL)           return false;
   int i, nOrder, nNum, nFormat = pmc->GetFormat();

   if (GetLocus())
   {
      // Complex FFT
   }
   else if (IsSorted())
   {
      if (   nFormat != CF_SINGLE_DOUBLE
         && nFormat != CF_SINGLE_FLOAT)
      {
         nFormat = CF_SINGLE_FLOAT;
         if (!pmc->SetFormat(nFormat))
         {
            return false;
         }
      }

      nOrder = (int)ceil(log10((double)m_nNum) / LOG10_2);
      nNum = (int)pow(2.0, (double)nOrder);
      if (!pmc->SetSize(nNum))
      {
         return false;
      }

      if (nFormat == CF_SINGLE_FLOAT)
      {
         for (i=0; i<m_nNum; i++)
         {
            ((float*)pmc->m_pArray)[i] = (float)GetY_Value(i);
         }
         for (; i<nNum; i++)
         {
            ((float*)pmc->m_pArray)[i] = 0.0f;
         }
         CETSMathClass::RealFFT(bT2F ? 0 : 1, nOrder, (float*)pmc->m_pArray, NULL, NULL);
      }
      else if (nFormat == CF_SINGLE_DOUBLE)
      {
         for (i=0; i<m_nNum; i++)
         {
            ((double*)pmc->m_pArray)[i] = GetY_Value(i);
         }
         for (; i<nNum; i++)
         {
            ((double*)pmc->m_pArray)[i] = 0.0;
         }
         CETSMathClass::RealFFT(bT2F ? 0 : 1, nOrder, (double*)pmc->m_pArray, NULL, NULL);
      }
   }
   return true;
}
/******************************************************************************
* Name       : Average                                                        *
* Definition : bool Average(double, double, int, CMathCurve);                 *
* Zweck      : Mittelwertbildung über einen Bereich an einem Punkt innerhalb  *
*              dieses Bereichs.                                               *
* Aufruf     : Integrate_X(range, where, nsteps, pmc);                        *
* Parameter  :                                                                *
* range  (E) : Bereich über den gemittelt wird (!) linear       (double)      *
*              Ist die Kurveneinteilung logarithmisch, so muß range ein       *
*              Faktor größer als 1 sein.                                      *
* where  (E) : Lage des Punktes auf der X-Achse bezogen auf den (double)      *
*              Interval, an dem der berechnete Mittelwert gespeichert wird.   *
*              Vorzugsweise (0 <= x <= 1); 0.5 bedeutet Mitte des Intervals.  *
* pmc    (EA): Zeiger auf ein neues Kurvenobjekt.               (CMathCurve*) *
* Funktionswert : (true, false)                                               *
******************************************************************************/
bool CMathCurve::Average(double range, double where, CMathCurve *pmc)
{
   int      i, j, nStart, nEnd, nPStep, nPCount=0, nPProg=0, num = m_nNum;
   UINT     count;
   double   xstart, xend, sum;

   if (!IsWindow(m_hProgressWnd)) m_hProgressWnd = NULL;

   if (!m_pArray)          return false;
   if (!pmc)               return false;     // keine kurve vorhanden
   if (!pmc->SetSize(num)) return false;     // neue Kurve erzeugen

   nPStep = num >> 7;                        // Fortschrittsanzeige num / 128
   if (!nPStep) nPStep++;

   if (GetLocus())
   {
      CFloatPrecision _fp;
      Complex cSum;
      SCurve  sc;
      for (i=0; i<num; i++)
      {
         if (m_hProgressWnd)
         {
            if (!::IsWindowVisible(m_hProgressWnd))
               return false;
            if (++nPCount>=nPStep)
            {
               SendMessage(m_hProgressWnd, PBM_SETPOS, ++nPProg, 0L);
               nPCount=0;
            }
         }

         if (m_Parameter.xDivision== CCURVE_LINEAR)
         {
            xstart = GetLocusValue(i) - range * where;
            xend   = xstart + range;
         }
         else
         {
            xstart = GetLocusValue(i) * pow(range, -where);
            xend   = xstart * range;
         }

         xstart -= MC_EPS;                                        // Rundungsfehler vermeiden
         xend   += MC_EPS;

         Get_Locus_Value(xstart, &nStart);
         Get_Locus_Value(xend  , &nEnd);
         cSum = Complex(0, 0);
         for (j=nStart; j<nEnd; j++)
         {
            sc = GetCurveValue(j);
            cSum += *((Complex*)&sc);
         }
         count = nEnd - nStart;

         if (count < 2)
         {
            pmc->SetCurveValue(i, *((SCurve*)&cSum));
            continue;
         }
   //      TRACE("Count : %d\n", count);
         cSum /= (double)count;
         pmc->SetCurveValue(i, *((SCurve*)&cSum));
      }
   }
   else if (IsSorted())
   {
      CFloatPrecision _fp;

      for (i=0; i<num; i++)
      {
         if (m_hProgressWnd)
         {
            if (!::IsWindowVisible(m_hProgressWnd))
               return false;
            if (++nPCount>=nPStep)
            {
               SendMessage(m_hProgressWnd, PBM_SETPOS, ++nPProg, 0L);
               nPCount=0;
            }
         }

         if (m_Parameter.xDivision== CCURVE_LINEAR)
         {
            xstart = GetX_Value(i) - range * where;
            xend   = xstart + range;
         }
         else
         {
            xstart = GetX_Value(i) * pow(range, -where);
            xend   = xstart * range;
         }
         xstart -= MC_EPS;                                        // Rundungsfehler vermeiden
         xend   += MC_EPS;
         Get_Y_Value(xstart, &nStart);
         Get_Y_Value(xend  , &nEnd);
         for (j=nStart, sum=0.0; j<nEnd; j++)
         {
            sum += GetY_Value(j);
         }
         count = nEnd - nStart;
         pmc->SetX_Value(i, GetX_Value(i));
         if (count < 2)
         {
            pmc->SetY_Value(i, GetY_Value(i));
            continue;
         }
   //      TRACE("Count : %d\n", count);
         pmc->SetY_Value(i, sum / (double)count);
      }
   }
   return true;
}

/******************************************************************************
* Name       : OperateValue                                                   *
* Definition : bool OperateValue(double, int, CMathCurve *pmc = NULL);        *
* Zweck      : Mathematische Grundoperationen mit den Y-Werten der Kurve und  *
*              dem Operanden value;                                           *
* Aufruf     : OperateValue(what, value[, pmc]);                              *
* Parameter  :                                                                *
* what   (E) : durchzuführende Operation                        (int)         *
*              (MC_ADD, MC_SUB, MC_MUL, MC_DIV, MC_POW, MC_LOG, MC_LOG10)     *
* value  (E) : Operand                                          (double)      *
* [pmc]  (EA): Zeiger auf ein neues Kurvenobjekt. [optional]    (CMathCurve*) *
*              Wird hier ein gültiger Zeiger auf ein Kurvenobjekt übergeben,  *
*              so wird das Ergebnis der Berechnung in dem übergebenen Objekt  *
*              zurückgegeben. Anderenfalls wird dieses Objekt verändert.      *
* Funktionswert : (true, false)                                               *
******************************************************************************/
bool CMathCurve::OperateValue(int  what, double value, CMathCurve *pmc)
{
   int      nPStep, nPCount=0, nPProg=0, i, num = m_nNum;
   double   dY(0), dYerg(0);
   if (!m_pArray)                        return false;
   if (!IsSorted())                      return false;
   if ((what == MC_DIV) && (value == 0)) return false;         // nicht durch 0 teilen !!
   if (((what == MC_LOG)||(what == MC_LOG10)) && (value <= 0)) 
                                         return false;         // Ergebnis undefiniert !!
   if (!pmc->SetSize(num))               return false;         

   nPStep = num >> 7;                                          // Fortschrittsanzeige num / 128
   if (!nPStep) nPStep++;

   CFloatPrecision _fp;

   for (i=0; i<num; i++)
   {
      if (m_hProgressWnd)
      {
         if (!::IsWindowVisible(m_hProgressWnd))
            return false;
         if (++nPCount>=nPStep)
         {
            SendMessage(m_hProgressWnd, PBM_SETPOS, ++nPProg, 0L);
            nPCount=0;
         }
      }
      pmc->SetX_Value(i, GetX_Value(i));
      dY = GetY_Value(i);
      switch(what)
      {
         case  MC_ADD:   dYerg = dY + value;         break;
         case  MC_SUB:   dYerg = dY - value;         break;
         case  MC_MUL:   dYerg = dY * value;         break;
         case  MC_DIV:   dYerg = dY / value;         break;
         case  MC_POW:
            if ((dY < 0) && (fmod(value, 1) != 0)) return false;
            else         dYerg = pow(dY    , value); break;
         case  MC_LOG:   dYerg = log(dY)   * value;  break;
         case  MC_LOG10: dYerg = log10(dY) * value;  break;
      }
      pmc->SetY_Value(i, dYerg);
   }

   return true;
}

/******************************************************************************
* Name       : OperateCurve                                                   *
* Definition : bool OperateCurve(int, CMathCurve *, CMathCurve *);            *
* Zweck      : Mathematische Grundoperationen mit Kurvenobjekten.             *
* Aufruf     : OperateCurve(what, pci, pco);                                  *
* Parameter  :                                                                *
* what   (E) : durchzuführende Operation                        (int)         *
*              (MC_ADD, MC_SUB, MC_MUL, MC_DIV, MC_POW)                       *
* pci    (E) : Zeiger auf ein neues Kurvenobjekt.               (CMathCurve*) *
* pco    (EA): Zeiger auf ein neues Kurvenobjekt. Wird ein Kurvenobjekt mit   *
*              gültigen Werten übergeben, so werden dessen Kurvenwerte ge-    *
*              löscht und neue Kurvenwerte erzeugt.             (CMathCurve*) *
* Funktionswert : (true, false)                                 (bool)        *
******************************************************************************/
bool CMathCurve::OperateCurve(int what, CMathCurve *pci, CMathCurve *pco)
{
   int         i, i1=0, i2=0, nPStep, nPCount=0, nPProg=0, num = 0;
   double      step, step1, step2;
   double      xstart, xend, y1, y2, dYerg;

   if (!m_pArray)          return false;
   if (!pci)               return false;
   if (!pco)               return false;
   if (!IsSorted())        return false;
   if (!pci->IsSorted())   return false;

   if (GetXMin() > pci->GetXMin()) xstart = GetXMin();
   else                            xstart = pci->GetXMin();
   if (GetXMax() < pci->GetXMax()) xend   = GetXMax();
   else                            xend   = pci->GetXMax();

   if (xstart > xend)                                 return false;

   step1 = GetMinXStep();
   if (step1 <= 0.0)                                  return false;
   step2 = pci->GetMinXStep();
   if (step2 <= 0.0)                                  return false;

   if   (step1 < step2) step = step1;
   else                 step = step2;

   num  = 1 + (int) ((xend - xstart) / step);

   if (!pco->SetSize(num))                            return false;

   nPStep = num >> 7;                                          // Fortschrittsanzeige num / 128
   if (!nPStep) nPStep++;

   CFloatPrecision _fp;

   for (i=0; i<num; i++)
   {
      if (m_hProgressWnd)
      {
         if (!::IsWindowVisible(m_hProgressWnd))
            return false;
         if (++nPCount>=nPStep)
         {
            SendMessage(m_hProgressWnd, PBM_SETPOS, ++nPProg, 0L);
            nPCount=0;
         }
      }
      pco->SetX_Value(i, xstart);
      y1 = Get_Y_Value(xstart, &i1);
      y2 = pci->Get_Y_Value(xstart, &i2);
      switch(what)
      {
         case  MC_ADD: dYerg = y1 + y2; break;
         case  MC_SUB: dYerg = y1 - y2; break;
         case  MC_MUL: dYerg = y1 * y2; break;
         case  MC_DIV:
            if (y2 == 0)                        return false;  // nicht durch 0 teilen !
            else dYerg = y1 / y2;       break;
         case  MC_POW:
            if ((y1 < 0) && (fmod(y2, 1) != 0)) return false;  // Ergebnis darf nicht Complex werden
            else dYerg = pow(y1, y2);   break;
         default :                              return false;
      }
      pco->SetY_Value(i, dYerg);
      xstart += step;
   }

   return true;
}


/******************************************************************************
* Name       : Get_Y_Value, Get_Y_Value                                       *
* Definition : Get_X_Value(double), Get_Y_Value(double,int*startindex=NULL);  *
* Zweck      : Liefert den X- bzw. Y-Wert der Kurve zu dem übergebenen X- bzw.*
*              Y-Wert. Wenn nötig bzw. möglich wird der exakte Wert           *
*              Interpoliert bzw. Extrapoliert.                                *
* Aufruf     : Get_X_Value(y); Get_Y_Value(x, startindex);                    *
* Parameter  :                                                                *
* x      (E) : übergebener X-Wert der Kurvenfunktion (*1)                     *
* y      (E) : übergebener Y-Wert der Kurvenfunktion (*2)                     *
*[startindex]: Anfangssuchindex für den X-Wert der Kurve mit der geringsten   *
*        (EA)  Abweichung zu dem übergebenen X-Wert. Der Index des gefundenen *
*              X-Wertes wird wieder zurückgegeben. Wird diese Variable nicht  *
*              übergeben so wird das Kurvenfeld stets vom Index 0 an          *
*              durchsucht.                                                    *
* Funktionswert : gesuchter X- bzw. Y-Wert                        (double)    *
******************************************************************************/
double CMathCurve::Get_X_Value(double y)
{
   double dActDist, dMinDist = fabs(GetY_Value(0) - y);
   int i, nBest = 0;
   for (i=0; i<m_nNum-1; i++)                               // Alle Werte müssen überprüft werden
   {
      dActDist = fabs(GetY_Value(i) - y);
      if (dActDist < dMinDist)
      {
         dMinDist = dActDist;
         nBest    = i;
      }
   }
   return GetX_Value(nBest);                                   // Keine Interpolation
}

double CMathCurve::Get_Y_Value(double x, int *pnFound)
{
   int i(0);
   if (!m_pArray) return 0.0;

   if (IsSorted())                                             // Sorierte X-Werte
   {
      switch (m_Parameter.format)
      {
         case CF_DOUBLE: case CF_FLOAT: case CF_SHORT:         // Binäre Suche
         {
            int    iMid, iLeft = 0, iRight = m_nNum-1;
            if      (x > GetX_Value(iRight)) i = iRight-1;
            else if (x < GetX_Value(iLeft )) i = iLeft;
            else
            {
               do
               {
                  iMid = (iLeft + iRight) >> 1;
                  if (x > GetX_Value(iMid)) iLeft  = iMid;
                  else                      iRight = iMid;
               } while((iRight - iLeft)>1);
               i = iLeft;
            }
         } break;
         case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
         {                                                     // Position berechnen
            switch(GetXDivision())
            {
               case CCURVE_POLAR: case CCURVE_LINEAR:          // Winkelabstände sind auch linear
                  i = (int) ((x - m_dOrg) / m_dStep);
                  break;
               case CCURVE_LOGARITHMIC:                        // Schrittweite an Dekaden orientiert
               case CCURVE_FREQUENCY:                          // Schrittweite an Oktaven orientiert
                  i = (int) (log(x / m_dOrg) / log(m_dStep));
                  break; 
               default: break;
            } break;
         } break;
         case CF_ARBITRARY:
         case CF_ARBITRARY_CONTR:
         if (m_pGetCrvParamFkt)
         {
            SCurve sc = {x, 0};
            CF_GET_CURVE_PARAMS gcp = {CF_GET_Y_VALUE, 0, this, &sc, m_pGetCrvParam};
            if (pnFound) gcp.nIndex = *pnFound;
            i = m_pGetCrvParamFkt(&gcp);
         } break;

      }
      if (i <  0       ) i = 0;                                // Extrapolation bei zu kleinen x-Werten
      if (i >= m_nNum-1) i = m_nNum - 2;                       // Extrapolation bei zu großen x-Werten
                                                               // bzw. Interpolation
      if (pnFound) *pnFound = i;                               // aktuellen Suchindex zurückgeben
      double dY_i = GetY_Value(i);
      double dX_i = GetX_Value(i);
      if (dX_i == x) return dY_i;
      return dY_i + (GetY_Value(i+1) - dY_i) * (x - dX_i) / (GetX_Value(i+1) - dX_i);
   }
   else                                                        // Nicht sortiert
   {
      double dActDist, dMinDist = fabs(GetX_Value(0) - x);
      int nBest = 0;
      for (i=0; i<m_nNum-1; i++)                               // Alle Werte müssen überprüft werden
      {
         dActDist = fabs(GetX_Value(i) - x);
         if (dActDist < dMinDist)
         {
            dMinDist = dActDist;
            nBest    = i;
         }
      }
      if (pnFound) *pnFound = nBest;                           // aktuellen Suchindex zurückgeben
      return GetY_Value(nBest);                                // Keine Interpolation
   }
}

SCurve CMathCurve::Get_Locus_Value(double x, int *pnIndex)
{
   SCurve sc = {0,0};
   int i(0);
   double dTemp(0);
   if (!m_pArray) return sc;

   switch(GetLocus())
   {
      case CCURVE_POLAR:
         return sc;
      case CCURVE_LINEAR:                                      // linear
         dTemp = ((x - m_dOrg) / m_dStep);
         break;
      case CCURVE_LOGARITHMIC:                                 // Schrittweite an Dekaden orientiert
      case CCURVE_FREQUENCY:                                   // Schrittweite an Oktaven orientiert
         dTemp = (log(x / m_dOrg) / log(m_dStep));
         break; 
   }

   i = (int)dTemp;

   if (i <  0       ) i = 0;                                   // Extrapolation bei zu kleinen x-Werten
   if (i >= m_nNum-1) i = m_nNum - 2;                          // Extrapolation bei zu großen x-Werten

   dTemp -= i;
                                                               
   if (pnIndex) *pnIndex = i;                                  // aktuellen Suchindex zurückgeben
   if (fabs(dTemp) > 1e-13)                                    // Inter-/Extrapolation
   {
      Complex cVi, cVip1;
      double  dLi, dLip1;
      *((SCurve*)&cVi  ) = GetCurveValue(i);
      *((SCurve*)&cVip1) = GetCurveValue(i+1);
      dLi   = GetLocusValue(i);
      dLip1 = GetLocusValue(i+1);

      cVi += (cVip1 - cVi) * (x - dLi) / (dLip1 - dLi);
      return *((SCurve*)&cVi);
   }
   else return GetCurveValue(i);
}

/******************************************************************************
* Name       : GetMinXStep                                                    *
* Definition : double GetMinXStep()                                           *
* Zweck      : Findet die kleinste Schrittweite in X-Richtung.                *
* Aufruf     : GetMinXStep()                                                  *
* Parameter  : keine                                                          *
* Funktionswert : kleinste X-Schrittweite im Kurvenfeld           (double)    *
******************************************************************************/
double CMathCurve::GetMinXStep()
{
   double step(0);
   if (!m_pArray) return 0.0;
   switch (m_Parameter.format)
   {
      case CF_DOUBLE: case CF_FLOAT: case CF_SHORT:
      {
         int    i;
         double temp;
         step = GetX_Value(1) - GetX_Value(0);
         for (i=2; i<m_nNum; i++)
         {
            temp = GetX_Value(i) - GetX_Value(i-1);
            if (temp < step) step = temp;
         }
      } break;
      case CF_SINGLE_DOUBLE: case CF_SINGLE_FLOAT: case CF_SINGLE_SHORT:
         step = m_dStep;
         break;
      case CF_ARBITRARY:
      case CF_ARBITRARY_CONTR:
      if (m_pGetCrvParamFkt)
      {
         SCurve sc;
         CF_GET_CURVE_PARAMS gcp = {CF_GET_X_MIN_STEP, 0, this, &sc, m_pGetCrvParam};
         VERIFY(m_pGetCrvParamFkt(&gcp)!=0);
         step = sc.x;
      } break;

      default: ASSERT(false); break;
   }
   return step;
}

/******************************************************************************
* Name       : Average                                                        *
* Definition : double Average(int nmode, double x1=0, double x2=0);           *
* Zweck      : Bildet den Mittelwert der Y-Werte im Interval x1, x2.          *
* Aufruf     : Average(nmode[, x1, x2]);                                      *
* Parameter  :                                                                *
* nmode  (E) : Modus : 0 = arithmetischer, 1 = quadratischer Mittelwert (int) *
*[x1, x2](E) : untere und obere Grenze [optional]                    (double) *
*              ohne Angabe : gesamter Bereich der Kurve                       *
* Funktionswert : Mittelwert im Interval x1, x2                      (double) *
******************************************************************************/
double CMathCurve::Average(int nmode, double x1, double x2)
{
   int      i, count=0, num = m_nNum;
   double   y, sum;

   if (!m_pArray)    return 0.0;
   if (!IsSorted())  return 0.0;

   if ((x1 == 0) && (x2 == 0))
   {
       x1 = GetX_Value(0);
       x2 = GetX_Value(num-1);
   }
   if (x1 > x2) return 0;

   CFloatPrecision _fp;

   for (i=0, count=0, sum=0.0; i<num; i++)
   {
      if (x1 <= GetX_Value(i))
      {
         if (GetX_Value(i) <= x2)
         {
            y = GetY_Value(i);
            if (nmode) y *= y;
            sum += y;
            count++;
         }
         else break;
      }
   }

   double dErg = (nmode) ? sqrt(sum) / count : sum / count;

   if (!count) return 0.0;
   else        return dErg;
}

/******************************************************************************
* Name       : Integrate                                                      *
* Definition : double Integrate(int nmode, double x1=0, double x2=0);         *
* Zweck      : Berechnet die Fläche unter der Funktionskurve im               *
*              Interval x1, x2 nach dem Trapezverfahren.                      *
* Aufruf     : Integrate(nmode[, x1, x2]);                                    *
* Parameter  :                                                                *
* nmode  (E) : Modus : 0 = nur Summation, 1 = Betragsbildung                  *
*[x1, x2](E) : untere und obere Grenze [optional]                    (double) *
*              ohne Angabe : gesamter Bereich der Kurve                       *
* Funktionswert : Berechnete Fläche                                  (double) *
******************************************************************************/
double CMathCurve::Integrate(int nmode, double x1, double x2)
{
   int      i;
   double   sum, y, dx, x_i, x_im1, y_i, y_im1;

   if (!m_pArray)   return 0.0;
   if (!IsSorted()) return 0.0;

   if ((x1 == 0) && (x2 == 0))
   {
      x1 = GetX_Value(0);
      x2 = GetX_Value(m_nNum-1);
   }
   if (x1 > x2) return 0;

   CFloatPrecision _fp;

   y  = Get_Y_Value(x1, &i);
   y  = (GetY_Value(++i) + y) * 0.5;
   dx =  GetX_Value(i) - x1;
   if (nmode) sum = fabs(y * dx);
   else       sum = y * dx;

   x_im1 = GetX_Value(i);
   y_im1 = GetY_Value(i);
   x_i   = GetX_Value(++i);
   y_i   = GetY_Value(  i);
   while (x_i < x2)
   {
      y   = (y_i + y_im1) * 0.5;
      dx  =  x_i - x_im1;
      if (nmode) sum += fabs(y * dx);
      else       sum += y * dx;
      x_im1 = x_i;
      y_im1 = y_i;
      x_i   = GetX_Value(++i);
      y_i   = GetY_Value(  i);
   }
   i-=2;
   y  = Get_Y_Value(x2, &i);

   y   = (y  + GetY_Value(i)) * 0.5;
   dx  =  x2 - GetX_Value(i);

   if (nmode) sum += fabs(y * dx);
   else       sum += y * dx;

   return sum;
}

/******************************************************************************
* Name       : Slope                                                          *
* Definition : double Slope(double x, double &y);                             *
* Zweck      : Berechnen der Steigung der Kurve an der Stelle x               *
* Aufruf     : Slope(double x, double &y);                                    *
* Parameter  :                                                                *
* x      (E) : X-Wert für die zu ermittelnde Steigung.             (double)   *
* y      (A) : ermittelter Y-Wert an der Stelle x.                 (double&)  *
* Funktionswert : Berechnete Steigung                              (double)   *
******************************************************************************/
double CMathCurve::Slope(double x, double &y, int *startindex)
{
   double   dx, dy;
   CFloatPrecision _fp;

   if (!m_pArray) return 0.0;
   y = Get_Y_Value(x, startindex);
   if (*startindex == 0)        (*startindex)++;
   dx = GetX_Value(*startindex) - GetX_Value(*startindex-1);
   if (dx == 0) return 0;
   dy = GetY_Value(*startindex) - GetY_Value(*startindex-1);
   return dy / dx;
}

/******************************************************************************
* Name       : FindZero                                                          *
* Definition : double FindZero(double dStart);                             *
* Zweck      : Berechnen der Steigung der Kurve an der Stelle x               *
* Aufruf     : FindZero(double dStart);                                    *
* Parameter  :                                                                *
* dStart (E) : Startwert für die Suche des Nullpunktes             (double)   *
* Funktionswert : Berechnete Steigung                              (double)   *
******************************************************************************/
bool CMathCurve::FindZero(double& dStart)
{
   double dY(0), dSlope(0), dY2(0);
   int n = 0, nDir = -1, n2;
   bool bFound = false;
   CFloatPrecision _fp;
  
   dSlope = Slope(dStart, dY, &n);
   if (fabs(dY) < MC_EPS)
   {
      return true;
   }
   if (dSlope > 0)
   {
      if (dY < 0)
      {
         nDir = 1;
      }
   }
   else
   {
      if (dY > 0)
      {
         nDir = 1;
      }
   }
   n2 = n;
   if (nDir > 0)
   {
      for (n2++; n2<m_nNum; n2++, n++)
      {
         dY2 = GetY_Value(n2);
         if (   (dY <= 0 && dY2 >= 0)
             || (dY >= 0 && dY2 <= 0))
         {
            bFound = true;
            break;
         }
         dY = dY2;
      }
   }
   else
   {
      for (n2--; n2>=0; n2--, n--)
      {
         dY2 = GetY_Value(n2);
         if (   (dY <= 0 && dY2 >= 0)
             || (dY >= 0 && dY2 <= 0))
         {
            bFound = true;
            break;
         }
         dY = dY2;
      }
   }

   if (bFound)
   {
      double ddX = (GetX_Value(n2) - GetX_Value(n)) * (double)nDir;
      double ddY = (dY2 - dY) * (double)nDir;
      dStart = GetX_Value(n) - ddX*dY/ddY;
   }
   return bFound;
}

/******************************************************************************
* Name       : Spline                                                         *
* Definition : double Spline(int,double x1=0,double x2=0,CMathCurve *pmc=NULL)*
* Zweck      : Interpolation von zusätzlichen Kurvenwerten durch ein          *
*              kubisches Splinepolynom.                                       *
* Aufruf     : Spline(nsteps[, x1, x2][, pmc]);                               *
* Parameter  :                                                                *
* nsteps (E) : Anzahl zusätzlicher Zwischenschritte                  (int)    *
*[x1, x2](E) : untere und obere Grenze [optional]                    (double) *
*              ohne Angabe : gesamter Bereich der Kurve                       *
* pmc    (EA): Zeiger auf ein neues Kurvenobjekt.               (CMathCurve*) *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
bool CMathCurve::Spline(int nsteps, CMathCurve *pmc, double x1, double x2)
{
   int      i1, i2, num, nPStep, nPCount=0, nPProg=0, j;
   double   xstep, weight, dXvalue, dInvSteps;
   bool     bLog = false;

   if (!IsWindow(m_hProgressWnd)) m_hProgressWnd = NULL;

   if (!m_pArray)    return false;
   if (!pmc)         return false;
                                                               // X-Axenteilung logarithmisch
   bLog = ((pmc->GetXDivision() == CCURVE_LOGARITHMIC) || (pmc->GetXDivision() == CCURVE_FREQUENCY)) ? true : false;
   if ((x1 == 0) && (x2 == 0))
   {
      i1 = 1;
      i2 = m_nNum-2;
   }
   else if (GetLocus())
   {
      i1 = 0;
      Get_Locus_Value(x1, &i1);
      if (i1 <= 0) i1++;
      Get_Locus_Value(x2, &i2);
      if (i2 >= m_nNum-1) i2--;
   }
   else
   {
      i1 = 0;
      Get_Y_Value(x1, &i1);
      if (i1 <= 0) i1++;
      Get_Y_Value(x2, &i2);
      if (i2 >= m_nNum-1) i2--;
   }

   num = i2 - i1;
   if (num < 0)              return false;

   nPStep = num >> 7;                                          // Fortschrittsanzeige num / 128
   if (!nPStep) nPStep++;

   num = (nsteps + 1) * (num + 2) + 1;
   if (!pmc->SetSize(num)) return false;
   if (GetLocus())
   {
      CFloatPrecision _fp;
      SCInterpolynom  ip1, ip2;
      Complex         sc;

      if (!MakeInterpolynom(i1-1, i1, i1+1, &ip1)) return false;
      j = 0;

      dInvSteps = 1.0 / (nsteps+1);
      dXvalue = GetLocusValue(i1-1);
      pmc->SetLocusValue(j  , dXvalue);                           // Original Werte
      pmc->SetCurveValue(j++, GetCurveValue(i1-1));
      if (bLog) xstep = pow((double)10, log10(GetLocusValue(i1) / dXvalue) * dInvSteps);
      else      xstep =              (GetLocusValue(i1) - dXvalue) * dInvSteps;
       
      for (num=nsteps+1; j<num; j++)                              // Interpolierte Werte
      {
         if (bLog) dXvalue *= xstep;
         else      dXvalue += xstep;
         sc = PolynomValue(dXvalue, &ip1);
         pmc->SetCurveValue(j, *((SCurve*)&sc));
      }

      for (i1++; i1<=i2; i1++)
      {
         if (m_hProgressWnd)
         {
            if (!::IsWindowVisible(m_hProgressWnd))
               return false;
            if (++nPCount>=nPStep)
            {
               SendMessage(m_hProgressWnd, PBM_SETPOS, ++nPProg, 0L);
               nPCount=0;
            }
         }
         if (!MakeInterpolynom(i1-1, i1, i1+1, &ip2)) return false;

         dXvalue = GetLocusValue(i1-1);
         pmc->SetCurveValue(j++, GetCurveValue(i1-1));

         if (bLog) xstep = pow((double)10, log10(GetLocusValue(i1) / dXvalue) * dInvSteps);
         else      xstep  = (GetLocusValue(i1) - dXvalue) * dInvSteps;
         weight = dInvSteps;

         for (num+=nsteps+1; j<num; j++)                       // Interpolierte Werte
         {
            if (bLog) dXvalue *= xstep;
            else      dXvalue += xstep;
            sc = (1.0 - weight) * PolynomValue(dXvalue, &ip1) +
                        weight  * PolynomValue(dXvalue, &ip2);
            pmc->SetCurveValue(j, *((SCurve*)&sc));
            weight += dInvSteps;
         }
         ip1 = ip2;
      }

      dXvalue = GetLocusValue(i1-1);
      pmc->SetCurveValue(j++, GetCurveValue(i1-1));
      if (bLog) xstep = pow((double)10, log10(GetLocusValue(i1) / dXvalue) * dInvSteps);
      else      xstep =              (GetLocusValue(i1) - dXvalue) * dInvSteps;
    
      for (num+=nsteps+1; j<num; j++)                          // Interpolierte Werte
      {
         if (bLog) dXvalue *= xstep;
         else      dXvalue += xstep;
         sc = PolynomValue(dXvalue, &ip1);
         pmc->SetCurveValue(j, *((SCurve*)&sc));
      }
      pmc->SetLocusValue(j, GetLocusValue(i1));                // Original Wert
      pmc->SetCurveValue(j, GetCurveValue(i1));
   }
   else if (IsSorted())
   {
      CFloatPrecision _fp;
      SInterpolynom ip1, ip2;

      if (!MakeInterpolynom(i1-1, i1, i1+1, &ip1)) return false;
      j = 0;

      dInvSteps = 1.0 / (nsteps+1);
      dXvalue = GetX_Value(i1-1);
      pmc->SetX_Value(j  , dXvalue);                              // Original Werte
      pmc->SetY_Value(j++, GetY_Value(i1-1));
      if (bLog) xstep = pow((double)10, log10(GetX_Value(i1) / dXvalue) * dInvSteps);
      else      xstep =              (GetX_Value(i1) - dXvalue) * dInvSteps;
       
      for (num=nsteps+1; j<num; j++)                              // Interpolierte Werte
      {
         if (bLog) dXvalue *= xstep;
         else      dXvalue += xstep;
         pmc->SetX_Value(j, dXvalue);
         pmc->SetY_Value(j, PolynomValue(pmc->GetX_Value(j), &ip1));
      }

      for (i1++; i1<=i2; i1++)
      {
         if (m_hProgressWnd)
         {
            if (!::IsWindowVisible(m_hProgressWnd))
               return false;
            if (++nPCount>=nPStep)
            {
               SendMessage(m_hProgressWnd, PBM_SETPOS, ++nPProg, 0L);
               nPCount=0;
            }
         }
         if (!MakeInterpolynom(i1-1, i1, i1+1, &ip2)) return false;

         dXvalue = GetX_Value(i1-1);
         pmc->SetX_Value(j,   dXvalue);                           // Original Wert
         pmc->SetY_Value(j++, GetY_Value(i1-1));

         if (bLog) xstep = pow((double)10, log10(GetX_Value(i1) / dXvalue) * dInvSteps);
         else      xstep  = (GetX_Value(i1) - dXvalue) * dInvSteps;
         weight = dInvSteps;

         for (num+=nsteps+1; j<num; j++)                          // Interpolierte Werte
         {
            if (bLog) dXvalue *= xstep;
            else      dXvalue += xstep;
            pmc->SetX_Value(j, dXvalue);
            pmc->SetY_Value(j, (1.0 - weight) * PolynomValue(pmc->GetX_Value(j), &ip1) +
                                      weight  * PolynomValue(pmc->GetX_Value(j), &ip2));
            weight += dInvSteps;
         }
         ip1 = ip2;
      }

      dXvalue = GetX_Value(i1-1);
      pmc->SetX_Value(j,   dXvalue);                              // Original Wert
      pmc->SetY_Value(j++, GetY_Value(i1-1));
      if (bLog) xstep = pow((double)10, log10(GetX_Value(i1) / dXvalue) * dInvSteps);
      else      xstep =              (GetX_Value(i1) - dXvalue) * dInvSteps;
    
      for (num+=nsteps+1; j<num; j++)                             // Interpolierte Werte
      {
         if (bLog) dXvalue *= xstep;
         else      dXvalue += xstep;
         pmc->SetX_Value(j, dXvalue);
         pmc->SetY_Value(j, PolynomValue(pmc->GetX_Value(j), &ip1));
      }
      pmc->SetX_Value(j, GetX_Value(i1));                         // Original Wert
      pmc->SetY_Value(j, GetY_Value(i1));
   }
   return true;
}

/******************************************************************************
* Name       : MakeInterpolynom                                               *
* Definition : bool MakeInterpolynom(int, int, int, SInterpolynom *);         *
* Zweck      : Berechnen eines Interpolationspolynoms aus drei gegebenen      *
*              Kurvenwerten. Die X-Werte der Kurve müssen stetig steigen.     *
* Aufruf     : MakeInterpolynom(ni_prev, ni, ni_next, pip)                    *
* Parameter  :                                                                *
* ni_prev (E): Index für vorhergehenden Kurvenwert                   (int)    *
* ni      (E): Index für mittleren Kurvenwert                        (int)    *
* ni_next (E): Index für folgenden Kurvenwert                        (int)    *
* pip     (E): Zeiger auf Struktur mit den                (SInterpolynom*)    *
*              Polynomkoeffizienten.                                          *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
bool CMathCurve::MakeInterpolynom(int ni_prev, int ni, int ni_next, SInterpolynom *pip)
{
   if (ni_prev < 0)           return false;
   if (ni_prev >= ni)         return false;
   if (ni >= ni_next)         return false;
   if (ni_next >= m_nNum)     return false;

   double dXprev = GetX_Value(ni_prev);
   double dX     = GetX_Value(ni);
   double dXnext = GetX_Value(ni_next);
   double dYprev = GetY_Value(ni_prev);
   double dY     = GetY_Value(ni);
   double dYnext = GetY_Value(ni_next);

   pip->c0 =  dYprev;                       // c0 = p[i-n].y
   pip->c1 = (dY - dYprev) /                // c1 = (p[i].y - p[i-n].y) / (p[i].x - p[i-n].x)
             (dX - dXprev);           

   pip->c2 = (dYnext - pip->c0 - pip->c1 *  // c2 = (p[i+m].y - c0 - c1 * (p[i+m].x - p[i-n].x)) /
             (dXnext - dXprev)) /           //      ((p[i+m].x - p[i-n].x) * (p[i+m].x - p[i].x))
            ((dXnext - dXprev) *
             (dXnext - dX));

   pip->s0 =  dXprev;                       // s0 = p[i-n].x
   pip->s1 =  dX;                           // s1 = p[i].x

   return  true;
}

/******************************************************************************
* Name       : MakeInterpolynom                                               *
* Definition : bool MakeInterpolynom(int, int, int, SInterpolynom *);         *
* Zweck      : Berechnen eines Interpolationspolynoms aus drei gegebenen      *
*              Kurvenwerten. Die X-Werte der Kurve müssen stetig steigen.     *
* Aufruf     : MakeInterpolynom(ni_prev, ni, ni_next, pip)                    *
* Parameter  :                                                                *
* ni_prev (E): Index für vorhergehenden Kurvenwert                   (int)    *
* ni      (E): Index für mittleren Kurvenwert                        (int)    *
* ni_next (E): Index für folgenden Kurvenwert                        (int)    *
* pip     (E): Zeiger auf Struktur mit den                (SCInterpolynom*)   *
*              Polynomkoeffizienten.                                          *
* Funktionswert : (true, false)                                      (bool)   *
******************************************************************************/
bool CMathCurve::MakeInterpolynom(int ni_prev, int ni, int ni_next, SCInterpolynom *pip)
{
   if (ni_prev < 0)           return false;
   if (ni_prev >= ni)         return false;
   if (ni >= ni_next)         return false;
   if (ni_next >= m_nNum)     return false;

   double dXprev  = GetLocusValue(ni_prev);
   double dX      = GetLocusValue(ni);
   double dXnext  = GetLocusValue(ni_next);
   SCurve scVal   = GetCurveValue(ni_prev);
   Complex dYprev = *((Complex*)&scVal);
   scVal          = GetCurveValue(ni);
   Complex dY     = *((Complex*)&scVal);
   scVal          = GetCurveValue(ni_next);
   Complex dYnext = *((Complex*)&scVal);

   pip->c0 =  dYprev;                       // c0 = p[i-n].y
   pip->c1 = (dY - dYprev) /                // c1 = (p[i].y - p[i-n].y) / (p[i].x - p[i-n].x)
             (dX - dXprev);           

   pip->c2 = (dYnext - pip->c0 - pip->c1 *  // c2 = (p[i+m].y - c0 - c1 * (p[i+m].x - p[i-n].x)) /
             (dXnext - dXprev)) /           //      ((p[i+m].x - p[i-n].x) * (p[i+m].x - p[i].x))
            ((dXnext - dXprev) *
             (dXnext - dX));

   pip->s0 =  dXprev;                       // s0 = p[i-n].x
   pip->s1 =  dX;                           // s1 = p[i].x

   return  true;
}

/******************************************************************************
* Name       : PolynomValue                                                   *
* Definition : double PolynomValue(double, SInterpolynom *)                   *
* Zweck      : Berechnen des Polynomwertes an der Stelle X.                   *
* Aufruf     : PolynomValue(double x, SInterpolynom *pip)                     *
* Parameter  :                                                                *
* x      (E) : X-Wert.                                             (double)   *
* pip     (E): Zeiger auf Struktur mit den                 (SInterpolynom*)   *
*              Polynomkoeffizienten.                                          *
* Funktionswert : Berechneter Y-Wert des Polynoms                  (double)   *
******************************************************************************/
double CMathCurve::PolynomValue(double x, SInterpolynom *pip)
{
   double fa1,fa2;

   fa1 =       (x - pip->s0);
   fa2 = fa1 * (x - pip->s1);
   return pip->c0 + pip->c1*fa1 + pip->c2*fa2;
}

Complex CMathCurve::PolynomValue(double x, SCInterpolynom *pip)
{
   Complex fa1,fa2;

   fa1 =       (x - pip->s0);
   fa2 = fa1 * (x - pip->s1);
   return pip->c0 + pip->c1*fa1 + pip->c2*fa2;
}

int CMathCurve::SCurvexSortfunc(const void *v1, const void *v2)
{
   if (((SCurve*)v1)->x > ((SCurve*)v2)->x) return  1;
   if (((SCurve*)v1)->x < ((SCurve*)v2)->x) return -1;
   return 0;
}
int CMathCurve::SCurvexSortfuncF(const void *v1, const void *v2)
{
   if (((SCurveFloat*)v1)->x > ((SCurveFloat*)v2)->x) return  1;
   if (((SCurveFloat*)v1)->x < ((SCurveFloat*)v2)->x) return -1;
   return 0;
}
int CMathCurve::SCurvexSortfuncS(const void *v1, const void *v2)
{
   if (((SCurveShort*)v1)->x > ((SCurveShort*)v2)->x) return  1;
   if (((SCurveShort*)v1)->x < ((SCurveShort*)v2)->x) return -1;
   return 0;
}

void CMathCurve::DivideUnit(TCHAR * pszUnit)
{
   if (pszUnit && m_pszUnity)
   {
      TCHAR  szAbove[UNIT_STR_LEN] = "";
      TCHAR  szUnder[UNIT_STR_LEN] = "";
      SplitUnit(m_pszUnity, szAbove, szUnder);
      if (!DivideUnitPart(szAbove, pszUnit))
         MultiplyUnitPart(szUnder, pszUnit);
      if (_tcsclen(szUnder)>0)
      {
         strncat(szAbove, "/"    , UNIT_STR_LEN);
         strncat(szAbove, szUnder, UNIT_STR_LEN);
      }
      SetYUnit(szAbove);
   }
}
void CMathCurve::MultiplyUnit(TCHAR * pszUnit)
{
   if (pszUnit && m_pszUnity)
   {
      TCHAR  szAbove[UNIT_STR_LEN] = "";
      TCHAR  szUnder[UNIT_STR_LEN] = "";
      SplitUnit(m_pszUnity, szAbove, szUnder);
      if (!DivideUnitPart(szUnder, pszUnit))
         MultiplyUnitPart(szAbove, pszUnit);
      if (_tcsclen(szUnder)>0)
      {
         strncat(szAbove, "/"    , UNIT_STR_LEN);
         strncat(szAbove, szUnder, UNIT_STR_LEN);
      }
      SetYUnit(szAbove);
   }
}
bool CMathCurve::MultiplyUnitPart(TCHAR * pszPart, TCHAR * pszUnit)
{
   if ((pszPart != NULL) && (pszUnit != NULL))
   {
      TCHAR *ptr = _tcsstr(pszPart, pszUnit);
      if (ptr)
      {
         ptr = _tcsstr(pszPart, "^");
         if (ptr)
         {
            ptr++;
            int nExp = atoi(ptr) + 1;
            itoa(nExp, ptr, 10);
         }
         else
         {
            strncat(pszPart, "^", UNIT_STR_LEN);
            strncat(pszPart, "2", UNIT_STR_LEN);
         }
         return true;
      }
      else
      {
         if (_tcsclen(pszPart)>0) strncat(pszPart, "*", UNIT_STR_LEN);
         strncat(pszPart, pszUnit, UNIT_STR_LEN);
         return false;
      }
   }
   return false;
}

bool CMathCurve::DivideUnitPart(TCHAR * pszPart, TCHAR * pszUnit)
{
   if ((pszPart != NULL) && (pszUnit != NULL))
   {
      TCHAR *ptr = _tcsstr(pszPart, pszUnit);
      if (ptr)
      {
         int nOffset = ptr - pszPart;
         TCHAR *ptr1 = _tcsstr(pszPart, "^");
         if (ptr1)
         {
            ptr1++;
            int nExp = atoi(ptr1) - 1;
            if (nExp <= 1) ptr1[-1] = 0;
            else           itoa(nExp, ptr1, 10);
         }
         else if ((nOffset>1) && (ptr[-1] == '*'))
            ptr[-1] = 0;
         else 
            ptr[0]  = 0;
         return true;
      }
   }
   return false;
}

void CMathCurve::SplitUnit(TCHAR * pszUnit, TCHAR * pszAbove, TCHAR * pszUnder)
{
   TCHAR *ptr = _tcsstr(pszUnit, "/");
   if (ptr)
   {
      strncpy(pszUnder, &ptr[1], UNIT_STR_LEN);
      *ptr = 0;
   }
   strncpy(pszAbove, pszUnit, UNIT_STR_LEN);
}

Complex CMathCurve::GetComplexValue(int n)
{
   SCurve sc = GetCurveValue(n);
   return Complex(sc.x, sc.y);
}

bool CMathCurve::SetComplexValue(int n, Complex &c)
{
   return SetCurveValue(n, *((SCurve*)&c));
}

