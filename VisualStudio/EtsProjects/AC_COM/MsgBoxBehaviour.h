#if !defined(AFX_MSGBOXBEHAVIOUR_H__DAE3F97C_F267_480C_AE69_3F06A92E740F__INCLUDED_)
#define AFX_MSGBOXBEHAVIOUR_H__DAE3F97C_F267_480C_AE69_3F06A92E740F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgBoxBehaviour.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CMsgBoxBehaviour 

class CMsgBoxBehaviour : public CDialog
{
// Konstruktion
public:
	void SetFlags();
	CMsgBoxBehaviour(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CMsgBoxBehaviour();                      // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CMsgBoxBehaviour)
	enum { IDD = IDD_MSG_BOX_BEHAVIOUR };
	int   m_nEvent;
	BOOL  m_bAuto;
	BOOL  m_bRequest;
	//}}AFX_DATA
   DWORD m_dwFlags;
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CMsgBoxBehaviour)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CMsgBoxBehaviour)
	afx_msg LRESULT OnExitSizeMove(WPARAM, LPARAM);
	afx_msg void OnMbbCkRequest();
	afx_msg void OnMbbCkAuto();
	afx_msg void OnSelchangeMbbCombo();
	afx_msg void OnMbbBtnDefault();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_MSGBOXBEHAVIOUR_H__DAE3F97C_F267_480C_AE69_3F06A92E740F__INCLUDED_
