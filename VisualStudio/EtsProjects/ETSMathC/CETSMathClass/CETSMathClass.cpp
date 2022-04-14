/*********************************************************************

         Schnittstellen-CPP/LIB-Datei für die DLL CETSMATHCLASS
                   (nur für das Aufrufprogramm)
              Definition der Aufruf-Klasse CEtsMathClass

  Die Schnittstellen-Funktionen von ETSMATHC werden vom aufrufenden
  Programm wie statische Klassen-Funktionen aufgerufen:
      CETSMathClass::publicFunction(..);

  Das aufrufende Programm deklariert an einer geeigneten Stelle
  eine Instanz der Klasse CETSMathClass als lokale (globale) Variable.
  Die Art und der Ort der Instanzdeklaration richtet sich nach dem
  Nutzungsbereich der ETSMATHC.DLL. Wenn die Instanz gelöscht wird,
  wird über den Destructor der Klasse CETSMathClass die DLL freigegeben

  Das aufrufende Programm 'includes' die CETSMathClass.H und bindet die
  Dateien CETSMathClass.LIB und CETSMathClass.H in ihr Projekt mit ein.


      programmed by Dipl. Ing. Rolf Ehlers
      Version 1.0          15.05.98

********************************************************************/

#define  STRICT
#include <WINDOWS.H>
#include "ETSBIND.h"                               // Header für die Bindungs-DLL
#include "CEtsMathClass.h"

#define  CETSMATHCLASS_DLLERROR     0xFFFF         // DLL-Datei konnte nicht geladen werden
#ifndef ASSERT
   #define ASSERT
#endif

#define NO_OF_CETSMATHCLASS_FUNCTIONS 7

#define CETSMATHCLASS_MODULENAME       "ETSMATHC.DLL"

namespace CEtsMathClass_Global
{
   HMODULE hDLL                                                                              = NULL;
   DWORD  (*pGetRandomValue)()                                                               = NULL;
   void   (*pRealFFT)(int,int,double*,PROGRESSFUNC,void*)                                    = NULL;
   void   (*pCalculateFilter)(CFilterBox*,int,double,double*,double*,PROGRESSFUNC,void*)     = NULL;
   void   (*pRealFFT_float)(int,int,float*,PROGRESSFUNC,void*)                               = NULL;
   void   (*pCalculateFilter_float)(CFilterBox*,int,double,float*,float*,PROGRESSFUNC,void*) = NULL;
   void   (*pMultiplyFilterExp)(double,double,double&,double&)                               = NULL;
   void   (*pPinkNoiseFilterExp)(double,double,double&,double&)                              = NULL;

   bool pascal BindDll()                           // Funktion DLL Einbinden (pascal da eventuell oft vorhanden)
   {
      return ETSBind(CETSMATHCLASS_MODULENAME,     // Dateinamen der DLL
                    hDLL,                          // obigen Module Handle für die DLL (reference)
                    (void**)&pGetRandomValue,      // reference auf die Adresse des ersten Funktionspointer
                    NO_OF_CETSMATHCLASS_FUNCTIONS, // die Anzahl der Funktionspointer
                    false);                        // Flag, Programmabbruch wenn DLL nicht gefunden
   }

   void inline UnBindDll()                         // Funktion DLL ausbinden (inline da selten vorhanden)
   {
      ETSUnBind(CETSMATHCLASS_MODULENAME,          // Dateinamen der DLL
                hDLL,                              // obigen Module Handle für die DLL (reference)
                (void**)&pGetRandomValue,          // reference auf die Adresse des ersten Funktionspointer
                NO_OF_CETSMATHCLASS_FUNCTIONS,     // die Anzahl der Funktionspointer
                NULL, NULL);                       // NULL für statische Schnittstellenklasse
   }

   void inline TestBind()                          // Funktion Ausbindung überprüfen (inline da selten vorhanden)
   {
      ETSTestUnBind(CETSMATHCLASS_MODULENAME,      // Dateinamen der DLL
                    hDLL,                          // obigen Module Handle für die DLL (reference)
                    NULL);                         // NULL für statische Schnittstellenklasse
   }
};

CFilterBox::CFilterBox(int nSize)
{
   pcFilterBox = new Complex[nSize];
   pdFilterFrq = new double[nSize];
}

CFilterBox::~CFilterBox()
{
   if (pcFilterBox) delete[] pcFilterBox;
   if (pdFilterFrq) delete[] pdFilterFrq;
}

bool    CFilterBox::SetFrequency(int nVal, double dVal)
{
   if (!InRange(nVal)) return false;
   pdFilterFrq[nVal] = dVal;
   return true;
}

double  CFilterBox::GetFrequency(int nVal)
{
   if (InRange(nVal))
      return pdFilterFrq[nVal];
   return 0.0;
}

bool    CFilterBox::SetCoefficient(int nVal, Complex cCoef)
{
   if (!InRange(nVal)) return false;
   pcFilterBox[nVal] = cCoef;
   return true;
}

Complex CFilterBox::GetCoefficient(int nVal)
{
   if (InRange(nVal))
      return pcFilterBox[nVal];
   return Complex(0.0, 0.0);
}

CETSMathClass::CETSMathClass()                     // Destructor
{
}

void CETSMathClass::Destructor()
{
   CEtsMathClass_Global::UnBindDll();
}

DWORD CETSMathClass::GetRandomValue()
{
   if(CEtsMathClass_Global::hDLL == NULL)
   {
      CEtsMathClass_Global::BindDll();
      if (CEtsMathClass_Global::hDLL == NULL) return 0;
   }
   return (CEtsMathClass_Global::pGetRandomValue());
}

void CETSMathClass::RealFFT(int bRetransform, int nTau, double *pdFourier, PROGRESSFUNC pProgresCallback, void*pParam)
{
   if(CEtsMathClass_Global::hDLL == NULL)
   {
      CEtsMathClass_Global::BindDll();
      if (CEtsMathClass_Global::hDLL == NULL) return;
   }
   CEtsMathClass_Global::pRealFFT(bRetransform, nTau, pdFourier, pProgresCallback, pParam);
}

void CETSMathClass::CalculateFilter(CFilterBox*pFB, int n, double dFrequ, double*dL, double*dR, PROGRESSFUNC pFunc, void*pParam)
{
   if(CEtsMathClass_Global::hDLL == NULL)
   {
      CEtsMathClass_Global::BindDll();
      if (CEtsMathClass_Global::hDLL == NULL) return;
   }
   CEtsMathClass_Global::pCalculateFilter(pFB, n, dFrequ, dL, dR, pFunc, pParam);
}

void CETSMathClass::RealFFT(int bRetransform, int nTau, float*pfFourier, PROGRESSFUNC pProgresCallback, void*pParam)
{
   if(CEtsMathClass_Global::hDLL == NULL)
   {
      CEtsMathClass_Global::BindDll();
      if (CEtsMathClass_Global::hDLL == NULL) return;
   }
   CEtsMathClass_Global::pRealFFT_float(bRetransform, nTau, pfFourier, pProgresCallback, pParam);
}

void CETSMathClass::CalculateFilter(CFilterBox*pFB, int n, double dFrequ, float*dL, float*dR, PROGRESSFUNC pFunc, void*pParam)
{
   if(CEtsMathClass_Global::hDLL == NULL)
   {
      CEtsMathClass_Global::BindDll();
      if (CEtsMathClass_Global::hDLL == NULL) return;
   }
   CEtsMathClass_Global::pCalculateFilter_float(pFB, n, dFrequ, dL, dR, pFunc, pParam);
}

void CETSMathClass::MultiplyFilter(Complex cFK, double &a, double &b)
{
   if(CEtsMathClass_Global::hDLL == NULL)
   {
      CEtsMathClass_Global::BindDll();
      if (CEtsMathClass_Global::hDLL == NULL) return;
   }
   CEtsMathClass_Global::pMultiplyFilterExp(cFK.realteil, cFK.imagteil, a, b);
}

Complex CETSMathClass::PinkNoiseFilter(double dF0, double dFrequ)
{
   Complex cErg;
   if(CEtsMathClass_Global::hDLL == NULL)
   {
      CEtsMathClass_Global::BindDll();
      if (CEtsMathClass_Global::hDLL == NULL) return cErg;
   }
   CEtsMathClass_Global::pPinkNoiseFilterExp(dF0, dFrequ, cErg.realteil, cErg.imagteil);
   return cErg;
}

