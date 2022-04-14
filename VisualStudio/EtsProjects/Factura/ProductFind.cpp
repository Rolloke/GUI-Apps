// ProductFind.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "FacturaDoc.h"
#include "ProductFind.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CProductFind 
int CProductFind::gm_nWhat[COLUMNS_PRODUCTFIND]  = {IDS_PRODUCT_NO, IDS_PRODUCT_DESCRIPTION, IDS_PRODUCT_COUNT, IDS_PRODUCT_PRICE};


CProductFind::CProductFind(CWnd* pParent /*=NULL*/)
	: CDialog(CProductFind::IDD, pParent)
{
   //{{AFX_DATA_INIT(CProductFind)
   m_strWhat = _T("");
   m_strSearchFor = _T("");
	m_bShowHidden = FALSE;
	//}}AFX_DATA_INIT
   m_nItem         = -1;
   m_nColumn       =  0;
   m_nSearchStrLen =  0;
}


void CProductFind::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CProductFind)
   DDX_Control(pDX, IDC_FIND_LIST, m_FindList);
   DDX_Text(   pDX, IDC_FIND_STR, m_strWhat);
   DDX_Text(   pDX, IDC_FIND_EDIT, m_strSearchFor);
	DDX_Check(pDX, IDC_PRODUCT_PRICE_HIDDEN, m_bShowHidden);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProductFind, CDialog)
   //{{AFX_MSG_MAP(CProductFind)
   ON_NOTIFY(LVN_COLUMNCLICK, IDC_FIND_LIST, OnColumnclickFindList)
   ON_EN_CHANGE(IDC_FIND_EDIT, OnChangeFindEdit)
   ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_FIND_LIST, OnBeginlabeleditFindList)
   ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FIND_LIST, OnEndlabeleditFindList)
	ON_CBN_SELCHANGE(IDC_COMBO_FIND_PRICE, OnSelchangeComboFindPrice)
	ON_CBN_SELCHANGE(IDC_COMBO_FIND_PRODUCT, OnSelchangeComboFindProduct)
	ON_BN_CLICKED(IDC_PRODUCT_PRICE_HIDDEN, OnProductPriceHidden)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CProductFind 

BOOL CProductFind::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_FindList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT,
      LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT);

   CString string;
   int nWidth[COLUMNS_PRODUCTFIND] = {50, 340, 50, 70};
   for (int i=0; i<4; i++)
   {
      string.LoadString(gm_nWhat[i]);
      m_FindList.InsertColumn(i, string, LVCFMT_LEFT, nWidth[i], 0);
   }

   CWinApp *pApp = AfxGetApp();
   if (!pApp)             return false;
   if (!pApp->m_pMainWnd) return false;

   m_pDoc = (CFacturaDoc*) ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
   if (!m_pDoc)             return false;

   m_nProductGroup = 0;
   m_nPriceGroup   = 0;
   InitProductList();

   string.LoadString(gm_nWhat[m_nColumn]);
   m_strWhat.LoadString(IDS_SEARCHING_FOR);
   m_strWhat += string;
   if (m_nItem != -1)
   {
      m_strSearchFor.Format("%d", m_nItem);
      FindItem();
   }
   
   int nItem;
   ProductOrPriceGroup *pPOP;
   POSITION pos;

   SendDlgItemMessage(IDC_COMBO_FIND_PRODUCT, CB_ADDSTRING, 0, (LPARAM) "------");
   nItem = 1;
   pos = m_pDoc->m_ProductGroup.GetHeadPosition();
   while (pos)
   {
      pPOP = (ProductOrPriceGroup*)m_pDoc->m_ProductGroup.GetNext(pos);
      if (pPOP)
      {
         SendDlgItemMessage(IDC_COMBO_FIND_PRODUCT, CB_ADDSTRING, 0, (LPARAM) LPCTSTR(pPOP->strDescription));
         SendDlgItemMessage(IDC_COMBO_FIND_PRODUCT, CB_SETITEMDATA, nItem++, pPOP->nNo);
      }
   }
   SendDlgItemMessage(IDC_COMBO_FIND_PRODUCT, CB_SETCURSEL, 0, 0);

   SendDlgItemMessage(IDC_COMBO_FIND_PRICE, CB_ADDSTRING, 0, (LPARAM) "------");
   nItem = 1;
   pos   = m_pDoc->m_PriceGroup.GetHeadPosition();
   while (pos)
   {
      pPOP = (ProductOrPriceGroup*)m_pDoc->m_PriceGroup.GetNext(pos);
      if (pPOP)
      {
         SendDlgItemMessage(IDC_COMBO_FIND_PRICE, CB_ADDSTRING, 0, (LPARAM) LPCTSTR(pPOP->strDescription));
         SendDlgItemMessage(IDC_COMBO_FIND_PRICE, CB_SETITEMDATA, nItem++, pPOP->nNo);
      }
   }
   SendDlgItemMessage(IDC_COMBO_FIND_PRICE, CB_SETCURSEL, 0, 0);

   UpdateData(false);
	
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CProductFind::OnColumnclickFindList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
   if (pNMListView->iSubItem != m_nColumn)
   {
      m_nColumn = pNMListView->iSubItem;
      switch (m_nColumn)
      {
         case 0 : m_FindList.SortItems(CProductFind::CompareNo         , NULL); break;
         case 1 : m_FindList.SortItems(CProductFind::CompareDescription, NULL); break;
         case 2 : m_FindList.SortItems(CProductFind::CompareCount      , NULL); break;
         case 3 : break;
//            m_FindList.SortItems(CProductFind::ComparePrice      , (DWORD)this); break;
      }
      m_strSearchFor.Empty();
      CString string;
      string.LoadString(gm_nWhat[m_nColumn]);
      m_strWhat.LoadString(IDS_SEARCHING_FOR);
      m_strWhat += string;
      UpdateData(false);
   }	
   *pResult = 0;
}
int __stdcall CProductFind::CompareNo(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   UNUSED(lParamSort);
   if      (((Product*)lParam1)->nNo > ((Product*)lParam2)->nNo) return  1;
   else if (((Product*)lParam1)->nNo < ((Product*)lParam2)->nNo) return -1;
   else                                                          return  0;
}
int __stdcall CProductFind::CompareDescription(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   UNUSED(lParamSort);
   return strcmpext(((Product*)lParam1)->szDescription, ((Product*)lParam2)->szDescription);
}
int __stdcall CProductFind::CompareCount(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   UNUSED(lParamSort);
   if      (((Product*)lParam1)->nCount > ((Product*)lParam2)->nCount) return  1;
   else if (((Product*)lParam1)->nCount < ((Product*)lParam2)->nCount) return -1;
   else                                                                return  0;
}

int __stdcall CProductFind::ComparePrice(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   CProductFind *pThis = (CProductFind*)lParamSort;
   int nPrice = 0;
   if      (((Product*)lParam1)->GetPrice((BYTE)pThis->m_nPriceGroup, &nPrice) > 
            ((Product*)lParam2)->GetPrice((BYTE)pThis->m_nPriceGroup, &nPrice)) return  1;
   else if (((Product*)lParam1)->GetPrice((BYTE)pThis->m_nPriceGroup, &nPrice) < 
            ((Product*)lParam2)->GetPrice((BYTE)pThis->m_nPriceGroup, &nPrice)) return -1;
   else                                                                        return  0;
}
int CProductFind::strcmpext(const char * str1, const char * str2)
{
   if      (str1[0] == 0) return  1;
   else if (str2[0] == 0) return -1;
   else return strcmp(str1, str2);
}


void CProductFind::OnChangeFindEdit() 
{
   if (UpdateData(true) && !m_strSearchFor.IsEmpty())
   {
      if (m_nColumn == 0) FindItem();
      else
      {
         LV_ITEM lvItem = { LVIF_PARAM|LVIF_IMAGE, 0, 0, 0, 0, NULL, 0, 0, NULL};
         int nCount     = m_FindList.GetItemCount();
         int nLength    = m_strSearchFor.GetLength();
         const char *ptrS = LPCTSTR(m_strSearchFor);
         if ((m_nItem == -1)||(nLength < m_nSearchStrLen)) m_nItem = 0;
         m_nSearchStrLen = nLength;
         lvItem.iItem    = m_nItem;
         for (;lvItem.iItem<nCount; lvItem.iItem++)
         {
            m_FindList.GetItem(&lvItem);
            if (lvItem.lParam)
            {
               Product *pC = (Product*) lvItem.lParam;
               if (m_nColumn == 1)
               {
                  if  (strnicmp(pC->szDescription, ptrS, nLength)==0)
                  {
                     m_nItem   = lvItem.iItem;
                     LV_ITEM lvI = {LVIF_STATE, 0, 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED, NULL, 0, 0, NULL};
                     lvI.iItem = lvItem.iItem;
                     m_FindList.SetItemState(lvI.iItem, &lvI);
                     m_FindList.EnsureVisible(lvI.iItem, false);
                     break;
                  }
               }
               else if (m_nColumn == 2)
               {
               }
               else if (m_nColumn == 3)
               {
               }
            }
         }
      }
   }
   else m_nItem = -1;
}


void CProductFind::OnCancel() 
{
   m_nItem = -1;   
   CDialog::OnCancel();
}

void CProductFind::OnOK() 
{
   int nCount;
   LVITEMA lvItem = { LVIF_STATE|LVIF_IMAGE|LVIF_PARAM, 0, 0, 0, LVIS_SELECTED, NULL, 0, 0, NULL};

   nCount       = m_FindList.GetItemCount();
   lvItem.iItem = m_FindList.GetTopIndex();
   m_nItem      = -1;
   for (;lvItem.iItem<nCount; lvItem.iItem++)
   {
      m_FindList.GetItem(&lvItem);
      if (lvItem.state & LVIS_SELECTED)
      {
         POSITION pos = m_pDoc->m_Products.GetHeadPosition();
         m_nItem = 0;
         while (pos)
         {
            if (m_pDoc->m_Products.GetNext(pos) == (void*)lvItem.lParam) break;
            m_nItem++;
         }
         m_nPriceGroup = lvItem.iImage;
         CDialog::OnOK();
         break;
      }
   }
}

void CProductFind::FindItem()
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

void CProductFind::OnBeginlabeleditFindList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	
	*pResult = 0;
}

void CProductFind::OnEndlabeleditFindList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	
	*pResult = 0;
}

void CProductFind::InitProductList()
{
   int nItem = 0, i, nCount, nImage;
   float fPrice;
   POSITION  pos;
   ProductOrPriceGroup *pPOP = NULL;
   Product *pProduct;
   pos = m_pDoc->m_Products.GetHeadPosition();
   char text[256];
   m_FindList.DeleteAllItems();
   while (pos)
   {
      pProduct = (Product*) m_pDoc->m_Products.GetNext(pos);
      if (pProduct)
      {
         if ((m_nProductGroup != 0) && (m_nProductGroup != pProduct->sEAN_Code.ucProductGroup))
            continue;
         if (!m_bShowHidden)
         {
            if (pProduct->sEAN_Code.ucPriceGroup & PRICE_GROUP_ARTIKEL_HIDDEN)
               continue;
         }
         if (m_nPriceGroup   != 0)
         {
            i = -1;
            fPrice = pProduct->GetPrice((BYTE)m_nPriceGroup, &i);
            if (i == -1)continue;
            nCount = 1;
         }
         else
         {
            nCount = pProduct->GetNoOfPrices();
            i=0;
            if(nCount == 1) fPrice = pProduct->GetPrice(0, &i);
         }
         for (i=0; i<nCount; i++)
         {
            char *szCur = "";
            if(nCount > 1) fPrice = pProduct->GetPrice(0, &i);
            wsprintf(text, "%d", pProduct->nNo);
            pPOP = m_pDoc->GetPriceGroup(pProduct->GetPriceGroup(i));
            if(nCount == 1)
            {
               nImage = m_nPriceGroup;
            }
            else
            {
               if (pPOP) nImage = pPOP->nNo;
            }
            if (m_FindList.InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE, nItem, text, nImage, 0, nImage, (long) pProduct) != -1)
            {
               if(nCount == 1)
               {
                  m_FindList.SetItem(nItem, 1, LVIF_TEXT, pProduct->szDescription, 0, 0, 0, 0);
               }
               else if (pPOP)
               {
                  wsprintf(text, "%s, %s", pProduct->szDescription, pPOP->strDescription);
                  m_FindList.SetItem(nItem, 1, LVIF_TEXT, text, 0, 0, 0, 0);
                  szCur = (char*)LPCTSTR(pPOP->strCurrency);
               }
               else
               {
                  m_FindList.SetItem(nItem, 1, LVIF_TEXT, pProduct->szDescription, 0, 0, 0, 0);
               }
               if (pProduct->nCount == NOT_COUNTABLE) strcpy(  text, "--");
               else                                   wsprintf(text, "%d", pProduct->nCount);
               m_FindList.SetItem(nItem, 2, LVIF_TEXT, text, 0, 0, 0, NULL);
               sprintf(text, "%.02f %s", fPrice, szCur);
               m_FindList.SetItem(nItem, 3, LVIF_TEXT, text, 0, 0, 0, NULL);
               nItem++;
            }
         }
      }
   }
}

void CProductFind::OnSelchangeComboFindPrice() 
{
   int nOld = m_nPriceGroup;
   int nSel = SendDlgItemMessage(IDC_COMBO_FIND_PRICE, CB_GETCURSEL, 0, 0);
   if (nSel != CB_ERR)
   {
      m_nPriceGroup = SendDlgItemMessage(IDC_COMBO_FIND_PRICE, CB_GETITEMDATA, nSel, 0);
   }
   if (nOld != m_nPriceGroup)
      InitProductList();
}

void CProductFind::OnSelchangeComboFindProduct() 
{
   int nOld = m_nProductGroup;
   int nSel = SendDlgItemMessage(IDC_COMBO_FIND_PRODUCT, CB_GETCURSEL, 0, 0);
   if (nSel != CB_ERR)
   {
      m_nProductGroup = SendDlgItemMessage(IDC_COMBO_FIND_PRODUCT, CB_GETITEMDATA, nSel, 0);
   }
   if (nOld != m_nProductGroup)
      InitProductList();
}  

void CProductFind::OnProductPriceHidden() 
{
   CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_PRODUCT_PRICE_HIDDEN, m_bShowHidden);
   InitProductList();
}

LRESULT CProductFind::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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
