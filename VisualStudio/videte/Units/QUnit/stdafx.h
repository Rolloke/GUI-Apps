// stdafx.h : Includedatei für Standardsystem-Includedateien,
// oder häufig verwendete, projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden
#endif

#define WINVER 0x0500
#define OEMRESOURCE

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

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// einige CString-Konstruktoren sind explizit

// Deaktiviert das Ausblenden von einigen häufigen und oft ignorierten Warnungen
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen

#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für allgemeine Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxmt.h>
#include <mmsystem.h>
#include <WK_Names.h>
#include <WK_Timer.h>
#include "wkclasses\WK_Profile.h"
#include "wkclasses\WK_RuntimeError.h"
#include "wkclasses\WK_Util.h"
#include "wkclasses\WK_File.h"
#include "wkclasses\WK_TRACE.H"
#include "WK_Msg.h"
#include <WKClasses\WK_PerfMon.h>
#include <WKClasses\WK_Utilities.h>
#include <WKClasses\WK_Utilities.h>
#include "CIPCStringDefs.h"

#define SETBIT(w,b)    ((__int64)((__int64)(w) | ((__int64)1L << (b))))
#define CLRBIT(w,b)    ((__int64)((__int64)(w) & ~((__int64)1L << (b))))
#define TSTBIT(w,b)    ((BOOL)((__int64)(w) & ((__int64)1L << (b)) ? TRUE : FALSE))

#include "Cap5BoardLibEx.h"
#include "Cap3BoardLib.h"
#include "CUDP.h"
#include "QProfile.h"
