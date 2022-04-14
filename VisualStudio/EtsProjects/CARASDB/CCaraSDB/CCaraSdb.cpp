
#define  STRICT

#include <WINDOWS.H>
#include "ETSBIND.h"                             // Header für die Bindungs-DLL
#include "CCaraSdb.h"
#include "CustErr.h"

#define NO_OF_CCARASDB_FUNCTIONS 2
#define CCARASDB_MODULENAME "CARASDB.DLL"

namespace CCaraSdb_Global
{
   HMODULE   hDLL                           = NULL;
   int     (*pDoModal )   (CARASDB_PARAMS&) = NULL;
   void    (*pUpdateDataBase)(char *)       = NULL;
   int       nInstances                     = 0;

   bool inline BindDll()                           // Funktion DLL Einbinden (pascal da eventuell oft vorhanden)
   {
      return ETSBind(CCARASDB_MODULENAME,          // Dateinamen der DLL
                     hDLL,                         // obigen Module Handle für die DLL (reference)
                     (void**)&pDoModal,            // reference auf die Adresse des ersten Funktionspointer
                     NO_OF_CCARASDB_FUNCTIONS,     // die Anzahl der Funktionspointer
                     false);                       // Flag, Programmabbruch wenn DLL nicht gefunden
   }

   void inline UnBindDll(void *pThis)              // Funktion DLL ausbinden (inline da selten vorhanden)
   {
      ETSUnBind(CCARASDB_MODULENAME,               // Dateinamen der DLL
                hDLL,                              // obigen Module Handle für die DLL (reference)
                (void**)&pDoModal,                 // reference auf die Adresse des ersten Funktionspointer
                NO_OF_CCARASDB_FUNCTIONS,          // die Anzahl der Funktionspointer
                &nInstances,                       // reference auf Instanzzähler da dynamische Schnittstellenklasse
                pThis);                            // This-Pointer der Klasse
   }

   void inline TestBind()                          // Funktion Ausbindung überprüfen (inline da selten vorhanden)
   {
      ETSTestUnBind(CCARASDB_MODULENAME,           // Dateinamen der DLL
                    hDLL,                          // obigen Module Handle für die DLL (reference)
                    &nInstances);                  // reference auf Instanzzähler da dynamische Schnittstellenklasse
   }
};

CCaraSdb::CCaraSdb()                               // Konstructor
{
   CCaraSdb_Global::nInstances++;
}

CCaraSdb::~CCaraSdb()                              // Destructor
{
   CCaraSdb_Global::nInstances--;
   CCaraSdb_Global::TestBind();
}

bool CCaraSdb::DoModal(CARASDB_PARAMS &param)
{
   if(CCaraSdb_Global::hDLL == NULL) CCaraSdb_Global::BindDll();
   if(CCaraSdb_Global::pDoModal)
   {
      int nLastError = CCaraSdb_Global::pDoModal(param);
      if (nLastError == 1) return true;
   }
   return false;
}

void CCaraSdb::UpdateDataBase(char *pszBoxName)
{
   if(CCaraSdb_Global::hDLL == NULL) CCaraSdb_Global::BindDll();
   if(CCaraSdb_Global::pUpdateDataBase)
   {
      CCaraSdb_Global::pUpdateDataBase(pszBoxName);
   }
}


void CCaraSdb::Destructor()
{
   CCaraSdb_Global::UnBindDll(this);
}
