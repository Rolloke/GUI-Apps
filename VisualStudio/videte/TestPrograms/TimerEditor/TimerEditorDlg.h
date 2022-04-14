// TimerEditorDlg.h : header file
//

#if !defined(AFX_TIMEREDITORDLG_H__57CDD63A_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
#define AFX_TIMEREDITORDLG_H__57CDD63A_260F_11D2_B8FB_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TimerEditorSpan.h"
#include "TimerEditorCombi.h"

#include "SecTimer.h"

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorDlg dialog

class CTimerEditorDlg : public CDialog
{
// Construction
public:
	CTimerEditorDlg(CWnd* pParent = NULL);	// standard constructor
	~CTimerEditorDlg();

// Dialog Data
	//{{AFX_DATA(CTimerEditorDlg)
	enum { IDD = IDD_TIMEREDITOR_DIALOG };
	CButton	m_groupDummy;
	CListCtrl	m_listTimer;
	int		m_iTypeSelected;
	CString	m_sSelectedName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerEditorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTimerEditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRadioTimerTypeCombination();
	afx_msg void OnRadioTimerTypeSpan();
	afx_msg void OnItemchangedTimerList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimerNew();
	afx_msg void OnTimerDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ShowSpanEditor();
	void ShowCombiEditor();

	CTimerEditorSpan*	m_TimerEditorSpan;
	CTimerEditorCombi*	m_TimerEditorCombi;

	CSecTimerArray m_timers;
	CSecTimer * m_pSelectedTimer;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEREDITORDLG_H__57CDD63A_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
