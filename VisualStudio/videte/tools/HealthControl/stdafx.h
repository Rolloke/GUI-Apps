// stdafx.h : Includedatei für Standardsystem-Includedateien,
// oder häufig verwendete, projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden
#endif

/*
// Ändern Sie folgende Definitionen für Plattformen, die älter als die unten angegebenen sind.
// Unter MSDN finden Sie die neuesten Informationen über die entsprechenden Werte für die unterschiedlichen Plattformen.
#ifndef WINVER				// Lassen Sie die Verwendung von Features spezifisch für Windows 95 und Windows NT 4 oder später zu.
#define WINVER 0x0400		// Ändern Sie den entsprechenden Wert, um auf Windows 98 und mindestens Windows 2000 abzuzielen.
#endif

#ifndef _WIN32_WINNT		// Lassen Sie die Verwendung von Features spezifisch für Windows NT 4 oder später zu.
#define _WIN32_WINNT 0x0400		// Ändern Sie den entsprechenden Wert, um auf Windows 98 und mindestens Windows 2000 abzuzielen.
#endif						

#ifndef _WIN32_WINDOWS		// Lassen Sie die Verwendung von Features spezifisch für Windows 98 oder später zu.
#define _WIN32_WINDOWS 0x0410 // Ändern Sie den entsprechenden Wert, um auf mindestens Windows Me abzuzielen.
#endif

#ifndef _WIN32_IE			// Lassen Sie die Verwendung von Features spezifisch für IE 4.0 oder später zu.
#define _WIN32_IE 0x0400	// Ändern Sie den entsprechenden Wert, um auf mindestens IE 5.0 abzuzielen.
#endif
*/
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// einige CString-Konstruktoren sind explizit

// Deaktiviert das Ausblenden von einigen häufigen und oft ignorierten Warnungen
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <afxdisp.h>        // MFC-Automatisierungsklassen
#include <afxmt.h>

#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für allgemeine Windows-Steuerelemente

// #define WK_AFX_EXT_CLASS AFX_EXT_CLASS

#define WKCLASSES 0

#if WKCLASSES == 1

 #include <wkclasses\wk_trace.h>
 #include <wkclasses\wk_util.h>
 #include <wkclasses\wk_utilities.h>
 #include <wkclasses\wk_profile.h>
 #include <wkclasses\wk_RunTimeError.h>
 #include <wkclasses\WK_Utilities.h>

#else

 #define WK_DELETE(ptr)		{if(ptr){delete ptr; ptr=NULL;}}
 #define WK_TRACE          TRACE
 #define WK_TRACE_ERROR    TRACE
 #define DEVICE_UNKNOWN    0

 #define setbit(a,b,c) ((a) = ((c) ? (a)|(1<<(b)) : (a)&(~(1<<(b)))))
 #define getbit(a,b)   ((a) & (1<<(b))) 
 #define invbit(a,b)   ((a) = ((a) ^ (1<<(b))))

 #define IsBetween(val, lowerval, upperval) ((val>=lowerval) && (val<= upperval))
 #define IsInArray(val, upperval) ((val>=0) && (val<upperval))

 #define ZERO_INIT(X) ZeroMemory(&X, sizeof(X))

#endif

 
#define OEMGUI 0
#if OEMGUI == 1
   #include <oemgui\oemguiapi.h>
#endif

#include <WK_Msg.h>
#include <WK_Names.h>


#endif // _AFX_NO_AFXCMN_SUPPORT

