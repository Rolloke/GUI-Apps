// UnitTest.h : main header file for the UnitTest application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CUnitTestApp:
// See UnitTest.cpp for the implementation of this class
//
class CTestTool;
class COutputTester;

class CUnitTestApp : public CWinApp
{
public:
	CUnitTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnitTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CUnitTestApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CTestTool*		m_pCTestTool;
	COutputTester*	m_pOutput;

private:
	int	DoTest(const CString &sHost);
};


///////////////////////////////////////////////////////////////
