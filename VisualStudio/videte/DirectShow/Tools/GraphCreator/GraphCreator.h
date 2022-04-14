// GraphCreator.h : main header file for the GRAPHCREATOR application
//

#if !defined(AFX_GRAPHCREATOR_H__9C9F51C0_BE86_4568_A252_1E9D9FEBB521__INCLUDED_)
#define AFX_GRAPHCREATOR_H__9C9F51C0_BE86_4568_A252_1E9D9FEBB521__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorApp:
// See GraphCreator.cpp for the implementation of this class
//
#define WPARAM_CTRL_REMOVED		0x1000
#define WPARAM_CLOSE_DOC		0x1001
#define WPARAM_UPDATE_GRAPH		0x1002
#define WPARAM_INSERT_FILTER	0x1003

#define SETTINGS_SECTION	_T("Settings")
#define CTRL_FONT_SIZE		_T("FontSize")
#define CTRL_FONT			_T("FontName")
extern CFactoryTemplate g_Templates[];

class CGraphCreatorApp : public CWinApp
{
public:
	CGraphCreatorApp();

	BOOL GetInterfaces(IUnknown*pUnk, CStringArray &sa);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphCreatorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual HINSTANCE LoadAppLangResourceDLL();
	//}}AFX_VIRTUAL

// Implementation
public:

#if GRAPH_CREATOR_PROPERTY_TEST
   static HRESULT ShowFilterPropertyPage(IBaseFilter *pFilter, HWND hwndParent);
#endif
	CView *GetActiveView();
	static void	DoUpdateMenuPopup(CMenu*pMenu, CWnd*pWnd);
	//{{AFX_MSG(CGraphCreatorApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileRenderMedia();
	afx_msg void OnViewAutoRearrangeFilters();
	afx_msg void OnUpdateViewAutoRearrangeFilters(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int m_nMaxRecentFiles;
public:
	BOOL m_bRenderMediaFile;
	BOOL m_bAutoRearrangeFilters;

};

extern CGraphCreatorApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHCREATOR_H__9C9F51C0_BE86_4568_A252_1E9D9FEBB521__INCLUDED_)
