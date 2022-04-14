#pragma once

class CObserveDirectory
{
public:
	CObserveDirectory(void);
	CObserveDirectory(const CString& sObserveDir, const CString& sBackupDir, BOOL bRecursive=TRUE);
	virtual ~CObserveDirectory(void);

	BOOL Open(const CString& sObserveDir, const CString& sBackupDir, BOOL bRecursive=TRUE);
	void Close();

	BOOL AddFilterExtension(const CString& sExt);
	BOOL AddFilterExtensionList(const CString& sExtList);
	BOOL RemoveFilterExtension(const CString& sExt);
	BOOL RemoveAllFilterExtension();

	BOOL BackupDirectory();
	BOOL RestoreDirectory();

protected:
	virtual BOOL OnNotifyFileAdded(const CString& sFileName);
	virtual BOOL OnNotifyFileRemoved(const CString& sFileName);
	virtual BOOL OnNotifyFileModify(const CString& sFileName);
	virtual BOOL OnNotifyFileRenamedOldName(const CString& sFileName);
	virtual BOOL OnNotifyFileRenamedNewName(const CString& sFileName);
	virtual BOOL CheckFileExtensions(const CString& sSrcFileName);

private:
	static UINT MonitorThread(LPVOID pData);
	BOOL RenameFile(const CString& sOldName, const CString& sNewName);
	BOOL CreateDirectoryRekursive(const CString& sDir);
	BOOL DeleteDirectoryRecursiv(const CString& sDir);
	BOOL CopyDirectoryRecursiv(const CString& sSrcDir, const CString& sDstDir);
	BOOL CopyFile(const CString& sSrcFileName, const CString& sDstFileName);
	BOOL RemoveFile(const CString& sFileName);
	BOOL DoesFileExist(const CString sFileOrDirectory);	
	BOOL IsDirectory(const CString& sFileName);
	
	HANDLE Lock(const CString& sFileName);
	void Unlock(HANDLE hFile);

	CWinThread*	m_pMonitorThread;
	BOOL		m_bRunning;

	HANDLE	m_hObservedDirectory;
	CString m_sObservedDirectory;
	CString m_sBackupDirectory;
	CString m_sOldFileName;
	CString m_sNewFileName;

	CStringArray m_FilterExts;

	BOOL	m_bRecursive;
};
