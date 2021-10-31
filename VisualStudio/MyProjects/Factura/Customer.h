#ifndef AFX_CUSTOMER_H__FA828F0F_A94A_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_CUSTOMER_H__FA828F0F_A94A_11D2_9DB9_0000B458D891__INCLUDED_

// Customer.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCustomer 
class Customer;
class CListDlg;

class CCustomer : public CDialog
{
// Konstruktion
public:
   CCustomer(CWnd* pParent = NULL);   // Standardkonstruktor
   Customer * GetCustomer();
   void SaveChangedData();
   void InitCustomerData();
   void SetDocChanged();
   bool GetCustomerData();
   bool SaveNewCustomer();
   void SaveCustomer(Customer*);

// Dialogfelddaten
   //{{AFX_DATA(CCustomer)
	enum { IDD = IDD_CUSTOMER };
   CString	m_strCity;
   CString	m_strEmail;
   CString	m_strFax;
   CString	m_strStreetNo;
   CString	m_strName;
   CString	m_strPhone;
   CString	m_strPostCode;
   CString	m_strFirstName;
   CString	m_strState;
   CString	m_strStreet;
   CString	m_strTitle;
   int		m_nNo;
   int      m_nCustomerSex;
   CString	m_strComment;
   CString	m_strCompany;
   CSpinButtonCtrl	m_CustomerSpin;
	//}}AFX_DATA

   CString   m_strCommentOfImport;
   CString   m_strAmountOfImport;
   int       m_nIndex;
   CPtrList *m_pCustomers;
   int       m_nCustomerFlags;
   bool      m_bChanged;
// Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CCustomer)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
   //}}AFX_VIRTUAL

// Implementierung
protected:

   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(CCustomer)
   afx_msg void OnCustomerFind();
   afx_msg void OnCustomerNew();
   virtual void OnCancel();
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   afx_msg void OnChangeCustomer();
   afx_msg void OnCustomerDelete();
   afx_msg void OnDeltaposCustomerSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditPaste();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnKillfocusCustomerPlz();
	afx_msg void OnCustomerSendemail();
	afx_msg void OnCustomerPrintAdrKl();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
public:
	afx_msg void OnCustomerFlags();
	afx_msg void OnCustomerImport();
   DECLARE_MESSAGE_MAP()
private:
	unsigned int m_nMaxLen;
	static int CustomerFlags(CListDlg *pDlg, int nReason);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CUSTOMER_H__FA828F0F_A94A_11D2_9DB9_0000B458D891__INCLUDED_
