#if !defined(AFX_SENDEMAIL_H__5762FA6F_4822_4C56_8C5A_C7A8D2F82942__INCLUDED_)
#define AFX_SENDEMAIL_H__5762FA6F_4822_4C56_8C5A_C7A8D2F82942__INCLUDED_

#include "LogOn.h"	// Hinzugefügt von der Klassenansicht
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendEmail.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSendEmail 

class CSendEmail : public CDialog
{
// Konstruktion
public:
	void UpdateOKBtn();
	void UpdateAttDeleteBtn();
	CSendEmail(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSendEmail)
	enum { IDD = IDD_SENDEMAIL };
	CComboBox	m_cAttachments;
	CString	m_strAdress;
	CString	m_strSubject;
	CString	m_strBody;
	CString	m_strAdressCopy;
	CString	m_strAdressBlindCopy;
	CString	m_strName;
	CString	m_strNameFrom;
	CString	m_strAdressFrom;
	//}}AFX_DATA

   int      m_nCurrAttachment;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSendEmail)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSendEmail)
	afx_msg void OnInsertAdress();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSendDirect();
	afx_msg void OnBtnDeleteAttachment();
	afx_msg void OnBtnInsertAttachment();
	afx_msg void OnSelchangeComboAtttachments();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeCopies();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditUserData();
	afx_msg void OnSaveUser();
	afx_msg void OnInsertAdress2();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CLogOn m_LogOn;
   int    m_nInsertTo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SENDEMAIL_H__5762FA6F_4822_4C56_8C5A_C7A8D2F82942__INCLUDED_
