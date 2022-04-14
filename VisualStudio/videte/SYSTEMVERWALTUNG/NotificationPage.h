#if !defined(AFX_NOTIFICATIONPAGE_H__CB7AB793_6520_11D2_B4CC_00C095EC9EFA__INCLUDED_)
#define AFX_NOTIFICATIONPAGE_H__CB7AB793_6520_11D2_B4CC_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NotificationPage.h : header file
//
#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CNotificationPage dialog

class CNotificationPage : public CSVPage
{
// Construction
public:
	CNotificationPage(CSVView* pParent);   // standard constructor


// Dialog Data
	//{{AFX_DATA(CNotificationPage)
	enum { IDD = IDD_NOTIFICATION };
	CButton	m_radioURL;
	CButton	m_radioMessage;
	CEdit	m_editURL;
	CButton	m_btnBrowse;
	CEdit	m_editMessage;
	CEdit	m_editName;
	CListCtrl	m_Notification;
	CString	m_sName;
	CString	m_sMessage;
	CString	m_sURL;
	int		m_iKind;
	//}}AFX_DATA


// Overrides
public:
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
	virtual void OnNew();
	virtual void OnDelete();
	virtual BOOL StretchElements();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotificationPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNotificationPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeName();
	afx_msg void OnItemchangedNotification(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonBrowse();
	afx_msg void OnChangeEditMessage();
	afx_msg void OnChangeEditUrl();
	afx_msg void OnRadioMessage();
	afx_msg void OnRadioUrl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual	void	SaveChanges();
	virtual	void	CancelChanges();
	virtual	BOOL	IsDataValid();
			void	EnableExceptNew();
			void	SelectItem(int i);
			void	FillNotificationListBox();
			int		InsertItem(CNotificationMessage* pNM);
			void	ClearPageExceptList();
			BOOL	CheckActivationsWithNotification(CSecID nID);

			void GenerateHTMLFile();
			void GenerateHTMLFile(CNotificationMessage* pNM);

private:
	CNotificationMessageArray*	m_pNotifications;
	CNotificationMessage*		m_pSelectedNotification;
	CInputList*					m_pInputList;
	int							m_iSelectedItem;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTIFICATIONPAGE_H__CB7AB793_6520_11D2_B4CC_00C095EC9EFA__INCLUDED_)
