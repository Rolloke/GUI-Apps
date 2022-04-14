// TimerEditorDlg.h : header file
//

#if !defined(AFX_TIMEREDITORDLG_H__57CDD63A_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
#define AFX_TIMEREDITORDLG_H__57CDD63A_260F_11D2_B8FB_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SVPage.h"

#include "TimerEditorSpan.h"
#include "TimerEditorCombi.h"

#include "SecTimer.h"

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorPage dialog
extern int GetIndexByLocalizedWeekDayName(const CString &sDay);

class CTimerEditorPage : public CSVPage
{
// Construction
public:
	CTimerEditorPage(CSVView* pParent);	// standard constructor
	~CTimerEditorPage();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerEditorPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	// override SVPage fns
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

	virtual void OnNew();
	virtual void OnDelete();
	
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual BOOL StretchElements();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTimerEditorPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioTimerTypeCombination();
	afx_msg void OnRadioTimerTypeSpan();
	afx_msg void OnItemchangedTimerList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeTimerLocks();
	afx_msg void OnUpdateTimerSelectedName();
	afx_msg void OnChangeTimerSelectedName();
	afx_msg void OnTimerDoTest();
	afx_msg void OnTimerExport();
	afx_msg void OnTimerImport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void FillListBox();
	void FillLockListBox();
	void ShowSpanEditor();
	void ShowCombiEditor();
	void SelectTimer(CSecTimer *pTimer);
	void SelectTimerByIndex(CSecTimer *pTimer);
	int GetIndexOfTimer(CSecTimer *pTimer);

// Dialog Data
public:
	CSecTimerArray* m_pTimers;
private:
	void EnableExceptNew();
	//{{AFX_DATA(CTimerEditorPage)
	enum { IDD = IDD_TIMEREDITOR };
	CButton	m_btnTimerExport;
	CButton	m_btnTimerTest;
	CEdit	m_ctlSelectedName;
	CButton	m_ctlSpanType;
	CButton	m_ctlCombinationType;
	CCheckListBox	m_ctlLocks;
	CButton	m_groupDummy;
	CListCtrl	m_listTimer;
	int		m_iTypeSelected;
	CString	m_sSelectedName;
	//}}AFX_DATA

	CTimerEditorSpan*	m_pTimerEditorSpan;
	CTimerEditorCombi*	m_pTimerEditorCombi;

	CSecTimer*			m_pSelectedTimer;
	int					m_iNewCounter;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEREDITORDLG_H__57CDD63A_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
