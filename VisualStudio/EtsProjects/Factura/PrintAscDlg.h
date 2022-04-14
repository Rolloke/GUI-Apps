#ifndef AFX_PRINTASCDLG_H__B9D543E2_AF7D_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_PRINTASCDLG_H__B9D543E2_AF7D_11D2_9DB9_0000B458D891__INCLUDED_

// PrintAscDlg.h : Header-Datei
//
#include <winspool.h>

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPrintAscDlg 

class CPrintAscDlg : public CDialog
{
// Konstruktion
public:
   CPrintAscDlg(CWnd* pParent = NULL);   // Standardkonstruktor
   virtual ~CPrintAscDlg();                   	// Standard-Destruktor

// Dialogfelddaten
   //{{AFX_DATA(CPrintAscDlg)
	enum { IDD = IDD_PRINTASC_DIALOG };
   CComboBox	m_cComboPrinter;
	BOOL	m_bConvertFont;
	BOOL	m_bFormFeed;
	//}}AFX_DATA


// Überschreibungen
   // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
   //{{AFX_VIRTUAL(CPrintAscDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
   //}}AFX_VIRTUAL

// Implementierung
public:
	DWORD GetPrinterAttributes();
   char * GetPortName();
   void static  GetDefaultPrinter(PRINTER_INFO_2 **);
   CString      m_SelectPrinterName;
   CString      m_strDlgHeading;

protected:
	HICON m_hIcon;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPrintAscDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetup();
	afx_msg void OnSelchangePrinter();
	virtual void OnOK();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
   afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   void        ChangeSetup();
   void        UpdateParams();
   static int  Rounding(double);
   static void GetDefaultParams(char *,PRINTER_INFO_2 **);

   PRINTER_INFO_2*  m_pActualInfo;
   int              m_nOldSelect;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_PRINTASCDLG_H__B9D543E2_AF7D_11D2_9DB9_0000B458D891__INCLUDED_
