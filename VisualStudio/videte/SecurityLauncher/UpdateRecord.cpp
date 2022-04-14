/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: UpdateRecord.cpp $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/UpdateRecord.cpp $
// CHECKIN:		$Date: 24.04.04 15:09 $
// VERSION:		$Revision: 30 $
// LAST CHANGE:	$Modtime: 24.04.04 14:04 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SecurityLauncher.h"
#include "UpdateRecord.h"

extern CSecurityLauncherApp theApp;

//////////////////////////////////////////////////////////////////////////////////////
CUpdateRecord::CUpdateRecord(	int iDestination,
								const CString &sFileName,
								const void *pData,
								DWORD dwDataLen,
								BOOL bNeedsReboot)
{
	m_bNeedsReboot = bNeedsReboot;
	m_dwDataLen = dwDataLen;
	m_iDestination = iDestination;

	if (m_dwDataLen>0)
	{
		m_pData = new BYTE[m_dwDataLen];
		CopyMemory(m_pData,pData,m_dwDataLen);
	}
	else
	{
		m_pData = NULL;
	}

	CString mes;
	mes = sFileName;
	if (iDestination!=RFU_STRING_INTERFACE)
	{
		m_sDestinationFileName = MakeFullPathName(iDestination,sFileName);
	}
	else
	{
		m_sStringCommand = sFileName;
	}
};
//////////////////////////////////////////////////////////////////////////////////////
CUpdateRecord::	~CUpdateRecord()
{
	WK_DELETE_ARRAY(m_pData);
}
//////////////////////////////////////////////////////////////////////////////////////
void CUpdateRecord::SaveTemporary()
{
	CFile tempFile;
	CString sTempPathName;
	CString sTempFileName;
	CFile fFile;

	if (m_pData)
	{
		GetTempPath(_MAX_PATH, sTempPathName.GetBuffer(MAX_PATH));
		sTempPathName.ReleaseBuffer();
		GetTempFileName(sTempPathName, _T("sec"), 0, sTempFileName.GetBuffer(MAX_PATH));
		sTempFileName.ReleaseBuffer();
		
		if (tempFile.Open(sTempFileName,
			CFile::modeCreate|CFile::modeWrite))
		{
			TRY
			{
				tempFile.Write(m_pData,m_dwDataLen);
				tempFile.Flush();
				m_sTemporaryFileName = sTempFileName;
				WK_TRACE(_T("temporary is %s\n"),m_sTemporaryFileName);
				tempFile.Close();
			}
			CATCH(CFileException, e)
			{
				WK_TRACE_ERROR(_T("temp file not saved %s %s\n"), sTempFileName, GetLastErrorString(e->m_lOsError));
			}
			END_CATCH
		}
		else
		{
			WK_TRACE_ERROR(_T("temp file not saved %s\n"), sTempFileName);
		}
	}
	else
	{
		// may be directory to create ??
		m_sTemporaryFileName.Empty();
	}

	Sleep(100);
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CUpdateRecord::TemporaryToOriginal()
{
	return TemporaryTo(m_sDestinationFileName);
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CUpdateRecord::TemporaryTo(CString sDestination)
{
	DWORD dwLen = 0;
	CFile temp;
	CFile orig;
	CFileException cfe;
	CFileStatus cfs;
	LPVOID pBuffer = NULL;
	BOOL bRet = FALSE;
	BOOL bOpen = FALSE;
	int nMax;

	// read
	if (!m_sTemporaryFileName.IsEmpty())
	{
		if (temp.Open(m_sTemporaryFileName,CFile::modeRead,&cfe))
		{
			TRY
			{
				if (temp.GetStatus(cfs))
				{
					dwLen = (DWORD)cfs.m_size;
					pBuffer = malloc(dwLen);
					if (dwLen!=temp.Read(pBuffer,dwLen))
					{
						free(pBuffer);
						pBuffer = NULL;
					}
				}
				temp.Close();
			}
			CATCH (CFileException,e)
			{
			}
			END_CATCH
		}
	}
	else
	{
		// may be dir !!
		if (WK_CreateDirectory(sDestination))
		{
			WK_TRACE(_T("directory created %s\n"),sDestination);
			return TRUE;
		}
		else
		{
			WK_TRACE_ERROR(_T("directory not created %s\n"),sDestination);
			return FALSE;
		}
	}

	CString sDir;
	int p;

	p = sDestination.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		sDir = sDestination.Left(p);
		if (!DoesFileExist(sDir))
		{
			WK_CreateDirectory(sDir);
		}
	}

	if (pBuffer!=NULL)
	{
		DWORD dwStartTime;
		DWORD dwStopTime;
		CString s;

		dwStartTime = GetTickCount();
		nMax = 400;
		while ((nMax>0) && !bOpen)
		{
			Sleep(50);
			bOpen = orig.Open(sDestination,CFile::modeCreate|CFile::modeWrite,&cfe); 
			nMax--;
		}
		dwStopTime = GetTickCount();
		WK_TRACE(_T("waiting for open %d ms\n"),dwStopTime-dwStartTime);

		if (bOpen)
		{
			BOOL bWrite = TRUE;
			TRY
			{
				orig.Write(pBuffer,dwLen);
				orig.Flush();
				orig.Close();
			}
			CATCH (CFileException,e)
			{
				WK_TRACE_ERROR(_T("couldn't write update file %s %s\n"),
									sDestination,GetLastErrorString(e->m_lOsError));
				bWrite = FALSE;
			}
			END_CATCH
			if (!bWrite)
			{
				WK_TRACE_ERROR(_T("couldn't write update file %s"),sDestination);
			}
			bRet = bWrite;
		}
		else
		{
			WK_TRACE_ERROR(_T("couldn't open update file %s\n"),sDestination);
		}
		free(pBuffer);
	}
	else
	{
		WK_TRACE_ERROR(_T("couldn't read temporary %s\n"),m_sTemporaryFileName);
	}

	if (bRet)
	{
		WK_TRACE(_T("temporary copied to destination %s\n"),m_sDestinationFileName);
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////////////////////
void CUpdateRecord::DeleteTemporary()
{
	if (DoesFileExist(m_sTemporaryFileName))
	{
		TRY
		{
			CFile::Remove(m_sTemporaryFileName);
		}
		CATCH (CFileException, cfe)
		{
		}
		END_CATCH
	}
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CUpdateRecord::Decompress()
{
	CFile fCompress;
	CFileStatus cfs;
	CString sUnZip;
	CString sMes;
	BOOL bRet = FALSE;
	int p;

	SaveTemporary();
	p = m_sTemporaryFileName.ReverseFind(_T('.'));
	sUnZip = m_sTemporaryFileName.Left(p) + _T(".laz");

	WK_TRACE(_T("decompressing %s to %s\n"),m_sTemporaryFileName,sUnZip);
	WK_DecompressFile(m_sTemporaryFileName,sUnZip);
	WK_TRACE(_T("decompressed\n"));
	DeleteTemporary();
	free(m_pData);
	m_dwDataLen = 0;

	if (fCompress.Open(sUnZip,CFile::modeRead))
	{
		TRY
		{
			if (fCompress.GetStatus(cfs))
			{
				m_dwDataLen = (DWORD)cfs.m_size;
				m_pData = malloc(m_dwDataLen);
				if (cfs.m_size == (LONG)fCompress.Read(m_pData,m_dwDataLen) )
				{
					WK_TRACE(_T("read decompressed file\n"));
					bRet = TRUE;
				}
				else
				{
					WK_TRACE_ERROR(_T("cannot read decompressed file\n"));
				}
			}
			else
			{
				WK_TRACE_ERROR(_T("cannot get size of decompressed file\n"));
			}
			fCompress.Close();
		}
		CATCH (CFileException,e)
		{
		}
		END_CATCH

		TRY
		{ CFile::Remove(sUnZip);}
		CATCH(CFileException, e)
		{}
		END_CATCH

	}
	else
	{
		WK_TRACE_ERROR(_T("cannot open decompressed file\n"));
	}

	return bRet;
	
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CUpdateRecord::MultipleUpdate()
{
	CString sFiles,sFile,sName,sSize,sDest,sPath;
	DWORD dwSize;
	LPVOID  pFile;
	int p;
	CFile fFile;
	CFileException cfe;
	DWORD dwStartTime;
	DWORD dwStopTime;
	CString s;
	BOOL bOpen,bRet;
	int nMax;

	if (!Decompress())
	{
		return FALSE;
	}

	p = m_sStringCommand.Find(_T(' '));
	sFiles = m_sStringCommand.Mid(p+1);
	bRet = TRUE;

	pFile = m_pData;
	while (sFiles.IsEmpty()==FALSE)
	{
		p = sFiles.Find(_T(';'));
		sFile = sFiles.Left(p);
		sFiles = sFiles.Mid(p+1);

		p = sFile.Find(_T(','));
		sName = sFile.Left(p);
		sFile = sFile.Mid(p+1);
		p = sFile.Find(_T(','));
		sSize = sFile.Left(p);
		sDest = sFile.Mid(p+1);

		if (sDest==_T("m"))
		{
			sPath = theApp.GetWorkingDirectory() + _T("\\mico\\") + sName;
		}
		else if (sDest==_T("c"))
		{
			sPath = theApp.GetWorkingDirectory() + _T("\\coco\\") + sName;
		}
		else if (sDest==_T("p"))
		{
			sPath = theApp.GetWorkingDirectory() + _T("\\") + sName;
		}
		else if (sDest==_T("w"))
		{
			sPath = theApp.GetWindowsDirectory() + _T("\\") + sName;
		}
		else if (sDest==_T("s"))
		{
			sPath = theApp.GetSystemDirectory() + _T("\\") + sName;
		}
		else if (sDest==_T("t"))
		{
			sPath = theApp.GetTempDirectory() + _T("\\") + sName;
		}
		else if (sDest==_T("f"))
		{
			sPath = sName;
		}
		sPath.MakeLower();
		WK_TRACE(_T("multiple update file %s\n"),sPath);

		CApplication* pAR = theApp.m_Apps.GetApplication(sPath);
		if (pAR)
		{
			CString s = pAR->GetName();
			WK_TRACE(_T("stopping (by update) %s\n"),s);
			pAR->Terminate();
		}
		else
		{
			CString sExeName;
			GetModuleFileName(theApp.m_hInstance, sExeName.GetBuffer(MAX_PATH), MAX_PATH);
			sExeName.MakeLower();
			if (sPath == sExeName)
			{
				// OOPS it's myself
				SaveTemporary();
				theApp.UpdateMySelf(this); // this call never comes back
				return TRUE;
			}
		}

		dwSize = _ttoi(sSize);

		dwStartTime = GetTickCount();
		nMax = 50;
		bOpen = FALSE;
		while ((nMax>0) && !bOpen)
		{
			Sleep(50);
			bOpen = fFile.Open(sPath,CFile::modeCreate|CFile::modeWrite,&cfe); 
			nMax--;
		}
		dwStopTime = GetTickCount();
		WK_TRACE(_T("waiting for open %d ms\n"),dwStopTime-dwStartTime);

		if (bOpen)
		{
			TRY
			{
				fFile.Write(pFile,dwSize);
				fFile.Flush();
				fFile.Close();
			}
			CATCH (CFileException,e)
			{
				WK_TRACE_ERROR(_T("couldn't update file %s %s\n"),sPath,
								GetLastErrorString(e->m_lOsError));
			}
			END_CATCH
			pFile = (LPVOID)((LPBYTE)pFile + dwSize);
			WK_TRACE(_T("wrote file %s\n"),sPath);
		}
		else
		{
			WK_TRACE_ERROR(_T("couldn't update file %s\n"),sPath);
			bRet = FALSE;
		}

		Sleep(500);

		if (pAR && pAR->KeepAlive())
		{
			CString s = pAR->GetName();
			WK_TRACE(_T("restart (after update) %s\n"),s);
			pAR->Execute();
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CUpdateRecord::Setup(const CString& sCommand)
{
	WK_TRACE(_T("Setup %s\n"),sCommand);

	// make sure that we will be startet
	// after reboot by setup
	if (!theApp.AmIShell())
	{
		CString sExe;
		GetModuleFileName(theApp.m_hInstance,sExe.GetBuffer(_MAX_PATH),_MAX_PATH);
		sExe.ReleaseBuffer();

		CRunService rs;
		rs.WriteRun(_T("Launcher"),sExe);
	}
	else
	{
		// restart by shell
	}

	theApp.SetExecuteAtExit(sCommand);
	theApp.SetRebootAtExit(FALSE);
	theApp.SetShutdownAtExit(FALSE);
	theApp.SetShutdownAtExitShell(FALSE);

	theApp.m_pMainWnd->PostMessage(WM_COMMAND,ID_QUITE_EXIT);


	return TRUE;
}
