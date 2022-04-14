#if !defined(AFX_PRINTBANKKL_H__306DB4C0_E6B9_11D2_9E4E_0000B458D891__INCLUDED_)
#define AFX_PRINTBANKKL_H__306DB4C0_E6B9_11D2_9E4E_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintBankKl.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPrintBankKl 

class CPrintBankKl : public CDialog
{
// Konstruktion
public:
	void PrintBankKl();
	virtual ~CPrintBankKl();
	CPrintBankKl(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CPrintBankKl)
	enum { IDD = IDD_PRINT_BANK_KL };
	CString	m_strAmount;
	CString	m_strBankName;
	CString	m_strBlz;
	CString	m_strCurrency;
	CString	m_strKtoNo;
	int		m_nCount;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPrintBankKl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPrintBankKl)
	virtual void OnOK();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PRINTBANKKL_H__306DB4C0_E6B9_11D2_9E4E_0000B458D891__INCLUDED_
