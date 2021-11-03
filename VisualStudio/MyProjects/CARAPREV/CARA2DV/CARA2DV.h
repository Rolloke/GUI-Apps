// CARA2DV.h : Haupt-Header-Datei für die Anwendung CARA2DV
//

#if !defined(AFX_CARA2DV_H__30FA9EA5_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_CARA2DV_H__30FA9EA5_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole
#include "CEtsDiv.h"
#include "..\CaraWinApp.h"
#include "..\Curve.h"

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVApp:
// Siehe CARA2DV.cpp für die Implementierung dieser Klasse
//
#define REGKEY_DEFAULT_PLOT      "Plot"
#define REGKEY_CURVE_TEXT_MACROS "CurveTextMacros"
#define REGKEY_DEFAULT_CURVEDIR  "CurveDir"
#define REGKEY_CALCULATOR_PATH   "Calculator"

#define CURVE_EXTENSION          "CRV"
#define CURVE_EXTENSION_2        "CVE"
#define TARGET_FUNCTION_FIXED      9998
#define TARGET_FUNCTION_CHANGEABLE 9999

#define WM_USER_OPT_UPDATE  (WM_APP + 17)

// wParam = Länge des Speichers für die Zeichenkette mit dem Dateinamen
// lParam = HGLOBAL mit Zeichenkette

class CCARA2DVApp : public CCaraWinApp
{

public:
   CCARA2DVApp();
   virtual ~CCARA2DVApp();

   // Speichern und Laden der Default- und Standardparameter
   void        SaveDefaultSettings();
   void        LoadDefaultSettings();

   // Überladungen
   //{{AFX_VIRTUAL(CCARA2DVApp)
	public:
   virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementierung

   //{{AFX_MSG(CCARA2DVApp)
   afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
   afx_msg void OnUpdateFileFunc(CCmdUI* pCmdUI);
   afx_msg void OnHeapwatch();
	afx_msg void OnFileOpen();
	afx_msg void OnViewCalculator();
	afx_msg void OnUpdateViewCalculator(CCmdUI* pCmdUI);
	afx_msg void OnViewCalcForeground();
	afx_msg void OnUpdateViewCalcForeground(CCmdUI* pCmdUI);
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
	void TestCalculator();
   bool              m_bEditMode;
   bool              m_bEditable;
   CString           m_strEditPathName;
   CString           m_strDefaultCurveDir;
   CString           m_strCalculatorPath;

#ifdef ETS_OLE_SERVER
	COleTemplateServer m_server;
#endif

private:
   CEtsDiv m_2DVDiv;
   PROCESS_INFORMATION m_PiCalculator;
   static BOOL CALLBACK EnumWindowsOfCalc(HWND, LPARAM);
protected:
	void OnRegister();
};

extern CCARA2DVApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_CARA2DV_H__30FA9EA5_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
