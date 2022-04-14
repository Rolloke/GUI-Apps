/********************************************************************/
/*                                                                  */
/*                    Programm Faktura                              */
/*                                                                  */
/*     Datenbank für die Fakturierung                               */
/*                                                                  */
/*     programmed by Rolf Kary-Ehlers                               */
/*                                                                  */
/*     Version 2.2            04.04.2002                            */
/*                                                                  */
/********************************************************************/
// Factura.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "Factura.h"

#include "MainFrm.h"
#include "FacturaDoc.h"
#include "FacturaView.h"
#include "PrintAscDlg.h"
#include "CCaraInfo.h"
#include "resource.h"
#include "PrintLabel.h"
#include "CEtsHelp.h"

extern "C" __declspec(dllimport) bool g_bDoReport;
extern "C" __declspec(dllimport) char g_szAppName[16];
extern "C" __declspec(dllimport) int g_nFileVersion;

#define  ETSDEBUG_INCLUDE 
#ifdef _DEBUG
#define  ETSDEBUG_CREATEDATA "FACTURA", &g_bDoReport
#else
#define  ETSDEBUG_CREATEDATA g_szAppName, &g_bDoReport
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFacturaApp

BEGIN_MESSAGE_MAP(CFacturaApp, CCaraWinApp)
	//{{AFX_MSG_MAP(CFacturaApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_EXTRA_PRINT_LABEL, OnExtraPrintLabel)
	ON_COMMAND(ID_OPTIONS_LOGFILE_PRINTER, OnOptionsLogfilePrinter)
	//}}AFX_MSG_MAP
	// Dateibasierte Standard-Dokumentbefehle
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard-Druckbefehl "Seite einrichten"
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	ON_COMMAND((ID_FILE_PRINT+ID_TB_RIGHT_EXT), OnFilePrintSetup)
	ON_COMMAND(ID_OPTIONS_ADR_KL_PRINTER, OnOptionsAdrKlPrinter)
	ON_COMMAND(ID_EXTRA_PRINT_ADR+ID_TB_RIGHT_EXT, OnOptionsAdrKlPrinter)
	ON_COMMAND(ID_EXTRA_PRINT_BANK+ID_TB_RIGHT_EXT, OnOptionsAdrKlPrinter)
	ON_COMMAND(ID_OPTIONS_NNZS_PRINTER, OnOptionsNnzsPrinter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFacturaApp Konstruktion
#define PRINTER_SETTINGS       "Printer"
#define ADR_KL_PRINTER_PORT    "AdrKlPort"
#define ADR_KL_PRINTER_NAME    "AdrKlName"
#define ADR_KL_PRINTER_FLAG    "AdrKlFlag"
#define NNZS_PRINTER_PORT      "NNZSPort"
#define NNZS_PRINTER_NAME      "NNZSName"
#define NNZS_PRINTER_FLAG      "NNZSFlag"
#define LOGFILE_PRINTER_PORT   "LogFilePort"
#define LOGFILE_PRINTER_NAME   "LogFileName"
#define LOGFILE_PRINTER_FLAG   "LogFileFlag"

CFacturaApp::CFacturaApp()
{
   m_hStdPrinter = INVALID_HANDLE_VALUE;
   ZeroMemory(&m_OverLapped, sizeof(OVERLAPPED));
   char szName[64];
   LoadString(m_hInstance, AFX_IDS_APP_TITLE, szName, 64);
   g_bDoReport = DoDebugReport(szName);
   m_bPrintNameOfAdress    = true;
   m_bPrintAdressWithInvNo = false;
}

CFacturaApp::~CFacturaApp()
{
   ETSDEBUGEND;
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CFacturaApp-Objekt
CFacturaApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFacturaApp Initialisierung

BOOL CFacturaApp::InitInstance() 
{
   BEGIN("InitInstance");
   // Standardinitialisierung
   // Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
   //  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
   //  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.
#ifdef _AFXDLL
   Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
   Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif

   CCaraWinApp::InitInstance();

   g_nFileVersion = 125;       // diese Fileversion gilt für die Haupt-Dokumentendateien

   // Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
   //  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.
   CSingleDocTemplate* pDocTemplate;
   pDocTemplate = new CSingleDocTemplate(
      IDR_MAINFRAME,
      RUNTIME_CLASS(CFacturaDoc),
      RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
      RUNTIME_CLASS(CFacturaView));
   AddDocTemplate(pDocTemplate);

   // Befehlszeile parsen, um zu prüfen auf Standard-Umgebungsbefehle DDE, Datei offen
   CCommandLineInfo cmdInfo;
   ParseCommandLine(cmdInfo);

   // Verteilung der in der Befehlszeile angegebenen Befehle
   if (!ProcessShellCommand(cmdInfo))
      return FALSE;

   // Das einzige Fenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
   m_pMainWnd->ShowWindow(SW_SHOW);
   m_pMainWnd->UpdateWindow();

   m_strAdrKlPrinterPort  = GetProfileString(PRINTER_SETTINGS, ADR_KL_PRINTER_PORT, "LPT1:");
   m_strAdrKlPrinterName  = GetProfileString(PRINTER_SETTINGS, ADR_KL_PRINTER_NAME);
   m_bAdrKlPrinterConvert = GetProfileInt(PRINTER_SETTINGS, ADR_KL_PRINTER_FLAG, true);

   m_strNNZSPrinterPort   = GetProfileString(PRINTER_SETTINGS, NNZS_PRINTER_PORT,   "LPT2:");
   m_strNNZSPrinterName   = GetProfileString(PRINTER_SETTINGS, NNZS_PRINTER_NAME);
   m_bNNZSPrinterConvert  = GetProfileInt(PRINTER_SETTINGS, NNZS_PRINTER_FLAG, true);

   m_strLogFilePrinterPort = GetProfileString(PRINTER_SETTINGS, LOGFILE_PRINTER_PORT,   "LPT2:");
   m_strLogFilePrinterName = GetProfileString(PRINTER_SETTINGS, LOGFILE_PRINTER_NAME);
   m_bLogFilePrinterConvert= GetProfileInt(PRINTER_SETTINGS, LOGFILE_PRINTER_FLAG, true);
   if (m_pszHelpFilePath)
   {
      char old = 0;
      int i;
      for (i=strlen(m_pszHelpFilePath)-1; i>0; i--)
         if (m_pszHelpFilePath[i] == '\\')
         {
            old = m_pszHelpFilePath[i+1];
            ((char*)m_pszHelpFilePath)[i+1] = 0;
            break;
         }
      if (old)
      {
         m_strStartDir = _T(m_pszHelpFilePath);
         ((char*)m_pszHelpFilePath)[i+1] = old;
      }
   }
   return TRUE;
}

// Anwendungsbefehl, um das Dialogfeld aufzurufen
void CFacturaApp::OnAppAbout()
{
   if (m_pMainWnd)
   {
      CCaraInfo CaraInfo;     // eine lokale Instanz der DLL-Klasse
      char exename[32];
      strcpy(exename, m_pszExeName);
      strcat(exename, ".exe");
      CCaraInfo::CallInfoResource(m_pMainWnd->m_hWnd, exename);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CFacturaApp-Befehle
void CFacturaApp::OnOptionsAdrKlPrinter() 
{
   CPrintAscDlg Printdlg;
   Printdlg.m_SelectPrinterName = m_strAdrKlPrinterName;
   Printdlg.m_strDlgHeading.LoadString(IDS_ADR_KL_PRINTDLG);
   Printdlg.m_bConvertFont = m_bAdrKlPrinterConvert;
   if (Printdlg.DoModal() == IDOK)
   {
      m_strAdrKlPrinterPort  = Printdlg.GetPortName();
      m_strAdrKlPrinterName  = Printdlg.m_SelectPrinterName;
      m_bAdrKlPrinterConvert = Printdlg.m_bConvertFont;
   }
}

void CFacturaApp::OnOptionsNnzsPrinter() 
{
   CPrintAscDlg Printdlg;
   Printdlg.m_SelectPrinterName = m_strNNZSPrinterName;
   Printdlg.m_strDlgHeading.LoadString(IDS_NNZS_PRINTDLG);
   Printdlg.m_bConvertFont = m_bNNZSPrinterConvert;
   if (Printdlg.DoModal() == IDOK)
   {
      m_strNNZSPrinterPort  = Printdlg.GetPortName();
      m_strNNZSPrinterName  = Printdlg.m_SelectPrinterName;
      m_bNNZSPrinterConvert = Printdlg.m_bConvertFont;
   }
}

void CFacturaApp::OnOptionsLogfilePrinter() 
{
   CPrintAscDlg Printdlg;
   Printdlg.m_SelectPrinterName = m_strLogFilePrinterName;
   Printdlg.m_strDlgHeading.LoadString(IDS_LOGFILE_PRINTDLG);
   Printdlg.m_bConvertFont = m_bLogFilePrinterConvert;
   if (Printdlg.DoModal() == IDOK)
   {
      m_strLogFilePrinterPort  = Printdlg.GetPortName();
      m_strLogFilePrinterName  = Printdlg.m_SelectPrinterName;
      m_bLogFilePrinterConvert = Printdlg.m_bConvertFont;
   }
}

bool CFacturaApp::OpenPrinter() 
{
   CPrintAscDlg Printdlg;
   Printdlg.m_SelectPrinterName = m_strActPrinterName;
   Printdlg.m_strDlgHeading.LoadString(IDS_LOGFILE_PRINTDLG);
   Printdlg.m_bConvertFont = m_bConvert;
   if (Printdlg.DoModal() == IDOK)
   {
      m_strActPrinterName = Printdlg.m_SelectPrinterName;
      m_strActPrinterPort = _T(Printdlg.GetPortName());
      if (Printdlg.GetPrinterAttributes() & PRINTER_ATTRIBUTE_NETWORK)
      {
         m_hStdPrinter = (HANDLE) fopen(Printdlg.GetPortName(), "wt");
         if (m_hStdPrinter == 0)
         {
            m_hStdPrinter = INVALID_HANDLE_VALUE;
            return false;
         }
         m_bConvert = Printdlg.m_bConvertFont;
         m_bPrinter = NETWORK_PRINTER;
         return true;
      }
      else
      {
         m_bPrinter = SIMPLE_PRINTER;
         while (m_hStdPrinter == INVALID_HANDLE_VALUE)
         {
            m_nPrinterNo = NNZS_PRINTER;
            m_hStdPrinter = CreateFile(Printdlg.GetPortName(),
               GENERIC_WRITE, 0, NULL, OPEN_EXISTING, /*FILE_FLAG_OVERLAPPED|FILE_ATTRIBUTE_SYSTEM*/0, NULL);
            if (m_hStdPrinter != INVALID_HANDLE_VALUE)
            {
               m_bConvert = Printdlg.m_bConvertFont;
               m_OverLapped.hEvent = CreateEvent(NULL, true, false, NULL);
               return true;
            }
            if (RequestCancelPrinter()) return false;
         };
      }
   }
   return false;
}

int CFacturaApp::ExitInstance() 
{
   WriteProfileString(PRINTER_SETTINGS, ADR_KL_PRINTER_PORT, m_strAdrKlPrinterPort);
   WriteProfileString(PRINTER_SETTINGS, ADR_KL_PRINTER_NAME, m_strAdrKlPrinterName);
   WriteProfileInt(PRINTER_SETTINGS, ADR_KL_PRINTER_FLAG, m_bAdrKlPrinterConvert);

   WriteProfileString(PRINTER_SETTINGS, NNZS_PRINTER_PORT,   m_strNNZSPrinterPort);
   WriteProfileString(PRINTER_SETTINGS, NNZS_PRINTER_NAME,   m_strNNZSPrinterName);
   WriteProfileInt(PRINTER_SETTINGS, NNZS_PRINTER_FLAG,      m_bNNZSPrinterConvert);

   WriteProfileString(PRINTER_SETTINGS, LOGFILE_PRINTER_PORT, m_strLogFilePrinterPort);
   WriteProfileString(PRINTER_SETTINGS, LOGFILE_PRINTER_NAME, m_strLogFilePrinterName);
   WriteProfileInt(PRINTER_SETTINGS, LOGFILE_PRINTER_FLAG,    m_bLogFilePrinterConvert);
   return CCaraWinApp::ExitInstance();
}

int CFacturaApp::StandardPrint(int nOffset, const char *str, int nSpace, int nLineFeed, int nlen)
{
   int i;
   char strout[3] = "  ";
   char szDosText[1024];
   if (m_hStdPrinter==INVALID_HANDLE_VALUE) return 0;

   if (!nSpace) strout[1] = 0;
   for (i=0; i<nOffset; i++)
      PrintPort(strout);

   nlen -= nOffset;
   if (strlen(str) < 1024)
   {
      if (m_bConvert) CharToOem(str, (char*)szDosText);
      else            strcpy(szDosText, str);
      str = szDosText;
   }
   for (i=0; ((str[i]!=0)&&(i<nlen)); i++)
   {
      strout[0] = str[i];
      PrintPort(strout);
   }

   for (int j=0; j<nLineFeed; j++)
      PrintPort("\n");

   return i;
}

void CFacturaApp::SetActualPrinter(int nNo)
{
   if (m_hStdPrinter != INVALID_HANDLE_VALUE) return;

   if (nNo == NNZS_PRINTER)
   {
      m_bConvert = m_bNNZSPrinterConvert;
      m_strActPrinterName = m_strNNZSPrinterName;
      m_strActPrinterPort = m_strNNZSPrinterPort;
   }
   else if (nNo == ADR_KL_PRINTER)
   {
      m_bConvert = m_bAdrKlPrinterConvert;
      m_strActPrinterName = m_strAdrKlPrinterName;
      m_strActPrinterPort = m_strAdrKlPrinterPort;
   }
   else if (nNo == LOGFILE_PRINTER)
   {
      m_bConvert = m_bLogFilePrinterConvert;
      m_strActPrinterName = m_strLogFilePrinterName;
      m_strActPrinterPort = m_strLogFilePrinterPort;
   }
}

bool CFacturaApp::GetStandardPrinter(int nNo)
{
   if (m_hStdPrinter != INVALID_HANDLE_VALUE) return true;
   SetActualPrinter(nNo);
   if (m_bConvert & PRINTER_ATTRIBUTE_NETWORK)
   {
      m_bConvert &= ~PRINTER_ATTRIBUTE_NETWORK;
      m_hStdPrinter = (HANDLE) fopen(m_strActPrinterPort, "wt");
      if (m_hStdPrinter == 0)
      {
         m_hStdPrinter = INVALID_HANDLE_VALUE;
         return false;
      }
      m_bPrinter = NETWORK_PRINTER;
      return true;
   }
   while (m_hStdPrinter == INVALID_HANDLE_VALUE)
   {
      m_nPrinterNo = nNo;
      m_hStdPrinter = CreateFile(m_strActPrinterPort,
         GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
      if (m_hStdPrinter != INVALID_HANDLE_VALUE)
      {
         m_OverLapped.hEvent = CreateEvent(NULL, true, false, NULL);
         m_bPrinter = SIMPLE_PRINTER;
         return true;
      }
      if (RequestCancelPrinter()) return false;
   };
   return false;
}

void CFacturaApp::CloseStandardPrinter()
{
   if (m_bConvert & DO_FORM_FEED)
   {
      char szFF[2] = "\f";
      PrintPort(szFF);
   }

   if (m_OverLapped.hEvent)
   {
      CloseHandle(m_OverLapped.hEvent);
      m_OverLapped.hEvent = NULL;
   }
   if (m_hStdPrinter != INVALID_HANDLE_VALUE)
   {
      if (m_bPrinter == NETWORK_PRINTER)
      {
         fclose((FILE*)m_hStdPrinter);
      }
      else if (m_bPrinter == WINDOW_HANDLE)
      {
         SetPrinterHandle(NULL);
      }
      else
      {
         CloseHandle(m_hStdPrinter);
      }
      m_hStdPrinter = INVALID_HANDLE_VALUE;
   }
   m_nPrinterNo = 0;
}

void CFacturaApp::FormatConcat(char *format, const char *Concat, int nMax, int& nSum)
{
   int i, nlen = strlen(Concat);
   if (nlen>0)
   {
      for (i=0; (i<nlen)&&(nSum<nMax); nSum++, i++) format[nSum] = Concat[i];
      format[nSum++] = ' ';
      format[nSum] = 0;
   }
}

void CFacturaApp::MakeFormatS2(char * format, char * str1, char * str2, int nMaxlen)
{
   int i, nlen1 = strlen(str1);
   int j, nlen2 = strlen(str2);
   if (nlen1 > nMaxlen) nlen1 = nMaxlen;
   if ((nlen1 + nlen2) > nMaxlen) nlen2 = nMaxlen - nlen1;
   for (i=0; i<nlen1; i++)      format[i] = str1[i];
   format[i++] = '/';
   for (j=0; j<nlen2; i++, j++) format[i] = str2[j];
   format[i] = 0;
}

void CFacturaApp::PrintAdrKl(Customer *pCustomer, const char *pszInvoiceNo)
{
   if (GetStandardPrinter(ADR_KL_PRINTER))
   {
      char text[64];
      int  nSum,
           nMax = GetProfileInt(LABEL_SETTINGS, LABEL_LINE_LENGTH, 31),
           nLF  = GetProfileInt(LABEL_SETTINGS, LABEL_LINES, 12) - 8;
      if (strlen(pCustomer->szCompany) > 0)
      {
         char *pstr1 = pCustomer->szCompany, 
              *pstr2 = strstr(pstr1, "\n");

         while (pstr2)                                         // Firma, mehrzeilig
         {
            pstr2[0] = 0;
            StandardPrint(0, pstr1, 0, 1, nMax);
            pstr2[0] = '\n';
            pstr1 = &pstr2[1];
            pstr2 = strstr(pstr1, "\n");
            nLF--;
         }
         StandardPrint(0, pstr1, 0, 1, nMax);
      }
      else                                                     // sonst Anrede
      {
         if (pCustomer->nFlags & CUSTOMER_FEMALE) StandardPrint(0, "Frau",  0, 1, nMax);
         else                                     StandardPrint(0, "Herrn", 0, 1, nMax);
      }
      text[0] = 0;
      nSum    = 0;
      if (m_bPrintNameOfAdress)                                // Name der Adresse
      {
         FormatConcat(text, pCustomer->szTitle    , nMax, nSum);
         FormatConcat(text, pCustomer->szFirstName, nMax, nSum);
         FormatConcat(text, pCustomer->szName     , nMax, nSum);
         StandardPrint(0, text, 0, 1, nMax);
      }
      else StandardPrint(0, "", 0, 1, nMax);

      text[0] = 0;
      nSum    = 0;
      FormatConcat(text, pCustomer->szStreet, nMax*(nLF-1), nSum);
      FormatConcat(text, pCustomer->szStreetNo, nMax*(nLF-1), nSum);
      StandardPrint(0, "", 0, 1, nMax);                        // Zeilenvorschub
      if (strlen(text) > 0)                                    // Straße
      {
         char *pstr1 = text, 
              *pstr2 = strstr(pstr1, "\n");
         while (pstr2)                                         // mehrzeilig
         {
            pstr2[0] = 0;
            StandardPrint(0, pstr1, 0, 1, nMax);
            pstr1 = &pstr2[1];
            pstr2 = strstr(pstr1, "\n");
            nLF--;
         }
         StandardPrint(0, pstr1, 0, 1, nMax);
      }

      text[0] = 0;
      nSum    = 0;
      char *ptr = strstr(pCustomer->szPostalCode, "§");        // PLZ, Ort
      if (ptr) FormatConcat(text, &ptr[4], nMax, nSum);
      else     FormatConcat(text, pCustomer->szPostalCode, nMax, nSum);
      FormatConcat(text, pCustomer->szCity, nMax, nSum);
      StandardPrint(0, text, 0, 1, nMax);

      text[0] = 0;
      nSum    = 0;
      if(strcmp(pCustomer->szState, "BRD") != NULL)
         FormatConcat(text, pCustomer->szState, nMax, nSum);

      StandardPrint(0, text, 0, 3, nMax);                      // Land

      text[0] = 0;
      if(pszInvoiceNo)                                         // Rechnungsnummer
      {
         nSum    = 0;
         FormatConcat(text, "Rg.Nr.:", nMax, nSum);
         FormatConcat(text, pszInvoiceNo, nMax, nSum);
         StandardPrint(7, text, 0, nLF, nMax);
      }
      else StandardPrint(0, text, 0, nLF, 0);
      CloseStandardPrinter();
   }
}

void CFacturaApp::OnExtraPrintLabel() 
{
   CPrintLabel dlg;
   dlg.DoModal();
}

int CFacturaApp::PrintPort(char *pszText)
{
   if (m_hStdPrinter != INVALID_HANDLE_VALUE)
   {
      DWORD dwBytes, dwResult;
      ASSERT(pszText != NULL);

      if (m_bPrinter == NETWORK_PRINTER)
      {
         fprintf((FILE*)m_hStdPrinter, pszText);
         return strlen(pszText);
      }
      else if (m_bPrinter == WINDOW_HANDLE)
      {
         CString str;
         CWnd *pWnd = CWnd::FromHandle((HWND)m_hStdPrinter);
         if (pWnd)
         {
            CString strAdd;
            if (pszText[0] == '\n') strAdd = _T("\r\n");
            else                    strAdd = _T(pszText);
            pWnd->GetWindowText(str);
            str += strAdd;
            pWnd->SetWindowText(str);
         }
         return 0;
      }

      WriteFile(m_hStdPrinter, pszText, strlen(pszText), &dwBytes, &m_OverLapped);
      dwResult = ::WaitForSingleObject(m_OverLapped.hEvent, 200);
      switch(dwResult)
      {
         case WAIT_TIMEOUT:
         case WAIT_FAILED:
            RequestCancelPrinter();
            return 0;
         case WAIT_ABANDONED:
         case WAIT_OBJECT_0:
            return m_OverLapped.Internal;
      }
   }
   return 0;
}

bool CFacturaApp::RequestCancelPrinter()
{
   CString str;
   str.Format(IDS_PRINTER_NOT_READY, m_strActPrinterName, m_strActPrinterPort);
   if (AfxMessageBox(str, MB_OKCANCEL|MB_ICONEXCLAMATION)==IDCANCEL)
   {
      CloseStandardPrinter();
      return true;
   }
   return false;
}



void CFacturaApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	if (HELP_CONTEXTPOPUP == nCmd)
   {
      HELPINFO *pHI  = (HELPINFO*) dwData;
      CString str;
      if (pHI->iCtrlId == 0xffff) return;
      if (str.LoadString(pHI->iCtrlId))
      {
         CEtsHelp::CreateContextWnd(str, pHI->MousePos.x, pHI->MousePos.y);
         return;
      }
   }
	CCaraWinApp::WinHelp(dwData, nCmd);
}

int CFacturaApp::Run() 
{
   CEtsHelp etshelp;
   int nReturn = CCaraWinApp::Run();
   etshelp.Destructor();

   return nReturn;
}

bool CFacturaApp::SetPrinterHandle(HWND hwnd)
{
   if (hwnd)
   {
      if (m_hStdPrinter == INVALID_HANDLE_VALUE)
      {
         m_hStdPrinter = (HANDLE) hwnd;
         ASSERT(IsWindow(hwnd));
         ::SetWindowText(hwnd, "");
         m_bPrinter    = WINDOW_HANDLE;
         return true;
      }
   }
   else
   {
      if (m_hStdPrinter != INVALID_HANDLE_VALUE)
      {
         ASSERT(IsWindow((HWND)m_hStdPrinter));
         m_hStdPrinter = (HANDLE) INVALID_HANDLE_VALUE;
         m_bPrinter    = SIMPLE_PRINTER;
         return true;
      }
   }
   return false;
}
