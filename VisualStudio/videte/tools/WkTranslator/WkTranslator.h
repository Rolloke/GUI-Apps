// WkTranslator.h : main header file for the WKTRANSLATOR application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorApp:
// See WkTranslator.cpp for the implementation of this class
//
class CWkTranslatorApp : public CWinApp
{
public:
	void SetFontForChildren(HWND);
	CWkTranslatorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWkTranslatorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWkTranslatorApp)
	afx_msg void OnAppAbout();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnViewEditfont();
	afx_msg void OnUpdateViewEditfont(CCmdUI* pCmdUI);
	afx_msg void OnGlossarySelectCodepage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CFont	m_lfEdit;
	BOOL	m_bCmdLineFile;
	static BOOL CALLBACK EnumChildToSetFont(HWND hwnd, LPARAM lParam);
	public:
	WORD           m_wCodePage;
};

extern CWkTranslatorApp theApp;


/////////////////////////////////////////////////////////////////////////////
