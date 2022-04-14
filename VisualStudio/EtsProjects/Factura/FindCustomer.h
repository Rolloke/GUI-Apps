#ifndef AFX_FINDCUSTOMER_H__FA828F12_A94A_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_FINDCUSTOMER_H__FA828F12_A94A_11D2_9DB9_0000B458D891__INCLUDED_

// FindCustomer.h : Header-Datei
//
#include "CustomerList.h"
#define  COLUMNS_CUSTOMERFIND 8
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFindCustomer 

class CFindCustomer : public CDialog
{
// Konstruktion
public:
   CFindCustomer(CWnd* pParent = NULL);   // Standardkonstruktor
   virtual ~CFindCustomer();              // Destruktor

// Dialogfelddaten
   //{{AFX_DATA(CFindCustomer)
   enum { IDD = IDD_CUSTOMER_FIND };
   CCustomerList  m_FindList;
   CString	    m_strWhat;
   CString	    m_strSearchFor;
   //}}AFX_DATA


// Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CFindCustomer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
   //}}AFX_VIRTUAL

// Implementierung
protected:

   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(CFindCustomer)
   virtual void OnCancel();
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   afx_msg void OnColumnclickFindList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnChangeFindEdit();
   afx_msg void OnGetdispinfoFindList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFindFullText();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
   LRESULT OnUserMsg(WPARAM, LPARAM);
   DECLARE_MESSAGE_MAP()

public:
   Customer ** GetCustomerList();
   int       m_nItem;
   int       m_nMultiSelect;
   int       m_nMaskFlags;

   bool      m_bExternalSearch;
   int       m_nColumn;
   int       m_nSearchStrLen;
   int       m_nFullTextItem;

   static int __cdecl CompareNo(const void*, const void*);
   static int __cdecl CompareName(const void*, const void*);
   static int __cdecl CompareFirstName(const void*, const void*);
   static int __cdecl CompareCity(const void*, const void*);
   static int __cdecl ComparePostalCode(const void*, const void*);
   static int __cdecl CompareStreet(const void*, const void*);
   static int __cdecl CompareCompany(const void*, const void*);
   static int __cdecl CompareEmail(const void*, const void*);
   static int strcmpext(const char *str1, const char* str2);

private:
   int       m_nCount;
   Customer**m_ppList;

   static int  gm_nWhat[COLUMNS_CUSTOMERFIND];
   static long gm_lOldFindEditWndProc;
   static LRESULT CALLBACK FindEditSubClassProc(HWND, UINT, WPARAM, LPARAM);
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_FINDCUSTOMER_H__FA828F12_A94A_11D2_9DB9_0000B458D891__INCLUDED_
