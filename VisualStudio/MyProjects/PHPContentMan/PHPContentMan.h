// PHPContentMan.h : Haupt-Header-Datei für die Anwendung PHPCONTENTMAN
//

#if !defined(AFX_PHPCONTENTMAN_H__E1221F30_D5D0_403A_BA9C_B166B4C59C5C__INCLUDED_)
#define AFX_PHPCONTENTMAN_H__E1221F30_D5D0_403A_BA9C_B166B4C59C5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CPHPContentManApp:
// Siehe PHPContentMan.cpp für die Implementierung dieser Klasse
//
#define LISTHTMLSIZE 101
#define TEXT2HTML      1
#define HTML2TEXT     -1

struct ListHTMLSigns
{
   char cSign;
   char szHTML[9];
};

class CPictureCatalog;
class CHtmlSignDlg;

class CPHPContentManApp : public CWinApp
{
public:
	CPHPContentManApp();
	virtual ~CPHPContentManApp();

	bool StartFrameWnd(int nMenu=0);

   static void ReadHTML(CArchive&, CString&);
	static void WriteHTML(CArchive&, LPCTSTR pszText);
	static LPTSTR GetBuffer(CString&, LPTSTR, UINT&, UINT);
	static void SetDirection(int);
   static const char* GetHTMLCode(int);
	static char GetCharCode(int);

	CString m_strAppPath;
   // Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPHPContentManApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CPHPContentManApp)
	afx_msg void OnAppAbout();
	afx_msg void OnCallPictureContentMan();
	afx_msg void OnUpdateCallSettings(CCmdUI* pCmdUI);
	afx_msg void OnCallHtmlCode();
	afx_msg void OnUpdateCancel(CCmdUI* pCmdUI);
	virtual void OnCancel();
	afx_msg void OnHtmlcodeRestore();
	afx_msg void OnAppTopmost();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   CPictureCatalog *m_pPictureDlg;
   CHtmlSignDlg    *m_pHtmlSignDlg;


private:
   static ListHTMLSigns gm_CharHTMLList[LISTHTMLSIZE];
   static int           gm_nDirection;
	static int _cdecl CompareHTML(const void*, const void*);
	static int _cdecl FindHTML(const void*, const void*);
	static int _cdecl CompareChar(const void*, const void*);
	static int _cdecl FindChar(const void*, const void*);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_PHPCONTENTMAN_H__E1221F30_D5D0_403A_BA9C_B166B4C59C5C__INCLUDED_)
