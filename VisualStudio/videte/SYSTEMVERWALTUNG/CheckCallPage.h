#if !defined(AFX_CHECKCALLPAGE_H__58F70A6A_01FE_11D3_B5E2_004005A19028__INCLUDED_)
#define AFX_CHECKCALLPAGE_H__58F70A6A_01FE_11D3_B5E2_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckCallPage.h : header file
//
#include "SVPage.h"
/////////////////////////////////////////////////////////////////////////////
// CCheckCallPage dialog

class CCheckCallPage : public CSVPage
{
// Construction
public:
	CCheckCallPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCheckCallPage)
	enum { IDD = IDD_PROZ_CHECK_CALL };
	CComboBox	m_cbPermission;
	BOOL	m_bSetTime;
	//}}AFX_DATA

// Overrides
public:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckCallPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitProcess();
	void ProcessToControl(CProcess* pProcess);
	void ControlToProcess(CProcess* pProcess);
	void FillPermissions();

	// Generated message map functions
	//{{AFX_MSG(CCheckCallPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboPermission();
	afx_msg void OnCheckTime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CProcessList*	m_pProcessList;
	CProcess*		m_pCheckCallProcess;
	CPermissionArray*	m_pPermissionArray;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKCALLPAGE_H__58F70A6A_01FE_11D3_B5E2_004005A19028__INCLUDED_)
