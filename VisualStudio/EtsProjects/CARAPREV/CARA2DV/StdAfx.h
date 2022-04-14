// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//

#if !defined(AFX_STDAFX_H__30FA9EA7_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_STDAFX_H__30FA9EA7_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
//#include <afxdisp.h>        // MFC OLE-Automatisierungsklassen
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>		// MFC-Unterstützung für gängige Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

#define _USE_MATH_DEFINES 1
#include <math.h>
#include <float.h>

#include "CustErr.h"

#define WM_NEWCURVE        (WM_USER+1)
#define WM_ENDTHREAD       (WM_USER+2)
#define WM_RESETPICKSTATES (WM_USER+3)

#ifdef _DEBUG
   #define ASSERT_LAST_ERROR(B) if (!B) {TRACE("Last Error No.:%x\n", GetLastError()); ASSERT(false);}
#else
   #define ASSERT_LAST_ERROR
#endif

//#define ETSDEBUG_REPORT 1

#ifdef ETS_OLE_SERVER
   #include <AFXOLE.H>
#endif

//#define _REGISTER_BUILD 1


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_STDAFX_H__30FA9EA7_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
