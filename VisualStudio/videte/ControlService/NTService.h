#pragma once

class CNTService
{
public:
	CNTService(const CString& sServiceName, const CString& sDisplayName, const CString& sServiceHelp);
	~CNTService(void);

	BOOL InstallService();
	BOOL DeleteService();
	BOOL StartService();
	BOOL EndService();
	BOOL StartDispatcher();
	BOOL PauseService();
	BOOL ContinueService();
	BOOL GetStatus(SERVICE_STATUS& Status);

protected:
	static void WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
	static void WINAPI ServiceCtrlHandler(DWORD Opcode);
	
protected:
	virtual BOOL OnStop();
	virtual BOOL OnPause();
	virtual BOOL OnContinue();
	virtual BOOL OnRun();

	static CNTService*	m_pThis;
private:
	SERVICE_STATUS			m_ServiceStatus;
	SERVICE_STATUS_HANDLE	m_ServiceStatusHandle;

	CString m_sServiceName;
	CString m_sDisplayName;
	CString m_sServiceHelp;


};
