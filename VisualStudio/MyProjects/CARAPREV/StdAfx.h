// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//
#if !defined(AFX_STDAFX_H__30FA9E70_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_STDAFX_H__30FA9E70_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden

//#define _WIN32_IE 0x0501

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
/*
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE-Klassen
#include <afxodlgs.h>       // MFC OLE-Dialogfeldklassen
#include <afxdisp.h>        // MFC OLE-Automatisierungsklassen
#else
#define ETS_OLE_SERVER 1
#endif // _AFX_NO_OLE_SUPPORT
*/

//#include <CommCtrl.h>

#include <afxadv.h>        // MFC Memory-Files

#include <afxpriv.h>        // MFC-Help
#include <winspool.h>         // Unterstützung der Druckerfunktionen

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC-Datenbankklassen
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO-Datenbankklassen
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für gängige Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

#define _USE_MATH_DEFINES 1
#include <math.h>

// globale Typdefinitionen
typedef UINT (CALLBACK* COMMDLGPROC)(HWND, UINT, UINT, LONG);

//#define ETSDEBUG_REPORT 1

#ifdef ETS_OLE_SERVER
   #include <AFXOLE.H>
#endif

#include <float.h>
#include <mmsystem.h>

#define SAFE_DELETE(X) {if (X) {delete X; X = NULL;}}
// globale Variablen
/*
extern "C" __declspec(dllexport) void     *g_AfxCommDlgProc;
extern "C" __declspec(dllexport) HINSTANCE g_hCaraPreviewInstance;

extern "C" __declspec(dllexport) int       g_nVersion;
extern "C" __declspec(dllexport) COLORREF  g_CustomColors[16];

// globale Funktionen
extern "C" __declspec(dllexport) UINT CALLBACK CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
extern "C" __declspec(dllexport) UINT CALLBACK CFHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
*/

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_STDAFX_H__30FA9E70_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
