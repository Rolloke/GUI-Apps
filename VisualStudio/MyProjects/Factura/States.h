#ifndef AFX_STATES_H__DF52C342_B10D_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_STATES_H__DF52C342_B10D_11D2_9DB9_0000B458D891__INCLUDED_

// States.h : Header-Datei
//
#include "CustomerList.h"

#define  COLUMNS_STATES 2

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CStates 

class CStates : public CDialog
{
// Konstruktion
public:
	void SetDocChanged();
   CStates(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
   //{{AFX_DATA(CStates)
	enum { IDD = IDD_STATE };
   CCustomerList	m_FindList;
	CString	m_strStateID;
	CString	m_strStateName;
	//}}AFX_DATA


// Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CStates)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
   //}}AFX_VIRTUAL

// Implementierung
protected:

   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(CStates)
   virtual void OnCancel();
   virtual void OnOK();
	afx_msg void OnBeginlabeleditFindList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditFindList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnStateNew();
	afx_msg void OnStateDelete();
	afx_msg void OnStateInsert();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
private:
   static int gm_nWhat[COLUMNS_STATES];
   CPtrList * m_pList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_STATES_H__DF52C342_B10D_11D2_9DB9_0000B458D891__INCLUDED_
