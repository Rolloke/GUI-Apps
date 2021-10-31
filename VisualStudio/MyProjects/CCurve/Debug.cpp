#include "Debug.h"

#ifdef _DEBUG

#ifndef _WINDOWS_
   #define  STRICT
   #include <WINDOWS.H>
#endif

#include <stdio.h>
#include <stdarg.h>

   void _cdecl OutputDebug(const char* lpszFormat, ...)
   {
      va_list args;
      va_start(args, lpszFormat);
      int nBuf;
      TCHAR szBuffer[1024];

      nBuf = vsprintf(szBuffer, lpszFormat, args);

      OutputDebugString(szBuffer);
      va_end(args);
   }
#endif //_DEBUG