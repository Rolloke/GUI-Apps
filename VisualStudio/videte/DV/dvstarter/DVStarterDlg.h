// DVStarterDlg.h : header file
//

#if !defined(AFX_DVSTARTERDLG_H__2E0EDA37_A6F8_11D3_8DFA_004005A11E32__INCLUDED_)
#define AFX_DVSTARTERDLG_H__2E0EDA37_A6F8_11D3_8DFA_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CApplication.h"
#include "CWatchDog.h"
#include "Piezo.h"
#include "CIPCDrive.h"

#define WM_USER_RTERROR_ARRIVED WM_USER + 1

/////////////////////////////////////////////////////////////////////////////
// CDVStarterDlg dialog
class CDVStarterDlg : public CDialog
{
// Construction
public:
	CDVStarterDlg(CWnd* pParent = NULL);	// standard constructor
	~CDVStarterDlg();


typedef BOOL  (WINAPI *ENUMPROCESS)(DWORD *lpidProcess, DWORD cb, DWORD *cbNeeded);
typedef BOOL  (WINAPI *GETPROCESSMEMORYINFO)(HANDLE Process, PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb);
typedef BOOL  (WINAPI *ENUMPROCESSMODULES)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded);
typedef DWORD (WINAPI *GETMODULEBASENAME)(HANDLE hProcess, HMODULE hModule, LPTSTR lpBaseName, DWORD nSize);

enum FS_Producer
{
	FSP_INVALID = 0,
	FSP_UDF_ADAPTEC_DIRECTCD = 1,
	FSP_UDF_CEQUADRAT_PACKETCD = 2,
	FSP_IOMEGA_JAZ = 3,
	FSP_HARDDISK = 4,
	FSP_FLOPPYDISK = 5
};

enum DCD_Version
{
	DCD_VERSION_UNKNOWN = 1000,
	DCD_VERSION_26		= 1001,
	DCD_VERSION_30		= 1002,
};

// Dialog Data
	//{{AFX_DATA(CDVStarterDlg)
	enum { IDD = IDD_DVSTARTER_DIALOG };
	CString	m_sTotalUsedMemory;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVStarterDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDVStarterDlg)
	virtual void OnOk();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnRTErrorArrived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnExit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPiezoUseChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCheckRTE(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg BOOL OnDeviceChange( UINT nEventType, DWORD dwData );
	afx_msg void OnBnClickedExit();
	afx_msg BOOL OnQueryEndSession( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


protected:
	BOOL IsNetworkInstalled();
	BOOL IsISDNInstalled();
	BOOL IsNetworkAllowed();
	BOOL IsISDNAllowed();
	
	void ReCheckISDNLAN();

	void StartRTErrorThread();
	void StopRTErrorThread();
	static UINT RTErrorThreadProc(LPVOID pContext);
	int CheckDCDVersion(CWK_Profile* pwkProf);
	int CheckBackupProgram();
	void InitRegForDirectCD();
	void InitRegForDirectCDVers5();
	void MoveDirectCDFromRunToDVRT();
	void MoveDirectCDVers5FromRunToDVRT();
	void QueryMemoryInformation();
	void EnumerateMemoryInfo(DWORD processID, int nI, BOOL bTraceToLogFile);
	BOOL CheckIfPowerLossed();
	BOOL CheckHDTemperaturs();
	BOOL CheckSMARTparameters();

private:
	void CheckRuntimeErrors();
	static CDVStarterDlg*	m_pThis;
	BOOL					m_bRun;
	CWinThread*				m_pRTErrorThread;

	CApplication			m_AppJaCob1;
	CApplication			m_AppJaCob2;
	CApplication			m_AppJaCob3;
	CApplication			m_AppJaCob4;
	CApplication			m_AppSaVic1;
	CApplication			m_AppSaVic2;
	CApplication			m_AppSaVic3;
	CApplication			m_AppSaVic4;
	CApplication			m_AppTasha1;
	CApplication			m_AppTasha2;
	CApplication			m_AppTasha3;
	CApplication			m_AppTasha4;
	CApplication			m_AppQ;

	CApplication			m_AppDVProcess;
	CApplication			m_AppDVStorage;
	CApplication			m_AppDVClient;
	CApplication			m_AppSocket;
	CApplication			m_AppISDN1;
	CApplication			m_AppISDN2;
	CApplication			m_AppDVHook;

	CApplication			m_AppACDC;
	CApplication			m_AppCommUnit;
	CApplication			m_AppAudioUnit;
	CApplication			m_AppHealthControl;

	CWatchDog				m_WatchDog;
	CPiezo					m_Piezo;
	int						m_nTimerIntervall;
	BOOL					m_bTraceMemInf;

	HINSTANCE				m_hPsApiDll;
	ENUMPROCESS				m_EnumProcesses;
	GETPROCESSMEMORYINFO	m_GetProcessMemoryInfo;
	ENUMPROCESSMODULES		m_EnumProcessModules;
	GETMODULEBASENAME		m_GetModuleBaseName;
	CString					m_sAvailPhysMemory;
	CString					m_sAvailPageMemory;
	CString					m_sUsedPhysMemory;
	CString					m_sUsedPageMemory;
	CString					m_sMemoryLoad;
	CString					m_sAllocMemory;
	CString					m_sAvailSumme;

	DWORD					m_dwTotalUsedMemory;

	int						m_nNumbersOfJaCobs; // Anzahl der im System gefundenen und zugelassenen JaCobs
	int						m_nNumbersOfSaVics; // Anzahl der im System gefundenen und zugelassenen SaVics
	int						m_nNumbersOfTashas; // Anzahl der im System gefundenen und zugelassenen Tashas
	int						m_nNumbersOfQs;		// Anzahl der im System gefundenen und zugelassenen UDP-Boarss (Qs)

	BOOL					m_bIsNetworkInstalled;
	BOOL					m_bIsISDNInstalled;

	int						m_nMaxCheckedDrives;
	int*					m_pnLastTemperatures;
	BOOL*					m_pbSMARTFailureReported;

	BOOL					m_bQueryEndSessionReturn;
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVSTARTERDLG_H__2E0EDA37_A6F8_11D3_8DFA_004005A11E32__INCLUDED_)
