#if !defined(AFX_PATHDLG_H__EDB829C1_86D2_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_PATHDLG_H__EDB829C1_86D2_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PathDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPathDlg 

class CPathDlg : public CDialog
{
// Konstruktion
public:
	~CPathDlg();
	CPathDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CPathDlg)
	enum { IDD = IDD_PATH_DLG };
	CString	m_strDllPath;
	int		m_nPathSelection;
	BOOL	m_bUseDebugDll;
	BOOL	m_bUseLocalDll;
	//}}AFX_DATA
   bool m_bPathChanged;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPathDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	void SavePathInRegistry();
	HICON m_hIcon;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPathDlg)
	virtual void OnOK();
	afx_msg void OnSearchPath();
	virtual BOOL OnInitDialog();
	afx_msg void OnRPath();
	afx_msg void OnChangeDllPath();
	afx_msg void OnCkDebug();
	afx_msg void OnCkNoDllpath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PATHDLG_H__EDB829C1_86D2_11D3_B6EC_0000B458D891__INCLUDED_
