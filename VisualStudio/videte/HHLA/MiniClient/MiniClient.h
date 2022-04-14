// MiniClient.h : main header file for the MINICLIENT application
//

#if !defined(AFX_MINICLIENT_H__AEE94395_8A89_11D2_8CA3_004005A11E32__INCLUDED_)
#define AFX_MINICLIENT_H__AEE94395_8A89_11D2_8CA3_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMiniClientApp:
// See MiniClient.cpp for the implementation of this class
//
class CMiniClientApp : public CWinApp
{
public:
	CMiniClientApp();
	~CMiniClientApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiniClientApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CMiniClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int		m_nCamNr;			// Kameranummer, die über die Kommandozeile mit übergeben wurde.
	BOOL	m_bMiniClientTest;	// TRUE -> Der Client läuft im Testmode
private:
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MINICLIENT_H__AEE94395_8A89_11D2_8CA3_004005A11E32__INCLUDED_)
