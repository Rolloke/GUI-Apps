#if !defined(AFX_EDITFORMDLG_H__BF4629A0_DA58_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_EDITFORMDLG_H__BF4629A0_DA58_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditFormDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEditFormDlg 
class CFacturaView;

class CEditFormDlg : public CDialog
{
// Konstruktion
public:
	static int EnumFormLabel(CLabel*, void*);
	CEditFormDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CEditFormDlg)
	enum { IDD = IDD_EDIT_FORM };
	CComboBox	m_cCombo;
	CString	   m_strCombo;
	//}}AFX_DATA
   CFacturaView *m_pParent;
   int           m_nPosition;
   int           m_nForms;
   int           m_bNoNewForms;
	CString	     m_strCaption;
   CPtrList     *m_pInvoices;
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEditFormDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
   bool m_bTextChanged;
   static LRESULT CALLBACK SubClass(HWND, UINT, WPARAM, LPARAM);
   static LRESULT (CALLBACK *gm_pOldWF)(HWND, UINT, WPARAM, LPARAM);

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CEditFormDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnEfNew();
	afx_msg void OnSelchangeEfCombo();
	afx_msg void OnKillfocusEfCombo();
	afx_msg void OnEditchangeEfCombo();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_EDITFORMDLG_H__BF4629A0_DA58_11D3_B6EC_0000B458D891__INCLUDED_
