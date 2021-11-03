#ifndef AFX_INTEGRATE_H__EA0688E2_A8A6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_INTEGRATE_H__EA0688E2_A8A6_11D2_9DB9_0000B458D891__INCLUDED_

// Integrate.h : Header-Datei
//
#include "CurveDialog.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CIntegrate 

class CIntegrate : public CCurveDialog
{
// Konstruktion
public:
	CIntegrate(CWnd* pParent = NULL);   // Standardkonstruktor

   // Statische Funktionen
private:
	static UINT ThreadFunction(void*);

// Dialogfelddaten
	//{{AFX_DATA(CIntegrate)
	enum { IDD = IDD_INTEGRATE };
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CIntegrate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual void InitCurveParams();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CIntegrate)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_INTEGRATE_H__EA0688E2_A8A6_11D2_9DB9_0000B458D891__INCLUDED_
