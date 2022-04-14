#ifndef AFX_EDITDOUBLECTRL_H__59BEC5A2_BC3C_11D2_8859_E4D22B16A728__INCLUDED_
#define AFX_EDITDOUBLECTRL_H__59BEC5A2_BC3C_11D2_8859_E4D22B16A728__INCLUDED_

#include "Calcmplx.h"
// EditDoubleCtrl.h : Header-Datei
//
/////////////////////////////////////////////////////////////////////////////
// Fenster CEditDoubleCtrl 

class CEditDoubleCtrl : public CListCtrl
{
// Konstruktion
public:
	CEditDoubleCtrl();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CEditDoubleCtrl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementierung
public:
	Complex m_cValue;
	virtual ~CEditDoubleCtrl();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CEditDoubleCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_EDITDOUBLECTRL_H__59BEC5A2_BC3C_11D2_8859_E4D22B16A728__INCLUDED_
