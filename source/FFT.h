#ifndef FFT_H
#define FFT_H

#include <vector>
#include <complex>


double calcRectFilter(int, int);
double calcHammingFilter(int, int);
double calcHanningFilter(int, int);
double calcCosineFilter(int, int);
double calcGaussFilter(int, int);
double calcLanczosFilter(int, int);

void calcPhase(const std::complex<double>* aFourier, std::vector<double>& aDest);
void calcReal(const std::complex<double>* aFourier, std::vector<double>& aDest);
void calcImaginary(const std::complex<double>* aFourier, std::vector<double>& aDest);
void calcAmplitude(const std::complex<double>* aFourier, std::vector<double>& aDest);


template <class Type>
void calcFFT(int aRetransform, int aTau, Type *aFourier)
{
   int    md_2, md_4, nSigma, nSigma2, j_2, min_n, n_min_0, n_min_1;
   int    ind_1, ind_2, k, j, n, l, k_2, nTau2_2k, nTau2;

   Type dFaktor, dArg, dArg_m, dArg_md2, dYtemp;
   Type dEw_r, dEw_i, dEps_r, dEps_i, dUr, dUi, dWr, dWi;
   Type dTemp, dTemp1, dTemp2, dTemp3, dTemp4;

   nTau2    = 2<<(aTau-1);
   md_2     = nTau2>>1;
   md_4     = md_2>>1;
   dFaktor  = 1.0 / md_2;
   dArg_md2 = 2*M_PI * dFaktor;
   dArg_m   =   M_PI * dFaktor;

   min_n    = md_2;
   n_min_1  = 1;

   if (aRetransform)                      // Zusammenfassung der reellen Daten zur Durchführung
   {								      // einer FFT halber Länge fals mode=1
      dFaktor = 1;
      dYtemp = aFourier[1];
      aFourier[1] = aFourier[0] - aFourier[nTau2-1];
      aFourier[0]+= aFourier[nTau2-1];
      dEw_r   = cos(dArg_m);              // Real-,Imaginärteil der nTau2 ten Einheitswurzel
      dEw_i   = sin(dArg_m);
      dEps_r  = 1.0;					  // Real-,Imaginärteil von (nTau2 te Einheitwurzel)^k
      dEps_i  = 0.0;

      for (k=1;k<md_4;k++)
      {
           k_2      = k<<1;
           nTau2_2k = nTau2 - k_2;
           dTemp    = dEps_r;
           dEps_r   = dTemp*dEw_r - dEps_i*dEw_i;
           dEps_i   = dTemp*dEw_i + dEps_i*dEw_r;

           dTemp1   = .5* (dEps_r* (dYtemp - aFourier[nTau2_2k-1]) + dEps_i* (aFourier[k_2] + aFourier[nTau2_2k]));
           dTemp2   = .5* (dEps_i* (dYtemp - aFourier[nTau2_2k-1]) - dEps_r* (aFourier[k_2] + aFourier[nTau2_2k]));
           dTemp3   = .5* (dYtemp + aFourier[nTau2_2k-1]);
           dTemp4   = .5* (aFourier[k_2] - aFourier[nTau2_2k]);

           dYtemp               = aFourier[k_2+1];
           aFourier[k_2]        = dTemp3 - dTemp2;
           aFourier[k_2+1]      = dTemp1 - dTemp4;
           aFourier[nTau2_2k]   = dTemp2 + dTemp3;
           aFourier[nTau2_2k+1] = dTemp1 + dTemp4;
      }

      aFourier[md_2+1] = aFourier[md_2];
      aFourier[md_2]   = dYtemp;
   }

   for (j=0;j<md_2;j++)
   {
      k=j;							            // Umspeicherung mit Bit-Umkehrfunktion
      for(n=1,nSigma=0;n<aTau;n++)			// (gleichzeitige Normierung, falls mode = 0 )
      {
           nSigma = (nSigma<<1) + (k&1);
           k>>=1;
      }

      if (j<= nSigma)
      {
           j_2      = j<<1;
           nSigma2  = nSigma<<1;
           dUr      = aFourier[j_2];
           dUi      = aFourier[j_2+1];

           aFourier[j_2]       = dFaktor * aFourier[nSigma2];
           aFourier[j_2+1]     = dFaktor * aFourier[nSigma2+1];
           aFourier[nSigma2]   = dFaktor * dUr;
           aFourier[nSigma2+1] = dFaktor * dUi;
      }
   }

                                                 // Durchführung der FFT halber Länge
   min_n   = md_2;
   n_min_1 = 1;

   for (n = 1; n < aTau;n++)
   {
      min_n  >>= 1;
      n_min_0  = n_min_1<<1;
      dArg      = dArg_md2 * min_n;


      dWr = cos(dArg);
      dWi = sin(dArg) * ((aRetransform) ? 1.0:-1.0);

      dEps_r = 1;
      dEps_i = 0;
                                                  // Berechnung mit der Einheitswurzel w^l=1 mit (l=0)
                                                  // d.h. keine Multiplikation nötig
      for(j = 0;j <= md_2 - n_min_0;j += n_min_0)
      {
           ind_1 = j<<1;
           ind_2 = ind_1 + n_min_0;

           dUr   = aFourier[ind_2];
           dUi   = aFourier[ind_2+1];

           aFourier[ind_2]    = aFourier[ind_1]   - dUr;
           aFourier[ind_2+1]  = aFourier[ind_1+1] - dUi;
           aFourier[ind_1]   += dUr;
           aFourier[ind_1+1] += dUi;
      }

      for(l=1;l<n_min_1;l++)
      {
           dTemp  = dEps_r;                // berechne den neuen Multiplikator w^l = w^0 * w * w ....
           dEps_r = dTemp*dWr - dEps_i*dWi;
           dEps_i = dTemp*dWi + dEps_i*dWr;
                                                  // Berechnung mit der Einheitswurzel w^l
           for(j = 0;j <= md_2 - n_min_0;j += n_min_0)
           {
              ind_1 = (j+l)<<1;
              ind_2 = ind_1 + n_min_0;
              dUr   = aFourier[ind_2]*dEps_r - aFourier[ind_2+1]*dEps_i;
              dUi   = aFourier[ind_2]*dEps_i + aFourier[ind_2+1]*dEps_r;

              aFourier[ind_2]    = aFourier[ind_1]   - dUr;
              aFourier[ind_2+1]  = aFourier[ind_1+1] - dUi;
              aFourier[ind_1]   += dUr;
              aFourier[ind_1+1] += dUi;
           }
      }
      n_min_1 = n_min_0;
   }

   if (!aRetransform)
   {								               // Trennung der zusammengefaáten transformierten
      dYtemp            = aFourier[nTau2-1];// Daten falls mode = 0
      aFourier[nTau2-1] = .5* (aFourier[0] - aFourier[1]);
      aFourier[0]       = .5* (aFourier[0] + aFourier[1]);

      dEw_r   = cos(dArg_m);
      dEw_i   = -sin(dArg_m);
      dEps_r  = 1.0;
      dEps_i  = 0.0;

      for(k=1;k<md_4;k++)
      {
           dTemp  = dEps_r;
           dEps_r = dTemp*dEw_r - dEps_i*dEw_i;
           dEps_i = dTemp*dEw_i + dEps_i*dEw_r;
           ind_1  = k<<1;
           ind_2  = nTau2 - ind_1;

           dTemp1 = .5* (dEps_i* (aFourier[ind_1] - aFourier[ind_2]) + dEps_r* (aFourier[ind_1+1] + dYtemp));
           dTemp2 = .5* (dEps_r* (aFourier[ind_1] - aFourier[ind_2]) - dEps_i* (aFourier[ind_1+1] + dYtemp));
           dTemp3 = .5* (aFourier[ind_1] + aFourier[ind_2]);
           dTemp4 = .5* (aFourier[ind_1+1] - dYtemp);

           dYtemp            = aFourier[ind_2-1];
           aFourier[ind_1-1] = dTemp1 + dTemp3;
           aFourier[ind_1]   = dTemp2 - dTemp4;
           aFourier[ind_2-1] = dTemp3 - dTemp1;
           aFourier[ind_2]   = dTemp2 + dTemp4;
      }

      aFourier[md_2-1] = aFourier[md_2];
      aFourier[md_2]   = dYtemp;
   }
}

#endif // FFT_H
