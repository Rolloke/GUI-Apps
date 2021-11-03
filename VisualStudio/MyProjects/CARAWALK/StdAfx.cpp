// stdafx.cpp : Quelltextdatei, die nur die Standard-Includes einbindet
//	CARAWALK.pch ist die vorkompilierte Header-Datei
//	stdafx.obj enthält die vorkompilierte Typinformation

#include "stdafx.h"
#include "CaraWalkDll.h"

void* __cdecl operator new(size_t nSize)
{
   return CCaraWalkDll::Alloc(nSize);
}

void  __cdecl operator delete(void* p)
{
  CCaraWalkDll:: Free(p);
}
