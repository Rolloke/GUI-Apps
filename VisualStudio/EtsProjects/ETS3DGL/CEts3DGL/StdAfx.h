// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__1F60166B_CFCA_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_STDAFX_H__1F60166B_CFCA_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

#define  STRICT
#include <windows.h>

#if defined _DEBUG
   #include <crtdbg.h>
   void _cdecl OutputDebug(LPCTSTR lpszFormat, ...);
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

// ZU ERLEDIGEN: Verweisen Sie hier auf zusätzliche Header-Dateien, die Ihr Programm benötigt

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt zusätzliche Deklarationen unmittelbar vor der vorherigen Zeile ein.

#endif // !defined(AFX_STDAFX_H__1F60166B_CFCA_11D2_9E4E_0000B458D891__INCLUDED_)
