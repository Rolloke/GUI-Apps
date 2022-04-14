/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: userpage.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/userpage.h $
// CHECKIN:		$Date: 3.06.99 10:09 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 3.06.99 9:37 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#ifndef USERPAGE_H
#define USERPAGE_H

#include "SVPage.h"

class CUser;
class CUserArray;
class CPermissionArray;
class CPermission;
			 
/////////////////////////////////////////////////////////////////////////////
// CUserPage dialog
class CUserPage : public CSVPage
{
	DECLARE_DYNAMIC(CUserPage)

// Construction
public:
	CUserPage(CSVView* pParent);
	~CUserPage();

// Overrides
protected:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUserPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual void SaveChanges();
	virtual void CancelChanges();
	virtual void Initialize();
	virtual BOOL IsDataValid();

public:
	virtual BOOL CanDelete();
	virtual BOOL CanNew();
	virtual BOOL StretchElements();
	virtual void OnDelete();
	virtual void OnNew();
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUserPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedListUser(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeName();
	afx_msg void OnPassword();
	afx_msg void OnSelchangeComboPermission();
	afx_msg void OnButtonAlarm();
	afx_msg void OnCheckDoppelLogin();
	afx_msg void OnClickListUser(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListUser(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListUser(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkListUser(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void FillUsers();
	void ClearUsers();
	void ClearActualUser();
	void FillPermissions();
	int	 InsertUser(CUser* pUser, BOOL bSelectIt = FALSE);
	int	 InsertPermission(CPermission* pPermission);
	void EnableExceptNew();
	void SelectUser(int i, BOOL bSetSel = TRUE);

	// Dialog Data
protected:
	//{{AFX_DATA(CUserPage)
	enum { IDD = IDD_USERLIST };
	CButton	m_btnDualLogin;
	CButton	m_btnPassword;
	CEdit	m_editName;
	CComboBox	m_Permissions;
	CListCtrl	m_Users;
	CString	m_Name;
	BOOL	m_bDualLogin;
	//}}AFX_DATA
	CUserArray*			m_pUserArray;
	CPermissionArray*	m_pPermissionArray;
	CUser*				m_pSelectedUser;
	int					m_iSelectedItem;
	CString				m_sPassword;
};

#endif	// USERPAGE_H
