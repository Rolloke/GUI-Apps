// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__1F601665_CFCA_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_STDAFX_H__1F601665_CFCA_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Fügen Sie hier Ihre Header-Dateien ein
#define WIN32_LEAN_AND_MEAN		// Selten benutzte Teile der Windows-Header nicht einbinden

#define  STRICT
#include <windows.h>
#include <math.h>
#include <stdlib.h>

#include <GL/gl.h>            // Open-Gl Headerdateien
#include <GL/glu.h>
#include <process.h>

//#include <dinput.h>           // Direct X
// ZU ERLEDIGEN: Verweisen Sie hier auf zusätzliche Header-Dateien, die Ihr Programm benötigt

#if defined _DEBUG
   #include <crtdbg.h>
   #define ASSERT(expr) \
        do { if (!(expr) && \
             (1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, NULL))) \
             _CrtDbgBreak(); } while (0)

   #define VERIFY(f) ASSERT(f)
   void  ReportLastWinError();
   #define REPORT_LAST_WIN_ERROR ReportLastWinError()
#else
   #define ASSERT(f)    ((void)(0))
   #define VERIFY(f)    ((void)(f))
   #define REPORT_LAST_WIN_ERROR ((void)(0))
#endif


#define DFI_RELEASE 0

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt zusätzliche Deklarationen unmittelbar vor der vorherigen Zeile ein.

#endif // !defined(AFX_STDAFX_H__1F601665_CFCA_11D2_9E4E_0000B458D891__INCLUDED_)
