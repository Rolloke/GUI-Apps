// MSDNEdit.h : main header file for the MSDNEDIT application
//

#if !defined(AFX_MSDNEDIT_H__3553575B_23F1_42BB_B20A_209A208581C5__INCLUDED_)
#define AFX_MSDNEDIT_H__3553575B_23F1_42BB_B20A_209A208581C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMSDNEditApp:
// See MSDNEdit.cpp for the implementation of this class
//

#define REG_SETTINGS "Settings"

class CMSDNIntegration;

class CMSDNEditApp : public CWinApp
{
public:
	CMSDNEditApp();
	~CMSDNEditApp();

 	void RemoveDocTemplates();
	void SetDocTemplates();
	int  TestMSDNIntProcess();
   CSingleDocTemplate*GetDT2() {return m_pDT2;};
   CMSDNIntegration *m_pIntegration;
   PROCESS_INFORMATION m_piMSNDIntegratior;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMSDNEditApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMSDNEditApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
	afx_msg void OnEditComileBatch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
   private:
   CSingleDocTemplate *m_pDT2;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSDNEDIT_H__3553575B_23F1_42BB_B20A_209A208581C5__INCLUDED_)
