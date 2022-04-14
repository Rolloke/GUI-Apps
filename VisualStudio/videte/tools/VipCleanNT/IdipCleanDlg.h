#if !defined(AFX_IDIPCLEANDLG_H__ADBC2C0A_C434_11D4_8C8E_004005A1D890__INCLUDED_)
#define AFX_IDIPCLEANDLG_H__ADBC2C0A_C434_11D4_8C8E_004005A1D890__INCLUDED_

#include "VipCleanReg.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IdipCleanDlg.h : header file
//

#include "Clean98Dlg.h"
#include "VipCleanNTDlg.h"

class CCleanNTDlg;

#ifdef _IDIP

/////////////////////////////////////////////////////////////////////////////
// CIdipCleanDlg dialog

class CIdipCleanDlg : public CDialog
{
// Construction
public:
	CIdipCleanDlg(CWnd* pParent = NULL);   // standard constructor
	~CIdipCleanDlg();

// Dialog Data
	//{{AFX_DATA(CIdipCleanDlg)
	enum { IDD = IDD_IDIPCLEAN_DIALOG };
	CEdit	m_cZone;
	CEdit	m_cDate;
	CEdit	m_cTime;
	CButton	m_cOK;
	CEdit	m_cInfoNotDelDir;
	CEdit	m_cInfoDelDir;
	CButton	m_cDelArchives;
	CButton	m_cDelLog;
	CButton	m_cDelTemp;
	CString	m_sBuild;
	CString	m_sOS;
	CString	m_sSR;
	CString	m_sDrives;
	CString	m_sScreen;
	CString	m_sAlleArchive;
	CString	m_sTemp;
	CString	m_sInfoDelDir;
	CString	m_sInfoNotDelDir;
	CString	m_sZone;
	CString	m_sTime;
	CString	m_sDate;
	CString	m_sHz;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIdipCleanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL PrintTreeRecursiv(CString sStartDir,  
					  CString sResultFile,
					  BOOL bDelete);
	CString InsertDezPoints(__int64 iValue);
	BOOL DoesFileExist(LPCTSTR szFileOrDirectory);
	inline BOOL GetAenderungenClicked();
	inline void SetAenderungenClicked(BOOL bValue);

// Implementation
protected:
	 OS_TYPE GetOSVersion();
  	 BOOL CheckHarddiskDrives();
	 CString GetDisplaySettings();
	 CString GetDrivesForArchivDelete();
	 CString GetTempStatus();
	 CString GetTimeZone();
	 void InitTimezone();
	 void InitDateTime();
	 BOOL ChangeDateTime();
	// Generated message map functions
	//{{AFX_MSG(CIdipCleanDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnDeldir();
	virtual void OnOK();
	afx_msg void OnChangeDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeTimezone();
	afx_msg void OnDatetimezone();
	afx_msg void OnBeenden();
	afx_msg void On60Hz();
	afx_msg void On75hz();
	afx_msg void OnBtnAudioVolumes();
	afx_msg void OnCkAudioTest();
	afx_msg long OnMessageUpdateWnd(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedTweakTool();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
private:
	void InitAllDlgs();
	void GetMonitorConfigurations();
	CString GetHDDID();

private:

	CVipCleanReg	m_Reg;
	CString			m_sCams;
	OS_TYPE			m_OsType;
	CCleanNTDlg*	m_pNTDlg;
	CClean98Dlg*	m_p98Dlg;
	CStringArray	m_saDrives;
	CString			m_sDriveLetters;
	BOOL			m_bAenderungenClicked;
	BOOL			m_bVerzeichnisseClicked;
	SYSTEMTIME		m_SystemDateTime;
	TIME_ZONE_INFORMATION m_pTimeZoneInformation;
	CTime			m_StartTime;
	CWK_CPLApplet*	m_pDateTime;
	DEVMODE m_lpDevMode; //beinhaltet alle Monitoreinstellungen


public:
};

/////////////////////////////////////////////////////////////////////////////
inline BOOL CIdipCleanDlg::GetAenderungenClicked()
{
	return m_bAenderungenClicked;
}

/////////////////////////////////////////////////////////////////////////////
inline void CIdipCleanDlg::SetAenderungenClicked(BOOL bValue)
{
	m_bAenderungenClicked = bValue;
}

#endif

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDIPCLEANDLG_H__ADBC2C0A_C434_11D4_8C8E_004005A1D890__INCLUDED_)
