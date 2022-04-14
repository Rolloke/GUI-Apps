#if !defined __MY_DEBUG_H_INCLUDED_
#define __MY_DEBUG_H_INCLUDED_

#if defined _DEBUG

   #ifndef _WINDOWS_
      #define  STRICT
      #include <WINDOWS.H>
   #endif
   #include <crtdbg.h>
   #include <math.h>

   void _cdecl OutputDebug(LPCTSTR lpszFormat, ...);
   int  _matherr(struct _exception *e);

   #define TRACE  OutputDebug
   #define ASSERT(expr) \
        do { if (!(expr) && \
             (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, NULL))) \
             _CrtDbgBreak(); } while (0)

   #define VERIFY(f) ASSERT(f)
#else

   #define TRACE        ((void)(0))
   #define ASSERT(f)    ((void)(0))
   #define VERIFY(f)    ((void)(f))

#endif

#endif // __MY_DEBUG_H_INCLUDED_