// Invoice.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "Customer.h"
#include "Invoice.h"
#include "Product.h"
#include "FacturaDoc.h"
#include "FacturaView.h"
#include "FindCustomer.h"
#include "ProductFind.h"
#include "InvoiceFind.h"
#include "MainFrm.h"
#include "EditFormDlg.h"
#include "ListDlg.h"
#include "PrintBankKl.h"
#include "TextEdit.h"
#include "TextLabel.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInvoice 
#define FIND_CUSTOMER      1
#define FIND_CUSTOMER_NAME 2

#define SPEEDY_CHOOSE 0
#define SPEEDY_SAVE   1
#define SPEEDY_FLAGS  2

CInvoice::CInvoice(CWnd* pParent /*=NULL*/)
	: CDialog(CInvoice::IDD, pParent)
{
   InitInvoiceData();
   m_strInvoiceTotalNettoStr  = _T("Betrag (netto), DM:");
   m_strTaxStr                = _T("Mehrwertsteuer (16%) in DM");
   m_strInvoiceTotalStr       = _T("Rechnungsbetrag (Summe total, DM)");
   m_strInvoiceNo     = _T("");
   m_strInvoiceDate   = _T("");
   m_nCustomerIndex   = -1;
   m_nInvoiceIndex    = -1;
   m_nProductIndex    = -1;
   m_nCurrentInvNo    =  0;
   m_nCustomerNo      = -1;
   m_fTax             = 0.16f;
   m_fNNBetrag        = 7.50f;
 	m_nFindCustInvoice = 0;
   m_nPayMod          = -1;
   m_pParent          = NULL;
   m_nSpeedyPos       = -1;
   m_pComment         = NULL;
   m_nRefCurrency     = CB_ERR;
   m_nBaseCurrency    = CB_ERR;
   m_nCurrency        = CB_ERR;

   m_nMaskFlags       = 0;
}

CInvoice::~CInvoice()
{
   DeleteProductList();
   if (m_pComment) delete m_pComment;
}

void CInvoice::DeleteProductList()
{
   POSITION pos = m_InvoiceProduct.GetHeadPosition();
   InvoiceProduct *pPI;
   for (; pos!=NULL;)
   {
      pPI = (InvoiceProduct*) m_InvoiceProduct.GetNext(pos);
      if (pPI)
      {
         if (pPI->pPos) delete pPI->pPos;
         delete pPI;
      }
   }
   m_InvoiceProduct.RemoveAll();
}

void CInvoice::InitInvoiceData()
{
   //{{AFX_DATA_INIT(CInvoice)
	m_bInvoiceCanceled = FALSE;
	m_strBankKlAmount = _T("");
	m_fRabatt = 0.0f;
	m_fRabattAmount = 0.0f;
	//}}AFX_DATA_INIT
   m_strCustomerNo.Empty();
   m_strCustomerName.Empty();
   m_strCustomerCity.Empty();
   m_strCustomerStreet.Empty();
   m_strCustomerCompany.Empty();
   m_strInvoiceNo.Empty();
   m_strInvoiceComment.Empty();

   m_strImportComment.Empty();

   m_strInvoiceTotal      = _T("0.0");
   m_strInvoiceTotalNetto = _T("0.0");
   m_strTax               = _T("0.0");
   m_nInvoiceFlag         = 1;
	m_nPayMod              = -1;
   m_nCurrency            = -1;
	m_bInvoicePaid         = false;
   m_bInvoiceCanceled     = false;
}

void CInvoice::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CInvoice)
	DDX_Control(pDX, IDC_INVOICE_CURRENCY, m_cCurrency);
	DDX_Control(pDX, IDC_INVOICE_PAY_MOD, m_cPayMode);
   DDX_Control(pDX, IDC_PRODUCT_LIST, m_ProductList);
   DDX_Text(pDX, IDC_CUSTOMER_NO, m_strCustomerNo);
   DDX_Text(pDX, IDC_INVOICE_DATE, m_strInvoiceDate);
   DDX_Text(pDX, IDC_INVOICE_NN_AMOUNT, m_strNN_Betrag);
   DDX_Text(pDX, IDC_INVOICE_TOTAL, m_strInvoiceTotal);
   DDX_Text(pDX, IDC_INVOICE_TOTAL_NETTO, m_strInvoiceTotalNetto);
   DDX_Text(pDX, IDC_INVOICE_TOTAL_NETTO_STR, m_strInvoiceTotalNettoStr);
   DDX_Text(pDX, IDC_INVOICE_TOTAL_STR, m_strInvoiceTotalStr);
   DDX_Text(pDX, IDC_INVOICE_TAX, m_strTax);
   DDX_Text(pDX, IDC_INVOICE_TAX_STR, m_strTaxStr);
   DDX_Text(pDX, IDC_INVOICE_COMMENT, m_strInvoiceComment);
   DDX_Radio(pDX, IDC_INVOICE0, m_nInvoiceFlag);
   DDX_Text(pDX, IDC_INVOICE_NO, m_strInvoiceNo);
	DDX_CBIndex(pDX, IDC_INVOICE_PAY_MOD, m_nPayMod);
	DDX_Check(pDX, IDC_INVOICE_PAYED, m_bInvoicePaid);
	DDX_Check(pDX, IDC_INVOICE_CANCELED, m_bInvoiceCanceled);
	DDX_Text(pDX, IDC_INVOICE_BANK_KL_AMOUNT, m_strBankKlAmount);
	//}}AFX_DATA_MAP
/*
   DDX_Text(pDX, IDC_CUSTOMER_COMPANY, m_strCustomerCompany);
   DDX_Text(pDX, IDC_CUSTOMER_NAME, m_strCustomerName);
   DDX_Text(pDX, IDC_CUSTOMER_CITY, m_strCustomerCity);
   DDX_Text(pDX, IDC_CUSTOMER_STREET, m_strCustomerStreet);
*/  
   if (pDX->m_bSaveAndValidate)
   {
	   DDX_Text(pDX, IDC_INVOICE_RABATT_AMOUNT, m_fRabattAmount);
   	DDX_Text(pDX, IDC_INVOICE_RABATT, m_fRabatt);
   	DDV_MinMaxFloat(pDX, m_fRabatt, 0.f, 100.f);
   }
   else
   {
      CString strText;
      strText.Format("%.2f", m_fRabattAmount);
	   DDX_Text(pDX, IDC_INVOICE_RABATT_AMOUNT, strText);
      strText.Format("%.2f", m_fRabatt);
	   DDX_Text(pDX, IDC_INVOICE_RABATT, strText);
   }
}


BEGIN_MESSAGE_MAP(CInvoice, CDialog)
   //{{AFX_MSG_MAP(CInvoice)
   ON_BN_CLICKED(IDC_CUSTOMER_FIND, OnCustomerFind)
   ON_BN_CLICKED(IDC_INVOICE_DELETE, OnInvoiceDelete)
   ON_BN_CLICKED(IDC_INVOICE_FIND, OnInvoiceFind)
   ON_BN_CLICKED(IDC_INVOICE_NEW, OnInvoiceNew)
   ON_BN_CLICKED(IDC_INVOICE_PRINT_ADR_KL, OnInvoicePrintAdrKl)
   ON_BN_CLICKED(IDC_INVOICE_PRINT_NNZS, OnInvoicePrintNnzs)
   ON_BN_CLICKED(IDC_CUSTOMER_NEW, OnCustomerNew)
   ON_BN_CLICKED(IDC_INVOICE_PRINT, OnInvoicePrint)
   ON_EN_CHANGE(IDC_INVOICE_COMMENT, OnChangeInvoice)
   ON_NOTIFY(NM_DBLCLK, IDC_PRODUCT_LIST, OnDblclkProductList)
   ON_BN_CLICKED(IDC_INVOICE_PRINT_ADR_RG_NO, OnInvoicePrintAdrRgNo)
   ON_EN_KILLFOCUS(IDC_INVOICE_NO, OnKillfocusInvoiceNo)
   ON_WM_RBUTTONDBLCLK()
   ON_NOTIFY(LVN_GETDISPINFO, IDC_PRODUCT_LIST, OnGetdispinfoProductList)
   ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PRODUCT_LIST, OnEndlabeleditProductList)
   ON_NOTIFY(NM_RCLICK, IDC_PRODUCT_LIST, OnRclickProductList)
   ON_NOTIFY(NM_RDBLCLK, IDC_PRODUCT_LIST, OnRdblclkProductList)
   ON_NOTIFY(LVN_KEYDOWN, IDC_PRODUCT_LIST, OnKeydownProductList)
   ON_BN_CLICKED(IDC_CUSTOMER_SPEEDY, OnCustomerSpeedy)
	ON_BN_DOUBLECLICKED(IDC_CUSTOMER_FIND, OnDoubleclickedCustomerFind)
	ON_BN_CLICKED(IDC_CUSTOMER_NO, OnCustomerNo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_INVOICE_SPIN, OnDeltaposInvoiceSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CUSTOMER_SPIN, OnDeltaposCustomerSpin)
	ON_BN_CLICKED(IDC_INVOICE_FIND_CUST, OnInvoiceFindCustInvoice)
	ON_CBN_SELCHANGE(IDC_INVOICE_CURRENCY, OnSelchangeInvoiceCurrency)
	ON_CBN_SELCHANGE(IDC_INVOICE_PAY_MOD, OnSelchangeInvoicePayMod)
	ON_BN_CLICKED(IDC_CUSTOMER_SPEEDY_SAVE, OnCustomerSpeedySave)
	ON_BN_CLICKED(IDC_INVOICE_FLAGS, OnInvoiceFlags)
	ON_BN_CLICKED(IDC_CUSTOMER_IMPORT, OnCustomerImport)
	ON_BN_CLICKED(IDC_INVOICE_PRINT_BANK_KL, OnInvoicePrintBankKl)
	ON_EN_KILLFOCUS(IDC_INVOICE_RABATT, OnKillfocusInvoiceRabatt)
	ON_BN_CLICKED(IDC_CUSTOMER_PRT_LOGFILE, OnCustomerPrtLogfile)
	ON_WM_CTLCOLOR()
	ON_EN_KILLFOCUS(IDC_INVOICE_COMMENT, OnKillfocusInvoiceComment)
   ON_EN_CHANGE(IDC_INVOICE_NO, OnChangeInvoice)
	ON_BN_CLICKED(IDC_INVOICE_CANCELED, OnChangeInvoice)
	ON_WM_CONTEXTMENU()
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CInvoice 

void CInvoice::OnCustomerFind() 
{
   CFindCustomer FindCustomer;
   if (m_nMaskFlags) FindCustomer.m_nMaskFlags = m_nMaskFlags;
   if (m_nFindCustInvoice == FIND_CUSTOMER_NAME)
   {
      FindCustomer.m_strSearchFor    = m_strCustomerName;
      FindCustomer.m_bExternalSearch = true;
      FindCustomer.m_nColumn         = 1;
   }
   if (FindCustomer.DoModal() == IDOK)
   {
      m_nCustomerIndex = FindCustomer.m_nItem;
      if (GetCustomerData() && UpdateData(false))
      {
         OnChangeInvoice();
      }
   }
   else m_nFindCustInvoice = 0;
}
void CInvoice::OnCustomerNew() 
{
   CCustomer Customer;
   if (Customer.DoModal() == IDOK)
   {
      m_nCustomerIndex = Customer.m_nIndex;
      if (GetCustomerData() && UpdateData(false))
      {
         OnChangeInvoice();
      }
   }
}

void CInvoice::OnCustomerNo() 
{
   CCustomer Customer;
   Customer.m_nIndex = m_nCustomerIndex;
   if (Customer.DoModal() == IDOK)
   {
      m_nCustomerIndex = Customer.m_nIndex;
      if (GetCustomerData() && UpdateData(false))
      {
         OnChangeInvoice();
      }
   }
}

bool CInvoice::GetCustomerData(int nNo)
{
   Customer *pCustomer = GetCustomer(nNo);
   if (pCustomer==NULL) return false;
   CString SPC(" ");
   m_strCustomerCompany = _T(pCustomer->szCompany);
   if (strlen(pCustomer->szTitle) > 0) m_strCustomerName = _T(pCustomer->szTitle) + SPC;
   else m_strCustomerName.Empty();
   m_strCustomerName   += _T(pCustomer->szFirstName) + SPC + _T(pCustomer->szName);
   m_strCustomerStreet  = _T(pCustomer->szStreet)    + SPC + _T(pCustomer->szStreetNo);
   m_strCustomerCity    = _T(pCustomer->szPostalCode)+ SPC + _T(pCustomer->szCity);
   m_nCustomerNo        = pCustomer->nNo;
   m_strCustomerNo.Format("%d", m_nCustomerNo);

   CFacturaApp *pApp = (CFacturaApp*) AfxGetApp();
   pApp->SetPrinterHandle(::GetDlgItem(m_hWnd, IDC_INVOICE_ADR_FIELD));
   const char * str = NULL;
   if (pApp->m_bPrintAdressWithInvNo) str = m_strInvoiceNo;
   pApp->PrintAdrKl(pCustomer, str);
   pApp->SetPrinterHandle(NULL);
   
   return true;
}
void CInvoice::OnChangeInvoice() 
{
   GetDlgItem(IDOK)->EnableWindow(true);	
}

void CInvoice::OnCancel() 
{
   CDialog::OnCancel();
}

void CInvoice::OnOK() 
{
   CWnd *pOK    = GetDlgItem(IDOK);
   CWnd *pFocus = GetFocus();
   if (pOK == pFocus)
   {
      if (UpdateData(true))
      {
         if (SaveInvoice())
            CDialog::OnOK();
      }
   }
   else if (pFocus)
   {
      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
}

BOOL CInvoice::OnInitDialog() 
{
   if (CDialog::OnInitDialog())
   {
      if (!m_pParent) return false;
      CFacturaDoc* pDoc = m_pParent->GetDocument();
      m_pCustomers = &pDoc->m_Customers;
      m_pInvoices  = &pDoc->m_Invoices;
      m_pProducts  = &pDoc->m_Products;
      m_pPayModes  = &pDoc->m_PayModes;
      m_pCurrUnits = &pDoc->m_CurrencyUnits;
      m_pPriceGroup= &pDoc->m_PriceGroup;
      m_fTax       = pDoc->m_fTax;
      m_fNNBetrag  = pDoc->m_fNNAmount;

      m_ProductList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_TRACKSELECT);

      char text[128];
      int i, nWidth[5]  = {50, 50, 240, 60, 69};
      int nString[5]    = {IDS_PRODUCT_COUNT, IDS_PRODUCT_NO, IDS_PRODUCT_DESCRIPTION, IDS_PRODUCT_PRICE, IDS_PRODUCT_TOTAL};
      LVCOLUMN lvColumn = {LVCF_TEXT|LVCF_WIDTH|LVCF_ORDER, 0, 0, text, 0, 0, 0};
      for (i=0; i<5; i++)
      {
         lvColumn.iSubItem = i;
         lvColumn.cx     = nWidth[lvColumn.iSubItem];
         lvColumn.iOrder = lvColumn.iSubItem;
         if (lvColumn.iSubItem!=2) lvColumn.mask |=  LVCF_SUBITEM;
         LoadString(AfxGetInstanceHandle(), nString[lvColumn.iSubItem], text, 128);
         m_ProductList.InsertColumn(lvColumn.iSubItem, &lvColumn);
      }
      int nOrder[5] = {1, 2, 0, 3, 4};
      m_ProductList.SendMessage(LVM_SETCOLUMNORDERARRAY, 5, (LPARAM)&nOrder);
      m_ProductList.DeleteAllItems();
      GetDlgItem(IDOK)->EnableWindow(false);
      POSITION pos = m_pPayModes->GetHeadPosition();
      while (pos)
      {
         PayMode *pPM = (PayMode*) m_pPayModes->GetNext(pos);
         if (pPM)m_cPayMode.AddString(pPM->szMode);
      }
      pos = m_pCurrUnits->GetHeadPosition();
      i   = 0;
      while (pos)
      {
         CurrencyUnit *pCU = (CurrencyUnit*) m_pCurrUnits->GetNext(pos);
         if (pCU)
         {
            m_cCurrency.AddString(pCU->szUnitSign);
            if (pCU->szUnit[0] == '!')
            {
               m_nBaseCurrency = i;
            }
            if (pCU->fCurrency == 1.0f)
            {
               m_nRefCurrency  = i;
            }
            i++;
         }
      }
      OnInvoiceNew();
      return TRUE;
   }
   return false;
}

void CInvoice::SetDocChanged()
{
   m_pParent->GetDocument()->SetModifiedFlag(true);
}

void CInvoice::OnInvoiceNew()
{
   SaveChangeData();
   _tzset();
   time_t     Timer;
   time(&Timer);
   tm *pTm = localtime(&Timer);
   if (pTm)
   {
      m_nYear  = pTm->tm_year + ((pTm->tm_year < 17) ? 2000 : 1900);
      m_nMonth = pTm->tm_mon+1;
      m_nDay   = pTm->tm_mday;
   }

   m_nInvoiceIndex = -1;
   if (m_pInvoices->GetCount())
   {
      Invoice *pInvoice = (Invoice*) m_pInvoices->GetTail();
      if (pInvoice)
      {
         if ((pInvoice->nMonth == m_nMonth) && (pInvoice->nYear == m_nYear))
            m_nCurrentInvNo = pInvoice->nInvNo +1;
         else
            m_nCurrentInvNo = 1;
      }
      else ASSERT(FALSE);
   }
   else m_nCurrentInvNo = 1;

   InitInvoiceData();
   m_ProductList.DeleteAllItems();
   DeleteProductList();
   SetInvoiceDate();
   UpdateData(false);
   GetDlgItem(IDOK)->EnableWindow(false);
   SetDlgItemText(IDC_INVOICE_ADR_FIELD, "");
}

void CInvoice::OnInvoiceDelete()
{
   if (AfxMessageBox(IDS_DELETE_INVOICE, MB_YESNO|MB_ICONEXCLAMATION) == IDYES)
   {
      Invoice *pInvoice = GetInvoice();
      if (pInvoice)
      {
         POSITION pos = m_pInvoices->Find(pInvoice);
         ASSERT(pos != NULL);
         if (pos) m_pInvoices->RemoveAt(pos);
         DeleteInvoicePositions(pInvoice);
         delete pInvoice;
         DeleteProductList();
         SetDocChanged();
      }
      OnInvoiceNew();
   }
}

void CInvoice::OnInvoiceFind()
{
   SaveChangeData();
   CInvoiceFind dlg;
   if (m_nMaskFlags) dlg.m_nMaskFlags = m_nMaskFlags;
   if (m_nFindCustInvoice)
   {
      dlg.m_strSearchFor    = m_strCustomerName;
      dlg.m_bExternalSearch = true;
      if (m_nFindCustInvoice == FIND_CUSTOMER_NAME) dlg.m_nColumn = 2;
      else                                          dlg.m_nColumn = 3;
   }
   if (dlg.DoModal()==IDOK)
   {
      m_nInvoiceIndex = dlg.m_nItem;
      Invoice *pInvoice = GetInvoice();
      if (pInvoice && GetCustomerData(pInvoice->nCustNo))
      {
         CString string;
         int nSend = -1;
         if (m_nFindCustInvoice)
         {
            CString format;
            string.Format(IDS_CREATE_NEW_IVOICE_FOR_CUSTOMER, ((!m_strCustomerName.IsEmpty()) ? m_strCustomerName : m_strCustomerCompany), pInvoice->nYear, pInvoice->nMonth, pInvoice->nInvNo);
            for (int i=0; i<pInvoice->nPositions; i++)
            {
               Product *pProduct = GetProductFromNo(pInvoice->pPosition[i].nProductNo);
               if (pProduct)
               {
                  format.Format("%d * Art.Nr. %d : %s\n", pInvoice->pPosition[i].nCount, pProduct->nNo, pProduct->szDescription);
                  if ((pProduct->nNo == 5) || (pProduct->nNo == 6) || (pProduct->nNo == 7))
                     nSend = pProduct->nNo;
                  string += format;
               }
            }
         }
         if (m_nFindCustInvoice && (AfxMessageBox(string, MB_YESNO)== IDYES))
         {
            CString str;
            if (!m_strImportComment.IsEmpty()) str = m_strImportComment;
            OnInvoiceNew();
            if (!str.IsEmpty()) m_strImportComment = m_strInvoiceComment = str;

            if (GetCustomerData(pInvoice->nCustNo))
            {
               m_nCustomerNo   = pInvoice->nCustNo;                  // Kundennummer
               UpdateData(false);
            }
         }
         else
         {
            if (InitInvoiceData(pInvoice))
               UpdateData(false);
         }
      }
   }
   else m_nFindCustInvoice = 0;
}

bool CInvoice::SaveInvoice()
{
   if ((m_strCustomerNo.IsEmpty() && m_strCustomerName.IsEmpty() && m_strCustomerCity.IsEmpty() && m_strCustomerStreet.IsEmpty()) ||
       (m_nPayMod == -1)||
       (m_InvoiceProduct.GetCount() == 0))
   {
      if (AfxMessageBox(IDS_DATA_INCOMPLETE, MB_YESNO|MB_ICONEXCLAMATION) == IDNO)
         return false;
   }
   Invoice *pInvoice = NULL;
   if (m_nInvoiceIndex == -1)
   {
      pInvoice = new Invoice;
      POSITION pos = NULL;
      if (m_nCurrentInvNo == -1) pos = m_pInvoices->AddHead(pInvoice);
      else                       pos = m_pInvoices->AddTail(pInvoice);
      if (pos == NULL)
      {
         delete pInvoice;
         pInvoice = NULL;
      }
      m_nInvoiceIndex = m_pInvoices->GetCount()-1;
   }
   else
   {
      POSITION pos = m_pInvoices->FindIndex(m_nInvoiceIndex);
      if (pos) pInvoice = (Invoice*)m_pInvoices->GetAt(pos);
   }

   if (pInvoice == NULL) return false;

   pInvoice->nInvNo     = m_nCurrentInvNo;                     // Rechnungsnummer
   pInvoice->nCustNo    = m_nCustomerNo;                       // Kundennummer
   pInvoice->nYear      = m_nYear;                             // Jahr
   pInvoice->nMonth     = m_nMonth;                            // Monat
   pInvoice->nDay       = m_nDay;                              // nDay
   pInvoice->fRabatt    = m_fRabatt;
   switch (m_nInvoiceFlag)                                     // Flags für Angebot, Rechnung, Bezahlt,...
   {
      case 0: pInvoice->nFlags = FLAG_SUPPLY;  break;
      case 1: pInvoice->nFlags = FLAG_INVOICE; break;
   }
   if (m_bInvoicePaid)     pInvoice->nFlags |= FLAG_PAID;
   if (m_bInvoiceCanceled) pInvoice->nFlags |= FLAG_CANCELED;
   POSITION pos = m_pPayModes->FindIndex(m_nPayMod);
   if (pos)
   {
      PayMode *pPM = (PayMode*) m_pPayModes->GetAt(pos);
      if (pPM) pInvoice->nFlags |= (pPM->nMode << 16);
   }

   int nLen = m_strCurrencyUnit.GetLength();
   if (nLen < 8)
   {
      strcpy(pInvoice->szUnitSign, LPCTSTR(m_strCurrencyUnit));
      pInvoice->szUnitSign[nLen] = 0;
   }

   DeleteInvoicePositions(pInvoice);
   pInvoice->nPositions = m_ProductList.GetItemCount();        // Anzahl Positionen
   if (pInvoice->nPositions>0) pInvoice->pPosition  = new Position[pInvoice->nPositions];  // Positionen
   else                        pInvoice->pPosition  = NULL;

   pos = m_InvoiceProduct.GetHeadPosition();
   InvoiceProduct *pPI;
   for (int i=0; (pos!=NULL)&&(i<pInvoice->nPositions); i++)
   {
      pPI = (InvoiceProduct*) m_InvoiceProduct.GetNext(pos);
      if (pPI)
      {
         pInvoice->pPosition[i].nProductNo = pPI->pPro->nNo;
         pInvoice->pPosition[i].nCount     = pPI->pPos->nCount;
         pInvoice->pPosition[i].cPriceGroup= pPI->pPos->cPriceGroup;

         if (pPI->pPro->nCount != NOT_COUNTABLE)
         {
            pPI->pPro->nCount -= pInvoice->pPosition[i].nCount;
         }
      }
   }

   pInvoice->FreeAllText();
   if (!m_strInvoiceComment.IsEmpty())
   {
      pInvoice->szComment = strdup(m_strInvoiceComment);
   }
   if (m_nCustomerNo != -1)
      m_pParent->GetDocument()->SetActualInvoice(pInvoice);
   SetDocChanged();

   return true;
}

Invoice * CInvoice::GetInvoice()
{
   if (m_nInvoiceIndex == -1)  return false;
   POSITION pos = m_pInvoices->FindIndex(m_nInvoiceIndex);
   if (pos==NULL)       return false;
   return (Invoice*) m_pInvoices->GetAt(pos);
}

Customer * CInvoice::GetCustomer(int nNo)
{
   if (nNo != -1)
   {
      POSITION pos = m_pCustomers->GetHeadPosition();
      for(int i=0; pos!=NULL; i++)
      {
         Customer *pCustomer = (Customer*) m_pCustomers->GetNext(pos);
         if (pCustomer && (pCustomer->nNo == nNo))
         {
            m_nCustomerIndex = i;
            return pCustomer;
         }
      }
      m_nCustomerIndex = -1;
   }
   else
   {
      if (m_nCustomerIndex == -1)  return NULL;
      POSITION pos = m_pCustomers->FindIndex(m_nCustomerIndex);
      if (pos==NULL)               return NULL;
      return (Customer*) m_pCustomers->GetAt(pos);
   }
   return NULL;
}
Product * CInvoice::GetProduct(int nIndex)
{
   if (nIndex== -1)  return NULL;
   POSITION pos = m_pProducts->FindIndex(nIndex);
   if (pos==NULL)    return NULL;
   return (Product*) m_pProducts->GetAt(pos);
}
Product* CInvoice::GetProductFromNo(int nNo)
{
   POSITION pos = m_pProducts->GetHeadPosition();
   while (pos)
   {
      Product *pProduct = (Product*) m_pProducts->GetNext(pos);
      if (pProduct && (pProduct->nNo == nNo)) return pProduct;
   }
   return NULL;
}
void CInvoice::SetInvoiceDate()
{
   CTime time(m_nYear, m_nMonth, m_nDay, 1, 1, 1, 0);
   setlocale( LC_ALL, "German");
   m_strInvoiceDate = time.FormatGmt("%d. %B %Y");
   m_strInvoiceNo.Format("%04d%02d%04d", m_nYear, m_nMonth, m_nCurrentInvNo);
}

void CInvoice::OnInvoicePrintAdrKl()
{
   Customer *pCustomer = GetCustomer();
   if (pCustomer)
   {
      CFacturaApp *pApp = (CFacturaApp*) AfxGetApp();
      const char * str = NULL;
      if (pApp->m_bPrintAdressWithInvNo) str = m_strInvoiceNo;
      pApp->PrintAdrKl(pCustomer, str);
   }
   
}
void CInvoice::OnInvoicePrintNnzs()
{
   Customer *pCustomer = GetCustomer();
   if (pCustomer && UpdateData(true))
   {
      CFacturaApp *pApp = (CFacturaApp*) AfxGetApp();
      int nOffset = 0;
      if (pApp->GetStandardPrinter(NNZS_PRINTER))
      {
         char text[32];
         int  nMax = 27;
//         pApp->StandardPrint(nOffset, ""   , 1, 2, nMax);
         pApp->StandardPrint(nOffset, "ELAC Techn. Software GmbH"   , 1, 2, nMax);
         pApp->StandardPrint(nOffset, "7480734            21040010" , 1, 2, nMax);
         pApp->StandardPrint(nOffset, "Commerzbank AG Kiel"         , 1, 2, nMax);
         CurrencyUnit *pInvCU = NULL;
         POSITION pos = m_pCurrUnits->FindIndex(m_nCurrency);                 // Währung der Rechnung
         if (pos) pInvCU  = (CurrencyUnit*)m_pCurrUnits->GetAt(pos);
         if (pInvCU) wsprintf(text, "%s %s", pInvCU->szUnitSign, m_strNN_Betrag);
         else        wsprintf(text, "EUR %s", m_strNN_Betrag);
         pApp->StandardPrint(11, text, 1, 2, nMax);
         if (strlen(pCustomer->szName)==0)
            CFacturaApp::MakeFormatS2(text, pCustomer->szCompany, pCustomer->szCity, nMax);
         else
            CFacturaApp::MakeFormatS2(text, pCustomer->szName   , pCustomer->szCity, nMax);
         pApp->StandardPrint(nOffset, text, 1, 2, nMax);
         wsprintf(text, "ETS Rg.-Nr.:%s", m_strInvoiceNo);
         pApp->StandardPrint(nOffset, text, 1, 2, nMax);
         pApp->StandardPrint(nOffset,"\f", 0, 0, 1);                     // Formfeed
         pApp->CloseStandardPrinter();                                   // Printer schließen
      }
   }
}

void CInvoice::OnInvoicePrint()
{
   if (SaveChangeData())
   {
      m_pParent->m_bPrintDirect = true;
      m_pParent->OnExtraPrintForm();
      m_pParent->m_bPrintDirect = false;
   }
}

void CInvoice::OnDblclkProductList(NMHDR* pNMHDR, LRESULT* pResult)
{
   int nItem = ListView_GetSelectionMark(pNMHDR->hwndFrom);
   if (nItem != -1)
   {
      ListView_EditLabel(pNMHDR->hwndFrom, nItem);
      *pResult = 1;
   }
   else
   {
      *pResult = 0;
   }
}

void CInvoice::OnRdblclkProductList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnRclickProductList(pNMHDR, pResult);
}
void CInvoice::OnRclickProductList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   UNUSED(pNMHDR);

   Product * pProduct = NULL;
   UINT Flags = 0;
   POINT ptCursor, ptScreen;
   CMenu Menu, * pContext = NULL;
   LV_ITEM lvItem = { LVIF_STATE|LVIF_PARAM|LVIF_IMAGE, -1, 0, 0, LVIS_SELECTED, NULL, 0, 0, NULL};

   GetCursorPos(&ptCursor);
   ptScreen = ptCursor;
   m_ProductList.ScreenToClient(&ptCursor);
   ptCursor.x = 2;
   Menu.LoadMenu(IDR_PRODUCT_LISTMENU);
   pContext = Menu.GetSubMenu(0);
   if (pContext)
   {
      lvItem.iItem = m_ProductList.HitTest(ptCursor, &Flags);

      if ((lvItem.iItem != -1) && (Flags & LVHT_ONITEMLABEL))
      {
         m_ProductList.GetItem(&lvItem);
         if ((lvItem.state & LVIS_SELECTED)==0)
         {
            lvItem.iItem = -1;
         }
      }
      if (lvItem.iItem == -1)
      {
         pContext->EnableMenuItem(ID_EDIT_PRODUCT_DELETE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
      }
      switch (pContext->TrackPopupMenu(TPM_RETURNCMD, ptScreen.x, ptScreen.y, this))
      {
         case ID_EDIT_PRODUCT_DELETE:
            DeleteProduct(lvItem.iItem);
            CheckConvert();
            CalculateAmount();
            OnChangeInvoice();
          break;
         case ID_EDIT_PRODUCT_FIND:
         {
            Product * pProduct = NULL;
            CProductFind FindProduct;
            if (FindProduct.DoModal() == IDOK)
            {
               pProduct = GetProduct(FindProduct.m_nItem);
            }
            if (pProduct)
            {
               Position Pos;
               Pos.nCount = 1;
               Pos.cPriceGroup = (BYTE)FindProduct.m_nPriceGroup;
               InsertProduct(pProduct, &Pos);
               CheckConvert();
               CalculateAmount();
               OnChangeInvoice();
            }
         } break;
         case ID_EDIT_PRODUCT_NEW:
         {
            CProduct dlg;
            if (dlg.DoModal() == IDOK)
            {
               Position Pos;
               Pos.nCount = 1;
               Pos.cPriceGroup = (BYTE)dlg.m_nPriceGroup;
               InsertProduct(GetProductFromNo(dlg.m_nNo), &Pos);
               CheckConvert();
               CalculateAmount();
               OnChangeInvoice();
            }
         }break;
         default: 
          break;
      }
   }
   Menu.DestroyMenu();
   *pResult = 0;
}

void CInvoice::CheckConvert()
{
   POSITION pos;
   InvoiceProduct *pPI;
   CurrencyUnit   *pProdCU = NULL;
   CurrencyUnit   *pInvCU  = NULL;

   pos = m_pCurrUnits->FindIndex(m_nCurrency);                 // Währung der Rechnung
   if (pos) pInvCU  = (CurrencyUnit*)m_pCurrUnits->GetAt(pos);

   m_bConvert = false;
   pos = m_InvoiceProduct.GetHeadPosition();
   while(pos)
   {
      pPI = (InvoiceProduct*) m_InvoiceProduct.GetNext(pos);
      if (pPI)
      {
         pProdCU = m_pParent->GetDocument()->GetCurrencyOfPriceGroup(pPI->pPos->cPriceGroup);
         if ((pProdCU != NULL) && (pInvCU != NULL) && (pProdCU != pInvCU))
         {
             m_bConvert = true;                                  // nur eine muß unterschiedlich sein
             break;
         }
      }
   }
}

void CInvoice::CalculateAmount()
{
   POSITION pos;
   InvoiceProduct *pPI;
   float           fTotalNetto = 0.0, fTax, fTotal, fPrice;
   const char     *pszUnit = "";
   CurrencyUnit   *pBaseCU = NULL;
   CurrencyUnit   *pRefCU  = NULL;
   CurrencyUnit   *pProdCU = NULL;
   CurrencyUnit   *pInvCU  = NULL;

   pos = m_pCurrUnits->FindIndex(m_nBaseCurrency);             // Basiswährung
   if (pos) pBaseCU = (CurrencyUnit*)m_pCurrUnits->GetAt(pos);
   pos = m_pCurrUnits->FindIndex(m_nRefCurrency);              // Referenzwährung
   if (pos) pRefCU  = (CurrencyUnit*)m_pCurrUnits->GetAt(pos);
   pos = m_pCurrUnits->FindIndex(m_nCurrency);                 // Währung der Rechnung
   if (pos) pInvCU  = (CurrencyUnit*)m_pCurrUnits->GetAt(pos);

   pos = m_InvoiceProduct.GetHeadPosition();
   while(pos)
   {
      pPI = (InvoiceProduct*) m_InvoiceProduct.GetNext(pos);
      if (pPI)
      {
         fPrice  = pPI->pPro->GetPrice(pPI->pPos->cPriceGroup) * pPI->pPos->nCount;
         if (m_bConvert)
         {
            pProdCU = m_pParent->GetDocument()->GetCurrencyOfPriceGroup(pPI->pPos->cPriceGroup);
            if ((pProdCU != NULL) && (pRefCU != NULL) && (pProdCU != pRefCU))
            {
               fPrice /= pProdCU->fCurrency;                   // Umrechnen in Referenzwährung, prüfen !!!
            }
         }
         fTotalNetto += fPrice;
      }
   }

   if ((m_nCurrency == CB_ERR) ||                              // Keine Währung gewählt
       (m_nCurrency == m_nBaseCurrency))                       // oder Basiswährung
   {
      if (pBaseCU)
      {
         pszUnit = pBaseCU->szUnitSign;
         if (m_bConvert) fTotalNetto *= pBaseCU->fCurrency;
      }
      m_strInvoiceTotalNettoStr.Format("Betrag (netto, %s):", pszUnit);
      m_strTaxStr.Format("Mehrwertsteuer (%.0f%%) in %s", m_fTax*100.0f, pszUnit);
      m_strInvoiceTotalStr.Format("Rechnungsbetrag (Summe total, %s)", pszUnit);
      fTax   = fTotalNetto * m_fTax;
      fTotal = fTotalNetto + fTax;
      CalculateRabatt(fTotal);

      m_strInvoiceTotalNetto.Format("%.02f", fTotalNetto);
      m_strTax.Format(              "%.02f", fTax);
      m_strInvoiceTotal.Format(     "%.02f", fTotal);
      m_strNN_Betrag.Format(        "%.02f", fTotal);
      m_strBankKlAmount.Format(     "%.02f", fTotal+m_fNNBetrag);
   }
   else if ((m_nCurrency == m_nRefCurrency) && (pInvCU != NULL))// Referenzeinheit, in Europa : Euro
   {
      if (pRefCU)
      {
         pszUnit = pRefCU->szUnitSign;
         if (!m_bConvert) fTotalNetto /= pInvCU->fCurrency;    // nur konvertieren,wenn ungleich
      }
      m_strInvoiceTotalNettoStr.Format("Betrag (netto, %s):", pszUnit);
      m_strTaxStr.Format("Mehrwertsteuer (%.0f%%) in %s", m_fTax*100.0f, pszUnit);
      m_strInvoiceTotalStr.Format("Rechnungsbetrag (Summe total, %s)", pszUnit);

      fTax   = fTotalNetto * m_fTax;
      fTotal = fTotalNetto + fTax;
      CalculateRabatt(fTotal);

      m_strInvoiceTotalNetto.Format("%.02f", fTotalNetto);
      m_strTax.Format(              "%.02f", fTax);
      m_strInvoiceTotal.Format(     "%.02f", fTotal);
      m_strNN_Betrag.Format(        "%.02f", fTotal);
      m_strBankKlAmount.Format(     "%.02f", fTotal+m_fNNBetrag);
   }
   else if (pBaseCU && pInvCU && pRefCU)                       // andere Währungen
   {
      if (m_bConvert)
         m_strInvoiceTotalNettoStr.Format("Betrag (netto, %s):", pRefCU->szUnitSign);
      else
         m_strInvoiceTotalNettoStr.Format("Betrag (netto, %s):", pInvCU->szUnitSign);

      m_strTaxStr.Empty();
      m_strInvoiceTotalStr.Format("Betrag (netto, %s)", pInvCU->szUnit);

      m_strInvoiceTotalNetto.Format("%.02f", fTotalNetto);

      if (m_bConvert) fTotal = fTotalNetto * pInvCU->fCurrency;
      else            fTotal = fTotalNetto;

      CalculateRabatt(fTotal);

      m_strTax.Empty();
      m_strInvoiceTotal.Format(     "%.02f", fTotal);
      m_strNN_Betrag.Empty();
      m_strBankKlAmount.Empty();
   }

   UpdateData(false);
}

void CInvoice::CalculateRabatt(float &fTotal)
{
   if (m_fRabatt > 0)
   {
      m_fRabattAmount = m_fRabatt * 0.01f * fTotal;
      fTotal -= m_fRabattAmount;
   }
   else m_fRabattAmount = 0;
}

bool CInvoice::SaveChangeData()
{
   bool bReturn = false;
   if (GetDlgItem(IDOK)->IsWindowEnabled())
   {
      if (UpdateData(true) && (AfxMessageBox(IDS_SAVE_INVOICE, MB_YESNO) == IDYES))
      {
         bReturn = SaveInvoice();
         if (bReturn) GetDlgItem(IDOK)->EnableWindow(false);
      }
      else bReturn = false;
   }
   else bReturn = true;
   return bReturn;
}

void CInvoice::OnInvoicePrintAdrRgNo()
{
   UpdateData(true);
}


void CInvoice::InsertProduct(Product *pProduct, Position *pPos)
{
   InvoiceProduct *pPI = new InvoiceProduct;
   pPI->pPos = new Position;
   pPI->pPos->nCount      = pPos->nCount;
   pPI->pPos->cPriceGroup = pPos->cPriceGroup;
   pPI->pPos->cDummy      = pPos->cDummy;
   pPI->pPro = pProduct;
   m_InvoiceProduct.AddTail(pPI);
   int nCount = m_ProductList.GetItemCount();
   if (nCount == 0)
   {
      CurrencyUnit *pCU = m_pParent->GetDocument()->GetCurrencyOfPriceGroup(pPI->pPos->cPriceGroup);
      if (pCU) m_nCurrency = m_cCurrency.FindString(-1, pCU->szUnitSign);
      else     m_nCurrency = m_nBaseCurrency;
      m_cCurrency.SetCurSel(m_nCurrency);
      m_cCurrency.GetWindowText(m_strCurrencyUnit);
   }
//   m_ProductList.DeleteAllItems();
   m_ProductList.SetItemCount(nCount+1);
}
void CInvoice::DeleteProduct(int nIndex)
{
   POSITION pos = m_InvoiceProduct.FindIndex(nIndex);
   if (pos)
   {
      InvoiceProduct *pPI = (InvoiceProduct*) m_InvoiceProduct.GetAt(pos);
      m_InvoiceProduct.RemoveAt(pos);
      if (pPI)
      {
         if (pPI->pPos) delete pPI->pPos;
         delete pPI;
      }
      int nCount = m_ProductList.GetItemCount();
      m_ProductList.DeleteAllItems();
      if (nCount>0) m_ProductList.SetItemCount(nCount-1);
   }
}

void CInvoice::OnKillfocusInvoiceNo() 
{
   CString strOld = m_strInvoiceNo;
   if (UpdateData(true) && !m_strInvoiceNo.IsEmpty())
   {
      CString strNo, strTemp = m_strInvoiceNo;
      strNo.Format("%04d%02d", m_nYear, m_nMonth);
      int nLeft = strTemp.Find(strNo);
      if (nLeft!=-1) strTemp = _T((&LPCTSTR(strTemp)[nLeft+6]));
      if (!strTemp.IsEmpty() && (strTemp.GetLength() <= 4))
      {
         int i = 0, nInvNo = atoi(strTemp);
         POSITION pos = m_pInvoices->GetHeadPosition();
         while (pos)
         {
            Invoice *pInvoice = (Invoice*) m_pInvoices->GetNext(pos);
            if ( pInvoice && (i++ != m_nInvoiceIndex) &&
                (pInvoice->nInvNo == nInvNo) &&
                (pInvoice->nYear  == m_nYear) &&
                (pInvoice->nMonth == m_nMonth) &&
                (pInvoice->nDay   == m_nDay))
            {
               AfxMessageBox(IDS_INVOICENO_EXITS);
               break;
            }
         }
         if (pos == NULL)
         {
            m_nCurrentInvNo = nInvNo;
            GetDlgItem(IDC_INVOICE_NO)->EnableWindow(false);
            return;
         }
         else
         {
            GetDlgItem(IDC_INVOICE_NO)->SetFocus();
         }
      }
   }
   m_strInvoiceNo = strOld;
   UpdateData(false);
}

void CInvoice::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
   CWnd *pInvoiceNo = GetDlgItem(IDC_INVOICE_NO);
   CRect rcWnd;
   if (pInvoiceNo)
   {
      pInvoiceNo->GetWindowRect(&rcWnd);
      CPoint pt = point;
      ClientToScreen(&pt);
      if (rcWnd.PtInRect(pt))
      {
         pInvoiceNo->EnableWindow(true);
         pInvoiceNo->SetFocus();
      }
   }
   CDialog::OnRButtonDblClk(nFlags, point);
}

void CInvoice::OnContextMenu(CWnd* /*pWnd*/, CPoint point) 
{
   CWnd *pPrintAdrKl = GetDlgItem(IDC_INVOICE_PRINT_ADR_KL);
   CRect rcWnd;
   if (pPrintAdrKl)
   {
      pPrintAdrKl->GetWindowRect(&rcWnd);
      CPoint pt = point;
//      ClientToScreen(&pt);
      if (rcWnd.PtInRect(pt))
      {
         CMenu Menu;
         Menu.LoadMenu(IDR_INV_PRT_ADR_KL);
         CMenu * pContext = Menu.GetSubMenu(0);
         if (pContext)
         {  
            CFacturaApp *pApp = (CFacturaApp*)AfxGetApp();
            pContext->CheckMenuItem(ID_PRT_NAME_OF_ADR , ((pApp->m_bPrintNameOfAdress)    ? MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
            pContext->CheckMenuItem(ID_PRT_ADR_W_INV_NO, ((pApp->m_bPrintAdressWithInvNo) ? MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
            switch (pContext->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, this))
            {
               case ID_PRT_NAME_OF_ADR:
                  pApp->m_bPrintNameOfAdress = !pApp->m_bPrintNameOfAdress;
                  break;
               case ID_PRT_ADR_W_INV_NO:
                  pApp->m_bPrintAdressWithInvNo = !pApp->m_bPrintAdressWithInvNo;
                  break;
               case IDCANCEL: default:break;
            }
         }
      }
   }
   CWnd *pCustFind = GetDlgItem(IDC_CUSTOMER_FIND);
   if (pCustFind)
   {
      pCustFind->GetWindowRect(&rcWnd);
      CPoint pt = point;
      if (rcWnd.PtInRect(pt))
      {
         CCustomer ccust;
         ccust.m_nCustomerFlags = 0;
         ccust.OnCustomerFlags();
         if (ccust.m_nCustomerFlags)
         {
            m_nMaskFlags = ccust.m_nCustomerFlags;
            OnCustomerFind();
            m_nMaskFlags = 0;
         }
      }
   }
   pCustFind = GetDlgItem(IDC_INVOICE_FIND_CUST);
   if (pCustFind)
   {
      pCustFind->GetWindowRect(&rcWnd);
      CPoint pt = point;
      if (rcWnd.PtInRect(pt))
      {
         CCustomer ccust;
         ccust.m_nCustomerFlags = 0;
         ccust.OnCustomerFlags();
         if (ccust.m_nCustomerFlags)
         {
            m_nMaskFlags = ccust.m_nCustomerFlags;
            OnInvoiceFindCustInvoice();
            m_nMaskFlags = 0;
         }
      }
   }
   pCustFind = GetDlgItem(IDC_INVOICE_FIND);
   if (pCustFind)
   {
      pCustFind->GetWindowRect(&rcWnd);
      CPoint pt = point;
      if (rcWnd.PtInRect(pt))
      {
         CCustomer ccust;
         ccust.m_nCustomerFlags = 0;
         ccust.OnCustomerFlags();
         if (ccust.m_nCustomerFlags)
         {
            m_nMaskFlags = ccust.m_nCustomerFlags;
            OnInvoiceFind();
            m_nMaskFlags = 0;
         }
      }
   }
}

void CInvoice::OnGetdispinfoProductList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if(pDispInfo->item.mask & LVIF_TEXT)
   {
      POSITION pos = m_InvoiceProduct.FindIndex(pDispInfo->item.iItem);
      if (pos)
      {
         InvoiceProduct * pIP =(InvoiceProduct*) m_InvoiceProduct.GetAt(pos);
         switch (pDispInfo->item.iSubItem)
         {
            case 0: sprintf(pDispInfo->item.pszText, "%d", pIP->pPos->nCount ); break;
            case 1: sprintf(pDispInfo->item.pszText, "%d", pIP->pPro->nNo    ); break;
            case 2:
            {
               int nIndex = -1;
               pIP->pPro->GetPrice(pIP->pPos->cPriceGroup, &nIndex);
               if (pIP->pPro->IsPriceGroupVisible(nIndex))
               {
                  ProductOrPriceGroup*pPOP = m_pParent->GetDocument()->GetPriceGroup(pIP->pPos->cPriceGroup);
                  if (pPOP)
                  {
                     wsprintf(pDispInfo->item.pszText,"%s %s", pIP->pPro->szDescription, pPOP->strDescription);
                     break;
                  }
               }
               lstrcpy(pDispInfo->item.pszText, pIP->pPro->szDescription);
            } break;
            case 3: sprintf(pDispInfo->item.pszText, "%.02f", pIP->pPro->GetPrice(pIP->pPos->cPriceGroup)); break;
            case 4: sprintf(pDispInfo->item.pszText, "%.02f", pIP->pPro->GetPrice(pIP->pPos->cPriceGroup)*pIP->pPos->nCount); break;
         }
      }
   }
   *pResult = 0;
}

void CInvoice::OnEndlabeleditProductList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   CEdit *pEdit = m_ProductList.GetEditControl();
   *pResult = 0;
   if (m_ProductList.UpdateData(true))
   {
      POSITION pos = m_InvoiceProduct.FindIndex(pDispInfo->item.iItem);
      InvoiceProduct *pPI = (InvoiceProduct*) m_InvoiceProduct.GetAt(pos);
      if (pPI)
      {
         if (pPI->pPos->nCount != (WORD)m_ProductList.m_nValue)
         {
            pPI->pPos->nCount = (WORD)m_ProductList.m_nValue;
            *pResult = 1;
            m_ProductList.Update(pDispInfo->item.iItem);
            CalculateAmount();
            OnChangeInvoice();
         }
      }
   }
}

void CInvoice::DeleteInvoicePositions(Invoice *pInvoice)
{
   if (pInvoice->pPosition)
   {
      for (int i=0; i<pInvoice->nPositions; i++)
      {
         Product *pP = GetProductFromNo(pInvoice->pPosition[i].nProductNo);
         if (pP && pP->nCount != NOT_COUNTABLE)
         {
            pP->nCount += pInvoice->pPosition[i].nCount;
         }
      }
      delete[] pInvoice->pPosition;
      pInvoice->pPosition  = NULL;
      pInvoice->nPositions = 0;
   }
}


void CInvoice::OnKeydownProductList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
   TRACE("Key %d\n", pLVKeyDow->wVKey);
   // TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
   *pResult = 0;
}

void CInvoice::OnCustomerSpeedySave()
{
   if (UpdateData(true))
   {
      m_nSpeedyMode = SPEEDY_SAVE;
      CListDlg ListDlg;
      ListDlg.m_strSelected.LoadString(IDS_SAVE_SPEEDY_CAPTION);
      ListDlg.m_pCallBack = InvoiceSpeedy;
      ListDlg.m_pParam    = this;
      ListDlg.m_bOkButton = true;
      ListDlg.m_szSize.cx = 250;
      ListDlg.m_szSize.cy = 200;

      CString strInvoiceComment = m_strInvoiceComment;

      if (ListDlg.DoModal() == IDOK)
      {
         int nMonth        = m_nMonth;                         // Sichern der alten Daten
         int nCurrentInvNo = m_nCurrentInvNo;
         int nYear         = m_nYear;
         int nCustomerNo   = m_nCustomerNo;
         int nDay          = m_nDay;
         int nInvoiceIndex = m_nInvoiceIndex;
         
         m_nCurrentInvNo = -1;
         m_nYear         = -1;
         m_nCustomerNo   = -1;
         m_nDay          = -1;
         m_nInvoiceIndex = -1;

         SaveInvoice();                                        // speichern

         m_nMonth        = nMonth;                             // rücksichern
         m_nCurrentInvNo = nCurrentInvNo;
         m_nYear         = nYear;
         m_nCustomerNo   = nCustomerNo;
         m_nDay          = nDay;
         m_nInvoiceIndex = nInvoiceIndex;
      }
      m_strInvoiceComment = strInvoiceComment;
   }
}

void CInvoice::OnCustomerSpeedy()
{
   if (m_nCustomerNo == -1) return;

   m_nSpeedyMode = SPEEDY_CHOOSE;
   CListDlg ListDlg;
   ListDlg.m_nCursel = m_nSpeedyPos;
   ListDlg.m_strSelected.LoadString(IDS_CHOOSE_SPEEDY_CAPTION);
   ListDlg.m_pCallBack = InvoiceSpeedy;
   ListDlg.m_pParam    = this;
   ListDlg.m_bOkButton = true;
   ListDlg.m_szSize.cx = 250;
   ListDlg.m_szSize.cy = 200;

   if (ListDlg.DoModal() == IDOK)
   {
      m_nSpeedyPos = ListDlg.m_nCursel;
      Invoice *pInvoice = (Invoice *) ListDlg.m_pParam;
      if (pInvoice)
      {
         ASSERT(pInvoice->nInvNo == -1);
         int nMonth        = pInvoice->nMonth;                 // Sichern der alten Daten
         int nYear         = pInvoice->nYear;
         int nCustomerNo   = pInvoice->nCustNo;
         int nDay          = pInvoice->nDay;
         int nInvNo        = pInvoice->nInvNo;
         char cOld         = pInvoice->szComment[0];

         pInvoice->szComment[0] = 0;            // Initiaslisieren der Daten
         pInvoice->nCustNo = m_nCustomerNo;
         pInvoice->nDay    = m_nDay;
         pInvoice->nMonth  = m_nMonth;
         pInvoice->nYear   = m_nYear;
         pInvoice->nInvNo  = m_nCurrentInvNo;
         if (InitInvoiceData(pInvoice))
         {
            UpdateData(false);
            OnChangeInvoice();
            if (pInvoice->nFlags & FLAG_PRINT_CUST) 
               OnInvoicePrintAdrKl();

            if (pInvoice->nFlags & FLAG_PRINT_NN) 
               OnInvoicePrintNnzs();

            if (pInvoice->nFlags & FLAG_PRINT_BNK) 
               OnInvoicePrintBankKl();

            if (pInvoice->nFlags & FLAG_PRINT_INV) 
               OnInvoicePrint();
         }
         pInvoice->nMonth  = nMonth;                           // Rücksichern der alten Daten
         pInvoice->nYear   = nYear;
         pInvoice->nCustNo = nCustomerNo;
         pInvoice->nDay    = nDay;
         pInvoice->szComment[0] = cOld;
         pInvoice->nInvNo  = nInvNo;
      }
   }
}

void CInvoice::OnInvoicePrintBankKl() 
{
   CFacturaDoc* pDoc = m_pParent->GetDocument();
   pDoc->m_dlgPrintBankKl.m_strAmount = m_strBankKlAmount;
   pDoc->m_dlgPrintBankKl.PrintBankKl();
}

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM )
{
   Invoice *pI1      = (Invoice*)lParam1;
   Invoice *pI2      = (Invoice*)lParam2;

   return (pI1->nDay > pI2->nDay) ? 1 : -1;
}

int CInvoice::InvoiceSpeedy(CListDlg *pDlg, int nReason)
{
   CInvoice    *pThis = (CInvoice*)pDlg->m_pParam;
   switch (nReason)
   {
      case ONINITDIALOG:
      {
         int i = 0;
         CRect rc;
         CString str;
         pDlg->m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 
            LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT, LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

         pDlg->m_List.ModifyStyle(0, LVS_EDITLABELS|LVS_NOCOLUMNHEADER, 0);
         pDlg->m_List.GetClientRect(&rc);
         pDlg->m_List.InsertColumn(0, "", LVCFMT_LEFT, rc.right-2, 0);

         POSITION pos = pThis->m_pInvoices->GetHeadPosition();
         Invoice *pInvoice;
         while (pos)
         {
            pInvoice = (Invoice*) pThis->m_pInvoices->GetNext(pos);
            if (pInvoice && (pInvoice->nInvNo == -1))
            {
               pDlg->m_List.InsertItem(LVIF_PARAM|LVIF_TEXT, i++, pInvoice->szComment, 0, 0, 0, (long)pInvoice);
            }
            else break;
         }
         if (pThis->m_nSpeedyMode == SPEEDY_SAVE)
         {
            pDlg->m_List.InsertItem(i, "Neuer Speedy");
            pDlg->m_List.SetSelectionMark(i);
            pDlg->m_List.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
            pDlg->m_List.EnsureVisible(i, false);
         }
         else if (pDlg->m_nCursel != -1)
         {
            pDlg->m_List.SetSelectionMark(pDlg->m_nCursel);
            pDlg->m_List.SetItemState(pDlg->m_nCursel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
            pDlg->m_List.EnsureVisible(pDlg->m_nCursel, false);
         }
      
         return 1;
      } break;

      case ONOK:
      if (pThis->m_nSpeedyMode == SPEEDY_SAVE)
      {
         pThis->SetDocChanged();
      }
      else if (pThis->m_bSpeedySorted)    // nicht im SaveModus sortieren
      {
         int      i, nCount = pDlg->m_List.GetItemCount();
         POSITION pos;
         for (i=0; i<nCount; i++)
         {
            pos = pThis->m_pInvoices->Find((void*)pDlg->m_List.GetItemData(i));
            if (pos) pThis->m_pInvoices->RemoveAt(pos);
         }
         for (i=nCount-1; i>=0; i--)
         {
            pThis->m_pInvoices->AddHead((void*)pDlg->m_List.GetItemData(i));
         }
         pThis->SetDocChanged();
      }

      if (pDlg->m_nCursel != -1)
      {
         pDlg->m_pParam = (void*)pDlg->m_List.GetItemData(pDlg->m_nCursel);
         return 1;
      }break;
      case ONDBLCLKLSTLIST: break;
      case ONRCLICKLSTLIST:
      {
         CMenu Menu;
         Menu.LoadMenu(IDR_INV_SPEEDY_ITEM);
         CMenu * pContext = Menu.GetSubMenu(0);
         if (pContext)
         {  
            CFacturaApp *pApp = (CFacturaApp*)AfxGetApp();
            CPoint pt;
            GetCursorPos(&pt);
            pContext->EnableMenuItem(ID_SPEEDY_DELETE  , MF_BYCOMMAND|((pThis->m_nSpeedyMode == SPEEDY_SAVE) && (pDlg->m_nCursel == (pDlg->m_List.GetItemCount()-1))) ? MF_GRAYED : MF_ENABLED);
            pContext->EnableMenuItem(ID_SPEEDY_SETTINGS, MF_BYCOMMAND|(pThis->m_nSpeedyMode == SPEEDY_CHOOSE) ?  MF_ENABLED : MF_GRAYED);
            switch (pContext->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, pDlg))
            {
               case ID_SPEEDY_DELETE:
               if (AfxMessageBox(IDS_DELETE_SPEEDY_CAPTION, MB_YESNO|MB_ICONQUESTION) == IDYES)
               {
                  Invoice *pInvoice = (Invoice*)pDlg->m_List.GetItemData(pDlg->m_nCursel);
                  if (pInvoice)
                  {
                     POSITION pos = pThis->m_pInvoices->Find(pInvoice);
                     ASSERT(pos != NULL);
                     if (pos) pThis->m_pInvoices->RemoveAt(pos);
                     pThis->DeleteInvoicePositions(pInvoice);
                     delete pInvoice;
                     pThis->SetDocChanged();
                  }
                  pDlg->m_List.DeleteItem(pDlg->m_nCursel);
               } break;
               case ID_SPEEDY_SETTINGS:
               if (pThis->m_nSpeedyMode == SPEEDY_CHOOSE)
               {
                  Invoice *pI = (Invoice*)pDlg->m_List.GetItemData(pDlg->m_List.GetSelectionMark());
                  if (pI)
                  {
                     int nOldFlags = pThis->m_nInvoiceFlag;
                     pThis->m_nInvoiceFlag = pI->nFlags;
                     pThis->m_nSpeedyMode  = SPEEDY_FLAGS;

                     CListDlg ListDlg;
                     ListDlg.m_strSelected.LoadString(IDS_CUSTOMER_FLAGS_DLG);
                     ListDlg.m_pCallBack = InvoiceFlags;
                     ListDlg.m_pParam    = pThis;
                     ListDlg.m_bOkButton = true;
                     ListDlg.m_szSize.cx = 200;
                     ListDlg.m_szSize.cy = 200;
                     ListDlg.DoModal();

                     pI->nFlags = pThis->m_nInvoiceFlag;
                     pThis->m_nInvoiceFlag = nOldFlags;
                     pThis->m_nSpeedyMode = SPEEDY_CHOOSE;
                  }
               } break;
            }
         }
      }

      case ONBEGINDRAGLSTLIST:
         if (pThis->m_nSpeedyMode == SPEEDY_CHOOSE)
            return true;
         break;
      case WM_LBUTTONUP:
      if (pDlg->m_pNMHdr)
      {
         NMLISTVIEW *pLV = (NMLISTVIEW*)pDlg->m_pNMHdr;
         if ((pDlg->m_nCursel != -1) &&   // Startpunkt
             (pLV->iItem      != -1) &&   // Endpunkt
             (pLV->iItem != pDlg->m_nCursel))
         {
            int i, nCount = pDlg->m_List.GetItemCount();
            for (i=0; i<nCount; i++)
            {
               Invoice*pI =(Invoice*)pDlg->m_List.GetItemData(i);
               if (pDlg->m_nCursel < pLV->iItem)
               {
                  if       (i==pDlg->m_nCursel)                    pI->nDay = pLV->iItem;
                  else if ((i >pDlg->m_nCursel) && (i<pLV->iItem)) pI->nDay = i-1;
                  else                                             pI->nDay = i;
               }
               else
               {
                  if       (i==pDlg->m_nCursel)                    pI->nDay = pLV->iItem+1;
                  else if ((i >pLV->iItem) && (i<pDlg->m_nCursel)) pI->nDay = i+1;
                  else                                             pI->nDay = i;
               }
            }
            pDlg->m_List.SortItems(CompareFunc, 0);
            pThis->m_bSpeedySorted = true;
         }
      }break;

      case ONENDLABELEDITLSTLIST:
      {
         LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pDlg->m_pNMHdr;
         if ((pDispInfo->item.mask & LVIF_TEXT) &&
             (pDispInfo->item.pszText != NULL) &&
             (pDispInfo->item.iItem >= 0) && 
             (pDispInfo->item.iItem <  pDlg->m_List.GetItemCount()))
         {
            if (pThis->m_nSpeedyMode == SPEEDY_SAVE)
            {
               pThis->m_strInvoiceComment = _T(pDispInfo->item.pszText);
            }
            else
            {
               Invoice *pInvoice = (Invoice*)pDlg->m_List.GetItemData(pDispInfo->item.iItem);
               if (pInvoice)
               {
                  pInvoice->FreeAllText();
                  pInvoice->szComment = strdup(pDispInfo->item.pszText);
                  pThis->SetDocChanged();
               }
            }
            return 1;
         }
      } break;
   }
   return 0;
}

void CInvoice::OnDoubleclickedCustomerFind()
{
   return;
}

void CInvoice::OnDeltaposInvoiceSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
   SaveChangeData();
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
   int nOldPos = m_nInvoiceIndex;
   m_nInvoiceIndex -= pNMUpDown->iDelta;
   
   if      (m_nInvoiceIndex <                        0) m_nInvoiceIndex = (pNMHDR->code != 0) ? m_pInvoices->GetCount()-1 : -1;
   else if (m_nInvoiceIndex >= m_pInvoices->GetCount()) m_nInvoiceIndex = (pNMHDR->code != 0) ? 0 : -1;
   Invoice *pInvoice = GetInvoice();

   if (pInvoice)
   {
      if (InitInvoiceData(pInvoice))
      {
         GetCustomerData(m_nCustomerNo);
         UpdateData(false);
         pNMHDR->code = 1;
      }
      else
      {
         pNMHDR->code = 0;
         OnDeltaposInvoiceSpin(pNMHDR, pResult);
         if (!pNMHDR->code)
            m_nInvoiceIndex = nOldPos;
      }
   }
   else
   {
      m_nInvoiceIndex = nOldPos;
   }
   *pResult = 0;
}

void CInvoice::OnDeltaposCustomerSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
   bool bChange = true;
   if (GetDlgItem(IDOK)->IsWindowEnabled() || (m_ProductList.GetItemCount() > 0))
   {
      if (AfxMessageBox(IDS_CHANGE_CUSTOMER_IN_INVOICE, MB_YESNO) != IDYES) bChange = false;
   }

   if (bChange)
   {
      NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
      int nOldPos = m_nCustomerIndex;
      m_nCustomerIndex -= pNMUpDown->iDelta;
      if      (m_nCustomerIndex <                        0) m_nCustomerIndex = m_pCustomers->GetCount()-1;
      else if (m_nCustomerIndex >= m_pInvoices->GetCount()) m_nCustomerIndex = 0;

      if (GetCustomerData(-1))
      {
         UpdateData(false);
         OnChangeInvoice();
      }
      else
      {
         m_nCustomerIndex = nOldPos;
      }
   }
   *pResult = 0;
}

void CInvoice::OnInvoiceFindCustInvoice()
{
	m_nFindCustInvoice = FIND_CUSTOMER;
   OnInvoiceFind();
	m_nFindCustInvoice = 0;
}

bool CInvoice::InitInvoiceData(Invoice *pInvoice)
{
   if (pInvoice)
   {
      if (pInvoice->nInvNo == -1) return false;
      m_pParent->GetDocument()->SetActualInvoice(pInvoice);
      m_nCurrentInvNo = pInvoice->nInvNo;                   // Rechnungsnummer
      m_nCustomerNo   = pInvoice->nCustNo;                  // Kundennummer
      m_nYear         = pInvoice->nYear;                    // Jahr
      m_nMonth        = pInvoice->nMonth;                   // Monat
      m_nDay          = pInvoice->nDay;                     // nDay
      m_fRabatt       = pInvoice->fRabatt;                  // Rabatt

      if      (pInvoice->nFlags&FLAG_SUPPLY ) m_nInvoiceFlag = 0;
      else if (pInvoice->nFlags&FLAG_INVOICE) m_nInvoiceFlag = 1;
      m_bInvoicePaid     = ((pInvoice->nFlags&FLAG_PAID    )!=0) ? true : false;
      m_bInvoiceCanceled = ((pInvoice->nFlags&FLAG_CANCELED)!=0) ? true : false;

      int n = 0, nPayMod = (pInvoice->nFlags&FLAG_PAY_MOD_MASK) >> 16;
      m_nPayMod = -1;
      POSITION pos = m_pPayModes->GetHeadPosition();
      while (pos)
      {
         PayMode *pPM = (PayMode*) m_pPayModes->GetNext(pos);
         if (pPM && (pPM->nMode == nPayMod))
         {
            m_nPayMod = n; 
            break;
         }
         n++;
      }

      m_strCurrencyUnit   = _T(pInvoice->szUnitSign);

      if (!m_strImportComment.IsEmpty())
         m_strInvoiceComment = m_strImportComment;  // Bankverbindung, IDENT
      else
         m_strInvoiceComment = _T(pInvoice->szComment);
      
      m_ProductList.DeleteAllItems();
      DeleteProductList();
      for (int i=0; i<pInvoice->nPositions; i++)
      {
         Product *pProduct = GetProductFromNo(pInvoice->pPosition[i].nProductNo);
         if (pProduct)
         {
            InsertProduct(pProduct, &pInvoice->pPosition[i]);
         }
         else
         {
            AfxMessageBox(IDS_MISSING_PRODUCT);
         }
      }

      int nSel = m_cCurrency.FindString(-1, pInvoice->szUnitSign);
      if (nSel == CB_ERR)
      {
         m_cCurrency.GetWindowText(pInvoice->szUnitSign, 8);
      }
      else
      {
         m_nCurrency = m_cCurrency.SetCurSel(nSel);
      }
      m_strCurrencyUnit = _T(pInvoice->szUnitSign);

      SetInvoiceDate();
      CheckConvert();
      CalculateAmount();
      GetDlgItem(IDOK)->EnableWindow(false);
      return true;
   }
   return false;
}

void CInvoice::OnSelchangeInvoiceCurrency() 
{
   m_nCurrency = m_cCurrency.GetCurSel();
   if ((m_nCurrency != CB_ERR) && UpdateData(true))
   {
      m_cCurrency.GetWindowText(m_strCurrencyUnit);
      CheckConvert();
      CalculateAmount();
      OnChangeInvoice();
   }
}

void CInvoice::OnSelchangeInvoicePayMod() 
{
   m_nPayMod = m_cPayMode.GetCurSel();
   OnChangeInvoice();
}

void CInvoice::OnInvoiceFlags() 
{
   CListDlg ListDlg;
   ListDlg.m_strSelected.LoadString(IDS_CUSTOMER_FLAGS_DLG);
   ListDlg.m_pCallBack = InvoiceFlags;
   ListDlg.m_pParam    = this;
   ListDlg.m_bOkButton = true;
   ListDlg.m_szSize.cx = 250;
   ListDlg.m_szSize.cy = 250;

   ListDlg.DoModal();
}

int CInvoice::InvoiceFlags(CListDlg *pDlg, int nReason)
{
   CInvoice *pThis = (CInvoice*)pDlg->m_pParam;
   if (nReason == ONINITDIALOG)
   {
      int i;
      CRect rc;
      CString str;
      pDlg->m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
      pDlg->m_List.GetClientRect(&rc);
      pDlg->m_List.InsertColumn(0, "", LVCFMT_LEFT, rc.right, 0);
      CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();

      if (pThis->m_nSpeedyMode == SPEEDY_FLAGS)
      {
         pDlg->m_List.ModifyStyle(0, LVS_NOCOLUMNHEADER, 0);

         for (i=0; i<4; i++)
         {
            str.LoadString(IDS_SPEEDY_PRINT_INV+i);
            pDlg->m_List.InsertItem(i, str);
         }

         for (i=0; i<4;i++)
         {
            if (pThis->m_nInvoiceFlag & (1 << (i+4)))
               ListView_SetCheckState(pDlg->m_List.m_hWnd, i, 1);
         }
      }
      else
      {
         pDlg->m_List.ModifyStyle(0, LVS_NOCOLUMNHEADER|LVS_EDITLABELS, 0);

         for (i=0; i<FLAG_USER_CNT; i++)
            pDlg->m_List.InsertItem(i, pDoc->m_strInvoiceFlags[i]);

         for (i=0; i<FLAG_USER_CNT;i++)
         {
            if (pThis->m_nInvoiceFlag & (1 << (i+8)))
               ListView_SetCheckState(pDlg->m_List.m_hWnd, i, 1);
         }
      }
      return 1;
   }
   else if (nReason == ONOK)
   {
      int i, nCount = FLAG_USER_CNT, 
             nOffset= 8;
      long lFlag;
      bool bChecked, bState, bChanged = false;
      if (pThis->m_nSpeedyMode == SPEEDY_FLAGS)
      {
         nCount  = 4;
         nOffset = 4;
      }
      for (i=0; i<nCount; i++)
      {
         lFlag    = 1 << (i+nOffset);
         bState   = (pThis->m_nInvoiceFlag&lFlag) ? true : false;
         bChecked = ListView_GetCheckState(pDlg->m_List.m_hWnd, i) ? true : false;
         if (bState != bChecked)
         {
            bChanged = true;
            if (bChecked) pThis->m_nInvoiceFlag |=  lFlag;
            else          pThis->m_nInvoiceFlag &= ~lFlag;
         }
      }
      if (bChanged) pThis->OnChangeInvoice();
   }
   else if (nReason == ONENDLABELEDITLSTLIST)
   {
      LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pDlg->m_pNMHdr;
      if ((pDispInfo->item.mask & LVIF_TEXT) &&
          (pDispInfo->item.pszText != NULL) &&
          (pDispInfo->item.iItem >= 0) && 
          (pDispInfo->item.iItem <  FLAG_USER_CNT))
      {
         CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();
         pDoc->m_strInvoiceFlags[pDispInfo->item.iItem] = _T(pDispInfo->item.pszText);
         pDoc->SetModifiedFlag(true);
         return 1;
      }
   }
   return 0;
}

void CInvoice::OnCustomerImport() 
{
   CCustomer Customer;                                         // Customer Dialoginstanz

   Customer.OnCustomerImport();                                // Kunden Importieren
   if (Customer.m_strComment.IsEmpty()) return;

   if (!m_pComment)         m_pComment = new CTextEdit(this);  // Anzeige der Bestellinfos
   if (!IsWindow(m_pComment->m_hWnd)) m_pComment->Create(IDD_TEXTDLG, this);
   m_pComment->m_cTextEdit.SetWindowText(Customer.m_strComment);
   m_pComment->SetWindowPos(NULL, 0, 0, 200, 400, SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_NOZORDER);
   Customer.m_strComment.Empty();                              // Kommentar löschen

   m_strCustomerName   = Customer.m_strName;                   // Kundennamen zum Suchen
                                                               // evtl. Vorname zum suchen
   m_nFindCustInvoice  = FIND_CUSTOMER_NAME;                   // Spezialsuche

   m_strImportComment = m_strInvoiceComment=Customer.m_strCommentOfImport;// enthält Bankverbindung und IDENT Nr.
   if (Customer.m_strCommentOfImport.GetLength() < 17)
      Customer.m_strComment = Customer.m_strCommentOfImport;   // Ident Nr.

   m_strImportAmount  = Customer.m_strAmountOfImport;          // Betrag zum Vergleich
   int i, j;
   for (i=0, j=0; i<Customer.m_strAmountOfImport.GetLength(); i++)// Betrag zum Vergleich
   {
      TCHAR tc = Customer.m_strAmountOfImport.GetAt(i);
      if (isdigit(tc) || (tc == ','))
         m_strImportAmount.SetAt(j++, tc);
   }
   m_strImportAmount.SetAt(j++, 0);
   OnInvoiceFind();                                            // nach Rechnung des Kunden
   if (m_nFindCustInvoice) m_nFindCustInvoice = 3;             // Rechnung des Kunden gefunden
   else                                                        // Rechnung des Kunden nicht gefunden
   {
      m_nFindCustInvoice = FIND_CUSTOMER_NAME;                 // Spezialsuche
      OnCustomerFind();                                        // nach dem Kunden selbst
      if (m_nFindCustInvoice) m_nFindCustInvoice = 3;          // Kunden gefunden
      else                                                     // Kunden nicht gefunden
      {
         if (Customer.SaveNewCustomer())                       // Kunden Speichern
         {
            m_nCustomerIndex = Customer.m_nIndex;
            if (GetCustomerData() && UpdateData(false))
            {
               m_nFindCustInvoice = 3;                         // Kunde gespeichert
               OnChangeInvoice();
            }
         }
      }
   }
}

void CInvoice::OnKillfocusInvoiceRabatt() 
{
   float fRabatt;
   CDataExchange dx(this, true);
  	DDX_Text(&dx, IDC_INVOICE_RABATT, fRabatt);

   if ((fRabatt != m_fRabatt) && (fRabatt >= 0.0f) && (fRabatt <= 100.0f))
   {
      m_fRabatt = fRabatt;
      CalculateAmount();
      OnChangeInvoice();
   }
}


void CInvoice::OnCustomerPrtLogfile() 
{
   AfxGetMainWnd()->SendMessage(WM_COMMAND, (WPARAM)MAKELONG(ID_FILE_PRINT_LOGFILE, 0), (LPARAM)m_hWnd);
}

HBRUSH CInvoice::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   if (pWnd->GetDlgCtrlID() == IDC_INVOICE_TOTAL)
   {
      if (!m_strImportAmount.IsEmpty() && !m_strNN_Betrag.IsEmpty())
      {
         double dTotal        = atof(m_strInvoiceTotal);
         double dImportAmount = atof(m_strImportAmount);
         const char *pstr = strstr(m_strImportAmount, " EUR + ");
         if (pstr) dImportAmount += atof(&pstr[6]);
         if (fabs(dTotal - dImportAmount) > 0.009)
            pDC->SetTextColor(RGB(255,0,0));
//         if (m_strImportAmount != m_strInvoiceTotal)
      }
//   DDX_Text(pDX, IDC_INVOICE_TOTAL, m_strInvoiceTotal);
//   DDX_Text(pDX, IDC_INVOICE_NN_AMOUNT, m_strNN_Betrag);
   }
	return hbr;
}

void CInvoice::OnKillfocusInvoiceComment() 
{
   CDataExchange dx(this, true);
   DDX_Text(&dx, IDC_INVOICE_COMMENT, m_strInvoiceComment);
}


LRESULT CInvoice::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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

