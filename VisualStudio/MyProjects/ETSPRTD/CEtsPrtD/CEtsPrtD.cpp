/**************************************+****************************************

                                  CEtsPrtD.cpp
                                  ------------

           Implementation der Schnittstellenklasse für die ETSPRTD.DLL

                     (C) 1999 ELAC Technische Software GmbH

                          VERSION 1.0 Stand 27.08.1999


                                                   programmed by Oliver Wesnigk
*******************************************************************************/

#define STRICT
#include <windows.h>
#include "ETSBIND.h"
#include "CEtsPrtD.h"


namespace CEtsPrtD_Global                        // ein eigener Namensbereich ist äußerst wichtig, da
{                                                // viele Komponenten die gleiche Funktion LoadFunctions enthalten können etc.
   HMODULE hDLL                                    = NULL;
   int     (*pDoDataExchange)(ETSPRTD_PARAMETER *) = NULL;
   int     (*pEndPrinting)   (ETSPRTD_PARAMETER *) = NULL;
   int     (*pDoDialog)      (ETSPRTD_PARAMETER *) = NULL;

   bool pascal BindDll()                         // Funktion DLL Einbinden (pascal da eventuell oft vorhanden)
   {
      return ETSBind("ETSPRTD.DLL",              // Dateinamen der DLL
                     hDLL,                       // obigen Module Handle für die DLL (reference)
                     (void**)&pDoDataExchange,   // reference auf die Adresse des ersten Funktionspointer
                     3,                          // die Anzahl der Funktionspointer
                     false);                     // Flag, Programmabbruch wenn DLL nicht gefunden
   }

   void inline UnBindDll()                       // Funktion DLL ausbinden (inline da selten vorhanden)
   {
      ETSUnBind("ETSPRTD.DLL",                   // Dateinamen der DLL
                hDLL,                            // obigen Module Handle für die DLL (reference)
                (void**)&pDoDataExchange,        // reference auf die Adresse des ersten Funktionspointer
                3,                               // die Anzahl der Funktionspointer
                NULL,                            // NULL für statische Schnittstellenklasse
                NULL);                           // kein Parameter nötig
   }

   void inline TestBind()                        // Funktion Ausbindung überprüfen (inline da selten vorhanden)
   {
      ETSTestUnBind("ETSPRTD.DLL",               // Dateinamen der DLL
                    hDLL,                        // obigen Module Handle für die DLL (reference)
                    NULL);                       // NULL für statische Schnittstellenklasse
   }
};


CEtsPrtD::~CEtsPrtD()
{
   CEtsPrtD_Global::TestBind();                  // Aufruf der Funktion im richtigen namespace
}


void CEtsPrtD::Destructor()
{
   CEtsPrtD_Global::UnBindDll();                 // Aufruf der Funktion im richtigen namespace
}

int CEtsPrtD::DoDataExchange(ETSPRTD_PARAMETER * pParams)
{
   if(CEtsPrtD_Global::hDLL==NULL) 
      if(CEtsPrtD_Global::BindDll()) return -1;

   return CEtsPrtD_Global::pDoDataExchange(pParams);
}

int CEtsPrtD::EndPrinting(ETSPRTD_PARAMETER * pParams)
{
   if(CEtsPrtD_Global::hDLL==NULL) 
      if(CEtsPrtD_Global::BindDll()) return -1;

   return CEtsPrtD_Global::pEndPrinting(pParams);
}

int CEtsPrtD::DoDialog(ETSPRTD_PARAMETER * pParams)
{
   if(CEtsPrtD_Global::hDLL==NULL) 
      if(CEtsPrtD_Global::BindDll()) return -1;

   return CEtsPrtD_Global::pDoDialog(pParams);
}
