// ExtractBuildLog.h : main header file for the EXTRACTBUILDLOG application
//

#if !defined(AFX_EXTRACTBUILDLOG_H__CE2DA227_7526_11D5_BB45_0050BF49CEBF__INCLUDED_)
#define AFX_EXTRACTBUILDLOG_H__CE2DA227_7526_11D5_BB45_0050BF49CEBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
enum EnumFileType
{
	FT_DOCOMATIC_LIST,
	FT_BUILDLOG
};
/////////////////////////////////////////////////////////////////////////////
// CExtractBuildLogApp:
class CExtractBuildLogApp : public CWinApp
{
public:
	CExtractBuildLogApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtractBuildLogApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CExtractBuildLogApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	EnumFileType	m_eFileType;
	CString			m_sWorkspaceDir;
	CString			m_sWorkspaceName;
};

extern CExtractBuildLogApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTRACTBUILDLOG_H__CE2DA227_7526_11D5_BB45_0050BF49CEBF__INCLUDED_)
