#if !defined(AFX_COPYFOLDER_H__D2227460_982D_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_COPYFOLDER_H__D2227460_982D_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CopyFolder.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCopyFolder 

class CCopyFolder : public CDialog
{
// Konstruktion
public:
	BOOL CreateDirectoryAndSubs(char*);
	void EnableControls(bool);
	void TerminateCopyThread();
	CCopyFolder(CWnd* pParent = NULL);     // Standardkonstruktor
	~CCopyFolder();                        // StandardDestruktor
	void OutputLineToLogWnd(LPCTSTR);
	void MakeStringArrays();
	void DeleteStringArrays();
	void EnumFiles(char*);

// Dialogfelddaten
	//{{AFX_DATA(CCopyFolder)
	enum { IDD = IDD_COPY_FOLDER };
	CComboBox	m_cIncludes;
	CEdit	m_cLogWindow;
	CString	m_strDestinatonPath;
	CString	m_strIncludes;
	CString	m_strSourcePath;
	BOOL     m_bOnlyNewerFiles;
	int		m_nDirOptions;
	//}}AFX_DATA
   int    m_nIncludes;
   int    m_nSel;
   bool   m_bChanged;
   char **m_ppszIncludes;
private:
	static unsigned int __stdcall CopyFilesS(void *);
	unsigned int CopyFiles();

	static unsigned int __stdcall CopyIniControlledS(void *);
	unsigned int CopyIniControlled();


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCopyFolder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;
   int m_nLogTextLimit;
   HANDLE m_hThread;
   unsigned int m_nThreadID;
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCopyFolder)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnCpfSearchDestinationPath();
	afx_msg void OnCpfSearchSourcePath();
	virtual BOOL OnInitDialog();
	afx_msg void OnCopy();
	afx_msg void OnSelchangeCpfIncludes();
	afx_msg void OnKillfocusCpfIncludes();
	afx_msg void OnEditchangeCpfIncludes();
	afx_msg void OnKillfocusCpfSourcePath();
	afx_msg void OnKillfocusCpfDestinationPath();
	afx_msg void OnBtnFiles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   BOOL  (__stdcall*pGetFileAttributesEx)(LPCSTR, GET_FILEEX_INFO_LEVELS, LPVOID);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_COPYFOLDER_H__D2227460_982D_11D3_B6EC_0000B458D891__INCLUDED_
