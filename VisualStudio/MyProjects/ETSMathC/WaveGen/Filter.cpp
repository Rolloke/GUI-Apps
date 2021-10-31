#include "stdafx.h"
#include "Filter.h"


/********************************************************************/
/*                                                                  */
/*           Berechnet die Filterkoeffizienten für ein              */
/*              (aktives) boxenspezifisches Filter                  */
/*                                                                  */
/********************************************************************/
void CalcFilterBox( FilterBox *sFilterStruct )
{
   Complex     cSn;                              // = j*2Pi*freq/2Pi*Grenzfreq
   Complex     cFilterKoeff1, cFilterKoeff2;

   double      da1[5][5], da2[5][5], db1[5][5], db2[5][5]; // Koeffizienten[Order][Type] der Filterpolynome
   int         i, j;

   
   for( i = 0; i < 5; i++ )                      // alle Koeff. auf Null setzen, max. Ordnung=4; max. Typ=4
   {                                             // i = Ordnung, j = Typ
      for( j = 0; j < 5; j++ )
      {
         da1[i][j] = 0.0;
         da2[i][j] = 0.0;
         db1[i][j] = 0.0;
         db2[i][j] = 0.0;
      }
   }
   da1[1][0] = da1[1][2] = da1[1][3] = 1.0;      // 1. Ordnung nur für Butterworth, Bessel u. Tschebyscheff

   da1[2][0] = 1.4142;                           // 2. Ordnung, Butterworth
   db1[2][0] = 1.0;
   da1[2][1] = 1.0;                              // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
   db1[2][1] = 1.0;
   da1[2][2] = 1.3617;                           // 2. Ordnung, Bessel
   db1[2][2] = 0.6180;
   da1[2][3] = 1.3614;                           // 2. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
   db1[2][3] = 1.3827;

   da1[3][0] = 1.0;                              // 3. Ordnung, Butterworth
   da2[3][0] = 1.0;
   db2[3][0] = 1.0;
   da1[3][1] = 1.0;                              // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
   da2[3][1] = 1.0;
   db2[3][1] = 1.0;
   da1[3][2] = 0.7560;                           // 3. Ordnung, Bessel
   da2[3][2] = 0.9996;
   db2[3][2] = 0.4772;
   da1[3][3] = 1.8636;                           // 3. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
   da2[3][3] = 0.6402;
   db2[3][3] = 1.1931;

   da1[4][0] = 1.8478;                           // 4. Ordnung, Butterworth
   db1[4][0] = 1.0;
   da2[4][0] = 0.7654;
   db2[4][0] = 1.0;
   da1[4][2] = 1.3397;                           // 4. Ordnung, Bessel
   db1[4][2] = 0.4889;
   da2[4][2] = 0.7743;
   db2[4][2] = 0.3890;
   da1[4][3] = 2.6282;                           // 4. Ordnung, Tschebyscheff mit 0.5 dB Welligkeit
   db1[4][3] = 3.4341;
   da2[4][3] = 0.3648;
   db2[4][3] = 1.1509;
   da1[4][4] = 1.4142;                           // 4. Ordnung, Linkwitz-Riley
   db1[4][4] = 1.0;
   da2[4][4] = 1.4142;
   db2[4][4] = 1.0;

   for( i = 0; i < CALCNOOFFREQ; i++ )
   {
      cFilterKoeff1 = Complex( 1.0, 0.0 );
      cFilterKoeff2 = Complex( 1.0, 0.0 );

      if( sFilterStruct->dHPFreqSelected > 0.0 )
         cSn = Complex( 0.0, g_Freq.m_dFrequency[i]/sFilterStruct->dHPFreqSelected );
      else
         cSn = Complex( 0.0, g_Freq.m_dFrequency[i] ); // HPFreq = 1 HZ

      if( (sFilterStruct->nHPType == 1) && (sFilterStruct->dHPQFaktor > 0.0) )
      {
         da1[2][1] = 1.0/sFilterStruct->dHPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         da2[3][1] = 1.0/sFilterStruct->dHPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
      }

      if( (sFilterStruct->nHPOrder > 0) && (sFilterStruct->nHPOrder < 5) &&
          (sFilterStruct->nHPType >= 0) && (sFilterStruct->nHPType  < 5) )
      {
         cFilterKoeff1  = 1.0 + da1[sFilterStruct->nHPOrder][sFilterStruct->nHPType]/cSn +
                                db1[sFilterStruct->nHPOrder][sFilterStruct->nHPType]/(cSn*cSn);
         cFilterKoeff1 *= 1.0 + da2[sFilterStruct->nHPOrder][sFilterStruct->nHPType]/cSn +
                                db2[sFilterStruct->nHPOrder][sFilterStruct->nHPType]/(cSn*cSn);
         cFilterKoeff1  = 1.0/cFilterKoeff1;
      }

      if( sFilterStruct->dTPFreqSelected > 0.0 )
         cSn = Complex( 0.0, g_Freq.m_dFrequency[i]/sFilterStruct->dTPFreqSelected );
      else
         cSn = Complex( 0.0, g_Freq.m_dFrequency[i]/1000000.0 ); // TPFreq = 1 MHZ

      if( (sFilterStruct->nTPType == 1) && (sFilterStruct->dTPQFaktor > 0.0) )
      {
         da1[2][1] = 1.0/sFilterStruct->dTPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         da2[3][1] = 1.0/sFilterStruct->dTPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
      }

      if( (sFilterStruct->nTPOrder > 0) && (sFilterStruct->nTPOrder < 5) &&
          (sFilterStruct->nTPType >= 0) && (sFilterStruct->nTPType  < 5) )
      {
         cFilterKoeff2  = 1.0 + da1[sFilterStruct->nTPOrder][sFilterStruct->nTPType]*cSn +
                                db1[sFilterStruct->nTPOrder][sFilterStruct->nTPType]*cSn*cSn;
         cFilterKoeff2 *= 1.0 + da2[sFilterStruct->nTPOrder][sFilterStruct->nTPType]*cSn +
                                db2[sFilterStruct->nTPOrder][sFilterStruct->nTPType]*cSn*cSn;
         cFilterKoeff2  = 1.0/cFilterKoeff2;
      }

      sFilterStruct->cFilterBox[i] = cFilterKoeff1*cFilterKoeff2;
   }
}
