/********************************************************************/
/*                                                                  */
/*                  Programm ETS3DGL Dialog                         */
/*                                                                  */
/*     Setup Dialog für die Einstellung von Bugfixes für diverse    */
/*     Grafikkarten um Darstellungsfehler zu vermeiden              */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// ETS3DGLDLG.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "ETS3DGLDLG.h"
#include "ETS3DGLDLGDlg.h"
#include "CarawalkDlg.h"
#include "ReportDlg.h"
#include "PathDlg.h"
#include "PresentationDlg.h"
#include "CopyFolder.h"
#include "AnimationDlg.h"
#include "ETSBIND.h"
#include "Ets3dGLRegKeys.h"
#include "ErrorMessageDlg.h"
#include "MaterialComponents.h"
#include "MainFrame.h"
#include "CCaraInfo.h"
#include "CEtsCDPlayer.h"
#include "TimerDlg.h"

#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CETS3DGLDLGApp

BEGIN_MESSAGE_MAP(CETS3DGLDLGApp, CWinApp)
	//{{AFX_MSG_MAP(CETS3DGLDLGApp)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_CALL_CARAVIEW_SETUP, OnCallCaraviewSetup)
	ON_COMMAND(ID_CALL_CARAWALKSETUP, OnCallCarawalksetup)
	ON_COMMAND(ID_CALL_DLL_PATHSETUP, OnCallDllPathsetup)
	ON_COMMAND(ID_CALL_GLSETUP, OnCallGlsetup)
	ON_COMMAND(ID_SET_DEBUG_PATH, OnSetDebugPath)
	ON_COMMAND(ID_SET_RELEASE_ENG_PATH, OnSetReleaseEngPath)
	ON_COMMAND(ID_SET_RELEASE_PATH, OnSetReleasePath)
	ON_COMMAND(ID_CALL_SAVE_PROJECT, OnCallSaveProject)
	ON_COMMAND(ID_CALL_MATERIALSETUP, OnCallMaterialsetup)
	ON_COMMAND(ID_CALL_REPORT_SETUP, OnCallReportSetup)
	ON_UPDATE_COMMAND_UI(IDCANCEL, OnUpdateCancel)
	ON_COMMAND(ID_CALL_ETSMMPLAYER, OnCallEtsmmplayer)
	ON_COMMAND(ID_CALL_TIMEPLAN, OnCallTimeplan)
	ON_COMMAND(ID_CALL_ETSMMRECORDER, OnCallEtsmmrecorder)
	ON_COMMAND(ID_CALL_ETSMIXER, OnCallEtsmixer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CETS3DGLDLGApp Konstruktion

CETS3DGLDLGApp::CETS3DGLDLGApp()
{
   m_pCDPlayer = NULL;
   m_pRecorder = NULL;
}

CETS3DGLDLGApp::~CETS3DGLDLGApp()
{
   StructOfDates *pDel;
   while (m_ListDates.GetHeadPosition())
   {
      pDel = (StructOfDates*)m_ListDates.RemoveHead();
      delete pDel;
   }

   if (m_pCDPlayer != NULL)
   {
      if (m_pRecorder == NULL) m_pCDPlayer->Destructor();
      delete m_pCDPlayer;
      m_pCDPlayer = NULL;
   }
   if (m_pRecorder != NULL)
   {
      m_pRecorder->Destructor();
      delete m_pRecorder;
      m_pRecorder = NULL;
   }

//	m_CDPlayer.Destructor();
}


/////////////////////////////////////////////////////////////////////////////
// Das einzige CETS3DGLDLGApp-Objekt

CETS3DGLDLGApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CETS3DGLDLGApp Initialisierung

BOOL CETS3DGLDLGApp::InitInstance()
{
	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.
   char  szFileName[64];
   DWORD dwDummy = 0;
   wsprintf(szFileName, "%s.EXE", m_pszExeName);
   int   bytes = GetFileVersionInfoSize(szFileName, &dwDummy);
   int nVersion = 1;
   int nSubVers = 0;
   if(bytes)
   {
      void * buffer = new char[bytes];
      GetFileVersionInfo(szFileName, 0, bytes, buffer);
      VS_FIXEDFILEINFO * Version;
      unsigned int     length;
      if(VerQueryValue(buffer,"\\",(void **) &Version, &length))
      {
         nVersion = HIWORD(Version->dwFileVersionMS);
         nSubVers = LOWORD(Version->dwFileVersionMS);
         if ((nSubVers > 0) && (nSubVers < 10)) nSubVers *= 10;
      }
      delete[] buffer;
   }
 
   SetRegistryKey(_T("ETSKIEL"));// Firmenname

   if (m_pszProfileName)
   {
      char profilename[_MAX_PATH];                                   // und Versionsnummer
      wsprintf(profilename, "%s\\V%d.%02d", m_pszProfileName, nVersion, nSubVers);
      free((void*)m_pszProfileName);
      m_pszProfileName = _tcsdup(profilename);
   }

//#ifdef _AFXDLL
//	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
//#else
//	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
//#endif

   char szLocale[32];                                          // ermitteln
   ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, szLocale, 32);
   setlocale(LC_ALL, szLocale);                                // und setzen

   if ((m_lpCmdLine != NULL) && (m_lpCmdLine[0] == '/'))
   {
      if (m_lpCmdLine[1] == '?')
      {
         AfxMessageBox(IDS_SHORT_KEY_HELP, MB_OK|MB_ICONINFORMATION);
         return false;
      }
      else if (m_lpCmdLine[1] == 'k')
      {
         CRect rcWnd(0,0,100,100);
         try
         {
            m_pActiveWnd = new CMainFrame;
            if (!m_pActiveWnd) throw (int) 1;
            ((CMainFrame*)m_pActiveWnd)->m_nSpecial = (m_lpCmdLine[2] == 's') ? 1 : 0;
            if (!((CMainFrame*)m_pActiveWnd)->Create(NULL, "CARA Setup", 0, rcWnd, NULL, 0, NULL)) throw (int) 2;
            m_pMainWnd = m_pActiveWnd;
         }
         catch(int nError)
         {
            char text[32];
            wsprintf(text, "Error No.: %d", nError);
            AfxMessageBox(text, MB_OK|MB_ICONEXCLAMATION, 0);
            if (m_pActiveWnd)  delete m_pActiveWnd;
            m_pMainWnd = m_pActiveWnd = NULL;
            return false;
         }
         return true;
      }
      else if ((m_lpCmdLine[1] == 'm') && (m_lpCmdLine[2] == 'm'))
      {
         OnCallEtsmmplayer();
      }
      else if ((m_lpCmdLine[1] == 'm') && (m_lpCmdLine[2] == 'r'))
      {
         OnCallEtsmmrecorder();
      }
      else if ((m_lpCmdLine[1] == 'm') && (m_lpCmdLine[2] == 'i'))
      {
         OnCallEtsmixer();
      }
      else if (CallSpecialDlg(m_lpCmdLine[1]))
      {
         
      }
#ifdef _DEBUG
      else if (m_lpCmdLine[1] == 'a')     // Animations-Dialog
      {
         CAnimationDlg dlg;
	      m_pMainWnd = &dlg;
	      dlg.DoModal();
      }
      else if (m_lpCmdLine[1] == 'e')     // Error-Dialog
      {
	      CErrorMessageDlg dlg;
         if (m_lpCmdLine[2] == ':')
         {
            dlg.m_nErrorMessage = atoi(&m_lpCmdLine[3]);
         }
	      m_pMainWnd = &dlg;
	      dlg.DoModal();
      }
#endif // _DEBUG
   }
   else                                   // Setup/Bugfix-Dialog
   {
	   CETS3DGLDLGDlg dlg;
	   m_pMainWnd = &dlg;
      dlg.m_bSpecial = false;
	   dlg.DoModal();
   }

   m_pMainWnd = NULL;
   return FALSE;
}

void CETS3DGLDLGApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	CWinApp::WinHelp(dwData, nCmd);
}

bool CETS3DGLDLGApp::SetProfileName(char *pszModuleName, char *pszRegName)
{
   bool              bReturn = false;
   int               bytes   = 0;
   VS_FIXEDFILEINFO *Version;
   char              szModuleName[_MAX_PATH];

   if (m_pszProfileName)
   {
      free((void*)m_pszProfileName);
      m_pszProfileName = NULL;
   }

   if (pszModuleName)
   {
      bytes = GetFileVersionInfoSize(pszModuleName,NULL);

      if (bytes == 0)
      {
         ETSGetCommonPath(szModuleName, _MAX_PATH);
         strncat(szModuleName, pszModuleName, _MAX_PATH);
         pszModuleName = szModuleName;
         bytes = GetFileVersionInfoSize(pszModuleName, NULL);
      }

      if(bytes)
      {
         void * buffer = new char[bytes];
         GetFileVersionInfo(pszModuleName, NULL, bytes, buffer);
         unsigned int     length;
         if(VerQueryValue(buffer,"\\",(void **) &Version, &length))
         {
            char profilename[100];                                   // und Versionsnummer
            int nVersion = HIWORD(Version->dwFileVersionMS);
            int nSubVers = LOWORD(Version->dwFileVersionMS);
            if ((nSubVers > 0) && (nSubVers < 10)) nSubVers *= 10;
            wsprintf(profilename, "%s\\V%d.%02d", pszRegName, nVersion, nSubVers);
            m_pszProfileName = _tcsdup(profilename);
            bReturn = true;
         }
         delete[] buffer;
      }
   }
   else if (pszRegName)
   {
      m_pszProfileName = _tcsdup(pszRegName);
      bReturn = true;
   }

   return bReturn;
}

int CALLBACK CETS3DGLDLGApp::BrowseFolderCallback(HWND hwndDlg, UINT nMsg, LPARAM lParam, LPARAM lData)
{
   BROWSEINFO *pbi = (BROWSEINFO*) lParam;
   switch (nMsg)
   {
      case BFFM_INITIALIZED:
         if (lData)
         {
            FolderCallBackStruct *pfcs = (FolderCallBackStruct*)lData;
            ::SendMessage(hwndDlg, BFFM_SETSELECTION, 1, (LPARAM)pfcs->pszPath);
            if (pfcs->nFlags & FOLDERPATH_NO_BROWSER)
            {
               ::SendMessage(hwndDlg, WM_COMMAND, IDOK, 0);
            }
         }
         break;
      case BFFM_SELCHANGED:
         break;
      case BFFM_VALIDATEFAILED:
         break;
   }
/*
      case BFFM_ENABLEOK:
         break;
      case BFFM_SETSELECTION:
         break;
      case BFFM_SETSTATUSTEXT:
         break;
*/
   
   return 0;
}

bool CETS3DGLDLGApp::GetFolderPath(CString &strFolderPath, ITEMIDLIST**ppidl, UINT nFlags)
{
   FolderCallBackStruct fcs = {"", nFlags};
   BROWSEINFO bi;
   bi.hwndOwner = AfxGetMainWnd()->m_hWnd;
   
   HRESULT hResult = ::SHGetSpecialFolderLocation(bi.hwndOwner, CSIDL_DESKTOP, (ITEMIDLIST**)&bi.pidlRoot);
   if (hResult == NOERROR)
   {
      strcpy(fcs.pszPath, LPCTSTR(strFolderPath));
      int nlen = strlen(fcs.pszPath);
      if ((nlen > 1) && (fcs.pszPath[nlen-1] == '\\')) fcs.pszPath[nlen-1] = 0;
      bi.pszDisplayName = fcs.pszPath;
      bi.lpszTitle      = "Dll-Pfad suchen";
      if (nFlags & FOLDERPATH_RETURN_FILES)
         bi.ulFlags        = BIF_BROWSEINCLUDEFILES;
      else
         bi.ulFlags        = BIF_RETURNONLYFSDIRS;
      bi.lpfn           = CETS3DGLDLGApp::BrowseFolderCallback;
      bi.lParam         = (long)&fcs;
      bi.iImage         = 0;
      ITEMIDLIST *pidl  = ::SHBrowseForFolder(&bi);
      if (pidl)
      {
         ::SHGetPathFromIDList(pidl, fcs.pszPath);
         if (ppidl)
         {
            if (*ppidl) ::CoTaskMemFree((void*)*ppidl);
            *ppidl = pidl;
         }
         else
         {
            ::CoTaskMemFree((void*)pidl);
         }
         if (nFlags &FOLDERPATH_CONCAT_SLASH) strFolderPath.Format("%s\\", fcs.pszPath);
         else                                 strFolderPath = _T(fcs.pszPath);
         return true;
      }
      ::CoTaskMemFree((void*)bi.pidlRoot);
   }
   return false;
}

void CETS3DGLDLGApp::OnAppExit() 
{
   if (m_pActiveWnd)
   {
      m_pActiveWnd->SendMessage(WM_CLOSE);
   }	
}

void CETS3DGLDLGApp::OnAppAbout() 
{
   CCaraInfo CaraInfo;     // eine lokale Instanz der DLL-Klasse
   char exename[32];
   strcpy(exename, m_pszExeName);
   strcat(exename, ".exe");
   CCaraInfo::CallInfoResource(m_pMainWnd->m_hWnd, exename);
   CaraInfo.Destructor();
}

bool CETS3DGLDLGApp::CallSpecialDlg(char c)
{
   bool bReturn = true;
   if      (c == 'c')     // Carawalk-Dialog
   {
	   CCarawalkDlg dlg(m_pMainWnd);
      if (!m_pActiveWnd) m_pMainWnd = &dlg;
	   dlg.DoModal();
   }
   else if (c == 'r')     // Report-Dialog
   {
	   CReportDlg dlg(m_pMainWnd);
      if (!m_pActiveWnd) m_pMainWnd = &dlg;
	   dlg.DoModal();
   }
   else if (c == 'm')     // Material-Dialog
   {
      CMaterialComponents dlg(m_pMainWnd);
      if (!m_pActiveWnd) m_pMainWnd = &dlg;
	   dlg.DoModal();
   }
   else if (c == 'p')     // Dll-Pfad-Dialog
   {
	   CPathDlg dlg(m_pMainWnd);
      if (!m_pActiveWnd) m_pMainWnd = &dlg;
	   dlg.DoModal();
   }
   else if (c == 'd')     // Darstellungs-Dialog
   {
	   CPresentationDlg dlg(m_pMainWnd);
      if (!m_pActiveWnd) m_pMainWnd = &dlg;
	   dlg.DoModal();
   }
   else if (c == 's')     // Sicherungs-Dialog
   {
	   CCopyFolder dlg(m_pMainWnd);
      if (!m_pActiveWnd) m_pMainWnd = &dlg;
	   dlg.DoModal();
   }
   else if (c == 't')     // Terminplaner
   {
	   CTimerDlg dlg(m_pMainWnd);
      dlg.m_pListDates = &m_ListDates;
      if (!m_pActiveWnd) m_pMainWnd = &dlg;
	   dlg.DoModal();
   }
   else if (c == 'S')     // Spezial
   {
	   CETS3DGLDLGDlg dlg(m_pMainWnd);
      if (!m_pActiveWnd) m_pMainWnd = &dlg;
      dlg.m_bSpecial = true;
	   dlg.DoModal();
   }
   else bReturn = false;
   return bReturn;
}

void CETS3DGLDLGApp::OnCallCaraviewSetup()
{
   ASSERT(m_pActiveWnd != NULL);
   CallSpecialDlg('d');
}

void CETS3DGLDLGApp::OnCallCarawalksetup()
{
   ASSERT(m_pActiveWnd != NULL);
   CallSpecialDlg('c');
}

void CETS3DGLDLGApp::OnCallDllPathsetup()
{
   ASSERT(m_pActiveWnd != NULL);
   CallSpecialDlg('p');
}

void CETS3DGLDLGApp::OnCallGlsetup()
{
   ASSERT(m_pActiveWnd != NULL);
   if (((CMainFrame*)m_pActiveWnd)->m_nSpecial)
      CallSpecialDlg('S');
	else
   {
	   CETS3DGLDLGDlg dlg(m_pMainWnd);
	   if (!m_pActiveWnd) m_pMainWnd = &dlg;
	   dlg.DoModal();
   }
}
void CETS3DGLDLGApp::OnCallSaveProject() 
{
   CallSpecialDlg('s');
}
void CETS3DGLDLGApp::OnCallMaterialsetup() 
{
   CallSpecialDlg('m');
}
void CETS3DGLDLGApp::OnCallReportSetup() 
{
   CallSpecialDlg('r');
}

void CETS3DGLDLGApp::OnCallTimeplan() 
{
   CallSpecialDlg('t');
}

void CETS3DGLDLGApp::OnSetDebugPath() 
{
   SetProfileName(NULL, ETSBIND_REGKEY);
   CString str = GetProfileString("", REGKEY_DLLPATH_DEBUG);
   WriteProfileString("", REGKEY_DLLPATH, str);
}

void CETS3DGLDLGApp::OnSetReleaseEngPath() 
{
   SetProfileName(NULL, ETSBIND_REGKEY);
   CString str = GetProfileString("", REGKEY_DLLPATH_RELEASE_E);
   WriteProfileString("", REGKEY_DLLPATH, str);
}

void CETS3DGLDLGApp::OnSetReleasePath() 
{
   SetProfileName(NULL, ETSBIND_REGKEY);
   CString str = GetProfileString("", REGKEY_DLLPATH_RELEASE);
   WriteProfileString("", REGKEY_DLLPATH, str);
}


int CETS3DGLDLGApp::ExitInstance() 
{
   int nReturn = CWinApp::ExitInstance();
   return nReturn;
}


void CETS3DGLDLGApp::OnUpdateCancel(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable();	
}


void CETS3DGLDLGApp::OnCallEtsmmplayer() 
{
   if (m_pCDPlayer == NULL)
   {
      m_pCDPlayer = new CEtsCDPlayer;
   }
   if (m_pCDPlayer != NULL)
   {
      if ((m_pMainWnd != NULL) && (m_pMainWnd->m_hWnd != NULL))
         m_pCDPlayer->Create(m_pMainWnd->m_hWnd);
      else
      {
         m_pCDPlayer->DoModal(NULL);
      }
   }
}

void CETS3DGLDLGApp::OnCallEtsmixer() 
{
   if (m_pCDPlayer == NULL)
   {
      m_pCDPlayer = new CEtsCDPlayer;
   }
   if (m_pCDPlayer != NULL)
   {
      if ((m_pMainWnd != NULL) && (m_pMainWnd->m_hWnd != NULL))
         m_pCDPlayer->CreateMixerDialog(m_pMainWnd->m_hWnd);
      else
      {
         m_pCDPlayer->DoMixerDlg(NULL);
      }
   }
}

void CETS3DGLDLGApp::OnCallEtsmmrecorder() 
{
   if (m_pRecorder == NULL)
   {
      m_pRecorder = new CEtsCDPlayer;
   }
   if (m_pRecorder != NULL)
   {
      m_pRecorder->SetNoOfTitles(TITLES_FOR_RECORDING|1);
      if ((m_pMainWnd != NULL) && (m_pMainWnd->m_hWnd != NULL))
         m_pRecorder->Create(m_pMainWnd->m_hWnd);
      else
      {
         m_pRecorder->DoModal(NULL);
      }
   }
}

int CETS3DGLDLGApp::Run() 
{
	int nResult =  CWinApp::Run();
	return nResult;
}
