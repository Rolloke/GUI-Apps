// BatchCompile.cpp : implementation file
//

#include "stdafx.h"
#include "MSDNEdit.h"
#include "BatchCompile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBatchCompile dialog
#define  BATCH_JOB  "Batch"
#define  BATCH_JOBX "Job%02d"
#define  BATCH_EXE  "Execute"
#define  BATCH_CMDS "Cmds"

CBatchCompile::CBatchCompile(CWnd* pParent /*=NULL*/)
	: CDialog(CBatchCompile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchCompile)
	m_strPath = _T("");
	m_strCommands = _T("");
	//}}AFX_DATA_INIT
   m_hRenderThread = NULL;
   m_bRun = false;
}

CBatchCompile::~CBatchCompile()
{
   if (m_hRenderThread) CloseHandle(m_hRenderThread);
}


void CBatchCompile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchCompile)
	DDX_Control(pDX, IDC_COMPILE_LIST, m_cCompileList);
	DDX_Text(pDX, IDC_EDT_COMPILER_PATH, m_strPath);
	DDX_Text(pDX, IDC_EDT_COMPILER_COMMANDS, m_strCommands);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchCompile, CDialog)
	//{{AFX_MSG_MAP(CBatchCompile)
	ON_NOTIFY(NM_DBLCLK, IDC_COMPILE_LIST, OnDblclkCompileList)
	ON_BN_CLICKED(IDC_BUILD_ALL, OnBuildAll)
	ON_BN_CLICKED(IDC_SELECT_COMPILER, OnSelectCompiler)
	ON_BN_CLICKED(IDC_DELETE_FILE, OnDeleteFile)
	ON_NOTIFY(NM_CLICK, IDC_COMPILE_LIST, OnClickCompileList)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchCompile message handlers

BOOL CBatchCompile::OnInitDialog() 
{
   CWinApp*pApp = AfxGetApp();
	CString str;
   m_strPath     = pApp->GetProfileString(BATCH_JOB, BATCH_EXE);
   m_strCommands = pApp->GetProfileString(BATCH_JOB, BATCH_CMDS);
	CDialog::OnInitDialog();
   str.LoadString(IDS_LIST_COMPILE);
   m_cCompileList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP);
   m_cCompileList.InsertColumn(0, str, LVCFMT_LEFT, 250);
   for (int i=0; ; i++)
   {
      str.Format(BATCH_JOBX, i);
      str = pApp->GetProfileString(BATCH_JOB, str);
      if (!str.IsEmpty())
      {
         if (str == _T("--")) break;
         m_cCompileList.InsertItem(i, str);
      }
      else
      {
         break;
      }
   }	
   m_cCompileList.InsertItem(i, "--");
   EnableToolTips(true);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBatchCompile::OnDblclkCompileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   CString str;
   LPCTSTR pstr = NULL;
   int nSel = m_cCompileList.GetSelectionMark();
	if (nSel != -1)
   {
      str = m_cCompileList.GetItemText(nSel, 0);
      pstr = str;
   }
   
   CFileDialog dialog(true, "DOX", pstr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Doc-O-Matic Dateien (*.dox)|*.dox||", this);
   
   if (dialog.DoModal() ==IDOK)
   {
      str = dialog.GetPathName();
      m_cCompileList.InsertItem(nSel, str);
   }
	*pResult = 0;
}

unsigned __stdcall CBatchCompile::RenderThread(void *pParameter)
{
   CBatchCompile *pThis = (CBatchCompile*)pParameter;
   if (pThis)
   {
      if (!pThis->m_strPath.IsEmpty())
      {
         int i, nItem, nCount = pThis->m_cCompileList.GetSelectedCount();
         CString str, strFile;
         if (nCount)
         {
            for (i=0, nItem = -1; i<nCount; i++)
            {
               nItem = pThis->m_cCompileList.GetNextItem(nItem, LVNI_SELECTED);
               strFile = pThis->m_cCompileList.GetItemText(nItem, 0);
               if (strFile != _T("--"))
               {
                  str.Format("\"%s\" %s %s", pThis->m_strPath, pThis->m_strCommands, strFile);
                  pThis->SetDlgItemText(IDC_TXT_COMPILING_FILE, strFile);
                  system(str);
                  if (!pThis->m_bRun)
                     break;
               }
            }
         }
      }
      if (pThis->m_bRun)
      {
         pThis->EnableDlgItems(true);
         pThis->SetDlgItemText(IDC_TXT_COMPILING_FILE, "");
      }
      pThis->m_bRun = false;
      if (pThis->m_hRenderThread) CloseHandle(pThis->m_hRenderThread);
      pThis->m_hRenderThread = NULL;
   }
   
   return 0;
}

void CBatchCompile::OnBuildAll() 
{
   if (UpdateData())
   {
      unsigned int nThreadId;
      m_hRenderThread = (HANDLE) _beginthreadex(NULL, 0, RenderThread, this, 0, &nThreadId);
      if (m_hRenderThread)
      {
         m_bRun = true;
         EnableDlgItems(false);
      }
   }
}

void CBatchCompile::OnOK() 
{
	CDialog::OnOK();
   CWinApp*pApp = AfxGetApp();
	CString str, strItem;
   int i, nCount = m_cCompileList.GetItemCount();
   for (i=0; i<nCount; i++)
   {
      str.Format(BATCH_JOBX, i);
      strItem = m_cCompileList.GetItemText(i, 0);
//      if (strItem == _T("--")) break;
      pApp->WriteProfileString(BATCH_JOB, str, strItem);
   }	
   if (!m_strPath.IsEmpty())
   {
      pApp->WriteProfileString(BATCH_JOB, BATCH_EXE, m_strPath);
   }
   if (!m_strCommands.IsEmpty())
   {
      pApp->WriteProfileString(BATCH_JOB, BATCH_CMDS, m_strCommands);
   }
}

void CBatchCompile::OnSelectCompiler() 
{
   UpdateData(true);
   CFileDialog dialog(true, "EXE", m_strPath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Ausführbare Datei (*.exe)|*.exe||", this);
   if (dialog.DoModal() ==IDOK)
   {
      m_strPath = dialog.GetPathName();
      UpdateData(false);
   }
}

void CBatchCompile::OnCancel() 
{
   if (m_hRenderThread)
   {
      DWORD dwExitCode;
      m_bRun = false;
      do 
      {
         if (m_hRenderThread == NULL)
            break;
         if (!GetExitCodeThread(m_hRenderThread, &dwExitCode))
            break;
      	Sleep(100);
      } while(dwExitCode == STILL_ACTIVE);
      EnableDlgItems(true);
      SetDlgItemText(IDC_TXT_COMPILING_FILE, "");

      return;
   }
	CDialog::OnCancel();
}

void CBatchCompile::EnableDlgItems(bool bEnable)
{
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_BUILD_ALL), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDOK), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_EDT_COMPILER_PATH), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_COMPILE_LIST), bEnable);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_DELETE_FILE), bEnable);
}

void CBatchCompile::OnDeleteFile() 
{
   int i, nItem, nCount = m_cCompileList.GetSelectedCount();
   if (nCount)
   {
      for (i=0; i<nCount; i++)
      {
         nItem =m_cCompileList.GetNextItem(-1, LVNI_SELECTED);
         if (nItem != -1) 
         {
            if (m_cCompileList.GetItemText(nItem, 0) == _T("--"))
               break;
            m_cCompileList.DeleteItem(nItem);
         }
      }
   }
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_DELETE_FILE), false);
}

void CBatchCompile::OnClickCompileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   bool bEnable = false;
   int nSel = m_cCompileList.GetSelectionMark();
   if (nSel != -1)
   {
      if (nSel < (m_cCompileList.GetItemCount()-1))
         bEnable = true;
   }
   ::EnableWindow(::GetDlgItem(m_hWnd, IDC_DELETE_FILE), bEnable);
	*pResult = 0;
}

BOOL CBatchCompile::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
   TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
   CString strTipText;
   UINT nID = pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
      pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = ::GetDlgCtrlID((HWND)nID);
   }

   if (nID != 0)
   {// will be zero on a separator
      pTTTA->lpszText = MAKEINTRESOURCE(nID);
      pTTTA->hinst = AfxGetResourceHandle();
      return(TRUE);
   }

   if (pNMHDR->code == TTN_NEEDTEXTA)
      lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
   else
      _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
   *pResult = 0;

   return TRUE;    // message was handled
}
