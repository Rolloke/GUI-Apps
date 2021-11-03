#ifndef AFX_PASTESELECTIVDLG_H__CAC08F32_25F8_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_PASTESELECTIVDLG_H__CAC08F32_25F8_11D2_9DB9_0000B458D891__INCLUDED_

#include "resource.h"
// PasteSelectivDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPasteSelectivDlg 

class CPasteSelectivDlg : public CDialog
{
// Konstruktion
public:
	CPasteSelectivDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CPasteSelectivDlg)
	enum { IDD = IDD_PASTE_SELECTIV };
	int		m_nSelect;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPasteSelectivDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(CPasteSelectivDlg)
   virtual BOOL OnInitDialog();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	virtual void OnOK();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_PASTESELECTIVDLG_H__CAC08F32_25F8_11D2_9DB9_0000B458D891__INCLUDED_
