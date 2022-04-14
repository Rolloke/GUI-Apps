#pragma once
#include <afxmt.h>

class CHive
{
public:
	CHive(void);
	virtual ~CHive(void);

	BOOL Open(HKEY hRootKey, const CString& sSubKey, const CString& sHivePathFile, BOOL bAutoMonitorHive=TRUE, DWORD dwCheckIntervall=30000);
	void Close();

	BOOL BackupKeys();
	BOOL RestoreKeys();

	BOOL LoadKey(HKEY hRootKey, const CString& sSubKey);
	BOOL UnLoadKey(HKEY hRootKey, const CString& sSubKey);
	BOOL DeleteKey(HKEY hRootKey, const CString& sSubKey);

private:
	BOOL Monitor();
	BOOL DoesFileExist(const CString sFileOrDirectory);
	BOOL AdjustPrivileges(LPCTSTR privilege, DWORD dwAccess = SE_PRIVILEGE_ENABLED);
	static UINT MonitorThread(LPVOID pData);

private:
	HANDLE	m_hNotifyEvent;
	HKEY	m_hRootKey;
	HKEY	m_hKey;

	BOOL	m_bRunning;
	BOOL	m_bAutoMonitorHive;
	DWORD	m_dwCheckIntervall;
	DWORD	m_dwLastCheckTC;

	CWinThread*	m_pMonitorThread;
	CString m_sHivePathFile;
	CString m_sHiveBackupPathFile;
	CString m_sSubKey;
	CCriticalSection m_csBackup;
};
