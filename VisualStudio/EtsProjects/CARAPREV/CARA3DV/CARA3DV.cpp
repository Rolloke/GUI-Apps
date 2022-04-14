/********************************************************************/
/*                                                                  */
/*                     Programm ETS3DV                              */
/*                                                                  */
/*     Darstellung und Bearbeitung von 3D Diagrammen                */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// CARA3DV.cpp : Legt das Klassenverhalten für die Anwendung fest.
//
#include "stdafx.h"
#include "CARA3DV.h"

#include "MainFrm.h"
#include "CARA3DVDoc.h"
#include "CARA3DVView.h"
#include "CCaraInfo.h"

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
  #define  ETSDEBUG_CREATEDATA "ETS3DV", &g_bDoReport
 #else
  #define  ETSDEBUG_CREATEDATA g_szAppName, &g_bDoReport
 #endif
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCARA3DVApp

BEGIN_MESSAGE_MAP(CCARA3DVApp, CCaraWinApp)
	//{{AFX_MSG_MAP(CCARA3DVApp)
	//}}AFX_MSG_MAP
	// Dateibasierte Standard-Dokumentbefehle
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	ON_COMMAND((ID_FILE_PRINT+ID_TB_RIGHT_EXT), OnFilePrintSetup)
	ON_COMMAND(ID_APP_ABOUT, CCaraWinApp::OnAppAbout)
   ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CCaraWinApp::OnFileOpen)
END_MESSAGE_MAP()

#ifdef ETS_OLE_SERVER
   // Dieser Bezeichner wurde als statistisch eindeutig für Ihre Anwendung generiert.
   // Sie können ihn ändern, wenn Sie einen bestimmten Bezeichnernamen bevorzugen.
   // {579C69C0-3BA8-11D4-B6EC-0000B458D891}
   static const CLSID clsid =
   { 0x579c69c0, 0x3ba8, 0x11d4, { 0xb6, 0xec, 0x0, 0x0, 0xb4, 0x58, 0xd8, 0x91 } };
#endif

/////////////////////////////////////////////////////////////////////////////
// CCARA3DVApp Konstruktion

CCARA3DVApp::CCARA3DVApp()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVApp");
#endif

   char szName[64];
   LoadString(m_hInstance, AFX_IDS_APP_TITLE, szName, 64);
   g_bDoReport = DoDebugReport(szName);
}

CCARA3DVApp::~CCARA3DVApp()
{
#ifdef _REPORT_EXTENDED_
   REPORT("~CCARA3DVApp()");
#endif
   ETSDEBUGEND;

   AfxTermLocalData(NULL, true);
}
/////////////////////////////////////////////////////////////////////////////
// Das einzige CCARA3DVApp-Objekt
CCARA3DVApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCARA3DVApp Initialisierung
BOOL CCARA3DVApp::InitInstance()
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVApp::InitInstance");
#endif

#ifdef ETS_OLE_SERVER
	if (!AfxSocketInit())
	{
      REPORT("IDP_SOCKETS_INIT_FAILED");
		return FALSE;
	}
	if (!AfxOleInit())         // OLE-Bibliotheken initialisieren
	{
      REPORT("OLE_INIT_FAILED");
		return FALSE;
	}

	AfxEnableControlContainer();
#endif

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

   CCaraWinApp::InitInstance();

   // Befehlszeile parsen, um zu prüfen auf Standard-Umgebungsbefehle DDE, Datei offen
   ParseCommandLine(m_CmdInfo);

   CSingleDocTemplate* pDocTemplate;
   pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CCARA3DVDoc),
      RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
      RUNTIME_CLASS(CCARA3DVView));

#ifdef ETS_OLE_SERVER
   pDocTemplate->SetServerInfo(
		IDR_SRVR_EMBEDDED, IDR_SRVR_INPLACE,
		RUNTIME_CLASS(CInPlaceFrame));
#endif

   AddDocTemplate(pDocTemplate);

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
      CDocument *pDoc = ((CMainFrame*)m_pMainWnd)->GetActiveDocument();
      if (pDoc)
      {
         ASSERT_KINDOF(CCARA3DVDoc, pDoc);
         ((CMainFrame*)m_pMainWnd)->InvalidateFrameContent();
         ((CMainFrame*)m_pMainWnd)->AttachColorArray((CColorArray*)&((CCARA3DVDoc*)pDoc)->m_LevelColor);
      }

      m_pMainWnd->ShowWindow(SW_SHOW);
      m_pMainWnd->UpdateWindow();
      m_pMainWnd->DragAcceptFiles();   // Enable drag/drop open
   }
   return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CCARA3DVApp-Befehle

int CCARA3DVApp::ExitInstance() 
{
#ifdef _REPORT_EXTENDED_
   BEGIN("CCARA3DVApp::ExitInstance");
#endif
	int nResult = CCaraWinApp::ExitInstance();
   return nResult; 
}

BOOL CCARA3DVApp::OnIdle(LONG lCount) 
{
   if (m_pMainWnd)
   {
      ((CMainFrame*)m_pMainWnd)->OnUpdateFrameContent();
   }
	return CCaraWinApp::OnIdle(lCount);
}

int CCARA3DVApp::Run() 
{
   CEtsDiv  c3DVDiv;
	int nResult = CCaraWinApp::Run();
   c3DVDiv.Destructor();
	return nResult;
}
