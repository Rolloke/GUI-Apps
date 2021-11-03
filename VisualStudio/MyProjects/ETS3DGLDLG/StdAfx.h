// stdafx.h : Include-Datei für Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die häufig benutzt, aber
//      in unregelmäßigen Abständen geändert werden.
//
 
#if !defined(AFX_STDAFX_H__2C3EF286_8225_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_STDAFX_H__2C3EF286_8225_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für gängige Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <process.h>

#include <shellapi.h>
#include <float.h>

BOOL DDX_TextVar(CDataExchange *, int, int, double &, bool bThrow=true);
BOOL DDX_TextVar(CDataExchange *, int, int, float  &, bool bThrow=true);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_STDAFX_H__2C3EF286_8225_11D3_B6EC_0000B458D891__INCLUDED_)
