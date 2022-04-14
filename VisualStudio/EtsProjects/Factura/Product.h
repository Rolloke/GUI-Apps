#ifndef AFX_PRODUCT_H__53DB9F44_AC57_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_PRODUCT_H__53DB9F44_AC57_11D2_9DB9_0000B458D891__INCLUDED_

// Product.h : Header-Datei
//
#include "CustomerList.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CProduct 
class Product;
class ProductOrPriceGroup;

class CProduct : public CDialog
{
// Konstruktion
public:
   CProduct(CWnd* pParent = NULL);   // Standardkonstruktor
   Product * GetProduct();
   void SaveChangedData();
   void InitProductData();
   void SetDocChanged();
   bool GetProductData();
   void SaveProduct();

// Dialogfelddaten
   //{{AFX_DATA(CProduct)
	enum { IDD = IDD_PRODUCT };
	CComboBox	m_cCurrency;
   int      m_nProductCount;
   float    m_fProductPrice;
   int      m_nNo;
   CString  m_strProductDescription;
	int		m_nProductGroup;
	int		m_nPriceGroup;
	int		m_nReleaseL;
	int		m_nReleaseH;
	int		m_nCompanyCode;
	BOOL	m_bPriceGroupVisible;
	BOOL	m_bProductHidden;
	//}}AFX_DATA
   // Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CProduct)
	protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementierung
public:
	void SetPriceGroup();
	static ProductOrPriceGroup* FindPOPGroup(CPtrList*, int, int &);
	void CloseGroupEdtCtrl();

protected:

   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(CProduct)
   virtual void OnCancel();
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   afx_msg void OnProductDelete();
   afx_msg void OnProductNew();
   afx_msg void OnProductFind();
   afx_msg void OnChangeProduct();
   afx_msg void OnChangeEAN();
   afx_msg void OnChangeProductPrice();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEditchangeComboGroup();
	afx_msg void OnSelchangeComboProductGroup();
	afx_msg void OnSelchangeComboProductPriceGroup();
	afx_msg void OnSelchangeComboProductCurrency();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
	CString	m_strGroupEdt;
   DWORD m_dwGroupFlags;
   int   m_nProductIndex;
	int   m_nPriceGroupSel;
   bool  m_bPriceChanged;
   bool  m_bCurrencyChanged;
   int   m_nBaseCurrency;
   CPtrList *m_pProducts;
   CPtrList *m_pPriceGroup;
   CPtrList *m_pProductGroup;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_PRODUCT_H__53DB9F44_AC57_11D2_9DB9_0000B458D891__INCLUDED_
