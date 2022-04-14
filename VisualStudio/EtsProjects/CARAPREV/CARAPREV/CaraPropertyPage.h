#if !defined(AFX_CARAPROPERTYPAGE_H__ECF19B41_3DDD_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_CARAPROPERTYPAGE_H__ECF19B41_3DDD_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaraPropertyPage.h : Header-Datei
//
#define CHANGE_PAGE        0x00000001
#define CHANGES_SAVED      0x00000002
#define CHANGES_REJECTED   0x00000004

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCaraPropertyPage 

class CCaraPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCaraPropertyPage)

// Konstruktion
public:
	CCaraPropertyPage();
	CCaraPropertyPage(UINT);
// Destruktion
	virtual ~CCaraPropertyPage();

// Memberfunktionen
	COLORREF GetPreviewBkGndColor();
	BOOL     RequestSaveChanges(bool);
//   BOOL     DDX_TextVar(CDataExchange*, int, int, double&, bool bThrow=true);
	virtual  BOOL UpdateData(BOOL);

// Dialogfelddaten
	//{{AFX_DATA(CCaraPropertyPage)
		// HINWEIS - Der Klassen-Assistent fügt hier Datenelemente ein.
		//    Innerhalb dieser generierten Quellcodeabschnitte NICHTS BEARBEITEN!
	//}}AFX_DATA


// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCaraPropertyPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL


// Implementierung
protected:
   bool m_bValuesChanged;
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCaraPropertyPage)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CARAPROPERTYPAGE_H__ECF19B41_3DDD_11D3_B6EC_0000B458D891__INCLUDED_
