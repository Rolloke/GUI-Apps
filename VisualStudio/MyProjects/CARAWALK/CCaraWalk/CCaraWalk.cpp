// CaraWalk.cpp: Implementierung der Klasse CCaraWalk.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ETSBIND.h"                             // Header für die Bindungs-DLL
#include "CustErr.h"
#include "CCaraWalk.h"
#include <stdlib.h>
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
#define NO_OF_CCARAWALK_FUNCTIONS 4
#define CCARAWALK_MODULENAME  "CARAWALK.DLL"

namespace CCaraWalk_Global
{
   HMODULE   hDLL                                                         = NULL;
   HWND    (*pCreate )          (CCaraWalk*, LPCTSTR, DWORD, RECT*, HWND) = NULL;
   bool    (*pDoDataExchange)   (CCaraWalk*, DWORD, int, void *)          = NULL;
   BOOL    (*pMoveWindow)       (CCaraWalk*, int, int, int, int, bool)    = NULL;
   void    (*pDelete)           (void*)                                   = NULL;

   int       nInstances                                                   = 0;

   bool pascal BindDll()                           // Funktion DLL Einbinden (pascal da eventuell oft vorhanden)
   {
      return ETSBind(CCARAWALK_MODULENAME,         // Dateinamen der DLL
                     hDLL,                         // obigen Module Handle für die DLL (reference)
                     (void**)&pCreate,             // reference auf die Adresse des ersten Funktionspointer
                     NO_OF_CCARAWALK_FUNCTIONS,    // die Anzahl der Funktionspointer
                     false);                       // Flag, Programmabbruch wenn DLL nicht gefunden
   }

   void inline UnBindDll(void *pThis)              // Funktion DLL ausbinden (inline da selten vorhanden)
   {
      ETSUnBind(CCARAWALK_MODULENAME,              // Dateinamen der DLL
                hDLL,                              // obigen Module Handle für die DLL (reference)
                (void**)&pCreate,                  // reference auf die Adresse des ersten Funktionspointer
                NO_OF_CCARAWALK_FUNCTIONS,         // die Anzahl der Funktionspointer
                &nInstances,                       // reference auf Instanzzähler da dynamische Schnittstellenklasse
                pThis);                            // den This-Pointer der Klasse übergeben
   }

   void inline TestBind()                          // Funktion Ausbindung überprüfen (inline da selten vorhanden)
   {
      ETSTestUnBind(CCARAWALK_MODULENAME,          // Dateinamen der DLL
                    hDLL,                          // obigen Module Handle für die DLL (reference)
                    &nInstances);                  // reference auf Instanzzähler da dynamische Schnittstellenklasse
   }
};

CCaraWalk::CCaraWalk()
{
   CCaraWalk_Global::nInstances++;
   m_pData = NULL;
}

CCaraWalk::~CCaraWalk()
{
   CCaraWalk_Global::nInstances--;
   CCaraWalk_Global::TestBind();

   if ((CCaraWalk_Global::nInstances>0) && (m_pData != NULL) && (CCaraWalk_Global::pDelete != NULL))
   {
      CCaraWalk_Global::pDelete(m_pData);
      m_pData = NULL;
   }
}

void CCaraWalk::Destructor()
{
   CCaraWalk_Global::UnBindDll(this);
}

HWND CCaraWalk::Create(LPCTSTR strWndText, DWORD dwFlags, RECT *prcWnd, HWND hWndParent)
{
   if (IsWindow(hWndParent) ==0) return NULL; // Elternfenster muß vorhanden sein !
   if ((!CCaraWalk_Global::pCreate) && (CCaraWalk_Global::BindDll())) return NULL;
   return CCaraWalk_Global::pCreate(this, strWndText, dwFlags, prcWnd, hWndParent);
}

bool CCaraWalk::DoDataExchange(DWORD dwType, int nSize, void *pParam)
{
   if ((!CCaraWalk_Global::pDoDataExchange) && (CCaraWalk_Global::BindDll())) return false;
   return CCaraWalk_Global::pDoDataExchange(this, dwType, nSize, pParam);
}

BOOL CCaraWalk::MoveWindow(int nLeft, int nTop, int nWidth, int nHeight, bool bRedraw)
{
   if ((!CCaraWalk_Global::pMoveWindow) && (CCaraWalk_Global::BindDll())) return false;
   return CCaraWalk_Global::pMoveWindow(this, nLeft, nTop, nWidth, nHeight, bRedraw);
}
