#if !defined(AFX_BASICPARAMDLG_H__4EA6CCB1_EAC8_11D4_87C5_0000B48B0FC3__INCLUDED_)
#define AFX_BASICPARAMDLG_H__4EA6CCB1_EAC8_11D4_87C5_0000B48B0FC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasicParamDlg.h : Header-Datei
//

#include "Plot.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CBasicParamDlg 

class CBasicParamDlg : public CDialog
{
// Konstruktion
public:
	CBasicParamDlg(CWnd* pParent = NULL);   // Standardkonstruktor
	static void Swap(double&, double&);

// Dialogfelddaten
	//{{AFX_DATA(CBasicParamDlg)
	enum { IDD = IDD_BASIC_PARAM };
	int      m_nDivision;
	int      m_nPollMeasurementTime;
	int      m_nPortTrials;
	BOOL     m_bTrack10_ACD3;
	BOOL     m_bTrack9_ACD2;
	long	m_nEventTime;
	//}}AFX_DATA

   RANGE m_Range1;
   RANGE m_Range2;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CBasicParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CBasicParamDlg)
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg void OnBsBtnDefault();
	virtual void OnOK();
	afx_msg void OnBsCkTrack9TestCD();
	afx_msg void OnBsCkTrack10TestCD();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_BASICPARAMDLG_H__4EA6CCB1_EAC8_11D4_87C5_0000B48B0FC3__INCLUDED_
