// PrintAscDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "replall.h"
#include "PrintAscDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintAscDlg Dialogfeld

CPrintAscDlg::CPrintAscDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintAscDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CPrintAscDlg)
	m_nTabLength = 6;
	m_bPrintClipboard = FALSE;
	//}}AFX_DATA_INIT
   // Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
   m_hIcon = AfxGetApp()->LoadIcon(IDI_VIDOKU);
   m_nOldSelect   = 0;
   m_pActualInfo  = NULL;
}

CPrintAscDlg::~CPrintAscDlg()
{
   if(m_pActualInfo)
   {
      ::HeapFree(::GetProcessHeap(), NULL, m_pActualInfo);
   }
}

void CPrintAscDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPrintAscDlg)
   DDX_Control(pDX, IDC_COMBO_PRINTER, m_cComboPrinter);
	DDX_Text(pDX, IDC_TAB_LENGTH, m_nTabLength);
	DDV_MinMaxInt(pDX, m_nTabLength, 1, 40);
	DDX_Check(pDX, IDC_CLIPBOARD_PRINT, m_bPrintClipboard);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrintAscDlg, CDialog)
	//{{AFX_MSG_MAP(CPrintAscDlg)
	ON_BN_CLICKED(IDC_SETUP, OnSetup)
	ON_CBN_SELCHANGE(IDC_COMBO_PRINTER, OnSelchangePrinter)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintAscDlg Nachrichten-Handler

BOOL CPrintAscDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Hinzufügen des Menübefehls "Info..." zum Systemmenü.
   CWinApp *pApp = AfxGetApp();
   m_SelectPrinterName = pApp->GetProfileString(SECTION_KEY, PRINTER_NAME);
   m_nTabLength        = pApp->GetProfileInt(SECTION_KEY, TAB_LENGTH, 6);

   // Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
   //  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
   SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
   SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
   if (m_strText.IsEmpty()) m_bPrintClipboard = true;
   DWORD  i;
   DWORD  dwNeeded;
   DWORD  dwPrinters;
   bool   bOk = false;
   OSVERSIONINFO osv = {sizeof(OSVERSIONINFO) , 0, 0, 0, 0, ""};
   ::GetVersionEx(&osv);               // Betriebssystem ermitteln
   
   ::EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 5, NULL, 0, &dwNeeded, &dwPrinters);
   if(dwNeeded)
   {
      PRINTER_INFO_5 * pPrinterInfo;

      pPrinterInfo = (PRINTER_INFO_5 * ) new BYTE[dwNeeded];

      if(pPrinterInfo!=NULL)
      {
         EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 5, (BYTE*)pPrinterInfo, dwNeeded, &dwNeeded, &dwPrinters);
      
         for(i=0;i<dwPrinters;i++)
         {
            m_cComboPrinter.AddString(pPrinterInfo[i].pPrinterName);
         }

         delete pPrinterInfo;
         bOk = true;
      }
   }
   
   if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
   {
      ::EnumPrinters(PRINTER_ENUM_REMOTE, NULL, 1, NULL, 0, &dwNeeded, &dwPrinters);
      if(dwNeeded)
      {
         PRINTER_INFO_1  * pPrinterInfo;

         pPrinterInfo = (PRINTER_INFO_1 * ) new BYTE[dwNeeded];

         if(pPrinterInfo!=NULL)
         {
            EnumPrinters(PRINTER_ENUM_REMOTE, NULL, 1, (BYTE*)pPrinterInfo, dwNeeded, &dwNeeded, &dwPrinters);
      
            for(i=0;i<dwPrinters;i++)
            {
               if (pPrinterInfo[i].Flags & PRINTER_ENUM_CONTAINER)
                  continue;
               m_cComboPrinter.AddString(pPrinterInfo[i].pName);
            }

            delete pPrinterInfo;
            bOk = true;
         }
      }
      ::EnumPrinters(PRINTER_ENUM_NETWORK, NULL, 1, NULL, 0, &dwNeeded, &dwPrinters);
      if(dwNeeded)
      {
         PRINTER_INFO_1  * pPrinterInfo;

         pPrinterInfo = (PRINTER_INFO_1 * ) new BYTE[dwNeeded];

         if(pPrinterInfo!=NULL)
         {
            EnumPrinters(PRINTER_ENUM_NETWORK, NULL, 1, (BYTE*)pPrinterInfo, dwNeeded, &dwNeeded, &dwPrinters);
      
            for(i=0;i<dwPrinters;i++)
            {
               if (pPrinterInfo[i].Flags & PRINTER_ENUM_CONTAINER)
                  continue;
               m_cComboPrinter.AddString(pPrinterInfo[i].pName);
            }

            delete pPrinterInfo;
            bOk = true;
         }
      }
      ::EnumPrinters(PRINTER_ENUM_CONNECTIONS, NULL, 1, NULL, 0, &dwNeeded, &dwPrinters);
      if(dwNeeded)
      {
         PRINTER_INFO_1  * pPrinterInfo;

         pPrinterInfo = (PRINTER_INFO_1 * ) new BYTE[dwNeeded];

         if(pPrinterInfo!=NULL)
         {
            EnumPrinters(PRINTER_ENUM_CONNECTIONS, NULL, 1, (BYTE*)pPrinterInfo, dwNeeded, &dwNeeded, &dwPrinters);
      
            for(i=0;i<dwPrinters;i++)
            {
               if (pPrinterInfo[i].Flags & PRINTER_ENUM_CONTAINER)
                  continue;
               m_cComboPrinter.AddString(pPrinterInfo[i].pName);
            }

            delete pPrinterInfo;
            bOk = true;
         }
      }
   }

   if (bOk)
   {
      GetDefaultParams((char*)LPCTSTR(m_SelectPrinterName), &m_pActualInfo);

      if (m_pActualInfo == NULL) GetDefaultPrinter(&m_pActualInfo);

      if (m_pActualInfo)
      {
         m_nOldSelect = m_cComboPrinter.FindStringExact(-1, m_pActualInfo->pPrinterName);
         m_cComboPrinter.SetCurSel(m_nOldSelect);
      }
      UpdateParams();
   }

   m_cComboPrinter.SetFocus();

   return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CPrintAscDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   CDialog::OnSysCommand(nID, lParam);
}

void CPrintAscDlg::OnDestroy()
{
	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CPrintAscDlg::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // Gerätekontext für Zeichnen

      SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
      // Symbol in Client-Rechteck zentrieren
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Symbol zeichnen
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
   GetDlgItem(IDC_CLIPBOARD_PRINT)->EnableWindow(::IsClipboardFormatAvailable(CF_TEXT));
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CPrintAscDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

void CPrintAscDlg::OnSetup() 
{
   ChangeSetup();
}

void CPrintAscDlg::OnSelchangePrinter() 
{
   int nSel = m_cComboPrinter.GetCurSel();
              
   if(nSel!=m_nOldSelect)
   {
      m_cComboPrinter.GetLBText(nSel, m_SelectPrinterName);

      if(m_pActualInfo) ::HeapFree(::GetProcessHeap(), NULL, m_pActualInfo);

      GetDefaultParams((char*)LPCTSTR(m_SelectPrinterName), &m_pActualInfo);
      UpdateParams();
      m_nOldSelect = nSel;
   }
}

void CPrintAscDlg::ChangeSetup()
{
   HANDLE    hPrinter;
   DWORD     size;

   if(m_pActualInfo==NULL) return;

   char * szName = m_pActualInfo->pPrinterName;

   if(::OpenPrinter(szName,&hPrinter,NULL))
   {
      size = m_pActualInfo->pDevMode->dmSize + m_pActualInfo->pDevMode->dmDriverExtra;

      DEVMODE * pDevModeOut = (DEVMODE *) new BYTE[size];

      if(pDevModeOut)
      {
         if(::DocumentProperties(m_hWnd, hPrinter,szName,pDevModeOut,m_pActualInfo->pDevMode,DM_IN_BUFFER|DM_OUT_BUFFER|DM_IN_PROMPT)==IDOK)
         {
            ::CopyMemory(m_pActualInfo->pDevMode,pDevModeOut,size);
         }

         delete pDevModeOut;
      }

      ::ClosePrinter(hPrinter);
   }
}

void CPrintAscDlg::UpdateParams()
{
   CString strStatus;
   CString strMore;

   if(m_pActualInfo)
   {
      int  id;

      if(m_pActualInfo->Attributes&PRINTER_ATTRIBUTE_DEFAULT)
      {
         strStatus.LoadString(IDS_PRINTDIALOG_STD);
      }
		
      if(m_pActualInfo->Attributes&PRINTER_ATTRIBUTE_WORK_OFFLINE) id = IDS_PRINTDIALOG_OFFLINE;
      else if(m_pActualInfo->Status==0)									 id = IDS_PRINTDIALOG_OK;
      else																			 id = IDS_PRINTDIALOG_ERROR;

      if(strMore.LoadString(id))
      {
         strStatus += strMore;
         SetDlgItemText(IDC_STATUS_TEXT, LPCTSTR(strStatus));
      }
      if (m_pActualInfo->pPortName)
		{
			SetDlgItemText(IDC_LOCATION_TEXT, m_pActualInfo->pPortName);
		}
      if (m_pActualInfo->pComment)
		{
			SetDlgItemText(IDC_COMMENT_TEXT , m_pActualInfo->pComment);
		}
      if (m_pActualInfo->pComment && m_pActualInfo->pComment[0])
		{
			SetDlgItemText(IDC_COMMENT_TEXT, m_pActualInfo->pComment);
		}
      else if (m_pActualInfo->pLocation && m_pActualInfo->pLocation[0])
		{
			SetDlgItemText(IDC_COMMENT_TEXT, m_pActualInfo->pLocation);
		}
      else
      {
         strStatus = _T("");
         if (m_pActualInfo->pPrintProcessor && m_pActualInfo->pPrintProcessor[0])
			{
				strStatus += CString(m_pActualInfo->pPrintProcessor);
			}
         if (m_pActualInfo->pDatatype && m_pActualInfo->pDatatype[0])
			{
				strStatus += _T(", ") + CString(m_pActualInfo->pDatatype);
			}
         SetDlgItemText(IDC_COMMENT_TEXT, strStatus);
      }
   }
   else
   {
      strStatus.LoadString(IDS_PRINTINFO_ERROR);

      SetDlgItemText(IDC_STATUS_TEXT,   LPCTSTR(strStatus));
      SetDlgItemText(IDC_LOCATION_TEXT, LPCTSTR(strStatus));
      SetDlgItemText(IDC_COMMENT_TEXT,  LPCTSTR(strStatus));
   }
   UpdateData(false);
}
#undef MAX_STRING 

/*********************************************************************************************
 Class      : CPrintAscDlg
 Function   : GetDefaultParams
 Description: Liefert die Standard Druckerparameter in der Printerinfo 2 Struktur

 Parameters:   
  szName: (E): Name des Druckers    (char *)
  ppGet : (E): Printerinfo 2 Struktur    (PRINTER_INFO_2**)

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: June, 16 2005
 <TITLE GetDefaultParams>
*********************************************************************************************/
void CPrintAscDlg::GetDefaultParams(char * szName,PRINTER_INFO_2 **ppGet)
{
   HANDLE hPrinter;

   *ppGet = NULL;
   
   if(::OpenPrinter(szName,&hPrinter,NULL))
   {
      DWORD  dwNeeded;

      ::GetPrinter(hPrinter,2,NULL,0,&dwNeeded);

      if(dwNeeded)
      {
         *ppGet = (PRINTER_INFO_2 *) ::HeapAlloc(::GetProcessHeap(),HEAP_ZERO_MEMORY,dwNeeded);

         if(*ppGet)
         {
            if(!::GetPrinter(hPrinter,2,(LPBYTE) *ppGet,dwNeeded,&dwNeeded))
            {
               ::HeapFree(::GetProcessHeap(),NULL,*ppGet);
               *ppGet = NULL;
            }
         }
      }
      
      ::ClosePrinter(hPrinter);
   }
}

/*********************************************************************************************
 Klasse  : CPrintAscDlg
 Funktion: GetDefaultPrinter
 Zweck   : Liefert die Parameter des Standarddruckers

 Parameter:  
  ppDefaultInfo: (E): Printerinfostruktur  (PRINTER_INFO_2 **)

 Rückgabewert:  (void)
 Author: Rolf Kary-Ehlers
 erstellt am: 28. Mai 2002
 <TITLE GetDefaultPrinter>
*********************************************************************************************/
void CPrintAscDlg::GetDefaultPrinter(PRINTER_INFO_2 ** ppDefaultInfo)
{
   char szPrinter[512];

   *ppDefaultInfo = NULL;

   if(GetProfileString("windows","device","none",szPrinter,512))
   {
      int i = 0;

      if(lstrcmp(szPrinter,"none")==0) return; // keinen Drucker gefunden

      while((szPrinter[i]!=44)&&(i<512)) i++; // suche das Komma hinterm Druckernamen

      if(szPrinter[i]==44) szPrinter[i]=0; // String hier abschneiden
            
      GetDefaultParams(szPrinter,ppDefaultInfo);
   }
}

int CPrintAscDlg::Rounding(double dval)
{
   int temp = (int) dval;
   dval -= temp;
   if (dval >=  0.5) temp++;
   if (dval <= -0.5) temp--;
   return temp;
}

void CPrintAscDlg::OnOK() 
{
   if (m_pActualInfo)
   {
      char * text = NULL;
      HANDLE hPrinter = NULL;
      bool   bEndDoc  = false;
      bool   bEndPage = false;
      try
      {
         DOC_INFO_1 DocInfo;
         DWORD      dwJob, dwCount;
         DWORD      dwBytesWritten;
         if (m_bPrintClipboard)
         {
            if (!::IsClipboardFormatAvailable(CF_TEXT))                       throw (DWORD)1;
            if (!::OpenClipboard(NULL))                                       throw (DWORD)2;
            text= (char*)::GetClipboardData(CF_TEXT);
         }
         else
         {
            if (m_strText.IsEmpty())                                          throw (DWORD)3;
            text= (char*)LPCTSTR(m_strText);
         }
         
         if(!::OpenPrinter((char*)m_pActualInfo->pDevMode->dmDeviceName, &hPrinter, NULL))
                                                                              throw (DWORD)4;
         DocInfo.pDocName    = "Raw Text Printer";
         DocInfo.pOutputFile = NULL;
         DocInfo.pDatatype   = "RAW";
         if((dwJob = ::StartDocPrinter(hPrinter, 1, (LPBYTE)&DocInfo)) == 0)  throw (DWORD)5;
         bEndDoc = true;
         // Start a page.
         if(!::StartPagePrinter(hPrinter))                                    throw (DWORD)6;
         bEndPage = true;
         dwCount  = strlen(text);
         if(!::WritePrinter(hPrinter, text, dwCount, &dwBytesWritten))        throw (DWORD)7;
         if(dwBytesWritten != dwCount)                                        throw (DWORD)8;
      }
      catch(DWORD dwError)
      {
         CString str;
         str.Format("Printer Error %d", dwError);
         AfxMessageBox(str);
      }

      if (bEndPage) EndPagePrinter(hPrinter);
      if (bEndDoc)  EndDocPrinter(hPrinter);
      if (hPrinter) ClosePrinter(hPrinter);

      if (m_bPrintClipboard && text) ::CloseClipboard();

      CWinApp *pApp = AfxGetApp();
      pApp->WriteProfileString(SECTION_KEY, PRINTER_NAME, m_SelectPrinterName);
      pApp->WriteProfileInt(SECTION_KEY, TAB_LENGTH, m_nTabLength);

      CDialog::OnOK();
   }
}

/*
void CPrintAscDlg::OnOK() 
{
   if (m_pActualInfo)
   {
      char * text = NULL;
      FILE *fpout = NULL;
      try
      {
         int c, nLastChar = 0;
         DOC_INFO_1 DocInfo;
         DWORD      dwJob, dwCount;
         DWORD      dwBytesWritten;
         if (m_bPrintClipboard)
         {
            if (!::IsClipboardFormatAvailable(CF_TEXT))                       throw (DWORD)1;
            if (!::OpenClipboard(NULL))                                       throw (DWORD)2;
            text= (char*)::GetClipboardData(CF_TEXT);
         }
         else
         {
            if (m_strText.IsEmpty())                                          throw (DWORD)3;
            text= (char*)LPCTSTR(m_strText);
         }
         
         if (text==NULL) throw (DWORD)4;
         int j = 0;
         char *name;
         name = GetPortName();

         fpout = fopen(name, "wt");
         if (!fpout) throw (DWORD)5;

         while( (c = text[j++]) != 0 )
         {
            switch(c)
            {
               case -28: c = 132; break; // ä
               case -10: c = 148; break; // ö
               case  -4: c = 129; break; // ü
               case -60: c = 142; break; // Ä
               case -42: c = 153; break; // Ö
               case -36: c = 154; break; // Ü
               case -33: c = 225; break; // ß
            }
            if (c == 9)
            {
               for (int k=0; k<m_nTabLength; k++)
                  fputc(' ', fpout);
            }
            else fputc( c, fpout);
            nLastChar = c;
         }

         fputc(0x0c, fpout);

      }
      catch(DWORD dwError)
      {
         CString str;
         str.Format("Printer Error %d", dwError);
         AfxMessageBox(str);
      }

      if (fpout) fclose(fpout);

      if (m_bPrintClipboard && text) ::CloseClipboard();

      CWinApp *pApp = AfxGetApp();
      pApp->WriteProfileString(SECTION_KEY, PRINTER_NAME, m_SelectPrinterName);
      pApp->WriteProfileInt(SECTION_KEY, TAB_LENGTH, m_nTabLength);

      CDialog::OnOK();
   }
}
*/

/*********************************************************************************************
 Klasse  : CPrintAscDlg
 Funktion: GetPortName
 Zweck   : Liefert den Druckerportnamen

 Parameter: Keine

 Rückgabewert: Portname (char *)
 erstellt am: 28. Mai 2002
 <TITLE GetPortName>
*********************************************************************************************/
char * CPrintAscDlg::GetPortName()
{
   if (m_pActualInfo)
   {
      return m_pActualInfo->pPortName;
   }
   else return NULL;
}


/*********************************************************************************************
 Klasse  : CPrintAscDlg
 Funktion: OnHelpInfo
 Zweck   : Wertet die Nachricht WM_HELP für den Dialog aus

 Parameter:  
  pHelpInfo: (E): HelpInfostruktur  (HELPINFO*)

 Rückgabewert: Nachricht bearbeitet (BOOL)
 erstellt am: 28. Mai 2002
 <TITLE OnHelpInfo>
*********************************************************************************************/
BOOL CPrintAscDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
   if (pHelpInfo->iContextType == HELPINFO_WINDOW)
   {
         return true;
   }
   return CDialog::OnHelpInfo(pHelpInfo);
}
