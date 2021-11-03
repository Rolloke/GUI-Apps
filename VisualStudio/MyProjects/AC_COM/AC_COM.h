// AC_COM.h : Haupt-Header-Datei für die Anwendung AC_COM
//

#if !defined(AFX_AC_COM_H__37BC0AE2_C14D_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_AC_COM_H__37BC0AE2_C14D_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

#include "AudioControlData.h"

/////////////////////////////////////////////////////////////////////////////
// CAC_COMApp:
// Siehe AC_COM.cpp für die Implementierung dieser Klasse
//

#define FOLDERPATH_CONCAT_SLASH 0x00000001
#define FOLDERPATH_NO_BROWSER   0x00000002
#define FOLDERPATH_RETURN_FILES 0x00000004

#define MBF_REQU_SENDDATA       0x00000001
#define MBF_AUTO_SENDDATA       0x00000002
#define MBF_REQU_SAVEDATA       0x00000010
#define MBF_AUTO_SAVEDATA       0x00000020
#define MBF_REQU_INSERTDATA     0x00000100
#define MBF_AUTO_INSERTDATA     0x00000200
#define MBF_REQU_CHANGEPORT     0x00001000
#define MBF_AUTO_CHANGEPORT     0x00002000

#define MBF_WARN_DISCONECT      0x00010000
#define MBF_WARN_NO_CURVESEL    0x00100000
#define MBF_WARN_TRACK9         0x01000000
#define MBF_WARN_TRACK10        0x10000000

#define MBF_WARN_REQU_ALL  (MBF_WARN_TRACK10|MBF_WARN_TRACK9|MBF_WARN_NO_CURVESEL|MBF_WARN_DISCONECT|MBF_REQU_CHANGEPORT|MBF_REQU_INSERTDATA|MBF_REQU_SAVEDATA|MBF_REQU_SENDDATA)


class CAC_COMApp : public CWinApp
{
public:

	CAC_COMApp();
	virtual ~CAC_COMApp();
	void TestDirectory();
	bool GetFolderPath(CString &, ITEMIDLIST**, UINT, LPCTSTR);
	bool DoFileDialog(bool, bool, int, CString&);
   virtual LRESULT ProcessWndProcException(CException*, const MSG*);
	static int GetColorDiff(DWORD, DWORD);

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAC_COMApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CAC_COMApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   public:
	CCommandLineInfo m_cmdInfo;

   static CString gm_strStartDirectory;
   static CString gm_strCARAPath;
   static CString gm_strEditFileName;
   static bool    gm_bSpecialistMode;
   static bool    gm_bEditCurves;
   static bool    gm_bClosePortImediate;
   static int     gm_nCARAVersion;
   static bool    gm_bDoReport;
   static DWORD   gm_dwMessageBoxFlags;
   static bool    gm_bWindowsXP;

   private:
   HACCEL m_hAcc;
   static int CALLBACK BrowseFolderCallback(HWND, UINT, LPARAM, LPARAM);
   struct FolderCallBackStruct
   {
      char pszPath[MAX_PATH];
      UINT nFlags;
   };
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_AC_COM_H__37BC0AE2_C14D_11D4_B6EC_0000B458D891__INCLUDED_)
