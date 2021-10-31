#ifndef AFX_DERIVE_H__EA0688E3_A8A6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_DERIVE_H__EA0688E3_A8A6_11D2_9DB9_0000B458D891__INCLUDED_

// Derive.h : Header-Datei
//
#include "CurveDialog.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDerive 

class CDerive : public CCurveDialog
{
// Konstruktion
public:
	CDerive(CWnd* pParent = NULL);   // Standardkonstruktor

// Statische Funktionen
private:
	static UINT ThreadFunction(void*);

// Dialogfelddaten
	//{{AFX_DATA(CDerive)
	enum { IDD = IDD_DERIVE };
	//}}AFX_DATA

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDerive)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual void InitCurveParams();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDerive)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.
#endif // AFX_DERIVE_H__EA0688E3_A8A6_11D2_9DB9_0000B458D891__INCLUDED_
