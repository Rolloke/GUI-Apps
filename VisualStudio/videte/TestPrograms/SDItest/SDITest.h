/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDITest.h $
// ARCHIVE:		$Archive: /Project/TestPrograms/SDITest/SDITest.h $
// CHECKIN:		$Date: 21.12.05 16:50 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 21.12.05 10:14 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#if !defined(AFX_SDITEST_H__2D22C519_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDITEST_H__2D22C519_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "SDITestCommandLineInfo.h"

/////////////////////////////////////////////////////////////////////////////
// CSDITestApp:
// See SDITest.cpp for the implementation of this class
//

class CSDITestApp : public CWinApp
{
public:
	CSDITestApp();


public:
	CString GetLocalIP();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDITestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	RegisterWindowClass();
	void	ProcessSDITestShellCommand(CSDITestCommandLineInfo& cmdInfo);

	//{{AFX_MSG(CSDITestApp)
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data
protected:
	CString		m_sDirectory;
};
extern CSDITestApp theApp;
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITEST_H__2D22C519_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
