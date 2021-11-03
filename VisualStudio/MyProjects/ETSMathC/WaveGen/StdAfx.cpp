// stdafx.cpp : Quelltextdatei, die nur die Standard-Includes einbindet
//	WaveGen.pch ist die vorcompilierte Header-Datei
//	stdafx.obj enthält die vorcompilierte Typinformation

#include "stdafx.h"
#include "custerr.h"

int _matherr(struct _exception *e)
{
   int nError = MATHERR_UNKNOWN;
   switch(e->type)
   {
      case _DOMAIN   : nError = MATHERR_DOMAIN;   TRACE("Argument Domain error\n");      break;
      case _SING     : nError = MATHERR_SING;     TRACE("Argument singularity\n");       break;
      case _OVERFLOW : nError = MATHERR_OVERFLOW; TRACE("Overflow range error\n");       break;
      case _PLOSS    : nError = MATHERR_PLOSS;    TRACE("Partial loss of Significan\n"); break;
      case _TLOSS    : nError = MATHERR_TLOSS;    TRACE("Total loss of Significan\n");   break;
      default        :                            TRACE("Unknown Error\n");              break;
   }
   SetLastError(nError);
   return 0;
}


