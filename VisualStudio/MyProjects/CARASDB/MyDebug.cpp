#include "MyDebug.h"

#if defined _DEBUG

#include <WINDOWS.H>
#include <stdio.h>

void _cdecl OutputDebug(LPCTSTR lpszFormat, ...)
{
   va_list args;
   va_start(args, lpszFormat);
   int nBuf;
   TCHAR szBuffer[1024];

   nBuf = vsprintf(szBuffer, lpszFormat, args);

   OutputDebugString(szBuffer);
   va_end(args);
}

/****************************************************************************************
* Name   : matherr                                                                      *
* Zweck  : Ausgabe einer Fehlermeldung eines Fehlers in der FPU mit einer MessageBox    *
* Aufruf : durch einen Interrupt, die Funktionsdefinition der Standard matherr-Funktion *
*          wird durch die neue Deklaration umdefiniert.                                 *
* Parameter :                                                                           *
* exc   (E) : Struktur mit Fehlerbeschreibung                      (struct exception *) *
****************************************************************************************/
int  _matherr(struct _exception *e)
{
   static char *text[] = 
   { 
      "Argument Domain error",
      "Argument singularity",
      "Overflow range error",
      "Partial loss of Significan",
      "Total loss of Significan",
      "Unknown Error"
   };
   int nError = 0;

   switch(e->type)
   {
      case _DOMAIN   : nError = 0; break;
      case _SING     : nError = 1; break;
      case _OVERFLOW : nError = 2; break;
      case _PLOSS    : nError = 3; break;
      case _TLOSS    : nError = 4; break;
      default        : nError = 5; break;
   }
   ASSERT(false);
   OutputDebug("Math-error occurred !\nFunction : %s\nArgument 1 : %10.3e\nArgument 2 : %10.3e\nRet. Value : %10.3e\n%s",
               e->name, e->arg1, e->arg2, e->retval, text[nError]);

   return 0;
}

#endif // _DEBUG

