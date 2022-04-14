#ifndef AFX_INVOICEFIND_H__836BF004_B05A_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_INVOICEFIND_H__836BF004_B05A_11D2_9DB9_0000B458D891__INCLUDED_

// InvoiceFind.h : Header-Datei
//
#include "CustomerList.h"

#define COLUMNS_INVOICEFIND 10
class Customer;
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CInvoiceFind 
struct InvoiceFind;
class CInvoiceFind : public CDialog
{
// Konstruktion
public:
   CInvoiceFind(CWnd* pParent = NULL);   // Standardkonstruktor
   virtual ~CInvoiceFind();              // Destruktor
   void FindItem();
   Customer * GetCustomerFromNo(CPtrList*, int);

// Dialogfelddaten
   //{{AFX_DATA(CInvoiceFind)
   enum { IDD = IDD_INVOICE_FIND };
   CCustomerList  m_FindList;
   CString	m_strSearchFor;
   CString	m_strWhat;
   //}}AFX_DATA


// Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CInvoiceFind)
	protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementierung
protected:

   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(CInvoiceFind)
   afx_msg void OnChangeFindEdit();
   afx_msg void OnColumnclickFindList(NMHDR* pNMHDR, LRESULT* pResult);
   virtual void OnOK();
   virtual void OnCancel();
   virtual BOOL OnInitDialog();
	afx_msg void OnGetdispinfoFindList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFindFullText();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg void OnRclickFindList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
   LRESULT OnUserMsg(WPARAM, LPARAM);
   DECLARE_MESSAGE_MAP()
public:
   int          m_nItem;
   bool         m_bExternalSearch;
   int          m_nColumn;
   int          m_nMaskFlags;
private:
   int          m_nSearchStrLen;
   int          m_nCount;
   int          m_nFullTextItem;
   InvoiceFind* m_pList;

   static int  gm_nWhat[COLUMNS_INVOICEFIND];
   static long gm_lOldFindEditWndProc;
   static int  gm_nWidth[COLUMNS_INVOICEFIND];
   static int  gm_nOrder[COLUMNS_INVOICEFIND];

   static LRESULT CALLBACK FindEditSubClassProc(HWND, UINT, WPARAM, LPARAM);

   static int __cdecl CompareInvNo(const void*, const void*);
   static int __cdecl CompareCustNo(const void*, const void*);
   static int __cdecl CompareCustName(const void*, const void*);
   static int __cdecl CompareCustCompany(const void*, const void*);
   static int __cdecl CompareCustFirstName(const void*, const void*);
   static int __cdecl CompareCustCity(const void*, const void*);
   static int __cdecl CompareCustPC(const void*, const void*);
   static int __cdecl CompareCustStreet(const void*, const void*);
   static int __cdecl CompareCustEmail(const void*, const void*);
   static int __cdecl CompareInvDate(const void*, const void*);
   static int strcmpext(const char *str1, const char* str2);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_INVOICEFIND_H__836BF004_B05A_11D2_9DB9_0000B458D891__INCLUDED_
