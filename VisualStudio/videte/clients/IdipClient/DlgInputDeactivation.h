#if !defined(AFX_DlgInputDeactivation_H__1BF28C33_A59B_11D4_8683_004005A26A3B__INCLUDED_)
#define AFX_DlgInputDeactivation_H__1BF28C33_A59B_11D4_8683_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInputDeactivation.h : header file
//
#include "Server.h"
#include "resource.h"


class CViewAlarmList;
/////////////////////////////////////////////////////////////////////////////
// CDlgInputDeactivation dialog

class CDlgInputDeactivation : public CSkinDialog
{
	// Construction
public:
	CDlgInputDeactivation(CViewAlarmList* pParent, CServer* pServer, CSecID id);   // standard constructor
		~CDlgInputDeactivation();

	inline WORD GetServerID() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputDeactivation)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInputDeactivation)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonNew();
	afx_msg void OnButtonDelete();
	afx_msg void OnDatetimechangeStartDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeStartTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeStopDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeStopTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListDeactivations(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListDeactivations(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListDeactivations(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkListDeactivations(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListDeactivations(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	void EnableControls();
	void ChangeTimeSpan(CSystemTimeSpan *pSystemTimeSpan,int iCtrl=0);
	void SetActualTime();
	BOOL CheckTimeSpan(int iCtrl = 0);
	void UpdateListSpans(BOOL bAddItem);		// bAddItem = TRUE: Neues Elememt einfügen, 
												// FALSE: Gesamte Liste erneuern
	void OnChangeListEntry(int iNewSelection);
// Dialog Data
	//{{AFX_DATA(CDlgInputDeactivation)
	enum  { IDD = IDD_INPUT_DEACTIVATION };
	CSkinButton	m_Button_OK;
	CSkinButton	m_Button_New;
	CSkinButton	m_Button_Delete;
	CSkinDateTimeCtrl	m_ctrlStopTime;
	CSkinDateTimeCtrl	m_ctrlStartTime;
	CSkinDateTimeCtrl	m_ctrlStopDate;
	CSkinDateTimeCtrl	m_ctrlStartDate;
	CSkinListCtrlX		m_ListSpans;
	//}}AFX_DATA
	int					m_iSelectedItemIndex;
	CSystemTimeSpans*	m_pAlarmOffSpans;
	CViewAlarmList*		m_pParent;
	WORD				m_wServerID;
	WORD				m_wDummy;
	CSecID				m_InputID;
	BOOL				m_bChanged;
};
inline WORD CDlgInputDeactivation::GetServerID() const
{
	return m_wServerID;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgInputDeactivation_H__1BF28C33_A59B_11D4_8683_004005A26A3B__INCLUDED_)
