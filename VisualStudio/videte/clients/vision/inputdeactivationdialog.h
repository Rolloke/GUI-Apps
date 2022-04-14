#if !defined(AFX_INPUTDEACTIVATIONDIALOG_H__1BF28C33_A59B_11D4_8683_004005A26A3B__INCLUDED_)
#define AFX_INPUTDEACTIVATIONDIALOG_H__1BF28C33_A59B_11D4_8683_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputDeactivationDialog.h : header file
//
#include "Server.h"
#include "resource.h"


class CObjectView;
/////////////////////////////////////////////////////////////////////////////
// CInputDeactivationDialog dialog

class CInputDeactivationDialog : public CWK_Dialog
{
	// Construction
public:
	CInputDeactivationDialog(CObjectView* pParent, CServer* pServer, CSecID id);   // standard constructor
		~CInputDeactivationDialog();

	inline WORD GetServerID() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputDeactivationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputDeactivationDialog)
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

	BOOL OnInitDialog();

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
	//{{AFX_DATA(CInputDeactivationDialog)
	enum { IDD = IDD_INPUT_DEACTIVATION };
	CButton	m_Button_OK;
	CButton	m_Button_New;
	CButton	m_Button_Delete;
	CDateTimeCtrl	m_ctrlStopTime;
	CDateTimeCtrl	m_ctrlStartTime;
	CDateTimeCtrl	m_ctrlStopDate;
	CDateTimeCtrl	m_ctrlStartDate;
	CListCtrl	m_ListSpans;
	//}}AFX_DATA
	int					m_iSelectedItemIndex;
	CSystemTimeSpans*	m_pAlarmOffSpans;
	CObjectView*		m_pParent;
	WORD				m_wServerID;
	CServer*			m_pServer;
	CSecID				m_InputID;
	BOOL				m_bChanged;
};
inline WORD CInputDeactivationDialog::GetServerID() const
{
	return m_wServerID;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTDEACTIVATIONDIALOG_H__1BF28C33_A59B_11D4_8683_004005A26A3B__INCLUDED_)
