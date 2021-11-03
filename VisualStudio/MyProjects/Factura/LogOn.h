#if !defined(AFX_LOGON_H__22FD20A2_F39C_4F86_95FC_79FEE2918CBF__INCLUDED_)
#define AFX_LOGON_H__22FD20A2_F39C_4F86_95FC_79FEE2918CBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogOn.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CLogOn 

class CLogOn : public CDialog
{
// Konstruktion
public:
	CLogOn(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CLogOn)
	enum { IDD = IDD_LOG_ON };
	BOOL	m_bSavePassword;
	CString	m_strPassWord;
	CString	m_strProfile;
	BOOL	m_bNewSession;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CLogOn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CLogOn)
	afx_msg void OnLogonSave();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_LOGON_H__22FD20A2_F39C_4F86_95FC_79FEE2918CBF__INCLUDED_
