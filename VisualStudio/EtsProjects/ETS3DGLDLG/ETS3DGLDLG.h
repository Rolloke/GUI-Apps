// ETS3DGLDLG.h : Haupt-Header-Datei für die Anwendung ETS3DGLDLG
//

#if !defined(AFX_ETS3DGLDLG_H__2C3EF282_8225_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_ETS3DGLDLG_H__2C3EF282_8225_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CETS3DGLDLGApp:
// Siehe ETS3DGLDLG.cpp für die Implementierung dieser Klasse
//
#define NOT_AVAILABLE 2

#define FOLDERPATH_CONCAT_SLASH 0x00000001
#define FOLDERPATH_NO_BROWSER   0x00000002
#define FOLDERPATH_RETURN_FILES 0x00000004

class CEtsCDPlayer;

#define DATE_SECTION "Dates\\No%d"
#define DATE_TIME    "Time"
#define DATE_DATE    "Date"
#define DATE_FLAG    "Flag"
#define DATE_MSG     "Msg"
#define DATE_PATH    "Path"

struct StructOfDates
{
	CTime   cDate;
	CTime   cTime;
	CString strMessage;
	CString strTimerPath;
   DWORD   dwFlags;
};

class CETS3DGLDLGApp : public CWinApp
{
public:
	CETS3DGLDLGApp();
	virtual ~CETS3DGLDLGApp();

   struct FolderCallBackStruct
   {
      char pszPath[MAX_PATH];
      UINT nFlags;
   };
	
   bool        SetProfileName(char *, char*);
	bool        GetFolderPath(CString &, ITEMIDLIST**, UINT);
	bool        CallSpecialDlg(char);
   static int  CALLBACK BrowseFolderCallback(HWND, UINT, LPARAM, LPARAM);

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CETS3DGLDLGApp)
	public:
	virtual BOOL InitInstance();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CETS3DGLDLGApp)
	afx_msg void OnAppExit();
	afx_msg void OnAppAbout();
	afx_msg void OnCallCaraviewSetup();
	afx_msg void OnCallCarawalksetup();
	afx_msg void OnCallDllPathsetup();
	afx_msg void OnCallGlsetup();
	afx_msg void OnSetDebugPath();
	afx_msg void OnSetReleaseEngPath();
	afx_msg void OnSetReleasePath();
	afx_msg void OnCallSaveProject();
	afx_msg void OnCallMaterialsetup();
	afx_msg void OnCallReportSetup();
	afx_msg void OnUpdateCancel(CCmdUI* pCmdUI);
	afx_msg void OnCallEtsmmplayer();
	afx_msg void OnCallTimeplan();
	afx_msg void OnCallEtsmmrecorder();
	afx_msg void OnCallEtsmixer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
   CPtrList m_ListDates;
private:

   CEtsCDPlayer *m_pCDPlayer;
   CEtsCDPlayer *m_pRecorder;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ETS3DGLDLG_H__2C3EF282_8225_11D3_B6EC_0000B458D891__INCLUDED_)
