// FindCustomer.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "FacturaDoc.h"
#include "FindCustomer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFindCustomer 
int  CFindCustomer::gm_nWhat[COLUMNS_CUSTOMERFIND] = {IDS_CUSTOMER_NO, IDS_CUSTOMER_NAME, IDS_CUSTOMER_FIRSTNAME, IDS_CUSTOMER_PC, IDS_CUSTOMER_CITY, IDS_CUSTOMER_STREET, IDS_CUSTOMER_COMPANY, IDS_CUSTOMER_EMAIL};
long CFindCustomer::gm_lOldFindEditWndProc = 0;


CFindCustomer::CFindCustomer(CWnd* pParent /*=NULL*/)
	: CDialog(CFindCustomer::IDD, pParent)
{
   //{{AFX_DATA_INIT(CFindCustomer)
   m_strWhat       = _T("");
   m_strSearchFor  = _T("");
   //}}AFX_DATA_INIT
   m_nItem         = -1;
   m_nCount        =  0;
   m_nColumn       =  0;
   m_nSearchStrLen =  0;
   m_nFullTextItem = -1;
   m_ppList        = NULL;
   m_nMultiSelect  = 0;
   m_bExternalSearch = false;
   m_nMaskFlags    = 0;
}

Customer ** CFindCustomer::GetCustomerList()
{
   return m_ppList;
}

CFindCustomer::~CFindCustomer()
{
   if (m_ppList) delete[] ((BYTE*)m_ppList);
}

void CFindCustomer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CFindCustomer)
   DDX_Control(pDX, IDC_FIND_LIST, m_FindList);
   DDX_Text(   pDX, IDC_FIND_STR , m_strWhat);
   DDX_Text(   pDX, IDC_FIND_EDIT, m_strSearchFor);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindCustomer, CDialog)
   //{{AFX_MSG_MAP(CFindCustomer)
   ON_NOTIFY(LVN_COLUMNCLICK, IDC_FIND_LIST, OnColumnclickFindList)
   ON_EN_CHANGE(IDC_FIND_EDIT, OnChangeFindEdit)
   ON_NOTIFY(LVN_GETDISPINFO, IDC_FIND_LIST, OnGetdispinfoFindList)
	ON_MESSAGE(WM_USER, OnUserMsg)
	ON_BN_CLICKED(IDC_FIND_FULL_TEXT, OnFindFullText)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CFindCustomer 

void CFindCustomer::OnCancel() 
{
   m_nItem = -1;   
   CDialog::OnCancel();
}

void CFindCustomer::OnOK() 
{
   CWinApp *pApp = AfxGetApp();
   if (!pApp)             return;
   if (!pApp->m_pMainWnd) return;
   CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
   if (!pDoc)             return;
   bool  bMulti    = (m_nMultiSelect>0) ? true : false;
   m_nMultiSelect  = 0;
   CPtrList *plist = &pDoc->m_Customers;
   LV_ITEM lvItem  = { LVIF_STATE|LVIF_IMAGE, 0, 0, 0, LVIS_SELECTED, NULL, 0, 0, NULL};
   m_nItem         = -1;
   lvItem.iItem    = (bMulti) ? 0 : m_FindList.GetTopIndex();
   for (;lvItem.iItem<m_nCount; lvItem.iItem++)
   {
      m_FindList.GetItem(&lvItem);
      if (lvItem.state & LVIS_SELECTED)
      {
         m_nItem = lvItem.iItem;
         POSITION  pos   = plist->GetHeadPosition();
         void *ptr = m_ppList[m_nItem];
         int nCount = plist->GetCount();
         for (int i=0; (pos!=NULL) && (i<nCount); i++)
         {
            if (ptr == plist->GetNext(pos))
            {
               m_nItem = i;
               if (bMulti)
               {
                  m_ppList[m_nMultiSelect++] = (Customer*) ptr;
               }
               else
               {
                  CDialog::OnOK();
                  return;
               }
            }
         }
      }
   }
   if (bMulti) CDialog::OnOK();
}

BOOL CFindCustomer::OnInitDialog()
{
   CDialog::OnInitDialog();
   CString string;
   if (m_nMultiSelect > 0)
   {
      m_FindList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
      m_FindList.ModifyStyle(LVS_SINGLESEL, 0);
   }
   else
   {
      m_FindList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT);
   }

   int i, nWidth[COLUMNS_CUSTOMERFIND] = {50, 90, 90, 50, 95, 95, 95, 90};
   for (i=0; i<COLUMNS_CUSTOMERFIND; i++)
   {
      string.LoadString(gm_nWhat[i]);
      m_FindList.InsertColumn(i, string, (i==0) ? LVCFMT_RIGHT:LVCFMT_LEFT, nWidth[i]);
   }

   CWinApp *pApp = AfxGetApp();
   if (!pApp)             return false;
   if (!pApp->m_pMainWnd) return false;

   CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
   if (!pDoc)             return false;
   CPtrList *plist =  &pDoc->m_Customers;
   POSITION  pos;
   m_nCount  = plist->GetCount();
   m_ppList  = (Customer**) new BYTE[sizeof(Customer*)*m_nCount];
   m_FindList.DeleteAllItems();
   pos = plist->GetHeadPosition();
   i = 0;
   while (pos)
   {
      Customer *pC = (Customer*) plist->GetNext(pos);
      if (m_nMaskFlags)
      {
         if (!(pC->nFlags & m_nMaskFlags))
            continue;
      }
      m_ppList[i++] = pC;
   }
   m_nCount = i;
   m_FindList.SetItemCount(m_nCount);
   if (m_bExternalSearch)
   {
      UpdateData(false);
      LRESULT     lResult;
      NM_LISTVIEW lv;
      ZeroMemory(&lv, sizeof(NM_LISTVIEW));
      lv.iSubItem = m_nColumn;
      m_nColumn =0;
      OnColumnclickFindList((NMHDR *)&lv, &lResult);
      OnChangeFindEdit();
   }
   else
   {
      string.LoadString(gm_nWhat[m_nColumn]);
      m_strWhat.LoadString(IDS_SEARCHING_FOR);
      m_strWhat += string;
      if (m_nItem != -1)
      {
         m_strSearchFor.Format("%d", m_nItem);
         PostMessage(WM_USER, 0, 0);
      }
   }
   UpdateData(false);

   if (gm_lOldFindEditWndProc == 0)
      gm_lOldFindEditWndProc = ::SetWindowLong(GetDlgItem(IDC_FIND_EDIT)->m_hWnd, GWL_WNDPROC, (long)FindEditSubClassProc);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CFindCustomer::OnColumnclickFindList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   if (pNMListView->iSubItem != m_nColumn)
   {
      m_nColumn = pNMListView->iSubItem;
      switch (m_nColumn)
      {
         case 0 : qsort(&m_ppList[0], m_nCount, sizeof(Customer*), CFindCustomer::CompareNo        ); break;
         case 1 : qsort(&m_ppList[0], m_nCount, sizeof(Customer*), CFindCustomer::CompareName      ); break;
         case 2 : qsort(&m_ppList[0], m_nCount, sizeof(Customer*), CFindCustomer::CompareFirstName ); break;
         case 3 : qsort(&m_ppList[0], m_nCount, sizeof(Customer*), CFindCustomer::ComparePostalCode); break;
         case 4 : qsort(&m_ppList[0], m_nCount, sizeof(Customer*), CFindCustomer::CompareCity      ); break;
         case 5 : qsort(&m_ppList[0], m_nCount, sizeof(Customer*), CFindCustomer::CompareStreet    ); break;
         case 6 : qsort(&m_ppList[0], m_nCount, sizeof(Customer*), CFindCustomer::CompareCompany   ); break;
         case 7 : qsort(&m_ppList[0], m_nCount, sizeof(Customer*), CFindCustomer::CompareEmail     ); break;
      }
      if (!m_bExternalSearch) m_strSearchFor.Empty();
      m_FindList.InvalidateRect(NULL);
      CString string;
      string.LoadString(gm_nWhat[m_nColumn]);
      m_strWhat.LoadString(IDS_SEARCHING_FOR);
      m_strWhat += string;
      CDataExchange dx(this, false);
      DDX_Text(&dx, IDC_FIND_EDIT, m_strSearchFor);
      DDX_Text(&dx, IDC_FIND_STR , m_strWhat);
      PostMessage(WM_USER, 0, 0);
   }	
   *pResult = 0;
}

int __cdecl CFindCustomer::CompareNo(const void* lParam1, const void* lParam2)
{
   if      ( (*((Customer**)lParam1))->nNo > (*((Customer**)lParam2))->nNo) return  1;
   else if ( (*((Customer**)lParam1))->nNo < (*((Customer**)lParam2))->nNo) return -1;
   else                                                                     return  0;
}
int __cdecl CFindCustomer::CompareName(const void* lParam1, const void* lParam2)
{
   int             nComp = strcmpext((*((Customer**)lParam1))->szName     , (*((Customer**)lParam2))->szName     );
   if (nComp == 0) nComp = strcmpext((*((Customer**)lParam1))->szFirstName, (*((Customer**)lParam2))->szFirstName);
   return nComp;
}
int __cdecl CFindCustomer::CompareCity(const void* lParam1, const void* lParam2)
{
   return strcmpext((*((Customer**)lParam1))->szCity, (*((Customer**)lParam2))->szCity);
}
int __cdecl CFindCustomer::CompareFirstName(const void* lParam1, const void* lParam2)
{
   int             nComp = strcmpext((*((Customer**)lParam1))->szFirstName, (*((Customer**)lParam2))->szFirstName);
   if (nComp == 0) nComp = strcmpext((*((Customer**)lParam1))->szName     , (*((Customer**)lParam2))->szName     );
   return nComp;
}
int __cdecl CFindCustomer::ComparePostalCode(const void* lParam1, const void* lParam2)
{
   return strcmpext((*((Customer**)lParam1))->szPostalCode, (*((Customer**)lParam2))->szPostalCode);
}
int __cdecl CFindCustomer::CompareStreet(const void* lParam1, const void* lParam2)
{
   return strcmpext((*((Customer**)lParam1))->szStreet, (*((Customer**)lParam2))->szStreet);
}
int __cdecl CFindCustomer::CompareCompany(const void* lParam1, const void* lParam2)
{
   return strcmpext((*((Customer**)lParam1))->szCompany, (*((Customer**)lParam2))->szCompany);
}
int __cdecl CFindCustomer::CompareEmail(const void* lParam1, const void* lParam2)
{
   return strcmpext((*((Customer**)lParam1))->szEmail, (*((Customer**)lParam2))->szEmail);
}

int CFindCustomer::strcmpext(const char * str1, const char * str2)
{
   if      (str1[0] == 0) return  1;
   else if (str2[0] == 0) return -1;
   else return strcmp(str1, str2);
}

void CFindCustomer::OnChangeFindEdit()
{
   CDataExchange dx(this, true);
   DDX_Text(&dx, IDC_FIND_EDIT, m_strSearchFor);
   if (!m_strSearchFor.IsEmpty())
   {
      int i, nLength, nNumber;
      const char *ptrI, *ptrS = LPCTSTR(m_strSearchFor);       // Such String
      CString strSecond;
      switch (m_nColumn)
      {
         case 1: case 2:                                       // Name, Vorname
         {
            nLength = m_strSearchFor.GetLength();
            for (i=0; i<nLength; i++)
            {
               if (m_strSearchFor.GetAt(i) == '&')             // zweite Bedingung gefunden
               {
                  strSecond = m_strSearchFor.Right(nLength-i-1);
                  nLength   = i;
                  break;
               }
            }
         }break;
         case 0: nNumber = atoi(m_strSearchFor); // Kundennummer
         default: nLength = m_strSearchFor.GetLength(); break;
      }
      if ((m_nItem==-1)||(nLength<m_nSearchStrLen)||(nLength==1)) m_nItem = 0;
      m_nSearchStrLen = nLength;
      for (i=m_nItem; i<m_nCount; i++)
      {
         Customer *pC = m_ppList[i];
         if (m_nColumn == 0)
         {
            if (pC->nNo == nNumber) break;
         }
         else
         {
            switch (m_nColumn)
            {
               case 1 : ptrI = pC->szName;       break;
               case 2 : ptrI = pC->szFirstName;  break;
               case 3 : ptrI = pC->szPostalCode; break;
               case 4 : ptrI = pC->szCity;       break;
               case 5 : ptrI = pC->szStreet;     break;
               case 6 : ptrI = pC->szCompany;    break;
               case 7 : ptrI = pC->szEmail;      break;
               default: ptrI = NULL;             break;
            }

            if (!strSecond.IsEmpty())                          // zweite Bedingung
            {
               if (ptrI && (strnicmp(ptrI, ptrS, nLength)==0))
               {
                  if (m_nColumn == 1)                          // Name, Vorname
                  {
                     if (strnicmp(pC->szFirstName, strSecond, strSecond.GetLength())==0) break;
                  }
                  else                                         // Vorname, Name
                  {
                     if (strnicmp(pC->szName, strSecond, strSecond.GetLength())==0) break;
                  }
               }
            }
            else if (ptrI && (strnicmp(ptrI, ptrS, nLength)==0)) break;
         }         
      }
      if (i<m_nCount)                                          // gefunden ?
      {
         m_nItem   = i;
         LV_ITEM lvI = {LVIF_STATE, 0, 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED, NULL, 0, 0, NULL};
         lvI.iItem = i;
         m_FindList.SetItemState(lvI.iItem, &lvI);
         m_FindList.EnsureVisible(lvI.iItem, false);
      }
   }
   else m_nItem = -1;
   m_nFullTextItem = -1;
}

void CFindCustomer::OnGetdispinfoFindList(NMHDR* pNMHDR, LRESULT* pResult)
{
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   Customer * pC = m_ppList[pDispInfo->item.iItem];
   if(pDispInfo->item.mask & LVIF_TEXT)
   {
      switch (pDispInfo->item.iSubItem)
      {
         case 0: wsprintf(pDispInfo->item.pszText, "%d", pC->nNo   ); break;
         case 1: lstrcpy( pDispInfo->item.pszText, pC->szName      ); break;
         case 2: lstrcpy( pDispInfo->item.pszText, pC->szFirstName ); break;
         case 3: lstrcpy( pDispInfo->item.pszText, pC->szPostalCode); break;
         case 4: lstrcpy( pDispInfo->item.pszText, pC->szCity      ); break;
         case 5: lstrcpy( pDispInfo->item.pszText, pC->szStreet    ); break;
         case 6: lstrcpy( pDispInfo->item.pszText, pC->szCompany   ); break;
         case 7: lstrcpy( pDispInfo->item.pszText, pC->szEmail     ); break;
      }
   }
   if(pDispInfo->item.mask & LVIF_STATE)
   {
      int N =0;
   }
   *pResult = 0;
}

LRESULT CFindCustomer::OnUserMsg(WPARAM, LPARAM)
{
   GetDlgItem(IDC_FIND_EDIT)->SetFocus();
   return 0;
}

LRESULT CFindCustomer::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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

LRESULT CALLBACK CFindCustomer::FindEditSubClassProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   if (Msg == WM_KEYDOWN)
   {
      if ((wParam == 38)|| (wParam == 40))                     // Pfeil hoch, Pfeil runter
      {
         HWND hWndList = ::GetDlgItem(::GetParent(hWnd), IDC_FIND_LIST);
         if (hWndList) ::SendMessage(hWndList, Msg, wParam, lParam);
         return 0;
      }
   }
   else if (Msg == WM_DESTROY)
   {
      LRESULT lResult = CallWindowProc((WNDPROC)gm_lOldFindEditWndProc, hWnd, Msg, wParam, lParam);
      SetWindowLong(hWnd, GWL_WNDPROC, gm_lOldFindEditWndProc);
      gm_lOldFindEditWndProc = 0;
      return lResult;
   }
   return CallWindowProc((WNDPROC)gm_lOldFindEditWndProc, hWnd, Msg, wParam, lParam);
}

/*
void CFindCustomer::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDI) 
{
   if ((nIDCtl == IDC_FIND_LIST) && (lpDI->CtlType == ODT_LISTVIEW) && (lpDI->itemAction & ODA_SELECT))
   {
      DrawFrameControl(lpDI->hDC, &lpDI->rcItem, DFC_BUTTON, DFCS_BUTTONCHECK|DFCS_CHECKED);
   }
   else CDialog::OnDrawItem(nIDCtl, lpDI);
}
*/


void CFindCustomer::OnFindFullText() 
{
   if (!m_strSearchFor.IsEmpty())
   {
      int i = m_nFullTextItem;
      if      (i == -1      ) i = 0;   // 
      else if (i >= m_nCount) i = 0;
      else                    i++;     //
	   for (; i<m_nCount; i++)
      {
         if (strstr(m_ppList[i]->szComment   , m_strSearchFor))
            break;
         if (strstr(m_ppList[i]->szCompany   , m_strSearchFor))
            break;
         if (strstr(m_ppList[i]->szCity      , m_strSearchFor))
            break;
         if (strstr(m_ppList[i]->szEmail     , m_strSearchFor))
            break;
         if (strstr(m_ppList[i]->szFaxNo     , m_strSearchFor))
            break;
         if (strstr(m_ppList[i]->szName      , m_strSearchFor))
            break;
         if (strstr(m_ppList[i]->szFirstName , m_strSearchFor))
            break;
         if (strstr(m_ppList[i]->szPhoneNo   , m_strSearchFor))
            break;
         if (strstr(m_ppList[i]->szPostalCode, m_strSearchFor))
            break;
         if (strstr(m_ppList[i]->szState     , m_strSearchFor))
            break;
         if (strstr(m_ppList[i]->szStreet    , m_strSearchFor))
            break;
         if (strstr(m_ppList[i]->szTitle     , m_strSearchFor))
            break;
      }
      if (i<m_nCount)
      {
         m_nItem   = i;
         LV_ITEM lvI = {LVIF_STATE, 0, 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED, NULL, 0, 0, NULL};
         lvI.iItem = m_nFullTextItem = i;
         m_FindList.SetItemState(lvI.iItem, &lvI);
         m_FindList.EnsureVisible(lvI.iItem, false);
      }
   }
}
