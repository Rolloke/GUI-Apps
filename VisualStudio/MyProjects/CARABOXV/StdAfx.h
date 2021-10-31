// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__AC4576E2_6F40_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_STDAFX_H__AC4576E2_6F40_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Fügen Sie hier Ihre Header-Dateien ein
#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

#include <windows.h>
#include <process.h>

#if defined _DEBUG
   #include <crtdbg.h>
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

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt zusätzliche Deklarationen unmittelbar vor der vorherigen Zeile ein.

#endif // !defined(AFX_STDAFX_H__AC4576E2_6F40_11D3_B6EC_0000B458D891__INCLUDED_)
