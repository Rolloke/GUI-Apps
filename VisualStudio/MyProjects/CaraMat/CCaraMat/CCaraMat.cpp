/********************************************************************/
/*                                                                  */
/*         Schnittstellen-CPP/LIB-Datei für die DLL CARAMAT         */
/*                  (nur für das Aufrufprogramm)                    */
/*              Definition der Aufruf-Klasse CCaraMat               */
/*                                                                  */
/* Die Schnittstellen-Funktionen von CARAMAT werden vom aufrufenden */
/* Programm wie statische Klassen-Funktionen aufgerufen:            */
/*     CCaraMat::publicFunction(..);                                */
/*                                                                  */
/* Das aufrufende Programm deklariert an einer geeigneten Stelle    */
/* eine Instanz der Klasse CCaraMat als lokale (globale) Variable.  */
/* Die Art und der Ort der Instanzdeklaration richtet sich nach dem */
/* Nutzungsbereich der CARAMAT.DLL. Wenn die Instanz gelöscht wird, */
/* wird über den Destructor der Klasse CCaraMat die DLL freigegeben.*/
/* Das aufrufende Programm 'includes' die CCaraMat.H und bindet die */
/* Dateien CCaraMat.LIB und CCaraMat.H in ihr Projekt mit ein.      */
/*                                                                  */
/*                                                                  */
/*     programmed by Dr.U.F.Thomanek                                */
/*                                                                  */
/*     Version 1.1          30.09.99                                */
/*                                                                  */
/*     Edited by O. Wesnigk 27.08.99                                */
/*                                                                  */
/********************************************************************/


#define  STRICT
#include <WINDOWS.H>
#include "ETSBIND.h"                             // neu 27.08.99
#include "CCaraMat.h"


namespace CCaraMat_Global                        // ein eigener Namensbereich ist äußerst wichtig, da
{                                                // alle Komponenten die gleiche Namen wie BindDll enthalten
                                                 // Vorbelegung Handle und Funktionspointer auf NULL
   HMODULE hDLL                                                 = NULL;
   int     (*pGetLastError)()                                   = NULL;
   bool    (*pEditMaterial)( HWND )                             = NULL;
   bool    (*pSelectMaterial)( HWND, int *, char * )            = NULL;
   bool    (*pGetBitmapInfo)( int, int, HANDLE, BITMAPINFO ** ) = NULL;
   bool    (*pGetMatDscrDaten)( int, CARAMAT_MATDSCRS * )       = NULL;
   bool    (*pGetMatReflDaten)( int, CARAMAT_MATREFLS * )       = NULL;

   bool pascal BindDll()                         // Funktion DLL Einbinden (pascal da eventuell oft vorhanden)
   {
      return ETSBind("CARAMAT.DLL",              // Dateinamen der DLL
                     hDLL,                       // obigen Module Handle für die DLL (reference)
                     (void**)&pGetLastError,     // reference auf die Adresse des ersten Funktionspointer
                     6,                          // die Anzahl der Funktionspointer
                     true);                      // Flag, Programmabbruch wenn DLL nicht gefunden hier gesetzt
   }

   void inline UnBindDll()                       // Funktion DLL ausbinden (inline da selten vorhanden)
   {
      ETSUnBind("CARAMAT.DLL",                   // Dateinamen der DLL
                hDLL,                            // obigen Module Handle für die DLL (reference)
                (void**)&pGetLastError,          // reference auf die Adresse des ersten Funktionspointer
                6,                               // die Anzahl der Funktionspointer
                NULL,                            // NULL für statische Schnittstellenklasse
                NULL);                           // kein Parameter nötig
   }

   void inline TestBind()                        // Funktion Ausbindung überprüfen (inline da selten vorhanden)
   {
      ETSTestUnBind("CARAMAT.DLL",               // Dateinamen der DLL
                    hDLL,                        // obigen Module Handle für die DLL (reference)
                    NULL);                       // NULL für statische Schnittstellenklasse
   }
};

CCaraMat::~CCaraMat()                            // Destructor
{
   CCaraMat_Global::TestBind();                  // test ob DLL- Ausgebunden
}

void CCaraMat::Destructor()
{
   CCaraMat_Global::UnBindDll();                 // die DLL ausbinden
}

int   CCaraMat::GetLastError()                   // letzten Fehler abfragen
{
   if(CCaraMat_Global::hDLL == NULL)             // wurde die DLL noch nicht eingebunden ?
      if(CCaraMat_Global::BindDll()) return CARAMAT_DLLERROR;

   return CCaraMat_Global::pGetLastError();      
}

bool   CCaraMat::EditMaterial( HWND hWnd )       // Eingabe neuer Absorbermaterialien/Ändern vorhandener
{
   if(CCaraMat_Global::hDLL == NULL)             // wurde die DLL noch nicht eingebunden ?
      if(CCaraMat_Global::BindDll()) return false;

   return CCaraMat_Global::pEditMaterial(hWnd);
}
                                                 // Auswahl eines Materialindexes
bool   CCaraMat::SelectMaterial( HWND hWnd, int *pnMatIndex, char *pszCaption )
{
   if(CCaraMat_Global::hDLL == NULL)             // wurde die DLL noch nicht eingebunden ?
      if(CCaraMat_Global::BindDll()) return false;

   return CCaraMat_Global::pSelectMaterial(hWnd, pnMatIndex, pszCaption );
}
                                                 // Abfrage der BitMapInfo zum Material nMatIndex
bool   CCaraMat::GetBitmapInfo( int nMatIndex, int nType, HANDLE hMyHeap, BITMAPINFO **ppBitmapInfo )
{
   if(CCaraMat_Global::hDLL == NULL)             // wurde die DLL noch nicht eingebunden ?
      if(CCaraMat_Global::BindDll()) return false;

   return CCaraMat_Global::pGetBitmapInfo( nMatIndex, nType, hMyHeap, ppBitmapInfo );
}
                                                 // Auswahl eines Materialindexes
bool   CCaraMat::GetMatDscrDaten( int nMatIndex, CARAMAT_MATDSCRS *pMatDscrDaten )
{
   if(CCaraMat_Global::hDLL == NULL)             // wurde die DLL noch nicht eingebunden ?
      if(CCaraMat_Global::BindDll()) return false;

   return CCaraMat_Global::pGetMatDscrDaten(nMatIndex, pMatDscrDaten);
}
                                                 // Auswahl eines Materialindexes
bool   CCaraMat::GetMatReflDaten( int nMatIndex, CARAMAT_MATREFLS *pMatReflDaten )
{
   if(CCaraMat_Global::hDLL == NULL)             // wurde die DLL noch nicht eingebunden ?
      if(CCaraMat_Global::BindDll()) return false;

   return CCaraMat_Global::pGetMatReflDaten( nMatIndex, pMatReflDaten);
}
