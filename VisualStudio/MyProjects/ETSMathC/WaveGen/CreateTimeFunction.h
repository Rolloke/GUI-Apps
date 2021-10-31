#if !defined(AFX_CREATETIMEFUNCTION_H__B430D184_EA30_4B3E_B55E_3B63A47DA410__INCLUDED_)
#define AFX_CREATETIMEFUNCTION_H__B430D184_EA30_4B3E_B55E_3B63A47DA410__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreateTimeFunction.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCreateTimeFunction 

class CCreateTimeFunction : public CDialog
{
// Konstruktion
public:
	CCreateTimeFunction(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CCreateTimeFunction)
	enum { IDD = IDD_CREATE_TIME_FUNC };
	double	m_dFrq1;
	double	m_dFrq2;
	double	m_dT1;
	double	m_dT2;
	double	m_dTG1;
	double	m_dTG2;
	BOOL	   m_bSweep;
	int		m_nFkt;
	float	   m_fSweepLength;
	//}}AFX_DATA
	double	m_dMinFrq;
	double	m_dMaxFrq;
	double	m_dMinTG;
	double	m_dMaxTG;


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCreateTimeFunction)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	void CalcInverseValue(int, int, bool, double, double, double&, double&);

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCreateTimeFunction)
	virtual BOOL OnInitDialog();
	afx_msg void OnCfCkSweep();
	afx_msg void OnKillfocusCfEdtFrq1();
	afx_msg void OnKillfocusCfEdtFrq2();
	afx_msg void OnKillfocusCfEdtTg1();
	afx_msg void OnKillfocusCfEdtTg2();
	virtual void OnOK();
	afx_msg void OnKillfocusCfEdtT1();
	afx_msg void OnKillfocusCfEdtT2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_CREATETIMEFUNCTION_H__B430D184_EA30_4B3E_B55E_3B63A47DA410__INCLUDED_
