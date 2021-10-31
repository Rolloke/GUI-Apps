// States.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "FacturaDoc.h"
#include "States.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CStates 

int CStates::gm_nWhat[COLUMNS_STATES]  = {IDS_STATE_ID, IDS_STATE_NAME};

CStates::CStates(CWnd* pParent /*=NULL*/)
	: CDialog(CStates::IDD, pParent)
{
   //{{AFX_DATA_INIT(CStates)
	m_strStateID = _T("");
	m_strStateName = _T("");
	//}}AFX_DATA_INIT
   m_pList = NULL;
}


void CStates::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CStates)
   DDX_Control(pDX, IDC_FIND_LIST, m_FindList);
	DDX_Text(pDX, IDC_STATE_ID, m_strStateID);
	DDV_MaxChars(pDX, m_strStateID, 3);
	DDX_Text(pDX, IDC_STATE_NAME, m_strStateName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStates, CDialog)
   //{{AFX_MSG_MAP(CStates)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_FIND_LIST, OnBeginlabeleditFindList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FIND_LIST, OnEndlabeleditFindList)
	ON_BN_CLICKED(IDC_STATE_NEW   , OnStateNew)
	ON_BN_CLICKED(IDC_STATE_DELETE, OnStateDelete)
	ON_BN_CLICKED(IDC_STATE_INSERT, OnStateInsert)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CStates 

void CStates::OnCancel() 
{
   CDialog::OnCancel();
}

void CStates::OnOK() 
{
   CDialog::OnOK();
}

void CStates::OnBeginlabeleditFindList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   *pResult = 0;
}

void CStates::OnEndlabeleditFindList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   // TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   *pResult = 0;
}

BOOL CStates::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   m_FindList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT);
	
   CString string;
   int nWidth[COLUMNS_STATES] = {45, 214};
   for (int i=0; i<COLUMNS_STATES; i++)
   {
      string.LoadString(gm_nWhat[i]);
      m_FindList.InsertColumn(i, string, LVCFMT_LEFT, nWidth[i], 0);
   }

   CWinApp *pApp = AfxGetApp();
   if (!pApp)             return false;
   if (!pApp->m_pMainWnd) return false;

   CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
   if (!pDoc)             return false;
   m_pList = &pDoc->m_States;
   int nItem = 0;
   POSITION  pos;
   State *pState;
   pos = m_pList->GetHeadPosition();
   while (pos)
   {
      pState = (State*) m_pList->GetNext(pos);
      if (pState)
      {
         if (m_FindList.InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE, nItem, pState->szStateSign, 0, 0, nItem, (long) pState) != -1)
         {
            m_FindList.SetItem(nItem, 1, LVIF_TEXT, pState->szState, 0, 0, 0, NULL);
            nItem++;
         }
      }
   }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CStates::OnStateNew() 
{
   m_strStateID.Empty();
   m_strStateName.Empty();
   UpdateData(false);

	GetDlgItem(IDC_STATE_NEW   )->ShowWindow(SW_HIDE);
	GetDlgItem(IDOK            )->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATE_DELETE)->ShowWindow(SW_HIDE);

   GetDlgItem(IDC_STATE_NAME  )->ShowWindow(SW_SHOW);
   GetDlgItem(IDC_STATE_INSERT)->ShowWindow(SW_SHOW);
	CWnd *pWnd = GetDlgItem(IDC_STATE_ID);
   ASSERT(pWnd != NULL);
   pWnd->ShowWindow(SW_SHOW);
   pWnd->SetFocus();
}

void CStates::OnStateDelete() 
{
   int i, nItems = m_FindList.GetItemCount();
   for (i=0; i<nItems; i++)
   {
      LV_ITEM lvItem = { LVIF_STATE|LVIF_PARAM|LVIF_IMAGE, i, 0, 0, LVIS_SELECTED, NULL, 0, 0, NULL};
      m_FindList.GetItem(&lvItem);
      if (lvItem.state & LVIS_SELECTED)
      {
         POSITION pos = m_pList->FindIndex(i);
         if (pos)
         {
            BYTE *pState = (BYTE*)m_pList->GetAt(pos);
            if (pState == (BYTE*)lvItem.lParam)
            {
               m_pList->RemoveAt(pos);
               delete[] pState;
               m_FindList.DeleteItem(i);
               SetDocChanged();
            }
         }
      }
   }
}

void CStates::OnStateInsert() 
{
   if (UpdateData(true))
   {
      int nSize = sizeof(State)+m_strStateName.GetLength()+1;
      State *pState = (State *)new BYTE[nSize];
      if (pState)
      {
         ZeroMemory(pState, nSize);
         pState->nSize = nSize;
         strcpy(pState->szStateSign, m_strStateID);
         strcpy(pState->szState    , m_strStateName);
         m_pList->AddTail(pState);
         int nItem = m_FindList.GetItemCount();
         if (m_FindList.InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE, nItem, pState->szStateSign, 0, 0, nItem, (long) pState) != -1)
         {
            m_FindList.SetItem(nItem, 1, LVIF_TEXT, pState->szState, 0, 0, 0, NULL);
         }
         m_FindList.UpdateWindow();
         SetDocChanged();
      }
   }
	GetDlgItem(IDC_STATE_NEW   )->ShowWindow(SW_SHOW);
	GetDlgItem(IDOK            )->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATE_DELETE)->ShowWindow(SW_SHOW);

   GetDlgItem(IDC_STATE_NAME  )->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_STATE_ID    )->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_STATE_INSERT)->ShowWindow(SW_HIDE);
}

void CStates::SetDocChanged()
{
   CWinApp *pApp = AfxGetApp();
   if (pApp && pApp->m_pMainWnd)
   {
      CDocument* pDoc = ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
      if (pDoc) pDoc->SetModifiedFlag(true);
   }
}

LRESULT CStates::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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
