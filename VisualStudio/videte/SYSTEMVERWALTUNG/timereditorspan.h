/* GlobalReplace: CTimerEditorDlg --> CTimerEditorPage */
#if !defined(AFX_TIMEREDITORSPAN_H__57CDD644_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
#define AFX_TIMEREDITORSPAN_H__57CDD644_260F_11D2_B8FB_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TimerEditorSpan.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorSpan dialog

class CTimerEditorPage;
class CSecTimer;

class CTimerEditorSpan : public CWK_Dialog
{
// Construction
public:
	CTimerEditorSpan(CTimerEditorPage * pParent);   // standard constructor

// Overrides
	virtual BOOL StretchElements();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerEditorSpan)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimerEditorSpan)
	virtual BOOL OnInitDialog();
	afx_msg void OnSpanEdit();
	afx_msg void OnItemchangedTimerSpanList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimeSpanRemove();
	afx_msg void OnTimeSpanAdd();
	afx_msg void OnDblclkTimerSpanList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void	FillFromTimer(CSecTimer *pTimer);
	void	UpdateActionButtons();
private:
	void	UpdateSelectedTimer();
	void	RemoveTimeSpan(BOOL bModified);


// Dialog Data
private:
	//{{AFX_DATA(CTimerEditorSpan)
	enum { IDD = IDD_TIMER_SPAN_EDITOR };
	CButton	m_buttonSpanRemove;
	CButton	m_buttonSpanEdit;
	CButton	m_buttonSpanAdd;
	CListCtrl	m_listSpans;
	//}}AFX_DATA

	CTimerEditorPage*	m_pParent;
	CSecTimer*			m_pSelectedTimer;
	int					m_ixSelectedSpan;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEREDITORSPAN_H__57CDD644_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)

