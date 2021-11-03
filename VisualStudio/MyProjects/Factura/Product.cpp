// Product.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "Product.h"
#include "FacturaDoc.h"
#include "ProductFind.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GROUP_CHANGED            0x00000001
#define NEW_GROUP_ITEM           0x00000002
#define GROUP_ITEM_EDITMODE      0x00000004
#define PRODUCT_GROUP_EDITMODE   0x00000010
#define PRICE_GROUP_EDITMODE     0x00000020
#define COMPANY_CODE_EDITMODE    0x00000040

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CProduct 


CProduct::CProduct(CWnd* pParent /*=NULL*/)
	: CDialog(CProduct::IDD, pParent)
{
   InitProductData();
   m_nBaseCurrency = -1;
}

void CProduct::InitProductData()
{
   //{{AFX_DATA_INIT(CProduct)
   m_fProductPrice = 0.0f;
   m_nNo = 0;
   m_strProductDescription = _T("");
	m_nProductGroup = 0;
	m_nPriceGroup   = -1;
	m_nReleaseL     = 0;
	m_nReleaseH     = 0;
	m_nCompanyCode  = 4039549;
	m_bPriceGroupVisible = FALSE;
	m_bProductHidden = FALSE;
	//}}AFX_DATA_INIT
	m_strGroupEdt = _T("");
   m_nProductCount = 0;
   m_nProductIndex = -1;
   m_dwGroupFlags  = 0;
   m_nPriceGroup   = 0;
   m_nProductGroup = 0;
   m_bPriceChanged = false;
   m_bCurrencyChanged = false;
}


void CProduct::DoDataExchange(CDataExchange* pDX)
{
   int nEANCheck;
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProduct)
	DDX_Control(pDX, IDC_COMBO_PRODUCT_CURRENCY, m_cCurrency);
	DDX_Text(pDX, IDC_PRODUCT_NO, m_nNo);
	DDX_Text(pDX, IDC_PRODUCT_DESCRIPTION, m_strProductDescription);
	DDX_Text(pDX, IDC_EDT_PRODUCT_RELEASEH, m_nReleaseH);
   DDV_MinMaxInt(pDX, m_nReleaseH, 0, 9);
	DDX_Check(pDX, IDC_PRODUCT_PRICE_GROUP_VISIBLE, m_bPriceGroupVisible);
	DDX_Check(pDX, IDC_PRODUCT_PRICE_HIDDEN, m_bProductHidden);
	//}}AFX_DATA_MAP


   CString strCount;
   if (pDX->m_bSaveAndValidate)
   {
	   DDX_Text(pDX, IDC_EDT_PRODUCT_RELEASEL, m_nReleaseL);
	   DDV_MinMaxInt(pDX, m_nReleaseL, 0, 99);
	   DDX_Text(pDX, IDC_PRODUCT_COUNT, strCount);
      if (strCount == _T("--")) m_nProductCount = NOT_COUNTABLE;
      else DDX_Text(pDX, IDC_PRODUCT_COUNT, m_nProductCount);
   	DDX_Text(pDX, IDC_PRODUCT_PRICE, m_fProductPrice);

      pDX->m_bSaveAndValidate = false;
      EAN_Code Code = {m_nCompanyCode, (unsigned char)m_nProductGroup, (unsigned char)m_nReleaseH, (unsigned char)m_nReleaseL, (unsigned char)0};
      nEANCheck = Code.GetEAN();
   	DDX_Text(pDX, IDC_EDT_PRODUCT_EAN, nEANCheck);
      strCount.Format("%02d%1d%02d", Code.ucProductGroup, Code.ucReleaseH, Code.ucReleaseL);
      DDX_Text(pDX, IDC_EDT_PRODUCT_EAN_ART_CODE, strCount);
   }
   else
   {
      if (m_nProductCount == NOT_COUNTABLE)
      {
         strCount = _T("--");
         DDX_Text(pDX, IDC_PRODUCT_COUNT, strCount);
      }
      else DDX_Text(pDX, IDC_PRODUCT_COUNT, m_nProductCount);
      strCount.Format("%02d", m_nProductGroup);
      DDX_Text(pDX, IDC_EDT_PRODUCT_GROUP, strCount);
      strCount.Format("%02d", m_nPriceGroup);
	   DDX_Text(pDX, IDC_EDT_PRODUCT_PRICE_GROUP, strCount);
      strCount.Format("%02d", m_nReleaseL);
	   DDX_Text(pDX, IDC_EDT_PRODUCT_RELEASEL, strCount);
      strCount.Format("%.3f", m_fProductPrice);
   	DDX_Text(pDX, IDC_PRODUCT_PRICE, strCount);
      EAN_Code Code = {m_nCompanyCode, (unsigned char)m_nProductGroup, (unsigned char)m_nReleaseH, (unsigned char)m_nReleaseL, (unsigned char)0};
      nEANCheck = Code.GetEAN();
   	DDX_Text(pDX, IDC_EDT_PRODUCT_EAN, nEANCheck);
      strCount.Format("%02d%1d%02d", Code.ucProductGroup, Code.ucReleaseH, Code.ucReleaseL);
      DDX_Text(pDX, IDC_EDT_PRODUCT_EAN_ART_CODE, strCount);
	   DDX_Text(pDX, IDC_EDT_PRODUCT_COMPANY_CODE, m_nCompanyCode);
   }
}


BEGIN_MESSAGE_MAP(CProduct, CDialog)
	//{{AFX_MSG_MAP(CProduct)
	ON_BN_CLICKED(IDC_PRODUCT_DELETE, OnProductDelete)
	ON_BN_CLICKED(IDC_PRODUCT_NEW, OnProductNew)
	ON_BN_CLICKED(IDC_PRODUCT_FIND, OnProductFind)
	ON_EN_CHANGE(IDC_PRODUCT_DESCRIPTION, OnChangeProduct)
	ON_EN_CHANGE(IDC_PRODUCT_PRICE, OnChangeProductPrice)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_EN_CHANGE(IDC_EDT_PRODUCT_GROUP1, OnEditchangeComboGroup)
	ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT_GROUP, OnSelchangeComboProductGroup)
	ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT_PRICE_GROUP, OnSelchangeComboProductPriceGroup)
	ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT_CURRENCY, OnSelchangeComboProductCurrency)
	ON_BN_CLICKED(IDC_PRODUCT_PRICE_GROUP_VISIBLE, OnChangeProductPrice)
	ON_EN_CHANGE(IDC_PRODUCT_NO, OnChangeProduct)
	ON_EN_CHANGE(IDC_PRODUCT_COUNT, OnChangeProduct)
	ON_EN_CHANGE(IDC_EDT_PRODUCT_PRICE_GROUP1, OnEditchangeComboGroup)
	ON_EN_CHANGE(IDC_EDT_PRODUCT_RELEASEH, OnChangeEAN)
	ON_EN_CHANGE(IDC_EDT_PRODUCT_RELEASEL, OnChangeEAN)
	ON_EN_CHANGE(IDC_EDT_PRODUCT_GROUP, OnEditchangeComboGroup)
	ON_EN_CHANGE(IDC_EDT_PRODUCT_PRICE_GROUP, OnEditchangeComboGroup)
	ON_BN_CLICKED(IDC_PRODUCT_PRICE_HIDDEN, OnChangeProduct)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CProduct 

void CProduct::OnCancel() 
{
   CDialog::OnCancel();
}

void CProduct::OnOK() 
{
   CWnd *pOK    = GetDlgItem(IDOK);
   CWnd *pFocus = GetFocus();
   if (pOK == pFocus)
   {
      if (UpdateData(true))
      {
         SaveProduct();
         pOK->EnableWindow(false);	
      }
   	EndDialog(IDOK);
   }
   else if (pFocus)
   {
      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
}

BOOL CProduct::OnInitDialog()
{
   CDialog::OnInitDialog();
	
   CWinApp *pApp = AfxGetApp();
   if (!pApp)             return false;
   if (!pApp->m_pMainWnd) return false;
   CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
   if (!pDoc)             return false;

   m_pProducts     = &pDoc->m_Products;
   m_pPriceGroup   = &pDoc->m_PriceGroup;
   m_pProductGroup = &pDoc->m_ProductGroup;

   ProductOrPriceGroup *pPOP;
   CurrencyUnit        *pCU;
   POSITION pos;
   int      i;

   SendDlgItemMessage(IDC_COMBO_PRODUCT_GROUP, CB_ADDSTRING, 0, (LPARAM) "------");
   pos = m_pProductGroup->GetHeadPosition();
   while (pos)
   {
      pPOP = (ProductOrPriceGroup*)m_pProductGroup->GetNext(pos);
      if (pPOP)
      {
         SendDlgItemMessage(IDC_COMBO_PRODUCT_GROUP, CB_ADDSTRING, 0, (LPARAM) LPCTSTR(pPOP->strDescription));
      }
   }

   SendDlgItemMessage(IDC_COMBO_PRODUCT_PRICE_GROUP, CB_ADDSTRING, 0, (LPARAM) "------");
   pos = m_pPriceGroup->GetHeadPosition();
   i   = 1;
   while (pos)
   {
      pPOP = (ProductOrPriceGroup*)m_pPriceGroup->GetNext(pos);
      if (pPOP)
      {
         SendDlgItemMessage(IDC_COMBO_PRODUCT_PRICE_GROUP, CB_ADDSTRING  , 0, (LPARAM) LPCTSTR(pPOP->strDescription));
         SendDlgItemMessage(IDC_COMBO_PRODUCT_PRICE_GROUP, CB_SETITEMDATA, i++, (LPARAM)pPOP);
      }
   }

   pos = pDoc->m_CurrencyUnits.GetHeadPosition();
   i   = 0;
   while (pos)
   {
      pCU = (CurrencyUnit*)pDoc->m_CurrencyUnits.GetNext(pos);
      if (pCU)
      {
         if (pCU->szUnit[0] == '!') m_nBaseCurrency = i;
         SendDlgItemMessage(IDC_COMBO_PRODUCT_CURRENCY, CB_ADDSTRING, 0, (LPARAM) LPCTSTR(pCU->szUnitSign));
         SendDlgItemMessage(IDC_COMBO_PRODUCT_CURRENCY, CB_SETITEMDATA, i++, (LPARAM)pCU);
      }
   }

   GetDlgItem(IDOK)->EnableWindow(false);

   if (GetProductData()) UpdateData(false);
   else OnProductNew();

   GetDlgItem(IDC_PRODUCT_DELETE)->EnableWindow(m_nProductIndex != -1);
	
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CProduct::OnProductNew() 
{
   SaveChangedData();
   InitProductData();
   if (m_pProducts->GetCount())
   {
      Product *pProduct =(Product*) m_pProducts->GetTail();
      if (pProduct) m_nNo = pProduct->nNo+1;
      else ASSERT(FALSE);
   }
   else m_nNo = 1;

   SendDlgItemMessage(IDC_COMBO_PRODUCT_GROUP, CB_SETCURSEL, 0, 0);
   m_nPriceGroupSel = SendDlgItemMessage(IDC_COMBO_PRODUCT_PRICE_GROUP, CB_SETCURSEL, 0, 0);
   SendDlgItemMessage(IDC_COMBO_PRODUCT_CURRENCY, CB_SETCURSEL, m_nBaseCurrency, 0);

   UpdateData(false);
}

void CProduct::OnProductFind() 
{
   CProductFind FindProduct;

   SaveChangedData();
   Product *pProduct = GetProduct();
   if (pProduct)
   {
      FindProduct.m_nItem       = pProduct->nNo;
      FindProduct.m_nPriceGroup = m_nPriceGroup;
   }

   if (FindProduct.DoModal() == IDOK)
   {
      m_nProductIndex = FindProduct.m_nItem;
      m_nPriceGroup   = FindProduct.m_nPriceGroup;
      if (GetProductData() && UpdateData(false))
      {
         GetDlgItem(IDOK)->EnableWindow(false);
         GetDlgItem(IDC_PRODUCT_DELETE)->EnableWindow(true);
      }
   }
}

Product * CProduct::GetProduct()
{
   if (m_nProductIndex == -1)  return false;
   POSITION pos = m_pProducts->FindIndex(m_nProductIndex);
   if (pos==NULL)       return false;
   return (Product*) m_pProducts->GetAt(pos);
}

bool CProduct::GetProductData()
{
   Product *pProduct = GetProduct();
   ProductOrPriceGroup *pPOP;
   int    nSel, nPGi;
   if (pProduct==NULL) return false;

   m_nNo           = pProduct->nNo;
   if (m_nPriceGroup != -1) nPGi = -1;
   else                     nPGi =  0;
   m_fProductPrice = pProduct->GetPrice((BYTE)m_nPriceGroup, &nPGi);
   m_nProductCount = pProduct->nCount;
   m_nReleaseL     = pProduct->sEAN_Code.ucReleaseL;
   m_nReleaseH     = pProduct->sEAN_Code.ucReleaseH;
   m_strProductDescription = _T(pProduct->szDescription);
   m_bProductHidden = (pProduct->sEAN_Code.ucPriceGroup & PRICE_GROUP_ARTIKEL_HIDDEN) ? true: false;

   m_nProductGroup = 0;
   pPOP = FindPOPGroup(m_pProductGroup, pProduct->sEAN_Code.ucProductGroup, nSel);
   if (pPOP) m_nProductGroup = pPOP->nNo;
   else      nSel = -1;
   SendDlgItemMessage(IDC_COMBO_PRODUCT_GROUP, CB_SETCURSEL, nSel+1, 0);

   m_nPriceGroup        = pProduct->GetPriceGroup(nPGi);
   m_bPriceGroupVisible = pProduct->IsPriceGroupVisible(nPGi);

   SetPriceGroup();

   return true;
}

void CProduct::OnProductDelete() 
{
   if (m_nProductIndex != -1)
   {
      POSITION pos = m_pProducts->FindIndex(m_nProductIndex);
      if (pos)
      {
         Product *pProduct =(Product*) m_pProducts->GetAt(pos);
         if (pProduct)
         {
            if (m_nPriceGroupSel > 0)
            {
               if (AfxMessageBox("Preisgruppe wirklich löschen ?", MB_YESNO|MB_ICONQUESTION) == IDYES)
               {
                  ProductOrPriceGroup *pPOP = (ProductOrPriceGroup*)SendDlgItemMessage(IDC_COMBO_PRODUCT_PRICE_GROUP, CB_GETITEMDATA, (WPARAM)m_nPriceGroupSel, 0);
                  if (pPOP)
                  {
                     pProduct->RemovePrice((BYTE)pPOP->nNo);
                     m_nPriceGroupSel = -1;
                     m_bPriceChanged = false;
                     SendDlgItemMessage(IDC_COMBO_PRODUCT_PRICE_GROUP, CB_SETCURSEL, 0, 0);
                     OnSelchangeComboProductPriceGroup();
                  }
               }
            }
            else
            {
               if (AfxMessageBox("Produkt wirklich löschen ?", MB_YESNO|MB_ICONQUESTION) == IDYES)
               {
                  m_pProducts->RemoveAt(pos);
                  delete pProduct;
                  GetDlgItem(IDOK)->EnableWindow(false);
                  SetDocChanged();
                  OnProductNew();
               }
            }
         }
      }
   }
}

void CProduct::SaveChangedData()
{
   if (GetDlgItem(IDOK)->IsWindowEnabled() && UpdateData(true))
   {
      if (AfxMessageBox(IDS_SAVE_PRODUCT, MB_YESNO) == IDYES)
      {
         SaveProduct();
      }
   }
}

void CProduct::SaveProduct()
{
   Product *pProduct = NULL;
   if (m_nProductIndex == -1)
   {
      pProduct = new Product;
      if (pProduct)
      {
         m_pProducts->AddTail(pProduct);
      }
   }
   else
   {
      POSITION pos = m_pProducts->FindIndex(m_nProductIndex);
      if (pos)
      {
         pProduct = (Product*) m_pProducts->GetAt(pos);
      }
   }

   if (pProduct == NULL) return;
   pProduct->SetPrice((BYTE)m_nPriceGroup, m_fProductPrice, (m_bPriceGroupVisible!=0)?true:false);
   pProduct->nNo                      = m_nNo;
   pProduct->nCount                   = m_nProductCount;
   pProduct->sEAN_Code.ucProductGroup = (unsigned char)m_nProductGroup;
   pProduct->sEAN_Code.ucReleaseH     = (unsigned char)m_nReleaseH;
   pProduct->sEAN_Code.ucReleaseL     = (unsigned char)m_nReleaseL;
   pProduct->sEAN_Code.ulCompanyEAN   = m_nCompanyCode;
   if (m_bProductHidden) pProduct->sEAN_Code.ucPriceGroup |=  PRICE_GROUP_ARTIKEL_HIDDEN;
   else                  pProduct->sEAN_Code.ucPriceGroup &= ~PRICE_GROUP_ARTIKEL_HIDDEN;

   if (m_nPriceGroupSel != CB_ERR)
   {
      ProductOrPriceGroup *pPOP = (ProductOrPriceGroup*)SendDlgItemMessage(IDC_COMBO_PRODUCT_PRICE_GROUP, CB_GETITEMDATA, m_nPriceGroupSel, 0);
      if (pPOP) GetDlgItem(IDC_COMBO_PRODUCT_CURRENCY)->GetWindowText(pPOP->strCurrency);
   }

   if (!m_strProductDescription.IsEmpty())
   {
      pProduct->FreeAllText();
      pProduct->szDescription = strdup(m_strProductDescription);
   }
   GetDlgItem(IDOK)->EnableWindow(false);
   SetDocChanged();
}

void CProduct::OnChangeProduct() 
{
   GetDlgItem(IDOK)->EnableWindow(true);	
}
void CProduct::OnChangeEAN() 
{
   UpdateData(true);
   OnChangeProduct();
}
void CProduct::OnChangeProductPrice() 
{
   m_bPriceChanged = true;
   OnChangeProduct();
}

void CProduct::SetDocChanged()
{
   CWinApp *pApp = AfxGetApp();
   if (pApp && pApp->m_pMainWnd)
   {
      CDocument* pDoc = ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
      if (pDoc) pDoc->SetModifiedFlag(true);
   }
}

void CProduct::OnEditchangeComboGroup() 
{
   m_dwGroupFlags |= GROUP_CHANGED;
}

void CProduct::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
   OnLButtonDblClk(nFlags|MK_RBUTTON, point);
}

void CProduct::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   if (!m_dwGroupFlags)
   {
      CWnd *pProductGroupEdt = GetDlgItem(IDC_EDT_PRODUCT_GROUP);
      CWnd *pPriceGroupEdt   = GetDlgItem(IDC_EDT_PRODUCT_PRICE_GROUP);
      CWnd *pCompanyCode     = GetDlgItem(IDC_EDT_PRODUCT_COMPANY_CODE);
      CWnd *pGroup = NULL; 
      CWnd *pEdt   = NULL;
      CWnd *pEdt1  = NULL;

      CRect rcWnd;
      CPoint pt = point;
      ClientToScreen(&pt);
      if (pProductGroupEdt)
      {
         pProductGroupEdt->GetWindowRect(&rcWnd);
         if (rcWnd.PtInRect(pt))
         {
            m_dwGroupFlags |= PRODUCT_GROUP_EDITMODE;
            pGroup = GetDlgItem(IDC_COMBO_PRODUCT_GROUP);
            pEdt   = pProductGroupEdt;
            pEdt1  = GetDlgItem(IDC_EDT_PRODUCT_GROUP1);
         }
      }

      if (pPriceGroupEdt)
      {
         pPriceGroupEdt->GetWindowRect(&rcWnd);
         if (rcWnd.PtInRect(pt))
         {
            m_dwGroupFlags |= PRICE_GROUP_EDITMODE;
            pGroup = GetDlgItem(IDC_COMBO_PRODUCT_PRICE_GROUP);
            pEdt   = pPriceGroupEdt;
            pEdt1  = GetDlgItem(IDC_EDT_PRODUCT_PRICE_GROUP1);
         }
      }

      if (pCompanyCode)
      {
         pCompanyCode->GetWindowRect(&rcWnd);
         if (rcWnd.PtInRect(pt))
         {
            CDataExchange dx(this, false);
            DDX_Text(&dx, IDC_EDT_PRODUCT_COMPANY_CODE, m_nCompanyCode);
            m_dwGroupFlags |= COMPANY_CODE_EDITMODE;
            pCompanyCode->EnableWindow(true);
            pCompanyCode->SetFocus();
            m_dwGroupFlags |= GROUP_ITEM_EDITMODE;
         }
      }

      if (pGroup && pEdt && pEdt1)
      {
         if ((pGroup->SendMessage(CB_GETCOUNT ,0,0) == 0) ||
             (pGroup->SendMessage(CB_GETCURSEL,0,0) == 0) || 
             (nFlags&MK_RBUTTON))                              // Neuer Item
         {
            m_strGroupEdt.Empty();
            m_dwGroupFlags |= NEW_GROUP_ITEM;
         }
         else
         {
            pGroup->GetWindowText(m_strGroupEdt);              // Alten Editieren
         }
         pGroup->ShowWindow(SW_HIDE);
         pEdt->EnableWindow(true);
         pEdt1->SetWindowText(m_strGroupEdt);                  // Windowtext setzen
         pEdt1->ShowWindow(SW_SHOW);
         pEdt1->SetFocus();
         m_dwGroupFlags |= GROUP_ITEM_EDITMODE;
      }
   }
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CProduct::OnSelchangeComboProductGroup() 
{
   UpdateData(true);
   m_nProductGroup = 0;
   int nSel = SendDlgItemMessage(IDC_COMBO_PRODUCT_GROUP, CB_GETCURSEL, 0, 0);
   if (nSel != CB_ERR)
   {
      POSITION pos = m_pProductGroup->FindIndex(nSel-1);
      if (pos)
      {
         ProductOrPriceGroup  *pPOPGroup = (ProductOrPriceGroup*) m_pProductGroup->GetAt(pos);
         if (pPOPGroup) m_nProductGroup = pPOPGroup->nNo;
      }
      UpdateData(false);
      OnChangeProduct();
   }
}

void CProduct::OnSelchangeComboProductPriceGroup() 
{
   int nSel =  SendDlgItemMessage(IDC_COMBO_PRODUCT_PRICE_GROUP, CB_GETCURSEL, 0, 0);
   if (nSel != CB_ERR)
   {
      Product             *pP      = GetProduct();
      if (nSel == m_nPriceGroupSel) return;
      ProductOrPriceGroup *pPOP    = (ProductOrPriceGroup*)SendDlgItemMessage(IDC_COMBO_PRODUCT_PRICE_GROUP, CB_GETITEMDATA, (WPARAM)nSel, 0);
      if ((int)pPOP == -1) pPOP = NULL;
      ProductOrPriceGroup *pPOPold = (ProductOrPriceGroup*)SendDlgItemMessage(IDC_COMBO_PRODUCT_PRICE_GROUP, CB_GETITEMDATA, (WPARAM)m_nPriceGroupSel, 0);
      if ((int)pPOPold == -1) pPOPold = NULL;
      UpdateData(true);

      if (m_bCurrencyChanged && pPOPold)
      {
         GetDlgItem(IDC_COMBO_PRODUCT_CURRENCY)->GetWindowText(pPOPold->strCurrency);
         m_bCurrencyChanged = false;
      }

      if (pPOP)
      {
         m_nPriceGroup = pPOP->nNo;
         if (pPOP->strCurrency.IsEmpty())
         {
            SendDlgItemMessage(IDC_COMBO_PRODUCT_CURRENCY, CB_SETCURSEL, m_nBaseCurrency, 0);
            m_bCurrencyChanged = true;
         }
         else
         {
            int nPos = SendDlgItemMessage(IDC_COMBO_PRODUCT_CURRENCY, CB_FINDSTRING, (WPARAM)-1, (LPARAM)LPCTSTR(pPOP->strCurrency));
            SendDlgItemMessage(IDC_COMBO_PRODUCT_CURRENCY, CB_SETCURSEL, nPos, 0);
         }
      }
      else m_bCurrencyChanged = false;

      if (pP)
      {
         int   nIndex = -1;
         if (m_bPriceChanged && pPOPold)
         {
            pP->SetPrice((BYTE)pPOPold->nNo, m_fProductPrice, (m_bPriceGroupVisible!=0)?true:false);
            m_bPriceChanged    = false;
         }

         if (pPOP)
         {
            m_fProductPrice = pP->GetPrice((BYTE)pPOP->nNo, &nIndex);
         }
         else
         {
            nIndex = -1;
            m_fProductPrice = pP->GetPrice(0, &nIndex);
            SendDlgItemMessage(IDC_COMBO_PRODUCT_CURRENCY, CB_SETCURSEL, m_nBaseCurrency, 0);
            m_nPriceGroup = pP->GetPriceGroup(nIndex);
         }
         m_bPriceGroupVisible = pP->IsPriceGroupVisible(nIndex);
      }

      m_nPriceGroupSel = nSel;
      UpdateData(false);
      OnChangeProduct();
   }
}

void CProduct::CloseGroupEdtCtrl()
{
   CDataExchange dxIn(this, true);
   CDataExchange dxOut(this, false);
   ProductOrPriceGroup  *pPOPGroup = NULL;
   CWnd     *pGroup  = NULL;
   CWnd     *pEdt    = NULL;  // nNo
   CWnd     *pEdt1   = NULL;  // nDescription
   CPtrList *ptrList = NULL;
   int      *pnNo    = NULL;
   int       nSel, nIdEdt = 0;

   if (m_dwGroupFlags & PRODUCT_GROUP_EDITMODE)                // Produktgruppen
   {
      pGroup  = GetDlgItem(IDC_COMBO_PRODUCT_GROUP);
      pEdt    = GetDlgItem(IDC_EDT_PRODUCT_GROUP);
      nIdEdt  = IDC_EDT_PRODUCT_GROUP;
      pEdt1   = GetDlgItem(IDC_EDT_PRODUCT_GROUP1);
      ptrList = m_pProductGroup;
      pnNo    = &m_nProductGroup;
	   DDX_Text(&dxIn, nIdEdt, m_nProductGroup);                // Nummer lesen
      if (m_nProductGroup <  0) m_nProductGroup=  0;           // Bereich prüfen
      if (m_nProductGroup > 99) m_nProductGroup= 99;
      m_strGroupEdt.Format("%02d", m_nProductGroup);           // Nummer wieder ausgeben
   }
   else if (m_dwGroupFlags & PRICE_GROUP_EDITMODE)             // Preisgruppen
   {
      pGroup  = GetDlgItem(IDC_COMBO_PRODUCT_PRICE_GROUP);
      pEdt    = GetDlgItem(IDC_EDT_PRODUCT_PRICE_GROUP);
      nIdEdt  = IDC_EDT_PRODUCT_PRICE_GROUP;
      pEdt1   = GetDlgItem(IDC_EDT_PRODUCT_PRICE_GROUP1);
      ptrList = m_pPriceGroup;
      pnNo    = &m_nPriceGroup;
	   DDX_Text(&dxIn, nIdEdt, m_nPriceGroup);                  // Nummer lesen
      if (m_nPriceGroup < 0   ) m_nPriceGroup= 0;              // Bereich prüfen
      if (m_nPriceGroup > 0x7f) m_nPriceGroup= 0x7f;
      m_strGroupEdt.Format("%02d", m_nPriceGroup);             // Nummer wieder ausgeben
   }
   else if (m_dwGroupFlags & COMPANY_CODE_EDITMODE)            // Ländercode
   {
      GetDlgItem(IDC_EDT_PRODUCT_COMPANY_CODE)->EnableWindow(false);
      DDX_Text(&dxIn, IDC_EDT_PRODUCT_COMPANY_CODE, m_nCompanyCode);
   }

   if (pGroup && pEdt && ptrList)                              // Wurde editiert ?
   {
      pGroup->ShowWindow(SW_SHOW);                             // Listenfeldt anzeigen
	   pEdt1->ShowWindow(SW_HIDE);                              // Editierfeld nicht anzeigen
      pEdt->EnableWindow(false);                               // Nummerneditierfeld disablen
      DDX_Text(&dxOut, nIdEdt, m_strGroupEdt);                 // Achtung : mit m_strGroupEdt !!!
      pEdt1->GetWindowText(m_strGroupEdt);                     // Editierten Text holen

      if (m_dwGroupFlags&GROUP_CHANGED)                        // wurde etwas geändert
      {
         if (m_dwGroupFlags&NEW_GROUP_ITEM)                    // neuer Eintrag
         {                                                        
            if (!m_strGroupEdt.IsEmpty() &&                    // wenn Editierfeld != 0
                (FindPOPGroup(ptrList, *pnNo, nSel) == NULL) &&// Doppeleinträge vermeiden
                (CB_ERR != pGroup->SendMessage(CB_ADDSTRING, 0, (LPARAM) LPCTSTR(m_strGroupEdt))))
            {
               pPOPGroup = new ProductOrPriceGroup;            // neu anlegen
               ptrList->AddTail(pPOPGroup);                    // einfügen
               int nItem = pGroup->SendMessage(CB_GETCOUNT,0, 0)-1;
               nItem = pGroup->SendMessage(CB_SETCURSEL, nItem, 0);
               if (m_dwGroupFlags & PRICE_GROUP_EDITMODE) m_nPriceGroupSel = nItem;
               pGroup->SendMessage(CB_SETITEMDATA, nItem, (LPARAM)pPOPGroup);
            }
         }
         else                                                  // vorhandener Eintrag
         {
            int nSel = pGroup->SendMessage(CB_GETCURSEL, 0, 0);// Position ermitteln
            POSITION pos = ptrList->FindIndex(nSel-1);
            if (pos)
            {
               pPOPGroup = (ProductOrPriceGroup*) ptrList->GetAt(pos);
               if (pPOPGroup)                                  // eintrag vorhanden ?
               {
                  if (m_strGroupEdt.IsEmpty())                 // wenn Editierfeld = 0
                  {                                            // löschen
                     pGroup->SendMessage(CB_DELETESTRING, nSel, 0);
                     int nItem = pGroup->SendMessage(CB_SETCURSEL, 0, 0);// Cursor setzen
                     if (m_dwGroupFlags & PRICE_GROUP_EDITMODE) m_nPriceGroupSel = nItem;
                     ptrList->RemoveAt(pos);                   // Zeiger aus der Liste entfernen
                     delete pPOPGroup;                         // und löschen
                     pPOPGroup = NULL;
                  }
                  else                                         // sonst Update
                  {                                            // neuen einfügen
                     pGroup->SendMessage(CB_INSERTSTRING, nSel, (LPARAM) LPCTSTR(m_strGroupEdt));
                     pGroup->SendMessage(CB_SETITEMDATA , nSel, pGroup->SendMessage(CB_GETITEMDATA, nSel+1, 0));
                     pGroup->SendMessage(CB_DELETESTRING, nSel+1, 0);// alten löschen
                     nSel = pGroup->SendMessage(CB_SETCURSEL   , nSel, 0);// Cursor setzen
                     if (m_dwGroupFlags & PRICE_GROUP_EDITMODE) m_nPriceGroupSel = nSel;
                  }
               }
            }
         }
      }
      if (pPOPGroup)                                           // Einträge vorhanden oder neu
      {
         pPOPGroup->nNo = *pnNo;                               // Updaten
         pPOPGroup->strDescription = m_strGroupEdt;
      }
      else if (m_dwGroupFlags&GROUP_CHANGED)                   // wurde etwas geändert
      {
         UpdateData(false);
      }

      OnChangeProduct();
   }
   m_dwGroupFlags = 0;
}

BOOL CProduct::OnCommand(WPARAM wParam, LPARAM lParam) 
{
   if (m_dwGroupFlags&GROUP_ITEM_EDITMODE)
   {
      switch(HIWORD(wParam))
      {
         case EN_SETFOCUS: case CBN_SETFOCUS: 
         case BN_SETFOCUS: case BN_HILITE: case BN_CLICKED:
            switch(LOWORD(wParam))
            {
               case IDC_EDT_PRODUCT_GROUP:       case IDC_EDT_PRODUCT_GROUP1:
               case IDC_EDT_PRODUCT_PRICE_GROUP: case IDC_EDT_PRODUCT_PRICE_GROUP1:
                  break;
               default:
                  CloseGroupEdtCtrl();
                  break;
            }
            break;
         default :break;
      }
   }
	return CDialog::OnCommand(wParam, lParam);
}

ProductOrPriceGroup* CProduct::FindPOPGroup(CPtrList *pList, int nNo, int &nSel)
{
   ProductOrPriceGroup *pPOP = NULL;
   POSITION pos;
   nSel = -1;
   pos = pList->GetHeadPosition();
   while (pos)
   {
      nSel++;
      pPOP = (ProductOrPriceGroup*) pList->GetNext(pos);
      if (pPOP && pPOP->nNo == nNo)
      {
         if (nNo == 0) nSel = -1;
         return pPOP;
      }
   }
   return NULL;
}

void CProduct::SetPriceGroup()
{
   ProductOrPriceGroup *pPOP;
   int nSel;

   if (m_nPriceGroup == 0)
   {
      nSel = -1;
   }
   else
   {
      pPOP = FindPOPGroup(m_pPriceGroup, m_nPriceGroup, nSel);
      if (pPOP)
      {
         if (pPOP->strCurrency.IsEmpty())
         {
            SendDlgItemMessage(IDC_COMBO_PRODUCT_CURRENCY, CB_SETCURSEL, m_nBaseCurrency, 0);
            m_bCurrencyChanged = true;
         }
         else
         {
            int nPos = SendDlgItemMessage(IDC_COMBO_PRODUCT_CURRENCY, CB_FINDSTRING, (WPARAM)-1, (LPARAM)LPCTSTR(pPOP->strCurrency));
            SendDlgItemMessage(IDC_COMBO_PRODUCT_CURRENCY, CB_SETCURSEL, nPos, 0);
         }
      }
      else
      {
         nSel = -1;
         SendDlgItemMessage(IDC_COMBO_PRODUCT_CURRENCY, CB_SETCURSEL, m_nBaseCurrency, 0);
      }
   }

   m_nPriceGroupSel = SendDlgItemMessage(IDC_COMBO_PRODUCT_PRICE_GROUP, CB_SETCURSEL, nSel+1, 0);
}

void CProduct::OnSelchangeComboProductCurrency() 
{
   m_bCurrencyChanged = true;;
   OnChangeProduct();
}

LRESULT CProduct::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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
