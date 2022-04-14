#if !defined(AFX_REPORTDLG_H__EDB829C0_86D2_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_REPORTDLG_H__EDB829C0_86D2_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CReportDlg 

class CReportDlg : public CDialog
{
// Konstruktion
public:
	~CReportDlg();
	CReportDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CReportDlg)
	enum { IDD = IDD_REPORT_DLG };
	BOOL	m_bReportCarawalk;
	BOOL	m_bReportEts2DView;
	BOOL	m_bReportEts3DGL;
	BOOL	m_bReportEts3DView;
	BOOL	m_bReportEtsPrtD;
	BOOL	m_bReportCaraSdb;
	BOOL	m_bReportCaraBox;
	BOOL	m_bReportETSCDPL;
	BOOL	m_bReportAC_Com;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CReportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CReportDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_REPORTDLG_H__EDB829C0_86D2_11D3_B6EC_0000B458D891__INCLUDED_
