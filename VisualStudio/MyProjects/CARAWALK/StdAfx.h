// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__INCLUDED_)
#define AFX_STDAFX_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Fügen Sie hier Ihre Header-Dateien ein
#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden
#define  STRICT

#include <windows.h>
//#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten

#include <stdlib.h>
#include <commctrl.h>         // Common Controls
#include <math.h>
#include <Commdlg.h>
// OpenGL Header Dateien
#include <GL/gl.h>            // Open-Gl Headerdateien
#include <GL/glu.h>

// Debug Info defines
#if defined _DEBUG
   #include <crtdbg.h>
   #define ASSERT(expr) \
        do { if (!(expr) && \
             (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, NULL))) \
             _CrtDbgBreak(); } while (0)

   #define VERIFY(f) ASSERT(f)
#else
   #define ASSERT(f)    ((void)(0))
   #define VERIFY(f)    ((void)(f))
#endif

#define M_PI 3.1415926535898

void* __cdecl operator new(size_t nSize);
void  __cdecl operator delete(void* p);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt zusätzliche Deklarationen unmittelbar vor der vorherigen Zeile ein.
//_573CA681_DDD9_11D2_9E4E_0000B458D891_

#endif // !defined(AFX_STDAFX_H__INCLUDED_)

