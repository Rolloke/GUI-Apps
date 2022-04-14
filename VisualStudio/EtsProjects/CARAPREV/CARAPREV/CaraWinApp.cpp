// CaraWinApp.cpp: Implementierungsdatei
//
#include "stdafx.h"
#include "CaraWinApp.h"
#include "CaraFrameWnd.h"
#include "CaraDoc.h"
#include "CaraPreview.h"
#include "Label.h"
#include "CurveLabel.h"
#include "PlotLabel.h"
#include "BezierLabel.h"
#include "CircleLabel.h"
#include "CARAPREV.h"
#include "Curve.h"
#include "resource.h"
#include "ETS3DGLRegKeys.h"
#include "CEtsMapi.h"
#include "CEtsHelp.h"
#include "EtsBind.h"
#include "CCaraInfo.h"

#include <eh.h>
#include <locale.h>

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef ETSDEBUG_REPORT
 extern CEtsDebug g_etsdebuginstance;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PRINTER_NAME                   "Name"
#define PRINTER_PARAMETER              "Parameter"
#define REGKEY_DEFAULT_DIRECTORY       "Directory"

extern TCHAR g_szAppName[16];

static const int nMaxMRUFiles = (ID_FILE_MRU_LAST-ID_FILE_MRU_FILE1)+1;

class CCallDestructor
{
public:
    CCallDestructor(){};
    ~CCallDestructor();
private:
    CEtsMapi EtsMapi;
    CEtsHelp EtsHelp;
    CEtsDiv  EtsDiv;
};



CCallDestructor calldestructor;

CCallDestructor::~CCallDestructor()
{
    EtsMapi.Destructor();
    EtsHelp.Destructor();
    EtsDiv.Destructor();
}



CUndoParams::CUndoParams()
{
   ZeroMemory(this, sizeof(CUndoParams));
}

CUndoParams::~CUndoParams()
{
   if (pszFileName)
   {
      ::DeleteFile((LPTSTR)pszFileName);
      free(pszFileName);
   }
}

void CUndoParams::CreateUndoFileName()
{
   TCHAR szTempPath[MAX_PATH];
   TCHAR szTempFile[MAX_PATH];
   if (!GetTempPath(MAX_PATH, szTempPath))
 		AfxThrowFileException(CFileException::badPath, GetLastError(), szTempPath);
   if (!GetTempFileName(szTempPath, "Udo", 0, szTempFile))
 		AfxThrowFileException(CFileException::badPath, GetLastError(), szTempFile);

   pszFileName = _tcsdup(szTempFile);

   if (pszFileName == NULL)
	   AfxThrowFileException(CFileException::genericException, GetLastError(), szTempFile);
}

CCaraSharedFile::CCaraSharedFile(UINT nAllocFlags, UINT nGrowBytes)
	: CSharedFile(nAllocFlags, nGrowBytes)
{
}

BYTE* CCaraSharedFile::Alloc(DWORD dwBytes)
{
   TestMemory(dwBytes);
   return CSharedFile::Alloc(dwBytes);
}

BYTE* CCaraSharedFile::Realloc(BYTE*, DWORD dwBytes)
{
   TestMemory(dwBytes);
   return CSharedFile::Realloc(NULL, dwBytes);
}

bool CCaraSharedFile::TestMemory(DWORD dwBytes)
{
   DWORD dwSize     = CCaraWinApp::GetTotalPhysicalMem();
   DWORD dwTestSize = dwSize;
   if (m_bAllowGrow > 1) dwTestSize = dwSize*m_bAllowGrow;
   if (dwBytes == 0xffffffff) dwBytes = m_nBufferSize;
   if (dwBytes > dwTestSize)
   {
      if (dwBytes >= CCaraWinApp::GetTotalVirtualMem())
      {
         m_bAllowGrow = false;
         return false;
      }
      else
      {
         CString str;
         dwBytes>>=20;
         dwSize>>=20;
         str.Format(IDS_FORMAT_LARGE_MEM_REQUEST, dwBytes, dwSize);
         if (AfxMessageBox(str, MB_YESNO|MB_ICONEXCLAMATION) == IDNO)
         {
            m_bAllowGrow = false;
            return false;
         }
         else m_bAllowGrow+=2;
      }
   }
   return true;
}

IMPLEMENT_DYNAMIC(CCaraSharedFile, CSharedFile)

/////////////////////////////////////////////////////////////////////////////
// CCaraWinApp
OSVERSIONINFO CCaraWinApp::gm_OsVersionInfo = {sizeof(OSVERSIONINFO) , 0, 0, 0, 0, ""};
MEMORYSTATUS  CCaraWinApp::gm_MemStatus = {sizeof(MEMORYSTATUS), 0, 0, 0, 0, 0, 0, 0};
HWND          CCaraWinApp::gm_hCurrentTmeWnd = NULL;

IMPLEMENT_DYNAMIC(CCaraWinApp, CWinApp)

CCaraWinApp::CCaraWinApp()
{
   BEGIN("CCaraWinApp");
   m_pCmdInfo   = NULL;
   m_pPrtD      = NULL;
   m_pPrtParams = NULL;
   memset(&m_Margins, 0, sizeof(m_Margins));
   m_Margins.nScaleLabel=RESTORE_POSITIONS;
   m_bUndoListOverflow = false;
   m_nDeleteCount      = 1;
   m_hCallingWindowHandle  = NULL;
   m_nCallingWindowMessage = 0;
   m_CallingWindowwParam   = 0;
   m_CallingWindowlParam   = ::GetCurrentProcessId();
   m_bNoUndo = false;
   m_bToolTipInit = false;
}

CCaraWinApp::~CCaraWinApp()
{
   EndPrinting();                                              // Druckauftrag beenden
   DeletePrinterDLL();                                         // Printer DLL entfernen
   ETSDEBUGEND;                                                // Debug DLL entfernen
   AfxTermLocalData(NULL, true);
}

void CCaraWinApp::OnAppAbout()
{
   if (m_pMainWnd)
   {
      CCaraInfo CaraInfo;                                      // eine lokale Instanz der Info-DLL-Klasse
      TCHAR exename[32];
      strcpy(exename, m_pszExeName);
      strcat(exename, ".exe");
      CCaraInfo::CallInfoResource(m_pMainWnd->m_hWnd, exename);
      CaraInfo.Destructor();
   }
}

BOOL CCaraWinApp::InitInstance()
{
//   BEGIN("InitInstance()");
   CWinApp::InitInstance();
   TCHAR  szFileName[_MAX_PATH];
   DWORD dwDummy = 0;
   wsprintf(szFileName, _T("%s.EXE"), m_pszExeName);               // Die Dateiversion des Programms ermitteln

   int bytes    = GetFileVersionInfoSize(szFileName, &dwDummy);
   int nVersion = 1;
   int nSubVers = 0;
   if(bytes)
   {
      void * buffer = new TCHAR[bytes];
      GetFileVersionInfo(szFileName, 0, bytes, buffer);
      VS_FIXEDFILEINFO * Version;
      unsigned int     length;
      if(VerQueryValue(buffer, _T("\\"),(void **) &Version, &length))
      {                                                        // Bsp.: Version 1.20 => 0x00010014
         nVersion = HIWORD(Version->dwFileVersionMS);
         nSubVers = LOWORD(Version->dwFileVersionMS);
         if ((nSubVers > 0) && (nSubVers < 10)) nSubVers *= 10;
      }
      delete[] buffer;
   }
   
#ifdef ETS_OLE_SERVER
   REPORT("Ole Server Support");
#endif

   g_nFileVersion = 123;                                       // diese Fileversion gilt für die Haupt-Dokumentendateien

   CCaraInfo CaraInfo;                                         // eine lokale Instanz der DLL-Klasse
   switch (CaraInfo.GetPackageID())
   {
      case 0x434f4f4c:  // COOL  : alte Version 2.00
      case 0x504c5553:  // PLUS  : CARA Plus Auralisation
      case 0x50524f46:  // PROF  : Profiversion
         m_bLightVersion = false;
         break;
      case 0x45415359:  // EASY  : Lightversionen mit eingeschränkter funktionalität
      case 0x41434c48:  // ACLH
      default:          // keine Dll
         m_bLightVersion = true;
         break;
   }
   m_bLightVersion = false;

   CaraInfo.Destructor();

   SetRegistryKey(_T("ETSKIEL"));                              // Firmenname
   if (m_pszProfileName)
   {
      TCHAR profilename[_MAX_PATH];                             // und Versionsnummer
      wsprintf(profilename, _T("%s\\V%d.%02d"), m_pszProfileName, nVersion, nSubVers);
      free((void*)m_pszProfileName);
      m_pszProfileName = _tcsdup(profilename);
   }
/*
   if (!WriteProfileInt(REGKEY_DEFAULT, "Test", 1))            // Kein Zugriff
   {
      TCHAR profilename[_MAX_PATH];
      free((void*)m_pszRegistryKey);                           // RegistryKey löschen
      m_pszRegistryKey = NULL;
      free((void*)m_pszProfileName);
      GetWindowsDirectory(profilename, _MAX_PATH); 
      strncat(profilename, "\\", _MAX_PATH);
      strncat(profilename, m_pszAppName, _MAX_PATH);
      strncat(profilename, ".ini", _MAX_PATH);                 // Inidateipfad erzeugen
      m_pszProfileName = _tcsdup(profilename);
   }
*/
   VERIFY(::GetVersionEx(&gm_OsVersionInfo)!=0);               // Betriebssystem ermitteln
   ::GlobalMemoryStatus(&gm_MemStatus);                        // Speicherstatus ermitteln

   ParseCommandLine(m_CmdInfo);                                // Kommandozeile auswerten

   SetCurrentProgramDir();                                     // Programmverzeichnis setzen

   BYTE * byteptr=NULL;
   UINT   size;                                                // Custom Colors für Farbdialog setzen
   GetProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_CUST_COL, &byteptr, &size);
   if (byteptr)
   {
      if (size == sizeof(COLORREF)*16)
      {
         memcpy(g_CustomColors, byteptr, size);
      }
      delete[] byteptr;
   }                                                           // Hintergrungfarbe des Hauptfensters
   m_Backgroundcolor    = (COLORREF) GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_BKCOLOR, (int)RGB(255,255,255));
   m_strDefaultDir      = GetProfileString(REGKEY_DEFAULT, REGKEY_DEFAULT_DIRECTORY);
                                                               // Bugfix für fehlerhafte Grafikkarten
   CLabel::gm_bFillPath = GetProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_FILLPATH, SETTINGS_FILLPATH_DEFAULT) ? true : false;

   int nMax = GetProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_MAX_MRU_FILES, 6);
   if (nMax >  nMaxMRUFiles) nMax = nMaxMRUFiles;              // Most Recent Files
   if (nMax <=            0) nMax = 1;
   LoadStdProfileSettings(nMax);    // Standard-INI-Dateioptionen einlesen (einschließlich MRU)

   m_strPreviewHelpPath.Empty();
   if (m_pszHelpFilePath)                                      // Haupt Hilfedatei des ETS2DV-Moduls
   {
      TCHAR *ptr = (TCHAR*)_tcsstr(m_pszHelpFilePath, "ETS2DV.HLP");
      if (ptr == NULL)                                         // 2DV-Modul braucht es nicht
      {
         ptr = (TCHAR*)_tcsstr(m_pszHelpFilePath, ".HLP");              // nach der Endung .HLP suchen
         if (ptr)
         {
            int i;
            const TCHAR *pBegin = m_pszHelpFilePath;            // nach dem Backslash suchen
            for (i=0; ((&ptr[i]) > pBegin) && (ptr[i] != '\\'); i--);
            ptr[i] = 0;                                        // ETS2DV.HLP einfügen
            m_strPreviewHelpPath = _T(m_pszHelpFilePath) + CString("\\ETS2DV.HLP");
            ptr[i] = '\\';
         }
      }
   }

   for (int i = 1; i < __argc; i++)                            // Eigene Auswertung der Commandline
   {
      LPCTSTR pszParam = __targv[i];
      if (pszParam[0] == ':')                                  // eigene Parameter suchen
      {
         switch (pszParam[1])
         {
            case 'H':                                          // Handle of Calling Window
                m_hCallingWindowHandle  = (HWND) atoi(&pszParam[2]);
               break;
            case 'M':                                          // Expected Message Identifier
                m_nCallingWindowMessage = atoi(&pszParam[2]);
               break;
            case 'W':                                          // wParam
                m_CallingWindowwParam   = atoi(&pszParam[2]);
               break;
            case 'L':                                          // lParam
                m_CallingWindowlParam   = atoi(&pszParam[2]);
               break;
            default :
               break;
         }
      }
   }

   m_bMailAvailable = CEtsMapi::IsAvailable();                 // Ist der E-Mail-Client vorhanden
//   REPORT("E-Mail Client: %d", m_bMailAvailable);

   TCHAR * pszLocale;                                           // Länderspezifische Einstellungen
   TCHAR szLocale[32];                                          // ermitteln
   ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, szLocale, 32);
   pszLocale = setlocale(LC_ALL, szLocale);                    // und setzen
   REPORT("%s", pszLocale);
//   TCHAR szPath[_MAX_PATH];
//   ETSGetCommonPath(szPath, _MAX_PATH);

   return TRUE;
}

int CCaraWinApp::ExitInstance()
{
   BEGIN("CCaraWinApp::ExitInstance");
   WriteProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_CUST_COL, (BYTE*)g_CustomColors, sizeof(COLORREF)*16);
   WriteProfileInt(REGKEY_DEFAULT, REGKEY_DEFAULT_BKCOLOR, (int) m_Backgroundcolor);
   WriteProfileString(REGKEY_DEFAULT, REGKEY_DEFAULT_DIRECTORY, m_strDefaultDir);

   CLabel::DeleteGraficObjects(true);                          // Grafikobjekt cash löschen
   CBezierLabel::ResetGlobalObjects();
   CCircleLabel::ResetGlobalObjects();
   CLineLabel::ResetGlobalObjects();
   CRectLabel::ResetGlobalObjects();
   CTextLabel::ResetGlobalObjects();

   DeleteUndoList();                                           // Undoliste löschen

   PostMsgToCallingWnd();                                      // Aufrufendes Fenster über Programmende benachrichtigen

#ifndef _DEBUG
   if (CLabel::HeapWatch()) CLabel::DestroyHeap();             // CLabel eigenen Heap löschen
#endif

   if (CCurve::HeapWatch()) CCurve::DestroyHeap();             // CCurve eigenen Heap löschen

   return CWinApp::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCaraWinApp, CWinApp)
	//{{AFX_MSG_MAP(CCaraWinApp)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCaraWinApp 

/******************************************************************************
* Name       : GetCaraPrinterDefaults                                         *
* Definition : bool GetCaraPrinterDefaults(CPrintInfo *);                     *
* Zweck      : Defaulteinstellungen für CPrintInfo ermitteln.                 *
* Aufruf     : GetCaraPrinterDefaults(pInfo);                                 *
* Parameter  :                                                                *
* pInfo  (EA):                                                  (CPrintInfo*) *
* Funktionswert : true, false                                   (bool)        *
******************************************************************************/
bool CCaraWinApp::GetCaraPrinterDefaults(CPrintInfo * pInfo)
{
   if (m_pPrtD == NULL)                                        // Drucker noch nicht initialisiert
   {
      LoadPrinterSettings();
   }
   if (m_pPrtD)
   {
      if (pInfo)
      {  // Das Flag m_bDocObject unterdrückt das initialisieren des HDC durch den
         // MFC-Anwendungsrahmen (StartDoc, EndDoc). Dies muß dann in OnBeginPrinting
         // und OnEndPrinting durchgeführt werden.
         if (pInfo->m_bPreview) pInfo->m_bDocObject = false;
         else                   pInfo->m_bDocObject = true;
         pInfo->m_lpUserData = &m_Margins;                     // Randeinstellungen
      }
      return true;
   }
   else                                                        // Kein Drucker
   {
      AfxMessageBox(IDS_NO_PRINTER|MB_ICONINFORMATION);
      return false;
   }
}

/******************************************************************************
* Name       : IsAPrinterAvailabel                                            *
* Definition : bool IsAPrinterAvailabel();                                    *
* Zweck      : Ermittelt, ob ein Druckertreiber installiert ist               *
* Aufruf     : IsAPrinterAvailabel();                                         *
* Parameter  :                                                                *
* Funktionswert : true, false                                   (bool)        *
******************************************************************************/
bool CCaraWinApp::IsAPrinterAvailabel()
{
   if (m_pPrtD) return true;                                   // Ok
   else
   {
      CEtsPrtD prtd;                                           // lokale Instanz
      ETSPRTD_PARAMETER params;                                // Struktur mit den Übergabeparametern zu der DLL
      ZeroMemory(&params,sizeof(params));
      params.dwFlags = ETSPRTD_QUIET;                          // damit keine Meldung ausgegeben wird !
      bool bReturn = (CEtsPrtD::DoDataExchange(&params)==0) ? true : false;// ein Drucker ist vorhanden
      prtd.Destructor();                                       // sofort wieder entfernen
      return bReturn;
   }
}

/******************************************************************************
* Name       : GetPrinter                                                     *
* Definition : bool GetPrinter();                                             *
* Zweck      : Laden der Druckereinstellungen                                  *
* Aufruf     : GetPrinter();                                                  *
* Parameter  :                                                                *
* Funktionswert : true, false                                   (bool)        *
******************************************************************************/
bool CCaraWinApp::GetPrinter()
{
   if (m_pPrtD) return true;
   else         LoadPrinterSettings();
   return (m_pPrtD) ? true : false;
}

/******************************************************************************
* Name       : LoadPrinterSettings                                            *
* Definition : void LoadPrinterSettings();                                    *
* Zweck      : Laden und initialisieren des Druckers                          *
* Aufruf     : LoadPrinterSettings();                                         *
* Parameter  :                                                                *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWinApp::LoadPrinterSettings()
{
   BYTE * byteptr=NULL;
   UINT   size;
   if ((m_pPrtD                   != NULL                          ) && // Drucker schon geladen UND
       ((m_CmdInfo.m_nShellCommand == CCommandLineInfo::FilePrint  ) || // Shellkommando Print ODER PrintTo ODER Open
        (m_CmdInfo.m_nShellCommand == CCommandLineInfo::FilePrintTo) || // Druckereinstellungen des Dokumentes nehmen
        (m_CmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen   ))) return;// und hier nichts tun

   bool bPrinterOk = false;
   if (m_pPrtD == NULL) m_pPrtD = new CEtsPrtD;                // Instanz anlegen
   if (m_pPrtD)
   {                                                           // Registryparameter laden
      GetProfileBinary(PRINTER, PRINTER_PARAMETER, &byteptr, &size);
      if (byteptr)                                             // vorhanden ?
      {                                                        // Printerdaten setzen
         if (size > 0) bPrinterOk = SetPrinterData(byteptr, size);
         delete[] byteptr;
      }
      if (!bPrinterOk)                                         // alter Printer konnte nicht gesetzt werden ?
      {
         REPORT("Registry-Printerparameter not set");
         ETSPRTD_PARAMETER params;
         ZeroMemory(&params, sizeof(ETSPRTD_PARAMETER));       // einen anderen Drucker nehmen
         bPrinterOk = (CEtsPrtD::DoDataExchange(&params)==0) ? true : false;
      }
      if (!bPrinterOk)                                         // keiner gefunden
      {
         REPORT("No Printer");                                 // Pech gehabt
         DeletePrinterDLL(); 
      }
   }
}

/******************************************************************************
* Name       : SavePrinterSettings                                            *
* Definition : void SavePrinterSettings();                                    *
* Zweck      : Druckereinstellungen in der Registry speichern                 *
* Aufruf     : SavePrinterSettings();                                         *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWinApp::SavePrinterSettings()
{
   if ((m_CmdInfo.m_nShellCommand == CCommandLineInfo::FilePrintTo) ||
       (m_CmdInfo.m_nShellCommand == CCommandLineInfo::FilePrint))
   {  // Hier würden sonst die Einstellungen des Dokumentes gespeichert werden.
      return;
   }
   if (m_pPrtD)                                                // Drucker vorhanden ?
   {
      DWORD dwSize;
      BYTE *pParams = GetPrinterData(dwSize);                  // Strukturdaten ermitteln
      if (pParams)
      {                                                        // und speichern
         WriteProfileBinary(PRINTER, PRINTER_PARAMETER, pParams, dwSize);
         delete[] pParams;
      }
   }
}

/******************************************************************************
* Name       : AllocatePrinterParams                                          *
* Definition : bool AllocatePrinterParams();                                  *
* Zweck      : Allocieren des Speicher für die Druckerdaten                   *
* Aufruf     : AllocatePrinterParams();                                       *
* Parameter  :                                                                *
* Funktionswert : true, false                                   (bool)        *
******************************************************************************/
bool CCaraWinApp::AllocatePrinterParams()
{
   if (m_pPrtParams == NULL)                                   // noch nicht vorhanden
   {
      m_pPrtParams = new ETSPRTD_PARAMETER;                    // allocieren
      if (m_pPrtParams)
      {
         ZeroMemory(m_pPrtParams, sizeof(ETSPRTD_PARAMETER));  // initialisieren
         m_pPrtParams->pPrinterData = (void*) new ETSPRTD_PRINTER;
         if (m_pPrtParams->pPrinterData)
         {
            ZeroMemory(m_pPrtParams->pPrinterData, sizeof(ETSPRTD_PRINTER));
            return true;
         }
      }
   }
   else                                                        // reinitialisieren
   {
      void *pData = m_pPrtParams->pPrinterData;
      ASSERT(pData != NULL);
      ASSERT(m_pPrtParams->dwSize == sizeof(ETSPRTD_PRINTER));
      ZeroMemory(m_pPrtParams, sizeof(ETSPRTD_PARAMETER));
      ZeroMemory(pData, sizeof(ETSPRTD_PRINTER));
      m_pPrtParams->pPrinterData = pData;
      return true;
   }
   return false;
}

/******************************************************************************
* Name       : CreateCaraPrinterDC                                            *
* Definition : HDC CreateCaraPrinterDC(bool);                                 *
* Zweck      : Stellt den Drucker devicekontext zur Verfügung                 *
* Aufruf     : CreateCaraPrinterDC(bPreview);                                 *
* Parameter  :                                                                *
* bPreview (E): Für Preview erstellen (true, false)                  (bool)   *
* Funktionswert : Devicecontext des Druckers                         (HDC)    *
******************************************************************************/
HDC CCaraWinApp::CreateCaraPrinterDC(bool bPreview)
{
   HDC hDC = NULL;
   if (GetPrinter())                                           // Drucker vorhanden
   {                                                           // Daten schon initialsiert ?
      if ((m_pPrtParams != NULL) && (m_pPrtParams->pPrinterData != NULL))
      {
         hDC = ((ETSPRTD_PRINTER*)m_pPrtParams->pPrinterData)->hDC;// HDC holen
      }
      else                                                     // initialisieren
      {
         if (AllocatePrinterParams())                          // Alloziert
         {                             // DC holen         in lowmetrics      ohne Dialogaufruf
            m_pPrtParams->dwFlags       = ETSPRTD_RETURNDC|ETSPRTD_LOWMETRICS|ETSPRTD_QUIET;
            if (bPreview) m_pPrtParams->dwFlags |= ETSPRTD_4MFCPREVIEW;// für Previev erzeugen
            ASSERT(m_pMainWnd != NULL);
            m_pPrtParams->hParent       = m_pMainWnd->m_hWnd;  // Mainwnd ist Parent
            m_pPrtParams->dwSize        = sizeof(ETSPRTD_PRINTER);
            m_pPrtParams->szApplication = m_pszAppName;        // Name der Anwendung
            CCARADoc *pDoc = (CCARADoc*) ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();
            if (pDoc) m_pPrtParams->szDocument = pDoc->GetDocName();// Name des Dokumentes

            if (CEtsPrtD::DoDataExchange(m_pPrtParams) == 0)   // Daten holen
            {
               SetPageParams(m_pPrtParams);                    // Randeinstellungen setzen
               return ((ETSPRTD_PRINTER*)m_pPrtParams->pPrinterData)->hDC;
            }
            DeletePrinterParams();                             // Fehler ! Druckerdaten löschen
            REPORT("No Printer DC");
         }
      }
   }
   return hDC;
}
 
/******************************************************************************
* Name       : GetPageSettings                                                *
* Definition : void GetPageSettings();                                        *
* Zweck      : Ermitteln der Seitenränder                                     *
* Aufruf     : GetPageSettings();                                             *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWinApp::GetPageSettings()
{
   ETSPRTD_PARAMETER Params;
   ETSPRTD_PRINTER   Data;
   ZeroMemory(&Params, sizeof(ETSPRTD_PARAMETER));             // initialisieren
   ZeroMemory(&Data  , sizeof(ETSPRTD_PRINTER));
   Params.pPrinterData = &Data;
   Params.dwSize      = sizeof(ETSPRTD_PRINTER);
   Params.dwFlags     = ETSPRTD_LOWMETRICS;                    // in lowmetrics
   if (CEtsPrtD::DoDataExchange(&Params) == 0)                 // Daten holen
   {
      SetPageParams(&Params);                                  // Randeinstellungen setzen
   }
}

/******************************************************************************
* Name       : SetPageParams                                                  *
* Definition : void SetPageParams(ETSPRTD_PARAMETER *);                       *
* Zweck      : Setzen der Seitenränder und des Druckeroffsets                 *
* Aufruf     : SetPageParams(pParam);                                         *
* Parameter  :                                                                *
* pParam (E) : Druckerparameter                          (ETSPRTD_PARAMETER*) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWinApp::SetPageParams(ETSPRTD_PARAMETER *pParam)
{
   ETSPRTD_PRINTER *pData = (ETSPRTD_PRINTER *) pParam->pPrinterData;

   ETSPRTD_PARAMETER param;                                    // Druckeroffset
   ZeroMemory(&param, sizeof(ETSPRTD_PARAMETER));
   param.dwFlags = ETSPRTD_PRTOFFSET|ETSPRTD_LOWMETRICS;       // in lowmetric ermitteln
   param.dwSize  = sizeof(POINT);
   DWORD dwOffsetCorrection = GetProfileInt(PRINTER, OFFSET_CORRECTION, 0);
   if (dwOffsetCorrection)                                     // Korrekturwert in der Registry ?
   {
      param.pPrinterData = &m_Margins.OffsetLoMetric;
      m_Margins.OffsetDev.x = m_Margins.OffsetDev.y = 0;
      CEtsPrtD::DoDataExchange(&param);
      m_Margins.OffsetLoMetric.x -= (short)LOWORD(dwOffsetCorrection);// x-Korrektur
      m_Margins.OffsetLoMetric.y -= (short)HIWORD(dwOffsetCorrection);// y-Korrektur
   }
   else
   {
      param.pPrinterData = &m_Margins.OffsetLoMetric;
      CEtsPrtD::DoDataExchange(&param);                        // in lowmetric ermitteln
      param.dwFlags &= ~ETSPRTD_LOWMETRICS;
      param.dwFlags |= ETSPRTD_DEVICEUNITS;
      param.pPrinterData = &m_Margins.OffsetDev;
      CEtsPrtD::DoDataExchange(&param);                        // in devcoord ermitteln
   }
                                                               // Rechtecke für die Seiten berechnen
   m_Margins.rcDrawLoMetric.left   =   pData->rcMargin.left;   // Zeichenrechteck
   m_Margins.rcDrawLoMetric.top    = - pData->rcMargin.top;
   m_Margins.rcDrawLoMetric.right  =   pData->siPageSize.cx - pData->rcMargin.right;
   m_Margins.rcDrawLoMetric.bottom = -(pData->siPageSize.cy - pData->rcMargin.bottom);
   
   m_Margins.rcPageLoMetric.left   =   0;                      // Seitenrechteck
   m_Margins.rcPageLoMetric.top    =   0;
   m_Margins.rcPageLoMetric.right  =   pData->siPageSize.cx;
   m_Margins.rcPageLoMetric.bottom = - pData->siPageSize.cy;
}

/******************************************************************************
* Name       : OnFilePrintSetup                                               *
* Definition : void OnFilePrintSetup();                                       *
* Zweck      : Aufrufen des Druckerdialogs für Einstellungen am Drucker       *
* Aufruf     : OnFilePrintSetup();                                            *
******************************************************************************/
void CCaraWinApp::OnFilePrintSetup()
{
   if (DoCaraPrintDialog(true)==IDOK)                          // Dialog für Einstellungen
   {
      LPARAM lParam = UPDATE_PRINT_SETTINGS;                   // Druckereinstellungen anpassen
      CView     *pView = ((CFrameWnd*)m_pMainWnd)->GetActiveView();
      if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARAPreview))) lParam |= UPDATE_PREVIEW_LABEL;
      CDocument *pDoc  = ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();
      if (pDoc) pDoc->UpdateAllViews(NULL, lParam, (CObject*)&m_Margins);
      SavePrinterSettings();                                   // in der Registry speichern
   }
}

/******************************************************************************
* Name       : DoCaraPrintDialog                                              *
* Definition : int DoCaraPrintDialog(bool );                                  *
* Zweck      : Druckerdialog aufrufen                                         *
* Aufruf     : DoCaraPrintDialog(bSetup);                                     *
* Parameter  :                                                                *
* bSetup (E) : nur Setup aufrufen (true, false)                      (bool)   *
* Funktionswert : (IDOK, IDCANCEL)                                   (int)    *
******************************************************************************/
int CCaraWinApp::DoCaraPrintDialog(bool bSetup)
{
   if (GetPrinter())                                           // Drucker vorhanden
   {
      if (AllocatePrinterParams())                             // initialisieren
      {
         m_pPrtParams->dwSize        = sizeof(ETSPRTD_PRINTER);
         m_pPrtParams->dwFlags       = ETSPRTD_LOWMETRICS;
         if (!bSetup) m_pPrtParams->dwFlags |= ETSPRTD_RETURNDC;// HDC erzeugen, wenn kein Setup

         ASSERT(m_pMainWnd != NULL);
         m_pPrtParams->hParent       = m_pMainWnd->m_hWnd;     // Anwendungsdaten übergeben
         m_pPrtParams->szApplication = m_pszAppName;           // Anwendungsname
         CView     *pView = ((CFrameWnd*)m_pMainWnd)->GetActiveView();
         if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARAPreview)))// Preview ?
         {                                                     // Vorschau Metafile erzeugen
            m_pPrtParams->hPreView = ((CCARAPreview*)pView)->GetPreviewMetaFile();
            m_pPrtParams->dwFlags |= ETSPRTD_ANISO;            // anisotropische Darstellung
         }
         else m_pPrtParams->dwFlags |= ETSPRTD_NOPREVIEW;

         CCARADoc *pDoc = (CCARADoc*) ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();
         if (pDoc) m_pPrtParams->szDocument = pDoc->GetDocName();// Dokumentenname

         if (((CRect*)&m_Margins.rcDrawLoMetric)->IsRectEmpty())// Seiteneinstellungen ermitteln
         {
            GetPageSettings();
         }
         m_Margins.rcOldDrawLoMetric = m_Margins.rcDrawLoMetric;// alte Einstellungen merken (Vergleich)
         if (CEtsPrtD::DoDialog(m_pPrtParams) == 0)
         {
            if (m_pPrtParams->hPreView) DeleteEnhMetaFile(m_pPrtParams->hPreView);
            SetPageParams(m_pPrtParams);                       // Seiteneinstellungen setzen
            CheckPageScaling();                                // Seiten einstellungen überprüfen
            pDoc->DeleteDocPrtParam();                         // Druckereinstellungen des Dokumente löschen
            if (bSetup) DeletePrinterParams();                 // bei Setup: Druckerdaten löschen
            return IDOK;
         }
         DeletePrinterParams();                                // bei Fehler: Druckerdaten löschen 
      }
   }
   return IDCANCEL;
}

/******************************************************************************
* Name       : EndPrinting                                                    *
* Definition : void EndPrinting(bool);                                        *
* Zweck      : Druckvorgang beenden                                           *
* Aufruf     : EndPrinting(bSetup);                                           *
* Parameter  :                                                                *
* bSetup (E) : Bei Setup EndPrinting der Dll nicht aufrufen          ( bool)  *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWinApp::EndPrinting(bool bSetup)
{
   if (m_pPrtParams && m_pPrtParams->pPrinterData)
   {
      if (!bSetup) CEtsPrtD::EndPrinting(m_pPrtParams);
   }
   DeletePrinterParams();
}

/******************************************************************************
* Name       : DeletePrinterParams()                                          *
* Definition : void DeletePrinterParams();                                    *
* Zweck      : Löschen der Druckerdaten                                       *
* Aufruf     : DeletePrinterParams()                                          *
******************************************************************************/
void CCaraWinApp::DeletePrinterParams()
{
   if (m_pPrtParams)
   {
      if (m_pPrtParams->pPrinterData)
      {
         delete m_pPrtParams->pPrinterData;
      }
      delete m_pPrtParams;
      m_pPrtParams = NULL;
   }
}

/******************************************************************************
* Name       : GetPrinterData                                                 *
* Definition : BYTE * GetPrinterData(DWORD &);                                *
* Zweck      : Liefert die Druckerdaten zum speichern                         *
* Aufruf     : BYTE * CCaraWinApp::GetPrinterData(dwSize);                    *
* Parameter  :                                                                *
* dwSize (EA): Länge der Daten                                       (DWORD&) *
* Funktionswert : Zeiger auf die Daten                               (BYTE*)  *
******************************************************************************/
BYTE * CCaraWinApp::GetPrinterData(DWORD &dwSize)
{
   ETSPRTD_PARAMETER params;
   ZeroMemory(&params, sizeof(ETSPRTD_PARAMETER));
   params.dwFlags = ETSPRTD_GETALLDATA;
   if (CEtsPrtD::DoDataExchange(&params) == 0)
   {
      BYTE * byteptr = new BYTE[params.dwSize];
      if (byteptr)
      {
         params.pPrinterData = byteptr;
         if (CEtsPrtD::DoDataExchange(&params) == 0)
         {
            dwSize = params.dwSize;
            return byteptr;
         }
         else delete[] byteptr;
      }
   }
   dwSize = 0;
   REPORT("Fehler :ETSPRTD_GETALLDATA");
   return NULL;
}

/******************************************************************************
* Name       : SetPrinterData                                                 *
* Definition : bool SetPrinterData(BYTE *, DWORD);                            *
* Zweck      : Setzen der Druckerdaten.                                       *
* Aufruf     : SetPrinterData(pData, dwSize);                                 *
* Parameter  :                                                                *
* pData  (E) : Zeiger auf die Daten                                  (BYTE*)  *
* dwSize (E) : Länge der Daten                                       (DWORD)  *
* Funktionswert : (Druckerdaten gesetzt (true, false)                (bool)   *
******************************************************************************/
bool CCaraWinApp::SetPrinterData(BYTE *pData, DWORD dwSize)
{
   bool bAllocated = false;
   ETSPRTD_PARAMETER params;
   ZeroMemory(&params, sizeof(ETSPRTD_PARAMETER));
   params.dwFlags      = ETSPRTD_SETALLDATA;
   params.dwSize       = dwSize;
   params.pPrinterData = pData;

   if (m_CmdInfo.m_nShellCommand == CCommandLineInfo::FilePrintTo)
      return false;                                            // nicht den Drucker des Dokumentes nehmen !

   if (m_pPrtD == NULL)                                        // nicht vorhanden
   {
      m_pPrtD = new CEtsPrtD;                                  // allocieren
      bAllocated = true;
   }
   if (!m_pPrtD) return false;                                 // Fehler

   int nResult = CEtsPrtD::DoDataExchange(&params);
   if (nResult == 0) return true;                              // Alles OK

   if (nResult == 2)                                           // Anderer printer, Parameter konnten gesetzt werden
   {
      REPORT("Printer Changed, Parameter set");
      return true;
   }

   if (bAllocated) DeletePrinterDLL();                         // bei Fehler wieder freigeben
   REPORT("Printer-Parameter not set");
   return false;
}
 
/******************************************************************************
* Name       : ReadPrinterData10                                              *
* Definition : bool ReadPrinterData10(CArchive &);                            *
* Zweck      : Lesen der Druckerdaten aus 1.00 Dokumenten                     *
* Aufruf     : ReadPrinterData10(ar);                                         *
* Parameter  :                                                                *
* ar     (E) : Archiv aus dem gelesen wird                       (CArchive&)  *
* Funktionswert : (true, false)                                  (bool)       *
******************************************************************************/
bool CCaraWinApp::ReadPrinterData10(CArchive &ar)
{
   int nLength = 0;
   struct
   {
      RECT  rcMargin;         // für CMarginDialog in mm
      RECT  rcMinMargin;
      SIZE  PaperSize;

      POINT PhysicalOffset;   // zum Drucken in Deviceeinheiten
      POINT DrawOrigin;
      SIZE  DrawSize;
      SIZE  PageSize;

      bool  bHPFix;           // Fix für einige HP-Drucker
      bool  bISO;             // isotropische Abbildung erzwingen
      bool  bFrame;           // Rahmen um Druckbereich zeichnen

      RECT  rcPageLoMetric;   // zum zeichnen im MM_LOMETRIC MapMode
      RECT  rcDrawLoMetric;
      POINT OffsetLoMetric;   // zum Drucken in MM_LOMETRIC

      int   nScaleLabel;      // zum Scalieren der Label
      RECT  rcOldDrawLoMetric;
   } Param;
   
   if ((ar.Read(&nLength, sizeof(int))==sizeof(int)) && (nLength > 0) && (nLength < 256))// wenn vorhanden
   {
      ETSPRTD_PARAMETER   param;
      ETSPRTD_SPECIALDATA special;
      ZeroMemory(&param, sizeof(param));
      param.dwFlags      = ETSPRTD_SETALLDATA|ETSPRTD_LOWMETRICS;
      param.dwSize       = sizeof(special);
      param.pPrinterData = &special;

      ar.Read(special.szPrinterName, nLength);
      if (m_CmdInfo.m_nShellCommand == CCommandLineInfo::FilePrintTo)
         lstrcpy((LPTSTR)special.szPrinterName, m_CmdInfo.m_strPrinterName);

      ar.Read(&Param, sizeof(Param));

      special.rcMargin.left   = Param.rcMargin.left   * 10;
      special.rcMargin.top    = Param.rcMargin.top    * 10;
      special.rcMargin.right  = Param.rcMargin.right  * 10;
      special.rcMargin.bottom = Param.rcMargin.bottom * 10;
      ar.Read(&special.dmOrientation, sizeof(short));// Orientierung laden
      ar.Read(&special.dmPaperSize  , sizeof(short));// Papierformat laden

      switch (CEtsPrtD::DoDataExchange(&param))
      {
         case 3: GetPageSettings();    // Drucker, Ränder, Papier und Orientation  mußten geändert werden
         case 0:                       // Ok
         case 2:                       // Drucker wurde nicht gefunden nur durch ETSPRTD_SETALLDATA aber
            return true;               // alle Ränder und Papiereinstellungen konnten übernommen werden
         case 1: case -1: default:break;
      }
   }
   REPORT("Printerdata 1.00 not set");
   return false;
}

/******************************************************************************
* Name       : SetUndoCmd                                                     *
* Definition : void SetUndoCmd(int, CLabel*, CLabel*, int, CLabel*);          *
* Zweck      : Speichern der Undo Commands in der Undo Liste                  *
* Aufruf     : SetUndoCmd(nCmd, pl, pFrom, nPos, pTo);                        *
* Parameter  :                                                                *
* nCmd   (E) : Kommando, das rückgangig gemacht werden kann                   *
* pl     (E) : geändertes Labelobject                               (CLabel*) *
* pFrom  (E) : bisheriger Container des Labelobjektes               (CLabel*) *
* nPos   (E) : Position in dem bisherigen Container                 (int)     *
* pTo    (E) : Container in den das Labeloject eingefügt wurde      (CLabel*) *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWinApp::SetUndoCmd(int nCmd, CLabel * pl, CLabel * pFrom, int nPos, CLabel * pTo)
{
   if (m_bNoUndo) return;
   if (!pl) return;

   CLabel::SetFileVersion(g_nFileVersion);
   CUndoParams *pUps = new CUndoParams;
   try
   {
      if (nCmd == LAST_CMD_CURVE_EDIT)
      {
         if (pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
         {
            pUps->CreateUndoFileName();
            CFile File(pUps->pszFileName, CFile::modeWrite);
            CArchive ar(&File, CArchive::store);
            ((CCurveLabel*)pl)->SaveEditUndo(ar);
            ar.Flush();
            File.Close();
            pUps->pLabel = pl;
         }
         else ASSERT(false);
      }
      else if (nCmd == LAST_CMD_MOVE)
      {
         pUps->CreateUndoFileName();
         CFile File(pUps->pszFileName, CFile::modeWrite);
         CArchive ar(&File, CArchive::store);
         pl->CLabel::Serialize(ar);
         ar.Flush();
         File.Close();
      }
      else
      {
         pl->ResetPickStates();
         if (nCmd == ID_EDIT_CLEAR)
         {
            pUps->CreateUndoFileName();
            CFile File(pUps->pszFileName, CFile::modeWrite);
            CArchive ar(&File, CArchive::store);
            CLabel::SaveGDIObjects(ar);
            ar.WriteObject(pl);
            ar.Flush();
            File.Close();
            UpdateUndoList(pUps, pl);
         }
         else
         {
            pUps->pLabel = pl;
         }
         pUps->pFrom  = pFrom;
         pUps->nPos   = nPos;
         pUps->pTo    = pTo;
      }
   }
   catch (CException *e)
   {
      TCHAR szCause[255];
      e->GetErrorMessage(szCause, 255);
      REPORT("SetUndo-Fehler:%s", szCause);
      if (pUps) delete pUps;
      return;
   }

   pUps->nCmd   = nCmd;
   m_UndoList.AddTail(pUps);
   
   if (m_UndoList.GetCount() > MAX_UNDO)
   {
      m_bUndoListOverflow = true;
      RemoveUndoListElement(true, true);
   }
}

/******************************************************************************
* Name       : UpdateUndoList                                                 *
* Definition : void UpdateUndoList(CUndoParams*, CLabel*);                    *
* Zweck      : Updaten der Liste, wenn ein Label gelöscht wird                *
* Aufruf     : UpdateUndoList(pUps, pl);                                      *
* Parameter  :                                                                *
* pUps   (EA): Undo Parameter                                  (CUndoParams*) *
* pl     (E) : Label, das gelöscht wurde                       (CLabel*)      *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWinApp::UpdateUndoList(CUndoParams *pUps, CLabel *pl)
{
   POSITION pos =  m_UndoList.GetTailPosition();         // Suchen nach dem Label in der Undoliste

   CUndoParams *pTest;
   while (pos)
   {
      pTest = (CUndoParams*) m_UndoList.GetPrev(pos);
      if (pTest == NULL) break;
      // Pointer gefunden   : LabelPointer  = NULL, Löschnummer vergeben
      if (pTest->pLabel==pl) {pTest->pLabel = NULL; pTest->nLabel = m_nDeleteCount;}
      if (pTest->pFrom ==pl) {pTest->pFrom  = NULL; pTest->nFrom  = m_nDeleteCount;}
      if (pTest->pTo   ==pl) {pTest->pTo    = NULL; pTest->nTo    = m_nDeleteCount;}
   }
   if (pUps)
   {
      pUps->nLabel = m_nDeleteCount;
      m_nDeleteCount++;                                        // Löschnummer erhöhen
   }
}

/******************************************************************************
* Name       : OnEditUndo                                                     *
* Definition : void OnEditUndo();                                             *
* Zweck      : Letze Aktion rückgängig machen                                 *
* Aufruf     : OnEditUndo();                                                  *
******************************************************************************/
void CCaraWinApp::OnEditUndo()
{
   CLabel::SetFileVersion(g_nFileVersion);
   if (m_UndoList.GetCount())                                  // ist nocht etwas da ?
   {
      CUndoParams *pUps = (CUndoParams*) m_UndoList.GetTail();
      RemoveUndoListElement(false, false);                     // Element sofort aus der Liste nehmen
      try
      {
         CCARADoc* pdoc = (CCARADoc*) ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();
         CView* pView   = ((CFrameWnd*)m_pMainWnd)->GetActiveView();
         LPARAM lHint   = 0;
         if (pView->IsKindOf(RUNTIME_CLASS(CPreviewView)))
         {
            lHint = UPDATE_PREVIEW_LABEL;
         }
         switch(pUps->nCmd)                                    // Undo Command auswerten
         {
            case LAST_CMD_CURVE_EDIT:                          // Kurven editieren
            if (pUps->pszFileName && pUps->pLabel)
            {
               CFile File(pUps->pszFileName, CFile::modeRead);
               CArchive ar(&File, CArchive::load);
               ar.m_pDocument = pdoc;
               ((CCurveLabel*)pUps->pLabel)->RestoreEditUndo(ar);
               pdoc->UpdateAllViews(NULL, lHint|UPDATE_CURVE_UNDO, pUps->pLabel);
               ar.Flush();
               File.Close();
            } break;
            case ID_EDIT_CLEAR:                                // Löschen
            if (pUps->pszFileName)
            {
               CFile File(pUps->pszFileName, CFile::modeRead);
               CArchive ar(&File, CArchive::load);
               CLabel::LoadGDIObjects(ar);
               CObject *po = ar.ReadObject(RUNTIME_CLASS(CLabel));
               CLabel::DeleteLOGArray();
               if (po && po->IsKindOf(RUNTIME_CLASS(CLabel)))
               {
                  POSITION pos =  m_UndoList.GetTailPosition();
                  CUndoParams *pTest;
                  while (pos)
                  {
                     pTest = (CUndoParams*) m_UndoList.GetPrev(pos);
                     if (pTest != pUps)
                     {
                        // Löschnummer gleich             : LabelPointer belegen         , Löschnummer löschen
                        if (pTest->nLabel == pUps->nLabel) {pTest->pLabel  = (CLabel*) po; pTest->nLabel  = 0;}
                        if (pTest->nFrom  == pUps->nLabel) {pTest->pFrom   = (CLabel*) po; pTest->nFrom   = 0;}
                        if (pTest->nTo    == pUps->nLabel) {pTest->pTo     = (CLabel*) po; pTest->nTo     = 0;}
                     }
                  }
                  CLabel * pl = (CLabel*) po;
                  if (!pdoc->Undo(pUps, pl, lHint))
                  {
                     CLabelContainer *plc = pdoc->GetLabelContainer();
                     if (plc->InsertLabel(pl))
                     {
                        pdoc->UpdateAllViews(NULL, lHint|UPDATE_INSERT_LABEL, pl);
                     }
                  }
               }
               File.Close();
            }
            break;
            case ID_EDIT_PASTE:                                // einfügen
            if (!pdoc->Undo(pUps, NULL, lHint))
            {
               if (pUps->pLabel)
               {
                  CLabelContainer *plc = pdoc->GetLabelContainer();
                  if (plc->RemoveLabel(pUps->pLabel))
                  {
                     pdoc->UpdateAllViews(NULL, lHint|UPDATE_LABEL_REGION, pUps->pLabel);
                     delete pUps->pLabel;
                  }
               }
            }
            break;
            case LAST_CMD_MOVE:                                // bewegen
            if (!pdoc->Undo(pUps, NULL, lHint))                // zum Plot umleiten
            {                                                  // sonst Labelposition rückgängig 
               if (pUps->pszFileName && pUps->pLabel)
               {
                  CFile File(pUps->pszFileName, CFile::modeRead);
                  CArchive ar(&File, CArchive::load);
                  ar.m_pDocument = pdoc;
                  pUps->pLabel->CLabel::Serialize(ar);
                  pdoc->UpdateAllViews(NULL, lHint|UPDATE_CURVE_UNDO, pUps->pLabel);
                  ar.Flush();
                  File.Close();
               } break;
            }                                                  // noch nicht implementiert
            break;
            default : break;
         }
      }
      catch (CException *e)
      {
         TCHAR szCause[255];
         e->GetErrorMessage(szCause, 255);
         REPORT("SetUndo-Fehler:%s", szCause);
      }
      if (pUps) delete pUps;                                   // Undo Objekt löschen
   }
}

/******************************************************************************
* Name       : RemoveUndoListElement                                          *
* Definition : void RemoveUndoListElement(bool , bool );                      *
* Zweck      : Löschen eines Undo Objektes aus der Undo-Liste                 *
* Aufruf     : RemoveUndoListElement(bHead, bDelete);                         *
* Parameter  :                                                                *
* bHead   (E): Am Kopf der Liste (true, false)                       (bool)   *
* bDelete (E): Element auch löschen (true, false)                    (bool)   *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWinApp::RemoveUndoListElement(bool bHead, bool bDelete)
{
   CUndoParams *pUps;
   if (bHead) pUps = (CUndoParams*) m_UndoList.RemoveHead();
   else       pUps = (CUndoParams*) m_UndoList.RemoveTail();

   if (pUps)
   {
      if (bDelete) delete pUps;
      if (m_pMainWnd && (m_UndoList.GetCount() == 0) && !m_bUndoListOverflow)
         ((CFrameWnd*)m_pMainWnd)->GetActiveDocument()->SetModifiedFlag(false);
   }
}

/******************************************************************************
* Name       : DeleteUndoList                                                 *
* Definition : void DeleteUndoList();                                         *
* Zweck      : Löschen der gesamten Liste                                     *
* Aufruf     : DeleteUndoList();                                              *
******************************************************************************/
void CCaraWinApp::DeleteUndoList()
{
   m_bUndoListOverflow = false;
   while (m_UndoList.GetCount()) RemoveUndoListElement(true, true);
}

void CCaraWinApp::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_UndoList.GetCount());
}

/******************************************************************************
* Name       : InsertIntoClipboard                                            *
* Definition : void InsertIntoClipboard(CLabel*, CDC*, bool, bool);           *
* Zweck      : Einfügen eines Labels in die Zwischenablage                    *
* Aufruf     : InsertIntoClipboard(pl, pDC, bDraw, bLabel);                   *
* Parameter  :                                                                *
* pl      (E): Einzufügendes Labelobjekt                          (CLabel*)   *
* pDC     (E): Devicekontext                                      (CDC*)      *
* bDraw   (E): Bitmap bzw. Metafile erzeugen                      (bool)      *
* bLabel  (E): Label als Object speichern                         (bool)      *
******************************************************************************/
void CCaraWinApp::InsertIntoClipboard(CLabel *pl, CDC *pDC, bool bDraw, bool bLabel)
{
   if ((m_pMainWnd != NULL) && m_pMainWnd->OpenClipboard() && (pl != NULL))
   {
      CLabel::SetFileVersion(g_nFileVersion);
      CCaraFrameWnd * pCFW = (CCaraFrameWnd*) m_pMainWnd;
      ::EmptyClipboard();
      BSState oldStates = pl->GetStates();
      pl->ResetPickStates();

      if (bLabel && pCFW->m_nCF_LABELOBJECT)
      {  // Block für die Erzeugung des User-Clipboardformats CLabelObject
         CCaraSharedFile sf(GMEM_DDESHARE|GMEM_MOVEABLE, 1024);
         CArchive ar(&sf, CArchive::store | CArchive::bNoFlushOnDelete, 1024);
         ar.m_pDocument = ((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();
         ASSERT_KINDOF(CCARADoc, ar.m_pDocument);
         CCARADoc *pDoc = (CCARADoc*) ar.m_pDocument;
         pDoc->m_FileHeader.SetVersion(g_nFileVersion);
         pDoc->m_FileHeader.SetType(LABEL_OBJECT);
         pDoc->WriteFileHeader(ar);
         pDoc->m_FileHeader.CalcChecksum();
         CLabel::SaveGDIObjects(ar);
         ar.WriteObject(pl);
         ar.Flush();
         HGLOBAL hMem = sf.Detach();
         if (hMem)
         {
            void *p = ::GlobalLock(hMem);
            if (p)
            {
               ((CFileHeader*)p)->SetChecksum(pDoc->m_FileHeader.GetChecksum());
               ((CFileHeader*)p)->SetLength(pDoc->m_FileHeader.GetLength());
            }
            ::GlobalUnlock(hMem);
            if (SetClipboardData(pCFW->m_nCF_LABELOBJECT, hMem)==NULL)
            {
               REPORT("Error:SetClipboardData(CF_LABELOBJECT)");
               ::GlobalFree(hMem);
            }
         }
      }

      if (bDraw)
      {
         CRect  rcLabel = pl->GetRect(pDC, DEV_COORD);
         rcLabel.InflateRect(1,1,1,1);
         rcLabel.OffsetRect(-rcLabel.left, -rcLabel.top);

         {  // Block für die Erzeugung eines Metafile-Clipboardformats
            CMetaFileDC  MetaDC;
            CRect rcMeta(rcLabel);
            rcMeta.right  = MulDiv(rcLabel.right+1 , pDC->GetDeviceCaps(HORZSIZE)*100, pDC->GetDeviceCaps(HORZRES));
            rcMeta.bottom = MulDiv(rcLabel.bottom+1, pDC->GetDeviceCaps(VERTSIZE)*100, pDC->GetDeviceCaps(VERTRES));
            if (MetaDC.CreateEnhanced(pDC, NULL, rcMeta, "ETS2DVIEW"))
            {
               MetaDC.SetAttribDC(pDC->GetSafeHdc());
               MetaDC.SetMapMode(pDC->GetMapMode());
               CRect rcLog = pl->GetRect(pDC);
               MetaDC.SetWindowOrg(rcLog.left, rcLog.bottom);
               MetaDC.SaveDC();
               pl->Draw(&MetaDC);
               MetaDC.RestoreDC(-1);
               HENHMETAFILE hmeta = MetaDC.CloseEnhanced();
               if (hmeta)
               {
                  if (SetClipboardData(CF_ENHMETAFILE, hmeta)==NULL)
                  {
                     REPORT("Error:SetClipboardData(CF_ENHMETAFILE)");
                     ::DeleteEnhMetaFile(hmeta);
                  }
               }
            }
         }

         {  // Block für die Erzeugung eines Bitmap-Clipboardformats
            CDC      MemDC;
            CBitmap  MemBmp;
            CBrush bkbrush;
            LOGBRUSH logbrush = {BS_SOLID, m_Backgroundcolor, 0};
            if (MemDC.CreateCompatibleDC(pDC) && MemBmp.CreateCompatibleBitmap(pDC, rcLabel.Width(), rcLabel.Height()))
            {
               MemDC.SelectObject(&MemBmp);
               bkbrush.CreateBrushIndirect(&logbrush);
               MemDC.SelectObject(bkbrush);
               MemDC.PatBlt(0, 0, rcLabel.Width(), rcLabel.Height(), PATCOPY);
               MemDC.SetMapMode(pDC->GetMapMode());
               CRect rcLog = pl->GetRect(pDC);
               MemDC.SetWindowOrg(rcLog.left, rcLog.bottom);
               MemDC.SaveDC();
               pl->Draw(&MemDC);
               MemDC.RestoreDC(-1);
               if (MemBmp.m_hObject)
               {
                  if (SetClipboardData(CF_BITMAP, MemBmp.m_hObject) != NULL)
                  {
                     MemBmp.m_hObject = NULL;                        // auf NULL setzen, damit ~CBitmap m_hObject nicht löscht
                  }
                  else
                  {
                     REPORT("Error:SetClipboardData(CF_BITMAP)");
                  }
               }
            }
         }
      }

      pl->SetStates(oldStates);
      ::CloseClipboard();
   }
}

/******************************************************************************
* Name       :DoDebugReport 
* Definition : bool DoDebugReport(TCHAR *);
* Zweck      : Ermittelt, ob Debug Report durchgeführt werden soll.
* Aufruf     : DoDebugReport(pszAppName);
* Parameter  :
* pszAppName (E): Der Name des Programms     (TCHAR*)
* Funktionswert : true, false                (bool)
******************************************************************************/
bool CCaraWinApp::DoDebugReport(TCHAR *pszAppName)
{
   bool bRValue = false;
   HKEY hKey;
   TCHAR szKey[128] = "Software\\ETSKIEL\\";
   strcat(szKey, pszAppName);
   if (RegOpenKeyEx(HKEY_CURRENT_USER, szKey, 0, KEY_READ|KEY_WRITE, &hKey)==ERROR_SUCCESS)
   {
      DWORD dwSize = sizeof(DWORD), dwValue, dwType;
      if (RegQueryValueEx(hKey, REPORT_KEY, NULL, &dwType, (BYTE*) &dwValue, &dwSize)==ERROR_SUCCESS)
      {
         if ((dwType== REG_DWORD)&&(dwValue!=0))
         {
            szKey[0] = 0;
            strncpy(szKey, g_szAppName, 8);
            strcat(szKey, ".dbg");
            DeleteFile(szKey);
            bRValue = true;
         }
      }
      else
      {
         DWORD dwValue = 0;
         RegSetValueEx(hKey, REPORT_KEY, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD));
      }
      RegCloseKey(hKey);
   }
   return bRValue;
}

void CCaraWinApp::OnFileOpen() 
{
   BEGIN("OnFileOpen");
   TCHAR szCurDir[_MAX_PATH];
   ::GetCurrentDirectory(_MAX_PATH, szCurDir);
   if (!m_strDefaultDir.IsEmpty())
   {
      SetCurrentDirectory(m_strDefaultDir.GetBuffer(m_strDefaultDir.GetLength()));
      m_strDefaultDir.ReleaseBuffer();
   }
   CWinApp::OnFileOpen();
   int nCount = ::GetCurrentDirectory(0,NULL);
   if (nCount)
   {
      LPTSTR str = m_strDefaultDir.GetBufferSetLength(nCount);
      if (str)
      {
         ::GetCurrentDirectory(nCount, str);
         m_strDefaultDir.ReleaseBuffer();
      }
   }
   ::SetCurrentDirectory(szCurDir);
}

bool CCaraWinApp::IsOnWin95()
{
   return ((gm_OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (gm_OsVersionInfo.dwMinorVersion == 0)) ? true : false;
}

bool CCaraWinApp::IsOnWin98()
{
   return (( gm_OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
           ( gm_OsVersionInfo.dwMajorVersion > 4) ||
           ((gm_OsVersionInfo.dwMajorVersion == 4) && (gm_OsVersionInfo.dwMinorVersion > 0)))
      ? true : false;
}

bool CCaraWinApp::IsOnWinNT()
{
   return (gm_OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)      ? true : false;
}

bool CCaraWinApp::IsOnWin2000()
{
   return ((gm_OsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
           (gm_OsVersionInfo.dwMajorVersion > 4))
           ? true : false;
}

int CCaraWinApp::Run() 
{
//   BEGIN("Run()");

   //CEtsMapi EtsMapi;
   //CEtsHelp EtsHelp;
   //CEtsDiv  EtsDiv;
   int nReturn = CWinApp::Run();
   //EtsMapi.Destructor();
   //EtsHelp.Destructor();
   //EtsDiv.Destructor();
   return nReturn;
}

/******************************************************************************
* Name       : PostMsgToCallingWnd
* Definition : void PostMsgToCallingWnd();
* Zweck      : Senden einer Nachricht zum Aufrufenden Programm.
* Aufruf     : Bei Programmende
* Parameter  :
* Funktionswert : (void)
******************************************************************************/
void CCaraWinApp::PostMsgToCallingWnd()
{
   if (IsWindow(m_hCallingWindowHandle) && (m_nCallingWindowMessage != 0))
   {
      REPORT("PostMsgToCallingWnd");
      ::PostMessage(m_hCallingWindowHandle, m_nCallingWindowMessage,
                    m_CallingWindowwParam, m_CallingWindowlParam);
   }
}

void CCaraWinApp::Se_TranslatorFnc(unsigned int e, _EXCEPTION_POINTERS*p)
{
   switch (e)
   {
      case EXCEPTION_ACCESS_VIOLATION:      REPORT("ACCESS_VIOLATION");      break;
      case EXCEPTION_DATATYPE_MISALIGNMENT: REPORT("DATATYPE_MISALIGNMENT"); break;
      case EXCEPTION_PRIV_INSTRUCTION:      REPORT("PRIV_INSTRUCTION");      break;  
      case EXCEPTION_IN_PAGE_ERROR:         REPORT("IN_PAGE_ERROR");         break;  
      case EXCEPTION_STACK_OVERFLOW:        REPORT("STACK_OVERFLOW");        break;  
      case EXCEPTION_INVALID_DISPOSITION:   REPORT("INVALID_DISPOSITION");   break;  
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: REPORT("ARRAY_BOUNDS_EXCEEDED"); break;
      case EXCEPTION_FLT_DENORMAL_OPERAND:  REPORT("FLT_DENORMAL_OPERAND");  break;
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:    REPORT("FLT_DIVIDE_BY_ZERO");    break;
      case EXCEPTION_FLT_INEXACT_RESULT:    REPORT("FLT_INEXACT_RESULT");    break;
      case EXCEPTION_FLT_INVALID_OPERATION: REPORT("FLT_INVALID_OPERATION"); break;
      case EXCEPTION_FLT_OVERFLOW:          REPORT("FLT_OVERFLOW");          break;
      case EXCEPTION_FLT_STACK_CHECK:       REPORT("FLT_STACK_CHECK");       break;
      case EXCEPTION_FLT_UNDERFLOW:         REPORT("FLT_UNDERFLOW");         break;
      case EXCEPTION_INT_DIVIDE_BY_ZERO:    REPORT("INT_DIVIDE_BY_ZERO");    break;
      case EXCEPTION_INT_OVERFLOW:          REPORT("INT_OVERFLOW");          break;
      case EXCEPTION_INVALID_HANDLE:        REPORT("INVALID_HANDLE");        break;
      default : e = 0;
         break;
   }
   if (p)
   {
      REPORT("Exception at %x", p->ExceptionRecord->ExceptionAddress);
      if ((p->ExceptionRecord                       != NULL) &&
          (p->ExceptionRecord->ExceptionInformation != NULL))
      {
         unsigned int i;
         for (i=0; i<p->ExceptionRecord->NumberParameters; i++)
            REPORT("%x", p->ExceptionRecord->ExceptionInformation[i]);
      }
   }
//   ASSERT(false);
}

/******************************************************************************
* Name       : SetCurrentProgramDir()                                         *
* Definition : void SetCurrentProgramDir()();                                 *
* Zweck      : Aufrufpfad des Programms setzen                                *
* Aufruf     : SetCurrentProgramDir();                                        *
* Parameter  : keine                                                          *
* Funktionswert : keiner                                                      *
******************************************************************************/
void CCaraWinApp::SetCurrentProgramDir()
{
   TCHAR      szCurrentdir[2048];
   int       i, nLen = l_tcsclen(m_pszHelpFilePath);
   if(nLen < 2048)                                    // teste nur wenn nicht länger als 2047 Zeichen
   {
      strcpy(szCurrentdir, m_pszHelpFilePath);        // kopieren
      for (i=nLen-1; i>=0; i--)                       // von hinten nach dem Punkt von .hlp suchen
      {
         if (szCurrentdir[i] == '\\')
         {
            szCurrentdir[i++] = '\\';
            szCurrentdir[i]   = 0;
            break;
         }
      }

      SetCurrentDirectory(szCurrentdir);              // nun enthält pszDir das Directory dieses Programmes
   }
}

void CCaraWinApp::CheckPageScaling()
{
   if ((*(CRect*)&m_Margins.rcOldDrawLoMetric) != (*(CRect*)&m_Margins.rcDrawLoMetric))
   {
      CString str;
      CSize   szOld = ((CRect*)&m_Margins.rcOldDrawLoMetric)->Size();
      CSize   szNew = ((CRect*)&m_Margins.rcDrawLoMetric)->Size();
      str.Format(IDS_PRINTPARAMS_CHANGED, 
                 0.1*(float)(m_Margins.rcDrawLoMetric.left  - m_Margins.rcOldDrawLoMetric.left),
                 0.1*(float)(m_Margins.rcDrawLoMetric.top   - m_Margins.rcOldDrawLoMetric.top ),
                 (float)(szOld.cx*100) / (float)szNew.cx,
                 (float)(szOld.cy*100) / (float)szNew.cy);
      if (AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION)==IDYES)
         m_Margins.nScaleLabel |= SCALE_LABEL;
      else
         m_Margins.nScaleLabel &= ~SCALE_LABEL;
   }
}

bool CCaraWinApp::IsLightVersion()
{
   if (m_bLightVersion)
   {
      CString str;
      str.Format(IDS_LIGHTVERSIONINFO, m_pszAppName);
      if (AfxMessageBox(str, MB_YESNO|MB_ICONINFORMATION) == IDYES)
      {
         OnAppAbout();
      }
      return true;
   }
   return false;
}

int CCaraWinApp::GetUndoCount()
{
   return m_UndoList.GetCount();
}

void CCaraWinApp::DeletePrinterDLL()
{
   if (m_pPrtD)
   {
      m_pPrtD->Destructor();
      delete m_pPrtD;
      m_pPrtD = NULL;
   }
}

CDocument* CCaraWinApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
   BEGIN("OpenDocumentFile");
   CDocument*pDoc = CWinApp::OpenDocumentFile(lpszFileName);
   return pDoc;
}

void* CCaraSharedFile::LockHandle()
{
   if (m_hGlobalMemory) return ::GlobalLock(m_hGlobalMemory);
   return NULL;
}

bool CCaraSharedFile::UnlockHandle()
{
   if (m_hGlobalMemory != NULL)
   {
      BOOL bLock = ::GlobalUnlock(m_hGlobalMemory);
      if (bLock == 0)
      {
         DWORD dwError = GetLastError();
         if ((dwError == NO_ERROR) || (dwError == ERROR_NOT_LOCKED)) return true;
      }
      else
      {
         return UnlockHandle();
      }
   }
   return false;
}

bool CCaraSharedFile::FreeBuffer()
{
   if (m_hGlobalMemory)
   {
      HGLOBAL hFree = ::GlobalFree(m_hGlobalMemory);
      if (hFree)
      {
         DWORD dwError = GetLastError();
      }
      else
      {
         m_hGlobalMemory = NULL;
         m_nBufferSize   = 0;
         m_nFileSize     = 0;
         m_nPosition     = 0;
         m_lpBuffer      = NULL;
      }
   }   
   return false;
}

