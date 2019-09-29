// AnalyseMapFile.h : Hauptheaderdatei für die AnalyseMapFile-Anwendung
//
#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"       // Hauptsymbole


// CAnalyseMapFileApp:
// Siehe AnalyseMapFile.cpp für die Implementierung dieser Klasse
//

#define ANALYSEMAPFILE_WND _T("AnalyseMapFileWnd")

#define WM_OPEN_FILE (WM_USER + 1)


class CMainFrame;

struct FindSymbols
{
	FindSymbols();
	void SetModule(CString sModule);

	DWORD64	dwAddress;
	DWORD	dwSize;
	DWORD	dwOffset;
	DWORD64 dwBaseAddress;
	DWORD	dwModuleSize;
	DWORD	dwModuleOffset;
	CString sModule;
	CString sPath;
	CString sName;
};

#define IsBetween(val, lowerval, upperval) ((val>=lowerval) && (val<= upperval))

#define FORCE_HEX     1
#define FORCE_DECIMAL 2

class CAnalyseMapFileApp : public CWinApp
{
public:
	CAnalyseMapFileApp();

// Attribute
	CMainFrame* GetMainFrame();

// Überschreibungen
public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual int ExitInstance();

// Implementierung
	void RegisterWindowClass();
	BOOL AddModule(FindSymbols &fs);
	BOOL InitializeSymbols();
	BOOL CleanUpSymbols();
	BOOL FindSymbolInModule(FindSymbols&fs);
	BOOL FindModule(FindSymbols&fs);
	DWORD64 StringToDWORD64(CString &sNumber, BOOL bForceType=0);
	CString DWORD64ToString(DWORD64 nNumber, BOOL bType=0);

	
// Messages
	//{{AFX_MSG(CAnalyseMapFileApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	static BOOL CALLBACK FindSymbolProc(PCSTR SymbolName, DWORD64 SymbolAddress, ULONG SymbolSize, PVOID UserContext);
	static BOOL CALLBACK EnumerateLoadedModulesProc64(PCSTR ModuleName, ULONG ModuleBase, ULONG ModuleSize, PVOID UserContext);

	CPtrList m_LoadedHandles;
};

extern CAnalyseMapFileApp theApp;
