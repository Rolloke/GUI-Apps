/********************************************************************/
/*                                                                  */
/*        Schnittstellen-CPP/LIB-Datei für die DLL CARADIV          */
/*                  (nur für das Aufrufprogramm)                    */
/*             Definition der Aufruf-Klasse CEtsDiv                 */
/*                                                                  */
/* Die Schnittstellen-Funktionen von CARADIV werden vom aufrufenden */
/* Programm wie statische Klassen-Funktionen aufgerufen:            */
/*     CEtsDiv::publicFunction(..);                                 */
/*                                                                  */
/* Das aufrufende Programm deklariert an einer geeigneten Stelle    */
/* eine Instanz der Klasse CEtsDiv als lokale (globale) Variable.   */
/* Die Art und der Ort der Instanzdeklaration richtet sich nach dem */
/* Nutzungsbereich der CARADIV.DLL. Wenn die Instanz gelöscht wird, */
/* wird über den Destructor der Klasse CEtsDiv die DLL freigegeben  */
/* Das aufrufende Programm 'includes' die CEtsDiv.H und bindet die  */
/* Dateien CEtsDiv.LIB und CEtsDiv.H in ihr Projekt mit ein.        */
/*                                                                  */
/*                                                                  */
/*     programmed by Dipl. Ing. Rolf Ehlers                         */
/*                                                                  */
/*     Version 1.0          15.05.98                                */
/*                                                                  */
/********************************************************************/

#define  STRICT
#include <WINDOWS.H>
#include "ETSBIND.h"                             // Header für die Bindungs-DLL
#include "CCaraDiv.h"

#define  CARADIV_DLLERROR              0xFFFF   // DLL-Datei konnte nicht geladen werden
#ifndef ASSERT
   #define ASSERT
#endif

#define NO_OF_CCARADIV_FUNCTIONS 11
#define CARADIV_MODULENAME       "ETSDIV.DLL"

namespace CEtsDiv_Global
{
   HMODULE hDLL                                                         = NULL;
   bool  (*pCalculateLinDivision)(ETSDIV_DIV *, double&, double&)       = NULL;
   bool  (*pCalculateLogDivision)(ETSDIV_DIV *, double&, double&)       = NULL;
   bool  (*pCalculateFrqDivision)(ETSDIV_DIV *, double&, double&)       = NULL;
   bool  (*pNumberToText)(ETSDIV_NUTOTXT*, char*)                       = NULL;
   ULONG (*pTeiler)(ULONG, int)                                         = NULL;
   int   (*pTestFrqValue)(double)                                       = NULL;
   int   (*pTestLogValue)(double)                                       = NULL;
   int   (*pRound)(double)                                              = NULL;
   int   (*pDrawScale)(ETSDIV_SCALE*)                                   = NULL;
   bool  (*pRoundMinMaxValues)(ETSDIV_DIV*)                             = NULL;
   int   (*pGetSineCosineOfAngle)(int, DWORD, double, double&, double&) = NULL;
// ursprüngliche Anzahl 11
   bool  (*pNumberRepresentationDlg)(ETSDIV_NUTOTXT*, HWND)             = NULL;
   bool  (*pUnitConvertDlg)(ETSDIV_NUTOTXT*, HWND)                      = NULL;
   double(*pTextToNumber)(char*)                                        = NULL;


   bool pascal BindDll()                           // Funktion DLL Einbinden (pascal da eventuell oft vorhanden)
   {
      void *pFunc[11];
      ETSBind(CARADIV_MODULENAME,                  // Dateinamen der DLL
              hDLL,                                // obigen Module Handle für die DLL (reference)
              (void**)pFunc,
//              (void**)&pCalculateLinDivision,    // reference auf die Adresse des ersten Funktionspointer
              NO_OF_CCARADIV_FUNCTIONS,            // die Anzahl der Funktionspointer
              false);                              // Flag, Programmabbruch wenn DLL nicht gefunden

      if (hDLL)
      {
         *((void**)&pCalculateLinDivision) = pFunc[0];
         *((void**)&pCalculateLogDivision) = pFunc[1];
         *((void**)&pCalculateFrqDivision) = pFunc[2];
         *((void**)&pNumberToText)         = pFunc[3];
         *((void**)&pTeiler)               = pFunc[4];
         *((void**)&pTestFrqValue)         = pFunc[5];
         *((void**)&pTestLogValue)         = pFunc[6];
         *((void**)&pRound)                = pFunc[7];
         *((void**)&pDrawScale)            = pFunc[8];
         *((void**)&pRoundMinMaxValues)    = pFunc[9];
         *((void**)&pGetSineCosineOfAngle) = pFunc[10];
                                                   // 3 neue [optional]
         *((void**)&pNumberRepresentationDlg) = (void*)GetProcAddress(hDLL, MAKEINTRESOURCE(13));
         *((void**)&pUnitConvertDlg)          = (void*)GetProcAddress(hDLL, MAKEINTRESOURCE(14));
         *((void**)&pTextToNumber)            = (void*)GetProcAddress(hDLL, MAKEINTRESOURCE(15));
      }
      return (hDLL != NULL) ? true : false;
   }

   void inline UnBindDll()                         // Funktion DLL ausbinden (inline da selten vorhanden)
   {
      ETSUnBind(CARADIV_MODULENAME,                // Dateinamen der DLL
                hDLL,                              // obigen Module Handle für die DLL (reference)
                (void**)&pCalculateLinDivision,    // reference auf die Adresse des ersten Funktionspointer
                NO_OF_CCARADIV_FUNCTIONS,          // die Anzahl der Funktionspointer
                NULL, NULL);                       // NULL für statische Schnittstellenklasse
   }

   void inline TestBind()                          // Funktion Ausbindung überprüfen (inline da selten vorhanden)
   {
      ETSTestUnBind(CARADIV_MODULENAME,            // Dateinamen der DLL
                    hDLL,                          // obigen Module Handle für die DLL (reference)
                    NULL);                         // NULL für statische Schnittstellenklasse
   }
};

CEtsDiv::CEtsDiv()                          // Destructor
{
}

void CEtsDiv::Destructor()
{
   CEtsDiv_Global::UnBindDll();
}

bool CEtsDiv::CalculateLinDivision(ETSDIV_DIV *pdiv, double& fstep, double& fstart)
{
   if (pdiv)
   {
      if(CEtsDiv_Global::hDLL == NULL)
      {
         if (!CEtsDiv_Global::BindDll()) return false;
      }
      return (CEtsDiv_Global::pCalculateLinDivision( pdiv, fstep, fstart));
   }
   return (false);
}

bool CEtsDiv::CalculateFrqDivision(ETSDIV_DIV *pdiv, double& fstep, double& fstart)
{
   if (pdiv)
   {
      if(CEtsDiv_Global::hDLL == NULL)
      {
         if (!CEtsDiv_Global::BindDll()) return false;
      }
      return (CEtsDiv_Global::pCalculateFrqDivision(pdiv, fstep, fstart));
   }
   return (false);
}

bool CEtsDiv::CalculateLogDivision(ETSDIV_DIV *pdiv, double& fstep, double& fstart)
{
   if (pdiv)
   {
      if(CEtsDiv_Global::hDLL == NULL)
      {
         if (!CEtsDiv_Global::BindDll()) return false;
      }
      return (CEtsDiv_Global::pCalculateLogDivision(pdiv, fstep, fstart));
   }
   return (false);
}

bool CEtsDiv::NumberToText(ETSDIV_NUTOTXT *pntx, char *pszText)
{
   if (pntx && pszText)
   {
      if(CEtsDiv_Global::hDLL == NULL)
      {
         if (!CEtsDiv_Global::BindDll()) return false;
      }
      return (CEtsDiv_Global::pNumberToText(pntx, pszText));
   }
   return (false);
}

bool CEtsDiv::NumberRepresentationDlg(ETSDIV_NUTOTXT*pntx, HWND hwndParent)
{
   if (pntx)
   {
      if(CEtsDiv_Global::hDLL == NULL)
      {
         if (!CEtsDiv_Global::BindDll()) return false;
      }
      else if (CEtsDiv_Global::pNumberRepresentationDlg == NULL) return false;

      return (CEtsDiv_Global::pNumberRepresentationDlg(pntx, hwndParent));
   }
   return (false);
}

bool CEtsDiv::UnitConvertDlg(ETSDIV_NUTOTXT *pntx, HWND hwndParent)
{
   if (pntx && ::IsWindow(hwndParent))
   {
      if(CEtsDiv_Global::hDLL == NULL)
      {
         if (!CEtsDiv_Global::BindDll()) return false;
      }
      else if (CEtsDiv_Global::pUnitConvertDlg == NULL) return false;

      return (CEtsDiv_Global::pUnitConvertDlg(pntx, hwndParent));
   }
   return (false);
}


ULONG CEtsDiv::Teiler(ULONG lt, int nt)
{
   if(CEtsDiv_Global::hDLL == NULL)
   {
      if (!CEtsDiv_Global::BindDll()) return (0);
   }
   return (CEtsDiv_Global::pTeiler( lt, nt));
}

int CEtsDiv::TestFrqValue(double fvalue)
{
   if(CEtsDiv_Global::hDLL == NULL)
   {
      if (!CEtsDiv_Global::BindDll()) return (0);
   }

   return (CEtsDiv_Global::pTestFrqValue(fvalue));
}

int CEtsDiv::TestLogValue(double fvalue)
{
   if(CEtsDiv_Global::hDLL == NULL)
   {
      if (!CEtsDiv_Global::BindDll()) return (0);
   }

   return (CEtsDiv_Global::pTestLogValue(fvalue));
}

int CEtsDiv::Round(double dval)
{
   if(CEtsDiv_Global::hDLL == NULL)
   {
      if (!CEtsDiv_Global::BindDll()) return (0);
   }

   return (CEtsDiv_Global::pRound(dval));
}

int CEtsDiv::DrawScale(ETSDIV_SCALE* pScFkt)
{
   if(CEtsDiv_Global::hDLL == NULL)
   {
      if (!CEtsDiv_Global::BindDll()) return (0);
   }

   return (CEtsDiv_Global::pDrawScale(pScFkt));
}

bool CEtsDiv::RoundMinMaxValues(ETSDIV_DIV*pDiv)
{
   if(CEtsDiv_Global::hDLL == NULL)
   {
      if (!CEtsDiv_Global::BindDll()) return false;
   }

   return (CEtsDiv_Global::pRoundMinMaxValues(pDiv));
}

int CEtsDiv::GetSineCosineOfAngle(int nIndex, DWORD dwFlags, double dAngle, double &dSine, double &dCosine)
{
   if(CEtsDiv_Global::hDLL == NULL)
   {
      if (!CEtsDiv_Global::BindDll()) return -1;
   }

   return (CEtsDiv_Global::pGetSineCosineOfAngle(nIndex,dwFlags,dAngle,dSine,dCosine));
}

double CEtsDiv::TextToNumber(char*psz)
{
   if(CEtsDiv_Global::hDLL == NULL)
   {
      if (!CEtsDiv_Global::BindDll()) return 0.0;
   }
   else if (CEtsDiv_Global::pTextToNumber == NULL) return 0.0;
   return (CEtsDiv_Global::pTextToNumber(psz));
}
