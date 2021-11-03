// WaveGen.h : Haupt-Header-Datei für die Anwendung WAVEGEN
//

#if !defined(AFX_WAVEGEN_H__349819B6_B864_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_WAVEGEN_H__349819B6_B864_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "CaraWinApp.h"
#include "resource.h"       // Hauptsymbole
#include "ProgressDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CWaveGenApp:
// Siehe WaveGen.cpp für die Implementierung dieser Klasse
//
class CWaveDocManager : public CDocManager
{
public:
   virtual BOOL DoPromptFileName(CString& , UINT , DWORD , BOOL , CDocTemplate* );
   void RemoveDocTemplate(CDocTemplate*pdt);
};

class CWaveDocTemplate : public CDocTemplate
{
public:
   void SetResourceID(int);
};

class CWaveGenApp : public CCaraWinApp
{
public:
	CWaveGenApp();
	void ProgressDlgSetRange(int);
	void DestroyProgressDlg();
	void ProgressDlgStep();
	void CreateProgressDlg(char*);

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CWaveGenApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

   afx_msg void OnFileOpen();

// Implementierung
	//{{AFX_MSG(CWaveGenApp)
	afx_msg void OnAppAbout();
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
   CProgressDlg m_ProgressDlg;
};


extern CWaveGenApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_WAVEGEN_H__349819B6_B864_11D3_B6EC_0000B458D891__INCLUDED_)
