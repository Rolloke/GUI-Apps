// PrintAscDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "PrintAscDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPrintAscDlg 


CPrintAscDlg::CPrintAscDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintAscDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CPrintAscDlg)
	m_bConvertFont = FALSE;
	m_bFormFeed    = FALSE;
	//}}AFX_DATA_INIT
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
	DDX_Check(pDX, IDC_CONVERT_FONT, m_bConvertFont);
	DDX_Check(pDX, IDC_FORM_FEED, m_bFormFeed);
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
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPrintAscDlg 
BOOL CPrintAscDlg::OnInitDialog()
{
   if (m_bConvertFont&DO_FORM_FEED) m_bFormFeed = true;
   CDialog::OnInitDialog();

   if (!m_strDlgHeading.IsEmpty()) SetWindowText(m_strDlgHeading);

   SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
   SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	

   DWORD  i;
   DWORD  dwNeeded;
   DWORD  dwPrinters;
   bool   bOk = false;

   ::EnumPrinters(PRINTER_ENUM_LOCAL,NULL,5,NULL,0,&dwNeeded,&dwPrinters);

   if(dwNeeded)
   {
      PRINTER_INFO_5 * pPrinterInfo;

      pPrinterInfo = (PRINTER_INFO_5 * ) new BYTE[dwNeeded];

      if(pPrinterInfo!=NULL)
      {
         EnumPrinters(PRINTER_ENUM_LOCAL,NULL,5,(BYTE*) pPrinterInfo,dwNeeded,&dwNeeded,&dwPrinters);
      
         for(i=0;i<dwPrinters;i++)
         {
            m_cComboPrinter.AddString(pPrinterInfo[i].pPrinterName);
         }

         delete pPrinterInfo;
         bOk = true;
      }
   }

   if (((CCaraWinApp*)AfxGetApp())->IsOnWinNT())
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
      
      if(m_pActualInfo->Status==0) id = IDS_PRINTDIALOG_OK;
      else                         id = IDS_PRINTDIALOG_ERROR;

      if(strMore.LoadString(id))
      {
         strStatus += strMore;
         SetDlgItemText(IDC_STATUS_TEXT, LPCTSTR(strStatus));
      }
      SetDlgItemText(IDC_LOCATION_TEXT, m_pActualInfo->pPortName);
      SetDlgItemText(IDC_COMMENT_TEXT,  m_pActualInfo->pComment);
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
   if (m_pActualInfo && !(m_pActualInfo->Attributes&PRINTER_ATTRIBUTE_WORK_OFFLINE))
   {
      CDialog::OnOK();
      m_bConvertFont |= (m_pActualInfo->Attributes &PRINTER_ATTRIBUTE_NETWORK);
      if (m_bFormFeed) m_bConvertFont |= DO_FORM_FEED;
   }
}

char * CPrintAscDlg::GetPortName()
{
   if (m_pActualInfo)
   {
      return m_pActualInfo->pPortName;
   }
   else return NULL;
}


DWORD CPrintAscDlg::GetPrinterAttributes()
{
   if (m_pActualInfo)
   {
      return m_pActualInfo->Attributes;
   }
   else return 0;
}

LRESULT CPrintAscDlg::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
{
   HELPINFO *pHI  = (HELPINFO*) lParam;

   if (pHI->iContextType == HELPINFO_WINDOW)                   // Hilfe für ein Control oder window
   {
      ((CFacturaApp*)AfxGetApp())->WinHelp((ULONG)pHI, HELP_CONTEXTPOPUP);
   }
   else                                                        // Hilfe für einen Menüpunkt
   {
      CDialog::WinHelp(pHI->dwContextId, HELP_FINDER);
   }
   return true;
}
