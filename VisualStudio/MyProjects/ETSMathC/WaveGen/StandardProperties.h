#if !defined(AFX_STANDARDPROPERTIES_H__EDF7AC83_C417_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_STANDARDPROPERTIES_H__EDF7AC83_C417_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StandardProperties.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld StandardProperties 

class StandardProperties : public CDialog
{
// Konstruktion
public:
	StandardProperties(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(StandardProperties)
	enum { IDD = IDD_STANDARD_PROPERTIES };
	CSpinButtonCtrl	m_cOrderSpin;
	int		m_nNumVal;
	int		m_nOrder;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(StandardProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(StandardProperties)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeOrder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_STANDARDPROPERTIES_H__EDF7AC83_C417_11D3_B6EC_0000B458D891__INCLUDED_
