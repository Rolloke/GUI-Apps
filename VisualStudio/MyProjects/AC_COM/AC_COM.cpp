/********************************************************************/
/*                                                                  */
/*                     Program AC_COM                               */
/*                                                                  */
/*     Schnittstelle für den Audio Controller                       */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// AC_COM.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "AC_COM.h"
#include "AC_COMDlg.h"
#include "CCaraInfo.h"
#include "ETS3DGLRegKeys.h"
#include "IRcodeDlg.h"
#include "CEtsCDPlayer.h"
#include <locale.h>

#define  AC_PROG_NAME "AC_COM"
#define  ETSDEBUG_INCLUDE 
#define  ETSDEBUG_CREATEDATA AC_PROG_NAME, &CAC_COMApp::gm_bDoReport
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAC_COMApp

CString CAC_COMApp::gm_strStartDirectory;
CString CAC_COMApp::gm_strCARAPath;
CString CAC_COMApp::gm_strEditFileName;
bool    CAC_COMApp::gm_bSpecialistMode    = false;
bool    CAC_COMApp::gm_bEditCurves        = false;
bool    CAC_COMApp::gm_bClosePortImediate = false;
int     CAC_COMApp::gm_nCARAVersion       = 100;
bool    CAC_COMApp::gm_bDoReport          = false;
DWORD   CAC_COMApp::gm_dwMessageBoxFlags  = MBF_WARN_REQU_ALL;
bool    CAC_COMApp::gm_bWindowsXP         = false;

BEGIN_MESSAGE_MAP(CAC_COMApp, CWinApp)
	//{{AFX_MSG_MAP(CAC_COMApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAC_COMApp Konstruktion
 
CAC_COMApp::CAC_COMApp()
{
   HKEY hKey;

   char szKey[128] = "Software\\ETSKIEL\\";
   strcat(szKey, AC_PROG_NAME);
   if (RegOpenKeyEx(HKEY_CURRENT_USER, szKey, 0, KEY_READ|KEY_WRITE, &hKey)==ERROR_SUCCESS)
   {
      DWORD dwSize = sizeof(DWORD), dwValue, dwType;
      if (RegQueryValueEx(hKey, REPORT_KEY, NULL, &dwType, (BYTE*) &dwValue, &dwSize)==ERROR_SUCCESS)
      {
         if ((dwType==REG_DWORD)&&(dwValue!=0))
         {
            gm_bDoReport = true;
            szKey[0] = 0;
            strncpy(szKey, AC_PROG_NAME, 8);
            strcat(szKey, ".dbg");
            DeleteFile(szKey);
         }
      }
      else
      {
         DWORD dwValue = 0;
         RegSetValueEx(hKey, REPORT_KEY, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD));
      }
      RegCloseKey(hKey);
   }

   REPORT("CAC_COMApp()");
   m_hAcc = NULL;
}

CAC_COMApp::~CAC_COMApp()
{
   CAudioControlData::DeleteCorrectionValues();
   if (m_hAcc) ::DestroyAcceleratorTable(m_hAcc);
   ETSDEBUGEND;
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CAC_COMApp-Objekt

CAC_COMApp theApp;

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

/////////////////////////////////////////////////////////////////////////////
// CAC_COMApp Initialisierung

BOOL CAC_COMApp::InitInstance()
{
   BEGIN("InitInstance");
	AfxEnableControlContainer();

	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

   char szLocale[32];                                          // ermitteln
   ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, szLocale, 32);
   setlocale(LC_ALL, szLocale);                                // und setzen


   REPORT("Version");
   DWORD dwDummy = 0;
   char  szFileName[64];
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

   REPORT("SetRegKey");
   SetRegistryKey(_T("ETSKIEL"));                                    // Firmenname
   if (m_pszProfileName)
   {
      char profilename[_MAX_PATH];                                   // und Versionsnummer
      wsprintf(profilename, "%s\\V%d.%02d", m_pszProfileName, nVersion, nSubVers);
      free((void*)m_pszProfileName);
      m_pszProfileName = _tcsdup(profilename);
   }

   REPORT("CARAPath");
   gm_strCARAPath        = GetProfileString(REGKEY_DEFAULT, CARA_PATH, NULL);
   if (gm_strCARAPath.IsEmpty())
   {
      static const char szUninstallKey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
      HKEY  hKey          = 0;
      if ((::RegOpenKeyEx(HKEY_LOCAL_MACHINE, szUninstallKey, 0, KEY_QUERY_VALUE|KEY_READ, &hKey) == ERROR_SUCCESS) &&
          (hKey != NULL))
      {
         FILETIME ft;
         int i;
         char  szSubKey[MAX_PATH];
         DWORD dwType, dwLength;
         for (i=0, dwLength = MAX_PATH; ::RegEnumKeyEx(hKey, i, szSubKey, &dwLength, NULL, NULL, 0, &ft) == ERROR_SUCCESS; i++, dwLength = MAX_PATH)
         {
            if (strncmp(szSubKey, "CARA", 4) == 0)
            {
               char szKey[MAX_PATH];
               int nVersion = atoi(&szSubKey[4]);
               if (nVersion < 200) continue;
               HKEY hSubKey = 0;
               strncpy(szKey, szUninstallKey, MAX_PATH);
               strncat(szKey, szSubKey, MAX_PATH);
               if ((::RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, KEY_QUERY_VALUE|KEY_READ, &hSubKey) == ERROR_SUCCESS) &&
                   (hSubKey != NULL))
               {
                  dwLength = MAX_PATH;
                  if ((::RegQueryValueEx(hSubKey, "UninstallString", NULL, &dwType, (BYTE*)&szKey, &dwLength) == ERROR_SUCCESS) && 
                      (dwType == REG_SZ))
                  {
                     char *pstr1 = strstr(&szKey[3], ":\\");
                     char *pstr2 = strstr(&szKey[3], "CARA.");
                     if (pstr1 && pstr2)
                     {
                        pstr2[0] = 0;
                        gm_strCARAPath = _T(&pstr1[-1]);
                     }
                  }
                  ::RegCloseKey(hSubKey);
               }
            }
         }
         ::RegCloseKey(hKey);
      }
   }

   REPORT("TestRegistry");
   if (!WriteProfileString(REGKEY_DEFAULT, CARA_PATH, gm_strCARAPath))
   {
      char profilename[_MAX_PATH];                                   // und Versionsnummer
      free((void*)m_pszRegistryKey);
      m_pszRegistryKey = NULL;
      free((void*)m_pszProfileName);
      GetWindowsDirectory(profilename, _MAX_PATH); 
      strncat(profilename, "\\", _MAX_PATH);
      strncat(profilename, m_pszAppName, _MAX_PATH);
      strncat(profilename, ".ini", _MAX_PATH);
      m_pszProfileName = _tcsdup(profilename);
   }

/*
#ifdef _DEBUG
   CCaraDocManager doc;
   m_pDocManager =&doc;
   CSingleDocTemplate dt1(IDR_MAINFRAME, NULL, NULL, NULL);
   CSingleDocTemplate dt2(IDR_FILE_TYPECD, NULL, NULL, NULL);
   CSingleDocTemplate dt3(IDR_FILE_TYPEIRC, NULL, NULL, NULL);
   doc.AddDocTemplate(&dt1);
   doc.AddDocTemplate(&dt2);
   doc.AddDocTemplate(&dt3);
   RegisterShellFileTypes(true);
   doc.RemoveDocTemplate(&dt1);
   doc.RemoveDocTemplate(&dt2);
   doc.RemoveDocTemplate(&dt3);
   m_pDocManager = NULL;
#endif
*/
   TestDirectory();

   gm_bSpecialistMode    = (GetProfileInt(  REGKEY_DEFAULT, PROFI_MODE , 0) != 0) ? true : false;
   gm_bEditCurves        = (GetProfileInt(  REGKEY_DEFAULT, EDIT_CURVES, 0) != 0) ? true : false;
   gm_bClosePortImediate = (GetProfileInt(  REGKEY_DEFAULT, CLOSE_PORT , 0) != 0) ? true : false;
   gm_dwMessageBoxFlags  = GetProfileInt(   REGKEY_DEFAULT, MB_FLAGS, gm_dwMessageBoxFlags);

   REPORT("CommandLine");
	ParseCommandLine(m_cmdInfo);
   if (m_cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
   {
      m_cmdInfo.m_strFileName.MakeLower();
      if (m_cmdInfo.m_strFileName.Find(".cdi") != -1)
      {
         CEtsCDPlayer CD;
         CD.DoTitleDlg(NULL, (char*)LPCTSTR(m_cmdInfo.m_strFileName));
         CD.Destructor();
         return false;
      }
      else if (m_cmdInfo.m_strFileName.Find(".irc") != -1)
      {
         CIRcodeDlg dlg;
      	m_pMainWnd = &dlg;
         dlg.m_hIcon = LoadIcon(IDR_FILE_TYPEIRC);
         if (dlg.ReadData(m_cmdInfo.m_strFileName))
         {
            if (dlg.DoModal() == IDOK)
            {
               dlg.WriteData(m_cmdInfo.m_strFileName);
            }
         }
         return false;
      }
   }

   m_hAcc = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_AC_ACCEL));
   CAC_COMDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	return false;
}

void CAC_COMApp::TestDirectory()
{
   BEGIN("TestDirectory");
   int nLen = lstrlen(m_pszHelpFilePath);
   if(nLen < 2048)                                             // teste nur wenn nicht länger als 2047 Zeichen
   {
      for (int i=nLen; i>=0; i--)
         if (m_pszHelpFilePath[i] == '\\') break;              // die HelpDatei extrahieren
      if (i>0)
      {
         CString str;
         i++;
         strncpy(gm_strStartDirectory.GetBufferSetLength(i), m_pszHelpFilePath, i);
         gm_strStartDirectory.ReleaseBuffer();
         SetCurrentDirectory(gm_strStartDirectory);

         str = GetProfileString(REGKEY_DEFAULT, FILE_PATH, NULL);
         if (str.IsEmpty())                                    // kein Key in der Registry
         {
            str = gm_strStartDirectory + _T("HAC");
            WriteProfileString(REGKEY_DEFAULT, FILE_PATH, str);// Datei-Pfad speichern
            if (::CreateDirectory(str, NULL) == 0)             // Fehler bei Erstellung
            {
               if (GetLastError()!=ERROR_ALREADY_EXISTS)       // echter Fehler beim erstellen
               {
                  WriteProfileString(REGKEY_DEFAULT, FILE_PATH, gm_strStartDirectory);// Start-Pfad speichern
               }
            }
            else                                               // Verzeichnis erstellt
            {
               WriteProfileString(REGKEY_DEFAULT, FILE_PATH, str);// CD-Info-Pfad speichern
            }
         }
      }
   }
}

BOOL CAC_COMApp::OnIdle(LONG lCount) 
{
	return CWinApp::OnIdle(lCount);
}

int CALLBACK CAC_COMApp::BrowseFolderCallback(HWND hwndDlg, UINT nMsg, LPARAM lParam, LPARAM lData)
{
   BEGIN("BrowseFolderCallback");
   BROWSEINFO *pbi = (BROWSEINFO*) lParam;
   switch (nMsg)
   {
      case BFFM_INITIALIZED:
         if (lData)
         {
            FolderCallBackStruct *pfcs = (FolderCallBackStruct*)lData;
            if (pfcs->pszPath)
            {
               int i, nLen = strlen(pfcs->pszPath);
               for (i=nLen-1; i>0; i--)
                  if (pfcs->pszPath[i] == '\\')
                  {
                     pfcs->pszPath[i+1] = 0;
                     break;
                  }
               ::SendMessage(hwndDlg, BFFM_SETSELECTION, 1, (LPARAM)pfcs->pszPath);
            }
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

bool CAC_COMApp::DoFileDialog(bool bSave, bool bMulti, int nResource, CString &strFileName)
{
   BEGIN("DoFileDialog");
   CDocManager        dm;
   CSingleDocTemplate dt(nResource, NULL, NULL, NULL);
   int                nIDTitle = bSave ? AFX_IDS_SAVEFILE : AFX_IDS_OPENFILE;
   DWORD              dwFlags  = OFN_HIDEREADONLY|OFN_PATHMUSTEXIST;
   CString            strFile(strFileName);
   CString            strDefaultDir = AfxGetApp()->GetProfileString(REGKEY_DEFAULT, FILE_PATH, NULL);
   bool               bReturn = false;
   char               szCurrentDir[MAX_PATH];
   if (bMulti && !bSave) dwFlags |= OFN_ALLOWMULTISELECT;
   ::GetCurrentDirectory(MAX_PATH, szCurrentDir);
   if (!strDefaultDir.IsEmpty())
   {
      ::SetCurrentDirectory(LPCTSTR(strDefaultDir));
   }
   if (dm.DoPromptFileName(strFile, nIDTitle, dwFlags, !bSave, &dt))
   {
      char szCurrentNew[MAX_PATH];
      strFileName = strFile;
      ::GetCurrentDirectory(MAX_PATH, szCurrentNew);
      AfxGetApp()->WriteProfileString(REGKEY_DEFAULT, FILE_PATH, szCurrentNew);
      bReturn = true;
   }
   ::SetCurrentDirectory(szCurrentDir);

   return bReturn;
}

bool CAC_COMApp::GetFolderPath(CString &strFolderPath, ITEMIDLIST**ppidl, UINT nFlags, LPCTSTR pszTitle)
{
   BEGIN("GetFolderPath");
   FolderCallBackStruct fcs = {"", nFlags};
   BROWSEINFO bi;
   bi.hwndOwner = AfxGetMainWnd()->m_hWnd;
   
   HRESULT hResult = ::SHGetSpecialFolderLocation(bi.hwndOwner, CSIDL_DESKTOP, (ITEMIDLIST**)&bi.pidlRoot);
   if (hResult == NOERROR)
   {
      strcpy(fcs.pszPath, LPCTSTR(strFolderPath));
      int nlen = strlen(fcs.pszPath);
      if (fcs.pszPath[nlen-1] == '\\') fcs.pszPath[nlen-1] = 0;
      bi.pszDisplayName = fcs.pszPath;
      bi.lpszTitle      = pszTitle;
      if (nFlags & FOLDERPATH_RETURN_FILES)
         bi.ulFlags     = BIF_BROWSEINCLUDEFILES;
      else
         bi.ulFlags     = BIF_RETURNONLYFSDIRS;
      bi.lpfn           = CAC_COMApp::BrowseFolderCallback;
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

int CAC_COMApp::GetColorDiff(DWORD c1, DWORD c2)
{
   return (abs(GetRValue(c1) - GetRValue(c2)) + 
           abs(GetGValue(c1) - GetBValue(c2)) +
           abs(GetBValue(c1) - GetBValue(c2))) / 3;
}


LRESULT CAC_COMApp::ProcessWndProcException(CException* e, const MSG* pMsg)
{
   if ((pMsg->message == 0) && (pMsg->wParam != 0))
   {
      if (e->IsKindOf(RUNTIME_CLASS(CArchiveException)))
      {
         ((CArchiveException*)e)->m_strFileName = _T((char*)pMsg->wParam);
      }
   }
   e->ReportError();
   e->Delete();
   return 0;
}

void CAC_COMApp::OnAppAbout() 
{
   CCaraInfo CaraInfo;     // eine lokale Instanz der DLL-Klasse
   char exename[32];
   strcpy(exename, m_pszExeName);
   strcat(exename, ".exe");
   CCaraInfo::CallInfoResource(m_pMainWnd->m_hWnd, exename);
   CaraInfo.Destructor();
}

BOOL CAC_COMApp::PreTranslateMessage(MSG* pMsg) 
{
	if (m_hAcc)
   {
      ASSERT(m_pMainWnd != NULL);
	   ::TranslateAccelerator(m_pMainWnd->m_hWnd, m_hAcc, pMsg);
   }
	return CWinApp::PreTranslateMessage(pMsg);
}


int CAC_COMApp::ExitInstance() 
{
   WriteProfileInt(REGKEY_DEFAULT, MB_FLAGS, gm_dwMessageBoxFlags);
	return CWinApp::ExitInstance();
}

int CAC_COMApp::Run() 
{
   CEtsCDPlayer EtsCDPlayer;

	int nResult = CWinApp::Run();

   EtsCDPlayer.Destructor();

	return nResult;
}
