// InvoiceFind.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "InvoiceFind.h"
#include "FacturaDoc.h"
#include "FindCustomer.h"
#include "Customer.h"
#include "ETS3DGLRegKeys.h"
#include "..\\CARAPREV\\resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INV_FIND_COLUMNS "InvFindCol"
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInvoiceFind 
int  CInvoiceFind::gm_nWhat[COLUMNS_INVOICEFIND]  = {IDS_INVOICE_NO, IDS_CUSTOMER_NO, IDS_CUSTOMER_NAME, IDS_CUSTOMER_FIRSTNAME, IDS_CUSTOMER_CITY, IDS_INVOICE_DATE, IDS_CUSTOMER_COMPANY, IDS_CUSTOMER_PC, IDS_CUSTOMER_STREET, IDS_CUSTOMER_EMAIL};
long CInvoiceFind::gm_lOldFindEditWndProc = 0;
int  CInvoiceFind::gm_nWidth[COLUMNS_INVOICEFIND] = {70, 50, 80, 85, 90, 70, 100, 50, 80, 80};
int  CInvoiceFind::gm_nOrder[COLUMNS_INVOICEFIND] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

CInvoiceFind::CInvoiceFind(CWnd* pParent /*=NULL*/)
	: CDialog(CInvoiceFind::IDD, pParent)
{
   //{{AFX_DATA_INIT(CInvoiceFind)
   m_strSearchFor = _T("");
   m_strWhat = _T("");
   //}}AFX_DATA_INIT
   m_nItem         = -1;
   m_nColumn       =  0;
   m_nSearchStrLen =  0;
   m_nCount        =  0;
   m_nFullTextItem = -1;
   m_pList         = NULL;
   m_bExternalSearch = false;
   CWinApp *pApp = AfxGetApp();
   BYTE* pData  = NULL;
   UINT  nBytes = 0;
   pApp->GetProfileBinary(REGKEY_DEFAULT, INV_FIND_COLUMNS, &pData, &nBytes);
   if (pData)
   {
      if (nBytes == (sizeof(int)*COLUMNS_INVOICEFIND*2))
      {
         memcpy(gm_nWidth, pData, sizeof(int)*COLUMNS_INVOICEFIND*2);
      }
      delete pData;
   }
   else
   {
      for (int i=0; i<COLUMNS_INVOICEFIND; i++)
      {
         gm_nWidth[i] |= 0x00010000;
      }
   }
   m_nMaskFlags = 0;
}

CInvoiceFind::~CInvoiceFind()
{
   if (m_pList) delete[] m_pList;
}

void CInvoiceFind::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CInvoiceFind)
   DDX_Control(pDX, IDC_FIND_LIST, m_FindList);
   DDX_Text(pDX, IDC_FIND_EDIT, m_strSearchFor);
   DDX_Text(pDX, IDC_FIND_STR, m_strWhat);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInvoiceFind, CDialog)
   //{{AFX_MSG_MAP(CInvoiceFind)
   ON_EN_CHANGE(IDC_FIND_EDIT, OnChangeFindEdit)
   ON_NOTIFY(LVN_COLUMNCLICK, IDC_FIND_LIST, OnColumnclickFindList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_FIND_LIST, OnGetdispinfoFindList)
	ON_BN_CLICKED(IDC_FIND_FULL_TEXT, OnFindFullText)
	ON_WM_KEYDOWN()
   ON_MESSAGE(WM_HELP, OnHelp)
	ON_MESSAGE(WM_USER, OnUserMsg)
	ON_NOTIFY(NM_RCLICK, IDC_FIND_LIST, OnRclickFindList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CInvoiceFind 
BOOL CInvoiceFind::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_FindList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT);
//   m_FindList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_OWNERDRAWFIXED,LVS_OWNERDRAWFIXED);
   CString string;
   int i;
   for (i=0; i<COLUMNS_INVOICEFIND; i++)
   {
      string.LoadString(gm_nWhat[i]);
      m_FindList.InsertColumn(i, string, LVCFMT_LEFT, (HIWORD(gm_nWidth[i]) != 0) ? LOWORD(gm_nWidth[i]) : 0, 0);
   }
   m_FindList.SetColumnOrderArray(COLUMNS_INVOICEFIND, gm_nOrder);
   CWinApp *pApp = AfxGetApp();
   if (!pApp)             return false;
   if (!pApp->m_pMainWnd) return false;
   CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
   if (!pDoc)             return false;
   CPtrList * pInvoiceList  = &pDoc->m_Invoices;
   POSITION  pos;
   m_nCount = pInvoiceList->GetCount();
   m_pList  = new InvoiceFind[m_nCount];
   m_FindList.DeleteAllItems();
   m_FindList.SetItemCount(m_nCount);

   CPtrList * pCustomerList  = &pDoc->m_Customers;
   int nCustCount   = pCustomerList->GetCount();
   Customer**ppList = (Customer**) new BYTE[sizeof(Customer*)*nCustCount];
   Customer  C, *pC =&C;
   void *pFound;
   pos = pCustomerList->GetHeadPosition();
   i = 0;
   while (pos)
   {
      ppList[i++] = (Customer*) pCustomerList->GetNext(pos);
   }

   i = 0;
   pos = pInvoiceList->GetHeadPosition();
   while (pos)
   {
      m_pList[i].pI = (Invoice*) pInvoiceList->GetNext(pos);
      if (m_pList[i].pI && (m_pList[i].pI->nInvNo != -1))
      {
         C.nNo = m_pList[i].pI->nCustNo;
         pFound = bsearch(&pC, ppList, nCustCount, sizeof(Customer*), CFindCustomer::CompareNo);
         if (pFound)
         {
            if (m_nMaskFlags)
            {
               if (!((*(Customer**)pFound)->nFlags & m_nMaskFlags))
                  continue;
            }
            m_pList[i].pC = (*(Customer**)pFound);
            i++;
         }
         else
         {
            CString format, string;
            format.LoadString(IDS_CUSTOMER_NOTAVAILABLE);
            string.Format(format, m_pList[i].pI->nYear, m_pList[i].pI->nMonth, m_pList[i].pI->nInvNo);
            int nResult = AfxMessageBox(string, MB_YESNOCANCEL);
            if (nResult==IDYES)
            {
               CCustomer CustomerDlg;
               if (CustomerDlg.DoModal() == IDOK)
               {
                  POSITION p = pCustomerList->FindIndex(CustomerDlg.m_nIndex);
                  if (p)
                  {
                     m_pList[i].pC          = (Customer*)pCustomerList->GetAt(p);
                     m_pList[i].pI->nCustNo = m_pList[i].pC->nNo;
                  }
               }               
            }
            else if (nResult==IDNO) // Rechnung löschen !!!
            {
            }
         }
      }
   }
   if (i!=m_nCount)
   {
      m_nCount = i;
      m_FindList.DeleteAllItems();
      m_FindList.SetItemCount(m_nCount);
   }

   if (ppList) delete[]((BYTE*)ppList);

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
      string.LoadString(gm_nWhat[0]);
      m_strWhat.LoadString(IDS_SEARCHING_FOR);
      m_strWhat += string;
      UpdateData(false);
      PostMessage(WM_USER, 0, 0);
   }

   if (gm_lOldFindEditWndProc == 0)
      gm_lOldFindEditWndProc = ::SetWindowLong(GetDlgItem(IDC_FIND_EDIT)->m_hWnd, GWL_WNDPROC, (long)FindEditSubClassProc);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

Customer * CInvoiceFind::GetCustomerFromNo(CPtrList *plist, int nNo)
{
   POSITION  pos;
   pos = plist->GetHeadPosition();
   Customer *pCustomer;
   while (pos)
   {
      pCustomer  = (Customer*) plist->GetNext(pos);
      if (nNo == pCustomer->nNo) return pCustomer;
   }
   return NULL;
}

void CInvoiceFind::OnChangeFindEdit() 
{
   CDataExchange dx(this, true);
   DDX_Text(&dx, IDC_FIND_EDIT, m_strSearchFor);
   if (!m_strSearchFor.IsEmpty())
   {
      int i, nLength, nNumber;
      const char *ptrI, *ptrS = LPCTSTR(m_strSearchFor);
      CString strSecond;
      switch (m_nColumn)
      {
         case 2: case 3:                                       // Name, Vorname
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
         case 1: nNumber = atoi(m_strSearchFor);
         default: nLength = m_strSearchFor.GetLength(); break;
      }
      if ((m_nItem==-1)||(nLength<m_nSearchStrLen)||(nLength==1)) m_nItem = 0;
      m_nSearchStrLen = nLength;
      char text[64];
      for (i=m_nItem; i<m_nCount; i++)
      {
         InvoiceFind *pIF = &m_pList[i];
         if (m_nColumn == 1)
         {
            if (pIF->pC->nNo == nNumber) break;
         }
         else
         {
            switch (m_nColumn)
            {
               case 0 :
                  wsprintf(text, "%04d%02d%04d", pIF->pI->nYear, pIF->pI->nMonth, pIF->pI->nInvNo);
                  ptrI = text;
                  break;
               case 2 : ptrI = pIF->pC->szName;      break;
               case 3 : ptrI = pIF->pC->szFirstName; break;
               case 4 : ptrI = pIF->pC->szCity;      break;
               case 5 :
                  wsprintf(text, "%02d.%02d.%04d", pIF->pI->nDay, pIF->pI->nMonth, pIF->pI->nYear);
                  ptrI = text;
                  break;
               case 6 : ptrI = pIF->pC->szCompany; break;
               case 7 : ptrI = pIF->pC->szPostalCode; break;
               case 8 : ptrI = pIF->pC->szStreet; break;
               case 9 : ptrI = pIF->pC->szEmail; break;
               default: ptrI = NULL;               break;
            }
            if (!strSecond.IsEmpty())                          // zweite Bedingung
            {
               if (ptrI && (strnicmp(ptrI, ptrS, nLength)==0))
               {
                  if (m_nColumn == 2)                          // Name, Vorname
                  {
                     if (strnicmp(pIF->pC->szFirstName, strSecond, strSecond.GetLength())==0) break;
                  }
                  else                                         // Vorname, Name
                  {
                     if (strnicmp(pIF->pC->szName, strSecond, strSecond.GetLength())==0) break;
                  }
               }
            }
            else if (ptrI && (strnicmp(ptrI, ptrS, nLength)==0)) break;
         }         
      }
      if (i<m_nCount)
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

void CInvoiceFind::OnColumnclickFindList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   
   if ((m_pList != NULL)&&(pNMListView->iSubItem != m_nColumn))
   {
      m_nColumn = pNMListView->iSubItem;
      switch (m_nColumn)
      {
         case 0 : qsort(m_pList, m_nCount, sizeof(InvoiceFind), CInvoiceFind::CompareInvNo        ); break;
         case 1 : qsort(m_pList, m_nCount, sizeof(InvoiceFind), CInvoiceFind::CompareCustNo       ); break;
         case 2 : qsort(m_pList, m_nCount, sizeof(InvoiceFind), CInvoiceFind::CompareCustName     ); break;
         case 3 : qsort(m_pList, m_nCount, sizeof(InvoiceFind), CInvoiceFind::CompareCustFirstName); break;
         case 4 : qsort(m_pList, m_nCount, sizeof(InvoiceFind), CInvoiceFind::CompareCustCity     ); break;
         case 5 : qsort(m_pList, m_nCount, sizeof(InvoiceFind), CInvoiceFind::CompareInvDate      ); break;
         case 6 : qsort(m_pList, m_nCount, sizeof(InvoiceFind), CInvoiceFind::CompareCustCompany  ); break;
         case 7 : qsort(m_pList, m_nCount, sizeof(InvoiceFind), CInvoiceFind::CompareCustPC       ); break;
         case 8 : qsort(m_pList, m_nCount, sizeof(InvoiceFind), CInvoiceFind::CompareCustStreet   ); break;
         case 9 : qsort(m_pList, m_nCount, sizeof(InvoiceFind), CInvoiceFind::CompareCustEmail    ); break;
      }
      m_FindList.InvalidateRect(NULL);
      if (!m_bExternalSearch) m_strSearchFor.Empty();
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

void CInvoiceFind::OnOK() 
{
   LV_ITEM lvItem = { LVIF_STATE|LVIF_IMAGE, 0, 0, 0, LVIS_SELECTED, NULL, 0, 0, NULL};
   m_nItem = -1;
   lvItem.iItem = m_FindList.GetTopIndex();
   for (;lvItem.iItem<m_nCount; lvItem.iItem++)
   {
      m_FindList.GetItem(&lvItem);
      if (lvItem.state & LVIS_SELECTED)
      {
         m_nItem = lvItem.iItem;
         break;
      }
   }

   if (m_nItem != -1)
   {
      CWinApp *pApp = AfxGetApp();
      if (!pApp)             return;
      if (!pApp->m_pMainWnd) return;
      CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
      if (!pDoc)             return;
      CPtrList *plist = &pDoc->m_Invoices;
      POSITION  pos   = plist->GetHeadPosition();
      void *ptr = m_pList[m_nItem].pI;
      int nCount = plist->GetCount();
      for (int i=0; (pos!=NULL) && (i<nCount); i++)
      {
         if (ptr == plist->GetNext(pos))
         {
            m_nItem = i;
            break;
         }
      }
      CDialog::OnOK();
   }
}
void CInvoiceFind::FindItem()
{
   LV_FINDINFO FI = { LVFI_PARTIAL, m_strSearchFor, NULL };
   LV_ITEM lvItem = {LVIF_STATE, 0, 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED, NULL, 0, 0, NULL};
   lvItem.iItem   = m_FindList.FindItem(&FI);
   if (lvItem.iItem != -1)
   {
      m_FindList.SetItemState(lvItem.iItem, &lvItem);
      m_FindList.EnsureVisible(lvItem.iItem, false);
   }
}
void CInvoiceFind::OnCancel() 
{
   CDialog::OnCancel();
}

int CInvoiceFind::strcmpext(const char * str1, const char * str2)
{
   if      (str1[0] == 0) return  2;
   else if (str2[0] == 0) return -2;
   else return strcmp(str1, str2);
}
int __cdecl CInvoiceFind::CompareCustNo(const void* lP1, const void* lP2)
{
   if      (((InvoiceFind*)lP1)->pC->nNo > ((InvoiceFind*)lP2)->pC->nNo) return  1;
   else if (((InvoiceFind*)lP1)->pC->nNo < ((InvoiceFind*)lP2)->pC->nNo) return -1;
   else                                                                  return  0;
}
int __cdecl CInvoiceFind::CompareInvNo(const void* lP1, const void* lP2)
{
   int n1 = ((InvoiceFind*)lP1)->pI->nYear * 1000000 + ((InvoiceFind*)lP1)->pI->nMonth * 10000 + ((InvoiceFind*)lP1)->pI->nInvNo;
   int n2 = ((InvoiceFind*)lP2)->pI->nYear * 1000000 + ((InvoiceFind*)lP2)->pI->nMonth * 10000 + ((InvoiceFind*)lP2)->pI->nInvNo;
   if      (n1 > n2) return  1;
   else if (n1 < n2) return -1;
   else              return  0;
}
int __cdecl CInvoiceFind::CompareCustName(const void* lP1, const void* lP2)
{
   int nComp = strcmpext(((InvoiceFind*)lP1)->pC->szName, ((InvoiceFind*)lP2)->pC->szName);
   if (nComp == 0) nComp = strcmpext(((InvoiceFind*)lP1)->pC->szFirstName, ((InvoiceFind*)lP2)->pC->szFirstName);
   if (nComp == 0) nComp = CompareInvNo(lP1, lP2);
   return nComp;
}
int __cdecl CInvoiceFind::CompareCustFirstName(const void* lP1, const void* lP2)
{
   int nComp = strcmpext(((InvoiceFind*)lP1)->pC->szFirstName, ((InvoiceFind*)lP2)->pC->szFirstName);
   if (nComp == 0) nComp = strcmpext(((InvoiceFind*)lP1)->pC->szName, ((InvoiceFind*)lP2)->pC->szName);
   if (nComp == 0) nComp = CompareInvNo(lP1, lP2);
   return nComp;
}
int __cdecl CInvoiceFind::CompareCustCity(const void* lP1, const void* lP2)
{
   return strcmpext(((InvoiceFind*)lP1)->pC->szCity, ((InvoiceFind*)lP2)->pC->szCity);
}
int __cdecl CInvoiceFind::CompareCustCompany(const void* lP1, const void* lP2)
{
   int nComp = strcmpext(((InvoiceFind*)lP1)->pC->szCompany, ((InvoiceFind*)lP2)->pC->szCompany);
   if (nComp == 0) nComp = CompareInvNo(lP1, lP2);
   return nComp;
}
int __cdecl CInvoiceFind::CompareCustPC(const void* lP1, const void* lP2)
{
   return strcmpext(((InvoiceFind*)lP1)->pC->szPostalCode, ((InvoiceFind*)lP2)->pC->szPostalCode);
}
int __cdecl CInvoiceFind::CompareCustStreet(const void* lP1, const void* lP2)
{
   return strcmpext(((InvoiceFind*)lP1)->pC->szStreet, ((InvoiceFind*)lP2)->pC->szStreet);
}
int __cdecl CInvoiceFind::CompareCustEmail(const void* lP1, const void* lP2)
{
   return strcmpext(((InvoiceFind*)lP1)->pC->szEmail, ((InvoiceFind*)lP2)->pC->szEmail);
}

int __cdecl CInvoiceFind::CompareInvDate(const void* lP1, const void* lP2)
{
   int n1 = ((InvoiceFind*)lP1)->pI->nYear * 10000 + ((InvoiceFind*)lP1)->pI->nMonth * 100 + ((InvoiceFind*)lP1)->pI->nDay;
   int n2 = ((InvoiceFind*)lP2)->pI->nYear * 10000 + ((InvoiceFind*)lP2)->pI->nMonth * 100 + ((InvoiceFind*)lP2)->pI->nDay;
   if      (n1 > n2) return  1;
   else if (n1 < n2) return -1;
   else              return  0;
}

void CInvoiceFind::OnGetdispinfoFindList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NMLVDISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   
   Invoice * pI = m_pList[pDispInfo->item.iItem].pI;
   Customer* pC = m_pList[pDispInfo->item.iItem].pC;
   if(pDispInfo->item.mask & LVIF_TEXT)
   {
      switch (pDispInfo->item.iSubItem)
      {
         case 0: wsprintf(pDispInfo->item.pszText, "%04d%02d%04d", pI->nYear, pI->nMonth, pI->nInvNo); break;
         case 1: wsprintf(pDispInfo->item.pszText, "%d", pC->nNo);                                     break;
         case 2: lstrcpy( pDispInfo->item.pszText, pC->szName   );                                     break;
         case 3: lstrcpy( pDispInfo->item.pszText, pC->szFirstName);                                   break;
         case 4: lstrcpy( pDispInfo->item.pszText, pC->szCity   );                                     break;
         case 5: wsprintf(pDispInfo->item.pszText, "%02d.%02d.%04d", pI->nDay, pI->nMonth, pI->nYear); break;
         case 6: lstrcpy( pDispInfo->item.pszText, pC->szCompany);                                     break;
         case 7: lstrcpy( pDispInfo->item.pszText, pC->szPostalCode);                                  break;
         case 8: lstrcpy( pDispInfo->item.pszText, pC->szStreet);                                      break;
         case 9: lstrcpy( pDispInfo->item.pszText, pC->szEmail);                                       break;
      }
   }
   *pResult = 0;
}

BOOL CInvoiceFind::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
   if (((NMHDR*)lParam)->code == NM_CUSTOMDRAW)
   {
/*
      NMCUSTOMDRAW *pNCD = (NMCUSTOMDRAW*)lParam;
      TRACE("NM_CUSTOMDRAW:%d\n",pNCD->dwDrawStage);
      if (pNCD->dwDrawStage == CDDS_PREPAINT)
      {
         ::SetTextColor(pNCD->hdc, RGB(255, 0, 0));
         return CDRF_NEWFONT;
      }
*/
   }
	return CDialog::OnNotify(wParam, lParam, pResult);
}

LRESULT CInvoiceFind::OnUserMsg(WPARAM, LPARAM)
{
   GetDlgItem(IDC_FIND_EDIT)->SetFocus();
   return 0;
}

LRESULT CALLBACK CInvoiceFind::FindEditSubClassProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
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

void CInvoiceFind::OnFindFullText() 
{
   if (!m_strSearchFor.IsEmpty())
   {
      int i = m_nFullTextItem;
      if      (i == -1      ) i = 0;   // 
      else if (i >= m_nCount) i = 0;
      else                    i++;     //
	   for (; i<m_nCount; i++)
      {
         if (strstr(m_pList[i].pI->szComment, m_strSearchFor))
            break;
         if (strstr(m_pList[i].pC->szCompany, m_strSearchFor))
            break;
         if (strstr(m_pList[i].pC->szCity, m_strSearchFor))
            break;
         if (strstr(m_pList[i].pC->szEmail, m_strSearchFor))
            break;
         if (strstr(m_pList[i].pC->szFaxNo, m_strSearchFor))
            break;
         if (strstr(m_pList[i].pC->szName, m_strSearchFor))
            break;
         if (strstr(m_pList[i].pC->szFirstName, m_strSearchFor))
            break;
         if (strstr(m_pList[i].pC->szPhoneNo, m_strSearchFor))
            break;
         if (strstr(m_pList[i].pC->szPostalCode, m_strSearchFor))
            break;
         if (strstr(m_pList[i].pC->szState, m_strSearchFor))
            break;
         if (strstr(m_pList[i].pC->szStreet, m_strSearchFor))
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

void CInvoiceFind::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

LRESULT CInvoiceFind::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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

void CInvoiceFind::OnRclickFindList(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
   CMenu   menu;
   if (menu.CreatePopupMenu())
   {
      CString string;
      CPoint  pt;
      int i;
      ::GetCursorPos(&pt);
      for (i=0; i<COLUMNS_INVOICEFIND; i++)
      {
         string.LoadString(gm_nWhat[i]);
	      menu.InsertMenu(i, MF_BYPOSITION|MF_STRING, i+10, string);
         menu.CheckMenuItem(i, MF_BYPOSITION|((HIWORD(gm_nWidth[i]) != 0) ? MF_CHECKED : MF_UNCHECKED));
      }
      menu.InsertMenu(i++, MF_BYPOSITION|MF_SEPARATOR);
      string.LoadString(IDS_SAVE_SETTINGS);
      menu.InsertMenu(i++, MF_BYPOSITION|MF_STRING, IDOK, string);
      string.LoadString(IDS_CANCEL);
      menu.InsertMenu(i++, MF_BYPOSITION|MF_STRING, IDCANCEL, string);

      int nPos = menu.TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, this);
      if (nPos == IDOK)
      {
         CWinApp *pApp = AfxGetApp();
         m_FindList.GetColumnOrderArray(gm_nOrder, COLUMNS_INVOICEFIND);
         for (i=0; i<COLUMNS_INVOICEFIND; i++)
         {
            int nWidth = m_FindList.GetColumnWidth(i);
            if (nWidth) gm_nWidth[i] = MAKELONG(nWidth, 1);
         }
         pApp->WriteProfileBinary(REGKEY_DEFAULT, INV_FIND_COLUMNS, (BYTE*)&gm_nWidth[0], sizeof(int)*COLUMNS_INVOICEFIND*2);
      }
      else if (nPos != IDCANCEL)
      {
         int nPos1=0, nPos2=COLUMNS_INVOICEFIND-1;
         if (gm_nWidth[nPos-10] & 0x00010000)
            gm_nWidth[nPos-10] &= ~0x00010000;
         else
            gm_nWidth[nPos-10] |=  0x00010000;
         
         for (i=0; i<COLUMNS_INVOICEFIND; i++)
         {
            if (gm_nWidth[i] & 0x00010000)
            {
               m_FindList.SetColumnWidth(i, LOWORD(gm_nWidth[i]));
            }
            else
            {
               m_FindList.SetColumnWidth(i, 0);
            }
         }
         m_FindList.InvalidateRect(NULL);
      }
   }
	*pResult = 0;
}
