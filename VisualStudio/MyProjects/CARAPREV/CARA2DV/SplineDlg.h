#ifndef AFX_SPLINEDLG_H__35D25003_1578_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_SPLINEDLG_H__35D25003_1578_11D2_9DB9_0000B458D891__INCLUDED_

// SplineDlg.h : Header-Datei
//
#include "CurveDialog.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSplineDlg 

class CSplineDlg : public CCurveDialog
{
// Konstruktion
public:
	CSplineDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Statische Funktionen
private:
	static UINT ThreadFunction(void*);

// Dialogfelddaten
	//{{AFX_DATA(CSplineDlg)
	enum { IDD = IDD_SPLINECURVE };
	int	   m_nNumSteps;
	double	m_dRangeX1;
	double	m_dRangeX2;
	//}}AFX_DATA
	CString  m_strUnitx;
	double   m_dMinX;
	double   m_dMaxX;


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSplineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual void InitCurveParams();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSplineDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_SPLINEDLG_H__35D25003_1578_11D2_9DB9_0000B458D891__INCLUDED_
