// Globale Makros, Variablen, Strukturen und Prototypen für DLL CARABOX
//*********************************************************************
// 25.11.99, Tho

#ifndef __BOX_GLOBAL_H
#define __BOX_GLOBAL_H


#define  STRICT
#include <WINDOWS.H>

//#ifdef _DEBUG
//  #define _DEBUG_HEAP
//#endif

#ifdef _DEBUG_HEAP
   #ifdef DLL_DEBUG_NEW
     #undef DLL_DEBUG_NEW
   #endif
   #ifdef new
     #undef new
   #endif
   #ifdef Alloc
      #undef Alloc
   #endif
   bool HeapWatch();
   void* __cdecl Alloc(size_t nSize, void* pOld, char *pLine, int nLine);
   void* __cdecl operator new(size_t nSize, char *pFile, int nLine);
   #define DLL_DEBUG_NEW  new(__FILE__, __LINE__)
   #define DEBUG_ID_STRING "ETS"
#else
   void* __cdecl Alloc(size_t nSize, void *pOld);
   void* __cdecl operator new(size_t nSize);
#endif

void  __cdecl Free(void* p);
void  __cdecl operator delete(void* p);

bool IsBetween(double dMin, double dVal, double dMax);

void Se_TranslatorFnc(unsigned int , _EXCEPTION_POINTERS*);

#endif  // __BOX_GLOBAL_H
