#ifndef AFX_LIGHTDLG_H__1F2B2A06_170A_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_LIGHTDLG_H__1F2B2A06_170A_11D2_9DB9_0000B458D891__INCLUDED_

// LightDlg.h : Header-Datei
//
#include "NonModalDlg.h"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CLightDlg 
struct Cara3DLight;

class CLightDlg : public CNonModalDlg
{
// Konstruktion
public:
	static double GetOptDistance(double dMaxDim, double dSpotCutoff);
	CLightDlg(CWnd* pParent = NULL);   // Standardkonstruktor

 	void ReturnLightParam(Cara3DLight*);
	void ReturnAmbientLight(float*);

// Dialogfelddaten
	//{{AFX_DATA(CLightDlg)
	enum { IDD = IDD_LIGHT_DIALOG };
	CComboBox    m_cLightCombo;
	CSliderCtrl  m_cLightIntens;
   float        m_fLightDistance;
   float        m_fLightAnglePhi;
   float        m_fLightAngleTheta;
   float        m_fSpotCutOff;
   float        m_fSpotExponent;
   BOOL         m_bLightOn;
	//}}AFX_DATA
   float        m_fLightIntens;
   float        m_fMaxDim;
   Cara3DLight *m_pLights;
   int          m_nSelectedLight;
   float       *m_pfAmbient;
   COLORREF     m_cLightColor;
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CLightDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
	void UpdateLightCtrls();
	void SelectLight(int nLight);
protected:
	bool OnApplyNow();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CLightDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusLightDistance();
	afx_msg void OnLightdlgDefault();
	afx_msg void OnKillfocusSpotCutoff();
	afx_msg void OnSelchangeLightCombo();
	afx_msg void OnLightColor();
	afx_msg void OnApplyNowCmd();
	afx_msg void OnKillfocusLightPhi();
	afx_msg void OnKillfocusLightTheta();
	afx_msg void OnLightOn();
	afx_msg void OnReleasedcaptureLightIntens0(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_LIGHTDLG_H__1F2B2A06_170A_11D2_9DB9_0000B458D891__INCLUDED_
