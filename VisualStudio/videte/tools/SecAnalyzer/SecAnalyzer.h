// SecAnalyzer.h : main header file for the SECANALYZER application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerApp:
// See SecAnalyzer.cpp for the implementation of this class
//
class CMainFrame;
class CSecAnalyzerDoc;

extern BOOL DecompressLGZFile(const CString & sName, CString &sNewName);

class CSecAnalyzerApp : public CWinApp
{
public:
	CSecAnalyzerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecAnalyzerApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSecAnalyzerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnDeleteFile();
	afx_msg void OnViewWordpad();
	afx_msg void OnFileNew();
	afx_msg void OnDoTimerTest();
	//}}AFX_MSG
	afx_msg void OnMyFileOpen();
	DECLARE_MESSAGE_MAP()
public:
		CSecAnalyzerDoc *m_pDoc;
		CString m_sDefaultFile;
protected:

};

inline CSecAnalyzerApp* GetApp()
{
	return (CSecAnalyzerApp*) AfxGetApp();
}

inline CMainFrame	*GetMain()
{
	if (GetApp()) {
		return (CMainFrame*)GetApp()->m_pMainWnd;
	} else {
		return NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
