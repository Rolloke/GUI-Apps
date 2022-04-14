#if !defined(AFX_TIMERIMPORTDIALOG_H__59E7D6B6_B506_11D2_B9A6_00400531137E__INCLUDED_)
#define AFX_TIMERIMPORTDIALOG_H__59E7D6B6_B506_11D2_B9A6_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimerImportDialog.h : header file
//
#include "WKClasses/WK_String.h"

/////////////////////////////////////////////////////////////////////////////
// CTimerImportDialog dialog

class CTimerImportDialog : public CWK_Dialog
{
// Construction
public:
	CTimerImportDialog(const CStringArray &names, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimerImportDialog)
	enum { IDD = IDD_TIMER_IMPORT };
	CListCtrl	m_ctlDuplicates;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerImportDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimerImportDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CStringArray m_names;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMERIMPORTDIALOG_H__59E7D6B6_B506_11D2_B9A6_00400531137E__INCLUDED_)
