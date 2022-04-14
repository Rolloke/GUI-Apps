#if !defined(AFX_DRIVEACCELERATION_H__3BD71361_131F_4F82_8E33_ACA379EB9C2E__INCLUDED_)
#define AFX_DRIVEACCELERATION_H__3BD71361_131F_4F82_8E33_ACA379EB9C2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DriveAcceleration.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDriveAcceleration dialog

class CDriveAcceleration : public CPropertyPage
{
	DECLARE_DYNCREATE(CDriveAcceleration)

// Construction
public:
	CDriveAcceleration();
	~CDriveAcceleration();

// Dialog Data
	//{{AFX_DATA(CDriveAcceleration)
	enum { IDD = IDD_HDD_MANAGEMENT };
	BOOL	m_bClearPageFileAtSchutdown;
	BOOL	m_bDisable8dot3Filenames;
	BOOL	m_bDisableSystemEventLog;
	int	m_nMFTCacheSize;
	BOOL	m_bEnlargeCDRomCache;
	BOOL	m_bNtfsDisableLastAccessUpdate;
	BOOL	m_bEnableBigLBA;
	int		m_nKeyCacheValidationPeriod;
	//}}AFX_DATA
	bool m_bChanged;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDriveAcceleration)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDriveAcceleration)
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnBtnRestore();
	afx_msg void OnBtnSave();
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg void OnBtnImageSetttings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVEACCELERATION_H__3BD71361_131F_4F82_8E33_ACA379EB9C2E__INCLUDED_)
