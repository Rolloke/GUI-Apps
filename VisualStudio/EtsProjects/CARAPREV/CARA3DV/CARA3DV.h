// CARA3DV.h : Haupt-Header-Datei für die Anwendung CARA3DV
//

#if !defined(AFX_CARA3DV_H__30FA9E78_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_CARA3DV_H__30FA9E78_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole
#include "CEtsDiv.h"
#include "Ets3DGL.h"	// Hinzugefügt von der Klassenansicht
#include "..\CaraWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// CCARA3DVApp:
// Siehe CARA3DV.cpp für die Implementierung dieser Klasse
//

class CCARA3DVApp : public CCaraWinApp
{
public:
   CCARA3DVApp();
   virtual ~CCARA3DVApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CCARA3DVApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CCARA3DVApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

#ifdef ETS_OLE_SERVER
	COleTemplateServer m_server;
#endif

private:
//   CEtsPrtD m_3DPrtD;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_CARA3DV_H__30FA9E78_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
