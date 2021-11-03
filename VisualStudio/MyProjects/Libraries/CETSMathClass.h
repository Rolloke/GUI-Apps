/*******************************************************************************
                                 CEts3DGL
  Zweck:
  Header:            Bibliotheken:        DLL´s:
  Benutzung:
   Die ETSBIND.dll ist eine statisch gebundene Dll. Die ETSDIV.dll ist eine
   "load on call-Dll". D.h., sie wird erst geladen, wenn sie das erste mal be-
   nötigt wird.
  Copyright:
   (C) 1999 ELAC Technische Software GmbH
  VERSION 
   1.00 Stand 22.12.1999
                                                   programmed by Rolf Ehlers
*******************************************************************************/


#ifndef _CETSMATH_CLASS_H_
#define _CETSMATH_CLASS_H_

typedef void (*PROGRESSFUNC) (void *, int);

#include "calcmplx.h"

class CFilterBox
{
public:
   CFilterBox(int nSize);
   CFilterBox() { memset(this, 0, sizeof(*this)); }
   ~CFilterBox();
   bool    SetFrequency(int, double);
   double  GetFrequency(int);
   bool    SetCoefficient(int,Complex);
   Complex GetCoefficient(int);

   int          nHPOrder;        // Hochpaß-Filterordnung: 1..4, 0->kein HP
   int          nHPType;         // 0->Butterworth,
                                 // 1->'entdämpft' (d.h. Q variabel, nur für 2./3. Ordnung)
                                 // 2->Bessel
                                 // 3->Tschebyscheff mit 0.5 dB Welligkeit
                                 // 4->Linkwitz-Riley (nur f. 4. Ordnung)
   double       dHPQFaktor;      // gilt nur für nHPType=1 'entdämpft', sonst dHPQFaktor=0
   double       dHPFreqSelected; // ausgewählte Grenzfrequenz

   int          nTPOrder;        // Tiefpaß-Filterordnung: 1..4, 0->kein HP
   int          nTPType;         // 0->Butterworth,
                                 // 1->'entdämpft' (d.h. Q variabel, nur für 2./3. Ordnung)
                                 // 2->Bessel
                                 // 3->Tschebyscheff mit 0.5 dB Welligkeit
                                 // 4->Linkwitz-Riley (nur f. 4. Ordnung)
   double       dTPQFaktor;      // gilt nur für nTPType=1 'entdämpft', sonst dTPQFaktor=0
   double       dTPFreqSelected; // ausgewählte Grenzfrequenz

   bool         bCalcArray;
private:
   bool InRange(int nRange)
   {
      return ((nRange >= 0) && (nRange < nNoOfFrequencies)) ? true : false;
   }
   int          nNoOfFrequencies;
   double      *pdFilterFrq;     // Filterfrequenzen
   Complex     *pcFilterBox;     // komplexe Filterkoeffizienten, frequenzabhängig
};

#ifndef DONT_USE_ETSBIND
class CETSMathClass
{
   public:
      CETSMathClass();
      void  Destructor();

      // Statische Funktionen
	   static DWORD GetRandomValue();
      static void  RealFFT(int, int, double *, PROGRESSFUNC, void *);
      static void  CalculateFilter(CFilterBox*, int, double, double*, double*, PROGRESSFUNC, void*);
      static void  RealFFT(int, int, float*, PROGRESSFUNC, void*);
      static void  CalculateFilter(CFilterBox*, int, double, float*, float*, PROGRESSFUNC, void*);
      static void  MultiplyFilter(Complex, double &, double &);
      static Complex PinkNoiseFilter(double, double);

   private:
};
#else
// hiding these numbers is so silly olli!
// ordinal numbers
#define ETSMATHCLASS_GetRandomValue    2
#define ETSMATHCLASS_RealFFT_D         3
#define ETSMATHCLASS_CalculateFilter_D 4
#define ETSMATHCLASS_RealFFT_F         5
#define ETSMATHCLASS_CalculateFilter_F 6
#define ETSMATHCLASS_MultiplyFilter    7
#define ETSMATHCLASS_PinkNoiseFilter   8

// function definitions
typedef DWORD (__cdecl* PF_GETRANDOMVALUE)();
typedef void  (__cdecl* PF_REALFFT)(int,int,double*,PROGRESSFUNC,void*);
typedef void  (__cdecl* PF_CALCULATEFILTER)(CFilterBox*,int,double,double*,double*,PROGRESSFUNC,void*);
typedef void  (__cdecl* PF_REALFFT_FLOAT)(int,int,float*,PROGRESSFUNC,void*);
typedef void  (__cdecl* PF_CALCULATEFILTER_FLOAT)(CFilterBox*,int,double,float*,float*,PROGRESSFUNC,void*);
typedef void  (__cdecl* PF_MULTIPLYFILTEREXP)(double,double,double&,double&);
typedef void  (__cdecl* PF_PINKNOISEFILTEREXP)(double,double,double&,double&);

#endif

#endif      // _CETSMATH_CLASS_H_

