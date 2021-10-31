#if !defined(AFX_PROGRESSDLG_H__8B428D21_C526_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_PROGRESSDLG_H__8B428D21_C526_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDlg.h : Header-Datei
//
#include "resource.h"       // Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CProgressDlg 

class CProgressDlg : public CDialog
{
// Konstruktion
public:
	void Step();
	void SetRange(int nR);
	CProgressDlg(CWnd* pParent = NULL);   // Standardkonstruktor
	~CProgressDlg();   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_PROGRESS_DLG };
	CProgressCtrl	m_cProgress;
	//}}AFX_DATA
   int m_nPos;
   int m_nStep;
   int m_nStepCount;
   CRITICAL_SECTION m_csProgress;   

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CProgressDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PROGRESSDLG_H__8B428D21_C526_11D3_B6EC_0000B458D891__INCLUDED_
