#if !defined(AFX_GLOSSARYINFODLG_H__070D061F_3505_4D86_B4A2_4D00867FBBB7__INCLUDED_)
#define AFX_GLOSSARYINFODLG_H__070D061F_3505_4D86_B4A2_4D00867FBBB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

// GlossaryInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGlossaryInfoDlg dialog

class CGlossaryInfoDlg : public CDialog
{
// Construction
public:
	CGlossaryInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlossaryInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void Create();
	void SetGlossaryCount(int iCount);
	void SetTextCount(int iCount);
	void SetFoundCount(int nFound);

protected:
	void SetBtnText();

	// Generated message map functions
	//{{AFX_MSG(CGlossaryInfoDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnCkStop();
	afx_msg void OnCkReplace();
	afx_msg void OnStep();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
	//{{AFX_DATA(CGlossaryInfoDlg)
	enum { IDD = IDD_IMPORT_GLOSSARY_INFO };
	CStatic	m_cFilename;
	CStatic	m_txtTextNo;
	CStatic	m_txtGlossaryNo;
	CString	m_sFormat;
	//}}AFX_DATA
public:
	void SetFileName(LPCTSTR);
	volatile BOOL m_bCancel;
	volatile BOOL m_bStop;
	volatile BOOL m_bStep;
	BOOL	m_bReplace;
	CStatic	m_cTranslatedText;
	CStatic	m_cOriginalText;
	CStatic	m_cTranslate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOSSARYINFODLG_H__070D061F_3505_4D86_B4A2_4D00867FBBB7__INCLUDED_)
