#include "StdAfx.h"
#include ".\chive.h"
#include <direct.h>
#include <winnt.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
CHive::CHive(void)
{
	m_sHivePathFile			= _T("");
	m_sHiveBackupPathFile	= _T("");
	m_sSubKey				= _T("");
	m_hKey					= NULL;
	m_pMonitorThread		= NULL;
	m_bRunning				= FALSE;
	m_bAutoMonitorHive		= FALSE;
	m_dwCheckIntervall		= 30000;
	m_dwLastCheckTC			= 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
CHive::~CHive(void)
{
	Close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHive::Open(HKEY hRootKey, const CString& sSubKey, const CString& sHivePathFile, BOOL bAutoMonitorHive/*=TRUE*/, DWORD dwCheckIntervall/*=30000*/)
{
	BOOL bResult = FALSE;

	// Nur ein OPEN ermöglichen
	if (m_bRunning)
		return bResult;

	m_hRootKey = hRootKey;

	DWORD dwResult = RegCreateKeyEx(m_hRootKey,
									sSubKey,
									0,
									NULL,
									REG_OPTION_NON_VOLATILE,
									KEY_ALL_ACCESS,
									NULL,
									&m_hKey,
									NULL);

	if (dwResult == ERROR_SUCCESS)
	{
		CString sPath(sHivePathFile);
		int nIndex = sPath.ReverseFind('\\');
		if (nIndex != -1)
			sPath = sPath.Left(nIndex);
		_tmkdir(sPath);

		m_sHivePathFile			= sHivePathFile;
		m_sHiveBackupPathFile	= m_sHivePathFile+_T(".bak");
		m_sSubKey				= sSubKey;
		m_bAutoMonitorHive		= bAutoMonitorHive;
		m_dwCheckIntervall		= dwCheckIntervall;

		bResult				= TRUE;

		if (m_bAutoMonitorHive)
		{
			// Create an event.
			m_hNotifyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (m_hNotifyEvent)
			{
				// Den kompletten DVRT-Schlüssel auf Änderungen überwachen
				DWORD dwResult = RegNotifyChangeKeyValue(m_hKey, TRUE,	REG_NOTIFY_CHANGE_NAME|
																			REG_NOTIFY_CHANGE_ATTRIBUTES|
																			REG_NOTIFY_CHANGE_LAST_SET|
																			REG_NOTIFY_CHANGE_SECURITY,
																			m_hNotifyEvent,
																			TRUE);	
				if (dwResult == ERROR_SUCCESS)
				{
					m_bRunning		= TRUE;
					m_pMonitorThread = AfxBeginThread(MonitorThread, this);
					if (m_pMonitorThread)
					{
						m_pMonitorThread->m_bAutoDelete = FALSE;	
						
						//m_pReadDataThread->SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);	
						m_pMonitorThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);	
					}
				}
			}
		}
	}
	else
		TRACE(_T("RegCreateKeyEx failed. dwResult:0x%x\n"), dwResult);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHive::Close()
{
	m_bRunning = FALSE;

	if (m_bAutoMonitorHive)
	{
		// Warte bis 'ReadDataThread' beendet ist.
		if (m_pMonitorThread)
			WaitForSingleObject(m_pMonitorThread->m_hThread, 2000);

		delete m_pMonitorThread;
		m_pMonitorThread = NULL;
		
		if (m_hNotifyEvent)
		{
			CloseHandle(m_hNotifyEvent);
			m_hNotifyEvent = NULL;
		}
	}

	if (m_hKey)
	{
		RegCloseKey(m_hKey);
		m_hKey = NULL;
	}

	m_sHivePathFile			= _T("");
	m_sHiveBackupPathFile	= _T("");
	m_sSubKey				= _T("");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHive::RestoreKeys()
{
	DWORD dwResult = !ERROR_SUCCESS;
	BOOL bResult = FALSE;

	if (m_hKey)
	{
		AdjustPrivileges(SE_RESTORE_NAME);

		m_csBackup.Lock();
		if (DoesFileExist(m_sHivePathFile))
			dwResult = RegRestoreKey(m_hKey, m_sHivePathFile, REG_FORCE_RESTORE);
		if ((dwResult != ERROR_SUCCESS) && DoesFileExist(m_sHiveBackupPathFile))
				dwResult = RegRestoreKey(m_hKey, m_sHiveBackupPathFile, REG_FORCE_RESTORE);
		m_csBackup.Unlock();

		if (dwResult == ERROR_SUCCESS)
			bResult = TRUE;
		else
			TRACE(_T("RegRestoreKey failed. dwResult:0x%x\n"), dwResult);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHive::BackupKeys()
{
	DWORD dwResult = 0;
	BOOL bResult = FALSE;

	if (m_hKey)
	{
		m_csBackup.Lock();
		AdjustPrivileges(SE_BACKUP_NAME);

		try
		{
			if (DoesFileExist(m_sHiveBackupPathFile) && DoesFileExist(m_sHivePathFile))
				CFile::Remove(m_sHiveBackupPathFile);

			if (DoesFileExist(m_sHivePathFile))
				CFile::Rename(m_sHivePathFile, m_sHiveBackupPathFile);
		}
		catch(CFileException* pEx)
		{
			int m_cause = pEx->m_cause;
			TRACE(_T("BackupKeys CFile::Rename CFileException cause=%d\n"), m_cause);
		}

		TRACE(_T("Exporting Registry hive to %s...\n"), m_sHivePathFile);

		dwResult = RegSaveKey(m_hKey, m_sHivePathFile, NULL);

		if (dwResult == ERROR_SUCCESS)
			bResult = TRUE;
		else
			TRACE(_T("RegSaveKey failed. dwResult:0x%x\n"), dwResult);

		m_csBackup.Unlock();
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHive::LoadKey(HKEY hRootKey, const CString& sSubKey)
{
	BOOL bResult = FALSE;

	if (hRootKey == NULL)
		hRootKey = m_hRootKey;

	if (hRootKey)
	{
		AdjustPrivileges(SE_RESTORE_NAME);
		AdjustPrivileges(SE_BACKUP_NAME);

		bResult = (RegLoadKey(hRootKey, sSubKey, m_sHivePathFile) == ERROR_SUCCESS);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHive::UnLoadKey(HKEY hRootKey, const CString& sSubKey)
{
	BOOL bResult = FALSE;

	if (hRootKey == NULL)
		hRootKey = m_hRootKey;

	if (hRootKey)
	{
		AdjustPrivileges(SE_RESTORE_NAME);
		AdjustPrivileges(SE_BACKUP_NAME);
		
		bResult = (RegUnLoadKey(hRootKey, sSubKey) == ERROR_SUCCESS);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHive::DeleteKey(HKEY hRootKey, const CString& sSubKey)
{
	BOOL bResult = FALSE;
	if (hRootKey == NULL)
		hRootKey = m_hRootKey;

	if (hRootKey)
		bResult = (RegDeleteKey(hRootKey, sSubKey) == ERROR_SUCCESS);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHive::AdjustPrivileges(LPCTSTR privilege,	DWORD dwAccess /*= SE_PRIVILEGE_ENABLED*/)
{
	HANDLE hToken;              // handle to process token 
	TOKEN_PRIVILEGES tkp;        // ptr. to token structure 
	
	BOOL fResult;                  // system shutdown flag 
	
	// 
	// Get the current process token handle 
	// so we can get debug privilege. 
	
	
	OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) ;
	
	// Get the LUID for debug privilege. 
	
	LookupPrivilegeValue(NULL, privilege, 
		&tkp.Privileges[0].Luid); 
	
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = dwAccess; 
	
	// Get shutdown privilege for this process. 
	
	fResult = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES) NULL, 0); 

	
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT CHive::MonitorThread(LPVOID pData)
{
	CHive* pThis = (CHive*)pData;
	if (pThis)
	{
		pThis->m_dwLastCheckTC = GetTickCount();

		while(pThis->m_bRunning)
		{
			DWORD dwTC = GetTickCount();
			if (dwTC - pThis->m_dwLastCheckTC > pThis->m_dwCheckIntervall)
			{
				pThis->m_dwLastCheckTC = dwTC;

				if (pThis->m_hKey)
				{
					if (WaitForSingleObject(pThis->m_hNotifyEvent, 100) == WAIT_OBJECT_0)
					{
						// Den kompletten DVRT-Schlüssel auf Änderungen überwachen
						DWORD dwResult = RegNotifyChangeKeyValue(pThis->m_hKey, TRUE,	REG_NOTIFY_CHANGE_NAME|
																							REG_NOTIFY_CHANGE_ATTRIBUTES|
																							REG_NOTIFY_CHANGE_LAST_SET|
																							REG_NOTIFY_CHANGE_SECURITY,
																							pThis->m_hNotifyEvent,
																							TRUE);	
						// backup in eigenen hive
						pThis->BackupKeys();
					}
				}
			}
			else
				Sleep(100);
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHive::DoesFileExist(const CString sFileOrDirectory)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(sFileOrDirectory, &FindFileData);
	if (h == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	FindClose(h);
	return TRUE;
}

