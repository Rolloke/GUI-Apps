#if !defined(AFX_REGISTERDLG_H__EABB5DE4_6BE7_4ACF_8A41_629F11802A7B__INCLUDED_)
#define AFX_REGISTERDLG_H__EABB5DE4_6BE7_4ACF_8A41_629F11802A7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegisterDlg.h : Header-Datei
//
#ifdef _REGISTER_BUILD

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CRegisterDlg 

class CRegisterDlg : public CDialog
{
// Konstruktion
public:
	int m_nNum;
	bool IsRegistered();
	CRegisterDlg(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CRegisterDlg(); 

// Dialogfelddaten
	//{{AFX_DATA(CRegisterDlg)
	enum { IDD = IDD_REGISTERED };
	BOOL	m_bRegister;
	CString	m_strName;
	CString	m_strStreet;
	CString	m_strCompany;
	CString	m_strCity;
	CString	m_strRegNo;
	//}}AFX_DATA

	CString	m_strComputer;
	CString	m_strUser;
   CTime    m_FirstTime;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CRegisterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CRegisterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	afx_msg void OnRegBtnRegister();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
   HFONT m_hTitleFont;
   HFONT m_hPleaseFont;
};

#endif // _REGISTER_BUILD

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_REGISTERDLG_H__EABB5DE4_6BE7_4ACF_8A41_629F11802A7B__INCLUDED_
