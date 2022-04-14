/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: LauncherError.cpp $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/LauncherError.cpp $
// CHECKIN:		$Date: 18.07.06 11:46 $
// VERSION:		$Revision: 77 $
// LAST CHANGE:	$Modtime: 17.07.06 14:27 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$


#include "stdafx.h"
#include "SecurityLauncher.h"
#include "LauncherControl.h"
#include "CIPCInputVirtualAlarm.h"
#include "ErrorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DO NOT USE theApp.Trace in this proc
// It's not thread save !!!
UINT ErrorThreadProc(LPVOID lpParam)
{
	XTIB::SetThreadName(_T("RunTimeErrorThread"));
	CWK_RunTimeError l_RunTimeError;
	while (theApp.m_bRun)
	{
		if (l_RunTimeError.ReadGlobalError())
		{
			theApp.m_newErrors.SafeAdd(new CWK_RunTimeError(l_RunTimeError));
			AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_ERROR_ARRIVED);
			l_RunTimeError.ConfirmGlobalError();
		}
	}
	return 0x0E;
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::StartErrorThread()
{
	if (m_Mode == LM_NORMAL)
	{
		m_bRun = TRUE;

		m_pErrorThread = AfxBeginThread(ErrorThreadProc,NULL);
		m_pErrorThread->m_bAutoDelete = FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::StopErrorThread()
{
	m_bRun = FALSE;

	if (m_pErrorThread)
	{
		WaitForSingleObject(m_pErrorThread->m_hThread,2000);
		WK_DELETE(m_pErrorThread);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnNewError()
{
	CWK_RunTimeError* pLastError = NULL;
	m_newErrors.Lock();
	if (m_newErrors.GetSize())
	{
		pLastError = m_newErrors.GetAt(0);
		m_newErrors.RemoveAt(0);
	}
	m_newErrors.Unlock();

	if (pLastError)
	{
		pLastError->CloseHandles();
		OnErrorArrived(pLastError);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnErrorArrived(CWK_RunTimeError* pLastError)
{
	// we are definitiv in main thread
	CSystemTime st;
	st.GetLocalTime();


	// Handling der 'virtuellen' Alarme und reine Infos rausfiltern
	if (    m_pCIPCVirtualAlarm 
		&& !m_pCIPCVirtualAlarm->OnErrorArrived(pLastError))
	{
		if (pLastError->GetError()==RTE_ARCHIVE_FUEL)
		{
			m_iAlarmPercent = pLastError->GetParam2();
		}
		else if (pLastError->GetError() == RTE_ISDN_BCHANNEL)
		{
			WK_ApplicationId wai = pLastError->GetApplicationId();
			CApplication* pApp = m_Apps.GetApplication(wai);
			if (pApp)
			{
				int nCurrentBC = pLastError->GetParam2();
				pApp->SetNrOfBChannels(nCurrentBC);
				int nOtherBC = m_Apps.GetNrOfBChannels(wai);
				m_iISDNBChannel = nCurrentBC + nOtherBC;
				CApplication* pApp = m_Apps.GetApplication(WAI_IDIP_CLIENT);
				if (pApp)
				{
					pApp->PostMessage(WM_ISDN_B_CHANNELS, MAKELONG(m_iISDNBChannel, pLastError->GetApplicationId()), MAKELPARAM(nCurrentBC, nOtherBC));
				}
			}
		}
		else if (pLastError->GetError()==RTE_TIME_CHANGED)
		{
			OnTimeChangedBySoftware();
		}
		WK_DELETE(pLastError);
		// Keine Messagebox zeigen
		return;
	}

	// all other errors will be shown in dialog box

	if (pLastError->GetError()==RTE_DONGLE) 
	{
		// comes from lib so we give him a name
		CString sL;
		sL.LoadString(IDS_DONGLE_EXE);
		pLastError->SetErrorText(sL);
	}
	
	if (pLastError->GetError() == RTE_OS_EXCEPTION)
	{
		IncreaseGPCounter();
		CApplication* pApp = m_Apps.GetApplication(pLastError->GetApplicationId());
		if (pApp)
		{
			WK_TRACE_ERROR(_T("GP in %s version %s\n"),
				pApp->GetExePathName(),pApp->GetVersion());
		}
	}
	
	WK_TRACE(_T("%s\n"),(LPCTSTR)pLastError->GetFormattedError());

	m_Errors.SafeAdd(pLastError);
	m_bWriteErrorFile = TRUE; //is set to FALSE after rterror.dat is written to disc

	int nErrors = m_Errors.GetSize();
	if (nErrors>0)
	{
		while (nErrors > m_nMaxErrors)
		{
			m_Errors.RemoveAt(0);
			nErrors = m_Errors.GetSize();
		}
		
		BOOL bPopup = theApp.DoPopupErrorDlg(pLastError);
		if (m_pErrorDialog == NULL)
		{
			if (bPopup)
			{
				OnError();	// startet den Errordialog, wenn m_pErrorDialog == NULL
			}
		}
		else
		{
			m_pErrorDialog->InsertError(pLastError);
			if (bPopup)
			{
				m_pErrorDialog->ShowWindow(SW_SHOW);
			}
		}

		// actualize error counters
		CWK_RunTimeError* pError;
		SYSTEMTIME et;
		DWORD	   dwLastHour = 0;
		DWORD	   dwLastDay = 0;
		int		   i;
		for (i=0;i<m_Errors.GetSize();i++)
		{
			pError = m_Errors.GetAt(i);
			if (pError && (pError->GetLevel()==REL_REBOOT_ERROR))
			{
				et = pError->GetTimeStamp();

				if (et.wDay==st.wDay)
				{
					dwLastDay++;
				}
				if (et.wHour==st.wHour)
				{
					dwLastHour++;
				}
			}
		}

		if (pLastError->GetLevel()==REL_REBOOT_ERROR)
		{
			if (   pLastError->GetError() == RTE_EWF_RAM_OVERFLOW)
			{
				WK_TRACE(_T("EWF allocated to much memory from RAM %d of %d Bytes\n"), pLastError->GetParam1(), pLastError->GetParam2());
				ErrorReboot(TRUE); // exits windows and app
			}
			else if (pLastError->GetError() == RTE_HARDDISK_FALURE)
			{
				WK_TRACE(_T("HDD Failure on Drive %c:\\\n"), pLastError->GetParam1());
				CString sLog = GetLogPath();
				sLog.MakeLower();
				if (sLog.GetAt(0) == pLastError->GetParam1())
				{
					DeleteLogPath();
				}
				m_sErrorDat = DEFAULT_ERRORFILE;
				DeleteLogPath();
				ErrorReboot(TRUE); // exits windows and app
			}
			else if ( ( (dwLastHour>0) && (dwLastHour%3==0) ) || 
				 ( (dwLastDay>0)  && (dwLastDay%9==0) ) 
				)
			{
				WK_TRACE(_T(" %d Fehler pro Stunde %d Fehler pro Tag\n"),dwLastHour,dwLastDay);
				if (m_WatchDog.PiezoSetFlag(PF_WKRT_ERROR))
				{
					WK_TRACE_ERROR(_T("Störungsmelder ein zu viele Softwarefehler.\n"));
				}
				ErrorReboot(); // exits windows and app
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::ErrorReboot(BOOL bForce/*=FALSE*/)
{
	CWK_Profile wkp;
	if (0==wkp.GetInt(_T("SecurityLauncher\\Debug"), _T("NoReboot"), 0))
	{
		if (  bForce 
			|| 
			  (   m_pDongle
			   && m_pDongle->IsTransmitter()
			   && AmIShell()
			 ))
		{
			m_bShallReboot = TRUE;

			if (!bForce)
			{
				WK_TRACE_ERROR(_T("TOO MANY SOFTWARE ERRORS REBOOTING!\n"));
			}
			
			m_WatchDog.Reset(600);

			int iResetCounter = wkp.GetInt(_T("SecurityLauncher"), _T("ResetCounter"),0);
			iResetCounter++;
			WK_STAT_LOG(_T("Restart"),iResetCounter,_T("ResetCounter"));
			wkp.WriteInt(_T("SecurityLauncher"),_T("ResetCounter"),iResetCounter);
			
			m_WatchDog.PiezoSetFlag(PF_ERROR_REBOOT);
			TerminateAll();
			m_WatchDog.PiezoSetFlag(PF_ERROR_REBOOT);
			m_pMainWnd->KillTimer(1);
			
			CleanUp();

			Sleep(1000);

			if (IsNT())
			{
				AdjustPrivileges(SE_SHUTDOWN_NAME);
			}
			UINT nFlags = EWX_REBOOT;
			if (!ExitWindowsEx(nFlags, 0))
			{
				if (bForce) nFlags |= EWX_FORCE;
				Sleep(1000);
				if (!ExitWindowsEx(nFlags, 0))
				{
					ExitWindowsEx(EWX_FORCE,0);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSecurityLauncherApp::OnError() 
{
	if (m_pErrorDialog)
	{
		m_pErrorDialog->DestroyWindow();
		m_pErrorDialog = NULL;
	}
	else
	{
		m_pErrorDialog = new CErrorDialog(m_pMainWnd);
		m_pErrorDialog->SetWindowPos(&CWnd::wndTopMost, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		m_pErrorDialog->InsertErrors(&m_Errors);
		m_pErrorDialog->ShowWindow(SW_SHOW);
	}
}
