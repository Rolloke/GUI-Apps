// ClientPicTest.h : main header file for the ClientPicTest application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CClientPicTestApp:
// See ClientPicTest.cpp for the implementation of this class
//
class CTestTool;
class COutputTester;



class CClientPicTestApp : public CWinApp
{
public:
	CClientPicTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientPicTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CClientPicTestApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CTestTool*		m_pCTestTool;
	COutputTester*	m_pOutput;

private:
	int	DoTest(CString &sHost);
};

extern CClientPicTestApp theApp; 

///////////////////////////////////////////////////////////////
