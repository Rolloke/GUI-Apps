//{{AFX_INCLUDES()
#include "animation.h"
#include "mci.h"
//}}AFX_INCLUDES
#if !defined(AFX_ANIMATIONDLG_H__A4310CA2_6C6D_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ANIMATIONDLG_H__A4310CA2_6C6D_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnimationDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CAnimationDlg 

#ifdef _DEBUG

class CAnimationDlg : public CDialog
{
// Konstruktion
public:
	CAnimationDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CAnimationDlg)
	enum { IDD = IDD_ANIMATION };
	CAnimation	m_cAnimationCtrl;
	Cmci	m_cMMControl;
	//}}AFX_DATA
   VARIANT m_varRpt;
   VARIANT m_varStart;
   VARIANT m_varStop;


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CAnimationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CAnimationDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPlayClickMmcontrol(short FAR* Cancel);
	afx_msg void OnStopClickMmcontrol(short FAR* Cancel);
	afx_msg void OnNextClickMmcontrol(short FAR* Cancel);
	afx_msg void OnPrevClickMmcontrol(short FAR* Cancel);
	afx_msg void OnBackClickMmcontrol(short FAR* Cancel);
	afx_msg void OnStepClickMmcontrol(short FAR* Cancel);
	afx_msg void OnPauseClickMmcontrol(short FAR* Cancel);
	afx_msg void OnEjectClickMmcontrol(short FAR* Cancel);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.
#endif // _DEBUG

#endif // AFX_ANIMATIONDLG_H__A4310CA2_6C6D_11D4_B6EC_0000B458D891__INCLUDED_
