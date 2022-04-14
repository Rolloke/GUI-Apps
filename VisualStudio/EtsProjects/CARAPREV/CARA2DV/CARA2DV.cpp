/********************************************************************/
/*                                                                  */
/*                     Programm ETS2DV                              */
/*                                                                  */
/*     Darstellung und Bearbeitung von 2D Diagrammen                */
/*     X/Y-Diagramme, Polardiagramme, Ortskurven                    */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// CARA2DV.cpp : Legt das Klassenverhalten für die Anwendung fest.
//
#include "stdafx.h"
#include "CARA2DV.h"

#include "MainFrm.h"
#include "CARA2DVDoc.h"
#include "CARA2DVView.h"

#include "CCaraInfo.h"
#include "CEtsHelp.h"
#include "ETS3DGLRegKeys.h"

#include "RegisterDlg.h"

#include "..\Resource.h"


#ifdef ETS_OLE_SERVER
   #include "IpFrame.h"
   #include <AFXSOCK.H>
#endif

extern "C" __declspec(dllimport) bool g_bDoReport;
extern "C" __declspec(dllimport) char g_szAppName[16];
extern "C" __declspec(dllimport) int  g_nFileVersion;

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
 #ifdef _DEBUG
  #define  ETSDEBUG_CREATEDATA "ETS2DV", &g_bDoReport
 #else
  #define  ETSDEBUG_CREATEDATA g_szAppName, &g_bDoReport
 #endif
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
//#define _REGISTER_BUILD 1
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#else
#undef _REGISTER_BUILD
#endif

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVApp

BEGIN_MESSAGE_MAP(CCARA2DVApp, CCaraWinApp)
	//{{AFX_MSG_MAP(CCARA2DVApp)
	ON_COMMAND(IDX_HEAPWATCH, OnHeapwatch)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_VIEW_CALCULATOR, OnViewCalculator)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CALCULATOR, OnUpdateViewCalculator)
	ON_COMMAND(ID_VIEW_CALC_FOREGROUND, OnViewCalcForeground)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CALC_FOREGROUND, OnUpdateViewCalcForeground)
	ON_COMMAND(ID_APP_ABOUT, OnRegister)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	ON_COMMAND((ID_FILE_PRINT+ID_TB_RIGHT_EXT), OnFilePrintSetup)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileFunc)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileFunc)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileFunc)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileFunc)
	//}}AFX_MSG_MAP
	// Dateibasierte Standard-Dokumentbefehle
END_MESSAGE_MAP()

#ifdef ETS_OLE_SERVER
   // Dieser Bezeichner wurde als statistisch eindeutig für Ihre Anwendung generiert.
   // Sie können ihn ändern, wenn Sie einen bestimmten Bezeichnernamen bevorzugen.
   // {E217CE60-3215-11D4-B6EC-0000B458D891}
   static const CLSID clsid =
   { 0xe217ce60, 0x3215, 0x11d4, { 0xb6, 0xec, 0x0, 0x0, 0xb4, 0x58, 0xd8, 0x91 } };
#endif

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVApp Konstruktion
CCARA2DVApp::CCARA2DVApp()
{
   m_bEditMode             = false;
   m_bEditable             = false;
   ZeroMemory(&m_PiCalculator, sizeof(PROCESS_INFORMATION));

   char szName[64];
   LoadString(m_hInstance, AFX_IDS_APP_TITLE, szName, 64);
   g_bDoReport = DoDebugReport(szName);
}

CCARA2DVApp::~CCARA2DVApp()
{
   m_2DVDiv.Destructor();
   if (m_PiCalculator.dwThreadId)
   {
      m_PiCalculator.dwProcessId = 3;  // schließen
      TestCalculator();
      if (m_PiCalculator.hProcess) ::CloseHandle(m_PiCalculator.hProcess);
      if (m_PiCalculator.hThread ) ::CloseHandle(m_PiCalculator.hThread );
   }
   ETSDEBUGEND;
   AfxTermLocalData(NULL, true);
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CCARA2DVApp-Objekt
CCARA2DVApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVApp Initialisierung
BOOL CCARA2DVApp::InitInstance()
{ 
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA2DVApp::InitInstance");
#endif

	if (!AfxSocketInit())
	{
      REPORT("IDP_SOCKETS_INIT_FAILED");
		return FALSE;
	}

#ifdef ETS_OLE_SERVER
	if (!AfxOleInit())               // OLE-Bibliotheken initialisieren
	{
      REPORT("OLE_INIT_FAILED");
		return FALSE;
	}

	AfxEnableControlContainer();
#endif

#if _MFC_VER < 0x0710
   #ifdef _AFXDLL
	   Enable3dControls();			      // Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
   #else
	   Enable3dControlsStatic();	      // Diese Funktion bei statischen MFC-Anbindungen aufrufen
   #endif
#endif
   CCaraWinApp::InitInstance();

#ifdef _REGISTER_BUILD
      CRegisterDlg dlg;
      if (!dlg.IsRegistered())
         dlg.DoModal();
#endif
   // Befehlszeile parsen, um zu prüfen auf Standard-Umgebungsbefehle DDE, Datei offen
   ParseCommandLine(m_CmdInfo);

   CSingleDocTemplate* pDocTemplate;
   pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CCARA2DVDoc),
      RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
      RUNTIME_CLASS(CCARA2DVView));

#ifdef ETS_OLE_SERVER
   pDocTemplate->SetServerInfo(
		IDR_SRVR_EMBEDDED, IDR_SRVR_INPLACE,
		RUNTIME_CLASS(CInPlaceFrame));
#endif

   AddDocTemplate(pDocTemplate);       // Dokument Templat einfügen
   
   EnableShellOpen();                  // DDE-Execute-Open aktivieren

#ifdef ETS_OLE_SERVER
	if (m_CmdInfo.m_bRunEmbedded || m_CmdInfo.m_bRunAutomated)
	{
	   m_server.ConnectTemplate(clsid, pDocTemplate, TRUE);
		COleTemplateServer::RegisterAll();
//	   m_server.UpdateRegistry(OAT_INPLACE_SERVER);
		return TRUE;
	}
#else
   if (m_CmdInfo.m_bRunEmbedded || m_CmdInfo.m_bRunAutomated)
      return false;
#endif

   // Verteilung der in der Befehlszeile angegebenen Befehle
   if (!ProcessShellCommand(m_CmdInfo))
   {
      return FALSE;
   } 

   if (m_pMainWnd)
   {
      ((CMainFrame*)m_pMainWnd)->SetMainPaneInfo();
      m_pMainWnd->ShowWindow(SW_SHOW);
      m_pMainWnd->UpdateWindow();
      m_pMainWnd->DragAcceptFiles();   // Enable drag/drop open

      CCARA2DVDoc* pdoc = (CCARA2DVDoc*) ((CMainFrame*)m_pMainWnd)->GetActiveDocument();
      if (pdoc) pdoc->AppendPlotMenu();
      if (m_nCallingWindowMessage == WM_USER_OPT_UPDATE)
      {
         ::PostMessage(m_hCallingWindowHandle, m_nCallingWindowMessage, (WPARAM)m_pMainWnd->m_hWnd, (LPARAM)::GetCurrentProcessId());
      }
   }
   bool bCalc = true;
   if (IsOnWinNT())                                            // NT nicht
   {
      if (!IsOnWin2000()) bCalc = false;                       // W2k doch
   }
   if (bCalc && m_strCalculatorPath.IsEmpty())
   {
      char szPath[MAX_PATH];
      UINT uSize = MAX_PATH;
      uSize = GetWindowsDirectory(szPath, uSize);
      if (uSize)
      {
         strcat(szPath, "\\calc.exe");
         if (GetFileAttributes(szPath) != 0xffffffff) m_strCalculatorPath = _T(szPath);
         else
         {
            szPath[uSize] = 0;
            strcat(szPath, "\\System32\\calc.exe");
            if (GetFileAttributes(szPath) != 0xffffffff) m_strCalculatorPath = _T(szPath);
            else
            {
               szPath[uSize] = 0;
               strcat(szPath, "\\System\\calc.exe");
            }
         }
      }
   }

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCARA2DVApp-Befehle

void CCARA2DVApp::LoadDefaultSettings()
{
   BYTE * byteptr=NULL;
   UINT   size;
   GetProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_PLOT, &byteptr, &size);
   if (byteptr)
   {
      if (size == sizeof(SStandardPlotSettings))
      {
         memcpy(&CPlotLabel::gm_StdPltSets, byteptr, size);
      }
      delete[] byteptr;
   }
   else
   {
      HDC hdc = ::GetDC(NULL);
      int nLogPixels = ::GetDeviceCaps(hdc, LOGPIXELSY);
      ::ReleaseDC(NULL, hdc);
      if (IsOnWinNT()) nLogPixels = MulDiv(nLogPixels, 4, 3);
      CPlotLabel::gm_StdPltSets.sm_HeadingLogFont.lfHeight  = -MulDiv(24, nLogPixels, 72);
      CPlotLabel::gm_StdPltSets.sm_HeadingLogFont.lfWidth   =  MulDiv(10, nLogPixels, 72);
      CPlotLabel::gm_StdPltSets.sm_GridTextLogFont.lfHeight = -MulDiv(20, nLogPixels, 72);
      CPlotLabel::gm_StdPltSets.sm_GridTextLogFont.lfWidth  =  MulDiv( 8, nLogPixels, 72);
      CPlotLabel::gm_StdPltSets.sm_CurveLogFont.lfHeight    = -MulDiv(20, nLogPixels, 72);
      CPlotLabel::gm_StdPltSets.sm_CurveLogFont.lfWidth     =  MulDiv( 8, nLogPixels, 72);
      REPORT("Heading(%d,%d),Grid(%d,%d),Curve(%d,%d)",
         CPlotLabel::gm_StdPltSets.sm_HeadingLogFont.lfHeight,
         CPlotLabel::gm_StdPltSets.sm_HeadingLogFont.lfWidth,
         CPlotLabel::gm_StdPltSets.sm_GridTextLogFont.lfHeight,
         CPlotLabel::gm_StdPltSets.sm_GridTextLogFont.lfWidth,
         CPlotLabel::gm_StdPltSets.sm_CurveLogFont.lfHeight,
         CPlotLabel::gm_StdPltSets.sm_CurveLogFont.lfWidth);
   }
   m_strDefaultCurveDir = GetProfileString(REGKEY_DEFAULT, REGKEY_DEFAULT_CURVEDIR);
   m_strCalculatorPath  = GetProfileString(REGKEY_DEFAULT, REGKEY_CALCULATOR_PATH);
}

void CCARA2DVApp::SaveDefaultSettings()
{                                                              // Ploteinstellungen
   WriteProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_PLOT, (BYTE*)&CPlotLabel::gm_StdPltSets, sizeof(SStandardPlotSettings));
}

void CCARA2DVApp::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(/*(m_bEditMode==false) &&*/                      // im Editiermodus nicht einfügen ?
                  (m_pMainWnd != NULL) &&                      // Frame Window vorhanden ?
                  (((CCaraFrameWnd*)m_pMainWnd)->m_nCF_LABELOBJECT != 0) && // Clipboardformat vorhanden und verfügbar ?
                  (  ::IsClipboardFormatAvailable(((CCaraFrameWnd*)m_pMainWnd)->m_nCF_LABELOBJECT)
                   ||::IsClipboardFormatAvailable(CF_TEXT)));
}
void CCARA2DVApp::OnUpdateFileFunc(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(!m_bEditMode);
};

void CCARA2DVApp::OnHeapwatch()
{
#ifndef _DEBUG
   CLabel::HeapWatch();
#endif
}

int CCARA2DVApp::ExitInstance() 
{
   BEGIN("ExitInstance");
   if (!m_strDefaultCurveDir.IsEmpty())
      WriteProfileString(REGKEY_DEFAULT, REGKEY_DEFAULT_CURVEDIR, m_strDefaultCurveDir);

   if (!m_strCalculatorPath.IsEmpty())
      WriteProfileString(REGKEY_DEFAULT, REGKEY_CALCULATOR_PATH, m_strCalculatorPath);

	return CCaraWinApp::ExitInstance();
}

int CCARA2DVApp::Run() 
{
   CEtsHelp etshelp;
	int nResult = CCaraWinApp::Run();
   etshelp.Destructor();
   return nResult;
}

BOOL CCARA2DVApp::OnIdle(LONG lCount)
{
	return CCaraWinApp::OnIdle(lCount);
}

class CCaraDocManager : public CDocManager
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

void CCARA2DVApp::OnFileOpen() 
{
   ASSERT(m_pDocManager != NULL);

   CSingleDocTemplate dt2(IDR_MAINFRAME2, NULL, NULL, NULL);
   m_pDocManager->AddDocTemplate(&dt2);
   CSingleDocTemplate dt3(IDR_MAINFRAME3, NULL, NULL, NULL);
   m_pDocManager->AddDocTemplate(&dt3);
   CSingleDocTemplate dt4(IDR_MAINFRAME4, NULL, NULL, NULL);
   m_pDocManager->AddDocTemplate(&dt4);

	CString newName;
   BOOL bReturn = m_pDocManager->DoPromptFileName(newName, AFX_IDS_OPENFILE, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, TRUE, NULL);

   ((CCaraDocManager*)m_pDocManager)->RemoveDocTemplate(&dt2);
   ((CCaraDocManager*)m_pDocManager)->RemoveDocTemplate(&dt3);
   ((CCaraDocManager*)m_pDocManager)->RemoveDocTemplate(&dt4);

   if (bReturn)
      OpenDocumentFile(newName);
}

void CCARA2DVApp::OnViewCalculator() 
{
   if (m_PiCalculator.dwThreadId == 0)
   {
      STARTUPINFO startupinfo;
      ZeroMemory(&startupinfo, sizeof(STARTUPINFO));           // StartupInfo Struktur füllen
      startupinfo.cb          = sizeof(STARTUPINFO);
      startupinfo.wShowWindow = SW_SHOW;
      if (CreateProcess(NULL, (char*)LPCTSTR(m_strCalculatorPath), NULL, NULL, false, 0, NULL, NULL, &startupinfo, &m_PiCalculator))
      {
         m_PiCalculator.dwProcessId = 2;                       // wird für Steuerung mißbraucht
      }
   }
   else
   {
      m_PiCalculator.dwProcessId = 3;  // schließen
      TestCalculator();
   }
}

void CCARA2DVApp::OnUpdateViewCalculator(CCmdUI* pCmdUI) 
{
   BOOL bEmpty = m_strCalculatorPath.IsEmpty();
   pCmdUI->Enable(!bEmpty);
   if (!bEmpty)
   {
      m_PiCalculator.dwProcessId |= 0x04;
      TestCalculator();
      m_PiCalculator.dwProcessId &= ~0x04;
   }
   pCmdUI->SetCheck((m_PiCalculator.dwProcessId & 0x00000003)?1:0);
}

void CCARA2DVApp::OnViewCalcForeground() 
{
   if (m_PiCalculator.dwThreadId != 0)
   {
      if (m_PiCalculator.dwProcessId == 1) m_PiCalculator.dwProcessId = 2;
      else                                 m_PiCalculator.dwProcessId = 1;
      TestCalculator();
   }	
}

void CCARA2DVApp::OnUpdateViewCalcForeground(CCmdUI* pCmdUI) 
{
   BOOL bEmpty = m_strCalculatorPath.IsEmpty();
   pCmdUI->Enable(!bEmpty);
   if (!bEmpty)
   {
      m_PiCalculator.dwProcessId |= 0x04;
      TestCalculator();
      m_PiCalculator.dwProcessId &= ~0x04;
   }
   pCmdUI->Enable((m_PiCalculator.hProcess != 0)? 1 : 0);
   pCmdUI->SetCheck(m_PiCalculator.dwProcessId == 1);
}

void CCARA2DVApp::TestCalculator()
{
   if (m_PiCalculator.dwThreadId)
   {
      EnumThreadWindows(m_PiCalculator.dwThreadId, CCARA2DVApp::EnumWindowsOfCalc, (long)&m_PiCalculator);
      if (m_PiCalculator.dwProcessId & 0x00001000) // Prozeß lebt noch
         m_PiCalculator.dwProcessId &= ~0x00001000; // Alive-Flag löschen
      else                                         // sonst Handle löschen
      {
          CloseHandle(m_PiCalculator.hProcess);
          CloseHandle(m_PiCalculator.hThread);
          ZeroMemory(&m_PiCalculator, sizeof(PROCESS_INFORMATION));
      }
   }
}

BOOL CALLBACK CCARA2DVApp::EnumWindowsOfCalc(HWND hwnd, LPARAM lParam)
{
   PROCESS_INFORMATION *pi = (PROCESS_INFORMATION*)lParam;
   HWND hwndMost = NULL;
   if (::GetParent(hwnd) != NULL) return 1;

   switch (pi->dwProcessId)
   {
      case 1:                          // in den Vordergrund bringen
          hwndMost = HWND_TOPMOST;
          break;
      case 2:                          // Kein Vordergrundfenster
          hwndMost = HWND_NOTOPMOST;
          break;
      case 3:                          // Schließen
          ::PostMessage(hwnd, WM_CLOSE, 0, 0);
//          pi->dwProcessId = 0;
          return 0;
      case 5: case 6: case 7:          // Testen
         pi->dwProcessId |= 0x00001000;// es lebt !
         return 0;
      default : ASSERT(false); break;
   }
   WINDOWPLACEMENT wpl;
   wpl.length = sizeof(wpl);
   GetWindowPlacement(hwnd, &wpl);
   if (wpl.showCmd == SW_SHOWMINIMIZED)
   {
      wpl.showCmd = SW_SHOWNORMAL;
      ::SetWindowPlacement(hwnd, &wpl);
   }
   else
   {
      ::SetWindowPos(hwnd, hwndMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER);
   }
   pi->dwProcessId |= 0x00001000;      // es lebt !
   return 0;
}

void CCARA2DVApp::OnRegister()
{
#ifdef _REGISTER_BUILD
      CRegisterDlg dlg;
      if (!dlg.IsRegistered())
         dlg.DoModal();
      else
         CCaraWinApp::OnAppAbout();
#else   
   CCaraWinApp::OnAppAbout();
#endif
}
