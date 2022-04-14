/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: LauncherUpdate.cpp $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/LauncherUpdate.cpp $
// CHECKIN:		$Date: 20.01.06 10:24 $
// VERSION:		$Revision: 29 $
// LAST CHANGE:	$Modtime: 20.01.06 10:24 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "SecurityLauncher.h"
#include "LauncherControl.h"
#include "UpdateRecord.h"
#include "ServiceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static TCHAR szServiceInf [] = _T("a:\\service.inf");
static TCHAR szServiceDat [] = _T("a:\\service.dat");

/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnDisk()
{
	CString sInfo;
	UINT	nNr = 0;
	CFileStatus cfs;
	LONG    lSize;


	if (!CFile::GetStatus(szServiceInf,cfs))
	{
		return;
	}
	lSize = (DWORD)cfs.m_size;

	nNr = GetPrivateProfileInt(_T("service"),_T("nr"),0,szServiceInf);
	if (nNr%1997!=0)
	{
		WK_TRACE_ERROR(_T("invalide service disc incompatible magic nr\n"));
		return;
	}

	::GetPrivateProfileString(_T("service"),_T("info"),_T(""),sInfo.GetBuffer(lSize),lSize,szServiceInf);
	sInfo.ReleaseBuffer();
	if (sInfo.IsEmpty())
	{
		return;
	}

	WK_TRACE(_T("service disc Nr %d found %s\n"),nNr,sInfo);

	CServiceDialog dlg(m_pMainWnd);

	dlg.m_sService = sInfo;
	dlg.m_bDone = FALSE; // TODO

	if (IDCANCEL==dlg.DoModal())
	{
		WK_TRACE(_T("service canceled by user\n"));
		return;
	}

	CString sSC;
	::GetPrivateProfileString(_T("service"),_T("command"),_T(""),sSC.GetBuffer(lSize),lSize,szServiceInf);
	sSC.ReleaseBuffer();

	CFile file;
	CFileException cfe;
	LPVOID pBuffer = NULL;
	DWORD dwLen = 0;

	if (file.Open(szServiceDat,CFile::modeRead,&cfe))
	{
		TRY
		{
			if (file.GetStatus(cfs))
			{
				dwLen = (DWORD)cfs.m_size;
				pBuffer = malloc(dwLen);
				if (dwLen!=file.Read(pBuffer,dwLen))
				{
					free(pBuffer);
					pBuffer = NULL;
				}
			}
			file.Close();
		}
		CATCH (CFileException, e)
		{
		}
		END_CATCH
	}

	CString sVersion;
	::GetPrivateProfileString(_T("service"),_T("version"),_T(""),sVersion.GetBuffer(lSize),lSize,szServiceInf);
	sVersion.ReleaseBuffer();

	if (pBuffer!=NULL)
	{
		CUpdateRecord* pUpdateRecord;
		pUpdateRecord = new CUpdateRecord(RFU_STRING_INTERFACE,sSC,pBuffer,dwLen,FALSE);
		free(pBuffer);
		BOOL bSuccess = pUpdateRecord->MultipleUpdate();
		BOOL bShowSuccess = TRUE;
		if (bSuccess)
		{
			if (!sVersion.IsEmpty())
			{
				CWK_Profile wkp;
				wkp.WriteString(_T("Version"),_T("Number"),sVersion);
			}

			WK_TRACE(_T("service successfully completed\n"));
			CString sExec;
			::GetPrivateProfileString(_T("service"),_T("execute"),_T(""),sExec.GetBuffer(lSize),lSize,szServiceInf);
			sExec.ReleaseBuffer();
			if (!sExec.IsEmpty())
			{
				int p = sExec.Find(_T('@'));
				CString sName,sDest,sPath;

				if (p!=-1)
				{
					sName = sExec.Left(p);
					sDest = sExec.Mid(p+1);
					if (sDest==_T("p"))
					{
						sPath = GetWorkingDirectory() + _T("\\") + sName;
					}
					else if (sDest==_T("w"))
					{
						sPath = GetWindowsDirectory() + _T("\\") + sName;
					}
					else if (sDest==_T("s"))
					{
						sPath = GetSystemDirectory() + _T("\\") + sName;
					}
					else if (sDest==_T("t"))
					{
						sPath = GetTempDirectory() + _T("\\") + sName;
					}
					else if (sDest==_T("f"))
					{
						sPath = sName;
					}
				}
				WK_TRACE(_T("executing %s\n"), sPath);
				CWK_String sPathExe(sPath);
				WinExec(sPathExe ,SW_SHOW);
				bShowSuccess = FALSE;
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("service error\n"));
		}
		delete pUpdateRecord;
		if (bShowSuccess)
		{
			dlg.m_bSuccess = bSuccess;
			dlg.m_bShowSuccess = TRUE;
			dlg.DoModal();
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnUpdate(LPARAM lParam)
{
	m_pMainWnd->KillTimer(1);	
	m_WatchDog.Reset(300); // 5 min for updating an application

	CString dest,exe;
	CUpdateRecord* pUpdateRecord = (CUpdateRecord*)lParam;
	BOOL bReboot = pUpdateRecord->NeedsReboot();
	CString sSC = pUpdateRecord->GetStringCommand();

	if (sSC.IsEmpty()==FALSE)
	{
		WK_TRACE(_T("String Interface \n"),sSC);
		CString sCommand;
		int p;

		p = sSC.Find(_T(' '));
		if (p!=-1)
		{
			sCommand = sSC.Left(p);
			sSC = sSC.Mid(p+1);
		}
		else
		{
			sCommand = sSC;
		}

		if (sCommand == _T("MultipleUpdate"))
		{
			pUpdateRecord->MultipleUpdate();
		}
		if (sCommand == _T("Setup"))
		{
			pUpdateRecord->Setup(sSC);
		}
		// add more string interface here
	}
	else
	{
		if (!pUpdateRecord->IsEmpty())
		{
			pUpdateRecord->SaveTemporary();

			dest = pUpdateRecord->GetDestination();
			dest.MakeLower();
			CString sExeName;
			GetModuleFileName(m_hInstance, sExeName.GetBuffer(MAX_PATH),MAX_PATH);
			sExeName.ReleaseBuffer();
			sExeName.MakeLower();

			if (dest==sExeName)
			{
				// it's myself
				UpdateMySelf(pUpdateRecord);
			}
			else
			{
				CApplication* pAR = m_Apps.GetApplication(dest);

				if (pAR)
				{
					pAR->Terminate();

					if (pUpdateRecord->TemporaryToOriginal())
					{
						WK_TRACE(_T("%s updated\n"), (LPCTSTR)pUpdateRecord->GetDestination());
						pUpdateRecord->DeleteTemporary();

						Sleep(500);

						if (!pUpdateRecord->NeedsReboot())
						{
							if (pAR->KeepAlive())
							{
								pAR->Execute();
							}
						}
					}
					else
					{
						WK_TRACE_ERROR(_T("couldn't update %s\n"), (LPCTSTR)pUpdateRecord->GetDestination());
					}
				}
				else
				{
					if (pUpdateRecord->TemporaryToOriginal())
					{
						WK_TRACE(_T("%s updated\n"), (LPCTSTR)dest);
						pUpdateRecord->DeleteTemporary();
					}
				}
			}
		}
	}

	delete pUpdateRecord;

	// REBOOT
	if (bReboot)
	{
		CleanUp();
		WK_TRACE(_T(" ++++ Rebooting by Update    ++++\n"));
		MessageBeep(0);
		Sleep(100);
		MessageBeep(0);
		
		TerminateAll();

		if (IsNT())
		{
			AdjustPrivileges(SE_SHUTDOWN_NAME);
		}
		if (!ExitWindowsEx(EWX_REBOOT,0))
		{
			Sleep(1000);
			if (!ExitWindowsEx(EWX_REBOOT,0))
			{
				ExitWindowsEx(EWX_FORCE,0);
			}
		}
	}
	else
	{
		m_WatchDog.Reset(60); // reset after file update to normal time
		m_pMainWnd->SetTimer(1,TIMER_SECONDS*1000,NULL);	
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::UpdateMySelf(CUpdateRecord* pUR)
{
	CString sNewExe,sCommandLine,sStartDir;
	int p;
	STARTUPINFO			StartUpInfo;
	PROCESS_INFORMATION ProzessInformation;
	BOOL bP;

	GetModuleFileName(m_hInstance, sNewExe.GetBuffer(MAX_PATH), MAX_PATH);
	sNewExe.ReleaseBuffer();
	sNewExe.MakeLower();
	sCommandLine = sNewExe;
	p = sNewExe.ReverseFind(_T('.'));
	if (p==-1)
		return;
	sNewExe = sNewExe.Left(p);
	sNewExe += _T("new.exe");
	if (!pUR->TemporaryTo(sNewExe))
	{
		return;
	}

	p = sNewExe.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		sStartDir = sNewExe.Left(p);
	}
	pUR->DeleteTemporary();
	WK_TRACE(_T("updating myself \n"),sNewExe);

	WIN32_FIND_DATA FindFileData;
	int nMax = 200;
	HANDLE h;
	do
	{
		Sleep(50);
		h = FindFirstFile(sNewExe, &FindFileData);
		nMax--;
	}
	while ((h==INVALID_HANDLE_VALUE) && (nMax>0));
	FindClose(h);

	StartUpInfo.cb = sizeof(STARTUPINFO);
    StartUpInfo.lpReserved      = NULL;
    StartUpInfo.lpDesktop       = NULL;
    StartUpInfo.lpTitle	       = NULL;
    StartUpInfo.dwX	       = 0;
    StartUpInfo.dwY	       = 0;
    StartUpInfo.dwXSize	       = 0;
    StartUpInfo.dwYSize	       = 0;
    StartUpInfo.dwXCountChars   = 0;
    StartUpInfo.dwYCountChars   = 0;
    StartUpInfo.dwFillAttribute = 0;
    StartUpInfo.dwFlags	       = STARTF_FORCEONFEEDBACK | STARTF_USESHOWWINDOW;
    StartUpInfo.wShowWindow     = SW_SHOWNORMAL;
    StartUpInfo.cbReserved2     = 0;
    StartUpInfo.lpReserved2     = NULL;

	
	sNewExe += _T(' ');
	sNewExe += sCommandLine;
	SetCurrentDirectory(sStartDir);
	bP = CreateProcess(
				NULL,					// LPCTSTR pointer to name of executable module 
				sNewExe.GetBuffer(0),	// LPTSTR pointer to command line string
				NULL,					// LPSECURITY_ATTRIBUTES pointer to process security attributes 
				NULL,					// LPSECURITY_ATTRIBUTES pointer to thread security attributes 
				FALSE,					// handle inheritance flag 
				NORMAL_PRIORITY_CLASS,	//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
				NULL,					// LPVOID pointer to new environment block 
				NULL,					// pointer to current directory name 
				&StartUpInfo,			// LPSTARTUPINFO pointer to STARTUPINFO 
				&ProzessInformation 	// LPPROCESS_INFORMATIONpointer to PROCESS_INFORMATION  
				);
	if (bP==FALSE)
	{
		WK_TRACE_ERROR(_T("%s\n"), GetLastErrorString());
	}
	sNewExe.ReleaseBuffer();
	WK_CLOSE_HANDLE(ProzessInformation.hProcess);
	WK_CLOSE_HANDLE(ProzessInformation.hThread);
	PostQuitMessage(0);
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::UpdateMySelfInSecondInstance()
{
	CString sNewExe;
	CString sStartDir;
	CString sOldExe = m_lpCmdLine;
	int p;

	CFile oldExe;
	CFile newExe;
	CFileException cfe;
	CFileStatus cfs;
	LPVOID pBuffer;
	STARTUPINFO			StartUpInfo;
	PROCESS_INFORMATION ProzessInformation;
	int nMax = 100;
	DWORD dwLen;
	BOOL bOpen = FALSE;
	BOOL bP = FALSE;
	BOOL bWrite = TRUE;

	WK_TRACE(_T("updating myself in second instance\n"));
	GetModuleFileName(m_hInstance, sNewExe.GetBuffer(MAX_PATH), MAX_PATH);
	sNewExe.ReleaseBuffer();
	sNewExe.MakeLower();
	p = sNewExe.Find(_T("new.exe"));

	if (p==-1)
		return;

	if (sOldExe.IsEmpty())
		return;

	p = sNewExe.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		sStartDir = sNewExe.Left(p);
	}

	TRY
	{
		if (newExe.Open(sNewExe,CFile::modeRead|CFile::shareDenyNone,&cfe))
		{
			WK_TRACE(_T("opened new exe\n"));
			if (newExe.GetStatus(cfs))
			{
				dwLen = (DWORD)cfs.m_size;
				pBuffer = malloc(dwLen);
				if (dwLen==newExe.Read(pBuffer,dwLen))
				{
					WK_TRACE(_T("read new exe \n"));
					while ((nMax>0) && !bOpen)
					{
						Sleep(100);
						bOpen = oldExe.Open(sOldExe,CFile::modeCreate|CFile::modeWrite,&cfe); 
						nMax--;
					}

					if (bOpen)
					{
						TRY
						{
							oldExe.Write(pBuffer,dwLen);
							oldExe.Flush();
							oldExe.Close();
						}
						CATCH (CFileException,e)
						{
							bWrite = FALSE;
						}
						END_CATCH
						if (bWrite)
							WK_TRACE(_T("wrote new exe to old place\n"));
						else
							WK_TRACE_ERROR(_T("couldn't write new exe to old place\n"));
					}
				}
				free(pBuffer);
			}
			newExe.Close();

			WIN32_FIND_DATA FindFileData;
			HANDLE h;
			nMax = 200;
			do
			{
				Sleep(50);
				h = FindFirstFile(sOldExe, &FindFileData);
				nMax--;
			}
			while ((h==INVALID_HANDLE_VALUE) && (nMax>0));
			FindClose(h);

			StartUpInfo.cb = sizeof(STARTUPINFO);
			StartUpInfo.lpReserved      = NULL;
			StartUpInfo.lpDesktop       = NULL;
			StartUpInfo.lpTitle	       = NULL;
			StartUpInfo.dwX	       = 0;
			StartUpInfo.dwY	       = 0;
			StartUpInfo.dwXSize	       = 0;
			StartUpInfo.dwYSize	       = 0;
			StartUpInfo.dwXCountChars   = 0;
			StartUpInfo.dwYCountChars   = 0;
			StartUpInfo.dwFillAttribute = 0;
			StartUpInfo.dwFlags	       = STARTF_FORCEONFEEDBACK | STARTF_USESHOWWINDOW;
			StartUpInfo.wShowWindow     = SW_SHOWNORMAL;
			StartUpInfo.cbReserved2     = 0;
			StartUpInfo.lpReserved2     = NULL;

			WK_TRACE(_T("starting updated exe \n"),sOldExe);
			SetCurrentDirectory(sStartDir);

			bP = CreateProcess(
						sOldExe,				// LPCTSTR pointer to name of executable module 
						NULL,					// LPTSTR pointer to command line string
						NULL,					// LPSECURITY_ATTRIBUTES pointer to process security attributes 
						NULL,					// LPSECURITY_ATTRIBUTES pointer to thread security attributes 
						FALSE,					// handle inheritance flag 
						NORMAL_PRIORITY_CLASS,	//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
						NULL,					// LPVOID pointer to new environment block 
						sStartDir,				// LPCTSTR pointer to current directory name 
						&StartUpInfo,			// LPSTARTUPINFO pointer to STARTUPINFO 
						&ProzessInformation 	// LPPROCESS_INFORMATIONpointer to PROCESS_INFORMATION  
						);
			if (bP)
			{
				WK_TRACE(_T("updated myself\n"));
			}
			else
			{
				WK_TRACE_ERROR(_T("%s\n"),GetLastErrorString());
			}
			WK_CLOSE_HANDLE(ProzessInformation.hProcess);
			WK_CLOSE_HANDLE(ProzessInformation.hThread);
		}
		else
		{
			WK_TRACE_ERROR(_T("couldn't open file %s\n"),sNewExe);
		}
	}
	CATCH (CFileException, e)
	{
	}
	END_CATCH
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::DeleteNewExeInThirdInstance()
{
	CString sNewExe;
	BOOL bDel = TRUE;
	int p;

	GetModuleFileName(m_hInstance, sNewExe.GetBuffer(MAX_PATH), MAX_PATH);
	sNewExe.ReleaseBuffer();
	sNewExe.MakeLower();
	p = sNewExe.ReverseFind(_T('.'));
	if (p==-1)
		return;
	sNewExe = sNewExe.Left(p);
	sNewExe += _T("new.exe");

	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(sNewExe, &FindFileData);
	FindClose(h);
	if (h != INVALID_HANDLE_VALUE)
	{
		Sleep(500);

		TRY
		{
			CFile::Remove(sNewExe);
		}
		CATCH (CFileException,e)
		{
			bDel = FALSE;
		}
		END_CATCH

		if (bDel)
		{
			WK_TRACE(_T("%s deleted\n"),sNewExe);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::ReconnectToServer()
{
	if (m_pLauncherControl==NULL)
	{
		m_pLauncherControl = new CLauncherControl();
	}
	else
	{
		if ( (m_pLauncherControl->GetIPCState()==CIPC_STATE_INVALID) || 
			 (m_pLauncherControl->GetIPCState()==CIPC_STATE_DISCONNECTED) )
		{
			WK_DELETE(m_pLauncherControl);
			m_pLauncherControl = new CLauncherControl();
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::SetExecuteAtExit(const CString& s)
{
	m_sExecuteAtExit = s;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::SetRebootAtExit(BOOL bRebootAtExit)
{
	m_bRebootAtExit = bRebootAtExit;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::SetShutdownAtExit(BOOL bShutdownAtExit)
{
	m_bShutDownAtExit = bShutdownAtExit;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::SetShutdownAtExitShell(BOOL bShutdownAtExitShell)
{
	m_bShutDownAtExitShell = bShutdownAtExitShell;
}
