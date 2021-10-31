// ETSMathC.cpp : Definiert den Einsprungpunkt für die DLL-Anwendung.
//

#include "stdafx.h"

#include "calcmplx.h"

#ifndef M_PI
  #define M_PI                3.1415926535898
#endif

class CFilterBox
{
public:
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
   int          nNoOfFrequencies;
   double      *pcFilterFrq;     // Filterfrequenzen
   Complex     *pcFilterBox;     // komplexe Filterkoeffizienten, frequenzabhängig
};

typedef void (*PROGRESSFUNC) (void *, int);

extern "C"
{
   void  Destructor(void*);
   DWORD GetRandomValue();
   void  RealFFT(int, int, double*, PROGRESSFUNC, void*);
   void  CalculateFilter(CFilterBox*, int, double, double*, double*, PROGRESSFUNC, void*);
   void  RealFFT_float(int, int, float*, PROGRESSFUNC, void*);
   void  CalculateFilter_float(CFilterBox*, int, double, float*, float*, PROGRESSFUNC, void*);
   void  MultiplyFilterExp(double, double, double&, double&);
   void  PinkNoiseFilterExp(double, double, double&, double&);
}

void MultiplyFilter(Complex, double &, double &);
Complex PinkNoiseFilter(double, double);

HINSTANCE g_hInstance = NULL;                        // globaler Instance-Handle

// Funktionen für den Zufallsgenerator
void  Init_PSBMG();
BYTE  GetNext_PSBMG();
void  Fill_Generator();
// Variablen für den Zufallsgenerator
DWORD g_GeneratorField[256];                     // Feld für den Additiven Generator (Wiederholänge ca. 2^256-1)
DWORD g_GeneratorIndex = 0;                      // Arbeitsindex für den Additiven Generator
DWORD g_PSBMG          = 0;                      // Arbeitsfeld für den PseudoStatistischer Bitmuster Generator (Wiederhollänge ca. 2^31-1)

// Funktionen
void  Destructor(void*)
{

}

void Init_PSBMG()
{
   MEMORYSTATUS ms;         // Struktur für Hauptspeicherinformationen
   LARGE_INTEGER qpc1;      // Struktur für vergangene Zeit nach Systemstart
   LARGE_INTEGER qpc2;      // Struktur für vergangene Zeit nach Systemstart
   LARGE_INTEGER qpc3;      // Struktur für vergangene Zeit nach Systemstart
   DWORD SectorsPerCluster; // Informationen über die Festplatte
   DWORD BytesPerSector;
   DWORD FreeClusters;
   DWORD Clusters;
   DWORD Time;              // Windowszeit
   DWORD disk;
   DWORD memory;

   do
   {
      QueryPerformanceCounter(&qpc1);     // Hochauflösenden Zähler besorgen

      Sleep(1);                           // Thread kurz beenden (es ist nicht genau bekannt wie lange das dauert ! "Systemabhängig")

      ZeroMemory(&ms,sizeof(ms));         // Information über aktuelle Hauptspeicherbelegung besorgen
      ms.dwLength = sizeof(ms);
      GlobalMemoryStatus(&ms); 

      Sleep(1);                           // Thread kurz beenden (es ist nicht genau bekannt wie lange das dauert ! "Systemabhängig")

      QueryPerformanceCounter(&qpc2);     // Hochauflösenden Zähler besorgen

      GetDiskFreeSpace("C:\\",            // Information über aktuelle Festplatte besorgen
                       &SectorsPerCluster,
                       &BytesPerSector,
                       &FreeClusters,
                       &Clusters);

      disk = SectorsPerCluster;
      disk^= BytesPerSector;
      disk^= FreeClusters;
      disk^= Clusters;

      memory = ms.dwTotalPhys;
      memory^= ~ms.dwAvailPhys;
      memory^= ms.dwTotalPageFile;
      memory^= ~ms.dwAvailPageFile;
      memory^= ms.dwTotalVirtual;
      memory^= ~ms.dwAvailVirtual;

      Sleep(1);                           // Thread kurz beenden (es ist nicht genau bekannt wie lange das dauert ! "Systemabhängig")

      Time   = GetCurrentTime();          // vergangene Zeit nach Windowsstart

      Sleep(1);                           // Thread kurz beenden (es ist nicht genau bekannt wie lange das dauert ! "Systemabhängig")

      QueryPerformanceCounter(&qpc3);     // Hochauflösenden Zähler besorgen

      g_PSBMG  = memory;                  // Daten über den Hauptspeicher
      g_PSBMG<<=1;                        // Verschieben
      g_PSBMG ^= qpc1.LowPart;            // Xor mit Hochauflösenden Zähler Get(1)
      g_PSBMG<<=1;                        // Verschieben
      g_PSBMG ^= disk;                    // Xor mit Daten über die Festplatte
      g_PSBMG<<=1;                        // Verschieben
      g_PSBMG ^= ~qpc3.LowPart;           // Xor mit Hochauflösenden Zähler Get(3)
      g_PSBMG<<=1;                        // Verschieben
      g_PSBMG ^= Time;                    // Xor mit Windowszeit
      g_PSBMG<<=1;                        // Verschieben
      g_PSBMG ^= qpc2.LowPart;            // Xor mit Hochauflösenden Zähler Get(2)
   }while(g_PSBMG==0);                    // g_PSBMG darf nicht 0 sein 1
}

BYTE GetNext_PSBMG()
{
   const bool UseBits[32]={false,false,false,false,false,false,false,true ,
                           true ,false,false,false,false,false,true ,false,
                           false,false,false,true ,false,false,false,false,
                           false,true ,false,false,false,true ,false,false};
   int       i;
   DWORD     g = g_PSBMG;
   DWORD     e = g_PSBMG;

   for (i=0;i<32;i++)
   {
      if(UseBits[i]) e^= g;
      g>>=1;
   }
   
   g_PSBMG<<=1;
   g_PSBMG |=e&1;

   return (BYTE) g_PSBMG;
}
 
void Fill_Generator()
{
   DWORD value;
   for (int i=0;i<256;i++)
   {
      value  = GetNext_PSBMG();
      value<<= 8;
      value |= GetNext_PSBMG();
      value<<= 8;
      value |= GetNext_PSBMG();
      value<<= 8;
      value |= GetNext_PSBMG();

      g_GeneratorField[i] = value;
   }

   g_GeneratorIndex = GetNext_PSBMG();
}

DWORD GetRandomValue()
{
   if (g_PSBMG == 0)
   {
      ::Init_PSBMG();
      ::Fill_Generator();
   }
   int ia1 = (GetNext_PSBMG() + g_GeneratorIndex) & 255;
   int ia2 = (GetNext_PSBMG() + g_GeneratorIndex) & 255;

   g_GeneratorIndex++;
   g_GeneratorIndex&=255;

   return (g_GeneratorField[g_GeneratorIndex] = g_GeneratorField[ia1] + g_GeneratorField[ia2]);
}

void RealFFT(int bRetransform, int nTau, double *pdFourier, PROGRESSFUNC pProgressCallback, void* pParam)
{
   int    md_2, md_4, nSigma, nSigma2, j_2, min_n, n_min_0, n_min_1;
   int    ind_1, ind_2, k, j, n, l, k_2, nTau2_2k, nTau2;

   double dFaktor, dArg, dArg_m, dArg_md2, dYtemp;
   double dEw_r, dEw_i, dEps_r, dEps_i, dUr, dUi, dWr, dWi;
   double dTemp, dTemp1, dTemp2, dTemp3, dTemp4;

   nTau2    = 2<<(nTau-1);
   md_2     = nTau2>>1;
   md_4     = md_2>>1;
   dFaktor   = 1.0 / md_2;
   dArg_md2 = 2*M_PI * dFaktor;
   dArg_m    =   M_PI * dFaktor;

   int nCount = 0;
   min_n   = md_2;
   n_min_1 = 1;

   if (pProgressCallback)
   {
      for (n = 1; n < nTau;n++)
      {
         min_n  >>= 1;
         n_min_0  = n_min_1<<1;
         for(j = 0;j <= md_2 - n_min_0;j += n_min_0)
            nCount++;

         for(l=1;l<n_min_1;l++)
	          for(j = 0;j <= md_2 - n_min_0;j += n_min_0)
            nCount++;
         n_min_1 = n_min_0;
      }
      pProgressCallback(pParam, nCount + md_4);
   }

   if (bRetransform)                      // Zusammenfassung der reellen Daten zur Durchführung
   {								               // einer FFT halber Länge fals mode=1
      dFaktor = 1;
      dYtemp = pdFourier[1];
      pdFourier[1] = pdFourier[0] - pdFourier[nTau2-1];
      pdFourier[0]+= pdFourier[nTau2-1];
      dEw_r   = cos(dArg_m);              // Real-,Imaginärteil der nTau2 ten Einheitswurzel
      dEw_i   = sin(dArg_m);
      dEps_r  = 1.0;						      // Real-,Imaginärteil von (nTau2 te Einheitwurzel)^k
      dEps_i  = 0.0;

      for (k=1;k<md_4;k++)
      {
	       k_2      = k<<1;
	       nTau2_2k = nTau2 - k_2;
	       dTemp     = dEps_r;
	       dEps_r    = dTemp*dEw_r - dEps_i*dEw_i;
	       dEps_i    = dTemp*dEw_i + dEps_i*dEw_r;

	       dTemp1   = .5* (dEps_r* (dYtemp - pdFourier[nTau2_2k-1]) + dEps_i* (pdFourier[k_2] + pdFourier[nTau2_2k]));
	       dTemp2   = .5* (dEps_i* (dYtemp - pdFourier[nTau2_2k-1]) - dEps_r* (pdFourier[k_2] + pdFourier[nTau2_2k]));
	       dTemp3   = .5* (dYtemp + pdFourier[nTau2_2k-1]);
	       dTemp4   = .5* (pdFourier[k_2] - pdFourier[nTau2_2k]);

	       dYtemp              = pdFourier[k_2+1];
	       pdFourier[k_2]        = dTemp3 - dTemp2;
	       pdFourier[k_2+1]      = dTemp1 - dTemp4;
	       pdFourier[nTau2_2k]   = dTemp2 + dTemp3;
	       pdFourier[nTau2_2k+1] = dTemp1 + dTemp4;
          if (pProgressCallback) pProgressCallback(pParam, -1);
      }

      pdFourier[md_2+1] = pdFourier[md_2];
      pdFourier[md_2]   = dYtemp;
   }

   for (j=0;j<md_2;j++)
   {
      k=j;							            // Umspeicherung mit Bit-Umkehrfunktion
      for(n=1,nSigma=0;n<nTau;n++)			// (gleichzeitige Normierung, falls mode = 0 )
      {
	       nSigma = (nSigma<<1) + (k&1);
	       k>>=1;
      }

      if (j<= nSigma)
      {
	       j_2      = j<<1;
	       nSigma2  = nSigma<<1;
	       dUr       = pdFourier[j_2];
	       dUi       = pdFourier[j_2+1];

	       pdFourier[j_2]       = dFaktor * pdFourier[nSigma2];
	       pdFourier[j_2+1]     = dFaktor * pdFourier[nSigma2+1];
	       pdFourier[nSigma2]   = dFaktor * dUr;
	       pdFourier[nSigma2+1] = dFaktor * dUi;
      }
   }

							                     // Durchführung der FFT halber Länge
   min_n   = md_2;
   n_min_1 = 1;

   for (n = 1; n < nTau;n++)
   {
      min_n  >>= 1;
      n_min_0  = n_min_1<<1;
      dArg      = dArg_md2 * min_n;


      dWr = cos(dArg);
      dWi = sin(dArg) * ((bRetransform) ? 1.0:-1.0);

      dEps_r = 1;
      dEps_i = 0;
								                  // Berechnung mit der Einheitswurzel w^l=1 mit (l=0)
								                  // d.h. keine Multiplikation nötig
      for(j = 0;j <= md_2 - n_min_0;j += n_min_0)
      {
	       ind_1 = j<<1;
	       ind_2 = ind_1 + n_min_0;

	       dUr    = pdFourier[ind_2];
	       dUi    = pdFourier[ind_2+1];

	       pdFourier[ind_2]    = pdFourier[ind_1]   - dUr;
	       pdFourier[ind_2+1]  = pdFourier[ind_1+1] - dUi;
	       pdFourier[ind_1]   += dUr;
	       pdFourier[ind_1+1] += dUi;
          if (pProgressCallback) pProgressCallback(pParam, -1);
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
	          dUr    = pdFourier[ind_2]*dEps_r - pdFourier[ind_2+1]*dEps_i;
	          dUi    = pdFourier[ind_2]*dEps_i + pdFourier[ind_2+1]*dEps_r;

	          pdFourier[ind_2]    = pdFourier[ind_1]   - dUr;
	          pdFourier[ind_2+1]  = pdFourier[ind_1+1] - dUi;
	          pdFourier[ind_1]   += dUr;
	          pdFourier[ind_1+1] += dUi;
             if (pProgressCallback) pProgressCallback(pParam, -1);
	       }
      }
      n_min_1 = n_min_0;
   }

   if (!bRetransform)
   {								               // Trennung der zusammengefaáten transformierten
      dYtemp             = pdFourier[nTau2-1];// Daten falls mode = 0
      pdFourier[nTau2-1] = .5* (pdFourier[0] - pdFourier[1]);
      pdFourier[0]       = .5* (pdFourier[0] + pdFourier[1]);

      dEw_r       = cos(dArg_m);
      dEw_i       = -sin(dArg_m);
      dEps_r      = 1.0;
      dEps_i      = 0.0;

      for(k=1;k<md_4;k++)
      {
	       dTemp   = dEps_r;
	       dEps_r  = dTemp*dEw_r - dEps_i*dEw_i;
	       dEps_i  = dTemp*dEw_i + dEps_i*dEw_r;
	       ind_1  = k<<1;
	       ind_2  = nTau2 - ind_1;

	       dTemp1 = .5* (dEps_i* (pdFourier[ind_1] - pdFourier[ind_2]) + dEps_r* (pdFourier[ind_1+1] + dYtemp));
	       dTemp2 = .5* (dEps_r* (pdFourier[ind_1] - pdFourier[ind_2]) - dEps_i* (pdFourier[ind_1+1] + dYtemp));
	       dTemp3 = .5* (pdFourier[ind_1] + pdFourier[ind_2]);
	       dTemp4 = .5* (pdFourier[ind_1+1] - dYtemp);

	       dYtemp           = pdFourier[ind_2-1];
	       pdFourier[ind_1-1] = dTemp1 + dTemp3;
	       pdFourier[ind_1]   = dTemp2 - dTemp4;
	       pdFourier[ind_2-1] = dTemp3 - dTemp1;
	       pdFourier[ind_2]   = dTemp2 + dTemp4;
          if (pProgressCallback) pProgressCallback(pParam, -1);
      }

      pdFourier[md_2-1] = pdFourier[md_2];
      pdFourier[md_2]   = dYtemp;
   }
}

void RealFFT_float(int bRetransform, int nTau, float *pdFourier, PROGRESSFUNC pProgressCallback, void* pParam)
{
   int    md_2, md_4, nSigma, nSigma2, j_2, min_n, n_min_0, n_min_1;
   int    ind_1, ind_2, k, j, n, l, k_2, nTau2_2k, nTau2;

   float dFaktor, dArg, dArg_m, dArg_md2, dYtemp;
   float dEw_r, dEw_i, dEps_r, dEps_i, dUr, dUi, dWr, dWi;
   float dTemp, dTemp1, dTemp2, dTemp3, dTemp4;

   nTau2    = 2<<(nTau-1);
   md_2     = nTau2>>1;
   md_4     = md_2>>1;
   dFaktor  = (float)(1.0 / md_2);
   dArg_md2 = (float)(2*M_PI * dFaktor);
   dArg_m   = (float)(  M_PI * dFaktor);

   int nCount = 0;
   min_n   = md_2;
   n_min_1 = 1;

   if (pProgressCallback)
   {
      for (n = 1; n < nTau;n++)
      {
         min_n  >>= 1;
         n_min_0  = n_min_1<<1;
         for(j = 0;j <= md_2 - n_min_0;j += n_min_0)
            nCount++;

         for(l=1;l<n_min_1;l++)
	          for(j = 0;j <= md_2 - n_min_0;j += n_min_0)
                 nCount++;
         n_min_1 = n_min_0;
      }
      pProgressCallback(pParam, nCount + md_4);
   }

   if (bRetransform)                      // Zusammenfassung der reellen Daten zur Durchführung
   {								               // einer FFT halber Länge fals mode=1
      dFaktor = 1;
      dYtemp = pdFourier[1];
      pdFourier[1] = pdFourier[0] - pdFourier[nTau2-1];
      pdFourier[0]+= pdFourier[nTau2-1];
      dEw_r   = (float)cos(dArg_m);       // Real-,Imaginärteil der nTau2 ten Einheitswurzel
      dEw_i   = (float)sin(dArg_m);
      dEps_r  = 1.0f;						   // Real-,Imaginärteil von (nTau2 te Einheitwurzel)^k
      dEps_i  = 0.0f;

      for (k=1;k<md_4;k++)
      {
	       k_2      = k<<1;
	       nTau2_2k = nTau2 - k_2;
	       dTemp     = dEps_r;
	       dEps_r    = dTemp*dEw_r - dEps_i*dEw_i;
	       dEps_i    = dTemp*dEw_i + dEps_i*dEw_r;

	       dTemp1   = (float).5* (dEps_r* (dYtemp - pdFourier[nTau2_2k-1]) + dEps_i* (pdFourier[k_2] + pdFourier[nTau2_2k]));
	       dTemp2   = (float).5* (dEps_i* (dYtemp - pdFourier[nTau2_2k-1]) - dEps_r* (pdFourier[k_2] + pdFourier[nTau2_2k]));
	       dTemp3   = (float).5* (dYtemp + pdFourier[nTau2_2k-1]);
	       dTemp4   = (float).5* (pdFourier[k_2] - pdFourier[nTau2_2k]);

	       dYtemp              = pdFourier[k_2+1];
	       pdFourier[k_2]        = dTemp3 - dTemp2;
	       pdFourier[k_2+1]      = dTemp1 - dTemp4;
	       pdFourier[nTau2_2k]   = dTemp2 + dTemp3;
	       pdFourier[nTau2_2k+1] = dTemp1 + dTemp4;
          if (pProgressCallback) pProgressCallback(pParam, -1);
      }

      pdFourier[md_2+1] = pdFourier[md_2];
      pdFourier[md_2]   = dYtemp;
   }

   for (j=0;j<md_2;j++)
   {
      k=j;							            // Umspeicherung mit Bit-Umkehrfunktion
      for(n=1,nSigma=0;n<nTau;n++)			// (gleichzeitige Normierung, falls mode = 0 )
      {
	       nSigma = (nSigma<<1) + (k&1);
	       k>>=1;
      }

      if (j<= nSigma)
      {
	       j_2      = j<<1;
	       nSigma2  = nSigma<<1;
	       dUr       = pdFourier[j_2];
	       dUi       = pdFourier[j_2+1];

	       pdFourier[j_2]       = dFaktor * pdFourier[nSigma2];
	       pdFourier[j_2+1]     = dFaktor * pdFourier[nSigma2+1];
	       pdFourier[nSigma2]   = dFaktor * dUr;
	       pdFourier[nSigma2+1] = dFaktor * dUi;
      }
   }

							                     // Durchführung der FFT halber Länge
   min_n   = md_2;
   n_min_1 = 1;

   for (n = 1; n < nTau;n++)
   {
      min_n  >>= 1;
      n_min_0  = n_min_1<<1;
      dArg      = dArg_md2 * min_n;


      dWr = (float)cos(dArg);
      dWi = (float)sin(dArg) * ((bRetransform) ? 1.0f:-1.0f);

      dEps_r = 1;
      dEps_i = 0;
								                  // Berechnung mit der Einheitswurzel w^l=1 mit (l=0)
								                  // d.h. keine Multiplikation nötig
      for(j = 0;j <= md_2 - n_min_0;j += n_min_0)
      {
	       ind_1 = j<<1;
	       ind_2 = ind_1 + n_min_0;

	       dUr    = pdFourier[ind_2];
	       dUi    = pdFourier[ind_2+1];

	       pdFourier[ind_2]    = pdFourier[ind_1]   - dUr;
	       pdFourier[ind_2+1]  = pdFourier[ind_1+1] - dUi;
	       pdFourier[ind_1]   += dUr;
	       pdFourier[ind_1+1] += dUi;
          if (pProgressCallback) pProgressCallback(pParam, -1);
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
	          dUr    = pdFourier[ind_2]*dEps_r - pdFourier[ind_2+1]*dEps_i;
	          dUi    = pdFourier[ind_2]*dEps_i + pdFourier[ind_2+1]*dEps_r;

	          pdFourier[ind_2]    = pdFourier[ind_1]   - dUr;
	          pdFourier[ind_2+1]  = pdFourier[ind_1+1] - dUi;
	          pdFourier[ind_1]   += dUr;
	          pdFourier[ind_1+1] += dUi;
             if (pProgressCallback) pProgressCallback(pParam, -1);
	       }
      }
      n_min_1 = n_min_0;
   }

   if (!bRetransform)
   {								               // Trennung der zusammengefaáten transformierten
      dYtemp             = pdFourier[nTau2-1];// Daten falls mode = 0
      pdFourier[nTau2-1] = (float).5* (pdFourier[0] - pdFourier[1]);
      pdFourier[0]       = (float).5* (pdFourier[0] + pdFourier[1]);

      dEw_r       = (float)cos(dArg_m);
      dEw_i       = (float)-sin(dArg_m);
      dEps_r      = 1.0;
      dEps_i      = 0.0;

      for(k=1;k<md_4;k++)
      {
	       dTemp   = dEps_r;
	       dEps_r  = dTemp*dEw_r - dEps_i*dEw_i;
	       dEps_i  = dTemp*dEw_i + dEps_i*dEw_r;
	       ind_1  = k<<1;
	       ind_2  = nTau2 - ind_1;

	       dTemp1 = (float).5* (dEps_i* (pdFourier[ind_1] - pdFourier[ind_2]) + dEps_r* (pdFourier[ind_1+1] + dYtemp));
	       dTemp2 = (float).5* (dEps_r* (pdFourier[ind_1] - pdFourier[ind_2]) - dEps_i* (pdFourier[ind_1+1] + dYtemp));
	       dTemp3 = (float).5* (pdFourier[ind_1] + pdFourier[ind_2]);
	       dTemp4 = (float).5* (pdFourier[ind_1+1] - dYtemp);

	       dYtemp           = pdFourier[ind_2-1];
	       pdFourier[ind_1-1] = dTemp1 + dTemp3;
	       pdFourier[ind_1]   = dTemp2 - dTemp4;
	       pdFourier[ind_2-1] = dTemp3 - dTemp1;
	       pdFourier[ind_2]   = dTemp2 + dTemp4;
          if (pProgressCallback) pProgressCallback(pParam, -1);
      }

      pdFourier[md_2-1] = pdFourier[md_2];
      pdFourier[md_2]   = dYtemp;
   }
}

void CalculateFilter(CFilterBox *pFS, int nFrequ, double dStep, double* pdFourierL, double *pdFourierR, PROGRESSFUNC pProgressCallback, void* pParam)
{
   int    i, j; 
   double dFrequ;

   if (pFS->bCalcArray)
   {
      nFrequ = pFS->nNoOfFrequencies;
   }
   else
   {
      dFrequ = 0;
   }

   if ((pFS->nTPOrder == 0) && (pFS->nHPOrder == 0))  // Pink Noise
   {
      double  dF0 = pFS->dTPFreqSelected;
      if (pProgressCallback) pProgressCallback(pParam, nFrequ);

      for(i = 0; i < nFrequ; i++ )
      {
         if (pFS->bCalcArray) dFrequ = 5.0 * pow(2.0, (double)i/9.0);
         else                 dFrequ += dStep;

         if (pFS->bCalcArray) // Filterfunktin im Array ablegen
         {
            pFS->pcFilterBox[i] = PinkNoiseFilter(dF0, dFrequ);
         }
         else                 // Filterfunktion heranmultiplizieren
         {
            j = i << 1;
            if (pdFourierL)
               MultiplyFilter(PinkNoiseFilter(dF0, dFrequ), pdFourierL[j+1], pdFourierL[j+2]);
            if (pdFourierR)
               MultiplyFilter(PinkNoiseFilter(dF0, dFrequ), pdFourierR[j+1], pdFourierR[j+2]);
         }
         if (pProgressCallback) pProgressCallback(pParam, -1);
      }
   }
   else                                             // Others
   {
      Complex     cSn;                              // = j*2Pi*freq/2Pi*Grenzfreq
      Complex     cFilterKoeff1, cFilterKoeff2;

      double      da1[5][5], da2[5][5], db1[5][5], db2[5][5]; // Koeffizienten[Order][Type] der Filterpolynome

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

      if (pProgressCallback) pProgressCallback(pParam, nFrequ);

      for( i = 0; i < nFrequ; i++ )
      {
         if (pFS->bCalcArray)
         {
            dFrequ = 5.0 * pow(2.0, (double)i/9.0);
         }
         else
         {
            dFrequ += dStep;
         }
         cFilterKoeff1 = Complex( 1.0, 0.0 );
         cFilterKoeff2 = Complex( 1.0, 0.0 );

         if( pFS->dHPFreqSelected > 0.0 )
            cSn = Complex( 0.0, dFrequ/pFS->dHPFreqSelected );
         else
            cSn = Complex( 0.0, dFrequ);           // HPFreq = 1 HZ

         if( (pFS->nHPType == 1) && (pFS->dHPQFaktor > 0.0) )
         {
            da1[2][1] = 1.0/pFS->dHPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
            da2[3][1] = 1.0/pFS->dHPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         }

         if( (pFS->nHPOrder > 0) && (pFS->nHPOrder < 5) &&
             (pFS->nHPType >= 0) && (pFS->nHPType  < 5) )
         {
            cFilterKoeff1  = 1.0 + da1[pFS->nHPOrder][pFS->nHPType]/cSn +
                                   db1[pFS->nHPOrder][pFS->nHPType]/(cSn*cSn);
            cFilterKoeff1 *= 1.0 + da2[pFS->nHPOrder][pFS->nHPType]/cSn +
                                   db2[pFS->nHPOrder][pFS->nHPType]/(cSn*cSn);
            cFilterKoeff1  = 1.0/cFilterKoeff1;
         }

         if( pFS->dTPFreqSelected > 0.0 )
            cSn = Complex( 0.0, dFrequ/pFS->dTPFreqSelected);
         else
            cSn = Complex( 0.0, dFrequ/1000000.0 ); // TPFreq = 1 MHZ

         if( (pFS->nTPType == 1) && (pFS->dTPQFaktor > 0.0) )
         {
            da1[2][1] = 1.0/pFS->dTPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
            da2[3][1] = 1.0/pFS->dTPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         }

         if( (pFS->nTPOrder > 0) && (pFS->nTPOrder < 5) &&
             (pFS->nTPType >= 0) && (pFS->nTPType  < 5) )
         {
            cFilterKoeff2  = 1.0 + da1[pFS->nTPOrder][pFS->nTPType]*cSn +
                                   db1[pFS->nTPOrder][pFS->nTPType]*cSn*cSn;
            cFilterKoeff2 *= 1.0 + da2[pFS->nTPOrder][pFS->nTPType]*cSn +
                                   db2[pFS->nTPOrder][pFS->nTPType]*cSn*cSn;
            cFilterKoeff2  = 1.0/cFilterKoeff2;
         }

         if (pFS->bCalcArray) // Filterfunktin im Array ablegen
         {
            pFS->pcFilterBox[i] = cFilterKoeff1*cFilterKoeff2;
         }
         else                 // Filterfunktion heranmultiplizieren
         {
            j = i << 1;
            if (pdFourierL)
               MultiplyFilter(cFilterKoeff1*cFilterKoeff2, pdFourierL[j+1], pdFourierL[j+2]);
            if (pdFourierR)
               MultiplyFilter(cFilterKoeff1*cFilterKoeff2, pdFourierR[j+1], pdFourierR[j+2]);
         }
         if (pProgressCallback) pProgressCallback(pParam, -1);
      }
   }
}

void CalculateFilter_float(CFilterBox *pFS, int nFrequ, double dStep, float* pdFourierL, float *pdFourierR, PROGRESSFUNC pProgressCallback, void* pParam)
{
   int    i, j; 
   double dFrequ, dA, dB;

   if (pFS->bCalcArray)
   {
      nFrequ = pFS->nNoOfFrequencies;
   }
   else
   {
      dFrequ = 0;
   }

   if ((pFS->nTPOrder == 0) && (pFS->nHPOrder == 0))  // Pink Noise
   {
      double  dF0 = pFS->dTPFreqSelected;
      if (pProgressCallback) pProgressCallback(pParam, nFrequ);

      for(i = 0; i < nFrequ; i++ )
      {
         if (pFS->bCalcArray) dFrequ = 5.0 * pow(2.0, (double)i/9.0);
         else                 dFrequ += dStep;

         if (pFS->bCalcArray) // Filterfunktin im Array ablegen
         {
            pFS->pcFilterBox[i] = PinkNoiseFilter(dF0, dFrequ);
         }
         else                 // Filterfunktion heranmultiplizieren
         {
            j = i << 1;
            if (pdFourierL)
            {
               dA = pdFourierL[j+1];
               dB = pdFourierL[j+2];
               MultiplyFilter(PinkNoiseFilter(dF0, dFrequ), dA, dB);
               pdFourierL[j+1] = (float)dA;
               pdFourierL[j+2] = (float)dB;
            }
            if (pdFourierR)
            {
               dA = pdFourierR[j+1];
               dB = pdFourierR[j+2];
               MultiplyFilter(PinkNoiseFilter(dF0, dFrequ), dA, dB);
               pdFourierR[j+1] = (float)dA;
               pdFourierR[j+2] = (float)dB;
            }
         }
         if (pProgressCallback) pProgressCallback(pParam, -1);
      }
   }
   else                                             // Others
   {
      Complex     cSn;                              // = j*2Pi*freq/2Pi*Grenzfreq
      Complex     cFilterKoeff1, cFilterKoeff2;

      double      da1[5][5], da2[5][5], db1[5][5], db2[5][5]; // Koeffizienten[Order][Type] der Filterpolynome

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

      if (pProgressCallback) pProgressCallback(pParam, nFrequ);

      for( i = 0; i < nFrequ; i++ )
      {
         if (pFS->bCalcArray)
         {
            dFrequ = 5.0 * pow(2.0, (double)i/9.0);
         }
         else
         {
            dFrequ += dStep;
         }
         cFilterKoeff1 = Complex( 1.0, 0.0 );
         cFilterKoeff2 = Complex( 1.0, 0.0 );

         if( pFS->dHPFreqSelected > 0.0 )
            cSn = Complex( 0.0, dFrequ/pFS->dHPFreqSelected );
         else
            cSn = Complex( 0.0, dFrequ);           // HPFreq = 1 HZ

         if( (pFS->nHPType == 1) && (pFS->dHPQFaktor > 0.0) )
         {
            da1[2][1] = 1.0/pFS->dHPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
            da2[3][1] = 1.0/pFS->dHPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         }

         if( (pFS->nHPOrder > 0) && (pFS->nHPOrder < 5) &&
             (pFS->nHPType >= 0) && (pFS->nHPType  < 5) )
         {
            cFilterKoeff1  = 1.0 + da1[pFS->nHPOrder][pFS->nHPType]/cSn +
                                   db1[pFS->nHPOrder][pFS->nHPType]/(cSn*cSn);
            cFilterKoeff1 *= 1.0 + da2[pFS->nHPOrder][pFS->nHPType]/cSn +
                                   db2[pFS->nHPOrder][pFS->nHPType]/(cSn*cSn);
            cFilterKoeff1  = 1.0/cFilterKoeff1;
         }

         if( pFS->dTPFreqSelected > 0.0 )
            cSn = Complex( 0.0, dFrequ/pFS->dTPFreqSelected);
         else
            cSn = Complex( 0.0, dFrequ/1000000.0 ); // TPFreq = 1 MHZ

         if( (pFS->nTPType == 1) && (pFS->dTPQFaktor > 0.0) )
         {
            da1[2][1] = 1.0/pFS->dTPQFaktor; // 2. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
            da2[3][1] = 1.0/pFS->dTPQFaktor; // 3. Ordnung, Butterworth mit beliebigem Q_HP oder Q_TP
         }

         if( (pFS->nTPOrder > 0) && (pFS->nTPOrder < 5) &&
             (pFS->nTPType >= 0) && (pFS->nTPType  < 5) )
         {
            cFilterKoeff2  = 1.0 + da1[pFS->nTPOrder][pFS->nTPType]*cSn +
                                   db1[pFS->nTPOrder][pFS->nTPType]*cSn*cSn;
            cFilterKoeff2 *= 1.0 + da2[pFS->nTPOrder][pFS->nTPType]*cSn +
                                   db2[pFS->nTPOrder][pFS->nTPType]*cSn*cSn;
            cFilterKoeff2  = 1.0/cFilterKoeff2;
         }

         if (pFS->bCalcArray) // Filterfunktin im Array ablegen
         {
            pFS->pcFilterBox[i] = cFilterKoeff1*cFilterKoeff2;
         }
         else                 // Filterfunktion heranmultiplizieren
         {
            j = i << 1;
            if (pdFourierL)
            {
               dA = pdFourierL[j+1];
               dB = pdFourierL[j+2];
               MultiplyFilter(cFilterKoeff1*cFilterKoeff2, dA, dB);
               pdFourierL[j+1] = (float)dA;
               pdFourierL[j+2] = (float)dB;
            }
            if (pdFourierR)
            {
               dA = pdFourierR[j+1];
               dB = pdFourierR[j+2];
               MultiplyFilter(cFilterKoeff1*cFilterKoeff2, dA, dB);
               pdFourierR[j+1] = (float)dA;
               pdFourierR[j+2] = (float)dB;
            }
         }
         if (pProgressCallback) pProgressCallback(pParam, -1);
      }
   }
}

void MultiplyFilterExp(double real, double imag, double &a, double &b)
{
   Complex cFK(real, imag);
   MultiplyFilter(cFK, a, b);
} 

void MultiplyFilter(Complex cFK, double &a, double &b)
{
   double  dA    = hypot (a,b);
   double  dPhi  = atan2(a,b);
   Complex cTemp = dA * Complex(cos(dPhi), sin(dPhi)) * cFK;
   dA   = Betrag(cTemp);
   dPhi = atan2(Imag(cTemp), Real(cTemp));
   a    = dA * sin(dPhi);
   b    = dA * cos(dPhi);
}

void PinkNoiseFilterExp(double dF0, double dFrequ, Complex&pErg)
{
   pErg = PinkNoiseFilter(dF0, dFrequ);
}

Complex PinkNoiseFilter(double dF0, double dFrequ)
{
   double dArgument = atan2(-dFrequ, dF0) * 0.5;
   return Complex(cos(dArgument), sin(dArgument)) / 
                  pow(1.0 + (dFrequ*dFrequ)/(dF0*dF0), 0.25);
}

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
   switch(ul_reason_for_call)
   {
      case DLL_PROCESS_ATTACH:
         g_hInstance = (HINSTANCE) hModule;
         break;
      case DLL_PROCESS_DETACH:
      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
      default: return false;
   }
   return TRUE;
}
