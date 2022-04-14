// CreateControlledProcessDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "CreateControlledProcess.h"
#include "CreateControlledProcessDlg.h"

#include "wk_util.h"

#include <cipcdrives.h>
#include ".\createcontrolledprocessdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#if _TEST_DISK_PARTITION == 1

#define DISKPART_CONSOLE_OPEN               1
#define DISKPART_LIST_DISKS                  2
#define DISKPART_SELECT_DISK               3
#define DISKPART_DISK_SELECTED               4
#define DISKPART_LIST_PARTITIONS            5
#define DISKPART_CREATE_PARTITION_EXT_FINNISHED   6
#define DISKPART_PARTITION_EXT_OK            7
#define DISKPART_CREATE_PARTITION_LOG_FINNISHED   8
#define DISKPART_PARTITION_LOG_OK            9
#define DISKPART_DRIVE_LETTER_ASSIGNED         10
#define FORMAT_BEGIN                     11
#define FORMAT_CMD_CONSOLE_OPEN               12
#define FORMAT_CONFIRM_START               13
#define FORMAT_FINNISHED                  14

#endif

#define STD_OUT_READ_EVENT                  100

#define WM_TRAYCLICKED WM_USER +1
//////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// CCreateControlledProcessDlg Dialogfeld
CCreateControlledProcessDlg::CCreateControlledProcessDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CCreateControlledProcessDlg::IDD, pParent)
{
   m_sProcessName = _T("");
   m_sCmdLine = _T("");
   m_sSendCmd = _T("");
   m_bTerminate = TRUE;
   m_bOutput = TRUE;
   m_bInput  = TRUE;
   m_bInputReturn = TRUE;
   m_bTestOutput = TRUE;
   m_bShowWindow = FALSE;
   m_nKeepAliveTimer = 0;

   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_pProcess = NULL;
}

CCreateControlledProcessDlg::~CCreateControlledProcessDlg()   // Standarddestruktor
{
   WK_DELETE(m_pProcess);
}

void CCreateControlledProcessDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_EDT_PROCESS_NAME, m_sProcessName);
   DDX_Text(pDX, IDC_EDT_CMD_LINE, m_sCmdLine);
   DDX_Text(pDX, IDC_EDT_SEND_CMD, m_sSendCmd);
   DDX_Check(pDX, IDC_CK_TERMINATE, m_bTerminate);
   DDX_Check(pDX, IDC_CK_OUTPUT, m_bOutput);
   DDX_Check(pDX, IDC_CK_INPUT, m_bInput);
   DDX_Control(pDX, IDC_BTN_SEND_CMD, m_btnSendCmd);
   DDX_Control(pDX, IDC_EDT_SEND_CMD, m_edtSendCmd);
   DDX_Control(pDX, IDC_EDT_OUTPUT, m_edtOutput);
   DDX_Check(pDX, IDC_CK_INPUT_RETURN, m_bInputReturn);
   DDX_Check(pDX, IDC_CK_OUTPUT_TEST, m_bTestOutput);
   DDX_Check(pDX, IDC_CK_SHOW_WND, m_bShowWindow);
}

BEGIN_MESSAGE_MAP(CCreateControlledProcessDlg, CDialog)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_BTN_START, OnBnClickedBtnStart)
   ON_WM_TIMER()
   ON_BN_CLICKED(IDC_BTN_SEND_CMD, OnBnClickedBtnSendCmd)
   ON_BN_CLICKED(IDC_BTN_TERMINATE, OnBnClickedTerminate)
   ON_BN_CLICKED(IDC_CK_INPUT, OnBnClickedCkInput)
   ON_BN_CLICKED(IDC_BTN_CLEAR, OnBnClickedBtnClear)
   ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)   
   ON_WM_CREATE()
   ON_WM_DESTROY()
END_MESSAGE_MAP()


// CCreateControlledProcessDlg Meldungshandler

BOOL CCreateControlledProcessDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Hinzufügen des Menübefehls "Info..." zum Systemmenü.

   // IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   if (pSysMenu != NULL)
   {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if (!strAboutMenu.IsEmpty())
      {
         pSysMenu->AppendMenu(MF_SEPARATOR);
         pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
   }

   // Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
   //  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
   SetIcon(m_hIcon, TRUE);         // Großes Symbol verwenden
   SetIcon(m_hIcon, FALSE);      // Kleines Symbol verwenden

   m_FixedFont.CreatePointFont(9*10, _T("Courier New"));
   m_edtOutput.SetFont(&m_FixedFont);

   OnBnClickedCkInput();
   if (_tcsclen(theApp.m_lpCmdLine) > 0)
   {
      UINT nToHide[] = {IDC_EDT_CMD_LINE, IDC_EDT_SEND_CMD, IDC_BTN_SEND_CMD, 
         IDC_BTN_TERMINATE, IDC_BTN_CLEAR, IDC_CK_INPUT, IDC_CK_TERMINATE, 
         IDC_CK_OUTPUT, IDC_CK_INPUT_RETURN, IDC_CK_OUTPUT_TEST, 
         IDC_CK_SHOW_WND, IDC_TXT_PARAM, IDC_TXT_CMD_LINE};
      int i, n = sizeof(nToHide) / sizeof(UINT);
      CWnd *pWnd;
      for (i=0; i<n; i++)
      {
         pWnd = GetDlgItem(nToHide[i]);
         if (pWnd) pWnd->ShowWindow(SW_HIDE);
      }
   }
   return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CCreateControlledProcessDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   if ((nID & 0xFFF0) == IDM_ABOUTBOX)
   {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   }
   else if (nID == SC_MINIMIZE)
   {
      PostMessage(WM_TRAYCLICKED, 0, WM_RBUTTONDOWN);
   }
   else
   {
      CDialog::OnSysCommand(nID, lParam);
   }
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CCreateControlledProcessDlg::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // Gerätekontext zum Zeichnen

      SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

      // Symbol in Clientrechteck zentrieren
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

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CCreateControlledProcessDlg::OnQueryDragIcon()
{
   return static_cast<HCURSOR>(m_hIcon);
}

void CCreateControlledProcessDlg::OnBnClickedBtnStart()
{
   if (UpdateData())
   {
      WK_DELETE(m_pProcess);
      Sleep(2000);
      if (!m_sProcessName.IsEmpty())
      {
         DWORD dwFlags = 0;
         if (m_bTerminate) dwFlags |= WKCPF_TERMINATE;
         if (m_bOutput) dwFlags |= WKCPF_OUTPUT;
         if (m_bInput) dwFlags |= WKCPF_INPUT;
         if (m_bTestOutput) dwFlags |= WKCPF_OUTPUT_CHECK;

         m_pProcess = new CWKControlledProcess(m_sProcessName, NULL, m_sCmdLine, dwFlags, m_bShowWindow ? SW_SHOW : SW_HIDE);
         m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, STD_OUT_READ_EVENT, 0);
         m_pProcess->StartThread();
      }
#if _TEST_DISK_PARTITION == 1
      else
      {
         CString  sFmt;
         DWORD dwFlags, dwMask=1, dwLogical = GetLogicalDrives();
         m_saFreeDrives.RemoveAll();
         int i;            // list the available disks
         for (i=3,dwMask=8; i<26; i++,dwMask<<=1)
         {
            if (!(dwMask & dwLogical))
            {
               sFmt.Format(_T("%c:"), i+'a');
               m_saFreeDrives.Add(sFmt);
            }
         }

         // open diskpart console
         dwFlags = WKCPF_INPUT|WKCPF_OUTPUT|WKCPF_OUTPUT_CHECK|WKCPF_TERMINATE;
         m_pProcess = new CWKControlledProcess(_T("diskpart.exe"), NULL, NULL, dwFlags);
         m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, DISKPART_CONSOLE_OPEN, 0);
         m_pProcess->StartThread();
      }
#endif
   }
}

void CCreateControlledProcessDlg::OnTimer(UINT nIDEvent)
{
   if (nIDEvent == m_nKeepAliveTimer)
   {
      if (m_pProcess == NULL)
      {
         OnBnClickedBtnStart();
      }
      else if (!m_pProcess->IsStillActive())
      {
         OnBnClickedBtnStart();
      }
   }
   if (m_pProcess)
   {
      CWK_String *pstr;
#if _TEST_DISK_PARTITION == 1
      BOOL bDiskpart = FALSE;
      if (IsBetween(nIDEvent, DISKPART_CONSOLE_OPEN, DISKPART_DRIVE_LETTER_ASSIGNED))
      {
         pstr = m_pProcess->FindString(_T("DISKPART>"), TRUE, TRUE);
         if (pstr)
         {
            bDiskpart = TRUE;
            delete pstr;
         }
      }
#endif
      switch(nIDEvent)
      {
         case STD_OUT_READ_EVENT:
         {
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               SetOutputString(*pstr);
               delete pstr;
            }
         }break;
#if _TEST_DISK_PARTITION == 1
////////////////////////////////////////////////////////////////////////////////////////
         case DISKPART_CONSOLE_OPEN:   // diskpart console opened
         {
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               SetOutputString(*pstr);
               pstr->MakeLower();
               if      (pstr->Find(_T("auf computer:")) !=-1)
               {
                  m_sPartition = _T("partition");
                  m_sExtended  = _T("erweitert");
               }
               else if (pstr->Find(_T("on computer:")) !=-1)
               {
                  m_sPartition = _T("partition");
                  m_sExtended  = _T("extended");
               }
               delete pstr;
            }
            if (bDiskpart)
            {
               m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, DISKPART_LIST_DISKS, 0);
               CString str = _T("list disk");
               m_pProcess->WriteStdIn(str, TRUE);
//               SetOutputString(str, TRUE);
               m_nDisks = 0;
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case DISKPART_LIST_DISKS:   // disks are listed
         {
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               m_nDisks++;
               if (m_nDisks >= 3)
               {
                  int i, nFind = pstr->Find(_T("0 B"));
                  if (nFind == -1)
                  {
                     LPCTSTR psz = *pstr;
                     for (i=0; i<pstr->GetLength(); i++)
                     {
                        if (isdigit(psz[i]))
                        {
                           break;
                        }
                     }
                     CString str = pstr->Mid(i, 3);
                     str.TrimRight();
                     str.MakeLower();
                     m_saDisks.Add(str);
                     *pstr += _T("#");
                  }
               }
               SetOutputString(*pstr);
               delete pstr;
            }
            if (bDiskpart)
            {
               m_nDisks = (int)m_nDisks-2;            // 2 lines are used for headline and dashes
               OnTimer(DISKPART_SELECT_DISK);         // 1 line for: diskpart>
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case DISKPART_SELECT_DISK:   // select disk 
         if (m_saDisks.GetCount())
         {
            CString sFmt;
            sFmt.Format(_T("select disk %s"), m_saDisks.GetAt(0));   // select disk
            m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, DISKPART_DISK_SELECTED, 0);
            m_pProcess->WriteStdIn(sFmt, TRUE);
            SetOutputString(sFmt, TRUE);
            m_saDisks.RemoveAt(0);
            m_saPartitions.RemoveAll();
         }
         else
         {
            OnTimer(FORMAT_BEGIN);
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case DISKPART_DISK_SELECTED:   // disk selected
         {
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               SetOutputString(*pstr);
               delete pstr;
            }
            if (bDiskpart)
            {
               m_nLinesCounted = 0;
               m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, DISKPART_LIST_PARTITIONS, 0);
               m_pProcess->WriteStdIn(_T("list partition"), TRUE);
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case DISKPART_LIST_PARTITIONS:   // available partitions on disk
         {
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               SetOutputString(*pstr);
               pstr->MakeLower();
               m_saPartitions.Add(*pstr);
               delete pstr;
            }
            if (bDiskpart)
            {
               int nCount = m_saPartitions.GetCount();
               if (nCount == 1)         // no partitions available
               {
                  m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, DISKPART_CREATE_PARTITION_EXT_FINNISHED, 0);
                  m_pProcess->WriteStdIn(_T("create partition extended"), TRUE);
               }
               else 
               {
                  int i;
                  for (i=0; i<nCount; i++)
                  {
                     if (   m_saPartitions.GetAt(i).Find(m_sPartition) != -1
                        && m_saPartitions.GetAt(i).Find(m_sExtended)  != -1)
                     {
                        m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, DISKPART_CREATE_PARTITION_LOG_FINNISHED, 0);
                        m_pProcess->WriteStdIn(_T("create partition logical"), TRUE);
                        break;
                     }
                  }
                  if (i == nCount)
                  {
                     m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, DISKPART_CREATE_PARTITION_EXT_FINNISHED, 0);
                     m_pProcess->WriteStdIn(_T("create partition extended"), TRUE);
                  }
               }
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case DISKPART_CREATE_PARTITION_EXT_FINNISHED:   // create partition extended finnished
         {
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               SetOutputString(*pstr);
               delete pstr;
            }
            if (bDiskpart)
            {
               m_nLinesCounted = 0;
               m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, DISKPART_PARTITION_EXT_OK, 0);
               m_pProcess->WriteStdIn(_T("list partition"), TRUE);
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case DISKPART_PARTITION_EXT_OK:   // extended partition is ok?
         {
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               SetOutputString(*pstr);
               delete pstr;
               m_nLinesCounted++;
            }
            if (bDiskpart)
            {
               if (m_nLinesCounted > 2)         // extended partition created
               {
                  m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, DISKPART_CREATE_PARTITION_LOG_FINNISHED, 0);
                  m_pProcess->WriteStdIn(_T("create partition logical"), TRUE);
               }
               else
               {
                  OnTimer(DISKPART_SELECT_DISK);
               }
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case DISKPART_CREATE_PARTITION_LOG_FINNISHED:   // create partition logical finnished
         {
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               SetOutputString(*pstr);
               delete pstr;
            }
            if (bDiskpart)
            {
               m_nLinesCounted = 0;
               m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, DISKPART_PARTITION_LOG_OK, 0);
               m_pProcess->WriteStdIn(_T("list partition"), TRUE);
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case DISKPART_PARTITION_LOG_OK:   // logical partition is ok?
         {
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               SetOutputString(*pstr);
               delete pstr;
               m_nLinesCounted++;
            }
            CString sFmt;
            if (bDiskpart)
            {
               if (m_nLinesCounted > 3)         // logical partition created
               {
                  sFmt.Format(_T("assign letter=%s"), m_saFreeDrives.GetAt(0));
                  m_nLinesCounted = 0;
                  m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, DISKPART_DRIVE_LETTER_ASSIGNED, 0);
                  m_pProcess->WriteStdIn(sFmt, TRUE);
               }
               else
               {
                  OnTimer(DISKPART_SELECT_DISK);
               }
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case DISKPART_DRIVE_LETTER_ASSIGNED: // drive letter assigned?
         {
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               SetOutputString(*pstr);
               delete pstr;
               m_nLinesCounted++;
            }
            if (bDiskpart)
            {
               // two lines mean no success; hint for failure cause is added.
               if (m_nLinesCounted == 1)   // single line means success
               {               // add assigned drives to be formatet
                  m_saAssigned.Add(m_saFreeDrives.GetAt(0));
               }
               m_saFreeDrives.RemoveAt(0);// remove the letter always

               OnTimer(DISKPART_SELECT_DISK);
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case FORMAT_BEGIN: // begin to format the assigned letters
         {
            int nDisks = (int)m_saAssigned.GetSize();
            if (nDisks)
            {
               WK_DELETE(m_pProcess);
               DWORD dwFlags = WKCPF_INPUT|WKCPF_OUTPUT|WKCPF_OUTPUT_CHECK|WKCPF_TERMINATE;
               m_pProcess = new CWKControlledProcess(_T("cmd.exe"), NULL, NULL, dwFlags);
               m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, FORMAT_CMD_CONSOLE_OPEN, 0);
               m_pProcess->StartThread();
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case FORMAT_CMD_CONSOLE_OPEN: // command console started
         {
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               SetOutputString(*pstr);
               delete pstr;
            }
            int nDisks = (int)m_saAssigned.GetSize();
            if (nDisks)
            {
               CString sFmt;
               sFmt.Format(_T("format %s /FS:NTFS /V:data /Q"), m_saAssigned.GetAt(0));
               m_saAssigned.RemoveAt(0);
               m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, FORMAT_CONFIRM_START, 0);
               m_pProcess->WriteStdIn(sFmt, TRUE);
            }
            else
            {
               WK_DELETE(m_pProcess);
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case FORMAT_CONFIRM_START: // confirm start format
         {
            CString sFmt;
            sFmt = _T("");
            int iCount;
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               SetOutputString(*pstr);
               iCount = pstr->Find(_T(")?"));
               if (iCount != -1)
               {
                  sFmt = pstr->Mid(iCount-3, 1);
               }
               delete pstr;
            }
            if (!sFmt.IsEmpty())
            {
               m_pProcess->SetStdOutMsgWnd(m_hWnd, WM_TIMER, FORMAT_FINNISHED, 0);
               m_pProcess->WriteStdIn(sFmt, TRUE);
            }
         }break;
////////////////////////////////////////////////////////////////////////////////////////
         case FORMAT_FINNISHED: // format finnished
         {
            BOOL bReady = FALSE;
            while ((pstr = m_pProcess->GetAndRemoveOutputString()) != NULL)
            {
               if (pstr->Find(_T(":")) != -1 && pstr->Find(_T(">")) != -1)
               {
                  bReady =TRUE;
               }
               SetOutputString(*pstr);
               delete pstr;
            }
            if (bReady)
            {
               OnTimer(FORMAT_CMD_CONSOLE_OPEN);
            }
         }break;
#endif
      }
   }
   CDialog::OnTimer(nIDEvent);
}

void CCreateControlledProcessDlg::OnBnClickedBtnSendCmd()
{
   if (m_pProcess && UpdateData())
   {
      m_pProcess->WriteStdIn(m_sSendCmd, m_bInputReturn);
   }
}

void CCreateControlledProcessDlg::OnBnClickedTerminate()
{
   if (m_pProcess)
   {
      WK_DELETE(m_pProcess);
   }
}

void CCreateControlledProcessDlg::OnBnClickedCkInput()
{
   CDataExchange dx(this, TRUE);
   DDX_Check(&dx, IDC_CK_INPUT, m_bInput);
   m_btnSendCmd.EnableWindow(m_bInput);
   m_edtSendCmd.EnableWindow(m_bInput);
}

void CCreateControlledProcessDlg::OnBnClickedBtnClear()
{
   m_edtOutput.SetWindowText(_T(""));
}

void CCreateControlledProcessDlg::SetOutputString(LPCTSTR sOut, BOOL bCR)
{
   m_edtOutput.SetSel(-1, -1);
   m_edtOutput.ReplaceSel(sOut);
   if (bCR)
   {
      m_edtOutput.SetSel(-1, -1);
      m_edtOutput.ReplaceSel(_T("\r\n"));
   }
   int nLen = m_edtOutput.GetWindowTextLength();
   if (nLen > 29999)
   {
       m_edtOutput.GetWindowText(m_sOldText);
       OnBnClickedBtnClear();
   }
}

int CCreateControlledProcessDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CDialog::OnCreate(lpCreateStruct) == -1)
      return -1;

   // TODO! RKE: read CreateControlledProcess.xml if available
   if (_tcsclen(theApp.m_lpCmdLine) > 0)
   {
      NOTIFYICONDATA tnd;
      CString sCmdLine (theApp.m_lpCmdLine);
      CString sTipName(theApp.m_pszAppName);
      int nCmd       = sCmdLine.Find(_T("/c"));
      int nKeepAlive = sCmdLine.Find(_T("/k"));
      if (nCmd != -1)
      {
         m_sProcessName = sCmdLine.Left(nCmd-1);
         nCmd += 2;
         if (nKeepAlive != -1)
         {
            m_sCmdLine = sCmdLine.Mid(nCmd, nKeepAlive-nCmd-1);
         }
         else
         {
            m_sCmdLine = sCmdLine.Mid(nCmd);
         }
      }
      else if (nKeepAlive != -1)
      {
         m_nKeepAliveTimer = (UINT) SetTimer(50, 5000, NULL);
         m_sProcessName = sCmdLine.Left(nKeepAlive-1);
      }
      else
      {
         m_sProcessName = sCmdLine;
      }

      nCmd = m_sProcessName.ReverseFind(_T('\\'));
      if (nCmd == -1)
      {
         sTipName = m_sProcessName;
      }
      else
      {
         sTipName = m_sProcessName.Mid(nCmd+1);
      }

      tnd.cbSize = sizeof(NOTIFYICONDATA);
      tnd.hWnd     = m_hWnd;
      tnd.uID    = 1;
      tnd.hIcon  = m_hIcon;
      tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
      tnd.uCallbackMessage = WM_TRAYCLICKED;
      _tcsncpy(tnd.szTip, LPCTSTR(sTipName), sizeof(tnd.szTip)/sizeof(_TCHAR));
      Shell_NotifyIcon(NIM_ADD, &tnd);

      m_bInputReturn = TRUE;
      m_bTestOutput = TRUE;
      m_bShowWindow = FALSE;

      PostMessage(WM_TRAYCLICKED, 0, WM_RBUTTONDOWN);
      PostMessage(WM_TRAYCLICKED, 0, WM_CREATE);
   }
   return 0;
}

LRESULT CCreateControlledProcessDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
   switch (lParam)
   {
      case WM_LBUTTONDOWN: ShowWindow(SW_SHOW); break;
      case WM_RBUTTONDOWN: ShowWindow(SW_HIDE); break;
      case WM_CREATE:
      {
         UpdateData(FALSE);
         OnBnClickedBtnStart();
      }break;
   }

   return 0;
}

void CCreateControlledProcessDlg::OnDestroy()
{
   CFile file;
   if (file.Open(_T("Log.txt"), CFile::modeCreate|CFile::modeWrite))
   {
#ifdef _UNICODE
       WORD w = 0xfeff;
       file.Write(&w, 2);
#endif
       if (m_sOldText.GetLength())
       {
           file.Write(LPCTSTR(m_sOldText), m_sOldText.GetLength()*sizeof(_TCHAR));
       }
       m_edtOutput.GetWindowText(m_sOldText);
       file.Write(LPCTSTR(m_sOldText), m_sOldText.GetLength()*sizeof(_TCHAR));
   }

   NOTIFYICONDATA tnd;

   tnd.cbSize      = sizeof(NOTIFYICONDATA);
   tnd.hWnd      = m_hWnd;
   tnd.uID         = 1;
   tnd.uFlags      = 0;

   Shell_NotifyIcon(NIM_DELETE, &tnd);
   if (m_nKeepAliveTimer)
   {
      KillTimer(m_nKeepAliveTimer);
   }

   CDialog::OnDestroy();
}
