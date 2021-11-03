/********************************************************************/
/*                                                                  */
/*        Schnittstellen-CPP/LIB-Datei für die DLL CARABOX          */
/*                  (nur für das Aufrufprogramm)                    */
/*             Definition der Aufruf-Klasse CCaraBox                */
/*                                                                  */
/* Die Schnittstellen-Funktionen von CARABOX werden vom aufrufenden */
/* Programm wie statische Klassen-Funktionen aufgerufen:            */
/*     CCaraBox::publicFunction(..);                                */
/*                                                                  */
/* Das aufrufende Programm deklariert an einer geeigneten Stelle    */
/* eine Instanz der Klasse CCaraBox als lokale (globale) Variable.  */
/* Die Art und der Ort der Instanzdeklaration richtet sich nach dem */
/* Nutzungsbereich der CARABOX.DLL. Wenn die Instanz gelöscht wird, */
/* wird über den Destructor der Klasse CCaraBox die DLL freigegeben */
/* Das aufrufende Programm 'includes' die CCaraBox.H und bindet die */
/* Dateien CCaraBox.LIB und CCaraBox.H in ihr Projekt mit ein.      */
/*                                                                  */
/*                                                                  */
/*     programmed by Dr.U.F.Thomanek                                */
/*                                                                  */
/*     Version 1.0          01.09.99                                */
/*                                                                  */
/********************************************************************/


#define  STRICT
#include <WINDOWS.H>
#include "ETSBIND.H"
#include "CCaraBox.h"

#define BOXDLLNAME "CARABOX.DLL"
#define NO_OF_FNC  3

namespace CCaraBox_Global                        // ein eigener Namensbereich ist äußerst wichtig, da
{                                                // alle Komponenten die gleiche Namen wie BindDll enthalten
   HMODULE hDLL               = NULL;            // Module Handle für die DLL
   int     (*pGetLastError)()                   = NULL;
   bool    (*pBoxEdit)(HWND)                    = NULL;
   void    (*pSetBoxParam)(const char*, DWORD)  = NULL;

   bool pascal BindDll()                         // Funktion DLL Einbinden (pascal da eventuell oft vorhanden)
   {
      return ETSBind(BOXDLLNAME,                 // Dateinamen der DLL
                     hDLL,                       // obigen Module Handle für die DLL (reference)
                     (void**)&pGetLastError,     // Reference auf die Adresse des ersten Funktionspointer
                     NO_OF_FNC,                  // die Anzahl der Funktionspointer
                     false);                     // Flag, Programmabbruch wenn DLL nicht gefunden
   }

   void inline UnBindDll()                       // Funktion DLL ausbinden (inline da selten vorhanden)
   {
      ETSUnBind(BOXDLLNAME,                      // Dateinamen der DLL
                hDLL,                            // obigen Module Handle für die DLL (reference)
                (void**)&pGetLastError,          // Reference auf die Adresse des ersten Funktionspointer
                NO_OF_FNC,                       // die Anzahl der Funktionspointer
                NULL,                            // NULL für statische Schnittstellenklasse
                NULL);                           // ein Parameter der an den DLL - Destructor übergeben wird
   }

   void inline TestBind()                        // Funktion Ausbindung überprüfen (inline da selten vorhanden)
   {
      ETSTestUnBind(BOXDLLNAME,               // Dateinamen der DLL
                    hDLL,                        // obigen Module Handle für die DLL (reference)
                    NULL);                       // NULL für statische Schnittstellenklasse
   }
};


/**************************************+****************************************

                        Destructor der Schnittstellenklasse

Nötig weil: der Destructor der Schnittstellenklasse überprüft, ob der
            DLL-Destructor aufgerufen worden ist, also nur nötig um
            Programmierfehler zu vermeiden !

*******************************************************************************/

CCaraBox::~CCaraBox()
{
   CCaraBox_Global::TestBind();                  // Aufruf der Funktion im richtigen namespace
}


/**************************************+****************************************

                          Destructor der CARABOX.DLL

Nötig weil: dieser Destructor muß explizit aufgerufen werden um die DLL
            aus dem Adressraum der DLL / Anwendung zu entfernen.
            Wichtig, weil DLL´s die ander DLL´s einbinden diese freigeben
            müssen aber dieses nicht in der DLLMAIN erlaubt ist !

*******************************************************************************/

void CCaraBox::Destructor()
{
   CCaraBox_Global::UnBindDll();                 // Aufruf der Funktion im richtigen namespace
}

/**************************************+****************************************

                        CARABOX-DLL-Funktionen

*******************************************************************************/

int   CCaraBox::GetLastError()                   // letzten Fehler abfragen
{
   if(CCaraBox_Global::hDLL==NULL)               // DLL wurde noch nicht eingebunden, dann einbinden !
      if(CCaraBox_Global::BindDll())             // einbinden durch Aufruf BindDll
         return (CARABOX_DLLERROR);              // Fehler, DLL konnte nicht eingebunden werden

   return CCaraBox_Global::pGetLastError();
}

                                                 // Aufruf des Boxeneditors
bool CCaraBox::BoxEdit( HWND hParent )
{
   if(CCaraBox_Global::hDLL==NULL)               // DLL wurde noch nicht eingebunden, dann einbinden !
      if(CCaraBox_Global::BindDll())             // einbinden durch Aufruf BindDll
         return (false);                         // Fehler, DLL konnte nicht eingebunden werden

   return CCaraBox_Global::pBoxEdit( hParent );
}

void CCaraBox::SetBoxParams(const char *pszFile, DWORD dwColor)
{
   if(CCaraBox_Global::hDLL==NULL)               // DLL wurde noch nicht eingebunden, dann einbinden !
      if(CCaraBox_Global::BindDll())            // einbinden durch Aufruf BindDll
         return;
   CCaraBox_Global::pSetBoxParam(pszFile, dwColor);
}
