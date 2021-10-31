#if !defined(AFX_FILTERPROPERTIES_H__EDF7AC81_C417_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_FILTERPROPERTIES_H__EDF7AC81_C417_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterProperties.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CFilterProperties 
#define FILTER_TYPE_LOWPASS   0
#define FILTER_TYPE_HIGHPASS  1
#define FILTER_TYPE_BANDPASS  2
#define FILTER_TYPE_PINKNOISE 3

#define FILTER_CHA_BUTTERWORT     0
#define FILTER_CHA_BUTTERWORT_Q   1
#define FILTER_CHA_BESSEL         2
#define FILTER_CHA_TSCHEBYSCHEFF  3
#define FILTER_CHA_LINKWITZ_RILEY 4

class CFilterProperties : public CDialog
{
// Konstruktion
public:
	CFilterProperties(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CFilterProperties)
	enum { IDD = IDD_FILTER_PROPERTIES };
	CSpinButtonCtrl	m_cOrderTPSpin;
	CSpinButtonCtrl	m_cOrderHPSpin;
	int               m_nOrderTP;
	int               m_nOrderHP;
	int               m_nFilterCharacterTP;
	int               m_nFilterCharacterHP;
	double            m_dFrequencyTP;
	int               m_nFilterType;
	double            m_dFrequencyHP;
	double            m_dQFactorTP;
	double            m_dQFactorHP;
	double	         m_dFilterM;
	double	         m_dBandwidth;
	//}}AFX_DATA

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFilterProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
   bool     m_bBand;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CFilterProperties)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFilterType();
	afx_msg void OnSelchangeFilterCharacteristicTP();
	afx_msg void OnSelchangeFilterCharacteristicHP();
	afx_msg void OnKillfocusFilterBand();
	afx_msg void OnKillfocusFilterFrqHP();
	afx_msg void OnKillfocusFilterFrqTP();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_FILTERPROPERTIES_H__EDF7AC81_C417_11D3_B6EC_0000B458D891__INCLUDED_
