// AnalyseMapFile.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "AnalyseMapFile.h"
#include "MainFrm.h"

#include "AnalyseMapFileDoc.h"
#include "LeftView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)

/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileApp
BEGIN_MESSAGE_MAP(CAnalyseMapFileApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Dateibasierte Standarddokumentbefehle
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	// Standarddruckbefehl "Seite einrichten"
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
FindSymbols::FindSymbols()
{
	dwAddress = 0;
	dwSize = 0;
	dwBaseAddress = 0;
	dwOffset = 0;
	dwModuleSize = 0;
	dwModuleOffset = 0;
}
/////////////////////////////////////////////////////////////////////////////
void FindSymbols::SetModule(CString sM)
{
	sModule = sM;
	int nFind = sModule.ReverseFind(_T('\\'));
	if (nFind != -1)
	{
		sPath   = sModule.Left(nFind+1);
		sModule = sModule.Mid(nFind+1);
	}

}
/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileApp-Erstellung
CAnalyseMapFileApp::CAnalyseMapFileApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CAnalyseMapFileApp-Objekt
CAnalyseMapFileApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileApp Initialisierung
void CAnalyseMapFileApp::RegisterWindowClass()
{
	WNDCLASS  wndclass ;

	// register window class
	wndclass.style =         0;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor =       LoadCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = ANALYSEMAPFILE_WND;

	// main app window
	AfxRegisterClass(&wndclass);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAnalyseMapFileApp::InitInstance()
{
	HWND hWndFirstInstance = FindWindow(ANALYSEMAPFILE_WND, NULL);
	if (hWndFirstInstance)
	{
		SetWindowPos(hWndFirstInstance,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		SetWindowPos(hWndFirstInstance,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		if (m_lpCmdLine[0] != 0)
		{
			SendMessage(hWndFirstInstance, WM_SETTEXT, 0, (LPARAM)m_lpCmdLine);
			PostMessage(hWndFirstInstance, WM_OPEN_FILE, 0, 0);
		}
		return FALSE;
	}

	RegisterWindowClass();

	InitCommonControls();

	CWinApp::InitInstance();
/*
	// OLE-Bibliotheken initialisieren
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
*/
	// Standardinitialisierung
	// Wenn Sie diese Features nicht verwenden und die Größe
	// der ausführbaren Datei verringern möchten, entfernen Sie
	// die nicht erforderlichen Initialisierungsroutinen.
	// Ändern Sie den Registrierungsschlüssel unter dem Ihre Einstellungen gespeichert sind.
	// TODO: Ändern Sie diese Zeichenfolge entsprechend,
	// z.B. zum Namen Ihrer Firma oder Organisation.
	SetRegistryKey(m_pszAppName);
	LoadStdProfileSettings(4);  // Standard INI-Dateioptionen laden (einschließlich MRU)
	// Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
	//  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAnalyseMapFileDoc),
		RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
		RUNTIME_CLASS(CLeftView));

	if (!pDocTemplate)
	{
		return FALSE;
	}

	AddDocTemplate(pDocTemplate);
	// DDE-Execute-Open aktivieren
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// Befehlszeile parsen, um zu prüfen auf Standardumgebungsbefehle DDE, Datei offen
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Verteilung der in der Befehlszeile angegebenen Befehle. Es wird FALSE zurückgegeben, wenn
	// die Anwendung mit /RegServer, /Register, /Unregserver oder /Unregister gestartet wurde.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// Das einzige Fenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// Rufen Sie DragAcceptFiles nur auf, wenn eine Suffix vorhanden ist.
	//  In einer SDI-Anwendung ist dies nach ProcessShellCommand erforderlich
	// Öffnen mit Drag  Drop aktivieren
	m_pMainWnd->DragAcceptFiles();

	InitializeSymbols();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAnalyseMapFileApp::InitializeSymbols()
{
	CString sSymbolPath;
	TCHAR szPath[_MAX_PATH];

   // Creating the default path
   // ".;%_NT_SYMBOL_PATH%;%_NT_ALTERNATE_SYMBOL_PATH%;%SYSTEMROOT%;%SYSTEMROOT%\System32;"
	sSymbolPath = _T(".");

	// environment variable _NT_SYMBOL_PATH
	if ( GetEnvironmentVariable(_T("_NT_SYMBOL_PATH"), szPath, _MAX_PATH))
	{
		sSymbolPath += _T(";");
		sSymbolPath += szPath;
	}

	// environment variable _NT_ALTERNATE_SYMBOL_PATH
	if ( GetEnvironmentVariable(_T("_NT_ALTERNATE_SYMBOL_PATH"), szPath,  _MAX_PATH) )
	{
		sSymbolPath += _T(";");
		sSymbolPath += szPath;
	}

	// environment variable SYSTEMROOT
	if ( GetEnvironmentVariable(_T("SYSTEMROOT"), szPath,  _MAX_PATH) )
	{
		sSymbolPath += _T(";");
		sSymbolPath += szPath;
		sSymbolPath += _T(";");
		sSymbolPath += szPath;
		sSymbolPath += _T("\\System32");
	}

	LPCTSTR str = sSymbolPath;
	DWORD    symOptions = SymGetOptions();
	symOptions |= SYMOPT_LOAD_LINES; 
	symOptions &= ~SYMOPT_UNDNAME;
	SymSetOptions( symOptions );

	BOOL bRet = SymInitialize(GetCurrentProcess(), (PSTR)str, TRUE);
	if (bRet == 0)
	{
		TRACE(_T("LastError:%d\n"), GetLastError());
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAnalyseMapFileApp::CleanUpSymbols()
{
	BOOL bRet = SymCleanup(GetCurrentProcess());
	if (bRet == 0)
	{
		TRACE(_T("LastError:%d\n"), GetLastError());
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAnalyseMapFileApp::AddModule(FindSymbols &fs)
{
	HINSTANCE hInst = LoadLibrary(fs.sPath + fs.sModule);
	if (hInst)
	{
		m_LoadedHandles.AddTail((void*)hInst);
		CleanUpSymbols();
		InitializeSymbols();
		return FindModule(fs);
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAnalyseMapFileApp::FindModule(FindSymbols&fs)
{
	return EnumerateLoadedModules(GetCurrentProcess(), EnumerateLoadedModulesProc64, (void*)&fs);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAnalyseMapFileApp::FindSymbolInModule(FindSymbols&fs)
{
	BOOL bResult = FindModule(fs);
	if (fs.dwModuleSize == 0 && !fs.sModule.IsEmpty())
	{
		AddModule(fs);
	}
	if (fs.dwModuleSize)
	{
		if (fs.dwAddress == 0 && fs.dwOffset != 0 && fs.dwOffset != 0xffffffff)
		{
			fs.dwAddress = fs.dwBaseAddress + fs.dwOffset;
		}
		SymEnumerateSymbols64(GetCurrentProcess(), fs.dwBaseAddress, FindSymbolProc, (void*)&fs);
		if (fs.dwSize)
		{
			fs.dwOffset = (DWORD)(fs.dwAddress - fs.dwBaseAddress);
			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
		}
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CAnalyseMapFileApp::EnumerateLoadedModulesProc64(PCSTR ModuleName, ULONG ModuleBase, ULONG ModuleSize, PVOID UserContext)
{
	FindSymbols*pfs = (FindSymbols*) UserContext;
	CString fModuleName(ModuleName);
	if (	(!pfs->sModule.IsEmpty() && pfs->sModule.CompareNoCase(fModuleName) == 0)
		||	(IsBetween(pfs->dwAddress, ModuleBase, (ModuleBase+ModuleSize-1)))
		||	(pfs->dwOffset == 0xffffffff))
	{
		pfs->dwBaseAddress = ModuleBase;
		pfs->dwModuleSize  = ModuleSize;
		if (pfs->sModule.IsEmpty())
		{
			pfs->sModule = ModuleName;
		}
		return 0;
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CAnalyseMapFileApp::FindSymbolProc(PCSTR SymbolName, DWORD64 SymbolAddress, ULONG SymbolSize, PVOID UserContext)
{
	FindSymbols*pfs = (FindSymbols*) UserContext;
	CString fSymbolName(SymbolName);
	if (pfs->dwOffset == 0xffffffff)
	{
		DWORD *pdwAddress= (DWORD*)&pfs->dwAddress;
		if (pdwAddress[0]++ == pdwAddress[1])
		{
			pfs->sName = fSymbolName;
			pfs->dwSize = SymbolSize;
			pfs->dwAddress = SymbolAddress;
			return 0;
		}
	}
	else if (	(pfs->dwAddress && IsBetween(pfs->dwAddress, SymbolAddress, (SymbolAddress+SymbolSize-1)))
			 ||	(pfs->sName.CompareNoCase(fSymbolName) == 0))
	{
		pfs->sName = fSymbolName;
		pfs->dwSize = SymbolSize;
		pfs->dwAddress = SymbolAddress;
		return 0;
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
CMainFrame* CAnalyseMapFileApp::GetMainFrame()
{
	ASSERT(m_pMainWnd != NULL);
	ASSERT_KINDOF(CMainFrame, m_pMainWnd);
	return (CMainFrame*)m_pMainWnd;
}
/////////////////////////////////////////////////////////////////////////////
DWORD64 CAnalyseMapFileApp::StringToDWORD64(CString &sNumber, BOOL bForceType/*=0*/)
{
	DWORD64 nNumber = 0;
	DWORD  *pnNumber = (DWORD*)&nNumber;
	BOOL bDecimal = FALSE, bHex = FALSE;
	switch (bForceType)
	{
		case FORCE_HEX: bHex = TRUE; break;
		case FORCE_DECIMAL: bDecimal = TRUE; break;
	}
	int nFind = sNumber.Find(_T("0d"));
	if (nFind == 0)
	{
		sNumber = sNumber.Mid(2);
		bDecimal = TRUE;
		bHex = FALSE;
	}

	nFind = sNumber.Find(_T("0x"));
	if (nFind == 0)
	{
		sNumber = sNumber.Mid(2);
		bHex = TRUE;
	}

	nFind = sNumber.FindOneOf(_T("abcdef"));
	if (nFind != -1)
	{
		bHex = TRUE;
	}
	else if (!bHex)
	{
		bDecimal = TRUE;
	}

	if (bHex)
	{
		nFind = sNumber.GetLength();
		if (nFind > 8)
		{
			_stscanf_s(sNumber.Mid(nFind-8), _T("%x"), &pnNumber[0]);
			_stscanf_s(sNumber.Left(nFind-8), _T("%x"), &pnNumber[1]);
		}
		else
		{
			_stscanf_s(sNumber, _T("%lx"), &nNumber);
		}
	}
	if (bDecimal)
	{
		_stscanf_s(sNumber, _T("%lu"), &nNumber);
	}
	return nNumber;
}
/////////////////////////////////////////////////////////////////////////////
CString CAnalyseMapFileApp::DWORD64ToString(DWORD64 nNumber, BOOL bType)
{
	CString sNumber;
	if (bType == FORCE_DECIMAL)
	{
		sNumber.Format(_T("0d%lu"), nNumber);
	}
	else
	{
		sNumber.Format(_T("0x%08x"), nNumber);
		if (nNumber > 0xffffffff)
		{
			CString sHighNumber;
			DWORD  *pnNumber = (DWORD*)&nNumber;
			sHighNumber.Format(_T("0x%08x"), pnNumber[1]);
			sNumber = sHighNumber + sNumber.Mid(2);
		}
	}
	return sNumber;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl 'Info'
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}
/////////////////////////////////////////////////////////////////////////////
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Anwendungsbefehl zum Ausführen des Dialogfelds
void CAnalyseMapFileApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
/////////////////////////////////////////////////////////////////////////////
// CAnalyseMapFileApp Meldungshandler
LONG WINAPI UEFilter(_EXCEPTION_POINTERS* ExceptionInfo)
{
	CStdioFile file(_T("Exception.Log"), CFile::modeWrite|CFile::modeCreate);
	CString str;
	str.Format(_T("Adr:%x, Code: %x"), ExceptionInfo->ExceptionRecord->ExceptionAddress, ExceptionInfo->ExceptionRecord->ExceptionCode);
	file.WriteString(str);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int CAnalyseMapFileApp::Run()
{
	int nResult;
#ifdef _DEBUG
	LPTOP_LEVEL_EXCEPTION_FILTER pTLEH = SetUnhandledExceptionFilter(UEFilter);
	nResult = CWinApp::Run();
	SetUnhandledExceptionFilter(pTLEH);
#else
	nResult = CWinApp::Run();
#endif
	return nResult;
}
/////////////////////////////////////////////////////////////////////////////
int CAnalyseMapFileApp::ExitInstance()
{
	CleanUpSymbols();
	while (m_LoadedHandles.GetCount())
	{
		FreeLibrary((HMODULE)m_LoadedHandles.RemoveHead());
	}

	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
class CMyExDocmanager : public CDocManager
{
public:
	void RemoveDocTemplate(CDocTemplate*pdt)
	{
		POSITION pos = NULL;
		pos = m_templateList.Find(pdt);
		if (pos != NULL)
		{
			m_templateList.RemoveAt(pos);
		}
	}
};
/////////////////////////////////////////////////////////////////////////////
void CAnalyseMapFileApp::OnFileOpen() 
{
	ASSERT(m_pDocManager != NULL);

	CSingleDocTemplate dt2(IDR_MAINFRAME2, NULL, NULL, NULL);
	m_pDocManager->AddDocTemplate(&dt2);

	CString newName;
	BOOL bReturn = m_pDocManager->DoPromptFileName(newName, AFX_IDS_OPENFILE, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, TRUE, NULL);

	((CMyExDocmanager*)m_pDocManager)->RemoveDocTemplate(&dt2);

	if (bReturn)
	{
		OpenDocumentFile(newName);
	}
}
/////////////////////////////////////////////////////////////////////////////
