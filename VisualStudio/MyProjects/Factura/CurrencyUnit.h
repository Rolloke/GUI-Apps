#if !defined(AFX_CURRENCYUNIT_H__76FCF540_DF0B_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_CURRENCYUNIT_H__76FCF540_DF0B_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CurrencyUnit.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCurrencyUnit 

class CCurrencyUnit : public CDialog
{
// Konstruktion
public:
	CCurrencyUnit(CWnd* pParent = NULL);   // Standardkonstruktor
	bool SaveChangedData(bool bAsk=true);
	bool SetStateString();
	void InitData();

// Dialogfelddaten
	//{{AFX_DATA(CCurrencyUnit)
	enum { IDD = IDD_CURRENCY_UNIT };
	CComboBox	m_cUnit;
	CString	   m_strUnit;
	CString	   m_strUnitSign;
	CString	   m_strStateSign;
	float	      m_fCurrency;
	CString	   m_strState;
	CString	   m_strReference;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCurrencyUnit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCurrencyUnit)
	afx_msg void OnChangeValues();
	afx_msg void OnSelchangeCuUnit();
	virtual BOOL OnInitDialog();
	afx_msg void OnCuDelete();
	afx_msg void OnCuNew();
	afx_msg void OnCuSave();
	afx_msg void OnKillfocusCuStatesign();
	virtual void OnCancel();
	virtual void OnOK();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int       m_nIndex;
   CPtrList *m_pCurrencyUnit;
   CPtrList *m_pStates;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CURRENCYUNIT_H__76FCF540_DF0B_11D3_B6EC_0000B458D891__INCLUDED_
