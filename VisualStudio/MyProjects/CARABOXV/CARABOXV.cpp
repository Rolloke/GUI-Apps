/********************************************************************/
/*                                                                  */
/*                    DLL CARABOXView Dialog                        */
/*                                                                  */
/*     Zur Darstellung von Lautsprechern in 3D                      */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// CARABOXV.cpp : Definiert den Einsprungpunkt für die DLL-Anwendung.
//

#include "stdafx.h"
#include "CaraBoxViewDlg.h"



HINSTANCE   g_hInstance;                        // globaler Instance-Handle
CCaraWalk   g_CaraWalk;

extern "C"
{
   void  Destructor(void *);
   bool  Create(CCaraBoxViewDll *);
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
   switch (dwReason)
   {
      case DLL_PROCESS_ATTACH:                  // Einbinden der DLL in den Adressraum des Prozesses
         DisableThreadLibraryCalls(hInstance);
         g_hInstance = hInstance;               // Instance-Handle global speichern
         break;

      case DLL_PROCESS_DETACH:                  // Die DLL wird freigegeben
         break;
   }

   return TRUE;
}


bool Create(CCaraBoxViewDll *pCBVDll)
{
   if (pCBVDll)
   {
      return pCBVDll->Create();
   }

   return false;
}

void Destructor(void *p)
{
   g_CaraWalk.Destructor();
   CEtsDialog::Destructor();
}
