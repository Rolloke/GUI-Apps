// FacturaDoc.cpp : Implementierung der Klasse CFacturaDoc
//

#include "stdafx.h"
#include "Factura.h"
#include "FacturaDoc.h"
#include "TreeDlg.h"
#include "TextLabel.h"
#include "FindCustomer.h"
#include "ListDlg.h"
#include "ETS3DGLRegKeys.h"

#define  ETSDEBUG_INCLUDE 
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FACTURA_FILE "FAC"
#define TAX_FACTOR   "Tax"
#define NN_AMOUNT    "NNamount"

/////////////////////////////////////////////////////////////////////////////
// CFacturaDoc

#define UNIT_SIGN  0
#define STATE_SIGN 1
#define UNIT       2
#define BASE_UNIT  3
#define REF_UNIT   4
#define STATE      5

IMPLEMENT_DYNCREATE(CFacturaDoc, CCARADoc)

BEGIN_MESSAGE_MAP(CFacturaDoc, CCARADoc)
	//{{AFX_MSG_MAP(CFacturaDoc)
	ON_COMMAND(ID_EDIT_INSERT_HIAK_DATA, OnEditInsertHiakData)
	ON_COMMAND(ID_EXTRA_EXPORT_CUSTOMER, OnExtraExportCustomer)
	ON_UPDATE_COMMAND_UI(ID_EXTRA_EXPORT_CUSTOMER, OnUpdateExtraExportCustomer)
	ON_COMMAND(ID_EXTRA_IMPORT_CUSTOMER, OnExtraImportCustomer)
	ON_COMMAND(IDX_SHOW_LABEL_STRUCTURE, OnShowLabelStrukture)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
END_MESSAGE_MAP()

const char szEmptyString[] = "";

Customer::Customer()
{
   nNo          = 0;
   nFlags       = 0;
   InitAllText();
}

Customer::~Customer()
{
   FreeAllText();
}

void Customer::InitAllText()
{
   szName       = (char*)szEmptyString;
   szFirstName  = (char*)szEmptyString;
   szTitle      = (char*)szEmptyString;
   szPhoneNo    = (char*)szEmptyString;
   szFaxNo      = (char*)szEmptyString;
   szCity       = (char*)szEmptyString;
   szState      = (char*)szEmptyString;
   szStreet     = (char*)szEmptyString;
   szCompany    = (char*)szEmptyString;
   szPostalCode = (char*)szEmptyString;
   szStreetNo   = (char*)szEmptyString;
   szEmail      = (char*)szEmptyString;
   szComment    = (char*)szEmptyString;
}

void Customer::FreeAllText()
{
   if (szName       != szEmptyString) free((void*)szName);
   if (szFirstName  != szEmptyString) free((void*)szFirstName);
   if (szTitle      != szEmptyString) free((void*)szTitle);
   if (szPhoneNo    != szEmptyString) free((void*)szPhoneNo);
   if (szFaxNo      != szEmptyString) free((void*)szFaxNo);
   if (szCity       != szEmptyString) free((void*)szCity);
   if (szState      != szEmptyString) free((void*)szState);
   if (szStreet     != szEmptyString) free((void*)szStreet);
   if (szCompany    != szEmptyString) free((void*)szCompany);
   if (szPostalCode != szEmptyString) free((void*)szPostalCode);
   if (szStreetNo   != szEmptyString) free((void*)szStreetNo);
   if (szEmail      != szEmptyString) free((void*)szEmail);
   if (szComment    != szEmptyString) free((void*)szComment);
   InitAllText();
}

void Customer::Serialize(CArchive &ar)
{
   if (ar.IsStoring())
   {
      WriteText(ar, szName);
      WriteText(ar, szFirstName);
      WriteText(ar, szTitle);
      WriteText(ar, szPhoneNo);
      WriteText(ar, szFaxNo);
      WriteText(ar, szCity);
      WriteText(ar, szState);
      WriteText(ar, szStreet);
      WriteText(ar, szCompany);
      WriteText(ar, szPostalCode);
      WriteText(ar, szStreetNo);
      WriteText(ar, szEmail);
      WriteText(ar, szComment);

      CLabel::ArchiveWrite(ar, &nNo   , sizeof(long));
      CLabel::ArchiveWrite(ar, &nFlags, sizeof(long));
   }
   else
   {
      FreeAllText();
      szName       = ReadText(ar);
      szFirstName  = ReadText(ar);
      szTitle      = ReadText(ar);
      szPhoneNo    = ReadText(ar);
      szFaxNo      = ReadText(ar);
      szCity       = ReadText(ar);
      szState      = ReadText(ar);
      szStreet     = ReadText(ar);
      szCompany    = ReadText(ar);
      szPostalCode = ReadText(ar);
      szStreetNo   = ReadText(ar);
      szEmail      = ReadText(ar);
      szComment    = ReadText(ar);
/*
      if (strcmp(szState, "BRD") == 0)
      {
         free((void*)szState);
         szState = (char*)szEmptyString;
      }
*/
      CLabel::ArchiveRead(ar, &nNo   , sizeof(long));
      CLabel::ArchiveRead(ar, &nFlags, sizeof(long));
   }
}

char * Customer::ReadText(CArchive &ar)
{
   char * pszText = (char*)szEmptyString;
   int nLength;
   CLabel::ArchiveRead(ar, &nLength, sizeof(int));
   if (nLength)
   {
      pszText = (char*)malloc(nLength+1);
      if (pszText == NULL)
         AfxThrowMemoryException();
      CLabel::ArchiveRead(ar, pszText, nLength);
      pszText[nLength] = 0;
   }
   return pszText;
}

void Customer::WriteText(CArchive &ar, char *pszText)
{
   int nLength = strlen(pszText);
   CLabel::ArchiveWrite(ar, &nLength, sizeof(int));
   if (nLength) CLabel::ArchiveWrite(ar, pszText, nLength);
}

void Customer::Read1_0(CArchive &ar)
{
   struct                                    // alte Struktur für Kunden
   {
      char  szName[NAME_LENGTH];             // VorName des Kunden
      char  szFirstName[FIRSTNAME_LENGTH];   // Familienname des Kunden
      char  szTitle[TITLE_LENGTH];           // Titel, Akademische Grade
      char  szPhoneNo[PHONE_LENGTH];         // Telefonnummer
      char  szFaxNo[FAX_LENGTH];             // Telefonnummer
      char  szCity[CITY_LENGTH];             // Ort
      char  szState[STATE_LENGTH];           // Land
      char  szStreet[STREET_LENGTH];         // Straße
      char  szCompany[COMPANY_LENGTH];       // Firma
      char  szPostCode[PC_LENGTH];           // Postleitzahl
      char  szStreetNo[STREETNO_LENGTH];     // Hausnummer
      char  szEmail[EMAIL_LENGTH];           // E-Mail Adresse
      char  szComment[COMMENT_LENGTH];       // Kommentar
      long  nNo;                             // Kundennummer
      long  nFlags;                          // Flags für ...
   }CustomerOld;

   CLabel::ArchiveRead(ar, &CustomerOld, sizeof(CustomerOld), true);

   FreeAllText();
   if (strlen(CustomerOld.szCity))     szCity      = _tcsdup(CustomerOld.szCity);
   if (strlen(CustomerOld.szEmail))    szEmail     = _tcsdup(CustomerOld.szEmail);
   if (strlen(CustomerOld.szFaxNo))    szFaxNo     = _tcsdup(CustomerOld.szFaxNo);
   if (strlen(CustomerOld.szStreetNo)) szStreetNo  = _tcsdup(CustomerOld.szStreetNo);
   if (strlen(CustomerOld.szName))     szName      = _tcsdup(CustomerOld.szName);
   if (strlen(CustomerOld.szPhoneNo))  szPhoneNo   = _tcsdup(CustomerOld.szPhoneNo);
   if (strlen(CustomerOld.szPostCode)) szPostalCode= _tcsdup(CustomerOld.szPostCode);
   if (strlen(CustomerOld.szFirstName))szFirstName = _tcsdup(CustomerOld.szFirstName);
   if (strlen(CustomerOld.szState))    szState     = _tcsdup(CustomerOld.szState);
   if (strlen(CustomerOld.szStreet))   szStreet    = _tcsdup(CustomerOld.szStreet);
   if (strlen(CustomerOld.szTitle))    szTitle     = _tcsdup(CustomerOld.szTitle);
   if (strlen(CustomerOld.szComment))  szComment   = _tcsdup(CustomerOld.szComment);
   if (strlen(CustomerOld.szCompany))  szCompany   = _tcsdup(CustomerOld.szCompany);
   nNo    = CustomerOld.nNo;
   nFlags = CustomerOld.nFlags;
}

Invoice::Invoice()
{
   ZeroMemory(this, sizeof(*this));
   szComment = (char*)szEmptyString;
}

Invoice::~Invoice()
{
   FreeAllText();
   if (pPosition) delete[] pPosition;
}

void Invoice::FreeAllText()
{
   if (szComment != szEmptyString) free((void*)szComment);
   szComment = (char*)szEmptyString;
}

void Invoice::Serialize(CArchive&ar, int nVersion)
{
   if (ar.IsLoading())
   {
      FreeAllText();
      CLabel::ArchiveRead(ar, &nInvNo, sizeof(long));
      CLabel::ArchiveRead(ar, &nCustNo, sizeof(long));
      CLabel::ArchiveRead(ar, &nYear, sizeof(long));
      CLabel::ArchiveRead(ar, &nMonth, sizeof(long));
      CLabel::ArchiveRead(ar, &nDay, sizeof(long));
      CLabel::ArchiveRead(ar, &nFlags, sizeof(long));
      if (nVersion >= 125) CLabel::ArchiveRead(ar, &fRabatt, sizeof(float));
      CLabel::ArchiveRead(ar, &nPositions, sizeof(long));
      if (nPositions > 0)
      {
         pPosition = new Position[nPositions];
         if (!pPosition) AfxThrowMemoryException();
         CLabel::ArchiveRead(ar, pPosition, sizeof(Position) * nPositions, true);
      }
      szComment = Customer::ReadText(ar);
      CLabel::ArchiveRead(ar, szUnitSign, 8);
   }
   else
   {
      CLabel::ArchiveWrite(ar, &nInvNo, sizeof(long));
      CLabel::ArchiveWrite(ar, &nCustNo, sizeof(long));
      CLabel::ArchiveWrite(ar, &nYear, sizeof(long));
      CLabel::ArchiveWrite(ar, &nMonth, sizeof(long));
      CLabel::ArchiveWrite(ar, &nDay, sizeof(long));
      CLabel::ArchiveWrite(ar, &nFlags, sizeof(long));
      CLabel::ArchiveWrite(ar, &fRabatt, sizeof(float));
      CLabel::ArchiveWrite(ar, &nPositions, sizeof(long));
      if (nPositions > 0)
         CLabel::ArchiveWrite(ar, pPosition, sizeof(Position) * nPositions);
      Customer::WriteText(ar, szComment);
      CLabel::ArchiveWrite(ar, szUnitSign, 8);
   }
}

void Invoice::Read1_1(CArchive &ar)
{
   int nSize;

   ASSERT(ar.IsLoading());

   FreeAllText();
   CLabel::ArchiveRead(ar, &nSize, sizeof(long));
   nSize -= 10 * sizeof(long);
   szComment = (char*)malloc(nSize+1);
   if (szComment == NULL)
   {
      szComment = (char*)szEmptyString;
      AfxThrowMemoryException();
   }

   CLabel::ArchiveRead(ar, &nInvNo, sizeof(long));
   CLabel::ArchiveRead(ar, &nCustNo, sizeof(long));
   CLabel::ArchiveRead(ar, &nYear, sizeof(long));
   CLabel::ArchiveRead(ar, &nMonth, sizeof(long));
   CLabel::ArchiveRead(ar, &nDay, sizeof(long));
   CLabel::ArchiveRead(ar, &nFlags, sizeof(long));
   CLabel::ArchiveRead(ar, &nPositions, sizeof(long));
   CLabel::ArchiveRead(ar, &pPosition, sizeof(void*));
   CLabel::ArchiveRead(ar, &pPosition, sizeof(void*));
   if (nPositions > 0)
   {
      pPosition = new Position[nPositions];
      if (!pPosition) AfxThrowMemoryException();
      CLabel::ArchiveRead(ar, pPosition, sizeof(Position) * nPositions, true);
   }
   else pPosition = NULL;
   CLabel::ArchiveRead(ar, szComment, nSize);
   szComment[nSize] = 0;
}

void Invoice::Update125(CFacturaDoc*pDoc)
{
   int i;
   Product *pPn, *pPa;

   for (i=0; i<nPositions; i++)
   {
      pPn = pDoc->GetProduct(pPosition[i].nProductNo);
      if (pPn)
      {
         pPosition[i].cPriceGroup = pPn->sEAN_Code.ucPriceGroup;
         if (pPn->IsDummy())
         {
            pPa = pDoc->GetProduct(pPn->nCount);
            if (pPa) pPosition[i].nProductNo  = pPa->nNo;
         }
      }
   }
   if (szUnitSign[0] == 0)
   {
      CurrencyUnit *pCU = pDoc->GetUnit("?", BASE_UNIT);
      if (pCU)
      {
         strcpy(szUnitSign, pCU->szUnitSign);
      }
   }
}

Product::Product()
{
   ZeroMemory(this, sizeof(*this));
   szDescription = (char*)szEmptyString;
}

Product::~Product()
{
   FreeAllText();
   if (pfPrice)       free(pfPrice);
   if (pucPriceGroup) free(pucPriceGroup);
}

void Product::FreeAllText()
{
   if (szDescription != szEmptyString) free((void*)szDescription);
   szDescription = (char*)szEmptyString;
}

void Product::ReallocPrice()
{
   pfPrice       = (float*)realloc(pfPrice      , sizeof(float)*nPrices);
   pucPriceGroup = (BYTE*) realloc(pucPriceGroup, sizeof(BYTE )*nPrices);
}

#define NO_PRICE_GROUP      0x80       // altes Format

void Product::Read1_1(CArchive &ar, CPtrList *pProductList, int nVersion)
{
   struct // alte Struktur Artikel
   {
      long     nSize;
      long     nNo;
      long     nCount;
      float    fPrice;
      EAN_Code sEAN_Code;
      char    *pszDescription;
   }Product1_1;

   int   nSizeWithoutText = sizeof(long)*3+sizeof(float);
   ASSERT(ar.IsLoading());
   FreeAllText();
   CLabel::ArchiveRead(ar, &Product1_1.nSize , sizeof(long) , true);
   CLabel::ArchiveRead(ar, &Product1_1.nNo   , sizeof(long) , true);
   CLabel::ArchiveRead(ar, &Product1_1.nCount, sizeof(long) , true);
   CLabel::ArchiveRead(ar, &Product1_1.fPrice, sizeof(float), true);
   if (nVersion > 121)                                         // mit EAN-Code läßt sich der Artikel einordnen
   {
      nSizeWithoutText += sizeof(EAN_Code);
      CLabel::ArchiveRead(ar, &Product1_1.sEAN_Code, sizeof(EAN_Code), true);
      Product1_1.nSize -= nSizeWithoutText;
      Product1_1.pszDescription = (char*)malloc(Product1_1.nSize+1);
      CLabel::ArchiveRead(ar, Product1_1.pszDescription, Product1_1.nSize, true);
      if (Product1_1.sEAN_Code.ucPriceGroup & NO_PRICE_GROUP)
      {
         Product1_1.sEAN_Code.ucPriceGroup = 0;
      }
      POSITION pos = pProductList->GetHeadPosition();          // Artikel suchen
      bool bFound = false;
      Product *pP;
      while (pos)
      {
         pP = (Product*)pProductList->GetNext(pos);
         if ((pP != NULL) &&
             (pP->sEAN_Code.ulCompanyEAN   == Product1_1.sEAN_Code.ulCompanyEAN  ) &&
             (pP->sEAN_Code.ucProductGroup == Product1_1.sEAN_Code.ucProductGroup) &&
             (pP->sEAN_Code.ucReleaseH     == Product1_1.sEAN_Code.ucReleaseH    ) &&
             (pP->sEAN_Code.ucReleaseL     == Product1_1.sEAN_Code.ucReleaseL    ))
         {
            bFound = true;                                     // Artikel gefunden : Preisgruppe eintragen
            pP->SetPrice(Product1_1.sEAN_Code.ucPriceGroup, Product1_1.fPrice, false);
            Product1_1.nCount = pP->nNo;                       // in nCount steht die neue Artikelnummer
            break;
         }
      }
      nNo           = Product1_1.nNo;
      nCount        = Product1_1.nCount;
      sEAN_Code     = Product1_1.sEAN_Code;
      szDescription = Product1_1.pszDescription;
      if (!bFound)                                             // neuer Artikel
      {
         SetPrice(Product1_1.sEAN_Code.ucPriceGroup, Product1_1.fPrice, false);
      }
   }
   else                                                        // sonst wird er nur gespeichert
   {
      nNo     = Product1_1.nNo;
      nCount  = Product1_1.nCount;
      sEAN_Code.ucProductGroup = (unsigned char) (Product1_1.nNo&0x000000FF);
      sEAN_Code.ucReleaseH     = (unsigned char)((Product1_1.nNo&0x0000FF00)>> 8);
      sEAN_Code.ucReleaseL     = (unsigned char)((Product1_1.nNo&0x00FF0000)>>16);
      SetPrice(Product1_1.sEAN_Code.ucPriceGroup, Product1_1.fPrice, false);
      Product1_1.nSize -= nSizeWithoutText;
      szDescription = (char*)malloc(Product1_1.nSize+1);
      CLabel::ArchiveRead(ar, szDescription, Product1_1.nSize, true);
   }
}
#undef NO_PRICE_GROUP

void Product::Serialize(CArchive&ar)
{
   if (ar.IsLoading())
   {
      FreeAllText();
      CLabel::ArchiveRead(ar, &nNo   , sizeof(long)               , true);
      CLabel::ArchiveRead(ar, &nCount, sizeof(long)               , true);
      CLabel::ArchiveRead(ar, &nPrices, sizeof(long)              , true);
      ReallocPrice();
      CLabel::ArchiveRead(ar, pfPrice, sizeof(float)*nPrices     , true);
      CLabel::ArchiveRead(ar, pucPriceGroup, sizeof(BYTE)*nPrices, true);
      CLabel::ArchiveRead(ar, &sEAN_Code, sizeof(EAN_Code)        , true);
      szDescription = Customer::ReadText(ar);
   }
   else
   {
      CLabel::ArchiveWrite(ar, &nNo   , sizeof(long));
      CLabel::ArchiveWrite(ar, &nCount, sizeof(long));
      CLabel::ArchiveWrite(ar, &nPrices, sizeof(long));
      CLabel::ArchiveWrite(ar, pfPrice, sizeof(float)*nPrices);
      CLabel::ArchiveWrite(ar, pucPriceGroup, sizeof(BYTE)*nPrices);
      CLabel::ArchiveWrite(ar, &sEAN_Code, sizeof(EAN_Code));
      Customer::WriteText(ar ,szDescription);
   }
}

bool Product::SetPrice(unsigned char ucPriceGroup, float fPrice, bool bVisible)
{
   int nIndex = -1;
   GetPrice(ucPriceGroup, &nIndex);
   if (bVisible) ucPriceGroup |= PRICE_GROUP_VISIBLE;
   if (nIndex == -1)
   {
      nPrices++;
      ReallocPrice();
      pucPriceGroup[nPrices-1] = ucPriceGroup;
      pfPrice[nPrices-1]       = fPrice;
      return true;
   }
   else
   {
      pucPriceGroup[nIndex] = ucPriceGroup;
      pfPrice[nIndex]       = fPrice;
      return false;
   }
}

void Product::RemovePrice(unsigned char ucPriceGroup)
{
   int nIndex = -1;
   GetPrice(ucPriceGroup, &nIndex);
   if ((nIndex > 0) && (nIndex < nPrices))
   {
      for (int i=nIndex; i<nPrices-1; i++)
      {
         pucPriceGroup[i] = pucPriceGroup[i+1];
         pfPrice[i]       = pfPrice[i+1];
      }
      nPrices--;
   }
}

float Product::GetPrice(unsigned char ucPriceGroup, int *pnIndex) const
{
   if ((pnIndex != NULL) && (*pnIndex>=0) && (*pnIndex<nPrices))
      return pfPrice[*pnIndex];
   else
   {
      int i;
      for (i=0; i<nPrices; i++)
      {
         if ((pucPriceGroup[i]&~PRICE_GROUP_VISIBLE) == ucPriceGroup)
         {
            if (pnIndex) *pnIndex = i;
            return pfPrice[i];
         }
      }
   }

   return 0;
}

BYTE Product::GetPriceGroup(int nIndex) const
{
   return BYTE(((nIndex >=0)&&(nIndex<nPrices)) ? (pucPriceGroup[nIndex]&~PRICE_GROUP_VISIBLE) : 0);
};

bool Product::IsPriceGroupVisible(int nIndex) const
{
   if ((nIndex >=0)&&(nIndex<nPrices))
   {
      return ((pucPriceGroup[nIndex]&PRICE_GROUP_VISIBLE)!=0) ? true : false;
   }
   else return false;
}

int EAN_Code::GetEAN()
{
   CString strEAN;
   int nCheck = 0, nLen, i, nN;
   LPTSTR pszEAN;
   strEAN.Format("%07d%02d%1d%02d", ulCompanyEAN, ucProductGroup, ucReleaseH, ucReleaseL);
   nLen = strEAN.GetLength();
   pszEAN = strEAN.GetBuffer(nLen);
   for (i=0; i<nLen; i++)
   {
      nN = pszEAN[i] - '0';
      nCheck += (nN * ((i&1) ? 3 : 1));
   }
   nN = (nCheck / 10 + 1) * 10;
   nCheck = nN - nCheck;
   strEAN.ReleaseBuffer();
   return nCheck;
}

void ProductOrPriceGroup::Read1_1(CArchive &ar)
{
   ASSERT(ar.IsLoading());
   strDescription.Empty();
   CLabel::ArchiveRead(ar, &nNo, sizeof(long));
   CFacturaDoc::ReadVariableString(ar, strDescription);
}

void ProductOrPriceGroup::Serialize(CArchive&ar)
{
   if (ar.IsLoading())
   {
      strDescription.Empty();
      CLabel::ArchiveRead(ar, &nNo, sizeof(long));
      CFacturaDoc::ReadVariableString(ar, strCurrency);
      CFacturaDoc::ReadVariableString(ar, strDescription);
   }
   else
   {
      CLabel::ArchiveWrite(ar, &nNo, sizeof(long));
      CFacturaDoc::WriteVariableString(ar, strCurrency);
      CFacturaDoc::WriteVariableString(ar, strDescription);
   }
}
void ProductOrPriceGroup::Update125(CFacturaDoc*pDoc)
{
   if (strCurrency.IsEmpty())
   {
      CurrencyUnit *pCU = pDoc->GetUnit("?", BASE_UNIT);
      if (pCU)
      {
         strCurrency = _T(pCU->szUnitSign);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CFacturaDoc Konstruktion/Destruktion

CFacturaDoc::CFacturaDoc()
{
   CWinApp *pApp = AfxGetApp();
   m_pActualInvoice  = NULL;
   m_pActualCustomer = NULL;
   m_fTax            = (float)(pApp->GetProfileInt(REGKEY_DEFAULT, TAX_FACTOR, 16) * 0.01);
   m_fNNAmount       = (float)(pApp->GetProfileInt(REGKEY_DEFAULT, NN_AMOUNT, 153) * 0.01);
   m_nActualPosition = -1;
   m_nActualCustomer = -1;
   m_nActualInvoice  = -1;
   m_nActualProduct  = -1;
   int i;
   for (i=0; i<CUSTOMER_FLAG_CNT; i++)
   {
      if (m_strCustomerFlags[i].IsEmpty())
         m_strCustomerFlags[i].Format("%d", i+1);
   }
   for (i=0; i<FLAG_USER_CNT; i++)
   {
      if (m_strInvoiceFlags[i].IsEmpty())
         m_strInvoiceFlags[i].Format("%d", i+1);
   }
}

CFacturaDoc::~CFacturaDoc()
{
   BEGIN("~CFacturaDoc()");
   CWinApp *pApp = AfxGetApp();
   pApp->WriteProfileInt(REGKEY_DEFAULT, TAX_FACTOR, (int)(100.0*m_fTax+1e-5));
   pApp->WriteProfileInt(REGKEY_DEFAULT, NN_AMOUNT, (int)(100.0f*m_fNNAmount+1e-4));
   
   DeleteContents();
}

BOOL CFacturaDoc::OnNewDocument()
{
   BOOL bOk = CCARADoc::OnNewDocument();
	return bOk;
}
/////////////////////////////////////////////////////////////////////////////
// CFacturaDoc Serialisierung

void CFacturaDoc::Serialize(CArchive& ar)
{
   Customer      *pCustomer;
   Invoice       *pInvoice;
   Product       *pProduct;
   State         *pState;
   CurrencyUnit  *pCurrencyUnit;
   PayMode       *pPayMode;
   ProductOrPriceGroup  *pPOPGroup;

   long           nCount;

   m_FileHeader.SetType(LABEL_FILE);
   CCARADoc::Serialize(ar);

   if (ar.IsStoring())
   {
      m_FileHeader.SetType(FACTURA_FILE);
      m_FileHeader.CalcChecksum();
      WriteFileHeader(ar);
      POSITION  pos;
      nCount = m_Customers.GetCount();                         // Customers
      CLabel::ArchiveWrite(ar, &nCount, sizeof(long));
      pos = m_Customers.GetHeadPosition();
      while (pos)
      {
         pCustomer = (Customer*) m_Customers.GetNext(pos);
         if (pCustomer) pCustomer->Serialize(ar);
      }
      nCount = m_Products.GetCount();                          // Products
      CLabel::ArchiveWrite(ar, &nCount, sizeof(long));
      pos = m_Products.GetHeadPosition();
      while (pos)
      {
         pProduct = (Product*) m_Customers.GetNext(pos);
         if (pProduct) pProduct->Serialize(ar);
      }
      nCount = m_Invoices.GetCount();                          // Invoices
      CLabel::ArchiveWrite(ar, &nCount, sizeof(long));
      pos = m_Invoices.GetHeadPosition();
      while (pos)
      {
         pInvoice = (Invoice*) m_Invoices.GetNext(pos);
         if (pInvoice) pInvoice->Serialize(ar);
      }
      nCount = m_States.GetCount();                            // States
      CLabel::ArchiveWrite(ar, &nCount, sizeof(long));
      pos = m_States.GetHeadPosition();
      while (pos)
      {
         State* pState = (State*) m_States.GetNext(pos);
         if (pState) CLabel::ArchiveWrite(ar, pState, pState->nSize);
      }
      nCount = m_CurrencyUnits.GetCount();                     // CurrencyUnits
      CLabel::ArchiveWrite(ar, &nCount, sizeof(long));
      pos = m_CurrencyUnits.GetHeadPosition();
      while (pos)
      {
         pCurrencyUnit = (CurrencyUnit*) m_CurrencyUnits.GetNext(pos);
         if (pCurrencyUnit) CLabel::ArchiveWrite(ar, pCurrencyUnit, sizeof(CurrencyUnit));
      }
      nCount = m_PayModes.GetCount();                          // PayModes
      CLabel::ArchiveWrite(ar, &nCount, sizeof(long));
      pos = m_PayModes.GetHeadPosition();
      while (pos)
      {
         pPayMode = (PayMode*) m_PayModes.GetNext(pos);
         if (pPayMode) CLabel::ArchiveWrite(ar, pPayMode, pPayMode->nSize);
      }
      nCount = m_ProductGroup.GetCount();                      // m_ProductGroup
      CLabel::ArchiveWrite(ar, &nCount, sizeof(long));
      pos = m_ProductGroup.GetHeadPosition();
      while (pos)
      {
         pPOPGroup = (ProductOrPriceGroup*) m_ProductGroup.GetNext(pos);
         if (pPOPGroup) pPOPGroup->Serialize(ar);
      }
      nCount = m_PriceGroup.GetCount();                        // m_PriceGroup
      CLabel::ArchiveWrite(ar, &nCount, sizeof(long));
      pos = m_PriceGroup.GetHeadPosition();
      while (pos)
      {
         pPOPGroup = (ProductOrPriceGroup*) m_PriceGroup.GetNext(pos);
         if (pPOPGroup) pPOPGroup->Serialize(ar);
      }
      
      for (nCount=0; nCount<CUSTOMER_FLAG_CNT; nCount++)          // Customer User Flags
      {
         WriteVariableString(ar, m_strCustomerFlags[nCount]);
      }
      
      for (nCount=0; nCount<FLAG_USER_CNT; nCount++)             // Invoice User Flags
      {
         WriteVariableString(ar, m_strInvoiceFlags[nCount]);
      }

      WriteVariableString(ar, m_dlgPrintBankKl.m_strBankName);
      WriteVariableString(ar, m_dlgPrintBankKl.m_strBlz);
      WriteVariableString(ar, m_dlgPrintBankKl.m_strKtoNo);
      WriteVariableString(ar, m_dlgPrintBankKl.m_strCurrency);
      WriteVariableString(ar, m_dlgPrintBankKl.m_strAmount);

      WriteFileHeader(ar, false);
   }
   else
   {
      unsigned long lChecksum = 0;
      if (m_FileHeader.IsFileHeader() && m_FileHeader.IsType(FACTURA_FILE))
      {
         lChecksum = m_FileHeader.GetChecksum();
         m_FileHeader.CalcChecksum();
      }
      long i, nSize;
      CLabel::ArchiveRead(ar, &nCount, sizeof(long), true);// Customers
      if (m_FileHeader.GetVersion() >= 111)
      {
         for (i=0; i<nCount; i++)
         {
            pCustomer = new Customer;
            m_Customers.AddTail(pCustomer);
            pCustomer->Serialize(ar);
         }
      }
      else
      {
         for (i=0; i<nCount; i++)
         {
            pCustomer = new Customer;
            m_Customers.AddTail(pCustomer);
            pCustomer->Read1_0(ar);
         }
      }
      CLabel::ArchiveRead(ar, &nCount, sizeof(long), true);// Products
      for (i=0; i<nCount; i++)
      {
         pProduct = new Product;
         if (m_FileHeader.GetVersion() >=125) pProduct->Serialize(ar);
         else                                 pProduct->Read1_1(ar, &m_Products, m_FileHeader.GetVersion());
         m_Products.AddTail(pProduct);
      }
      CLabel::ArchiveRead(ar, &nCount, sizeof(long), true);// Invoices
      if (m_FileHeader.GetVersion() >= 121)
      {
         for (i=0; i<nCount; i++)
         {
            pInvoice = new Invoice;
            m_Invoices.AddTail(pInvoice);
            pInvoice->Serialize(ar, m_FileHeader.GetVersion());
         }
      }
      else
      {
         for (i=0; i<nCount; i++)
         {
            pInvoice = new Invoice;
            m_Invoices.AddTail(pInvoice);
            pInvoice->Read1_1(ar);
         }
      }
      CLabel::ArchiveRead(ar, &nCount, sizeof(long), true);// States
      for (i=0; i<nCount; i++)
      {
         CLabel::ArchiveRead(ar, &nSize, sizeof(long), true);
         pState = (State*) new BYTE[nSize];
         pState->nSize = nSize;
         CLabel::ArchiveRead(ar, &pState->szStateSign[0], nSize-sizeof(long), true);
         m_States.AddTail(pState);
      }
      CLabel::ArchiveRead(ar, &nCount, sizeof(long), true);// CurrencyUnits
      for (i=0; i<nCount; i++)
      {
         pCurrencyUnit = new CurrencyUnit;
         CLabel::ArchiveRead(ar, pCurrencyUnit, sizeof(CurrencyUnit), true);
         m_CurrencyUnits.AddTail(pCurrencyUnit);
      }
      if (m_FileHeader.GetVersion() >= 121)
      {
         CLabel::ArchiveRead(ar, &nCount, sizeof(long), true);// PayModes
         for (i=0; i<nCount; i++)
         {
            CLabel::ArchiveRead(ar, &nSize, sizeof(long), true);
            pPayMode = (PayMode*) new BYTE[nSize];
            pPayMode->nSize = nSize;
            CLabel::ArchiveRead(ar, &pPayMode->nMode, nSize-sizeof(long), true);
            m_PayModes.AddTail(pPayMode);
         }
      }

      if (m_FileHeader.GetVersion() >= 122)
      {
         CLabel::ArchiveRead(ar, &nCount, sizeof(long), true);// ProductGroup
         for (i=0; i<nCount; i++)
         {
            pPOPGroup = new ProductOrPriceGroup;
            if (m_FileHeader.GetVersion() >= 125) pPOPGroup->Serialize(ar);
            else
            {
               CLabel::ArchiveRead(ar, &pPOPGroup->nNo, sizeof(long), true);
               ReadVariableString(ar, pPOPGroup->strDescription);
            }
            m_ProductGroup.AddTail(pPOPGroup);
         }
         CLabel::ArchiveRead(ar, &nCount, sizeof(long), true);// PriceGroup
         for (i=0; i<nCount; i++)
         {
            pPOPGroup = new ProductOrPriceGroup;
            if (m_FileHeader.GetVersion() >= 125)
            {
               pPOPGroup->Serialize(ar);
            }
            else
            {
               CLabel::ArchiveRead(ar, &pPOPGroup->nNo, sizeof(long), true);
               ReadVariableString(ar, pPOPGroup->strDescription);
               pPOPGroup->Update125(this);                     // Preisgruppen aktualisieren
            }
            m_PriceGroup.AddTail(pPOPGroup);
         }
      }
      if (m_FileHeader.GetVersion() >= 123)
      {
         for (i=0; i<CUSTOMER_FLAG_CNT; i++)                   // Customer User Flags
            ReadVariableString(ar, m_strCustomerFlags[i]);
      
         for (i=0; i<FLAG_USER_CNT; i++)                       // Invoice User Flags
            ReadVariableString(ar, m_strInvoiceFlags[i]);
      }

      if (m_FileHeader.GetVersion() >= 124)
      {
         ReadVariableString(ar, m_dlgPrintBankKl.m_strBankName);
         ReadVariableString(ar, m_dlgPrintBankKl.m_strBlz);
         ReadVariableString(ar, m_dlgPrintBankKl.m_strKtoNo);
         ReadVariableString(ar, m_dlgPrintBankKl.m_strCurrency);
         ReadVariableString(ar, m_dlgPrintBankKl.m_strAmount);
      }

      if ((lChecksum != 0) && !m_FileHeader.IsValid(lChecksum))// Checksumme überprüfen
         AfxThrowArchiveException(CRC_EXCEPTION, NULL);
      
      if (m_FileHeader.GetVersion()<125)                    
      {
         POSITION pos = m_Invoices.GetHeadPosition();
         while (pos)
         {
            pInvoice = (Invoice*)m_Invoices.GetNext(pos);
            if (pInvoice) pInvoice->Update125(this);           // Artikel in der Rechnung Updaten
         }
         POSITION posI = m_Products.GetHeadPosition();
         while (posI)
         {
            pos = posI;
            pProduct = (Product*)m_Products.GetAt(pos);
            m_Products.GetNext(posI);
            if ((pProduct != NULL) && pProduct->IsDummy())
            {
               m_Products.RemoveAt(pos);                       // überflüssige Artikel löschen
               delete pProduct;
            }
         }
      }

   }
}

/////////////////////////////////////////////////////////////////////////////
// CFacturaDoc Diagnose

#ifdef _DEBUG
void CFacturaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFacturaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFacturaDoc Befehle

void CFacturaDoc::DeleteContents() 
{
   BEGIN("CFacturaDoc::DeleteContents");
//   REPORT("m_Customers");
   if (!m_Customers.IsEmpty())
   {
      while (m_Customers.GetHeadPosition())
      {
         Customer *pCustomer = (Customer*) m_Customers.RemoveHead();
         if (pCustomer) delete pCustomer;
      }
   }
//   REPORT("m_Invoices");
   if (!m_Invoices.IsEmpty())
   {
      while (m_Invoices.GetHeadPosition())
      {
         Invoice *pInvoice = (Invoice*) m_Invoices.RemoveHead();
         if (pInvoice) delete pInvoice;
      }
   }
//   REPORT("m_Products");
   if (!m_Products.IsEmpty())
   {
      while (m_Products.GetHeadPosition())
      {
         Product *pProduct = (Product*) m_Products.RemoveHead();
         if (pProduct) delete pProduct;
      }
   }
//   REPORT("m_States");
   if (!m_States.IsEmpty())
   {
      while (m_States.GetHeadPosition())
      {
         BYTE *pState = (BYTE*) m_States.RemoveHead();
         if (pState) delete[] pState;
      }
   }
//   REPORT("m_CurrencyUnits");
   if (!m_CurrencyUnits.IsEmpty())
   {
      while (m_CurrencyUnits.GetHeadPosition())
      {
         CurrencyUnit *pCurrencyUnit = (CurrencyUnit*) m_CurrencyUnits.RemoveHead();
         if (pCurrencyUnit) delete pCurrencyUnit;
      }
   }
//   REPORT("m_PayModes");
   if (!m_PayModes.IsEmpty())
   {
      while (m_PayModes.GetHeadPosition())
      {
         BYTE *pPM = (BYTE*) m_PayModes.RemoveHead();
         if (pPM) delete[] pPM;
      }
   }
//   REPORT("m_ProductGroup");
   if (!m_ProductGroup.IsEmpty())
   {
      while (m_ProductGroup.GetHeadPosition())
      {
         ProductOrPriceGroup *pPOP = (ProductOrPriceGroup *) m_ProductGroup.RemoveHead();
         if (pPOP) delete pPOP;
      }
   }
//   REPORT("m_PriceGroup");
   if (!m_PriceGroup.IsEmpty())
   {
      while (m_PriceGroup.GetHeadPosition())
      {
         ProductOrPriceGroup *pPOP = (ProductOrPriceGroup *) m_PriceGroup.RemoveHead();
         if (pPOP) delete pPOP;
      }
   }

//   REPORT("m_Container");
   m_Container.DeleteAll();
   UpdateAllViews(NULL, UPDATE_DELETE_CONTENTS);
   CCARADoc::DeleteContents();
}

void CFacturaDoc::OnEditInsertHiakData() 
{
   struct adressen
   {
      char vorname[16],
	   nachname[16],
	   strasse[30],
	   plz[12],
	   ort[20],
	   land[30],
	   kennung[12],
	   lft[3];
   };
   adressen adr;

   CFileDialog fd(true);
   fd.m_pOFN->nFilterIndex   = 1;
   fd.m_pOFN->nFileExtension = 1;
//   fd.m_pOFN->Flags         |= OFN_SHOWHELP;
   CString string;
   string.LoadString(IDS_HIAK_FILE);
   fd.m_pOFN->lpstrFilter    = LPCTSTR(string);
   for (int i=string.GetLength()-1; i>=0; i--) if (fd.m_pOFN->lpstrFilter[i] == '\n') ((char*)fd.m_pOFN->lpstrFilter)[i] = 0;
   fd.m_pOFN->nMaxCustFilter = 1;
   fd.m_pOFN->lpstrDefExt    = &fd.m_pOFN->lpstrFilter[strlen(fd.m_pOFN->lpstrFilter)];

   CFile file;
   if ((fd.DoModal() == IDOK) && (fd.m_pOFN->lpstrFileTitle != NULL) &&
        file.Open(fd.m_pOFN->lpstrFileTitle, CFile::modeRead))
   {
      int nBytes, i,  nSize = sizeof(adressen);
      Customer *pCustomer;
      for (i=1; ; i++)
      {
         nBytes = file.Read(&adr, nSize);
         if (nBytes != nSize) break;
         if (i==939) 
            nSize--;
         pCustomer = new Customer;
         if (!pCustomer) break;
         pCustomer->nNo = i;
         OemToChar(adr.vorname,  adr.vorname);
         pCustomer->szFirstName = _tcsdup(adr.vorname);
         OemToChar(adr.nachname, adr.nachname);
         pCustomer->szName = _tcsdup(adr.nachname);
         OemToChar(adr.plz, adr.plz);
         pCustomer->szPostalCode = _tcsdup(adr.plz);
         OemToChar(adr.ort, adr.ort);
         pCustomer->szCity = _tcsdup(adr.ort);
         OemToChar(adr.land, adr.land);
         pCustomer->szState = _tcsdup(adr.land);
         OemToChar(adr.strasse,  adr.strasse);
         pCustomer->szStreet = _tcsdup(adr.strasse);
         pCustomer->szStreetNo = 0;
         m_Customers.AddTail(pCustomer);
      }
      file.Close();
   }	
}

void CFacturaDoc::OnUpdateExtraExportCustomer(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(m_Customers.GetCount() > 0);
}

void CFacturaDoc::OnExtraImportCustomer() 
{
   int               i;
   DBFileHeader      DbFh;
   DBFieldDescriptor*pDbFd = NULL;
   BYTE              cTerminator;
   CFileDialog fd(true);
   fd.m_pOFN->nFilterIndex   = 1;
   fd.m_pOFN->nFileExtension = 1;
//   fd.m_pOFN->Flags         |= OFN_SHOWHELP;
   CString string;
   string.LoadString(IDS_DBASE_FILE);
   fd.m_pOFN->lpstrFilter    = LPCTSTR(string);
   for (i=string.GetLength()-1; i>=0; i--) if (fd.m_pOFN->lpstrFilter[i] == '\n') ((char*)fd.m_pOFN->lpstrFilter)[i] = 0;
   fd.m_pOFN->nMaxCustFilter = 1;
   fd.m_pOFN->lpstrDefExt    = &fd.m_pOFN->lpstrFilter[strlen(fd.m_pOFN->lpstrFilter)];

   CFile file;
   if ((fd.DoModal() == IDOK) && (fd.m_pOFN->lpstrFileTitle != NULL) &&
        file.Open(fd.m_pOFN->lpstrFileTitle, CFile::modeRead))
   {
      file.Read(&DbFh, sizeof(DBFileHeader));
      int nFields = (DbFh.nHeaderSize-sizeof(DBFileHeader)) / sizeof(DBFieldDescriptor);
      pDbFd = new DBFieldDescriptor[nFields];

      for (i=0; i< nFields; i++)
         file.Read(&pDbFd[i], sizeof(DBFieldDescriptor));

      file.Read(&cTerminator, 1);
      delete[] pDbFd;
      file.Close();      
   }

}

#define  NO_OF_FIELDS 12
struct DBFields
{
   char *pszFName;
   int   nFNlen;
   bool  bSave;
};

int CFacturaDoc::CustomerListCallBack(CListDlg *pDlg, int nReason)
{
   int i;
   DBFields *pFields = (DBFields*)pDlg->m_pParam;
   if (nReason == ONINITDIALOG)
   {
      pDlg->m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
      pDlg->m_List.ModifyStyle(0, LVS_NOCOLUMNHEADER, 0);
      pDlg->m_List.InsertColumn(0, "", LVCFMT_LEFT, 100, 0);
      for (i=0; i<NO_OF_FIELDS; i++)
      {
         pDlg->m_List.InsertItem(i, pFields[i].pszFName);
         ListView_SetCheckState(pDlg->m_List.m_hWnd, i, MF_CHECKED);
      }
      return 1;
   }
   else if (nReason == ONOK)
   {
      for (i=0; i<NO_OF_FIELDS; i++)
      {
         if (ListView_GetCheckState(pDlg->m_List.m_hWnd, i))
         {
            pFields[i].bSave = true;
         }
      }
   }
   return 0;
}

void CFacturaDoc::OnExtraExportCustomer() 
{
    int i;
    BYTE cSpace       = (BYTE)0x20;
    BYTE cTerminator  = (BYTE)0x0d;
    char cLineFeed[]  = "\r\n";
    char cSeparator[] = ",";
    DBFieldDescriptor DbFd;
    DBFileHeader DbFh;
    ZeroMemory(&DbFh, sizeof(DBFileHeader));
    DbFh.cVersion  = 0x03;
    DbFh.pcDATE[0] = 99;
    DbFh.pcDATE[1] = 04;
    DbFh.pcDATE[2] = 27;
    struct fields{
        enum {company, title, name,firstname, street, postalcode, city, state,phoneno,faxno,email,comment};
    };

    DBFields dbFields[NO_OF_FIELDS] =
    {
        {"COMPANY"     , COMPANY_LENGTH  , true}, // 0
        {"TITLE"       , TITLE_LENGTH    , true}, // 1
        {"SURNAME"     , NAME_LENGTH     , true}, // 3
        {"FIRSTNAME"   , FIRSTNAME_LENGTH, true}, // 2
        {"STREET"      , STREET_LENGTH   , true}, // 4
//        {"STREETNO"    , STREETNO_LENGTH , true}, // 5
        {"PC"          , PC_LENGTH       , true}, // 5
        {"CITY"        , CITY_LENGTH     , true}, // 6
        {"STATE"       , STATE_LENGTH    , true}, // 7
        {"PHONENO"     , PHONE_LENGTH    , true}, // 8
        {"FAXNO"       , FAX_LENGTH      , true}, // 9
        {"EMAIL"       , EMAIL_LENGTH    , true}, // 10
        {"COMMENT"     , COMMENT_LENGTH  , true}  // 11
    };

    CListDlg ListDlg;
    ListDlg.m_pCallBack = CustomerListCallBack;
    ListDlg.m_pParam    = dbFields;
    ListDlg.DoModal();

    DbFh.nRecordSize  = 1;  // Space
    DbFh.nHeaderSize  = sizeof(DBFileHeader);
    for (i=0; i<NO_OF_FIELDS; i++)
    {
        if (dbFields[i].bSave)
        {
            DbFh.nHeaderSize += sizeof(DBFieldDescriptor);
            DbFh.nRecordSize = (short) (DbFh.nRecordSize + (short)dbFields[i].nFNlen);
        }
    }
    DbFh.nHeaderSize++;

    CFindCustomer dlgFC;
    dlgFC.m_nMultiSelect = 1;
    if (dlgFC.DoModal() != IDOK) return;
    int        nCount = dlgFC.m_nMultiSelect;
    Customer **ppList = dlgFC.GetCustomerList();
    if (ppList==NULL) return;
    if (nCount == 0)  return;
    DbFh.nRecords     = nCount;

    CFileDialog fd(false);
    fd.m_pOFN->nFilterIndex   = 1;
    fd.m_pOFN->nFileExtension = 1;
    CString string;
    string.LoadString(IDS_DBASE_FILE);
    fd.m_pOFN->lpstrFilter    = LPCTSTR(string);
    for (i=string.GetLength()-1; i>=0; i--) if (fd.m_pOFN->lpstrFilter[i] == '\n') ((char*)fd.m_pOFN->lpstrFilter)[i] = 0;
    fd.m_pOFN->nMaxCustFilter = 3;
    fd.m_pOFN->lpstrDefExt    = &fd.m_pOFN->lpstrFilter[strlen(fd.m_pOFN->lpstrFilter)];

    CFile file;
    if (   (fd.DoModal() == IDOK) 
       && (fd.m_pOFN->lpstrFileTitle != NULL) 
       && file.Open(fd.m_pOFN->lpstrFile,CFile::modeCreate|CFile::modeWrite))
    {
        if (fd.m_pOFN->nFilterIndex == 3)
        {
            for (i=0; i<NO_OF_FIELDS; i++)
            {
                if (dbFields[i].bSave)
                {
                    file.Write(dbFields[i].pszFName, strlen(dbFields[i].pszFName));
                    file.Write(&cSeparator[0], 1);
                }
            };
            file.Write("empty", 5);
            file.Write(&cLineFeed[0], 2);

            POSITION  pos;
            pos = m_Customers.GetHeadPosition();
            Customer * pCustomer;
            int bConvert = fd.m_pOFN->nFilterIndex;
            for (int i=0; i<nCount; i++)
            {
                pCustomer = (Customer*) ppList[i];
                if (pCustomer)
                {
                    file.Write(&cSpace, 1);
                    if (dbFields[fields::company].bSave) WriteText(&file, pCustomer->szCompany   , 0, bConvert);
                    if (dbFields[fields::title].bSave) WriteText(&file, pCustomer->szTitle     , 0, bConvert);
                    if (dbFields[fields::name].bSave) WriteText(&file, pCustomer->szFirstName , 0, bConvert);
                    if (dbFields[fields::firstname].bSave) WriteText(&file, pCustomer->szName      , 0, bConvert);
                    if (dbFields[fields::street].bSave)
                    {
                        CString s = pCustomer->szStreet;
                        s += " ";
                        s += pCustomer->szStreetNo;
                        WriteText(&file, (char*)LPCTSTR(s), 0, bConvert);
                    }
                    //if (dbFields[fields::].bSave) WriteText(&file, pCustomer->szStreetNo  , 0, bConvert);
                    if (dbFields[fields::postalcode].bSave) WriteText(&file, pCustomer->szPostalCode, 0, bConvert);
                    if (dbFields[fields::city].bSave) WriteText(&file, pCustomer->szCity      , 0, bConvert);
                    if (dbFields[fields::state].bSave) WriteText(&file, pCustomer->szState     , 0, bConvert);
                    if (dbFields[fields::phoneno].bSave) WriteText(&file, pCustomer->szPhoneNo   , 0, bConvert);
                    if (dbFields[fields::faxno].bSave) WriteText(&file, pCustomer->szFaxNo     , 0, bConvert);
                    if (dbFields[fields::email].bSave) WriteText(&file, pCustomer->szEmail     , 0, bConvert);
                    if (dbFields[fields::comment].bSave) WriteText(&file, pCustomer->szComment   , 0, bConvert);
                    file.Write("empty", 5);
                    file.Write(&cLineFeed[0], 2);
                }
            }
        }
        else
        {
            file.Write(&DbFh, sizeof(DBFileHeader));
            for (i=0; i<NO_OF_FIELDS; i++)
            {
                if (dbFields[i].bSave)
                {
                    ZeroMemory(&DbFd, sizeof(DBFieldDescriptor));
                    strcpy((char*)DbFd.pcFieldName, dbFields[i].pszFName);
                    DbFd.cFieldType  = 'C';
                    DbFd.cFieldSize  = (BYTE) dbFields[i].nFNlen;
                    file.Write(&DbFd, sizeof(DBFieldDescriptor));
                }
            };
            file.Write(&cTerminator, 1);
            POSITION  pos;
            pos = m_Customers.GetHeadPosition();
            Customer * pCustomer;
            int bConvert = fd.m_pOFN->nFilterIndex;
            for (int i=0; i<nCount; i++)
            {
                pCustomer = (Customer*) ppList[i];
                if (pCustomer)
                {
                    int j=-1;
                    file.Write(&cSpace, 1);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szCompany   , dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szTitle     , dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szFirstName , dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szName      , dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szStreet    , dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szStreetNo  , dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szPostalCode, dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szCity      , dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szState     , dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szPhoneNo   , dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szFaxNo     , dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szEmail     , dbFields[j].nFNlen, bConvert);
                    if (dbFields[++j].bSave) WriteText(&file, pCustomer->szComment   , dbFields[j].nFNlen, bConvert);
                }
            }
        }
        file.Close();      
    }
}
void CFacturaDoc::WriteText(CFile *pF, char *pszText, UINT nSize, int bConvert)
{
   ASSERT(nSize < 256);

   if (bConvert == 3)
   {
       CString s(pszText);
       s.Replace("\r\n", " ");
       s.Replace(",", ";");
       pF->Write(LPCTSTR(s), s.GetLength());
       pF->Write(",", 1);
   }
   else if (bConvert == 1)
   {
      char  szDosText[256];
      ZeroMemory(szDosText, 256);
      CharToOem(pszText, (char*)szDosText);
      pF->Write(szDosText, nSize);
   }
   else
   {
      char  szDosText[256];
      ZeroMemory(szDosText, 256);
      strcpy(szDosText, pszText);
      pF->Write(pszText, nSize);
   }
}

void CFacturaDoc::SetTitle(LPCTSTR lpszTitle) 
{
   CCARADoc::SetTitle(lpszTitle);
}

void CFacturaDoc::FormatDataString(const char *pszFormat, CString &strText)
{
   const char   *str;
   char *strBracket1 = NULL, *strBracket2 = NULL;
   int     nCode;
   CString strFormat;
   for (str = strstr(pszFormat, "$"); str !=NULL; str = strstr(pszFormat, "$"))
   {
      if (strBracket2) strBracket2[0] = '}';           // Klammer 2 wiederherstellen !

      strBracket1 = (char*)strstr(str            , "{");      // Formatstring Anfang
      if (strBracket1==NULL) break;
      strBracket2 = (char*)strstr(&strBracket1[1], "}");      // Formatstring Ende
      if (strBracket2==NULL) break;
      strBracket2[0] = 0;
      pszFormat = strBracket2;                         // Zum nächsten springen
      nCode = atoi(&str[1]);                           // Formatierungscode
      strFormat.Empty();

      if ((nCode >= CUSTOMER_IDS) && (nCode < INVOICE_IDS))
      {
         nCode -= CUSTOMER_IDS;
         switch (nCode)
         {
            case 14:
               SetInteger(strFormat, &strBracket1[1], m_Customers.GetCount());
               m_nActualCustomer = -1;
               break;
            case 15:
            {
               SetInteger(strFormat, &strBracket1[1], ++m_nActualCustomer + 1);
               POSITION pos = m_Customers.FindIndex(m_nActualCustomer);
               if (pos) m_pActualCustomer = (Customer*)m_Customers.GetAt(pos);
            } break;
         }
         if (m_pActualCustomer == NULL) continue;
         switch (nCode)
         {
            case 0: SetString( strFormat, &strBracket1[1], m_pActualCustomer->szFirstName);  break;
            case 1: SetString( strFormat, &strBracket1[1], m_pActualCustomer->szName);       break;
            case 2: SetString( strFormat, &strBracket1[1], m_pActualCustomer->szTitle);      break;
            case 3: SetString( strFormat, &strBracket1[1], m_pActualCustomer->szPhoneNo);    break;
            case 4: SetString( strFormat, &strBracket1[1], m_pActualCustomer->szFaxNo);      break;
            case 5: SetString( strFormat, &strBracket1[1], m_pActualCustomer->szCity);       break;
            case 6: SetString( strFormat, &strBracket1[1], m_pActualCustomer->szState);      break;
            case 7: SetString( strFormat, &strBracket1[1], m_pActualCustomer->szStreet);     break;
            case 8: SetString( strFormat, &strBracket1[1], m_pActualCustomer->szCompany);    break;
            case 9: SetString( strFormat, &strBracket1[1], m_pActualCustomer->szPostalCode); break;
            case 10:SetString( strFormat, &strBracket1[1], m_pActualCustomer->szStreetNo);   break;
            case 11:SetString( strFormat, &strBracket1[1], m_pActualCustomer->szEmail);      break;
            case 12:SetString( strFormat, &strBracket1[1], m_pActualCustomer->szComment);    break;
            case 13:SetInteger(strFormat, &strBracket1[1], m_pActualCustomer->nNo);          break;
            default: break;
         }
      }

      if ((nCode >= INVOICE_IDS) && (nCode < PRODUCT_IDS))
      {
         nCode -= INVOICE_IDS;
         switch (nCode)
         {
            case 16:
               SetInteger(strFormat, &strBracket1[1], m_Invoices.GetCount());             
               m_nActualInvoice = -1;
               break;
            case 17:
            {
               SetInteger(strFormat, &strBracket1[1], ++m_nActualInvoice + 1);
               POSITION pos = m_Invoices.FindIndex(m_nActualInvoice);
               if (pos) m_pActualInvoice = (Invoice*) m_Invoices.GetAt(pos);
               if (m_pActualInvoice)
               {
                  pos = m_Customers.FindIndex(m_pActualInvoice->nCustNo);
                  if (pos) m_pActualCustomer = (Customer*) m_Customers.GetAt(pos);
               }
            } break;
         }
         if (m_pActualInvoice == NULL) continue;
         switch (nCode)
         {
            case 0: strFormat.Format(&strBracket1[1], m_pActualInvoice->nYear, m_pActualInvoice->nMonth, m_pActualInvoice->nInvNo); break;
            case 1: SetInteger(strFormat, &strBracket1[1], m_pActualInvoice->nCustNo);    break;
            case 2: SetInteger(strFormat, &strBracket1[1], m_pActualInvoice->nYear);      break;
            case 3: SetInteger(strFormat, &strBracket1[1], m_pActualInvoice->nMonth);     break;
            case 4: 
            {
               CTime time (m_pActualInvoice->nYear, m_pActualInvoice->nMonth, 1, 1, 1, 1, 0);
               setlocale( LC_ALL, "German" );
               SetString(strFormat,  &strBracket1[1], (char*)LPCTSTR(time.FormatGmt("%B")));
            } break;
            case 5: SetInteger(strFormat, &strBracket1[1], m_pActualInvoice->nDay);       break;
            case 6: SetInteger(strFormat, &strBracket1[1], m_pActualInvoice->nPositions); 
               m_nActualPosition = -1;
               break;
            case 7: SetInteger(strFormat, &strBracket1[1], ++m_nActualPosition + 1);      break;
            case 8: SetString( strFormat, &strBracket1[1], m_pActualInvoice->szComment);  break;
            case 9: SetFloat(  strFormat, &strBracket1[1], m_fTax*100.0f);                break;
            case 10: case 11: case 12: case 13: case 14: case 18: case 19: case 21: case 22:
            {
               float fAmount = CalcActAmount();
               if ((nCode == 21)||(nCode == 22))               // Rabatt betrag
               {
                  fAmount *= (m_pActualInvoice->fRabatt*0.01f);
               }
               else if (m_pActualInvoice->fRabatt>0)
               {
                  fAmount -= fAmount*(m_pActualInvoice->fRabatt*0.01f);
               }

               if (nCode == 10)                                 fAmount *= m_fTax;        // MwSt Betrag
               if ((nCode == 12)||(nCode == 13)||(nCode == 22)) fAmount *= (1.0f+m_fTax); // MwSt Gesamt

               if (!m_bConvert && ((nCode == 13)||(nCode == 18)))// Referenzwährung
               {
                  CurrencyUnit* pCU = GetUnit(m_pActualInvoice->szUnitSign, UNIT_SIGN);
                  if (pCU) fAmount /= pCU->fCurrency;
               }
               if (m_bConvert&&((nCode == 14)||(nCode == 19))) // andere Währung
               {
                  CurrencyUnit* pCU;
                  pCU = GetUnit(m_pActualInvoice->szUnitSign, UNIT_SIGN);
                  if ((pCU==NULL) && (m_pActualCustomer!=NULL))
                  {
                     State *pS = GetState(m_pActualCustomer->szState, STATE);
                     pCU = GetUnit(pS->szStateSign, STATE_SIGN);
                  }
                  if (pCU)
                  {
                     fAmount *= pCU->fCurrency;
                  }
               }
               SetFloat(strFormat, &strBracket1[1], fAmount); 
            } break;
            case 20: 
            {
               if (m_pActualInvoice->fRabatt>0)
                  SetFloat(strFormat, &strBracket1[1], m_pActualInvoice->fRabatt); 
            }break;
            case 15:SetString(strFormat, &strBracket1[1], GetPayMode(m_pActualInvoice->nFlags)); break;
         }
      }

      if ((nCode >= PRODUCT_IDS) && (nCode < CURRENCY_IDS))
      {
         nCode -= PRODUCT_IDS;
         switch (nCode)
         {
            case 10: SetInteger(strFormat, &strBracket1[1], m_Products.GetCount());
               m_nActualProduct = -1;
               break;
            case 11:
            {
               SetInteger(strFormat, &strBracket1[1], ++m_nActualProduct); break;
            }
         }
         if ((m_pActualInvoice != NULL) && (m_pActualInvoice->pPosition != NULL) &&
             (m_nActualPosition >= 0) && (m_nActualPosition < m_pActualInvoice->nPositions))
         {
            Product *pProduct;
            switch (nCode)
            {
               case 0: SetInteger(strFormat, &strBracket1[1], m_pActualInvoice->pPosition[m_nActualPosition].nProductNo); break;
               case 1: SetInteger(strFormat, &strBracket1[1], m_pActualInvoice->pPosition[m_nActualPosition].nCount);     break;
            }
            pProduct = GetProduct(m_pActualInvoice->pPosition[m_nActualPosition].nProductNo);
            if (pProduct)
            {
               switch (nCode)
               {
                  case 2:
                  {
                     CString str(pProduct->szDescription);
                     int nIndex = -1;
                     pProduct->GetPrice(m_pActualInvoice->pPosition[m_nActualPosition].cPriceGroup, &nIndex);
                     if (pProduct->IsPriceGroupVisible(nIndex))
                     {
                        ProductOrPriceGroup *pPOP = GetPriceGroup(m_pActualInvoice->pPosition[m_nActualPosition].cPriceGroup);
                        if (pPOP) str.Format("%s %s", pProduct->szDescription, pPOP->strDescription);
                     }
                     SetString( strFormat, &strBracket1[1], str);break;
                  }
                  case 12:SetInteger(strFormat, &strBracket1[1], pProduct->sEAN_Code.ucReleaseH); break;
                  case 13:SetInteger(strFormat, &strBracket1[1], pProduct->sEAN_Code.ucReleaseL); break;
                  case 14:SetInteger(strFormat, &strBracket1[1], pProduct->sEAN_Code.ucProductGroup); break;
//                  case 16:SetInteger(strFormat, &strBracket1[1], pProduct->sEAN_Code.ucPriceGroup&~NO_PRICE_GROUP); break;
                  case 18:SetInteger(strFormat, &strBracket1[1], pProduct->sEAN_Code.ulCompanyEAN); break;
                  case 15:
                  {
                     pProduct->sEAN_Code.ucProductGroup;
                  } break;
                  case 17:
                  {
                     pProduct->sEAN_Code.ucPriceGroup;
                  } break;
                  case 3: case 4: case 5: case 6: case 7: case 8: case 9:
                  {
                     float fPrice = 0;
                     fPrice = pProduct->GetPrice(m_pActualInvoice->pPosition[m_nActualPosition].cPriceGroup);
                     if ((nCode == 4) || (nCode == 6) || (nCode == 9)) fPrice *= (float)m_pActualInvoice->pPosition[m_nActualPosition].nCount;
                     if ((nCode == 5) || (nCode == 6) || (nCode == 9)) fPrice *= (1.0f+m_fTax);
                     if ((nCode == 7) || (nCode == 8))                 fPrice *= m_fTax;
                     if (nCode == 9)
                     {
                        CurrencyUnit* pCU = GetUnit("?", BASE_UNIT);
                        if (pCU) fPrice /= pCU->fCurrency;
                     }
                     if (nCode == 19)
                     {
                        CurrencyUnit* pCBU = GetUnit("?", BASE_UNIT);
                        CurrencyUnit* pCU  = GetUnit(m_pActualInvoice->szUnitSign, UNIT_SIGN);
                        if (pCU && pCBU) fPrice *= (pCU->fCurrency / pCBU->fCurrency);
                     }
                     SetFloat(strFormat, &strBracket1[1], fPrice);
                  } break;
               }
            }
         }
         else
         {
            POSITION pos = m_Products.FindIndex(m_nActualProduct);
            if (pos)
            {
               int nPrice;
               Product *pProduct = (Product*) m_Products.GetAt(pos);
               if (pProduct)
               {
                  for (nPrice=0; nPrice<pProduct->GetNoOfPrices(); nPrice++)
                  {
                     switch (nCode)
                     {
                        case 0: SetInteger(strFormat, &strBracket1[1], pProduct->nNo          ); break;
                        case 1: SetInteger(strFormat, &strBracket1[1], pProduct->nCount       ); break;
                        case 2: SetString( strFormat, &strBracket1[1], pProduct->szDescription); break;
                        case 3: case 5: case 7: case 9:
                        {
                           float fPrice = 0;
                           fPrice = pProduct->GetPrice(0xff, &nPrice);
                           if ((nCode == 5) || (nCode == 9)) fPrice *= (1.0f+m_fTax);
                           if (nCode == 7)                   fPrice *= m_fTax;
                           if (nCode == 9)
                           {
                              CurrencyUnit* pCU = GetUnit("?", BASE_UNIT);
                              if (pCU) fPrice /= pCU->fCurrency;
                           }
                           SetFloat(strFormat, &strBracket1[1], fPrice);
                        } break;
                        case 12:SetInteger(strFormat, &strBracket1[1], pProduct->sEAN_Code.ucReleaseH); break;
                        case 13:SetInteger(strFormat, &strBracket1[1], pProduct->sEAN_Code.ucReleaseL); break;
                        case 14:SetInteger(strFormat, &strBracket1[1], pProduct->sEAN_Code.ucProductGroup); break;
//                        case 15:SetInteger(strFormat, &strBracket1[1], pProduct->sEAN_Code.ucPriceGroup); break;
                        case 18:SetInteger(strFormat, &strBracket1[1], pProduct->sEAN_Code.ulCompanyEAN); break;
                     }
                  }
               }
            }
         }
      }

      if ((nCode >= CURRENCY_IDS) && (nCode < CURRENCY_IDS+100))
      {
         nCode -= CURRENCY_IDS;
         CurrencyUnit *pCU = NULL;
         switch (nCode)
         {
            case 0: case 3: case 6:                            // Andere Währungen
               if (m_pActualInvoice) pCU = GetUnit(m_pActualInvoice->szUnitSign, UNIT_SIGN);
               break;
            case 1: case 4: case 7: pCU = GetUnit("?", BASE_UNIT); break;// Basiswährung !
            case 2: case 5:         pCU = GetUnit("?", REF_UNIT ); break;// Bezugswährung
         }
         if (pCU)
         {
            switch (nCode)
            {
               case 0:case 1:case 2: SetString(strFormat, &strBracket1[1], pCU->szUnit    ); break;
               case 3:case 4:case 5: SetString(strFormat, &strBracket1[1], pCU->szUnitSign); break;
               case 6:case 7:        SetFloat( strFormat, &strBracket1[1], pCU->fCurrency ); break;
            }
         }
      }
      strText += strFormat;
      if (strBracket2) strBracket2[0] = '}';           // Klammer 2 wiederherstellen !
   }
   if (strBracket2) strBracket2[0] = '}';
}

void CFacturaDoc::SetString(CString &strFormat, const char*pszFormat, const char *pszInput)
{
   if ((pszInput != NULL) && (pszInput != szEmptyString))
   {
      strFormat.Format(pszFormat, pszInput); 
   }
}

void CFacturaDoc::SetInteger(CString &strFormat, const char *pszFormat, int nInput)
{
   if (strstr(pszFormat, "%s") == NULL)
   {
      strFormat.Format(pszFormat, nInput);
   }
}

void CFacturaDoc::SetFloat(CString &strFormat, const char *pszFormat, float fInput)
{
   if (strstr(pszFormat, "%s") == NULL)
   {
      strFormat.Format(pszFormat, fInput);
   }
}

void CFacturaDoc::SetActualInvoice(Invoice * pInvoice)
{
   if ((pInvoice == NULL) && m_Invoices.GetCount())
   {
      m_pActualInvoice = (Invoice*) m_Invoices.GetHead();
   }
   else m_pActualInvoice  = pInvoice;

   if (m_pActualInvoice)
   {
      m_pActualCustomer = NULL;
      m_nActualPosition = -1;
      POSITION  pos     = m_Customers.GetHeadPosition();
      if (pos)
      {
         Customer *pCustomer;
         int       nCustNo = m_pActualInvoice->nCustNo;
         while (pos)
         {
            pCustomer = (Customer*)m_Customers.GetNext(pos);
            if (pCustomer->nNo == nCustNo)
            {
               m_pActualCustomer = pCustomer;
               break;
            }
         }
      }
   }
   else
   {
      m_nActualPosition = -2;
   }
}

void CFacturaDoc::SetActualCustomer(Customer *pCustomer)
{
   m_pActualCustomer = pCustomer;
}

void CFacturaDoc::OnShowLabelStrukture()
{
   ShowLabelStructure();	
}

float CFacturaDoc::CalcActAmount()
{
   float fAmount = 0;
   if (m_pActualInvoice && m_pActualInvoice->pPosition)
   {
      CurrencyUnit   *pInvCU  = GetUnit(m_pActualInvoice->szUnitSign, UNIT_SIGN);
      CurrencyUnit   *pBaseCU = GetUnit("?", BASE_UNIT);
      CurrencyUnit   *pRefCU  = GetUnit("?", REF_UNIT);
      CurrencyUnit   *pProdCU = NULL;
      int i;
      float fPrice;
      Product *pProduct;
      m_bConvert = false;
      for (i=0; i<m_pActualInvoice->nPositions; i++)
      {
         pProduct = GetProduct(m_pActualInvoice->pPosition[i].nProductNo);
         if (pProduct)
         {
            pProdCU = GetCurrencyOfPriceGroup(m_pActualInvoice->pPosition[i].cPriceGroup);
            if ((pProdCU != NULL) && (pInvCU != NULL) && (pProdCU != pInvCU))
            {
                m_bConvert = true;                                  // nur eine muß unterschiedlich sein
                break;
            }
         }
      }
      for (i=0; i<m_pActualInvoice->nPositions; i++)
      {
         pProduct = GetProduct(m_pActualInvoice->pPosition[i].nProductNo);
         if (pProduct)
         {
            fPrice = pProduct->GetPrice(m_pActualInvoice->pPosition[i].cPriceGroup) * m_pActualInvoice->pPosition[i].nCount;
            if (m_bConvert)
            {
               pProdCU = GetCurrencyOfPriceGroup(m_pActualInvoice->pPosition[i].cPriceGroup);
               if ((pProdCU != NULL) && (pRefCU != NULL) && (pProdCU != pRefCU))
               {
                  fPrice /= pProdCU->fCurrency;                   // Umrechnen in Referenzwährung, prüfen !!!
               }
            }
            fAmount += fPrice;
         }
      }
   }
   return fAmount;
}

Product * CFacturaDoc::GetProduct(int nProdNo)
{
   POSITION  pos = m_Products.GetHeadPosition();
   while (pos)
   {
      Product  *pProduct = (Product*)m_Products.GetNext(pos);
      if (pProduct  && (pProduct->nNo == nProdNo)) return pProduct;
   }
   return NULL;
}

char* CFacturaDoc::GetPayMode(long nFlags)
{
   int nPayMode = (nFlags&FLAG_PAY_MOD_MASK) >> 16;
   POSITION pos = m_PayModes.GetHeadPosition();
   while (pos)
   {
      PayMode *pPM = (PayMode*) m_PayModes.GetNext(pos);
      if (pPM && (pPM->nMode == nPayMode)) return pPM->szMode;
   }
   return "";
}

void CFacturaDoc::CheckActualPointers()
{
   if (m_pActualCustomer)
   {
      if (m_Customers.Find(m_pActualCustomer) == NULL) m_pActualCustomer = NULL;
   }
   if (m_pActualInvoice)
   {
      if (m_Invoices.Find(m_pActualInvoice)== NULL)    m_pActualInvoice = NULL;
   }
}

State * CFacturaDoc::GetState(const char *pszFind, UINT nFind)
{
   POSITION pos = m_States.GetHeadPosition();
   while (pos)
   {
      State *pS = (State*) m_States.GetNext(pos);
      if (pS)
      {
         if      ((nFind == STATE_SIGN) && (strcmp(pS->szStateSign, pszFind)==0)) return pS;
         else if ((nFind == STATE)      && (strcmp(pS->szState    , pszFind)==0)) return pS;
      }
   }
   return NULL;
}

CurrencyUnit *CFacturaDoc::GetUnit(const char *pszFind, UINT nFind)
{
   if ((pszFind != szEmptyString) && (pszFind != NULL))
   {
      POSITION pos = m_CurrencyUnits.GetHeadPosition();
      while (pos)
      {
         CurrencyUnit * pCU = (CurrencyUnit*) m_CurrencyUnits.GetNext(pos);
         if (pCU)
         {
            switch (nFind)
            {
               case UNIT_SIGN:  if (strcmp(pCU->szUnitSign , pszFind) == 0) return pCU; else break;
               case STATE_SIGN: if (strcmp(pCU->szStateSign, pszFind) == 0) return pCU; else break;
               case UNIT:       if (strcmp(pCU->szUnit     , pszFind) == 0) return pCU; else break;
               case BASE_UNIT:  if (pCU->szUnit[0] == '!')                  return pCU; else break;
               case REF_UNIT:   if (pCU->fCurrency == 1.0f)                 return pCU; else break;
            }
         }
      }
   }
   return NULL;
}

void CFacturaDoc::WriteVariableString(CArchive &ar, CString &str)
{
   if (str.IsEmpty())
      CLabel::WriteVariableString(ar, "");
   else
      CLabel::WriteVariableString(ar, (char*)LPCTSTR(str));
}

void CFacturaDoc::ReadVariableString(CArchive &ar, CString &str)
{
   long nSize;
   CLabel::ArchiveRead(ar, &nSize, sizeof(long), true);
   CLabel::ArchiveRead(ar, str.GetBufferSetLength(nSize), nSize, true);
   str.ReleaseBuffer();
}

ProductOrPriceGroup* CFacturaDoc::GetPriceGroup(BYTE cPriceGroup)
{
   POSITION pos = m_PriceGroup.GetHeadPosition();
   while (pos)
   {
      ProductOrPriceGroup* pPOP = (ProductOrPriceGroup*)m_PriceGroup.GetNext(pos);
      if (pPOP && (pPOP->nNo == cPriceGroup))
      {
         return pPOP;
      }
   }
   return NULL;
}

CurrencyUnit* CFacturaDoc::GetCurrencyOfPriceGroup(BYTE cPriceGroup)
{
   ProductOrPriceGroup* pPOP = GetPriceGroup(cPriceGroup);
   if (pPOP)
   {
      return GetUnit(pPOP->strCurrency, UNIT_SIGN);
   }

   return NULL;
}
