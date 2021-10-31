#ifndef _DEBUG_H_INCLUDED
   #define _DEBUG_H_INCLUDED

#ifdef _DEBUG
   #include <crtdbg.h>
   void _cdecl OutputDebug(const char*, ...);
#endif //_DEBUG

#ifndef ASSERT
   #ifdef _DEBUG
      #define ASSERT(expr) \
           do { if (!(expr) && \
                (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, NULL))) \
                _CrtDbgBreak(); } while (0)
      #define VERIFY(f) ASSERT(f)
   #else
      #define ASSERT(f)    ((void)(0))
      #define VERIFY(f)    ((void)(f))
   #endif//_DEBUG
#endif

#ifndef TRACE
   #ifdef _DEBUG
      #define TRACE OutputDebug
   #else
      #define TRACE /##/
   #endif //_DEBUG
#endif // TRACE

#endif //_DEBUG_H_INCLUDED