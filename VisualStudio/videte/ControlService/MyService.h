#pragma once
#include "ntservice.h"

class CMyService :
	public CNTService
{
public:
	CMyService( BOOL bAutoClose,
				const CString& sControlAppPath,
				const CString& sServiceName,
				const CString& sServiceHelp=_T(""));

	~CMyService(void);

	BOOL FindRunningProcess(const CString& sModule);

protected:
	BOOL StartProcess(const CString &sExePathName,
				const CString &sWorkingDirectory = _T("."),
				BOOL bWait = FALSE,
				DWORD dwStartFlags = SW_SHOWNORMAL,
				DWORD dwStartTime = 30000);
	BOOL CloseProcess();

	HWND FindOutWindowHandle();
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam);

public: //protected:
	virtual BOOL OnStop();
	virtual BOOL OnPause();
	virtual BOOL OnContinue();
	virtual BOOL OnRun();

private:
	STARTUPINFO			m_startUpInfo;
	PROCESS_INFORMATION m_prozessInformation;
	HWND				m_hWndProcess;
	BOOL				m_bRunning;
	BOOL				m_bPaused;
	BOOL				m_bAutoClose;
	CString				m_sCtrlApp;


};
