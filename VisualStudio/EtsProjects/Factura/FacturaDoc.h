// FacturaDoc.h : Schnittstelle der Klasse CFacturaDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACTURADOC_H__FA828EEA_A94A_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_FACTURADOC_H__FA828EEA_A94A_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CARADoc.h"

#include "PrintBankKl.h"

#define NAME_LENGTH      64             // String Längen für Kunden
#define FIRSTNAME_LENGTH 64             
#define PHONE_LENGTH     64
#define FAX_LENGTH       64
#define CITY_LENGTH      64
#define STREET_LENGTH    64
#define COMPANY_LENGTH   64
#define STATE_LENGTH     64
#define TITLE_LENGTH     64
#define STREETNO_LENGTH  32
#define PC_LENGTH        32
#define EMAIL_LENGTH    128
#define COMMENT_LENGTH  256
                                              // Flags für Kunden
#define CUSTOMER_FEMALE          0x00000001   // mänl. / weibl.
#define CUSTOMER_SERVICE_CARD    0x00000002   // ETS-Service-Karte ausgefüllt
#define CUSTOMER_HIAK            0x00000004   // alter Hiak-Kunde
#define CUSTOMER_CALE            0x00000008   // alter Cale-Kunde
#define CUSTOMER_FLAG_CNT        31

#define CUSTOMER_IDS  200
#define INVOICE_IDS   300
#define PRODUCT_IDS   400
#define CURRENCY_IDS  500

class  Customer                           // Struktur für Kunden
{
public:
   Customer();
   ~Customer();
	void FreeAllText();
	void Serialize(CArchive&);
   void Read1_0(CArchive&);

// lesen und schreiben : Grundfunktion
   static void WriteText(CArchive&,char*);
	static char * ReadText(CArchive&);

private:
   void InitAllText();

public:
   char *szName;      // VorName des Kunden
   char *szFirstName; // Familienname des Kunden
   char *szTitle;     // Titel, Akademische Grade
   char *szPhoneNo;   // Telefonnummer
   char *szFaxNo;     // Telefonnummer
   char *szCity;      // Ort
   char *szState;     // Land
   char *szStreet;    // Straße
   char *szCompany;   // Firma
   char *szPostalCode;// Postleitzahl
   char *szStreetNo;  // Hausnummer
   char *szEmail;     // E-Mail Adresse
   char *szComment;   // Kommentar
   long  nNo;         // Kundennummer
   long  nFlags;      // Flags für ...
};

struct Position                        // Struktur für Artikelpositionen
{
   long  nProductNo;                   // Artikelnummer
// long  nCount;                       // Anzahl der Artikel wurde aufgeteilt in :
   WORD  nCount;                       // LOWORD() = Anzahl der Artikel
   BYTE  cPriceGroup;                  // Preisgruppe
   BYTE  cDummy;                       // weiteres
};                   

#define FLAG_SUPPLY       0x00000001   // Angebot               
#define FLAG_INVOICE      0x00000002   // Rechnung
#define FLAG_PAID         0x00000004   // Rechnung bezahlt
#define FLAG_CANCELED     0x00000008   // Rechnung storniert
#define FLAG_PRINT_INV    0x00000010   // Rechnung drucken
#define FLAG_PRINT_NN     0x00000020   // Nachnahmezahlschein drucken
#define FLAG_PRINT_CUST   0x00000040   // Adresslabel drucken
#define FLAG_PRINT_BNK    0x00000080   // Bankaufkleber drucken
#define FLAG_USER_MASK    0x0000ff00   // User Flags
#define FLAG_USER_CNT     8            // Anzahl User Flags
#define FLAG_PAY_MOD_MASK 0xffff0000   // Bezahlungsart

class CFacturaDoc;

class Invoice                          // Struktur für Rechnung usw.
{
public:
   Invoice();
   ~Invoice();
	void Serialize(CArchive&, int nVersion=0);
   void Read1_1(CArchive&);
	void Update125(CFacturaDoc*);
	void FreeAllText();

   long     nInvNo;                    // Rechnungsnummer
   long     nCustNo;                   // Kundennummer
   long     nYear;                     // Jahr
   long     nMonth;                    // Monat
   long     nDay;                      // nDay
   long     nFlags;                    // Flags für Angebot, Rechnung, Bezahlt,...
   char     szUnitSign[8];             // Währungseinheit
   float    fRabatt;
   long     nPositions;                // Anzahl Positionen
   Position *pPosition;                // Positionen
   char     *szComment;                // Kommentar
};

#define NOT_COUNTABLE       0x80000000
#define PRICE_GROUP_VISIBLE 0x80       // neues Format, Preisgruppe unsichbar

#define PRICE_GROUP_ARTIKEL_HIDDEN 0x40 // neues Format, Preisgruppe Artikel versteckt

struct EAN_Code
{
   unsigned long  ulCompanyEAN;
   unsigned char  ucProductGroup;
   unsigned char  ucReleaseH;
   unsigned char  ucReleaseL;
   unsigned char  ucPriceGroup;
public:
	int GetEAN();
};

class Product                         // Struktur Artikel
{
   public:
	   void RemovePrice(unsigned char ucPriceGroup);
   Product();
   ~Product();
	void  Serialize(CArchive&);
   void  Read1_1(CArchive&, CPtrList *, int);
	void  FreeAllText();
   float GetPrice(unsigned char, int *pnIndex=NULL) const;
   bool  SetPrice(unsigned char, float, bool);
   void  ReallocPrice();
   bool  IsDummy() {return (pfPrice == NULL) ? true : false;};
   int   GetNoOfPrices() const {return nPrices;};
   BYTE  GetPriceGroup(int) const;
   bool  IsPriceGroupVisible(int) const;
 
   long     nNo;
   long     nCount;
   EAN_Code sEAN_Code;
   char    *szDescription;
private:
   long     nPrices;
   float   *pfPrice;
   unsigned char *pucPriceGroup;
};

class ProductOrPriceGroup
{
public:
   ProductOrPriceGroup()
   {
      nNo         = 0;
   }
   void Read1_1(CArchive&);
	void Serialize(CArchive&);
   long nNo;
	void Update125(CFacturaDoc*);
   CString strCurrency;
   CString strDescription;
};

struct State                           // Struktur für Länder
{
   long nSize;
   char szStateSign[4];
   char szState[1];
};

struct CurrencyUnit                    // Struktur für Währungseinheiten
{
   char  szUnitSign[8];
   char  szStateSign[4];
   char  szUnit[48];
   float fCurrency;
};

struct InvoiceFind                     // Suchstruktur für Rechnungen
{
   Invoice  *pI;
   Customer *pC;
};

struct InvoiceProduct                  // Struktur für Produkte
{
   Position *pPos;
   Product  *pPro;
};

struct PayMode                         // Struktur für Bezahlungsart
{
   long nSize,
        nMode;
   char szMode[1];
};

struct DBFieldDescriptor               // Feld Beschreibung für DBase
{
   BYTE  pcFieldName[11];
   BYTE  cFieldType;
   void *pData;
   BYTE  cFieldSize;
   BYTE  cFieldCount;
   BYTE  pcReserved[14];
};

struct DBFileHeader                    // DBase FileHeader
{
   BYTE  cVersion;
   BYTE  pcDATE[3];
   long  nRecords;
   short nHeaderSize;
   short nRecordSize;
   BYTE  pcReserved[20];
};

class CListDlg;

class CFacturaDoc : public CCARADoc
{
protected: // Nur aus Serialisierung erzeugen
   CFacturaDoc();
   DECLARE_DYNCREATE(CFacturaDoc)

// Attribute
public:

// Operationen
public:
// Überladungen
   // Vom Klassenassistenten generierte Überladungen virtueller Funktionen
   //{{AFX_VIRTUAL(CFacturaDoc)
	public:
   virtual BOOL OnNewDocument();
   virtual void Serialize(CArchive& ar);
   virtual void DeleteContents();
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

// Implementierung
public:
	ProductOrPriceGroup* GetPriceGroup(BYTE);
	bool m_bConvert;
   virtual ~CFacturaDoc();
	void          SetActualCustomer(Customer*);
	void          SetActualInvoice(Invoice*);
	void          CheckActualPointers();
	void          FormatDataString(const char *, CString&);
	float         CalcActAmount();
	char*         GetPayMode(long nFlags);
	Product*      GetProduct(int);
	CurrencyUnit* GetUnit(const char*, UINT nFind=0);
	State*        GetState(const char *, UINT);
	CurrencyUnit* GetCurrencyOfPriceGroup(BYTE);

 	static void ReadVariableString(CArchive&, CString&);
   static void WriteVariableString(CArchive&, CString&);

#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

   CPtrList m_Customers;
   CPtrList m_Invoices;
   CPtrList m_Products;
   CPtrList m_States;
   CPtrList m_CurrencyUnits;
   CPtrList m_PayModes;
   CPtrList m_ProductGroup;
   CPtrList m_PriceGroup;
   CString  m_strInvoiceFlags[FLAG_USER_CNT];
   CString  m_strCustomerFlags[CUSTOMER_FLAG_CNT];
   float    m_fTax;
   float    m_fNNAmount;
   CPrintBankKl m_dlgPrintBankKl;

protected:
   Invoice  *m_pActualInvoice;
   int       m_nActualPosition;
   Customer *m_pActualCustomer;
   int       m_nActualCustomer;
   int       m_nActualInvoice;
   int       m_nActualProduct;

// Generierte Message-Map-Funktionen
protected:
   //{{AFX_MSG(CFacturaDoc)
   afx_msg void OnEditInsertHiakData();
	afx_msg void OnExtraExportCustomer();
	afx_msg void OnUpdateExtraExportCustomer(CCmdUI* pCmdUI);
	afx_msg void OnExtraImportCustomer();
	afx_msg void OnShowLabelStrukture();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
private:
	void WriteText(CFile *pF, char*, UINT, int);

	static void SetString( CString &, const char*, const char*);
	static void SetInteger(CString&, const char *, int);
	static void SetFloat(  CString&, const char *, float);
	static int  CustomerListCallBack(CListDlg*, int);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_FACTURADOC_H__FA828EEA_A94A_11D2_9DB9_0000B458D891__INCLUDED_)
