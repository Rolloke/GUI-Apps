#include "StdAfx.h"
#include ".\cobservedirectory.h"
#include <direct.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
CObserveDirectory::CObserveDirectory(void)
{
	m_hObservedDirectory	= INVALID_HANDLE_VALUE;
	m_sObservedDirectory	= "";
	m_bRecursive			= TRUE;
	m_sOldFileName			= "";
	m_sNewFileName			= "";
	m_pMonitorThread		= NULL;
	m_bRunning				= FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
CObserveDirectory::CObserveDirectory(const CString& sObserveDir, const CString& sBackupDir, BOOL bRecursive/*=TRUE*/)
{
	m_hObservedDirectory	= INVALID_HANDLE_VALUE;
	m_sObservedDirectory	= sObserveDir;
	m_sBackupDirectory		= sBackupDir;
	m_bRecursive			= bRecursive;
	m_sOldFileName			= "";
	m_sNewFileName			= "";
	m_pMonitorThread		= NULL;
	m_bRunning				= FALSE;

	Open(m_sObservedDirectory, sBackupDir, m_bRecursive);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
CObserveDirectory::~CObserveDirectory(void)
{
	Close();

	BackupDirectory();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::Open(const CString& sObserveDir, const CString& sBackupDir, BOOL bRecursive/*=TRUE*/)
{
	BOOL bResult = FALSE;

	m_sObservedDirectory	= sObserveDir;
	m_sBackupDirectory		= sBackupDir;
	m_bRecursive			= bRecursive;

	m_hObservedDirectory = CreateFile(m_sObservedDirectory,
								GENERIC_READ,
								FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
								NULL,
								OPEN_EXISTING,
								FILE_FLAG_BACKUP_SEMANTICS,
								NULL);
	if (m_hObservedDirectory != INVALID_HANDLE_VALUE)
	{
		m_bRunning = TRUE;
		m_pMonitorThread = AfxBeginThread(MonitorThread, this);
		if (m_pMonitorThread)
		{
			m_pMonitorThread->m_bAutoDelete = FALSE;	
			m_pMonitorThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);

			bResult = TRUE;
		}
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void  CObserveDirectory::Close()
{
	if (m_pMonitorThread)
	{
		m_bRunning = FALSE;

		// Exit ReadDirectoryChangesW...
		CString sTmpFile = m_sObservedDirectory+_T(".tmp");
		CFile fileTmp(sTmpFile, CFile::modeCreate);

		WaitForSingleObject(m_pMonitorThread->m_hThread, 2000);

		delete m_pMonitorThread;
		m_pMonitorThread = NULL;

		fileTmp.Close();
		CFile::Remove(sTmpFile);
	}

	if (m_hObservedDirectory != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hObservedDirectory);
		m_hObservedDirectory = INVALID_HANDLE_VALUE;
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::BackupDirectory()
{
	BOOL bResult = FALSE;

	CopyDirectoryRecursiv(m_sObservedDirectory, m_sBackupDirectory);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::RestoreDirectory()
{
	BOOL bResult = FALSE;

	CopyDirectoryRecursiv(m_sBackupDirectory, m_sObservedDirectory);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define FILE_NOTIFICATION_BUFFER_SIZE 128000

UINT CObserveDirectory::MonitorThread(LPVOID pData)
{
	CObserveDirectory* pThis = (CObserveDirectory*)pData;
	if (pThis)
	{
		FILE_NOTIFY_INFORMATION* pFileNotifyInfo = (FILE_NOTIFY_INFORMATION*)new BYTE[FILE_NOTIFICATION_BUFFER_SIZE];
		while(pThis->m_bRunning && (pThis->m_hObservedDirectory != INVALID_HANDLE_VALUE))
		{
			ZeroMemory(pFileNotifyInfo, FILE_NOTIFICATION_BUFFER_SIZE);
			DWORD BytesReturned = 0;
			if (ReadDirectoryChangesW(	pThis->m_hObservedDirectory,
										pFileNotifyInfo,
										FILE_NOTIFICATION_BUFFER_SIZE,
										pThis->m_bRecursive,
										FILE_NOTIFY_CHANGE_FILE_NAME |
										FILE_NOTIFY_CHANGE_DIR_NAME |
										FILE_NOTIFY_CHANGE_ATTRIBUTES |
										FILE_NOTIFY_CHANGE_LAST_WRITE |
										FILE_NOTIFY_CHANGE_SIZE |
										FILE_NOTIFY_CHANGE_CREATION,
										&BytesReturned,
										NULL,
										NULL))
			{
				if (!pThis->m_bRunning)
					break;

				if (BytesReturned == 0)
				{
					Beep(1000, 100);
					TRACE(_T("ReadDirectoryChangesW failed. Buffer too small\n"));
				}

				FILE_NOTIFY_INFORMATION* pFNI=pFileNotifyInfo;
				HANDLE hFile = INVALID_HANDLE_VALUE;

				while (true && BytesReturned>0)
				{
					CString sFileName(pFNI->FileName, pFNI->FileNameLength/sizeof(WCHAR));
					sFileName = pThis->m_sObservedDirectory+sFileName;
					Sleep(100);

					switch (pFNI->Action)
					{
						case FILE_ACTION_ADDED:
							hFile = pThis->Lock(sFileName);
							pThis->OnNotifyFileAdded(sFileName);
							pThis->Unlock(hFile);
							break;
						case FILE_ACTION_REMOVED:
							pThis->OnNotifyFileRemoved(sFileName);
							break;
						case FILE_ACTION_MODIFIED:
							hFile = pThis->Lock(sFileName);
							pThis->OnNotifyFileModify(sFileName);
							pThis->Unlock(hFile);
							break;
						case FILE_ACTION_RENAMED_OLD_NAME:
							pThis->OnNotifyFileRenamedOldName(sFileName);
							break;
						case FILE_ACTION_RENAMED_NEW_NAME:
							hFile = pThis->Lock(sFileName);
							pThis->OnNotifyFileRenamedNewName(sFileName);
							pThis->Unlock(hFile);
							break;
						default:
							break;
					}
					if (pFNI->NextEntryOffset == 0)
						break;
					pFNI = (FILE_NOTIFY_INFORMATION*)((BYTE*)pFNI + pFNI->NextEntryOffset);
				}
			}
		}
		delete pFileNotifyInfo;
		pFileNotifyInfo = NULL;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::OnNotifyFileAdded(const CString& sSrcFileName)
{
	BOOL bResult = FALSE;
	TRACE(_T("OnNotifyFileAdded: file=%s\n"), sSrcFileName);

	CString sDstFileName = m_sBackupDirectory+sSrcFileName.Mid(m_sObservedDirectory.GetLength());

	if (IsDirectory(sSrcFileName))
		bResult = CreateDirectoryRekursive(sDstFileName);
	else
	{
		// Den Dateinamnen vom Pfad trennen und Verzeichnisse anlegen.
		int nIndex = sDstFileName.ReverseFind('\\');
		if (nIndex != -1)
			CreateDirectoryRekursive(sDstFileName.Left(nIndex));
		bResult = CopyFile(sSrcFileName, sDstFileName);
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::OnNotifyFileRemoved(const CString& sSrcFileName)
{
	BOOL bResult = FALSE;

	TRACE(_T("OnNotifyFileRemoved: file=%s\n"), sSrcFileName);

	CString sDstFileName = m_sBackupDirectory+sSrcFileName.Mid(m_sObservedDirectory.GetLength());

	if (IsDirectory(sDstFileName))
		bResult = DeleteDirectoryRecursiv(sDstFileName);
	else
		bResult = RemoveFile(sDstFileName);

	if (bResult)
		TRACE(_T("RemoveFile %s\n"), sDstFileName);
	else
	{
		DWORD dwError = GetLastError();
		TRACE(_T("RemoveFile failed Errorcode=%d\n"), dwError);
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::OnNotifyFileModify(const CString& sSrcFileName)
{
	BOOL bResult = FALSE;
	TRACE(_T("OnNotifyFileModify: file=%s\n"), sSrcFileName);

	CString sDstFileName = m_sBackupDirectory+sSrcFileName.Mid(m_sObservedDirectory.GetLength());
	
	if (IsDirectory(sSrcFileName))
		bResult = CreateDirectoryRekursive(sDstFileName);
	else
	{
		// Den Dateinamnen vom Pfad trennen und Verzeichnisse anlegen.
		int nIndex = sDstFileName.ReverseFind('\\');
		if (nIndex != -1)
			CreateDirectoryRekursive(sDstFileName.Left(nIndex));
		bResult = CopyFile(sSrcFileName, sDstFileName);
	}

//	if (!IsDirectory(sSrcFileName))
//		bResult = CopyFile(sSrcFileName, sDstFileName);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::OnNotifyFileRenamedOldName(const CString& sFileName)
{
	BOOL bResult = FALSE;
	
	TRACE(_T("OnNotifyFileRenamedOldName: file=%s\n"), sFileName);

	m_sOldFileName = sFileName;

	if (!m_sOldFileName.IsEmpty() && !m_sNewFileName.IsEmpty())
	{
		RenameFile(m_sOldFileName, m_sNewFileName);
		m_sNewFileName = "";
		m_sOldFileName = "";
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::OnNotifyFileRenamedNewName(const CString& sFileName)
{
	BOOL bResult = FALSE;
	
	TRACE(_T("OnNotifyFileRenamedNewName: file=%s\n"), sFileName);
	
	m_sNewFileName = sFileName;

	if (!m_sOldFileName.IsEmpty() && !m_sNewFileName.IsEmpty())
	{
		RenameFile(m_sOldFileName, m_sNewFileName);

		m_sNewFileName = "";
		m_sOldFileName = "";
	}

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::RenameFile(const CString& sOldFileName, const CString& sNewFileName)
{
	BOOL bResult = FALSE;
	
	CString sOldBackupFileName = m_sBackupDirectory+sOldFileName.Mid(m_sObservedDirectory.GetLength());
	CString sNewBackupFileName = m_sBackupDirectory+sNewFileName.Mid(m_sObservedDirectory.GetLength());
	
	if (!CheckFileExtensions(sOldFileName))
	{
		TRACE(_T("Did not rename File (%s to %s)  - Filtered by file extension\n"), sOldFileName, sNewFileName);
		return TRUE;
	}

	TRACE(_T("--> RenameFile file=%s to %s\n"), sOldFileName, sNewFileName);

	try
	{
		if (DoesFileExist(sOldBackupFileName))
			bResult = MoveFileEx(sOldBackupFileName, sNewBackupFileName, MOVEFILE_COPY_ALLOWED);
		else
			bResult = CopyFile(sNewFileName, sNewBackupFileName);

	}
	catch(...)
	{
		TRACE(_T("MoveFileEx failed\n"));
	}

	return bResult;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE CObserveDirectory::Lock(const CString& sFileName)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	
	// Wenns ein Verzeichnis ist, dann gleich wieder raus hier.
	if (IsDirectory(sFileName))
		return hFile;
	
	// Warte bis Filehandle verfügbar ist.
	int nCounter = 500;
	TRACE(_T("CObserveDirectory::Locking... <%s>\n"), sFileName);
	do
	{
		hFile =  CreateFile(sFileName, GENERIC_READ, FILE_SHARE_READ|
													 FILE_SHARE_WRITE|
													 FILE_SHARE_DELETE,
													 NULL, OPEN_EXISTING,
													 FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			Sleep(10);
	} while ((hFile == INVALID_HANDLE_VALUE) && (nCounter-- > 0));
	TRACE(_T("CObserveDirectory::Locked <%s>\n"), sFileName);

	return hFile;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void CObserveDirectory::Unlock(HANDLE hFile)
{
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		TRACE(_T("CObserveDirectory::unlocked\n"));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::CreateDirectoryRekursive(const CString& sDirectory)
{
	CString sPath	= _T("");
	CString sDir	= sDirectory;
	int nIndex		= 2;

	if (sDir.GetLength() >=3)
	{
		if (sDir[sDir.GetLength()-1]!='\\') 
			sDir += '\\';
		do
		{
			nIndex = sDir.Find('\\', nIndex+1);
			if (nIndex != -1)
			{
				sPath = sDir.Left(nIndex);
				if (!DoesFileExist(sPath))
				{
					_tmkdir(sPath);

					TRACE(_T("Create Directory <%s>\n"), sPath);
				}
			}
			
		}while(nIndex != -1);
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::DoesFileExist(const CString sFileOrDirectory)
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

/////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::CopyFile(const CString& sSrcFileName, const CString& sDstFileName)
{
	BOOL bResult = FALSE;
	
	// Einer der Excluded Extensions?
	if (!CheckFileExtensions(sSrcFileName))
	{
		TRACE(_T("Did not copy File (%s to %s)  - Filtered by file extension\n"), sSrcFileName, sDstFileName);
		return TRUE;
	}

#if (0)
	CString sFrom = sSrcFileName + '\0';
	CString sTo   = sDstFileName + '\0';

	SHFILEOPSTRUCT	FileOp;
	FileOp.fAnyOperationsAborted = FALSE;
	FileOp.fFlags = FOF_SILENT|FOF_NOERRORUI|FOF_NOCONFIRMMKDIR|FOF_NOCONFIRMATION;
	FileOp.hNameMappings = NULL;
	FileOp.hwnd = NULL;
	FileOp.lpszProgressTitle = NULL;
	FileOp.pFrom = (LPCTSTR)sFrom;
	FileOp.pTo = (LPCTSTR)sTo;
	FileOp.wFunc = FO_COPY;

	bResult = (SHFileOperation(&FileOp) == 0);

	if (bResult)
		TRACE(_T("SHFileOperation 'FO_COPY' %s to %s\n"),sSrcFileName, sDstFileName);
	else
	{
		DWORD dwError = GetLastError();
		TRACE(_T("SHFileOperation 'FO_COPY' failed to copy %s to %s <Errorcode=%d>\n"), sSrcFileName, sDstFileName, dwError);
	}
#else
	bResult = ::CopyFile(sSrcFileName, sDstFileName, FALSE);
	if (bResult)
		TRACE(_T("CopyFile %s to %s\n"),sSrcFileName, sDstFileName);
	else
	{
		DWORD dwError = GetLastError();
		TRACE(_T("failed to copy %s to %s <Errorcode=%d>\n"), sSrcFileName, sDstFileName, dwError);
	}
#endif
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::RemoveFile(const CString& sFileName)
{
	if (!CheckFileExtensions(sFileName))
	{
		TRACE(_T("Did not remove File (%s)  - Filtered by file extension\n"), sFileName);
		return TRUE;
	}

	return DeleteFile(sFileName);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::DeleteDirectoryRecursiv(const CString& sDir)
{
	HANDLE hF;
	WIN32_FIND_DATA FindFileData;
	BOOL bErr = FALSE;
	CString sPath = sDir;

	if (sPath.GetLength()==0)
		return FALSE;
	
	if (sPath[sPath.GetLength()-1]!='\\') 
		sPath += '\\';

	CString sSearch = sPath + _T("*.*");

	hF = FindFirstFile(LPCTSTR(sSearch), &FindFileData);
	if (hF != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((_tcscmp(FindFileData.cFileName, _T(".")) != 0) && (_tcscmp(FindFileData.cFileName, _T("..")) != 0))
			{
				CString sFile = sPath+FindFileData.cFileName;
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					bErr |= !DeleteDirectoryRecursiv(sFile);
				else
					bErr |= !RemoveFile(sFile);
			}
		}
		while (FindNextFile(hF,&FindFileData));
	
		FindClose(hF);
	}

	bErr |= !RemoveDirectory(sDir);

	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::CopyDirectoryRecursiv(const CString& sSrcDir, const CString& sDestDir)
{
	BOOL bErr = FALSE;

	HANDLE hF;
	WIN32_FIND_DATA FindFileData;
	CString sSrcPath = sSrcDir;
	
	if (sSrcPath[sSrcPath.GetLength()-1]!='\\') 
		sSrcPath += '\\';

	CString sSearch = sSrcPath + _T("*.*");

	CreateDirectoryRekursive(sDestDir);

	hF = FindFirstFile(LPCTSTR(sSearch), &FindFileData);
	if (hF != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ( (_tcscmp(FindFileData.cFileName, _T(".")) != 0) && (_tcscmp(FindFileData.cFileName, _T("..")) != 0))
			{
				CString sSrcFile = sSrcPath+FindFileData.cFileName;
				CString sDstFile = sDestDir+sSrcFile.Mid(sSrcDir.GetLength());		
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					bErr |= !CopyDirectoryRecursiv(sSrcFile, sDstFile);
				else
					bErr |= !CopyFile(sSrcFile, sDstFile);
			}
		}
		while (FindNextFile(hF,&FindFileData));
	
		FindClose(hF);
	}
	return !bErr;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::IsDirectory(const CString& sFileName)
{
	BOOL bResult = FALSE;

	DWORD dwAttrib = GetFileAttributes(sFileName);
	if ((dwAttrib != INVALID_FILE_ATTRIBUTES) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
		bResult = TRUE;

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::CheckFileExtensions(const CString& sSrcFileName)
{
	BOOL bResult = FALSE;

	int nIndex = sSrcFileName.ReverseFind('.');
	if (nIndex != -1)
	{
		CString sSrcExt = sSrcFileName.Mid(nIndex);
		sSrcExt.MakeLower();
		for (int nI = 0; (bResult == FALSE) && (nI < m_FilterExts.GetCount()); nI++)
		{
			if (m_FilterExts.GetAt(nI) == sSrcExt)
				bResult = TRUE;
		}
	}
	
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::AddFilterExtension(const CString& sFilterExt)
{
	CString sTmpExtList = sFilterExt;
	sTmpExtList.MakeLower();
	m_FilterExts.Add(sTmpExtList);
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::AddFilterExtensionList(const CString& sExtList)
{
	BOOL bResult = FALSE;
	int nIndex  = 0;
	CString sTmpExtList = sExtList;
	do
	{
		nIndex = sTmpExtList.Find(_T(","));
		if (nIndex != -1)
		{
			AddFilterExtension(sTmpExtList.Left(nIndex));
			sTmpExtList = sTmpExtList.Mid(nIndex+1);
			bResult = TRUE;
		}

	}while(nIndex != -1);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::RemoveFilterExtension(const CString& sFilterExt)
{
	BOOL bResult = FALSE;
	CString sTmpFilterExt = sFilterExt;
	sTmpFilterExt.MakeLower();
	
	for (int nI = 0; (bResult == FALSE) && (nI < m_FilterExts.GetCount()); nI++)
	{
		if (m_FilterExts.GetAt(nI) == sFilterExt)
		{
			m_FilterExts.RemoveAt(nI);	
			bResult = TRUE;
		}
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CObserveDirectory::RemoveAllFilterExtension()
{
	m_FilterExts.RemoveAll();

	return TRUE;
}