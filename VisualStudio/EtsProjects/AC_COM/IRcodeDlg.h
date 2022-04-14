#if !defined(AFX_IRCODEDLG_H__99CBCC1C_C2C9_4484_A849_60F9AD8746FE__INCLUDED_)
#define AFX_IRCODEDLG_H__99CBCC1C_C2C9_4484_A849_60F9AD8746FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IRcodeDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CIRcodeDlg 
#define CODE_PARAMETER  27
#define CODE_COMMANDS   10
#define CODE_TRACK      18

class CIRcodeDlg : public CDialog
{
// Konstruktion
public:
	CIRcodeDlg(CWnd* pParent = NULL);   // Standardkonstruktor
   ~CIRcodeDlg()
   {
	   if (m_hIcon != INVALID_HANDLE_VALUE)
         ::DestroyIcon(m_hIcon);
   }

	bool ReadData(CString&);
	bool WriteData(CString&);

// Dialogfelddaten
	//{{AFX_DATA(CIRcodeDlg)
	enum { IDD = IDD_IR_CODE_DESCR };
	CListCtrl	m_cCodeList;
	CString	m_strDevice;
	//}}AFX_DATA

   HICON       m_hIcon;

   CString m_strFileName;
   BYTE m_cCD_CodeParameter[CODE_PARAMETER];
   BYTE m_cCD_CodeCommands[CODE_COMMANDS];
   BYTE m_cCD_CodeTrack[CODE_TRACK];

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CIRcodeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CIRcodeDlg)
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	afx_msg void OnBeginlabeleditIrCodeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditIrCodeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoIrCodeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickIrCodeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIrBtnLoad();
	afx_msg void OnIrBtnSave();
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_IRCODEDLG_H__99CBCC1C_C2C9_4484_A849_60F9AD8746FE__INCLUDED_
