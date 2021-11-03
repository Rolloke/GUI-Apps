#if !defined(AFX_ERRORMESSAGEDLG_H__1EB5CD62_6F68_11D4_B6EC_0000B458D891__INCLUDED_)
#define AFX_ERRORMESSAGEDLG_H__1EB5CD62_6F68_11D4_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ErrorMessageDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CErrorMessageDlg 
#ifdef _DEBUG

class CErrorMessageDlg : public CDialog
{
// Konstruktion
public:
	void GetErrorNumber();
	~CErrorMessageDlg();
	void GetErrorMessage();
	CErrorMessageDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CErrorMessageDlg)
	enum { IDD = IDD_ERROR_MESSAGE };
	DWORD	m_nErrorMessage;
	CString	m_strErrorMessage;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CErrorMessageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CErrorMessageDlg)
	afx_msg void OnKillfocusEdtErrorNumber();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _DEBUG

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_ERRORMESSAGEDLG_H__1EB5CD62_6F68_11D4_B6EC_0000B458D891__INCLUDED_

