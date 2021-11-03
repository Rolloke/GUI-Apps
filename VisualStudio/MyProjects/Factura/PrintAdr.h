#ifndef AFX_PRINTADR_H__22F956E4_B438_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_PRINTADR_H__22F956E4_B438_11D2_9DB9_0000B458D891__INCLUDED_

// PrintAdr.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPrintAdr 

class CPrintAdr : public CDialog
{
// Konstruktion
public:
   CPrintAdr(CWnd* pParent = NULL);   // Standardkonstruktor
   virtual ~CPrintAdr();

// Dialogfelddaten
   //{{AFX_DATA(CPrintAdr)
	enum { IDD = IDD_PRINT_ADR_KL };
   int	  m_nFrom;
   int	  m_nTo;
   BOOL  m_bHiak;
   BOOL  m_bCale;
   BOOL  m_bServiceCard;
	BOOL	m_bPrintNameOfAdress;
	//}}AFX_DATA
   int   m_nMax;

// Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CPrintAdr)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
   //}}AFX_VIRTUAL

// Implementierung
protected:

   // Generierte Nachrichtenzuordnungsfunktionen
   //{{AFX_MSG(CPrintAdr)
   afx_msg void OnPrintAdrSelect();
   virtual void OnCancel();
   virtual void OnOK();
   virtual BOOL OnInitDialog();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   CPtrList *m_pCustomers;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_PRINTADR_H__22F956E4_B438_11D2_9DB9_0000B458D891__INCLUDED_
