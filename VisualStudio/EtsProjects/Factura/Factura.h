// Factura.h : Haupt-Header-Datei für die Anwendung FACTURA
//

#if !defined(AFX_FACTURA_H__FA828EE4_A94A_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_FACTURA_H__FA828EE4_A94A_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resourceF.h"       // Hauptsymbole
#include "CaraWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// CFacturaApp:
// Siehe Factura.cpp für die Implementierung dieser Klasse
//
#define NNZS_PRINTER    1
#define ADR_KL_PRINTER  2
#define INVOICE_PRINTER 3
#define LOGFILE_PRINTER 4

#define SIMPLE_PRINTER  0
#define NETWORK_PRINTER 1
#define WINDOW_HANDLE   2
#define DO_FORM_FEED    0x04

class Customer;

class CFacturaApp : public CCaraWinApp
{
public:
   CFacturaApp();
   virtual ~CFacturaApp();

// Überladungen
   // Vom Klassenassistenten generierte Überladungen virtueller Funktionen
   //{{AFX_VIRTUAL(CFacturaApp)
	public:
   virtual BOOL InitInstance();
   virtual int ExitInstance();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementierung

   //{{AFX_MSG(CFacturaApp)
   afx_msg void OnAppAbout();
   afx_msg void OnOptionsAdrKlPrinter();
   afx_msg void OnOptionsNnzsPrinter();
	afx_msg void OnExtraPrintLabel();
	afx_msg void OnOptionsLogfilePrinter();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()

public:
	CString m_strStartDir;
	bool         SetPrinterHandle(HWND);
	void         SetActualPrinter(int nNo);
	bool         RequestCancelPrinter();
   void         PrintAdrKl(Customer *, const char *pszInvoiceNo = NULL);
   int          StandardPrint(int nOffset, const char *str, int nSpace, int nLineFeed, int nlen);
   bool         GetStandardPrinter(int);
   bool         OpenPrinter();
   void         CloseStandardPrinter();
   static void  MakeFormatS2(char *format, char *str1, char *str2, int nMaxlen);
   static void  FormatConcat(char *format, const char *Concat, int nMax, int& nSum);
   int          PrintPort(char *pszText);

	BOOL     m_bPrinter;
   bool     m_bPrintNameOfAdress;
   bool     m_bPrintAdressWithInvNo;

   private:
   int      m_nVersion;
   HANDLE   m_hStdPrinter;
   OVERLAPPED m_OverLapped;
   int      m_nPrinterNo;
   BOOL     m_bConvert;
   CString  m_strActPrinterPort;
   CString  m_strActPrinterName;

   CString  m_strAdrKlPrinterPort;  // Adressaufkleber Drucker Port
   CString  m_strAdrKlPrinterName;  // Adressaufkleber Drucker Name
   BOOL     m_bAdrKlPrinterConvert; // Adressaufkleber Drucker DOS Konvertierung

   CString  m_strNNZSPrinterPort;   // Nachnahme Zahlschein Drucker Port
   CString  m_strNNZSPrinterName;   // Nachnahme Zahlschein Drucker Name
   BOOL     m_bNNZSPrinterConvert;  // Nachnahme Zahlschein Drucker DOS Konvertierung

   CString  m_strLogFilePrinterPort;// LogFile Drucker Port
   CString  m_strLogFilePrinterName;// LogFile Drucker Name
   BOOL     m_bLogFilePrinterConvert;// LogFile Drucker DOS Konvertierung

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_FACTURA_H__FA828EE4_A94A_11D2_9DB9_0000B458D891__INCLUDED_)
