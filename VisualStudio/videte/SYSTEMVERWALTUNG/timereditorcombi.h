/* GlobalReplace: Resrtict --> Restrict */
/* GlobalReplace: TimerComi --> TimerCombi */
#if !defined(AFX_TIMEREDITORCOMBI_H__57CDD643_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
#define AFX_TIMEREDITORCOMBI_H__57CDD643_260F_11D2_B8FB_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TimerEditorCombi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimerEditorCombi dialog

class CSecTimer;
class CSecTimerArray;

class CTimerEditorPage;

class CTimerEditorCombi : public CWK_Dialog
{
// Construction
public:
	void UpdateActionButtons(HWND hWnd = NULL );
	CTimerEditorCombi(CTimerEditorPage* pParent);   // standard constructor

	void FillFromTimer(CSecTimer *pTimer,const CSecTimerArray & timers);


// Dialog Data
	//{{AFX_DATA(CTimerEditorCombi)
	enum { IDD = IDD_TIMER_COMBI_EDITOR };
	CListCtrl	m_ctlTimerListSourceBlocked;
	CListCtrl	m_ctlTimerListSource;
	CButton	m_ctlButtonRestrict;
	CButton	m_ctlButtonInclude;
	CButton	m_ctlButtonExclude;
	CButton	m_ctlButtonBack;
	CListCtrl	m_ctlTimerListExclude;
	CListCtrl	m_ctlTimerListInclude;
	CListCtrl	m_ctlTimerListRestriction;
	//}}AFX_DATA


// Overrides
	virtual BOOL StretchElements();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimerEditorCombi)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTimerEditorCombi)
	afx_msg void OnSetfocusTimerCombiInclude(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTimerCombiRestrict(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTimerCombiExclude(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonTimerCombiInclude();
	afx_msg void OnButtonTimerCombiRestrict();
	afx_msg void OnButtonTimerCombiExclude();
	afx_msg void OnButtonTimerCombiBack();
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedTimerCombiSource(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedTimerCombiInclude(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedTimerCombiExclude(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedTimerCombiRestrict(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTimerCombiSource(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTimerCombiInclude(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTimerCombiExclude(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTimerCombiResrtict(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTimerCombiSource(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTimerCombiSourceBlocked(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void TranferTimers(CListCtrl &src, CListCtrl &destination);
	CTimerEditorPage* m_pParent;
	CSecTimer * m_pSelectedTimer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEREDITORCOMBI_H__57CDD643_260F_11D2_B8FB_00C095ECA33E__INCLUDED_)
