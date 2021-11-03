/********************************************************************/
/*                                                                  */
/*                    Programm PHPContenMan                         */
/*                                                                  */
/*     Contenmanager für die PHP Seiten auf der ELAC HomePage       */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// PHPContentMan.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "PHPContentMan.h"
#include "MainFrame.h"
#include "CCaraInfo.h"
#include "PHPContentManDlg.h"
#include "PictureCatalog.h"
#include "HtmlSignDlg.h"

#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPHPContentManApp

BEGIN_MESSAGE_MAP(CPHPContentManApp, CWinApp)
	//{{AFX_MSG_MAP(CPHPContentManApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_CALL_PICTURE_CONTENT_MAN, OnCallPictureContentMan)
	ON_UPDATE_COMMAND_UI(ID_CALL_SETTINGS, OnUpdateCallSettings)
	ON_COMMAND(ID_CALL_HTML_CODE, OnCallHtmlCode)
	ON_UPDATE_COMMAND_UI(IDCANCEL, OnUpdateCancel)
	ON_COMMAND(ID_HTMLCODE_RESTORE, OnHtmlcodeRestore)
	ON_COMMAND(ID_APP_TOPMOST, OnAppTopmost)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPHPContentManApp Konstruktion
int CPHPContentManApp::gm_nDirection = 0; 
ListHTMLSigns CPHPContentManApp::gm_CharHTMLList[LISTHTMLSIZE] = 
{
   {(char) 60, "&lt;"},
   {(char) 62, "&gt;"},
   {(char)'&', "&amp;"},
   {(char) 22, "&quot;"},
   {(char)128, "&euro;"},
   {(char)160, "&nbsp;"},
   {(char)161, "&iexcl;"},
   {(char)162, "&cent;"},
   {(char)163, "&pound;"},
   {(char)164, "&curren;"},
   {(char)165, "&yen;"},
   {(char)166, "&brvbar;"},
   {(char)167, "&sect;"},
   {(char)168, "&uml;"},
   {(char)169, "&copy;"},
   {(char)170, "&ordf;"},
   {(char)171, "&laquo;"},
   {(char)172, "&not;"},
   {(char)173, "&shy;"},
   {(char)174, "&reg;"},
   {(char)175, "&macr;"},
   {(char)176, "&deg;"},
   {(char)177, "&plusmn;"},
   {(char)178, "&sup2;"},
   {(char)179, "&sup3;"},
   {(char)180, "&acute;"},
   {(char)181, "&micro;"},
   {(char)182, "&para;"},
   {(char)183, "&middot;"},
   {(char)184, "&cedil;"},
   {(char)185, "&sup1;"},
   {(char)186, "&ordm;"},
   {(char)187, "&raquo;"},
   {(char)188, "&frac14;"},
   {(char)189, "&frac12;"},
   {(char)190, "&frac34;"},
   {(char)191, "&iquest;"},
   {(char)192, "&Agrave;"},
   {(char)193, "&Aacute;"},
   {(char)194, "&Acirc;"},
   {(char)195, "&Atilde;"},
   {(char)196, "&Auml;"},
   {(char)197, "&Aring;"},
   {(char)198, "&AElig;"},
   {(char)199, "&Ccedil;"},
   {(char)200, "&Egrave;"},
   {(char)201, "&Eacute;"},
   {(char)202, "&Ecirc;"},
   {(char)203, "&Euml;"},
   {(char)204, "&Igrave;"},
   {(char)205, "&Iacute;"},
   {(char)206, "&Icirc;"},
   {(char)207, "&Iuml;"},
   {(char)208, "&ETH;"},
   {(char)209, "&Ntilde;"},
   {(char)210, "&Ograve;"},
   {(char)211, "&Oacute;"},
   {(char)212, "&Ocirc;"},
   {(char)213, "&Otilde;"},
   {(char)214, "&Ouml;"},
   {(char)215, "&times;"},
   {(char)216, "&Oslash;"},
   {(char)217, "&Ugrave;"},
   {(char)218, "&Uacute;"},
   {(char)219, "&Ucirc;"},
   {(char)220, "&Uuml;"},
   {(char)221, "&Yacute;"},
   {(char)222, "&THORN;"},
   {(char)223, "&szlig;"},
   {(char)224, "&agrave;"},
   {(char)225, "&aacute;"},
   {(char)226, "&acirc;"},
   {(char)227, "&atilde;"},
   {(char)228, "&auml;"},
   {(char)229, "&aring;"},
   {(char)230, "&aelig;"},
   {(char)231, "&ccedil;"},
   {(char)232, "&egrave;"},
   {(char)233, "&eacute;"},
   {(char)234, "&ecirc;"},
   {(char)235, "&euml;"},
   {(char)236, "&igrave;"},
   {(char)237, "&iacute;"},
   {(char)238, "&icirc;"},
   {(char)239, "&iuml;"},
   {(char)240, "&eth;"},
   {(char)241, "&ntilde;"},
   {(char)242, "&ograve;"},
   {(char)243, "&oacute;"},
   {(char)244, "&ocirc;"},
   {(char)245, "&otilde;"},
   {(char)246, "&ouml;"},
   {(char)247, "&divide;"},
   {(char)248, "&oslash;"},
   {(char)249, "&ugrave;"},
   {(char)250, "&uacute;"},
   {(char)251, "&ucirc;"},
   {(char)252, "&uuml;"},
   {(char)253, "&yacute;"},
   {(char)254, "&thorn;"},
   {(char)255, "&yuml;"}
};


CPHPContentManApp::CPHPContentManApp()
{
   m_pHtmlSignDlg = NULL;
   m_pPictureDlg  = NULL;
}

CPHPContentManApp::~CPHPContentManApp()
{
   if (m_pHtmlSignDlg) delete m_pHtmlSignDlg;
   if (m_pPictureDlg)  delete m_pPictureDlg;
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CPHPContentManApp-Objekt

CPHPContentManApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPHPContentManApp Initialisierung

BOOL CPHPContentManApp::InitInstance()
{
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

   if ((m_lpCmdLine != NULL) && (m_lpCmdLine[0] == '/'))
   {
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

      if (m_pszHelpFilePath)
      {
         m_strAppPath = _T(m_pszHelpFilePath);
         int nFind = m_strAppPath.ReverseFind('\\');
         if (nFind != -1) m_strAppPath = m_strAppPath.Left(nFind+1);
      }

      if (m_lpCmdLine[1] == 'k')
      {
         return StartFrameWnd();
      }
      if (m_lpCmdLine[1] == 'c')
      {
         OnCallPictureContentMan();
      }
   }
   else
   {
      char * psz = (char*)m_pszProfileName;
      if (psz)
      {
         char szPath[MAX_PATH];
         ::GetCurrentDirectory(MAX_PATH, szPath);
         int nLen = strlen(szPath);
         if (szPath[nLen-1] != '\\')
         {
            szPath[nLen++] = '\\';
            szPath[nLen++] = 0;
         }
         strcat(szPath, m_pszProfileName);
         free((void*)m_pszProfileName);
         m_pszProfileName = _tcsdup(szPath);
      }
      bool bReturn = StartFrameWnd(1);
//      OnCallPictureContentMan();
      OnCallHtmlCode();
      if (bReturn && m_pHtmlSignDlg)
      {
         m_pHtmlSignDlg->m_bCloseFrame = true;
      }
      return bReturn;
   }
	return FALSE;
}

bool CPHPContentManApp::StartFrameWnd(int nMenu)
{
   CRect rcWnd(0,0,100,100);
   try
   {
      m_pActiveWnd = new CMainFrame;
      if (!m_pActiveWnd) throw (int) 1;
      ((CMainFrame*)m_pActiveWnd)->m_nContextMenu = nMenu;
      if (!((CMainFrame*)m_pActiveWnd)->Create(NULL, "PHP Content Man", 0, rcWnd, NULL, 0, NULL)) throw (int) 2;
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

void CPHPContentManApp::OnAppAbout() 
{
   CCaraInfo CaraInfo;     // eine lokale Instanz der DLL-Klasse
   char exename[32];
   strcpy(exename, m_pszExeName);
   strcat(exename, ".exe");
   CCaraInfo::CallInfoResource(m_pMainWnd->m_hWnd, exename);
   CaraInfo.Destructor();
}

void CPHPContentManApp::OnCallPictureContentMan() 
{
   if (m_pPictureDlg == NULL)
   {
      m_pPictureDlg = new CPictureCatalog;
   }
	if (m_pMainWnd == NULL)
   {
      m_pMainWnd = m_pPictureDlg;
	   m_pPictureDlg->DoModal();
   }
   else
   {
      if (m_pPictureDlg->m_hWnd == NULL)
         m_pPictureDlg->Create(IDD_PICTURE_CATALOG, m_pMainWnd);
      else ::ShowWindow(m_pPictureDlg->m_hWnd, SW_SHOW);
   }
}

void CPHPContentManApp::OnCallHtmlCode() 
{
   if (m_pHtmlSignDlg == NULL)
   {
      m_pHtmlSignDlg = new CHtmlSignDlg;
   }
	if (m_pMainWnd == NULL)
   {
      m_pMainWnd = m_pHtmlSignDlg;
	   m_pHtmlSignDlg->DoModal();
   }
   else
   {
      if (m_pHtmlSignDlg->m_hWnd == NULL)
         m_pHtmlSignDlg->Create(IDD_HTML_SIGN_DLG, m_pMainWnd);
      else ::ShowWindow(m_pHtmlSignDlg->m_hWnd, SW_SHOW);
   }
}

void CPHPContentManApp::OnUpdateCallSettings(CCmdUI* pCmdUI) 
{
	CPHPContentManDlg dlg;
	if (m_pMainWnd == NULL) m_pMainWnd = &dlg;
	dlg.DoModal();
}

int _cdecl CPHPContentManApp::CompareHTML(const void *p1, const void *p2)
{
   return strcmp(((ListHTMLSigns*)p1)->szHTML,((ListHTMLSigns*)p2)->szHTML);
}

int _cdecl CPHPContentManApp::FindHTML(const void *p1, const void *p2)
{
   ListHTMLSigns *pHS = (ListHTMLSigns*)p2;
   int nLen = strlen(pHS->szHTML);
   return strncmp((char*)p1, pHS->szHTML, nLen);
}

int _cdecl CPHPContentManApp::CompareChar(const void *p1, const void *p2)
{
   if      (((ListHTMLSigns*)p1)->cSign == ((ListHTMLSigns*)p2)->cSign) return  0;
   else if (((ListHTMLSigns*)p1)->cSign <  ((ListHTMLSigns*)p2)->cSign) return -1;
   else                                                                 return  1;
}

int _cdecl CPHPContentManApp::FindChar(const void *p1, const void *p2)
{
   if      ((char)p1 == ((ListHTMLSigns*)p2)->cSign) return  0;
   else if ((char)p1 <  ((ListHTMLSigns*)p2)->cSign) return -1;
   else                                              return  1;
}

void CPHPContentManApp::WriteHTML(CArchive &ar, LPCTSTR pszText)
{
   SetDirection(TEXT2HTML);
   ListHTMLSigns *pFound;
   int i, nLen = strlen(pszText);
   for (i=0; i<nLen; i++)
   {
      pFound = (ListHTMLSigns*) bsearch((const void*)pszText[i], gm_CharHTMLList, LISTHTMLSIZE, sizeof(ListHTMLSigns), FindChar);
      if (pFound) ar.Write(pFound->szHTML, strlen(pFound->szHTML));
      else        ar.Write(&pszText[i], 1);
   }
}

void CPHPContentManApp::ReadHTML(CArchive &ar, CString &string)
{
   SetDirection(HTML2TEXT);
   ListHTMLSigns *pFound;
   UINT i, nRead, nLen = 0, nPos = 0;
   char    szRead[256];
   LPTSTR  pszString = NULL;
   for (;;)
   {
      nRead = ar.Read(szRead, 255);
      pszString = GetBuffer(string, pszString, nLen, nLen+nRead);
      for (i=0;i<nRead; i++)
      {
         if (szRead[i] == '&')
         {
            pFound = (ListHTMLSigns*)bsearch(&szRead[i], gm_CharHTMLList, LISTHTMLSIZE, sizeof(ListHTMLSigns), FindHTML);
            if (pFound)
            {
               pszString[nPos++] = pFound->cSign;
               i += strlen(pFound->szHTML)-1;
               continue;
            }
         }
         pszString[nPos++] = szRead[i];
      }
      if (nRead < 255) break;
   }
   string.ReleaseBuffer(nPos);
}

LPTSTR CPHPContentManApp::GetBuffer(CString &string, LPTSTR  pszString, UINT &nLen, UINT nNewLen)
{
   if (nNewLen < nLen)
   {
      return pszString;
   }
   else
   {
      string.ReleaseBuffer();
      pszString = string.GetBufferSetLength(nNewLen);
      nLen = string.GetLength();
      return pszString;
   }
}

void CPHPContentManApp::SetDirection(int nDir)
{
   if (nDir != gm_nDirection)
   {
      if (nDir == TEXT2HTML)
      {
         qsort(gm_CharHTMLList, LISTHTMLSIZE, sizeof(ListHTMLSigns), CompareChar);
      }
      else if (nDir == HTML2TEXT)
      {
         qsort(gm_CharHTMLList, LISTHTMLSIZE, sizeof(ListHTMLSigns), CompareHTML);
      }
      gm_nDirection = nDir;
   }
}


char CPHPContentManApp::GetCharCode(int nCode)
{
   if ((nCode>=0) && (nCode<LISTHTMLSIZE))
      return gm_CharHTMLList[nCode].cSign;
   return 0;
}

const char* CPHPContentManApp::GetHTMLCode(int nCode)
{
   if ((nCode>=0) && (nCode<LISTHTMLSIZE))
      return (const char *) gm_CharHTMLList[nCode].szHTML;
   return "@";
}


void CPHPContentManApp::OnUpdateCancel(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

void CPHPContentManApp::OnCancel() 
{

}

void CPHPContentManApp::OnHtmlcodeRestore() 
{
   if (m_pHtmlSignDlg)
   {
      m_pHtmlSignDlg->SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
   }
}

void CPHPContentManApp::OnAppTopmost() 
{
   if (m_pHtmlSignDlg)
   {
      ::SetWindowPos(m_pHtmlSignDlg->m_hWnd, m_pHtmlSignDlg->m_bTopMost ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOREDRAW);
      m_pHtmlSignDlg->m_bTopMost = !m_pHtmlSignDlg->m_bTopMost;
   }
}
