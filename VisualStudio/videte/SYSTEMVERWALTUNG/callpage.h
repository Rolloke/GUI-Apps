/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: callpage.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/callpage.h $
// CHECKIN:		$Date: 12.11.03 12:54 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 12.11.03 11:29 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef CALLPAGE_H
#define CALLPAGE_H

#include "SVPage.h"
#include "processlist.h"

/////////////////////////////////////////////////////////////////////////////
#define MAX_SELECTED_HOSTS	5

/////////////////////////////////////////////////////////////////////////////
class CHostArray;
class CInputList;
class CPermissionArray;

/////////////////////////////////////////////////////////////////////////////
// CCallPage dialog
class CCallPage : public CSVPage
{
	DECLARE_DYNAMIC(CCallPage)
// Construction
public:
	CCallPage(CSVView* pParent);
	~CCallPage();

// Overrides
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCallPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void CancelChanges();
	virtual void SaveChanges();
	virtual void OnNew();
	virtual void OnDelete();
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual BOOL IsDataValid();

protected:
	// Generated message map functions
	//{{AFX_MSG(CCallPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListproc();
	afx_msg void OnInterval();
	afx_msg void OnForever();
	afx_msg void OnChangeCalltime();
	afx_msg void OnHold();
	afx_msg void OnChangePausetime();
	afx_msg void OnChangeMaintime();
	afx_msg void OnChangeName();
	afx_msg void OnDblclkIsdnListHostCall();
	afx_msg void OnDblclkIsdnListHostAll();
	afx_msg void OnSelchangeComboPermission();
	afx_msg void OnSelchangeListHostAll();
	afx_msg void OnChangeEditMct();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	ProcessToControl(CProcess* pProcess);
	void	ControlToProcess(CProcess* pProcess);
	void	ShowHide();
	void	ClearListBoxProcess();
	void	FillListBoxProcess();
	void	ResetHostList();
	int 	InsertHost(CHost* pHost);
	void    FillPermissions();
	void	SetStandardEntriesForHostToCall(int iIndex);

// Dialog Data
protected:
	//{{AFX_DATA(CCallPage)
	enum { IDD = IDD_PROZ_ISDN };
	CEdit	m_editMCT;
	CComboBox	m_cbPermissions;
	CListBox	m_lbAllHost;
	CButton	m_btnInterval;
	CButton	m_btnHold;
	CEdit	m_editCallTime;
	CStatic	m_sTxtPause;
	CEdit	m_editPauseTime;
	CListBox	m_lbToCallHost;
	CButton	m_btnIntervalGrp;
	CButton	m_btnMainTime;
	CButton	m_btnForever;
	CStatic	m_sTxtNotForever;
	CEdit	m_editMainTime;
	CStatic	m_sSec2;
	CStatic	m_sSec1;
	CEdit	m_editName;
	CListBox	m_lbProcess;
	CString	m_sName;
	int		m_iProcCurSel;
	BOOL	m_bHold;
	BOOL	m_bInterval;
	BOOL	m_bForever;
	CString	m_sCallTime;
	CString	m_sMainTime;
	CString	m_sPauseTime;
	DWORD	m_dwConnectionTime;
	//}}AFX_DATA
	CInputList*			m_pInputList;
	CProcessList*		m_pProcessList;
	CHostArray*			m_pHostArray;
	CPermissionArray*	m_pPermissionArray;

	CProcess*	m_pSelectedProcess;
	int			m_iSelectedItem;
};
#endif // ISDNCHILDDLG_H
