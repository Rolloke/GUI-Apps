#if !defined(AFX_ISDNPAGE_H__EB347BA4_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_ISDNPAGE_H__EB347BA4_17C0_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ISDNPage.h : header file
//

#include "SVPage.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CISDNPage dialog

class CISDNPage : public CSVPage
{
// Construction
public:
	CISDNPage(CSVView* pParent, int nIsdnNr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CISDNPage)
	enum { IDD = IDD_ISDN };
	CStatic	m_staticMinimumPrefix;
	CStatic	m_staticPrefix;
	CEdit	m_editPrefix;
	CEdit	m_editMinimumPrefix;
	CButton	m_btnRejectWithoutnumber;
	CButton	m_btnUseHostList;
	CRichEditCtrl	m_EditHostList;
	CButton	m_btnISDN;
	CButton m_btnB1;
	CButton m_btnB2;
	CButton m_btnNr;
	CButton m_btnAll;
	CButton m_btnNothing;
	CButton	m_btnLow;
	CButton	m_btnMedium;
	CButton	m_btnHigh;
	CButton	m_btnKnock;
	CEdit	m_editRufnummer;
	BOOL	m_bISDN;
	CString	m_sRufNummer;
	BOOL	m_bKnock;
	BOOL	m_bSupportMSN;
	int		m_iSystem;
	int		m_iBChannels;
	int		m_iAccept;
	BOOL	m_bUseHostList;
	BOOL	m_bRejectWithoutNumber;
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
	afx_msg void OnRadioNr();
	afx_msg void OnChange();
	afx_msg void OnUseHostlist();
	afx_msg void OnRejectWithoutNumber();
	afx_msg void OnChangeHostList();
	afx_msg void OnChangeRufnummer();
	afx_msg void OnCopyIsdnHosts();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL LoadHostListFromRegistry();
	BOOL SaveHostListToRegistry();
	BOOL IsHostInHostList(const CString &sHost);
private:
	CString m_sSection;
	CString m_sAppName;
	int	 m_nIsdnNr;
	BOOL m_bISDNOther; // Ist die andere ISDN-Unit angewählt?
	BOOL m_bLoadingHostList;
	CFont m_fixedFont;
public:
	CComboBox m_cCompression;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISDNPAGE_H__EB347BA4_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
