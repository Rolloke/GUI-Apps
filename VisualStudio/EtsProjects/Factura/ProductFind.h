#ifndef AFX_PRODUCTFIND_H__AF9901C0_AE2C_11D2_8859_E239100AA94A__INCLUDED_
#define AFX_PRODUCTFIND_H__AF9901C0_AE2C_11D2_8859_E239100AA94A__INCLUDED_

// ProductFind.h : Header-Datei
//
#include "CustomerList.h"
#define COLUMNS_PRODUCTFIND 4
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CProductFind 

class CFacturaDoc;
class CProductFind : public CDialog
{
// Konstruktion
public:
   CProductFind(CWnd* pParent = NULL);   // Standardkonstruktor
   void FindItem();

// Dialogfelddaten
   //{{AFX_DATA(CProductFind)
	enum { IDD = IDD_PRODUCT_FIND };
   CCustomerList  m_FindList;
   CString        m_strWhat;
   CString        m_strSearchFor;
	BOOL	m_bShowHidden;
	//}}AFX_DATA


// Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CProductFind)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
   //}}AFX_VIRTUAL

// Implementierung
protected:
	void InitProductList();

   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(CProductFind)
   afx_msg void OnColumnclickFindList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnChangeFindEdit();
   virtual BOOL OnInitDialog();
   virtual void OnCancel();
   virtual void OnOK();
	afx_msg void OnBeginlabeleditFindList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditFindList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboFindPrice();
	afx_msg void OnSelchangeComboFindProduct();
	afx_msg void OnProductPriceHidden();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
public:
   int m_nItem;
   int m_nPriceGroup;

private:
   CFacturaDoc *m_pDoc;
   int m_nColumn;
   int m_nSearchStrLen;
   int m_nProductGroup;
   static int gm_nWhat[COLUMNS_PRODUCTFIND];
   static int __stdcall CompareNo(LPARAM, LPARAM, LPARAM);
   static int __stdcall ComparePrice(LPARAM, LPARAM, LPARAM);
   static int __stdcall CompareCount(LPARAM, LPARAM, LPARAM);
   static int __stdcall CompareDescription(LPARAM, LPARAM, LPARAM);
   static int strcmpext(const char *str1, const char* str2);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_PRODUCTFIND_H__AF9901C0_AE2C_11D2_8859_E239100AA94A__INCLUDED_
