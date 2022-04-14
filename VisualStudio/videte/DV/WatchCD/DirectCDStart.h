// DirectCDStart.h: interface for the CDirectCDStart class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTCDSTART_H__40B4FE64_EB4F_11D3_AE54_004005A1D890__INCLUDED_)
#define AFX_DIRECTCDSTART_H__40B4FE64_EB4F_11D3_AE54_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWatchCDDlg;
class CDirectCDError;

class CDirectCDStart
{

// Construction
public:
	CDirectCDStart(CWatchCDDlg* pWatchCD, CDirectCDError* pDCDError);
	virtual ~CDirectCDStart();

	// Implementation
public:
	
	BOOL IsStarted();
	BOOL IsEmptyCDR();
	BOOL ExecDirectCD();
	BOOL FindWelcomeDlg();
	BOOL FindDriveInfoDlg();
	BOOL FindEmptyCDR();
	BOOL OpenNextDlg();
	void CloseStartDialogs();
	static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);
	DWORD GetIDFinalizeFinishButton();
	HWND GetHdlFinalizeFinishButton();  
	HWND GetHdlDeviceField();
	HWND GetHdlDriveInfoDlg();
	void OpenCDRDrive();
	inline CString GetEjectingString() const;
	inline CString GetUnableToLockString() const;
	inline BOOL GetDeviceLockStatus() const;

	//DirectCD Version 5
	BOOL ExecDirectCDVersion5();
	BOOL FindWelcomeDlgVersion5();
	BOOL FindEmptyCDRVersion5(BOOL &bFoundCDReadyDialogVersion5);

	//Implementation
protected:
	void OnStartExecDirectCD();
	void OnStartFindWelcomeDlg();
	void OnStartFindDriveInfoDlg();
	void OnStartWaitForEmptyCDR();
	void OnStartWaitForCDRRelease();
	void OnStartOpenFormatDlg();
	void OnStartError();

	//DirectCD Version 5	
	void OnStartCloseCDReadyDlgVersion5();

	//Implementation
private:
	static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);

	void ChangeDCDStartStatus(DirectCDStatus DCDStartStatus);
	void ResetDCDStartStatus();
	void InitLanguageStrings();


	// data member
private:

	CWatchCDDlg*		m_pWatchCD;
	CDirectCDError*		m_pDCDError;
	DirectCDStatus		m_DCDStartStatus;
	DirectCDErrorStatus	m_DCDErrorStatus;
	CString				m_sDeviceIsReady;
	CString				m_sDeviceNotReady;
	CString				m_sPathToDirectCD;
	CString				m_sEjecting;
	CString				m_sUnableToLock;
	CString				m_sDirectCDDisc;
	HWND				m_hDCDWatchDog;
	HWND				m_hDCDWelcomeDlg;	
	HWND				m_hDCDDriveInfoDlg;
	HWND				m_hNextButton;
	HWND				m_hDeviceField;
	HWND				m_hFinalizeFinishButton;
	DWORD				m_dwIDButtonNext;
	DWORD				m_dwIDDeviceField;
	DWORD				m_dwIDFinalizeFinishButton;
	DWORD				m_dwTick;
	DWORD				m_dwTick_WaitForEmptyCDR;
	BOOL				m_bDeviceIsLocked;

	//DirectCD Version 5
	BOOL				m_bDirectCDVersion5;
	CString				m_sPathToDirectCDVersion5;
	HWND				m_hFormatCDDialogVersion5;  
	HWND				m_hCDReadyDialogVersion5;
	HWND				m_hCDReadyDialogTextVersion5;
	CString				m_sWndTitleWithDriveVersion5;
	DWORD				m_dwCDReadyDialogTextVersion5;

};


//////////////////////////////////////////////////////////////////////
inline CString CDirectCDStart::GetEjectingString() const
{
	return m_sEjecting;
}

//////////////////////////////////////////////////////////////////////
inline CString CDirectCDStart::GetUnableToLockString() const
{
	return m_sUnableToLock;
}

//////////////////////////////////////////////////////////////////////
inline BOOL CDirectCDStart::GetDeviceLockStatus() const
{
	return m_bDeviceIsLocked;
}

#endif // !defined(AFX_DIRECTCDSTART_H__40B4FE64_EB4F_11D3_AE54_004005A1D890__INCLUDED_)
