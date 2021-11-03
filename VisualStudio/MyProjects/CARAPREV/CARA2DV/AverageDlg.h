#ifndef AFX_AVERAGEDLG_H__35D25002_1578_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_AVERAGEDLG_H__35D25002_1578_11D2_9DB9_0000B458D891__INCLUDED_

// AverageDlg.h : Header-Datei
//
#include "CurveDialog.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CAverageDlg 

class CAverageDlg : public CCurveDialog
{
// Konstruktion
public:
	CAverageDlg(CWnd* pParent = NULL);   // Standardkonstruktor
 
// Statische Funktionen
private:
	static UINT ThreadFunction(void*);

// Dialogfelddaten
	//{{AFX_DATA(CAverageDlg)
	enum { IDD = IDD_AVERAGE };
	CSliderCtrl	m_CAverageDlgWhere;
	double	   m_dAverageRange;
	int		   m_nNoOfSteps;
	int		   m_nStepIndexFrq;
	int		   m_nDecade_Divider;
	int		   m_nDivision;
	//}}AFX_DATA

   CString	   m_strUnitx;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CAverageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual void InitCurveParams();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CAverageDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickDivsion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_AVERAGEDLG_H__35D25002_1578_11D2_9DB9_0000B458D891__INCLUDED_
