#ifndef AFX_INVOICE_H__06AF3E62_ABA4_11D2_8859_B236EB6AD456__INCLUDED_
#define AFX_INVOICE_H__06AF3E62_ABA4_11D2_8859_B236EB6AD456__INCLUDED_

// Invoice.h : Header-Datei
//
#include "CustomerList.h"
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInvoice 
class  Invoice;
class  Product;
class  CFacturaView;
class  CListDlg;
class  CTextEdit;
struct CurrencyUnit;
struct Position;

class CInvoice : public CDialog
{
// Konstruktion
public:
   CInvoice(CWnd* pParent = NULL);   // Standardkonstruktor
   virtual ~CInvoice();
   void        SetDocChanged();
   void        SetInvoiceDate();
   void        InitInvoiceData();
	bool        InitInvoiceData(Invoice*);
   Invoice*    GetInvoice();
   Product*    GetProduct(int);
   Product*    GetProductFromNo(int nNo);
   Customer*   GetCustomer(int nNo = -1);
   bool        GetCustomerData(int nNo = -1);
   void        InsertProduct(Product*, Position *pPos);
   void        CalculateAmount();
   bool        SaveChangeData();
   bool        SaveInvoice();
   void        DeleteProduct(int);
   void        DeleteProductList();
   void        DeleteInvoicePositions(Invoice*);
	void        CalculateRabatt(float&);
	void        CheckConvert();

// Dialogfelddaten
   //{{AFX_DATA(CInvoice)
	enum { IDD = IDD_INVOICE };
	CComboBox	m_cCurrency;
	CComboBox	m_cPayMode;
   CCustomerList m_ProductList;
   CString     m_strCustomerNo;
   CString     m_strCustomerName;
   CString     m_strCustomerCity;
   CString     m_strCustomerStreet;
   CString     m_strInvoiceDate;
   CString     m_strNN_Betrag;
   CString     m_strCustomerCompany;
   CString     m_strInvoiceTotal;
   CString     m_strInvoiceTotalNetto;
   CString     m_strInvoiceTotalNettoStr;
   CString     m_strInvoiceTotalStr;
   CString     m_strTax;
   CString     m_strTaxStr;
   CString     m_strInvoiceComment;
   int         m_nInvoiceFlag;
   CString	   m_strInvoiceNo;
	int		   m_nPayMod;
	BOOL	      m_bInvoicePaid;
	BOOL	      m_bInvoiceCanceled;
	CString     m_strBankKlAmount;
	float       m_fRabatt;
	float       m_fRabattAmount;
	//}}AFX_DATA

   float       m_fTax;
   float       m_fNNBetrag;
   CFacturaView *m_pParent;
private:
   int         m_nCurrentInvNo;
   int         m_nCustomerNo;
   int         m_nYear;
   int         m_nMonth;
   int         m_nDay;
   int         m_nInvoiceIndex;
   int         m_nCustomerIndex;
   int         m_nProductIndex;
   CString     m_strImportComment;
   CString     m_strImportAmount;

   int         m_nSpeedyPos;
   int         m_nSpeedyMode;
   bool        m_bSpeedySorted;

   CPtrList*   m_pCustomers;
   CPtrList*   m_pInvoices;
   CPtrList*   m_pProducts;
	CPtrList*   m_pPayModes;
   CPtrList*   m_pCurrUnits;
   CPtrList*   m_pPriceGroup;
   CPtrList    m_InvoiceProduct;
   CString     m_strCurrencyUnit;
   long        m_nFindCustInvoice;
   CTextEdit*  m_pComment;

   int         m_nRefCurrency;
   int         m_nBaseCurrency;
   int         m_nCurrency;
   bool        m_bConvert;

	int m_nMaskFlags;

// Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CInvoice)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
   //}}AFX_VIRTUAL

	static int InvoiceFlags(CListDlg *pDlg, int nReason);
	static int InvoiceSpeedy(CListDlg *pDlg, int nReason);
// Implementierung
protected:

   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(CInvoice)
   afx_msg void OnCustomerFind();
   virtual void OnCancel();
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   afx_msg void OnInvoiceDelete();
   afx_msg void OnInvoiceFind();
   afx_msg void OnInvoiceNew();
   afx_msg void OnInvoicePrintAdrKl();
   afx_msg void OnInvoicePrintNnzs();
   afx_msg void OnCustomerNew();
   afx_msg void OnInvoicePrint();
   afx_msg void OnChangeInvoice();
   afx_msg void OnDblclkProductList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnInvoicePrintAdrRgNo();
   afx_msg void OnKillfocusInvoiceNo();
   afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnGetdispinfoProductList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnEndlabeleditProductList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickProductList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkProductList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownProductList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomerSpeedy();
	afx_msg void OnDoubleclickedCustomerFind();
	afx_msg void OnCustomerNo();
	afx_msg void OnDeltaposInvoiceSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposCustomerSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInvoiceFindCustInvoice();
	afx_msg void OnSelchangeInvoiceCurrency();
	afx_msg void OnSelchangeInvoicePayMod();
	afx_msg void OnCustomerSpeedySave();
	afx_msg void OnInvoiceFlags();
	afx_msg void OnCustomerImport();
	afx_msg void OnInvoicePrintBankKl();
	afx_msg void OnKillfocusInvoiceRabatt();
	afx_msg void OnCustomerPrtLogfile();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnKillfocusInvoiceComment();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_INVOICE_H__06AF3E62_ABA4_11D2_8859_B236EB6AD456__INCLUDED_
