/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SecurityLauncher.h $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/SecurityLauncher.h $
// CHECKIN:		$Date: 27.07.06 11:59 $
// VERSION:		$Revision: 122 $
// LAST CHANGE:	$Modtime: 26.07.06 14:09 $
// BY AUTHOR:	$Author: Tomas.krogh $
// $Nokeywords:$

#ifndef _SecurityLauncher_H_
#define _SecurityLauncher_H_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "Applications.h"
#include "WatchDog.h"
#include "MeszMez.h"

//////////////////////////////////////////////////////////////////////
typedef DWORD (APIENTRY* GET_LONG_PATH_NAME_EX_PTR)(LPCTSTR,LPTSTR,DWORD);

/////////////////////////////////////////////////////////////////////////////
//static TCHAR BASED_CODE szSection[] = _T("SecurityLauncher");
static TCHAR BASED_CODE szSectionDebug[] = _T("SecurityLauncher\\Debug");

/////////////////////////////////////////////////////////////////////////////
class CBackGroundWnd;

/////////////////////////////////////////////////////////////////////////////
#define LM_NORMAL		  0
#define LM_BACKUP		  1
/////////////////////////////////////////////////////////////////////////////
#define TIMER_SECONDS	 20
#define TIMER_15MIN		 45

#define ID_ERROR_ARRIVED	0x7000
#define ID_QUITE_EXIT		0x7001

#define DEFAULT_ERRORFILE		_T("c:\\rterrors.dat")

#define VIRTUAL_ALARM_SET		-50 // -50 °C wird es wohl nicht geben

/////////////////////////////////////////////////////////////////////////////
UINT	ErrorThreadProc(LPVOID lpParam);
/////////////////////////////////////////////////////////////////////////////
class CLauncherControl;
class CUpdateRecord;
class CErrorDialog;
class CWK_Dongle;

/////////////////////////////////////////////////////////////////////////////
class CSecurityLauncherApp : public CWinApp, public COsVersionInfo
{
	// construction/destruction
	friend class CIPCInputVirtualAlarm;
public:
	CSecurityLauncherApp();
	~CSecurityLauncherApp();
	// attributes
public:
	inline CString GetWorkingDirectory();
	inline CString GetWindowsDirectory();
	inline CString GetSystemDirectory();
	inline CString GetTempDirectory();
	inline CString GetName();

	inline CString GetVersion();
	inline CString GetOsVersion();
	inline CString GetOsVersionShort();
	inline int	   GetAlarmPercent();
	inline int	   GetISDNBChannel();
	inline CWatchDog& GetWatchDog();
	inline BOOL	   UseUpdateHandler() const;
	int GetMonitors() { return m_nMonitors;};
	CBackGroundWnd* GetBackgroundWnd(int n);
	LPCTSTR GetModuleSection();
	int		GetIntValue(LPCTSTR sValue, int nDefault=0);
	inline BOOL	   ShallReboot() const;

	void CheckSMARTparameters();
	void CheckHDDTemperatures();
	void AddNewError(CWK_RunTimeError* pNewRTE);

	// operations
public:
	// actualizing in certain time intervals
	void Actualize20s();
	void Actualize15min();
	void ActualizeHour();
	void ActualizeDay();

	void IncreaseGPCounter();
	void IncreaseRTECounter();
	void UpdateMySelf(CUpdateRecord* pUR);
	void SetExecuteAtExit(const CString& s);
	void SetRebootAtExit(BOOL bRebootAtExit);
	void SetShutdownAtExit(BOOL bShutdownAtExit);
	void SetShutdownAtExitShell(BOOL bShutdownAtExitShell);
	void QuiteExit();

	int	 SafeExitInstance();
	void TerminateAll(BOOL bRestartTimer = FALSE);

	BOOL Login(BOOL bForceSuperVisor = TRUE);

	BOOL AmIShell();
	BOOL IsBackupMode();
	BOOL IsNormalMode();
	int  GetMode();
	BOOL DoShowError(CWK_RunTimeError*pRTE);
	BOOL DoPopupErrorDlg(CWK_RunTimeError*pRTE);
	
	void OnUpdate(LPARAM lParam);
	void OnNewError();

	void OnTimeChangedBySoftware();
	void OnConfirmSelfcheck(WK_ApplicationId AppId);

	LRESULT OnApplication(WK_ApplicationId wai, LPARAM lParam);
	void StartClientApp(WK_ApplicationId Id, WORD wUserID=SECID_NO_SUBID);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecurityLauncherApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSecurityLauncherApp)
	afx_msg void OnDatabase();
	afx_msg void OnUpdateDatabase(CCmdUI* pCmdUI);
	afx_msg void OnVision();
	afx_msg void OnUpdateVision(CCmdUI* pCmdUI);
	afx_msg void OnSupervisor();
	afx_msg void OnUpdateSupervisor(CCmdUI* pCmdUI);
	afx_msg void OnSpace();
	afx_msg void OnError();
	afx_msg void OnProductView();
	afx_msg void OnBackup();
	afx_msg void OnDisk();
	afx_msg void OnVersion();
	afx_msg void OnSDIConfig();
	afx_msg void OnUpdateProductView(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVersion(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSdiConfig(CCmdUI* pCmdUI);
	afx_msg void OnExplorer();
	afx_msg void OnExplorerShell();
	afx_msg void OnLauncherShell();
	afx_msg void OnRestart();
	afx_msg void OnLogView();
	afx_msg void OnUpdateLogView(CCmdUI* pCmdUI);
	afx_msg void OnDatetime();
	afx_msg void OnEuroChange();
	afx_msg void OnImageCompare();
	afx_msg void OnUpdateImageCompare(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHandler();
	afx_msg void OnAppExit();
	afx_msg void OnSoftwareUpdate();
	afx_msg void OnUpdateSoftwareUpdate(CCmdUI *pCmdUI);
	afx_msg void OnExtensionCode();
	afx_msg void OnUpdateExtensionCode(CCmdUI *pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

// Implementation
private:
	// initializing code
	BOOL CreateCheckLogDir();
	void CheckOSVersion();
	void CheckSoftwareVersion();
	BOOL TimeCheck();
	BOOL TimeChangeCheck(const CSystemTime& st);
	void RegisterWindowClass();
	void ReconnectToServer();
	BOOL IsRegistryEmpty();
	void StartErrorThread();
	void StopErrorThread();
	void InitPathes();
	BOOL InitWindow();
	BOOL InitApps();
	void InitWatchDog();
	void PiezoCheck();
	void InitShell();
	void TraceSystemInformation();
	void OnErrorArrived(CWK_RunTimeError* pLastError);
	void ScanChk();
	void WriteErrorFile();
	//
/*
	BOOL InitializeDrive(int nDrive);
	void FormatAllUnformatedFixedVolumes();
*/
	// start code
	void StartAll();
	void RestartAll();

	BOOL AdjustPrivileges(LPCTSTR privilege, DWORD dwAccess = SE_PRIVILEGE_ENABLED);

	// check code
	void ErrorReboot(BOOL bForce=FALSE); // cleans up and reboots 
	void CleanUp();

	// paging file
	void GetPagingDrive();

	// update code
	void UpdateMySelfInSecondInstance();
	void DeleteNewExeInThirdInstance();
	void DisableWriteCache();

public:
	void SysDown();

private:
	void SysReboot();
	void RebootWinExplorer();
	void StartExplorer();
	void RebootWinLauncher();
	BOOL RebootBackup();
	void OnBackupFinished();

	void MakeMeShell();
	void MakeExplorerShell();

	// public members
public:
	LPCTSTR GetSection();
	LPCTSTR GetErrorDat();
	CApplications	m_Apps;
	CWK_Dongle*		m_pDongle;

	CBrush	   m_BkGndBrush;
	COLORREF   m_cTextColor;
	CBitmap    m_videteLogo;

	// private members
private:

	volatile BOOL	m_bRun;
	CWinThread* m_pErrorThread;

	// Run Mode
	int			m_Mode;

	// logfile
	CStdioFile  m_LogFile;

	// watch dog
	CWatchDog	m_WatchDog;
	WORD		m_wWatchDogTime;


	// update control
	CLauncherControl* m_pLauncherControl;

	// pathes
	CString	m_sWorkingDirectory;
	CString	m_sWindowsDirectory;
	CString	m_sSystemDirectory;
	CString	m_sTempDirectory;
	CString m_sPagingFile;
	CString m_sErrorDat;

	// Version
	CString	m_sVersion;
	CString m_sOsVersion;
	CString m_sOsVersionShort;
	CString m_sName;

	// misc.
	int		m_iActualize;
	int		m_iAlarmPercent;
	int		m_iISDNBChannel;
	CMemoryStatus m_Memory;
	CMeszMez	m_MeszMez;

	// shell stuff
	BOOL	m_bAmIShell;
	BOOL	m_bRebootAtExit;
	BOOL	m_bShutDownAtExit;
	BOOL	m_bShutDownAtExitShell;
	CString m_sExecuteAtExit;

	// Monitors and Backgroundwindows
	CBackGroundWnd**	   m_ppBackGroundWnd;
	int     m_nMonitors;
	HHOOK	m_hHook;

	// Updatehandler
	BOOL	m_bUseUpdateHandler;
	BOOL	m_bDateTimeDlgStarted;;

	// counters for run time error handling
	CSystemTime m_SystemTime;
	DWORD	m_dwGPCounterHour;
	DWORD	m_dwRTECounterHour;
	DWORD	m_dwGPCounterDay;
	DWORD	m_dwRTECounterDay;

	CSecID	m_TemporaryUserID;

	CWK_RunTimeErrors	m_newErrors;
	CWK_RunTimeErrors	m_Errors;
	BOOL				m_bWriteErrorFile;
	CErrorDialog*		m_pErrorDialog;
	int					m_nMaxErrors;
	DWORD				m_dwShowErrors;
	BOOL				m_bShallReboot;
	int					m_nMaxCheckedDrives;
	int*				m_pnLastTemperatures;
	BOOL*				m_pbSMARTFailureReported;
	DWORD				m_dwRTE_TemperatureBits;
	DWORD				m_dwRTE_FanBits;

	static WK_ApplicationId gm_AppButtons[];
	static BOOL gm_bAppStarted[];

	// Virtuelle Alarme
	CIPCInputVirtualAlarm *m_pCIPCVirtualAlarm;
	friend UINT	ErrorThreadProc(LPVOID lpParam);
	friend class CErrorDialog;
	friend class CMeszMez;
public:
};
#include "launcher.inl"
/////////////////////////////////////////////////////////////////////////////
extern CSecurityLauncherApp theApp;
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK ShellProc(int nCode,WPARAM wParam,LPARAM lParam);
/////////////////////////////////////////////////////////////////////////////
#endif
