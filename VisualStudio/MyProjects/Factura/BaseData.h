#if !defined(AFX_BASEDATA_H__9F4A4EA1_DFB8_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_BASEDATA_H__9F4A4EA1_DFB8_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseData.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld BaseData 

class BaseData : public CDialog
{
// Konstruktion
public:
	BaseData(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(BaseData)
	enum { IDD = IDD_BASE_SETTINGS };
	float	m_fNNAmount;
	float	m_fTax;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(BaseData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(BaseData)
	virtual void OnOK();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_BASEDATA_H__9F4A4EA1_DFB8_11D3_B6EC_0000B458D891__INCLUDED_
