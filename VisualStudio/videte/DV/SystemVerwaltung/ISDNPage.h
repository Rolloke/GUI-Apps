#if !defined(AFX_ISDNPAGE_H__EB347BA4_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_ISDNPAGE_H__EB347BA4_17C0_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ISDNPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CISDNPage dialog

class CISDNPage : public CSVPage
{
// Construction
public:
	CISDNPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CISDNPage)
	enum { IDD = IDD_ISDN };
	CEdit	m_editMinimumPrefix;
	CEdit	m_editPrefix;
	CButton	m_btnISDN;
	CButton m_btnB1;
	CButton m_btnB2;
	CEdit	m_editRufnummer;
	BOOL	m_bISDN;
	CString	m_sRufNummer;
	int		m_iBChannels;
	BOOL	m_bUseMSN;
	CString	m_sPrefix;
	int		m_iMinimumPrefix;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CISDNPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

	// Implementation
protected:
	void	ShowHide();

	// Generated message map functions
	//{{AFX_MSG(CISDNPage)
	afx_msg void OnCheckIsdn();
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnChangeRufnummer();
	afx_msg void OnCheckMsn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISDNPAGE_H__EB347BA4_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
