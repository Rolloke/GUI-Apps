// PrintAscDlg.h : Header-Datei
//

#if !defined(AFX_PRINTASCDLG_H__CAC08F26_25F8_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_PRINTASCDLG_H__CAC08F26_25F8_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CPrintAscDlg Dialogfeld

class CPrintAscDlg : public CDialog
{
// Konstruktion
public:
	CPrintAscDlg(CWnd* pParent = NULL);	// Standard-Konstruktor
	~CPrintAscDlg();                   	// Standard-Destruktor

// Dialogfelddaten
	//{{AFX_DATA(CPrintAscDlg)
	enum { IDD = IDD_PRINTASC_DIALOG };
	CComboBox	m_cComboPrinter;
	int		m_nTabLength;
	BOOL	m_bPrintClipboard;
	BOOL	m_bEtsHelpFormat;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPrintAscDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
public:
   char * GetPortName();
   void static  GetDefaultPrinter(PRINTER_INFO_2 **);
   CString      m_SelectPrinterName;

protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CPrintAscDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetup();
	afx_msg void OnSelchangePrinter();
	virtual void OnOK();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
   void        ChangeSetup();
   void        UpdateParams();
   static int  Rounding(double);
   static void GetDefaultParams(char *,PRINTER_INFO_2 **);
   PRINTER_INFO_2*  m_pActualInfo;
   int              m_nOldSelect;

   static DWORD gm_HelpArray[];
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_PRINTASCDLG_H__CAC08F26_25F8_11D2_9DB9_0000B458D891__INCLUDED_)
