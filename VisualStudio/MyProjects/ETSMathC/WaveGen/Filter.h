#if !defined(_FILTER_H_INCLUDED_)
#define _FILTER_H_INCLUDED_

#include "calcmplx.h"
#define NO_OF_FREQUENCIES 118

struct FilterBox
{
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
   Complex      pcFilterBox[NO_OF_FREQUENCIES];     // komplexe Filterkoeffizienten, frequenzabhängig
};

#endif // _FILTER_H_INCLUDED_
