// stdafx.h : Includedatei für Standardsystem-Includedateien,
// oder häufig verwendete, projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden
#endif


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// einige CString-Konstruktoren sind explizit

// Deaktiviert das Ausblenden von einigen häufigen und oft ignorierten Warnungen
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <afxdisp.h>        // MFC-Automatisierungsklassen

#include <afxdtctl.h>		// MFC-Unterstützung für allgemeine Steuerelemente von Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterstützung für allgemeine Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT

// videte header
#include <wk.h>
#include <WK_Msg.h>
#include <WK_names.h>

#include <wkclasses\WK_Trace.h>

struct ErrorStruct
{
	ErrorStruct(HRESULT hr, int nLine, const char*sFile) 
	{
		m_hr = hr;
		m_nLine = nLine;
		m_sFile = sFile;
	}
	int         m_nLine;
	HRESULT     m_hr;
	const char* m_sFile;
};

#define HRESULT_ERROR_AT_POS(hr) new ErrorStruct(hr, __LINE__, __FILE__)
#define HRESULT_EXCEPTION(hr) 	if (FAILED(hr)) throw HRESULT_ERROR_AT_POS(hr);

#define RELEASE_OBJECT(x) if (x) { x->Release(); x=NULL; }