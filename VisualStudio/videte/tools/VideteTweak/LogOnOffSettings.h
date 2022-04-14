#if !defined(AFX_LOGONOFFSETTINGS_H__4A007C31_8A61_42B8_A1B0_27D87630F2E5__INCLUDED_)
#define AFX_LOGONOFFSETTINGS_H__4A007C31_8A61_42B8_A1B0_27D87630F2E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogOnOffSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogOnOffSettings dialog

class CLogOnOffSettings : public CPropertyPage
{
	DECLARE_DYNCREATE(CLogOnOffSettings)

// Construction
public:
	CLogOnOffSettings();
	~CLogOnOffSettings();

// Dialog Data
	//{{AFX_DATA(CLogOnOffSettings)
	enum { IDD = IDD_LOGON_OFF };
	BOOL	m_bAutoAdminLogon;
	CString	m_strDefaultDomain;
	CString	m_strDefaultPassword;
	CString	m_strDefaultUserName;
	BOOL		m_bDonDisplayLastUserName;
	BOOL		m_bDeleteRoamingCache;
	int		m_nCachedLogonsCount;
	BOOL	m_bPowerDownAfterShutdown;
	int		m_nSlowLinkTimeOut;
	BOOL	m_bSlowLinkDetectEnabled;
	BOOL	m_bNoPopUpsOnBoot;
	//}}AFX_DATA
	bool m_bChanged;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLogOnOffSettings)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLogOnOffSettings)
	afx_msg void OnChange();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnRestore2();
	afx_msg void OnBtnSave2();
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg void OnBtnImageSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGONOFFSETTINGS_H__4A007C31_8A61_42B8_A1B0_27D87630F2E5__INCLUDED_)
