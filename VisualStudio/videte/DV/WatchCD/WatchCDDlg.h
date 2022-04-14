// WatchCDDlg.h : header file
//

#if !defined(AFX_WATCHCDDLG_H__218741A6_E856_11D3_AE43_004005A1D890__INCLUDED_)
#define AFX_WATCHCDDLG_H__218741A6_E856_11D3_AE43_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DirectCDError.h"
#include "DirectCDStart.h"
#include "DirectCDFormat.h"
#include "DirectCDBackup.h"
#include "DirectCDFinalize.h"


/////////////////////////////////////////////////////////////////////////////
// CWatchCDDlg dialog

class CWatchCDDlg : public CDialog
{

// Construction
public:
	CWatchCDDlg(CWnd* pParent = NULL);	// standard constructor

public:

	void OnIdle();
	CString GetDCDWndTitleWithoutDrive();
	CString GetDCDWndTitleWithDrive();
	BOOL IsDVClient();
	HWND GetHandleDVClient();
	inline BOOL IsDVClientBackupComplete();
	inline BOOL IsDVClientCopyingDataError();
	inline int GetDirectCDVersion();

	BOOL CheckForUnexpectedErrors();
	static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
	CString GetBackupDrive();
	CString GetStringFromTable(CString sPath, CString sFile, int iStringID);
	CString GetStringFromDlg(CString sPath, CString sFile, CString sDlgID);
	CString GetPathToDirectCD();
	BOOL FileExists(CString sPath, CString sFileName);

	//DirectCD Version 5
	BOOL	IsDirectCDVersion5();
	CString	GetDCDWndTitleWithDriveVersion5();
	CString GetPathToDirectCDVersion5();

// Dialog Data
	//{{AFX_DATA(CWatchCDDlg)
	enum { IDD = IDD_WATCHCD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchCDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL


// Implementation
protected:
	CString FindDirectCDPath();
	HICON m_hIcon;

	void ChangeWCDStatus(DirectCDStatus WCDStatus);

	// Generated message map functions
	//{{AFX_MSG(CWatchCDDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg long OnReceiveStatus(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	CString GetDirectCDFullVersionString();
	void OnWCDStart();
	void OnWCDWaitForCDR();
	void OnWCDFormatCDR();
	void OnWCDBackup();
	void OnWCDFinalize();
	void OnWCDIsDriveLocked();
	void OnWCDWaitForEject();
	void OnWCDWaitForEjectFinished();
	void OnWCDEjectError();
	void OnWCDWaitForFileSystem();
	void OnWCDForceToEject();

	void OnWCDError();

	BOOL ExecuteDirectCD();
	BOOL ExecuteDirectCDVers5();
	BOOL IsDriveLocked();
	BOOL IsCDEjected();

	//DirectCD Version5
	CString	GetDirectCDFullVersionStringVersion5();
// Implementation
private:

	void InitLanguageStrings();

	//DirectCD Version5
	void InitLanguageStringsVersion5();

// data member
private:

	CString			m_sDCDWndTitleWithoutDrive;
	CString			m_sDCDWndTitleWithDrive;
	DirectCDStatus	m_DCDStatus;
	DirectCDStatus	m_LastDCDStatus;
	HWND m_hWatchDog;
	HWND m_hStartDlg;

	HWND m_hDVClient;
	HWND m_hUnexpErrorWnd;
	HWND m_hOnUnexpErrorActiveDCDWnd;
	HWND m_hEjectDlg;
	HWND m_hWaitForFileSystemDlg;

	CDirectCDError*		m_pDCDError;
	CDirectCDStart*		m_pDCDStart;
	CDirectCDFormat*	m_pDCDFormat;
	CDirectCDBackup*	m_pDCDBackup;
	CDirectCDFinalize*	m_pDCDFinalize;

	BOOL m_bDVClientBackupComplete;
	BOOL m_bDVClientCopyingData;
	BOOL m_bDVClientCopyingDataError;
	
	int		m_iDCDVersion;
	DWORD	m_dwYesButtonOnErrorDlg;
	CString m_sAdaptec;
	CString m_sAdaptecWatchdog;
	BOOL	m_bDirectCDStarted;
	CString	m_sUnableToLock;
	CString m_sFullDrive;
	DWORD	m_dwTick;
	HANDLE	m_hProcessWizard;

	
	//DirectCD Version5
	BOOL	m_bDirectCDVersion5;
	CString	m_sDCDWndTitleWithDriveVersion5;

};

/////////////////////////////////////////////////////////////////////////////
inline BOOL CWatchCDDlg::IsDVClientBackupComplete()
{
	return m_bDVClientBackupComplete; 
}

/////////////////////////////////////////////////////////////////////////////
inline BOOL CWatchCDDlg::IsDVClientCopyingDataError()
{
	return m_bDVClientCopyingDataError;
}

/////////////////////////////////////////////////////////////////////////////
inline int CWatchCDDlg::GetDirectCDVersion()
{
	return m_iDCDVersion;
}


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHCDDLG_H__218741A6_E856_11D3_AE43_004005A1D890__INCLUDED_)

