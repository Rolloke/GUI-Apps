/////////////////////////////////////////////////////////////////////////////
// PROJECT:		LogView
// FILE:		$Workfile: LogView.h $
// ARCHIVE:		$Archive: /Project/Tools/LogView/LogView.h $
// CHECKIN:		$Date: 3/14/06 4:32p $
// VERSION:		$Revision: 13 $
// LAST CHANGE:	$Modtime: 3/14/06 3:22p $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGVIEW_H__4DB08FE5_BC2E_11D2_A9C4_004005A11E32__INCLUDED_)
#define AFX_LOGVIEW_H__4DB08FE5_BC2E_11D2_A9C4_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLogViewApp:
// See LogView.cpp for the implementation of this class
//
#define WM_OPEN_FILE (WM_USER + 1)

#define TEMPORARY_LOGFILE _T(".tmp.log")
#define PING_LOG		  _T("ping.log")

class CImpIDispatch;
class CCustomOccManager;
class CLogViewApp : public CWinApp
{
public:
	CLogViewApp();
	~CLogViewApp();

// Attributes
	inline BOOL DoReadUnicode() const;
	inline CImpIDispatch* GetDispOM() const;
    int    GetFixedFormats() const { return mFixedFormats; };


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogViewApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CLogViewApp)
	afx_msg void OnAppAbout();
	afx_msg void OnUpdateFileReadLogfileUnicode(CCmdUI *pCmdUI);
	afx_msg void OnFileReadLogfileUnicode();
	afx_msg void OnFileRuntimeErrors();
	afx_msg void OnUpdateFileRuntimeErrors(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFileEditTimeFormats(CCmdUI *pCmdUI);
    afx_msg void OnFileEditTimeFormats();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// implementation
protected:
	void RegisterWindowClass();
	BOOL SplitCommandLine(const CString sCommandLine, CStringArray& saParameters, LPCTSTR szParam);

private:
	CImpIDispatch*		m_pDispOM;
	BOOL	m_bDoReadUnicode;
    int     mFixedFormats;
public:
    CStringArray mFormats;
};

/////////////////////////////////////////////////////////////////////////////
inline BOOL CLogViewApp::DoReadUnicode() const
{
	return m_bDoReadUnicode;
}
/////////////////////////////////////////////////////////////////////////////
inline CImpIDispatch* CLogViewApp::GetDispOM() const
{
	return m_pDispOM;
}
extern CLogViewApp theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGVIEW_H__4DB08FE5_BC2E_11D2_A9C4_004005A11E32__INCLUDED_)
